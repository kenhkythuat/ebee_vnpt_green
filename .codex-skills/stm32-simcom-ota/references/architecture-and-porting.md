# Architecture and porting guide

## Target discovery

Confirm facts from the target project and MCU documentation rather than filenames alone:

- Exact STM32 family/part, total flash, SRAM ranges, erase unit, programming width, option-byte protections, and dual-bank behavior.
- Existing bootloader or reserved flash regions.
- Maximum observed application size plus realistic growth margin.
- Linker script actually selected by the active build configuration.
- System memory remap and `SCB->VTOR` requirements.
- SIMCom family and support for `AT+HTTPINIT`, `AT+HTTPPARA`, `AT+HTTPACTION`, and offset-based `AT+HTTPREAD`.
- Modem USART, RX mechanism, baud rate, callback ownership, MQTT URCs, watchdog, timers, and sensor activity.

Do not proceed with the staged design unless flash can contain all reserved regions without overlap.

## Flash layout

Define one shared layout header consumed by application and bootloader:

```text
flash base
├── bootloader
├── active application
├── staging image
├── pending metadata page/sector
└── optional persistent configuration
flash end
```

Validate:

- Each erase-controlled region begins on an erase boundary.
- `boot_end <= app_start <= app_end <= staging_start`.
- `staging_end <= metadata_start`.
- Metadata and configuration do not share an erase unit unless erasing both is intentional.
- Maximum image size fits both active and staging capacity.

EBEE's `0x08008000` application address is valid only for its 32 KiB bootloader reservation. Recalculate it for the target.

## Bootloader contract

The safe sequence is:

1. Validate metadata magic, schema, pending state, size, addresses, and metadata CRC.
2. Validate staged-image CRC before erasing the active application.
3. Erase only required application units.
4. Copy with the target MCU's programming width/alignment while feeding the watchdog.
5. Validate installed CRC and application vector table.
6. Clear pending metadata only after successful installation.
7. Disable SysTick/interrupts, clear pending NVIC interrupts, set VTOR, load MSP, and branch to the Thumb reset vector.

Validate the stack pointer against the target's real SRAM ranges and reset address against the active application region.

## Application contract

The application is linked at the selected base and relocates VTOR there. OTA should:

- Download and parse a bounded manifest.
- Compare dotted numeric versions against the compiled firmware version.
- Validate device identity, application address, image size, and CRC.
- Stream bounded binary chunks to staging while updating CRC32.
- Verify stream CRC and CRC read back from staging.
- Commit pending metadata last and reset into the bootloader.

Metadata's version string is informational; it cannot change the version compiled into the application.

## SIMCom HTTP transport

For supported A76xx firmware, a typical robust session is:

```text
AT+HTTPTERM
AT+CSSLCFG="sslversion",0,4
AT+CSSLCFG="authmode",0,0
AT+CSSLCFG="enableSNI",0,1
AT+HTTPINIT
AT+HTTPPARA="SSLCFG",0
AT+HTTPPARA="CONNECTTO",120
AT+HTTPPARA="RECVTO",120
AT+HTTPPARA="READMODE",1
AT+HTTPPARA="URL","..."
AT+HTTPACTION=0
AT+HTTPREAD=<offset>,<length>
```

Confirm commands against actual modem firmware. Parse HTTP status/content length. Keep binary body length separate from NUL-terminated diagnostics. A delayed zero-length trailer can enter the next capture; select a positive data header rather than assuming the first `+HTTPREAD:` token owns the body.

## UART and concurrency

- UART callbacks only capture/copy data, update small flags, and re-arm RX.
- Never print, parse large JSON, read sensors, erase/program flash, or block in UART/timer callbacks.
- Record UART errors and recover Receive-to-Idle after overrun/framing/noise failures.
- Check RX re-arm results and recover a stuck HAL RX state deliberately.
- Give modem USART the highest relevant priority; sensor UARTs are lower and timers lower again.
- During OTA, pause telemetry timers, sensor state machines, and sensor UART re-arming. Restore only resources active beforehand if OTA returns without reset.
- `HAL_UART_ERROR_ORE` means CPU-side RX starvation, not an HTTP/CRC failure.
- Prefer DMA Receive-to-Idle/circular buffering when the target supports it or traffic requires it.

Flash programming stalls instruction fetch on some STM32 families. Program only after capturing a complete chunk and assess RAM-resident routines or modem flow control when the target requires them.

## MQTT and porting sequence

Assemble fragmented modem URCs into a bounded window and require both expected topic and JSON command before setting an OTA request flag. Start OTA from the main loop/task, never from a UART callback.

EBEE's example contract is topic `v1/devices/me/attributes` with payload `{"OTA":"ota_check"}`; treat it as configurable.

Port in this order:

1. Establish the memory map.
2. Add shared layout definitions.
3. Build/flash bootloader and prove it jumps to a small relocated application.
4. Change application linker/VTOR and inspect vector words.
5. Add staging, metadata, and CRC.
6. Add manifest/binary HTTP download.
7. Add UART ownership, error recovery, and pause/resume isolation.
8. Add MQTT trigger and telemetry version.
9. Test success, no-update, corrupt image, interrupted download, and reset-during-install cases appropriate to the hardware.

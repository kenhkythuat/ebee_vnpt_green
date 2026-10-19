---
name: stm32-simcom-ota
description: Implement or port a staged bootloader OTA flow for STM32 firmware using a SIMCom modem, MQTT triggers, HTTPS AT commands, flash staging, CRC32, and a JSON manifest. Use when adding this OTA design to another STM32CubeIDE project or diagnosing its integration; do not assume EBEE flash addresses fit a different MCU.
---

# STM32 SIMCom OTA

Port the verified OTA design while preserving the target project's existing behavior and generated-code conventions. Treat the bundled EBEE files as a working reference, not as drop-in code for an unknown MCU.

## Required workflow

1. Inspect the target before editing:
   - MCU part number, flash/RAM size, erase page or sector geometry.
   - Existing linker script, vector-table relocation, startup file, watchdog, and boot path.
   - Application size and whether flash can hold bootloader + active image + staging image + metadata without overlap.
   - Modem model/firmware, AT-command transport, MQTT receive path, UART mode, interrupt priorities, timers, sensors, and JSON library.
   - Dirty Git changes; preserve unrelated user work.
2. Read [references/architecture-and-porting.md](references/architecture-and-porting.md) before designing the flash map or integration.
3. Produce an explicit memory map and validate every boundary. If the device cannot hold two images, stop and explain that this staged design is incompatible; do not silently reduce safety margins or overwrite the running application.
4. Implement only OTA-related changes. Adapt files under `assets/reference-implementation/` when compatible. Never copy EBEE addresses, device IDs, URLs, UART handles, or STM32F103 flash primitives without confirming them.
5. Keep these invariants:
   - Bootloader is linked at flash base; application is linked after its reserved region.
   - Application startup sets `SCB->VTOR` to the application vector base before interrupts are used.
   - Firmware downloads to staging, is CRC-checked in stream and flash, then pending metadata is committed last.
   - Bootloader validates metadata, staged CRC, initial stack pointer, Thumb reset vector, and address ranges before installation/jump.
   - Failed download or validation leaves the current application bootable.
6. Integrate modem receive ownership carefully:
   - Receive binary data without string functions over the body.
   - Configure A76xx HTTP `READMODE=1` when supported and read bounded chunks.
   - Handle split headers, delayed `+HTTPREAD: 0` trailers, retries, and UART errors.
   - Keep `printf`, parsing, sensor work, and blocking operations out of UART/timer ISRs.
   - Give modem RX higher interrupt priority than sensor UARTs/timers. Pause non-OTA timers, telemetry, and sensor RX while OTA owns the modem; restore prior state on a non-reset exit.
7. Add the MQTT trigger using the target broker/topic contract. Assemble fragmented MQTT URCs before matching topic and JSON payload.
8. Read [references/release-and-diagnostics.md](references/release-and-diagnostics.md) before generating or publishing an OTA artifact.
9. Build all changed targets and verify linker addresses, sizes, vector table, CRC, and manifest consistency. Do not upload firmware, push Git changes, or flash hardware unless requested.

## Reference implementation

`assets/reference-implementation/` contains the known-working EBEE STM32F103RE/SIMCom A76xx implementation. Use it to accelerate porting, but rewrite the hardware-specific constants and integration points identified in the references.

When finishing, report the chosen memory map, edited files, build result, produced artifacts, and any hardware-only checks still required.

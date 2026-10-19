# Release and diagnostics

## Release invariant

The version compiled into firmware must equal the manifest version and artifact filename. Changing only `manifest.json` labels an old binary as a new release and causes repeated updates.

Before packaging:

1. Update the tracked firmware version constant.
2. Clean/rebuild with the OTA application linker script.
3. Verify the vector table begins at the configured application address.
4. Convert HEX to a contiguous binary whose byte zero is the initial stack pointer; fill address gaps with `0xFF`.
5. Inspect the ELF/binary to confirm the embedded version.
6. Calculate size and CRC32 from the exact hosted binary.
7. Generate and re-read the manifest, checking device, address, size, CRC, URL, and version.
8. Publish binary before manifest so devices never see metadata for a missing image.

The bundled manifest tool is EBEE-specific. Adapt filename prefix, SRAM range, device ID, URL, application address, and maximum size before using it elsewhere.

## Build verification

- Bootloader is linked at flash base and fits its reservation.
- Application is linked exactly at application base.
- First vector word is an aligned SRAM stack pointer.
- Second vector word is an odd Thumb address inside the application region.
- Image fits both active and staging capacity.
- Metadata/config erase units do not overlap.
- Debug and release builds select the intended linker scripts.
- Builds succeed with warnings reviewed.

## Runtime evidence

A healthy update demonstrates HTTP 2xx responses, remote version greater than compiled current version, matching content length, monotonic chunk offsets, matching stream/staging CRCs, committed pending metadata, bootloader install, application boot, and telemetry reporting the new compiled version.

## Diagnostic signatures

| Evidence | Likely cause | Next check |
|---|---|---|
| HTTP 200, manifest rejected for size | Single-chunk assumption or bound too small | Bounded multi-chunk text download |
| Repeated `captured=0` | Modem gave no response or RX is not armed | Probe `AT`; log UART errors/re-arm status |
| Partial capture, missing header, error `0x08` | USART overrun | ISR duration, priorities, prints, isolation, DMA |
| Alternating reads fail | Delayed HTTPREAD trailer contaminates next capture | Positive-header scan and completion guard |
| Download completes, CRC differs | Lost/reordered bytes, wrong binary, or CRC convention | Compare hosted bytes, offsets, CRC parameters |
| Bootloader does not jump | Linker base, VTOR, vector, or SRAM validation mismatch | Inspect linker map and first vector words |
| OTA succeeds, telemetry version is old | Binary compiled with old version | Rebuild and regenerate manifest from new binary |
| Same release downloads repeatedly | Compiled version remains below manifest | Enforce release invariant |

## Safety boundaries

- Never erase active application before staged-image validation.
- Never trust manifest addresses without comparing compiled layout constants.
- CRC detects corruption, not malicious firmware; production threat models need signatures and anti-rollback.
- Do not flash hardware or publish artifacts without explicit authorization.

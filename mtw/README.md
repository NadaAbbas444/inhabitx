# PSoC Edge E84 source and compiled firmware

Snapshot of the supplied `mtw/Inhabix` project, uploaded on 2026-11-28.

## What this snapshot runs

This is Infineon's DEEPCRAFT Deploy Audio demonstration. The CM55 model outputs `unlabelled` and `baby_cry`; it is **not yet the classroom occupancy model**. The existing repository's `board/` MQTT/node code is separate and has not been integrated into this ModusToolbox application.

| Setting | Value |
| --- | --- |
| Target BSP | APP_KIT_PSE84_EVAL_EPC2 |
| Configuration | Debug |
| Toolchain | GCC_ARM, Arm GNU 14.2.1 |
| Inference CPU | CM55 |
| Projects | CM33 secure, CM33 non-secure, CM55 |
| Build tools used | ModusToolbox 3.9; Edge Protect Security Suite for final combine/sign processing |

## Contents

- `Inhabix/`: application source, model exports, BSP, configurator files, generated BSP source, dependency declarations/version locks, upstream documentation and licenses.
- `artifacts/APP_KIT_PSE84_EVAL_EPC2/Debug/app_combined.hex`: completed combined image from the supplied build.
- The same artifacts directory includes per-core ELF, HEX and BIN files, the CM33 secure image produced by the combine/sign process, and memory reports.
- `SHA256SUMS`: SHA-256 checksums of all included build artifacts, with paths relative to the repository root.
- `snapshot-manifest.json`: SHA-256, Git blob SHA-1 and sizes for every supplied source/artifact file.

Downloaded shared libraries (`mtb_shared`), generated library discovery caches, intermediate objects, IDE caches and generated BSP reference documentation are excluded. The project's dependency files and version locks are included so libraries can be fetched with `make getlibs`.

## Build locally

Install ModusToolbox 3.9, Arm GNU Toolchain 14.2.1 and Edge Protect Security Suite through ModusToolbox Setup. From modus-shell in a checkout path without spaces:

```bash
cd mtw/Inhabix
make getlibs
make -j8 build
```

No board is required for compilation or final image generation. The rebuilt combined image is written to `Inhabix/build/app_combined.hex` (relative to this directory). Local rebuilds may differ byte-for-byte because of paths and build metadata.

To regenerate editor configuration for your own installation, run `make vscode` from the application directory.

## Program later

Use the matching EPC2 board, connect its KitProg3 USB port, follow the upstream hardware setup in `Inhabix/README.md`, and run `make program` from the application directory after building. The individual raw BIN files have different load addresses; they are not interchangeable standalone firmware images.

## Verification and limits

All uploaded file contents were checked against their locally calculated Git blob hashes. Intel HEX record lengths, checksums and EOF records were validated for the combined image and available per-core HEX files. The combined-image SHA-256 is:

```text
f3e72f6c02c8402de0b409d33c10e5ae47dfaade28967407d6c577f54021cdd0
```

The supplied files contain the completed combine/sign output; the assistant did not rebuild or flash this snapshot, and runtime behavior on the board has not been verified here.

Upstream licenses remain with their source files. Classroom model integration, occupancy filtering and InhabitX network reporting remain subsequent work.

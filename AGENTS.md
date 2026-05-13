# Repository Guidelines

## Project Structure & Module Organization

This workspace contains AnyCloud37E SDK variants and hardware assets. The main SDK trees are `AnyCloud37E_SDK_V1.10_EXT/` and `AnyCloud37E_SDK_V1.10_TestBoard/`; use the tree for your board target.

- `bridge/`: board-specific DTS files and startup scripts copied into builds.
- `os/`: U-Boot, Linux kernel, and driver sources/build outputs.
- `platform/`: public headers, prebuilt libraries, and `platform/sample/` C examples.
- `rootfs/`: root filesystem inputs, scripts, kernel modules, and application resources.
- `tools/`: burn, environment, debug, and packaging tools.
- `upgrade/` and `image/`: generated firmware images and packaging inputs.
- `lcd_tp/`: LCD/touch-panel binaries, archives, and vendor notes.

## Build, Test, and Development Commands

Run SDK build commands from the selected SDK directory, for example:

```sh
cd AnyCloud37E_SDK_V1.10_EXT
./build.sh -a
```

`./build.sh -a` builds the environment image, rootfs, upgrade package, and copies images into `tools/burntool/`. Use `./build.sh -s` for the menu: `bu` builds U-Boot, `bk` builds the kernel, `be` builds env, `br` builds rootfs, `mi all` packages images, and `cp` copies burn-tool payloads.

Build samples from `platform/sample/` with `make`, or from a specific sample such as `platform/sample/venc/`.

## Coding Style & Naming Conventions

Most source is C, shell, DTS, Makefile, and vendor configuration. Follow the surrounding file style. C samples use names such as `ak_venc_sample.c` and `ak_*` APIs; keep new sample names module-oriented. Shell scripts should stay Bash compatible, use clear function names like `build_rootfs`, and preserve executable bits. Keep board files named by board and SoC, for example `EVB_CBDM_AK3760E_V1.0.1.dts`.

## Testing Guidelines

No repository-wide automated test suite is defined. Validate by building the smallest affected unit first, then the full target package when firmware artifacts change. For driver, kernel, rootfs, or boot-script work, include target-board smoke results: boot status, module load status, peripheral exercised, and image flashed.

## Commit & Pull Request Guidelines

The root is not a Git repository, and the tracked TestBoard SDK has only one visible commit, so there is no strong local convention. Use concise, scoped messages such as `rootfs: update wifi startup script` or `lcd: add JD9165T panel binary`. PRs should describe the SDK tree, board variant, build commands, generated image paths, and target-board validation. Include serial logs when boot, display, or flashing behavior changes.

## Agent-Specific Instructions

Do not delete generated or vendor binary artifacts unless explicitly requested. Many SDK directories contain dirty build outputs; inspect before editing and keep changes scoped to the requested SDK tree.

#!/bin/bash

set -euo pipefail

APP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(cd "$APP_DIR/.." && pwd)"
EXT_DIR="${EXT_SDK_DIR:-$WORKSPACE_DIR/AnyCloud37E_SDK_V1.10_EXT}"
ROOTFS_DIR="$EXT_DIR/rootfs"
UPGRADE_DIR="$EXT_DIR/upgrade"
PLATFORM_DIR="$UPGRADE_DIR/platform"
APP_BIN="$APP_DIR/build/DOORBELL_anyka_release.BIN"
APP_NAME="SAT_ANYKA.BIN"
OUT_IMAGE="$APP_DIR/build/SAT_ANYKA.IMG"
BUILD_APP=1

usage() {
    cat <<EOF
Usage:
  ./package_sd_upgrade.sh [options] [partition ...]

Partitions:
  app uboot env dtb kernel logo rootfs usr config data tuya home all

Options:
  --no-build    Skip make and use the existing app binary.
  --out <file>  Copy final SAT_ANYKA.IMG to this path.
  -h, --help    Show this help.

Examples:
  ./package_sd_upgrade.sh
  ./package_sd_upgrade.sh app
  ./package_sd_upgrade.sh app dtb kernel
  make sd PARTS="app dtb"
EOF
}

log() { printf '[package_sd_upgrade] %s\n' "$*"; }
die() { printf '[package_sd_upgrade][ERROR] %s\n' "$*" >&2; exit 1; }
need_file() { [ -f "$1" ] || die "missing file: $1"; }

normalize_part() {
    case "$1" in
        app|uboot|env|dtb|kernel|logo|rootfs|usr|config|data|tuya|home|all)
            printf '%s\n' "$1"
            ;;
        u-boot)
            printf 'uboot\n'
            ;;
        user)
            printf 'usr\n'
            ;;
        *)
            die "unsupported partition: $1"
            ;;
    esac
}

PARTS=()
while [ "$#" -gt 0 ]; do
    case "$1" in
        --no-build)
            BUILD_APP=0
            shift
            ;;
        --out)
            [ "$#" -ge 2 ] || die "--out requires a file path"
            OUT_IMAGE="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            PARTS+=("$(normalize_part "$1")")
            shift
            ;;
    esac
done

[ "${#PARTS[@]}" -gt 0 ] || PARTS=(app)
[ -d "$EXT_DIR" ] || die "EXT SDK not found: $EXT_DIR"
[ -d "$ROOTFS_DIR" ] || die "rootfs dir not found: $ROOTFS_DIR"
[ -d "$UPGRADE_DIR" ] || die "upgrade dir not found: $UPGRADE_DIR"
[ -d "$PLATFORM_DIR" ] || die "platform dir not found: $PLATFORM_DIR"

contains_part() {
    local target="$1" part
    for part in "${PARTS[@]}"; do
        [ "$part" = "all" ] && return 0
        [ "$part" = "$target" ] && return 0
    done
    return 1
}

if contains_part app; then
    if [ "$BUILD_APP" -eq 1 ]; then
        log "building app"
        make -C "$APP_DIR"
    fi

    need_file "$APP_BIN"
    log "copy app binary to EXT"
    cp -f "$APP_BIN" "$ROOTFS_DIR/rootfs/app/app/$APP_NAME"
    cp -f "$APP_BIN" "$ROOTFS_DIR/resource/app/app/$APP_NAME"

    log "build app.sqsh4"
    (
        cd "$ROOTFS_DIR"
        ./mksquashfs rootfs/app/app app.sqsh4 -noappend -comp xz
    )
    cp -f "$ROOTFS_DIR/app.sqsh4" "$PLATFORM_DIR/app.sqsh4"
fi

need_file "$UPGRADE_DIR/make_image.sh"

cd "$UPGRADE_DIR"
if [ "${#PARTS[@]}" -eq 1 ] && [ "${PARTS[0]}" = "app" ]; then
    log "packaging app partition"
    ./make_image.sh app
elif [ "${#PARTS[@]}" -eq 1 ] && [ "${PARTS[0]}" = "all" ]; then
    log "packaging all partitions"
    ./make_image.sh all
else
    if contains_part uboot; then
        need_file "$UPGRADE_DIR/boot_tool.bin"
    fi
    log "packaging custom partitions: ${PARTS[*]}"
    answers=()
    for part in uboot env dtb kernel logo rootfs usr config app data tuya home; do
        if contains_part "$part"; then
            answers+=("y")
        else
            answers+=("n")
        fi
    done
    printf '%s\n' "${answers[@]}" | ./make_image.sh
fi

mkdir -p "$(dirname "$OUT_IMAGE")"
cp -f "$UPGRADE_DIR/SAT_ANYKA.IMG" "$OUT_IMAGE"
log "done: $UPGRADE_DIR/SAT_ANYKA.IMG"
log "copy: $OUT_IMAGE"

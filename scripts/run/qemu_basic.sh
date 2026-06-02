#!/usr/bin/env bash
#  Trunk — Standard QEMU (TCG, SDL, UEFI with BIOS fallback)
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT_DIR/scripts/lib/common.sh"
source "$ROOT_DIR/config/qemu.cfg"

BOOT_MODE="auto"
while [[ $# -gt 0 ]]; do
    case "$1" in
        --iso)  BOOT_MODE="iso";  shift ;;
        --disk) BOOT_MODE="disk"; shift ;;
        *)      shift ;;
    esac
done

BOOT_FLAGS=$(get_boot_flags "$BOOT_MODE")
UEFI_FLAGS=$(get_uefi_flags)
LOG=$(make_log "qemu_basic")

info "Launching QEMU (basic)"
info "Log: $LOG"

$QEMU_BIN \
    -name    "Trunk" \
    -machine "$QEMU_MACHINE,accel=$QEMU_ACCEL_TCG" \
    -cpu     "$QEMU_CPU_BASIC" \
    -smp     "cores=$QEMU_SMP_CORES,threads=$QEMU_SMP_THREADS" \
    -m       "$QEMU_RAM" \
    -vga     "$QEMU_VGA" \
    -display "$QEMU_DISPLAY" \
    -serial  "$QEMU_SERIAL" \
    -D       "$LOG" \
    $UEFI_FLAGS \
    $BOOT_FLAGS \
    $QEMU_FLAGS_COMMON
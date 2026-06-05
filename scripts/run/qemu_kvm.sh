#!/usr/bin/env bash
#  Trunk — KVM accelerated QEMU (UEFI with BIOS fallback)
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

if [[ ! -e /dev/kvm ]]; then
    warn "/dev/kvm not found — falling back to TCG"
    ACCEL="$QEMU_ACCEL_TCG"
    CPU="$QEMU_CPU_BASIC"
else
    ok "KVM available"
    ACCEL="$QEMU_ACCEL_KVM"
    CPU="$QEMU_CPU_KVM"
fi

BOOT_FLAGS=$(get_boot_flags "$BOOT_MODE")
UEFI_FLAGS=$(get_uefi_flags)
LOG=$(make_log "qemu_kvm")

info "Launching QEMU (KVM)"
info "Log: $LOG"

$QEMU_BIN \
    -name    "Trunk [KVM]" \
    -machine "$QEMU_MACHINE,accel=$ACCEL" \
    -cpu     "$CPU" \
    -smp     "cores=$QEMU_SMP_CORES,threads=$QEMU_SMP_THREADS" \
    -m       "$QEMU_RAM" \
    -vga     "$QEMU_VGA" \
    -display "$QEMU_DISPLAY" \
    -serial  "$QEMU_SERIAL" \
    -D       "$LOG" \
    $UEFI_FLAGS \
    $BOOT_FLAGS \
    $QEMU_FLAGS_COMMON
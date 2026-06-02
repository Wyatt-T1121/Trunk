#!/usr/bin/env bash
#  Trunk — QEMU with GDB server (CPU frozen at boot)
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
LOG=$(make_log "qemu_gdb")

printf "\n"
info "QEMU GDB — CPU frozen at boot"
warn "In another terminal run:"
printf "    ${CYAN}x86_64-elf-gdb build/elf/trunk.elf${RESET}\n"
printf "    ${CYAN}(gdb) target remote :${QEMU_GDB_PORT}${RESET}\n"
printf "    ${CYAN}(gdb) continue${RESET}\n"
info "QEMU monitor: telnet :55555"
info "Log: $LOG"
printf "\n"

$QEMU_BIN \
    -name    "Trunk [GDB]" \
    -machine "$QEMU_MACHINE,accel=$QEMU_ACCEL_TCG" \
    -cpu     "$QEMU_CPU_BASIC" \
    -smp     "cores=$QEMU_SMP_CORES,threads=$QEMU_SMP_THREADS" \
    -m       "$QEMU_RAM" \
    -vga     "$QEMU_VGA" \
    -display "$QEMU_DISPLAY" \
    -serial  "$QEMU_SERIAL" \
    -monitor "telnet:127.0.0.1:55555,server,nowait" \
    -D       "$LOG" \
    $UEFI_FLAGS \
    $BOOT_FLAGS \
    $QEMU_FLAGS_COMMON \
    -s -S
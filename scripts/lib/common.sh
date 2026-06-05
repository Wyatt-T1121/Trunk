#!/usr/bin/env bash
#  Trunk — Shared library sourced by all scripts

# --- Colors ------------------------------------------------------------------
RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
BOLD='\033[1m'
RESET='\033[0m'

# --- Output ------------------------------------------------------------------
ok()   { printf "  ${GREEN}[ OK ]${RESET}    %s\n" "$1"; }
info() { printf "  ${CYAN}[ INFO ]${RESET}   %s\n" "$1"; }
warn() { printf "  ${YELLOW}[ WARN ]${RESET} %s\n" "$1"; }
fail() { printf "  ${RED}[ FAIL ]${RESET}    %s\n" "$1"; exit 1; }
step() { printf "  ${BLUE}[....]${RESET}     %s\n" "$1"; }

# --- Dependency checker ------------------------------------------------------
check_dep()  { command -v "$1" &>/dev/null || fail "Missing: $1"; }
check_deps() { for d in "$@"; do check_dep "$d"; done; ok "Dependencies OK"; }

# --- Boot flag resolver ------------------------------------------------------
# Usage: BOOT_FLAGS=$(get_boot_flags [auto|iso|disk])
# Requires QEMU_DISK and QEMU_ISO to be set (from qemu.cfg)
get_boot_flags() {
    local mode="${1:-auto}"
    case "$mode" in
        disk)
            [[ -f "$QEMU_DISK" ]] || fail "Disk image not found: $QEMU_DISK"
            info "Boot target: disk ($QEMU_DISK)" >&2
            echo "-drive file=$QEMU_DISK,format=raw,if=ide -boot order=c"
            ;;
        iso)
            [[ -f "$QEMU_ISO" ]] || fail "ISO not found: $QEMU_ISO"
            info "Boot target: ISO ($QEMU_ISO)" >&2
            echo "-cdrom $QEMU_ISO -boot order=d"
            ;;
        auto)
            if [[ -f "$QEMU_DISK" ]]; then
                info "Boot target: disk (auto)" >&2
                echo "-drive file=$QEMU_DISK,format=raw,if=ide -boot order=c"
            elif [[ -f "$QEMU_ISO" ]]; then
                info "Boot target: ISO (auto fallback)" >&2
                echo "-cdrom $QEMU_ISO -boot order=d"
            else
                fail "No boot target found. Run 'make' or 'make disk' first."
            fi
            ;;
        *) fail "get_boot_flags: unknown mode '$mode'" ;;
    esac
}

# --- OVMF UEFI firmware finder -----------------------------------------------
# Returns path to OVMF.fd or empty string if not found
find_ovmf() {
    local candidates=(
        "/usr/share/ovmf/OVMF.fd"
        "/usr/share/OVMF/OVMF.fd"
        "/usr/share/edk2/ovmf/OVMF.fd"
        "/usr/share/qemu/OVMF.fd"
    )
    for f in "${candidates[@]}"; do
        [[ -f "$f" ]] && echo "$f" && return
    done
    echo ""
}

# --- UEFI firmware flags -----------------------------------------------------
# Returns -bios flag if OVMF found, empty if not (falls back to SeaBIOS/BIOS)
get_uefi_flags() {
    local ovmf
    ovmf=$(find_ovmf)
    if [[ -n "$ovmf" ]]; then
        info "UEFI firmware: $ovmf" >&2
        echo "-bios $ovmf"
    else
        warn "OVMF not found — falling back to BIOS (install: sudo apt install ovmf)" >&2
        echo ""
    fi
}

# --- Log file creator --------------------------------------------------------
# Usage: LOG=$(make_log "qemu_basic")
# Requires QEMU_LOG to be set (from qemu.cfg)
make_log() {
    local name="$1"
    mkdir -p "$QEMU_LOG"
    echo "$QEMU_LOG/${name}_$(date +%Y%m%d_%H%M%S).log"
}

# --- Root check --------------------------------------------------------------
require_root() {
    [[ $EUID -eq 0 ]] || fail "This script must be run as root (sudo)"
}

# --- Kernel check ------------------------------------------------------------
require_kernel() {
    [[ -f "build/elf/trunk.elf" ]] || \
        fail "trunk.elf not found. Run 'make kernel' first."
}
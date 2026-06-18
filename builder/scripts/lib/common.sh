#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR_COMMON="${ROOT_DIR_COMMON:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)}"
[[ -f "$ROOT_DIR_COMMON/config/build.cfg" ]] && source "$ROOT_DIR_COMMON/config/build.cfg"

RED='\033[0;31m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
BOLD='\033[1m'
RESET='\033[0m'

VERSION="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"

OBJ_DIR="${BUILD_DIR}/obj"
ELF_DIR="${BUILD_DIR}/elf"
LIB_DIR="${BUILD_DIR}/lib"
ISO_DIR="${BUILD_DIR}/iso"
IMG_DIR="${BUILD_DIR}/img"
LOG_DIR="${BUILD_DIR}/logs"
LOG_QEMU_DIR="${LOG_DIR}/qemu"
LOG_BUILD_DIR="${LOG_DIR}/build"

LINKER_DIR="${SETUP_DIR}/linker"
GRUB_DIR="${SETUP_DIR}/grub"

TRUNK_ELF="${ELF_DIR}/trunk.elf"
TKLIB_A="${LIB_DIR}/tklib.a"
ISO_IMAGE="${ISO_DIR}/trunk.iso"
DISK_IMAGE="${IMG_DIR}/trunk.img"

ok()   { printf "  ${GREEN}[ OK ]${RESET}  %s\n"    "$1"; }
info() { printf "  ${CYAN}[ INFO ]${RESET}  %s\n"   "$1"; }
warn() { printf "  ${YELLOW}[ WARN ]${RESET}  %s\n" "$1"; }
fail() { printf "  ${RED}[ FAIL ]${RESET}  %s\n"    "$1"; exit 1; }
step() { printf "  ${BLUE}[ .... ]${RESET}  %s\n"   "$1"; }

check_dep()  { command -v "$1" &>/dev/null || fail "Missing: $1"; }
check_deps() { for d in "$@"; do check_dep "$d"; done; ok "Dependencies OK"; }

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

make_log() {
    local name="$1"
    mkdir -p "$QEMU_LOG"
    echo "$QEMU_LOG/${name}_$(date +%Y%m%d_%H%M%S).log"
}

require_root() {
    [[ $EUID -eq 0 ]] || fail "This script must be run as root (sudo)"
}

require_kernel() {
    [[ -f "$TRUNK_ELF" ]] || fail "trunk.elf not found. Run 'make' first."
}
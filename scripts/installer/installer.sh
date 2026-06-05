#!/usr/bin/env bash
#  Trunk — Installs GRUB (UEFI + BIOS fallback) and kernel onto disk
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT_DIR/scripts/lib/common.sh"
source "$ROOT_DIR/config/disk.cfg"

require_root
check_deps grub-install losetup mount umount

# --- Arguments ---------------------------------------------------------------
TARGET_TYPE="img"
TARGET="$DISK_IMAGE"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --disk) TARGET_TYPE="disk"; TARGET="$2"; shift 2 ;;
        --img)  TARGET_TYPE="img";  TARGET="$2"; shift 2 ;;
        *)      shift ;;
    esac
done

require_kernel
[[ -f "setup/grub/grub.cfg" ]] || fail "grub.cfg not found."

# --- Safety warning ----------------------------------------------------------
if [[ "$TARGET_TYPE" == "disk" ]]; then
    [[ -b "$TARGET" ]] || fail "$TARGET is not a block device"
    warn "WARNING: This will ERASE all data on $TARGET"
    warn "Press Ctrl+C within 5 seconds to cancel..."
    sleep 5
    PART1="${TARGET}1"
    LOOP=""
else
    [[ -f "$TARGET" ]] || fail "Image not found: $TARGET. Run 'make disk' first."
    step "Attaching loop device..."
    LOOP=$(losetup --find --show --partscan "$TARGET")
    ok "Loop: $LOOP"
    PART1="${LOOP}p1"
fi

# --- Cleanup trap ------------------------------------------------------------
MOUNT=$(mktemp -d)
cleanup() {
    umount "$MOUNT"      2>/dev/null || true
    rm -rf "$MOUNT"
    [[ -n "$LOOP" ]] && losetup -d "$LOOP" 2>/dev/null || true
}
trap cleanup EXIT

# --- Mount ESP ---------------------------------------------------------------
step "Mounting ESP..."
mount "$PART1" "$MOUNT"
ok "Mounted at $MOUNT"

# --- Copy files --------------------------------------------------------------
step "Copying kernel and grub.cfg..."
mkdir -p "$MOUNT/boot/grub"
cp build/elf/trunk.elf  "$MOUNT/boot/trunk.elf"
cp setup/grub/grub.cfg  "$MOUNT/boot/grub/grub.cfg"
ok "Files copied"

# --- Install GRUB UEFI -------------------------------------------------------
step "Installing GRUB (UEFI x86_64-efi)..."
mkdir -p build/logs/build
grub-install \
    --target=x86_64-efi \
    --efi-directory="$MOUNT" \
    --boot-directory="$MOUNT/boot" \
    --removable \
    --no-nvram \
    2>build/logs/build/grub_uefi.log \
    && ok "GRUB UEFI installed" \
    || warn "GRUB UEFI failed — see build/logs/build/grub_uefi.log"

# --- Install GRUB BIOS fallback ----------------------------------------------
step "Installing GRUB (BIOS i386-pc fallback)..."
grub-install \
    --target=i386-pc \
    --boot-directory="$MOUNT/boot" \
    "${LOOP:-$TARGET}" \
    2>build/logs/build/grub_bios.log \
    && ok "GRUB BIOS fallback installed" \
    || warn "GRUB BIOS failed — see build/logs/build/grub_bios.log"

ok "Installation complete: $TARGET"
info "UEFI: primary boot method"
info "BIOS: fallback if UEFI unavailable"
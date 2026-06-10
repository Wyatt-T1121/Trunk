#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

source "$ROOT_DIR/scripts/lib/common.sh"
source "$ROOT_DIR/config/build.cfg"
source "$ROOT_DIR/config/disk.cfg"

require_root
check_deps grub-install losetup mount umount

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
[[ -f "$GRUB_DIR/grub.cfg" ]] || fail "grub.cfg not found: $GRUB_DIR/grub.cfg"

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

MOUNT=$(mktemp -d)
cleanup() {
    umount "$MOUNT"    2>/dev/null || true
    rm -rf "$MOUNT"
    [[ -n "${LOOP:-}" ]] && losetup -d "$LOOP" 2>/dev/null || true
}
trap cleanup EXIT

step "Mounting ESP..."
mount "$PART1" "$MOUNT"
ok "Mounted at $MOUNT"

step "Copying kernel and grub.cfg..."
mkdir -p "$MOUNT/boot/grub"
cp "$TRUNK_ELF"          "$MOUNT/boot/trunk.elf"
cp "$GRUB_DIR/grub.cfg"  "$MOUNT/boot/grub/grub.cfg"
ok "Files copied"

step "Installing GRUB (UEFI x86_64-efi)..."
mkdir -p "$LOG_BUILD_DIR"
grub-install \
    --target=x86_64-efi \
    --efi-directory="$MOUNT" \
    --boot-directory="$MOUNT/boot" \
    --removable \
    --no-nvram \
    2>"$LOG_BUILD_DIR/grub_uefi.log" \
    && ok "GRUB UEFI installed" \
    || warn "GRUB UEFI failed — see $LOG_BUILD_DIR/grub_uefi.log"

step "Installing GRUB (BIOS i386-pc fallback)..."
grub-install \
    --target=i386-pc \
    --boot-directory="$MOUNT/boot" \
    "${LOOP:-$TARGET}" \
    2>"$LOG_BUILD_DIR/grub_bios.log" \
    && ok "GRUB BIOS fallback installed" \
    || warn "GRUB BIOS failed — see $LOG_BUILD_DIR/grub_bios.log"

ok "Installation complete: $TARGET"
info "UEFI: primary boot method"
info "BIOS: fallback if UEFI unavailable"
#!/usr/bin/env bash
#  Trunk — Creates a blank GPT disk image
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

source "$ROOT_DIR/scripts/lib/common.sh"
source "$ROOT_DIR/config/disk.cfg"

check_deps parted mkfs.fat mkfs.ext2 fallocate losetup

mkdir -p "$(dirname "$DISK_IMAGE")"

# --- Raw image ---------------------------------------------------------------
step "Creating raw image: $DISK_IMAGE ($DISK_SIZE)..."
rm -f "$DISK_IMAGE"
fallocate -l "$DISK_SIZE" "$DISK_IMAGE"
ok "Raw image created"

# --- GPT partition table -----------------------------------------------------
step "Writing GPT partition table..."
parted -s "$DISK_IMAGE" mklabel gpt

# ESP — EFI System Partition (FAT32, bootable)
parted -s "$DISK_IMAGE" mkpart "$BOOT_PART_NAME" fat32 \
    "$BOOT_PART_START" "$BOOT_PART_END"
parted -s "$DISK_IMAGE" set 1 esp on

# Root partition
parted -s "$DISK_IMAGE" mkpart "$ROOT_PART_NAME" ext2 \
    "$ROOT_PART_START" "$ROOT_PART_END"
ok "GPT partitions created"

# --- Loop device -------------------------------------------------------------
step "Setting up loop device..."
LOOP=$(sudo losetup --find --show --partscan "$DISK_IMAGE")
ok "Loop device: $LOOP"

cleanup() { sudo losetup -d "$LOOP" 2>/dev/null || true; }
trap cleanup EXIT

# --- Format ------------------------------------------------------------------
step "Formatting ESP (FAT32)..."
sudo mkfs.fat -F32 -n "$BOOT_PART_NAME" "${LOOP}p1"
ok "ESP formatted"

step "Formatting root (ext2)..."
sudo mkfs.ext2 -L "$ROOT_PART_NAME" "${LOOP}p2"
ok "Root formatted"

ok "Disk image ready: $DISK_IMAGE"
info "Next: run 'make install' to install GRUB and kernel"
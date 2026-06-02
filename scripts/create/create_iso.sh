#!/usr/bin/env bash
#  Trunk — Builds a hybrid UEFI+BIOS bootable ISO via grub-mkrescue
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
source "$ROOT_DIR/scripts/lib/common.sh"
source "$ROOT_DIR/config/build.cfg"

check_deps grub-mkrescue
require_kernel

[[ -f "setup/grub/grub.cfg" ]] || fail "grub.cfg not found at setup/grub/grub.cfg"

# --- Stage -------------------------------------------------------------------
step "Staging ISO..."
mkdir -p build/iso/boot/grub
cp build/elf/trunk.elf  build/iso/boot/trunk.elf
cp setup/grub/grub.cfg  build/iso/boot/grub/grub.cfg
ok "ISO staged"

# --- Build -------------------------------------------------------------------
step "Running grub-mkrescue (UEFI + BIOS hybrid)..."
mkdir -p build/logs/build
grub-mkrescue -o build/iso/trunk.iso build/iso \
    2>build/logs/build/grub.log

[[ -f "build/iso/trunk.iso" ]] || \
    fail "ISO creation failed. See build/logs/build/grub.log"

ok "ISO ready: build/iso/trunk.iso"
info "Size: $(ls -lh build/iso/trunk.iso | awk '{print $5}')"
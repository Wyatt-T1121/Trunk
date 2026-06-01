#!/usr/bin/env bash
set -euo pipefail

ISO="build/trunk.iso"

if [[ ! -f "$ISO" ]]; then
    echo "[trunk] ISO not found. Run 'make iso' first."
    exit 1
fi

qemu-system-x86_64 \
    -name "Trunk OS"       \
    -machine q35,accel=tcg \
    -cpu qemu64            \
    -smp cores=4,threads=1 \
    -m 2G                  \
    -cdrom "$ISO"          \
    -boot order=d          \
    -vga std               \
    -display sdl           \
    -serial stdio          \
    -no-reboot             \
    -D build/qemu.log
#!/usr/bin/env bash
#  Trunk OS — QEMU run script (standard, high settings)
set -euo pipefail

ISO="build/trunk.iso"

if [[ ! -f "$ISO" ]]; then
    echo "[trunk] ISO not found. Run 'make iso' first."
    exit 1
fi

echo "[trunk] Booting $ISO in QEMU..."

qemu-system-x86_64 \
    -name "Trunk OS"                \
    -machine q35,accel=tcg          \
    -cpu qemu64                     \
    -smp cores=4,threads=1          \
    -m 2G                           \
    -cdrom "$ISO"                   \
    -boot order=d                   \
    -drive file=disk.img,format=raw,if=ide,index=1 2>/dev/null || \
    qemu-system-x86_64 \
        -name "Trunk OS"            \
        -machine q35,accel=tcg      \
        -cpu qemu64                 \
        -smp cores=4,threads=1      \
        -m 2G                       \
        -cdrom "$ISO"               \
        -boot order=d               \
    -vga std                        \
    -display sdl                    \
    -serial stdio                   \
    -no-reboot                      \
    -D build/qemu.log               \
    2>&1

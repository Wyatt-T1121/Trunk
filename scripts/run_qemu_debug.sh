#!/usr/bin/env bash
#  Trunk OS — QEMU debug script
#  Starts QEMU frozen, waiting for GDB on localhost:1234
#  Usage: make run-debug   (then in another terminal: gdb build/trunk.elf)
set -euo pipefail

ISO="build/trunk.iso"
ELF="build/trunk.elf"

if [[ ! -f "$ISO" ]]; then
    echo "[trunk] ISO not found. Run 'make iso' first."
    exit 1
fi

echo "[trunk] Debug mode — QEMU frozen, GDB server on :1234"
echo "[trunk] In another terminal run:"
echo "          x86_64-elf-gdb $ELF"
echo "          (gdb) target remote :1234"
echo "          (gdb) continue"
echo ""

qemu-system-x86_64 \
    -name "Trunk OS [DEBUG]"        \
    -machine q35,accel=tcg          \
    -cpu qemu64                     \
    -smp cores=4,threads=1          \
    -m 2G                           \
    -cdrom "$ISO"                   \
    -boot order=d                   \
    -vga std                        \
    -display sdl                    \
    -serial stdio                   \
    -monitor telnet:127.0.0.1:55555,server,nowait \
    -no-reboot                      \
    -D build/qemu_debug.log         \
    -s -S                           \
    2>&1

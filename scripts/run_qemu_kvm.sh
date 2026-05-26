#!/usr/bin/env bash
#  Trunk OS — QEMU KVM script (hardware acceleration, fastest)
#  Requires: Linux host with KVM enabled and user in 'kvm' group
set -euo pipefail

ISO="build/trunk.iso"

if [[ ! -f "$ISO" ]]; then
    echo "[trunk] ISO not found. Run 'make iso' first."
    exit 1
fi

# Check KVM availability
if [[ ! -e /dev/kvm ]]; then
    echo "[trunk] /dev/kvm not found. KVM unavailable — falling back to TCG."
    ACCEL="tcg"
    CPU_FLAG="-cpu qemu64"
else
    ACCEL="kvm"
    CPU_FLAG="-cpu host"
    echo "[trunk] KVM available — using hardware acceleration."
fi

echo "[trunk] Booting $ISO with KVM..."

qemu-system-x86_64 \
    -name "Trunk OS [KVM]"              \
    -machine q35,accel=${ACCEL}         \
    ${CPU_FLAG}                         \
    -smp cores=4,threads=2,sockets=1    \
    -m 2G                               \
    -cdrom "$ISO"                       \
    -boot order=d                       \
    -vga std                            \
    -display sdl                        \
    -serial stdio                       \
    -no-reboot                          \
    -D build/qemu_kvm.log               \
    2>&1

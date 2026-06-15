#!/usr/bin/env python3
#  Trunk — Toolchain and system dependency checker

import shutil
import sys
import os
import subprocess

# --- Colors ------------------------------------------------------------------
RED    = '\033[0;31m'
GREEN  = '\033[0;32m'
CYAN   = '\033[0;36m'
YELLOW = '\033[0;33m'
RESET  = '\033[0m'

def ok(msg):   print(f"  {GREEN}[ OK ]{RESET}  {msg}")
def info(msg): print(f"  {CYAN}[INFO]{RESET}  {msg}")
def warn(msg): print(f"  {YELLOW}[WARN]{RESET}  {msg}")
def fail(msg): print(f"  {RED}[FAIL]{RESET}  {msg}")

# --- Dependency groups -------------------------------------------------------

CROSS_TOOLCHAIN = [
    ("x86_64-elf-g++",     "Cross C++ compiler"),
    ("x86_64-elf-ld",      "Cross linker"),
    ("x86_64-elf-as",      "Cross assembler"),
    ("x86_64-elf-objcopy", "Cross objcopy"),
    ("x86_64-elf-objdump", "Cross objdump"),
    ("x86_64-elf-nm",      "Cross nm"),
    ("x86_64-elf-readelf", "Cross readelf"),
]

BUILD_TOOLS = [
    ("nasm",           "NASM assembler"),
    ("make",           "GNU Make"),
    ("grub-mkrescue",  "GRUB ISO builder"),
    ("grub-install",   "GRUB installer"),
    ("xorriso",        "ISO creation backend"),
    ("python3",        "Python 3"),
]

DISK_TOOLS = [
    ("parted",         "Partition tool"),
    ("mkfs.fat",       "FAT32 formatter"),
    ("mkfs.ext2",      "ext2 formatter"),
    ("losetup",        "Loop device tool"),
]

QEMU_TOOLS = [
    ("qemu-system-x86_64", "QEMU x86_64"),
]

OPTIONAL = [
    ("ovmf",           "UEFI firmware (apt: ovmf)"),
]

# --- Checker -----------------------------------------------------------------

def check_group(name, deps):
    print(f"\n  {CYAN}{name}{RESET}")
    failed = []
    for cmd, desc in deps:
        path = shutil.which(cmd)
        if path:
            ok(f"{desc:<35} {cmd}")
        else:
            fail(f"{desc:<35} {cmd} — NOT FOUND")
            failed.append(cmd)
    return failed

def check_ovmf():
    candidates = [
        "/usr/share/ovmf/OVMF.fd",
        "/usr/share/OVMF/OVMF.fd",
        "/usr/share/edk2/ovmf/OVMF.fd",
        "/usr/share/qemu/OVMF.fd",
    ]
    print(f"\n  {CYAN}UEFI Firmware (OVMF){RESET}")
    for path in candidates:
        if os.path.exists(path):
            ok(f"OVMF found: {path}")
            return
    warn("OVMF not found — UEFI boot unavailable in QEMU")
    warn("Install with: sudo apt install ovmf")

def check_gcc_version():
    try:
        out = subprocess.check_output(
            ["x86_64-elf-g++", "--version"],
            stderr=subprocess.DEVNULL
        ).decode().split('\n')[0]
        info(f"Compiler: {out}")
    except Exception:
        pass

# --- Main --------------------------------------------------------------------

def main():
    print(f"\n  {CYAN}{'='*50}{RESET}")
    print(f"  Trunk — Dependency Check")
    print(f"  {CYAN}{'='*50}{RESET}")

    all_failed = []
    all_failed += check_group("Cross Toolchain",  CROSS_TOOLCHAIN)
    all_failed += check_group("Build Tools",       BUILD_TOOLS)
    all_failed += check_group("Disk Tools",        DISK_TOOLS)
    all_failed += check_group("QEMU",              QEMU_TOOLS)

    check_ovmf()
    check_gcc_version()

    print()
    if all_failed:
        fail(f"{len(all_failed)} missing dependencies: {', '.join(all_failed)}")
        print()
        sys.exit(1)
    else:
        ok("All required dependencies satisfied")
        print()
        sys.exit(0)

if __name__ == "__main__":
    main()
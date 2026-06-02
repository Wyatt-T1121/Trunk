#!/usr/bin/env python3
#  Trunk — Python disk image creation utility

import argparse
import os
import subprocess
import sys
import tempfile

# --- Colors ------------------------------------------------------------------
RED    = '\033[0;31m'
GREEN  = '\033[0;32m'
CYAN   = '\033[0;36m'
YELLOW = '\033[0;33m'
RESET  = '\033[0m'

def ok(msg):   print(f"  {GREEN}[ OK ]{RESET}  {msg}")
def info(msg): print(f"  {CYAN}[INFO]{RESET}  {msg}")
def warn(msg): print(f"  {YELLOW}[WARN]{RESET}  {msg}")
def fail(msg): print(f"  {RED}[FAIL]{RESET}  {msg}"); sys.exit(1)

def run(cmd, check=True, capture=False):
    """Run a shell command."""
    info(f"$ {' '.join(cmd)}")
    result = subprocess.run(
        cmd,
        capture_output=capture,
        text=True
    )
    if check and result.returncode != 0:
        if capture:
            print(result.stderr)
        fail(f"Command failed: {' '.join(cmd)}")
    return result

def check_root():
    if os.geteuid() != 0:
        fail("mkdisk.py must be run as root (sudo python3 mkdisk.py)")

def check_deps():
    deps = ["parted", "mkfs.fat", "mkfs.ext2", "losetup", "fallocate"]
    for dep in deps:
        result = subprocess.run(["which", dep], capture_output=True)
        if result.returncode != 0:
            fail(f"Missing dependency: {dep}")
    ok("Dependencies satisfied")

def parse_size(size_str):
    """Convert size string (1G, 512M) to bytes."""
    units = {'K': 1024, 'M': 1024**2, 'G': 1024**3, 'T': 1024**4}
    size_str = size_str.upper()
    if size_str[-1] in units:
        return int(size_str[:-1]) * units[size_str[-1]]
    return int(size_str)

def create_image(output, size):
    """Create raw disk image."""
    os.makedirs(os.path.dirname(output) or '.', exist_ok=True)
    if os.path.exists(output):
        os.remove(output)
    run(["fallocate", "-l", size, output])
    ok(f"Raw image created: {output} ({size})")

def partition_image(image):
    """Write GPT partition table with ESP + root."""
    run(["parted", "-s", image, "mklabel", "gpt"])
    run(["parted", "-s", image, "mkpart", "TRUNK_BOOT", "fat32", "1MiB", "256MiB"])
    run(["parted", "-s", image, "mkpart", "TRUNK_ROOT", "ext2", "256MiB", "100%"])
    run(["parted", "-s", image, "set", "1", "esp", "on"])
    ok("GPT partitions created (ESP + root)")

def format_partitions(loop):
    """Format ESP as FAT32 and root as ext2."""
    run(["mkfs.fat", "-F32", "-n", "TRUNK_BOOT", f"{loop}p1"])
    ok("ESP formatted (FAT32)")
    run(["mkfs.ext2", "-L", "TRUNK_ROOT", f"{loop}p2"])
    ok("Root formatted (ext2)")

def setup_loop(image):
    """Attach loop device and return its path."""
    result = run(
        ["losetup", "--find", "--show", "--partscan", image],
        capture=True
    )
    loop = result.stdout.strip()
    ok(f"Loop device: {loop}")
    return loop

def detach_loop(loop):
    """Detach loop device."""
    subprocess.run(["losetup", "-d", loop], capture_output=True)

# --- Main --------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(
        description="Trunk — disk image creator"
    )
    parser.add_argument(
        "--size", default="1G",
        help="Disk image size (default: 1G)"
    )
    parser.add_argument(
        "--output", default="build/img/trunk.img",
        help="Output path (default: build/img/trunk.img)"
    )
    args = parser.parse_args()

    print(f"\n  {CYAN}Trunk — mkdisk.py{RESET}")
    print(f"  Output : {args.output}")
    print(f"  Size   : {args.size}")
    print()

    check_root()
    check_deps()

    create_image(args.output, args.size)
    partition_image(args.output)

    loop = setup_loop(args.output)
    try:
        format_partitions(loop)
    finally:
        detach_loop(loop)

    ok(f"Disk image ready: {args.output}")
    info("Next: sudo ./scripts/installer/installer.sh")
    print()

if __name__ == "__main__":
    main()
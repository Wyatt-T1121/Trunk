# The Trunk operating system

Trunk is a hobby x86 64-bit Operating System programmed In C++.
I created it purely for fun and as a side project, it's not meant to be an every-day operating system.

## Design

I am currently designing trunk to be a hybrid between Windows and Linux.
It follows an MS-DOS style look.
But under the hood, i merged linux style systems with Windows.

Like the linux driver module is merged with the Windows Object manager.

Of course, it will support much more modern features then the likes of MS-DOS.

## Modern features (that MS-DOS lacks):

- UEFI
- Multitasking
- Threading
- Kernel space and user space
- Multiple file systems
- Better security and crash protection
- Much better memory management
- And much more...

## How to build and run.

You will likely be required to be on a linux machine.
I'm not sure if you can build it on windows, WSL might work but you might need to modify certain things.
Usually you are on Linux if you are compiling an operating system anyway.

# Steps

1. Run 'make check-deps' this launches a python script that checks if you have the required dependencies.
   If you do not have all of them, look up a command to install each one that you are missing.

2. Booting
   - If you want to boot via HDD with .img (recommended):
     make MODE=DEBUG
     make disk
     make install
     make run-kvm, or run-gdb, or run-headless.

   - If you want to boot via DVD/CD with a .iso:
     make MODE=DEBUG
     make run-iso

Here is the Makefile list of commands.
Feel free to use any of them, debug as you wish.

# All targets:

# make / make MODE=DEBUG Build kernel + ISO

# make kernel Build ELF only

# make iso Build ISO only

# make disk Create GPT disk image

# make install Install GRUB + kernel onto disk image

# make run-iso Boot ISO in QEMU (DVD/CD)

# make run-disk Boot disk image in QEMU (HDD)

# make run-kvm Boot disk image in QEMU (KVM)

# make run-gdb Boot disk image in QEMU (GDB :1234)

# make run-headless Boot disk image in QEMU (headless)

# make clean Remove build/ entirely

# make mrproper Remove build/ + disk images

# make check-deps Verify toolchain + system dependencies

# make info Show build configuration

# make list-srcs List all discovered source files

# make disasm Disassemble kernel

# make sym Dump symbol table

# make headers Show ELF headers

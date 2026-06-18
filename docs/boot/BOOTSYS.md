## The boot system for Trunk

Trunk uses GRUB and MULTIBOOT2 for booting.
Because GRUB Is an older bootloader, It does require you to write boot code.

This document explains the boot process, order, and features.

# trldr

Even tho Trunk does not use a custom bootloader, I still named my bootcode.
This Is because It still requires a lot of setup, so It fits.

So 'trldr' Is the official boot code name for Trunk.

# GRUB order

GRUB drops us off In 32-bit mode, meaning we have to manually setup to enter 64-bit mode.
Modern bootloaders like Limine drop you directly to 64-bit mode.

So why not use Limine?

Well, GRUB Is a lot more mature.

Limine Is a newer bootloader, meaning It does not contain as much support as GRUB.

# Trunk boot process

We start off In entry32.asm, 32-bit entry code Is contained here.

The boot process Is:

entry32.asm -> builds page tables with paging.asm -> enables long mode with paging.asm -> stores MB2 In memory -> calls entry64.asm

entry64.asm -> Aligns stack to 16-bytes, calls C++ global constructors, loads MB2 from memory, calls sysstart.asm

sysstart.asm -> This acts as a simple landing pad, right before the kernel Is loaded. This simply just calls CbkLoad()

boot.cpp -> Fills, walks, and parses MB2, stores In a grand 'BootInfo' struct, sends off to CbkStartup() In CbkLoad()

kinit.cpp -> Contains CbkLoad(), the final function for the kernel loading process. Initializes subsystems and set's up the actual kernel

So that Is the boot order.
Of course, there's many more steps to It, that's just a high level overview.

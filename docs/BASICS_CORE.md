# Absolute basics for Trunk

This document covers the absolute basics for Trunk.

## Table of contents

- [Bootloader](#chapter-1---Bootloader-Choice)
- [Hardware Support](#chapter-2---Architecture-Support)

# Chapter 1 - Bootloader

Trunk was designed to use GRUB.

GRUB Is a powerful bootloader from the GNU project.

It's very common within hobby operating systems.

When It comes to making an operating system, there's 3 main choices for bootloaders

1. GRUB - My choice
2. LIMINE - Modern bootloader, better but less mature then GRUB
3. Making your own - Writing a custom bootloader for your Operating system

See, most production-grade operating systems write their own.
However, hobbyist usually use LIMINE or GRUB

The thing with writing your own Is, it takes more time then you want to spend.
Usually, you'll focus on writing the kernel.
With a bootloader, you need to manage both.

And since GRUB and LIMINE are already mature, it makes the choice easy.

However, In the future, It is worth it to write your own bootloader if you can find the time

It allows for better customization, loading, etc..

For now, Trunk will stick with GRUB, along with it's MB2 protocal

# Chapter 2 - Hardware Support

The grand question when it comes to an operating system...
'What will It support'?

This Is a really big choice, It's important to think this out...

When it comes to choosing, there's core sections:

1. Architectures
2. 16/32/64 - BIT MODES

Choosing an architecture to support in your operating system means selecting the specific Instruction Set Architecture (ISA) and hardware design that the OS kernel and applications are compiled to execute. While the operating system abstracts physical hardware details like disk drives and peripherals, it does not abstract the CPU architecture for low-level execution.

In order to support multiple architectures In your operating system, you have to do a ton of work...
You need to write architecture-specific code, just to get it working.
It involves a lot of extra boilerplate.

COMMON ARCHITECTURES:
X86
ARM
RISC-V

Here's the information:

X86 - The most common, used by INTEL.

Over 90% of desktop pc's use X86...

ARM - Common In boards like rasberry-pi, and growing in laptops

X86 still dominates laptops, but ARM Is growing quickly, it current holds around 10% of the market

RISC-V - A new instruction set

RISC-V is growing as a new instruction set, mainly to encourage Open-source, and remove old boilerplate like 32-bit mode

However... Programming for multiple architectures isn't something you want to do as a solo devloper

So for Trunk, I decided It will be x86 ONLY

Then, there's 16/32/64 - BIT MODES

Older cpu's were 16 bit, then 32 bit, then 64 bit

Early operating systems, started as 16-bit as that was the only mode.

Then they moved to 32 bit mode

And now, 64-bit mode

When it comes to choosing which one, it's very simple

Most likely, go for 64 bit mode

Even tho It would be nice to support old pc's, 64-BIT mode was made standard in 2004

That was a very long time ago, and it's also way better.

If you choose to support 16/32 bit mode, there's a lot of extra work

Overall, if you aren't targetting pc's older then 2004, use 64 bit mode

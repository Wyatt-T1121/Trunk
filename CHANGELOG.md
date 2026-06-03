# Trunk OS complete CHANGELOG

This file tracks all the core changes made to the Trunk operating system.
It is tracked in 'week' clumps.
So every change made will be clumped into the week it was made in.

# TRUNK REPORT WEEK 1

## Boot

- Early 32-bit boot code, jumps to 64-bit. Entry32.asm, Entry64.asm
- Early paging for boot setup, Paging.asm
- MULTIBOOT2 grabbed from GRUB and passed along to Boot.cpp
- Boot.cpp parses MB2, get's each regions availability, then sends to kmain(), which will later send to the Physical memory manager.

## Kernel

- Basic kmain() to initalize core subsystems and grab multiboot2 from Boot.cpp

## Global Descriptor Table

- Basic 64-bit GDT setup

- CREATES:
  Kernel code
  Kernel data
  User code
  User data

## Drivers

- Added basic Serial(UART) driver for console debugging, only used for developers.

## TKLIB (Trunk kernel library)

- Added Rust style Result<> for better error handling
- Added 'String' memory manipulation

## Reworked build system

- Reworked the build system to include Recursive Makefiles
- Added colored output to build
- Added much more scripts, each with an independent job. Also added commmon.sh for all scripts
- Created .cfg files for scripts and Makefile so they won't have to hardcode everything
- GRUB and LINKERSCRIPT properly setup

## Comment requirement

- Added dev/COMMENT.cpp, contains comments for everything in a C++ file. (Functions, Top level, Sections)

# Design for Trunk

This document covers the design for Trunk.

It explains how Trunk Is designed, future plans for design, etc..

## Table of contents

- [Graphical User Interface design](#chapter-1---graphical-user-interface-design)
- [Windows NT style codebase](#chapter-2---windows-nt-style-codebase)
- [Installation assistant](#chapter-3---installation-assistant)

# Chapter 1 - Graphical User Interface design

I've changed the design choice quite a bit, but I've landed on a unique decision.

Originally, I wanted It to be MS-DOS based, purely a terminal.
But that has changed.

The final design choice is:

A 'console' style GUI, on a PC.

Weird right? Usually, you do a desktop, with a nice task bar, some apps, etc..

But Trunk, will be following the GUI design of consoles. Specifically, 'Playstation-SERIES'

It will contain a flat menu with all the applications listed in order, able to scroll left and right to select them.
Scrolling up will have the different menus, scrolling down as well...

Although the GUI will be Console-Based, the operating system won't be as restricted.

There will still be a Terminal, a Task manager, and more.

You will start in the 'apps' section, with a simple left to right selection of applications
Scrolling up will bring you to a menu selection (for basic stuff, like 'Settings' or 'Account')
Then scrolling down will bring you to an applications information

# Chapter 2 - Windows NT style codebase

Originally, I created Trunk to be a hybrid between Windows and Linux.
Not only code-base wise, but also feature wise.
I preferred Linux, but slowly, I shifted to liking NT style more.

I did a full switch over to an NT style codebase, and that's now the standard for Trunk

## TABLE OF CHANGES:

| BEFORE                    | AFTER                  | Description                                      |
| :------------------------ | :--------------------- | :----------------------------------------------- |
| `snake_case` functions    | `PascalCase` functions | Function naming convention shifted to PascalCase |
| `snake_case` type aliases | `CAPCASE` type aliases | Type alias naming convention shifted to CAPCASE  |

## NEW TYPE ALIASES:

| BEFORE (Linux) | AFTER (Windows) | Description                 |
| :------------- | :-------------- | :-------------------------- |
| `u8`           | `BYTE`          | 8-bit unsigned integer      |
| `u16`          | `WORD`          | 16-bit unsigned integer     |
| `u32`          | `DWORD`         | 32-bit unsigned integer     |
| `u64`          | `QWORD`         | 64-bit unsigned integer     |
| `i8`           | `CHAR`          | 8-bit signed integer (char) |
| `i16`          | `SHORT`         | 16-bit signed integer       |
| `i32`          | `LONG`          | 32-bit signed integer       |
| `i64`          | `LONGLONG`      | 64-bit signed integer       |
| `bool`         | `BOOL`          | Boolean value               |
| `void*`        | `PVOID`         | Generic pointer             |
| `char*`        | `PCSTR`         | Pointer to constant string  |
| `size_t`       | `SIZE_T`        | Size type                   |

# Chapter 3 - Installation assistant

Most operating systems have an 'Installation assistant'

Once you first boot-up the operating system, It detects you're a new user.

It then brings you to the 'configuration stage'

Here, you can customize the Operating system, and hardware.

This allows users to use the Operating system as they need, choose their devices, etc..

Then, It saves these choices, and reboots.

The installation assistant also handles installing and unpacking the disk

A basic setup looks like this

## Installation steps

| Step | Configuration Option   | Description                                                                                   |
| :--- | :--------------------- | :-------------------------------------------------------------------------------------------- |
| 1    | **Installation Mode**  | Choose to run in **Live Environment** (trial) or install to **Hard Disk** (permanent).        |
| 2    | **Language & License** | Select system language and accept the End User License Agreement (EULA).                      |
| 3    | **Partitioning**       | Define disk layout (e.g., manual partitions, dual-boot, or automatic).                        |
| 4    | **Formatting**         | Format selected partitions with file systems (e.g., `ext4`, `NTFS`, `FAT32`).                 |
| 5    | **Installation Type**  | Choose package set: **Full Install** (GUI + apps), **Minimal**, or **Bare-Bones** (CLI only). |
| 6    | **Input Devices**      | Select **Keyboard** layout and **Mouse** configuration.                                       |
| 7    | **System Identity**    | Set the **Computer Name** (hostname) for network identification.                              |
| 8    | **Time Settings**      | Configure **Timezone** and enable Network Time Protocol (NTP) synchronization.                |
| 9    | **Network**            | Configure **Wi-Fi/Ethernet**, hostname, and proxy settings.                                   |
| 10   | **Appearance**         | Select default **Theme** (Dark/Light) and desktop environment preferences.                    |

After you are done, rebooting will bring you back to the actual Operating system, launched with your configurations!

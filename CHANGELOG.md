## Trunk week 1 report

A lot has happened In the first week, technically.
Tho it's important to note, this was not the first week.
The project was started around 2 weeks ago, but I only started tracking now.
We will track based on a weekly basis, starting monday, ending sunday.
So this is the first 'week'

## WEEK 1

# Build system

Made a powerful recursive-Makefile, high memory kernel build system.
Every sub-dir and root-dir has a makefile, It's a whole chain.
The makefiles collect objects and place them into 'TRUNK_OBJS'
This is finally built together by the root 'Makefile'
Then, It uses scripts and configuration files for the actual running.
Checkout 'src/trunk/mergedcomp/' do see this in work
The scripts are in charge of launching QEMU and building the disk file, using DECLS from the configuration files.
'common.sh' Is the shared library for the scripts, It contains reusable code.
'trunk.ld' is the master linkerscript, setting the kernel virtual address, and boot code address.

# Booting

Trunk uses GRUB(bootloader) with MULTIBOOT2(protocol), there Is not a custom bootloader.
Because GRUB Is older, It does require you to write 'Boot Code' to get It going.
Most of this Is assembly, some C++.

The standard order Is:

GRUB drops us off at 32-bit mode.

entry32.asm set's up, then does a far jump to 64-bit mode.
entry64.asm set's up 64-bit mode.

Our boot code has some basic steps:

Basic paging to map 1GB of RAM (for GRUB and the kernel)
multiboot2-data Is collected and parsed, then stored Into 'BootInfo' structure and later passed to the physical memory manager.
Make's sure to align the stack to 16 before calling C++ global constructors. (Per System V ABI)

The trunk boot code Is grouped under 'trldr'
Even tho It's not a custom bootloader, I still named It.

Finally, the boot process chain:

entry64.asm -> TrSystemStartup(asm) -> Trkload()(C++) -> TrkStartup()(C++)

TrSystemStatup Is an assembly safety landing pad, Instead of entry64 calling Trkload() directly.
It acts as a safety net and make's sure the system Is setup properly.

Trkload() Is the C++ code that loads the main kernel function

It calls the parsing functions for MB2, verifies the MB2 MAGIC, then calls TrkStartup()

TrkStartup() Is the kmain() for Trunk.
It's the main kernel function, basically the start of the real operating system.

# Drivers

We have 2 very basic Drivers setup.

1. PIC - This driver was made for Interrupts, very basic timing mech.
2. UART - This driver was made for console printing, will be reworked very soon.

# Global Descriptor Table

We setup the basic Global Descriptor Table to get the OS moving.
I did start working on the TSS today, but It's not finished.
Tho It's still technically apart of week '1'.

# Memory Management

Same case as TSS, except even simpler.
I only added memblock code, I was going to work on Memory Management but decided to focus on IST and Interrupt Instead.
This will be finished by week 2-5.

# Interrupts

I made a bare-bones Interrupt System.

It maps all error codes (e.g GENERAL*PROTECTION_FAULT) to ISR_ERRCODE*(NUM)
Interrupts are 'trapped' and then 'dispatched' and then 'handled'

This Is the standard order:

trap.asm -> catches Interrupt -> collects-data -> sends off to kinterrupt_dispatcher()
kinterrupt_dispatcher() -> collects Interrupt -> Fires It off to execute_interrupt_handler()
execute_interrupt_handler() -> checks 2 things.

1. If the interrupt Is mapped to a function, If so, calls that function
2. If It is not mapped, It checks If the interrupt is an error, If so, -> kabort()

So our Interrupt system allows you to map a function to a specific interrupt(), even If It's an error.
So for example, let's say you want to map Interrupt 3 (BREAKPOINT) to a Kernel debugger

You can do so! Make the function, make sure It matches the correct structure, then register It via the register_handler() function.
Then, If the Interrupt Is fired, It will call that function Instead of kabort().

You can test this!
Perform:

asm volatile("int $3");

This tells assembly to fire off interrupt 3 (which is BREAKPOINT)

'kabort'?

kabort() Is basically the panic() of Trunk.

I wanted to be original, so I named It kabort()

Ever seen a windows blue screen? Well.. That's a panic() call.
If an error occurs, It Is called

kabort() prints an error message, and completely shuts down the kernel.

ENDING VERSION FOR TRUNK AS OF WEEK 1:
0.9.9

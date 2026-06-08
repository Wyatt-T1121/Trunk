OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

BOOT_BASE = 0x0000000000100000;

SECTIONS
{
    . = BOOT_BASE;

    .multiboot2 : {
        KEEP(*(.multiboot2))
    }

    .boot.text ALIGN(16) : {
        *(.boot.text)
        *(.boot.text.*)
    }

    .boot.rodata ALIGN(16) : {
        *(.boot.rodata)
        *(.boot.rodata.*)
    }

    .text ALIGN(0x1000) : {
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(0x1000) : {
        *(.rodata)
        *(.rodata.*)
    }

    .init_array ALIGN(8) : {
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    }

    .fini_array ALIGN(8) : {
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
    }

    .data ALIGN(0x1000) : {
        *(.data)
        *(.data.*)
    }

    .bss ALIGN(0x1000) : {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end   = .;
    }

    .boot.stack ALIGN(0x1000) : {
        __boot_stack_bottom = .;
        . += 0x4000;
        __boot_stack_top    = .;
    }

    __boot_phys_start = BOOT_BASE;
    __boot_phys_end   = .;

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}
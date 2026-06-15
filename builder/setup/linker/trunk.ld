OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

KERNEL_VMA = 0xFFFFFFFF80000000;
KERNEL_LMA = 0x0000000000100000;

SECTIONS
{
    . = KERNEL_LMA;

    .multiboot2 : {
        KEEP(*(.multiboot2))
    }

    .boot.text : {
        *(.boot.text)
        *(.boot.text.*)
    }

    .boot.rodata : {
        *(.boot.rodata)
        *(.boot.rodata.*)
    }

    . = ALIGN(0x1000);
    BOOT_END_LMA = .;

    . = KERNEL_VMA + BOOT_END_LMA;

    .text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_VMA) {
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - KERNEL_VMA) {
        *(.rodata)
        *(.rodata.*)
    }

    .init_array ALIGN(8) : AT(ADDR(.init_array) - KERNEL_VMA) {
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    }

    .fini_array ALIGN(8) : AT(ADDR(.fini_array) - KERNEL_VMA) {
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_VMA) {
        *(.data)
        *(.data.*)
    }

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_VMA) {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }

    .stack ALIGN(0x1000) : AT(ADDR(.stack) - KERNEL_VMA) {
        __stack_bottom = .;
        . += 0x4000;
        __stack_top = .;
    }

    __kernel_phys_start = KERNEL_LMA;
    __kernel_phys_end   = LOADADDR(.bss) + SIZEOF(.bss);

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}

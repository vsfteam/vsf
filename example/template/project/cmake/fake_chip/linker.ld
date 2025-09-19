/* Linker script for FAKE_DEVICE_UPPER (generic ARM Cortex-M4) */

/* Linker script to configure memory regions. */
MEMORY
{
    FLASH (rx)     : ORIGIN = 0x08000000, LENGTH = 512K
    RAM (rwx)      : ORIGIN = 0x20000000, LENGTH = 128K
}

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
    .text :
    {
        _stext = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .data : AT (_etext)
    {
        __data_start__ = .;
        *(.data*)
        . = ALIGN(4);
        __data_end__ = .;
    } > RAM

    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > RAM

    .heap :
    {
        __HeapBase = .;
        . = . + 0x1000; /* 4KB Heap */
        __HeapLimit = .;
    } > RAM

    .stack :
    {
        __StackLimit = .;
        . = . + 0x800; /* 2KB Stack */
        __StackTop = .;
    } > RAM

    PROVIDE(__stack = __StackTop);
    PROVIDE(_estack = __StackTop);

    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}

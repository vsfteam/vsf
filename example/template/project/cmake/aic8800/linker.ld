/* Linker script for aic8800 */

/* Linker script to configure memory regions. */
MEMORY
{
    FLASH (rwx)     : ORIGIN = 0x08000000, LENGTH = (2048K - 16K)
    EXRAM (rwx)     : ORIGIN = 0x00150000, LENGTH = 64K
    DRAM (rwx)      : ORIGIN = 0x001A0000, LENGTH = (160K - 0x100)
    LPIRAM (rwx)    : ORIGIN = 0x001C7F00, LENGTH = 0x100
    ipc_shared_memory (!rx) : ORIGIN = 0x00190000, len = 2K
    host_rxbuf_memory (!rx) : ORIGIN = 0x00198000, len = 32K
    SHARED_HOST (w)   : ORIGIN = 0x001E5000, LENGTH = 2K
    PRIVATE_HOST (w)  : ORIGIN = 0x001E5800, LENGTH = 2K
    PRIVATE_PATCH (w) : ORIGIN = 0x001E6000, LENGTH = 8K
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
        KEEP(*(.vectors))
        __Vectors_End = .;
        __Vectors_Size = __Vectors_End - __Vectors;
        __end__ = .;

        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
       __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
       __exidx_end = .;
    } > FLASH

    /* To copy multiple ROM to RAM sections,
     * uncomment .copy.table section and,
     * define __STARTUP_COPY_MULTIPLE in startup_ARMCMx.S */
    .copy.table :
    {
        . = ALIGN(4);
        __copy_table_start__ = .;
        LONG (__etext)
        LONG (__data_start__)
        LONG ((__data_end__ - __data_start__) >> 2)
        __copy_table_end__ = .;
    } > FLASH

    /* To clear multiple BSS sections,
     * uncomment .zero.table section and,
     * define __STARTUP_CLEAR_BSS_MULTIPLE in startup_ARMCMx.S */
    .zero.table :
    {
        . = ALIGN(4);
        __zero_table_start__ = .;
        LONG (__bss_start__)
        LONG ((__bss_end__ - __bss_start__) >> 2)
        __zero_table_end__ = .;
    } > FLASH

    __etext_lpiram = .;

    .lptext : AT (__etext_lpiram)
    {
        __lptext_start__ = .;
        *(HOST_LPIRAM)
        . = ALIGN(4);
        __lptext_end__ = .;
    } > LPIRAM

    __etext = __etext_lpiram + (__lptext_end__ - __lptext_start__);

    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array*)))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array*)))
        PROVIDE_HIDDEN (__fini_array_end = .);

        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;

    } > DRAM

    __image_end__ = __etext + SIZEOF(.data);

    .noinit (NOLOAD):
    {
        *(.bss.noinit* .noinit*)
    } > DRAM

    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
    } > DRAM

    .sharedram (NOLOAD):
    {
        *(SHAREDRAM)
    } > EXRAM

    .heap :
    {
        __end__ = .;
        *(.heap*)
        __HeapLimit = .;
    } > DRAM
    PROVIDE(__sbrk_start = ADDR(.heap));
    PROVIDE(__krbs_start = ADDR(.heap) + SIZEOF(.heap));

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy :
    {
        *(.stack)
    } > EXRAM

    /* Set stack top to end of IRAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(DRAM) + LENGTH(DRAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    /* Check if data + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region DRAM overflowed with stack")

    /* IPC shared RAM */
    IPC_SHARED (NOLOAD):
    {
        *(SHAREDRAMIPC)
    } > ipc_shared_memory

    HOST_RXBUF (NOLOAD):
    {
        _shrbram = . ;
        *(HOST_RXBUF)
        _ehrbram = . ;
    } > host_rxbuf_memory

    .shared_host (NOLOAD):
    {
        _start_shared_host = .;
        KEEP(*(SORT_BY_NAME(SHAREDMEM_HOST_*)))
        _end_shared_host = .;
    } > SHARED_HOST

    .private_host (NOLOAD):
    {
        _start_private_host = .;
        KEEP(*(SORT_BY_NAME(PRIVATEMEM_HOST_*)))
        _end_private_host = .;
    } > PRIVATE_HOST
}

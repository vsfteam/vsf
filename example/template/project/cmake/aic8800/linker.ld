/* Linker script for aic8800 */

/* Linker script to configure memory regions. */
MEMORY
{
    IRAM (rwx)      : ORIGIN = 0x08000000, LENGTH = (2048K - 16K)
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
        __code_start__ = .;
        /* the address 0 must contain the boot vectors */
        KEEP(*(.isr_vector))
        __isr_vector_end__ = .;
        *boot_startup.o(.text)
        *(.text*)

        *(.rodata*)
        __code_end__ = .;
    } > IRAM

    .ARM.exidx :
    {
       __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
       __exidx_end = .;
    } > IRAM

    __etext_lpiram = .;

    .lptext : AT (__etext_lpiram)
    {
        __lptext_start__ = .;
        *(HOST_LPIRAM)
        . = ALIGN(4);
        __lptext_end__ = .;
    } > LPIRAM

    __etext = __etext_lpiram + (__lptext_end__ - __lptext_start__);

    .vectors (ORIGIN(DRAM)) (NOLOAD):
    {
        __vectors_start__ = .;
        . = (__isr_vector_end__ - __code_start__);
        __vectors_end__ = .;
    } > DRAM

    .data : AT (__etext)
    {
        __data_start__ = .;
        Image$$RW_IRAM1$$Base = .;
        *(vtable)
        *(.data*)

        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;

    } > DRAM

    __image_end__ = __etext + SIZEOF(.data);

    .bss (NOLOAD):
    {
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        __bss_end__ = .;
        *(SHAREDRAM)
        Image$$RW_IRAM1$$ZI$$Limit = . ;
    } > DRAM

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
    } > DRAM

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

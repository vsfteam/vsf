/*
 * ref: https://github.com/nminaylov/F1C100s_projects/blob/master/f1c100s_dram.ld
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

STACK_UND_SIZE = 0x100;
STACK_ABT_SIZE = 0x100;
STACK_IRQ_SIZE = 0x100;
STACK_FIQ_SIZE = 0x100;
STACK_SVC_SIZE = 0x400;
HEAP_SIZE = 0x00100;

MEMORY
{
	sram : org = 0x00000000, len = 32K
}

SECTIONS
{
	.text :
	{
		PROVIDE(__image_start = .);
		PROVIDE(__text_start = .);
		build/start.o (.text)
		*(.text*)
		PROVIDE(__text_end = .);
	} > sram

	.rodata ALIGN(8) :
	{
		PROVIDE(__rodata_start = .);
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		PROVIDE(__rodata_end = .);
	} > sram

	.data ALIGN(8) :
	{
		PROVIDE(__data_start = .);
		*(.data*)
		. = ALIGN(8);
  		PROVIDE(__data_end = .);
  		PROVIDE(__image_end = __data_end);
	} > sram

	.ARM.exidx ALIGN(8) :
	{
		PROVIDE (__exidx_start = .);
		*(.ARM.exidx*)
		PROVIDE (__exidx_end = .);
	} > sram

	.ARM.extab ALIGN(8) :
	{
		PROVIDE (__extab_start = .);
		*(.ARM.extab*)
		PROVIDE (__extab_end = .);
	} > sram

	.bss ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__bss_start = .);
		*(.bss*)
		*(.sbss*)
		*(COMMON)
		. = ALIGN(8);
		PROVIDE(__bss_end = .);
	} > sram

	.heap ALIGN(8) (NOLOAD) :
    {
        PROVIDE(__heap_start = .);
        . = . + HEAP_SIZE;
        . = ALIGN(8);
        PROVIDE(__heap_end = .);
    } > sram

	.stack ALIGN(8) (NOLOAD) :
	{
		PROVIDE(__stack_start = .);
		PROVIDE(__stack_und_start = .);
		. += STACK_UND_SIZE;
		PROVIDE(__stack_und_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_abt_start = .);
		. += STACK_ABT_SIZE;
		PROVIDE(__stack_abt_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_irq_start = .);
		. += STACK_IRQ_SIZE;
		PROVIDE(__stack_irq_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_fiq_start = .);
		. += STACK_FIQ_SIZE;
		PROVIDE(__stack_fiq_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_svc_start = .);
		. += STACK_SVC_SIZE;
		PROVIDE(__stack_svc_end = .);
		. = ALIGN(8);
		PROVIDE(__stack_end = .);
	} > sram
}

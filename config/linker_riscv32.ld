OUTPUT_ARCH( "riscv" )
ENTRY( _start )
/* Define the memory regions */
MEMORY
{
	/* Fake ROM area */
	rom (rxa) : ORIGIN = 0x80000000, LENGTH = 512K
	ram (wxa) : ORIGIN = 0x80080000, LENGTH = 1024K
}

/* Define the entry point */
/* Define the stack size */
__stack_size = 0x1000; /* 4KB Stack size (adjust as needed) */
__guard_size = 0x1000;

/* Specify the sections */
SECTIONS
{
    .init :
	{
		_text = .;
		KEEP (*(SORT_NONE(.init)))
        KEEP(*(.vectors))
	} >rom AT>rom

    .text :
    {
        *(.text.startup .text.startup.*)
		*(.text .text.*)
    } > rom

	.fini :
	{
		KEEP (*(SORT_NONE(.fini)))
		_etext = .;
	} >rom AT>rom

    .rodata.align ALIGN(4):
	{
		_rodata = .;
	} >rom AT>rom

    .rodata.start :
	{
		_rodata_lma = LOADADDR(.rodata.start);
	} >rom AT>rom

	.rodata :
	{
		*(.rodata .rodata.*)
		. = ALIGN(8);
		*(.srodata.cst16)
		*(.srodata.cst8)
		*(.srodata.cst4)
		*(.srodata.cst2)
		*(.srodata .srodata.*)
		. = ALIGN(4);
		_erodata = .;
	} >rom AT>rom

	.data.align :
	{
		. = ALIGN(4);
		_data = .;
	} >ram AT>rom

	.data.start :
	{
		_data_lma = LOADADDR(.data.start);
	} >ram AT>rom

	.data :
	{
		*(.data .data.*)
		. = ALIGN(8);
		PROVIDE( __global_pointer$ = . + 0x800 );
		*(.sdata .sdata.*)
		*(.sdata2 .sdata2.*)
		. = ALIGN(4);
		_edata = .;
	} >ram AT>rom

	.bss.align :
	{
		. = ALIGN(4);
		_bss = .;
	} >ram

	.bss (NOLOAD) :
	{
		*(.sbss*)
		*(.bss .bss.*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} >ram

	. = ALIGN(8);

    /* End of BSS */
    _end = .;

    /* Stack at top of RAM */
    _stack_top = ORIGIN(ram) + LENGTH(ram);   /* highest address */
    _estack    = _stack_top;

    /* Stack bottom (start of stack region) */
    _sstack = _stack_top - __stack_size;

    /* Guard region below stack */
    _guard_start = _sstack - __guard_size;
    _guard_end   = _sstack;

    /* Heap between end of .bss and start of guard */
    _heap  = _end;
    _eheap = _guard_start;



    PROVIDE(end = .);
}



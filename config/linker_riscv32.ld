OUTPUT_ARCH( "riscv" )
ENTRY( _start )

/* Convenient size constants */
__1KB__   = 0x00000400;
__4KB__   = 0x00001000;
__16KB__  = 0x00004000;
__64KB__  = 0x00010000;
__1MB__   = 0x00100000;
__2MB__   = 0x00200000;
__4MB__   = 0x00400000;
__16MB__  = 0x01000000;
__128MB__ = 0x08000000;
__256MB__ = 0x10000000;
__1GB__   = 0x40000000;


/* Define the memory regions */
MEMORY
{
	/* Fake ROM area */
	ROM (rx) : ORIGIN = 0x80000000, LENGTH = 16M
	RAM (wxa) : ORIGIN = 0x81000000, LENGTH = 64M
}

/* Define the entry point */
/* Define the stack size */


__heap_size = 4M;
__stack_size = __16KB__;
__guard_size = __4KB__;


SECTIONS
{
	. = ORIGIN(RAM);
    _ram_start = .;
	
    /* Reset PC */
    . = ORIGIN(ROM);

    .init :
    {
        _text = .;
        KEEP (*(SORT_NONE(.init)))
        KEEP(*(.vectors))
    } >ROM AT>ROM

    .text :
    {
        *(.text.startup .text.startup.*)
        *(.text .text.*)
    } >ROM

    .fini :
    {
        KEEP (*(SORT_NONE(.fini)))
        _etext = .;
    } >ROM AT>ROM

    .rodata.align :
    {
        . = ALIGN(4);
        _rodata = .;
    } >ROM AT>ROM

    .rodata.start :
    {
        _rodata_lma = LOADADDR(.rodata.start);
    } >ROM AT>ROM

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
    } >ROM AT>ROM

    .data.align :
    {
        . = ALIGN(4);
        _data = .;
    } >RAM AT>ROM

    .data.start :
    {
        _data_lma = LOADADDR(.data.start);
    } >RAM AT>ROM

    .data :
    {
        *(.data .data.*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata .sdata.*)
        *(.sdata2 .sdata2.*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT>ROM

    .bss.align :
    {
        . = ALIGN(4);
        _bss = .;
    } >RAM

    .bss (NOLOAD) :
    {
        *(.sbss*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    _end = .;

	.heap (NOLOAD) :
	{
		. = ALIGN(4);
		_heap = .;

		/* Absolute max heap end */
		_heap_max = ORIGIN(RAM) + LENGTH(RAM) - __stack_size - __guard_size;

		/* Final heap end:
		- If __heap_size is defined → min(_heap + __heap_size, _heap_max)
		- If not defined → _heap_max
		*/
		. = (DEFINED(__heap_size)
			? MIN(_heap + __heap_size, _heap_max)
			: _heap_max);

		_eheap = .;
	} >RAM

    .guard (NOLOAD) :
    {
		. = ALIGN(4);
		_guard_start = .;
        . += __guard_size;
        _guard_end = .;
    } >RAM

    .stack (NOLOAD) :
    {
		. = ALIGN(16);
		_sstack = .;
        . += __stack_size;
        _stack_top = .;
        _estack = _stack_top;
    } >RAM

    PROVIDE(end = .);

	/DISCARD/ :
	{
		*(.comment)
		*(.note*)
	}

	/* --- ASSERTIONS --- */
	/* 1. Your check: Verifies the MEMORY map definitions themselves don't overlap */
	ASSERT( (ORIGIN(ROM) + LENGTH(ROM)) <= ORIGIN(RAM), "Error: ROM and RAM region definition overlap!" )

	/* 2. Content check: Verifies the actual data you're loading into ROM doesn't bleed into RAM */
	/* This is useful if you ever change your ROM/RAM origins to be closer together */
	ASSERT( LOADADDR(.data) + SIZEOF(.data) <= ORIGIN(RAM), "Error: ROM content (LMA) overlaps into RAM address space!")

	/* 3. Ensure the ROM (Fake ROM) has not overflowed */
	ASSERT(_erodata <= (ORIGIN(ROM) + LENGTH(ROM)), "Error: ROM region overflowed!")

	/* 4. Ensure the RAM has not overflowed (Data + BSS + Heap + Guard + Stack) */
	ASSERT(_estack <= (ORIGIN(RAM) + LENGTH(RAM)), "Error: RAM region overflowed!")

	/* 5. Ensure Heap actually fits (especially if you manually defined __heap_size) */
	ASSERT(_eheap >= _heap, "Error: Heap size is negative or invalid calculation!")

	/* 6. Ensure Stack is aligned for RISC-V (typically 16-byte alignment is safest for ABI) */
	ASSERT(_stack_top % 16 == 0, "Error: Stack top is not 16-byte aligned!")

}

/* C runtime symbols */
/* C runtime symbols */
PROVIDE(__data_start__ = _data);
PROVIDE(__data_end__   = _edata);

PROVIDE(__bss_start__  = _bss);
PROVIDE(__bss_end__    = _ebss);

/* Heap boundaries */
PROVIDE(__heap_start__ = _heap);
PROVIDE(__heap_end__   = _eheap);

/* Stack boundaries */
PROVIDE(__stack_start__ = _sstack);
PROVIDE(__stack_end__   = _stack_top);

/* Alias used by startup code for initial SP */
PROVIDE(__stack = _stack_top);

MEMORY
{
	FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 1M
	SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
	.mytext :
	{
		KEEP(*(.isr_vectors))
		*(.text)
        _mydata_lma_start = .;
	} > FLASH

	.mydata : AT(_mydata_lma_start)
	{
		*(.data)
	} > SRAM

}
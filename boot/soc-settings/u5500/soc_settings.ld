ENTRY(soc_settings_init)
SECTIONS
{
	. = 0;
	.start : { *(.text.soc_settings_init) }
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}


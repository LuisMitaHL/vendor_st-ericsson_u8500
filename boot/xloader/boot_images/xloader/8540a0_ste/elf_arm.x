/*
 * -----------------------------------------------------------------
 *
 *  GCC Link definitions for L8540
 *
 * -----------------------------------------------------------------
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH("arm")

.XLOADER_COPY_ADDRESS = 0x400427A8;
.XLOADER_STACK_ADDRESS = 0x40097000;

MEMORY
{
    XLOADER : ORIGIN = 0x400427A8, LENGTH = 0x40097000 - 0x400427A8
    STACK   : ORIGIN = 0x40097000, LENGTH = 0x2000
}

SECTIONS
{
    .pch : AT(0)
    {
        .pre_xloader_start = . ;

        KEEP(*(.xloader_header))

        . = ALIGN(4);

        .pre_xloader_end = . ;
    } > XLOADER

   .xloader :
    {
        .xloader_start = . ;

        .xloader_text_start = . ;

        KEEP(*(.startup))

        KEEP(*(.xloader_init_func))
        KEEP(*(.text .text.*))

        /* gcc stuff */
        *(.gnu.linkonce.t.*)
        *(.glue_7)
        *(.glue_7t)
        *(.gcc_except_table)
        *(.gnu.linkonce.r.*)

        KEEP(*(.rodata .rodata.*))

        KEEP(*(.param))

        . = ALIGN(4);

        .xloader_text_end = . ;

    } > XLOADER

    .xloader_data :
    {
        .xloader_data_start = . ;

        KEEP(*(.data .data.*))

        . = ALIGN(4);

        .xloader_data_end = . ;

    } > XLOADER

    .xloader_bss :
    {
        PROVIDE(linker_xloader_bss_start = . );


        KEEP(*(.bss .bss.*))
        KEEP(*(COMMON))

        . = ALIGN(4);

        PROVIDE(linker_xloader_bss_end = . );
    } > XLOADER = 0

    .xloader_heap :
    {
        PROVIDE (linker_xloader_heap_bottom = .);
        . += 0x8000;
        PROVIDE (linker_xloader_heap_top = .);

    } > XLOADER

    .xloader_stack :
    {
        PROVIDE (linker_xloader_stack_bottom = .);
        . += 0x4; /* TODO fix the stack */
        PROVIDE (linker_xloader_stack_top = .);
    } > STACK = 0


/*
  PROVIDE (linker_xloader_base = .xloader_start);
*/
  PROVIDE (linker_xloader_bss_size = (linker_xloader_bss_end - linker_xloader_bss_start) );
/*
  PROVIDE (linker_xloader_data_size = (.xloader_data_end - .xloader_data_start) );
  PROVIDE (linker_issw_text_size = (.xloader_text_end - .xloader_text_start) );
*/

}

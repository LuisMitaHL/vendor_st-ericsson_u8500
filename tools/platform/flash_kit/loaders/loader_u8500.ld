OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm",
	      "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(vectors)

SECTIONS
{
  .init :
  {
    *vectors.o(.vectors)
    . = ALIGN(4);
    *handlers.o(.handlers*)
    . = ALIGN(4);
    *init.o(.init)
  } =0
  . = ALIGN(4);
  .text :
  {
    . = ALIGN(4);
    *(.text .stub .text.* .gnu.linkonce.t.*)
    . = ALIGN(4);
    *(.glue_7t) *(.glue_7) *(.vfp11_veneer) *(.v4_bx)
  } =0
  . = ALIGN(4);
  .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
  . = ALIGN(4);
  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) }
  . = ALIGN(4);
  __exidx_start = .;
  .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
  __exidx_end = .;
  . = ALIGN(4);
  .data :
  {
    *(.data .data.* .gnu.linkonce.d.*)
  }
  . = ALIGN(4);
  .version :
  {
    KEEP(*(versionstring))
  }
  . = ALIGN(4);
  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    __bss_start = .;
    *(.dynbss)
    . = ALIGN(4);
    *(.bss .bss.* .gnu.linkonce.b.*)
    . = ALIGN(4);
    *(COMMON)
    . = ALIGN(4);
    __bss_end = .;
  }
  . = ALIGN(0x100000);
  .staticbuffers (NOLOAD):
  {
    __staticbuffers = .;
    PROVIDE (Image$$STATIC_BUFFERS$$Base = .);
    . = __staticbuffers + 0x01100000;
  }
  _end = .;
  . = ALIGN(4);
  _heap_start_addr = .;
  . = _heap_start_addr + 0x2000000;
  _heap_stop_addr = .;

/* stack is put at end of sram */
  . = 0x4009FFFC;
  PROVIDE ( __stack = .);
}

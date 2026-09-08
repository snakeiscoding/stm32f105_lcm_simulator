/* Linker script for STM32F105VC
 * Flash: 256K @ 0x08000000
 * SRAM : 64K  @ 0x20000000
 */

ENTRY(Reset_Handler)

/* Memories definition */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x0800D000, LENGTH = 208K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

/* End of RAM for stack */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Minimal heap and stack sizes */
_Min_Heap_Size  = 0x200;   /* 512 bytes */
_Min_Stack_Size = 0x400;   /* 1 KB */

/* Sections */
SECTIONS
{
  /* Interrupt vector table */
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  /* Program code and read-only data */
  .text :
  {
    *(.text*)
    *(.rodata*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP(*(.init))
    KEEP(*(.fini))
  } > FLASH
  
  .userinfo 0x0803FE00 :
  {
    KEEP(*(.userinfo))
  } > FLASH

  /* Exception handling */
  .ARM.extab :
  {
    *(.ARM.extab*)
  } > FLASH

  .ARM.exidx :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > FLASH

  /* Copy initialized data from FLASH to RAM */
  _sidata = LOADADDR(.data);

  .data :
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM AT > FLASH

  /* Zero-initialized data */
  .bss :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

  /* Heap and Stack */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } > RAM

  /* Remove unused sections */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a  ( * )
    libgcc.a ( * )
  }
}
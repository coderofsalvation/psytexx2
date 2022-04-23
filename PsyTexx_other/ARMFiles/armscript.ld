OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_Z15ARM_PalmOS_mainPKvPvPFmS0_mS0_mE)
SEARCH_DIR(/usr/arm-elf/lib);
SECTIONS
{
.text 0x00000000 : { *(arm_startup) *(text) *(.gnu*) *(sec1) *(sec2) *(sec3) *(sec4) }
.alldata : { *(.bss) *(.data) *(.rodata) *(.rodata.str1.4) }
.rel : { *(.data.rel.local) *(.data.rel) }
.got : { *(.got) *(.got.plt) }
}


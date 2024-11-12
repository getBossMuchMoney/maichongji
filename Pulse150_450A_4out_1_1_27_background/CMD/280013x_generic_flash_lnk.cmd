/* Define the memory block start/length for the F28
   PAGE 0 will be used to organize program sections
   PAGE 1 will be used to organize data sections

*/

MEMORY
{
PAGE 0:    /* Program Memory */
   BEGIN            : origin = 0x00088000, length = 0x00000002
   BOOT_RSVD        : origin = 0x00000002, length = 0x00000126

   //RAMLS0           : origin = 0x00008000, length = 0x00002000
   // RAMLS1_RSVD      : origin = 0x0000BFF8, length = 0x00000008 /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */

   RESET            : origin = 0x003FFFC0, length = 0x00000002
   VECTORS          : origin = 0x003FFFC2, length = 0x00003E     /* part of boot ROM  */

   /* Flash sectors */
   FLASH_BANK       : origin = 0x088002, length = 0x17F00  /* on-chip Flash */

 //  FLASH_BANK0_SEC_0_7     : origin = 0x080002, length = 0x1FFE  /* on-chip Flash */
  // FLASH_BANK0_SEC_8_15    : origin = 0x082000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_16_23   : origin = 0x084000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_24_31   : origin = 0x086000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_32_39   : origin = 0x088000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_40_47   : origin = 0x08A000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_48_55   : origin = 0x08C000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_56_63   : origin = 0x08E000, length = 0x2000  /* on-chip Flash */
 //  FLASH_BANK0_SEC_64_71   : origin = 0x090000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_72_79   : origin = 0x092000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_80_87   : origin = 0x094000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_88_95   : origin = 0x096000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_96_103  : origin = 0x098000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_104_111 : origin = 0x09A000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_112_119 : origin = 0x09C000, length = 0x2000  /* on-chip Flash */
  // FLASH_BANK0_SEC_120_127 : origin = 0x09E000, length = 0x1FF0  /* on-chip Flash */

PAGE 1 :   /* Data Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE0 for program allocation */
           /* Registers remain on PAGE1                                                  */
   RAMM0            : origin = 0x00000128, length = 0x000002D8
   RAMM1            : origin = 0x00000400, length = 0x000003F8
   // RAMM1_RSVD       : origin = 0x000007F8, length = 0x00000008 /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */

    //RAMLS0       	: origin = 0x00008000, length = 0x00002000
    RAMLS01         : origin = 0x00008000, length = 0x00003FF8
    //RAMLS1_RSVD   : origin = 0x0000BFF8, length = 0x00000008 /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
}

SECTIONS
{
   codestart        : > BEGIN   PAGE = 0

   .text            : >FLASH_BANK, PAGE = 0 ALIGN(8)

   .cinit           : > FLASH_BANK, ALIGN(8)
   .switch          : > FLASH_BANK, ALIGN(8)

   .reset           : > RESET,  TYPE = DSECT /* not used, */

   .stack           : > RAMM0

#if defined(__TI_EABI__)
   .bss             : > RAMLS01
   .bss:output      : > RAMLS01
   .init_array      : > FLASH_BANK, ALIGN(8)
   .const           : > FLASH_BANK, ALIGN(8)
   .data            : > RAMLS01
   .sysmem          : > RAMLS01
  .bss:cio          : > RAMLS01
#else
   .pinit           : > FLASH_BANK, ALIGN(8)
   .ebss            : > RAMLS01
   .econst          : > FLASH_BANK, ALIGN(8)
   .esysmem         : > RAMLS01
   .cio             : > RAMLS01
#endif

#if defined(__TI_EABI__)
   .TI.ramfunc      : LOAD = FLASH_BANK,
                      RUN = RAMLS01,
                      LOAD_START(RamfuncsLoadStart),
                      LOAD_SIZE(RamfuncsLoadSize),
                      LOAD_END(RamfuncsLoadEnd),
                      RUN_START(RamfuncsRunStart),
                      RUN_SIZE(RamfuncsRunSize),
                      RUN_END(RamfuncsRunEnd),
                      ALIGN(8)
#else
   .TI.ramfunc      : LOAD = FLASH_BANK,
                      RUN = RAMLS01,
                      LOAD_START(_RamfuncsLoadStart),
                      LOAD_SIZE(_RamfuncsLoadSize),
                      LOAD_END(_RamfuncsLoadEnd),
                      RUN_START(_RamfuncsRunStart),
                      RUN_SIZE(_RamfuncsRunSize),
                      RUN_END(_RamfuncsRunEnd),
                      ALIGN(8)
#endif

    /*  Allocate IQ math areas: */
   IQmath           : > FLASH_BANK, ALIGN(8)
   IQmathTables     : > FLASH_BANK, ALIGN(8)

      .reset              : > RESET,      PAGE = 0, TYPE = DSECT
   vectors             : > VECTORS     PAGE = 0, TYPE = DSECT
}

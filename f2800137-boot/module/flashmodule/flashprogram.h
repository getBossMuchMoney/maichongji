#ifndef __FLASH_PROGRAM_H__
#define __FLASH_PROGRAM_H__

#ifdef  FLASH_PROGRAM_GLOBALS
#define FLASH_PROGRAM_EXT
#else
#define FLASH_PROGRAM_EXT extern
#endif

FLASH_PROGRAM_EXT int flashProgram(uint32_t flashaddr, uint16_t *buf, uint32_t len);

#endif

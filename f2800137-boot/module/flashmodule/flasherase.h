#ifndef __FLASH_ERASE_H__
#define __FLASH_ERASE_H__

#ifdef  FLASH_ERASE_GLOBALS
#define FLASH_ERASE_EXT
#else
#define FLASH_ERASE_EXT extern
#endif

#include <stdint.h>

FLASH_ERASE_EXT int flashEraseSector(uint32_t sectorstartaddr, uint32_t length);

#endif

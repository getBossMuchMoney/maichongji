#ifndef __FLASH_INIT_H__
#define __FLASH_INIT_H__

#ifdef  FLASH_INIT_GLOBALS
#define FLASH_INIT_EXT
#else
#define FLASH_INIT_EXT extern
#endif

FLASH_INIT_EXT int flashInit();

#endif

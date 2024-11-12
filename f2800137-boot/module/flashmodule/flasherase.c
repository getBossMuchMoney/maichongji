#define FLASH_ERASE_GLOBALS

#include <stdint.h>
#include "driverlib.h"
#include "device.h"
#include "FlashTech_F280013x_C28x.h"
#include "flashinit.h"
#include "flashcommon.h"
#include "flasherase.h"

//前32个扇区为boot空间，禁止操作
#pragma CODE_SECTION(flashEraseSector, ".TI.ramfunc");
int flashEraseSector(uint32_t sectorstartaddr, uint32_t length)
{
    Fapi_StatusType  oReturnCheck;
    Fapi_FlashStatusType  oFlashStatus;
    Fapi_FlashStatusWordType  oFlashStatusWord;
    uint32_t u32CurrentAddress = 0;
    uint32_t sectorstart,sectorend,mask2=0,i;

    if((sectorstartaddr<Bzero_Sector32_start)    //前32扇区为boot空间，禁止操作
    || (sectorstartaddr+length > FlashBank0EndAddress))
            return -1;
    sectorstart = (sectorstartaddr - Bzero_Sector0_start)/Sector2KB_u16length;
    sectorend = (sectorstartaddr+length-1-Bzero_Sector0_start)/Sector2KB_u16length;

    for(i=sectorstart; i<=sectorend; i=i+8)
    {
        mask2 |= 1<<((i-32)/8);
    }
    mask2 = (~mask2)&0xfff;

    ClearFSMStatus();

    // Enable program/erase protection for select sectors where this example is
    // located
    // CMDWEPROTA is applicable for sectors 0-31
    // Bits 0-11 of CMDWEPROTB is applicable for sectors 32-127, each bit represents
    // a group of 8 sectors, e.g bit 0 represents sectors 32-39, bit 1 represents
    // sectors 40-47, etc
    Fapi_setupBankSectorEnable(FLASH_WRAPPER_PROGRAM_BASE+FLASH_O_CMDWEPROTA, 0xFFFFFFFF);
    Fapi_setupBankSectorEnable(FLASH_WRAPPER_PROGRAM_BASE+FLASH_O_CMDWEPROTB, mask2);

    u32CurrentAddress = Bzero_Sector0_start;

    //
    //Issue bank erase command
    //
    oReturnCheck = Fapi_issueBankEraseCommand((uint32 *)u32CurrentAddress);

    //
    // Wait until FSM is done with erase sector operation
    //
    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}

    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for possible errors
        return -2;
    }

    // Read FMSTAT register contents to know the status of FSM after
    // erase command to see if there are any erase operation related errors
    oFlashStatus = Fapi_getFsmStatus();
    if(oFlashStatus != 3)
    {
        // Check Flash API documentation for FMSTAT and debug accordingly
        // Fapi_getFsmStatus() function gives the FMSTAT register contents.
        // Check to see if any of the EV bit, ESUSP bit, CSTAT bit or
        // VOLTSTAT bit is set (Refer to API documentation for more details).
        return -3;
    }



    // Do blank check
    // Blank check is performed on all the sectors that are not protected
    // during Bank erase
    // Verify that Bank 0 is erased.
    // The Erase command itself does a verify as it goes.
    // Hence erase verify by CPU reads (Fapi_doBlankCheck()) is optional.
    u32CurrentAddress = Bzero_Sector32_start;
    oReturnCheck = Fapi_doBlankCheck((uint32 *)u32CurrentAddress,
                   ((sectorend-sectorstart+1)*Sector2KB_u32length),
                   &oFlashStatusWord);

    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for error info
        return -4;
    }

    return 0;
}

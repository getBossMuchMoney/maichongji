#define FLASH_PROGRAM_GLOBALS

#include "driverlib.h"
#include "device.h"
#include "FlashTech_F280013x_C28x.h"
#include "flashinit.h"
#include "flashcommon.h"
#include "flashprogram.h"

#pragma CODE_SECTION(flashProgram, ".TI.ramfunc");
int flashProgram(uint32_t flashaddr, uint16_t *buf, uint32_t len)
{
    uint32 u32Index = 0;
    uint16 i = 0;
    Fapi_StatusType  oReturnCheck;
    Fapi_FlashStatusType  oFlashStatus;
    Fapi_FlashStatusWordType  oFlashStatusWord;
    uint32_t sectorstart,sectorend,mask2=0;

    if(((uint32_t)buf % 16)||(len % 8))    //要求128位对齐
        return -1;
    if(!buf)
        return -2;
    if(flashaddr + len > FlashBank0EndAddress)
        return -3;

    sectorstart = (flashaddr - Bzero_Sector0_start)/Sector2KB_u16length;
    sectorend = (flashaddr+len-1-Bzero_Sector0_start)/Sector2KB_u16length;

    for(i=sectorstart; i<=sectorend; i=i+8)
    {
        mask2 |= 1<<((i-32)/8);
    }
    mask2 = (~mask2)&0xfff;
    //
    // A data buffer of max 8 16-bit words can be supplied to the program
    // function.
    // Each word is programmed until the whole buffer is programmed or a
    // problem is found. However to program a buffer that has more than 8
    // words, program function can be called in a loop to program 8 words for
    // each loop iteration until the whole buffer is programmed.
    //
    // Remember that the main array flash programming must be aligned to
    // 64-bit address boundaries and each 64 bit word may only be programmed
    // once per write/erase cycle.  Meaning the length of the data buffer
    // (3rd parameter for Fapi_issueProgrammingCommand() function) passed
    // to the program function can only be either 4 or 8.
    //
    // Program data in Flash using "AutoEccGeneration" option.
    // When AutoEccGeneration option is used, Flash API calculates ECC for the
    // given 64-bit data and programs it along with the 64-bit main array data.
    // Note that any unprovided data with in a 64-bit data slice
    // will be assumed as 1s for calculating ECC and will be programmed.
    //
    // Note that data buffer (Buffer) is aligned on 64-bit boundary for verify
    // reasons.
    //
    // Monitor ECC address for the sector below while programming with
    // AutoEcc mode.
    //
    // In this example, the number of bytes specified in the flash buffer
    // are programmed in the flash sector below along with auto-generated
    // ECC.
    //



    for(i=0, u32Index = flashaddr;
       (u32Index < (flashaddr + len));
       i+= 8, u32Index+= 8)
    {
        ClearFSMStatus();

        // Enable program/erase protection for select sectors where this example is
        // located
        // CMDWEPROTA is applicable for sectors 0-31
        // Bits 0-11 of CMDWEPROTB is applicable for sectors 32-127, each bit represents
        // a group of 8 sectors, e.g bit 0 represents sectors 32-39, bit 1 represents
        // sectors 40-47, etc
        Fapi_setupBankSectorEnable(FLASH_WRAPPER_PROGRAM_BASE+FLASH_O_CMDWEPROTA, 0xFFFFFFFF);
        Fapi_setupBankSectorEnable(FLASH_WRAPPER_PROGRAM_BASE+FLASH_O_CMDWEPROTB, mask2);


        oReturnCheck = Fapi_issueProgrammingCommand((uint32 *)u32Index,buf+i,
                                               8, 0, 0, Fapi_AutoEccGeneration);

        //
        // Wait until the Flash program operation is over
        //
        while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);

        if(oReturnCheck != Fapi_Status_Success)
        {
            //
            // Check Flash API documentation for possible errors
            //
            return -4;
        }

        //
        // Read FMSTAT register contents to know the status of FSM after
        // program command to see if there are any program operation related
        // errors
        //
        oFlashStatus = Fapi_getFsmStatus();
        if(oFlashStatus != 3)
        {
            //
            //Check FMSTAT and debug accordingly
            //
            return -5;
        }

        //
        // Verify the programmed values.  Check for any ECC errors.
        //
        oReturnCheck = Fapi_doVerify((uint32 *)u32Index,
                                     4, (uint32 *)(uint32)(buf + i),
                                     &oFlashStatusWord);

        if(oReturnCheck != Fapi_Status_Success)
        {
            //
            // Check Flash API documentation for possible errors
            //
            return -6;
        }
    }
    return 0;
}

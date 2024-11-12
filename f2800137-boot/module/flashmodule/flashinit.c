
#define FLASH_INIT_GLOBALS 1

#include "driverlib.h"
#include "device.h"
#include "FlashTech_F280013x_C28x.h"
#include "flashinit.h"
#include "flashcommon.h"

#pragma CODE_SECTION(flashInit, ".TI.ramfunc");
int flashInit()
{
    Fapi_StatusType  oReturnCheck;

    //
    // Initialize the Flash API by providing the Flash register base address
    // and operating frequency(in MHz).
    // This function is required to initialize the Flash API based on System
    // frequency before any other Flash API operation can be performed.
    // This function must also be called whenever System frequency or RWAIT is
    // changed.
    //
    oReturnCheck = Fapi_initializeAPI(FlashTech_CPU0_BASE_ADDRESS,
                                      DEVICE_SYSCLK_FREQ/1000000U);

    if(oReturnCheck != Fapi_Status_Success)
    {
        return -1;
    }

    //
    // Initialize the Flash banks and FMC for erase and program operations.
    // Fapi_setActiveFlashBank() function sets the Flash banks and FMC for
    // further Flash operations to be performed on the banks.
    //
    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);

    if(oReturnCheck != Fapi_Status_Success)
    {
        return -2;
    }

    return 0;
}

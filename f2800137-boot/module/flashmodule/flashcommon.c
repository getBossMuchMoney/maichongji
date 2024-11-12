#include "driverlib.h"
#include "device.h"
#include "FlashTech_F280013x_C28x.h"
#include "flashinit.h"
#include "flashcommon.h"

#pragma CODE_SECTION(ClearFSMStatus, ".TI.ramfunc");
int ClearFSMStatus(void)
{
    Fapi_FlashStatusType  oFlashStatus;
    Fapi_StatusType  oReturnCheck;

    //
    // Wait until FSM is done with the previous flash operation
    //
    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}
        oFlashStatus = Fapi_getFsmStatus();
        if(oFlashStatus != 0)
        {

            /* Clear the Status register */
            oReturnCheck = Fapi_issueAsyncCommand(Fapi_ClearStatus);
            //
            // Wait until status is cleared
            //
            while (Fapi_getFsmStatus() != 0) {}

            if(oReturnCheck != Fapi_Status_Success)
            {
                return -1;
            }
        }
        return 0;
}

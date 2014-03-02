/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

#include "c_system.h"
#include "system_applications_functions.h"
#include "hcl_defs.h"
#include "ab8500_core.h"
#include "memory_mapping.h"
#include "r_time_utilities.h"
#include "stn8500_usb_dma_support.h"


/**
 *  @addtogroup ldr_system_app
 *  @{
 */

#ifdef CFG_ENABLE_APPLICATION_SYSTEM

void Do_System_Reboot(void)
{
    /*
     * give some time to the PC to properly
     * disconnect the USB before restarting
     */

    Sleep(500);

    ShutdownUSB_DMA();
    Do_Timer_TimersStop();

    /* Writing to the lowest bit in APE_S0FTRST should
     * reboot the platform but this requires the PRCMU
     * firmware to be started at this point
     */
    *(volatile uint32 *)(PRCMU_REG_BASE_ADDR + 0x228) |= 0x00000001;
}



void Do_System_Shutdown(void)
{
    t_ab8500_core_error AB8500_Result = AB8500_CORE_ERROR;

    ShutdownUSB_DMA();

    AB8500_Result = AB8500_CORE_TurnOff();

    if (AB8500_CORE_OK != AB8500_Result) {
        /* UART is shut down, no printouts allowed! */
    }
}


#endif //CFG_ENABLE_APPLICATION_SYSTEM
/** @}*/

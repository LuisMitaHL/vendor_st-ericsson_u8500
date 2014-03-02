/***********************************************************************
 * $Copyright ST-Ericsson 2009 $
 **********************************************************************/

#include "c_system.h"
#include "error_codes.h"
#include "ldr_init.h"
#include "services.h"
#include "ab8500_core_services.h"
#include "r_time_utilities.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "chipid.h"
#include "r_time_utilities.h"

#if (STE_PLATFORM_NUM == 8500) && (defined(ENABLE_LOADER_HWI2C_WD) || !defined(DISABLE_HWI2C_WD_DELAY_KICK))
#include "watchdog.h"
#endif

ErrorCode_e Do_Loader_Hardware_Init(void)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    /* Init Interrupt Controller */
    SER_GIC_Init(0);

    /* Init GPIO */
    SER_GPIO_Init(0);

#if STE_PLATFORM_NUM == 8500
    /* Init SSP */
    SER_SSP_Init(INIT_SSP0);
#endif

    /* Init AB8500 Core */
    SER_AB8500_CORE_Init();

#if STE_PLATFORM_NUM == 5500
    {
        uint8 data_out[24];

        /*
         * For eMMC-MCL
         * Enable 2.9V supply
         * Should be removed when PRCMU is updated to enable the 2.9V
         */
        data_out[0] = 0x3E;

        if (SER_AB8500_CORE_OK != SER_AB8500_CORE_Write(0x03, 0x81, 0x1, data_out)) {
            A_(printf("SER_AB8500_CORE_Write(0x03, 0x81, 0x1, data_out) FAILED!!!.");)
        }
    }
#endif

    /* Initialize RTC hardware */
    SER_RTC_Init(0);

    /*
     * Initialize timers
     */
    ReturnValue = Do_Timer_TimersInit(NULL, MAX_TIMERS);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

#if (STE_PLATFORM_NUM == 8500) && defined(ENABLE_LOADER_HWI2C_WD)
    //enable watchdog
    (void)HWI2C_Enabe_WD(WATCHDOG_TIMEOUT);
#endif

    /* WD can be wake up prior loader execution,
     * so we need to feed the dog in that case
     * If ENABLE_LOADER_HWI2C_WD is enabled
     * WD is kicked in process loop and delayed kick
     * shall be disabled
     */
#if (STE_PLATFORM_NUM == 8500) && !defined(DISABLE_HWI2C_WD_DELAY_KICK)
    //Kick the WD every 5 seconds
    (void)HWI2C_Set_Watchdog_Delayed_Kicker(5000);
#endif

ErrorExit:
    return E_SUCCESS;
}

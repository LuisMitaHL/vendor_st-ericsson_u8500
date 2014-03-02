/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This file provides some watchdog features
* \author  ST-Ericsson
*/
/*****************************************************************************/


/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "ab8500_core.h"
#include "ab8500_core_p.h"
#include "stdlib.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_time_utilities.h"
#include "r_memory_utils.h"
#include "watchdog.h"

#include "services.h"
#include "ab8500_core_services.h"

#define write32(addr, value)    (*(volatile uint32 *)(addr) = (uint32)(value))
#define read32(addr)        (*(volatile uint32 *)(addr))
/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static t_ser_ab8500_core_error HWI2C_WriteBits(uint8 bank_addr, uint8 register_offset, uint8 value, uint8 mask);
C_(static void Read_WD_Status(void);)

/*******************************************************************************/

#define MAXWAIT_SHORT   100 /* Max loops for a short wait */
#define MAXWAIT_LONG    20000   /* Max loops for a long wait */

/*
 * Function to loop and check bit status
 *
 * @param [in] addr    Adress to read
 * @param [in] mask    Bit mask
 * @param [in] status  Expected status
 *
 * @return 1 If expected status OK
 * @return 0 If expected status NOK
 */
uint8 wait_status(uint32 addr, uint32 mask, uint32 status)
{
    volatile unsigned i = 0;

    while (((read32(addr) & mask) != status) && (i < MAXWAIT_LONG)) {
        i++; /* check status once again */
    }

    if (i == MAXWAIT_LONG) {
        return 1;
    }

    return 0;
}

static t_ser_ab8500_core_error HWI2C_WriteBits(uint8 bank_addr, uint8 register_offset, uint8 value, uint8 mask)
{
    t_ser_ab8500_core_error exit_status = SER_AB8500_CORE_HWI2C_OPERATION_FAILED;
    uint8 temp_reg = 0;
    uint8 dummy_data = 0xFF;

    if (SER_AB8500_CORE_OK != (exit_status = SER_AB8500_CORE_Read(bank_addr, register_offset, ONE_BYTE, &dummy_data, &temp_reg))) {
        A_(printf("watchdog.c (%d): Could not Read, reason:%d\n", __LINE__, exit_status);)
        return exit_status;
    }

    temp_reg = (uint8)((temp_reg & (~mask)) | (value & mask));

    if (SER_AB8500_CORE_OK != (exit_status = SER_AB8500_CORE_Write(bank_addr, register_offset, ONE_BYTE, &temp_reg))) {
        A_(printf("watchdog.c (%d): Could not Write, reason:%d\n", __LINE__, exit_status);)
        return exit_status;
    }

    C_(printf("watchdog.c (%d): HWI2C_WriteBits finished successfully\n", __LINE__);)
    return (SER_AB8500_CORE_OK);
}

/*
 * Function to loop and check bit status
 *
 * @param [in] secs    Watchdog timeout
 *
 * @return     Execution Status
 */
t_ser_ab8500_core_error HWI2C_Enabe_WD(uint8 secs)
{
    t_ser_ab8500_core_error exit_status = SER_AB8500_CORE_HWI2C_OPERATION_FAILED;

    t_uint8 data_buffer = 0x0;

    C_(printf("watchdog.c (%d): Enable WD with (%d)\n", __LINE__, secs);)

    if (secs > AB8500_CORE_MAIN_WDTMR_MAX_VAL) {           /* Max 127 secs duration */
        data_buffer = AB8500_CORE_MAIN_WDTMR_MAX_VAL;
        B_(printf("watchdog.c (%d): Warning, Input watchdog value=%d exceeded it's maximum value=%d. Watchdog timer set to maximum!\n", __LINE__, secs, AB8500_CORE_MAIN_WDTMR_MAX_VAL);)
    } else {
        data_buffer = secs;
    }

    /* write value 0x01 to the MainWDogTimer */
    if (SER_AB8500_CORE_OK != (exit_status = HWI2C_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_TMR_REG, data_buffer, MASK_ONE_BYTE))) {
        A_(printf("watchdog.c (%d): Could not WriteBits to Main-WD-Timer, reason:%d\n", __LINE__, exit_status);)
        return exit_status;
    }

    /* set WDExpTurnOnValid bit in MainWDogCtrl register */
    if (SER_AB8500_CORE_OK != (exit_status = HWI2C_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_CTRL_REG, AB8500_CORE_WDEXP_TURNONVALID, AB8500_CORE_WDEXP_TURNONVALID))) {
        A_(printf("watchdog.c (%d): Could not WriteBits to Main-WD-Control-Register, reason:%d\n", __LINE__, exit_status);)
        return exit_status;
    }

    /* Enable main watchdog timer */
    /* SET bit[0] to 1 in  MainWDogCtrl register  */
    if (SER_AB8500_CORE_OK != (exit_status = HWI2C_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_CTRL_REG, AB8500_CORE_MAINWDENABLE | AB8500_CORE_MAINWDKICK, AB8500_CORE_MAINWDENABLE | AB8500_CORE_MAINWDKICK))) {
        A_(printf("watchdog.c (%d): Could not WriteBits to Main-WD-Control-Register in order to ENABLE the WD, reason:%d\n", __LINE__, exit_status);)
        return exit_status;
    }

    C_(printf("watchdog.c (%d): Watchdog enabled successfully\n", __LINE__);)
    C_(Read_WD_Status();)
    return (SER_AB8500_CORE_OK);
}

/*
 * Function to reserve timer for periodically kicking the watchdog
 * Reserve new timer.
 *
 * @param [in] DelayTime  Time(milliseconds) to periodically kick the watchdog
 *
 * @return Index of reserved watchdog-kicker.
 * @return Index 0 if no free watchdog-kicker reserved.
 */
uint32 HWI2C_Set_Watchdog_Delayed_Kicker(uint32 DelayTime)
{
    uint32 timer_id;
    uint8 data = 0xFF;
    uint8 control_reg = 0xFF;
    Timer_t *wd_timer_p = NULL;
    wd_timer_p = (Timer_t *)malloc(sizeof(Timer_t));

    if (NULL == wd_timer_p) {
        A_(printf("watchdog.c (%d): Could not allocate memory for WD-timer \n", __LINE__);)
        return 0;
    }

    wd_timer_p->Time = DelayTime;
    wd_timer_p->PeriodicalTime = DelayTime;
    wd_timer_p->HandleFunction_p = (HandleFunction_t) HWI2C_Do_WatchDog_Kick;
    wd_timer_p->Data_p = NULL;
    wd_timer_p->Param_p = NULL;

    SER_AB8500_CORE_Read(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_CTRL_REG, ONE_BYTE, &data, &control_reg);

    if (!(control_reg & AB8500_CORE_MAINWDENABLE)) {
        //Since Watchdog is disabled there is no need for periodically kicking
        B_(printf("watchdog.c (%d): Watchdog currently disabled therefore the Watchdog kicker will be also disabled!. \n", __LINE__);)
        timer_id = 0;
        goto ErrorExit;
    }

    timer_id = Do_Timer_TimerGet(NULL, wd_timer_p);

    if (!timer_id) {
        A_(printf("watchdog.c (%d): Could not allocate WD-timer. No free timer available \n", __LINE__);)
        goto ErrorExit;
    }

    C_(printf("watchdog.c (%d): WD-Timer allocated Successfully\n", __LINE__);)
ErrorExit:
    BUFFER_FREE(wd_timer_p);
    return timer_id;
}

/*
 * WatchDog kick function
 */
void HWI2C_Do_WatchDog_Kick(void)
{
    t_ser_ab8500_core_error exit_status = SER_AB8500_CORE_HWI2C_OPERATION_FAILED;

    uint8 kick_bits = 0;
    kick_bits = AB8500_CORE_MAINWDKICK | AB8500_CORE_MAINWDENABLE | AB8500_CORE_WDEXP_TURNONVALID;

    /* Kick and wake the dog */
    if (SER_AB8500_CORE_OK != (exit_status = SER_AB8500_CORE_Write(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_CTRL_REG, ONE_BYTE, &kick_bits))) {
        A_(printf("watchdog.c (%d): Could not Kick WD, reason:%d\n", __LINE__, exit_status);)
        return;
    }

    C_(printf("watchdog.c (%d): Watchdog kicked! \n", __LINE__);)
}

void HWI2C_Do_WatchDog_Disable(void)
{
    t_ser_ab8500_core_error exit_status = SER_AB8500_CORE_HWI2C_OPERATION_FAILED;

    uint8 disable_bits = 0x0;

    /* Disable WD */
    if (SER_AB8500_CORE_OK != (exit_status = SER_AB8500_CORE_Write(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_CTRL_REG, ONE_BYTE, &disable_bits))) {
        A_(printf("watchdog.c (%d): Could not disable WD, reason:%d\n", __LINE__, exit_status);)
        return;
    }

    C_(printf("watchdog.c (%d): Watchdog disabled! \n", __LINE__);)
    C_(Read_WD_Status();)
}
C_(
    static void Read_WD_Status(void)
{
    uint8 data = 0xFF;
    uint8 control_reg = 0xFF;

    SER_AB8500_CORE_Read(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_CTRL_REG, ONE_BYTE, &data, &control_reg);

    C_(printf("watchdog.c (%d): WD Reg = 0x%x \n", __LINE__, control_reg);)
}
)

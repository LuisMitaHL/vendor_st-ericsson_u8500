/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/*
 * @addtogroup ldr_hw_abstraction
 * @{
 *   @addtogroup comm_devices
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "r_comm_devices.h"
#include "t_communication_abstraction.h"
#include <string.h>
#include <stdlib.h>

#include "hcl_defs.h"
#include "uart_irq.h"
#include "services.h"
#include "uart.h"

static t_uart_baud_rate GetDeviceSpeed(const CommDevId_t Device);

typedef struct DeviceSpeed_s {
    CommDevId_t       CommDevId;
    t_uart_baud_rate  Speed;
} DeviceSpeed_t;

#define NO_OF_COMM_UNITS 4

static DeviceSpeed_t DeviceSpeed[] = {
    {
        DEV_USB,
        /*
         * the value is irrelevant
         * (added to fix coverity checker PW.MIXED_ENUM_TYPE warning )
         *
         */
        UART_BR_LAST_BAUD
    },
    {
        DEV_UART0,
        UART_BR_115200_BAUD
    },
    {
        DEV_UART1,
        UART_BR_115200_BAUD
    },
    {
        DEV_UART2,
#ifdef STE_AP8540
        /* Fix for palladium. It might not work on real HW */
        UART_BR_921600_BAUD
#else
        UART_BR_115200_BAUD
#endif
    }
};

void Do_GetDeviceSettings(CommDevId_t Device, DeviceParam_t *DevParam)
{
    switch (Device) {
    case DEV_USB:
        DevParam->Device   = DEV_USB;
        /*
         * the value of DevParam->Speed is irrelevant
         * (set to  UART_BR_LAST_BAUD only to fix Coverity checker PW.MIXED_ENUM_TYPE warning)
         *
         */
        DevParam->Speed    = UART_BR_LAST_BAUD;
        DevParam->HiSpeed  = TRUE;
        break;

    case DEV_UART1:
        DevParam->Device   = DEV_UART1;
        DevParam->Speed = GetDeviceSpeed(Device);
        DevParam->HiSpeed  = TRUE;
        break;

    case DEV_UART2:
        DevParam->Device   = DEV_UART2;
        DevParam->Speed = GetDeviceSpeed(Device);
        DevParam->HiSpeed  = TRUE;
        break;

    case DEV_UART0:
        /* Fall through */
    default: {
        DevParam->Device = DEV_UART0;
        DevParam->Speed = GetDeviceSpeed(Device);
        DevParam->HiSpeed = TRUE;
    }
    break;
    }
}

static t_uart_baud_rate GetDeviceSpeed(const CommDevId_t Device)
{
    uint32 nIndex = 0;

    for (nIndex = 0; nIndex < NO_OF_COMM_UNITS; ++nIndex) {
        if (DeviceSpeed[nIndex].CommDevId == Device) {
            return DeviceSpeed[nIndex].Speed;
        }
    }

    return UART_BR_LAST_BAUD;
}

/* @} */
/* @} */

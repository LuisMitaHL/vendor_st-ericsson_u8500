/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/

/*
 * @addtogroup ldr_hw_abstraction
 * @{
 *   @addtogroup communication_abstraction
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include <stdlib.h>
#include <string.h>
#include "t_basicdefinitions.h"
#include "error_codes.h"
//#include "r_communication_abstraction.h"
#include "hcl_defs.h"
#include "uart_irq.h"
#include "uart.h"
#include "services.h"
#include "r_UART_abstraction.h"
#include "r_USB_abstraction.h"
#include "t_stn8500_uart_abstraction.h"
#include "t_stn8500_usb_abstraction.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "t_r15_network_layer.h"
#include "r_comm_devices.h"
#include "r_memory_utils.h"

#ifdef CFG_ENABLE_PRINT_SERVER
#include "t_debug_subsystem.h"
#endif

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

#define USB_CHANNEL          0x42535520
/* In ADL boot mode, value for the comm channel provided by x-loader is 0x00*/
#define USB_CHANNEL_ADL      0x00000000
#define UART_CHANNEL         0x54524155

#ifdef CFG_ENABLE_PRINT_SERVER
/**< */
extern uint32 Debug_Output_Channel;
#endif


/*******************************************************************************
 * Declaration of static functions
 ******************************************************************************/


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 * Initialization of currently connected communication device.
 *
 * Used communication devices USB or UART (UART0/UART1/UART2).
 *
 * @retval Pointer to the initialized communication device.
 * @retval NULL if initialization of device fails.
 */
CommunicationDevice_t *Do_CommDeviceInit(void)
{
    CommunicationDevice_t *CommunicationDevice_p = NULL;
    uint32 Communication_Channel = *((uint32 *)0x80151FEC);

    switch (Communication_Channel) {
    case UART_CHANNEL:
#if STE_PLATFORM_NUM == 8500
        CommunicationDevice_p = Do_CommDeviceInitUART(DEV_UART2, FALSE);
#else
        CommunicationDevice_p = Do_CommDeviceInitUART(DEV_UART0, FALSE);
#endif
        break;
    case USB_CHANNEL:
    case USB_CHANNEL_ADL:
        /*In ADL boot mode only USB comm channel is supported*/
        CommunicationDevice_p = Do_CommDeviceInitUSB();
        break;
    default:
        return CommunicationDevice_p;
    }

    return CommunicationDevice_p;
}

/*
 * Initialization of Debug communication device.
 *
 * Used communication devices UART (specified with UART_DEBUG_PORT).
 *
 * @param [in,out] Communication_p Communication module context.
 *
 * @return none.
 */

#ifdef ENABLE_DEBUG
void Do_CommDeviceDebugoutInit(Communication_t *const Communication_p)
{
    CommunicationDevice_t *CommunicationDevice_p = NULL;
    uint32 Communication_Channel = *((uint32 *)0x80151FEC);

    if (Communication_Channel != UART_CHANNEL) {
        Debug_Output_Channel = OUTPUT_CHANNEL_UART0;
        CommunicationDevice_p = Do_CommDeviceInitUART(UART_DEBUG_PORT, TRUE);
        Communication_p->CommunicationDevice_p = CommunicationDevice_p;

        if (NULL == CommunicationDevice_p) {
            Debug_Output_Channel = OUTPUT_CHANNEL_DEBUG_BUFFER;
        }
    } else {
        Debug_Output_Channel = OUTPUT_CHANNEL_DEBUG_BUFFER;
    }
}
#endif

/*
 * Shutdown of currently used communication device.
 *
 * @param [in,out] CommunicationDevice_p  Pointer to a communication device.
 *
 * @retval E_SUCCESS If is successfully executed.
 */
ErrorCode_e Do_CommDeviceShutDown(const CommunicationDevice_t *CommunicationDevice_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommDev_t *Device_p = ((CommDev_t *)CommunicationDevice_p->Context_p);

    Device_p->Descriptor_p->ShutDown_Fn_p(Device_p, (void *)CommunicationDevice_p);
    ReturnValue = E_SUCCESS;

    return ReturnValue;
}

/* @} */
/* @} */

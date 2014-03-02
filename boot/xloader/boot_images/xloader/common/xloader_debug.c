/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
 * \file xloader_debug.c
 * \author Nicolas Mareau ST-Ericsson
 *
 * This module is the driver for ROM code's debug traces
 *
 *---------------------------------------------------------------------------*/
/** \internal
 * \addtogroup DEBUG
 * @{
 */

/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/

#include "xloader.h"
#include "xl_drivers.h"

#include "boot_types.h"
#include "security.h"
#include "boot_types.h"
#include "boot_api.h"
#include "boot_error.h"
#include "uart.h"
#include "xloader_debug.h"
#include "public_rom_api.h"
#include "public_code_header.h"
#include "nomadik_mapping.h"
#include "nomadik_registers.h"
#include "xloader_pl011.h"

/*---------------------------------------------------------------------------*/
/*                 XLDBG_SendTrace()                                         */
/*---------------------------------------------------------------------------*/
/** \internal
 *
 * This routine sends on UARTx or USB debug message if the debug bit of
 * Boot Indication is set and if uart number is valid.
 *
 * \param[in] p_loaderInfo   Pointe on Loader info
 * \param[in] pdata          Pointer on data to send.
 * \param[in] size           Number of bytes to be transmitted.
 *
 * \return None
 *
 */
/*---------------------------------------------------------------------------*/

void XLDBG_SendTrace(t_pub_rom_loader_info *p_loaderInfo,
             t_address pdata, t_size size)
{
    t_boot_indication indication;
    t_secure_rom_boot_info bootInfo;
    t_boot_trace_info *p_bootTrace;
    void (**jmpTable) (void);   /* ROM API table */
    volatile t_uart_registers *pUartRegs;
	t_uart_number uartNumber;

    jmpTable = (void (**)(void))p_loaderInfo->bootRomJmpTbl;
    indication.scenario = p_loaderInfo->bootIndication;
    bootInfo.BootConf = p_loaderInfo->bootInfo;
    p_bootTrace = (t_boot_trace_info *) p_loaderInfo->pdebugInfo;


#if (XLDBG_PRINTF)
    XLPRINTF("XLDBG_SendTrace: %d\n", *(t_uint32 *)pdata);
#endif
    if (indication.bit.TraceEnable == 0)
        return;     /* Debug Traces not enable */

    switch (p_bootTrace->Bit.DbgBootIf) {

    case IF_USB:
		if (
#if defined(MACH_L8540)
			(bootInfo.Bit.UsbDisable == 0)
#else
			(bootInfo.Bit.UsbEnable == 1)
#endif
            && (p_bootTrace->Bit.DbgUsbEnumDone == 1)) {
            t_uint32 byteSent;
            volatile t_usb_register *p_usbRegs =
                (t_usb_register *)USBOTG_REG_START_ADDR;

            ROM_SHARED_USB_TRANSMIT_DATA(USB_EP2_NB,
                             pdata, size, jmpTable);
            byteSent = 0;
            /* wait until all bytes are transferred */
            while (byteSent != size) {
                ROM_SHARED_USB_GET_NB_BYTES_SENT(&byteSent,
                                 jmpTable);
                if ((byteSent == size) &&
                    ((p_usbRegs->TXCSR &
                      USB_TXCSR_TRDY) != 0)) {
                    /* All data written to fifo but
                     * not yet sent
                     */
                    byteSent = 0;
                }
            }
        }
        break;      /* case IF_USB */

	case IF_UART:
    default:
#ifdef MACH_L8540
		uartNumber=p_loaderInfo->pIfTypes->p_uartType->UartNb;
#else
		uartNumber=bootInfo.Bit.UartNumber;
#endif
        /* Call ROM API to send Data on UART */
		ROM_SHARED_FUNC_UART_SEND_DATA(uartNumber,
                           pdata,
                           size, jmpTable);

        /* choose suitable UART base adress  */
		switch (uartNumber) {
        case UART1:
            pUartRegs = (t_uart_registers *)UART1_REG_START_ADDR;
            break;

        case UART2_B:
#ifndef MACH_L8540
        case UART2_C:
#endif
            pUartRegs = (t_uart_registers *)UART2_REG_START_ADDR;
            break;

        default:
			/* unexpected value */
            return;
        }       /* end switch uartNumber */

        /* Wait TX Fifo is empty */
        while (!pl011_is_tx_fifo_empty(pUartRegs)) ;

        break;
    }           /* end switch DbgBootIf */

    return;
}

/* End of file xloader_debug.c */
/** @} */

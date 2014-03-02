/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides the debug APIs.
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "hcl_defs.h"
#include "debug.h"
#include "debug_services.h"
#include "services.h"

#define DBG_MAX_LINE_SIZE   100
#define DBG_MAX_INT_LINES   100

/*--------------------------------------------------------------------------*
 * Global Variables															*
 *--------------------------------------------------------------------------*/
#pragma arm section zidata = "ahb_share_memory"
char temp_string[100];
char        debug_message[DBG_MAX_LINE_SIZE];
char        debug_buffer[DBG_MAX_INT_LINES][DBG_MAX_LINE_SIZE];
t_uint32    interrupt_msg_count = 0;
t_uint32    interrupt_flag;
#pragma arm section zidata

/*--------------------------------------------------------------------------*
 * Private Functions														*
 *--------------------------------------------------------------------------*/
__inline void CheckInterruptMode(t_uint32 *flag)
{
  t_uint32 ch = 0;
    __asm
    { 		
        MRS ch, CPSR
        AND ch, ch, 0x80
        STR ch, [flag]
    }
   //coverity[self_assign]
    ch = ch;
}

/*--------------------------------------------------------------------------*
 * Public Functions														    *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:    SER_DEBUG_Init                                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the debug environment               */
/*                                                                          */
/* PARAMETERS: IN timer adress or 0 to set a counter as timeStamp           */
/*                                                                          */
/* RETURN: int                                                              */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_DEBUG_Init(t_uint8 debug_mask)
{
}

/****************************************************************************/
/* NAME:    SER_DEBUG_Close                                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine closes     the debug environment               */
/* the void as parameter, and int as return value are mandatory.            */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC void SER_DEBUG_Close(t_uint8 debug_mask)
{
}

/****************************************************************************/
/* NAME:    logMsg                                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine prints the debug messages , if the messages    */
/* are in interrupt mode they are bufferred until next call.                */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: int                                                              */
/*              															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC int logMsg
(
    unsigned long   debug_id,
    const char      *function_name,
    char            *arg_string,
    char            *arg1,
    unsigned long   arg2,
    unsigned long   arg3,
    unsigned long   arg4,
    unsigned long   arg5,
    unsigned long   arg6,
    unsigned long   arg7,
    long            exit_param
)
{
    t_uint8 index;

    snprintf(temp_string, sizeof(temp_string), "\n");
    strncat(debug_message, temp_string, sizeof(temp_string));

    switch (debug_id)
    {
        case UNKNOWN_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "UNKNOWN_HCL_DBG_ID");
            break;

        case APPLI_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "APPLI_DBG_ID");
            break;

        case GIC_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "GIC_HCL_DBG_ID");
            break;

        case TEST_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "TEST_DBG_ID");
            break;

        case DEBUG_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "DEBUG_HCL_DBG_ID");
            break;

        case UART_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "UART_HCL_DBG_ID");
            break;

        case DMA_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "DMA_HCL_DBG_ID");
            break;

        case RTC_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "RTC_HCL_DBG_ID");
            break;

        case TMR_HCL_DBG_ID:
           snprintf(temp_string,sizeof(temp_string), "%s :", "TMR_HCL_DBG_ID");
            break;

        case I2C_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "I2C_HCL_DBG_ID");
            break;

        case CODEC_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "CODEC_HCL_DBG_ID");
            break;

        case MSP_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "MSP_HCL_DBG_ID");
            break;

        case FSMC_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "FSMC_HCL_DBG_ID");
            break;

        case DMC_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string),"%s :", "DMC_HCL_DBG_ID");
            break;

        case GPIO_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "GPIO_HCL_DBG_ID");
            break;

        case PCRMU_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "PCRMU_HCL_DBG_ID");
            break;

        case HSI_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "HSI_HCL_DBG_ID");
            break;

        case MCDE_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "MCDE_HCL_DBG_ID");
            break;

		case DSI_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "DSI_HCL_DBG_ID");
			break;

        case MMCSD_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "MMCSD_HCL_DBG_ID");
            break;

        case SSP_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "SSP_HCL_DBG_ID");
            break;

		case SPI_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "SPI_HCL_DBG_ID");
			break;

		case PRCC_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "PRCC_HCL_DBG_ID");
			break;

		case USB_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "USB_HCL_DBG_ID");
            break;

        case PWL_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "PWL_HCL_DBG_ID");
            break;

        case SKE_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "SKE_HCL_DBG_ID");
            break;

		case HASH_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "HASH_HCL_DBG_ID");
			break;

		case CRYP_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "CRYP_HCL_DBG_ID");
            break;

		case SBAG_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "SBAG_HCL_DBG_ID");
			break;

		case STM_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "STM_HCL_DBG_ID");
            break;

		case PKA_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "PKA_HCL_DBG_ID");
            break;
            
        case RNG_HCL_DBG_ID:
            snprintf(temp_string,sizeof(temp_string), "%s :", "RNG_HCL_DBG_ID");
            break;

		case GENEPI_CODEC_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "GENEPI_CODEC_HCL_DBG_ID");
			break;

		case GENEPI_TVOUT_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "GENEPI_TVOUT_HCL_DBG_ID");
			break;

		case GENEPI_CORE_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "GENEPI_CORE_HCL_DBG_ID");
			break;

		case AB8500_CODEC_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "AB8500_CODEC_HCL_DBG_ID");
			break;

		case AB8500_TVOUT_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string), "%s :", "AB8500_TVOUT_HCL_DBG_ID");
			break;

		case AB8500_CORE_HCL_DBG_ID:
			snprintf(temp_string,sizeof(temp_string),"%s :", "AB8500_CORE_HCL_DBG_ID");
			break;


        default:
            snprintf(temp_string,sizeof(temp_string), "%s :", "UNKNOWN_HCL_DBG_ID");
            break;
    }

    strncat(debug_message, temp_string, sizeof(temp_string));

    snprintf(temp_string,sizeof(temp_string), "%s :", function_name);
    strncat(debug_message, temp_string, sizeof(temp_string));

    if (NULL != arg1)
    {
        snprintf(temp_string,sizeof(temp_string), "%s :", arg1);
        strncat(debug_message, temp_string, sizeof(temp_string));
    }

    if (NULL != arg_string)
    {
        if (NULL == arg3)
        {
            snprintf(temp_string,sizeof(temp_string), (char *) arg_string, arg2);
            strncat(debug_message, temp_string, sizeof(temp_string));
        }
        else if (NULL == arg4)
        {
            snprintf(temp_string,sizeof(temp_string), (char *) arg_string, arg2, arg3);
            strncat(debug_message, temp_string, sizeof(temp_string));
        }
        else if (NULL == arg5)
        {
            snprintf(temp_string,sizeof(temp_string), (char *) arg_string, arg2, arg3, arg4);
            strncat(debug_message, temp_string, sizeof(temp_string));
        }
        else if (NULL == arg6)
        {
            snprintf(temp_string,sizeof(temp_string), (char *) arg_string, arg2, arg3, arg4, arg5);
            strncat(debug_message, temp_string, sizeof(temp_string));
        }
        else if (NULL == arg7)
        {
            snprintf(temp_string,sizeof(temp_string), (char *) arg_string, arg2, arg3, arg4, arg5, arg6);
            strncat(debug_message, temp_string, sizeof(temp_string));
        }
        else
        {
            snprintf(temp_string,sizeof(temp_string), (char *) arg_string, arg2, arg3, arg4, arg5, arg6, arg7);
            strncat(debug_message, temp_string, sizeof(temp_string));
        }
    }

     if (NULL != arg1)
    {
    if (NULL == strcmp((char *) arg1, "Exiting"))
    {
        snprintf(temp_string,sizeof(temp_string), "Return Error: %ld", exit_param);
        strncat(debug_message, temp_string, sizeof(temp_string));
    }
    }
    snprintf(temp_string,sizeof(temp_string), "\n");
    strncat(debug_message, temp_string, sizeof(temp_string));

    CheckInterruptMode(&interrupt_flag);

    if (interrupt_flag != NULL)
    {
        strncpy(debug_buffer[interrupt_msg_count++], debug_message, sizeof(debug_message+1));
        if (interrupt_msg_count >= 100)
        {
            PRINT("INTERRUPT BUFFER OVERRUN!!!\n");
        }
    }
    else
    {
        if (interrupt_msg_count != NULL)
        {
            for (index = 0; index < interrupt_msg_count; index++)
            {
                PRINT(debug_buffer[index]);
                strncpy(debug_buffer[index], "", sizeof(debug_message));
            }

            interrupt_msg_count = 0;
        }

        PRINT(debug_message);
    }

    strncpy(debug_message, "", sizeof(debug_message));
    return(0);
}


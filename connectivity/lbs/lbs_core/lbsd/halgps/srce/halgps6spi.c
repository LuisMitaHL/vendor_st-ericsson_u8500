/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#define __HALGPS6SPI_C__

#if defined( GPS_OVER_SPI ) && defined( __RTK_E__ )
/**
* \file halgps6spi.c
* \date 04/06/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains all function that managed the SPI connection between sysol and GPS module.\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 17.07.08</TD><TD> M.CQUELIN </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#include "halgpsspi.h"

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_HALGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     6

/* FIXME: disable compilation optimisation */
//#pragma O0

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/*******************************************************************************/
/* HALGPS6_00SpiInit : initialises kid interface for SPI                       */
/*******************************************************************************/
bool HALGPS6_00SpiInit(void)
{
    t_kidError vl_ErrorStatus;
#if HALGPS_SPI_PORT_NUMBER == 0
    t_SPI *ptThisSPI = KID0_Access(SPI,/* HALGPS_SPI_PORT_NUMBER */ 0);
#elif HALGPS_SPI_PORT_NUMBER == 1
    t_SPI *ptThisSPI = KID0_Access(SPI,/* HALGPS_SPI_PORT_NUMBER */ 1);
#else
    #error Error: unknown SPI port
#endif

#ifndef HDL_CLEANUP_INTHDL_CPR
    t_kidSetRoutingServ  vl_SetRoutingServ;
#endif

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS6_00SpiInit");

    /*------------------------------------------------------------------------*/
    /* register of API SPI                                                    */
    /*------------------------------------------------------------------------*/
    vl_ErrorStatus = kid3apiSetup(SPI, (t_pfnInitFunc)SPI2_00Init, SPI2_01Reset, SPI2_02Read, SPI2_03Write, SPI2_04Ioctl);

    if (KIDERR_OK != vl_ErrorStatus)
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        return FALSE;
    }

    /* init KID SPI */
    vl_ErrorStatus = kid3_00Init ( SPI, HALGPS_SPI_PORT_NUMBER, (void *)&vg_HALGPS_SPIInitData );
    if (KIDERR_OK != vl_ErrorStatus)
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        return FALSE;
    }

    /*------------------------------------------------------------------------*/
    /* init global variable                                                   */
    /*------------------------------------------------------------------------*/
    vg_HALGPS_SpiWriteEndIndex = 0;

    vg_HALGPS_SpiWriteBeginIndex = 0;


#ifndef HDL_CLEANUP_INTHDL_CPR
    KID_SetService (&tSetRoutingServ,
                    INTC_INTERRUPT_PRIORITY_5, INTC_IRQ,
                    RawIntHandler_IRQ, HALGPS6_05SpiOnError, SPI1_05DMAProgress);

    if (kid3_04Ioctl (SPI, HALGPS_SPI_PORT_NUMBER, SPI_IOCTL_SET_DMA_SERVICE, &tSetRoutingServ) != KIDERR_OK)
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FOURTH_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        return(FALSE);
    }
#else /* HDL_CLEANUP_INTHDL_CPR */
    /* Init Interrupts and related vectors */
    inthdl4_01InitService(HALGPS_SPI_INT, INTHDL_SERVICE_DMA,
                          SPI1_00DST,
                          HALGPS6_05SpiOnError,
                          SPI1_05DMAProgress);
    inthdl4_02EnableInt(HALGPS_SPI_INT);
#endif /* HDL_CLEANUP_INTHDL_CPR */


    /* Set SPI speed */
    MC_HALGPS_TRACE_INF("SpiSpeed was: %d kbit/s (con.rate=%d). Trying to set it to %d kbit/s", \
              kid3_04Ioctl(SPI, HALGPS_SPI_PORT_NUMBER, SPI_IOCTL_GET_SPEED, 0), KID0_GetThisField(SPI, con, rate), HALGPS_SPI_SPEED);
    vl_ErrorStatus = kid3_04Ioctl( SPI, HALGPS_SPI_PORT_NUMBER, SPI_IOCTL_SET_SPEED, (void *) HALGPS_SPI_SPEED );
    if (KIDERR_OK != vl_ErrorStatus)
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIFTH_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        /* return(FALSE); */
    }

    MC_HALGPS_TRACE_INF("SpiSpeed is now: %d kbit/s (con.rate=%d)", \
              kid3_04Ioctl(SPI, HALGPS_SPI_PORT_NUMBER, SPI_IOCTL_GET_SPEED, 0), KID0_GetThisField(SPI, con, rate));

#if (defined(HALGPS_SPI_OPTIMIZATION))
    HALGPS6_06SpiDataInput();
#endif   /* HALGPS_SPI_OPTIMIZATION */

    MC_HALGPS_TRACE_INF("END : HALGPS6_00SpiInit");

    return TRUE;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/******************************************************************************/
/*                            HALGPS6_01SpiRcv                                */
/******************************************************************************/
uint32_t HALGPS6_01SpiRcv(uint8_t * pp_rxBuffer, uint32_t  vp_length)
{
    t_kidIORB *  pl_SpiRxIORB=NULL;

    int32_t vl_DataLength = 0;
    uint32_t vl_returnSize = 0;
    t_kidError vl_ErrorStatus;
    uint8_t * pl_buffer=NULL;
    uint32_t vl_i =0;
    uint32_t vl_j=0;
    bool vl_continue=TRUE;
    bool vl_receiveData=FALSE;
    uint8_t vl_maxRetry = HALGPS_MAX_RETRY;
    uint32_t vl_SendSize =0;
    uint32_t vl_copyLength = 0;

    if (vp_length ==0 || pp_rxBuffer==NULL)
    {
        return 0;
    }

    /*Init variable for the copy */
    vl_DataLength = vp_length;
    pl_buffer = pp_rxBuffer;

    while ((vl_DataLength >0) && (vl_continue==TRUE))
    {
        pl_SpiRxIORB = (t_kidIORB *)&vg_HALGPS_SpiRxBuffer[0];

        KID_SetIorb ( pl_SpiRxIORB, ((uint8_t*)pl_SpiRxIORB)+sizeof(*pl_SpiRxIORB), HALGPS_RX_BUFFER_MAX_SIZE, KID_READ, KID_MODE_DMA );
        KID_SetIorbParam1   ( pl_SpiRxIORB, SPI_MODE_BLOCK_TRANSFER | SPI_MODE_NO_MASK_SHIFT );

        KID_SetIorbWhenDone ( pl_SpiRxIORB, HALGPS6_03SpiIt );

#if (defined(HALGPS_SPI_OPTIMIZATION))
        /*configure theSPI data out like an input to solve power consumption issue*/
          HALGPS6_07SpiDataOutput(vg_SavedGpioValue,FALSE);
#endif   /* HALGPS_SPI_OPTIMIZATION */

        /* chip select of GPS chips*/
        if (kid3_04Ioctl ( GPIO, MC_GPIO_BANK_NUMBER(HALGPS_GPIO_SPI_SS), GPIO_IOCTL_SET_OUTPUT_ZERO, (uint32_t *)GPIO_MAP_PIN_TO_MASK(MC_GPIO_PIN_NUMBER(HALGPS_GPIO_SPI_SS)) ) != KIDERR_OK)
            MC_HALGPS_TRACE_INF("HALGPS RECV FAILED : IOCTL CHIP SELECT");

        /* Now send the data */
        vl_ErrorStatus = SPI2_03Write(SPI, HALGPS_SPI_PORT_NUMBER, (t_kidIORB *)&vg_HALGPS_SpiRxBuffer[0]);

        /* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
        if( vl_ErrorStatus != KIDERR_OK )
        {
            MC_HALGPS_TRACE_INF("HALGPS6_01SpiRcv: Call to SPI2_03Write has resulted in error : %d\n", vl_ErrorStatus); 
        }        
        /* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */

        /* wait the end of the read */
        MC_RTK_SEM_CONSUME(HALGPS_SPI_SEM);

#if (defined(HALGPS_SPI_OPTIMIZATION))
        HALGPS6_06SpiDataInput();
#endif   /* HALGPS_SPI_OPTIMIZATION */

        /* parse the read data*/
        for (vl_i=0;(vg_HALGPS_SpiRxBuffer[sizeof(*pl_SpiRxIORB)+vl_i] == 0) && (vl_i<(HALGPS_RX_BUFFER_MAX_SIZE-1));vl_i++);

        if ((vl_i== (HALGPS_RX_BUFFER_MAX_SIZE-1)) && (vg_HALGPS_SpiRxBuffer[sizeof(*pl_SpiRxIORB)+vl_i] == 0))
        {
            /* no valid data */
            if ((vl_receiveData ==TRUE) || (vl_maxRetry==0))
            {
                vl_continue = FALSE;
            }
            else
            {
                vl_maxRetry--;
            }
        }
        else
        {
            vl_receiveData = TRUE;

            /* If start of frame, set OS latency timer */
#if defined LBS_INTRPT_BB_READ
            if ( (vg_HALGPS_LantencyTimer==0) || (vg_HALGPS_ResetLantencyTimer == 1))
            {
                vg_HALGPS_LantencyTimer = GN_GPS_Get_OS_Time_ms();
                vg_HALGPS_ResetLantencyTimer = 0;
            }
#endif
            for (vl_j=(HALGPS_RX_BUFFER_MAX_SIZE-1);((vg_HALGPS_SpiRxBuffer[sizeof(*pl_SpiRxIORB)+vl_j] == 0)&& (vl_j>vl_i));vl_j--);
            vl_j++;
            vl_copyLength=vl_j-vl_i;
            if (vl_copyLength > vl_DataLength) {
                vl_copyLength = vl_DataLength;  /* Don't read and copy more than vp_length... In that case: data lost */
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
            }
            memcpy(pl_buffer, (uint8_t*)&vg_HALGPS_SpiRxBuffer[sizeof(*pl_SpiRxIORB)+vl_i], vl_copyLength);
            vl_DataLength = vl_DataLength - vl_copyLength;
            pl_buffer += vl_copyLength;
            vl_returnSize += vl_copyLength;
            if (vl_j!=HALGPS_RX_BUFFER_MAX_SIZE)
            {
                vl_continue= FALSE;
#if defined LBS_INTRPT_BB_READ
                /* Reset OS Latency flag to authorize a measurement at next frame */
                vg_HALGPS_ResetLantencyTimer = 1;
#endif
            }
            else
            {
                vl_continue = TRUE;
            }
        }
    }

    /* Pad the Tx data with 19 NULL bytes */
    if (vg_HALGPS_SpiWriteBeginIndex != vg_HALGPS_SpiWriteEndIndex)
    {
        HALGPS6_08PadSpiTxBuf();
    }
    /* verify if data are ready to send */
    while ((vg_HALGPS_SpiWriteBeginIndex != vg_HALGPS_SpiWriteEndIndex))
    {
        /* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
        //uint8_t  vl_TempChar;
        /* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */

        if (vg_HALGPS_SpiWriteEndIndex>vg_HALGPS_SpiWriteBeginIndex)
        {
            vl_SendSize = vg_HALGPS_SpiWriteEndIndex-vg_HALGPS_SpiWriteBeginIndex;
        }
        else
        {
            vl_SendSize = HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteBeginIndex;
        }

        if (vl_SendSize>HALGPS_RX_BUFFER_MAX_SIZE)
        {
            vl_SendSize = HALGPS_RX_BUFFER_MAX_SIZE;
        }


        pl_SpiRxIORB = (t_kidIORB *)&vg_HALGPS_SpiRxBuffer[0];

        KID_SetIorb ( pl_SpiRxIORB, (uint8_t*)&vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteBeginIndex], vl_SendSize, KID_WRITE, KID_MODE_DMA );
        KID_SetIorbParam1   ( pl_SpiRxIORB, SPI_MODE_BLOCK_TRANSFER | SPI_MODE_NO_MASK_SHIFT );

        KID_SetIorbWhenDone ( pl_SpiRxIORB, HALGPS6_03SpiIt );

#if (defined(HALGPS_SPI_OPTIMIZATION))
        HALGPS6_07SpiDataOutput(vg_SavedGpioValue,FALSE);
#endif       /* HALGPS_SPI_OPTIMIZATION */

        /* chip select of GPS chips*/
        if (kid3_04Ioctl ( GPIO, MC_GPIO_BANK_NUMBER(HALGPS_GPIO_SPI_SS), GPIO_IOCTL_SET_OUTPUT_ZERO, (uint32_t *)GPIO_MAP_PIN_TO_MASK(MC_GPIO_PIN_NUMBER(HALGPS_GPIO_SPI_SS)) ) != KIDERR_OK)
            MC_HALGPS_TRACE_INF("HALGPS SEND FAILED : IOCTL CHIP SELECT");

#if HALGPS_TRACE_SPI_SEND
/* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
{
        uint8_t vl_TempChar;
/* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
        vl_TempChar = vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteBeginIndex+vl_SendSize];
        *(&vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteBeginIndex]+vl_SendSize)=0;
        MC_HALGPS_TRACE_INF("SpiRcv: Sending len=%d begin=%d '%s'",vl_SendSize, vg_HALGPS_SpiWriteBeginIndex, &vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteBeginIndex]);
        vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteBeginIndex+vl_SendSize] = vl_TempChar;
/* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
}
/* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
#endif

        /* Now send the data */
        vl_ErrorStatus = SPI2_03Write(SPI, HALGPS_SPI_PORT_NUMBER, (t_kidIORB *)&vg_HALGPS_SpiRxBuffer[0]);

        /* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
        if( vl_ErrorStatus != KIDERR_OK )
        {
            MC_HALGPS_TRACE_INF("HALGPS6_01SpiRcv: Call to SPI2_03Write has resulted in error : %d\n", vl_ErrorStatus); 
        }        
        /* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */

        /* wait the end of the read */
        MC_RTK_SEM_CONSUME(HALGPS_SPI_SEM);

#if (defined(HALGPS_SPI_OPTIMIZATION))
        HALGPS6_06SpiDataInput();
#endif    /* HALGPS_SPI_OPTIMIZATION */

        if ((vg_HALGPS_SpiWriteBeginIndex+vl_SendSize) >= HALGPS_TX_BUFFER_MAX_SIZE)
        {
            vg_HALGPS_SpiWriteBeginIndex = 0;
        }
        else
        {
            vg_HALGPS_SpiWriteBeginIndex+= vl_SendSize;
        }
    }

#if HALGPS_TRACE_SPI_RCV
    *(pp_rxBuffer+vl_returnSize) = 0;
    MC_HALGPS_TRACE_INF("SpiRcv: len=%d '%s'", vl_returnSize, pp_rxBuffer);
#endif

    return (vl_returnSize);
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/******************************************************************************/
/**                           HALGPS6_02SpiSend                              **/
/******************************************************************************/
uint32_t HALGPS6_02SpiSend(uint8_t * pp_txBuffer, uint32_t  vp_length)
{
    uint32_t vl_freeSize =0;
    uint32_t vl_returnLenght =0;

    if (vg_HALGPS_SpiWriteBeginIndex>vg_HALGPS_SpiWriteEndIndex)
    {
        vl_freeSize = vg_HALGPS_SpiWriteBeginIndex-vg_HALGPS_SpiWriteEndIndex;
        if (vp_length<vl_freeSize)
        {
            memcpy((uint8_t*)&vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteEndIndex],pp_txBuffer,vp_length);
            vg_HALGPS_SpiWriteEndIndex+=vp_length;
            vl_returnLenght = vp_length;
        }
        else
        {
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }
    else
    {
        vl_freeSize = HALGPS_TX_BUFFER_MAX_SIZE-(vg_HALGPS_SpiWriteEndIndex-vg_HALGPS_SpiWriteBeginIndex);
        if (vp_length<vl_freeSize)
        {
            if (vp_length<(HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex))
            {
                memcpy((uint8_t*)&vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteEndIndex],pp_txBuffer,vp_length);
                vg_HALGPS_SpiWriteEndIndex+=vp_length;
                vl_returnLenght = vp_length;
            }
            else
            {
                memcpy((uint8_t*)&vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteEndIndex], pp_txBuffer, HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex);
                memcpy((uint8_t*)&vg_HALGPS_SpiTxBuffer[0], pp_txBuffer+HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex, vp_length-(HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex));
                vg_HALGPS_SpiWriteEndIndex=vp_length-(HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex);
                vl_returnLenght = vp_length;
            }
        }
        else
        {
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }

#if HALGPS_TRACE_SPI_SEND
    //*(pp_txBuffer+vl_returnLenght) = 0;
    MC_HALGPS_TRACE_INF("SpiSend - will send: len=%d '%s'",vl_returnLenght, pp_txBuffer);
#endif

    return vl_returnLenght;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/******************************************************************************/
/**                           HALGPS6_03SpiIt                             **/
/******************************************************************************/
t_kidError HALGPS6_03SpiIt(t_kidIORB * pp_IORB)
{
    /* we are under IT, don't used trace system*/
    kid3_04Ioctl ( GPIO, MC_GPIO_BANK_NUMBER(HALGPS_GPIO_SPI_SS), GPIO_IOCTL_SET_OUTPUT_ONE, (uint32_t *)GPIO_MAP_PIN_TO_MASK(MC_GPIO_PIN_NUMBER(HALGPS_GPIO_SPI_SS)) );

    MC_RTK_SEM_PRODUCE(HALGPS_SPI_SEM);

    return KIDERR_OK;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/******************************************************************************/
/**                           HALGPS6_04ResetSpi                             **/
/******************************************************************************/
void HALGPS6_04ResetSpi()
{

    vg_HALGPS_SpiWriteEndIndex = 0;
    vg_HALGPS_SpiWriteBeginIndex = 0;

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/******************************************************************************/
/*                            HALGPS6_05SpiOnError                            */
/******************************************************************************/
t_kidError HALGPS6_05SpiOnError( t_kidDevType type,
                                 t_kidDevInst instance,
                                 t_kidError   errcode)
{
    if (type == SPI)
    {
        if (instance == HALGPS_SPI_PORT_NUMBER)
        {
            /*an error occurs on SPI used by GPS module*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            vg_HALGPS_ErrorNumber|=((uint8_t)errcode);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }
    MC_HALGPS_TRACE_INF("HALGPS6_05SpiError");
    return KIDERR_OK;
}


#if (defined(HALGPS_SPI_OPTIMIZATION))
/******************************************************************************/
/*                            HALGPS6_06SpiDataInput                          */
/******************************************************************************/
void HALGPS6_06SpiDataInput(void)
{
     uint32_t vl_bank=0;
    uint32_t vl_GPIOvalue=0;
    uint32_t vl_ret=0;

    vl_bank = MC_GPIO_BANK_NUMBER(HALGPS_GPIO_SPI_DATIO1);

/* ++LMSqb95231 */
    /* Change sysmux from SPIDATA0 to GPIO */
/*
    switch(vl_bank)
    {
       case 0:
       {
          vl_GPIOvalue = SCON_MUX_GPIO0_GPIO;
          break;
       }
       case 1:
       {
          vl_GPIOvalue = SCON_MUX_GPIO1_GPIO;
          break;
       }
       case 2:
       {
          vl_GPIOvalue = SCON_MUX_GPIO2_GPIO;
          break;
       }
       case 3:
       {
          vl_GPIOvalue = SCON_MUX_GPIO3_GPIO;
          break;
       }
       case 4:
       {
          vl_GPIOvalue = SCON_MUX_GPIO4_GPIO;
          break;
       }
       default:
       {

          MC_HALGPS_TRACE_INF("HALGPS6_06SpiDataInput pb 1");
          return;
       }
    }
*/
    vl_GPIOvalue = SCON_MUX_GPIO4_GPIO;
/* --LMSqb95231 */

    HALGPS6_07SpiDataOutput(vl_GPIOvalue, TRUE);

    /*configure theSPI data out like an input */
    vl_ret = GPIO2_04Ioctl ( GPIO, 0, GPIO_IOCTL_GET_MULTIPLEXER_SETTINGS,(void *) 0 );
    if(vl_ret != KIDERR_OK)MC_HALGPS_TRACE_INF("HALGPS6_06SpiDataInput get multiplexeur fail");

    vl_ret = GPIO2_04Ioctl ( GPIO, MC_GPIO_BANK_NUMBER(HALGPS_GPIO_SPI_DATIO1), GPIO_IOCTL_SET_PIN_INPUT,(uint32_t*)MC_GPIO_PIN_NUMBER(HALGPS_GPIO_SPI_DATIO1));
    if(vl_ret != KIDERR_OK)MC_HALGPS_TRACE_INF("HALGPS6_06SpiDataInput set input fail, bank :%i,  pin number : %i",vl_bank,MC_GPIO_PIN_NUMBER(HALGPS_GPIO_SPI_DATIO1));

}

/******************************************************************************/
/*                            HALGPS6_07SpiDataOutput                         */
/******************************************************************************/
void HALGPS6_07SpiDataOutput(uint8_t vp_gpioValue, uint8_t vp_saveValue)
{
   t_SCONInitData tDin2SconInitData;
   uint32_t vl_ret=0;
   uint32_t vl_bank=0;
   /* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
   //uint32_t vl_GPIOvalue=0;
   /* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
   uint32_t vl_gpioNumber=0;
   uint32_t vl_gpioMuxID=0;
   uint32_t vl_gpioUp=0;

   /* Read the initialisation data out from the SCON */
   vl_ret = kid3_04Ioctl(SCON, 0, SCON_IOCTL_GET_CONFIGURATION, &tDin2SconInitData);
   if(vl_ret != KIDERR_OK)MC_HALGPS_TRACE_INF("HALGPS6_06SpiDataInput fail to get conf 2");

   vl_bank = MC_GPIO_BANK_NUMBER(HALGPS_GPIO_SPI_DATIO1);
   vl_gpioNumber = MC_GPIO_PIN_NUMBER(HALGPS_GPIO_SPI_DATIO1);
/* ++LMSqb95923 */
/*
   if(vl_gpioNumber > 15)
   {
      vl_gpioUp = 1;
      vl_gpioMuxID = vl_gpioNumber-16;
   }
   else
   {
      vl_gpioMuxID = vl_gpioNumber;
      vl_gpioUp = 0;
   }
*/
   vl_gpioUp = 1;
   vl_gpioMuxID = vl_gpioNumber-16;
/* --LMSqb95923 */

   tDin2SconInitData.atSCONMUXInitData[(vl_bank*2)+vl_gpioUp].sconMux &= ~( 0x03 << (vl_gpioMuxID*2));

   if(vp_saveValue)
   {
      vg_SavedGpioValue = (tDin2SconInitData.atSCONMUXInitData[(vl_bank*2)+vl_gpioUp].sconMux)>>(vl_gpioMuxID*2);
   }

   tDin2SconInitData.atSCONMUXInitData[(vl_bank*2)+vl_gpioUp].sconMux |= (( 0x03&vp_gpioValue) << (vl_gpioMuxID*2));

   /* Configuration data of SCON */
   vl_ret = kid3_04Ioctl(SCON, 0, SCON_IOCTL_SET_CONFIGURATION, &tDin2SconInitData);
   if(vl_ret != KIDERR_OK)MC_HALGPS_TRACE_INF("HALGPS6_06SpiDataInput fail to set the conf");

}
#endif /* HALGPS_SPI_OPTIMIZATION*/


/******************************************************************************/
/*                            HALGPS6_08PadSpiTxBuf                         */
/******************************************************************************/
void HALGPS6_08PadSpiTxBuf(void)
{
    uint32_t vl_PLen = HALGPS_SPI_PADDING_BUF_LEN;
    uint32_t vl_PfreeSize =0;
    uint8_t  vl_PaddingBuffer[HALGPS_SPI_PADDING_BUF_LEN];
    uint8_t  vl_Index;

    for ( vl_Index = 0 ; vl_Index < HALGPS_SPI_PADDING_BUF_LEN ; vl_Index++ )
    {
        vl_PaddingBuffer[vl_Index] = '\0';
    }

    if (vg_HALGPS_SpiWriteBeginIndex>vg_HALGPS_SpiWriteEndIndex)
    {
        vl_PfreeSize = vg_HALGPS_SpiWriteBeginIndex-vg_HALGPS_SpiWriteEndIndex;
        if (vl_PLen<vl_PfreeSize)
        {
            memcpy((uint8_t*)&vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteEndIndex],vl_PaddingBuffer,vl_PLen);
            vg_HALGPS_SpiWriteEndIndex+=vl_PLen;
        }
        else
        {
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }
    else
    {
        vl_PfreeSize = HALGPS_TX_BUFFER_MAX_SIZE-(vg_HALGPS_SpiWriteEndIndex-vg_HALGPS_SpiWriteBeginIndex);
        if (vl_PLen<vl_PfreeSize)
        {
            if (vl_PLen<(HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex))
            {
                memcpy((uint8_t*)&vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteEndIndex],vl_PaddingBuffer,vl_PLen);
                vg_HALGPS_SpiWriteEndIndex+=vl_PLen;
            }
            else
            {
                memcpy((uint8_t*)&vg_HALGPS_SpiTxBuffer[vg_HALGPS_SpiWriteEndIndex], vl_PaddingBuffer, HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex);
                /* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
                memcpy((uint8_t*)&vg_HALGPS_SpiTxBuffer[0], vl_PaddingBuffer+(HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex), vl_PLen-(HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex));
                /* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
                vg_HALGPS_SpiWriteEndIndex=vl_PLen-(HALGPS_TX_BUFFER_MAX_SIZE-vg_HALGPS_SpiWriteEndIndex);
            }
        }
        else
        {
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }

}


#endif /*GPS_OVER_SPI || __RTK_E__*/
#undef __HALGPS6SPI_C__

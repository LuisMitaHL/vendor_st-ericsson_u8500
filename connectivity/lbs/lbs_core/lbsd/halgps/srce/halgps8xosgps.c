/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#define __HALGPS8XOSGPS_C__
#ifdef GPS_OVER_XOSGPS
/**
* \file halgps8xosgps.c
* \date 26/05/2009
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
*             <TD> 17.07.08</TD><TD> Archana.B </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#include "halgpsxosgps.h"

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_HALGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     8

/* FIXME: disable compilation optimisation */
//#pragma O0


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/*******************************************************************************/
/* HALGPS8_00XosgpsInit : initialises XOSGPS                       */
/*******************************************************************************/
bool HALGPS8_00XosgpsInit(void)
{
    bool vl_RetValue = FALSE;
    MC_HALGPS_TRACE_INF("BEGIN: HALGPS8_00XosgpsInit");

    if( XOSGPS_Init() != 0 )
    {
        MC_HALGPS_TRACE_INF("HALGPS8_00XosgpsInit successful");
        vl_RetValue = TRUE;
    }

    /*------------------------------------------------------------------------*/
    /* init global variable                                                   */
    /*------------------------------------------------------------------------*/
    vg_HALGPS_XSpiWriteEndIndex = 0;

    vg_HALGPS_XSpiWriteBeginIndex = 0;



    MC_HALGPS_TRACE_INF("END: HALGPS8_00XosgpsInit");
    return vl_RetValue;
}



/******************************************************************************/
/*                            HALGPS8_05PadSpiTxBuf                         */
/******************************************************************************/
void HALGPS8_05PadSpiTxBuf(void)
{
    uint32_t vl_PLen = 19;//HALGPS_SPI_PADDING_BUF_LEN;
    uint32_t vl_PfreeSize =0;
    uint8_t  vl_PaddingBuffer[19];//HALGPS_SPI_PADDING_BUF_LEN];
    uint8_t  vl_Index;

    for ( vl_Index = 0 ; vl_Index < 19 ; vl_Index++ )
    {
        vl_PaddingBuffer[vl_Index] = '\0';
    }

    if (vg_HALGPS_XSpiWriteBeginIndex>vg_HALGPS_XSpiWriteEndIndex)
    {
        vl_PfreeSize = vg_HALGPS_XSpiWriteBeginIndex-vg_HALGPS_XSpiWriteEndIndex;
        if (vl_PLen<vl_PfreeSize)
        {
            memcpy((uint8_t*)&vg_HALGPS_XSpiTxBuffer[vg_HALGPS_XSpiWriteEndIndex],vl_PaddingBuffer,vl_PLen);
            vg_HALGPS_XSpiWriteEndIndex+=vl_PLen;
        }
        else
        {
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }
    else
    {
        vl_PfreeSize =  HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-(vg_HALGPS_XSpiWriteEndIndex-vg_HALGPS_XSpiWriteBeginIndex);
        if (vl_PLen<vl_PfreeSize)
        {
            if (vl_PLen<( HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex))
            {
                memcpy((uint8_t*)&vg_HALGPS_XSpiTxBuffer[vg_HALGPS_XSpiWriteEndIndex],vl_PaddingBuffer,vl_PLen);
                vg_HALGPS_XSpiWriteEndIndex+=vl_PLen;
            }
            else
            {
                memcpy((uint8_t*)&vg_HALGPS_XSpiTxBuffer[vg_HALGPS_XSpiWriteEndIndex], vl_PaddingBuffer,  HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex);
                memcpy((uint8_t*)&vg_HALGPS_XSpiTxBuffer[0], vl_PaddingBuffer+ HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex, vl_PLen-( HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex));
                vg_HALGPS_XSpiWriteEndIndex=vl_PLen-( HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex);
            }
        }
        else
        {
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }
}



uint32_t HALGPS8_04TransmitData()
{

    int32_t vl_DataLength = 0;
    uint32_t vl_returnSize = 0;
    uint8_t * pl_buffer;
    uint32_t vl_i =0;
    uint32_t vl_j=0;
    bool vl_continue=TRUE;
    bool vl_receiveData=FALSE;
    uint32_t vl_SendSize =0;
    uint32_t vl_BytesWritten =0;
    uint32_t vl_copyLength = 0;
    uint32_t vl_NumBytesToWrite =0;

        /* Pad the Tx data with 19 NULL bytes */
    if (vg_HALGPS_XSpiWriteBeginIndex != vg_HALGPS_XSpiWriteEndIndex)
    {
        HALGPS8_05PadSpiTxBuf();

         MC_HALGPS_TRACE_INF("Padding done");
    }
    /* verify if data are ready to send */
    while ((vg_HALGPS_XSpiWriteBeginIndex != vg_HALGPS_XSpiWriteEndIndex))
    {
        uint8_t  vl_TempChar;

        if (vg_HALGPS_XSpiWriteEndIndex>vg_HALGPS_XSpiWriteBeginIndex)
        {
            vl_SendSize = vg_HALGPS_XSpiWriteEndIndex-vg_HALGPS_XSpiWriteBeginIndex;
        }
        else
        {
            vl_SendSize = HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteBeginIndex;
        }

        if (vl_SendSize> 2048)
        {
            vl_SendSize = 2048;
        }



        MC_HALGPS_TRACE_INF("VlSendSize=%d",vl_SendSize);
        /* Now send the data */
        vl_BytesWritten = XOSGPS_Write((&vg_HALGPS_XSpiTxBuffer[vg_HALGPS_XSpiWriteBeginIndex]), vl_SendSize);

        if ((vg_HALGPS_XSpiWriteBeginIndex+vl_BytesWritten) >= HALGPS_XOSGPS_MAX_BUF_WRITE_LEN)
        {
            vg_HALGPS_XSpiWriteBeginIndex = 0;
        }
        else
        {
            vg_HALGPS_XSpiWriteBeginIndex+= vl_BytesWritten;
        }
        vl_NumBytesToWrite +=  vl_BytesWritten;
        

    }
     return;

}








#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/******************************************************************************/
/*                            HALGPS8_02XosgpsRcv                             */
/******************************************************************************/
uint32_t HALGPS8_02XosgpsRcv(uint8_t * pp_rxBuffer, uint32_t vp_length)
{

    uint32_t vl_BytesRead      = 0;
    uint32_t vl_TotalBytesRead = 0;
    uint16_t vl_BytesToRead    = 0;
    uint8_t  vl_Done           = 0;
    uint16_t vl_i              = 0;
    

    if ( vp_length ==0 || pp_rxBuffer==NULL )
    {
       vl_Done=1;
    }

      if(!vl_Done)
      {          
            vl_BytesToRead = vp_length - vl_TotalBytesRead;
            if(vl_BytesToRead > 256)
            {
                vl_BytesToRead = 256;
            }

        vl_BytesRead = XOSGPS_Read((pp_rxBuffer+vl_TotalBytesRead), vl_BytesToRead);
        MC_HALGPS_TRACE_INF("HALGPS8_02XosgpsRcv: Number of bytes read from XOSGPS_Read() = %d", vl_BytesRead);

        for (vl_i=0;(pp_rxBuffer[vl_i]==0 && vl_i<vl_BytesRead);vl_i++) ;
       
           if(vl_BytesRead == 0)
        {

          vl_Done=1;
        }
        else
        { 
            if(vl_i == vl_BytesToRead)
            {

              vl_Done=1;
            }
            else
            {
               vl_TotalBytesRead += vl_BytesRead;
            }
        }
      }


    while(!vl_Done)
    {
        if (vl_TotalBytesRead < vp_length)
        {
            vl_BytesToRead = vp_length - vl_TotalBytesRead;
            if(vl_BytesToRead > HALGPS_XOSGPS_MAX_BUF_READ_LEN)
            {
                vl_BytesToRead = HALGPS_XOSGPS_MAX_BUF_READ_LEN;
            }
        }
        else
        {
            vl_Done=1;
            continue;
        }


        vl_BytesRead = XOSGPS_Read((pp_rxBuffer+vl_TotalBytesRead), vl_BytesToRead);
        MC_HALGPS_TRACE_INF("HALGPS8_02XosgpsRcv: Number of bytes read from XOSGPS_Read() = %d", vl_BytesRead);

        if(vl_BytesRead == 0)
        {
            vl_Done= 1;
            continue;
        }
        else
        {
            vl_TotalBytesRead += vl_BytesRead;
        }
    }




    HALGPS8_04TransmitData();


    return vl_TotalBytesRead;
}




#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/******************************************************************************/
/**                           HALGPS8_02XosgpsSend                           **/
/******************************************************************************/
uint32_t HALGPS8_02XosgpsSend(uint8_t * pp_txBuffer, uint32_t  vp_length)
{
    uint32_t vl_BytesWritten    = 0;
    uint32_t vl_TotalBytesSent  = 0;
    uint32_t vl_NumBytesToWrite = 0;
    uint8_t  vl_Done            = 0;
    uint32_t vl_freeSize =0;
    uint32_t vl_returnLenght =0;

    if (vg_HALGPS_XSpiWriteBeginIndex>vg_HALGPS_XSpiWriteEndIndex)
    {
        vl_freeSize = vg_HALGPS_XSpiWriteBeginIndex-vg_HALGPS_XSpiWriteEndIndex;
        if (vp_length<vl_freeSize)
        {
            memcpy((uint8_t*)&vg_HALGPS_XSpiTxBuffer[vg_HALGPS_XSpiWriteEndIndex],pp_txBuffer,vp_length);
            vg_HALGPS_XSpiWriteEndIndex+=vp_length;
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
        vl_freeSize =  HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-(vg_HALGPS_XSpiWriteEndIndex-vg_HALGPS_XSpiWriteBeginIndex);
        if (vp_length<vl_freeSize)
        {
            if (vp_length<( HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex))
            {
                memcpy((uint8_t*)&vg_HALGPS_XSpiTxBuffer[vg_HALGPS_XSpiWriteEndIndex],pp_txBuffer,vp_length);
                vg_HALGPS_XSpiWriteEndIndex+=vp_length;
                vl_returnLenght = vp_length;
            }
            else
            {
                memcpy((uint8_t*)&vg_HALGPS_XSpiTxBuffer[vg_HALGPS_XSpiWriteEndIndex], pp_txBuffer,  HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex);
                memcpy((uint8_t*)&vg_HALGPS_XSpiTxBuffer[0], pp_txBuffer+ HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex, vp_length-( HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex));
                vg_HALGPS_XSpiWriteEndIndex=vp_length-( HALGPS_XOSGPS_MAX_BUF_WRITE_LEN-vg_HALGPS_XSpiWriteEndIndex);
                vl_returnLenght = vp_length;
            }
        }
        else
        {
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }


    return vl_returnLenght;





}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/******************************************************************************/
/**                           HALGPS8_03XosgpsCtrl                             **/
/******************************************************************************/
bool HALGPS8_03XosgpsCtrl( e_halgps_xosgps_ctrl vp_CtrlChar )
{
    bool vl_RetValue = FALSE;

    MC_HALGPS_TRACE_INF("BEGIN: HALGPS8_03XosgpsCtrl");

    switch( vp_CtrlChar )
    {
        case HALGPS_XOSGPS_POWER_ON:
        {
            if( XOSGPS_Ctrl(GPS_POWER_ON) != 0 )
            {
                vl_RetValue = TRUE;
            }
        }
        break;

        case HALGPS_XOSGPS_POWER_OFF:
        {
            if( XOSGPS_Ctrl(GPS_POWER_OFF) != 0 )
            {
                vl_RetValue = TRUE;
            }
        }
        break;

        case HALGPS_XOSGPS_RESET_ON:
        {
            if( XOSGPS_Ctrl(GPS_RESET_ON) != 0 )
            {
                vl_RetValue = TRUE;
            }
        }
        break;

        case HALGPS_XOSGPS_RESET_OFF:
        {
            if( XOSGPS_Ctrl(GPS_RESET_OFF) != 0 )
            {
                vl_RetValue = TRUE;
            }
        }
        break;
    }

    MC_HALGPS_TRACE_INF("END: HALGPS8_03XosgpsCtrl");

    return vl_RetValue;
}

#endif /*GPS_OVER_XOSGPS*/
#undef __HALGPS8XOSGPS_C__

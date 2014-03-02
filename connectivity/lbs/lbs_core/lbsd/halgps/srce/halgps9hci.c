/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#define __HALGPS9HCI_C__
#ifdef GPS_OVER_HCI
/**
* \file halgps9hci.c
* \date 02/02/2010
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains functions to manage the HCI connection between Linux Platforms and GPS module.\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 02.02.10</TD><TD> Archana.B </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#include "halgpshci.h"

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_HALGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     9

/* FIXME: disable compilation optimisation */
//#pragma O0


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/*******************************************************************************/
/* HALGPS9_00XosgpsInit : initialises GPS IP over HCI                     */
/*******************************************************************************/
uint8_t HALGPS9_00HciStart(void)
{
    uint8_t vl_RetVal = FALSE;

    if (vg_HALGPS_HciRxBuffer== NULL) return vl_RetVal;

    if( vg_HALGPS_HciState == K_HCI_STATE_NOT_CONNECTED )
    {
        /* Reset any old data remaining in the buffer */
        vg_HALGPS_HciRawReadLen = 0;

        if( HALGPS10_00HciInit() )
        {
            vg_HALGPS_HciState = K_HCI_STATE_CONNECTED;
            vg_HALGPS_Max_Num_Missed_Acks =0;

            if( vg_HALGPS_HciGpsState != K_HCI_GPS_DISABLED )
            {
                MC_HALGPS_TRACE_INF("HALGPS9_00HciStart: WARNING Unexpected vg_HALGPS_HciGpsState <%d>\n" , vg_HALGPS_HciGpsState );
                vg_HALGPS_HciGpsState = K_HCI_GPS_DISABLED;
            }

#ifdef __RTK_E__
#ifdef UTS_CPR
            MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,10);
#else
            MC_OST_START_TASK_TIMER(10);
#endif
#else
            OSA_Sleep( 10 );
#endif
            // Next we will enable the GPS IP
            //Send enable command


                   if( HALGPS9_09HciGpsEnable())
                {
                    vl_RetVal = TRUE;
                }

        }
    }
    else
    {
        MC_HALGPS_TRACE_INF("HALGPS9_00HciStart: WARNING Already in state <%d>\n" , vg_HALGPS_HciState);
        vl_RetVal = TRUE;
    }

    MC_HALGPS_TRACE_INF( "END: HALGPS9_00HciStart");

    return vl_RetVal;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*******************************************************************************/
/* HALGPS9_01HciStop : Disable GPS IP over HCI                     */
/*******************************************************************************/
uint8_t HALGPS9_01HciStop(void)
{
    uint8_t vl_RetVal;

    MC_HALGPS_TRACE_INF ("BEGIN: HALGPS9_01HciStop");

    if (HALGPS9_10HciGpsDisable() == FALSE)
    {
        MC_HALGPS_TRACE_INF ("HALGPS9_01HciStop: GPS IP DISABLE unsucessfull: ");
        vl_RetVal =  FALSE;
    }
    else
    {
        MC_HALGPS_TRACE_INF ("HALGPS9_01HciStop: GPS IP DISABLE");
        vl_RetVal =  TRUE;
    }

    HALGPS10_03HciDeinit();

    vg_HALGPS_HciState = K_HCI_STATE_NOT_CONNECTED;

    MC_HALGPS_TRACE_INF ("END: HALGPS9_01HciStop");
    return vl_RetVal;

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/******************************************************************************/
/**                           HALGPS9_02WaitForTxAck                                                               **/
/******************************************************************************/
uint8_t HALGPS9_02WaitForTxAck(uint8_t vp_Max_Retry)
{
    uint8_t vl_NumOfRetry =0;
    uint8_t vl_RetVal = FALSE;
    MC_HALGPS_HCI_TRACE_STRING(("HALGPS9_02WaitForTxAck; Entry"));

    while(( vg_HALGPS_HciState == K_HCI_STATE_WAITING_FOR_TXACK ) && (vl_NumOfRetry < vp_Max_Retry))
    {
        HALGPS9_08HciReadAndProcess();
#ifdef __RTK_E__
#ifdef UTS_CPR
        MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,HALGPS_HCI_POLL_TIME_MS);
#else
        MC_OST_START_TASK_TIMER(HALGPS_HCI_POLL_TIME_MS);
#endif
#else
        OSA_Sleep( HALGPS_HCI_POLL_TIME_MS );
#endif
       vl_NumOfRetry++;
    }
     //Need to check whther this is the correct place
    if (vg_HALGPS_Max_Num_Missed_Acks == HALGPS_HCI_MAX_NUM_MISSED_ACKS-1 )
    {
       MC_HALGPS_TRACE_INF(("ERROR: Missed 50 Ack- PE may reset"));
       //HALGPS9_15HciReset();
       vg_HALGPS_Max_Num_Missed_Acks =0;
    }

    /*Number of Retry is less than max nymber and hci state is not waiting for ack*/
    if (vg_HALGPS_HciState == K_HCI_STATE_CONNECTED)
    {
       /*Got the acknowledgement*/
       vl_RetVal = TRUE;
    }
    else
    {
       /*Missed the acknowledgement we may end up in hard reset*/
       vg_HALGPS_Max_Num_Missed_Acks++;
       MC_HALGPS_TRACE_INF(("WARNING: Ack is not received for last 100 ms- PE may reset"));
    }

    MC_HALGPS_HCI_TRACE_STRING(("HALGPS9_02WaitForTxAck; Exit"));
    return vl_RetVal;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/******************************************************************************/
/**                           HALGPS9_03HciRxBuffInit                                                                 **/
/******************************************************************************/
void HALGPS9_03HciRxBuffInit()
{
    /*Allocate the HCI Read Buffers*/
    if(vg_HALGPS_HciRxBuffer != NULL)
    {
       MC_RTK_FREE_MEMORY(vg_HALGPS_HciRxBuffer);
       vg_HALGPS_HciRxBuffer = NULL;
    }

    if(vg_HALGPS_HciRawReadBuffer != NULL)
    {
       MC_RTK_FREE_MEMORY(vg_HALGPS_HciRawReadBuffer);
       vg_HALGPS_HciRawReadBuffer = NULL;
    }

    vg_HALGPS_HciRxBuffer = (t_HALGPSCommBuffer*)MC_RTK_GET_MEMORY(sizeof(*vg_HALGPS_HciRxBuffer));

    if(vg_HALGPS_HciRxBuffer == NULL)
    {
         MC_HALGPS_HCI_TRACE_STRING(("ERROR: Unable to allocate memory for Hci Recieve buffer"));
    }
    else
    {
        vg_HALGPS_HciRxBuffer->v_DataStart = 0;
        vg_HALGPS_HciRxBuffer->v_BufferFreeStart = 0;

        vg_HALGPS_HciRawReadBuffer = MC_RTK_GET_MEMORY(HALGPS_HCI_MAX_READ_LEN+HALGPS_HCI_HEADER_LEN);

        if(vg_HALGPS_HciRawReadBuffer == NULL)
        {
           MC_HALGPS_HCI_TRACE_STRING(("ERROR: Unable to allocate memory for Hci Raw Read buffer"));
        }
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/******************************************************************************/
/**                           HALGPS9_04HciRxBuffUpdate                                                            **/
/**  Updates the receive buffer with data from HCI                                                            **/
/**  param pp_rxBuffer buffer which contains data to be updated                                        **/
/**param vp_length data length                                                                                       **/
/*****************************************************************************/
void HALGPS9_04HciRxBuffUpdate(uint8_t * pp_rxBuffer, uint32_t vp_length)
{
    /* two cases need to be considered
    1. v_End is before v_Start - buffer has rolled around. We only need to copy from end to start
    2. v_End is after v_Start
    */
   if (vg_HALGPS_HciRxBuffer== NULL) return;

    MC_HALGPS_HCI_TRACE_STRING_PARAM (("HALGPS9_04HciRxBuffUpdate : Length <%lu>", vp_length));

    if( vg_HALGPS_HciRxBuffer->v_DataStart > vg_HALGPS_HciRxBuffer->v_BufferFreeStart )
    {
        /* If v_End is before v_Start, we only have enough space from v_End to v_Start-1 */
       /**-------------------------------------------------------------------*/
       /*$$$$$$$$$$$$$$$$                            $$$$$$$$$$$$$$$$$$$$$$$$*/
       /*              BFS                          DS                       */
       /**-------------------------------------------------------------------*/


        if( vp_length > (vg_HALGPS_HciRxBuffer->v_DataStart - vg_HALGPS_HciRxBuffer->v_BufferFreeStart ) )
        {
            vp_length = vg_HALGPS_HciRxBuffer->v_DataStart - vg_HALGPS_HciRxBuffer->v_BufferFreeStart;
            MC_HALGPS_HCI_TRACE_STRING (("HALGPS9_04HciRxBuffUpdate : No Space in the buffer"));
        }
        memcpy( vg_HALGPS_HciRxBuffer->v_Buffer + vg_HALGPS_HciRxBuffer->v_BufferFreeStart ,
                pp_rxBuffer ,
                vp_length
                );

        /* Indicate the new end */
        vg_HALGPS_HciRxBuffer->v_BufferFreeStart += vp_length;
       /**-------------------------------------------------------------------*/
       /*$$$$$$$$$$$$$$$$>>>>>>>>>>                  $$$$$$$$$$$$$$$$$$$$$$$$*/
       /*                        BFS                 DS                      */
       /**-------------------------------------------------------------------*/

    }
    else
    {
        /* Two cases
        1. If data length is not sufficient to cause a wrap around. Direct memcpy and update
        2. The data is larger than expected
        3. If a wrap around is detected
        */
       /**-------------------------------------------------------------------*/
       /*               $$$$$$$$$$$$$$$$$$$$$$                               */
       /*               DS                   BFS                             */
       /**-------------------------------------------------------------------*/

        if( vp_length <= (HALGPS_HCI_MAX_BUF_READ_LEN - vg_HALGPS_HciRxBuffer->v_BufferFreeStart) )
        {
            memcpy( vg_HALGPS_HciRxBuffer->v_Buffer + vg_HALGPS_HciRxBuffer->v_BufferFreeStart ,
                pp_rxBuffer ,
                vp_length
                );

            /* Indicate the new end */
            vg_HALGPS_HciRxBuffer->v_BufferFreeStart += vp_length;

            if( vg_HALGPS_HciRxBuffer->v_BufferFreeStart == HALGPS_HCI_MAX_BUF_READ_LEN )
            {
                vg_HALGPS_HciRxBuffer->v_BufferFreeStart = 0; /* Wrap Around! */
            }

       /**-------------------------------------------------------------------*/
       /*                $$$$$$$$$$$$$$$$$$$$$$>>>>>>>>>>>>>>>>              */
       /*               DS                                   BFS             */
       /**-------------------------------------------------------------------*/

        }
        else if( vp_length <= (HALGPS_HCI_MAX_BUF_READ_LEN - vg_HALGPS_HciRxBuffer->v_BufferFreeStart + vg_HALGPS_HciRxBuffer->v_DataStart) )
        {
            uint32_t vl_LengthFirstPart;

            vl_LengthFirstPart = HALGPS_HCI_MAX_BUF_READ_LEN - vg_HALGPS_HciRxBuffer->v_BufferFreeStart;

            memcpy( vg_HALGPS_HciRxBuffer->v_Buffer + vg_HALGPS_HciRxBuffer->v_BufferFreeStart ,
                    pp_rxBuffer,
                    vl_LengthFirstPart ); /* Copy the first part into remaining space in array */

            memcpy( vg_HALGPS_HciRxBuffer->v_Buffer , /* Copy from the beginning now */
                    pp_rxBuffer + vl_LengthFirstPart ,
                    vp_length - vl_LengthFirstPart ); /* Copy the remaining data */

            vg_HALGPS_HciRxBuffer->v_BufferFreeStart = vp_length - vl_LengthFirstPart;
       /**-------------------------------------------------------------------*/
       /* >>>>>>>>       $$$$$$$$$$$$$$$$$$$$$$>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
       /*        BFS     DS                                                  */
       /**-------------------------------------------------------------------*/


        }
        else
        {
            MC_HALGPS_HCI_TRACE_STRING (("HALGPS9_04HciRxBuffUpdate : No Space in the buffer"));
        }

    }

    MC_HALGPS_HCI_TRACE_STRING_PARAM (("HALGPS9_04HciRxBuffUpdate : v_DataStart <%lu>  v_BufferFreeStart <%lu>", vg_HALGPS_HciRxBuffer->v_DataStart , vg_HALGPS_HciRxBuffer->v_BufferFreeStart));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/******************************************************************************/
/**                           HALGPS9_05HciRxBuffRcv                                                                **/
/**  retrieves data from the HCI buffer into the provided byte array                                    **/
/**  param pp_rxBuffer buffer which contains data to be updated                                        **/
/**param vp_length data length                                                                                       **/
/*****************************************************************************/

uint32_t HALGPS9_05HciRxBuffRcv(uint8_t * pp_rxBuffer, uint32_t vp_length)
{
    /* Two Cases
    1. No Wrap around in data
    2. Wrap around detected
    3. No Data;
    */

    uint32_t vl_BytesReturned = 0;
    if (vg_HALGPS_HciRxBuffer== NULL) return 0;

    MC_HALGPS_HCI_TRACE_STRING_PARAM (("HALGPS9_05HciRxBuffRcv: Length <%lu>", vp_length));

    if( vg_HALGPS_HciRxBuffer->v_DataStart < vg_HALGPS_HciRxBuffer->v_BufferFreeStart )
    {


       /**-------------------------------------------------------------------*/
       /*               $$$$$$$$$$$$$$$$$$$$$$                               */
       /*               DS                   BFS                             */
       /**-------------------------------------------------------------------*/

        vl_BytesReturned = vg_HALGPS_HciRxBuffer->v_BufferFreeStart - vg_HALGPS_HciRxBuffer->v_DataStart;

        if( vl_BytesReturned > vp_length )
        {
            vl_BytesReturned = vp_length; /* We only need as many as requested */
        }

        memcpy( pp_rxBuffer ,
                vg_HALGPS_HciRxBuffer->v_Buffer + vg_HALGPS_HciRxBuffer->v_DataStart ,
                vl_BytesReturned );

        vg_HALGPS_HciRxBuffer->v_DataStart += vl_BytesReturned;
       /**-------------------------------------------------------------------*/
       /*                         $$$$$$$$$$$$$                               */
       /*                        DS          BFS                             */
       /**-------------------------------------------------------------------*/

    }
    else if( vg_HALGPS_HciRxBuffer->v_DataStart > vg_HALGPS_HciRxBuffer->v_BufferFreeStart )
    {

       /**-------------------------------------------------------------------*/
       /*$$$$$$$$$$$$$$$$                            $$$$$$$$$$$$$$$$$$$$$$$$*/
       /*              BFS                          DS                       */
       /**-------------------------------------------------------------------*/

        vl_BytesReturned = HALGPS_HCI_MAX_BUF_READ_LEN - vg_HALGPS_HciRxBuffer->v_DataStart + vg_HALGPS_HciRxBuffer->v_BufferFreeStart;

        if( vl_BytesReturned > vp_length )
        {
            vl_BytesReturned = vp_length; /* We only need as many as requested */
        }

        /* Two cases
        1. The bytes needed are in one section
        2. The bytes needed are split
        */

        if( vl_BytesReturned <= HALGPS_HCI_MAX_BUF_READ_LEN - vg_HALGPS_HciRxBuffer->v_DataStart )
        {
            memcpy( pp_rxBuffer ,
                    vg_HALGPS_HciRxBuffer->v_Buffer + vg_HALGPS_HciRxBuffer->v_DataStart ,
                    vl_BytesReturned );

            vg_HALGPS_HciRxBuffer->v_DataStart += vl_BytesReturned;

            if( vg_HALGPS_HciRxBuffer->v_DataStart == HALGPS_HCI_MAX_BUF_READ_LEN )
            {
                vg_HALGPS_HciRxBuffer->v_DataStart = 0; /* Wrap Around! */
            }

       /**-------------------------------------------------------------------*/
       /*$$$$$$$$$$$$$$$$                                             $$$$$$$*/
       /*              BFS                                            DS     */
       /**-------------------------------------------------------------------*/



        }
        else
        {
            uint32_t vl_FirstPartLength;

            vl_FirstPartLength = HALGPS_HCI_MAX_BUF_READ_LEN - vg_HALGPS_HciRxBuffer->v_DataStart;

            memcpy( pp_rxBuffer ,
                    vg_HALGPS_HciRxBuffer->v_Buffer + vg_HALGPS_HciRxBuffer->v_DataStart ,
                    vl_FirstPartLength);

            memcpy( pp_rxBuffer + vl_FirstPartLength ,
                    vg_HALGPS_HciRxBuffer->v_Buffer ,
                    vl_BytesReturned - vl_FirstPartLength);

            vg_HALGPS_HciRxBuffer->v_DataStart = vl_BytesReturned - vl_FirstPartLength;
       /**-------------------------------------------------------------------*/
       /*       $$$$$$$$                                                     */
       /*      DS        BFS                                                 */
       /**-------------------------------------------------------------------*/


        }
    }
    else
    {
        /*DataStart and BufferFreeStart are same*/
        vl_BytesReturned = 0;
    }

    if( vg_HALGPS_HciRxBuffer->v_DataStart == vg_HALGPS_HciRxBuffer->v_BufferFreeStart )
    {
        vg_HALGPS_HciRxBuffer->v_DataStart = 0;
        vg_HALGPS_HciRxBuffer->v_BufferFreeStart = 0;
    }

    MC_HALGPS_HCI_TRACE_STRING_PARAM (("HALGPS9_05HciRxBuffRcv:v_DataStart <%lu>  v_BufferFreeStart <%lu>", vg_HALGPS_HciRxBuffer->v_DataStart , vg_HALGPS_HciRxBuffer->v_BufferFreeStart));

    return vl_BytesReturned;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6

uint32_t HALGPS9_06HciSend(uint8_t * pp_txBuffer, uint32_t vp_length)
{
    uint32_t bytes_written = 0;

    MC_HALGPS_HCI_TRACE_STRING_PARAM(("HALGPS9_06HciSend: Requested to write %d bytes\n", vp_length));

    if( vp_length > HALGPS_HCI_MAX_WRITE_LEN )
    {
        vp_length = HALGPS_HCI_MAX_WRITE_LEN;
    }

    /* If we are waiting for an ACK, issue a read to check if ACK is available */
    // HALGPS9_08HciReadAndProcess();

    /* Wait for Ack for any packets previously sent */
    if(HALGPS9_02WaitForTxAck(HALGPS_HCI_TX_ACK_MAX_RETRY))

    bytes_written = HALGPS9_11HciSendPacket(pp_txBuffer,vp_length,HALGPS_HCI_CMD_SEND_DATA);

    return( bytes_written );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7

uint32_t HALGPS9_07HciRcv(uint8_t * pp_rxBuffer, uint32_t vp_length)
{
    HALGPS9_08HciReadAndProcess();

    return HALGPS9_05HciRxBuffRcv(pp_rxBuffer,vp_length);
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8

void HALGPS9_08HciReadAndProcess(void)
{
    int tempBytesRead = 0;
    uint8_t  vl_ReadDone = FALSE;

    /* We read into the buffer after any previous data */
    if(vg_HALGPS_HciRawReadBuffer == NULL) return;

    do
    {
        tempBytesRead = HALGPS10_01HciReadRaw( vg_HALGPS_HciRawReadBuffer + vg_HALGPS_HciRawReadLen, HALGPS_HCI_MAX_READ_LEN+3 );

        /* Set previous data length to zero. If any data remains, we will set this value again */
        vg_HALGPS_HciRawReadLen = 0;

        if(tempBytesRead > 0)
        {
            int start = 0;

            while ( (start < tempBytesRead ) && ( !vl_ReadDone ) )
            {
                int opcode = 0;
                int length = 0;
                opcode = vg_HALGPS_HciRawReadBuffer[start++];
                length = vg_HALGPS_HciRawReadBuffer[start++];
                length += 256*vg_HALGPS_HciRawReadBuffer[start++];

                MC_HALGPS_HCI_TRACE_STRING_PARAM(("HALGPS9_08HciReadAndProcess : Opcode = %x , Length = %d", opcode, length ));

                if( length > ( tempBytesRead - start ) )
                {
                    //length = end - 3;
                    // We have an incomplete packet, so we ignore the rest of data
                    // and hope that any subsequent read will return only complete data.
                    vl_ReadDone = TRUE;
                    /* We move back the header length to indicate that it has not been processed */
                    start  -= HALGPS_HCI_HEADER_LEN;

                    vg_HALGPS_HciRawReadLen = tempBytesRead - start;
                    memmove( vg_HALGPS_HciRawReadBuffer + start , vg_HALGPS_HciRawReadBuffer , vg_HALGPS_HciRawReadLen );

                    MC_HALGPS_TRACE_INF("HALGPS9_08HciReadAndProcess: Broken data - Start <%d> , End <%d> , Length <%d>", start , tempBytesRead , length );

                }

                if (opcode == HALGPS_HCI_CMD_RECV_DATA)
                {
                    MC_HALGPS_TRACE_INF("HALGPS9_08HciReadAndProcess : Data :length = %d, start = %d", length,start);

                    HALGPS9_04HciRxBuffUpdate(vg_HALGPS_HciRawReadBuffer+start,length);
                }
                else if( opcode == HALGPS_HCI_CMD_ACK )
                {
                    /*Indication to request more data, value has been set more than the
                    size of internal read buffer set to 1027*/

                    if( vg_HALGPS_HciState == K_HCI_STATE_WAITING_FOR_TXACK )
                    {
                        /* We have received an ACK while waiting for it */
                        uint8_t vl_AckStatus;

                        vl_AckStatus = *(vg_HALGPS_HciRawReadBuffer+start);

                        MC_HALGPS_TRACE_INF("HALGPS9_08HciReadAndProcess : ACK status is %u" , vl_AckStatus);

                        if( vl_AckStatus > 0 )
                        {
                            vg_HALGPS_HciState = K_HCI_STATE_CONNECTED;
                            vg_HALGPS_Max_Num_Missed_Acks =0;
                        }
                        else
                        {
                            MC_HALGPS_TRACE_INF("HALGPS9_08HciReadAndProcess :ERR ACK status is %u" , vl_AckStatus);
                        }
                    }
                    else
                    {
                        MC_HALGPS_TRACE_INF("HALGPS9_08HciReadAndProcess : ERR Unexpected ACK received");
                    }

                }

                else if( opcode == HALGPS_HCI_CMD_RECV_SS_EVENT )
                {
                    uint8_t vl_SsAckStatus;

                    vl_SsAckStatus = *(vg_HALGPS_HciRawReadBuffer+start);

                    switch( vg_HALGPS_HciGpsState )
                    {
                        case K_HCI_GPS_DISABLE_REQ :
                            MC_HALGPS_TRACE_INF("HALGPS9_08HciReadAndProcess : K_HCI_GPS_DISABLE_REQ : Response %u" , *(vg_HALGPS_HciRawReadBuffer+start) );
                            vg_HALGPS_HciGpsState = K_HCI_GPS_DISABLED;
                            break;
                        case K_HCI_GPS_ENABLE_REQ :
                            MC_HALGPS_TRACE_INF("HALGPS9_08HciReadAndProcess : K_HCI_GPS_ENABLE_REQ : Response %u" , *(vg_HALGPS_HciRawReadBuffer+start) );
                            vg_HALGPS_HciGpsState = K_HCI_GPS_ENABLED;
                            break;
                        default:
                            MC_HALGPS_TRACE_INF("HALGPS9_08HciReadAndProcess : ERR Unexpected SS CMD ACK. Status is %u" , vl_SsAckStatus);
                    }
                }
                start += length;
            }
        }
    } while( tempBytesRead > 0 );

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9

uint8_t HALGPS9_09HciGpsEnable()
{
    uint32_t vl_BytesWritten = 0;
    uint8_t vl_RetVal = FALSE;
    uint8_t vl_Command;

    MC_HALGPS_TRACE_INF( "HALGPS9_09HciGpsEnable: Entry\n");

    if( vg_HALGPS_HciGpsState == K_HCI_GPS_DISABLED )
    {
        vl_Command = HALGPS_HCI_CMD_SS_GPS_ENABLE;

        MC_HALGPS_TRACE_INF( "HALGPS9_09HciGpsEnable: Attempting to send CMD <%u>\n" , vl_Command );

        /* Wait for Ack for any packets previously sent */
        if(HALGPS9_02WaitForTxAck(HALGPS_HCI_TX_ACK_MAX_RETRY_2))

        vl_BytesWritten  = HALGPS9_11HciSendPacket(&vl_Command,1,HALGPS_HCI_CMD_SEND_SS_EVENT);

        if (vl_BytesWritten == 0)
        {
           MC_HALGPS_TRACE_INF ("HALGPS9_09HciGpsEnable : Error Sending Command <%u>" , vl_Command );
           vl_RetVal =  FALSE;
        }
        else
        {
            MC_HALGPS_TRACE_INF("HALGPS9_09HciGpsEnable: Waiting for SS CMD RESP");

            vg_HALGPS_HciGpsState = K_HCI_GPS_ENABLE_REQ;

            if(HALGPS9_02WaitForTxAck(HALGPS_HCI_TX_ACK_MAX_RETRY_2))
            {

            /* We now wait for the correct response from chip */
            /* Ideally we need to abort after some time and indicate failure */
            while( vg_HALGPS_HciGpsState != K_HCI_GPS_ENABLED )
            {
#ifdef __RTK_E__
#ifdef UTS_CPR
                MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,HALGPS_HCI_POLL_TIME_MS);
#else
                MC_OST_START_TASK_TIMER(HALGPS_HCI_POLL_TIME_MS);
#endif
#else
                OSA_Sleep( HALGPS_HCI_POLL_TIME_MS );
#endif
                HALGPS9_08HciReadAndProcess();
            }

            vl_RetVal = TRUE;
        }
    }
    }
    else
    {
        MC_HALGPS_TRACE_INF( "HALGPS9_09HciGpsEnable : Attempting to send GPS ENABLE CMD in incorrect state-<%d>\n" , vg_HALGPS_HciGpsState);
        vl_RetVal = FALSE;
    }

    MC_HALGPS_TRACE_INF( "HALGPS9_09HciGpsEnable: Exit\n");

    return vl_RetVal;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10

uint8_t HALGPS9_10HciGpsDisable()
{
    uint32_t vl_BytesWritten = 0;
    uint8_t vl_RetVal = FALSE;
    uint8_t vl_Command;

    MC_HALGPS_TRACE_INF( "HALGPS9_10HciGpsDisable: Entry\n");

    if( vg_HALGPS_HciGpsState == K_HCI_GPS_ENABLED)
    {
        vl_Command = HALGPS_HCI_CMD_SS_GPS_DISABLE;

        MC_HALGPS_TRACE_INF( "HALGPS9_10HciGpsDisable: Attempting to send CMD <%u>\n" , vl_Command );

        /* Wait for Ack for any packets previously sent */
        if(HALGPS9_02WaitForTxAck(HALGPS_HCI_TX_ACK_MAX_RETRY_2))

        vl_BytesWritten  = HALGPS9_11HciSendPacket(&vl_Command,1,HALGPS_HCI_CMD_SEND_SS_EVENT);

        if (vl_BytesWritten == 0)
        {
           MC_HALGPS_TRACE_INF ("HALGPS9_10HciGpsDisable : Error Sending Command <%u>" , vl_Command );
           vl_RetVal =  FALSE;
        }
        else
        {
            MC_HALGPS_TRACE_INF("HALGPS9_10HciGpsDisable: Waiting for SS CMD RESP");

            vg_HALGPS_HciGpsState = K_HCI_GPS_DISABLE_REQ;

            /* Ideally we need to abort after some time and indicate failure */
            if(HALGPS9_02WaitForTxAck(HALGPS_HCI_TX_ACK_MAX_RETRY_2))
            {

            /* We now wait for the correct response from chip */
            /* Ideally we need to abort after some time and indicate failure */
            while( vg_HALGPS_HciGpsState != K_HCI_GPS_DISABLED )
            {
#ifdef __RTK_E__
#ifdef UTS_CPR
                MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,HALGPS_HCI_POLL_TIME_MS);
#else
                MC_OST_START_TASK_TIMER(HALGPS_HCI_POLL_TIME_MS);
#endif
#else
                OSA_Sleep( HALGPS_HCI_POLL_TIME_MS );
#endif
                HALGPS9_08HciReadAndProcess();
            }
            vl_RetVal = TRUE;
        }
            vl_RetVal = TRUE;
        }
    }
    else
    {
        MC_HALGPS_TRACE_INF( "HALGPS9_10HciGpsDisable : Attempting to send GPS ENABLE CMD in incorrect state-<%d>\n" , vg_HALGPS_HciGpsState);
        vl_RetVal= FALSE;
    }

    MC_HALGPS_TRACE_INF( "HALGPS9_10HciGpsDisable: Exit\n");

    return vl_RetVal;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11

uint32_t HALGPS9_11HciSendPacket(uint8_t * pp_txBuffer, uint32_t  vp_length, uint32_t vp_type)
{
    uint8_t preamble[HALGPS_HCI_HEADER_LEN];
    uint8_t *data =NULL;
    int32_t byteswritten = 0;

    MC_HALGPS_HCI_TRACE_STRING(("BEGIN: HALGPS9_11HciSendPacket"));

    if( pp_txBuffer == NULL || vp_length == 0 )
    {
        return 0;
    }

    if( vg_HALGPS_HciState == K_HCI_STATE_NOT_CONNECTED )
    {
        MC_HALGPS_TRACE_INF("HALGPS9_11HciSendPacket: Could not write as K_HCI_STATE_NOT_CONNECTED\n");
        return 0;
    }

    if( vg_HALGPS_HciState == K_HCI_STATE_WAITING_FOR_TXACK )
    {
        MC_HALGPS_HCI_TRACE_STRING_PARAM(("HALGPS9_11HciSendPacket: Could not write as WAITING FOR ACK\n"));
        return 0;
    }

    switch (vp_type)
    {
        case HALGPS_HCI_CMD_SEND_DATA:
        case HALGPS_HCI_CMD_SEND_SS_EVENT:
            preamble[0]= vp_type;
            preamble[1] = (uint8_t) (vp_length%256);
            preamble[2] = (uint8_t) (vp_length/256);
            MC_HALGPS_HCI_TRACE_STRING_PARAM(("HALGPS9_11HciSendPacket : Preamble %u %u" , (uint8_t)preamble[2] , (uint8_t)preamble[1] ));

            data = (uint8_t*) MC_RTK_GET_MEMORY((HALGPS_HCI_HEADER_LEN+ vp_length) * sizeof(uint8_t));

            if( NULL == data )
            {
                MC_HALGPS_TRACE_INF("HALGPS9_11HciSendPacket : ERR Could not allocate memory!");
                break;
            }

            memcpy ( data, preamble, HALGPS_HCI_HEADER_LEN);
            memcpy ( data+HALGPS_HCI_HEADER_LEN, pp_txBuffer,vp_length);


            MC_HALGPS_HCI_TRACE_STRING_PARAM(("HALGPS9_11HciSendPacket : Data sent : Actual Data:%d", (HALGPS_HCI_HEADER_LEN+vp_length)));


            byteswritten = HALGPS10_02HciWriteRaw(data, HALGPS_HCI_HEADER_LEN + vp_length);

            if( (uint32_t)(byteswritten) == HALGPS_HCI_HEADER_LEN + vp_length )
            {
                vg_HALGPS_HciState = K_HCI_STATE_WAITING_FOR_TXACK;
            }

            if( (uint32_t)(byteswritten) != (HALGPS_HCI_HEADER_LEN + vp_length) )
            {
                MC_HALGPS_TRACE_INF("HALGPS9_11HciSendPacket:ERR Not all data was sent: Sent:%d, Actual Data %d", byteswritten, (HALGPS_HCI_HEADER_LEN+vp_length));
            }
            if( byteswritten < HALGPS_HCI_HEADER_LEN )
            {
                byteswritten = HALGPS_HCI_HEADER_LEN;
            }

            MC_RTK_FREE_MEMORY(data);


            break;

        default:
            MC_HALGPS_TRACE_INF("HALGPS9_11HciSendPacket:ERR Unknown opcode : %d", vp_type);
    }
    MC_HALGPS_HCI_TRACE_STRING_PARAM(("HALGPS9_11HciSendPacket: Sent %d bytes ofData",byteswritten));
    MC_HALGPS_HCI_TRACE_STRING(("END: HALGPS9_11HciSendPacket"));

    return (byteswritten-HALGPS_HCI_HEADER_LEN);

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
/*******************************************************************************/
/* HALGPS9_12EnterDSM : Enter DSM Mode                    */
/*******************************************************************************/
void HALGPS9_12EnterDSM(void)
{
    MC_HALGPS_HCI_TRACE_STRING(("HALGPS9_12EnterDSM : Check for any ACKs is waiting"));

    HALGPS9_02WaitForTxAck(HALGPS_HCI_TX_ACK_MAX_RETRY_2);

    //HALGPS10_04EnterDSM();
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/*******************************************************************************/
/* HALGPS9_13ExitDSM : Exit DSM Mode                    */
/*******************************************************************************/
void HALGPS9_13ExitDSM(void)
{
    //HALGPS10_05ExitDSM();
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/*******************************************************************************/
/* HALGPS9_14HciRxBuffDeInit :                     */
/*******************************************************************************/

void HALGPS9_14HciRxBuffDeInit(void)
{
  if (vg_HALGPS_HciRxBuffer != NULL)
    {
      MC_RTK_FREE_MEMORY(vg_HALGPS_HciRxBuffer);
      vg_HALGPS_HciRxBuffer = NULL;
    }

  if (vg_HALGPS_HciRawReadBuffer != NULL)
    {
       MC_RTK_FREE_MEMORY(vg_HALGPS_HciRawReadBuffer);
       vg_HALGPS_HciRawReadBuffer = NULL;
    }

  return;
}


#endif /*GPS_OVER_HCI*/
#undef __HALGPS9HCI_C__

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#define __HALGPS6SPILINUX_C__
#if defined( GPS_OVER_SPI ) && defined( AGPS_LINUX_FTR )
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

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>

int GN_Port_Setup(
   CH *port,                     // i  -  Port Name
   U4  baud,                     // i  -  Port Baudrate
   CH *usage )                   // i  -  Port Usage description
{
   // Return Argument Definition
   int            hPort;         // Handle to the opened Comm port
   // Local Data Definitions
   int            err;           // Error code
   struct stat    status;        // Returns the status of the baseband serial port.
   struct termios curr_term;     // The current serial port configuration descriptor.

   MC_HALGPS_TRACE_INF( "+GN_Port_Setup:\r\n" );

   hPort = -1;

   // Check for a valid PC COM Port Name
   if ( port[0] == '\0' )
   {
      MC_HALGPS_TRACE_INF( " GN_Port_Setup:  ERROR: %s Port %s  Invalid Port Name\r\n\r\n",
              (char*)usage, port );
      return( -1 );
   }

   // First check the port exists.
   // This avoids thread cancellation if the port doesn't exist.
   hPort = -1;
   if ( ( err = stat( port, &status ) ) == -1 )
   {
      MC_HALGPS_TRACE_INF( " GN_Port_Setup: stat(%s,*) = %d,  errno %d\r\n", port, err, errno );
      return( -1 );
   }

   // Open the serial port.
   hPort = open( port, (O_RDWR | O_NOCTTY | O_NONBLOCK) );
   if ( hPort <= 0 )
   {
      MC_HALGPS_TRACE_INF( " GN_Port_Setup: open(%s,*) = %d, errno %d\r\n", port, hPort, errno );
      return( -1 );
   }

   return( hPort );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/*******************************************************************************/
/* HALGPS6_00SpiInit : initialises kid interface for SPI                       */
/*******************************************************************************/
bool HALGPS6_00SpiInit(void)
{
    bool returnVal = FALSE;
    MC_HALGPS_TRACE_INF("BEGIN : HALGPS6_00SpiInit");

    gn_iPort_GNB = GN_Port_Setup( gn_Port_GNB, 115200, "GN_GNB" );
    MC_HALGPS_TRACE_INF("HALGPS6_00SpiInit:GN_Port_Setup rc=%d\n",gn_iPort_GNB);

    if ( gn_iPort_GNB <= 0 )
    {
        MC_HALGPS_TRACE_INF( " HALGPS6_00SpiInit: EXIT: Failed to Open GN GPS Baseband Port <%s> = %d, errno %d\n",
            gn_Port_GNB, gn_iPort_GNB, errno );
    }
    else
    {
        MC_HALGPS_TRACE_INF( " HALGPS6_00SpiInit: Open GN GPS Baseband Port <%s>\n",
            gn_Port_GNB );
       returnVal = TRUE;
    }

    MC_HALGPS_TRACE_INF("END : HALGPS6_00SpiInit");
    return returnVal;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/******************************************************************************/
/*                            HALGPS6_01SpiRcv                                */
/******************************************************************************/
uint32_t HALGPS6_01SpiRcv(uint8_t * pp_rxBuffer, uint32_t  vp_length)
{
#define STEP_LEN 64
   CH inBuf[STEP_LEN];
   unsigned int bytes_read = 0;                  // Number of bytes actually read
   int step_len;
   int step_read;
   int done = 0;


   // Interface directly with the Input Driver.

    while(!done)
    {
        step_len = vp_length-bytes_read;
        if(step_len > STEP_LEN)
        {
            step_len = STEP_LEN;
        }

        step_read = read( gn_iPort_GNB, inBuf, step_len );
        if ( step_read <= 0 )
        {
            done = 1;
        }
        else
        {
            int i;
            char c;
        for(i = 0; i<step_read; i++)
        {
            if((c = inBuf[i]) != 0)
            {
                pp_rxBuffer[bytes_read++] = c;
            }
        }

        if(!c)
        {
            /* last input was 0. Chip transmission was over */
            done = 1;
        }

        if(bytes_read == vp_length)
        {
            /* buffer is full */
            done = 1;
        }

        }
    } /* end while */

   return( (U2)bytes_read );
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/******************************************************************************/
/**                           HALGPS6_02SpiSend                              **/
/******************************************************************************/
uint32_t HALGPS6_02SpiSend(uint8_t * pp_txBuffer, uint32_t  vp_length)
{
     int bytes_written;               // Number of bytes written
     /* +LMSqc13768 */     
     uint8_t  padding_bytes[HALGPS_SPI_PADDING_BUF_LEN];
     /* +LMSqc13768 */
     // Interface directly with the UART Output Driver.

     // write() returns -1 on an error (eg no data written).
    bytes_written = write( gn_iPort_GNB, pp_txBuffer, vp_length );
    if ( bytes_written < 0 )
    {
        int myErrno=errno;

        bytes_written = 0;
    }

    /* +LMSqc13768 */
    // Have to pad with HALGPS_SPI_PADDING_BUF_LEN zero bytes
    memset( padding_bytes ,0 , HALGPS_SPI_PADDING_BUF_LEN );
    write( gn_iPort_GNB, padding_bytes, HALGPS_SPI_PADDING_BUF_LEN );
    /* +LMSqc13768 */

    return( (uint32_t)bytes_written );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/******************************************************************************/
/**                           HALGPS6_04ResetSpi                             **/
/******************************************************************************/
void HALGPS6_04ResetSpi()
{
    if( gn_iPort_GNB > 0 )
    {
        MC_HALGPS_TRACE_INF("Closing SPI with handle %d" , gn_iPort_GNB );
        close( gn_iPort_GNB );
        gn_iPort_GNB = 0;

        HALGPS6_00SpiInit();
    }
}

#endif /*GPS_OVER_SPI || AGPS_LINUX_FTR*/
#undef __HALGPS6SPILINUX_C__

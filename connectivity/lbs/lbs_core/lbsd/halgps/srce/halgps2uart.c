/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define __HALGPS2UART_C__

#if defined (GPS_OVER_UART) 
/**
* \file halgps2uart.c
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains all function that managed the UART connection between sysol and GPS module.\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08</TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_HALGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     2

#include "halgpsuart.h"

#if defined( AGPS_LINUX_FTR )

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

   MC_HALGPS_TRACE_INF( "+GN_Port_Setup: port = %s\r\n" , port);

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
   hPort = open( port, (O_RDWR | O_NOCTTY | O_NONBLOCK ) );

   if ( hPort <= 0 )
   {
      MC_HALGPS_TRACE_INF( " GN_Port_Setup: open(%s,*) = %d, errno %d\r\n", port, hPort, errno );
      return( -1 );
   }

   switch( baud )
   {
        case 4800:
           baud = B4800;
           break;
        case 9600:
           baud = B9600;
           break;
        case 19200:
           baud = B19200;
           break;
        case 38400:
           baud = B38400;
           break;
        case 57600:
           baud = B57600;
           break;
        case 115200:
           baud = B115200;
           break;
        case 230400:
           baud = B230400;
           break;
        case 921600:
            baud = B921600;
            break;
        default:
           close( hPort );
           MC_HALGPS_TRACE_INF( " GN_Port_Setup: Received Invalid baudrate from Setup = %lu", baud );
           return( -1 );
           break;
  }



   // Get the current serial port terminal state.
   if ( ( err = tcgetattr( hPort, &curr_term ) ) != 0 )
   {
      MC_HALGPS_TRACE_INF( " GN_Port_Setup: tcgetattr(%d) = %d,  errno %d\r\n", hPort, err, errno );
      close( hPort );
      return( -1 );
   }



   // Set the terminal state to local (ie a local serial port, not a modem port),
   // raw (ie binary mode).
   #ifdef __USE_BSD
      curr_term.c_cflag  =  curr_term.c_cflag | CLOCAL;
      cfmakeraw( &curr_term );
   #else
      // Apparently the following does the same as cfmakeraw
      curr_term.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
      curr_term.c_oflag &= ~OPOST;
      curr_term.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
      curr_term.c_cflag &= ~(CSIZE|PARENB);
      curr_term.c_cflag |= CS8;
   #endif

   // Disable modem hang-up on port close.
   curr_term.c_cflag      =  curr_term.c_cflag & (~HUPCL);

   // Set way that the read() driver behaves.
// curr_term.c_cc[VMIN]   =  1;                 // read() returns when 1 byte available
   curr_term.c_cc[VMIN]   =  0;                 // read() returns immediately
   curr_term.c_cc[VTIME]  =  0;                 // read() Time-Out in 1/10th sec units

   // Set the requested baud rate in the terminal state descriptor.

   // Set the input baud rates in the termios.
   if ( cfsetispeed( &curr_term, baud ) )
   {
      close( hPort );
      return( -1 );
   }

   // Set the output baud rates in the termios.
   if ( cfsetospeed( &curr_term, baud ) )
   {
      close( hPort );
      return( -1 );
   }

   // Set the Parity state to NO PARITY.
   curr_term.c_cflag  =  curr_term.c_cflag  &  (~( PARENB | PARODD ));

   // Set the Flow control state to NONE.
   curr_term.c_cflag  =  curr_term.c_cflag  &  (~CRTSCTS);
   curr_term.c_iflag  =  curr_term.c_iflag  &  (~( IXON | IXOFF | IXANY ));

   // Set the number of data bits to 8.
   curr_term.c_cflag  =  curr_term.c_cflag  &  (~CSIZE);
   curr_term.c_cflag  =  curr_term.c_cflag  |  CS8;


#ifndef X86_64_UBUNTU_FTR
   // Set 2 stop bits.
   curr_term.c_cflag  =  curr_term.c_cflag  |  ( CSTOPB | CREAD );

#else
   // Set 1 stop bit
    curr_term.c_cflag  =  curr_term.c_cflag  |  CREAD ;
#endif

//   MC_HALGPS_TRACE_INF( " curr_term   i = %08X   o = %08X   l = %08X   c = %08X   cc = %08X \r\n",
//           curr_term.c_iflag, curr_term.c_oflag, curr_term.c_lflag, curr_term.c_cflag, curr_term.c_cc );

   // Now set the serial port configuration and flush the port.
   if ( tcsetattr( hPort, TCSAFLUSH, &curr_term ) != 0 )
   {
      close( hPort );
      return( -1 );
   }


/* +LMSqc16476 */
#ifdef AGPS_PID0001_FTR
/* -LMSqc16476 */
   /* AGPS_PID0001_FTR board currently uses the GNS7560 connected via USB.*/
   /* @todo : Should we move this out of here and into HALGPS0_06Start ? */
   gn_ToggleRts = TRUE;
   // On some later versions of the USB Dongle with GNS7560, the RTS line must be
   // toggled to power-up the baseband.

   if ( gn_ToggleRts )
   {
      #include <sys/ioctl.h>
      // #include <sys/io.h>
      int serstat;

      //!!! Enable I/O port controls
      //iopl(3);

      // Read the RTS line status
      if ( ioctl( hPort, TIOCMGET, &serstat ) == -1 )
      {
        perror( "ioctl - TIOCMGET1" );
        exit( 1 );
      }
      if ( (serstat & TIOCM_RTS) != 0 )
      {
         MC_HALGPS_TRACE_INF( "***RTS line is set, serstat = %d\n", serstat );
      }
      else
      {
         MC_HALGPS_TRACE_INF( "***RTS line is reset, serstat = %d\n", serstat );
      }

      // Clear RTS bit, this will power-up the baseband.
      serstat &= ~TIOCM_RTS;
      //sercmd = serstat & ~TIOCM_RTS;
      //ioctl( hPort, TIOCMBIC, &serstat );                       //0x5417
      //if ( ioctl( hPort, TIOCMBIC, (char*)&serstat ) == -1 )    //0x5417
      if ( ioctl( hPort, TIOCMSET, &serstat ) == -1 )
      {
        perror( "ioctl - TIOCMSET2" );
        exit( 1 );
      }

      // Read status the RTS pin status.
      if ( ioctl( hPort, TIOCMGET, &serstat ) == -1 )
      {
        perror("ioctl-TIOCMGET3");
        exit(1);
      }
      if ( (serstat & TIOCM_RTS) != 0 )
      {
         MC_HALGPS_TRACE_INF( "***RTS line is set, serstat = %d\n", serstat );
      }
      else
      {
         MC_HALGPS_TRACE_INF( "***RTS line is reset, serstat = %d\n", serstat );
      }
   }  // if ( G_ToggleRts ) */
/* +LMSqc16476 */
#endif /* #ifdef AGPS_PID0001_FTR */
/* -LMSqc16476 */

   return( hPort );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0

/****************************************************/
/**                       HALGPS2_00UartInit       **/
/****************************************************/
bool HALGPS2_00UartInit(void)
{
    bool returnVal = FALSE;
    uint8_t  pp_rxBuffer[100];

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS2_00UartInit");

    gn_iPort_GNB = -1;
#ifdef X86_64_UBUNTU_FTR
    gn_iPort_GNB = GN_Port_Setup( gn_Port_GNB, 921600, "GN_GNB" );
#else
    gn_iPort_GNB = GN_Port_Setup( gn_Port_GNB, 115200, "GN_GNB" );
#endif
    if ( gn_iPort_GNB <= 0 )
    {
       MC_HALGPS_TRACE_INF( " GN_GPS_Task_Start: EXIT: Failed to Open GN GPS Baseband Port <%s> = %d, errno %d\n",
               gn_Port_GNB, gn_iPort_GNB, errno );
    }
    else
    {
       MC_HALGPS_TRACE_INF( " GN_GPS_Task_Start: Opened GN GPS Baseband Port <%s> , handle <%d>\n",
               gn_Port_GNB , gn_iPort_GNB);
       returnVal = TRUE;
#ifdef ENABLE_ARM_FPGA_STANDALONE
    MC_HALGPS_TRACE_INF("START : HALGPS2_00UartInit Emulated ARM Binary Download");
    HALGPS2_02UartSend((uint8_t *)vg_HALGPS_Arm2GpsMesg,sizeof(vg_HALGPS_Arm2GpsMesg));
    MC_HALGPS_TRACE_INF("START : HALGPS2_00UartInit Emulated ARM Binary Download");
#endif

       MC_HALGPS_TRACE_INF( "UART Read Bytes asked to read 100, Actual Bytes read %d\n", read( gn_iPort_GNB, pp_rxBuffer, 100 ));

    }

    MC_HALGPS_TRACE_INF("END : HALGPS2_00UartInit");

    return returnVal;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/****************************************************/
/**                       HALGPS2_01UartRcv        **/
/****************************************************/
uint32_t HALGPS2_01UartRcv(uint8_t * pp_rxBuffer, uint32_t  vp_lenght)
{
    int bytes_read;                  // Number of bytes actually read

    // Interface directly with the UART Input Driver.

    if( gn_iPort_GNB <= 0 )
        return 0;

    // read() returns -1 on an error (eg no data read).
    bytes_read = read( gn_iPort_GNB, pp_rxBuffer, vp_lenght );

    if( -1 == bytes_read )
    {
        MC_HALGPS_TRACE_INF( "UART handle %d : Read Bytes asked to read %d, errno %d",gn_iPort_GNB , vp_lenght, errno );
        bytes_read = 0;
    }
    else
    {
        MC_HALGPS_TRACE_INF( "UART handle %d : Read Bytes asked to read %d, read bytes %d",gn_iPort_GNB , vp_lenght, bytes_read);
    }
    return( (U2)bytes_read );
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/****************************************************/
/**                       HALGPS2_02UartSend       **/
/****************************************************/
uint32_t HALGPS2_02UartSend(uint8_t * pp_txBuffer, uint32_t  vp_lenght)
{
    int bytes_written;                 // Number of bytes written

    // Interface directly with the UART Output Driver.
    if( gn_iPort_GNB <= 0 )
    {
        MC_HALGPS_TRACE_INF("HALGPS2_02UartSend:  gn_iPort_GNB <=0 ");
        return 0;
    }

    // write() returns -1 on an error (eg no data written).
    bytes_written = (U2)write( gn_iPort_GNB, pp_txBuffer, vp_lenght );
    if ( bytes_written < 0 )
    {
        MC_HALGPS_TRACE_INF( "UART Write Bytes failed with error : %d to port handle %d\n", errno , gn_iPort_GNB);
        bytes_written = 0;
    }
    else
    {
        MC_HALGPS_TRACE_INF( "UART Write Bytes written %d to port %d\n", bytes_written , gn_iPort_GNB);
    }
    return( (U2)bytes_written );

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/****************************************************/
/**            HALGPS2_13ResetUart                 **/
/****************************************************/
void HALGPS2_13ResetUart()
{
    if( gn_iPort_GNB > 0 )
    {
        MC_HALGPS_TRACE_INF("Closing UART with handle %d" , gn_iPort_GNB );
        close( gn_iPort_GNB );
        gn_iPort_GNB = 0;
    }
}

#elif defined (WIN32)

bool HALGPS2_00UartInit(void){ return TRUE; }
uint32_t HALGPS2_01UartRcv(uint8_t * pp_rxBuffer, uint32_t  vp_lenght){ return 0; }
uint32_t HALGPS2_02UartSend(uint8_t * pp_txBuffer, uint32_t  vp_lenght){ return 0; }

#endif /* AGPS_LINUX_FTR */

#if defined( __RTK_E__ )

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0

/****************************************************/
/**                       HALGPS2_00UartInit       **/
/****************************************************/
bool HALGPS2_00UartInit(void)
{
    uint32_t vl_Index;
    t_kidError vl_ErrorStatus;
    t_HALGPSTrame *pl_HALGPSTrame=NULL;

#ifndef HDL_CLEANUP_INTHDL_CPR
    t_kidSetRoutingServ  vl_SetRoutingServ;
#endif

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS2_00UartInit");

    /*------------------------------------------------------------------------*/
    /* register of API UART                                                   */
    /*------------------------------------------------------------------------*/
    vl_ErrorStatus = kid3apiSetup(UART,(t_pfnInitFunc)UART2_00Init,UART2_01Reset,UART2_02Read  , UART2_03Write  , UART2_04Ioctl);

    if (KIDERR_OK != vl_ErrorStatus)
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        MC_HALGPS_TRACE_INF("END 1 : HALGPS2_00UartInit");
        return FALSE;
    }

    vl_ErrorStatus = kid3_00Init ( UART, HALGPS_UART_PORT_NUMBER, (void *)&vg_HALGPS_UARTInitData );
    if (KIDERR_OK != vl_ErrorStatus)
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        MC_HALGPS_TRACE_INF("END 2 : HALGPS2_00UartInit");
        return FALSE;
    }

    vl_ErrorStatus = kid3_04Ioctl (UART, HALGPS_UART_PORT_NUMBER,UART_IOCTL_SET_LINE_MODE,(uint32_t *) UART_LINE_MODE_8N1);
    if (KIDERR_OK != vl_ErrorStatus)
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_THIRD_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        MC_HALGPS_TRACE_INF("END 3 : HALGPS2_00UartInit");
        return FALSE;
    }

    /*------------------------------------------------------------------------*/
    /* init global variable                                                   */
    /*------------------------------------------------------------------------*/
    pg_HALGPS_UARTCurrentReadData = vg_HALGPS_UARTRxBuffer;

    vg_HALGPS_UARTWriteTrameIndex = 0;

    vg_HALGPS_UARTReadTrameIndex = 0;

    memset(vg_HALGPS_UARTTrameInfo,0,(sizeof(*pl_HALGPSTrame))*HALGPS_MAX_RX_BUFFER_NUMBER);

    vg_WriteIndex = 0;

    vg_WriteLastFreeIndex = HALGPS_MAX_TX_BUFFER_NUMBER-1;

    /*------------------------------------------------------------------------*/
    /* init reception circular buffer                                         */
    /*------------------------------------------------------------------------*/
    for ( vl_Index = 0; vl_Index < (HALGPS_MAX_RX_BUFFER_NUMBER*2) ; vl_Index ++)
    {
        HALGPS2_10UartInitRxBuffer ( vl_Index );
    }

    /*------------------------------------------------------------------------*/
    /* init interruption service                                              */
    /*------------------------------------------------------------------------*/
#ifndef HDL_CLEANUP_INTHDL_CPR
    /* Adaptation from dhci module, function dhci2_0UartInit() */
    /* See file /vobs/package278/btps/modules/dhci/srce/DHCI2uart.c */

    /* set on error function and activate DMA */

    vl_SetRoutingServ.usILevel    = din2_98GetDeviceDefaultIntPriority ( HALGPS_UART_INT );
    vl_SetRoutingServ.pfnIST      = MC_DIN_UART_IST;
    vl_SetRoutingServ.usIMode     = INTC_IRQ;
    vl_SetRoutingServ.pfnOnError  = HALGPS2_05UartError;
    vl_SetRoutingServ.pfnProgress = UART1_05DMAProgress;

    vl_ErrorStatus = kid3_04Ioctl (UART, HALGPS_UART_PORT_NUMBER, UART_IOCTL_SET_DMA_SERVICE, &vl_SetRoutingServ);

    if ( vl_ErrorStatus == KIDERR_OK )
    {
        /* Register Interrupt Handler used by awIntHandler_IRQ */
        din2_99RegisterRawInteruptHandlerServiceTask( HALGPS_UART_INT, UART1_00DST );
        /* NO ERROR */
    }
    else
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FOURTH_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        MC_HALGPS_TRACE_INF("END 4 : HALGPS2_00UartInit");
        return FALSE;
    }
#else
    inthdl4_01InitService(HALGPS_UART_INT, INTHDL_SERVICE_DMA,
                          UART1_00DST,
                          HALGPS2_05UartError,
                          UART1_05DMAProgress);

    inthdl4_02EnableInt(HALGPS_UART_INT);
#endif

    vl_ErrorStatus = kid3_04Ioctl( UART, HALGPS_UART_PORT_NUMBER, UART_IOCTL_SET_SPEED,( uint32_t * ) UART_SPEED_115200 );

    /*------------------------------------------------------------------------*/
    /* Begin to acquire data on UART                                          */
    /*------------------------------------------------------------------------*/
    vl_ErrorStatus |= kid3_02Read(UART, HALGPS_UART_PORT_NUMBER, &vg_HALGPS_UARTRxIorb[0]);

    if (vl_ErrorStatus != KIDERR_OK)
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIFTH_ERROR);
        vg_HALGPS_ErrorNumber|=((uint8_t)vl_ErrorStatus);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        MC_HALGPS_TRACE_INF("END 5 : HALGPS2_00UartInit");
        return FALSE;
    }

    MC_HALGPS_TRACE_INF("END : HALGPS2_00UartInit");

    return TRUE;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/****************************************************/
/**                       HALGPS2_01UartRcv        **/
/****************************************************/
uint32_t HALGPS2_01UartRcv(uint8_t * pp_rxBuffer, uint32_t  vp_lenght)
{
    uint32_t vl_dataLenght=0;
    uint32_t vl_returnLenght=0;
    uint32_t vl_lenght=0;
    uint8_t * pl_dataPtr;

#if HALGPS_TRACE_DEBUG
    uint32_t vl_traceLenght=0;
    int8_t vp_traceString[512];
    int8_t* vp_traceStringPtr;
#endif

    /* MC_HALGPS_TRACE_INF("BEGIN : HALGPS2_01UartRcv"); */

#if HALGPS_TRACE_DEBUG
    vp_traceStringPtr = (int8_t*)&vp_traceString[0];
    memset(vp_traceStringPtr,0,512);
    vl_traceLenght += sprintf((char *)(vp_traceStringPtr+vl_traceLenght),"library want to read = 0x%x\n", vp_lenght);
#endif

    if (vp_lenght == 0 || pp_rxBuffer==NULL)
    {
        MC_HALGPS_TRACE_INF("END 1 : HALGPS2_01UartRcv");
        return 0;
    }

    if (vg_HALGPS_UARTReadTrameIndex==vg_HALGPS_UARTWriteTrameIndex)
    {
        /* MC_HALGPS_TRACE_INF("Not enough data received return 0"); */
        /*the current trame is not completely received*/
#if HALGPS_TRACE_DEBUG
        vl_traceLenght +=sprintf((char*)(vp_traceStringPtr+vl_traceLenght),"Index read is equal to index write overflow case\n");
#endif
        /* MC_HALGPS_TRACE_INF("END 2 : HALGPS2_01UartRcv"); */
        return 0;
    }

    vl_dataLenght = vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTReadTrameIndex].v_trameLenght;

#if HALGPS_TRACE_DEBUG
    vl_traceLenght +=sprintf((char*)(vp_traceStringPtr+vl_traceLenght), "Circular buffer size = 0x%x\n",((HALGPS_RX_BUFFER_MAX_SIZE+HALGPS_RX_BUFFER_IT_SIZE)*HALGPS_MAX_RX_BUFFER_NUMBER));
    vl_traceLenght +=sprintf((char*)(vp_traceStringPtr+vl_traceLenght), "Begin address of circular buffer  = 0x%x\n",(uint8_t*)&vg_HALGPS_UARTRxBuffer[0]);
    vl_traceLenght +=sprintf((char*)(vp_traceStringPtr+vl_traceLenght), "Circular buffer address before read = 0x%x\n",pg_HALGPS_UARTCurrentReadData);
#endif

    vl_lenght = vp_lenght;
    pl_dataPtr = pp_rxBuffer;

    while (vl_lenght>0 && vl_dataLenght!=0)
    {
        if (vl_lenght>=vl_dataLenght)
        {

            /*fisrt, copy the data*/
            HALGPS2_12CpyCircularBuffer((uint8_t**)&pl_dataPtr,(uint8_t**)&pg_HALGPS_UARTCurrentReadData,vl_dataLenght);

            pl_dataPtr += vl_dataLenght;

            /*reset the length of the data*/
            vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTReadTrameIndex].v_trameLenght =0;

            pg_HALGPS_UARTCurrentReadData = (uint8_t*)&vg_HALGPS_UARTRxBuffer[vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTReadTrameIndex].v_NextTrameOffset];

            vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTReadTrameIndex].v_NextTrameOffset =0 ;

            if (((pg_HALGPS_UARTCurrentReadData-(uint8_t*)&vg_HALGPS_UARTRxBuffer[0])%(HALGPS_RX_BUFFER_MAX_SIZE+HALGPS_RX_BUFFER_IT_SIZE))!=0)
            {
                /*generate an error it's not possible*/
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
            }

            /*set the next data lenght index*/
            HALGPS2_11UartGetNextRxIndex(vg_HALGPS_UARTReadTrameIndex,&vg_HALGPS_UARTReadTrameIndex,HALGPS_MAX_RX_BUFFER_NUMBER);

            /*return the data copied lenght*/
            vl_returnLenght += vl_dataLenght;

            vl_lenght -= vl_dataLenght;
            if (vg_HALGPS_UARTReadTrameIndex!=vg_HALGPS_UARTWriteTrameIndex)
            {
                vl_dataLenght = vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTReadTrameIndex].v_trameLenght;
            }
            else
            {
                vl_dataLenght = 0;
            }

        }
        else
        {
            HALGPS2_12CpyCircularBuffer((uint8_t**)&pl_dataPtr,(uint8_t**)&pg_HALGPS_UARTCurrentReadData,vl_lenght);
            vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTReadTrameIndex].v_trameLenght -=vl_lenght;
            vl_returnLenght += vl_lenght;

            pg_HALGPS_UARTCurrentReadData += vl_lenght;
            vl_lenght= 0;
        }
    }

#if HALGPS_TRACE_DEBUG
    vl_traceLenght +=sprintf((char*)(vp_traceStringPtr+vl_traceLenght), "Circular buffer address after read = 0x%x\n",pg_HALGPS_UARTCurrentReadData);
    MC_HALGPS_TRACE_INF(&vp_traceString[0]);
#endif
#if HALGPS_TRACE_UART_RCV
    *(pp_rxBuffer+vl_returnLenght) = 0;
    MC_HALGPS_TRACE_INF("UartRcv: len=%d '%s'",vl_returnLenght, pp_rxBuffer);
#endif

    /* MC_HALGPS_TRACE_INF("END : HALGPS2_01UartRcv"); */

    return vl_returnLenght;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/****************************************************/
/**                       HALGPS2_02UartSend       **/
/****************************************************/
uint32_t HALGPS2_02UartSend(uint8_t * pp_txBuffer, uint32_t  vp_lenght)
{
    t_kidIORB *  pl_UartTxIORB=NULL;
    uint32_t vl_DataLength = 0;
    uint32_t vl_bufferSize = 0;
    uint32_t vl_firstIndex =0;
    t_kidError vl_ErrorStatus;
    uint8_t * pl_buffer=NULL;

#if HALGPS_TRACE_DEBUG
    uint16_t vl_traceLenght=0;
    int8_t vp_traceString[512];
    int8_t* vp_traceStringPtr=NULL;
#endif

    /* MC_HALGPS_TRACE_INF("BEGIN : HALGPS2_02UartSend"); */

    if (vp_lenght ==0 || pp_txBuffer==NULL)
    {
        return 0;
    }
#if HALGPS_TRACE_DEBUG
    vp_traceStringPtr = (int8_t*)&vp_traceString[0];
    memset(vp_traceStringPtr,0,512);
    vl_traceLenght +=sprintf((char*)(vp_traceStringPtr+vl_traceLenght),"library want to send : 0x%x bytes\n ",vp_lenght);
#endif

    /*Init variable for the copy */
    vl_DataLength = vp_lenght;
    pl_buffer = pp_txBuffer;
    vl_firstIndex = vg_WriteIndex;

    while ((vl_DataLength >0) && (vg_WriteIndex !=  vg_WriteLastFreeIndex))
    {
        pl_UartTxIORB = (t_kidIORB *)&vg_HALGPS_UARTTxBuffer[vg_WriteIndex*HALGPS_TX_BUFFER_MAX_SIZE];

        /*fisrt calculated the buffer lenght */
        if ( vl_DataLength > (HALGPS_TX_BUFFER_MAX_SIZE-sizeof( *pl_UartTxIORB)) )
        {
            vl_DataLength -= HALGPS_TX_BUFFER_MAX_SIZE-sizeof(*pl_UartTxIORB);
            vl_bufferSize = HALGPS_TX_BUFFER_MAX_SIZE-sizeof(*pl_UartTxIORB);
        }
        else
        {
            vl_bufferSize = vl_DataLength;
            vl_DataLength = 0;
        }

        memcpy ( (void *)((uint32_t)pl_UartTxIORB + sizeof (*pl_UartTxIORB)),pl_buffer, vl_bufferSize );
        pl_buffer += vl_bufferSize;

        KID_SetIorb ( pl_UartTxIORB,
                      (uint8_t *)( (uint32_t) pl_UartTxIORB + sizeof (*pl_UartTxIORB)),
                      (uint16_t) vl_bufferSize,
                      KID_WRITE,
                      KID_MODE_DMA);

        /* memorise the index number in the IORB structure */
        KID_SetIorbPrivate  ( pl_UartTxIORB, vg_WriteIndex);

        /*set next index */
        HALGPS2_11UartGetNextRxIndex(vg_WriteIndex,&vg_WriteIndex,HALGPS_MAX_TX_BUFFER_NUMBER);

        if (vl_DataLength)
        {
            /*chain buffer*/
            KID_SetIorbChain(pl_UartTxIORB, (t_kidIORB *)&vg_HALGPS_UARTTxBuffer[vg_WriteIndex*HALGPS_TX_BUFFER_MAX_SIZE]);
        }

        KID_SetIorbWhenDone ( pl_UartTxIORB, HALGPS2_04UartItTx );
    }

    if (vl_DataLength<vp_lenght)
    {
        /* Now send the data */
        vl_ErrorStatus = UART2_03Write(UART, HALGPS_UART_PORT_NUMBER, (t_kidIORB *)&vg_HALGPS_UARTTxBuffer[vl_firstIndex*HALGPS_TX_BUFFER_MAX_SIZE]);
    }

#if HALGPS_TRACE_DEBUG
    vl_traceLenght +=sprintf((char*)(vp_traceStringPtr+vl_traceLenght),"0x%x bytes are sent\n ",vp_lenght-vl_DataLength);
    MC_HALGPS_TRACE_INF(&vp_traceString[0]);
#endif
#if HALGPS_TRACE_UART_SEND
    *(pp_txBuffer+(vp_lenght-vl_DataLength)) = 0;
    MC_HALGPS_TRACE_INF("UartSend: len=%d '%s'",vp_lenght-vl_DataLength, pp_txBuffer);
#endif

    /* MC_HALGPS_TRACE_INF("END : HALGPS2_02UartSend"); */
    return (vp_lenght-vl_DataLength);
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3

/****************************************************/
/**            HALGPS2_03UartItRx                  **/
/****************************************************/
t_kidError HALGPS2_03UartItRx(t_kidIORB * pp_IORB)
{
    t_kidError vl_ErrorStatus;
    t_kidError vl_ReturnStatus = KIDERR_OK;
    uint32_t        vl_Index = 0;

    /* we are under IT, don't used trace system*/

    vl_ErrorStatus = KID_GetIorbStatus(pp_IORB);

    vl_Index =KID_GetIorbPrivate(pp_IORB);

#if defined LBS_INTRPT_BB_READ
    if ( (vg_HALGPS_LantencyTimer==0) || (vg_HALGPS_ResetLantencyTimer == 1))
    {
        vg_HALGPS_LantencyTimer = GN_GPS_Get_OS_Time_ms();
        vg_HALGPS_ResetLantencyTimer = 0;
    }
#endif

    if (vl_Index == ((HALGPS_MAX_RX_BUFFER_NUMBER*2)-1))
    {
        /*it's the last block, set to zero the next block offset*/
        vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTWriteTrameIndex].v_NextTrameOffset = 0;
    }
    else
    {
        vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTWriteTrameIndex].v_NextTrameOffset +=KID_GetIorbDataLength(pp_IORB);
    }



    if ((vl_ErrorStatus == KIDERR_TIMEOUT) || ((vl_Index & 0x00000001) == 0x00000001 ))
    {

        /*end of trame get the data receive lenght*/
        uint32_t vl_NextTrameOffset=0;

        vl_NextTrameOffset = vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTWriteTrameIndex].v_NextTrameOffset;

        vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTWriteTrameIndex].v_trameLenght+= KID_GetIorbRealLength(pp_IORB);

        HALGPS2_11UartGetNextRxIndex(vg_HALGPS_UARTWriteTrameIndex,&vg_HALGPS_UARTWriteTrameIndex,HALGPS_MAX_RX_BUFFER_NUMBER);

        if (vg_HALGPS_UARTWriteTrameIndex!=vg_HALGPS_UARTReadTrameIndex)
        {
            vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTWriteTrameIndex].v_NextTrameOffset = vl_NextTrameOffset;
            vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTWriteTrameIndex].v_trameLenght = 0;
        }
        else
        {
            /*overflow the buffer size must be increase*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            vl_ReturnStatus = KIDERR_OVERFLOW;
        }
#if defined LBS_INTRPT_BB_READ
        if (vl_ErrorStatus == KIDERR_TIMEOUT)
        {
            /*end of trame */
            /* next trame we must memorize the OS time to calulated the latency */
            vg_HALGPS_ResetLantencyTimer = 1;
        }
#endif
    }
    else
    {
        vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTWriteTrameIndex].v_trameLenght+=KID_GetIorbDataLength(pp_IORB);
    }

    return vl_ReturnStatus;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/****************************************************/
/**                       HALGPS2_04UartItTx       **/
/****************************************************/
t_kidError HALGPS2_04UartItTx(t_kidIORB * pp_IORB)
{
    uint32_t vl_bufferIndex=0;
    uint32_t vl_nextBufferIndex=0;

    /* we are under IT, don't used trace system*/

    vl_bufferIndex = KID_GetIorbPrivate(pp_IORB);

    HALGPS2_11UartGetNextRxIndex(vg_WriteLastFreeIndex,&vl_nextBufferIndex,HALGPS_MAX_TX_BUFFER_NUMBER);

    if (vl_nextBufferIndex != vl_bufferIndex)
    {
        /* pb possibility of overflow */
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
    }

    vg_WriteLastFreeIndex = KID_GetIorbPrivate(pp_IORB);

    return KIDERR_OK;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/****************************************************/
/**                           HALGPS2_05UartError  **/
/****************************************************/
t_kidError HALGPS2_05UartError( t_kidDevType type,
                                t_kidDevInst instance,
                                t_kidError   errcode)
{
    MC_HALGPS_TRACE_INF("BEGIN : HALGPS2_05UartError");

    if (type ==UART)
    {
        if (instance == HALGPS_UART_PORT_NUMBER)
        {
            /*an error occurs on UART used by GPS module*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            vg_HALGPS_ErrorNumber|=((uint8_t)errcode);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }

    MC_HALGPS_TRACE_INF("END : HALGPS2_05UartError");
    return KIDERR_OK;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/****************************************************/
/**                 HALGPS2_10UartInitRxBuffer     **/
/****************************************************/
void HALGPS2_10UartInitRxBuffer(uint32_t vp_index)
{
    t_kidIORB*  pl_ThisIorb;
    uint32_t        vl_nextIndex;
    uint32_t        vl_Index;

    /* don't trace this function because there lots of call of this function*/
    /*MC_HALGPS_TRACE_INF("BEGIN : HALGPS2_10UartInitRxBuffer");*/

    HALGPS2_11UartGetNextRxIndex(vp_index,&vl_nextIndex,HALGPS_MAX_RX_BUFFER_NUMBER*2);

    pl_ThisIorb = &vg_HALGPS_UARTRxIorb[vp_index];

    vl_Index = vp_index>>1;/* divided by 2*/

    if ((vp_index & 0x0001)==0) /*modulo 2*/
    {
        KID_SetIorb(pl_ThisIorb,
                    (uint8_t*)&(vg_HALGPS_UARTRxBuffer[vl_Index*(HALGPS_RX_BUFFER_MAX_SIZE+HALGPS_RX_BUFFER_IT_SIZE)]),
                    HALGPS_RX_BUFFER_IT_SIZE,
                    KID_READ,
                    KID_MODE_DMA);
    }
    else
    {
        KID_SetIorb(pl_ThisIorb,
                    (uint8_t*)&(vg_HALGPS_UARTRxBuffer[(vl_Index*(HALGPS_RX_BUFFER_MAX_SIZE+HALGPS_RX_BUFFER_IT_SIZE))+HALGPS_RX_BUFFER_IT_SIZE]),
                    HALGPS_RX_BUFFER_MAX_SIZE,
                    KID_READ,
                    KID_MODE_DMA);

        /*set the timeout for this IORB*/
        KID_SetIorbParam0(pl_ThisIorb,HALGPS_TIMEOUT_VALUE);
    }

    KID_SetIorbPrivate  ( pl_ThisIorb, vp_index);
    KID_SetIorbWhenDone(pl_ThisIorb, HALGPS2_03UartItRx);
    KID_SetIorbChain(pl_ThisIorb, &(vg_HALGPS_UARTRxIorb[vl_nextIndex]));

    /*MC_HALGPS_TRACE_INF("END : HALGPS2_10UartInitRxBuffer");*/
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/****************************************************/
/**            HALGPS2_11UartGetNextRxIndex        **/
/****************************************************/
void HALGPS2_11UartGetNextRxIndex(uint32_t vp_currentIndex,uint32_t * pp_nextIndex,uint32_t vp_maxIndex)
{
    /*MC_HALGPS_TRACE_INF("BEGIN : HALGPS2_11UartGetNextRxIndex");*/

    if ( (vp_currentIndex +1) == vp_maxIndex )
    {
        *pp_nextIndex = 0;
    }
    else
    {
        *pp_nextIndex = vp_currentIndex +1 ;
    }
    /*MC_HALGPS_TRACE_INF("END : HALGPS2_11UartGetNextRxIndex");*/
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
/****************************************************/
/**            HALGPS2_12CpyCircularBuffer         **/
/****************************************************/
void HALGPS2_12CpyCircularBuffer(uint8_t** pp_Dst,uint8_t** pp_Src,uint32_t vp_lenght)
{
    uint8_t* pl_EndCircularBuffer;
    int32_t vl_dataEndLength;

    /* MC_HALGPS_TRACE_INF("BEGIN : HALGPS2_12CpyCircularBuffer"); */


    pl_EndCircularBuffer=((uint8_t*)&vg_HALGPS_UARTRxBuffer[0])+(HALGPS_MAX_RX_BUFFER_NUMBER*(HALGPS_RX_BUFFER_MAX_SIZE+HALGPS_RX_BUFFER_IT_SIZE));

    if ( (((uint8_t*)(pp_Src[0]))+vp_lenght) > pl_EndCircularBuffer)
    {
        vl_dataEndLength = pl_EndCircularBuffer-(uint8_t*)(pp_Src[0]);
        if (vl_dataEndLength<0)
        {
            /*generate an error this case is not possible*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
        else
        {
            memcpy((uint8_t*)(pp_Dst[0]),(uint8_t*)(pp_Src[0]),vl_dataEndLength);
            if ((vp_lenght-vl_dataEndLength)>0)
            {
                memcpy(((uint8_t*)(pp_Dst[0]))+vl_dataEndLength,(uint8_t*)&vg_HALGPS_UARTRxBuffer[0],vp_lenght-vl_dataEndLength);
            }
        }

    }
    else
    {
        memcpy((uint8_t*)pp_Dst[0],(uint8_t*)pp_Src[0],vp_lenght);
    }

    /* MC_HALGPS_TRACE_INF("END : HALGPS2_12CpyCircularBuffer"); */

}


/* +LMSqb88934 */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/****************************************************/
/**            HALGPS2_13ResetUartBuffer                 **/
/****************************************************/
void HALGPS2_13ResetUartBuffer()
{
    while (vg_HALGPS_UARTReadTrameIndex!=vg_HALGPS_UARTWriteTrameIndex)
    {
        /*reset the length of the data*/
        vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTReadTrameIndex].v_trameLenght =0;

        vg_HALGPS_UARTTrameInfo[vg_HALGPS_UARTReadTrameIndex].v_NextTrameOffset =0 ;

        /*set the next data lenght index*/
        HALGPS2_11UartGetNextRxIndex(vg_HALGPS_UARTReadTrameIndex,&vg_HALGPS_UARTReadTrameIndex,HALGPS_MAX_RX_BUFFER_NUMBER);

        pg_HALGPS_UARTCurrentReadData = (uint8_t*)&vg_HALGPS_UARTRxBuffer[vg_HALGPS_UARTReadTrameIndex*(HALGPS_RX_BUFFER_MAX_SIZE+HALGPS_RX_BUFFER_IT_SIZE)];
    }

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/****************************************************/
/**            HALGPS2_13ResetUartDriver                 **/
/****************************************************/
uint8_t HALGPS2_14ResetUartDriver()
{
    if(kid3_01Reset (UART, HALGPS_UART_PORT_NUMBER, (t_kidResetLevel) 0,(t_kidCancelAttrib) 0 )!= KIDERR_OK)
    {
            return FALSE;
    }
    return TRUE;

}
/* -LMSqb88934 */
#endif /* __RTK_E__ */

#endif /* GPS_OVER_UART  */
#undef __HALGPS2UART_C__

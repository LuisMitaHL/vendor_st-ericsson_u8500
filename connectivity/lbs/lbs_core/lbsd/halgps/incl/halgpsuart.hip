/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSUART_HIP__
#define __HALGPSUART_HIP__
/**
* \file halgpsuart.hip
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by halgps2Uart.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08 </TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/**
*\fn bool HALGPS2_00UartInit(void)
*\brief initialisation of HALGPS module UART part
*
* initialise all variable declare on haluart.hiv
* initialise the UART driver interface (see pskid module)
* And wait data on UART
*\return TRUE if initialisation was successfull, FALSE overwise
*/
bool HALGPS2_00UartInit(void);

/**
*\fn uint16_t HALGPS2_01UartRcv(uint8_t * pp_rxBuffer, uint32_t  vp_lenght)
*\brief get data from UART
*
*\param pp_rxBuffer pointer where the data will be copied
*\param vp_lenght desired data lenght
*\return copied data lenght
*/
uint32_t HALGPS2_01UartRcv(uint8_t * pp_rxBuffer, uint32_t  vp_lenght);

/**
*\fn uint16_t HALGPS2_02UartSend(uint8_t * pp_txBuffer, uint32_t  vp_lenght)
*\brief send data to UART
*
*\param pp_txBuffer buffer containing data
*\param vp_lenght data lenght
*\return the sent data lenght
*/
uint32_t HALGPS2_02UartSend(uint8_t * pp_txBuffer, uint32_t  vp_lenght);
#ifdef __RTK_E__
#ifdef __HALGPS2UART_C__

/**
*\fn t_kidError HALGPS2_03UartItRx(t_kidIORB * pp_IORB)
*\brief Interruption routine on UART reception
*
*\param pp_IORB IORB of the receive data (trigged by timeout or IT)
*\return an error is returned if circular buffer is overflow else return OK
*/
t_kidError HALGPS2_03UartItRx(t_kidIORB * pp_IORB);

/**
*\fn t_kidError HALGPS2_04UartItTx(t_kidIORB * pp_IORB)
*\brief Interruption routine on UART sending
*
*\param pp_IORB IORB of buffer sent
*\return always OK
*/
t_kidError HALGPS2_04UartItTx(t_kidIORB * pp_IORB);

/**
*\fn t_kidError HALGPS2_05UartError( t_kidDevType type,t_kidDevInst instance,t_kidError errcode)
*\brief
*
*\param type type of the device (SPI, UART....)
*\param instance the instance correponding of UART port number for HALGPS
*\param errcode the error happened
*\return always OK
*/
t_kidError HALGPS2_05UartError( t_kidDevType type,t_kidDevInst instance,t_kidError errcode);

/**
*\fn void HALGPS2_10UartInitRxBuffer(uint32_t vp_index)
*\brief for an given index initialise the corresponding IORB with it buffer
*
* The IORB is linked with the next IORB
*
*\param vp_index the index of buffer na IORB
*/
void HALGPS2_10UartInitRxBuffer(uint32_t vp_index);

/**
*\fn void HALGPS2_11UartGetNextRxIndex(uint32_t vp_currentIndex,uint32_t * pp_nextIndex,uint32_t vp_maxIndex)
*\brief get the next index of a circular buffer
*
*\param vp_currentIndex the current index of the circular buffer
*\param pp_nextIndex pointer where the next index must be stored
*\param vp_maxIndex max index value of the circular buffer
*/
void HALGPS2_11UartGetNextRxIndex(uint32_t vp_currentIndex,uint32_t * pp_nextIndex,uint32_t vp_maxIndex);

/**
*\fn void HALGPS2_12CpyCircularBuffer(uint8_t** pp_Dst,uint8_t** pp_Src,uint32_t vp_lenght)
*\brief copy data from receive circular buffer
*
*\param pp_Dst pointer where the data must be copied
*\param pp_Src pointer on vg_HALGPS_UARTRxBuffer buffer
*\param vp_lenght data wanted lenght
*/
void HALGPS2_12CpyCircularBuffer(uint8_t** pp_Dst,uint8_t** pp_Src,uint32_t vp_lenght);
#endif /* __HALGPS2UART_C__ */

/* +LMSqb88934 */
void HALGPS2_13ResetUartBuffer(void);


uint8_t HALGPS2_14ResetUartDriver(void);
/* -LMSqb88934 */

#endif /*__RTK_E__ */
#ifdef AGPS_LINUX_FTR
/**
*\fn void HALGPS2_13ResetUart()
*\brief reset varaible of uart driver
*
* If there are some data in the receive buffer there are lost 
*
*/

void HALGPS2_13ResetUart(void);
#endif /* AGPS_LINUX_FTR */

#endif /* __HALGPSUART_HIP__ */

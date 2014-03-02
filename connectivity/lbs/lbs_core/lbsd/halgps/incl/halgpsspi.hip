/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSSPI_HIP__
#define __HALGPSSPI_HIP__
/**
* \file halgpsspi.hip
* \date 17/07/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by halgps2spi.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 17.07.08</TD><TD> M.COQUELIN </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/**
*\fn bool HALGPS6_00SpiInit(void)
*\brief initialisation of HALGPS module SPI part
*
* initialise all variable declare on halgpsbus.hiv
* initialise the SPI driver interface (see pskid module)
* And wait data on SPI
*\return TRUE if initialisation was successfull, FALSE overwise
*/
bool HALGPS6_00SpiInit(void);

/**
*\fn uint16_t HALGPS6_01SpiRcv(uint8_t * pp_rxBuffer, uint32_t  vp_length)
*\brief get data from SPI
*
*\param pp_rxBuffer pointer where the data will be copied
*\param vp_length desired data length
*\return copied data length
*/
uint32_t HALGPS6_01SpiRcv(uint8_t * pp_rxBuffer, uint32_t  vp_length);

/**
*\fn uint16_t HALGPS6_01SpiSend(uint8_t * pp_txBuffer, uint32_t  vp_length)
*\brief send data to SPI
*
*\param pp_txBuffer buffer containing data
*\param vp_length data length
*\return the sent data length
*/
uint32_t HALGPS6_02SpiSend(uint8_t * pp_txBuffer, uint32_t  vp_length);

#if defined __HALGPS6SPI_C__ || defined __HALGPS6SPILINUX_C__

#ifdef __RTK_E__

/**
*\fn t_kidError HALGPS6_03SpiIt(t_kidIORB * pp_IORB)
*\brief Interruption routine on SPI
*
*\param pp_IORB IORB of the receive data (trigged by  IT)
*\return an error is returned if circular buffer is overflow else return OK
*/
t_kidError HALGPS6_03SpiIt(t_kidIORB * pp_IORB);


/**
*\fn t_kidError HALGPS6_05SpiOnError( t_kidDevType type,t_kidDevInst instance,t_kidError errcode)
*\brief
*
*\param type type of the device (SPI, UART....)
*\param instance the instance correponding of UART/SPI port number for HALGPS
*\param errcode the error happened
*\return always OK
*/
t_kidError HALGPS6_05SpiOnError( t_kidDevType type,t_kidDevInst instance,t_kidError errcode);
#endif /*__RTK_E__*/

#endif //__HALGPS6SPI_C__ || __HALGPS6SPILINUX_C__

/**
*\fn void HALGPS6_04ResetSpi(void)
*\brief Reset SPI buffer indexes
*
*/
void HALGPS6_04ResetSpi(void);

#if (defined(HALGPS_SPI_OPTIMIZATION))
/**
*\fn void HALGPS6_06SpiDataInput(void)
*\brief Set the SPI Data IO like an GPIO input
*
*/
void HALGPS6_06SpiDataInput(void);

/**
*\fn void HALGPS6_07SpiDataOutput(void)
*\brief Restore the configuration of the SPI DATA IO (modification done in HALGPS6_06SpiDataInput)
*
*/
void HALGPS6_07SpiDataOutput(uint8_t vp_gpioValue, uint8_t vp_saveValue);
#endif /* HALGPS_SPI_OPTIMIZATION*/


/**
*\fn void HALGPS6_08PadSpiTxBuf(void)
*\brief Add 19 bytes of NULL data to the SPI Tx data
*
*/
void HALGPS6_08PadSpiTxBuf(void);


#endif //__HALGPSSPI_HIP__

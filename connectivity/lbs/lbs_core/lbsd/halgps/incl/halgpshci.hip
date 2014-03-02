
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __HALGPSHCI_HIP__
#define __HALGPSHCI_HIP__
/**
* \file halgpshci.hip
* \date 02/02/2010
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all prototype used by halgps8xosgps.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 02.02.2010</TD><TD> Archana.B </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/**
*\fn uint8_t HALGPS9_00HciStart(void)
*\brief initialisation of HALGPS module HCI part
*
* initialises HCI
*\return TRUE if initialisation was successful, FALSE otherwise
*/
uint8_t HALGPS9_00HciStart(void);

/**
*\fn void HALGPS9_01HciStop();
*\brief Wait for Ack for Sent data
*\return TRUE if initialisation was successful, FALSE otherwise
*/
uint8_t HALGPS9_01HciStop(void);


/**
*\fn uint8_t HALGPS9_02WaitForTxAck(uint8_t vp_Max_Retry);
*\brief Wait for Ack for Sent data
*\param vp_Max_Retry Maximum number of retry waiting for acknowledgement
*\return TRUE if initialisation was successful, FALSE otherwise
*/
uint8_t HALGPS9_02WaitForTxAck(uint8_t vp_Max_Retry);

/**
*\fn void HALGPS9_03HciRxBuffInit();
*\brief Initializes the receive buffer for HCI data
*\return void
*/
void HALGPS9_03HciRxBuffInit();

/**
*\fn void HALGPS9_04HciRxBuffUpdate(uint8_t * pp_rxBuffer, uint32_t vp_length);
*\brief Updates the receive buffer with data from HCI
*\param pp_rxBuffer buffer which contains data to be updated
*\param vp_length data length
*\return void
*/
void HALGPS9_04HciRxBuffUpdate(uint8_t * pp_rxBuffer, uint32_t vp_length);

/**
*\fn uint32_t HALGPS9_05HciRxBuffRcv(uint8_t * pp_rxBuffer, uint32_t vp_length);
*\brief retrieves data from the HCI buffer into the provided byte array
*\param pp_rxBuffer buffer which contains data to be updated
*\param vp_length data length
*\return Number of bytes retrieved
*/
uint32_t HALGPS9_05HciRxBuffRcv(uint8_t * pp_rxBuffer, uint32_t vp_length);

/**
*\fn uint32_t HALGPS9_06HciSend(uint8_t * pp_txBuffer, uint32_t vp_length);
*\brief Sends data over HCI only if TX is possible [ i.e. not waiting for an ACK ]
*\param pp_txBuffer buffer which contains data to be updated
*\param vp_length data length
*\return Number of bytes sent
*/
uint32_t HALGPS9_06HciSend(uint8_t * pp_txBuffer, uint32_t vp_length);

/**
*\fn uint32_t HALGPS9_07HciRcv(uint8_t * pp_rxBuffer, uint32_t vp_length);
*\brief Retrieves data received over the HCI channel
*\param pp_txBuffer buffer which contains data
*\param vp_length Max data length
*\return Number of bytes retrieved
*/
uint32_t HALGPS9_07HciRcv(uint8_t * pp_rxBuffer, uint32_t vp_length);


/**
*\fn void HALGPS9_09HciReadAndProcess(void);
*\brief The main process function which reads data from the HCI and depending on the type of data, does the 
        appropriate state transitions
*\return VOID
*/
void HALGPS9_08HciReadAndProcess(void);

/**
*\fn uint8_t HALGPS9_09HciGpsEnable()
*\brief Sends a command to the ARM SS to enable GPS.
*\return TRUE in case of success and FALSE in case of Failure
*/

uint8_t HALGPS9_09HciGpsEnable();

/**
*\fn uint8_t HALGPS9_10HciGpsDisable()
*\brief Sends a command to the ARM SS to disable GPS.
*\return TRUE in case of success and FALSE in case of Failure
*/

uint8_t HALGPS9_10HciGpsDisable();

/**
*\fn uint32_t HALGPS9_11HciSendPacket(uint8_t * pp_txBuffer, uint32_t  vp_length, uint32_t vp_type)
*\brief send data to HCI in packet format
*
*\param pp_txBuffer buffer containing data
*\param vp_length data length
*\return the sent data length
*/
uint32_t HALGPS9_11HciSendPacket(uint8_t * pp_txBuffer, uint32_t  vp_length, uint32_t vp_type);

/**
*\fn void HALGPS9_12EnterDSM()
*\brief Indicate to HCI driver that we do not need CG2900 in short term.
* Allows CG29XX to exit Deep Sleep Mode
*/
void HALGPS9_12EnterDSM(void);

/**
*\fn void HALGPS9_13ExitDSM()
*\brief Indicate to HCI driver that we need CG2900 in short term.
* Allows CG29XX to exit Deep Sleep Mode
*/
void HALGPS9_13ExitDSM(void);

/**
*\fn void HALGPS9_14HciRxBuffDeInit(void)
*\free the HCI receive buffer
*/

void HALGPS9_14HciRxBuffDeInit(void);



/* The following are definitions from the HCI layer interface. 
   These are internally used by halgps9hci.c and must not be 
   used by any other layer 
   */

/**
*\fn uint8_t HALGPS10_00HciInit()
*\brief Connects to HCI Uart.
*\return TRUE in case of success and FALSE in case of Failure
*/

uint8_t HALGPS10_00HciInit();

/**
*\fn uint32_t HALGPS10_01HciReadRaw(uint8_t * pp_rxBuffer, uint32_t vp_length)
*\brief get data from HCI. Returns the raw data read from the HCI channels and not as packets
*
*\param pp_rxBuffer pointer where the data will be copied
*\param vp_length desired data length
*\return copied data length
*/
uint32_t HALGPS10_01HciReadRaw(uint8_t * pp_rxBuffer, uint32_t vp_length);

/**
*\fn uint32_t HALGPS10_02HciWriteRaw(uint8_t * pp_txBuffer, uint32_t vp_length)
*\brief writes data to HCI driver
*
*\param pp_rxBuffer pointer which contains data to be written
*\param vp_length desired data length
*\return data length written
*/
uint32_t HALGPS10_02HciWriteRaw(uint8_t * pp_txBuffer, uint32_t  vp_length);

/**
*\fn uint8_t HALGPS10_03HciDeinit()
*\brief Close connection to HCI driver
*\return TRUE on success and FALSE on failure
*/
uint8_t HALGPS10_03HciDeinit();

/**
*\fn void HALGPS10_04EnterDSM()
*\brief Indicate to HCI driver that we no longer need CG2900 in short term.
* Allows CG29XX to enter Deep Sleep Mode
*/
void HALGPS10_04EnterDSM();

/**
*\fn void HALGPS10_05ExitDSM()
*\brief Indicate to HCI driver that we need CG2900 in short term.
* Allows CG29XX to exit Deep Sleep Mode
*/
void HALGPS10_05ExitDSM();

#endif //__HALGPSHCI_HIP__

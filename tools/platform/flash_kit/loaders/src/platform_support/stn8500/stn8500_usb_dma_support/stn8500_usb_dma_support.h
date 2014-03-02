/***********************************************************************
 * $Copyright ST-Ericsson 2010$
 **********************************************************************/

#ifndef _USB_DMA_SUPPORT_H_
#define _USB_DMA_SUPPORT_H_

/**
 * @file   stn8500_usb_dma_support.h
 * @brief  Contains declarations of functions for enabling,
 *         initiating and controlling DMA powered USB transfer
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "error_codes.h"
#include "hcl_defs.h"
#include "usb.h"

typedef struct {
    uint8 *Data_p;
    uint32 NrBytes;
    uint32 Offset;
    boolean IsOpened;
    boolean MoreChunks;
} DMARecord_t;

typedef enum {
    RX_DMA = 0,
    TX_DMA,
    IDLE_TRANSFER_STATE
} DMATransfer_t;

typedef ErrorCode_e(*CallbackUSB_fn)(const uint8 *const, const uint32);

typedef struct {
    CallbackUSB_fn TxCallbackUSB_fn;
    CallbackUSB_fn RxCallbackUSB_fn;
} CallbackFunctions_t;

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/***************************************************************************//**
* @brief Initializes DMA services, sets the USB callback functions
*        and opens DMA channels
*
* @param[in] RxCallbackUSB  Pointer to Rx USB callback function
* @param[in] TxCallbackUSB  Pointer to Tx USB callback function
*
*******************************************************************************/
ErrorCode_e InitUSB_DMA(CallbackUSB_fn RxCallbackUSB, CallbackUSB_fn TxCallbackUSB);

/***************************************************************************//**
* @brief Shuts the DMA communication down
*
* The function is used to stop the DMA transfer and to close
* the Rx and Tx channels
*
*******************************************************************************/
ErrorCode_e ShutdownUSB_DMA(void);

/***************************************************************************//**
* @brief Initializes Tx transfer over DMA
*
* @param[in] Data_p      Pointer to data which is to be sent over USB
* @param[in] NrBytes     Number of bytes to send
*
* @return ErrorCode_e Forwards the response from DMA controller
*
*******************************************************************************/
ErrorCode_e InitDMATxTransfer(uint8 *const Data_p, uint32 NrBytes);

/***************************************************************************//**
* @brief Initializes Rx transfer over DMA
*
* @param[in] Data_p      Pointer to the buffer in which the received data is to be stored
* @param[in] NrBytes     Number of bytes to read
*
* @return ErrorCode_e Forwards the response from DMA controller
*
*******************************************************************************/
ErrorCode_e InitDMARxTransfer(uint8 *const Data_p, uint32 NrBytes);

/***************************************************************************//**
* @brief Opens DMA Rx transfer record
*
* The function is used to open transfer record for the data sent over USB
* The record keeps track on the data that is sent in DMA mode and the remaining
* data, which is not aligned to the USB max. packet size. The remaining unaligned data,
* if any, is sent in IT mode.
*
* @param[in] Data_p      Pointer to the buffer in which the received data is to be stored
* @param[in] NrBytes     Number of bytes to read
* @param[in] Offset      Number of bytes to be sent in IT mode, if any.
*
*******************************************************************************/
void OpenDMARxRecord(uint8 *const Data_p, const uint32 NrBytes,  uint32 Offset);

/***************************************************************************//**
* @brief Opens DMA Tx transfer record
*
* The function is used to open transfer record for the data received over USB
* The record keeps track on the data that is received in DMA mode and the remaining
* data, which is not aligned to the USB max. packet size. The remaining unaligned data,
* if any, is read in IT mode.
*
* @param[in] Data_p      Pointer to the buffer of the data which is to be sent
* @param[in] NrBytes     Number of bytes to write
* @param[in] Offset      Number of bytes to be written in IT mode, if any.
*
*******************************************************************************/
void OpenDMATxRecord(uint8 *const Data_p, const uint32 NrBytes, const uint32 Offset);

/***************************************************************************//**
* @brief Checks if Rx recored is opened
*
* @return boolean
*
*******************************************************************************/
boolean IsDMARxRecordOpened(void);

/***************************************************************************//**
* @brief Checks if Tx recored is opened
*
* @return boolean
*
*******************************************************************************/
boolean IsDMATxRecordOpened(void);

/***************************************************************************//**
* @brief Used to close transfer Rx/Tx records
*
*******************************************************************************/
void CloseDMATransferRecord(DMARecord_t *pTransferRecord);

#endif /* DMA_H_ */

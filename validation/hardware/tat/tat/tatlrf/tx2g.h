/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   isimsg.h
* \brief   routines to run GSM/EDGE transceiver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TX2G_H_
#define TX2G_H_

/**
 * Build and Send ISI message needed to Start/Stop TX 2G5
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl2_00StartTx2G5_exec(struct dth_element *elem);

/**
 * Set argument needed to do a StartTX2G5.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set. 
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl2_03StartTx2G5Parm_set(struct dth_element *elem, void *value);

/**
 * Set the operation to do on modem.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the operation to do. It can be StartTX2G5 or StopTX2G5.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl2_04StartTx2G5_set(struct dth_element *elem, void *value);

int tatl2_02StartTx2G5Parm_get(struct dth_element *elem, void *value);

#endif /* TX2G_H_ */

/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rx2g.h
* \brief   routines to run GSM/EDGE receiver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef RX2G_H_
#define RX2G_H_

/**
 * Build and Send ISI message needed to Start/Stop RX 2G5
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl1_00StartRx2G5_exec(struct dth_element *elem);

/**
 * Set argument needed to do a StartRX2G5.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set. 
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl1_03StartRx2G5Parm_set(struct dth_element *elem, void *value);

/**
 * Set the operation to do on modem.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the operation to do. It can be StartRX2G5 or StopRX2G5.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl1_04StartRx2G5_set(struct dth_element *elem, void *value);

/** Return the address of a C_HAL_SB_GSM_RX_CONTROL_STR structure with the RX 2G
 * inputs or NULL if RX 2G is not active. */
const C_HAL_SB_GSM_RX_CONTROL_STR *tatl1_06GetRx2GInfo(void);

#endif /* RX2G_H_ */

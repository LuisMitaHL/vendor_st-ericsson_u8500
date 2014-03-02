/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rx3g.h
* \brief   routines to run WCDMA receiver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef RX3G_H_
#define RX3G_H_

/**
 * Build and Send ISI message needed to Start/Stop RX 3G
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl6_00StartRx3G_exec(struct dth_element *elem);

/**
 * Set argument needed to do StartRX3G.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set. 
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl6_03StartRx3G_Parm_set(struct dth_element *elem, void *value);

/**
 * Set the operation to do on modem.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the operation to do. It can be StartRX3G or StopRX3G.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl6_04StartRx3G_set(struct dth_element *elem, void *value);

int tatl6_02StartRx3G_Parm_get(struct dth_element *elem, void *value);

/** Return the address of a C_HAL_SB_WCDMA_RX_CONTROL_STR structure with the RX 
 * 3G inputs or NULL if RX 3G is not active. */
const C_HAL_SB_WCDMA_RX_CONTROL_STR *tatl1_06GetRx3GInfo(void);

#endif /* RX3G_H_ */

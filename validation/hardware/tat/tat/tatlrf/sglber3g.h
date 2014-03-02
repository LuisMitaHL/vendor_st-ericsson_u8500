/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sglber3g.h
* \brief   routines to run Single Ended Bit Error Rate for WCDMA interface
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef SGLBER3G_H_
#define SGLBER3G_H_


#define SBER3G_NB_RESULTS             5	/* maximum. one per band */
#define SBER3G_QUOT_TX_LEVEL          8
#define SBER3G_QUOT_AVERAGE_RX_PWR    8
#define SBER3G_QUOT_AVERAGE_RSCP      8

/**
 * Build and send ISI messages to run 3G Single Ended BER
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl19_00SingleBer3G_exec(struct dth_element *elem);

/**
 * Set the operation to do on modem.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the operation to do.
 * @retval 0 success.
 * @retval >0 an error occured while processing.
 */
int tatl19_01SingleBer3G_set(struct dth_element *elem, void *value);

/**
 * Get the results and output arguments.
 * @param[in] elem specifies the DTH element.
 * @param[out] address of a variable receiving the value.
 * @retval 0 success.
 * @retval >0 an error occured while processing.
 */
int tatl19_02SingleBer3G_get(struct dth_element *elem, void *value);

#endif /* SGLBER3G_H_ */

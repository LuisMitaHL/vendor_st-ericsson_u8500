/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sglber2g.h
* \brief   routines to run Single Ended Bit Error Rate for GSM/EDGE interface
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef SGLBER2G_H_
#define SGLBER2G_H_


#define SBER2G_NB_RESULTS               4	/* maximum. one per band */
#define SBER2G_QUOT_BCCH_LEVEL          8
#define SBER2G_QUOT_RX_AVERAGE_POWER    8
#define SBER2G_QUOT_RX_LEVEL            8
#define SBER2G_QUOT_INIT_RX_LEVEL       8

/**
 * Build and send ISI messages to run 2G Single Ended BER
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl18_00SingleBer2G_exec(struct dth_element *elem);

/**
 * Set the operation to do on modem.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the operation to do.
 * @retval 0 success.
 * @retval >0 an error occured while processing.
 */
int tatl18_01SingleBer2G_set(struct dth_element *elem, void *value);

/**
 * Get the results and output arguments.
 * @param[in] elem specifies the DTH element.
 * @param[out] address of a variable receiving the value.
 * @retval 0 success.
 * @retval >0 an error occured while processing.
 */
int tatl18_02SingleBer2G_get(struct dth_element *elem, void *value);

#endif /* SGLBER2G_H_ */

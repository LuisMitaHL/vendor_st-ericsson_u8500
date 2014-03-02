/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   iqsample.h
* \brief   declare 2G/3G IQ samples DTH routines
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef IQSAMPLE_H_
#define IQSAMPLE_H_

/**
 * Build and Send ISI message to run 2G5/3G RX IQ sample
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl16_01IQSample_exec(struct dth_element *elem);

/**
 * Set argument needed to run 2G5/3G RX IQ sample
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl16_02IQSample_set(struct dth_element *elem, void *value);

/**
 * Get an argument returned after 2G5/3G RX IQ sample
 * @param[in] elem specifies the DTH element.
 * @param[out] receive the value.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl16_03IQSample_get(struct dth_element *elem, void *value);

#endif /* IQSAMPLE_H_ */

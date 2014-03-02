/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sftune2g.h
* \brief   routines to self-tune 2G and perform PA compression test
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef SFTUNE2G_H_
#define SFTUNE2G_H_

/**
 * Build and Send ISI messages needed to launch GSM RF Self-tuning test.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl11_00GsmTx_SelfTuning_Exec(struct dth_element *elem);

/**
 * Get output values after a GSM RF Self-tuning test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the location where data read must be stored.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl11_01GsmTx_SelfTuning_Get(struct dth_element *elem, void *value);

/**
 * Set an input argument needed for a GSM RF Self-tuning test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set. 
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl11_02GsmTx_SelfTuning_Set(struct dth_element *elem, void *value);

#endif /* SFTUNE2G_H_ */

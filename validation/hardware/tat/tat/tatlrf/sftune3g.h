/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sftune3g.h
* \brief   routines to self-tune 3G
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef SFTUNE3G_H
#define SFTUNE3G_H


/**
 * Build and Send ISI messages needed to launch WCDMA RF Self-tuning test.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl15_00WcdmaTx_SelfTuning_Exec(struct dth_element *elem);

/**
 * Get output values after a WCDMA RF Self-tuning test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the location where data read must be stored.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl15_01WcdmaTx_SelfTuning_Get(struct dth_element *elem, void *value);

/**
 * Set an input argument needed for a WCDMA RF Self-tuning test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl15_02WcdmaTx_SelfTuning_Set(struct dth_element *elem, void *value);

#endif // SFTUNE3G_H

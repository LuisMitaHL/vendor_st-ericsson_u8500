/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    tatl05selftest.h
* \brief   declaration of SIM self test routines
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATL05SELFTEST_H_
#define TATL05SELFTEST_H_

/* UICC SELF TEST INTERFACE */

#define TATLSIM_DEFAULT_SELFTEST_RESULT     0xFF
#define TATLSIM_DEFAULT_SELFTEST_ERROR      0xE0

#define TATLSIM_NB_SELFTEST_LINES           4

/**
 * Build and Send ISI messages needed to manage UICC self test
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl5_01SelfTest_exec(struct dth_element *elem);

/**
 * Get self test output values.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the location where data read must be stored.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl5_04SelfTest_get(struct dth_element *elem, void *value);

/**
 * Set self test inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl5_05SelfTest_set(struct dth_element *elem, void *value);

#endif /* TATL05SELFTEST_H_ */



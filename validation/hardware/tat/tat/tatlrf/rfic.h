/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rfic.h
* \brief   routines to access RFIC registers
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef RFIC_H_
#define RFIC_H_

/* RFIC Devices IDs
* For Levi:
* 00 = Not Used
* 01 = DRFIC 
* 10 = RFIC
* 11 = PWRIC
*/
#define C_TEST_RFIC_DEV_DRFIC   0x01
#define C_TEST_RFIC_DEV_RFIC    0x02
#define C_TEST_RFIC_DEV_PWRIC   0x03


/**
 * Initialize module data.
 * This function is called once by the framework.
 */
void tatl8_00Init(void);

/**
 * Build and send ISI message needed to read or write RFIC register.
 * @retval 0 success.
 * @retval errorCode if an error occurred while processing.
 */
int tatl8_01Register_exec(struct dth_element *elem);

/**
 * Get the value of the last RFIC register read.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the location where data read must be stored.
 * @retval 0 success.
 * @retval errorCode if an error occurred while processing.
 */
int tatl8_02Register_get(struct dth_element *elem, void *value);

/**
 * Set argument needed to read/write a RFIC register.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set. 
 *		It could be access right (Read or Write), register address or data to write.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl8_03Register_set(struct dth_element *elem, void *value);

#endif /* RFIC_H_ */

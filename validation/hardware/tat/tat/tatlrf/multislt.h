/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   multislt.h
* \brief   declare DTH routines for 2G RX/TX multislots
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef MULTISLT_H_
#define MULTISLT_H_


#define TAT_RF_RX_TX_MULTISLOTS_NB_SLOTS            4

/* AFC ppm value is Q10 encoded */
#define TAT_RF_MULTISLOT_QUOT_AFC_PPM               10

/* RX/TX level in dBm is Q8 encoded */
#define TAT_RF_MULTISLOT_QUOT_LEVEL_DBM             8


/**
 * Build and Send ISI messages to run RX or TX multi slots.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl13_00RxTxMultiSlots_Exec(void);

/**
 * Get output values after a RX or TX multi slots was run.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the location where data read must be stored.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl13_01RxTxMultiSlots_Get(struct dth_element *elem, void *value);

/**
 * Set an input argument needed for power level RF self-tuning test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set. 
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl13_02RxTxMultiSlots_Set(struct dth_element *elem, void *value);

#endif /* MULTISLT_H_ */

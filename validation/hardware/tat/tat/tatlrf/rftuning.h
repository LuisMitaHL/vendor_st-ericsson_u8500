/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rftuning.h
* \brief   routines to tune RF: AFC DCXO, GSM RSSI, WCDMA RSSI and internal
*          power meter.
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef RFTUNING_H_
#define RFTUNING_H_

union tx_pwr_seq_data {
    C_TEST_TX_PWR_SEQ_WCDMA_STR wcdma;
    C_TEST_TX_PWR_SEQ_GSM_STR   gsm;
};

/**
 * Get one of the tuning output.
 * @param[in] elem specifies the DTH element.
 * @param[out] value specifies location to store output. 
 * @retval 0 if success.
 * @retval an errno code if an error occured while processing.
 */
int tatl12_01TuningGet(struct dth_element *elem, void *Value);

/**
 * Set one of the tuning input.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the input value
 * @retval 0 if success.
 * @retval an errno code if an error occured while processing.
 */
int tatl12_02TuningSet(struct dth_element *elem, void *Value);

/**
 * Perform one of the tuning action available.
 * The operation behaviour depends of the inputs set before exec is called.
 * @param[in] elem not used.
 * @retval 0 success.
 * @retval errno if an error occured while processing.
 */
int tatl12_03TuningExec(struct dth_element *elem);

#endif /* RFTUNING_H_ */

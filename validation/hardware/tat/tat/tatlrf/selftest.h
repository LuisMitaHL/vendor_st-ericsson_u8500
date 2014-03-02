/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   selftest.h
* \brief   routines to run RF selftest
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef SELFTEST_H_
#define SELFTEST_H_

/* RF supply regulator indexes in RF_SUPPLY_OUTPUT.regulators array */
typedef enum {
	RF_SUPPLY_REGULATOR_VREG50BD,
	RF_SUPPLY_REGULATOR_VREG50S,
	RF_SUPPLY_REGULATOR_VHI,
	RF_SUPPLY_REGULATOR_VBATT,
	RF_SUPPLY_REGULATOR_VDIG_RX,
	RF_SUPPLY_REGULATOR_VLPF_RX,
	RF_SUPPLY_REGULATOR_VVCO_RX,
	RF_SUPPLY_REGULATOR_VBAM,
	RF_SUPPLY_REGULATOR_VFE,
	RF_SUPPLY_REGULATOR_VLO_RX,
	RF_SUPPLY_REGULATOR_VXO,
	RF_SUPPLY_REGULATOR_VLPF_TX,
	RF_SUPPLY_REGULATOR_VOPA,
	RF_SUPPLY_REGULATOR_VVCO_TX,
	RF_SUPPLY_REGULATOR_VDIG_TX,
	RF_SUPPLY_REGULATOR_VSEL,
	RF_SUPPLY_REGULATOR_VPAP,
	RF_SUPPLY_REGULATOR_VDP_TX,
	RF_SUPPLY_REGULATOR_VLO_TX,
	RF_SUPPLY_REGULATOR_VFB_TX,
	RF_SUPPLY_REGULATOR_COUNT	/* always last in enum */
} RF_SUPPLY_REGULATOR_INDEX;

/**
 * Build and Send ISI messages needed to start a RF Self-Test.
 * @param[in] elem specifies the DTH element.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl10_00SelfTest_Exec(struct dth_element *elem);

/**
 * Get output values after starting a RF Self-Test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the location where data read must be stored.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl10_02SelfTest_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments needed for RF Self-Test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set. 
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl10_03SelfTest_Set(struct dth_element *elem, void *value);

#endif /* SELFTEST_H_ */

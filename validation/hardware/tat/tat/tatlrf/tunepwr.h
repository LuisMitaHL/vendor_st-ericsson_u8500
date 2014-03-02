/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tunepwr.h
* \brief   routines to self-tune internal power metter
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TUNEPWR_H_
#define TUNEPWR_H_

/* ----------------------------------------------------------------------- */
/* RF Self-Tuning main DTH entry points                                    */
/* ----------------------------------------------------------------------- */

/**
 * Build and Send ISI messages needed to start a power level RF self-tuning test.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl9_00Pwr_SelfTuning_Exec(struct dth_element *elem);

/**
 * Get output values after a power level RF self-tuning test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the location where data read must be stored.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl9_02Pwr_SelfTuning_Get(struct dth_element *elem, void *value);

/**
 * Set an input argument needed for power level RF self-tuning test.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set. 
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl9_03Pwr_SelfTuning_Set(struct dth_element *elem, void *value);


/* ----------------------------------------------------------------------- */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */

/* GMSK/8PSK Tx power level constants. */
#define MAX_TUNING_DATA				20

/* WCDMA Tx power level constants. */
#define PWR_METER_ATT_START			0x0000
#define PWR_CTRL_START_VAL			0x012C
#define	DPDCH_CHANNEL_WEIGHT		15
#define	DPCCH_CHANNEL_WEIGHT		8
#define	MAX_WCDMA_TUNING_DATA		32

#define GSM_EGDE_PANEL_TUNING_DATA		16
#define WCDMA_PANEL_TUNING_DATA			32
#define GSM_LOW_BAND_TUNING_DATA		15
#define	GSM_HIGH_BAND_TUNING_DATA		16
#define EDGE_LOW_BAND_TUNING_DATA		12
#define	EDGE_HIGH_BAND_TUNING_DATA		14
#define	GSM_LOW_BAND_PWR_LEVEL_START	5
#define	GSM_HIGH_BAND_PWR_LEVEL_START	0
#define	EDGE_LOW_BAND_PWR_LEVEL_START	8
#define	EDGE_HIGH_BAND_PWR_LEVEL_START	2
#define GSM_LOW_BAND_SHIFT				0
#define	GSM_HIGH_BAND_SHIFT				0
#define EDGE_LOW_BAND_SHIFT				3
#define	EDGE_HIGH_BAND_SHIFT			2
#define	WCDMA_SHIFT						0

#define EDGE_ON						0x0001
#define EDGE_OFF					0x0000

/* QUOTIENT constants. */
#define QUOT_TARGET_POWER			 6
#define QUOT_WCDMA_TARGET_POWER	     3
#define QUOT_TX_RFIC_AGC_INDEX       2
#define NO_QUOTIENT                  0

#endif /* TUNEPWR_H_ */


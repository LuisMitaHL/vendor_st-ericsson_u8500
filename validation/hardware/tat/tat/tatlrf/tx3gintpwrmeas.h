/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tx3gintpwrmeas.h
* \brief   routines to manage WCDMA internal power measurement
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TX3GINTPWRMEAS_H_
#define TX3GINTPWRMEAS_H_

/**
 * Get one of the param.
 * @param[in] elem specifies the DTH element.
 * @param[out] value specifies location to store output.
 * @retval 0 if success.
 * @retval an errno code if an error occured while processing.
 */
int tatl22_11Tx3G_InternalPowerMeasure_Get(struct dth_element *elem, void *Value);

/**
 * Set one of the param.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the input value
 * @retval 0 if success.
 * @retval an errno code if an error occured while processing.
 */
int tatl22_12Tx3G_InternalPowerMeasure_Set(struct dth_element *elem, void *Value);

/**
 * Perform the action.
 * The operation behaviour depends of the inputs set before exec is called.
 * @param[in] elem not used.
 * @retval 0 success.
 * @retval errno if an error occured while processing.
 */
int tatl22_13Tx3G_InternalPowerMeasure_Exec(struct dth_element *elem);

int tatl22_14Tx3G_InternalPowerMeasure_Action();

#endif /* TX3GINTPWRMEAS_H_ */

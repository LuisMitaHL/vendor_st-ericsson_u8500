/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   pmmidmap.h
* \brief   routines to retrieve Permanent Modem Memory definition
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef PMMIDMAP_H_
#define PMMIDMAP_H_


/**
 * Get all RF parameters' data.
 * @retval 0 if success.
 * @retval a TAT error code if the function failed.
 */
int tatl14_00GetAllRfParam();
const RETURN_INFO_PARAM_STR *tatl14_GetRfParamInfo(size_t vp_position);
int tatl14_01CountRfParam();
int tatl14_02GetRfParam(size_t vp_position);
/**
 * Return id and index of a RF parameter
 * @return the parameter's id or -1 if an error occurred
 */
int tatl14_06GetRfParamId(char *param_name, size_t *pp_index);

RETURN_INFO_PARAM_STR *tatl14_03GetParamInfo(int vp_id);

void tatl14_04ReleaseParamInfo();

#endif /* PMMIDMAP_H_ */

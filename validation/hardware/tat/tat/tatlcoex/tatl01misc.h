/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATL01MISC_H_
#define TATL01MISC_H_

/*-----------------------------------------------------------------------------
Functions
-----------------------------------------------------------------------------*/
/**
 *  Manage all threads needed for TAT coexistence
 *  @param[in] thread thread to be managed.
 *	@param[in] threadData data needed for the thread.
 *	@param[in] function task performs by the thread.
 *	@param[in] order START/STOP.
 *	@param[in] state ACTIVATED/DEACTIVATED.
 *  @retval 0 success.
 *  @retval errorCode if an error occures while processing.
 */
int tatl01_00Manage_Processes(pthread_t *thread, void *threadData, void *function,
		DTH_COEX_ORDER order, DTH_COEX_STATE state);

/**
 *  Set input parameter from tatlvideo library.
 *  @param[in] id id of the parameter to be set.
 *  @param[in] type DTH type of the parameter to be set.
 *  @param[in] value new value of the parameter.
 *  @retval 0 success.
 *  @retval errorCode if an error occures while processing.
 */
int tatl01_01Set_Video_Parameter(int id, int type, void *value);

#endif /* TATL01MISC_H_ */


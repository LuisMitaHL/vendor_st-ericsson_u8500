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

#ifndef TATL09MODI2C_H_
#define TATL09MODI2C_H_

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * Definition of data needed to manage RF process
 */
typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process (START/STOP). */
	DTH_COEX_STATE state; /**< State of the process (ACTIVATED/DEACTIVATED). */
	char *error ; /**< Error string .*/
}DTH_MODI2C_DATA ;

/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_MODI2C_DATA v_tatcoex_modI2C_data; /* Data needed for MODI2C management. */

/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on ModI2C buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl09_00ModI2C_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl09_01ModI2C_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl09_02ModI2C_Set(struct dth_element *elem, void *value);

/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for MODI2C activity.
 */
void tatl09_03Init_ModI2C_Data();

#endif /* TATL09MODI2C_H_ */



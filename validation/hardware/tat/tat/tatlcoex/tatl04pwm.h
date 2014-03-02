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

#ifndef TATL04PWM_H_
#define TATL04PWM_H_

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * Definition of data needed to manage the memory process
 */
typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process (START/STOP). */
	DTH_COEX_STATE state; /**< State of the process (ACTIVATED/DEACTIVATED). */
	char *error ; /**< Error string .*/
	u32 pwmId; /**< Id to the PWM bus to be accessed. */
} DTH_PWM_PROCESS_DATA ;


/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_PWM_PROCESS_DATA v_tatcoex_pwm1_data; /* Data needed for PW1 process. */
pthread_t v_tatcoex_pwm1_thread; /* Thread to generate continuous access on PWM1 bus. */
pthread_t v_tatcoex_pwm2_thread; /* Thread to generate continuous access on PWM2 bus. */


/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on PWM buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl04_00Pwm_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl04_01Pwm_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl04_02Pwm_Set(struct dth_element *elem, void *value);

/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for PWM buses.
 *  @param[in] data information needed to manage PWM buses.
 *  @param[in] pwmId Id of the PWM buq selected.
 */
void tatl04_03Init_Pwm_Data (DTH_PWM_PROCESS_DATA *data, u32 pwmId);

#endif /* TATL04PWM_H_ */



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

#ifndef TATL05SENSOR_H_
#define TATL05SENSOR_H_

/* ----------------------------------------------------------------------- */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */
#define	PRIMARY_SENSOR		0
#define	SECONDARY_SENSOR	1

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * Definition of data needed to manage display
 */
typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process (START/STOP). */
	DTH_COEX_STATE state; /**< State of the process (ACTIVATED/DEACTIVATED). */
	char *error ; /**< Error string .*/
	u32 sensorId ; /**< Id of the sensor. */
}DTH_SENSOR_DATA ;


/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_SENSOR_DATA v_tatcoex_csi0_data; /* Data needed for CSI0 (primary sensor). */
DTH_SENSOR_DATA v_tatcoex_csi1_data; /* Data needed for CSI1 (secondary sensor). */

/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on DSI/YCBCR buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl05_00Sensor_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl05_01Sensor_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl05_02Sensor_Set(struct dth_element *elem, void *value);

/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for CSI buses.
 *  @param[in] data data needed to manage CSI buses.
 *  @param[in] sensorId Id of the sensor.
 */
void tatl05_03Init_Csi_Data(DTH_SENSOR_DATA *data, u32 sensorId);

#endif /* TATL05SENSOR_H_ */



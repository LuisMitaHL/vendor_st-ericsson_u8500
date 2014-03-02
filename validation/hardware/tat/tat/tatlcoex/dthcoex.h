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
#ifndef DTHCOEX_H_
#define DTHCOEX_H_

#include <dthsrvhelper/dthsrvhelper.h>

/* ----------------------------------------------------------------------- */
/* Public functions                                                        */
/* ----------------------------------------------------------------------- */

int dth_init_service();

/* Function tide to I2C buses management */
/****************************************/
int DthCoex_I2C_Set(struct dth_element *elem, void *Value);
int DthCoex_I2C_Get(struct dth_element *elem, void *Value);
int DthCoex_I2C_Exec(struct dth_element *elem);

/* Function tide to DSI and YCBCR buses management */
/***************************************************/
int DthCoex_Display_Set(struct dth_element *elem, void *Value);
int DthCoex_Display_Get(struct dth_element *elem, void *Value);
int DthCoex_Display_Exec(struct dth_element *elem);

/* Function tide to PWM buses management */
/****************************************/
int DthCoex_Pwm_Set(struct dth_element *elem, void *Value);
int DthCoex_Pwm_Get(struct dth_element *elem, void *Value);
int DthCoex_Pwm_Exec(struct dth_element *elem);

/* Function tide to CSI buses management */
/****************************************/
int DthCoex_Sensor_Set(struct dth_element *elem, void *Value);
int DthCoex_Sensor_Get(struct dth_element *elem, void *Value);
int DthCoex_Sensor_Exec(struct dth_element *elem);

/* Function tide to MSP, VIB and DMIC buses management */
/******************************************************/
int DthCoex_Audio_Set(struct dth_element *elem, void *Value);
int DthCoex_Audio_Get(struct dth_element *elem, void *Value);
int DthCoex_Audio_Exec(struct dth_element *elem);

/* Function tide to MC buses management */
/******************************************************/
int DthCoex_Memory_Set(struct dth_element *elem, void *Value);
int DthCoex_Memory_Get(struct dth_element *elem, void *Value);
int DthCoex_Memory_Exec(struct dth_element *elem);

/* Function tide to MOD_I2C buses management */
/******************************************************/
int DthCoex_ModI2C_Set(struct dth_element *elem, void *Value);
int DthCoex_ModI2C_Get(struct dth_element *elem, void *Value);
int DthCoex_ModI2C_Exec(struct dth_element *elem);

/* Function tide to APE_I2C buses management */
/******************************************************/
int DthCoex_ApeI2C_Set(struct dth_element *elem, void *Value);
int DthCoex_ApeI2C_Get(struct dth_element *elem, void *Value);
int DthCoex_ApeI2C_Exec(struct dth_element *elem);

/* Function tide to Connectivity buses management */
/******************************************************/
int DthCoex_Conn_Set(struct dth_element *elem, void *Value);
int DthCoex_Conn_Get(struct dth_element *elem, void *Value);
int DthCoex_Conn_Exec(struct dth_element *elem);

/* Function tide to victim management */
/******************************************************/
int DthCoex_Victim_Set(struct dth_element *elem, void *Value);
int DthCoex_Victim_Get(struct dth_element *elem, void *Value);
int DthCoex_Victim_Exec(struct dth_element *elem);

#endif /* DTHCOEX_H_ */


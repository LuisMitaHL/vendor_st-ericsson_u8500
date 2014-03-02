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

#include "tatpws.h"

int dth_init_service()
{
	int result = 0;

	PWS_SYSLOG(LOG_DEBUG, "dth_init_service of tatpws");

	/* initialize the gpio data structure */
	tatpws1_GPIO_Settings_Init();
	tatpws4_ABGPIO_Settings_Init();
	/*Register Action arborescence */
	tatlpws_register_dth_struct();

	return result;
}


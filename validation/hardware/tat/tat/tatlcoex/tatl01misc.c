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

#include "tatlcoex.h"

int tatl01_00Manage_Processes(pthread_t *thread, void *threadData, void *function, DTH_COEX_ORDER order, DTH_COEX_STATE state)
{
	int vl_error = 0;

	if ((order == START) && (state == DEACTIVATED)) {

		pthread_attr_t thread_attr;

		SYSLOGSTR(LOG_DEBUG, "Start process\n");

		vl_error = pthread_attr_init(&thread_attr);
		if (vl_error != 0) {
			SYSLOG(LOG_ERR, "pthread_attr_init: %s\n", strerror(vl_error));
			return vl_error ;
		}

		vl_error = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
		if (vl_error != 0)  {
			SYSLOG(LOG_ERR, "pthread_attr_setdetachstate: %s\n", strerror(vl_error));
			return vl_error ;
		}

		vl_error = pthread_create(thread, &thread_attr, function, threadData);
		if (vl_error < 0)  {
			SYSLOG(LOG_ERR, "pthread_create: %s\n", strerror(vl_error));
			return vl_error ;
		}

		pthread_attr_destroy(&thread_attr);
		sleep(2);
	} else if ((order == STOP) && (state == ACTIVATED)) {
		SYSLOGSTR(LOG_DEBUG, "Waiting for process termination\n");
		sleep(3);
	}

	return vl_error ;
}

int tatl01_01Set_Video_Parameter(int id, int type, void *value)
{
	int vl_error = 0;
	struct dth_element elem ;

	elem.type = type ;
	elem.user_data = id ;

	vl_error = Dth_ActVIDEOParam_Set(&elem, value);

	return  vl_error ;
}



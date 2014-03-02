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

#include "dthadc.h"
#include "tatladc.h"

/*Init default value for ADC */
/****************************/
int dth_init_service()
{
	/* Warning : Default ADC cyclicRead file path value is set here it doesn't come from dthladc.xml */
	char *default_ADCfileStoragePath = "/var/local/tat/tatladc/adc.txt";

	ADC_string[IN_FILESTORAGE]=(char*)calloc(strlen(default_ADCfileStoragePath)+1,sizeof(char));
	strncpy((char*)ADC_string[IN_FILESTORAGE],(const char*)default_ADCfileStoragePath,strlen(default_ADCfileStoragePath));

	int result = 0;
	result = tatladc_register_dth_struct();
	return result ;
}

int Dth_ActADC_exec(struct dth_element *elem)
{
	if (elem == NULL)
		return -1;

	return dthl41_00ActADC_exec(elem);
}

int Dth_ActADCParam_Set(struct dth_element *elem, void *value)
{
	return dthl41_02ActADCParam_Set(elem, value);
}

int Dth_ActADCParam_Get(struct dth_element *elem, void *value)
{
	return dthl41_03ActADCParam_Get(elem, value);
}

int Dth_ActTEST_exec(struct dth_element *elem)
{
	if (elem == NULL)
		return 0;
	return 0;
}

int Dth_ActTESTParam_Set(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return 0;

	return 0;
}

int Dth_ActTESTParam_Get(struct dth_element *elem, void *value)
{
	if ((elem == NULL) && (value == NULL))
		return 0;

	*((uint8_t *)value) = 0x1;
	return 0;
}


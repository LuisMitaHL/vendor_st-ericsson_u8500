/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/

#include <string.h>
#include "audiocodec_debug.h"

#ifdef ANDROID
	#include <cutils/log.h>
#else
	#include <stdio.h>
#endif

static char fcn_name[LOG_LENGTH_MAX];
static bool fcn_name_initialized = false;

const char* get_fcn_name(const char* fcn_name_long)
{
	int len = strlen(fcn_name_long);

	if (!fcn_name_initialized)
		for (int i = 0; i<LOG_LENGTH_MAX; i++)
			fcn_name[i]=(char)0;

	const char* pEnd = strchr(fcn_name_long , '(');
	if (pEnd == NULL)
		return fcn_name_long;

	const char* pStart = pEnd;
	while ((*pStart != ' ') && (pStart > fcn_name_long))
		pStart--;

	strncpy(fcn_name, pStart + 1, pEnd - pStart - 1);
	fcn_name[pEnd - pStart] = '\0';

	return fcn_name;
}


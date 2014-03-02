/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include "omxilosaldatetimeimpl.h"
using namespace omxilosalservices;

#include <los/api/los_api.h>
#include <sys/time.h>
#include <time.h>

void OmxIlOsalDateTime::OmxIlOsalGetDateTime(OmxIlOsalDateTimeType *pt)
{
	  struct timeval tv;
	  struct tm* ptm;
	  ptm = NULL;

	  gettimeofday (&tv, NULL);
	  ptm = localtime (&tv.tv_sec);

	  ptm->tm_year+=1900;                       //since ptm->tm_year contains years since 1900
	  ptm->tm_mon+=1;                           //since ptm->tm_mon contains months since january
	  pt->year    = (OMX_U16)ptm->tm_year;
	  pt->month   = (OMX_U8) ptm->tm_mon;
	  pt->day     = (OMX_U8) ptm->tm_mday;
	  pt->hour    = (OMX_U8) ptm->tm_hour;
	  pt->minutes = (OMX_U8) ptm->tm_min;
	  pt->seconds = (OMX_U8) ptm->tm_sec;
}


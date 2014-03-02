/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SENSORSECONDARY_H
#define _SENSORSECONDARY_H

#include "sensor.h"

/***********************/
/* Internal data types */
/***********************/


/*****************************/
/* Sensor object declaration */
/*****************************/

/** \brief abstract class implementing Sensor specifics parameters */
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CSensorSecondary);
#endif
class CSensorSecondary : public CSensor
{
    public:
        CSensorSecondary();                                   /*Standard Constructor*/
       	t_sint32 GetSensorId(void);
       	const char*  GetPeName(const t_uint32 aAddr);
        virtual ~CSensorSecondary();                       /*Standard Destructor*/
};
#endif

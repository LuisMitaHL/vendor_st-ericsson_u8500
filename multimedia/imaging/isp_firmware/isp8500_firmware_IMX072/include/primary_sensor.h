/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _SENSORPRIMARY_H
#define _SENSORPRIMARY_H

#include "sensor.h"

/***********************/
/* Internal data types */
/***********************/


/*****************************/
/* Sensor object declaration */
/*****************************/

/** \brief abstract class implementing Sensor specifics parameters */
class CSensorPrimary : public CSensor
{
    public:
       CSensorPrimary();
       t_sint32 GetSensorId(void);
       virtual const char*  GetPeName(const t_uint32 aAddr);
       virtual ~CSensorPrimary();
};
#endif

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
* \file    lightsensor_api.h
* \brief   
* \author  ST-Ericsson
*
*/
#ifndef __LIGHTSENSOR_API_H__
#define __LIGHTSENSOR_API_H__

#include <stdint.h>

/*
 * Get the light level that shall be used by the camera
 * data = light level used, in lux (lux)
 * return = 0 if successful
 * All calculations done in the context of this function to be used
 * for camera
*/
int als_getlux(float *data);

/*
 * To enable the sensor before calling get lux
 * return = 0 if successful
 * All calculations done in the context of this function to be used
 * for camera
*/
int als_enable();

/*
 * To disable the sensor after calling get lux
 * return = 0 if successful
 * All calculations done in the context of this function to be used
 * for camera
*/
int als_disable();


#endif /* __LIGHTSENSOR_API_H__ */

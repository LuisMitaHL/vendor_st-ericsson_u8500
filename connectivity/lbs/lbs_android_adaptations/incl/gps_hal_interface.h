/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
*
* gpsclient.h
* Date: 31-01-2011
* Author: Sanju Marikkar
* Email : sanju.marikkar@stericsson.com
*/
/*!
 * \file     gps_hal_interface.h
 */
#ifdef AGPS_ANDR_GINGER_FTR

#include <errno.h>
#include <termios.h>
#ifndef ARM_LINUX_AGPS_FTR
#include "android_log.h"
//#include <cutils/properties.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include "hardware/gps.h"



#ifndef _GPS_HAL_INTERFACE_H
#define _GPS_HAL_INTERFACE_H


const GpsInterface* gps_get_hardware_interface(struct gps_device_t *test);

#endif /* #ifndef _GPS_HAL_INTERFACE_H */
#endif
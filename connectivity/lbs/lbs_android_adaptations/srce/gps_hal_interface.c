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
 * \file     gps_hal_interface.c
 */

#ifdef AGPS_ANDR_GINGER_FTR
#include "gps_hal_interface.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define  LOG_TAG  "GpshalInterface"


static int close_gps(struct gps_device_t *dev)
{
    if (dev)
    {
        free(dev);
        dev = NULL;
    }
    return 0;
}

static int Gps_Open_Callback(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    struct gps_device_t *dev = malloc(sizeof(struct gps_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->get_gps_interface = gps_get_hardware_interface;
    dev->common.close = (int (*)(struct hw_device_t *)) close_gps;

    *device = (struct hw_device_t*)dev;
    return 0;
}

static struct hw_module_methods_t gps_module_methods = {
    .open = Gps_Open_Callback
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = GPS_HARDWARE_MODULE_ID,
    .name = "GPS module",
    .author = "ST-Ericsson",
    .methods = &gps_module_methods,
};

#endif

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ite_pageelements.h"

#include <los/api/los_api.h>
#include "ite_pipeinfo.h"
#include "ite_vpip.h"
#include "ite_framerate.h"
#include "ite_host2sensor.h"

extern ts_sensInfo SensorsInfo[2];

struct s_i2c_data {
  t_uint16 addr;
  t_uint16 val;
};

/* the following patch manually sets DigitalGain for RGB channels to 0x100 (1), because Ewarp sets it to
   incorrect values (null-terminated),
  0x3822=3: magic! (provided to us undocumented and uncommented...) */
t_uint16 patchS861[][2] = {{0x3822, 3}, {0x104, 1}, {0x20E,1}, {0x20F,0}, {0x210,1}, {0x211,0}, {0x212,1}, {0x213,0}, {0x214,1}, {0x215,0}, {0x104, 0}, {0, 0}};



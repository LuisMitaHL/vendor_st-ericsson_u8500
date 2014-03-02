/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
* Copyright (C) ST-Ericsson 2009
*
* <Mandatory description of the content (short)>
* Author: <Name and/or email address of author>
*/
#ifndef _CAM_DRV_PLATFORM_HARDCODING_H_
#   define _CAM_DRV_PLATFORM_HARDCODING_H_

#   define U16_ANALOG_GAIN_x1000   (1000)
#   define U16_DIGITAL_GAIN_x1000  (1000)
#   define SENSOR_USED_555         (1)
#   if SENSOR_USED_555
#      define SENSOR_anc_lines_top    3
#      define SENSOR_anc_lines_bottom 0

//    #   define SENSOR_black_pixels_capability  CAM_DRV_SENS_NO_BLACK_PIXELS
#      define SENSOR_black_pixels_left    0
#      define SENSOR_black_pixels_right   0
#      define SENSOR_black_pixels_top     0
#      define SENSOR_black_pixels_bottom  0

//    #   define SENSOR_dummy_pixel_position  CAM_DRV_SENS_DUMMY_PIXELS_NONE
#      define SENSOR_dummy_pixels_left    0
#      define SENSOR_dummy_pixels_right   8
#      define SENSOR_dummy_pixels_top     1
#      define SENSOR_dummy_pixels_bottom  0
#   endif
#endif //_CAM_DRV_PLATFORM_HARDCODING_H_


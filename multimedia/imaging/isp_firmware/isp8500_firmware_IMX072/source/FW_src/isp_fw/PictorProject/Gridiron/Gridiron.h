/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 

/**
 \file Gridiron.h
 Filename : Gridiron.h
 Creation Date: 26th March 2010
 COPYRIGHT       : STMicroelectronics, 2005

    This document contains proprietary and confidential information of the
                         STMicroelectronics Group.

 \brief  This file is a part of the release code. It contains the data
        structures, macros, enums and function declarations used by the module.
 \ingroup Gridiron
*/

#ifndef _GRIDIRON_H_
#define _GRIDIRON_H_

#   include "Gridiron_PlatformSpecific.h"
#   include "GenericFunctions.h"
#define ENABLE_GRIDIRON_TRACES (0)

#define DEFAULT_GRIDIRON_GRID_WIDTH     (66)
#define DEFAULT_GRIDIRON_GRID_HEIGHT    (50)
#define DEFAULT_GRIDIRON_ENABLE_FLAG  (Flag_e_FALSE)


#define DEFAULT_GRIDIRON_CR_HSTART    (0)
#define DEFAULT_GRIDIRON_CR_VSTART    (0)
#define DEFAULT_GRIDIRON_IM_HSIZE       (0)
#define DEFAULT_GRIDIRON_IM_VSIZE       (0)
#define DEFAULT_GRIDIRON_SENSOR_HSIZE   (0)

/// Default Value for LiveCast
#   define DEFAULT_GRIDIRON_LIVECAST   (2.0)

#define DEFAULT_GRIDIRON_SENSOR_HSCALE (0)
#define DEFAULT_GRIDIRON_SENSOR_VSCALE  (0)
#define DEFAULT_GRIDIRON_SENSOR_HSCALE_X256 (ConvertFloatToUnsigned_4_DOT_8_format(DEFAULT_GRIDIRON_SENSOR_HSCALE))
#define DEFAULT_GRIDIRON_SENSOR_VSCALE_X256  (ConvertFloatToUnsigned_4_DOT_8_format(DEFAULT_GRIDIRON_SENSOR_VSCALE))
#define DEFAULT_GRIDIRON_PHASE    (0)
#define DEFAULT_GRIDIRON_GRID_PITCH    (0)
#define DEFAULT_GRIDIRON_E_FLAG_CAST0  (Flag_e_FALSE)
#define DEFAULT_GRIDIRON_E_FLAG_CAST1  (Flag_e_FALSE)
#define DEFAULT_GRIDIRON_E_FLAG_CAST2  (Flag_e_FALSE)
#define DEFAULT_GRIDIRON_E_FLAG_CAST3  (Flag_e_FALSE)

#define DEFAULT_LOWER_BOUND_LOG_GRID_PITCH_V1 (4)
#define DEFAULT_UPPER_BOUND_LOG_GRID_PITCH_V1 (5)
#define DEFAULT_LOWER_BOUND_LOG_GRID_PITCH_V2 (4)
#define DEFAULT_UPPER_BOUND_LOG_GRID_PITCH_V2 (7)

#define K_MAX_CAST_COUNT (4)



/// Default Value for Cast Position0
#   define DEFAULT_GRIDIRON_CAST_POSITION0  (4.5)

/// Default Value for Cast Position1
#   define DEFAULT_GRIDIRON_CAST_POSITION1 (3.5)

/// Default Value for Cast Position2
#   define DEFAULT_GRIDIRON_CAST_POSITION2 (2.5)

/// Default Value for Cast Position3
#   define DEFAULT_GRIDIRON_CAST_POSITION3 (1.5)

/// Default Value for LogGridPitch
#   define DEFAULT_GRIDIRON_LOGGRIDPITCH   (4)

/// Default Value for fPhase
#   define DEFAULT_GRIDIRON_FPHASE   (0.0)

/// Default Value for u8_Phase
#   define DEFAULT_GRIDIRON_U8_PHASE   (0)

/// Default Value for Horizontal Flip
#   define DEFAULT_GRIDIRON_HOR_FLIP   (Flag_e_FALSE)

/// Default Value for Vertical Flip
#   define DEFAULT_GRIDIRON_VER_FLIP   (Flag_e_FALSE)

/// Default Value for number of active cast positions
#   define DEFAULT_ACTIVE_REFERENCE_CAST_COUNT (4)

/// e_GridPixelOrder
#   define DEFAULT_GRID_PIXEL_ORDER (PixelOrder_e_GrR_BGb)

void ComputeGridironCastsInterpolation(uint32_t max_casts_count, uint32_t cur_casts_count, const float_t * casts_references_array, float_t live_cast, 
                                                                  uint32_t log_grid_pitch, bool_t * casts_enables_array, uint32_t* phase, float* status_live_cast_ptr) TO_EXT_DDR_PRGM_MEM;

/// Structure to program LoggridPitch
/*
typedef struct
{
    // LOGRID_PITCH
    uint8_t u8_Loggridpitch;
} GridironInput_ts;
*/
#endif // _GRIDIRON_H_


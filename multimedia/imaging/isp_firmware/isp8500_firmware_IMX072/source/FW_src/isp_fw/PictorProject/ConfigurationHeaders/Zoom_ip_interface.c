/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
\file Zoom_ip_interface.c
\brief Implements the input interface functions required by Zoom module
\ingroup Frame Dimension
*/
#include "FrameDimension_op_interface.h"
#include "Zoom_IPInterface.h"

/**
   \fn void Zoom_Request(uint16_t u16_DesiredFOVX, uint16_t u16_DesiredFOVY, uint16_t u16_RequiredLineLength, int16_t s16_CenterOffsetX, int16_t s16_CenterOffsetY)
   \brief The function allows the zoom module to query a frame dimension corresponding to a particular zoom setting

    \param  u16_DesiredFOVX: The desired FOVX
            u16_DesiredFOVY: The desired FOVY
            u16_RequiredLineLength: The required line length
            s16_CenterOffsetX: X offset to the center of the imaging array
            s16_CenterOffsetY: Y offset to the center of the imaging array
            e_Flag_ScaleLineLengthForDerating: If set to Flag_e_TRUE then line length required is scaled to compensate for derating factor
                                               If set to Flag_e_FALSE then line length required is NOT scaled to compensate for derating factor


     \return    FrameDimensionRequestStatus_e_Accepted: If the frame dimension request can be fulfilled immediately.
                FrameDimensionRequestStatus_e_Denied: If the frame dimension request cannot be fulfilled at all.
                FrameDimensionRequestStatus_e_ReProgeamRequired: If the frame dimension request can be met but a reprogram of the sensor is required.
     \ingroup Frame Dimension
     \callgraph
     \callergraph
   */
uint8_t
Zoom_Request(
uint16_t    u16_DesiredFOVX,
uint16_t    u16_DesiredFOVY,
uint16_t    u16_RequiredLineLength,
uint16_t    u16_MaxLineLength,
int16_t     s16_CenterOffsetX,
int16_t     s16_CenterOffsetY,
uint8_t     e_Flag_ScaleLineLengthForDerating,
uint8_t     e_Flag_EnableModeSelection)
{
/*
* Cases to be considered:
    * 1. Output size not possible?
    * 2. Frame Rate not possible?
    * 3. FOV not possible?
    * 4. Line length not possible?
*
* The place should get best possible configuration from details structure, no need to call config API's
*/
    return
        (
            lla_abstractionFrameDimension_RequestFrameConfig(
            u16_DesiredFOVX,
            u16_DesiredFOVY,
            u16_RequiredLineLength,
            u16_MaxLineLength,
            s16_CenterOffsetX,
            s16_CenterOffsetY,
            e_Flag_ScaleLineLengthForDerating,
            e_Flag_EnableModeSelection)
        );

}


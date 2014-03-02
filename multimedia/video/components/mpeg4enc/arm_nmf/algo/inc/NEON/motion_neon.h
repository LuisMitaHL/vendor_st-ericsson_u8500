/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef MOTION_NEON_H
#define MOTION_NEON_H

#ifdef ALTERNATE_SUB_SAMPLING
ui32_t mp4e_ui_CalculateSAD_HalfADD_AlternateSubSampling_NEON(ui8_t * pui8_v1,ui8_t * pui8_v2, ui8_t * pui8_v3, ui32_t ui_swidth);
ui32_t mp4e_ui_CalculateSAD_AlternateSubSampling_NEON(ui8_t * pui8_v1,ui8_t * pui8_v2, ui32_t ui_swidth);
ui32_t mp4e_ui_CalculateSADWithHalfPixel_AlternateSubSampling_NEON(ui8_t * pui8_v1,ui8_t * pui8_v2, ui8_t * pui8_v3, ui32_t ui_swidth);
#else
ui32_t mp4e_ui_CalculateSAD_NEON(ui8_t * pui8_v1,ui8_t * pui8_v2, ui32_t ui_swidth);
ui32_t mp4e_ui_CalculateSADWithHalfPixel_NEON(ui8_t * pui8_v1,ui8_t * pui8_v2, ui8_t * pui8_v3, ui32_t ui_swidth);
ui32_t mp4e_ui_CalculateSAD_HalfADD_NEON(ui8_t * pui8_v1,ui8_t * pui8_v2, ui8_t * pui8_v3, ui32_t ui_swidth);
#endif
void mp4e_CopyArea_NEON(ui8_t * pui8_dest,ui8_t * pui8_source,ui32_t ui_swidth,pixel ui_start_coord);
#ifdef MBTYPE_USE_MAD
int mp4e_ChooseMode_NEON (unsigned char *curr, int pels, int x_pos, int y_pos, int min_SAD);
#endif
#endif

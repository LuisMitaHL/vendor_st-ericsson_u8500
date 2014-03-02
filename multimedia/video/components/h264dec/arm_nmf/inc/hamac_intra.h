/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef HAMAC_INTRA_H
#define HAMAC_INTRA_H

#include "types.h"
#include "hamac_types.h"


#define Intra16x16_Vertical 0
#define Intra16x16_Horizontal 1
#define Intra16x16_DC 2
#define Intra16x16_Plane 3

#define Chroma_DC 0
#define Chroma_Horizontal 1
#define Chroma_Vertical 2
#define Chroma_Plane 3

#define Intra4x4_Vertical 0
#define Intra4x4_Horizontal 1
#define Intra4x4_DC 2
#define Intra4x4_Diag_Down_Left 3
#define Intra4x4_Diag_Down_Right 4
#define Intra4x4_Vertical_Right 5
#define Intra4x4_Horizontal_Down 6
#define Intra4x4_Vertical_Left 7
#define Intra4x4_Horizontal_Up 8


t_uint16 DecodeIntra(t_hamac_mb_info *p_hamac, t_sint16 *p_decoded_mb);

#endif

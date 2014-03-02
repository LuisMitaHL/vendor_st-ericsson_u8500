/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef IDCT_NEON_H_
#define IDCT_NEON_H_

#include "ast_types.h"


void mp4e_IDCT_Luma_Intra_NEON  (si32_t Width, si16_t *data, ui8_t* Image_p); 
void mp4e_IDCT_Chroma_Intra_NEON(si32_t Width, si16_t *data_U, si16_t *data_V, ui8_t* Image_p); 
void mp4e_IDCT_Luma_Inter_NEON  (si32_t Width, si16_t *data, ui8_t* Image_p); 
void mp4e_IDCT_Chroma_Inter_NEON(si32_t Width, si16_t* Block_U, si16_t* Block_V, ui8_t* Image_p);

#endif /* IDCT_NEON_H_ */

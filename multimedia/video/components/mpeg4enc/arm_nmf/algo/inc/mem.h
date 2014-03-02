/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef  HMEMH
#define  HMEMH

#include <stdio.h>    /* fopen, fread, fwrite, fclose, printf */
#include <stdlib.h>   /* m-c-alloc; free; exit */

#include "ast_types.h"

ui32_t mp4e_ui32_AllocateMemoryBuffer(ui32_t ui32_image_size);
ui8_t *mp4e_pui8_AlignMemoryBytePtrTo64Bytes(si32_t si32_addr);


#endif /* HMEMH */ 

















/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __CGPSPATCH_HIC__
#define __CGPSPATCH_HIC__
/**
* \file cgpspatch.hic
* \date 06/04/2012
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all defines used by cgps2patch.c.\n
*
*/


#define K_CGPS_MAX_FSPATCH_NAME_LEN      (50)

#ifdef CGPS_USE_COMPRESSED_PATCH
#define K_CGPS_ITEMS_PER_MESSAGE           16
#endif


/**
* \enum e_cgps_PatchUpload_Source
*
* define the current source for ME patch code.
*
*/
typedef enum
{
    K_CGPS_PATCHUPLOAD_FS     =   0x00,
    K_CGPS_PATCHUPLOAD_FS_ERR =   0x01,
    K_CGPS_PATCHUPLOAD_STATIC =   0x02,
}e_cgps_PatchUpload_Source;



#endif /* __CGPSPATCH_HIC__ */

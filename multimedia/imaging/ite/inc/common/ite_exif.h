/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/* \brief   Exif 
*/


#ifndef _ITE_EXIF_H_
#define _ITE_EXIF_H_

#include "OMX_Types.h"


typedef enum
{
    ExifErrorNone    = 0,
    ExifErrorParsing  = -1,
    ExifErrorTagNotFound  = -2,
    ExifErrorBadParameter  = -3,
}TExifError;

typedef enum
{
    ExifT_BYTE		= 1,  
    ExifT_ASCII		= 2,  
    ExifT_SHORT		= 3,  
    ExifT_LONG		= 4,  
    ExifT_RATIONAL	= 5,  
    ExifT_UNDEFINED	= 7,  
    ExifT_SLONG		= 9,  
    ExifT_SRATIONAL	= 10,  
    ExifT_MAX	= 11,  
}TExifIDType;
#define ITE_EXIF_SIZE_COMMENT 64
typedef struct
    {
    unsigned short tag ; 
    TExifIDType type;
	unsigned short count;
	char comment[ITE_EXIF_SIZE_COMMENT];
  }TExifCheckElement;

#define ITE_EXIF__TIFF_HEADER_BYTE_ORDER_LITTLE_ENDIAN (0x4949)
#define ITE_EXIF__TIFF_HEADER_BYTE_ORDER_BIG_ENDIAN (0x4D4D)
#define ITE_EXIF__TIFF_HEADER_BYTE_42 (0x2A)
#define ITE_EXIF__TIFF_HEADER_BYTE_OFFSET_OF_IFD (8)

#define ITE_EXIF__APP1MARKER_MARKER_PREFIX (0xFF)
#define ITE_EXIF__APP1MARKER_APP1 (0xE1)
#define ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_0 'E'
#define ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_1 'x'
#define ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_2 'i'
#define ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_3 'f'
#define ITE_EXIF__APP1MARKER_EXIF_IDENTIFIER_CODER_4 0

typedef struct
    {
    unsigned char MarkerPrefix ; 
    unsigned char APP1;
	unsigned short LengthOfField;
	unsigned char ExifIndentifierCoder[6];
	unsigned short TiffHeader_ByteOrder;
 	unsigned short TiffHeader_filed_42;
 	unsigned short TiffHeader_OffsetOfIFD_0;
 	unsigned short TiffHeader_OffsetOfIFD_1;
  }TExifAPP1Marker;

#ifdef __cplusplus
extern "C"
{
#endif

TExifError ite_parse_exif(void *pExifData);

#ifdef __cplusplus
}
#endif

#endif // _ITE_EXIF_H_ 

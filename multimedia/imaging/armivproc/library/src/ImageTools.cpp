/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "SharedTools.h"
#include "ImageTools.h"
#include "ImageBuffer.h"
#include "ImageConverter.h"

#if (defined(_DEBUG) && defined(_MSC_VER))
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

const _sTableImageFormat SupportedFormat[]=
{
	// enum                             Pix depth    Stride1              CCCode
	//                           Is YUV      planes       Stride2                     Format description
	//                                          Stride0        Extension                                           Tech description
	{ eImageFormat_UnknowFormat, false, 0.0, 0, 0.0, 0.0, 0.0, ""        , ""        , "Unknow"                   , "Unknow" },
	{ eImageFormat_YUV420mb    , true , 1.5, 1, 1.5, 0.0, 0.0, "420mb"   , "xxxx"    , "YUV 420 macro block"      , "3 planes Y,Cr,Cb (1:4:4)" },
	{ eImageFormat_YUV422mb    , true , 2.0, 1, 2.0, 0.0, 0.0, "422mb"   , "xxxx"    , "YUV 422 macro block"      , "3 planes Y,Cr,Cb (1:2:2)" },
	{ eImageFormat_RGB565      , false, 2.0, 1, 2.0, 0.0, 0.0, "Rgb565"  , "R565"    , "RGB 565"                  , "1 plane RGB 16 bits" },
	{ eImageFormat_RGB888      , false, 3.0, 1, 3.0, 0.0, 0.0, "Rgb888"  , "RGB888"  , "RGB 888"                  , "1 plane RGB 3B/p" },
	{ eImageFormat_BGR888      , false, 3.0, 1, 3.0, 0.0, 0.0, "Bgr888"  , "BGR888"  , "BGR 888"                  , "1 plane BGR 3B/p" },
	{ eImageFormat_ARGB8888    , false, 4.0, 1, 4.0, 0.0, 0.0, "Argb8888", "ARGB8888", "ARGB 8888"                , "1 plane ARGB 4B/p" },
	{ eImageFormat_BGRA8888    , false, 4.0, 1, 4.0, 0.0, 0.0, "Bgra8888", "BGRA8888", "BGRA 8888"                , "1 plane BGRA 4B/p" },
	{ eImageFormat_YUV888i     , true , 3.0, 1, 3.0, 0.0, 0.0, "yuv888i" , "yuv888"  , "yuv 888i"                 , "1 plane YUV (8:8:8)" },
	{ eImageFormat_YUV844i     , true , 2.0, 1, 2.0, 0.0, 0.0, "yuv844i" , "yuv888"  , "yuv 844i"                 , "1 plane YUV (8:4:4)" },

	{ eImageFormat_YUV420_blue , true , 1.5, 3, 1.0, .25, .25, "I420"    , "I420"    , "YUV 420 blue first"       , "3 planes Y,Cb,Cr (1:4:4) 1.5B/p" },
	{ eImageFormat_YUV420_red  , true , 1.5, 3, 1.0, .25, .25, "YV12"    , "YV12"    , "YUV 420 red first"        , "3 planes Y,Cr,Cb (1:4:4) 1.5B/p" },
	{ eImageFormat_YUV422_blue , true , 2.0, 3, 1.0, 0.5, 0.5, "422pb"   , "xxxx"    , "YUV422 planar blue first" , "3 planes, 2B/p, Y,Cb,Cr (1:2:2)" },
	{ eImageFormat_YUV422_red  , true , 2.0, 3, 1.0, 0.5, 0.5, "422p"    , "xxxx"    , "YUV422 planar red first"  , "3 planes, 2B/p, Y,Cr,Cb (1:2:2)" },
	{ eImageFormat_NV21        , true , 1.5, 2, 1.0, 0.5, 0.0, "NV21"    , "NV21"    , "NV21 -> Y-V/U"            , "2 planes Y, V/U (1:4:4) 1.5B/p" },
	{ eImageFormat_NV12        , true , 1.5, 2, 1.0, 0.5, 0.0, "NV12"    , "NV12"    , "NV12 -> Y-U/V"            , "2 planes Y, V/U (1:4:4) 1.5B/p" },
	{ eImageFormat_YCbYCr      , true , 2.0, 1, 2.0, 0.0, 0.0, "YCbYCr"  , "YCbYCr"  , "YUV422i Y Cb Y Cr"        , "1 plane interleaved, 16 bits/p, Y0-Cb-Y1-Cr" },
	{ eImageFormat_CrYCbY      , true , 2.0, 1, 2.0, 0.0, 0.0, "CrYCbY"  , "CrYCbY"  , "YUV422i Cr Y Cb Y"        , "1 plane interleaved, 16 bits/p, Cr-Y0-Cb-Y1" },
	{ eImageFormat_YCrYCb      , true , 2.0, 1, 2.0, 0.0, 0.0, "YCrYCb"  , "YCrYCb"  , "YUV422i Y Cr Y Cb"        , "1 plane interleaved, 16 bits/p, Y0-Cr-Y1-Cb" },
	{ eImageFormat_CbYCrY      , true , 2.0, 1, 2.0, 0.0, 0.0, "CbYCrY"  , "CbYCrY"  , "YUV422i Cb Y Cr Y"        , "1 plane interleaved, 16 bits/p, Cb-Y0-Cr-Y1" },
	{ eImageFormat_YUV_blue    , true , 3.0, 3, 1.0, 1.0, 1.0, "YUVpb"   , "xxxx"    , "YUV planar blue first"    , "3 planes, 3B/p, Y,Cb,Cr" },
	{ eImageFormat_YUV_red     , true , 3.0, 3, 1.0, 1.0, 1.0, "YUVp"    , "xxxx"    , "YUV planar red first"     , "3 planes, 3B/p, Y,Cr,Cb" },
	{ eImageFormat_Raw8        , false, 1.0, 1, 1.0, 0.0, 0.0, "raw8"    , "xxxx"    , "raw8 camera format"       , "1 planes, 1B/p, " },
	{ eImageFormat_Raw12       , false, 1.5, 1, 1.5, 0.0, 0.0, "raw12"   , "xxxx"    , "raw12 camera format"      , "1 planes, 1.5B/p, " },
	{ eImageFormat_JPEG        , true , 0.0, 1, 0.0, 0.0, 0.0, "jpg"     , "jpg"     , "jpeg format"              , "to be filled" },
	{ eImageFormat_PNG         , true , 0.0, 1, 0.0, 0.0, 0.0, "png"     , "png"     , "png format"               , "to be filled" },
	{ eImageFormat_Bmp         , false, 3.0, 1, 3.0, 0.0, 0.0, "bmp"     , "bmp"     , "BMP format"               , "header=54 bytes" },

	{ eImageFormat_ChromaOnly_1_1, true, 1.0, 1, 1.0, 0.0, 0.0, "bmp"     , "xxxx"     , "Chroma only"               , "Chroma only 1 by 1" },
	{ eImageFormat_ChromaOnly_1_2, true, 2.0, 1, 2.0, 0.0, 0.0, "bmp"     , "xxxx"     , "Chroma only"               , "Chroma only 1 by 2" },
	{ eImageFormat_ChromaOnly_1_3, true, 3.0, 1, 3.0, 0.0, 0.0, "bmp"     , "xxxx"     , "Chroma only"               , "Chroma only 1 by 3" },
	{ eImageFormat_ChromaOnly_1_4, true, 4.0, 1, 4.0, 0.0, 0.0, "bmp"     , "xxxx"     , "Chroma only"               , "Chroma only 1 by 4" },

	{ eImageFormat_YUV420mb_oldendianness /*  devrait etre 422? */
	                           , true , 2.0, 3, 1.0, 0.5, 0.5, "420mb0" , "xxxx" , "YUV 420 mblock old endian" , "3 planes Y,Cr,Cb (1:4:4)" },
};


const size_t sizeof_SupportedFormat=sizeof(SupportedFormat)/sizeof(SupportedFormat[0]);


const _sTableFormatFromName SupportedFormatFromName[]=
{ // This table gives correspondance between Supported format enum and string found in the name
	{ eImageFormat_YUV420_blue   , "YUV420" },
	{ eImageFormat_YUV420_blue   , "I420" },
	{ eImageFormat_YUV420_red    , "YV12" },
	{ eImageFormat_YUV420_red    , "YUV" },

	{ eImageFormat_NV21          , "nv21"},
	{ eImageFormat_NV12          , "nv12"},

	{ eImageFormat_YCbYCr        , "YCbYCr"},
	{ eImageFormat_CrYCbY        , "CrYCbY"},
	{ eImageFormat_YCrYCb        , "YCrYCb"},
	{ eImageFormat_CbYCrY        , "CbYCrY"},

	{ eImageFormat_YUV422_blue   , "YUV422pb"},
	{ eImageFormat_YUV422_red    , "YUV422p"},
	{ eImageFormat_YUV422_blue   , "422pb"},
	{ eImageFormat_YUV422_red    , "422p"},

	{ eImageFormat_YUV_blue      , "YUVpb"},
	{ eImageFormat_YUV_red       , "YUVp"},
// a valider
	{ eImageFormat_YUV422mb      , "yuv422mb"},
	{ eImageFormat_YUV422mb      , "422mb"},

	{ eImageFormat_YUV420mb      , "420mb"},

	{ eImageFormat_CrYCbY        , "YUV422"},
	{ eImageFormat_CrYCbY        , "I422"},
	{ eImageFormat_CrYCbY        , "422itld"},

	{ eImageFormat_RGB565        , "rgb565"},
	{ eImageFormat_RGB565        , "rvb565"},

	{ eImageFormat_RGB888        , "rgb888"},
	{ eImageFormat_RGB888        , "rvb888"},
	{ eImageFormat_RGB888        , "rgb"},
	{ eImageFormat_RGB888        , "rvb"},

	{ eImageFormat_BGR888        , "bgr"},
	{ eImageFormat_BGR888        , "bvr"},

	{ eImageFormat_ARGB8888      , "argb8888"},
	{ eImageFormat_ARGB8888      , "arvb8888"},
	{ eImageFormat_ARGB8888      , "argb"},
	{ eImageFormat_ARGB8888      , "arvb"},

	{ eImageFormat_BGRA8888      , "bgra"},
	{ eImageFormat_BGRA8888      , "bvra"},

	{ eImageFormat_YUV888i       , "yuv888i"},
	{ eImageFormat_YUV844i       , "yuv844i"},

	{ eImageFormat_YUV420mb_oldendianness , "yuv420mb0"},
	{ eImageFormat_YUV420mb_oldendianness , "YUV422MB0"}, // A vérifier pour l'église

	{ eImageFormat_Raw8           , "raw8"},
	{ eImageFormat_Raw8           , "raw"},
	{ eImageFormat_Raw12          , "raw12"},

	{ eImageFormat_JPEG           , "jpg"},
	{ eImageFormat_Bmp            , "bmp"},
	{ eImageFormat_PNG            , "png"},

};

const size_t sizeof_FormatFromName=SIZEOF_ARRAY(SupportedFormatFromName);

/**
Search the format of the image using its name
*/
int GetFormatFromName(const char *name)
//*************************************************************************************
{
	unsigned int index;
	for (index=0; index < (sizeof(SupportedFormatFromName)/ sizeof(SupportedFormatFromName[0])); ++index)
	{
		if (strcasecmp(name, SupportedFormatFromName[index].name)==0)
			return(SupportedFormatFromName[index].format);
	}
	return(eImageFormat_UnknowFormat);
}


int DiffPixels(const _tPixel *pPixel1, const _tPixel *pPixel2, int Tolerance)
//*************************************************************************************
{ // Check if pixels are equals with a given tolerance
	int diff;
	diff = ((*pPixel1)[0] - (*pPixel2)[0]);
	if (diff > 0)
	{
		if (diff > Tolerance)
			return(1);
	}
	else
	{
		if ((-diff) > Tolerance)
			return(-1);
	}
	diff = ((*pPixel1)[1] - (*pPixel2)[1]);
	if (diff > 0)
	{
		if (diff > Tolerance)
			return(2);
	}
	else
	{
		if ((-diff) > Tolerance)
			return(-2);
	}
	diff = ((*pPixel1)[2] - (*pPixel2)[2]);
	if (diff > 0)
	{
		if (diff > Tolerance)
			return(3);
	}
	else
	{
		if ((-diff) > Tolerance)
			return(-3);
	}
	return(0);
}


const _sTableImageFormat* GetFormatStruct(enum_ImageFormat format)
//*************************************************************************************
{
	unsigned int index;
	for (index=0; index < sizeof_SupportedFormat; ++index)
	{
		if (SupportedFormat[index].FormatIndex==format)
			return(&SupportedFormat[index]);
	}
	return(NULL);
}

/**
Compute the amount of memory needed for a number of pixel for a given format
\param [in] format the enumeration format
\param [in] NbPix the number of pixels
*/
size_t ComputeNeededBufferSize(enum_ImageFormat format, size_t NbPix)
//*************************************************************************************
{
	const _sTableImageFormat* pFormat=GetFormatStruct(format);
	if (pFormat==NULL)
	{
		ASSERT(0);
		return(0);
	}
	return(size_t(NbPix *pFormat->PixelDepth));
}

/**
Compute the amount of memory needed for a number of pixel for a given format
\param [in] format description
\param [in] Stride for the given plane (usually 0)
\param [in] plane number (usually 0)
\return     The apparent width
*/
size_t ComputeApparentWidth(const _sTableImageFormat& Format, size_t Stride, const unsigned int plane)
//*************************************************************************************
{
	if ((plane >= Format.NbPlanes) || (plane > 2))
	{
		_fnReportError(0, "invalid  plane nb=%d max is %d --> found width is %g", plane, Format.NbPlanes);
		ASSERT(0);
		return(0);
	}
	double dPlaneDepth=*( &Format.Stride0+plane);

	size_t width= (size_t)(Stride/ dPlaneDepth);
	if ( ((size_t)(width* dPlaneDepth)) != Stride)
	{
		_fnReportError(0, "invalid stride %d for plane depth=%g --> found width is %g", Stride, dPlaneDepth, Stride/ dPlaneDepth );
		ASSERT(0);
	}
	return(width);
}

int ReadImageFile(const char *aInputName, const _sImageFormat &InputFormat, _tImageBuffer &InputBuffer)
//***************************************************************************************
{ // Read a file
	int res=S_OK;
	if (aInputName==NULL)
		return(eError_NoFilename);
	//unsigned int error=0;
	size_t Read_size=ComputeNeededBufferSize((enum_ImageFormat)InputFormat.Format, InputFormat.SizeX * InputFormat.SizeY);
	const _sTableImageFormat* pFormat_In=GetFormatStruct( (enum_ImageFormat)InputFormat.Format);
	if (pFormat_In==NULL)
	{
		return(eError_InputFileFormatUnknown);
	}
	// Fill The Source strutur
	FILE *fp=fopen(aInputName, "rb");
	if (fp==NULL)
	{
		return(eError_InputFileNotFound);
	}
	//Seek at the beginning of the image
	if ( fseek(fp, InputFormat.HeaderSize, SEEK_SET)!=0 )
	{ // seek on the stream to jump to right part of image
	}

	_tImageBuffer InputFile;
	InputFile.Allocate(Read_size);
	unsigned char *pBuffer=InputFile.GetBufferAddr();
	if (pBuffer==NULL)
	{
		fclose(fp);
		return(eError_CannotAllocateMemory);
	}
	//Bufferize the file image
	size_t nbr=fread(pBuffer, 1, Read_size, fp);
	fclose(fp);
	if (nbr!=Read_size)
	{
		_fnReportError(eError_NotEnoughPixels, "Not enough pixels read=%u required=%u", nbr, Read_size);
		res=eError_NotEnoughPixels;
		return(res);
	}

	InputFile.SetFormat(InputFormat.Format);
	InputFile.SetImageSize(InputFormat.SizeX, InputFormat.SizeY);
	res= ReadImage(InputFile, InputBuffer, InputFormat.Format);

	if (res!=S_OK)
	{
		_fnReportError(eError_NotEnoughPixels, "Read image fails, error code is %d", res);
		return(res);
	}
	return(res);
}

int CheckResolution_Downmscale(const size_t In_X, const size_t In_Y, const size_t Out_X, const size_t Out_Y)
//*************************************************************************************************************
{ // Chec wether output resolution is compatible with downscale
	// Return <0 when NOT
	// Return 0 or 90 when ok. 0 meens Ok without rotation, 90° ok but only if rotation
	if ( (In_X >= Out_X) && (In_Y >= Out_Y))
		return(0);
	if ( (In_X >= Out_Y) && (In_Y >= Out_X))
		return(90);
	return(-1); // Not compatible
}

//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
/**
Reset all parameter to default value
*/
void _sImageFormat::Reset()
//*************************************************************************************
{
	SizeX       = 0;
	SizeY       = 0;
	HeaderSize  = 0;
	Format      = eImageFormat_UnknowFormat;
	pUserPtr    = NULL;
	pImageStream= NULL; // Not used anymore
	ImageIndex  = 0;
}

typedef struct
{
	int val;
	const char *str;
} _tEnumToString;

#define ENUM_AND_STRING(val) { val, #val}

const _tEnumToString ErrorToStringTable[]=
{
	ENUM_AND_STRING(eError_UnsupportedInputFormat),
	ENUM_AND_STRING(eError_UnsupportedOutputFormat),
	ENUM_AND_STRING(eError_CannotAllocateMemory),
	ENUM_AND_STRING(eError_AttachedBufferTooSmall),
	ENUM_AND_STRING(eError_CannotCreateOutputFile),
	ENUM_AND_STRING(eError_CannotWriteOutputFile),
	ENUM_AND_STRING(eError_WrongResolutionOrFormat),
	ENUM_AND_STRING(eError_CannotAllocateBuffer),
	ENUM_AND_STRING(eError_CannotSetOutputSize),
	ENUM_AND_STRING(eError_InputFileNotFound),
	ENUM_AND_STRING(eError_InputFileFormatUnknown),
	ENUM_AND_STRING(eError_WrongResolution),
	ENUM_AND_STRING(eError_WrongResolutionForMB),
	ENUM_AND_STRING(eError_WrongOuptutBufferSize),
	ENUM_AND_STRING(eError_CannotSetImageSize),
	ENUM_AND_STRING(eError_CannotCreateImageBuffer),
	ENUM_AND_STRING(eError_CannotSaveImage),
	ENUM_AND_STRING(eError_CannotRescaleImage),
	ENUM_AND_STRING(eError_WrongInputFormat),
	ENUM_AND_STRING(eError_WrongOutputFormat),
	ENUM_AND_STRING(eError_UnsupportedRotationFormat),
	ENUM_AND_STRING(eError_UnsupportedMirroringFormat),
	ENUM_AND_STRING(eError_UnsupportedPixelDepth),
	ENUM_AND_STRING(eError_UnsupportedDownscaleFormat),

	ENUM_AND_STRING(eError_UnsupportedRotationCase),
	ENUM_AND_STRING(eError_UpScalingNotsupported),
	ENUM_AND_STRING(eError_NoInputStream),
	ENUM_AND_STRING(eError_NoOutputBufferProvided),

	ENUM_AND_STRING(eError_DownscaleFailed),
	ENUM_AND_STRING(eError_MirroringFailed),

	ENUM_AND_STRING(eError_NoFilename),
	ENUM_AND_STRING(eError_NotEnoughPixels),
	ENUM_AND_STRING(eError_ImageIsNull),
	ENUM_AND_STRING(eError_InvalidImageSize),
	ENUM_AND_STRING(eError_UnknowFormat),
	
	ENUM_AND_STRING(eError_NoFormatDuringSetImageSize),
	ENUM_AND_STRING(eError_StrideTooSmallDuringSetImageSize),
	ENUM_AND_STRING(eError_BufferTooSmallDuringSetImageSize),
	ENUM_AND_STRING(eError_InvalidOutputSize),
	ENUM_AND_STRING(eError_ImcompatibleFormat),
	{ eError_ImcompatibleInputFormat, "Imcompatible Input Format"},
	{ eError_ImcompatibleOutputFormat, "Imcompatible Output Format"},

	ENUM_AND_STRING(eError_ASSERTION),
	ENUM_AND_STRING(eError_LastEnumError_Image),

	ENUM_AND_STRING(eWarning_OperationCanceled),
};

const char * GetStringForArmIVProcError(int val)
//*************************************************************************************
{
	const _tEnumToString *pTable=ErrorToStringTable;
	for (int i=SIZEOF_ARRAY(ErrorToStringTable); i>0; --i)
	{
		if (val == pTable ->val)
			return(pTable ->str);
		++pTable;
	}
	return("unknow error");
}

/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMAGETOOLS_H_
#define _IMAGETOOLS_H_

enum enumError_Image
{
	eError_UnsupportedInputFormat=-1000,
	eError_UnsupportedOutputFormat,
	eError_CannotAllocateMemory,
	eError_AttachedBufferTooSmall,
	eError_CannotCreateOutputFile,
	eError_CannotWriteOutputFile,
	eError_WrongResolutionOrFormat,
	eError_CannotAllocateBuffer,
	eError_CannotSetOutputSize,
	eError_InputFileNotFound,
	eError_InputFileFormatUnknown,
	eError_WrongResolution,
	eError_WrongResolutionForMB,
	eError_WrongOuptutBufferSize,
	eError_CannotSetImageSize,
	eError_CannotCreateImageBuffer,
	eError_CannotSaveImage,
	eError_CannotRescaleImage,
	eError_WrongInputFormat,
	eError_WrongOutputFormat,
	eError_UnsupportedRotationFormat,
	eError_UnsupportedMirroringFormat,
	eError_UnsupportedPixelDepth,
	eError_UnsupportedDownscaleFormat,

	eError_UnsupportedRotationCase,
	eError_UpScalingNotsupported,
	eError_NoInputStream,
	eError_NoOutputBufferProvided,

	eError_DownscaleFailed,
	eError_MirroringFailed,

	eError_NoFilename,
	eError_NotEnoughPixels,
	eError_ImageIsNull,

	eError_InvalidImageSize,
	eError_UnknowFormat,
	eError_NoFormatDuringSetImageSize,
	eError_StrideTooSmallDuringSetImageSize,
	eError_BufferTooSmallDuringSetImageSize,
	eError_InvalidOutputSize,
	eError_ImcompatibleFormat,
	eError_ImcompatibleInputFormat,
	eError_ImcompatibleOutputFormat,

	eError_ASSERTION,
	eError_LastEnumError_Image,

	eWarning_OperationCanceled =1,
};


typedef struct 
{
	int          FormatIndex;
	bool         IsYUV;
	double       PixelDepth;
	unsigned int NbPlanes;
	double       Stride0;
	double       Stride1;
	double       Stride2;
	const char  *extension; //Default one
	const char  *cccode;
	const char  *name;
	const char  *description;
} _sTableImageFormat;

typedef struct 
{
	int format;
	const char *name;
} _sTableFormatFromName;

enum enum_ImageFormat
{
	eImageFormat_UnknowFormat =0,
	eImageFormat_YUV420_blue, //3 planes Y, Cb, Cr 1:4:4 ccode= I420 (blue first)
	eImageFormat_YUV420_red , //3 planes Y, Cr, Cb 1:4:4 ccode= YV12 (red first)

	eImageFormat_CbYCrY, // 1 plane, 16 bits/pixel
	eImageFormat_YCbYCr, // 1 plane, 16 bits/pixel
	eImageFormat_CrYCbY, // 1 plane, 16 bits/pixel
	eImageFormat_YCrYCb, // 1 plane, 16 bits/pixel

	eImageFormat_NV21, // YUV in 2 planes: Y and V/U interleaved in 2*2
	eImageFormat_NV12, // YUV in 2 planes: Y and U/V interleaved in 2*2

	eImageFormat_YUV422_blue, //3 planes Y, Cb, Cr 1:2:2 (blue first)
	eImageFormat_YUV422_red,  //3 planes Y, Cr, Cb 1:2:2  (red first)

// a verifier
	eImageFormat_YUV420mb,  // Marcro block
	eImageFormat_YUV422mb,
	eImageFormat_YUV420mb_oldendianness, // Macro block with old endianness
	// eImageFormat_YUV422Int, //interleaved
	eImageFormat_RGB565,
	eImageFormat_RGB888,
	eImageFormat_BGR888,
	eImageFormat_ARGB8888,
	eImageFormat_BGRA8888,
	eImageFormat_JPEG,
	eImageFormat_Bmp,
	eImageFormat_PNG,

	eImageFormat_YUV888i, // YUV interleaved 8:8:8
	eImageFormat_YUV844i, // YUV interleaved 8:4:4

	eImageFormat_YUV_blue, //3 planes Y, Cb, Cr 1:1:1  (blue first)
	eImageFormat_YUV_red,  //3 planes Y, Cr, Cb 1:1:1  (red first)

	eImageFormat_Raw8,
	eImageFormat_Raw12,

	eImageFormat_ChromaOnly_1_1,
	eImageFormat_ChromaOnly_1_2,
	eImageFormat_ChromaOnly_1_3,
	eImageFormat_ChromaOnly_1_4,

	eImageFormat_LastEnum, // Last value

	eImageFormat_Default=eImageFormat_YUV420_blue,
};

class  ImageStream;

IMAGECONVERTER_API typedef struct 
{
	unsigned int   SizeX;
	unsigned int   SizeY;
	size_t         HeaderSize;
	unsigned int   Format;
	void          *pUserPtr;
	ImageStream   *pImageStream;
	unsigned int   ImageIndex; // When multiple image gives current one in regard with header size
	IMAGECONVERTER_API void Reset();
} _sImageFormat;

enum eEnumRotation
{
	eRotation_None= 0,
	eRotation_90  = 90,
	eRotation_180 = 180,
	eRotation_270 = 270,
	eRotation_NotDefined = -1,
};

enum eEnumMirror
{
	eMirror_None=0,
	eMirror_X,
	eMirror_Y,
	eMirror_XY,
};

IMAGECONVERTER_API int GetFormatFromName(const char *name);


extern IMAGECONVERTER_API const _sTableImageFormat SupportedFormat[];
extern IMAGECONVERTER_API const size_t sizeof_SupportedFormat;

typedef struct 
{
	eEnumRotation Rotation;
	bool          NeedDownScaling;
} _sProcessingImage;


IMAGECONVERTER_API int ProcessImage(const _sImageFormat &Input, const _sImageFormat &Output, eEnumRotation aRot=eRotation_None);

IMAGECONVERTER_API const _sTableImageFormat* GetFormatStruct(enum_ImageFormat format);
size_t IMAGECONVERTER_API ComputeNeededBufferSize(enum_ImageFormat format, size_t NbPix);
size_t IMAGECONVERTER_API ComputeApparentWidth(const _sTableImageFormat& Format, size_t Stride, const unsigned int plane=0);


typedef  unsigned char  _tPixel[3];
IMAGECONVERTER_API int DiffPixels(const _tPixel *pPixel1, const _tPixel *pPixel2, int Tolerance);

IMAGECONVERTER_API extern const size_t sizeof_FormatFromName;
IMAGECONVERTER_API extern const _sTableFormatFromName SupportedFormatFromName[] ;

enum enum_RGBOffset
{
	eRGBOffset_Blue = 0,
	eRGBOffset_Green= 1,
	eRGBOffset_Red  = 2,
};

class _tImageBuffer;
IMAGECONVERTER_API int ReadImageFile(const char *aInputName, const _sImageFormat &InputFormat, _tImageBuffer &InputBuffer);

IMAGECONVERTER_API int CheckResolution_Downmscale(const size_t In_X, const size_t In_Y, const size_t Out_X, const size_t Out_Y);

IMAGECONVERTER_API const char * GetStringForArmIVProcError(int val);

#endif // End of #ifndef _IMAGETOOLS_H_

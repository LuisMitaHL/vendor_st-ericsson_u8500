/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMAGECONVERTER_H
#define _IMAGECONVERTER_H

int DefaultReportError(int error_code, const char *Fmt, ...);

struct sYUV
{
	unsigned char Luminance;
	unsigned char Chroma_Blue; // U = Cb
	unsigned char Chroma_Red;  // V = Cr
};

enum
{ //Offset for YUV and YCbCr
	eOffset_Y = 0,
	//doesn't work with android
	//eOffset_Cb= offsetof(sYUV, Chroma_Blue),
	//eOffset_Cr= offsetof(sYUV, Chroma_Red),
	eOffset_Cb= 1,
	eOffset_Cr= 2,
	eOffset_U = eOffset_Cb,
	eOffset_V = eOffset_Cr,
};


struct sRGB
{
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
};

struct sBGR
{
	unsigned char Red;
	unsigned char Green;
	unsigned char Blue;
};

struct sARGB
{
	unsigned char Blue;
	unsigned char Green;
	unsigned char Red;
	unsigned char Alpha;
};

struct sBGRA
{
	unsigned char Alpha;
	unsigned char Green;
	unsigned char Blue;
	unsigned char Red;
};

typedef union 
{
	struct 
	{
		unsigned char Cb;
		unsigned char Y0;
		unsigned char Cr;
		unsigned char Y1;
	} CbYCrY;
	struct 
	{
		unsigned char Y0;
		unsigned char Cb;
		unsigned char Y1;
		unsigned char Cr;
	} YCbYCr;
	struct 
	{
		unsigned char Cr;
		unsigned char Y0;
		unsigned char Cb;
		unsigned char Y1;
	} CrYCbY;
	struct 
	{
		unsigned char Y0;
		unsigned char Cr;
		unsigned char Y1;
		unsigned char Cb;
	} YCrYCb;
} _uCbYCr;

enum
{
	eAlpha_Default=255, // Means transparent 255-> Transparent  0 -> Opaque
};

#if 1
typedef struct sRGB InternalStructRGB;
#define InternalFormatRGB eImageFormat_RGB888
#define InternalFormatYUV eImageFormat_YUV888i
#else
//old 
typedef struct sBGR InternalStructRGB;
#define InternalFormatRGB eImageFormat_RGB888
#endif
const unsigned int InternalPixelDepth=3;

class  ArmIVProc_Engine;

int IMAGECONVERTER_API Write_YUV422       (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat=eImageFormat_YUV420_blue);
int IMAGECONVERTER_API Write_YUV420       (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat=eImageFormat_YUV420_blue);
int IMAGECONVERTER_API Write_YUV422_mb    (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
int IMAGECONVERTER_API Write_YUV420_mb    (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
int IMAGECONVERTER_API Write_NV21_NV12    (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
int IMAGECONVERTER_API Write_RGB565       (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
int IMAGECONVERTER_API Write_RGB888       (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
int IMAGECONVERTER_API Write_ARGB8888     (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat=eImageFormat_ARGB8888   );
int IMAGECONVERTER_API Write_YCbYCr       (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format=eImageFormat_CbYCrY      );
int IMAGECONVERTER_API Write_Raw          (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format                          );
int IMAGECONVERTER_API Write_YUV_planar   (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format                          );
int IMAGECONVERTER_API Write_YUV422_planar(const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format                          );

//To be removed
int IMAGECONVERTER_API Write_YUV420_mb_nostride(const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0);
int IMAGECONVERTER_API Write_YUV420_mb_old     (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0);
int IMAGECONVERTER_API Write_YUV422_mb_old     (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0);
//end to be removed
int IMAGECONVERTER_API WriteImage(/*const*/ _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat);


IMAGECONVERTER_API int RotateImage(const _tImageBuffer &InBuffer, _tImageBuffer &OutBuffer, eEnumRotation aRot);
IMAGECONVERTER_API int MirrorImage(_tImageBuffer &OutBuffer, eEnumMirror mirror);
IMAGECONVERTER_API int RescaleImage(const _tImageBuffer &InBuffer, _tImageBuffer &OutBuffer);

#define MACRO_BLOCK_SIZE 16U


IMAGECONVERTER_API int Read_NV12_21       (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format                          );
IMAGECONVERTER_API int Read_YUV420        (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat=eImageFormat_YUV420_blue);
IMAGECONVERTER_API int Read_RGB565        (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
IMAGECONVERTER_API int Read_RGB888        (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
IMAGECONVERTER_API int Read_ARGB8888      (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat=eImageFormat_ARGB8888   );
IMAGECONVERTER_API int Read_YCbYCr        (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat=eImageFormat_CbYCrY     );
IMAGECONVERTER_API int Read_YUV422mb      (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
IMAGECONVERTER_API int Read_YUV420mb      (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0             );
IMAGECONVERTER_API int Read_YUV422_planar (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat                         );
IMAGECONVERTER_API int Read_YUV_planar    (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat                         );
IMAGECONVERTER_API int Read_Raw           (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat                         );
IMAGECONVERTER_API int ReadChromaOnly     (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat                         );

IMAGECONVERTER_API int ReadRaw_YUV422mb_oldendianess (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/=0);

IMAGECONVERTER_API int ReadImage(const _tImageBuffer &InputBuffer, _tImageBuffer &Buffer, int aFormat);


typedef struct 
{
	enum_ImageFormat Format;
	int (*Write_fn) (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format);
	int (*Read_fn)  (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format);
	int (*Rotate_fn) (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format);
	int (*Mirror_fn) (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format);
} _sSupportedRWFormat;

extern IMAGECONVERTER_API const _sSupportedRWFormat Supported_RWFormat[];
extern IMAGECONVERTER_API const size_t sizeof_Supported_RWFormat;

IMAGECONVERTER_API const _sSupportedRWFormat *GetRWFunction(int aFormat);


class _tImageBuffer;
IMAGECONVERTER_API void Convert_YUV_to_RGB(const unsigned char *YUV, unsigned char *RVB);
IMAGECONVERTER_API void Convert_RGB_to_YUV(const unsigned char *RVB, unsigned char *YUV);
IMAGECONVERTER_API int  Convert_RGB_to_YUV(_tImageBuffer &Buffer);
IMAGECONVERTER_API int  Convert_YUV_to_RGB(_tImageBuffer &Buffer);

#endif // End of #ifndef _IMAGECONVERTER_H

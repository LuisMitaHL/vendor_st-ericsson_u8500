/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include "SharedTools.h"
#include "ImageTools.h"
#include "ImageBuffer.h"
#include "ImageStream.h"
#include "ImageConverter.h"

#if (defined(_DEBUG) && defined(_MSC_VER))
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/** Format References 
Format are describes in STn8500A0_SIA_HW_specification document: 
https://codex.cro.st.com/plugins/docman/?group_id=696&action=show&id=42321
https://codex.cro.st.com/plugins/docman/?group_id=696&action=show&id=79905
old
YUV422_itld : Cr0; Y0;Cb0;  Y1 ( p304 ) YCbCr 4:2:2 Interleaved Raster format
YUV420  I420    YCbCr 4:2:0 Planar Raster format (page 300)
YCbCrmb  4:2:x semi-planar MacroBlock format

YCbCr 4:2:2 Interleaved Raster format (page 309) 1 plane, 2b/p, OMX_COLOR_FormatCbYCrY->eImageFormat_CbYCrY 
	resizing algo:
	http://www.codinghorror.com/blog/2007/07/better-image-resizing.html
	http://www.compuphase.com/graphic/scale.htm
*/

const _sSupportedRWFormat Supported_RWFormat[]=
{
	{ eImageFormat_NV21       , Write_NV21_NV12    , Read_NV12_21      , NULL, NULL },
	{ eImageFormat_NV12       , Write_NV21_NV12    , Read_NV12_21      , NULL, NULL },
	{ eImageFormat_YUV420_blue, Write_YUV420       , Read_YUV420       , NULL, NULL },
	{ eImageFormat_YUV420_red , Write_YUV420       , Read_YUV420       , NULL, NULL },
	{ eImageFormat_YUV422_blue, Write_YUV422_planar, Read_YUV422_planar, NULL, NULL },
	{ eImageFormat_YUV422_red , Write_YUV422_planar, Read_YUV422_planar, NULL, NULL },
	{ eImageFormat_YCbYCr     , Write_YCbYCr       , Read_YCbYCr       , NULL, NULL },
	{ eImageFormat_CrYCbY     , Write_YCbYCr       , Read_YCbYCr       , NULL, NULL },
	{ eImageFormat_YCrYCb     , Write_YCbYCr       , Read_YCbYCr       , NULL, NULL },
	{ eImageFormat_CbYCrY     , Write_YCbYCr       , Read_YCbYCr       , NULL, NULL },
	{ eImageFormat_YUV420mb   , Write_YUV420_mb    , Read_YUV420mb     , NULL, NULL },
	{ eImageFormat_YUV422mb   , Write_YUV422_mb    , Read_YUV422mb     , NULL, NULL },
	{ eImageFormat_BGR888     , Write_RGB888       , Read_RGB888       , NULL, NULL },
	{ eImageFormat_RGB888     , Write_RGB888       , Read_RGB888       , NULL, NULL },

	{ eImageFormat_BGRA8888   , Write_ARGB8888     , Read_ARGB8888     , NULL, NULL },
	{ eImageFormat_ARGB8888   , Write_ARGB8888     , Read_ARGB8888     , NULL, NULL },

	{ eImageFormat_RGB565     , Write_RGB565       , Read_RGB565       , NULL, NULL },
	{ eImageFormat_YUV_blue   , Write_YUV_planar   , Read_YUV_planar   , NULL, NULL },
	{ eImageFormat_YUV_red    , Write_YUV_planar   , Read_YUV_planar   , NULL, NULL },


	{ eImageFormat_YUV888i    , Write_YCbYCr       , Read_YCbYCr       , NULL, NULL },
	{ eImageFormat_YUV844i    , Write_YCbYCr       , Read_YCbYCr       , NULL, NULL },

	{ eImageFormat_Raw8       , Write_Raw          , Read_Raw          , NULL, NULL },
	{ eImageFormat_Raw12      , NULL               , NULL              , NULL, NULL },

	{ eImageFormat_ChromaOnly_1_1, NULL            , ReadChromaOnly    , NULL, NULL },
	{ eImageFormat_ChromaOnly_1_2, NULL            , ReadChromaOnly    , NULL, NULL },
	{ eImageFormat_ChromaOnly_1_3, NULL            , ReadChromaOnly    , NULL, NULL },
	{ eImageFormat_ChromaOnly_1_4, NULL            , ReadChromaOnly    , NULL, NULL },

	{ eImageFormat_YUV420mb_oldendianness   , NULL , ReadRaw_YUV422mb_oldendianess, NULL, NULL },
// Supported by QT not natively
	{ eImageFormat_JPEG                     , NULL , NULL              , NULL, NULL },
	{ eImageFormat_Bmp                      , NULL , NULL              , NULL, NULL },
	{ eImageFormat_PNG                      , NULL , NULL              , NULL, NULL },
};

const size_t sizeof_Supported_RWFormat=SIZEOF_ARRAY(Supported_RWFormat);

const _sSupportedRWFormat *GetRWFunction(int aFormat)
//*************************************************************************************
{ // Return the structur for RW 
	unsigned int index;
	for (index=0; index < SIZEOF_ARRAY(Supported_RWFormat); ++index)
	{
		if (Supported_RWFormat[index].Format==aFormat)
			return(Supported_RWFormat+index);
	}
	return(NULL);
}

int WriteImage(/*const*/ _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{
#if 1
	const _sSupportedRWFormat *pRWFormat=GetRWFunction(aFormat);
	if ( (pRWFormat !=NULL) && (pRWFormat->Write_fn!=NULL) )
	{
		if (InputBuffer.IsYUV()==false)
		{ //For rgb format must check if destination is also rgb. If not convert
			const _sTableImageFormat* pOutFormat=GetFormatStruct((enum_ImageFormat)aFormat);
			ASSERT(pOutFormat!=NULL);
			if(aFormat && (pOutFormat->IsYUV==true))
			{
				Convert_RGB_to_YUV((_tImageBuffer &)InputBuffer);
			}
		}
		return(pRWFormat->Write_fn (InputBuffer, OutBuffer, aFormat));
	}
	else
	{
		_fnReportError(eError_UnsupportedOutputFormat, "Unsupported output format %d", aFormat);
		ASSERT(0);
		return( eError_UnsupportedOutputFormat);
	}
#else
	switch(aFormat)
	{
	case eImageFormat_NV21:
		return(Write_NV21_NV12 (InputBuffer, OutBuffer, eImageFormat_NV21));
	case eImageFormat_NV12:
		return(Write_NV21_NV12 (InputBuffer, OutBuffer, eImageFormat_NV12));
	case eImageFormat_YUV420_blue: //3planes 
	case eImageFormat_YUV420_red:  //3planes 
		return(Write_YUV420 (InputBuffer, OutBuffer, aFormat));
	case eImageFormat_YUV422_blue:
	case eImageFormat_YUV422_red:
		return(Write_YUV422_planar(InputBuffer, OutBuffer, aFormat));
	case eImageFormat_YUV_blue:
	case eImageFormat_YUV_red:
		return(Write_YUV_planar(InputBuffer, OutBuffer, aFormat));
	case eImageFormat_YCbYCr:
	case eImageFormat_CrYCbY:
	case eImageFormat_YCrYCb:
	case eImageFormat_CbYCrY:
	case eImageFormat_YUV888i:
	case eImageFormat_YUV844i:
		return(Write_YCbYCr (InputBuffer, OutBuffer, aFormat)); // All YCrYCb family

	case eImageFormat_YUV420mb:
		return(Write_YUV420_mb(InputBuffer, OutBuffer));
	case eImageFormat_YUV422mb:
		return(Write_YUV422_mb(InputBuffer, OutBuffer));
	case eImageFormat_RGB888:
	case eImageFormat_BGR888:
		return(Write_RGB888(InputBuffer, OutBuffer, aFormat));
	case eImageFormat_RGB565:
		return(Write_RGB565(InputBuffer, OutBuffer));
	case eImageFormat_Raw8:
	case eImageFormat_Raw12:
		return(Write_Raw(InputBuffer, OutBuffer, aFormat));

	case eImageFormat_ARGB8888:
	case eImageFormat_BGRA8888:
		return(Write_ARGB8888(InputBuffer, OutBuffer, aFormat));


	case eImageFormat_YUV420mb_oldendianness: // Macro block with old endianness
	case eImageFormat_JPEG:

	default:
		ASSERT(0);
		_fnReportError(eError_UnsupportedOutputFormat, "Unsupported output format %d", aFormat);
		return( eError_UnsupportedOutputFormat);
	}
#endif
}

int ReadImage(const _tImageBuffer &InputBuffer, _tImageBuffer &OutputBuffer, int aFormat)
//*************************************************************************************
{
	int res=S_OK;
#if 1
	const _sSupportedRWFormat *pRWFormat=GetRWFunction(aFormat);
	if ( (pRWFormat !=NULL) && (pRWFormat->Read_fn!=NULL) )
	{
		res=pRWFormat->Read_fn (InputBuffer, OutputBuffer, aFormat);
		return(res);
	}
	else
	{
		_fnReportError(eError_UnsupportedOutputFormat, "Unsupported output format %d", aFormat);
		ASSERT(0);
		return( eError_UnsupportedOutputFormat);
	}
#else
	switch (aFormat)
	{
	case eImageFormat_YUV420_blue:
	case eImageFormat_YUV420_red:
		res=Read_YUV420 (InputBuffer, OutputBuffer, aFormat);
		break;
	case eImageFormat_NV21:
	case eImageFormat_NV12:
		res=Read_NV12_21 (InputBuffer, OutputBuffer, aFormat);
		break;
	case eImageFormat_YCbYCr:
	case eImageFormat_CrYCbY:
	case eImageFormat_YCrYCb:
	case eImageFormat_CbYCrY:
	case eImageFormat_YUV888i:
	case eImageFormat_YUV844i:
		res=Read_YCbYCr (InputBuffer, OutputBuffer, aFormat);
		break;
	case eImageFormat_YUV422_blue:
	case eImageFormat_YUV422_red:
		res=Read_YUV422_planar(InputBuffer, OutputBuffer, aFormat);
		break;
	case eImageFormat_YUV_blue:
	case eImageFormat_YUV_red:
		res=Read_YUV_planar(InputBuffer, OutputBuffer, aFormat);
		break;

	case eImageFormat_YUV422mb:
		res=Read_YUV422mb (InputBuffer, OutputBuffer);
		break;
	case eImageFormat_YUV420mb:
		res=Read_YUV420mb (InputBuffer, OutputBuffer);
		break;
	case eImageFormat_YUV420mb_oldendianness:
		res=ReadRaw_YUV422mb_oldendianess (InputBuffer, OutputBuffer);
		break;
	case eImageFormat_RGB565:
		res=Read_RGB565 (InputBuffer, OutputBuffer);
		break;
		
	case eImageFormat_Raw8:
	case eImageFormat_Raw12:
		res=Read_Raw (InputBuffer, OutputBuffer, aFormat);
		break;

	case eImageFormat_BGR888:
	case eImageFormat_RGB888:
		res=Read_RGB888 (InputBuffer, OutputBuffer, aFormat);
		break;
	case eImageFormat_BGRA8888:
	case eImageFormat_ARGB8888:
		res=Read_ARGB8888 (InputBuffer, OutputBuffer, aFormat);
		break;

	case eImageFormat_JPEG:
		ASSERT(0);
		return(eError_UnsupportedInputFormat);
	case eImageFormat_UnknowFormat:
	default:
		_fnReportError(res, "Invalid format %d", aFormat);
		ASSERT(0);
		res=eError_InputFileFormatUnknown;
		break;
	}

	DEBUG_ONLY( const _sTableImageFormat* pFormat=GetFormatStruct((enum_ImageFormat)OutputBuffer.GetFormat()) );
	ASSERT(pFormat);
	ASSERT(pFormat && (OutputBuffer.bIsYUV==pFormat->IsYUV) );
	CHECK_ERROR(res);
	return(res);
#endif
}


int RescaleImage(const _tImageBuffer &InBuffer, _tImageBuffer &OutBuffer)
//*************************************************************************************
{ // Down scaling image, if
	double dStepX, dStepY;
	if ((OutBuffer.GetWidth()==0) || (OutBuffer.GetHeight()==0))
		return(eError_WrongResolution);
	dStepX= (double)InBuffer.GetWidth() / (double)OutBuffer.GetWidth();
	dStepY= (double)InBuffer.GetHeight()/ (double)OutBuffer.GetHeight();

	OutBuffer.SetFormat(InBuffer.GetFormat()); //Recopy flags
	const unsigned int SizeElem=(unsigned int)InBuffer.GetPixelDepth();
	if ((SizeElem!=3) && (SizeElem!=4))
	{
		ASSERT( (SizeElem==3) || (SizeElem==4));
		return(eError_UnsupportedDownscaleFormat);
	}
	unsigned int iStepX, iStepY;
	unsigned int Pixel[4];
	unsigned char *Ptr;
	if (dStepX > 1.)
		iStepX= (unsigned int)dStepX;
	else
		iStepX= 1;

	if (dStepY > 1.)
		iStepY= (unsigned int)dStepY;
	else
		iStepY= 1;
	const unsigned int Weight=iStepY*iStepX;
	unsigned int iX, iY;
	unsigned int iShift;
	unsigned char *pOut=OutBuffer.GetBufferAddr();
	bool bOptimized=true;
	if ( (IsPowerOfTwo(Weight, iShift)==false) || (bOptimized==false) )
	{
		for (unsigned int i=0; i< OutBuffer.GetHeight(); ++i)
		{
			iY=(unsigned int)(i*dStepY);
			for (unsigned int j=0; j< OutBuffer.GetWidth(); ++j)
			{
				iX=(unsigned int)(j*dStepX);
				Pixel[0]=0; Pixel[1]=0; Pixel[2]=0; Pixel[3]=0;//reset all
				//Now sum the rectangle
				Ptr=InBuffer.GetBufferAddr()+ (iY* InBuffer.GetStrideInByte()) + (iX* SizeElem);
				for (unsigned int k=0; k< iStepY; ++k)
				{
					for (unsigned int l=0; l< iStepX; ++l)
					{ //Sum one line
						Pixel[0]+=Ptr[0]; Pixel[1]+=Ptr[1]; Pixel[2]+=Ptr[2]; //reset all
						Ptr+=SizeElem;
					}
					Ptr+=InBuffer.GetStrideInByte()- (iStepX*SizeElem); //Go next start line
				}
				pOut[0]=(unsigned char )(Pixel[0]/ Weight);
				pOut[1]=(unsigned char )(Pixel[1]/ Weight);
				pOut[2]=(unsigned char )(Pixel[2]/ Weight);
				pOut+=SizeElem;
			}
		}
	}
	else
	{ //Power of two don't divide, shift
		for (unsigned int i=0; i< OutBuffer.GetHeight(); ++i)
		{
			iY=(unsigned int)(i*dStepY);
			for (unsigned int j=0; j< OutBuffer.GetWidth(); ++j)
			{
				iX=(unsigned int)(j*dStepX);
				Pixel[0]=0; Pixel[1]=0; Pixel[2]=0; Pixel[3]=0;//reset all
				//Now sum the rectangle
				Ptr=InBuffer.GetBufferAddr()+ (iY* InBuffer.GetStrideInByte()) + (iX* SizeElem);
				for (unsigned int k=0; k< iStepY; ++k)
				{
					for (unsigned int l=0; l< iStepX; ++l)
					{ //Sum one line
						Pixel[0]+=Ptr[0]; Pixel[1]+=Ptr[1]; Pixel[2]+=Ptr[2]; //reset all
						Ptr+=SizeElem;
					}
					Ptr+=InBuffer.GetStrideInByte()- (iStepX*SizeElem); //Go next start line
				}
				pOut[0]=(unsigned char )(Pixel[0]>> iShift);
				pOut[1]=(unsigned char )(Pixel[1]>> iShift);
				pOut[2]=(unsigned char )(Pixel[2]>> iShift);
				pOut+=SizeElem;
			}
		}
	}
	//OutBuffer.bIsYUV =InBuffer.bIsYUV; //Recopy flags
	return(S_OK);
}

int MirrorImage(_tImageBuffer &OutBuffer, eEnumMirror mirror)
//*************************************************************************************
{ // In place mirroring
	const unsigned int SizePixel=(unsigned int) OutBuffer.GetPixelDepth();
	if ( mirror == eMirror_None)
	{ // The easiest ....
		return(S_OK);
	}
	const bool bAllowOptim=true;
	if ((SizePixel !=4) || (bAllowOptim==false))
	{
		unsigned char cTmp;
		unsigned char *pSrc;
		unsigned char *pDest;
		unsigned char *pLineSrc;
		unsigned char *pLineDest;
		const size_t Stride=OutBuffer.GetStrideInByte();

		if ( mirror == eMirror_X)
		{
			pLineSrc =(unsigned char *)OutBuffer.GetBufferAddr();
			pLineDest=pLineSrc + (OutBuffer.GetWidth() -1)*SizePixel;
			for (unsigned int y=OutBuffer.GetHeight(); y>0; --y)
			{
				pSrc =pLineSrc;
				pDest=pLineDest;
				for (unsigned int x=OutBuffer.GetWidth()>>1; x>0; --x)
				{
					for (unsigned int i=SizePixel; i>0; --i)
					{
						cTmp=*pSrc; *pSrc=*pDest; *pDest=cTmp; ++pSrc; ++pDest;
					}
					pDest-=SizePixel << 1;
				}
				pLineSrc  += Stride;
				pLineDest += Stride;
			}
			return(S_OK);
		}
		else if ( mirror == eMirror_Y)
		{
			pLineSrc =(unsigned char *)OutBuffer.GetBufferAddr();
			pLineDest=pLineSrc + Stride*(OutBuffer.GetHeight() -1);
			for (unsigned int y=OutBuffer.GetHeight()>>1; y>0; --y)
			{
				pSrc =pLineSrc;
				pDest=pLineDest;
				for (unsigned int x=OutBuffer.GetWidth(); x>0; --x)
				{
					for (unsigned int i=SizePixel; i>0; --i)
					{
						cTmp=*pSrc; *pSrc=*pDest; *pDest=cTmp; pSrc++; pDest++;
					}
				}
				pLineSrc  += Stride;
				pLineDest -= Stride;
			}
			return(S_OK);
		}
		else if ( mirror == eMirror_XY)
		{
			pLineSrc =(unsigned char *)OutBuffer.GetBufferAddr();
			pLineDest=pLineSrc + Stride*(OutBuffer.GetHeight() -1) + (OutBuffer.GetWidth()-1)*SizePixel;
			for (unsigned int y=OutBuffer.GetHeight()>>1; y>0; --y)
			{
				pSrc =pLineSrc;
				pDest=pLineDest;
				for (unsigned int x=OutBuffer.GetWidth(); x>0; --x)
				{
					for (unsigned int i=SizePixel; i>0; --i)
					{
						cTmp=*pSrc; *pSrc=*pDest; *pDest=cTmp; pSrc++; pDest++;
					}
					pDest-=SizePixel << 1;
				}
				pLineSrc  += Stride;
				pLineDest -= Stride;
			}
			return(S_OK);
		}
	}
	else
	{ // Work with int
		unsigned int  cTmp;
		unsigned int *pSrc;
		unsigned int *pDest;
		unsigned int *pLineSrc;
		unsigned int *pLineDest;
		const size_t Stride=OutBuffer.GetStrideInByte()>>2;

		if ( mirror == eMirror_X)
		{
			pLineSrc =(unsigned int *)OutBuffer.GetBufferAddr();
			pLineDest=pLineSrc + (OutBuffer.GetWidth() -1);
			for (unsigned int y=OutBuffer.GetHeight(); y>0; --y)
			{
				pSrc =pLineSrc;
				pDest=pLineDest;
				for (unsigned int x=OutBuffer.GetWidth()>>1; x>0; --x)
				{
					cTmp=*pSrc; *pSrc=*pDest; *pDest=cTmp; 
					++pSrc; 
					--pDest;
				}
				pLineSrc  += Stride;
				pLineDest += Stride;
			}
			return(S_OK);
		}
		else if ( mirror == eMirror_Y)
		{
			pLineSrc =(unsigned int *)OutBuffer.GetBufferAddr();
			pLineDest=pLineSrc + Stride*(OutBuffer.GetHeight() -1);
			for (unsigned int y=OutBuffer.GetHeight()>>1; y>0; --y)
			{
				pSrc =pLineSrc;
				pDest=pLineDest;
				for (unsigned int x=OutBuffer.GetWidth(); x>0; --x)
				{
					cTmp=*pSrc; *pSrc=*pDest; *pDest=cTmp; pSrc++; pDest++;
				}
				pLineSrc  += Stride;
				pLineDest -= Stride;
			}
			return(S_OK);
		}
		else if ( mirror == eMirror_XY)
		{
			pLineSrc =(unsigned int *)OutBuffer.GetBufferAddr();
			pLineDest=pLineSrc + Stride*(OutBuffer.GetHeight() -1) + (OutBuffer.GetWidth()-1);
			for (unsigned int y=OutBuffer.GetHeight()>>1; y>0; --y)
			{
				pSrc =pLineSrc;
				pDest=pLineDest;
				for (unsigned int x=OutBuffer.GetWidth(); x>0; --x)
				{
					cTmp=*pSrc; *pSrc=*pDest; *pDest=cTmp; 
					++pSrc; 
					--pDest;
				}
				pLineSrc  += Stride;
				pLineDest -= Stride;
			}
			return(S_OK);
		}
	}
	//Unsupported rot
	_fnReportError(eError_UnsupportedMirroringFormat, "Unsupported mirroring format(%d)", mirror);
	return(eError_UnsupportedMirroringFormat);
}


int RotateImage(const _tImageBuffer &InBuffer, _tImageBuffer &OutBuffer, eEnumRotation m_Rotation)
//*************************************************************************************
{
	const unsigned int SizePixel=(unsigned int) InBuffer.GetPixelDepth();
	const unsigned int InputHeight=InBuffer.GetHeight();
	const unsigned int InputWidth =InBuffer.GetWidth();

	int result;
	OutBuffer.SetFormat(InBuffer.GetFormat()); //Recopy flags

	int stride    =InBuffer .GetStrideInByte(0);
	int stride_out;

	unsigned char *pOut=OutBuffer.GetBufferAddr();
	if ( m_Rotation == eRotation_None)
	{ // The easiest ....
		const unsigned char *pLineIn=InBuffer.GetBufferAddr(); //Goto first pixel
		const unsigned char *pSrc    =pLineIn;
		result=OutBuffer.SetImageSize(InputWidth, InputHeight, InBuffer.GetStrideInPixel_X());
		if (result != S_OK)
			return(eError_CannotSetImageSize);
		for (unsigned int y=InputHeight; y>0; --y)
		{
			memcpy(pOut, pSrc, SizePixel * InputWidth);
			pLineIn +=stride;
			pSrc      =pLineIn;
			pOut     +=stride;
		}
		return(S_OK);
	}

	if ( m_Rotation == eRotation_180)
	{ // The easiest ....
		const unsigned char *pLineIn =InBuffer.GetBufferAddr()  + (stride * (InputHeight-1)) + (InputWidth-1)*SizePixel; //Goto last pixel
		      unsigned char *pLineOut=OutBuffer.GetBufferAddr(); //Goto first pixel
		const unsigned char *pSrc;
		result =OutBuffer.SetImageSize(InputWidth, InputHeight, InBuffer.GetStrideInPixel_X());
		if (result != S_OK)
			return(eError_CannotSetImageSize);
		for (unsigned int y=InputHeight; y > 0; --y)
		{
			pSrc    = pLineIn;
			pOut    = pLineOut;
			for (unsigned int x=InputWidth; x > 0; --x)
			{
				memcpy(pOut, pSrc, SizePixel);
				pSrc-=SizePixel;
				pOut+=SizePixel;
			}
			pLineIn -= stride;
			pLineOut+= stride;
		}
		return(S_OK);
	}
	else if ( m_Rotation == eRotation_90)
	{ // trigo way
		const unsigned char *pLineIn =InBuffer.GetBufferAddr() + (InputWidth-1)*SizePixel; //Goto last pixel of first line
		      unsigned char *pLineOut=OutBuffer.GetBufferAddr(); //Goto first pixel
		const unsigned char *pSrc    =pLineIn;

		result =OutBuffer.SetImageSize(InputHeight, InputWidth);
		stride_out=OutBuffer.GetStrideInByte(0);
		if (result != S_OK)
			return(eError_CannotSetImageSize);
		for (unsigned int x=InputWidth; x>0; --x)
		{
			pSrc    = pLineIn;
			pOut    = pLineOut;
			for (unsigned int y=InputHeight; y>0; --y)
			{
				memcpy(pOut, pSrc, SizePixel);
				pSrc+=stride;
				pOut+=SizePixel;
			}
			pLineIn  -= SizePixel;
			pLineOut += stride_out;
		}
		return(S_OK);
	}
	else if ( m_Rotation == eRotation_270)
	{
		const unsigned char *pLineIn=InBuffer.GetBufferAddr() + (InputHeight-1)* stride; //Goto first pixel of last line
		      unsigned char *pLineOut=OutBuffer.GetBufferAddr(); //Goto first pixel
		const unsigned char *pSrc    =pLineIn;
		result =OutBuffer.SetImageSize(InputHeight, InputWidth);
		if (result != S_OK)
			return(eError_CannotSetImageSize);
		stride_out=OutBuffer.GetStrideInByte(0);
		for (unsigned int x=InputWidth; x>0; --x)
		{
			pSrc    = pLineIn;
			pOut    = pLineOut;
			for (unsigned int y=InputHeight; y>0; --y)
			{
				memcpy(pOut, pSrc, SizePixel);
				pSrc-=stride;
				pOut+=SizePixel;
			}
			pLineIn  += SizePixel;
			pLineOut += stride_out;
		}
		return(S_OK);
	}
	//Unsupported rot
	_fnReportError(eError_UnsupportedRotationFormat, "Unsupported rotation format(%d)", m_Rotation);

	return(eError_UnsupportedRotationFormat);
}

int Write_RGB565 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/)
//*************************************************************************************
{ // regular RGB565
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	
	if ( (OutBuffer.GetHeight() > InputBuffer.GetHeight()) || (OutBuffer.GetWidth() > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	// Fill The Source strutur
	const unsigned int PixelDepth_Input = (unsigned int) InputBuffer.GetPixelDepth();
	const unsigned int PixelDepth_Output=2;

	unsigned char *pInput;
	unsigned char *pOutput;
	unsigned char *pInputLine  = (unsigned char *)InputBuffer.GetBufferAddr();
	unsigned char *pOutputLine = OutBuffer.GetBufferAddr();
	unsigned char RGB[3];

	if (InputBuffer.IsYUV()==false)
	{ //no Need to convert
		for (unsigned int Line=OutputHeight; Line >0 ; --Line)
		{
			pInput  = pInputLine ;
			pOutput = pOutputLine;
			for (unsigned int Column=OutputWidth; Column >0; --Column)
			{
				(*(unsigned short *)pOutput) =
					(  pInput[offsetof(InternalStructRGB, Blue) ]>>3)       +
					( (pInput[offsetof(InternalStructRGB, Green)]>>2) << 5) +
					( (pInput[offsetof(InternalStructRGB, Red)  ]>>3) << 11) ;
				pOutput   +=PixelDepth_Output;
				pInput    +=PixelDepth_Input;
			}
			pInputLine += InputBuffer.GetStrideInByte();
			pOutputLine+= OutBuffer  .GetStrideInByte();
		}
	}
	else
	{ //Need to convert to rgb
		for (unsigned int Line=OutputHeight; Line >0 ; --Line)
		{
			pInput  = pInputLine ;
			pOutput = pOutputLine;
			for (unsigned int Column=OutputWidth; Column >0; --Column)
			{
				Convert_YUV_to_RGB(pInput,(unsigned char *) &RGB);
				(*(unsigned short *)pOutput) =  
					    (RGB[offsetof(InternalStructRGB, Blue)]>>3)  
					+ ( (RGB[offsetof(InternalStructRGB, Green)]>>2) << 5)
					+ ( (RGB[offsetof(InternalStructRGB, Red)]>>3) << 11);
				pOutput   +=PixelDepth_Output;
				pInput    +=PixelDepth_Input;
			}
			pInputLine += InputBuffer.GetStrideInByte();
			pOutputLine+= OutBuffer  .GetStrideInByte();
		}
	}
	CHECK_ERROR(res);
	return(res);
}

int Write_RGB888 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{ // regular RGB888 (like bmp without header) a valider
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	
	if ( (OutBuffer.GetHeight() > InputBuffer.GetHeight()) || (OutBuffer.GetWidth() > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}

	// Fill The Source strutur
	const unsigned int PixelDepth_Input = (unsigned int) InputBuffer.GetPixelDepth();
	const unsigned int PixelDepth_Output= 3;
	unsigned int Offset_Red, Offset_Green, Offset_Blue;
	if (format== eImageFormat_RGB888)
	{
		Offset_Red=2; Offset_Green=1; Offset_Blue=0;
		OutBuffer.SetFormat(eImageFormat_RGB888);
	}
	else if (format== eImageFormat_BGR888)
	{
		Offset_Red=0; Offset_Green=1; Offset_Blue=2;
		OutBuffer.SetFormat(eImageFormat_BGR888);
	}
	else
	{
		CHECK_ERROR(eError_UnsupportedInputFormat);
		return(eError_UnsupportedInputFormat);
	}

	unsigned char *pInput;
	unsigned char *pOutput;
	unsigned char *pInputLine  = (unsigned char *)InputBuffer.GetBufferAddr();
	unsigned char *pOutputLine = OutBuffer.GetBufferAddr();
	if (InputBuffer.IsYUV()==false)
	{ //no Need to convert
		for (unsigned int Line=OutputHeight; Line >0 ; --Line)
		{
			pInput  = pInputLine ;
			pOutput = pOutputLine;
			for (unsigned int Column=OutputWidth; Column >0; --Column)
			{
				*(pOutput +Offset_Red  ) = *(pInput+ offsetof(sRGB, Red)  );
				*(pOutput +Offset_Green) = *(pInput+ offsetof(sRGB, Green));
				*(pOutput +Offset_Blue ) = *(pInput+ offsetof(sRGB, Blue) );
				pOutput   +=PixelDepth_Output;
				pInput    +=PixelDepth_Input;
			}
			pInputLine += InputBuffer.GetStrideInByte();
			pOutputLine+= OutBuffer  .GetStrideInByte();
		}
	}
	else
	{ //Need to convert to rgb
		for (unsigned int Line=OutputHeight; Line >0 ; --Line)
		{
			sRGB Rgb;
			pInput  = pInputLine ;
			pOutput = pOutputLine;
			for (unsigned int Column=OutputWidth; Column >0; --Column)
			{
				Convert_YUV_to_RGB(pInput, (unsigned char *)&Rgb);
				*(pOutput +Offset_Red  ) = Rgb.Red;
				*(pOutput +Offset_Green) = Rgb.Green;
				*(pOutput +Offset_Blue ) = Rgb.Blue;
				pOutput   +=PixelDepth_Output;
				pInput    +=PixelDepth_Input;
			}
			pInputLine += InputBuffer.GetStrideInByte();
			pOutputLine+= OutBuffer  .GetStrideInByte();
		}
	}
	// OutBuffer.bIsYUV=false;
	CHECK_ERROR(res);
	return(res);
}

int Write_ARGB8888 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{ // regular ARGB888 1 plan
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	
	if ( (OutputHeight > InputBuffer.GetHeight()) || (OutputWidth > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}

	// Fill The Source strutur
	const unsigned int PixelDepth_Input = (unsigned int) InputBuffer.GetPixelDepth();
	const unsigned int PixelDepth_Output= 4;
	unsigned int Offset_Red, Offset_Green, Offset_Blue, Offset_Alpha;
	if (format== eImageFormat_ARGB8888)
	{
		Offset_Alpha=3; Offset_Red=2; Offset_Green=1; Offset_Blue=0;
	}
	else if (format== eImageFormat_BGRA8888)
	{
		Offset_Alpha=0; Offset_Red=1; Offset_Green=2; Offset_Blue=3;
	}
	else
	{
		CHECK_ERROR(eError_UnsupportedInputFormat);
		return(eError_UnsupportedInputFormat);
	}

	unsigned char *pInput;
	unsigned char *pOutput;
	unsigned char *pInputLine  = (unsigned char *)InputBuffer.GetBufferAddr();
	unsigned char *pOutputLine = OutBuffer.GetBufferAddr();
	if (InputBuffer.IsYUV()==false)
	{ //no Need to convert
		for (unsigned int Line=OutputHeight; Line >0; --Line)
		{
			pInput  = pInputLine ;
			pOutput = pOutputLine;
			for (unsigned int Column=OutputWidth; Column >0; --Column)
			{
				*(pOutput + Offset_Alpha)= eAlpha_Default;
				*(pOutput + Offset_Red  )= *(pInput+ offsetof(InternalStructRGB, Red));
				*(pOutput + Offset_Green)= *(pInput+ offsetof(InternalStructRGB, Green));
				*(pOutput + Offset_Blue )= *(pInput+ offsetof(InternalStructRGB, Blue));
				pOutput   +=PixelDepth_Output;
				pInput    +=PixelDepth_Input;
			}
			pInputLine += InputBuffer.GetStrideInByte();
			pOutputLine+= OutBuffer  .GetStrideInByte();
		}
	}
	else
	{ //Need to convert to rgb
		for (unsigned int Line=OutputHeight; Line >0; --Line)
		{
			InternalStructRGB InputRGB;
			pInput  = pInputLine ;
			pOutput = pOutputLine;
			for (unsigned int Column=OutputWidth; Column >0; --Column)
			{
				Convert_YUV_to_RGB(pInput, (unsigned char *)&InputRGB);
				*(pOutput + Offset_Alpha)= eAlpha_Default;
				*(pOutput + Offset_Red  )= InputRGB.Red;
				*(pOutput + Offset_Green)= InputRGB.Green;
				*(pOutput + Offset_Blue )= InputRGB.Blue;
				pOutput   +=PixelDepth_Output;
				pInput    +=PixelDepth_Input;
			}
			pInputLine += InputBuffer.GetStrideInByte();
			pOutputLine+= OutBuffer  .GetStrideInByte();
		}
	}

	//OutBuffer.bIsYUV=false;
	CHECK_ERROR(res);
	return(res);
}

int Write_Raw (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{ // Raw 8 or 12
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	
	if ( (OutputHeight > InputBuffer.GetHeight()) || (OutputWidth > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if (format==eImageFormat_Raw8)
	{
		const unsigned int OutputStride= OutBuffer  .GetStrideInByte();
		const unsigned int InputStride = InputBuffer.GetStrideInByte();

		unsigned char * pPixelL1;
		unsigned char * pPixelL2;
		unsigned char *pLineIn = InputBuffer.GetBufferAddr();
		unsigned char *pLineOut= OutBuffer  .GetBufferAddr();
		unsigned char *pReadL1, *pReadL2;
		const unsigned int InputPixelDepth=(unsigned int)InputBuffer.GetPixelDepth();
		const unsigned int Offset_Blue=eRGBOffset_Blue, Offset_Red=eRGBOffset_Red, Offset_Green=eRGBOffset_Green;
		if (InputBuffer.IsYUV()==false)
		{
			for (int i=OutputHeight>>1; i >0; --i)
			{
				pReadL1 = pLineIn;
				pReadL2 = pLineIn+InputStride;
				pPixelL1= pLineOut;
				pPixelL2= pLineOut+OutputStride;
				for (int j=OutputWidth>>1; j>0; --j)
				{
					//Line 1
					*(pPixelL1++) =*(pReadL1+Offset_Green); // G
					pReadL1 +=InputPixelDepth;
					*(pPixelL1++) =*(pReadL1+Offset_Red);   // R
					pReadL1 +=InputPixelDepth;

					//Line 2
					*(pPixelL2++) =*(pReadL2+Offset_Blue); // B
					pReadL2 +=InputPixelDepth;
					*(pPixelL2++) =*(pReadL2+Offset_Green); // G
					pReadL2 +=InputPixelDepth;
				}
				pLineIn  += InputStride  <<1;
				pLineOut += OutputStride << 1;
			}
		}
		else
		{ //YUV to raw
			for (int i=OutputHeight>>1; i >0; --i)
			{
				sRGB rgb1, rgb2;
				pReadL1 = pLineIn;
				pReadL2 = pLineIn+InputStride;
				pPixelL1= pLineOut;
				pPixelL2= pLineOut+OutputStride;
				for (int j=OutputWidth>>1; j>0; --j)
				{
					Convert_YUV_to_RGB(pReadL1, (unsigned char *)&rgb1);
					//Line 1
					*(pPixelL1++) =rgb1.Green; // G
					pReadL1 +=InputPixelDepth;
					*(pPixelL1++) =rgb1.Red;   // R
					pReadL1 +=InputPixelDepth;

					//Line 2
					Convert_YUV_to_RGB(pReadL2, (unsigned char *)&rgb2);
					*(pPixelL2++) =rgb2.Blue; // B
					pReadL2 +=InputPixelDepth;
					*(pPixelL2++) =rgb2.Green; // G
					pReadL2 +=InputPixelDepth;
				}
				pLineIn  += InputStride  <<1;
				pLineOut += OutputStride << 1;
			}
		}
	}
	else if (format==eImageFormat_Raw12)
	{
		ASSERT(0);
		return(eError_UnsupportedInputFormat);
	}
	else
	{
		ASSERT(0);
		return(eError_UnsupportedInputFormat);
	}
	CHECK_ERROR(res);
	//OutBuffer.bIsYUV=false;
	return(res);
}


int Write_NV21_NV12 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{ // NV21_NV12
	// 2 plans 1: Y one value for 1 pixel
	//         2: V-U one value for 4 pixels 2*2 for NV21
	//      or 2: U-V one value for 4 pixels 2*2 for NV12
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	const unsigned int InputPixelDepth=(unsigned int) InputBuffer.GetPixelDepth();

	int offset_U, offset_V;
	if (format==eImageFormat_NV12)
	{
		offset_U=0;
		offset_V=1;
	}
	else if (format==eImageFormat_NV21)
	{
		offset_U=1;
		offset_V=0;
	}
	else 
	{
		return(eError_WrongInputFormat);
	}

	if ( (OutBuffer.GetHeight() > InputBuffer.GetHeight()) || (OutBuffer.GetWidth() > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if ( (OutBuffer.GetHeight() & 1) || (OutBuffer.GetWidth() & 1) )
	{ // Must be even
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}

	const unsigned int StrideLuminance_In = InputBuffer.GetStrideInByte(0);
	const unsigned int StrideLuminance_Out= OutBuffer  .GetStrideInByte(0);
	const unsigned int StrideChroma_Out   = OutBuffer  .GetStrideInByte(1) << 1; // Use 2*2 pixels

	ASSERT((StrideChroma_Out!=0) && (StrideLuminance_Out!=0) && (StrideLuminance_In!=0) );

	unsigned char *pInput       = NULL;
	unsigned char *pInputLine   = (unsigned char *)InputBuffer.GetBufferAddr();
	unsigned char *pLineLuminance   = OutBuffer.GetBufferAddr();
	unsigned char *pLineChroma_Red  = pLineLuminance + (OutBuffer.GetStrideInByte(0)*OutBuffer.GetStrideInPixel_Y())+offset_V;
	unsigned char *pLineChroma_Blue = pLineLuminance + (OutBuffer.GetStrideInByte(0)*OutBuffer.GetStrideInPixel_Y())+offset_U;

	unsigned char *pLuminance;
	unsigned char *pChroma_Red;
	unsigned char *pChroma_Blue;


	unsigned int val;
	const unsigned int OffsetBlue=offsetof(sYUV, Chroma_Blue), OffsetRed=offsetof(sYUV, Chroma_Red);

	for (unsigned int Line=OutputHeight; Line > 0; Line-=2)
	{
		pInput= pInputLine;
		pLuminance  =pLineLuminance  ;
		pChroma_Red =pLineChroma_Red ;
		pChroma_Blue=pLineChroma_Blue;

		for (unsigned int Column=OutputWidth; Column > 0 ; Column-=2)
		{
			*pLuminance                       = *pInput;
			*(pLuminance+ 1)                  = *(pInput+InputPixelDepth);
			*(pLuminance+StrideLuminance_Out)   = *(pInput+StrideLuminance_In);
			*(pLuminance+StrideLuminance_Out+1) = *(pInput+StrideLuminance_In+InputPixelDepth);
			pLuminance+=2;
			
			val=*(pInput+OffsetRed) + *(pInput+InputPixelDepth+OffsetRed) + *(pInput+StrideLuminance_In+OffsetRed) + *(pInput+StrideLuminance_In+InputPixelDepth+OffsetRed);
			*pChroma_Red = (unsigned char)(val >> 2);
			pChroma_Red+=2;
			
			val=*(pInput+OffsetBlue) + *(pInput+InputPixelDepth+OffsetBlue) + *(pInput+StrideLuminance_In+OffsetBlue) + *(pInput+StrideLuminance_In+InputPixelDepth+OffsetBlue);
			*pChroma_Blue = (unsigned char)(val >> 2);
			pChroma_Blue+=2;

			pInput+= InputPixelDepth << 1;
		}
		pInputLine +=StrideLuminance_In << 1;
		pLineLuminance  += StrideLuminance_Out << 1;
		pLineChroma_Red += StrideChroma_Out;
		pLineChroma_Blue+= StrideChroma_Out;

	}
	//OutBuffer.bIsYUV=true;
	return(res);
}

int Write_YUV420 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{ // YUV420  I420 YV12   YCbCr 4:2:0 Planar Raster format (page 300)
	// 3 plans 1: Y one value for 1 pixel
	//         2: C one value for 4 pixels 2*2
	//         3: R one value for 4 pixels 2*2
	// YV12 (Red Chroma First)
	// I420 (Blue Chroma First)
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	// Fill The Source strutur
	const unsigned int InputPixelDepth= (unsigned int)InputBuffer.GetPixelDepth();

	if ( (OutBuffer.GetHeight() > InputBuffer.GetHeight()) || (OutBuffer.GetWidth() > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if ( (OutBuffer.GetHeight() & 1) || (OutBuffer.GetWidth() & 1) )
	{ // Must be even
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}


	unsigned char *pInput      = NULL;
	unsigned char *pLineInput  = (unsigned char *)InputBuffer.GetBufferAddr();
	unsigned char *pLineLuminance  = OutBuffer.GetBufferAddr();
	unsigned char *pLineChroma_Red;
	unsigned char *pLineChroma_Blue;
	unsigned char *pLuminance;
	unsigned char *pChroma_Red;
	unsigned char *pChroma_Blue;
	if (aFormat==eImageFormat_YUV420_blue)
	{
		pLineChroma_Blue= pLineLuminance   + ( OutBuffer.GetStrideInByte(0) * OutBuffer.GetStrideInPixel_Y());
		pLineChroma_Red = pLineChroma_Blue + ((OutBuffer.GetStrideInByte(1) * OutBuffer.GetStrideInPixel_Y()) );
	}
	else if (aFormat==eImageFormat_YUV420_red)
	{
		pLineChroma_Red = pLineLuminance  + ( OutBuffer.GetStrideInByte(0) * OutBuffer.GetStrideInPixel_Y());
		pLineChroma_Blue= pLineChroma_Red + ((OutBuffer.GetStrideInByte(1) * OutBuffer.GetStrideInPixel_Y()) );
	}
	else
	{
		ASSERT(0);
		return(eError_UnsupportedOutputFormat);
	}

	const unsigned int Stride_In0 =InputBuffer.GetStrideInByte();
	const unsigned int StrideLuminance_Out=OutBuffer.GetStrideInByte(0);
	const unsigned int StrideChroma_Out   =OutBuffer.GetStrideInByte(1) << 1;

	unsigned int val;
	const unsigned int OffsetBlue=offsetof(sYUV, Chroma_Blue), OffsetRed=offsetof(sYUV, Chroma_Red);

	for (unsigned int Line=OutputHeight; Line >0 ; Line-=2)
	{
		pInput=pLineInput;
		pLuminance= pLineLuminance;
		pChroma_Red = pLineChroma_Red;
		pChroma_Blue= pLineChroma_Blue;
		for (unsigned int Column=OutputWidth; Column >0 ; Column-=2)
		{
			*pLuminance                        = *pInput;
			*(pLuminance+ 1)                   = *(pInput+InputPixelDepth);
			*(pLuminance+StrideLuminance_Out)  = *(pInput+Stride_In0);
			*(pLuminance+StrideLuminance_Out+1)= *(pInput+Stride_In0+InputPixelDepth);
			pLuminance+=2;
			
			val=*(pInput+OffsetRed) + *(pInput+InputPixelDepth+OffsetRed) + *(pInput+Stride_In0+OffsetRed) + *(pInput+Stride_In0+InputPixelDepth+OffsetRed);
			*pChroma_Red = (unsigned char)(val >> 2);
			++pChroma_Red;
			
			val=*(pInput+OffsetBlue) + *(pInput+InputPixelDepth+OffsetBlue) + *(pInput+Stride_In0+OffsetBlue) + *(pInput+Stride_In0+InputPixelDepth+OffsetBlue);
			*pChroma_Blue = (unsigned char)(val >> 2);
			++pChroma_Blue;

			pInput+= InputPixelDepth << 1;
		}
		pLineLuminance += StrideLuminance_Out << 1; //Goto next line
		pLineChroma_Red +=StrideChroma_Out;
		pLineChroma_Blue+=StrideChroma_Out;
		pLineInput     += Stride_In0<<1; //Jump two lines
	}
	//OutBuffer.bIsYUV=true;
	return(res);
}


int Write_YUV422_planar (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{ // YUV42 Planar Raster format (page 300)
	// 3 plans 1: Y one value for 1 pixel
	//         2: C one value for 4 pixels 2*1
	//         3: R one value for 4 pixels 2*1
	// 
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	if ( (OutputHeight > InputBuffer.GetHeight()) || (OutputWidth > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if (OutputWidth & 1)
	{ // Must be even
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if ((aFormat!=eImageFormat_YUV422_blue) && (aFormat!=eImageFormat_YUV422_red))
	{
		ASSERT(0);
		return(eError_UnsupportedOutputFormat);
	}


	// Fill The Source strutur
	const unsigned int InputPixelDepth=(unsigned int)InputBuffer.GetPixelDepth();

	unsigned char *pLineIn     = (unsigned char *)InputBuffer.GetBufferAddr();
	unsigned char *pInput;
	unsigned char *pLuminance;
	unsigned char *pLineLuminance  = OutBuffer.GetBufferAddr();
	unsigned char *pChroma_Red;
	unsigned char *pChroma_Blue;
	unsigned char *pLineChroma_Red;
	unsigned char *pLineChroma_Blue;
	const unsigned int StrideLuma_out  =OutBuffer.GetStrideInByte(0);
	const unsigned int StrideChroma_out=OutBuffer.GetStrideInByte(1);

	if (aFormat==eImageFormat_YUV422_blue)
	{
		pLineChroma_Blue= pLineLuminance   + ( OutBuffer.GetStrideInByte(0) * OutBuffer.GetStrideInPixel_Y());
		pLineChroma_Red = pLineChroma_Blue + ((OutBuffer.GetStrideInByte(1) * OutBuffer.GetStrideInPixel_Y()));
	}
	else if (aFormat==eImageFormat_YUV422_red)
	{
		pLineChroma_Red = pLineLuminance + ( OutBuffer.GetStrideInByte(0) * OutBuffer.GetStrideInPixel_Y());
		pLineChroma_Blue= pLineChroma_Red+ ((OutBuffer.GetStrideInByte(1) * OutBuffer.GetStrideInPixel_Y()));
	}
	else
	{
		ASSERT(0);
		return(eError_UnsupportedOutputFormat);
	}

	unsigned int val;
	const unsigned int OffsetBlue=offsetof(sYUV, Chroma_Blue), OffsetRed=offsetof(sYUV, Chroma_Red);
	for (unsigned int Line=OutputHeight; Line >0; --Line)
	{
		pInput= pLineIn;
		pChroma_Red = pLineChroma_Red;
		pChroma_Blue= pLineChroma_Blue;
		pLuminance  = pLineLuminance;
		for (unsigned int Column=OutputWidth; Column >0; Column-=2)
		{
			*pLuminance      = *pInput;
			*(pLuminance+ 1) = *(pInput+InputPixelDepth);
			pLuminance+=2;
			
			val=*(pInput+OffsetRed) + *(pInput+InputPixelDepth+OffsetRed) ;
			*pChroma_Red = (unsigned char)(val >> 1);
			++pChroma_Red;
			
			val=*(pInput+OffsetBlue) + *(pInput+InputPixelDepth+OffsetBlue);
			*pChroma_Blue = (unsigned char)(val >> 1);
			++pChroma_Blue;

			pInput+= InputPixelDepth << 1;
		}
		// pLuminance+=LineLuminance_Out; //Goto next line
		pLineIn   +=InputBuffer.GetStrideInByte();
		pLineChroma_Red += StrideChroma_out;
		pLineChroma_Blue+= StrideChroma_out;
		pLineLuminance  += StrideLuma_out;
	}
	//OutBuffer.bIsYUV=true;
	return(res);
}

int Write_YCbYCr(const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{ // 1 plane interleaved , 16 bits/pixel 
	// p304 in  STn8500A0_SIA_HW_specification YCbCr 4:2:2 Interleaved Raster format
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	if ( (OutputHeight > InputBuffer.GetHeight()) || (OutputWidth > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if (OutputWidth & 1)
	{ // Must be even
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if ((aFormat!=eImageFormat_CbYCrY) && (aFormat!=eImageFormat_YCbYCr) && (aFormat!=eImageFormat_CrYCbY) && (aFormat!=eImageFormat_YCrYCb) && (aFormat!=eImageFormat_YUV888i) && (aFormat!=eImageFormat_YUV844i))
	{
		ASSERT(0);
		return(eError_UnsupportedOutputFormat);
	}

	// Fill The Source strutur
	const unsigned int  InputPixelDepth= (unsigned int) InputBuffer.GetPixelDepth();
	//unsigned int OutputPixelDepth=2;
	const unsigned int InputStride= InputBuffer.GetStrideInByte();
	//if (eImageFormat_YUV888i==aFormat)
	//	OutputPixelDepth=3;

	unsigned char *pInput=NULL;
	unsigned char *pLineInput =(unsigned char *)InputBuffer.GetBufferAddr();
	unsigned char *pLineOutput=(unsigned char *)OutBuffer.GetBufferAddr();
	_uCbYCr *pCbyCr= (_uCbYCr *)OutBuffer.GetBufferAddr();
	const unsigned int OffsetBlue=offsetof(sYUV, Chroma_Blue), OffsetRed=offsetof(sYUV, Chroma_Red);
	unsigned int Val;
	const unsigned int StrideOut=OutBuffer.GetStrideInByte(0);
	if (aFormat==eImageFormat_CbYCrY)
	{
		for (unsigned int Y=OutputHeight; Y >0; --Y)
		{
			pInput= pLineInput;
			pCbyCr= (_uCbYCr *)pLineOutput;
			for (unsigned int X=OutputWidth >>1; X>0; --X)
			{
				pCbyCr->CbYCrY.Y0 = *(pInput);
				pCbyCr->CbYCrY.Y1 = *(pInput+InputPixelDepth); //Second luma
				Val=(*(pInput+OffsetBlue+InputPixelDepth) + *(pInput+OffsetBlue));
				pCbyCr->CbYCrY.Cb = (unsigned char )(Val >> 1);
				Val=(*(pInput+OffsetRed+InputPixelDepth)  + *(pInput+OffsetRed ));
				pCbyCr->CbYCrY.Cr = (unsigned char )(Val >> 1);; //Second chroma
				pInput      += InputPixelDepth<<1;
				++pCbyCr;
			}
			pLineInput += InputStride;
			pLineOutput+= StrideOut;
		}
	}
	else if (aFormat==eImageFormat_YCbYCr)
	{
		for (unsigned int Y=OutputHeight; Y >0; --Y)
		{
			pInput= pLineInput;
			pCbyCr= (_uCbYCr *)pLineOutput;
			for (unsigned int X=OutputWidth >>1 ; X>0; --X)
			{
				pCbyCr->YCbYCr.Y0 = *(pInput);
				pCbyCr->YCbYCr.Y1 = *(pInput+InputPixelDepth); //Second luma
				Val=(*(pInput+OffsetBlue+InputPixelDepth) + *(pInput+OffsetBlue));
				pCbyCr->YCbYCr.Cb = (unsigned char )(Val >> 1);
				Val=(*(pInput+OffsetRed+InputPixelDepth)  + *(pInput+OffsetRed ));
				pCbyCr->YCbYCr.Cr = (unsigned char )(Val >> 1);; //Second chroma
				pInput      += InputPixelDepth<<1;
				++pCbyCr;
			}
			pLineInput+=InputStride;
			pLineOutput+= StrideOut;
		}
	}
	else if (aFormat==eImageFormat_CrYCbY)
	{
		for (unsigned int Y=OutputHeight; Y >0; --Y)
		{
			pInput= pLineInput;
			pCbyCr= (_uCbYCr *)pLineOutput;
			for (unsigned int X=OutputWidth >>1; X>0; --X)
			{
				pCbyCr->CrYCbY.Y0 = *(pInput);
				pCbyCr->CrYCbY.Y1 = *(pInput+InputPixelDepth); //Second luma
				Val=(*(pInput+OffsetBlue+InputPixelDepth) + *(pInput+OffsetBlue));
				pCbyCr->CrYCbY.Cb = (unsigned char )(Val >> 1);
				Val=(*(pInput+OffsetRed+InputPixelDepth)  + *(pInput+OffsetRed ));
				pCbyCr->CrYCbY.Cr = (unsigned char )(Val >> 1);; //Second chroma
				pInput      += InputPixelDepth<<1;
				++pCbyCr;
			}
			pLineInput+=InputStride;
			pLineOutput+= StrideOut;
		}
	}
	else if (aFormat==eImageFormat_YCrYCb)
	{
		for (unsigned int Y=OutputHeight; Y >0; --Y)
		{
			pInput= pLineInput;
			pCbyCr= (_uCbYCr *)pLineOutput;
			for (unsigned int X=OutputWidth >>1; X>0; --X)
			{
				pCbyCr->YCrYCb.Y0 = *(pInput);
				pCbyCr->YCrYCb.Y1 = *(pInput+InputPixelDepth); //Second luma
				Val=(*(pInput+OffsetBlue+InputPixelDepth) + *(pInput+OffsetBlue));
				pCbyCr->YCrYCb.Cb = (unsigned char )(Val >> 1);
				Val=(*(pInput+OffsetRed+InputPixelDepth)  + *(pInput+OffsetRed ));
				pCbyCr->YCrYCb.Cr = (unsigned char )(Val >> 1);; //Second chroma
				pInput      += InputPixelDepth<<1;
				++pCbyCr;
			}
			pLineInput+=InputStride;
			pLineOutput+= StrideOut;
		}
	}

	else if (aFormat==eImageFormat_YUV888i)
	{
		unsigned char *pOutput=(unsigned char *)OutBuffer.GetBufferAddr();
		for (unsigned int Y=OutputHeight; Y >0; --Y)
		{
			pInput= pLineInput;
			pOutput= pLineOutput;
			for (unsigned int X=OutputWidth; X>0; --X)
			{
				pOutput[0]=pInput[0];
				pOutput[1]=pInput[1];
				pOutput[2]=pInput[2];
				pInput  += InputPixelDepth;
				pOutput += 3;
			}
			pLineInput+=InputStride;
			pLineOutput+= StrideOut;
		}
	}
	else if (aFormat==eImageFormat_YUV844i)
	{
		unsigned char *pOutput=(unsigned char *)OutBuffer.GetBufferAddr();
		for (unsigned int Y=OutputHeight; Y >0; --Y)
		{
			pInput = pLineInput;
			pOutput= pLineOutput;
			for (unsigned int X=OutputWidth; X>0; --X)
			{
				pOutput[0]=pInput[0];
				pOutput[1]= (pInput[eOffset_Cb] >> 4) +(pInput[eOffset_Cr] & 0xF0);
				pInput  += InputPixelDepth;
				pOutput += 2;
			}
			pLineInput +=InputStride;
			pLineOutput+= StrideOut;
		}
	}
	else
	{
		ASSERT(0);
		res=eError_UnsupportedOutputFormat;
	}

	//OutBuffer.bIsYUV=true;
	return(res);
}

int Write_YUV_planar(const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{ // 3 planes, 3 Bytes/pixel
	// Y,Cb,Cr for eImageFormat_YUV422_blue
	// Y,Cr,Cb for eImageFormat_YUV422_red
	int res=0;
	const unsigned int OutputHeight=OutBuffer.GetHeight();
	const unsigned int OutputWidth =OutBuffer.GetWidth();
	if ( (OutputHeight > InputBuffer.GetHeight()) || (OutputWidth > InputBuffer.GetWidth()) )
	{
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if (OutputWidth & 1)
	{ // Must be even
		ASSERT(0);
		return(eError_InvalidOutputSize);
	}
	if ((aFormat!=eImageFormat_YUV_blue) && (aFormat!=eImageFormat_YUV_red))
	{
		ASSERT(0);
		return(eError_UnsupportedOutputFormat);
	}

	// Fill The Source strutur
	const unsigned int InputPixelDepth = (unsigned int) InputBuffer.GetPixelDepth();
	const unsigned int InputStride     = InputBuffer.GetStrideInByte(0);

	unsigned char *pInput;
	unsigned char *pLineInput  =(unsigned char *)InputBuffer.GetBufferAddr();
	unsigned char *pLineLuminance  = OutBuffer.GetBufferAddr();
	unsigned char *pLineChroma_Red;
	unsigned char *pLineChroma_Blue;
	unsigned char *pLuminance;
	unsigned char *pChroma_Red;
	unsigned char *pChroma_Blue;

	const unsigned int StrideLuma_out  =OutBuffer.GetStrideInByte(0);
	const unsigned int StrideChroma_out=OutBuffer.GetStrideInByte(1);

	if (aFormat==eImageFormat_YUV_blue)
	{
		pLineChroma_Blue= pLineLuminance  + StrideLuma_out  * OutBuffer.GetStrideInPixel_Y();
		pLineChroma_Red  =pLineChroma_Blue+ StrideChroma_out* OutBuffer.GetStrideInPixel_Y();
	}
	else if (aFormat==eImageFormat_YUV_red)
	{
		pLineChroma_Red = pLineLuminance + StrideLuma_out  * OutBuffer.GetStrideInPixel_Y();
		pLineChroma_Blue= pLineChroma_Red+ StrideChroma_out* OutBuffer.GetStrideInPixel_Y();
	}
	else
	{
		ASSERT(0);
		return(eError_UnsupportedOutputFormat);
	}

	const unsigned int OffsetBlue=offsetof(sYUV, Chroma_Blue), OffsetRed=offsetof(sYUV, Chroma_Red);
	for (unsigned int Y=OutputHeight; Y >0; --Y)
	{
		pInput= pLineInput;
		pLuminance  =pLineLuminance;
		pChroma_Red =pLineChroma_Red;
		pChroma_Blue=pLineChroma_Blue;
		for (unsigned int X=OutputWidth; X>0; --X)
		{
			*pLuminance   = *pInput;
			*pChroma_Red  = *(pInput+OffsetRed);
			*pChroma_Blue = *(pInput+OffsetBlue);
			++pLuminance;
			++pChroma_Red;
			++pChroma_Blue;
			pInput+= InputPixelDepth;
		}
		pLineInput +=InputStride;
		pLineLuminance  += StrideLuma_out;
		pLineChroma_Red += StrideChroma_out;
		pLineChroma_Blue+= StrideChroma_out;
	}
	return(res);
}

int Write_YUV420_mb(const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{ // YUV420_mb
	// p303 in  STn8500A0_SIA_HW_specification
	// 2 planes:  Luma 1x1 in 8*16, 1b/p, Chroma Cr then Cb 2*2 

	int res=0;
	const unsigned int OutputWidth =OutBuffer.GetStrideInPixel_X();
	const unsigned int OutputHeight=OutBuffer.GetStrideInPixel_Y();

	const unsigned int InputWidth  =InputBuffer.GetWidth();
	const unsigned int InputHeight =InputBuffer.GetHeight();
	// Fill The Source structur
	const unsigned int MacroBlock420_Width = 8;
	const unsigned int MacroBlock420_Height=16;
	const unsigned int OutputPixelStep=2;
	const unsigned int InputPixelDepth= (unsigned int)InputBuffer.GetPixelDepth();
	const unsigned int InputStride= InputBuffer.GetStrideInByte();
	if ( ((OutputWidth & 0x7)!=0) || ((OutputHeight & 0xF)!=0) )
	{ 
		CHECK_ERROR(eError_WrongResolutionForMB);
		return(eError_WrongResolutionForMB);
	}

	if ( (format != eImageFormat_YUV420mb) || (OutBuffer.GetFormat() != eImageFormat_YUV420mb) )
	{
		ASSERT(0);
		return(eError_ImcompatibleOutputFormat);
	}

	const unsigned char *pYUV  = (const unsigned char *)InputBuffer.GetBufferAddr();
	unsigned int Val;
	const unsigned int Offset_Blue=offsetof(sYUV, Chroma_Blue), Offset_Red=offsetof(sYUV, Chroma_Red);


	unsigned char *pLuma         = OutBuffer.GetBufferAddr();
	unsigned char *pChroma_red   = pLuma + (OutputWidth /* must take the width because of stride definition for 420mb 1.5 instead of 1  */* OutBuffer.GetStrideInPixel_Y());
	unsigned char *pChroma_blue  = pChroma_red+1;

	const unsigned int NbMacroBlock_V = (OutputHeight/ MacroBlock420_Height);
	const unsigned int NbMacroBlock_H = (OutputWidth / MacroBlock420_Width);

	unsigned int x, y;
	unsigned int PixelX, PixelY=0;
	unsigned int MaxWidth=InputWidth, MaxHeight=InputHeight;
	if (OutBuffer.GetWidth() < MaxWidth)
		MaxWidth=OutBuffer.GetWidth();
	if (OutBuffer.GetHeight() < MaxHeight)
		MaxHeight=OutBuffer.GetHeight();
	for (unsigned int MacroBlock_V=NbMacroBlock_V; MacroBlock_V > 0; --MacroBlock_V)
	{
		if (PixelY < MaxHeight)
			PixelX=0;
		else
			PixelX=MaxWidth; // In this case add black pixels
		for (unsigned int MacroBlock_H=NbMacroBlock_H; MacroBlock_H > 0; --MacroBlock_H)
		{
			//Start a new macro block works in 2x2 pixels
			for (y=MacroBlock420_Height>>1; y > 0; --y)
			{
				for (x=MacroBlock420_Width>>1; x > 0; --x)
				{ //Compute the macro block line
					if (PixelX < MaxWidth)
					{
						*pLuma                        = *pYUV;
						*(pLuma+1)                    = *(pYUV+InputPixelDepth);
						*(pLuma+MacroBlock420_Width)  = *(pYUV+InputStride  );
						*(pLuma+MacroBlock420_Width+1)= *(pYUV+InputStride+InputPixelDepth);
						pLuma+=2;
						//Chroma is red then blue
						Val=( pYUV[Offset_Red] + pYUV[InputPixelDepth+Offset_Red] + pYUV[InputStride +Offset_Red] + pYUV[InputPixelDepth+InputStride+Offset_Red])  /*+2 for rounding */;
						*pChroma_red = (unsigned char )(Val >> 2);
						pChroma_red +=OutputPixelStep;

						Val=(pYUV[Offset_Blue] + pYUV[InputPixelDepth+Offset_Blue] + pYUV[InputStride+Offset_Blue]+ pYUV[InputPixelDepth+InputStride+Offset_Blue] )  /*+ 2for rounding */;
						*pChroma_blue = (unsigned char )(Val >> 2);
						pChroma_blue +=OutputPixelStep;
					}
					else
					{ //Fill the rest of line
						*pLuma                        = 0;
						*(pLuma+1)                    = 0;
						*(pLuma+MacroBlock420_Width)  = 0;
						*(pLuma+MacroBlock420_Width+1)= 0;
						pLuma+=2;
						//Chroma is red then blue
						*pChroma_red = 0;
						pChroma_red +=OutputPixelStep;

						*pChroma_blue = 0;
						pChroma_blue +=OutputPixelStep;
					}
					PixelX+=2; //Increment the nb of pixel in the line
					pYUV+=InputPixelDepth<<1; //Next pixel
				} //End of macro line
				pYUV  +=((InputStride<<1) -MacroBlock420_Width*InputPixelDepth); //goto next beginning of line block
				pLuma +=MacroBlock420_Width; //Skip one line
				PixelX-=MacroBlock420_Width; //Increment the nb of pixel in the line
				PixelY +=2;
			} //End of macro block
			//goto next beginning of block
			pYUV -=(InputStride * MacroBlock420_Height);
			pYUV +=MacroBlock420_Width*InputPixelDepth;
			PixelX+=MacroBlock420_Width; //Increment the nb of pixel in the line
			PixelY -=MacroBlock420_Height;
		} // End of a line in 'block unit'
		pYUV +=(InputStride * MacroBlock420_Height);
		pYUV -=(InputWidth * InputPixelDepth);
		PixelY+=MacroBlock420_Height;
		PixelX=0; //restart a the line
	}
	return(res);
}

int Write_YUV422_mb(const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{ // YUV422_mb With stride management
	// p303 in  STn8500A0_SIA_HW_specification
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	const unsigned int OutputWidth =OutBuffer.GetStrideInPixel_X();
	const unsigned int OutputHeight=OutBuffer.GetStrideInPixel_Y();

	const unsigned int MacroBlock422_Width =8;
	const unsigned int MacroBlock422_Height=16;
	const unsigned int OutputPixelDepth=2;
	const unsigned int InputPixelDepth= (unsigned int)InputBuffer.GetPixelDepth();
	const unsigned int InputStride= InputBuffer.GetStrideInByte();

	if ( ((InputWidth & 0x7)!=0) || ((InputHeight & 0xF)!=0) )
	{ 
		CHECK_ERROR(eError_WrongResolutionForMB);
		return(eError_WrongResolutionForMB);
	}
	if ( (format != eImageFormat_YUV422mb) || (OutBuffer.GetFormat() != eImageFormat_YUV422mb) )
	{
		ASSERT(0);
		return(eError_ImcompatibleOutputFormat);
	}

	const unsigned char *pYUV  = (const unsigned char *)InputBuffer.GetBufferAddr();
	unsigned int Val;
	unsigned char *pLuma        = OutBuffer.GetBufferAddr();
	unsigned char *pChroma_red  = pLuma + (OutputWidth /* must take the width because of stride definition for 420mb 1.5 instead of 1  */* OutBuffer.GetStrideInPixel_Y());
	unsigned char *pChroma_blue = pChroma_red+1;

	const unsigned int NbMacroBlock_V = (OutputHeight/ MacroBlock422_Height);
	const unsigned int NbMacroBlock_H = (OutputWidth / MacroBlock422_Width);

	const unsigned int Offset_Blue=offsetof(sYUV, Chroma_Blue), Offset_Red=offsetof(sYUV, Chroma_Red);

	ASSERT(NbMacroBlock_V*MacroBlock422_Height ==(unsigned int)OutputHeight );
	ASSERT(NbMacroBlock_H*MacroBlock422_Width  ==(unsigned int)OutputWidth );
	unsigned int x, y;
	unsigned int PixelX, PixelY=0;
	unsigned int MaxWidth=InputWidth, MaxHeight=InputHeight;
	if (OutBuffer.GetWidth() < MaxWidth)
		MaxWidth=OutBuffer.GetWidth();
	if (OutBuffer.GetHeight() < MaxHeight)
		MaxHeight=OutBuffer.GetHeight();
	for (unsigned int MacroBlock_V=NbMacroBlock_V; MacroBlock_V > 0; --MacroBlock_V)
	{
		if (PixelY < MaxHeight)
			PixelX=0;
		else
			PixelX=MaxWidth; // In this case add black pixels
		for (unsigned int MacroBlock_H=NbMacroBlock_H; MacroBlock_H > 0; --MacroBlock_H)
		{
			for (y=MacroBlock422_Height; y > 0; --y)
			{
				for (x=MacroBlock422_Width >>1; x > 0; --x)
				{ //Compute the macro block line
					if (PixelX < MaxWidth)
					{
						*pLuma     = *pYUV;
						*(pLuma+1) = *(pYUV+InputPixelDepth);
						pLuma+=2;
						//Chroma is red the blue
						Val=pYUV[Offset_Red] + pYUV[InputPixelDepth+Offset_Red];
						*pChroma_red = (unsigned char )(Val >> 1);
						pChroma_red +=OutputPixelDepth;

						Val=pYUV[Offset_Blue] + pYUV[InputPixelDepth+Offset_Blue];
						*pChroma_blue = (unsigned char )(Val >> 1);
						pChroma_blue +=OutputPixelDepth;
					}
					else
					{ //Fill the rest of line
						*pLuma     = 0;
						*(pLuma+1) = 0;
						pLuma+=2;
						//Chroma is red the blue
						*pChroma_red = 0;
						pChroma_red +=OutputPixelDepth;
						*pChroma_blue = 0;
						pChroma_blue +=OutputPixelDepth;
					}
					PixelX+=2; //Increment the nb of pixel in the line
					pYUV+=InputPixelDepth <<1; //Next 2 pixels
				} //End of macro line
				pYUV +=(InputStride -MacroBlock422_Width*InputPixelDepth); //goto next beginning of line block
				PixelX-=MacroBlock422_Width; //Increment the nb of pixel in the line
				++PixelY;
			}
			//goto next beginning of block
			pYUV -=(InputStride * MacroBlock422_Height);
			pYUV +=MacroBlock422_Width*InputPixelDepth;
			PixelX +=MacroBlock422_Width; //Increment the nb of pixel in the line
			PixelY -=MacroBlock422_Height;
		} // End of a line in 'block unit'
		pYUV +=(InputStride * MacroBlock422_Height);
		pYUV -=InputWidth*InputPixelDepth;
		PixelY+=MacroBlock422_Height;
		PixelX=0; //restart a the line
	}
	return(res);
}



/*
const double coef0= 0.3313; // 0.3278
const double coef1= 0.41780; //0,4187
*/
/**
Convertion from RGB space to YUV one
*/
void Convert_RGB_to_YUV(const unsigned char *RGB, unsigned char *YUV)
//*************************************************************************************
{
	//unsigned char &R=RGB[2], &G=RGB[1], &B=RGB[0];
	const unsigned char
		&R=RGB[offsetof(InternalStructRGB, Red)],
		&G=RGB[offsetof(InternalStructRGB, Green)],
		&B=RGB[offsetof(InternalStructRGB, Blue)];
	unsigned char
		&Y =YUV[offsetof(struct sYUV, Luminance)],
		&Cr=YUV[offsetof(struct sYUV, Chroma_Red)],
		&Cb=YUV[offsetof(struct sYUV, Chroma_Blue)];
/* Original
	Y = (unsigned char)(0.299*R + 0.587*G + 0.114*B );
	U = (unsigned char)(0.147*R - 0.289*G + 0.436*B );
	V = (unsigned char)(0.615*R - 0.515*G - 0.100*B );
*/
	double dY  = 0.299  * R + 0.587  * G + 0.1140 * B;
	double dCb =-0.1687 * R - 0.3313 * G + 0.5000 * B + 128.0;
	double dCr = 0.5000 * R - 0.4178 * G - 0.0813 * B + 128.0;

	ASSERT(Y > 0.);
	ASSERT(Y < 255.5);
	Y = (unsigned char) (dY/*  + 0.5*/); // round to nearest value
	ASSERT(dCb > 0.);
	ASSERT(dCb <= 255.5);
	Cb= (unsigned char) (dCb);
	ASSERT(dCr > 0.);
	ASSERT(dCr <= 255.5);
	Cr= (unsigned char) (dCr);
}

/**
Convertion from YUV space to RGB one
*/
int Convert_RGB_to_YUV(_tImageBuffer &Buffer)
//*************************************************************************************
{
	int res=S_OK;
	//const unsigned char *YUV;
	//unsigned char *RGB;
	if ( Buffer.GetFormat()!= InternalFormatRGB)
	{
		ASSERT(0);
		return(eError_ImcompatibleFormat);
	}
	unsigned char *pLineInput=Buffer.GetBufferAddr();
	unsigned char *pInput;
	const unsigned int Stride=Buffer.GetStrideInByte(0);
	const unsigned int PixelDepth=(unsigned int )Buffer.GetPixelDepth();
	for (unsigned int y=Buffer.GetHeight(); y > 0; --y)
	{
		pInput=pLineInput;
		for (unsigned int x=Buffer.GetWidth(); x >0; --x)
		{ // convert YUV to RGB
			Convert_RGB_to_YUV(pInput, pInput);
			pInput +=PixelDepth;
		}
		pLineInput+= Stride;
	}
	Buffer.SetFormat(eImageFormat_YUV888i);
	return(res);
}



/**
Convertion from YUV space to RGB one
*/
void Convert_YUV_to_RGB(const unsigned char *YUV, unsigned char *RGB)
//*************************************************************************************
{ 
	//unsigned char &R=RGB[2], &G=RGB[1], &B=RGB[0];
	unsigned char 
		&R=RGB[offsetof(InternalStructRGB, Red)],
		&G=RGB[offsetof(InternalStructRGB, Green)],
		&B=RGB[offsetof(InternalStructRGB, Blue)];
	const unsigned char 
		&Y =YUV[offsetof(struct sYUV, Luminance)],
		&Cr=YUV[offsetof(struct sYUV, Chroma_Red)],
		&Cb=YUV[offsetof(struct sYUV, Chroma_Blue)];
	double dR = Y + 1.402   * (Cr - 128);
	double dG = Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128);
	double dB = Y + 1.772   * (Cb - 128);
	//do desaturation
	if (dR < 0)
		dR=0.;
	else if (dR>255.)
		dR=255.;
	if (dG < 0) 
		dG=0.;
	else if (dG>255.)
		dG=255.;
	if (dB < 0)
		dB=0.;
	else if (dB>255.)
		dB=255.;
	R= (unsigned char) dR;
	G= (unsigned char) dG;
	B= (unsigned char) dB;
}

/**
Convertion from YUV space to RGB one
*/
int Convert_YUV_to_RGB(_tImageBuffer &Buffer)
//*************************************************************************************
{
	int res=S_OK;
	//const unsigned char *YUV;
	//unsigned char *RGB;
	if ( Buffer.GetFormat()!= InternalFormatRGB)
	{
		ASSERT(0);
		return(eError_ImcompatibleFormat);
	}
	unsigned char *pLineInput=Buffer.GetBufferAddr();
	unsigned char *pInput;
	const unsigned int Stride=Buffer.GetStrideInByte(0);
	const unsigned int PixelDepth=(unsigned int )Buffer.GetPixelDepth();
	for (unsigned int y=Buffer.GetHeight(); y > 0; --y)
	{
		pInput=pLineInput;
		for (unsigned int x=Buffer.GetWidth(); x >0; --x)
		{ // convert YUV to RGB
			Convert_YUV_to_RGB(pInput, pInput);
			pInput +=PixelDepth;
		}
		pLineInput+= Stride;
	}
	Buffer.SetFormat(eImageFormat_RGB888);
	return(res);
}

int Read_YUV422_planar (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{ // YUV422 planar 3 planes, 2 Bytes/pixel 1 luma chroma 2*1
	// Y,Cb,Cr for eImageFormat_YUV422_blue
	// Y,Cr,Cb for eImageFormat_YUV422_red
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	// Fill The Source strutur
	const int OutPixelDepth=3;
	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if ( (OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK)
		|| (OutBuffer.SetImageSize(InputWidth, InputHeight, 0)!=S_OK))
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	int offset_ChromaFirstPlan, offset_ChromaSecondPlan;
	if (aFormat==eImageFormat_YUV422_red)
	{ //Red chroma first
		offset_ChromaFirstPlan  = offsetof(struct sYUV, Chroma_Red);
		offset_ChromaSecondPlan = offsetof(struct sYUV, Chroma_Blue);
	}
	else if (aFormat==eImageFormat_YUV422_blue)
	{ //Blue chroma first
		offset_ChromaFirstPlan  = offsetof(struct sYUV, Chroma_Blue);
		offset_ChromaSecondPlan = offsetof(struct sYUV, Chroma_Red);
	}
	else
	{
		ASSERT(0);
		return(eError_UnsupportedInputFormat);
	}
	unsigned char *pRead=InputBuffer.GetBufferAddr();

	do
	{
		unsigned char * pPixel= (unsigned char * )OutBuffer.GetBufferAddr();
		const unsigned int NbPixels=InputWidth*InputHeight;
		for (unsigned int i=0; i < NbPixels; ++i)
		{ // Read chrominance Y
			*pPixel=*pRead; ++pRead;
			pPixel+=OutPixelDepth;
		}
		pPixel= (unsigned char * )OutBuffer.GetBufferAddr()+offset_ChromaFirstPlan; //Offset for first chroma
		for (unsigned int i=0; i < (NbPixels>>1); ++i)
		{ // Read chrominance Cb 
			*pPixel=*pRead; ++pRead;
			*(pPixel+OutPixelDepth)= *pPixel;
			pPixel             +=OutPixelDepth<<1;
		}
		pPixel= (unsigned char * )OutBuffer.GetBufferAddr()+offset_ChromaSecondPlan; //Offset for second chroma
		for (unsigned int i=0; i < (NbPixels>>1); ++i)
		{ // Read chrominance Cr 
			*pPixel=*pRead; ++pRead;
			*(pPixel+OutPixelDepth)= *pPixel;
			pPixel             +=OutPixelDepth<<1;
		}
		ASSERT(pPixel== (OutBuffer.GetBufferAddr()+InputWidth * InputHeight * OutPixelDepth+offset_ChromaSecondPlan));
	} while (0);
	//OutBuffer.bIsYUV=true;
	CHECK_ERROR(res);
	return(res);
}

int Read_YUV_planar (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{ // YUV planar3 planes, 3 Bytes/pixel 
	// Y,Cb,Cr for eImageFormat_YUV_blue
	// Y,Cr,Cb for eImageFormat_YUV_red
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	// Fill The Source strutur
	const int OutPixelDepth=3;
	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if ( (OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK)
		|| (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK))
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	int offset_ChromaFirstPlan, offset_ChromaSecondPlan;
	if (aFormat==eImageFormat_YUV_red)
	{ //Red chroma first
		offset_ChromaFirstPlan  = offsetof(struct sYUV, Chroma_Red);
		offset_ChromaSecondPlan = offsetof(struct sYUV, Chroma_Blue);
	}
	else if (aFormat==eImageFormat_YUV_blue)
	{ //Blue chroma first
		offset_ChromaFirstPlan  = offsetof(struct sYUV, Chroma_Blue);
		offset_ChromaSecondPlan = offsetof(struct sYUV, Chroma_Red);
	}
	else
	{
		ASSERT(0);
		return(eError_UnsupportedInputFormat);
	}
	unsigned char *pRead=InputBuffer.GetBufferAddr();

	do
	{
		unsigned char * pPixel= (unsigned char * )OutBuffer.GetBufferAddr();
		const unsigned int NbPixels=InputWidth*InputHeight;
		for (unsigned int i=0; i < NbPixels; ++i)
		{ // Read chrominance Y
			*pPixel=*pRead; ++pRead;
			pPixel+=OutPixelDepth;
		}
		pPixel= (unsigned char * )OutBuffer.GetBufferAddr()+offset_ChromaFirstPlan; //Offset for first chroma
		for (unsigned int i=0; i < NbPixels; ++i)
		{ // Read chrominance Cb 
			*pPixel=*pRead; ++pRead;
			pPixel+=OutPixelDepth;
		}
		pPixel= (unsigned char * )OutBuffer.GetBufferAddr()+offset_ChromaSecondPlan; //Offset for second chroma
		for (unsigned int i=0; i < NbPixels; ++i)
		{ // Read chrominance Cr 
			*pPixel=*pRead; ++pRead;
			pPixel+=OutPixelDepth;
		}
	} while (0);
	//OutBuffer.bIsYUV=true;
	CHECK_ERROR(res);
	return(res);
}

int ReadChromaOnly (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{   // Read only chroma 1 By 1,2,3 or 4
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();

	// Fill The Source strutur
	/*const */int InPixelDepth=1;
	if (aFormat==eImageFormat_ChromaOnly_1_1)
	{
		/*reinterpret_cast<int>(InPixelDepth)*/InPixelDepth=1;
	}
	else if (aFormat==eImageFormat_ChromaOnly_1_2)
	{
		InPixelDepth=2;
		if (InputWidth & 1) 
			return(eError_InvalidImageSize);
	}
	else if (aFormat==eImageFormat_ChromaOnly_1_3)
	{
		InPixelDepth=3;
		if ((InputWidth % 3) !=0) 
			return(eError_InvalidImageSize);
	}
	else if (aFormat==eImageFormat_ChromaOnly_1_4)
	{
		InPixelDepth=4;
		if (InputWidth & 3) 
			return(eError_InvalidImageSize);
	}
	const int OutPixelDepth=3;
	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if ( (OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK)
		|| (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK))
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}

	const unsigned int StrideIn= InputBuffer.GetStrideInByte(0);
	const unsigned int StrideOut= OutBuffer.GetStrideInByte(0);
	unsigned char *pRead;
	unsigned char *pLineIn=InputBuffer.GetBufferAddr();
	unsigned char *pLineOut=(unsigned char * )OutBuffer.GetBufferAddr();
	unsigned char * pOut;
	for (unsigned int y=0; y < InputHeight; ++y)
	{ // Read chrominance Y
		pRead=pLineIn;
		pOut= pLineOut;
		for (unsigned int x=0; x < InputWidth; ++x)
		{ // Read chrominance Y
			*pOut= *(pRead);
			pOut[1]= 128; pOut[2]= 128;
			pOut += OutPixelDepth;
			pRead+= InPixelDepth;
		}
		pLineIn  += StrideIn;
		pLineOut += StrideOut;
	}
	CHECK_ERROR(res);
	return(res);
}


int Read_YUV420 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{ // YUV420  I420    YCbCr 4:2:0 Planar Raster format (page 300)
	// 3 plans 1: Y one value for 1 pixel
	//         2: Cb one value for 4 pixels 2*2
	//         3: Cr one value for 4 pixels 2*2
	// YV12 (Red Chroma First)
	// I420 (Blue Chroma First)
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();

	// Check that width and height are even
	if ( (InputWidth & 1) || (InputHeight & 1) )
	{
		return(eError_InvalidImageSize);
	}

	// Fill The Source strutur
	const int OutPixelDepth=3;
	const int LineWidth=OutPixelDepth*InputWidth;
	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if ( (OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK)
		|| (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK))
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	int offset_Plan2, offset_Plan3;
	if (aFormat==eImageFormat_YUV420_red)
	{
		offset_Plan2  = offsetof(struct sYUV, Chroma_Red);
		offset_Plan3  = offsetof(struct sYUV, Chroma_Blue);
	}
	else if (aFormat==eImageFormat_YUV420_blue)
	{
		offset_Plan2  = offsetof(sYUV, Chroma_Blue);
		offset_Plan3  = offsetof(sYUV, Chroma_Red);
	}
	else
	{
		ASSERT(0);
		return(eError_UnsupportedInputFormat);
	}

	unsigned char *pRead=InputBuffer.GetBufferAddr();
	do
	{
		char Val;
		unsigned char * pOut= (unsigned char * )OutBuffer.GetBufferAddr();
		for (unsigned int i=0; i < InputWidth*InputHeight; ++i)
		{ // Read chrominance Y
			*pOut= *(pRead++);
			pOut+=OutPixelDepth;
		}
		unsigned int iX=0;
		//Second plan 
		pOut= (unsigned char * )OutBuffer.GetBufferAddr()+offset_Plan2; //Offset for first chroma
		for (unsigned int i=0; i < (InputWidth*InputHeight) >> 2; ++i)
		{ // Read chrominance Cb 
			Val= *(pRead++);
			*pOut =Val; pOut+= OutPixelDepth;
			*pOut =Val; pOut+= LineWidth; //Go next line
			*pOut =Val; pOut-= OutPixelDepth;
			*pOut =Val; pOut-= LineWidth;
			pOut+=OutPixelDepth+OutPixelDepth;
			iX+=2;
			if (iX >=InputWidth)
			{
				iX=0;
				pOut+=LineWidth;
			}
		}
		pOut= (unsigned char * )OutBuffer.GetBufferAddr()+offset_Plan3; //Offset for second chroma
		iX=0;
		if (res!=0) 
			break;
		for (unsigned int i=0; i < (InputWidth*InputHeight) >> 2; ++i)
		{ // Read chrominance Cr 
			Val= *(pRead++);
			*pOut =Val; pOut+=OutPixelDepth;
			*pOut =Val; pOut+=LineWidth; //Go next line
			*pOut =Val; pOut-=OutPixelDepth;
			*pOut =Val; pOut-=LineWidth;
			pOut+=OutPixelDepth+OutPixelDepth;
			iX+=2;
			if (iX >=InputWidth)
			{
				iX=0;
				pOut+=LineWidth;
			}
		}
	} while (0);
	//OutBuffer.bIsYUV=true;
	CHECK_ERROR(res);
	return(res);
}

int Read_NV12_21(const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{ // NV21 or NV12 formats
	// 2 plans 1: Y one value for 1 pixel
	//         2: (U+V), one value for 4 pixels 2*2 for eImageFormat_NV12
	//      or 2: (V+U), one value for 4 pixels 2*2 for eImageFormat_NV21
	// Recognized format: eImageFormat_NV21, eImageFormat_NV12
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	// Check that width and height are even
	if ( (InputWidth & 1) || (InputHeight & 1) )
	{
		return(eError_InvalidImageSize);
	}

	int offset_U, offset_V;
	if (format==eImageFormat_NV12)
	{
		offset_U=0;
		offset_V=1;
	}
	else if (format==eImageFormat_NV21)
	{
		offset_U=1;
		offset_V=0;
	}
	else 
		return(eError_WrongInputFormat);
	
	// Check that width and height are even
	if ( (InputWidth & 1) || (InputHeight & 1) )
		return(eError_InvalidImageSize);

	const unsigned int StrideUV=InputBuffer.GetStrideInByte(1);
	const unsigned int StrideY =InputBuffer.GetStrideInByte(0);

	// Fill The Source strutur
	const int OutPixelDepth=3;
	const int LineWidth= OutPixelDepth * InputWidth;

	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if ((OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK) 
		|| (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK))
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	unsigned char *pRead;
	unsigned char *pLineIn= InputBuffer.GetBufferAddr();
	do
	{
		//Compute luminance
		unsigned char *pLineOut=(unsigned char * )OutBuffer.GetBufferAddr();
		unsigned char *pOut=pLineOut;
		for (unsigned int i=0; i < InputHeight; ++i)
		{
			pRead = pLineIn;
			pOut  = pLineOut;
			for (unsigned int j=0; j < InputWidth ; ++j)
			{ // Read chrominance Y
				*pOut =*(pRead++);
				pOut+=OutPixelDepth;
			}
			pLineIn  += StrideY;
			pLineOut +=LineWidth;
		}
		//Compute Chrominance
		pLineOut= (unsigned char * )OutBuffer.GetBufferAddr(); //Offset for blue color
		for (unsigned int i=InputHeight >> 1; i>0; --i)
		{
			pRead = pLineIn;
			pOut  = pLineOut;
			for (unsigned int j=InputWidth >> 1; j >0 ; --j)
			{ // Read chrominance U then V (V=Cr U=Cb) for eImageFormat_NV21, V then U for eImageFormat_NV12
				pOut[eOffset_V] =pRead[offset_V]; pOut[eOffset_U] =pRead[offset_U]; pOut+= OutPixelDepth;
				pOut[eOffset_V] =pRead[offset_V]; pOut[eOffset_U] =pRead[offset_U]; pOut+= LineWidth; //Go next line
				pOut[eOffset_V] =pRead[offset_V]; pOut[eOffset_U] =pRead[offset_U]; pOut-= OutPixelDepth;
				pOut[eOffset_V] =pRead[offset_V]; pOut[eOffset_U] =pRead[offset_U]; pOut-= LineWidth;
				pOut+=OutPixelDepth+OutPixelDepth;
				pRead+=2;
			}
			pLineIn += StrideUV<< 1;
			pLineOut+= LineWidth<< 1; //Go next line*2
		}
	} while (0);
	CHECK_ERROR(res);
	return(res);
}


union uRGB565
{
	unsigned int val;
	struct/**/ 
	{
		unsigned int Blue:5;
		unsigned int Green:6;
		unsigned int Red:5;
	} Color/**/;
} ;

int Read_RGB565 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/)
//*************************************************************************************
{
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	const int OutPixelDepth=3;
	const int InputPixelDepth=2;
	// Fill The Source strutur
	OutBuffer.SetFormat(InternalFormatRGB);
	if (OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK)
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	res= OutBuffer.SetImageSize(InputWidth, InputHeight);
	unsigned int StrideIn=InputWidth * InputPixelDepth;
	ASSERT(res==S_OK);
	do
	{
		unsigned char * pPixel= (unsigned char * )OutBuffer.GetBufferAddr();
		uRGB565 *pRGB565;
		unsigned char *pLineIn=InputBuffer.GetBufferAddr();
		unsigned char *pRead;
		for (int i=InputHeight; i >0; --i)
		{
			pRead=pLineIn;
			for (int j=InputWidth; j>0; --j)
			{
				pRGB565= (uRGB565 *)pRead;
				*(pPixel+ offsetof(InternalStructRGB, Red)  ) =(unsigned char)(pRGB565->Color.Red << 3);
				*(pPixel+ offsetof(InternalStructRGB, Green)) =(unsigned char)(pRGB565->Color.Green << 2);
				*(pPixel+ offsetof(InternalStructRGB, Blue) ) =(unsigned char)(pRGB565->Color.Blue << 3);
				pRead += InputPixelDepth;
				pPixel+= InternalPixelDepth;
			}
			pLineIn+=StrideIn;
		}
	} while (0);
	CHECK_ERROR(res);
	return(res);
}

int Read_ARGB8888 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	unsigned int Offset_Red, Offset_Green, Offset_Blue/*, Offset_Alpha*/;
	if (format== eImageFormat_ARGB8888)
	{
		/*Offset_Alpha=3; */Offset_Red=2; Offset_Green=1; Offset_Blue=0;
	}
	else if (format== eImageFormat_BGRA8888)
	{
		/*Offset_Alpha=0; */Offset_Red=1; Offset_Green=2; Offset_Blue=3;
	}
	else
	{
		CHECK_ERROR(eError_UnsupportedInputFormat);
		return(eError_UnsupportedInputFormat);
	}
	const _sTableImageFormat*pFormat= GetFormatStruct((enum_ImageFormat)format);
	const int InputPixelDepth=(int)pFormat->PixelDepth;

	OutBuffer.SetFormat(InternalFormatRGB);

	unsigned int Stride=InputWidth*InputPixelDepth;
	// Fill The Source strutur
	if (OutBuffer.Allocate(InputWidth * InputHeight * InputPixelDepth)!=S_OK)
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	if (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK)
	{
		CHECK_ERROR(eError_CannotSetOutputSize);
		return(eError_CannotSetOutputSize);
	}
	unsigned char * pPixel= (unsigned char * )OutBuffer.GetBufferAddr();
	unsigned char *pLineIn=InputBuffer.GetBufferAddr();
	unsigned char *pRead;
	for (int i=InputHeight; i >0; --i)
	{
		pRead=pLineIn;
		for (int j=InputWidth; j>0; --j)
		{
			*(pPixel+ offsetof(InternalStructRGB, Red)  ) =*(pRead+Offset_Red);   // R
			*(pPixel+ offsetof(InternalStructRGB, Green)) =*(pRead+Offset_Green); // G
			*(pPixel+ offsetof(InternalStructRGB, Blue) ) =*(pRead+Offset_Blue);  // B
			pRead +=InputPixelDepth;
			pPixel+=InternalPixelDepth;
		}
		pLineIn+=Stride;
	}
	CHECK_ERROR(res);
	return(res);
}


int Read_RGB888 (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	unsigned int Offset_Red, Offset_Green, Offset_Blue;
	if (format== eImageFormat_RGB888)
	{
		Offset_Red=2; Offset_Green=1; Offset_Blue=0;
	}
	else if (format== eImageFormat_BGR888)
	{
		Offset_Red=0; Offset_Green=1; Offset_Blue=2;
	}
	else
	{
		CHECK_ERROR(eError_UnsupportedInputFormat);
		return(eError_UnsupportedInputFormat);
	}
	const _sTableImageFormat*pFormat= GetFormatStruct((enum_ImageFormat)format);
	const int InputPixelDepth=(int)pFormat->PixelDepth;


	unsigned int Stride=InputWidth*InputPixelDepth;
	// Fill The Source strutur
	OutBuffer.SetFormat(InternalFormatRGB);
	if (OutBuffer.Allocate(InputWidth * InputHeight * InputPixelDepth)!=S_OK)
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	if (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK)
	{
		CHECK_ERROR(eError_CannotSetOutputSize);
		return(eError_CannotSetOutputSize);
	}
	unsigned char * pPixel= (unsigned char * )OutBuffer.GetBufferAddr();
	unsigned char *pLineIn=InputBuffer.GetBufferAddr();
	unsigned char *pRead;
	for (int i=InputHeight; i >0; --i)
	{
		pRead=pLineIn;
		for (int j=InputWidth; j>0; --j)
		{
			*(pPixel+ offsetof(InternalStructRGB, Red)  ) =*(pRead+Offset_Red);   // R
			*(pPixel+ offsetof(InternalStructRGB, Green)) =*(pRead+Offset_Green); // G
			*(pPixel+ offsetof(InternalStructRGB, Blue) ) =*(pRead+Offset_Blue);  // B
			pRead +=InputPixelDepth;
			pPixel+=InternalPixelDepth;
		}
		pLineIn+=Stride;
	}
	CHECK_ERROR(res);
	return(res);
}

int Read_Raw (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int format)
//*************************************************************************************
{ // Raw 8 or 12
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	const unsigned int OuptutPixelDepth=3;
	// Fill The Source strutur
	OutBuffer.SetFormat(InternalFormatRGB);
	if (OutBuffer.Allocate(InputWidth * InputHeight * OuptutPixelDepth)!=S_OK)
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	if (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK)
	{
		CHECK_ERROR(eError_CannotSetOutputSize);
		return(eError_CannotSetOutputSize);
	}
	const unsigned int OutputDepth = 3;
	const unsigned int OutputStride= OutBuffer.GetStrideInByte();
	const unsigned int Offset_Blue=eRGBOffset_Blue, Offset_Red=eRGBOffset_Red, Offset_Green=eRGBOffset_Green;
	//unsigned int Red1_0  , Red1_1  , Red2_0;
	//unsigned int Green1_0, Green1_1, Green2_0, Green2_1;
	//unsigned int Blue2_0, Blue2_1;

	////Default value
	//Red1_0  =1, Red1_1  =3, Red2_0  =-1;
	//Green1_0=0, Green1_1=2, Green2_0=1, Green2_1=3;
	//Blue2_0 =0, Blue2_1 =2;

	do
	{
		unsigned char * pPixelL1= (unsigned char * )OutBuffer.GetBufferAddr();
		unsigned char * pPixelL2= (unsigned char * )OutBuffer.GetBufferAddr()+OutputStride;
		unsigned char *pLineIn =InputBuffer.GetBufferAddr();
		unsigned char *pLineOut=OutBuffer.GetBufferAddr();
		unsigned char *pReadL1, *pReadL2;
		if (format==eImageFormat_Raw8)
		{
			const unsigned int InputPixelDepth=1;
			const unsigned int InputStride=InputWidth * InputPixelDepth;
			for (int i=InputHeight>>1; i >0; --i)
			{
				pReadL1 = pLineIn;
				pReadL2 = pLineIn+InputStride;
				pPixelL1= pLineOut;
				pPixelL2= pLineOut+OutputStride;
				for (int j=InputWidth>>2; j>0; --j)
				{
					//Line 1
					*(pPixelL1+Offset_Red  ) =*(pReadL1+1); // R
					*(pPixelL1+Offset_Green) =*(pReadL1);   // G
					*(pPixelL1+Offset_Blue ) =*(pReadL2);   // B
					pPixelL1 +=OutputDepth;

					*(pPixelL1+Offset_Red  ) =*(pReadL1+1); // R
					*(pPixelL1+Offset_Green) =*(pReadL2+1); // G
					*(pPixelL1+Offset_Blue ) =*(pReadL2);   // B
					pPixelL1 +=OutputDepth;

					*(pPixelL1+Offset_Red  ) =*(pReadL1+1); // R
					*(pPixelL1+Offset_Green) =*(pReadL1+2); // G
					*(pPixelL1+Offset_Blue ) =*(pReadL2+2); // B
					pPixelL1 +=OutputDepth;

					*(pPixelL1+Offset_Red  ) =*(pReadL1+3); // R
					*(pPixelL1+Offset_Green) =*(pReadL2+3); // G
					*(pPixelL1+Offset_Blue ) =*(pReadL2+2); // B
					pPixelL1 +=OutputDepth;

					//Line 2
					*(pPixelL2+Offset_Red  ) =*(pReadL1+1); // R
					*(pPixelL2+Offset_Green) =*(pReadL2+1); // G
					*(pPixelL2+Offset_Blue ) =*(pReadL2);   // B
					pPixelL2 +=OutputDepth;

					*(pPixelL2+Offset_Red  ) =*(pReadL1+1); // R
					*(pPixelL2+Offset_Green) =*(pReadL2+1); // G
					*(pPixelL2+Offset_Blue ) =*(pReadL2+2); // B
					pPixelL2 +=OutputDepth;

					*(pPixelL2+Offset_Red  ) =*(pReadL1+3); // R
					*(pPixelL2+Offset_Green) =*(pReadL2+3); // G
					*(pPixelL2+Offset_Blue ) =*(pReadL2+2); // B
					pPixelL2 +=OutputDepth;

					*(pPixelL2+Offset_Red  ) =*(pReadL1+3); // R
					*(pPixelL2+Offset_Green) =*(pReadL2+3); // G
					*(pPixelL2+Offset_Blue ) =*(pReadL2+2); // B
					pPixelL2 +=OutputDepth;

					pReadL1 +=4;
					pReadL2 +=4;
				}
				pLineIn  += InputStride  <<1;
				pLineOut += OutputStride << 1;
			}
		}
		else if (format==eImageFormat_Raw12)
		{
			ASSERT(0);
			return(eError_UnsupportedInputFormat);
		}
		else
		{
			ASSERT(0);
			return(eError_UnsupportedInputFormat);
		}

	} while (0);
	CHECK_ERROR(res);
	//OutBuffer.bIsYUV=false;
	return(res);
}

int Read_YCbYCr (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int aFormat)
//*************************************************************************************
{ // YCrYCb Interleaved Raster format 16 bits per pixel
	// aFormat gives the kind of supported format 
	// eImageFormat_YCbYCr 1 plane 16b/pix  Y0 Cb Y1 Cr
	// eImageFormat_CrYCbY 1 plane 16b/pix  Cr Y0 Cb Y1
	// eImageFormat_YCrYCb 1 plane 16b/pix  Y0 Cr Y1 Cb
	// eImageFormat_CbYCrY 1 plane 16b/pix  Cb Y0 Cr Y1
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	// Check that width and height are even
	if ( (InputWidth & 1) || (InputHeight & 1) )
	{
		return(eError_InvalidImageSize);
	}
	const int OutPixelDepth=3;
	unsigned int InputStride=InputWidth* 2;
	// Fill The Source strutur
	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if ((OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK) 
		|| (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK))
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}

	do
	{
		_uCbYCr *pCbYCr;
		unsigned char * pLineIn=InputBuffer.GetBufferAddr();
		unsigned char * pPixel;
		//unsigned char cVal;
		pPixel= (unsigned char * )OutBuffer.GetBufferAddr();
		if ( aFormat== eImageFormat_CbYCrY)
		{ // Data are encoded Lsb : Cb Y0 Cr Y1     Msb
			InputStride=InputWidth* 2;
			for (int i=InputHeight; i >0; --i)
			{
				pCbYCr=(_uCbYCr *)pLineIn;
				for (int j=InputWidth >>1; j>0; --j)
				{
					*(pPixel++) = pCbYCr->CbYCrY.Y0;
					*(pPixel++) = pCbYCr->CbYCrY.Cb;
					*(pPixel++) = pCbYCr->CbYCrY.Cr;
					*(pPixel++) = pCbYCr->CbYCrY.Y1;
					*(pPixel++) = pCbYCr->CbYCrY.Cb;
					*(pPixel++) = pCbYCr->CbYCrY.Cr;
					++pCbYCr;
				}
				pLineIn+=InputStride;
			}
		}
		else if ( aFormat== eImageFormat_YCbYCr)
		{ // Data are encoded Lsb : Y0 Cb Y1 Cr     Msb
			InputStride=InputWidth* 2;
			for (int i=InputHeight; i >0; --i)
			{
				pCbYCr=(_uCbYCr *)pLineIn;
				for (int j=InputWidth >>1; j>0; --j)
				{
					*(pPixel++) = pCbYCr->YCbYCr.Y0;
					*(pPixel++) = pCbYCr->YCbYCr.Cb;
					*(pPixel++) = pCbYCr->YCbYCr.Cr;
					*(pPixel++) = pCbYCr->YCbYCr.Y1;
					*(pPixel++) = pCbYCr->YCbYCr.Cb;
					*(pPixel++) = pCbYCr->YCbYCr.Cr;
					++pCbYCr;
				}
				pLineIn+=InputStride;
			}
		}
		else if ( aFormat== eImageFormat_CrYCbY)
		{ // Data are encoded Lsb :Cr Y0 Cb Y1  Msb
			InputStride=InputWidth* 2;
			for (int i=InputHeight; i >0; --i)
			{
				pCbYCr=(_uCbYCr *)pLineIn;
				for (int j=InputWidth >>1; j>0; --j)
				{
					*(pPixel++) = pCbYCr->CrYCbY.Y0;
					*(pPixel++) = pCbYCr->CrYCbY.Cb;
					*(pPixel++) = pCbYCr->CrYCbY.Cr;
					*(pPixel++) = pCbYCr->CrYCbY.Y1;
					*(pPixel++) = pCbYCr->CrYCbY.Cb;
					*(pPixel++) = pCbYCr->CrYCbY.Cr;
					++pCbYCr;
				}
				pLineIn+=InputStride;
			}
		}
		else if ( aFormat== eImageFormat_YCrYCb)
		{ // Data are encoded Lsb : Y0 Cb Y1 Cr     Msb
			InputStride=InputWidth* 2;
			for (int i=InputHeight; i >0; --i)
			{
				pCbYCr=(_uCbYCr *)pLineIn;
				for (int j=InputWidth >>1; j>0; --j)
				{
					*(pPixel++) = pCbYCr->YCrYCb.Y0;
					*(pPixel++) = pCbYCr->YCrYCb.Cb;
					*(pPixel++) = pCbYCr->YCrYCb.Cr;
					*(pPixel++) = pCbYCr->YCrYCb.Y1;
					*(pPixel++) = pCbYCr->YCrYCb.Cb;
					*(pPixel++) = pCbYCr->YCrYCb.Cr;
					++pCbYCr;
				}
				pLineIn+=InputStride;
			}
		}
		else if ( aFormat== eImageFormat_YUV844i)
		{ // YUV 16 bits interlaced 8:4:4
			unsigned char *pRead;
			InputStride=InputWidth* 2;
			for (int i=InputHeight; i >0; --i)
			{
				pRead=pLineIn;
				for (int j=InputWidth; j>0; --j)
				{ //read Luma
					*pPixel     = *(pRead++);     // Luma
					*(pPixel+offsetof(sYUV, Chroma_Blue )) = (*pRead  << 4); // 
					*(pPixel+offsetof(sYUV, Chroma_Red))   =  *pRead & 0xF0;
					pPixel+=OutPixelDepth;
					++pRead;
				}
				pLineIn+=InputStride;
			}
		}
		else  if ( aFormat== eImageFormat_YUV888i)
		{ // 1 plane  YUV 8:8:8
			pPixel= (unsigned char * )OutBuffer.GetBufferAddr();
			unsigned char *pRead;
			InputStride=InputWidth* 3;
			for (int i=InputHeight; i >0; --i)
			{
				pRead=pLineIn;
				for (int j=InputWidth; j>0; --j)
				{ //read Luma
					*pPixel     = *(pRead++);
					*(pPixel+offsetof(sYUV, Chroma_Blue )) = *(pRead++);
					*(pPixel+offsetof(sYUV, Chroma_Red ))  = *(pRead++);
					pPixel+=OutPixelDepth;
				}
				pLineIn+=InputStride;
			}
		}

		else
		{
			ASSERT(0);
			res=eError_UnsupportedInputFormat;
		}
	} while (0);
	//OutBuffer.bIsYUV=true;
	if (res==S_OK)
		OutBuffer.SetFormat(eImageFormat_YUV888i);
	CHECK_ERROR(res);
	return(res);
}

int Read_YUV420mb (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/)
//*************************************************************************************
{ // YCbCrmb  4:2:0 semi-planar MacroBlock format
	int res=0;
	// Fill The Source strutur
	const int OutPixelDepth=3;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	const unsigned int iMacroBlockSize_V = MACRO_BLOCK_SIZE;
	const unsigned int iMacroBlockSize_H = MACRO_BLOCK_SIZE >> 1;
	unsigned char * pPixel, *pBlock;
	const unsigned int iNbMacroBlockPerRow = InputWidth / iMacroBlockSize_H;
	if ( ((InputWidth & 0x7)!=0) || ((InputHeight & 0xF)!=0) )
	{ 
		CHECK_ERROR(eError_WrongResolutionForMB);
		return(eError_WrongResolutionForMB);
	}
	// Fill The Source strutur
	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if (OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK)
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	if (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK)
	{
		CHECK_ERROR(eError_CannotSetOutputSize);
		return(eError_CannotSetOutputSize);
	}
	const unsigned int Stride=OutBuffer.GetStrideInByte();
	do
	{
		unsigned int iRow, iPixel;
		unsigned int iBlock;
		// unsigned char cVal;
		pBlock= (unsigned char * )(OutBuffer.GetBufferAddr());
		pPixel= pBlock;
		iRow  =0;
		iPixel=0;
		iBlock=0;
		unsigned char *pRead=InputBuffer.GetBufferAddr();
		for (unsigned int i=0; i < InputWidth*InputHeight; ++i)
		{ //read Luma
			*pPixel= *(pRead++);
			pPixel+=OutPixelDepth;
			++iPixel;
			if (iPixel >= iMacroBlockSize_H)
			{//Jump to next line
				iPixel=0;
				++iRow;
				pPixel= pBlock + iRow*OutBuffer.GetStrideInByte(); //Point to next row
				if (iRow >= iMacroBlockSize_V)
				{ //Jump to next block
					iRow=0;
					pBlock += iMacroBlockSize_H*OutPixelDepth; //Point to next start block
					++iBlock;
					if ( iBlock >=iNbMacroBlockPerRow)
					{//Jump to next line block
						iBlock=0; //start a new block line
						pBlock+= (iMacroBlockSize_V-1)*OutBuffer.GetStrideInByte();
					}
					pPixel= pBlock;
				}
			}
		}
		// Process the chroma part!
		pBlock= (unsigned char * )(OutBuffer.GetBufferAddr() );
		pPixel= pBlock;
		iRow  =0;
		iPixel=0;
		iBlock=0;
		for (int i=(InputWidth*InputHeight) >> 2; i >=1  ; --i)
		{//read Chroma red first then blue
			// Red part
			*(pPixel + offsetof(sYUV, Chroma_Red )                )            = *(pRead);
			*(pPixel + offsetof(sYUV, Chroma_Red ) + OutPixelDepth    )        = *(pRead);
			*(pPixel + offsetof(sYUV, Chroma_Red )                  + Stride  )= *(pRead);
			*(pPixel + offsetof(sYUV, Chroma_Red ) + OutPixelDepth  + Stride  )= *(pRead);
			// Blue one
			*(pPixel + offsetof(sYUV, Chroma_Blue)             )               = *(pRead+1);
			*(pPixel + offsetof(sYUV, Chroma_Blue)+ OutPixelDepth  )           = *(pRead+1);
			*(pPixel + offsetof(sYUV, Chroma_Blue)               + Stride    ) = *(pRead+1);
			*(pPixel + offsetof(sYUV, Chroma_Blue)+ OutPixelDepth+ Stride    ) = *(pRead+1);
			pRead+=2;
			pPixel+=(OutPixelDepth << 1);
			iPixel +=2;
			if (iPixel >= iMacroBlockSize_H)
			{//Jump to next line
				iPixel=0;
				iRow +=2;
				pPixel= pBlock + iRow*OutBuffer.GetStrideInByte(); //Point to next row
				if (iRow >= iMacroBlockSize_V )
				{ //Jump to next block
					iRow=0;
					pBlock += iMacroBlockSize_H*OutPixelDepth; //Point to next start block
					++iBlock;
					if ( iBlock >=iNbMacroBlockPerRow)
					{//Jump to next line block
						iBlock=0; //start a new block line
						pBlock+= (iMacroBlockSize_V-1)*OutBuffer.GetStrideInByte(); 
					}
					pPixel= pBlock;
				}
			}
		}
	} while (0);
	CHECK_ERROR(res);
	return(res);
}


int Read_YUV422mb (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/)
//*************************************************************************************
{ // YCbCrmb  4:2:x semi-planar MacroBlock format
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	// Fill The Source strutur
	const int OutPixelDepth=3;
	const unsigned int iMacroBlockSize_V = MACRO_BLOCK_SIZE; //16
	const unsigned int iMacroBlockSize_H = MACRO_BLOCK_SIZE >> 1; //8
	unsigned char * pPixel, *pBlock;
	const unsigned int iNbMacroBlockPerRow = InputWidth / iMacroBlockSize_H;
	if ( ((InputWidth & 0x7)!=0) || ((InputHeight & 0xF)!=0) )
	{ 
		CHECK_ERROR(eError_WrongResolutionForMB);
		return(eError_WrongResolutionForMB);
	}

	// Fill The Source strutur
	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if (OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK)
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	if (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK)
	{
		CHECK_ERROR(eError_CannotSetOutputSize);
		return(eError_CannotSetOutputSize);
	}
	do
	{
		unsigned int iRow, iPixel;
		unsigned int iBlock;
		// unsigned char cVal;
		pBlock= (unsigned char * )(OutBuffer.GetBufferAddr());
		pPixel= pBlock;
		iRow  =0;
		iPixel=0;
		iBlock=0;
		unsigned char *pRead=InputBuffer.GetBufferAddr();
		for (unsigned int i=0; i < InputWidth*InputHeight; ++i)
		{ //read Luma
			*pPixel=*(pRead++);
			pPixel+=OutPixelDepth;
			++iPixel;
			if (iPixel >= iMacroBlockSize_H)
			{//Jump to next line
				iPixel=0;
				++iRow;
				pPixel= pBlock + iRow*OutBuffer.GetStrideInByte(); //Point to next row
				if (iRow >= iMacroBlockSize_V)
				{ //Jump to next block
					iRow=0;
					pBlock += iMacroBlockSize_H*OutPixelDepth; //Point to next start block
					++iBlock;
					if ( iBlock >=iNbMacroBlockPerRow)
					{//Jump to next line block
						iBlock=0; //start a new block line
						pBlock+= (iMacroBlockSize_V-1)*OutBuffer.GetStrideInByte(); 
					}
					pPixel= pBlock;
				}
			}
		}
		// Process the chroma part!
		pBlock= (unsigned char * )(OutBuffer.GetBufferAddr() );
		pPixel= pBlock;
		iRow  =0;
		iPixel=0;
		iBlock=0;
		for (int i=(InputWidth*InputHeight) >> 1; i >=1  ; --i)
		{//read Chroma
			*(pPixel  + offsetof(sYUV, Chroma_Red )             )    =*(pRead);
			*(pPixel  + offsetof(sYUV, Chroma_Red )+ OutPixelDepth ) =*(pRead);
			*(pPixel  + offsetof(sYUV, Chroma_Blue)             )    =*(pRead+1);
			*(pPixel  + offsetof(sYUV, Chroma_Blue)+ OutPixelDepth ) =*(pRead+1);
			pRead+=2;
			pPixel+=(OutPixelDepth << 1);
			iPixel +=2;
			if (iPixel >= iMacroBlockSize_H)
			{//Jump to next line
				iPixel=0;
				iRow +=1;
				pPixel= pBlock + iRow*OutBuffer.GetStrideInByte(); //Point to next row
				if (iRow >= iMacroBlockSize_V )
				{ //Jump to next block
					iRow=0;
					pBlock += iMacroBlockSize_H*OutPixelDepth; //Point to next start block
					++iBlock;
					if ( iBlock >=iNbMacroBlockPerRow)
					{//Jump to next line block
						iBlock=0; //start a new block line
						pBlock+= (iMacroBlockSize_V-1)*OutBuffer.GetStrideInByte(); 
					}
					pPixel= pBlock;
				}
			}
		}
	} while (0);
	CHECK_ERROR(res);
	return(res);
}

int ReadRaw_YUV422mb_oldendianess (const _tImageBuffer &InputBuffer, _tImageBuffer &OutBuffer, int /*unused_format*/)
//*************************************************************************************
{ // YCbCrmb  4:2:x semi-planar MacroBlock format
	int res=0;
	const unsigned int InputHeight=InputBuffer.GetHeight();
	const unsigned int InputWidth =InputBuffer.GetWidth();
	// Fill The Source strutur
	const int OutPixelDepth=3;
	const unsigned int iMacroBlockSize_V = MACRO_BLOCK_SIZE;
	const unsigned int iMacroBlockSize_H = MACRO_BLOCK_SIZE >> 1;
	unsigned char * pPixel, *pBlock;
	const unsigned int iNbMacroBlockPerRow = InputWidth / iMacroBlockSize_H;
	// Fill The Source strutur
	OutBuffer.SetFormat(eImageFormat_YUV888i);
	if ((OutBuffer.Allocate(InputWidth * InputHeight * OutPixelDepth)!=S_OK) 
		|| (OutBuffer.SetImageSize(InputWidth, InputHeight)!=S_OK))
	{
		CHECK_ERROR(eError_CannotAllocateBuffer);
		return(eError_CannotAllocateBuffer);
	}
	do
	{
		unsigned int iRow, iPixel;
		unsigned int iBlock;
		// unsigned char cVal;
		pBlock= (unsigned char * )(OutBuffer.GetBufferAddr());
		pPixel= pBlock;
		iRow  =0;
		iPixel=0;
		iBlock=0;
		unsigned char *pRead=InputBuffer.GetBufferAddr();
		for (unsigned int i=0; i < InputWidth*InputHeight; ++i)
		{ //read Luma
			*pPixel=*(pRead++);
			pPixel-=OutPixelDepth;
			++iPixel;
			if (iPixel >= iMacroBlockSize_H)
			{//Jump to next line
				iPixel=0;
				++iRow;
				pPixel= pBlock + iRow*OutBuffer.GetStrideInByte(); //Point to next row
				if (iRow >= iMacroBlockSize_V)
				{ //Jump to next block
					iRow=0;
					pBlock += iMacroBlockSize_H*OutPixelDepth; //Point to next start block
					++iBlock;
					if ( iBlock >=iNbMacroBlockPerRow)
					{//Jump to next line block
						iBlock=0; //start a new block line
						pBlock+= (iMacroBlockSize_V-1)*OutBuffer.GetStrideInByte(); 
					}
					pPixel= pBlock;
				}
			}
		}
		pBlock= (unsigned char * )(OutBuffer.GetBufferAddr() + (iMacroBlockSize_H-1)*OutPixelDepth);
		pPixel= pBlock;
		iRow  =0;
		iPixel=0;
		iBlock=0;
		if (res !=S_OK)
			break;
		for (int i=(InputWidth*InputHeight) >> 1; i >=1  ; --i)
		{//read Chroma 
			*(pPixel + offsetof(sYUV, Chroma_Red )    )                =*(pRead);
			*(pPixel + offsetof(sYUV, Chroma_Red )- OutPixelDepth    ) =*(pRead);
			*(pPixel + offsetof(sYUV, Chroma_Blue)   )                 =*(pRead+1);
			*(pPixel + offsetof(sYUV, Chroma_Blue) - OutPixelDepth  )  =*(pRead+1);
			pRead+=2;
			pPixel-=(OutPixelDepth << 1);
			iPixel +=2;
			if (iPixel >= iMacroBlockSize_H)
			{//Jump to next line
				iPixel=0;
				iRow+=/*2*/1;
				pPixel= pBlock + iRow*OutBuffer.GetStrideInByte(); //Point to next row
				if (iRow >= iMacroBlockSize_V )
				{ //Jump to next block
					iRow=0;
					pBlock += iMacroBlockSize_H*OutPixelDepth; //Point to next start block
					++iBlock;
					if ( iBlock >=iNbMacroBlockPerRow)
					{//Jump to next line block
						iBlock=0; //start a new block line
						pBlock+= (iMacroBlockSize_V-1)*OutBuffer.GetStrideInByte(); 
					}
					pPixel= pBlock;
				}
			}
		}
	} while (0);
	CHECK_ERROR(res);
	return(res);
}


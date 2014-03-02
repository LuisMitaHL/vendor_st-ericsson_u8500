/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "SharedTools.h"
#include "ImageTools.h"
#include "ImageBuffer.h"

#if (defined(_DEBUG) && defined(_MSC_VER))
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


_tImageBuffer::_tImageBuffer()
//*************************************************************************************
{
	pBuffer        = NULL;
	m_Size         = 0;
	m_AllocSize    = 0;
	m_AttachSize   = 0;
	m_IsLocked     = false;
	m_HeaderSize   = 0;
	m_BufferOffset = 0; //Used in get buffer addr
	m_Format       = eImageFormat_UnknowFormat;
	m_pFormatDescription = NULL;
	ResetSizeInfo();
}

_tImageBuffer::~_tImageBuffer()
//*************************************************************************************
{
	Free();
}

int _tImageBuffer::Allocate(size_t size)
//*************************************************************************************
{ // Create a new buffer if required
	if (m_AttachSize==0)
	{
		if (m_AllocSize < size)
			return(CreateNew(size));
		m_Size=size;
		return(S_OK);
	}
	else
	{
		if (m_AttachSize >= size)
		{
			m_Size=size;
			return(S_OK);
		}
		else
		{
			ASSERT(0);
			return(eError_AttachedBufferTooSmall);
		}
	}
}

int _tImageBuffer::CreateNew(size_t size)
//*************************************************************************************
{ // always free and allocate new buffer
	Free();
	if (size!=0)
		pBuffer= new unsigned char[size];
	else
		pBuffer=NULL;
	if (pBuffer)
	{
		m_Size=size;
		m_AllocSize =m_Size;
		return(S_OK);
	}
	else
	{
		m_Size=0;
		m_AllocSize=0;
		return(eError_CannotAllocateMemory);
	}
}

int _tImageBuffer::Free()
//*************************************************************************************
{
	if ((pBuffer!=NULL) && (m_AttachSize==0) && (m_AllocSize!=0))
		delete  [] pBuffer;
	pBuffer      = NULL;
	m_AttachSize  = 0;
	m_Size        = 0;
	m_AllocSize   = 0;
	m_BufferOffset= 0;
	return(S_OK);
}

int _tImageBuffer::WriteToFile(const char *pName)
//*************************************************************************************
{
	if ((pBuffer==NULL) || (m_Size==0))
	{
		return(S_OK);
	}
	FILE *fp=fopen(pName, "wb");
	if (fp==NULL)
	{
		_fnReportError(eError_CannotCreateOutputFile, "Cannot create output file %s", pName);
		return(eError_CannotCreateOutputFile);
	}
	size_t n=fwrite(pBuffer, 1, m_Size, fp);
	if (n !=m_Size)
	{
		ASSERT(n==m_Size);
	}
	fclose(fp);
	return(S_OK);
}

int _tImageBuffer::SetImageSize(unsigned int aWidth, unsigned int aHeight, unsigned int aStrideInPixel_X, unsigned int aStrideInPixel_Y)
//*************************************************************************************
{
	ASSERT(eImageFormat_UnknowFormat!= m_Format);
	if (m_pFormatDescription ==NULL)
	{
		ResetSizeInfo();
		DBG_MSG( "No format declared during SetImageSize()\n");
		return(eError_NoFormatDuringSetImageSize);

	}
	if (aStrideInPixel_X==0)
	{ //Take default stride
		aStrideInPixel_X= (unsigned int) aWidth;
	}
	if (aStrideInPixel_X < aWidth)
	{ //Wrong stride
		ResetSizeInfo();
		ASSERT(aStrideInPixel_X >= aWidth);
		CHECK_ERROR(eError_StrideTooSmallDuringSetImageSize);
		return(eError_StrideTooSmallDuringSetImageSize);
	}

	if (aStrideInPixel_Y==0)
	{ //Take default stride
		aStrideInPixel_Y= (unsigned int) aHeight;
	}
	if (aStrideInPixel_Y < aHeight)
	{ //Wrong stride
		ResetSizeInfo();
		ASSERT(aStrideInPixel_Y >= aHeight);
		CHECK_ERROR(eError_StrideTooSmallDuringSetImageSize);
		return(eError_StrideTooSmallDuringSetImageSize);
	}

	m_Width        = aWidth;
	m_Height       = aHeight;
	m_StridePixel_X= aStrideInPixel_X;
	m_StridePixel_Y= aStrideInPixel_Y;

	unsigned int compute_size=GetNeededSize();



	if (compute_size > m_Size)
	{ //Not enough size
		DBG_MSG( "error in SetImageSize (StrideInPixel(%d) * Height(%d) * PixelDepth(%g))=(%d)  > m_Size(%d)\n", aStrideInPixel_X, aHeight, m_pFormatDescription->PixelDepth, compute_size, m_Size);
		CHECK_ERROR(eError_BufferTooSmallDuringSetImageSize);
		ResetSizeInfo();
		return(eError_BufferTooSmallDuringSetImageSize);
	}
	else
	{
	}
	return(S_OK);
}

int _tImageBuffer::AttachBuffer(unsigned char *ptr, size_t size)
//*************************************************************************************
{
	Free();
	pBuffer     = ptr;
	m_Size      = size;
	m_AttachSize= size;
	return(S_OK);
}

int _tImageBuffer::SetFormat(int format)
//*************************************************************************************
{
	const _sTableImageFormat* pFmt=GetFormatStruct((enum_ImageFormat)format);
	if (pFmt != NULL)
	{
		m_pFormatDescription= pFmt;
		m_Format            = (enum_ImageFormat)format;
		if ( m_Size < GetNeededSize())
			ResetSizeInfo(); //for not having invalid parameters
		return(S_OK);
	}
	else
	{
		m_pFormatDescription= NULL;
		m_Format=eImageFormat_UnknowFormat;
		return(eError_UnknowFormat);
	}
}

size_t _tImageBuffer::GetNeededSize() const
//*************************************************************************************
{ // return the minimum buffer size for given size and format
	if (m_pFormatDescription == NULL)
		return(0);
	ASSERT(m_StridePixel_X >= m_Width);
	ASSERT(m_StridePixel_Y >= m_Height);
	return((size_t) (m_StridePixel_X * m_StridePixel_Y * m_pFormatDescription->PixelDepth));
}

double _tImageBuffer::GetPixelDepth() const 
//*************************************************************************************
{
	if (m_pFormatDescription == NULL)
		return(0.);
	return(m_pFormatDescription->PixelDepth);
}


int _tImageBuffer::ResetSizeInfo()
//*************************************************************************************
{
	m_Width      = 0;
	m_Height     = 0;
	// m_PixelDepth = 0;
	// m_Stride     = 0;
	m_HeaderSize = 0;
	m_StridePixel_X= 0; // the stride in Y axis is evaluated in pixel!
	m_StridePixel_Y= 0; // the stride in Y axis is evaluated in pixel!
	return(S_OK);
}

unsigned int _tImageBuffer::GetStrideInByte(unsigned int plane) const 
//*************************************************************************************
{ // 
	if ((m_pFormatDescription == NULL) || (m_pFormatDescription->NbPlanes <=plane))
		return(0);
	const double *pStride=&m_pFormatDescription->Stride0;
	return( (unsigned int) (pStride[plane] * m_StridePixel_X) );
}


unsigned int _tImageBuffer::ComputeSize(unsigned int width, unsigned int height, int *pStridepixel_X, int *pStridepixel_Y) const
//*************************************************************************************
{ // 
	unsigned int stridepixel_X=0;
	unsigned int stridepixel_Y=0;
	if (pStridepixel_X != NULL)
		stridepixel_X=*pStridepixel_X;
	if (pStridepixel_Y != NULL)
		stridepixel_Y=*pStridepixel_Y;
	if (m_pFormatDescription == NULL)
		return(0);
	if ((unsigned int)stridepixel_X < width)
		stridepixel_X= width;
	if ((unsigned int)stridepixel_Y < height)
		stridepixel_Y= height;
	unsigned int size=(unsigned int)(stridepixel_X * stridepixel_Y *m_pFormatDescription->PixelDepth);
	if (pStridepixel_X != NULL)
		*pStridepixel_X=stridepixel_X;
	if (pStridepixel_Y != NULL)
		*pStridepixel_Y=stridepixel_Y;
	return( size);
}

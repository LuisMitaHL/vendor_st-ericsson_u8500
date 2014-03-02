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
#include "ImageConverter.h"
#include "ImageStream.h"

#if (defined(_DEBUG) && defined(_MSC_VER))
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

ImageStream::ImageStream()
//*************************************************************************************
{
	m_pBuffer             = NULL;
	m_Pos                 = 0;
	m_State               = 0;
	m_SupportedProperties = 0;
}

ImageStream::~ImageStream()
//*************************************************************************************
{
}

int ImageStream::Reset()
//*************************************************************************************
{
	return(S_OK);
}

//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
ImageStream_Buffer::ImageStream_Buffer(): ImageStream()
//*************************************************************************************
{
	DetachBuffer();
}

ImageStream_Buffer::~ImageStream_Buffer()
//*************************************************************************************
{
}

size_t ImageStream_Buffer::Read (unsigned char *ptr,  size_t aElemSize, size_t aCount)
//*************************************************************************************
{
	if ((aCount == 0) ||  (aElemSize== 0))
		return(0);

	size_t nbr=aElemSize*aCount;
	const size_t ifree=(m_Size- m_PosRead);
	if (ifree < nbr)
	{
		aCount=(ifree/aElemSize);
		nbr   =aElemSize*aCount;
	}
	unsigned char *pRead=m_pBuffer+m_PosRead;
	memcpy(ptr, pRead, nbr);
	m_PosRead+=nbr;
	return(aCount);
}

size_t ImageStream_Buffer::Write(const unsigned char *ptr,  size_t aElemSize, size_t aCount)
//*************************************************************************************
{
	if ((aCount == 0) ||  (aElemSize== 0))
		return(0);

	size_t nbr=aElemSize*aCount;
	const size_t ifree=(m_Size- m_PosWrite);
	if (ifree < nbr)
	{
		aCount=(ifree/aElemSize);
		nbr   =aElemSize*aCount;
	}
	unsigned char *pWrite=m_pBuffer+m_PosWrite;
	memcpy(pWrite, ptr, nbr);
	m_PosWrite+= nbr;
	return(aCount);
}

size_t ImageStream_Buffer::Seek(int nbr, enumSeek seek)
//*************************************************************************************
{
	switch(seek)
	{
	case eSeek_Begin:
		m_PosWrite=0;
		m_PosRead =0;
		break;

	case eSeek_End:
		m_PosWrite= m_Size;
		m_PosRead = m_Size;
		break;
	default:
	case eSeek_Current:
		break;
	}
	if (nbr < 0)
	{
		if (m_PosWrite >= (unsigned int)nbr)
			m_PosWrite -= nbr;
		else
			m_PosWrite = 0;
	}
	else // >= 0
	{
		if ((m_PosWrite+nbr) >= m_Size)
			m_PosWrite = m_Size;
		else
			m_PosWrite += nbr;
	}
	m_PosRead = m_PosWrite;
	return(m_PosWrite);
}

int ImageStream_Buffer::AttachBuffer(unsigned char * pBuf, size_t size)
//*************************************************************************************
{
	if (pBuf==NULL)
		return(DetachBuffer());
	m_pBuffer  = pBuf;
	m_Size     = size ;
	m_PosWrite = 0;
	m_PosRead  = 0;
	return(size);
}

int ImageStream_Buffer::DetachBuffer()
//*************************************************************************************
{
	m_pBuffer  = NULL;
	m_Size     = 0 ;
	m_PosWrite = 0;
	m_PosRead  = 0;
	return(0);
}

//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
const char NoFile[]="";
ImageStream_File::ImageStream_File(): ImageStream()
//*************************************************************************************
{
	m_Filename= NoFile;
	m_FP      = NULL;
}

ImageStream_File::~ImageStream_File()
//*************************************************************************************
{
	CloseFile();
}

int ImageStream_File::CloseFile()
//*************************************************************************************
{
	if (m_FP)
	{
		fclose(m_FP);
		m_FP=NULL;
		m_Filename= NoFile;
		Reset();
	}
	return(S_OK);
}

int ImageStream_File::OpenFile(const char *aName, const char *aRights)
//*************************************************************************************
{
	CloseFile();
	m_FP=fopen(aName, aRights);
	if (m_FP==NULL)
		return(eError_CannotOpenFile);
	m_Filename=aName;
	return(S_OK);
}

size_t ImageStream_File::Read (unsigned char *ptr, size_t aElemSize, size_t aCount)
//*************************************************************************************
{
	if (m_FP==NULL)
		return(eError_NoFile);
	size_t res=fread(ptr, aElemSize, aCount, m_FP);
	return(res);
}

size_t ImageStream_File::Write(const unsigned char *ptr, size_t aElemSize, size_t aCount)
//*************************************************************************************
{
	if (m_FP==NULL)
		return(eError_NoFile);
	size_t res=fwrite(ptr, aElemSize, aCount, m_FP);

	return(res);
}

size_t ImageStream_File::Seek(int nbr, enumSeek seek)
//*************************************************************************************
{
	size_t res=(size_t)fseek(m_FP, nbr, seek);
	return(res);
}

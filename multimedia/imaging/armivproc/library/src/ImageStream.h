/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMAGESTREAM_H
#define _IMAGESTREAM_H


class IMAGECONVERTER_API ImageStream
//*************************************************************************************
{
public:
	ImageStream();
	virtual  ~ImageStream();
	virtual size_t Read (unsigned char *ptr,  size_t aElemSize, size_t aCount)=0;
	virtual size_t Write(const unsigned char *ptr,  size_t aElemSize, size_t aCount)=0;
	virtual size_t GetSize()  const { return(0);}
	enum enumSeek
	{
		eSeek_Begin,
		eSeek_End,
		eSeek_Current,
	};

	enum
	{
		eError_CannotOpenFile  =10000,
		eError_NoFile,
	};

	virtual size_t Seek(int nbr, enumSeek)=0;

	unsigned int GetSupportedProperties() const { return(m_SupportedProperties); }
	virtual  int Reset();
	unsigned char *GetBufferAddr() const {return m_pBuffer; }
	size_t         GetPos() const {return m_Pos; }
	unsigned int   GetState() const {return m_State; }

protected:
	unsigned char *m_pBuffer;
	size_t         m_Pos;
	unsigned int   m_State;
	unsigned int   m_SupportedProperties;
} /*_tImageBuffer*/;


class IMAGECONVERTER_API ImageStream_Buffer: public ImageStream
//*************************************************************************************
{
public:
	ImageStream_Buffer();
	virtual  ~ImageStream_Buffer();
	int AttachBuffer(unsigned char * pBuf, size_t size);
	int DetachBuffer();
	virtual size_t Read (unsigned char *ptr,  size_t aElemSize, size_t aCount);
	virtual size_t Write(const unsigned char *ptr,  size_t aElemSize, size_t aCount);
	virtual size_t Seek(int nbr, enumSeek);
	virtual size_t GetSize()  const { return(m_Size);}
protected:
	//unsigned char * m_pBuffer;
	unsigned char  *m_pCurrentPtr;
	size_t          m_Size;
	size_t          m_PosWrite;
	size_t          m_PosRead;
} /*ImageStream_Buffer*/;


class IMAGECONVERTER_API ImageStream_File: public ImageStream
//*************************************************************************************
{
public:
	ImageStream_File();
	virtual  ~ImageStream_File();
	virtual size_t Read (unsigned char *ptr,  size_t aElemSize, size_t aCount);
	virtual size_t Write(const unsigned char *ptr,  size_t aElemSize, size_t aCount);
	virtual size_t Seek(int nbr, enumSeek);
	int OpenFile(const char *name, const char *aRights);
	int CloseFile();
protected:
	const char * m_Filename;
	FILE       * m_FP;
} /*ImageStream_Buffer*/;

#endif // End of #ifndef _IMAGESTREAM_H

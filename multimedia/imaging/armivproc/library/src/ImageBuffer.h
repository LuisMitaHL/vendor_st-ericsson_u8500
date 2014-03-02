/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _IMAGEBUFFER_H_
#define _IMAGEBUFFER_H_


typedef unsigned char _tPixelElement;


class IMAGECONVERTER_API _tImageBuffer
//*************************************************************************************
{
public:
	_tImageBuffer();
	~_tImageBuffer();
	//bool         bIsYUV;

	int CreateNew(size_t size);
	int Allocate(size_t size);

	int Free();
	int WriteToFile(const char *pName);
	int SetImageSize(unsigned int aWidth, unsigned int aHeight, unsigned int aStrideInPixel_X=0, unsigned int aStrideInPixel_Y=0);
	size_t GetSize() const { return(m_Size);}  //Return the allocated size
	size_t GetNeededSize() const; //take care of the stride

	int AttachBuffer(unsigned char *ptr, size_t size);
	_tPixelElement *GetBufferAddr() const { return(pBuffer + m_BufferOffset);}
	double GetPixelDepth() const;

	unsigned int GetWidth()  const { return(m_Width);}
	unsigned int GetHeight() const { return(m_Height);}

	unsigned int GetStrideInByte(unsigned int plane=0) const;
	unsigned int GetStrideInPixel_X() const { return(m_StridePixel_X);}
	unsigned int GetStrideInPixel_Y() const { return(m_StridePixel_Y);}

	unsigned int ComputeSize(unsigned int width, unsigned int height, int *pStridepixel_X=NULL, int *pStridepixel_Y=NULL) const;

	int ResetSizeInfo();

	unsigned int GetBufferOffset() const { return(m_BufferOffset); }
	int  SetBufferOffset(unsigned int offset) { if (offset <m_Size) { m_BufferOffset=offset; return(S_OK);} else return(-1); }

	bool         IsYUV() const { if (m_pFormatDescription) return(m_pFormatDescription->IsYUV); else return(true);}
	bool         IsLocked() const { return(m_IsLocked);}
	void         Locked(bool locked)  { m_IsLocked=locked; }
	
	enum_ImageFormat GetFormat() const {return(m_Format); }
	int SetFormat(int format);

	size_t GetHeaderSize() const {return(m_HeaderSize); }
	void SetHeaderSize(size_t size) {m_HeaderSize=size; }
protected:
	unsigned char *pBuffer;

	size_t        m_Size;
	size_t        m_AllocSize;  // The size allocated by the class
	size_t        m_AttachSize; // When a buffer is attached to the class this value is not null
	//double        m_PixelDepth;

	unsigned int  m_Width;
	unsigned int  m_Height;
	//unsigned int  m_Stride;
	unsigned int  m_StridePixel_X; // the stride is evaluated in pixel!
	unsigned int  m_StridePixel_Y; // the stride for Y axis is evaluated in pixel!

	bool          m_IsLocked;

	size_t            m_HeaderSize;    // Not used
	size_t            m_BufferOffset;  // used in get buffer addr
	enum_ImageFormat  m_Format;
	const _sTableImageFormat *m_pFormatDescription;


};

#endif // End of #ifndef _IMAGEBUFFER_H_

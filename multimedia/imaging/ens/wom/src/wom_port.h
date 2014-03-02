/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_PORT_H_
#define _WOM_PORT_H_

enum
{
	eError_CannotCreateInputPort=-100,
	eError_CannotCreateOutputPort,
};

class WOM_API Wom_PortOther: public ImgEns_Port
//*************************************************************************************************************
{
public:
	Wom_PortOther(const ImgEns_CommonPortData& commonPortData, ImgEns_Component& ensComp);
	virtual ~Wom_PortOther();
	virtual OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const ;
	virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;
	virtual const _tImageInfo& GetImageInfo() const {return(m_ImageInfo);}

	OMX_PARAM_PORTDEFINITIONTYPE&  getParamPortDefinition() { return(mParamPortDefinition); }

	OMX_VIDEO_CODINGTYPE  Get_CompressionFormat() const;
	OMX_COLOR_FORMATTYPE  Get_ColorFormat() const;
	OMX_NATIVE_WINDOWTYPE Get_NativeWindow() const;
	OMX_U32               Get_Bitrate() const ;
	OMX_BOOL              Get_FlagErrorConcealment() const;
	OMX_U32               Get_Framerate() const;

	bool GetReserveExtradataForInput() const { return m_bReserveExtradataForInput; }
	void SetReserveExtradataForInput(bool reserve) { m_bReserveExtradataForInput=reserve; }
	OMX_BUFFERHEADERTYPE * GetBufferHeaderPtr(unsigned int index) /*const*/ { return(getEnsBufferHdrTablePtr()[index].mBufHdrPtr); }
protected:
	MMHwBuffer    *m_buffersPool;
	ImgEns_Component &m_Omx_Component;

	_tImageInfo m_ImageInfo;
	bool           m_bReserveExtradataForInput; // false by default

	//size_t    mImageSize; //Size of image in bytes
#if 0
	//Not enable for the moment
	_tBufferSkipping   m_Skipper;
public:
	_tBufferSkipping &Get_BufferSkipping(void) { return(m_Skipper); }
	bool                  Get_SkipBufferEnable() const { return(m_Skipper.bEnable); }
	void                  Set_SkipBufferEnable(bool enable) { m_Skipper.bEnable=enable; }
#endif
};


class WOM_API Wom_PortVideo: public Wom_PortOther
//*************************************************************************************************************
{
public:
	Wom_PortVideo(const ImgEns_CommonPortData& commonPortData, ImgEns_Component& ensComp);
	virtual ~Wom_PortVideo();
	//virtual OMX_ERRORTYPE checkCompatibility(OMX_HANDLETYPE hTunneledComponent, OMX_U32 nTunneledPort) const ;
	//virtual OMX_ERRORTYPE doBufferAllocation(OMX_U32 nBufferIndex,OMX_U32 nSizeBytes, OMX_U8 **ppData, void **ppBufferMetaData);
	//virtual OMX_ERRORTYPE doBufferDeAllocation(OMX_U32 nBufferIndex,void *pBufferMetaData);
	virtual OMX_ERRORTYPE setFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE &portdef) ;

protected:
};


#endif //_WOM_PORT_H_

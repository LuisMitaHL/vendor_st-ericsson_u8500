/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOMDEMOCPT_COMPONENT_H_
#define _WOMDEMOCPT_COMPONENT_H_

class womDemoCpt_Component: public Wom_Component
//*************************************************************************************************************
{
public:
	womDemoCpt_Component(const char *name="womDemoCpt", unsigned int instanceNb=0);
	virtual ~womDemoCpt_Component();
	virtual int Construct(OMX_COMPONENTTYPE &Handle, const char *name=NULL); //Make the real constuction
	virtual int PortFormatChanged(ImgEns_Port &port);/// call when a set param has been done on a port

	virtual void *        getConfigAddr(OMX_INDEXTYPE nConfigIndex, OMX_PTR pParameter, size_t *StructSize=NULL, int *pOffset=NULL) const;
	virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;
protected:
};

#endif // _WOMDEMOCPT_COMPONENT_H_

/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#ifndef _WOM_COMPONENT_H_
#define _WOM_COMPONENT_H_

class Img_EnsWrapper;

class WOM_API Wom_Component: public ImgEns_Component
//*******************************************************************************
{
public:
	Wom_Component(const char *name, unsigned int instanceNb = 0);
	virtual ~Wom_Component();

	virtual int Construct(OMX_COMPONENTTYPE &Handle, const char *name=NULL);

	virtual void *GetWrapper() { return(m_pWrapper);}
	virtual void SetWrapper(Img_EnsWrapper *pWrapper);

	virtual OMX_PTR getConfigAddr(OMX_INDEXTYPE nIndex, OMX_PTR pParameter, size_t *pStructSize, int *pOmxStructOffset) const;

protected:
	virtual int Destruct(); // call at the end of the life of component
	virtual int CreatePort(const ImgEns_CommonPortData &aPortData);

	Img_EnsWrapper *m_pWrapper;
	bool            m_bUseBufferSharing;
};

#endif // _WOM_COMPONENT_H_

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _OMXISPPROC_H_
#define _OMXISPPROC_H_


#include "OMX_Core.h"
#include "IFM_Types.h"
#include "ENS_Component.h"
#include "ImgConfig.h"

#undef OMXCOMPONENT
#define OMXCOMPONENT "OMXISPPROC"


OMX_ERRORTYPE OMXIspProcFactoryMethod(ENS_Component_p * ppENSComponent);


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(COmxIspProc_RDB);
#endif
class COmxIspProc_RDB : public ENS_ResourcesDB {
    public:
    	COmxIspProc_RDB();

};


#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(COmxIspProc);
#endif
class COmxIspProc: public ENS_Component {
	friend class IspProc;
public :
	COmxIspProc();
	virtual OMX_ERRORTYPE createResourcesDB();

    RM_STATUS_E getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData);
    
    virtual OMX_ERRORTYPE getParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure) const;

    virtual OMX_ERRORTYPE setParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure);
    virtual OMX_ERRORTYPE setConfig(
    		OMX_INDEXTYPE nIndex,
    		OMX_PTR pStructure);

    virtual OMX_ERRORTYPE getConfig(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure) const;

    virtual OMX_ERRORTYPE getExtensionIndex(
            OMX_STRING cParameterName,
            OMX_INDEXTYPE* pIndexType) const;

    void setRotation(int rotation) {mPortRotateParam.nRotation = rotation;}

    virtual void NmfPanicCallback(void *contextHandler,t_nmf_service_type serviceType, t_nmf_service_data *serviceData);

protected :


	// Specific indexes
	IFM_CONFIG_READPETYPE mIfmReadPEType;                 // IFM_IndexConfigCommonReadPe
	IFM_CONFIG_WRITEPETYPE mIfmWritePEType;               // IFM_IndexConfigCommonWritePe
	IFM_CONFIG_ISPSHARED mIspShared;
	OMX_SYMBIAN_CONFIG_BOOLEANTYPE  mAutoRotateParam;
	OMX_CONFIG_ROTATIONTYPE mPortRotateParam;
	OMX_BOOL bAutoRotation;

private :

	OMX_ERRORTYPE createIspprocPort(OMX_U32 nPortIndex,
	        OMX_DIRTYPE eDir,
	        OMX_BUFFERSUPPLIERTYPE eSupplierPref);
};

#endif /* _OMXISPPROC_H_ */

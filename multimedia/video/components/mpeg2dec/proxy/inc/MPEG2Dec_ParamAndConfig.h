/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __MPEG2DEC_PARAMANDCONFIG_H
#define __MPEG2DEC_PARAMANDCONFIG_H

#include "VFM_ParamAndConfig.h"

/// @ingroup MPEG2Decoder
/// @brief This class contains the parameters and configuration of the MPEG2 Decode OpenMax Component
/// It is also used to set and get the configuration, without processing.

class MPEG2Dec_ParamAndConfig: public VFM_ParamAndConfig
{
    public:
        /// @brief Constructor, that inializes the default configuration
        MPEG2Dec_ParamAndConfig(VFM_Component *component);
        OMX_ERRORTYPE reset();

        OMX_ERRORTYPE setIndexParamResourceSvaMcps(OMX_PTR pMcpsType);

        OMX_ERRORTYPE getIndexParamResourceSvaMcps(OMX_PTR pMcpsType) const;
        OMX_ERRORTYPE getIndexConfigCommonOutputCrop(OMX_PTR pt_org) const;
    
        OMX_U32 resourceSvaMcps() const;

        void resourceSvaMcps(const OMX_U32& mcps) ;

        //+CR369593
		   OMX_U32 CropWidth;
		   OMX_U32 CropHeight;
        //-CR369593

	private:
		OMX_U32   m_nSvaMcps;
};	

inline OMX_U32 MPEG2Dec_ParamAndConfig::resourceSvaMcps() const 
{ return m_nSvaMcps; }

inline void MPEG2Dec_ParamAndConfig::resourceSvaMcps(const OMX_U32& mcps)
{ m_nSvaMcps = mcps; }


#endif /* __MPEG2DEC_PARAMANDCONFIG_H */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264Dec_PROXY_H
#define __H264Dec_PROXY_H

#include "VFM_Component.h"
#include "H264Dec_ParamAndConfig.h"
#include "H264Dec_NmfHost_ProcessingComponent.h"

class H264Dec_GetDDepParameterCB;

/// @defgroup H264Decoder H264 Decoder Proxy
/// @brief The OpenMaxIL Proxy dedicated to the H264 Decoder is
/// implemented in H264Dec_Proxy, and is derived from
/// VFM_Component, that is derived from the ENS_Component. Such a component
/// can be instantiated using ENS_GetHandle() with the component name "OMX.ST.VFM.H264Dec".
///
/// @par Development of the H264 Decoder OpenMax Component
/// The H264 Decoder OpenMax Component is defined in 4 classes:
/// - H264Dec_Proxy class contains the proxy itself. It implements the 
///   OpenMax API, that is SetConfig(),... This class is derived from
///   VFM_Component, that is itself derived from ENS_Component, in order
///   to benefit from commonalized functionalities between all or part of
///   the encoders / decoders
/// - H264Dec_ParamAndConfig mainly contains the attributes to describes the
///   current parameters and configuration of the component. This class is
///   inherited by the proxy H264Dec_Proxy, so that the proxy knows all about
///   these parameters. This class is derived from VFMEnc_ParamAndConfig
///   in order to benefit from common parameters and configuration shared
///   by other encoders.

/// @ingroup H264Decoder
/// @brief If we want to optimize the computation of the header and quantization
/// table or not (if already computed)
const int _statusOptimize=1;

/// @ingroup H264Decoder
/// {
/// @brief The OpenMaxIL Proxy dedicated to the H264 Decoder.
/// Such a component can be instantiated using ENS_GetHandle()
/// with the component name "OMX.ST.VFM.H264Dec".
/// Refer to \ref H264Decoder for a complete description
///
/// This class is the base class of the H264 Decoder OpenMax Component.
/// This class inherits from
/// - to be done!
///
class H264Dec_Proxy: public VFM_Component
{
    friend class H264Dec_Port;
    friend class H264Dec_NmfHost_ProcessingComponent;
    friend class H264Dec_GetConfigCB;
    
    private:
        /// @brief Parameters and Configuration Storing
        H264Dec_ParamAndConfig mParam;
        t_cm_domain_id mpc_domain_id;
        H264Dec_NmfHost_ProcessingComponent processingComponent;

    public:                        /* all the methods */
        /// @brief Constructor, to initialize the configuration and the parameters.
        ///
        /// It mostly initialize the parameters, but
        /// you cannot afford setting the parameters of the ports
        /// The constructor is called when calling ENS_GetHandle()
        /// with the component name "OMX.ST.VFM.H264Dec"
        /// (check for function H264DecFactoryMethod() and VFM_Init())
        H264Dec_Proxy();

        /// Destructor
        virtual ~ H264Dec_Proxy();

        virtual OMX_ERRORTYPE construct();
        
        /// @brief Set parameters to the component, from the application. It is part
        /// of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
        /// - list to be done
        /// - All the ones supported by VFM_Component::setParameter()
        /// @param [in] pt the structure that contains the parameters we want to set
        /// @return OMX_ErrorNone if no error, an error otherwise
        /// @todo Adopt a generic way to handle error. This is a general comment
        virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);
        
        /// @brief Get parameters from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are: \n
        /// - list to be done
        /// - All the ones supported by VFM_Component::getParameter()
        /// @param [out] pt the structure that will contains the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

        /// @brief Set configuration to the component, from the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
        /// - OMX_IndexParamQFactor can be used dynamically using SetConfig()
        /// - OMX_IndexConfigCommonRotate to set on-the-fly rotation. Rotation of 0, 90, -90 and 270 degrees are supported
        /// - OMX_IndexConfigCommonOutputCrop
        /// - \ref VFM_IndexConfigTargetbpp
        /// - All the ones supported by VFM_Component::setConfig()
        /// @param [in] pt structure that contains the parameters we want to set
        /// @return OMX_ErrorNone if no error, an error otherwise
        virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

        /// @brief Set the configuration to the component, when we are still in loaded state.
        /// @param [in] nParamIndex index of the structure we want to set. Supported indexes are
        ///             the same than the ones of the setConfig() function
        /// @param [in] pt structure that contains the parameters we want to set
        /// @return OMX_ErrorNone if no error, an error otherwise
        /// @todo make the real configuration for it, like in the example from Geraldine
//        virtual OMX_ERRORTYPE setConfigInLoadedState(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
//        { return setConfig(nParamIndex, pt); }
            
        /// @brief Get configuration from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are:
        /// - list to be done
        /// - \ref VFM_IndexConfigTargetbpp
        /// - All the ones supported by VFM_Component::getConfig()
        /// @param [out] pt structure that will contain the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

        virtual OMX_ERRORTYPE getExtensionIndex(
            OMX_STRING cParameterName,
            OMX_INDEXTYPE* pIndexType) const;

        virtual OMX_ERRORTYPE reset() { return mParam.reset(); };

        virtual VFM_Port *newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);
        static VFM_Component *newProxy();
        
        virtual void setProfileLevelSupported();
        virtual void setVideoSettingsSupported();
    
        virtual RM_STATUS_E getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl, 
                        OMX_INOUT RM_EMDATA_T* pEstimationData);

        static int getDpbSizeInBytes(OMX_VIDEO_AVCLEVELTYPE level);
        
        virtual OMX_S32 getMaxBuffers(OMX_U32 portIndex) const;
        void h264dec_assert(int condition, int line, OMX_BOOL isFatal);
        static void h264dec_assert_static(int condition, int line, OMX_BOOL isFatal);
        virtual void NmfPanicCallback(
              void *contextHandler           , 
              t_nmf_service_type serviceType , 
              t_nmf_service_data *serviceData);
// +CR324558 CHANGE START FOR
		OMX_U32 addExtraPipeBuffers(OMX_U32 bufferCount);
		OMX_U32 subtractExtraPipeBuffers(OMX_U32 bufferCount);
// -CR324558 CHANGE END OF
};

/// @}

#endif // __H264Dec_PROXY_H

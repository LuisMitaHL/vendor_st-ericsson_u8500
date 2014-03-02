/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __H264Enc_PROXY_H
#define __H264Enc_PROXY_H

#include "VFM_Component.h"
#include "H264Enc_ParamAndConfig.h"
#include "H264Enc_NmfHost_ProcessingComponent.h" // include osi_trace.h => IN, OUT, MSG definitions
#include "H264ENC_top.h"


/// @defgroup H264Encoder H264 Encoder Proxy
/// @brief The OpenMaxIL Proxy dedicated to the H264 Encoder is
/// implemented in H264Enc_Proxy, and is derived from
/// VFM_Component, that is derived from the ENS_Component. Such a component
/// can be instantiated using ENS_GetHandle() with the component name "OMX.ST.VFM.H264Enc".
///
/// @par Development of the H264 Encoder OpenMax Component
/// The H264 Encoder OpenMax Component is defined in 4 classes:
/// - H264Enc_Proxy class contains the proxy itself. It implements the
///   OpenMax API, that is SetConfig(),... This class is derived from
///   VFM_Component, that is itself derived from ENS_Component, in order
///   to benefit from commonalized functionalities between all or part of
///   the encoders / decoders
/// - H264Enc_ParamAndConfig mainly contains the attributes to describes the
///   current parameters and configuration of the component. This class is
///   inherited by the proxy H264Enc_Proxy, so that the proxy knows all about
///   these parameters. This class is derived from VFM_ParamAndConfig
///   in order to benefit from common parameters and configuration shared
///   by other encoders.
class H264Enc_Proxy: public VFM_Component
{
    private:
        /// @brief Object to allow automatic NMF instantiation
        H264Enc_NmfHost_ProcessingComponent processingComponent;

        /// @brief Parameters and Configuration Storing
        //H264Enc_ParamAndConfig mParam;

    public:                        /* all the methods */
        /// @brief Constructor, to initialize the configuration and the parameters.
        ///
        /// It mostly initialize the parameters, but
        /// you cannot afford setting the parameters of the ports
        /// The constructor is called when calling ENS_GetHandle()
        /// with the component name "OMX.ST.VFM.H264Enc"
        /// (check for function H264EncFactoryMethod() and VFM_Init())
        H264Enc_Proxy();
        H264Enc_ParamAndConfig mParam;

        /// Destructor
        virtual ~ H264Enc_Proxy();

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
        // virtual OMX_ERRORTYPE setConfigInLoadedState(OMX_INDEXTYPE nParamIndex, OMX_PTR pt)
        // { return setConfig(nParamIndex, pt); }

        /// @brief Get configuration from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are:
        /// - list to be done
        /// - \ref VFM_IndexConfigTargetbpp
        /// - All the ones supported by VFM_Component::getConfig()
        /// @param [out] pt structure that will contain the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

        /// @brief Function called on OMX_GetExtensionIndex for video specific indexes
        /// @param[in] cParameterName The string of the index
        /// @param[out] pIndexType The corresponding index identifier
        /// @return An error if any
        virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;

        virtual OMX_BOOL isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const;

        virtual OMX_ERRORTYPE reset() { return mParam.reset(); };

        virtual VFM_Port *newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);
        static VFM_Component *newProxy();

        virtual void setProfileLevelSupported();
        virtual void setVideoSettingsSupported();

        virtual RM_STATUS_E getResourcesEstimation (OMX_IN const OMX_PTR pCompHdl,
                        OMX_INOUT RM_EMDATA_T* pEstimationData);

        H264Enc_ParamAndConfig  *getParamAndConfig() { return &mParam; }

        //this flag is introduced for handling of selecting algo_HDTV for resolutions above qHD
        OMX_BOOL isSpecialCTCase;

        /// @brief Function callback function used by Ploatform teams to enable OSTTraces
        virtual OMX_U32 getUidTopDictionnary(void) { return KOstH264ENC_top_ID; }

        virtual void NmfPanicCallback(void *contextHandler,	t_nmf_service_type serviceType, t_nmf_service_data *serviceData);

    public: // extension indexes
        // SHAI
        OMX_INDEXTYPE mIndexParamPixelAspectRatio;
        OMX_INDEXTYPE mIndexParamColorPrimary;
        OMX_INDEXTYPE mIndexParamVideoAvcSei;
        OMX_INDEXTYPE mIndexConfigVideoSequenceHeader;
        // Private
        OMX_INDEXTYPE mIndexParamHeaderDataChanging;
        OMX_INDEXTYPE mIndexParamCpbBufferSize;
};

/// @}

#endif // __H264Enc_PROXY_H

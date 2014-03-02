/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __EXIF_MIXER_PROXY_H
#define __EXIF_MIXER_PROXY_H

#include "VFM_Component.h"
#include "EXIFMixer_ParamAndConfig.h"
#include "EXIFMixer_ArmNmfProcessingComponent.h"


/// @defgroup EXIFMixer EXIF Mixer Proxy
/// @brief The OpenMaxIL Proxy dedicated to the EXIF Mixer is
/// implemented in JPEGEnc_Proxy, and is derived from
/// VFM_Component, that is derived from the ENS_Component. Such a component
/// can be instantiated using ENS_GetHandle() with the component name "OMX.ST.VFM.EXIFMIXER".
///
/// @par Development of the EXIF Mixer OpenMax Component
///
/// The EXIF Mixer OpenMax Component is defined in 4 classes:
/// - EXIFMixer_Proxy class contains the proxy itself. It implements the
///   OpenMax API, that is SetConfig(),... This class is derived from
///   VFM_Component, that is itself derived from ENS_Component, in order
///   to benefit from commonalized functionalities between all or part of
///   the encoders / decoders
/// - EXIFMixer_ParamAndConfig mainly contains the attributes to describes the
///   current parameters and configuration of the component. This class is
///   inherited by the proxy JPEGEnc_Proxy, so that the proxy knows all about
///   these parameters. This class is derived from VFMEnc_ParamAndConfig
///   in order to benefit from common parameters and configuration shared
///   by other encoders.
/// - EXIFMixer_ArmNMFHostProcessing is the class that groups the processing of the JPEG Encoder
///   that is performed on the ARM side. We can find the quantization table scaling
///   and the JPEG Header creation in this class.
///
/// @par EXIF Mixer OpenMax Component Overview
/// @image html EXIF_Mixer.png
///
///

/// @ingroup EXIFMixer
/// {
/// @brief The OpenMaxIL Proxy dedicated to the JPEG encoder.
/// Such a component can be instantiated using ENS_GetHandle()
/// with the component name "OMX.ST.VFM.EXIFMixer".
/// Refer to \ref EXIFMixer for a complete description
///
/// This class is the base class of the EXIF Mixer OpenMax Component.
/// This class inherits from
/// - VFM_Component, that inherits from the ENS_Component, to inherits from
///   common proxy functionalities
/// - EXIFMixer_ParamAndConfig to store the current parameter and configuration of the component
/// - EXIFMixer_Processing to access processing function (quantization table scaling and header creation)
///
class EXIFMixer_Proxy: public VFM_Component
{
    friend class EXIFMixer_ArmNmfProcessingComponent;
    friend class EXIFMixer_Port;


    private:
        /// @brief Parameters and Configuration Storing
        EXIFMixer_ParamAndConfig mParam;

        /// @brief Object to allow automatic NMF instantiation
        EXIFMixer_ArmNmfProcessingComponent processingComponent;

        /// @brief Is mpc param up-to-date or not on mpc side?
        VFM_Status mSendParamToARMNMF;



    public:                        /* all the methods */
        /// @brief Constructor, to initialize the configuration and the parameters.
        ///
        /// It mostly initialize the parameters, but
        /// you cannot afford setting the parameters of the ports
        /// The constructor is called when calling ENS_GetHandle()
        /// with the component name "OMX.ST.VFM.JPEGEnc"
        /// (check for function EXIFMixerFactoryMethod() and VFM_Init())
        EXIFMixer_Proxy();
		OMX_U32 size_jpeg;
        /// Destructor
        virtual ~ EXIFMixer_Proxy();

		virtual	OMX_ERRORTYPE construct();
		//virtual OMX_BOOL isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const;

		virtual VFM_Port *newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);

		void exif_mixer_proxy_assert(OMX_U32 condition, OMX_U32 errorType, OMX_U32 line, OMX_BOOL isFatal);

        /// @brief Set parameters to the component, from the application. It is part
        /// of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
        /// - OMX_IndexParamPortDefinition is mandatory to initialize the encoder
        /// - OMX_IndexParamImagePortFormat
        /// - OMX_IndexParamQFactor
        /// - All the ones supported by VFM_Component::setParameter()
        /// @param [in] pt the structure that contains the parameters we want to set
        /// @return OMX_ErrorNone if no error, an error otherwise
        /// @todo Adopt a generic way to handle error. This is a general comment
        //virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

        /// @brief Get parameters from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are: \n
        /// - OMX_IndexParamPortDefinition is mandatory to initialize the encoder
        /// - OMX_IndexParamImagePortFormat
        /// - OMX_IndexParamQFactor
        /// - All the ones supported by VFM_Component::getParameter()
        /// @param [out] pt the structure that will contains the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        //virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

        /// @brief Set configuration to the component, from the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
        /// - OMX_IndexParamQFactor can be used dynamically using SetConfig()
        /// - OMX_IndexConfigCommonRotate to set on-the-fly rotation. Rotation of 0, 90, -90 and 270 degrees are supported
        /// - OMX_IndexConfigCommonOutputCrop
        /// - \ref VFM_IndexConfigTargetbpp
        /// - All the ones supported by VFM_Component::setConfig()
        /// @param [in] pt structure that contains the parameters we want to set
        /// @return OMX_ErrorNone if no error, an error otherwise
       // virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);
		//virtual OMX_ERRORTYPE setConfigInternal(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

        /// @brief Get configuration from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are:
        /// - OMX_IndexParamQFactor
        /// - OMX_IndexConfigCommonRotate
        /// - OMX_IndexConfigCommonOutputCrop
        /// - \ref VFM_IndexConfigTargetbpp
        /// - All the ones supported by VFM_Component::getConfig()
        /// @param [out] pt structure that will contain the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        //virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

        virtual OMX_ERRORTYPE reset() { return OMX_ErrorNone; }

		virtual void setProfileLevelSupported() {}
		OMX_ERRORTYPE notifyportsetting(OMX_U32 size);

		virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);
		virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pt) const;
};

#endif // __EXIF_MIXER_PROXY_H

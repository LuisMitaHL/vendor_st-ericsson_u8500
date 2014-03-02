/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
 
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef __JPEGENC_PROXY_H
#define __JPEGENC_PROXY_H

#include "VFM_Component.h"
#include "JPEGEnc_ParamAndConfig.h"
#ifdef HOST_MPC_COMPONENT_BOTH
#include "JPEGEnc_NmfMpc_ProcessingComponent.h"
#endif
#include "JPEGEnc_ArmNmfProcessingComponent.h"

///  THUMBNAIL_W and THUMBNAIL_H defines the resolution of thumbanial image.
#define THUMBNAIL_W 160
#define THUMBNAIL_H	120

/// @defgroup JPEGEncoder JPEG Encoder Proxy
/// @brief The OpenMaxIL Proxy dedicated to the JPEG encoder is
/// implemented in JPEGEnc_Proxy, and is derived from
/// VFM_Component, that is derived from the ENS_Component. Such a component
/// can be instantiated using ENS_GetHandle() with the component name "OMX.ST.VFM.JPEGEnc".
///
/// @par Development of the JPEG Encoder OpenMax Component
///
/// The JPEG Encoder OpenMax Component is defined in 4 classes:
/// - JPEGEnc_Proxy class contains the proxy itself. It implements the
///   OpenMax API, that is SetConfig(),... This class is derived from
///   VFM_Component, that is itself derived from ENS_Component, in order
///   to benefit from commonalized functionalities between all or part of
///   the encoders / decoders
/// - JPEGEnc_ParamAndConfig mainly contains the attributes to describes the
///   current parameters and configuration of the component. This class is
///   inherited by the proxy JPEGEnc_Proxy, so that the proxy knows all about
///   these parameters. This class is derived from VFMEnc_ParamAndConfig
///   in order to benefit from common parameters and configuration shared
///   by other encoders.
/// - JPEGEnc_NMF is the class that groups the instantiation / deinstantiation
///   support of the NMF component
/// - JPEGEnc_Processing is the class that groups the processing of the JPEG Encoder
///   that is performed on the ARM side. We can find the quantization table scaling
///   and the JPEG Header creation in this class.
///
/// @par JPEG Encoder OpenMax Component Overview
/// @image html JPEGEnc_OMX.png
///
/// The component is a standard OpenMax Component, based on image_encoder.JPEG standard component. It takes an image in input
/// and returns the coded stream in output. It is conformed to the
/// JPEG Encoder as described on 8500 platform, with its limitation (no progressive encoding for example).
///
/// @note that unlike on the 8815 / 8820 architecture, the header of the JPEG coded stream, and the information parameters,
/// are computed directly by the OpenMax JPEG Encode, and no more by the application
///
/// The use of this OpenMaxIL Component is performed following this UML diagram. An example is provided
/// in video/unit_tests_vfm/JPEGEnc/TestJPEGEnc.cpp
///
/// @image html JPEGEnc_UML.png
///
/// Instantiation of the all the components on the SVA is performed in the following way:
/// @image html JPEGEnc_FW.png
///
///
/// \par Main differences with 8815 version of the JPEG Encoder
/// - Quantization Tables adjustment in the component, not in the application
/// - Huffman default tables in the component, not in the application
/// - no possibility to change the huffman tables (it was expected on 8815, it is not
///   a requirement on 8500)
/// - no more slice_mode. To be confirmed.
/// - no more segmented nor stream mode for buffers. To be confirmed
/// - no more ACE (Automatic Constrast Enhancement)
/// - limited set of parameters seens from the applications. This is the
///   general philosophy of all the OpenMax Components in order to keep
///   them as simple as possible
///
/// \par Non-conformances of this component with respect to the standard OpenMax component
/// - OMX_IndexParamQuantizationTable is not supported
/// - port indexes starts on 0, not on IPB+0
///
/// \par Functionalities of the Nomadik JPEG Encoder
/// Proprietary functionalities can be accessed through proprietary indexes
/// (cf. VFM_INDEXTYPE for all the VFM proprietary indexes, and JPEGEnc_Proxy_PARAM for the way to
/// set any parameters of the JPEG Encoder)
/// - OMX_IndexParamPortDefinition is mandatory to initialize the encoder
/// - OMX_IndexParamImagePortFormat is supported
/// - OMX_IndexParamQFactor can be used dynamically using SetConfig(), or statically using SetParameter()
/// - OMX_IndexConfigCommonRotate to set on-the-fly rotation. Rotation of 0, 90, -90 and 270 degrees are supported
/// - OMX_IndexConfigCommonOutputCrop to set cropping parameters (window size, and offsets)
/// - \ref VFM_IndexConfigTargetbpp to set the target bit-per-pixel to reach
///
/// \par Missing functionalities are
/// - thumbnail generation
///
/// \par To be defined by architecture team
/// - exif generation: defined the complete architecture, if it has to be done
///   in this OMX component or not
/// - can the application sets the restart interval. Right now, this is
///   fixed and sets to 8 rows




/// @ingroup JPEGEncoder
/// {
/// @brief The OpenMaxIL Proxy dedicated to the JPEG encoder.
/// Such a component can be instantiated using ENS_GetHandle()
/// with the component name "OMX.ST.VFM.JPEGEnc".
/// Refer to \ref JPEGEncoder for a complete description
///
/// This class is the base class of the JPEG Encoder OpenMax Component.
/// This class inherits from
/// - VFM_Component, that inherits from the ENS_Component, to inherits from
///   common proxy functionalities
/// - JPEGEnc_ParamAndConfig to store the current parameter and configuration of the component
/// - JPEGEnc_Processing to access processing function (quantization table scaling and header creation)
///
/// Moreover, it contains a link to a JPEGEnc_NMF object in order to instantiate / deinstantiate
/// NMF component
class JPEGEnc_Proxy: public VFM_Component
{
#ifdef HOST_MPC_COMPONENT_BOTH
    friend class JPEGEnc_NmfMpc_ProcessingComponent;
#endif
    friend class JPEGEnc_ArmNmfProcessingComponent;
    friend class JPEGEnc_Port;

    private:
#ifdef HOST_MPC_COMPONENT_BOTH
        /// @brief Object to allow automatic MPC NMF instantiation
        JPEGEnc_NmfMpc_ProcessingComponent *mpc_processingComponent;
#endif
        /// @brief Object to allow automatic ARM NMF instantiation
        JPEGEnc_ArmNmfProcessingComponent  *arm_processingComponent;



        /// @brief Is mpc param up-to-date or not on mpc side?
        VFM_Status mSendParamToMpc;
        VFM_Status mSendParamToARMNMF;

    public:
        /// @brief Parameters and Configuration Storing
        JPEGEnc_ParamAndConfig mParam;
    	/* all the methods */
        /// @brief Constructor, to initialize the configuration and the parameters.
        ///
        /// It mostly initialize the parameters, but
        /// you cannot afford setting the parameters of the ports
        /// The constructor is called when calling ENS_GetHandle()
        /// with the component name "OMX.ST.VFM.JPEGEnc"
        /// (check for function JPEGEncFactoryMethod() and VFM_Init())
        JPEGEnc_Proxy();
        OMX_ERRORTYPE JPEGEnc_Processing(OMX_U32);
        OMX_BOOL isMPCobject;
        OMX_BOOL isHVABased;

		OMX_BOOL is_full_powermgt;
		OMX_U32 sva_mcps;

        /// Destructor
        virtual ~ JPEGEnc_Proxy();

		virtual	OMX_ERRORTYPE construct();
		//virtual OMX_BOOL isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const;

		virtual VFM_Port *newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);

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
        virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

        /// @brief Get parameters from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are: \n
        /// - OMX_IndexParamPortDefinition is mandatory to initialize the encoder
        /// - OMX_IndexParamImagePortFormat
        /// - OMX_IndexParamQFactor
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
		virtual OMX_ERRORTYPE setConfigInternal(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

        /// @brief Get configuration from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are:
        /// - OMX_IndexParamQFactor
        /// - OMX_IndexConfigCommonRotate
        /// - OMX_IndexConfigCommonOutputCrop
        /// - \ref VFM_IndexConfigTargetbpp
        /// - All the ones supported by VFM_Component::getConfig()
        /// @param [out] pt structure that will contain the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

		virtual OMX_ERRORTYPE reset()
		{
			setFrameHeight(IPB+1, getFrameHeight(IPB+0));
			setFrameWidth(IPB+1, getFrameWidth(IPB+0));
			mParam.cropping_parameter.nLeft = 0;
			mParam.cropping_parameter.nTop = 0;

			#ifdef HOST_MPC_COMPONENT_BOTH
                        if (mpc_processingComponent)
                              mSendParamToMpc.set();
			#endif
                        if (arm_processingComponent)
                              mSendParamToARMNMF.set();
			return mParam.reset();
		}

		OMX_S32 getStride();
		OMX_U32 getSliceHeight();
		virtual void setProfileLevelSupported();
		virtual RM_STATUS_E	getResourcesEstimation(OMX_IN const OMX_PTR pCompHdl,OMX_INOUT RM_EMDATA_T* pEstimationData);
		OMX_ERRORTYPE detectPortSettingsAndNotify(OMX_U32 size);

		void jpegenc_proxy_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);

		virtual void NmfPanicCallback(void *contextHandler, t_nmf_service_type serviceType , t_nmf_service_data *serviceData);


};

/// @}

#endif // __JPEG_ENCODE_HPP

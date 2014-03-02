/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __MPEG4DEC_PROXY_H
#define __MPEG4DEC_PROXY_H

#include "MPEG4Dec_ParamAndConfig.h"
#include "MPEG4Dec_ProcessingComp.h"



/// @defgroup MPEG4Decoder MPEG4 Decoder Proxy
/// @brief The OpenMaxIL Proxy dedicated to the MPEG4 Decoder is
/// implemented in MPEG4Dec_Proxy, and is derived from
/// VFM_Component, that is derived from the ENS_Component. Such a component
/// can be instantiated using ENS_GetHandle() with the component name "OMX.ST.VFM.MPEG4Dec".
///
/// @par Development of the MPEG4 Decoder OpenMax Component
///
/// The MPEG4 Decoder OpenMax Component is defined in 4 classes:
/// - MPEG4Dec_Proxy class contains the proxy itself. It implements the
///   OpenMax API, that is SetConfig(),... This class is derived from
///   VFM_Component, that is itself derived from ENS_Component, in order
///   to benefit from commonalized functionalities between all or part of
///   the encoders / decoders
/// - MPEG4Dec_ParamAndConfig mainly contains the attributes to describes the
///   current parameters and configuration of the component. This class is
///   inherited by the proxy MPEG4Dec_Proxy, so that the proxy knows all about
///   these parameters. This class is derived from VFMEnc_ParamAndConfig
///   in order to benefit from common parameters and configuration shared
///   by other encoders.
/// - MPEG4Dec_NMF is the class that groups the instantiation / deinstantiation
///   support of the NMF component
/// - MPEG4Dec_Processing is the class that groups the processing of the MPEG4 Decoder
///   that is performed on the ARM side. We can find the quantization table scaling
///   and the MPEG4 Header creation in this class.
///
/// @par MPEG4 Decoder OpenMax Component Overview
/// @image html MPEG4Dec_OMX.png
///
/// The component is a standard OpenMax Component, based on video_decoder.MPEG4 standard component. It takes an bitstream in input
/// and returns the image YUV in output. It is conformed to the
/// MPEG4 Decoder as described on 8500 platform, with its limitation (no progressive encoding for example).
///
/// @note that unlike on the 8815 / 8820 architecture, the header of the MPEG4 coded stream, and the information parameters,
/// are computed directly by the OpenMax MPEG4 Decode, and no more by the application
///
/// The use of this OpenMaxIL Component is performed following this UML diagram. An example is provided
/// in video/unit_tests_vfm/MPEG4Dec/TestMPEG4Dec.cpp
///
/// @image html MPEG4Dec_UML.png
///
/// Instantiation of the all the components on the SVA is performed in the following way:
/// @image html MPEG4Dec_FW.png
///
///
/// \par Main differences with 8815 version of the MPEG4 Decoder
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
/// \par Functionalities of the Nomadik MPEG4 Decoder
/// Proprietary functionalities can be accessed through proprietary indexes
/// (cf. VFM_INDEXTYPE for all the VFM proprietary indexes, and MPEG4Dec_Proxy_PARAM for the way to
/// set any parameters of the MPEG4 Decoder)
/// - OMX_IndexParamPortDefinition is mandatory to initialize the decoder
/// - OMX_IndexParamVideoPortFormat is supported
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


/// @ingroup MPEG4Decoder
/// {
/// @brief The OpenMaxIL Proxy dedicated to the MPEG4 Decoder.
/// Such a component can be instantiated using ENS_GetHandle()
/// with the component name "OMX.ST.VFM.MPEG4Dec".
/// Refer to \ref MPEG4Decoder for a complete description
///
/// This class is the base class of the MPEG4 Decoder OpenMax Component.
/// This class inherits from
/// - VFM_Component, that inherits from the ENS_Component, to inherits from
///   common proxy functionalities
/// - MPEG4Dec_ParamAndConfig to store the current parameter and configuration of the component
/// - MPEG4Dec_Processing to access processing function (quantization table scaling and header creation)
///
/// Moreover, it contains a link to a MPEG4Dec_NMF object in order to instantiate / deinstantiate
/// NMF component

class MPEG4Dec_Proxy: public VFM_Component
{
    private:
        /// @brief Object to allow automatic NMF instantiation
//        MPEG4Dec_NMF nmf;
		MPEG4Dec_ProcessingComp processingComponent;

        /// @brief Parameters and Configuration Storing
//        MPEG4Dec_ParamAndConfig mParam;

    public:                        /* all the methods */
        MPEG4Dec_ParamAndConfig mParam;
		OMX_BOOL isMPCobject;
		OMX_BOOL is_full_powermgt;
		OMX_U32 sva_mcps;
		OMX_BOOL sorenson_flag;
		OMX_BOOL thumbnail;
		MPEG4Dec_Proxy();

        /// Destructor
        virtual ~ MPEG4Dec_Proxy();

		virtual OMX_ERRORTYPE construct();

		virtual VFM_Port *newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);

        /// @brief Set parameters to the component, from the application. It is part
        /// of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
        /// - OMX_IndexParamPortDefinition is mandatory to initialize the decoder
        /// - OMX_IndexParamVideoPortFormat
        /// - OMX_IndexParamQFactor
        /// - All the ones supported by VFM_Component::setParameter()
        /// @param [in] pt the structure that contains the parameters we want to set
        /// @return OMX_ErrorNone if no error, an error otherwise
        /// @todo Adopt a generic way to handle error. This is a general comment
        virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);
		virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);
        /// @brief Get parameters from the component, to the application. It is part of OpenMaxIL API
        /// @param [in] nParamIndex index of the structure we want to get. Supported indexes are: \n
        /// - OMX_IndexParamPortDefinition is mandatory to initialize the decoder
        /// - OMX_IndexParamVideoPortFormat
        /// - OMX_IndexParamQFactor
        /// - All the ones supported by VFM_Component::getParameter()
        /// @param [out] pt the structure that will contains the parameters we want to get
        /// @return OMX_ErrorNone if no error, an error otherwise
        virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;
		virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;
        virtual OMX_ERRORTYPE reset() { return mParam.reset(); };
		virtual RM_STATUS_E	getResourcesEstimation(OMX_IN const OMX_PTR pCompHdl,
                               OMX_INOUT RM_EMDATA_T* pEstimationData);
        //Method to replicate input port settings on output port
		//Output port is slave to input port
		OMX_ERRORTYPE updateOutputPortSettings(OMX_INDEXTYPE nParamIndex,t_uint32 width,t_uint32 height);

		void mpeg4dec_assert(int condition, int line, OMX_BOOL isFatal);
        virtual void setProfileLevelSupported();
        virtual void NmfPanicCallback(
              void *contextHandler           ,
              t_nmf_service_type serviceType ,
              t_nmf_service_data *serviceData);
};

#endif /* __MPEG4DEC_PROXY_H */

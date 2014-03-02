/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __JPEGDEC_PROXY_H
#define __JPEGDEC_PROXY_H

#include "VFM_Component.h"
#include "JPEGDec_ParamAndConfig.h"

#ifdef HOST_MPC_COMPONENT_BOTH
#include "JPEGDec_NmfMpc_ProcessingComponent.h"
#endif
#include "JPEGDEC_top.h"
#include "JPEGDec_ArmNmf_ProcessingComponent.h"

/// @defgroup JPEGDecoder JPEG Decoder Proxy
/// @brief The OpenMaxIL Proxy dedicated to the JPEG decoder is
/// implemented in JPEGDec_Proxy, and is derived from
/// VFM_Component, that is derived from the ENS_Component. Such a component
/// can be instantiated using ENS_GetHandle() with the component name "OMX.ST.VFM.JPEGDec".
///
/// @par Development of the JPEG Decoder OpenMax Component
///
/// The JPEG Decoder OpenMax Component is defined in 5 classes:
/// - JPEGDec_Proxy class contains the proxy itself. It implements the
///   OpenMax API, that is SetConfig(),... This class is derived from
///   VFM_Component, that is itself derived from ENS_Component, in order
///   to benefit from commonalized functionalities between all or part of
///   the encoders / decoders
/// - JPEGDec_ParamAndConfig mainly contains the attributes to describes the
///   current parameters and configuration of the component. This class is
///   instantiated by the proxy JPEGDec_Proxy, so that the proxy knows all about
///   these parameters. This class is derived from VFMDec_ParamAndConfig
///   in order to benefit from common parameters and configuration shared
///   by other decoders.
/// - JPEGDec_NMF is the class that groups the instantiation / deinstantiation
///   support of the NMF component
/// - JPEGDec_Processing is the class that groups the processing of the JPEG Decoder
///   that is performed on the ARM side.
/// - JPEGDec_Parser is a class which performs the parsing of the .jpg format to extract
///   required parameters,tables and encoded data for decoding. This class is instantiated
///	  in JPEGDec_Processing class.

///
/// @par JPEG Decoder OpenMax Component Overview
/// @image html JPEGDec_OMX.png
///
/// The component is a standard OpenMax Component, based on image_decoder.JPEG standard component. It takes an image in input
/// and returns the coded stream in output. It is conformed to the
/// JPEG Decoder as described on 8500 platform, with its limitation (no progressive encoding for example).
///
/// The use of this OpenMaxIL Component is performed following this UML diagram. An example is provided
/// in video/unit_tests_vfm/JPEGDec/TestJPEGDec.cpp
///
/// @image html JPEGDec_UML.png
///
/// Instantiation of the all the components on the SVA is performed in the following way:
/// @image html JPEGDec_FW.png
///
///
/// \par Main differences with 8815 version of the JPEG Decoder
/// - no more slice_mode. To be confirmed.
/// - no more segmented nor stream mode for buffers. To be confirmed
/// - no more ACE (Automatic Constrast Enhancement)
/// - no more progressive mode(only Baseline process is supported)
/// - limited set of parameters seen from the applications. This is the
///   general philosophy of all the OpenMax Components in order to keep
///   them as simple as possible
///
/// \par Non-conformances of this component with respect to the standard OpenMax component
/// - OMX_IndexParamQuantizationTable is not supported
/// - port indexes starts on 0, not on IPB+0
///
/// \par Functionalities of the Nomadik JPEG Deoder
/// - OMX_IndexParamPortDefinition is mandatory to initialize the encoder
/// - OMX_IndexParamImagePortFormat is supported
/// - OMX_IndexParamQFactor can be used dynamically using SetConfig(), or statically using SetParameter()
/// - OMX_IndexConfigCommonOutputCrop to set cropping parameters (window size, and offsets)
/// - \ref VFM_IndexConfigTargetbpp to set the target bit-per-pixel to reach
///
///
/// \par To be defined by architecture team
/// - exif generation: defined the complete architecture, if it has to be done
///   in this OMX component or not
/// - can the application sets the restart interval. Right now, this is
///   fixed and sets to 8 rows

/// @ingroup JPEGDecoder
/// {
/// @brief The OpenMaxIL Proxy dedicated to the JPEG decoder.
/// Such a component can be instantiated using ENS_GetHandle()
/// with the component name "OMX.ST.VFM.JPEGDec".
/// Refer to \ref JPEGDecoder for a complete description
///
/// This class is the base class of the JPEG Decoder OpenMax Component.
/// This class inherits from
/// - VFM_Component, that inherits from the ENS_Component, to inherits from
///   common proxy functionalities
/// - JPEGDec_ParamAndConfig to store the current parameter and configuration of the component
/// - JPEGDec_Processing to access processing function (quantization table scaling and header creation)
///
/// Moreover, it contains a link to a JPEGDec_NMF object in order to instantiate / deinstantiate
/// NMF component

class JPEGDec_Proxy: public VFM_Component
{
#ifdef HOST_MPC_COMPONENT_BOTH
    friend class JPEGDec_NmfMpc_ProcessingComponent;
#endif
    friend class JPEGDec_ArmNmf_ProcessingComponent;

private:

	/// @brief Parameters and Configuration Storing
	//JPEGDec_ProcessingComp processingComponent;
        /// @brief Object to allow automatic MPC NMF instantiation
#ifdef HOST_MPC_COMPONENT_BOTH
    JPEGDec_NmfMpc_ProcessingComponent *mpc_processingComponent;
#endif
        /// @brief Object to allow automatic ARM NMF instantiation
    JPEGDec_ArmNmf_ProcessingComponent *arm_processingComponent;

public:

   OMX_ERRORTYPE JPEGDec_Processing(OMX_U32);
   OMX_BOOL isMPCobject;
   OMX_BOOL isDualDecode;

	// TO remove friend classes. Anyways friend class was directly accessing  restricted private data
	JPEGDec_ParamAndConfig mParam;

	/// @brief Constructor, to initialize the configuration and the parameters.
	///
	/// It mostly initialize the parameters, but
	/// you cannot afford setting the parameters of the ports
	/// The constructor is called when calling ENS_GetHandle()
	/// with the component name "OMX.ST.VFM.JPEGDec"
	/// (check for function JPEGDecFactoryMethod() and VFM_Init())
    JPEGDec_Proxy();

	/// Destructor
	virtual ~ JPEGDec_Proxy();
    virtual OMX_ERRORTYPE construct();
   	virtual VFM_Port *newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);

	/// @brief Set parameters to the component, from the application. It is part
	/// of OpenMaxIL API
	/// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
	/// - OMX_IndexParamPortDefinition is mandatory to initialize the decoder
	/// - OMX_IndexParamImagePortFormat
	/// - All the ones supported by VFM_Component::setParameter()
	/// @param [in] pt the structure that contains the parameters we want to set
	/// @return OMX_ErrorNone if no error, an error otherwise
	/// @todo Adopt a generic way to handle error. This is a general comment
	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

	/// @brief Get parameters from the component, to the application. It is part of OpenMaxIL API
	/// @param [in] nParamIndex index of the structure we want to get. Supported indexes are: \n
	/// - OMX_IndexParamPortDefinition is mandatory to initialize the decoder
	/// - OMX_IndexParamImagePortFormat
	/// - All the ones supported by VFM_Component::getParameter()
	/// @param [out] pt the structure that will contains the parameters we want to get
	/// @return OMX_ErrorNone if no error, an error otherwise
	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

	/// @brief Set configuration to the component, from the application. It is part of OpenMaxIL API
	/// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
	/// - OMX_IndexConfigCommonScale can be used dynamically using SetConfig()
	/// - OMX_IndexConfigCommonOutputCrop
	/// - All the ones supported by VFM_Component::setConfig()
	/// @param [in] pt structure that contains the parameters we want to set
	/// @return OMX_ErrorNone if no error, an error otherwise
	virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

	/// @brief Get configuration from the component, to the application. It is part of OpenMaxIL API
	/// @param [in] nParamIndex index of the structure we want to get. Supported indexes are:
	/// - OMX_IndexParamQFactor
	/// - OMX_IndexConfigCommonScale
	/// - OMX_IndexConfigCommonOutputCrop
	/// - All the ones supported by VFM_Component::getConfig()
	/// @param [out] pt structure that will contain the parameters we want to get
	/// @return OMX_ErrorNone if no error, an error otherwise
	virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

	virtual OMX_ERRORTYPE reset();
	virtual OMX_U32 getUidTopDictionnary(void) { return KOstJPEGDEC_top_ID; };
	virtual void setProfileLevelSupported();
	OMX_ERRORTYPE updateOutputPortSettings(OMX_INDEXTYPE nParamIndex,const OMX_PARAM_PORTDEFINITIONTYPE *portDef);
	OMX_ERRORTYPE JPEG_Proxy_PortSettingsChangedNotify(OMX_PARAM_PORTDEFINITIONTYPE *portDef);
	OMX_ERRORTYPE detectPortSettingsAndNotify(OMX_PARAM_PORTDEFINITIONTYPE *portDef);
	void jpegdec_proxy_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);
	virtual void NmfPanicCallback(void *contextHandler, t_nmf_service_type serviceType , t_nmf_service_data *serviceData);

	public:
	virtual RM_STATUS_E	getResourcesEstimation(OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData);

};

/// @}*/

#endif // __JPEG_DECODE_HPP

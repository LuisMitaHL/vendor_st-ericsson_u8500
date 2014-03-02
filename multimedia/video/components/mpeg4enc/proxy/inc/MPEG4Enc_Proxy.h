/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __MPEG4ENC_PROXY_H
#define __MPEG4ENC_PROXY_H

#include "VFM_Component.h"
#include "MPEG4Enc_ParamAndConfig.h"
#ifdef HOST_MPC_COMPONENT_BOTH
#include "MPEG4Enc_NmfMpc_ProcessingComponent.h"
#endif
#include "MPEG4Enc_ArmNmf_ProcessingComponent.h"

#define OMX_IndexConfigVideoSequenceHeader OMX_IndexParamVideoSequenceHeader

/// @defgroup MPEG4Encoder MPEG4 Encoder Proxy
/// @brief The OpenMaxIL Proxy dedicated to the MPEG4 encoder is
/// implemented in MPEG4Enc_Proxy, and is derived from
/// VFM_Component, that is derived from the ENS_Component. Such a component
/// can be instantiated using ENS_GetHandle() with the component name "OMX.ST.VFM.MPEG4Enc".
///
/// @par Development of the MPEG4 Encoder OpenMax Component
///
/// The MPEG4 Encoder OpenMax Component is defined in 4 classes:
/// - MPEG4Enc_Proxy class contains the proxy itself. It implements the 
///   OpenMax API, that is SetConfig(),... This class is derived from
///   VFM_Component, that is itself derived from ENS_Component, in order
///   to benefit from commonalized functionalities between all or part of
///   the encoders / decoders
/// - MPEG4Enc_ParamAndConfig mainly contains the attributes to describes the
///   current parameters and configuration of the component. This class is
///   inherited by the proxy MPEG4Enc_Proxy, so that the proxy knows all about
///   these parameters. This class is derived from VFMEnc_ParamAndConfig
///   in order to benefit from common parameters and configuration shared
///   by other encoders.
/// - MPEG4Enc_NmfMpcProcessingComponent is the class that groups the instantiation / deinstantiation
///   support of the NMF component. It also performs the processing of the MPEG4 Encoder
///   that is performed on the ARM side. MPEG4 Header creation in this class.
/// - MPEG4Enc_Port is the class that groups all the port related functionality like setting the default
///   parameters and check over the parameters set by the user etc. This is derived from VFM_Port,
///   that is itself derived from ENS_Port.
///
/// @par MPEG4 Encoder OpenMax Component Overview
/// @image html MPEG4Enc_OMX.png
///
/// The component is a standard OpenMax Component, based on video_encoder.MPEG4 standard component. It takes an raw video in input
/// and returns the coded stream in output. It is conformed to the 
/// MPEG4 Encoder as described on 8500 platform, with its limitation (eg TBD).
///
/// @note that unlike on the 8815 / 8820 architecture, the header of the MPEG4 coded stream, and the information parameters,
/// are computed directly by the OpenMax MPEG4 Encode, and no more by the application
///
/// The use of this OpenMaxIL Component is performed following this UML diagram. An example is provided
/// in video/mpeg4enc//proxy/test_arm/TestMPEG4Enc.cpp
/// 
/// @image html MPEG4Enc_UML.png
///
/// Instantiation of the all the components on the SVA is performed in the following way:
/// @image html MPEG4Enc_FW.png
///
///
/// \par Main differences with 8815 version of the MPEG4 Encoder
///		- TBD
/// \par Non-conformances of this component with respect to the standard OpenMax component
///		- TBD
/// \par Functionalities of the Nomadik MPEG4 Encoder
/// Proprietary functionalities can be accessed through proprietary indexes
/// (cf. VFM_INDEXTYPE for all the VFM proprietary indexes, and MPEG4Enc_Proxy_PARAM for the way to
/// set any parameters of the MPEG4 Encoder)
/// - OMX_IndexParamPortDefinition is mandatory to initialize the encoder
/// - OMX_IndexParamVideoPortFormat is supported
///	- TBD
///
/// \par Missing functionalities are
///		- TBD
/// \par To be defined by architecture team
///		- TBD



/// @ingroup MPEG4Encoder
/// {
/// @brief The OpenMaxIL Proxy dedicated to the MPEG4 encoder.
/// Such a component can be instantiated using OMX_GetHandle()
/// with the component name "OMX.ST.VFM.MPEG4Enc".
/// Refer to \ref MPEG4Encoder for a complete description
///
/// This class is the base class of the MPEG4 Encoder OpenMax Component.
/// This class inherits from
/// - VFM_Component, that inherits from the ENS_Component, to inherits from
///   common proxy functionalities
///
/// Moreover, it contains a link to a MPEG4Enc_NmfProcessingComponent object in order to instantiate / deinstantiate
/// NMF component and pass parameter to nmf component.
class MPEG4Enc_Proxy: public VFM_Component
{
#ifdef HOST_MPC_COMPONENT_BOTH
	friend class MPEG4Enc_NmfMpc_ProcessingComponent;
#endif
	friend class MPEG4Enc_ArmNmf_ProcessingComponent;
	friend class MPEG4Enc_ProcessingComponent;
	friend class MPEG4Enc_Port;

private:
	/// @brief Parameters and Configuration Storing
	MPEG4Enc_ParamAndConfig mParam;

#ifdef HOST_MPC_COMPONENT_BOTH
	MPEG4Enc_NmfMpc_ProcessingComponent *mpcProcessingComponent;
#endif
	MPEG4Enc_ArmNmf_ProcessingComponent *armProcessingComponent;

	/// @brief Is mpc param up-to-date or not on mpc side?
	VFM_Status mSendParamToMpc;
	VFM_Status mSendParamToArmNmf;

public:						   /* all the methods */
	/// @brief Constructor, to initialize the configuration and the parameters.
	///
	/// It mostly initialize the parameters, but
	/// you cannot afford setting the parameters of the ports
	/// The constructor is called when calling ENS_GetHandle()
	/// with the component name "OMX.ST.VFM.MPEG4Enc"
	/// (check for function MPEG4EncFactoryMethod() and VFM_Init())
	MPEG4Enc_Proxy();
	OMX_ERRORTYPE MPEG4Enc_Processing(OMX_U32);
	OMX_BOOL isMpcObject;

	/// Destructor
	virtual ~ MPEG4Enc_Proxy();

	virtual OMX_ERRORTYPE construct();

	virtual VFM_Port *newPort(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);

	/// @brief Set parameters to the component, from the application. It is part
	/// of OpenMaxIL API
	/// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
	/// - OMX_IndexParamPortDefinition is mandatory to initialize the encoder
	/// - OMX_IndexParamVideoPortFormat
	/// - TBD
	/// - All the ones supported by VFM_Component::setParameter()
	/// @param [in] pt the structure that contains the parameters we want to set
	/// @return OMX_ErrorNone if no error, an error otherwise
	/// @todo Adopt a generic way to handle error. This is a general comment
	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

	/// @brief Get parameters from the component, to the application. It is part of OpenMaxIL API
	/// @param [in] nParamIndex index of the structure we want to get. Supported indexes are: \n
	/// - OMX_IndexParamPortDefinition is mandatory to initialize the encoder
	/// - OMX_IndexParamVideoPortFormat
	/// - TBD
	/// - All the ones supported by VFM_Component::getParameter()
	/// @param [out] pt the structure that will contains the parameters we want to get
	/// @return OMX_ErrorNone if no error, an error otherwise
	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

	/// @brief Set configuration to the component, from the application. It is part of OpenMaxIL API
	/// @param [in] nParamIndex index of the structure we want to set. Supported indexes are: \n
	/// - TBD
	/// - All the ones supported by VFM_Component::setConfig()
	/// @param [in] pt structure that contains the parameters we want to set
	/// @return OMX_ErrorNone if no error, an error otherwise
	virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

	/// @brief Get configuration from the component, to the application. It is part of OpenMaxIL API
	/// @param [in] nParamIndex index of the structure we want to get. Supported indexes are:
	/// - TBD
	/// - All the ones supported by VFM_Component::getConfig()
	/// @param [out] pt structure that will contain the parameters we want to get
	/// @return OMX_ErrorNone if no error, an error otherwise
	virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;

	virtual OMX_BOOL isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const;

	virtual RM_STATUS_E	getResourcesEstimation(OMX_IN const OMX_PTR pCompHdl, OMX_INOUT RM_EMDATA_T* pEstimationData);

	virtual OMX_ERRORTYPE reset() 
	{
	#if 0
		setFrameHeight(VPB+1, getFrameHeight(VPB+0)); 
		setFrameWidth(VPB+1, getFrameWidth(VPB+0)); 
	#endif

	#ifdef HOST_MPC_COMPONENT_BOTH
		if(mpcProcessingComponent)
			mSendParamToMpc.set();
	#endif
		if(armProcessingComponent)
			mSendParamToArmNmf.set();
		return mParam.reset(); 
	}

	virtual void setProfileLevelSupported();

	virtual void NmfPanicCallback(void *contextHandler,	t_nmf_service_type serviceType, t_nmf_service_data *serviceData);

};

/// @}

#endif // __MPEG4ENC_PROXY_H

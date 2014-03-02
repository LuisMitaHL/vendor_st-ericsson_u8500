/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __MPEG4ENC_PORT_H
#define __MPEG4ENC_PORT_H

#include "VFM_Port.h"

/// @ingroup MPEG4Encoder
/// {
/// @brief The Concrete Port dedicated to the MPEG4 encoder.
///
/// This class is the base class of the MPEG4 Encoder Port.
/// This class inherits from
/// - VFM_Port, that inherits from the ENS_Port, to inherit
///   common port functionalities

class MPEG4Enc_Port:public VFM_Port
{
public:
	///	This variable is kept to check for overscanned buffers.(i.e This is set to true if nstride = width and nsliceheight = height)
	OMX_BOOL bCheck;
	/// @brief Constructor, to initialize the configuration and the parameters.
	///
	/// It mostly initialize the parameters, but
	/// nothing is done currently
	MPEG4Enc_Port(const EnsCommonPortData& commonPortData, ENS_Component &enscomp);

	/// @brief Set a structure of type OMX_VIDEO_PARAM_PORTFORMATTYPE. It makes use
	/// of the mComponent attribute.
	/// @param [in] pt_org structure of type OMX_VIDEO_PARAM_PORTFORMATTYPE, that contains the
	/// information to set
	/// @param [out] has_changed true if the parameters have been modified by
	/// this operation, false otherwise
	/// @return OMX_ErrorNone if no error occured, an error otherwise (OMX_ErrorBadParameter)
	OMX_ERRORTYPE setIndexParamVideoPortFormat(OMX_PTR pt_org, OMX_BOOL *has_changed);

	/// @brief Get a structure of type OMX_VIDEO_PARAM_PORTFORMATTYPE. It makes use
	/// of the mComponent attribute.
	/// @param [in,out] pt_org structure of type OMX_VIDEO_PARAM_PORTFORMATTYPE,
	/// that will contain the information to get
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	OMX_ERRORTYPE getIndexParamVideoPortFormat(OMX_PTR pt_org) const;


	/// @brief Get the size of the slaved buffer (the output) given the size of the master one.
	/// This may depend on cropping and downsizing
	/// @param[in] nWidthMaster The width of the master port
	/// @param[in] nHeightMaster The height of the master port
	/// @param[out] nWidthSlave The resulting width of the slave port
	/// @param[out] nHeightSlave The resulting height of the slave port
	virtual void getSlavedSizeFromGivenMasterSize(OMX_U32 nWidthMaster, OMX_U32 nHeightMaster, OMX_U32 *nWidthSlave, OMX_U32 *nHeightSlave) const;

	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt) const;
	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pt);

	/// @brief SetDefault, to initialize the port parameters.
	///
	/// It mostly initialize the parameters, when the ports
	/// are constructed in the proxy construct function
	void setDefault();

protected:

	/// @brief Get the minimum buffer size requirement of the port, This is information is populated in Port
	/// nBufferSize field, which is in turn used by the user while allocating buffers for ports
	/// @return The buffersize computed
	virtual OMX_U32 getBufferSize() const;

	/// @brief Check the port parameters set by the user. Currently the range of resolution and Multiplicity of
	/// resolution with 16 are checked
	/// @return OMX_ErrorNone if no error, an error otherwise
	virtual OMX_ERRORTYPE checkSetFormatInPortDefinition(const OMX_PARAM_PORTDEFINITIONTYPE *portDef);

private:
	
	MPEG4Enc_Proxy *pProxyComponent;
};

/// @}

#endif // __MPEG4ENC_PORT_H


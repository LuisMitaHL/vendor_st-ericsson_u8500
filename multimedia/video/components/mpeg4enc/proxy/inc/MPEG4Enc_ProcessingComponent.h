/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __MPEG4ENC_Processing_H
#define __MPEG4ENC_Processing_H

#include "vfm_vec_mpeg4.idt"
//#include "host/mpeg4enc/mpc/api/ddep.hpp"
#include "mpeg4enc/arm_nmf/mpeg4enc_sw.hpp"

typedef enum
{
	SVA_BUFFERING_NONE,
	SVA_BUFFERING_VBV,
	SVA_BUFFERING_HRD,
	SVA_BUFFERING_ANNEXG
} t_sva_brc_buffering_model;

// forward definition
class MPEG4Enc_Proxy;

/// @ingroup MPEG4Encoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the MPEG4 Encoder, and instantiation / deinstantiation support
/// for these NMF components. It also groups the processing to be performed before
/// parameters are passed to MPC
/// @note this class derives from VFM_NmfProcessingComponent
class MPEG4Enc_ProcessingComponent
{
	friend class MPEG4Enc_ArmNmf_ProcessingComponent;
	friend class MPEG4Enc_NmfMpc_ProcessingComponent;
	friend class MPEG4Enc_Proxy;

public:
	MPEG4Enc_ProcessingComponent();

private:

	t_sva_brc_buffering_model mBufferingModel;

	t_uint32 mSwisBufSize;
	t_uint32 mVbvBufferSize;
	t_uint32 mVbvOccupancy;

	t_uint32 Write_VOS_VO_VOL(MPEG4Enc_Proxy *mpeg4enc, t_uint8* addr_pointer);

	/// @brief Prepare data to configure the algorithm
	///
	/// @brief Prepare parameters to be sent, through configureAlgo, to the firmware
	/// @param [in,out] mpeg4enc OpenMax Component related with this processing
	/// @param [out] pMecIn_parameters structure to be filled
	/// @todo clean this function with clean data structure
	void set_pMecIn_parameters(MPEG4Enc_Proxy *pProxyComponent, ts_t1xhv_vec_mpeg4_param_in *pMecIn_parameters, ts_ddep_vec_mpeg4_custom_param_in *pMecIn_custom_parameters);

};

#endif

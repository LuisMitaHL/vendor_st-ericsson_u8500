/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFMNmfMpc_Encoder.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "cm/inc/cm_macros.h"
#include "AFMNmfMpc_Encoder.h"
#include "AFM_Encoder.h"
#include "ENS_Nmf.h"

AFM_API_EXPORT OMX_ERRORTYPE
AFMNmfMpc_Encoder::construct(void)
{
    OMX_ERRORTYPE error;

    //setPriorityLevel((OMX_U32)NMF_SCHED_BACKGROUND);
    (static_cast<AFM_Port *>(mENSComponent.getPort(0)))->setPortPriorityLevel(OMX_PriorityBackground);
    (static_cast<AFM_Port *>(mENSComponent.getPort(1)))->setPortPriorityLevel(OMX_PriorityBackground);
     
    error = AFMNmfMpc_ProcessingComp::construct();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfMpc_Encoder::instantiateLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::instantiateNMFComponent(
            mENSComponent.getNMFDomainHandle(), "audiolibs.libbitstream",
            "libbitstream", &mNmfBitstreamLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, "libbitstream", mNmfBitstreamLib, "libbitstream");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, "genericfsm", mNmfGenericFsmLib, "genericfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, "componentfsm", mNmfComponentFsmLib, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentAsynchronous(
            mNmfMain, "me", mNmfMain, "postevent", 2);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfMpc_Encoder::deInstantiateLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponent(
            mNmfMain, "componentfsm");
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponent(mNmfMain, "genericfsm");
    if (error != OMX_ErrorNone ) return error;

    error = ENS::unbindComponent(mNmfMain, "libbitstream");
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfBitstreamLib);
    if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentAsynchronous(mNmfMain, "me");
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfMpc_Encoder::startLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfBitstreamLib);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

OMX_ERRORTYPE 
AFMNmfMpc_Encoder::stopLibraries(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfBitstreamLib);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfMpc_Encoder::instantiateMain(void) {
    OMX_ERRORTYPE error;

    MEMORY_TRACE_ENTER2("AFMNmfMpc_Encoder::instantiateAlgo (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    error = instantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    MEMORY_TRACE_LEAVE2("AFMNmfMpc_Encoder::instantiateAlgo (%s) @0x%08X", (const char*)mENSComponent.getName(), (unsigned int)&mENSComponent);

    error = ENS::instantiateNMFComponent(
            mENSComponent.getNMFDomainHandle(), "encoders.streamed.wrapper",
            "wrapper", &mNmfMain, NMF_SCHED_BACKGROUND);
    if (error != OMX_ErrorNone) return error;

    error = instantiateLibraries();
    if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponentFromHostEx(mNmfMain, "configure",
			&mIconfigure, 1);
	if (error != OMX_ErrorNone) return error;

    error = ENS::bindComponent(
            mNmfMain, "encoder", mNmfil, "encoder");
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}

AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfMpc_Encoder::deInstantiateMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::unbindComponent(mNmfMain, "encoder");
    if (error != OMX_ErrorNone) return error;

    error = ENS::freeMpcMemory(mMemory_buffer_in);
	if (error != OMX_ErrorNone) return error;

	error = ENS::freeMpcMemory(mMemory_buffer_out);
	if (error != OMX_ErrorNone) return error;

    error = ENS::unbindComponentFromHost(&mIconfigure);
	if (error != OMX_ErrorNone) return error;

    error = deInstantiateLibraries();
    if (error != OMX_ErrorNone) return error;

    error = deInstantiateAlgo();
    if (error != OMX_ErrorNone) return error;

    error = ENS::destroyNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfMpc_Encoder::startMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::startNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    error = startLibraries();
    if (error != OMX_ErrorNone) return error;

    error = startAlgo();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfMpc_Encoder::stopMain(void) {
    OMX_ERRORTYPE error;

    error = ENS::stopNMFComponent(mNmfMain);
    if (error != OMX_ErrorNone) return error;

    error = stopLibraries();
    if (error != OMX_ErrorNone) return error;

    error = stopAlgo();
    if (error != OMX_ErrorNone) return error;

    return OMX_ErrorNone;
}


AFM_API_EXPORT OMX_ERRORTYPE 
AFMNmfMpc_Encoder::configureMain(void) {
    OMX_ERRORTYPE error;
	int max_bitstream_frame_size_in_word;
	int max_frame_size_in_word;
    AFM_Encoder *afmenc = (AFM_Encoder *)&mENSComponent;

	t_uint32 dsp_buffer_in_address, dsp_buffer_out_adress;

	max_bitstream_frame_size_in_word = afmenc->getMaxFrameSize();
	max_bitstream_frame_size_in_word = max_bitstream_frame_size_in_word/ 16 + (max_bitstream_frame_size_in_word % 16? 1:0) + 1;
	max_frame_size_in_word = afmenc->getSampleFrameSize() * afmenc->getMaxChannels();

	error = ENS::allocMpcMemory(
                        mENSComponent.getNMFDomainHandle(), CM_MM_MPC_SDRAM24, 
                        max_frame_size_in_word, CM_MM_ALIGN_WORD, &mMemory_buffer_in);
	if (error != OMX_ErrorNone) return error;

	error = ENS::allocMpcMemory(
                        mENSComponent.getNMFDomainHandle(), CM_MM_MPC_SDRAM24, 
                        max_bitstream_frame_size_in_word, CM_MM_ALIGN_2WORDS, &mMemory_buffer_out);
	if (error != OMX_ErrorNone) return error;

	CM_GetMpcMemoryMpcAddress(mMemory_buffer_in, &dsp_buffer_in_address);
	CM_GetMpcMemoryMpcAddress(mMemory_buffer_out, &dsp_buffer_out_adress);

	error = configureAlgo();
	if (error != OMX_ErrorNone) return error;

	mIconfigure.setParameter((void*)dsp_buffer_in_address, (t_uint16)max_frame_size_in_word, (void*)dsp_buffer_out_adress, (t_uint16)max_bitstream_frame_size_in_word);

	return OMX_ErrorNone;
}


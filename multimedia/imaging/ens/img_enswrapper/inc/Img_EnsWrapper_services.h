/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#ifndef _IMG_ENSWRAPPER_SERVICES_H_
#define _IMG_ENSWRAPPER_SERVICES_H_

int computeASIC_ID();

#ifdef ANDROID
	#define OMX_TRACE_LEVEL_PROPERTY_NAME         "ste.omx.wrapper.trace"
	#define DEBUG_TRACE_ENABLED_PROPERTY_NAME     "ste.omx.wrapper.debug"
	#define RME_TRACE_ENABLED_PROPERTY_NAME       "ste.omx.wrapper.rme"
	#define NMF_TRACE_LEVEL_PROPERTY_NAME         "ste.omx.wrapper.NMF.CM"
	#define NMF_STM_ARM_TRACE_LEVEL_PROPERTY_NAME "ste.omx.nmf.arm.trace"
	#define NMF_STM_SIA_TRACE_LEVEL_PROPERTY_NAME "ste.omx.nmf.sia.trace"
	#define NMF_STM_SVA_TRACE_LEVEL_PROPERTY_NAME "ste.omx.nmf.sva.trace"
	#define STE_OMX_TRACE_PROPERTY_NAME           "ste.omx.trace"
	#define OMX_DUMP_LEVEL_PROPERTY_NAME          "ste.omx.dump.level"
	#define OMX_DUMP_OUTPUT_PROPERTY_NAME         "ste.omx.dump.output"
	#define OMX_DUMP_NAME_PROPERTY_NAME           "ste.omx.dump.name"
	#define LIBC_DEBUG_MALLOC_PROPERTY_NAME       "libc.debug.malloc"
	#define STE_NMF_AUTOIDLE_PROPERTY_NAME        "ste.nmf.autoidle"
#else
// The shell identifier compliant properties names
	#define OMX_TRACE_LEVEL_PROPERTY_NAME         "ste_omx_wrapper_trace"
	#define DEBUG_TRACE_ENABLED_PROPERTY_NAME     "ste_omx_wrapper_debug"
	#define RME_TRACE_ENABLED_PROPERTY_NAME       "ste_omx_wrapper_rme"
	#define NMF_TRACE_LEVEL_PROPERTY_NAME         "ste_omx_wrapper_NMF_CM"
	#define NMF_STM_ARM_TRACE_LEVEL_PROPERTY_NAME "ste_omx_nmf_arm_trace"
	#define NMF_STM_SIA_TRACE_LEVEL_PROPERTY_NAME "ste_omx_nmf_sia_trace"
	#define NMF_STM_SVA_TRACE_LEVEL_PROPERTY_NAME "ste_omx_nmf_sva_trace"
	#define STE_OMX_TRACE_PROPERTY_NAME           "ste_omx_trace"
	#define OMX_DUMP_LEVEL_PROPERTY_NAME          "ste_omx_dump_level"
	#define OMX_DUMP_OUTPUT_PROPERTY_NAME         "ste_omx_dump_output"
	#define OMX_DUMP_NAME_PROPERTY_NAME           "ste_omx_dump_name"
	#define LIBC_DEBUG_MALLOC_PROPERTY_NAME       "libc_debug_malloc"
	#define STE_NMF_AUTOIDLE_PROPERTY_NAME        "ste_nmf_autoidle"
#endif

char * strOMX_Error(OMX_ERRORTYPE error, char * buffer);

//void sprintCBMessage             (char * buffer, CB_Message * message);
void strcatOMX_STATETYPE         (char * buffer, OMX_STATETYPE state);
void strcatOMX_TUNNELSETUPTYPE   (char * buffer, OMX_TUNNELSETUPTYPE* pTunnelSetup);
void strcatOMX_BUFFERSUPPLIERTYPE(char * buffer, OMX_BUFFERSUPPLIERTYPE eBufferSupplier);
void dumpOMX_BUFFERHEADERTYPE    (OMX_BUFFERHEADERTYPE* pBuffer, char * buffer);
void dumpOMX_BUFFERDATA          (OMX_BUFFERHEADERTYPE* pBuffer, char * header_buffer);
char * strOMX_Error              (OMX_ERRORTYPE error, char * buffer);
void displaySendCommand          (const char*, OMX_HANDLETYPE hComponent, OMX_COMMANDTYPE Cmd, OMX_U32 nParam, OMX_PTR pCmdData);

#endif // #ifndef _IMG_ENSWRAPPER_SERVICES_H_

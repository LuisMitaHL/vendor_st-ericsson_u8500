/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __MPEG2DEC_PROCESSINGCOMP_H
#define __MPEG2DEC_PROCESSINGCOMP_H

#include "VFM_NmfHost_ProcessingComponent.h"
#include "mpeg2dec/arm_nmf/mpeg2deccomparm.hpp"
#include "vfm_vdc_mpeg2.idt"

class MPEG2Dec_Proxy;
class MPEG2Dec_ProcessingComp: public VFM_NmfHost_ProcessingComponent,
							  public mpeg2dec_arm_nmf_api_portSettingsDescriptor
{
        virtual OMX_ERRORTYPE construct(void);
        virtual OMX_ERRORTYPE destroy(void);

        virtual	OMX_ERRORTYPE applyConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure);

        virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData);

        virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

        virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

        virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);

        virtual OMX_ERRORTYPE allocateBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_U32 nSizeBytes,
                OMX_U8 **ppData,
                void **bufferAllocInfo,
                void **portPrivateInfo);

        virtual OMX_ERRORTYPE useBufferHeader(
                OMX_DIRTYPE dir,
                OMX_BUFFERHEADERTYPE* pBuffer);

        virtual OMX_ERRORTYPE useBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo);

        virtual OMX_ERRORTYPE freeBuffer(
				OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BOOL bBufferAllocated,
                void *bufferAllocInfo,
                void *portPrivateInfo);
        void mpeg2dec_processingcomp_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);
	private:
			OMX_BOOL memory_once_allocated;
			OMX_ERRORTYPE allocateRequiredMemory();
	public:
			OMX_BOOL firstFrame;
			MPEG2Dec_ProcessingComp(ENS_Component &enscomp);
			virtual ~MPEG2Dec_ProcessingComp();
			virtual void portSettings(t_uint32 width, t_uint32 height);
            void set_perf_parameters();
			void GetIndex_macroblock_error_map(OMX_PTR pt) const;
			void SetIndex_macroblock_error_reporting(OMX_BOOL flag){
				isErrorMapReportingEnable = flag;
			}

			OMX_BOOL GetIndex_macroblock_error_reporting() const{ 
				return isErrorMapReportingEnable; 
			}

			OMX_ERRORTYPE errorRecoveryDestroyAll(void);
			OMX_BOOL isNMFPanicSource(t_panic_source ee_type, OMX_U32 faultingComponent);

	protected:

	/// @brief Register the stubs and skels from ARM to DSP for the internal interfaces of the
	/// NMF Composite that is instantiated. This may be empty for full-sw implementation
	virtual void registerStubsAndSkels(void);

	/// @brief Unregister the stubs and skels from ARM to DSP for the internal interfaces of the
	/// NMF Composite that is instantiated. This may be empty for full-sw implementation
	 virtual void unregisterStubsAndSkels(void);

	/// @brief Create the codec and initialize the member mCodec
	virtual OMX_ERRORTYPE codecCreate(OMX_U32 domainId);

	/// @brief Destroy the codec and initialize the member mCodec
	virtual void codecDestroy(void);

	/// @brief Instantiate the specific interfaces between the proxy and the ddep.
	/// It includes the interface emptyThisBuffer /  FillThisBuffer, as well as specific
	/// interfaces.
	virtual OMX_ERRORTYPE codecInstantiate(void);

	/// @brief Start the NMF component. This may be empty in the specific implementation,
	/// apart if any specific actions has to be taken.
	virtual OMX_ERRORTYPE codecStart(void);

	/// @brief Stop the NMF component. This may be empty in the specific implementation,
	/// apart if any specific actions has to be taken.
	virtual OMX_ERRORTYPE codecStop(void);

	/// @brief Configure the NMF component. This may be empty in the specific implementation,
	/// apart if any specific actions has to be taken.
	virtual OMX_ERRORTYPE codecConfigure(void);

	/// @brief Deinstantiate the specific interfaces between the proxy and the ddep.
	/// It includes the interface emptyThisBuffer /  FillThisBuffer, as well as specific
	/// interfaces.
	virtual OMX_ERRORTYPE codecDeInstantiate(void);

	/// @todo to document
	virtual void sendConfigToCodec();

	virtual OMX_ERRORTYPE doSpecificEventHandle_cb(OMX_EVENTTYPE event, OMX_U32 nData1, 
												   OMX_U32 nData2);

private:
	MPEG2Dec_Proxy *pProxyComponent;
	//OMX_U8 memoryAllocated;
    ts_ddep_buffer_descriptor  *pParamBufferDesc;
	Impeg2dec_arm_nmf_api_setConfig iSetConfig;
	Impeg2dec_arm_nmf_api_sendportSettings iPortSettingsInfoSignal;
	
	ts_ddep_buffer_descriptor		mDebugBufferDesc;

	/*! Buffer descriptor for Param buffer
	*/
	ts_ddep_buffer_descriptor		mParamBufferDesc;

	/*! Buffer descriptor for Param buffer
	*/
	ts_ddep_buffer_descriptor		mLinkListBufferDesc;

	//VFM_Memory list
	void* vfm_mem_ctxt;

	OMX_BOOL fullDVFSSet;

	// Error Map structure
	t_uint16   cumulativeErrorMap[225];
	OMX_BOOL isErrorMapReportingEnable;
};

#endif //MPEG2DEC_PROCESSINGCOMP_H


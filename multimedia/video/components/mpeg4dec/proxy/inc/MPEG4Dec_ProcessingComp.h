/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef __MPEG4DEC_PROCESSINGCOMP_H
#define __MPEG4DEC_PROCESSINGCOMP_H

#include "VFM_Component.h"
#include "VFM_NmfHost_ProcessingComponent.h"
#ifndef   __MPEG4DEC_SOFT_DECODER
#include "mpeg4dec/arm_nmf/mpeg4deccomparm.hpp"
#endif
#include "mpeg4dec/arm_nmf/mpeg4dec_sw.hpp"
#include "vfm_vdc_mpeg4.idt"
#define KExtendedPar			0xF //extended PAR
typedef struct {

    OMX_U32 mpcAddress;
	OMX_BOOL isFree;
    OMX_BOOL isRequestedToFree;
    ts_ddep_vdc_mpeg4_header_desc *pHeaderDesc;
} ts_vfm_vdc_mpeg4_header_desc;

class MPEG4Dec_Proxy;
class MPEG4Dec_ProcessingComp: public VFM_NmfHost_ProcessingComponent,
							  public mpeg4dec_arm_nmf_api_portSettingsDescriptor
{
        virtual OMX_ERRORTYPE construct(void);
        virtual OMX_ERRORTYPE destroy(void);

        virtual	OMX_ERRORTYPE applyConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure) { return OMX_ErrorNone; };

        virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData);

        virtual OMX_ERRORTYPE emptyThisBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer);

        virtual OMX_ERRORTYPE fillThisBuffer(
                OMX_BUFFERHEADERTYPE* pBuffer);

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

	private:
			OMX_BOOL memory_once_allocated;
			OMX_ERRORTYPE allocateRequiredMemory();
	public:
			OMX_BOOL firstFrame;
			//+ER345950
			OMX_BOOL allocate_internal_memory;
			//-ER345950
            MPEG4Dec_ProcessingComp(ENS_Component &enscomp);
			virtual ~MPEG4Dec_ProcessingComp();
			virtual void portSettings(t_uint32 width, t_uint32 height,
									  t_uint8 aspect_ratio,t_uint8 nHorizontal,
									  t_uint8 nVertical,t_uint8 color_primary);
            void set_perf_parameters();
			void SetDeblockingFlag(OMX_BOOL flag){ deblocking_enable=flag; };
			void SetIndex_macroblock_error_reporting(OMX_BOOL flag);
			OMX_BOOL GetIndex_macroblock_error_reporting() const;
			OMX_BOOL GetDeblockingFlag() const;
			//+ER345950
			OMX_ERRORTYPE destroy_internal();
			OMX_ERRORTYPE codecConfigure_internal();
			//-ER345950
			void GetIndex_macroblock_error_map(OMX_PTR pt)const;
			OMX_ERRORTYPE getIndexParamColorPrimary(OMX_PTR pt) const;
			OMX_ERRORTYPE getIndexParamPixelAspectRatio(OMX_PTR pt) const;
			OMX_ERRORTYPE setIndexParamPixelAspectRatio(OMX_PTR pt);
			OMX_ERRORTYPE setIndexParamColorPrimary(OMX_PTR pt);
			void SetIndexParamVideoFastUpdate(OMX_BOOL flag);
			void GetIndexParamVideoFastUpdate(OMX_PTR pt) const;

            OMX_ERRORTYPE setIndexParamResourceSvaMcps(OMX_PTR pMcpsType);

			OMX_ERRORTYPE getIndexParamResourceSvaMcps(OMX_PTR pMcpsType) const;

			OMX_ERRORTYPE setIndexParamthumbnailgeneration(OMX_PTR pt);
			OMX_U32 resourceSvaMcps() const;

			void resourceSvaMcps(const OMX_U32& mcps) ;

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
	virtual OMX_ERRORTYPE codecCreate(OMX_U32);

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

	virtual void resetDecodeImmediateRelease();
	virtual OMX_ERRORTYPE doSpecificEventHandle_cb(OMX_EVENTTYPE event, OMX_U32 nData1,
												   OMX_U32 nData2);

private:
	OMX_U32   m_nSvaMcps;
	MPEG4Dec_Proxy *pProxyComponent;
    Impeg4dec_arm_nmf_api_memoryRequirements iProvideMemory;
	Impeg4dec_arm_nmf_api_sendportSettings iPortSettingsInfoSignal;

	OMX_BOOL deblocking_enable;
	t_uint16 error_reporting_enable;
	ts_ddep_buffer_descriptor		mDebugBufferDesc;

	/*! Memory Handle for Debug buffer
	*/
	//t_cm_memory_handle				mDebugBufferHandle;

	/*! Buffer descriptor for Param buffer
	*/
	ts_ddep_buffer_descriptor		mParamBufferDesc;

	/*! Memory Handle for Param buffer
	*/
	//t_cm_memory_handle				mParamBufferHandle;

	/*! Buffer descriptor for Param buffer
	*/
	ts_ddep_buffer_descriptor		mLinkListBufferDesc;

	/*! Memory Handle for Param buffer
	*/
	//t_cm_memory_handle				mLinkListParamBufferHandle;

	t_uint32    							nLogicalAddress[4];

	t_uint32								deblock_param_address[4];

	t_uint32								references_bufaddress[5];

	t_uint16   								error_map_copy[228];

	t_uint32 								buffer_i_frame_logical;

    OMX_BOOL fullDVFSSet;
    OMX_BOOL fullDDRSet; //for ER 345421

	void* vfm_mem_ctxt;

	t_uint8 AspectRatio;

	t_uint8 nHorizontal;

	t_uint8 nVertical;

	t_uint8 ColorPrimary;


};
inline OMX_U32 MPEG4Dec_ProcessingComp::resourceSvaMcps() const
{ return m_nSvaMcps; }

inline void MPEG4Dec_ProcessingComp::resourceSvaMcps(const OMX_U32& mcps)
{ m_nSvaMcps = mcps; }
#endif //MPEG4DEC_PROCESSINGCOMP_H


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

#ifndef __JPEGEnc_ArmNmfProcessingComponent_H
#define __JPEGEnc_ArmNmfProcessingComponent_H


#include "VFM_Component.h"
#include "VFM_Port.h"
#include "VFM_NmfHost_ProcessingComponent.h"
#include "JPEGEnc_ProcessingComponent.h"
#include <cm/inc/cm_macros.h>
//#include "jpegenc/arm_nmf/jpegenc_sw.hpp"

//#include "JPEGEnc_Proxy.h"
#include "t1xhv_vec_jpeg.idt"
#ifdef HVA_JPEGENC
#include "t1xhv_vec_jpeg_5500.idt"
#endif
#include "vfm_vec_jpeg.idt"

class JPEGEnc_Proxy;
/// @ingroup JPEGEncoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the JPEG Encoder, and instantiation / deinstantiation support
/// for these NMF components.
/// @note this class derives from VFM_NMF
class JPEGEnc_ArmNmfProcessingComponent : public VFM_NmfHost_ProcessingComponent, public jpegenc_arm_nmf_api_portSettingsDescriptor, public jpegenc_arm_nmf_api_cmd_ackDescriptor
{
	friend class JPEGEnc_Proxy;
	friend class JPEGEnc_ProcessingComponent;
	private:
		JPEGEnc_ProcessingComponent processingComp;

		OMX_HANDLETYPE OMXHandle;

		//JPEGEnc_ArmNmfProcessingComponentCB  jpegEncCallbackHandler ;

		/// @brief Buffer of the header
        //t_cm_memory_handle 								mHeaderBufferHandle;

        /// @brief Buffer for the Parameters
        //t_cm_memory_handle 								mParamBufferHandle;

        /// @brief Buffer description for Param buffer
		ts_ddep_buffer_descriptor						mParamBufferDesc;

#ifdef HVA_JPEGENC
        /// @brief Buffer description for HVA based task buffer descriptor
		ts_ddep_buffer_descriptor						mHVATaskDecs;

        /// @brief Buffer description for HVA based input params
		ts_ddep_buffer_descriptor						mHVAInParamsDecs;

        /// @brief Buffer description for HVA based input params
		ts_ddep_buffer_descriptor						mHVAOutParamsDecs;

        /// @brief Buffer description for HVA based input params
		ts_ddep_buffer_descriptor						mHVAQuantParamsDecs;
#endif

        /// @brief Buffer description for Header buffer
		ts_ddep_buffer_descriptor						mHeaderBufferDesc;

		/// @brief Buffer of the header
        //t_cm_memory_handle 								mEXIFHeaderBufferHandle;

        /// @brief Buffer description for Header buffer
		ts_ddep_buffer_descriptor						mEXIFHeaderBufferDesc;

		void* arm_vfm_mem_ctxt;

		OMX_U32 nNumber_arm;
        JPEGEnc_Proxy *pProxyComponent;

        /// @brief Creation of the header buffer
        ///
        /// It updates an internal buffer, and is mostly based on the quantization table and on the huffman tables
        /// @param [in,out] jpegenc OpenMax Component related with this processing
        /// @note This task was performed on the application side on the 8815 version
        void JPEGheaderCreation(JPEGEnc_Proxy *jpegenc);

        /// @brief Prepare data to configure the algorithm
        OMX_ERRORTYPE configureAlgo();

        virtual OMX_ERRORTYPE configure();

		void set_pJecOther_parameters(JPEGEnc_Proxy *jpegenc, ts_ddep_sec_jpeg_param_desc_ *ps_ddep_sec_jpeg_param_desc, t_uint32 header_size_in_bytes, t_uint8 *headerBufferPhysicalAddress, t_uint8* thumbnailImageBufferPhysicalAddress, t_uint8* runLevelBufferPhysicalAddress);

    public:
    	JPEGEnc_ArmNmfProcessingComponent(ENS_Component &enscomp);

        virtual ~JPEGEnc_ArmNmfProcessingComponent();

		OMX_ERRORTYPE jpegEncCreatePortInterface(VFM_Port *port, const char *nameInput, const char *nameCB);

		Ijpegenc_arm_nmf_api_setParam	   setparamitf;
		//+ER 354962
		OMX_U32 mNbPendingCommands;
		//-ER 354962
#ifdef HVA_JPEGENC
		Ijpegenc_arm_nmf_api_setMemoryParam setMemoryitf;
#endif


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

		virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex,OMX_U32 nBufferIndex,OMX_U32 nSizeBytes,OMX_U8 **ppData,void **bufferAllocInfo,void **portPrivateInfo);

		/// @todo to document
		virtual void sendConfigToCodec(){};

		virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

		virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

		virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler);
	    virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
			//	{bDeferredCmd=OMX_FALSE; return OMX_ErrorNone;};

		virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
        virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);

        void jpegenc_armnmf_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);

        virtual void portSettings(OMX_U32 size);
		//+ER 354962
		virtual void pendingCommandAck();
		void addPendingCommand();
		//-ER 354962
        OMX_ERRORTYPE errorRecoveryDestroyAll(void);

			 virtual OMX_ERRORTYPE applyConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure);

};

#endif

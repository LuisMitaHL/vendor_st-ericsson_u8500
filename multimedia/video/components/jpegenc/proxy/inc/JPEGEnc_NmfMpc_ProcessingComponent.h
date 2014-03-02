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

#ifndef __JPEGENC_NMF_H
#define __JPEGENC_NMF_H

#include "VFM_NmfMpc_ProcessingComponent.h"
//#include "JPEGEnc_Proxy.h"
#include "t1xhv_vec_jpeg.idt"
#include "vfm_vec_jpeg.idt"
// includes given by the FW
#include "host/jpegenc/mpc/api/ddep.hpp"
#include "host/jpegenc/mpc/api/portSettings.hpp"
#include "JPEGEnc_ProcessingComponent.h"

class JPEGEnc_Proxy;
class JPEGEnc_NmfMpc_ProcessingComponent;

/// @ingroup JPEGEncoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the JPEG Encoder, and instantiation / deinstantiation support
/// for these NMF components.
/// @note this class derives from VFM_NMF

class JPEGEnc_portSetting : public jpegenc_mpc_api_portSettingsDescriptor
{
	public:
	JPEGEnc_NmfMpc_ProcessingComponent *parentClass;
	virtual void portSettings(OMX_U32 size);
};
class JPEGEnc_NmfMpc_ProcessingComponent : public VFM_NmfMpc_ProcessingComponent
{
    friend class JPEGEnc_Proxy;
    friend class JPEGEnc_ProcessingComponent;
    friend class JPEGEnc_portSetting;
	public:
		JPEGEnc_NmfMpc_ProcessingComponent(ENS_Component &enscomp);
		virtual ~JPEGEnc_NmfMpc_ProcessingComponent();
	//	IMPORT_C static void eventHandler_cb(void *ctxt, OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2);
    private:
		JPEGEnc_Proxy *pProxyComponent;
		JPEGEnc_ProcessingComponent processingComp;
		JPEGEnc_portSetting portSettingCallBack;

        /// ----------------------------------------------
        /// Members to describe the NMF component and interfaces
        /// ----------------------------------------------

		/// @brief Variable to specify channel ID allocated by Resource Manager
		OMX_U32												   mChannelId;

		/// @brief Handle to the Data Dependency NMF Component
        t_cm_instance_handle                                   ddHandle;

		/// @brief Handle to the vpp NMF Component
        t_cm_instance_handle                                   VPPHandle;

        /// @brief Handle to the osttrace NMF Component
        t_cm_instance_handle                                   cOSTTrace;
        /// @brief Handle to the Jpeg Encoder Algo NMF Component
        t_cm_instance_handle                                   mNmfAlgoHandle;

        /// @brief Handle to the Resource Manager NMF Component
        t_cm_instance_handle                                   mNmfRmHandle;


		/// @brief NMF Interface from HOST to DD
        Ijpegenc_mpc_api_ddep                         iDdep;

        Ijpegenc_mpc_api_portSettings                 iPortSetting;

        /*! Buffer descriptor for Debug buffer

		*/
		ts_ddep_buffer_descriptor						mHeaderBufferDesc;

		/// @brief Buffer of the header
        //t_cm_memory_handle 								mHeaderBufferHandle;

		ts_ddep_buffer_descriptor						mEXIFHeaderBufferDesc;

		/// @brief Buffer of the header
        //t_cm_memory_handle 								mEXIFHeaderBufferHandle;


		/*! Buffer descriptor for Debug buffer

		*/
		ts_ddep_buffer_descriptor						mDebugBufferDesc;

		/*! Memory Handle for Debug buffer

		*/
		//t_cm_memory_handle								mDebugBufferHandle;

		/*! Buffer descriptor for Param buffer

		*/
		ts_ddep_buffer_descriptor						mParamBufferDesc;

		/*! Memory Handle for Param buffer

		*/
		//t_cm_memory_handle								mParamBufferHandle;

        //t_cm_memory_handle								temp_bufhandle;

		/*! Buffer descriptor for temp buffer

		*/
		ts_ddep_buffer_descriptor						mTempBufferDesc;


		/*! Buffer descriptor for Param buffer

		*/
		ts_ddep_buffer_descriptor						mLinkListBufferDesc;

		/*! Memory Handle for Param buffer

		*/
		//t_cm_memory_handle								mLinkListParamBufferHandle;

		void* mpc_vfm_mem_ctxt;


		OMX_BOOL fullDVFSSet;


    private:
        /// ----------------------------------------------
        /// Functions for the instantiation and deinstantiation of NMF components
        /// They are called directly by the ENS, and then by the VFM
        /// (cf. VFM_component::startNMF() for an example)
        /// Instantiation is permformed directly by the final proxy
        /// (cf. JPEGEnc_Proxy::instantiateNMF()).
        /// ----------------------------------------------

        /// @brief Instantiate and binds the NMF Components used by the Firmware
        /// @return OMX_ErrorNone if no error occured, an error otherwise
       virtual OMX_ERRORTYPE instantiate();

        /// @brief Start the NMF Components used by the Firmware
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        virtual OMX_ERRORTYPE start();

        /// @brief Configure the NMF components used by the Firmware
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        virtual OMX_ERRORTYPE configure();
		OMX_ERRORTYPE configureAlgo();
        /// @brief Stop the NMF Components used by the MPEG4 Decode
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        virtual OMX_ERRORTYPE stop();

        /// @brief Deinstantiates and unbinds the NMF Components used by the Firmware
        /// @return OMX_ErrorNone if no error occured, an error otherwise
        virtual OMX_ERRORTYPE deInstantiate();

        virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
        virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
		virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler);
		virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
		virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex,OMX_U32 nBufferIndex,OMX_U32 nSizeBytes,OMX_U8 **ppData,void **bufferAllocInfo,void **portPrivateInfo);
		virtual OMX_ERRORTYPE useBuffer(OMX_U32 nPortIndex,OMX_U32 nBufferIndex,OMX_BUFFERHEADERTYPE* pBufferHdr,void **portPrivateInfo);
        virtual void registerStubsAndSkels(void);
        virtual void unregisterStubsAndSkels(void);

        virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

        void jpegenc_nmfmpc_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);

        OMX_ERRORTYPE errorRecoveryDestroyAll(void);

		void portSettings(OMX_U32 size);


        /// @brief Creation of the header buffer
        ///
        /// It updates an internal buffer, and is mostly based on the quantization table and on the huffman tables
        /// @param [in,out] jpegenc OpenMax Component related with this processing
        /// @note This task was performed on the application side on the 8815 version
        void JPEGheaderCreation(JPEGEnc_Proxy *jpegenc);


		void set_pJecOther_parameters(JPEGEnc_Proxy *jpegenc, ts_ddep_sec_jpeg_param_desc_ *ps_ddep_sec_jpeg_param_desc, t_uint32 header_size_in_bytes, t_uint8 *headerBufferPhysicalAddress, t_uint8* thumbnailImageBufferPhysicalAddress, t_uint8* runLevelBufferPhysicalAddress);

		virtual OMX_ERRORTYPE applyConfig(
                    OMX_INDEXTYPE nParamIndex,
                    OMX_PTR pComponentConfigStructure);

};


#endif

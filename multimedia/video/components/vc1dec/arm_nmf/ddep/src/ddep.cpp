/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Data Dependency layer for vc1 decoder.
 * \author  ST-Ericsson
 */


#include "vc1dec/arm_nmf/ddep.nmf"
#include "nmf_lib.h"
#include <share/inc/macros.h>

#define REQUIRE_EOF
#ifdef PACKET_VIDEO_SUPPORT
#undef REQUIRE_EOF
#endif

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_vc1dec_arm_nmf_proxynmf_ddep_src_ddepTraces.h"
#endif

#define VC1DEC_INTERNAL_INPUT_BUFFERS_NB 5  // TODO check
#define VC1DEC_INTERNAL_OUTPUT_BUFFERS_NB 10  // TODO check

 // VC1 standard: max size of one row is MAX(6144, nb_horizontal_macroblocks*1536) in bits
 // plus other info ( 1- and 4-MV)
#define VC1DEC_MAX_FRAME_SIZE(width,height) (MAX(6144,(((width)/16)*1536)) * ((height)/16) / 8)
#define VC1DEC_INTERNAL_MAX_FRAME_SIZE (100*1024)

//#define VERBOSE_DDEP 1

// #define __DEBUG_DDEP__

// For debug purpose. Could be removed
#ifdef __DEBUG_DDEP__
vc1dec_arm_nmf_ddep *MyCodec;
#endif

#ifdef __DEBUG_DDEP__
volatile int _in_ddep=0;
inline void set_in_ddep(int flag)
{
    if (_in_ddep & flag) {
        NMF_PANIC("!!! ALREADY IN THE DDEP !!!\n");
    }
    _in_ddep |= flag;
}
inline void unset_in_ddep(int flag)
{
    if (!(_in_ddep & flag)) {
        NMF_PANIC("!!! in_ddep already false !!!\n");
    }
    _in_ddep &= ~(flag);
}
#else
inline void set_in_ddep(int flag) {};
inline void unset_in_ddep(int flag) {}
#endif


// List of improvements (to be discussed of course!)
// - construct() with arguments being the number of buffers in ports

// FIXME: no deinit of list / queue (or is it done automatically?)

#ifdef REQUIRE_EOF
#define VC1DEC_INPUT_BUFFER_MODE    VFMDDEP_BUFFERMODE_FASTANDCOPY
#else
#define VC1DEC_INPUT_BUFFER_MODE    VFMDDEP_BUFFERMODE_FASTPATH
#endif

vc1dec_arm_nmf_ddep::vc1dec_arm_nmf_ddep() :
	VFM_nmf_ddep( VC1DEC_INPUT_BUFFER_MODE,
//    VFMDDEP_BUFFERMODE_COPY, // temporary test
//    VFMDDEP_BUFFERMODE_FASTPATH, // temporary test
                VFMDDEP_BUFFERMODE_FASTPATH,
                sizeof(t_bit_buffer),
                0,
                mPorts,
                OMX_FALSE),
	mNeedMoreNal(FALSE),
	mSendLastNal(FALSE),
	mLastNalFlag(FALSE),
	pic_width(0),
	pic_height(0),
	ptrOMXHandle(0)
{
#ifdef __DEBUG_DDEP__
    MyCodec =  this;
#endif
    memset(&mFrameInfo, 0, sizeof(mFrameInfo));

    // Set DVFS values
    updateDVFS();
    setDVFS();
//+ ER335583
	updateDDR();
	setDDR();
//- ER335583
    VFM_PowerManagement::setLatency(this, VFM_PowerManagement::VFM_ResLatency_None);
}

vc1dec_arm_nmf_ddep::~vc1dec_arm_nmf_ddep()
{
    /* +ER 341788 CHANGE START FOR */
	OMX_PTR buffer;

	// flush buffers in case of error recovery
	// (avoid VFM assert in VFM_SetDDep::free when codec is destroyed)
	// Input
	buffer = mInputSet.FlushOneBuffer();
	while(buffer !=NULL)
	{
		mInputSet.getBufferHeader(buffer);
		buffer = mInputSet.FlushOneBuffer();
	}
	// Output
	buffer = mOutputSet.FlushOneBuffer();
	while(buffer !=NULL)
	{
		mOutputSet.getBufferHeader(buffer);
		buffer = mOutputSet.FlushOneBuffer();
	}
     /* +ER 341788 CHANGE START FOR */
    // Unset DVFS values
    unsetDVFS();
//+ ER335583
    unsetDDR();
//- ER335583
    VFM_PowerManagement::resetLatency(this);
}

void vc1dec_arm_nmf_ddep::start()
{
    OstTraceInt0(TRACE_API, "[DDEP] > start");

    OstTraceInt0(TRACE_API, "[DDEP] < start");
}

void vc1dec_arm_nmf_ddep::stop()
{
    OstTraceInt0(TRACE_API, "[DDEP] > stop");



    OstTraceInt0(TRACE_API, "[DDEP] > stop");
}


void METH(fsmInit)(fsmInit_t initFSM)
{
#if VERBOSE_DDEP >= 1
     NMF_LOG("DDEP: fsmInit()\n");
#endif

    bool ok;
    //mFsmInitCalled = true;

    t_uint16 portsDisabled = initFSM.portsDisabled;
    t_uint16 portsStdTunneled = initFSM.portsTunneled;
    // trace initialization
  if (initFSM.traceInfoAddr) {
        setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
        decoder_trace_init.initTraceInfo(initFSM.traceInfoAddr, initFSM.id1); // to convey trace structs/config to lower ARM-NMF components
    }

    OstTraceFiltInst0(TRACE_FLOW, "VC1Dec: arm_nmf: ddep: fsmInit()\n");
    /// --- Initialization common to all video Ddep
	/* + changes for ER 360599 */
	{
		OstTraceFiltInst0(TRACE_FLOW, "VC1Dec: arm_nmf: ddep: Passing parent handle in initVFMMemory");
#ifdef NO_HAMAC
	   	ok = initVFMMemory(1,(OMX_PTR)ptrOMXHandle);
#else
	    ok = initVFMMemory(0,(OMX_PTR)ptrOMXHandle);
#endif
	}
	/* - changes for ER 360599 */


    mNeedMoreNal = false;
    mSendLastNal = false;
    mLastNalFlag = 0;

    // --- Required by the FSM initialization
    // init(Direction, bufferSupplier, isHWport, sharingPort, OMX_BUFFERHEADERTYPE**, bufferCount, nmfitf, portIdx, isDisabled, componentOwner);
    // we are not buffer supplier as the application is ==> sharingPort=0 and OMX_BUFFERHEADERTYPE**=0
    // to be checked in tunneling support
    mPorts[0].init(InputPort,  false, true, 0, 0, mMaxInput, &inputport, 0, (portsDisabled & (1<<0)), (portsStdTunneled & (1<<0)), this);
    mPorts[1].init(OutputPort, false, true, 0, 0, mMaxOutput, &outputport, 1, (portsDisabled & (1<<1)), (portsStdTunneled & (1<<1)), this);
    init(2, mPorts, &proxy, &me, !ok);
}

void METH(reset)()
{
     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: ddep: reset()\n");
#if VERBOSE_DDEP >= 1
     NMF_LOG("DDEP: reset()\n");
#endif
}

void METH(process)()
{
     	OstTraceFiltInst2(TRACE_API,  "VC1Dec: arm_nmf: ddep: process(): %d bufIn, %d bufOut\n", mPorts[0].queuedBufferCount(), mPorts[1].queuedBufferCount());
#if VERBOSE_DDEP >= 1
     NMF_LOG("DDEP: process(): %d bufIn, %d bufOut\n", mPorts[0].queuedBufferCount(), mPorts[1].queuedBufferCount());
#endif
    set_in_ddep(1);

    // Check the VFM allows the start: no flush, ...
    if (!VFM_CheckStart()) {
        unset_in_ddep(1);
        return;
    }

    sendAllOutput();        // Send output buffers to the DPB
    checkInputSize();       // Check the size of the 1st input buffer in case of copy only
    if (mNeedMoreNal) {
        sendOneInput();     // Start internal copy and call codec as soon as possible
    }
    copyAllInput();
    unset_in_ddep(1);
}

void METH(checkInputSize)()
{
#ifdef REQUIRE_EOF
#else
    return;     // no check
#endif

    if (mInputBufferMode&VFMDDEP_BUFFERMODE_COPY) {
        return;
    }

    // Checks of buffer size in case of only fastpath
    OMX_BUFFERHEADERTYPE *pBuffer;
    if (mPorts[0].queuedBufferCount() != 0) {
        if (!(mInputBufferMode&VFMDDEP_BUFFERMODE_COPY) && (mInputBufferMode&VFMDDEP_BUFFERMODE_FASTPATH)) {
            pBuffer = mPorts[0].getBuffer(0);
            OMX_U32 width = mFrameInfo.common_frameinfo.pic_width;
            OMX_U32 height = mFrameInfo.common_frameinfo.pic_height;
            // VC1 standard: max size of one row is MAX(6144, nb_horizontal_macroblocks*1536) in bits
            OMX_U32 size = VC1DEC_MAX_FRAME_SIZE(width,height);
            //size = MAX(size, 32*1024);
            ARMNMF_DBC_ASSERT(pBuffer->nAllocLen >= size);
        }
    }
}

/**
   Copy all input buffers to internal buffers (when possible).
*/
void METH(copyAllInput)()
{
    while (mPorts[0].queuedBufferCount() != 0) {
        // Complete the copy, if possible
        if (!copyOneInput()) {
            break;      // no more copy is possible
        }
    }
}

void METH(destroy)()
{
	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: ddep: destroy()\n");
#if VERBOSE_DDEP >= 1
     NMF_LOG("DDEP: destroy()\n");
#endif
}

/**
    Main function to send input buffers to the DSP for decoding.
    Buffers can be either input buffers provided by the user of Openmax component
    or internal buffers copied from input buffers.
    Called by VFM_nmf_ddep::sendOneInput().
*/
EXPORT_C void METH(process_inputbuffer)(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal)
{
     	OstTraceFiltInst4(TRACE_API,  "VC1Dec: arm_nmf: ddep: process_inputbuffer() : pBuffer=0x%x pBuffer->pBuffer=%x pBuffer->nFilledLen=%d pBuffer->nFlags=%x\n", (unsigned int)pBuffer,(unsigned int)pBuffer->pBuffer, pBuffer->nFilledLen, pBuffer->nFlags);
     	OstTraceFiltInst1(TRACE_API,  "VC1Dec: arm_nmf: ddep: process_inputbuffer() : isInternal=%d\n", isInternal);
#if VERBOSE_DDEP >= 1
     NMF_LOG("DDEP: process_inputbuffer(): pBuffer=0x%x pBuffer->pBuffer=%x pBuffer->nFilledLen=%d pBuffer->nFlags=%x isInternal=%d\n",
            pBuffer,pBuffer->pBuffer,pBuffer->nFilledLen,pBuffer->nFlags,isInternal);
#endif

    OMX_U32 nFlags = pBuffer->nFlags;
    ARMNMF_DBC_ASSERT(!(nFlags & VFM_FLAG_INTERNALBUFFER));
    ARMNMF_DBC_ASSERT(pBuffer->nOffset+pBuffer->nFilledLen<=pBuffer->nAllocLen);
    ARMNMF_DBC_ASSERT(pBuffer->nFilledLen);

    // +CR334366 CHANGE START FOR
     // +VC1 codec validation
//#ifdef PACKET_VIDEO_SUPPORT
    OMX_U32 nSavedFilled = 0xFFFFFFFF;
    if ((pBuffer->nFlags & OMX_BUFFERFLAG_CODECCONFIG)) {
        // configuration as provided by the Android parser and as expected
        // by the codec are not the same
        // We only ave to keep the Struct_C structure, that is bytes 0x8, 0x9, 0xA and 0xB
        pBuffer->nOffset += 8;
        nSavedFilled = pBuffer->nFilledLen;
        pBuffer->nFilledLen = 4;
    }
//#endif
     // -VC1 codec validation
    // -CR334366 CHANGE END OF

    nFlags &= ~(OMX_BUFFERFLAG_ENDOFFRAME|OMX_BUFFERFLAG_CODECCONFIG);
    if (isInternal) {
        nFlags |= VFM_FLAG_INTERNALBUFFER;
        pBuffer->nFlags |= VFM_FLAG_INTERNALBUFFER;
    }

    //ARMNMF_DBC_ASSERT(mNeedMoreNal);
    mNeedMoreNal = 0;


    // create the nal, and add it to the queue of buffers that are being processed
    t_bit_buffer *pNal = (t_bit_buffer *)mInputSet.add(pBuffer);  // store this buffer in the dequeued list
    pNal->addr  = (OMX_U8 *)pBuffer->pBuffer + pBuffer->nOffset;
    pNal->os    = 8;
    pNal->start = pNal->addr;
    pNal->end   = (t_uint8*) ((t_uint32)(pNal->addr) + pBuffer->nFilledLen);
    pNal->inuse = 0;
    pNal->next  = 0;

#ifndef OMX_SKIP64BIT
    t_uint32 nTimeStampH = (pBuffer->nTimeStamp >> 32) & 0xFFFFFFFF;
    t_uint32 nTimeStampL = (pBuffer->nTimeStamp      ) & 0xFFFFFFFF;
#else
    t_uint32 nTimeStampL = pBuffer->nTimeStamp.nLowPart;
    t_uint32 nTimeStampH = pBuffer->nTimeStamp.nHighPart;
#endif

    if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS) {
        //mSendLastNal = true;
        mLastNalFlag = pBuffer->nFlags;
    }
    nal.frame(pNal, 0, nTimeStampH, nTimeStampL, nFlags);

    // +CR334366 CHANGE START FOR
     // +VC1 codec validation
//#ifdef PACKET_VIDEO_SUPPORT
    if (nSavedFilled != 0xFFFFFFFF) {
        pBuffer->nOffset -= 8;
        pBuffer->nFilledLen = nSavedFilled;
    }
//#endif
     // -VC1 codec validation
    // -CR334366 CHANGE END OF
}

EXPORT_C void METH(process_special_inputbuffer)(OMX_BUFFERHEADERTYPE *pBuffer)
{
    // special input buffer = buffer is empty ( nFilledLength == 0 )
     	OstTraceFiltInst3(TRACE_API,  "VC1Dec: arm_nmf: ddep: process_special_inputbuffer() : pBuffer=0x%x pBuffer->pBuffer=%x size=%d \n", (unsigned int)pBuffer, (unsigned int)pBuffer->pBuffer, pBuffer->nFilledLen);
#if VERBOSE_DDEP >= 1
    NMF_LOG("DDEP: process_special_inputbuffer(): pBuffer=0x%x pBuffer->pBuffer=0x%x size=%d\n",
             pBuffer, pBuffer->pBuffer, pBuffer->nFilledLen);
#endif
    ARMNMF_DBC_ASSERT(pBuffer->nFilledLen==0);

    if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS) {
        mNeedMoreNal = false;
        mSendLastNal = false;
        nal.frame(0, 0, 0, 0, mLastNalFlag | pBuffer->nFlags);
    } else {
        // nothing to do. Only return the buffer
        scheduleProcessEvent();
    }

    OMX_BUFFERHEADERTYPE *pBufferPort = mPorts[0].getBuffer(0);
    ARMNMF_DBC_ASSERT(pBuffer == pBufferPort);
    mPorts[0].dequeueAndReturnBuffer();
}

EXPORT_C void METH(process_outputbuffer)(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal)
{
     	OstTraceFiltInst2(TRACE_API,  "VC1Dec: arm_nmf: ddep: process_outputbuffer() : pBuffer=0x%x isInternal=%d\n", (unsigned int)pBuffer, isInternal);
#if VERBOSE_DDEP >= 1
    NMF_LOG("DDEP: process_outputbuffer(): pBuffer=0x%x isInternal=%d\n", pBuffer, isInternal);
#endif

    pBuffer->nOffset = 0;
    pBuffer->nFlags = 0;
    if (isInternal) {
        pBuffer->nFlags |= VFM_FLAG_INTERNALBUFFER;
    }

    mOutputSet.add(pBuffer, pBuffer->pBuffer);                              // store this buffer in the dequeued list
    output_buffer.output_buffer(pBuffer->pBuffer, pBuffer->nAllocLen);      // send the output buffer to the DPB
}
 /* +ER 341788 CHANGE START FOR */
void METH(flush)(void)
{
    NMF_LOG("\n INSIDE FLUSH IN DDEP.cpp \n ");
    flush_command_to_codec();
   
} /* +ER 341788 CHANGE START FOR */

void METH(nal_handshake)(t_handshake mode, t_bit_buffer *p)
{
     	OstTraceFiltInst2(TRACE_API,  "VC1Dec: arm_nmf: ddep: nal_handshake() : mode=%d p_bit_buffer=0x%x\n", mode, (unsigned int)p);
#if VERBOSE_DDEP >= 1
    NMF_LOG("DDEP: nal_handshake mode=%d p_bit_buffer=0x%x\n", mode, p);
#endif
    // Update status if more nals are required or not
    set_in_ddep(2);

    switch (mode) {
    case XON:
        mNeedMoreNal = true;
        break;
    case FLUSH_ACK:
        mNeedMoreNal = false;
        codec_ack_flushing(0);
        break;
    case XOFF:
#if VERBOSE_DDEP >= 1
        NMF_LOG("DDEP: handshake mode XOFF\n");
#endif
        break;
    case PAUSE_ACK:
#if VERBOSE_DDEP >= 1
        NMF_LOG("DDEP: handshake mode PAUSE_ACK\n");
#endif
        codec_ack_pause();
        break;
    default:
#if VERBOSE_DDEP >= 1
        NMF_LOG("DDEP: unhandled handshake mode: %d\n",mode);
#endif
        break;
    }

    // Free buffers when possible
    while (p) {
        OMX_BUFFERHEADERTYPE *pBuffer = mInputSet.getBufferHeader(p);
        returnBufferDDep(0, pBuffer);
        p=(t_bit_buffer*) p->next;
    }

    if (mNeedMoreNal) {
        if (mSendLastNal) {  /* this is H264 stuff that will never happen for VC1 */
            mNeedMoreNal = false;
            unset_in_ddep(2);
            nal.frame(0, 0, 0,0,mLastNalFlag);
            set_in_ddep(2);
        } else {
            scheduleProcessEvent();
        }
    }
    unset_in_ddep(2);
}

void METH(picture)(t_picture_output valid, t_uint8 *p_fb, t_uint32 nFlags, t_frameinfo info)
{
     	OstTraceFiltInst3(TRACE_API,  "VC1Dec: arm_nmf: ddep: picture(): valid=%d p_fb=0x%x nFlags=%x\n", valid, (unsigned int)p_fb, nFlags);
#if VERBOSE_DDEP >= 1
    NMF_LOG("DDEP: picture(): valid=%d p_fb=0x%x nFlags=%x\n", valid, p_fb, nFlags);
#endif
    set_in_ddep(4);
    nFlags &= ~(VFM_FLAG_INTERNALBUFFER);
    switch (valid) {
    case INVALID_PIC_FLUSH_ACK:
        codec_ack_flushing(1);
        break;

    case INVALID_PIC:
    	OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: Call from INVALID PICTURE");
        if (p_fb) {
            // this is a flush
            // not required discovered during 370888
            //if (!is_codec_flushing()) {
            //    NMF_PANIC("Codec is not flushing, but I receive a flushing code\n");
            //}
            OMX_BUFFERHEADERTYPE *pBuffer = mOutputSet.getBufferHeader(p_fb);
            pBuffer->nFilledLen = 0;
            pBuffer->nFlags = nFlags;
            returnBufferDDep(1, pBuffer);
            /*ER 370888 Change Start For*/
            if (nFlags & OMX_BUFFERFLAG_EOS) {
                // Send the EOS
                proxy.eventHandler(OMX_EventBufferFlag, 1, OMX_BUFFERFLAG_EOS);
            } /*ER 370888 Change Start For*/

        } else {
            check_picinfo(info);
        }
        break;

    case VALID_PIC:
        {
			OstTraceFiltInst0(TRACE_FLOW,  "VC1Dec: arm_nmf: Call from VALID PICTURE");
            check_picinfo(info);
            /* round up to a multiple of 16, to handle correctly cropping */
            t_uint16 tmp_width = (info.specific_frameinfo.max_picture_width+0xF)&(~0xF);
            t_uint16 tmp_height = (info.specific_frameinfo.max_picture_height+0xF)&(~0xF);
            t_uint32 size = tmp_width * tmp_height; // In case of multires we fill anyway the whole buf, so we use MAX_pic...

            // Set the flags of the buffer and time stamps
            OMX_BUFFERHEADERTYPE *pBufferToFill = mOutputSet.getBufferHeader(p_fb);

            if ((nFlags & OMX_BUFFERFLAG_DECODEONLY) && (!(nFlags & OMX_BUFFERFLAG_EOS)) && (!is_codec_flushing())) {
                // Decode only: don't output the buffer to next component.
                // Send the buffer back to codec (if no End Of Stream).
                process_outputbuffer(pBufferToFill, OMX_FALSE);
            } else {
                // Output the buffer to next component.
                // Set the flags of the buffer and time stamps
                if (pBufferToFill->nFlags & VFM_FLAG_INTERNALBUFFER) {
                    nFlags |= VFM_FLAG_INTERNALBUFFER;
                }
                pBufferToFill->nFlags = nFlags | OMX_BUFFERFLAG_ENDOFFRAME;
                pBufferToFill->nFilledLen = (size*3)/2;
#if VERBOSE_DDEP >= 1
                NMF_LOG("DDEP: picture(): filled output buffer 0x%x size %d nFlags %x\n",
                        pBufferToFill,pBufferToFill->nFilledLen,pBufferToFill->nFlags);
#endif

                // Update timestamps
#ifndef OMX_SKIP64BIT
                pBufferToFill->nTimeStamp = (((OMX_S64)info.common_frameinfo.nTimeStampH) << 32) | (((OMX_S64)info.common_frameinfo.nTimeStampL) & 0xFFFFFFFF);
#else
                pBufferToFill->nTimeStamp = { info.common_frameinfo.nTimeStampL, info.common_frameinfo.nTimeStampH };
#endif

                // return the buffer to the proxy, but do not remove it from the VFM_Memory
                // as it can be used by the decoder as a reference
                returnBufferDDep(1, pBufferToFill);

                // FIXME: check size
                // FIXME: return profile / level to application
                // FIXME: what about DPB size

                // Send event handler that the EOS flag has been received
                if (nFlags & OMX_BUFFERFLAG_EOS) {
                    // Send the EOS
#if VERBOSE_DDEP >= 1
                NMF_LOG("DDEP: picture(): sending event flag EOS\n");
#endif
                    proxy.eventHandler(OMX_EventBufferFlag, 1, OMX_BUFFERFLAG_EOS);
                }
            }
        }
        break;

    default:
        NMF_PANIC("vc1dec_arm_nmf_ddep::picture(): unspecified valid value\n");
        OstTraceInt0(TRACE_ERROR, "VC1Dec: arm_nmf: ddep: picture(): unspecified valid value\n");
        break;
    }
    unset_in_ddep(4);
}

void METH(check_picinfo)(const t_frameinfo &info)
{
//    printf("expected pic_width = %d\n",mInfo.pic_width);
//    if ((info.pic_width!=mInfo.pic_width) || (info.pic_height!=mInfo.pic_height)) {
//        NMF_LOG("vc1dec_arm_nmf_ddep::check_picinfo()  -  size of the frame is not correct\n");
//    }
    // FIXME t_uint16 profile_idc;
    // FIXME t_uint16 level_idc;
    // FIXME t_uint32 size16;
    // FIXME t_uint16 DPBsize;
}

void METH(set_configuration)(void *ptrHandle)
{
     	OstTraceInt1(TRACE_API,  "VC1Dec: arm_nmf: ddep: > set_configuration() ptrHandle (0x%x)",(t_uint32)ptrHandle);
		ptrOMXHandle=ptrHandle;
        initOMXHandles((OMX_PTR)ptrOMXHandle);
     	OstTraceInt0(TRACE_API,  "VC1Dec: arm_nmf: ddep: < set_configuration()");
}


void METH(set_config)(t_uint32 domainDdrId, t_uint32 domainEsramId, t_frameinfo frame_info)
{
     	OstTraceInt0(TRACE_API,  "VC1Dec: arm_nmf: ddep: set_config()\n");
#if VERBOSE_DDEP >= 1
     NMF_LOG("DDEP: set_config()\n");
#endif

    setDomainId(domainDdrId, domainEsramId);
    mFrameInfo = frame_info;

    // Set the DVFS value
    updateDVFS();
//+ ER335583
    updateDDR();
//- ER335583

    // allocate internal buffers
    if (mInputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
        t_uint32 width = ((mFrameInfo.common_frameinfo.pic_width+15)& 0xFFF0U);
        t_uint32 height = ((mFrameInfo.common_frameinfo.pic_height+15)& 0xFFF0U);

#ifdef _CACHE_OPT_
#define VC1DEC_IS_CACHED OMX_TRUE
#else
#define VC1DEC_IS_CACHED OMX_FALSE
#endif

        OMX_U32 size = VC1DEC_MAX_FRAME_SIZE(width,height);
        size = MAX(size, VC1DEC_INTERNAL_MAX_FRAME_SIZE);

        //t_uint32 size = 100*1024; // temporary test
        //t_uint32 size = width*height*4; // original setting
        OMX_ERRORTYPE error = mInputInternal.alloc(&mInputSet,
                                                    VC1DEC_INTERNAL_INPUT_BUFFERS_NB,
                                                    size,
                                                    VC1DEC_IS_CACHED);

        ARMNMF_DBC_ASSERT(error==OMX_ErrorNone);
#if VERBOSE_DDEP >= 1
     NMF_LOG("DDEP: set_config(): alloc %d internal input buffers size %d cached: %d\n",
                VC1DEC_INTERNAL_INPUT_BUFFERS_NB, size, VC1DEC_IS_CACHED);
#endif
    }

    if (mOutputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
        // Size of a frame. Need reallocation if size changes
        t_uint32 size = ((mFrameInfo.common_frameinfo.pic_width+15)& 0xFFF0U)
                        * ((mFrameInfo.common_frameinfo.pic_height+15)& 0xFFF0U);
        size = (size*3)/2; // YUV420MB
        t_uint32 buffer_nb = VC1DEC_INTERNAL_OUTPUT_BUFFERS_NB; // TODO check

        OMX_ERRORTYPE error = mOutputInternal.alloc(&mOutputSet, buffer_nb, size, OMX_FALSE);
        ARMNMF_DBC_ASSERT(error==OMX_ErrorNone);
#if VERBOSE_DDEP >= 1
     NMF_LOG("DDEP: set_config(): alloc %d internal output buffers size %d cached: %d\n",
                buffer_nb, size, VC1DEC_IS_CACHED);
#endif
    }

    set_decoder_parameter.set_picsize(frame_info.common_frameinfo.pic_width, frame_info.common_frameinfo.pic_height);
    set_decoder_parameter.set_ImmediateRelease(frame_info.specific_frameinfo.bImmediateRelease);
}

void METH(flush_command_to_codec)()
{
     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: ddep: flush_command_to_codec()\n");
#if VERBOSE_DDEP >= 1
    NMF_LOG("DDEP: flush_command_to_codec()\n");
#endif
    // +CR334366 CHANGE START FOR
     // +VC1 codec validation
    mNeedMoreNal = 0;
     // -VC1 codec validation
    // -CR334366 CHANGE END OF
    nal.input_decoder_command(COMMAND_FLUSH);
}


void METH(pause_command_to_codec)()
{
     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: ddep: pause_command_to_codec()\n");
    nal.input_decoder_command(COMMAND_PAUSE);
}

void METH(send_vfm_memory)(void *memory_ctxt)
{
     	OstTraceFiltInst0(TRACE_API,  "VC1Dec: arm_nmf: ddep: send_vfm_memory()\n");
#if VERBOSE_DDEP >= 1
    NMF_LOG("DDEP: send_vfm_memory()\n");
#endif
    set_decoder_parameter.set_memory_ctxt(memory_ctxt);
}

void METH(sendProxyEvent)(OMX_IN OMX_EVENTTYPE eEvent,  OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2)
{
     	OstTraceFiltInst3(TRACE_API,  "VC1Dec: arm_nmf: ddep: sendProxyEvent(): eEvent=%x nData1=%x nData2=%x\n", eEvent, nData1, nData2);
#if VERBOSE_DDEP >= 1
    NMF_LOG("DDEP: sendProxyEvent(): eEvent=%x nData1=%x nData2=%x\n",
            eEvent, nData1, nData2);
#endif
    proxy.eventHandler(eEvent, nData1, nData2);
}

EXPORT_C OMX_BOOL METH(specificActivateInputFastPath)(OMX_BUFFERHEADERTYPE *pBuffer) const
{
    // is it a readonly buffer?
    if (pBuffer->nFlags & OMX_PORTTUNNELFLAG_READONLY) {
        return OMX_FALSE;
    }

    return OMX_TRUE;
}

EXPORT_C OMX_BOOL METH(isSpecialCase)(OMX_BUFFERHEADERTYPE *pBuffer) const
{
    // special case if buffer is empty.
    return (pBuffer->nFilledLen==0 ? OMX_TRUE : OMX_FALSE);
}

void METH(updateDVFS)()
{
	/* +change for ER 345421 */
	OMX_U32 thresholdValueDVFS = (800/16)*(480/16); //DVFS  managed on WVGA resolution
	OMX_U32 actualMBs = (mFrameInfo.common_frameinfo.pic_width/16)*(mFrameInfo.common_frameinfo.pic_height/16);
	/* -change for ER 345421 */
	if (actualMBs > thresholdValueDVFS) {
		OstTraceInt0(TRACE_FLOW,  "VC1Dec: arm_nmf: ddep: updateDVFS, forcing to 100%");
        setDVFSValue(OMX_TRUE);
    } else {
		OstTraceInt0(TRACE_FLOW,  "VC1Dec: arm_nmf: ddep: updateDVFS, keeping at 50%");
        setDVFSValue(OMX_FALSE);
    }
}

//+ ER335583
void METH(updateDDR)()
{
	/* +change for ER 345421 */
	OMX_U32 thresholdValueDDR = (1280/16)*(720/16); //DDR to managed on 720p resolution
	OMX_U32 actualMBs = (mFrameInfo.common_frameinfo.pic_width/16)*(mFrameInfo.common_frameinfo.pic_height/16);
	/* to keep it to 50% even for 720p resolution */
	/* -change for ER 345421 */
    if (actualMBs > thresholdValueDDR) {
		OstTraceInt0(TRACE_FLOW,  "VC1Dec: arm_nmf: ddep: updateDDR, forcing to 100%");
        setDDRValue(OMX_TRUE);
    } else {
		OstTraceInt0(TRACE_FLOW,  "VC1Dec: arm_nmf: ddep: updateDDR, kept at 50%");
        setDDRValue(OMX_FALSE);
    }
}
//- ER335583

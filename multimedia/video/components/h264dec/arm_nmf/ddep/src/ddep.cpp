/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "h264dec/arm_nmf/ddep.nmf"
#include "VFM_Memory.h"
#include "VFM_Types.h"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "video_components_h264dec_arm_nmf_ddep_src_ddepTraces.h"
#endif

// Traces
//#ifdef PACKET_VIDEO_SUPPORT
//#define LOG_TAG "h264dec"
//#include<cutils/log.h>
//#define NMF_LOG LOGI
//#endif


#define H264DEC_DEBUG_NONE              0x0     // No dump
#define H264DEC_DEBUG_PROCESS           0x1     // Input/Output buffer in process()
#define H264DEC_DEBUG_PROCESS_INPUT     0x2     // Input buffer in process()
#define H264DEC_DEBUG_PROCESS_OUTPUT    0x4     // Output buffer in process()
#define H264DEC_DEBUG_FLUSHES           0x8     // Flushes
#define H264DEC_DEBUG_INPUT_ACK         0x10    // Input Acknowledgment
#define H264DEC_DEBUG_OUTPUT_ACK        0x20    // Output Acknowledgment
#define H264DEC_DEBUG_PAUSE             0x40    // Pause and Pause Ack
#define H264DEC_DEBUG_STATISTICS        0x80    // Statistics on starvation and performances

#define H264DEC_DEBUG_FLOW              0x100
#define H264DEC_DEBUG_DEBUG             0x200
#define H264DEC_DEBUG_DEBUG2            0x400
#define H264DEC_DEBUG_DEBUG3            0x800
#define H264DEC_DEBUG_WARNING           0x1000
#define H264DEC_DEBUG_ERROR             0x2000

#define H264DEC_DEBUG (H264DEC_DEBUG_ERROR | H264DEC_DEBUG_WARNING)
// #define H264DEC_DEBUG (H264DEC_DEBUG_STATISTICS)
// #define H264DEC_DEBUG (H264DEC_DEBUG_PROCESS | H264DEC_DEBUG_PROCESS_INPUT | H264DEC_DEBUG_PROCESS_OUTPUT | H264DEC_DEBUG_INPUT_ACK | H264DEC_DEBUG_OUTPUT_ACK)
// #define H264DEC_DEBUG (H264DEC_DEBUG_INPUT_ACK | H264DEC_DEBUG_OUTPUT_ACK)
// #define H264DEC_DEBUG (H264DEC_DEBUG_PROCESS_OUTPUT | H264DEC_DEBUG_OUTPUT_ACK)
// #define H264DEC_DEBUG (H264DEC_DEBUG_PAUSE | H264DEC_DEBUG_INPUT_ACK | H264DEC_DEBUG_OUTPUT_ACK)
// #define H264DEC_DEBUG (H264DEC_DEBUG_PROCESS_INPUT)
// #define H264DEC_DEBUG (0xFFFFFFFF)

// Number of safety bytes required in the codec in order to save:
// - 2 extra bytes in order to add some extra "0"?
// - 4 for the start code (4 chars) in HamacFillBistreamIn()
// - 2 more for safety
#define SAFETYBYTES 8

// OMX_TRUE to set final way for power management
#define FINAL_POWER_MANAGEMENT OMX_TRUE

inline void METH(h264dec_assert)(int condition, int line, OMX_BOOL isFatal, int param1, int param2)
{
    if (!condition) {
        if (isFatal) {
            OstTraceInt3(TRACE_ERROR, "H264DEC: arm_nmf: ddep: h264dec_assert: VIDEOTRACE Error line %d, param1=%d  param2=%d\n", line, param1, param2);
            VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_ERROR, NMF_LOG("H264DEC: arm_nmf: ddep: h264dec_assert: VIDEOTRACE Error line %d, param1=%d  param2=%d\n", line, param1, param2));
            ARMNMF_DBC_ASSERT(0==1);
        } else {
            OstTraceFiltInst3(TRACE_WARNING, "H264DEC: arm_nmf: ddep: h264dec_assert: VIDEOTRACE Warning line %d, param1=%d  param2=%d\n", line, param1, param2);
            VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_WARNING, NMF_LOG("H264DEC: arm_nmf: ddep: h264dec_assert: VIDEOTRACE Warning line %d, param1=%d  param2=%d\n", line, param1, param2));
        }
    }
}


typedef unsigned int size_t;
extern "C" IMPORT_C void *memset(void *s, int c, size_t n);

h264dec_arm_nmf_ddep::h264dec_arm_nmf_ddep() :
#ifdef REQUIRE_EOF
        VFM_nmf_ddep(VFMDDEP_BUFFERMODE_FASTANDCOPY, VFMDDEP_BUFFERMODE_FASTPATH, sizeof(t_bit_buffer), 0, mPorts, FINAL_POWER_MANAGEMENT),
#else
        VFM_nmf_ddep(VFMDDEP_BUFFERMODE_FASTPATH, VFMDDEP_BUFFERMODE_FASTPATH, sizeof(t_bit_buffer), 0, mPorts, FINAL_POWER_MANAGEMENT),
#endif  // REQUIRE_EOF
        mNeedMoreNal(0),
        mSendLastNal(0),
        mLastNalFlag(0),
        mFsmInitCalled(false),
        mPendingSendConfig(false),
        mPendingPicSize(false),
// +CR324558 CHANGE START FOR
        mPendingDPBSize(false),
// -CR324558 CHANGE END OF
        mH264DecMaskFlushingAck(0),
        mNbPendingCommands(0),
        mLastTimestamp(0),
	    pOMXHandle(0),
        mFsmInitDelayed(false)
{
    memset(&mSei, 0, sizeof(mSei));
    memset(&mFrameInfo, 0, sizeof(mFrameInfo));
    omxilosalservices::OmxILOsalMutex::MutexCreate(mMutexPendingCommand);

    if (FINAL_POWER_MANAGEMENT==OMX_FALSE) {
        // Set DVFS values
        setDVFSValue(OMX_TRUE);
        setDVFS();
//+ ER335583
        setDDRValue(OMX_TRUE);
        setDDR();
//- ER335583
        VFM_PowerManagement::setLatency(this, VFM_PowerManagement::VFM_ResLatency_None);
    }
}

h264dec_arm_nmf_ddep::~h264dec_arm_nmf_ddep()
{
    if (FINAL_POWER_MANAGEMENT==OMX_FALSE) {
        //Unset DVFS values
        unsetDVFS();
//+ ER335583
		unsetDDR();
//- ER335583
        VFM_PowerManagement::resetLatency(this);
    }

    omxilosalservices::OmxILOsalMutex::MutexFree(mMutexPendingCommand);
}

void METH(start)()
{
}

void METH(stop)()
{
    // lock and unlock the mutex to ensure no more activites is in the codec when stop ends
    mMutexPendingCommand->MutexLock();
    h264dec_assert((mNbPendingCommands==0), __LINE__, OMX_FALSE);
    mMutexPendingCommand->MutexUnlock();
}

void METH(fsmInit)(fsmInit_t initFSM)
{
    bool ok;
    mInitFSM = initFSM;
    mFsmInitCalled = true;

    t_uint16 portsDisabled = initFSM.portsDisabled;
    t_uint16 portsStdTunneled = initFSM.portsTunneled;
    // trace initialization
    if (initFSM.traceInfoAddr) {
        setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
        decoder_trace_init.initTraceInfo(initFSM.traceInfoAddr, initFSM.id1); // to convey trace structs/config to lower ARM-NMF components
    }

    /// Uncomment this line to check signature of input port
    // mInputSet.setCheckSignature(OMX_TRUE);
    {
            OstTraceFiltInst0(TRACE_FLOW, "H264Dec: arm_nmf: ddep: Passing parent handle in initVFMMemory");
#ifdef NO_HAMAC
            ok = initVFMMemory(1,(OMX_PTR)pOMXHandle);
#else
            ok = initVFMMemory(0,(OMX_PTR)pOMXHandle);
#endif
    }

    // --- Required by the FSM initialization
    // init(Direction, bufferSupplier, isHWport, sharingPort, OMX_BUFFERHEADERTYPE**, bufferCount, nmfitf, portIdx, isDisabled, isStdTunneled, componentOwner);
    // we are not buffer supplier as the application is ==> sharingPort=0 and OMX_BUFFERHEADERTYPE**=0
    // to be checked in tunneling support
    if(!ok) {
    mPorts[0].init(InputPort,  false, true, 0, 0, mMaxInput, &inputport, 0, (portsDisabled & (1<<0)), (portsStdTunneled & (1<<0)), this);
    mPorts[1].init(OutputPort, false, true, 0, 0, mMaxOutput, &outputport, 1, (portsDisabled & (1<<1)), (portsStdTunneled & (1<<1)), this);
    init(2, mPorts, &proxy, &me, !ok);
        return;
    }
    mFsmInitDelayed = true;
    if (mPendingSendConfig) {
        set_config(getDomainDdrId(), getDomainEsramId(), mFrameInfo);
    }
}

void METH(delayed_fsmInit)(fsmInit_t initFSM,bool invalid){
    t_uint16 portsDisabled = initFSM.portsDisabled;
    t_uint16 portsStdTunneled = initFSM.portsTunneled;
    mPorts[0].init(InputPort,  false, true, 0, 0, mMaxInput, &inputport, 0, (portsDisabled & (1<<0)), (portsStdTunneled & (1<<0)), this);
    mPorts[1].init(OutputPort, false, true, 0, 0, mMaxOutput, &outputport, 1, (portsDisabled & (1<<1)), (portsStdTunneled & (1<<1)), this);
    init(2, mPorts, &proxy, &me, invalid);
}
void METH(stateChangeIndication)(OMX_STATETYPE oldState, OMX_STATETYPE newState)
{
    if (newState == OMX_StateIdle) {
        // reinit of the decoder
        if (oldState!=OMX_StateLoaded)
        send_vfm_memory(0);

        addPendingCommand();
        set_parser_parameter.set_parser_parameter(1);

        // reinit of the h264dec internal variables
        mNeedMoreNal = 0;
        mSendLastNal = 0;
        mLastNalFlag = 0;
        mLastTimestamp = 0;
    }
}

void METH(process)()
{
    // Traces
#ifndef __ndk5500_a0__
    OstTraceFiltInst0(TRACE_FLOW, "H264DEC: arm_nmf: ddep: process\n");
    ost_port_status_flow();
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_FLOW, NMF_LOG("H264DEC: arm_nmf: ddep: process\n"));
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_FLOW, nmflog_port_status());
#endif
    addPerformance((H264DEC_DEBUG & H264DEC_DEBUG_STATISTICS), OMX_FALSE);

    // Raise the ErrorMap event if needed
    raiseEventErrorMap((Common_Error_Map *)mFrameInfo.specific_frameinfo.pErrorMap);

    // Check the VFM allows the start: no flush, ...
    if (!VFM_CheckStart()) {
        return;
    }

    sendAllOutput();        // Send output buffers to the DPB
    if (mNeedMoreNal) {
        sendOneInput();     // Start internal copy and call codec as soon as possible
    }
    copyAllInput();
}

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
}

EXPORT_C void METH(process_inputbuffer)(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal)
{
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_PROCESS_INPUT,
        NMF_LOG("ddep::process_inputbuffer(): Buffer of size %d (isInternal=%d)\n", pBuffer->nFilledLen, isInternal));

    // Sanity check that should never be activated, that is why they are fatal errors
    OMX_U32 nFlags = pBuffer->nFlags;
    OMX_U32 nFilledLen = pBuffer->nFilledLen;
    h264dec_assert((!(nFlags & VFM_FLAG_INTERNALBUFFER)), __LINE__, OMX_TRUE);
    h264dec_assert((pBuffer->nOffset+pBuffer->nFilledLen+SAFETYBYTES<=pBuffer->nAllocLen), __LINE__, OMX_TRUE);
    h264dec_assert((pBuffer->nFilledLen), __LINE__, OMX_TRUE);
    h264dec_assert((mNeedMoreNal), __LINE__, OMX_TRUE);

    nFlags &= ~(OMX_BUFFERFLAG_ENDOFFRAME|OMX_BUFFERFLAG_CODECCONFIG);
    if (isInternal) {
        nFlags |= VFM_FLAG_INTERNALBUFFER;
        pBuffer->nFlags |= VFM_FLAG_INTERNALBUFFER;
    }

    mNeedMoreNal = 0;

    // add an empty byte to help SVA to detect end of NAL
    pBuffer->pBuffer[pBuffer->nOffset+nFilledLen]=0;

    // Patch has been added to the SRC
    // more over if not last Buffer then add a second 0 ,
    // this is needed to pass ERC streams but this isnot really explained
    // concerned ERC are for > ERC_101
    if ((nFlags & OMX_BUFFERFLAG_EOS)==0)
    {
        nFilledLen++;
        pBuffer->pBuffer[pBuffer->nOffset+nFilledLen]=0;
    }

    // create the nal, and add it to the queue of buffers that are being processed
    t_bit_buffer *pNal = (t_bit_buffer *)mInputSet.add(pBuffer);  // store this buffer in the dequeued list
    pNal->addr  = (OMX_U8 *)pBuffer->pBuffer + pBuffer->nOffset;
    pNal->os    = 8;
    pNal->start = pNal->addr;
    pNal->end   = (t_uint8*) ((t_uint32)(pNal->addr) + nFilledLen);
    pNal->inuse = 0;
    pNal->next  = 0;

#ifndef OMX_SKIP64BIT
    t_uint32 nTimeStampH = (pBuffer->nTimeStamp >> 32) & 0xFFFFFFFF;
    t_uint32 nTimeStampL = (pBuffer->nTimeStamp      ) & 0xFFFFFFFF;
#else
    t_uint32 nTimeStampL = pBuffer->nTimeStamp.nLowPart;
    t_uint32 nTimeStampH = pBuffer->nTimeStamp.nHighPart;
#endif

    if (nFlags & OMX_BUFFERFLAG_EOS) {
        mSendLastNal = true;
    }
    mLastNalFlag = nFlags;

    // MMHwBuffer_EXT::DisplayState();  // Debug of MMHwBuffer

    // NMF_LOG("Input Flags=%x\n", pBuffer->nFlags);
    nal.nal(pNal, &mSei, nTimeStampH, nTimeStampL, nFlags);
}

EXPORT_C void METH(process_special_inputbuffer)(OMX_BUFFERHEADERTYPE *pBuffer)
{
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_PROCESS_INPUT,
        NMF_LOG("ddep::process_special_inputbuffer(): Buffer of size %d (isInternal=%d)\n", pBuffer->nFilledLen));

    // Sanity check that should never be activated, that is why they are fatal errors
    h264dec_assert((pBuffer->nFilledLen==0), __LINE__, OMX_TRUE);

    if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS) {
        mNeedMoreNal = false;
        mSendLastNal = false;
        nal.nal(0, &mSei, 0, 0, mLastNalFlag | pBuffer->nFlags);
    } else {
        // nothing to do. Only return the buffer
        scheduleProcessEvent();
    }

    OMX_BUFFERHEADERTYPE *pBufferPort = mPorts[0].getBuffer(0);
    h264dec_assert((pBuffer == pBufferPort), __LINE__, OMX_TRUE);
    mPorts[0].dequeueAndReturnBuffer();
}

EXPORT_C void METH(process_outputbuffer)(OMX_BUFFERHEADERTYPE *pBuffer, OMX_BOOL isInternal)
{
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_PROCESS_OUTPUT,
        NMF_LOG("ddep::process_outputbuffer(): Buffer (isInternal=%d)\n", isInternal));

    pBuffer->nOffset = 0;
    pBuffer->nFlags = 0;
    if (isInternal) {
        pBuffer->nFlags |= VFM_FLAG_INTERNALBUFFER;
    }

    mOutputSet.add(pBuffer, pBuffer->pBuffer);                              // store this buffer in the dequeued list
    output_buffer.output_buffer(pBuffer->pBuffer, pBuffer->nAllocLen);      // send the output buffer to the DPB
}


void METH(nal_handshake)(t_handshake mode, t_bit_buffer *p)
{
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_INPUT_ACK, NMF_LOG("nal_ctl_nal_handshake(): mode=%d, pointer=0x%x\n", mode, p));
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_INPUT_ACK, dump_port_status("nal_ctl_nal_handshake()", 0));
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_INPUT_ACK, dump_port_status("nal_ctl_nal_handshake()", 1));
    addPerformance((H264DEC_DEBUG & H264DEC_DEBUG_STATISTICS), OMX_FALSE);

    // Update status if more nals are required or not
    if (is_codec_flushing()) {
        h264dec_assert((!mNeedMoreNal), __LINE__, OMX_TRUE);
        if (mode==XON) {
            // even if we are flushing, older commands could be in the fifo
            // so we are overriding XON not to send any new command to the parser
            mode = XOFF;
        }
    }

    // Free buffers when possible
    while (p) {
        OMX_BUFFERHEADERTYPE *pBuffer = mInputSet.getBufferHeader(p);
        returnBufferDDep(0, pBuffer);
        p=(t_bit_buffer*) p->next;
    }

    switch (mode) {
    case XON:
        mNeedMoreNal = true;
        break;
    case FLUSH_ACK:
        mNeedMoreNal = false;
        VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_FLUSHES, NMF_LOG("nal_ctl_nal_handshake(): call codec_ack_flushing(0)\n"));
        codec_ack_flushing(0);
        codec_ack_flushingspec(0);
        break;
    case PAUSE_ACK:
        VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_PAUSE, NMF_LOG("nal_ctl_nal_handshake(): call codec_ack_pause()\n"));
        codec_ack_pause();
        break;
    default:
        break;
    }

    if (mNeedMoreNal) {
        if (mSendLastNal) {
            mNeedMoreNal = false;
            mSendLastNal = false;
            nal.nal(0, &mSei, 0, 0, mLastNalFlag);
        } else {
            scheduleProcessEvent();
        }
    }
}

void METH(picture)(t_picture_output valid, t_uint16 *p_fb, t_uint32 nFlags, t_frameinfo info)
{
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_OUTPUT_ACK, NMF_LOG("picture(): valid=%d, p_fb=0x%x, nFlags=%x\n", valid, p_fb, nFlags));
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_OUTPUT_ACK, dump_port_status("picture()", 0));
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_OUTPUT_ACK, dump_port_status("picture()", 1));
    nFlags &= ~(VFM_FLAG_INTERNALBUFFER);
    switch (valid) {
    case INVALID_PIC_FLUSH_ACK:
        VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_FLUSHES, NMF_LOG("picture(): call codec_ack_flushing(1)\n"));
        addPerformance((H264DEC_DEBUG & H264DEC_DEBUG_STATISTICS), OMX_FALSE);
        codec_ack_flushing(1);
        codec_ack_flushingspec(1);
        break;

    case INVALID_PIC:
        if (p_fb) {
            // this is a flush
            VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_FLUSHES, NMF_LOG("picture(): Received an invalid pic\n"));
            addPerformance((H264DEC_DEBUG & H264DEC_DEBUG_STATISTICS), OMX_FALSE);
            h264dec_assert((is_codec_flushing() || (nFlags & OMX_BUFFERFLAG_EOS)), __LINE__, OMX_TRUE);
            h264dec_assert((!is_codec_flushing() || !(nFlags & OMX_BUFFERFLAG_EOS)), __LINE__, OMX_TRUE);
            OMX_BUFFERHEADERTYPE *pBuffer = mOutputSet.getBufferHeader(p_fb);
            pBuffer->nFilledLen = 0;
            pBuffer->nFlags = nFlags;
            returnBufferDDep(1, pBuffer);
            // dump_port_status("INVALID_PIC while flushing: ", 0);
            // dump_port_status("INVALID_PIC while flushing: ", 1);

            if (nFlags & OMX_BUFFERFLAG_EOS) {
                // Send the EOS
                proxy.eventHandler(OMX_EventBufferFlag, 1, OMX_BUFFERFLAG_EOS);
            }
        } else {
            h264dec_assert((!(nFlags & OMX_BUFFERFLAG_EOS)), __LINE__, OMX_TRUE);
            check_picinfo(info);
        }

        break;

    case VALID_PIC:
        {
            /*
            if (flushing) {
                NMF_LOG("valid pic while flushing\n");
            }
            */
            addPerformance((H264DEC_DEBUG & H264DEC_DEBUG_STATISTICS), OMX_TRUE);
            displayPerformance((H264DEC_DEBUG & H264DEC_DEBUG_STATISTICS));

            // check_picinfo(info);
            OMX_BUFFERHEADERTYPE *pBufferToFill = mOutputSet.getBufferHeader(p_fb);

            if ((nFlags & OMX_BUFFERFLAG_DECODEONLY) && (!(nFlags & OMX_BUFFERFLAG_EOS)) && (!is_codec_flushing())) {
                // Decode only:
                //    if we have EOS, this should be passed to the next component in.
                //    order to pass the EOS flag. Next component will decide not to display it
                //    Otherwise the buffer can be sent again to the codec
                process_outputbuffer(pBufferToFill, OMX_FALSE);
            } else {
                // Set the flags of the buffer and time stamps
                t_uint32 size = info.common_frameinfo.pic_width * info.common_frameinfo.pic_height;
                if (pBufferToFill->nFlags & VFM_FLAG_INTERNALBUFFER) {
                    nFlags |= VFM_FLAG_INTERNALBUFFER;
                }
                pBufferToFill->nFlags = nFlags | OMX_BUFFERFLAG_ENDOFFRAME;
                pBufferToFill->nFilledLen = (size*3)/2;

                // Update timestamps
#ifndef OMX_SKIP64BIT
                pBufferToFill->nTimeStamp = (((OMX_S64)info.common_frameinfo.nTimeStampH) << 32) | (((OMX_S64)info.common_frameinfo.nTimeStampL) & 0xFFFFFFFF);
                //h264dec_assert((pBufferToFill->nTimeStamp>=mLastTimestamp), __LINE__, OMX_FALSE);
                mLastTimestamp = pBufferToFill->nTimeStamp;
#else
                pBufferToFill->nTimeStamp = { info.nTimeStampL, info.nTimeStampH };
#endif

                // return the buffer to the proxy, but do not remove it from the VFM_Memory
                // as it can be used by the decoder as a reference
                returnBufferDDep(1, pBufferToFill);

                // FIXME: check size
                // FIXME: return profile / level to application
                // FIXME: what about DPB size

                // Send event handler that the EOS flag has been received
                if (nFlags & OMX_BUFFERFLAG_EOS) {
                    // Send the memory status
                    VFM_MemoryStatus MemoryStatus;
                    VFM_GetMemoryListStatus(mCtxtMemory, &MemoryStatus);
                    get_config.get_memory(MemoryStatus);

                    // Send the EOS
                    proxy.eventHandler(OMX_EventBufferFlag, 1, OMX_BUFFERFLAG_EOS);
                }
            }
        }
        break;

    default:
        h264dec_assert((0==1), __LINE__, OMX_TRUE);
        break;
    }
}

void METH(check_picinfo)(t_frameinfo info)
{
    // Raise the ErrorMap event if needed
    raiseEventErrorMap((Common_Error_Map *)mFrameInfo.specific_frameinfo.pErrorMap);

    if ((info.common_frameinfo.pic_width!=mFrameInfo.common_frameinfo.pic_width) || (info.common_frameinfo.pic_height!=mFrameInfo.common_frameinfo.pic_height)) {
        // NMF_LOG("h264dec_arm_nmf_ddep::check_picinfo()  -  size of the frame is not correct\n");
        mPendingPicSize = true;
        if (info.common_frameinfo.pic_height > 1088) {
            // The error is not performed here, but in the proxy
        }
        updatePower();
    }
    info.specific_frameinfo.bThumbnailGeneration = mFrameInfo.specific_frameinfo.bThumbnailGeneration;
    info.specific_frameinfo.nParallelDeblocking = mFrameInfo.specific_frameinfo.nParallelDeblocking;
    info.specific_frameinfo.xFramerate = mFrameInfo.specific_frameinfo.xFramerate;
    info.specific_frameinfo.pErrorMap = mFrameInfo.specific_frameinfo.pErrorMap;
    info.common_frameinfo.nSupportedExtension = mFrameInfo.common_frameinfo.nSupportedExtension;
    info.specific_frameinfo.restrictMaxLevel = mFrameInfo.specific_frameinfo.restrictMaxLevel;

    // check common differences
    OMX_BOOL has_changed;
    has_changed = VFM_frameInfoDifferent(&info.common_frameinfo, &mFrameInfo.common_frameinfo);

// +CR324558 CHANGE START FOR
    if(info.common_frameinfo.dpb_size != mFrameInfo.common_frameinfo.dpb_size ) {
        OstTraceFiltInst0( TRACE_FLOW ,"\nDDEP_checkpicinfo::differnt dpb size.");
        OstTraceFiltInst2( TRACE_FLOW ,"\nDDep_check_picinfo::info.common_frameinfo.dpb_size=%d,mFrameInfo.common_frameinfo.dpb_size=%d",info.common_frameinfo.dpb_size,mFrameInfo.common_frameinfo.dpb_size);
        mPendingDPBSize = true;
        has_changed = OMX_TRUE;
    }
// -CR324558 CHANGE END OF
    // check specific differences
    // no need to check for nParallelDeblocking as not returned by the codec
    // no need to check for error map as not returned by the codec

    // store the current configuration
    mFrameInfo = info;

    // send the current configuration if a changed occured
    if (has_changed) {
        get_config.get_config(mFrameInfo);
    }
}

void METH(dumpMemoryStatus)(void) const
{
    /*
    t_uint32 curent_size=0, max_size=0, current_nb_item=0, max_nb_item=0;
    VFM_MemoryStatus MemoryStatus;
    VFM_GetMemoryListStatus(mCtxtMemory, &MemoryStatus);

    NMF_LOG("---------------------------------------------------------------------------------------\n");
    for (int i=0; i<VFM_MEM_LAST; i++) {
        NMF_LOG("%20s:  Current size = %6d bytes  |  Max Size = %6d bytes  |  Current Nb Item = %6d   |  Max Nb Item = %6d\n",
            VFM_MemTypeStringify((VFM_Mem_Type)i),
            MemoryStatus.mCurrentSize[i],
            MemoryStatus.mMaxSize[i],
            MemoryStatus.mCurrentNbItem[i],
            MemoryStatus.mMaxNbItem[i]);
        curent_size += MemoryStatus.mCurrentSize[i];
        max_size += MemoryStatus.mMaxSize[i];
        current_nb_item += MemoryStatus.mCurrentNbItem[i];
        max_nb_item += MemoryStatus.mMaxNbItem[i];
    }
    NMF_LOG("%20s:  Current size = %6d bytes  |  Max Size = %6d bytes  |  Current Nb Item = %6d   |  Max Nb Item = %6d\n",
        "Summary", curent_size, max_size, current_nb_item, max_nb_item);
    NMF_LOG("---------------------------------------------------------------------------------------\n");
    */
}

void METH(set_configuration)(void *omx_ptr)
{
	pOMXHandle = omx_ptr;
	initOMXHandles((OMX_PTR)pOMXHandle);
}

void METH(set_config)(t_uint32 domainDdrId, t_uint32 domainEsramId, t_frameinfo frame_info)
{
    setDomainId(domainDdrId, domainEsramId);
    mFrameInfo = frame_info;
    t_uint16 is1080pCapable = (t_uint16)isSoC1080pCapableDdep();
    OstTraceFiltInst1( TRACE_FLOW ,"\nDDEP_set_config::is1080pCapable=%d",is1080pCapable);

    // Set the DVFS and DDR value
    updatePower();

    // allocate internal buffers for the input
    // no internal buffers are used on the output
    if (mInputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
        // prepare the internal buffer to a small size, to optimize the ddr size in case they are not used
#ifdef _CACHE_OPT_
        OMX_ERRORTYPE error = mInputInternal.alloc(&mInputSet, HAMAC_PIPE_SIZE+1+NB_BUFFERS_INPUT_DDEP, SAFETYBYTES, OMX_TRUE);
#else
        OMX_ERRORTYPE error = mInputInternal.alloc(&mInputSet, HAMAC_PIPE_SIZE+1+NB_BUFFERS_INPUT_DDEP, SAFETYBYTES, OMX_FALSE);
#endif
        // FIXME: should be in error recovery
        h264dec_assert((error==OMX_ErrorNone), __LINE__, OMX_TRUE);
    }

    if (mOutputBufferMode & VFMDDEP_BUFFERMODE_COPY) {
        // Size of a frame. Need reallocation if size changes
        t_uint32 size = ((mFrameInfo.common_frameinfo.pic_width+15)& 0xFFF0U)
                        * ((mFrameInfo.common_frameinfo.pic_height+15)& 0xFFF0U);
        size = (size*3)/2;
        t_uint32 buffer_min = (12582912 / size);
        if (buffer_min<1) {
            buffer_min = 1;
        } else if (buffer_min>16) {
            buffer_min = 16;
        }

        buffer_min += (HAMAC_PIPE_SIZE+1);      // number of parallel processing
        buffer_min += NB_BUFFERS_OUTPUT_DDEP;   // received in the ddep

        OMX_ERRORTYPE error = mOutputInternal.alloc(&mOutputSet, buffer_min, size, OMX_FALSE);
        h264dec_assert((error==OMX_ErrorNone), __LINE__, OMX_TRUE);
    }

    if (!mFsmInitCalled) {
        mPendingSendConfig = true;
    } else {
        mPendingSendConfig = false;
        t_uint16 parallelDeblocking = frame_info.specific_frameinfo.nParallelDeblocking;
        h264dec_assert((parallelDeblocking==0 || parallelDeblocking==1 || parallelDeblocking==3), __LINE__, OMX_TRUE);
        addPendingCommand();
        set_decoder_parameter.set_nParallelDeblocking(true, parallelDeblocking);
// +CR324558 CHANGE START FOR
        addPendingCommand();
        set_decoder_parameter.set_picsize(true, frame_info.common_frameinfo.pic_width, frame_info.common_frameinfo.pic_height,
                                                                                     frame_info.common_frameinfo.dpb_size);
// -CR324558 CHANGE END OF
        addPendingCommand();
        set_decoder_parameter.set_error_map(true, frame_info.specific_frameinfo.pErrorMap);
        addPendingCommand();
        set_decoder_parameter.set_SupportedExtension(true, frame_info.common_frameinfo.nSupportedExtension);
        addPendingCommand();
        set_decoder_parameter.set_Framerate(true, frame_info.specific_frameinfo.xFramerate);
        addPendingCommand();
        set_decoder_parameter.set_Thumbnail(true, frame_info.specific_frameinfo.bThumbnailGeneration);
        addPendingCommand();
        set_decoder_parameter.set_ImmediateRelease(true, frame_info.specific_frameinfo.bImmediateRelease);
        /* CR 399075 immediate release I frame */
        frame_info.specific_frameinfo.bImmediateRelease = OMX_FALSE;

        addPendingCommand();
        set_decoder_parameter.set_par(true, frame_info.common_frameinfo.nPixelAspectRatioWidth, frame_info.common_frameinfo.nPixelAspectRatioHeight);
        addPendingCommand();
        set_decoder_parameter.set_cropping(
            true,
            frame_info.common_frameinfo.frame_cropping_flag,
            frame_info.common_frameinfo.frame_crop_right, frame_info.common_frameinfo.frame_crop_left,
            frame_info.common_frameinfo.frame_crop_top, frame_info.common_frameinfo.frame_crop_bottom);
        addPendingCommand();
        set_decoder_parameter.set_colorspace(true, frame_info.common_frameinfo.nColorPrimary);
        // set_decoder_parameter.set_sva_bypass(1);     // Uncomment this line to bypass the dsp
        addPendingCommand();
        //set_decoder_parameter.set_RestrictLevel(true, frame_info.specific_frameinfo.restrictMaxLevel);
        set_decoder_parameter.set_RestrictLevel(true, frame_info.specific_frameinfo.restrictMaxLevel,is1080pCapable);
        // Check colorspace validity because of hardcoded values in compute_colorspace()
        h264dec_assert((4 == OMX_ColorPrimaryBT709LimitedRange), __LINE__, OMX_TRUE);
        h264dec_assert((2 == OMX_ColorPrimaryBT601LimitedRange), __LINE__, OMX_TRUE);
        h264dec_assert((3 == OMX_ColorPrimaryBT709FullRange), __LINE__, OMX_TRUE);
        h264dec_assert((1 == OMX_ColorPrimaryBT601FullRange), __LINE__, OMX_TRUE);
   }
}


void METH(flush_command_to_codec)()
{
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_FLUSHES, dump_port_status("flush_command_to_codec()", 0));
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_FLUSHES, dump_port_status("flush_command_to_codec()", 1));

    if (!mFsmInitCalled) {
        // as fsmInit has not been called, flush is ok
        codec_ack_flushing(0);
        codec_ack_flushing(1);
    } else {
        mNeedMoreNal = 0;
        mH264DecMaskFlushingAck = 0;
        nal.input_decoder_command(COMMAND_FLUSH);
    }
}

void METH(codec_ack_flushingspec)(OMX_U32 nPortIndex)
{
// +CR324558 CHANGE START FOR
   if (mPendingPicSize || mPendingDPBSize) {
        mH264DecMaskFlushingAck |= (1<<nPortIndex);

       if (mH264DecMaskFlushingAck==0x3) {
           OstTraceFiltInst1( TRACE_FLOW ,"\nDDEP_codec_ack_flushingspec::mFrameInfo.common_frameinfo.dpb_size=%d",mFrameInfo.common_frameinfo.dpb_size);

           addPendingCommand();
           set_decoder_parameter.set_picsize(true, mFrameInfo.common_frameinfo.pic_width,
                                          mFrameInfo.common_frameinfo.pic_height,
                                          mFrameInfo.common_frameinfo.dpb_size);
           mPendingPicSize = false;
           mPendingDPBSize = false;
           mH264DecMaskFlushingAck = 0;
      }
    }

// -CR324558 CHANGE END OF
}

void METH(pause_command_to_codec)()
{
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_PAUSE, dump_port_status("pause_command_to_codec()", 0));
    VFMDDEP_TRACE(H264DEC_DEBUG & H264DEC_DEBUG_PAUSE, dump_port_status("pause_command_to_codec()", 1));
    nal.input_decoder_command(COMMAND_PAUSE);
}

void METH(addPendingCommand)()
{
    if (mNbPendingCommands==0) {
        // lock the mutex as we now wait for  feedback
        mMutexPendingCommand->MutexLock();
    }
    mNbPendingCommands++;
}

/* error=0 if OK, 1 if an error occurred */
void METH(pendingCommandAck)(t_ack_error_codes error)
{
    h264dec_assert((mNbPendingCommands>0), __LINE__, OMX_TRUE);
    mNbPendingCommands--;
    switch(error) {
    case COMMAND_COMPLETED:
        break;
    case INSUFFICIENT_INTERNAL_MEMORY:
        sendProxyEvent(OMX_EventError, (OMX_U32)OMX_ErrorInsufficientResources, 0);
        break;
    case INTERNAL_MEMORY_ALLOCATED:
        if(mFsmInitDelayed) {
            mFsmInitDelayed = false;
            delayed_fsmInit(mInitFSM,false);
        }
        break;
    }
    if (mNbPendingCommands==0) {
        // unlock the mutex as we do not wait for any other feedback
        mMutexPendingCommand->MutexUnlock();
    }
}

void METH(send_vfm_memory)(void *memory_ctxt)
{
    addPendingCommand();
    set_decoder_parameter.set_memory_ctxt(true, memory_ctxt);
}

void METH(sendProxyEvent)(OMX_IN OMX_EVENTTYPE eEvent,  OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2)
{
    proxy.eventHandler(eEvent, nData1, nData2);
}

EXPORT_C OMX_BOOL METH(specificActivateInputFastPath)(OMX_BUFFERHEADERTYPE *pBuffer) const
{
    // Does this buffer include extra bytes?
    // Plus we are adding in HamacFillBistreamIn() a start code (4 chars)
    // Plus 1 safety bytes ==> leads to 7
    if (pBuffer->nOffset+pBuffer->nFilledLen+SAFETYBYTES>pBuffer->nAllocLen) {
        return OMX_FALSE;
    }

    // is it a readonly buffer?
    if (pBuffer->nFlags & OMX_PORTTUNNELFLAG_READONLY) {
        return OMX_FALSE;
    }

    return OMX_TRUE;
}

EXPORT_C OMX_BOOL METH(isSpecialCase)(OMX_BUFFERHEADERTYPE *pBuffer) const
{
    return (pBuffer->nFilledLen==0 ? OMX_TRUE : OMX_FALSE);
}

void METH(updatePower)()
{
    if (FINAL_POWER_MANAGEMENT==OMX_TRUE) {
        t_uint32 size = ((mFrameInfo.common_frameinfo.pic_width+15)& 0xFFF0U)
            * ((mFrameInfo.common_frameinfo.pic_height+15)& 0xFFF0U) / (16*16);

        if (size <= ((1280*720)/(16*16))) {
            getPower()->set(VFM_PowerManagement::VFM_ResDVFS_Half, /* +ER335583 */ VFM_PowerManagement::VFM_ResDDR_Half, /* -ER335583 */ VFM_PowerManagement::VFM_ResLatency_Standard);
        } else {
            getPower()->set(VFM_PowerManagement::VFM_ResDVFS_Full, /* +ER335583 */ VFM_PowerManagement::VFM_ResDDR_Full, /* -ER335583 */ VFM_PowerManagement::VFM_ResLatency_None);
        }
    }
}

void METH(error_recovery)()
{
    error_recovery_out.error_recovery();
}


void METH(fatal_decoding_error)(t_uint16 error_code)
{
  switch(error_code)
  {
    case 0:
      // Bitstream not supported or fatal parsing error
      OstTraceInt1(TRACE_ERROR, "H264Dec: DDep: fatal_decoding_error VIDEOTRACE Error line %d: Bitstream not supported or fatal parsing error\n", __LINE__);
      proxy.eventHandler(OMX_EventError, OMX_ErrorFormatNotDetected, 0);
      break;
    case 1:
      // ESRAM Allocation failed
      OstTraceInt1(TRACE_ERROR, "H264Dec: DDep: fatal_decoding_error VIDEOTRACE Error line %d: ESRAM Allocation failed\n", __LINE__);
      proxy.eventHandler(OMX_EventError, OMX_ErrorDynamicResourcesUnavailable, 0);
      break;
    default:
      OstTraceInt2(TRACE_ERROR, "H264Dec: DDep: fatal_decoding_error VIDEOTRACE Error line %d: Undefined error %d\n", __LINE__, error_code);
      proxy.eventHandler(OMX_EventError, OMX_ErrorUndefined, 0);
      break;
  }
}

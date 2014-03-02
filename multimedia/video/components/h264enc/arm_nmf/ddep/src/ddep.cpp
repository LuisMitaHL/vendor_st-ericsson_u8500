/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "h264enc/arm_nmf/ddep.nmf"
#include "VFM_Memory.h"
#include "VFM_Types.h"


#include "nmf_lib.h" // for memset

#ifdef __SYMBIAN32__
#include <openmax/il/shai/OMX_Symbian_IVCommonExt.h>
#else
#include "OMX_Symbian_IVCommonExt_Ste.h"
#endif


// List of improvements (to be discussed of course!)
// - construct() with arguments being the number of buffers in ports

// FIXME: no deinit of list / queue (or is it done automatically?)
// TODO: provide timestamps to the codec
// TODO: implement buffers flushing

//#define _H264ENC_DDEP_TRACES_

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
        #include "video_components_h264enc_arm_nmf_h264enc_ddep_src_ddepTraces.h"
#endif




h264enc_arm_nmf_ddep::h264enc_arm_nmf_ddep() :
    VFM_nmf_ddep(VFMDDEP_BUFFERMODE_FASTPATH, VFMDDEP_BUFFERMODE_FASTPATH, 0, 0, mPorts, OMX_FALSE),
    mEncodingStatus(0),
    mIsFlushRequested(false),
    mIsPauseRequested(false),
    mGenerateSpsPps(true),
    mFrameNb(0),
    mState(StateUnloaded),
    mNbPendingCommands(0)
{
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] > h264enc_arm_nmf_ddep::h264enc_arm_nmf_ddep");

    memset((t_uint8*)&mFrameInfo, 0, sizeof(mFrameInfo));
    memset((t_uint8*)&mSequenceHeader, 0, sizeof(mSequenceHeader));
    osWidth=0;
    osHeight=0;
    ptrParam = 0;
    omxilosalservices::OmxILOsalMutex::MutexCreate(mMutexPendingCommand);

    // Set DVFS values
    updateDVFS();
//+ ER335583
    updateDDR();
//- ER335583
    setDVFS();
//+ ER335583
    setDDR();
//- ER335583
    VFM_PowerManagement::setLatency(this, VFM_PowerManagement::VFM_ResLatency_None);

    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < h264enc_arm_nmf_ddep::h264enc_arm_nmf_ddep");
}

h264enc_arm_nmf_ddep::~h264enc_arm_nmf_ddep()
{
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] > h264enc_arm_nmf_ddep::~h264enc_arm_nmf_ddep");

    // Unset DVFS and DDR values
    unsetDVFS();
//+ ER335583
    unsetDDR();
//- ER335583
    VFM_PowerManagement::resetLatency(this);

    omxilosalservices::OmxILOsalMutex::MutexFree(mMutexPendingCommand);

    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < h264enc_arm_nmf_ddep::~h264enc_arm_nmf_ddep");
}

void METH(start)()
{
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] > start");

    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < start");
}

void METH(stop)()
{
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] > stop");

    addPendingCommand();
    init_encoder.close();
    waitForCommandCompletion();

    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] > stop");
}

void METH(waitForCommandCompletion)()
{
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] > waitForCommandCompletion");

    // lock and unlock the mutex to ensure no more activites is in the codec when stop ends
    mMutexPendingCommand->MutexLock();
    if (mNbPendingCommands!=0)
    {
        OstTraceInt0(TRACE_ERROR, "[DDEP] mNbPendingCommands!=0");
        NMF_PANIC("[DDEP] mNbPendingCommands!=0");
    }
    mMutexPendingCommand->MutexUnlock();

    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < waitForCommandCompletion");
}

void METH(addPendingCommand)()
{
    OstTraceInt1(TRACE_API, "[ARMNMF DDEP] > addPendingCommand - mNbPendingCommands:%d", mNbPendingCommands);

    if (mNbPendingCommands==0) {
        // lock the mutex as we now wait for  feedback
        mMutexPendingCommand->MutexLock();
    }
    mNbPendingCommands++;

    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < addPendingCommand");
}

/* error=0 if OK, 1 if an error occurred */
void METH(pendingCommandAck)(t_uint32 error)
{
    OstTraceInt2(TRACE_API, "[ARMNMF DDEP] > pendingCommandAck - error=%d - mNbPendingCommands:%d",error, mNbPendingCommands);

    if (mNbPendingCommands<=0)
    {
        OstTraceInt0(TRACE_ERROR, "[DDEP] mNbPendingCommands<=0");
        NMF_PANIC("[DDEP] mNbPendingCommands<=0");
    }

    if (error) {
        // return an event that
        sendProxyEvent(OMX_EventError, (OMX_U32)OMX_ErrorUndefined, 0);
    }

    nextState();

    mNbPendingCommands--;
    if (mNbPendingCommands==0) {
        // unlock the mutex as we do not wait for any other feedback
        mMutexPendingCommand->MutexUnlock();
    }

    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < pendingCommandAck");
}


void METH(nextState)()
{
    OstTraceInt1(TRACE_API, "[ARMNMF DDEP] > nextState - currentState:%d", mState);

    switch(mState)
    {
    case StateInitPhase1:
        goToInit2();
        break;
    case StateInitPhase2:
        goToInit3();
        break;
    case StateInitPhase3:
        goToLoaded();
        break;
    case StateLoadedSpsPpsRequest:
        fill_sps_pps_buffers();
        break;
    case StateUnloaded:
    case StateLoaded:
    case StateLoadedSpsPpsReady:
    default:
        //panic here
        break;
    }

    OstTraceInt1(TRACE_API, "[ARMNMF DDEP] < nextState - newState:%d", mState);
}


void METH(set_param)(void *ptrParamConfig)
{
	ptrParam = ptrParamConfig;
	OstTraceInt1(TRACE_API, "[ARMNMF DDEP] > set_param value of ptrParam : 0x%x \n",(OMX_U32)ptrParam);
}

void METH(set_config)(t_uint32 domainDdrId, t_uint32 domainEsramId, t_frameinfo frame_info)
{
    OstTraceInt2(TRACE_API, "[ARMNMF DDEP] > set_config (domainDdrId=%d - domainEsramId=%d)", domainDdrId, domainEsramId);

    setDomainId(domainDdrId, domainEsramId);

    if ( (frame_info.specific_frameinfo.Bitrate != mFrameInfo.specific_frameinfo.Bitrate) ||
         (frame_info.specific_frameinfo.FrameRate != mFrameInfo.specific_frameinfo.FrameRate) )
    {
        OstTraceInt0(TRACE_FLOW, "[ARMNMF DDEP] bitrate and/or framerate changed => SPS/PPS to be regenerated");
        mGenerateSpsPps = true;
    }

    mFrameInfo = frame_info;

    switch(mFrameInfo.common_frameinfo.eLevel)
    {
        case OMX_VIDEO_AVCLevel1:
            mFrameInfo.specific_frameinfo.EncoderLevel = 10;
            break;
        case OMX_VIDEO_AVCLevel1b:
            mFrameInfo.specific_frameinfo.EncoderLevel = 101;
            break;
        case OMX_VIDEO_AVCLevel11:
            mFrameInfo.specific_frameinfo.EncoderLevel = 11;
            break;
        case OMX_VIDEO_AVCLevel12:
            mFrameInfo.specific_frameinfo.EncoderLevel = 12;
            break;
        case OMX_VIDEO_AVCLevel13:
            mFrameInfo.specific_frameinfo.EncoderLevel = 13;
            break;
        case OMX_VIDEO_AVCLevel2:
            mFrameInfo.specific_frameinfo.EncoderLevel = 20;
            break;
        case OMX_VIDEO_AVCLevel21:
            mFrameInfo.specific_frameinfo.EncoderLevel = 21;
            break;
        case OMX_VIDEO_AVCLevel22:
            mFrameInfo.specific_frameinfo.EncoderLevel = 22;
            break;
        case OMX_VIDEO_AVCLevel3:
            mFrameInfo.specific_frameinfo.EncoderLevel = 30;
            break;
        case OMX_VIDEO_AVCLevel31:
            mFrameInfo.specific_frameinfo.EncoderLevel = 31;
            break;
        case OMX_VIDEO_AVCLevel32:
            mFrameInfo.specific_frameinfo.EncoderLevel = 32;
            break;
        case OMX_VIDEO_AVCLevel4:
            mFrameInfo.specific_frameinfo.EncoderLevel = 40;
            break;
        default:
            OstTraceInt0(TRACE_ERROR, "[DDEP] unsupported level");
            NMF_PANIC("[DDEP] unsupported level");
    }


    mInputSet.setHeaderDataChanging((OMX_BOOL)mFrameInfo.specific_frameinfo.HeaderDataChanging);
    mOutputSet.setHeaderDataChanging((OMX_BOOL)mFrameInfo.specific_frameinfo.HeaderDataChanging);

    updateDVFS();
//+ ER335583
    updateDDR();
//- ER335583


    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < set_config");
}

void METH(set_VideoStabParam)(OMX_U32 OSWidth, OMX_U32 OSHeight)
{
    osWidth = OSWidth;
    osHeight = OSHeight;
}

void METH(fsmInit)(fsmInit_t initFSM)
{
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] > fsmInit");
    mOk = true;

    mPortsDisabled    = initFSM.portsDisabled;
    mPortsStdTunneled = initFSM.portsTunneled;
    // trace initialization
    if (initFSM.traceInfoAddr)
    {
        setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
        addPendingCommand();
        encoder_trace_init.initTraceInfo(initFSM.traceInfoAddr, initFSM.id1); // to convey trace structs/config to lower ARM-NMF components
    }
    initOMXHandles(0);
#ifdef NO_HAMAC
    mOk = initVFMMemory(1,0);
    OstTraceInt1(TRACE_FLOW, "[ARMNMF DDEP] >abt to call initParamConfig with value : 0x%x \n",(OMX_U32)ptrParam );
    initParamConfig(ptrParam);
#else
    OstTraceInt0(TRACE_API,"Abt to call iniVFMMemory \n");
    mOk = initVFMMemory(0,0);
	OstTraceInt1(TRACE_API, "[ARMNMF DDEP] >abt to call initParamConfig with value : 0x%x \n",(OMX_U32)ptrParam );
    initParamConfig(ptrParam);
#endif

    mState = StateInitPhase1;

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < fsmInit");
}

void METH(goToInit2)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > goToInit2");

    // --- Required by the FSM initialization
    // init(Direction, bufferSupplier, isHWport, sharingPort, OMX_BUFFERHEADERTYPE**, bufferCount, nmfitf, portIdx, isDisabled, componentOwner);
    // we are not buffer supplier as the application is ==> sharingPort=0 and OMX_BUFFERHEADERTYPE**=0
    // to be checked in tunneling support
    mPorts[0].init(InputPort,  false, true, 0, 0, mMaxInput,  &inputport,  0, (mPortsDisabled & (1<<0)), (mPortsStdTunneled & (1<<0)), this);
    mPorts[1].init(OutputPort, false, true, 0, 0, mMaxOutput, &outputport, 1, (mPortsDisabled & (1<<1)), (mPortsStdTunneled & (1<<1)), this);
    //init(2, mPorts, &proxy, &me, !mOk);

    OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] init sequence header");
    addPendingCommand();
    get_headers.get_headers(&mFrameInfo, mSequenceHeader.data, &mSequenceHeader.size,
                                         0, 0, 1);

    mState = StateInitPhase2;

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < goToInit2");
}

void METH(goToInit3)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > goToInit3");

    //assert mSequenceHeader.size!=0
    OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] SPS-PPS length %d",mSequenceHeader.size);
    for (int i=0; i<mSequenceHeader.size; i++)
    {
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] sequence header %02x",mSequenceHeader.data[i]);
    }

    mFrameNb = 0;

    mState = StateInitPhase3;

    //addPendingCommand();
    get_sps_pps.get_sps_pps(&mSequenceHeader);

    goToLoaded();

OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < goToInit3");
}

void METH(goToLoaded)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > goToLoaded");
    init(2, mPorts, &proxy, &me, !mOk);//this will give OMX_EventCmdComplete from loaded to idle
    mState = StateLoaded;

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < goToLoaded");
}


void METH(reset)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > reset");

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < reset");
}

void METH(process)()
{
    OstTraceFiltInst1(TRACE_API, "[ARMNMF DDEP] > process() - mState %d",mState);
    t_uint32 flag_tbd = 0;
    if (mState == StateLoadedSpsPpsReady)
    {
        OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] deque SPS-PPS");
        mPorts[1].dequeueAndReturnBuffer();
#ifndef H24ENC_SPS_PPS_IN_ONE_BUFFER
        mPorts[1].dequeueAndReturnBuffer();
#endif
        mFrameNb += 2;
        mState = StateLoaded;
    }

    if(mState != StateLoaded)
    {
            OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < process - state != StateReady");
        return;
    }

    if (!check_start())
    {
            OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < process - not ready to encode");
        return;
    }

    //mGenerateSpsPps ||= mFrameNb == (3 + 2);

    // process SPS/PPS
    if (mGenerateSpsPps)
    {
        fill_sps_pps_buffers_request();
        OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < process - waiting for SPS-PPS");
        return;
    }

    mEncodingStatus = 3; //0b11

    // extract next input buffer
    OMX_BUFFERHEADERTYPE* input_buf = mPorts[0].dequeueBuffer();

    OMX_BUFFERHEADERTYPE* output_buf = mPorts[1].dequeueBuffer();

    
    // propagate the nTimeStamp from input buffer to output buffer
    output_buf->nTimeStamp = input_buf->nTimeStamp;

    if((input_buf->nFilledLen == 0)&&(input_buf->nFlags&OMX_BUFFERFLAG_EOS))
    {
        output_buf->nFilledLen = 0;
        output_buf->nFlags = input_buf->nFlags;
        output_buf->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
        mEncodingStatus = 0;
        returnBufferDDep(1, output_buf);
        returnBufferDDep(0, input_buf);

        proxy.eventHandler(OMX_EventBufferFlag, 1, OMX_BUFFERFLAG_EOS);
        if(mIsFlushRequested)
            flush_command_to_codec();
        if(mIsPauseRequested)
            pause_command_to_codec();
        scheduleProcessEvent();
        return;
    }

	mInputSet.add(input_buf, input_buf->pBuffer);
    mOutputSet.add(output_buf, output_buf->pBuffer);
   
#ifndef OMX_SKIP64BIT
    mFrameInfo.common_frameinfo.nTimeStampH = (input_buf->nTimeStamp >> 32) & 0xFFFFFFFF;
    mFrameInfo.common_frameinfo.nTimeStampL = (input_buf->nTimeStamp      ) & 0xFFFFFFFF;
#else
    mFrameInfo.common_frameinfo.nTimeStampH = input_buf->nTimeStamp.nHighPart;
    mFrameInfo.common_frameinfo.nTimeStampL = input_buf->nTimeStamp.nLowPart;
#endif
    input_buf->nFilledLen = (osWidth * osHeight *3)/2 ;
    if((osWidth>mFrameInfo.common_frameinfo.pic_width)||(osHeight>mFrameInfo.common_frameinfo.pic_height))
    {
        OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] OverScan Width and Over Scan Height are more than Frame Width and Fram Height resptectively");
        flag_tbd =1;
    }

    if (input_buf->nFlags & OMX_BUFFERFLAG_EXTRADATA)
    {
        OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] Extradata Flag Enabled");
        processExtraData(input_buf,flag_tbd); // fills mFrameInfo with stabilization info
    }
    else if(flag_tbd)// Case When OMX_BUFFERFLAG_EXTRADATA is disabled
    {
        OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] Extradata Flag Disabled");
        mFrameInfo.specific_frameinfo.sfw = osWidth;
        mFrameInfo.specific_frameinfo.sfh = osHeight;
    }

    output_buf->nOffset = 0;

    // Todo: remove me => val source should set the timestamps in bufferheader
    // Fix for test appli: in case of BRC tests, nTimestamp must have non-null values
    // Won't work if "NB_frame > (0xFFFFFFFF*FrameRate)/1000000"
    // OK for the moment as we don't have tests with so many frames
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] timestamp %d",mFrameInfo.common_frameinfo.nTimeStampL);
    if ((mFrameInfo.common_frameinfo.nTimeStampH == 0) && (mFrameInfo.common_frameinfo.nTimeStampL == 0))
    {
        OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] No timestamps - calculate timestamp values");
        mFrameInfo.common_frameinfo.nTimeStampL = ((mFrameNb-2)*1000000)/(mFrameInfo.specific_frameinfo.FrameRate>>16);
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] new timestamp %d",mFrameInfo.common_frameinfo.nTimeStampL);
    }

        OstTraceFiltInst4(TRACE_FLOW, "[ARMNMF DDEP] now encoding IN: 0x%x (filled len: %d) OUT: 0x%x (alloc len: %d)", (unsigned int)input_buf->pBuffer, input_buf->nFilledLen, (unsigned int)output_buf->pBuffer, output_buf->nAllocLen);
    PrintParams();
    OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] mFrameInfo.specific_frameinfo.ForceIntra : %d",mFrameInfo.specific_frameinfo.ForceIntra);
    output_buffer.output_buffer(output_buf->pBuffer + output_buf->nOffset, output_buf->nAllocLen);
    input.input(input_buf->pBuffer + input_buf->nOffset, mFrameInfo, input_buf->nFlags);

    //+ code change for CR 361667
    mFrameInfo.specific_frameinfo.ForceIntra = 0;
    //- code change for CR 361667

    mFrameNb++;
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < process");
}

void METH(fill_sps_pps_buffers_request)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > fill_sps_pps_buffers_request");

    if (0) // mFrameNb == (3 + 2)
    {
        mFrameInfo.specific_frameinfo.Bitrate = 400000;
        mFrameInfo.specific_frameinfo.FrameRate = 30<<16;
    }

    OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Processing SPS-PPS (mPorts[1].queuedBufferCount():%d",mPorts[1].queuedBufferCount());

    if (mPorts[1].queuedBufferCount()<2)
    {
        OstTraceFiltInst0(H264ENC_TRACE_GROUP_DDEP,"[DDEP] < process (queuedbuffercount<2)");
        return;
    }

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] Processing SPS-PPS");

#ifdef H24ENC_SPS_PPS_IN_ONE_BUFFER
    mOutputBufSpsPps = mPorts[1].getBuffer(0);
    mOutputBufSpsPps->nOffset = 0;
    mSizeSpsPps = mOutputBufSpsPps->nAllocLen;
#else
    mOutputBufSps = mPorts[1].getBuffer(0);
    mOutputBufPps = mPorts[1].getBuffer(1);
    mOutputBufSps->nOffset = 0;
    mOutputBufPps->nOffset = 0;
    mSizeSps = mOutputBufSps->nAllocLen;
    mSizePps = mOutputBufPps->nAllocLen;
#endif

    PrintParams();

    OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] get_headers");
    addPendingCommand();
#ifdef H24ENC_SPS_PPS_IN_ONE_BUFFER
    get_headers.get_headers(&mFrameInfo, mOutputBufSpsPps->pBuffer+mOutputBufSpsPps->nOffset, &mSizeSpsPps,
                                         NULL, 0, 1);
#else
    get_headers.get_headers(&mFrameInfo, mOutputBufSps->pBuffer+mOutputBufSps->nOffset, &mSizeSps,
                                         mOutputBufPps->pBuffer+mOutputBufPps->nOffset, &mSizePps, 1);
#endif

    mGenerateSpsPps = false;
    mState = StateLoadedSpsPpsRequest;

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < fill_sps_pps_buffers_request");
}


void METH(fill_sps_pps_buffers)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > fill_sps_pps_buffers");

#ifdef H24ENC_SPS_PPS_IN_ONE_BUFFER
    OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] SPS-PPS length %d",mSizeSpsPps);

    for (int i=0; i<mSizeSpsPps; i++)
    {
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] SPS-PPS %02x",mOutputBufSpsPps->pBuffer[i]);
    }

    mOutputBufSpsPps->nFilledLen = mSizeSpsPps;
    mOutputBufSpsPps->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME | OMX_BUFFERFLAG_CODECCONFIG;

    mSequenceHeader.size = mSizeSpsPps;
    memcpy(mSequenceHeader.data, mOutputBufSpsPps->pBuffer+mOutputBufSpsPps->nOffset, mSizeSpsPps);
#else
    OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] SPS length %d",mSizeSps);
    OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] PPS length %d",mSizePps);

    for (int i=0; i<mSizeSps; i++)
    {
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] SPS %02x",mOutputBufSps->pBuffer[i]);
    }

    for (int i=0; i<mSizePps; i++)
    {
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] PPS %02x",mOutputBufPps->pBuffer[i]);
    }

    mOutputBufSps->nFilledLen = mSizeSps;
    mOutputBufPps->nFilledLen = mSizePps;

    mOutputBufSps->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME | OMX_BUFFERFLAG_CODECCONFIG;
    mOutputBufPps->nFlags |= OMX_BUFFERFLAG_ENDOFFRAME | OMX_BUFFERFLAG_CODECCONFIG;

    mSequenceHeader.size = mSizeSps+mSizePps;
    memcpy(mSequenceHeader.data, mOutputBufSps->pBuffer+mOutputBufSps->nOffset, mSizeSps);
    memcpy(mSequenceHeader.data + mSizeSps, mOutputBufPps->pBuffer+mOutputBufPps->nOffset, mSizePps);
#endif

    mState = StateLoadedSpsPpsReady;

    scheduleProcessEvent();

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < fill_sps_pps_buffers");
}


// for starting encode
//   * no encoding must be running at this time
//   * a buffer in and a buffer out must be available in queue
bool METH(check_start)()
{
    OstTraceFiltInst3(TRACE_API, "[ARMNMF DDEP] EncodingStatus: %d - port0: %d - port1: %d",mEncodingStatus, mPorts[0].queuedBufferCount(), mPorts[1].queuedBufferCount());
    return (!mEncodingStatus) && (mPorts[0].queuedBufferCount()>0) && (mPorts[1].queuedBufferCount()>0);
}



void METH(processExtraData)(OMX_BUFFERHEADERTYPE_p pBufferHdr,t_uint32 flag_tbd)
{
    // typedef struct OMX_OTHER_EXTRADATATYPE {
    //     OMX_U32 nSize;
    //     OMX_VERSIONTYPE nVersion;
    //     OMX_U32 nPortIndex;
    //     OMX_EXTRADATATYPE eType;
    //     OMX_U32 nDataSize;
    //     OMX_U8 data[1];
    // } OMX_OTHER_EXTRADATATYPE;

    // typedef enum OMX_SYMBIAN_EXTRADATATYPE {
    // OMX_SYMBIAN_ExtraDataVideoStabilization = OMX_ExtraDataVendorStartUnused + OMX_SYMBIAN_IV_COMMON_EXTENSIONS_START_OFFSET,
    // OMX_SYMBIAN_ExtraDataMax = 0x7FFFFFFF
    // } OMX_SYMBIAN_EXTRADATATYPE;

    // typedef struct OMX_SYMBIAN_DIGITALVIDEOSTABTYPE {
    //     OMX_BOOL bState;   			//**< The state of digital image stabilization */
    //     OMX_U32 nTopLeftCropVectorX; 	//**< Horizontal coordinate of the crop vector from top left angle, absolute wrt overscanning width  */
    //     OMX_U32 nTopLeftCropVectorY; 	//**< Vertical coordinate of the crop vector from top left angle, absolute wrt overscanning height  */
    //     OMX_U32 nMaxOverscannedWidth; 	//**<  Max W+30% */
    //     OMX_U32 nMaxOverscannedHeight; 	//**<  Max H+30% */
    // } OMX_SYMBIAN_DIGITALVIDEOSTABTYPE;

    // by default, set frame == window
    mFrameInfo.specific_frameinfo.sfw = mFrameInfo.common_frameinfo.pic_width;
    mFrameInfo.specific_frameinfo.sfh = mFrameInfo.common_frameinfo.pic_height;
    mFrameInfo.specific_frameinfo.sho = 0;
    mFrameInfo.specific_frameinfo.svo = 0;
    
    //Traverse the list of extra data sections
        OMX_OTHER_EXTRADATATYPE *pExtra;
        OMX_U8 *pTmp = pBufferHdr->pBuffer + pBufferHdr->nOffset + pBufferHdr->nFilledLen + 3;
        pBufferHdr->nFlags &= (~(OMX_BUFFERFLAG_EXTRADATA));//Resetting Buffer Flag
        pExtra = (OMX_OTHER_EXTRADATATYPE *) (((OMX_U32) pTmp) & ~3);
        OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] Extradata Flag Enabled");
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata pExtra=0x%x",(unsigned int)pExtra);
    
        while(pExtra->eType != OMX_ExtraDataNone)
        {
            OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata pExtra->nSize         %d",pExtra->nSize);
            OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata pExtra->nVersion      %d",pExtra->nVersion.nVersion);
            OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata pExtra->nPortIndex    %d",pExtra->nPortIndex);
            OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata pExtra->eType         %d",pExtra->eType);
            OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata pExtra->nDataSize     %d",pExtra->nDataSize);
    
            if (pExtra->eType == (OMX_EXTRADATATYPE)OMX_SYMBIAN_ExtraDataVideoStabilization)
            {
                OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] Extradata stab found");
                break;
            }
    
            pExtra = (OMX_OTHER_EXTRADATATYPE *) (((OMX_U8 *) pExtra) + pExtra->nSize);
            OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata type= %d", pExtra->eType);
        }
    
        if (pExtra->eType == OMX_ExtraDataNone) // OMX_SYMBIAN_ExtraDataVideoStabilization not found
        {
            OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] Extradata stab not found");
            if(flag_tbd)
            {
                mFrameInfo.specific_frameinfo.sfw = osWidth;
                mFrameInfo.specific_frameinfo.sfh = osHeight;
            }
            return;
        }
    
    
        if (pExtra->nDataSize!=sizeof(OMX_SYMBIAN_DIGITALVIDEOSTABTYPE)) // TODO: what to do in such case
        {
            OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] Extradata ERROR: wrong nDataSize");
            //return;
        }
    
        OMX_SYMBIAN_DIGITALVIDEOSTABTYPE* stabinfo = reinterpret_cast<OMX_SYMBIAN_DIGITALVIDEOSTABTYPE*>(pExtra->data);
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata bState=%d",stabinfo->bState);
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata nMaxOverscannedWidth=%d",stabinfo->nMaxOverscannedWidth);
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata nMaxOverscannedHeight=%d",stabinfo->nMaxOverscannedHeight);
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata nTopLeftCropVectorX=%d",stabinfo->nTopLeftCropVectorX);
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] Extradata nTopLeftCropVectorY=%d",stabinfo->nTopLeftCropVectorY);
    
        // "bState == 0" means stabilization is not activated (i.e: frame == window)

        if (stabinfo->bState == 0)
        {
            OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] Extradata Stab Present but bState Disabled");
            if(flag_tbd)
            {
                mFrameInfo.specific_frameinfo.sfw = osWidth;
                mFrameInfo.specific_frameinfo.sfh = osHeight;
            }
        }
        else 
        {
            OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] Extradata Stab Present and bState enabled");
            mFrameInfo.specific_frameinfo.sfw = stabinfo->nMaxOverscannedWidth;
            mFrameInfo.specific_frameinfo.sfh = stabinfo->nMaxOverscannedHeight;
            mFrameInfo.specific_frameinfo.sho = stabinfo->nTopLeftCropVectorX;
            mFrameInfo.specific_frameinfo.svo = stabinfo->nTopLeftCropVectorY;
        }

   return;
}
void METH(destroy)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > destroy");
    //init_encoder.close();
    //VFM_CloseMemoryList(mCtxtMemory);
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < destroy");
}


void METH(notify_input_buffer)(void *p_b, t_uint32 size, t_valid_output valid, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags)
{
    OstTraceFiltInst1(TRACE_API, "[ARMNMF DDEP] > notify_input_buffer %d",valid);

    //clear input bit
    mEncodingStatus &= 1;

    OMX_BUFFERHEADERTYPE* input_buf = mInputSet.getBufferHeader(p_b);

    returnBufferDDep(0, input_buf);

    if(mIsFlushRequested)
    {
        flush_command_to_codec();
    }

    if(mIsPauseRequested)
    {
        pause_command_to_codec();
    }

    scheduleProcessEvent();
    OstTraceFiltInst1(TRACE_API, "[ARMNMF DDEP] < notify_input_buffer %d",valid);
}

void METH(notify_output_buffer)(void *p_b, t_uint32 offset, t_uint32 size, t_valid_output valid, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags)
{
    OstTraceFiltInst3(TRACE_API, "[ARMNMF DDEP] > notify_output_buffer 0x%x - offset %d - size: %d", (unsigned int)p_b, offset, size);

    //clear output bit
    mEncodingStatus &= 2;

    OMX_BUFFERHEADERTYPE* output_buf = mOutputSet.getBufferHeader(p_b);

    if( (size + offset) > output_buf->nAllocLen)
    {
		OstTraceInt0(TRACE_ERROR, "[DDEP] Buffer size is greater than nAllocLen");
        NMF_PANIC("[DDEP] Buffer size is greater than nAllocLen");
    }

    if ( (size == 0) && !(nFlags & OMX_BUFFERFLAG_EOS))
    {
        OstTraceFiltInst1(TRACE_FLOW, "[ARMNMF DDEP] frame skipped 0x%x", (unsigned int)output_buf);
        mPorts[1].requeueBuffer(output_buf);
    }
    else
    {
        nFlags |= OMX_BUFFERFLAG_ENDOFFRAME;
        if (valid == VALID_OUT_I_FRAME)
        {
            nFlags |= OMX_BUFFERFLAG_SYNCFRAME;
        }

        output_buf->nFlags     = nFlags;
        output_buf->nFilledLen = size;
        output_buf->nOffset    = offset;

        returnBufferDDep(1, output_buf);

        if (nFlags & OMX_BUFFERFLAG_EOS) {
            OstTraceFiltInst0(TRACE_FLOW, "[ARMNMF DDEP] LAST BUFFER");
            proxy.eventHandler(OMX_EventBufferFlag, 1, OMX_BUFFERFLAG_EOS);
        }
    }

    if(mIsFlushRequested)
    {
        flush_command_to_codec();
    }

    if(mIsPauseRequested)
    {
        pause_command_to_codec();
    }

    scheduleProcessEvent();
    OstTraceFiltInst2(TRACE_API, "[ARMNMF DDEP] < notify_output_buffer %d - size: %d", valid,size);
}




void METH(flush_command_to_codec)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > flush_command_to_codec");
    if(mEncodingStatus == 0)
    {
        flushInput();
        flushOutput();
        mIsFlushRequested = false;
    }
    else
    {
        mIsFlushRequested = true;
    }
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < flush_command_to_codec");
}


void METH(pause_command_to_codec)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > pause_command_to_codec");
    if(mEncodingStatus == 0)
    {
        codec_ack_pause();
        mIsPauseRequested = false;
    }
    else
    {
        mIsPauseRequested = true;
    }
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < pause_command_to_codec");
}


void METH(flushInput)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > flushInput");

    OMX_PTR pt;
    OMX_BUFFERHEADERTYPE *pBuffer;
    while (0!= (pt=mInputSet.FlushOneBuffer())) {
        pBuffer = mInputSet.getBufferHeader(pt);
        pBuffer->nFilledLen = 0;
        returnBufferDDep(0, pBuffer);
    }

    codec_ack_flushing(0);

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < flushInput");
}


void METH(flushOutput)()
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > flushOutput");

    OMX_PTR pt;
    OMX_BUFFERHEADERTYPE *pBuffer;
    while (0!= (pt=mOutputSet.FlushOneBuffer())) {
        pBuffer = mOutputSet.getBufferHeader(pt);
        pBuffer->nFilledLen = 0;
        returnBufferDDep(1, pBuffer);
    }

    codec_ack_flushing(1);

    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < flushOutput");
}

//updating DVFS
void METH(updateDVFS)()
{
    OstTraceInt0(H264ENC_TRACE_GROUP_DDEP,"[DDEP] > updateDVFS");
    // always true for the moment
    /* + change for ER 346056 */
	OMX_U32 thresholdValueDVFS = (1280/16)*(720/16); //DVFS to managed on 720p resolution
	OMX_U32 actualMBs = (mFrameInfo.common_frameinfo.pic_width/16)*(mFrameInfo.common_frameinfo.pic_height/16);
    if (actualMBs >= thresholdValueDVFS) {
	/* - change for ER 346056 */
        setDVFSValue(OMX_TRUE);
    } else {
        setDVFSValue(OMX_TRUE);
    }
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < updateDVFS");
}

//+ ER335583
//updating DDR
void METH(updateDDR)()
{
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] > updateDDR");
    // always true for the moment
    /* + change for ER 346056 */
	OMX_U32 thresholdValueDDR = (1280/16)*(720/16); //DDR to managed on 720p resolution
	OMX_U32 actualMBs = (mFrameInfo.common_frameinfo.pic_width/16)*(mFrameInfo.common_frameinfo.pic_height/16);
    if (actualMBs >= thresholdValueDDR) {
	/* - change for ER 346056 */
        setDDRValue(OMX_TRUE);
    } else {
        setDDRValue(OMX_TRUE);
    }
    OstTraceInt0(TRACE_API, "[ARMNMF DDEP] < updateDDR");
}
//- ER335583

void METH(send_vfm_memory)(void *memory_ctxt)
{
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] > send_vfm_memory");
    addPendingCommand();
    init_encoder.init(memory_ctxt);
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < send_vfm_memory");
}

void METH(sendProxyEvent)(OMX_IN OMX_EVENTTYPE eEvent,  OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2)
{
    OstTraceFiltInst3(TRACE_API, "[ARMNMF DDEP] > sendProxyEvent eEvent=%d, nData1=%d, nData2=%d", eEvent, nData1, nData2);
    proxy.eventHandler(eEvent, nData1, nData2);
    OstTraceFiltInst0(TRACE_API, "[ARMNMF DDEP] < sendProxyEvent");
}


void METH(PrintParams)()
{

    OstTraceFiltInst0(TRACE_DEBUG, "DDEP parameters:");
    OstTraceFiltInst0(TRACE_DEBUG, "================");
    OstTraceFiltInst2(TRACE_DEBUG, "size:                      %dx%d",mFrameInfo.common_frameinfo.pic_width,mFrameInfo.common_frameinfo.pic_height);
    OstTraceFiltInst2(TRACE_DEBUG, "profile - level            %d-%d",mFrameInfo.common_frameinfo.eProfile, mFrameInfo.common_frameinfo.eLevel);
    OstTraceFiltInst1(TRACE_DEBUG, "nTimeStampH:               %d",mFrameInfo.common_frameinfo.nTimeStampH);
    OstTraceFiltInst1(TRACE_DEBUG, "nTimeStampL:               %d",mFrameInfo.common_frameinfo.nTimeStampL);
    OstTraceFiltInst1(TRACE_DEBUG, "nColorPrimary:             %d",mFrameInfo.common_frameinfo.nColorPrimary);
    OstTraceFiltInst1(TRACE_DEBUG, "nSupportedExtension:       %d",mFrameInfo.common_frameinfo.nSupportedExtension);
    OstTraceFiltInst1(TRACE_DEBUG, "sfw:                       %d",mFrameInfo.specific_frameinfo.sfw);
    OstTraceFiltInst1(TRACE_DEBUG, "sfh:                       %d",mFrameInfo.specific_frameinfo.sfh);
    OstTraceFiltInst1(TRACE_DEBUG, "sho:                       %d",mFrameInfo.specific_frameinfo.sho);
    OstTraceFiltInst1(TRACE_DEBUG, "svo:                       %d",mFrameInfo.specific_frameinfo.svo);
    OstTraceFiltInst1(TRACE_DEBUG, "n:                         %d",mFrameInfo.specific_frameinfo.n);
    OstTraceFiltInst1(TRACE_DEBUG, "algo:                      %d",mFrameInfo.specific_frameinfo.algo);
    OstTraceFiltInst1(TRACE_DEBUG, "OutFileMode:               %d",mFrameInfo.specific_frameinfo.OutFileMode);
    OstTraceFiltInst1(TRACE_DEBUG, "IDRIntraEnable:            %d",mFrameInfo.specific_frameinfo.IDRIntraEnable);
    OstTraceFiltInst1(TRACE_DEBUG, "IntraPeriod:               %d",mFrameInfo.specific_frameinfo.IntraPeriod);
    OstTraceFiltInst1(TRACE_DEBUG, "SliceMode:                 %d",mFrameInfo.specific_frameinfo.SliceMode);
    OstTraceFiltInst1(TRACE_DEBUG, "MbSliceSize:               %d",mFrameInfo.specific_frameinfo.MbSliceSize);
    OstTraceFiltInst1(TRACE_DEBUG, "ByteSliceSize:             %d",mFrameInfo.specific_frameinfo.ByteSliceSize);
    OstTraceFiltInst1(TRACE_DEBUG, "UseConstrainedIntraPred:   %d",mFrameInfo.specific_frameinfo.UseConstrainedIntraPred);
    OstTraceFiltInst1(TRACE_DEBUG, "QPISlice:                  %d",mFrameInfo.specific_frameinfo.QPISlice);
    OstTraceFiltInst1(TRACE_DEBUG, "QPPSlice:                  %d",mFrameInfo.specific_frameinfo.QPPSlice);
    OstTraceFiltInst1(TRACE_DEBUG, "PicOrderCntType:           %d",mFrameInfo.specific_frameinfo.PicOrderCntType);
    OstTraceFiltInst1(TRACE_DEBUG, "BrcType:                   %d",mFrameInfo.specific_frameinfo.BrcType);
    OstTraceFiltInst1(TRACE_DEBUG, "Bitrate:                   %d",mFrameInfo.specific_frameinfo.Bitrate);
    OstTraceFiltInst1(TRACE_DEBUG, "VBRConfig:                 %d",mFrameInfo.specific_frameinfo.VBRConfig);
    OstTraceFiltInst1(TRACE_DEBUG, "FrameRate:                 %d",mFrameInfo.specific_frameinfo.FrameRate);
    OstTraceFiltInst1(TRACE_DEBUG, "disableH4D:                %d",mFrameInfo.specific_frameinfo.disableH4D);
    OstTraceFiltInst1(TRACE_DEBUG, "SearchWindow               %d",mFrameInfo.specific_frameinfo.SearchWindow);
    OstTraceFiltInst1(TRACE_DEBUG, "EncoderComplexity          %d",mFrameInfo.specific_frameinfo.EncoderComplexity);
    OstTraceFiltInst1(TRACE_DEBUG, "ProfileIDC                 %d",mFrameInfo.specific_frameinfo.ProfileIDC);
    OstTraceFiltInst1(TRACE_DEBUG, "TransformMode              %d",mFrameInfo.specific_frameinfo.TransformMode);
    OstTraceFiltInst1(TRACE_DEBUG, "IntraRefreshType           %d",mFrameInfo.specific_frameinfo.IntraRefreshType);
    OstTraceFiltInst1(TRACE_DEBUG, "AirMbNum                   %d",mFrameInfo.specific_frameinfo.AirMbNum);
    OstTraceFiltInst1(TRACE_DEBUG, "CirMbNum                   %d",mFrameInfo.specific_frameinfo.CirMbNum);
    OstTraceFiltInst1(TRACE_DEBUG, "ForceIntraPredMode         %d",mFrameInfo.specific_frameinfo.ForceIntraPredMode);
    OstTraceFiltInst1(TRACE_DEBUG, "HrdSendMessages            %d",mFrameInfo.specific_frameinfo.HrdSendMessages);
    OstTraceFiltInst1(TRACE_DEBUG, "HeaderDataChanging         %d",mFrameInfo.specific_frameinfo.HeaderDataChanging);
    OstTraceFiltInst1(TRACE_DEBUG, "video_full_range_flag      %d",mFrameInfo.specific_frameinfo.video_full_range_flag);
    OstTraceFiltInst1(TRACE_DEBUG, "colour_primaries           %d",mFrameInfo.specific_frameinfo.colour_primaries);
    //+ code change for CR 361667
    OstTraceFiltInst1(TRACE_DEBUG, "ForceIntra                 %d",mFrameInfo.specific_frameinfo.ForceIntra);
    //- code change for CR 361667
    //+ code for step 2 of CR 332873
    OstTraceFiltInst1(TRACE_DEBUG, "CpbBufferSize              %d",mFrameInfo.specific_frameinfo.CpbBufferSize);
    OstTraceFiltInst1(TRACE_DEBUG, "CBR_clipped                %d",mFrameInfo.specific_frameinfo.CBR_clipped);
    OstTraceFiltInst1(TRACE_DEBUG, "CBR_clipped_min_QP         %d",mFrameInfo.specific_frameinfo.CBR_clipped_min_QP);
    OstTraceFiltInst1(TRACE_DEBUG, "CBR_clipped_max_QP         %d",mFrameInfo.specific_frameinfo.CBR_clipped_max_QP);
    //- code for step 2 of CR 332873

}






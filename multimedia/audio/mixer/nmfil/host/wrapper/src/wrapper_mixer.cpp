/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   wrapper_mixer.cpp
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <mixer/nmfil/host/wrapper.nmf>
#include <string.h>
#include <verbose.h>
#include <armnmf_dbc.h>
#include "AFM_Utils.h"


#include "OstTraceDefinitions_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mixer_nmfil_host_wrapper_src_wrapper_mixerTraces.h"
#endif
#include "OpenSystemTrace_ste.h"


#define MIXER_SAMPLE_16_BITS 16
#define MIXER_SAMPLE_32_BITS 32

METH(mixer_nmfil_host_wrapper)() {

    int i,j;
    for(i=0;i<NB_INPUT;i++){
        mHeap[i] = NULL;
        mSrcOutputBufferHeader[i].nSize = sizeof(OMX_BUFFERHEADERTYPE);
        mSrcOutputBufferHeader[i].nVersion.s.nVersionMajor    = 0;
        mSrcOutputBufferHeader[i].nVersion.s.nVersionMinor    = 0;
        mSrcOutputBufferHeader[i].nVersion.s.nRevision	    = 0;
        mSrcOutputBufferHeader[i].nVersion.s.nStep		    = 0;
        mSrcOutputBufferHeader[i].pBuffer = NULL;
        mSrcOutputBufferHeader[i].nAllocLen = 0;
        mSrcOutputBufferHeader[i].nFilledLen = 0;
        mSrcOutputBufferHeader[i].nOffset = 0;
        mSrcOutputBufferHeader[i].pAppPrivate = NULL;
        mSrcOutputBufferHeader[i].pPlatformPrivate= NULL;
        mSrcOutputBufferHeader[i].pInputPortPrivate= NULL;
        mSrcOutputBufferHeader[i].pOutputPortPrivate= NULL;
        mSrcOutputBufferHeader[i].hMarkTargetComponent= NULL;
        mSrcOutputBufferHeader[i].pMarkData= NULL;
        mSrcOutputBufferHeader[i].nTickCount=0;
        mSrcOutputBufferHeader[i].nTimeStamp=0;
        mSrcOutputBufferHeader[i].nFlags=0;
        mSrcOutputBufferHeader[i].nOutputPortIndex=0;
        mSrcOutputBufferHeader[i].nInputPortIndex=i+1;
        mSrcOutputBuffer[i] = &mSrcOutputBufferHeader[i];


        mVolCtrlOutputBuf[i].nSize = sizeof(OMX_BUFFERHEADERTYPE);
        mVolCtrlOutputBuf[i].nVersion.s.nVersionMajor    = 0;
        mVolCtrlOutputBuf[i].nVersion.s.nVersionMinor    = 0;
        mVolCtrlOutputBuf[i].nVersion.s.nRevision	    = 0;
        mVolCtrlOutputBuf[i].nVersion.s.nStep		    = 0;
        mVolCtrlOutputBuf[i].pBuffer = NULL;
        mVolCtrlOutputBuf[i].nAllocLen = 0;
        mVolCtrlOutputBuf[i].nFilledLen = 0;
        mVolCtrlOutputBuf[i].nOffset = 0;
        mVolCtrlOutputBuf[i].pAppPrivate = NULL;
        mVolCtrlOutputBuf[i].pPlatformPrivate= NULL;
        mVolCtrlOutputBuf[i].pInputPortPrivate= NULL;
        mVolCtrlOutputBuf[i].pOutputPortPrivate= NULL;
        mVolCtrlOutputBuf[i].hMarkTargetComponent= NULL;
        mVolCtrlOutputBuf[i].pMarkData= NULL;
        mVolCtrlOutputBuf[i].nTickCount=0;
        mVolCtrlOutputBuf[i].nTimeStamp=0;
        mVolCtrlOutputBuf[i].nFlags=0;
        mVolCtrlOutputBuf[i].nOutputPortIndex=0;
        mVolCtrlOutputBuf[i].nInputPortIndex=i+1;

        // SRC settings
        mSrcMode[i]= SRC_STANDARD_QUALITY;
        
        mInputFreq[i] = FREQ_UNKNOWN;
        for(j=0;j<MAXCHANNELS_SUPPORT;j++){
            mChannelTypeIn[i][j] = CHANNEL_NONE;
        }

        //Upmix or Downmix info
        mUpOrDownmix[i] = NOMIX;
    }
    mOutputFreq = FREQ_UNKNOWN;
    mBitsPerSample = 16;
    mBytePerSample = mBitsPerSample / 8;
    
    //By default all EOS are received
    mEosInputs     = 0xFF;
    //By default all inputs are not paused
    mPausedInputs = 0x00;
}

METH(~mixer_nmfil_host_wrapper)() { 
    int portIdx;
    
    for(portIdx=INPUT_PORT_IDX;portIdx<NB_INPUT;portIdx++)
    {
        if(mHeap[portIdx-INPUT_PORT_IDX] != NULL){
            free(mHeap[portIdx-INPUT_PORT_IDX]);
        }
        if(mSrcOutputBuffer[portIdx-INPUT_PORT_IDX]->pBuffer != NULL){
            free(mSrcOutputBuffer[portIdx-INPUT_PORT_IDX]->pBuffer);
        }
        if(mVolCtrlOutputBuf[portIdx-INPUT_PORT_IDX].pBuffer != NULL)
        {
            delete [] mVolCtrlOutputBuf[portIdx-INPUT_PORT_IDX].pBuffer;
        }
    }
}

void
METH(fillBufferWithZeroes)(OMX_BUFFERHEADERTYPE_p  buf) 
{
  
    void	*outputBuffer;
    
    OstTraceFiltInst3(TRACE_DEBUG,"MixerNmfHost::FillBufferWithZero with offset %d and size nAllocLen=%d nFilledLen=%d",buf->nOffset,buf->nAllocLen,buf->nFilledLen);
    outputBuffer= (void *)((t_uint32)buf->pBuffer + buf->nOffset + buf->nFilledLen);
    memset(outputBuffer,0, (buf->nAllocLen-buf->nFilledLen));
    buf->nFilledLen = buf->nAllocLen;
}

void
METH(sampleRateConversion)(int idx,OMX_BUFFERHEADERTYPE_p  inputBuf,OMX_BUFFERHEADERTYPE_p  outputBuf, int blocksize, bool *needInputBuf, bool *filledOutputBuf ,int nbchannel)
{
    int consumed = 0, produced = 0, flush = 0, flush_in=0;
    int nbsmp;

    
    int *inputbuffer,*outputBuffer;
    inputbuffer     = (int *)((int)inputBuf->pBuffer  + inputBuf->nOffset);
    outputBuffer    = (int *)((int)outputBuf->pBuffer + outputBuf->nOffset);
   

    OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfHost::sampleRateConversion idx %d and low latency=%d",idx,mSrcMode[idx-INPUT_PORT_IDX]);
    
    if (mSrcMode[idx-INPUT_PORT_IDX] == SRC_LOW_LATENCY_IN_MSEC) {
        consumed=blocksize;
        produced=0;
    } else {
        produced = consumed = blocksize;
    }

    flush       = (inputBuf->nFlags & OMX_BUFFERFLAG_EOS) ? true : false;
    flush_in    = flush;


    if((*mResampleContext[idx-INPUT_PORT_IDX].processing) (
           &mResampleContext[idx-INPUT_PORT_IDX], 
           (int*)inputbuffer,
           nbchannel, 
           &consumed, 
           (int*)outputBuffer, 
           &produced, 
           &flush) != 0)
    {
        ARMNMF_DBC_ASSERT(FALSE);
    }

    if (mSrcMode[idx-INPUT_PORT_IDX] == SRC_LOW_LATENCY_IN_MSEC) {
        if (flush_in == 0) {
            ARMNMF_DBC_POSTCONDITION(consumed == blocksize);
            ARMNMF_DBC_POSTCONDITION(produced == blocksize);
        }
        consumed*=(mResampleContext[idx-INPUT_PORT_IDX].fin_khz);
        produced*=(mResampleContext[idx-INPUT_PORT_IDX].fout_khz);
    }
   
    if (mSrcMode[idx-INPUT_PORT_IDX] != SRC_LOW_LATENCY_IN_MSEC) {
        ARMNMF_DBC_POSTCONDITION(consumed == blocksize || consumed == 0);
        ARMNMF_DBC_POSTCONDITION(produced == blocksize || produced == 0);
    }
    if ((inputBuf->nFlags & OMX_BUFFERFLAG_EOS) && !flush) {
        // on EOS, we must return the input buffer only 
        // when all corresponding output data has been generated 
        *needInputBuf = false;
    }
    else
    {
        if (mSrcMode[idx-INPUT_PORT_IDX] != SRC_LOW_LATENCY_IN_MSEC) { 
            nbsmp=blocksize;
        } else {
            nbsmp=consumed;
        }

        if (consumed == nbsmp || flush) {
            *needInputBuf = true;
        }
    }

    if (mSrcMode[idx-INPUT_PORT_IDX] != SRC_LOW_LATENCY_IN_MSEC) {
        nbsmp = blocksize;
    } else {
        nbsmp = produced;
    }

    if (produced == nbsmp || flush) {
        
        outputBuf->nFilledLen = produced *nbchannel*mBytePerSample;
        if (flush) {
            outputBuf->nFlags |= OMX_BUFFERFLAG_EOS;
        }
        *filledOutputBuf = true;
    } 
}

void
METH(applyVolumeAndSrc)(int idx) {

    OMX_BUFFERHEADERTYPE_p buf;
    bool needInputBuf = false, filledOutputBuf = false;
    
    if (mMixerInputBuffer[idx-INPUT_PORT_IDX]) {
        return;
    }
    
    buf = mSrcInputBuffer[idx-INPUT_PORT_IDX];

    if (buf == 0) {
        
        if (mPorts[idx].queuedBufferCount() == 0) {
            return;
        }
        OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::applying volume on port %d", idx);
        buf = mPorts[idx].getBuffer(FIRST_BUFFER);
        if (buf->nFlags & OMX_BUFFERFLAG_EOS) {
            fillBufferWithZeroes(buf);
        }
        
        if(mBitsPerSample == MIXER_SAMPLE_16_BITS){
            t_sint16 *inputbuffer,*outputBuffer;
            //Volctrl works inplace mode
            // Input and Output are the same buffer
            inputbuffer    = (t_sint16 *)((int)buf->pBuffer + buf->nOffset);
            outputBuffer    = (t_sint16 *)((int)buf->pBuffer + buf->nOffset);
            volctrl[idx-INPUT_PORT_IDX].process_16bits(inputbuffer, outputBuffer,(t_uint16)(buf->nFilledLen/mBytePerSample));
        }
        else{
            int *inputbuffer,*outputBuffer;
            //Volctrl works inplace mode
            // Input and Output are the same buffer
            inputbuffer    = (int *)((t_uint32)buf->pBuffer + buf->nOffset);
            outputBuffer    = (int *)((t_uint32)buf->pBuffer + buf->nOffset);
            volctrl[idx-INPUT_PORT_IDX].process(inputbuffer, outputBuffer,(t_uint16)(buf->nFilledLen/mBytePerSample));
        }
    }


    if (mInputFreq[idx-INPUT_PORT_IDX] == mOutputFreq) {
        mMixerInputBuffer[idx-INPUT_PORT_IDX] = buf; 
        return;
    } else {
        mSrcInputBuffer[idx-INPUT_PORT_IDX] = buf;
    }

    OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::applying SRC on port %d", idx);
    if(mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nFlags & OMX_BUFFERFLAG_EOS) {
    }
    if (mSrcMode[idx-INPUT_PORT_IDX] == SRC_LOW_LATENCY_IN_MSEC) {
        // FIXME: how to handle framing
        sampleRateConversion(idx, buf, mSrcOutputBuffer[idx-INPUT_PORT_IDX], 20, &needInputBuf, &filledOutputBuf,mOutputChannels);
    } else {
        sampleRateConversion(idx, buf, mSrcOutputBuffer[idx-INPUT_PORT_IDX], mOutputBlockSize, &needInputBuf, &filledOutputBuf,mOutputChannels);
    }

    if (filledOutputBuf) {
        // propagate timestamp for telephony usecase only (time alignment & latency control)
        // => low latency mode => output and input buffers are time aligned, just copy input values to output ones
        if (isLowLatencySRC(idx)) {
            mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nTimeStamp = buf->nTimeStamp;
            mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nFlags &= ~OMX_BUFFERFLAG_STARTTIME;            
            mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nFlags |= (buf->nFlags & OMX_BUFFERFLAG_STARTTIME);
        }
       
        mMixerInputBuffer[idx-INPUT_PORT_IDX] = mSrcOutputBuffer[idx-INPUT_PORT_IDX];
        if(mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nFlags & OMX_BUFFERFLAG_EOS) {
            
        }
    }

    if (needInputBuf) {
        OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost:: returning buffer on port %d", idx);
        mSrcInputBuffer[idx-INPUT_PORT_IDX] = 0;
        mPorts[idx].dequeueAndReturnBuffer();
    }
}


bool METH(isLowLatencySRC)(int idx) {
    if ((mInputFreq[idx] == FREQ_8KHZ || mInputFreq[idx] == FREQ_16KHZ || mInputFreq[idx] == FREQ_48KHZ)
            && (mOutputFreq == FREQ_8KHZ || mOutputFreq == FREQ_16KHZ || mOutputFreq == FREQ_48KHZ)) {
        return true;
    } else {
        return false;
    }
}

void
METH(applySrcAndVolume)(int idx) {

    OMX_BUFFERHEADERTYPE_p buf;
    OMX_BUFFERHEADERTYPE_p volCtrlInputBuf;
    
    bool needInputBuf = false, filledOutputBuf = false;
    
    if (mMixerInputBuffer[idx-INPUT_PORT_IDX]) {
        return;
    }
    
    if (mPorts[idx].queuedBufferCount() == 0) {
        return;
    }
    buf = mPorts[idx].getBuffer(FIRST_BUFFER);
    if (buf->nFlags & OMX_BUFFERFLAG_EOS) {
                
        fillBufferWithZeroes(buf);
    }
    

    if (mInputFreq[idx-INPUT_PORT_IDX] == mOutputFreq) {
        volCtrlInputBuf= buf; 
           
    } else {
            
        if (mSrcMode[idx-INPUT_PORT_IDX] == SRC_LOW_LATENCY_IN_MSEC) {
            // FIXME: how to handle framing
            sampleRateConversion(idx, buf, mSrcOutputBuffer[idx-INPUT_PORT_IDX], 20, &needInputBuf, &filledOutputBuf,mInputChannels[idx-INPUT_PORT_IDX]);
        } else {
            sampleRateConversion(idx, buf, mSrcOutputBuffer[idx-INPUT_PORT_IDX], mOutputBlockSize, &needInputBuf, &filledOutputBuf,mInputChannels[idx-INPUT_PORT_IDX]);
        }

        if (needInputBuf) {
            OstTraceFiltInst1(TRACE_FLOW,"MixerMpc:: returning buffer on port %d", idx);
            mPorts[idx].dequeueAndReturnBuffer();
        }

        if (filledOutputBuf) {
            // propagate timestamp for telephony usecase only (time alignment & latency control)
            // => low latency mode => output and input buffers are time aligned, just copy input values to output ones
            if (isLowLatencySRC(idx)) {
                mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nTimeStamp = buf->nTimeStamp;
                mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nFlags &= ~OMX_BUFFERFLAG_STARTTIME;            
                mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nFlags |= (buf->nFlags & OMX_BUFFERFLAG_STARTTIME);
            }

            volCtrlInputBuf = mSrcOutputBuffer[idx-INPUT_PORT_IDX];
        }
        else{
            return;
        }
        
    }
    if(mBitsPerSample == MIXER_SAMPLE_16_BITS){
        t_sint16 *buffer,*bufferout;
        //Volctrl can work inplace mode when upmix is selected
        buffer    = (t_sint16 *)((int)volCtrlInputBuf->pBuffer + volCtrlInputBuf->nOffset);
        bufferout = (t_sint16 *)mVolCtrlOutputBuf[idx-INPUT_PORT_IDX].pBuffer;
        
        volctrl[idx-INPUT_PORT_IDX].process_16bits(buffer, bufferout,(t_uint16)(volCtrlInputBuf->nFilledLen/mBytePerSample));
    }
    else
    {
        int *buffer,*bufferout;
        //Volctrl can work inplace mode when upmix is selected
        buffer    = (int *)((t_uint32)volCtrlInputBuf->pBuffer + volCtrlInputBuf->nOffset);
        bufferout = (int *)mVolCtrlOutputBuf[idx-INPUT_PORT_IDX].pBuffer;
        
        volctrl[idx-INPUT_PORT_IDX].process(buffer, bufferout,(t_uint16)(volCtrlInputBuf->nFilledLen/mBytePerSample));
    }
    
    mVolCtrlOutputBuf[idx-INPUT_PORT_IDX].nFlags = volCtrlInputBuf->nFlags;
    
    mMixerInputBuffer[idx-INPUT_PORT_IDX] = &mVolCtrlOutputBuf[idx-INPUT_PORT_IDX];
    if(mMixerInputBuffer[idx-INPUT_PORT_IDX]->nFlags & OMX_BUFFERFLAG_EOS) {
    }
    
}

bool 
METH(hasBufferToMix)(int idx, int nbInputWithData) {
    OMX_BUFFERHEADERTYPE_p inbuf;

    switch(mUpOrDownmix[idx-INPUT_PORT_IDX])
    {
        case NOMIX:
        case DOWNMIX:
            applyVolumeAndSrc(idx);
            break;
        case UPMIX:
            applySrcAndVolume(idx);
            break;
        default:
            ARMNMF_DBC_ASSERT(0);
    }
    

    inbuf = mMixerInputBuffer[idx-INPUT_PORT_IDX];

    if (inbuf) {
        
        mIn_list[nbInputWithData]   = (int*)inbuf->pBuffer;
        CLEAR_BIT(mEosInputs, idx-INPUT_PORT_IDX);
        return true;
    }

    return false;
}

void METH(do_mixing)(t_uint16 nbInput)
{
    mOutputBufPtr = mPorts[OUTPUT_PORT_IDX].getBuffer(FIRST_BUFFER);
	
    OstTraceFiltInst2(TRACE_FLOW,"MixerNmfHost::do_mixing %d inputs,mOutputChannels=%d ", nbInput,mOutputChannels);

    int * outbuf = (int * )((t_uint32)mOutputBufPtr->pBuffer+mOutputBufPtr->nOffset);
    int size = mOutputBlockSize * mOutputChannels;
    if (nbInput == 0) {
        // Fill output buf with zeroes
        memset(outbuf,0,mBytePerSample*mOutputBlockSize * mOutputChannels);
    }
    else if (nbInput == 1) {
        MMshort * inbuf = mIn_list[FIRST_BUFFER];
        memcpy( outbuf,inbuf, mBytePerSample*mOutputBlockSize * mOutputChannels);
    }
    else if (mOutputChannels == MIXER_STEREO) {
        mixer_stereo(mIn_list, nbInput, (MMshort *)outbuf,size, &mAlgoStruct);
    } 
    else if (mOutputChannels == MIXER_MONO) {
        mixer_mono(mIn_list, nbInput, (MMshort *)outbuf,size, 1, &mAlgoStruct);
    }
    else {
        //FIXME
        // Multichannel case
        mixer_nch(mIn_list, nbInput, (MMshort *)mOutputBufPtr->pBuffer,
                mAlgoStruct.data_block_size, mOutputChannels, 1, &mAlgoStruct);
    }

    mOutputBufPtr->nFilledLen = mBytePerSample*mOutputBlockSize * mOutputChannels;
}

void METH(releaseInputBuffer)(t_uint16 idx)
{
   
    if(mMixerInputBuffer[idx-INPUT_PORT_IDX]->nFlags & OMX_BUFFERFLAG_EOS) {
        SET_BIT(mEosInputs, idx-INPUT_PORT_IDX);
        
        proxy.eventHandler(OMX_EventBufferFlag, idx, mMixerInputBuffer[idx-INPUT_PORT_IDX]->nFlags);
    }

    // Reset flags when releasing buffer
    // Only usefull when mixerInputBuffer = srcOutputBuffer
    mMixerInputBuffer[idx-INPUT_PORT_IDX]->nFlags = 0;
    mMixerInputBuffer[idx-INPUT_PORT_IDX] = NULL;

    if (mInputFreq[idx-INPUT_PORT_IDX] == mOutputFreq) {
        mPorts[idx].dequeueAndReturnBuffer();
    }
}

void METH(releaseOutputBuffer)() 
{
    if ((mEosInputs | (mDisabledPorts >> 1) | mPausedInputs) == ALL_INPUTS) {
        mOutputBufPtr->nFlags |= OMX_BUFFERFLAG_EOS;
        
        proxy.eventHandler(OMX_EventBufferFlag, OUTPUT_PORT_IDX, mOutputBufPtr->nFlags);
    }
    else{
        mOutputBufPtr->nFlags = (mOutputBufPtr->nFlags>>1)<<1;
    }

        
    mPorts[OUTPUT_PORT_IDX].dequeueAndReturnBuffer();
    mOutputBufPtr = NULL;
}

void METH(process)()
{
    t_uint16 idx;
    t_uint16 nbInputWithData = 0;

    if (!mPorts[OUTPUT_PORT_IDX].queuedBufferCount()) return;

    for (idx = INPUT_PORT_IDX; idx <= NB_INPUT ; idx++) {
        if (isDisabled(idx) || isPausedInput(idx)) {
            continue;
        }
        else if (hasBufferToMix(idx, nbInputWithData)) {
            nbInputWithData++;
        }
        else if (isEosInput(idx)) {
            continue;
        }
        else {
            return;
        }
    }

    OstTraceFiltInst3(TRACE_DEBUG,"MixerNmfHost::process mEosInputs=0x%x mDisabledPorts=0x%x mPausedInputs=0x%x",mEosInputs,(mDisabledPorts >> 1),mPausedInputs);
    
    if ((mEosInputs | (mDisabledPorts >> 1) | mPausedInputs) == ALL_INPUTS) {
        OstTraceFiltInst0(TRACE_FLOW,"MixerNmfHost::no input to mix");
        ARMNMF_DBC_ASSERT(nbInputWithData == 0);
        return;
    }

    do_mixing(nbInputWithData);
    
    for (idx = INPUT_PORT_IDX; idx <= NB_INPUT ; idx++) {
        if ((!isDisabled(idx)) && (mMixerInputBuffer[idx-INPUT_PORT_IDX])) {
            if (idx == INPUT_PORT_IDX) {    
                mOutputBufPtr->nTimeStamp    = mMixerInputBuffer[idx-INPUT_PORT_IDX]->nTimeStamp;
                mOutputBufPtr->nFlags        = mMixerInputBuffer[idx-INPUT_PORT_IDX]->nFlags;
            }
            releaseInputBuffer(idx);
        }
    }
    
    releaseOutputBuffer();
}

void METH(initInputVolctrl)(int idx) {
    ARMNMF_DBC_PRECONDITION(idx <= NB_INPUT && idx >= INPUT_PORT_IDX);
    volctrl[idx-INPUT_PORT_IDX].init(mInputChannels[idx-INPUT_PORT_IDX], mOutputChannels, mInputFreq[idx-INPUT_PORT_IDX],mChannelTypeIn[idx-INPUT_PORT_IDX],mChannelTypeOut);

    if(mInputChannels[idx-INPUT_PORT_IDX] > mOutputChannels){
        mUpOrDownmix[idx-INPUT_PORT_IDX] = DOWNMIX;
    }
    if(mOutputChannels > mInputChannels[idx-INPUT_PORT_IDX]){
        mUpOrDownmix[idx-INPUT_PORT_IDX] = UPMIX;
    }
}

void *METH(portMalloc)(int idx, int size) {
    void * p;
    
    OstTraceFiltInst2(TRACE_FLOW,"MixerNmfHost::allocating %d words on port %d", size, idx);
    ARMNMF_DBC_ASSERT(mHeap[idx-INPUT_PORT_IDX]  == NULL);
    p = malloc(size);
    
    ARMNMF_DBC_ASSERT(p != NULL);
    mHeap[idx-INPUT_PORT_IDX] = p;
    return p;
}

void
METH(initInputSrc)(int idx) {
    int status, heapsize;
    void *buf;

    
    
    ARMNMF_DBC_PRECONDITION(idx <= NB_INPUT && idx >= INPUT_PORT_IDX);

    if(mUpOrDownmix[idx-INPUT_PORT_IDX] == UPMIX)
    {
        int size = mOutputBlockSize * mInputChannels[idx-INPUT_PORT_IDX];

        //We need to allocate an output buffer for the volctrl
        // In this case the SRC is done before the volctrl
        // The SRC blocksize is different
        // We must use the input nbChannel
        mVolCtrlOutputBuf[idx-INPUT_PORT_IDX].pBuffer = new OMX_U8[mOutputBlockSize * mOutputChannels*mBytePerSample];

        if (mInputFreq[idx-INPUT_PORT_IDX] == mOutputFreq) {
            return;
        }
        if(mSrcMode[idx-INPUT_PORT_IDX] == SRC_LOW_RIPPLE){
            OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::initInputSrc SRC_LOW_RIPPLE for port %d",idx);
        }
        else
        {
            
            if (isLowLatencySRC(idx-INPUT_PORT_IDX)) {
                mSrcMode[idx-INPUT_PORT_IDX] = SRC_LOW_LATENCY_IN_MSEC;
                OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::initInputSrc LOW LATENCY for port %d",idx);
            } 
            else {
                OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::initInputSrc SRC_STANDARD_QUALITY for port %d",idx);

            }
        }

        resample_calc_max_heap_size_fixin_fixout(mInputFreq[idx-INPUT_PORT_IDX],mOutputFreq,mSrcMode[idx-INPUT_PORT_IDX],&heapsize,mOutputBlockSize * mInputChannels[idx-INPUT_PORT_IDX],mInputChannels[idx-INPUT_PORT_IDX],0);
        
        buf = portMalloc(idx, heapsize); 
        if(mBitsPerSample == MIXER_SAMPLE_16_BITS){
            status = resample_x_init_ctx_low_mips_fixin_fixout_sample16(
                (char*)buf, heapsize, &mResampleContext[idx-INPUT_PORT_IDX], mInputFreq[idx-INPUT_PORT_IDX], 
                mOutputFreq, mSrcMode[idx-INPUT_PORT_IDX], mInputChannels[idx-INPUT_PORT_IDX], size);
        }
        else{
            status = resample_x_init_ctx_low_mips_fixin_fixout(
                (char*)buf, heapsize, &mResampleContext[idx-INPUT_PORT_IDX], mInputFreq[idx-INPUT_PORT_IDX], 
                mOutputFreq, mSrcMode[idx-INPUT_PORT_IDX], mInputChannels[idx-INPUT_PORT_IDX], size);  
        }
        ARMNMF_DBC_ASSERT(status == 0);

        buf = malloc(mOutputBlockSize * mInputChannels[idx-INPUT_PORT_IDX] *mBytePerSample);
        ARMNMF_DBC_ASSERT(buf != NULL);
        mSrcOutputBuffer[idx-INPUT_PORT_IDX]->pBuffer = (OMX_U8*)buf;
        mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nOffset = 0;
        mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nAllocLen = mOutputBlockSize * mInputChannels[idx-INPUT_PORT_IDX] *mBytePerSample;
    }
    else
    {
        int size = mOutputBlockSize * mOutputChannels;
        if (mInputFreq[idx-INPUT_PORT_IDX] == mOutputFreq) {
            return;
        }

        if(mSrcMode[idx-INPUT_PORT_IDX] == SRC_LOW_RIPPLE){
            OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::initInputSrc SRC_LOW_RIPPLE for port %d",idx);
        }
        else
        {
            if (isLowLatencySRC(idx-INPUT_PORT_IDX)) {
                mSrcMode[idx-INPUT_PORT_IDX] = SRC_LOW_LATENCY_IN_MSEC;
                OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::initInputSrc LOW LATENCY for port %d",idx);
            } 
            else {
                OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::initInputSrc SRC_STANDARD_QUALITY for port %d",idx);
            }
        }
        resample_calc_max_heap_size_fixin_fixout(
            mInputFreq[idx-INPUT_PORT_IDX],mOutputFreq,mSrcMode[idx-INPUT_PORT_IDX],&heapsize,
            size,mOutputChannels,0);
        
        buf = portMalloc(idx, heapsize); 
        if(mBitsPerSample == MIXER_SAMPLE_16_BITS){
            status = resample_x_init_ctx_low_mips_fixin_fixout_sample16(
                (char*)buf, heapsize, &mResampleContext[idx-INPUT_PORT_IDX], mInputFreq[idx-INPUT_PORT_IDX], 
                mOutputFreq, mSrcMode[idx-INPUT_PORT_IDX], mOutputChannels, size);
        }
        else{
            status = resample_x_init_ctx_low_mips_fixin_fixout(
                (char*)buf, heapsize, &mResampleContext[idx-INPUT_PORT_IDX], mInputFreq[idx-INPUT_PORT_IDX], 
                mOutputFreq, mSrcMode[idx-INPUT_PORT_IDX], mOutputChannels, size);
        }
        ARMNMF_DBC_ASSERT(status == 0);

        buf = malloc(mOutputBlockSize * mOutputChannels*mBytePerSample);
        ARMNMF_DBC_ASSERT(buf != NULL);
        mSrcOutputBuffer[idx-INPUT_PORT_IDX]->pBuffer = (OMX_U8*)buf;
        mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nOffset = 0;
        mSrcOutputBuffer[idx-INPUT_PORT_IDX]->nAllocLen = mOutputBlockSize * mOutputChannels*mBytePerSample;
    }

}

void
METH(initEnabledInputPort)(int idx) {
    OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::initEnabledInputPort %d", idx);

    if (isDisabled(OUTPUT_PORT_IDX)) return;
    
    initInputVolctrl(idx);
    initInputSrc(idx);
}


 
void
METH(initEnabledInputPorts)() {
    int i;
    OstTraceFiltInst0(TRACE_FLOW,"MixerNmfHost::initEnabledInputPorts");

    for (i =  INPUT_PORT_IDX; i <= NB_INPUT; i++) {
        if (isDisabled(i)) continue;
        initEnabledInputPort(i);
    }
}

void METH(resetPort)(t_uint32 idx) {
    OstTraceFiltInst1(TRACE_FLOW,"MixerNmfHost::resetPort %d", idx);

    if (idx == OUTPUT_PORT_IDX) {
        mOutputBufPtr = NULL;
    } else {
        mMixerInputBuffer[idx-INPUT_PORT_IDX]  = NULL;
        mSrcInputBuffer[idx-INPUT_PORT_IDX]    = NULL;
        SET_BIT(mEosInputs, idx-INPUT_PORT_IDX);
    }
}

// Interface for fsm
// reset is called when entering Idle state, inside Component::init called from fsmInit
void METH(reset)()
{
    OstTraceFiltInst0(TRACE_FLOW,"MixerNmfHost::reset");

    
    for (int idx=0 ; idx<(NB_INPUT+NB_OUTPUT); idx++) {
      resetPort(idx);
    }
}

void METH(disablePortIndication)(t_uint32 portIdx)
{
    OstTraceFiltInst1(TRACE_DEBUG,"MixerNmfHost::disablePortIndication Port Idx %d",(portIdx));

    SET_BIT(mDisabledPorts, portIdx);
    
    resetPort(portIdx);

    if(portIdx != OUTPUT_PORT_IDX)
    {    
        if(mHeap[portIdx-INPUT_PORT_IDX] != NULL){
            free(mHeap[portIdx-INPUT_PORT_IDX]);
            mHeap[portIdx-INPUT_PORT_IDX] = NULL;
        }
        
        if(mSrcOutputBuffer[portIdx-INPUT_PORT_IDX]->pBuffer != NULL){
            free(mSrcOutputBuffer[portIdx-INPUT_PORT_IDX]->pBuffer);
            mSrcOutputBuffer[portIdx-INPUT_PORT_IDX]->pBuffer = NULL;
        }
        if(mVolCtrlOutputBuf[portIdx-INPUT_PORT_IDX].pBuffer != NULL)
        {
            delete [] mVolCtrlOutputBuf[portIdx-INPUT_PORT_IDX].pBuffer;
            mVolCtrlOutputBuf[portIdx-INPUT_PORT_IDX].nFlags = 0;
            mVolCtrlOutputBuf[portIdx-INPUT_PORT_IDX].pBuffer = NULL;
        }
    }

    if (mPorts[OUTPUT_PORT_IDX].queuedBufferCount()) {    
        // mixer may be able to generate output buf now
        // that this port is disabled
        scheduleProcessEvent();
    }
}

void METH(enablePortIndication)(t_uint32 portIdx)
{
    OstTraceFiltInst1(TRACE_DEBUG,"MixerNmfHost::enablePortIndication Port Idx %d",(portIdx));

    CLEAR_BIT(mDisabledPorts, portIdx);

    if (portIdx != OUTPUT_PORT_IDX) {
        initEnabledInputPort(portIdx);
    } else {
        initEnabledInputPorts();
    }
}

void METH(flushPortIndication)(t_uint32 portIdx)
{
    OstTraceFiltInst1(TRACE_DEBUG,"MixerNmfHost::flushPortIndication Port Idx %d",(portIdx));
}

bool METH(checkConfig)(HostMixerParam_t param) {
    // TODO
    return true;
}

void METH(mixerDefaultSettings)() {
    // SAMPLE16 should be 0x10000
    // I hope that this precondition is always true
    ARMNMF_DBC_PRECONDITION(mOutputBlockSize <= SAMPLE16);

    mAlgoStruct.alpha_dw  = ALPHA_DW_DEFAULT;
    mAlgoStruct.alpha_up  = ALPHA_UP_DEFAULT;
    mAlgoStruct.one_minus_alpha_dw = (0x800000U - ALPHA_DW_DEFAULT);
    mAlgoStruct.one_minus_alpha_up = (0x800000U - ALPHA_UP_DEFAULT);
    mAlgoStruct.threshold = THRESHOLD_DEFAULT;
    
    mAlgoStruct.coef_poly[0] = COEF_1;
    mAlgoStruct.coef_poly[1] = (MMlong)COEF_2;
    mAlgoStruct.coef_poly[2] = COEF_3;
    mAlgoStruct.coef_poly[3] = COEF_4;
    
    mAlgoStruct.env_st[0].abs_x0  = 0;
    mAlgoStruct.env_st[0].abs_x1  = 0;
    mAlgoStruct.env_st[0].abs_in  = 0;
    mAlgoStruct.env_st[0].envelop = 0;
    
    mAlgoStruct.env_st[1].abs_x0  = 0;
    mAlgoStruct.env_st[1].abs_x1  = 0;
    mAlgoStruct.env_st[1].abs_in  = 0;
    mAlgoStruct.env_st[1].envelop = 0;
    
    if(mBitsPerSample == MIXER_SAMPLE_16_BITS){
        // Force the 16 bits mode for the mixer
        mAlgoStruct.data_block_size = ((int)(mOutputBlockSize) | SAMPLE16);   
    }
    else{
        mAlgoStruct.data_block_size = ((int)(mOutputBlockSize)); 
    }
}


////////////////////////////////////////////////////////////
//			Provided Interfaces
////////////////////////////////////////////////////////////

void METH(start)(void) {

}
void METH(stop)(void) {
    
}

void METH(setMuteInputPort)(t_uint16 portIdx, BOOL bMute) {
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_INPUT && portIdx >= INPUT_PORT_IDX);
    if (!IS_NULL_INTERFACE(volctrl[portIdx-INPUT_PORT_IDX], setMute)) {
        volctrl[portIdx-INPUT_PORT_IDX].setMute(bMute);
    }
}

void METH(setBalanceInputPort)(t_uint16 portIdx, t_sint16 nBalance) {
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_INPUT && portIdx >= INPUT_PORT_IDX);
    if (!IS_NULL_INTERFACE(volctrl[portIdx-INPUT_PORT_IDX], setBalance)) {
        volctrl[portIdx-INPUT_PORT_IDX].setBalance(nBalance);
    }
}

void METH(setVolumeInputPort)(t_uint16 portIdx, t_sint16 nVolume) {
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_INPUT && portIdx >= INPUT_PORT_IDX);
    if (!IS_NULL_INTERFACE(volctrl[portIdx-INPUT_PORT_IDX], setVolume)) {
        volctrl[portIdx-INPUT_PORT_IDX].setVolume(nVolume);
    }
}

void METH(setVolumeRampInputPort)(
        t_uint16 portIdx, 
        t_sint16 nStartVolume, 
        t_sint16 nEndVolume, 
        t_uint16 nChannels, 
        t_uint24 nDuration, 
        BOOL bTerminate) 
{
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_INPUT && portIdx >= INPUT_PORT_IDX);
    if (!IS_NULL_INTERFACE(volctrl[portIdx-INPUT_PORT_IDX], setVolumeRamp)) {
        volctrl[portIdx-INPUT_PORT_IDX].setVolumeRamp(nStartVolume, nEndVolume, nChannels, nDuration, bTerminate);
    }
}

void METH(setPausedInputPort)(t_uint16 portIdx, BOOL bIsPaused)
{
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_INPUT && portIdx >= INPUT_PORT_IDX);
    OstTraceFiltInst2(TRACE_DEBUG,"MixerNmfHost::setPausedInputPort Port Idx %d,bIsPaused=%d",portIdx,bIsPaused);

    if (bIsPaused) {
        SET_BIT(mPausedInputs, portIdx-INPUT_PORT_IDX);
    } else {
        CLEAR_BIT(mPausedInputs, portIdx-INPUT_PORT_IDX);
    }

    // FIXME: testing Mixer.reset not very explicit
    if (mPorts[OUTPUT_PORT_IDX].queuedBufferCount()) {
        scheduleProcessEvent();
    }
}

void METH(setOutputPortParameter)(HostOutputPortParam_t outputPortParam) {
    int i;
    
    mOutputFreq         = outputPortParam.nSamplingRate;
    mOutputChannels     = outputPortParam.nChannels;
    mOutputBlockSize    = outputPortParam.nBlockSize;
    mBitsPerSample      = outputPortParam.nBitsPerSample;
    mBytePerSample = mBitsPerSample / 8;
    for(i=0;i<MAXCHANNELS_SUPPORT;i++){    
        mChannelTypeOut[i]     = outputPortParam.channel_type[i];
    }
    
    OstTraceFiltInst3(TRACE_FLOW,"MixerNmfHost::setOutputPortParameter Freq=%d Channels=%d Blocksize=%d", mOutputFreq, mOutputChannels, mOutputBlockSize);
}


void METH(setInputPortParameter)(t_uint16 portIdx, HostInputPortParam_t inputPortParam) {
    int i;
    ARMNMF_DBC_PRECONDITION(portIdx <= NB_INPUT && portIdx >= INPUT_PORT_IDX);
    // Params
    mInputChannels[portIdx-INPUT_PORT_IDX]    = inputPortParam.nChannels;
    mInputFreq[portIdx-INPUT_PORT_IDX]        = inputPortParam.nSamplingRate; 
    if(inputPortParam.nSrcMode == AFM_SRC_MODE_CUSTOM1){
        mSrcMode[portIdx-INPUT_PORT_IDX] =  SRC_LOW_RIPPLE;     
    }


    for(i=0;i<MAXCHANNELS_SUPPORT;i++){    
        mChannelTypeIn[portIdx-INPUT_PORT_IDX][i]    = inputPortParam.channel_type[i];
    }

    OstTraceFiltInst3(TRACE_FLOW,"MixerNmfHost::setInputPortParameter idx=%d Freq=%d Channels=%d ", portIdx, mInputFreq[portIdx-INPUT_PORT_IDX], mInputChannels[portIdx-INPUT_PORT_IDX]);

    // Configs
    setPausedInputPort(portIdx, inputPortParam.bPaused);
    setMuteInputPort(portIdx, inputPortParam.bMute);
    setBalanceInputPort(portIdx, inputPortParam.nBalance);
    setVolumeInputPort(portIdx, inputPortParam.nVolume);
    setVolumeRampInputPort(
            portIdx, inputPortParam.nVolume, inputPortParam.nRampEndVolume, inputPortParam.nRampChannels, 
            inputPortParam.nRampDuration, inputPortParam.bRampTerminate);
}

void METH(setParameter)(HostMixerParam_t mixerParam)
{
    int i;
    setTraceInfo((TraceInfo_t *)mixerParam.traceAddr,0);
    OstTraceFiltInst0(TRACE_FLOW,"MixerNmfHost::setParameter");


    ARMNMF_DBC_PRECONDITION(checkConfig(mixerParam));

    setOutputPortParameter(mixerParam.outputPortParam);

    for (i = INPUT_PORT_IDX; i <= NB_INPUT; i++) {
       setInputPortParameter(i, mixerParam.inputPortParam[i-INPUT_PORT_IDX]); 
    }

    mixerDefaultSettings();
}

void METH(fsmInit)(fsmInit_t initFsm)
{
    // Init this component...
    t_uint16 idx;
    ARMNMF_DBC_PRECONDITION(initFsm.traceInfoAddr != NULL);

    setTraceInfo(initFsm.traceInfoAddr, initFsm.id1);
    
  	OstTraceFiltInst0(TRACE_FLOW,"hst_mixer_wrapper::fsmInit");

    for (idx=INPUT_PORT_IDX; idx<=NB_INPUT ; idx++) {
        mPorts[idx].init(InputPort, 
			    false, 
			    false, 
			    0, 
			    0,
			    1, 
			    &inputport[idx-INPUT_PORT_IDX], 
			    idx, 
			    (initFsm.portsDisabled & (1 << idx)),
			    (initFsm.portsTunneled & (1 << idx)), 
			    this);
    }
    mPorts[OUTPUT_PORT_IDX].init(OutputPort, false, false, 0, 0, 1, &outputport,OUTPUT_PORT_IDX,
                                 (initFsm.portsDisabled & (1 << (OUTPUT_PORT_IDX))), 
                                 (initFsm.portsTunneled & (1 << (OUTPUT_PORT_IDX))), this);

    mDisabledPorts = initFsm.portsDisabled;

    initEnabledInputPorts();
    // Init this component...
    init(NB_INPUT+NB_OUTPUT, mPorts, &proxy, &me, false);

}


void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ARMNMF_DBC_ASSERT(0);
}

void METH(newFormat)(t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize, t_uint8 idx) {
    
}

// events from volctrl 
void METH(eventHandler)(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2, t_uint8 idx) {
    
    proxy.eventHandler(event, idx+INPUT_PORT_IDX, data2);
}

void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 idx)  
{
    OstTraceFiltInst3(TRACE_FLOW,"MixerNmfHost::emptyThisBuffer portIdx=%d  nFilledLen=%d nFlags=%d", idx+INPUT_PORT_IDX,buffer->nFilledLen, buffer->nFlags);
    

    Component::deliverBuffer(INPUT_PORT_IDX+idx, buffer); 
} 

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer)   
{ 
    OstTraceFiltInst0(TRACE_FLOW,"MixerNmfHost::fillThisBuffer");

    
    OMX_BUFFERHEADERTYPE* omxBuffer = (OMX_BUFFERHEADERTYPE*)buffer;
    omxBuffer->nFilledLen = 0;
    Component::deliverBuffer(OUTPUT_PORT_IDX, buffer); 
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    OstTraceFiltInst2(TRACE_FLOW,"MixerNmfHost::sendCommand cmd=%d, param=%d",cmd,param);

    Component::sendCommand(cmd, param);
}


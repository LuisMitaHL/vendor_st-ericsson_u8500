/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   tester.c
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include "mixer/nmfil/wrapper/test/tester.nmf"
#include <stdio.h>
#include <stdlib.h>
#include "dbc.h"
#include "string.h"
#include "fsm/component/include/Component.inl"

#define NB_MAX_MIXER_INPUTS 8
#define OUT 1
#define IN  0

////////////////////////////////////////////////////////////
//			Global Variables
////////////////////////////////////////////////////////////

typedef struct
{
	Buffer_t    buffer;
    int         eosReached;
    int         nbSample;
    int         isEnabled;
    int         count;
}InputBuffer;

static InputBuffer      mInBuffer[NB_MAX_MIXER_INPUTS];
static Buffer_t         mOutBuffer;
static void *           mFifoIn[1];
static void *           mFifoOut[NB_MAX_MIXER_INPUTS][1];
static Port             mPorts[NB_MAX_MIXER_INPUTS + 1];
Component               mTester;

static FILE *           mFIN[NB_MAX_MIXER_INPUTS] = NULL;
static char *           mOutputFile = NULL;
static FILE *           mFOUT = NULL;

static int              mBufferSizeIn;
static int              mBufferSizeOut;
static int              mNbInputToMix;
static int              mNbSampleToMix;

static int              roundval=0x00000080;
static int              mBufferIdx;

static bool             mSendNewFormat = FALSE;
static bool             mEosReceivedFromMixerOutputPort = FALSE;
static int              mNbChannels = 0;

////////////////////////////////////////////////////////////
// Interfaces for fsm
////////////////////////////////////////////////////////////

static void
ReadData(Buffer_t *Buf, t_uint16 idx) {
    int i;
    int size;

    unsigned char *arr;
    unsigned short *ptrdata;

    t_ExtendedAllocParams  allocParams;

    allocParams.bank = MEM_XTCM;
    allocParams.trace_p = NULL;
    arr = vmalloc(Buf->allocLen*2, &allocParams);

    for(i=0; i<Buf->allocLen*2; i++)
    {
        arr[i] = fgetc(mFIN[idx]);
        if(feof(mFIN[idx])){
            Buf->flags |= BUFFERFLAG_EOS;
            mInBuffer[idx].eosReached = 1;
            break;
        }
    }
    size = i;

    Buf->filledLen = i/2;   // in 16 bit word

    ptrdata = (unsigned short *)Buf->data;
    for(i=0; i<size; i+=2) {
        *ptrdata++ = ((arr[i]<<16)) | (arr[i+1]<<8) ;
    }

    if( (size%2) != 0){
        ptrdata--;
        *ptrdata = *ptrdata & 0xFF00 ;
        Buf->filledLen += 1; //in 16 bit word
    }

    vfree(arr, &allocParams);
}

static bool
checkWrapperInputBuffer(Buffer_p buf, t_uint16 idx) {
    if (buf->data != mInBuffer[idx].buffer.data) {
        return false;
    }
    if (buf->filledLen != 0) {
        return false;
    }
    return true;
}

static void
fillInputBuffer(Buffer_p buf, t_uint16 idx) {
    ReadData(buf, idx);
	mInBuffer[idx].nbSample += buf->filledLen + 2;
    if(!mInBuffer[idx].eosReached &&
        mInBuffer[idx].nbSample >= mNbSampleToMix) {
        printf("Tester Send EOS on mixer port %d\n", OUT+idx);
        buf->flags |= BUFFERFLAG_EOS;
        mInBuffer[idx].eosReached = 1;
    }
}

static bool
checkWrapperOutputBuffer(Buffer_p buf) {
    int i;

    if (buf->data != mOutBuffer.data) {
        return false;
    }
    if (buf->allocLen != mOutBuffer.allocLen) {
        return false;
    }

    return true;
}

static void
fillOutputFile(Buffer_p buf) {
    int i;

    for (i = 0; i < buf->filledLen; i++) {
        buf->data[i] = waddsat(buf->data[i],roundval);
        buf->data[i] = buf->data[i] >> 8;
    }

    fwrite16(buf->data,1, buf->filledLen, mFOUT);
    buf->filledLen = 0;
    if(buf->flags & BUFFERFLAG_EOS){
        mEosReceivedFromMixerOutputPort = TRUE;
        proxy.eventHandler(OMX_EventBufferFlag, IN, buf->flags);
    }
}

void
reset(Component * this) {
    int idx;

    mEosReceivedFromMixerOutputPort = FALSE;

    for(idx=0 ; idx<mNbInputToMix ; idx++) {
        mInBuffer[idx].eosReached = 0;
        mInBuffer[idx].nbSample = 0;
        mInBuffer[idx].isEnabled = 1;
        mInBuffer[idx].count = 0;

        if(mFIN[idx]) {
            rewind(mFIN[idx]);
        }
    }

    if(mFOUT){
        fclose(mFOUT);
        strcat(mOutputFile, "2");
        mFOUT = 0;
    }
    printf("Open new Output File : %s\n", mOutputFile);
    mFOUT = fopen(mOutputFile, "wb");
    ASSERT(mFOUT != NULL);
    mBufferIdx = 0;

    mSendNewFormat = TRUE;
}

void
disablePortIndication(t_uint32 portIdx) {
}

void
enablePortIndication(t_uint32 portIdx) {
}

void
flushPortIndication(t_uint32 portIdx) {
}

void
process(Component * this) {
    t_uint16 idx;

    if (Port_queuedBufferCount(&this->ports[IN])) {
        Buffer_p buf = Port_dequeueBuffer(&this->ports[IN]);
        ASSERT(mEosReceivedFromMixerOutputPort == FALSE);
        ASSERT(checkWrapperOutputBuffer(buf));
        mBufferIdx++;
        fillOutputFile(buf);
        Port_returnBuffer(&this->ports[IN], buf);
    }

    for(idx=0; idx<mNbInputToMix ; idx++) {
        if(!mInBuffer[idx].eosReached &&
            mInBuffer[idx].isEnabled &&
            Port_queuedBufferCount(&this->ports[OUT+idx])){
                Buffer_p buf =  Port_dequeueBuffer(&this->ports[OUT+idx]);
                ASSERT(checkWrapperInputBuffer(buf, idx));
                fillInputBuffer(buf, idx);
                mInBuffer[idx].count++;
                //printf("Count for buffer %d is %d\n",idx,mInBuffer[idx].count);
                Port_returnBuffer(&this->ports[OUT+idx], buf);
        }
    }
}


////////////////////////////////////////////////////////////
// Provided Interfaces
////////////////////////////////////////////////////////////


void METH(start)() {
    int i;

    mNbInputToMix = NB_MAX_MIXER_INPUTS;
    mNbSampleToMix = 0;

    for(i=0 ; i<NB_MAX_MIXER_INPUTS ; i++) {
        mInBuffer[i].buffer.data = NULL;;
        mInBuffer[i].buffer.allocLen = 0;
        mInBuffer[i].buffer.filledLen = 0;
        mInBuffer[i].buffer.byteInLastWord = 0;
        mInBuffer[i].buffer.flags = 0;
        mInBuffer[i].eosReached = 0;
        mInBuffer[i].nbSample = 0;
        mInBuffer[i].isEnabled = 1;
    }

    mOutBuffer.data = NULL;
    mOutBuffer.allocLen = 0;
    mOutBuffer.filledLen = 0;
    mOutBuffer.byteInLastWord = 0;
    mOutBuffer.flags = 0;
}

void METH(setParameter)(
        t_uint16 buffer_size_in,
        t_uint16 buffer_size_out,
        t_uint16 nbInputToMix,
        t_uint16 nbSampleToMix,
        t_uint16 nChannels,
        char *inputFile[8],
        char *outputFile)
{
    int i;
    t_ExtendedAllocParams  allocParams;

    mBufferSizeIn = buffer_size_in*nChannels;
    mBufferSizeOut = buffer_size_out*nChannels;
    mNbInputToMix = nbInputToMix;
    mNbSampleToMix = nbSampleToMix;

    for(i=0 ; i<nbInputToMix ; i++) {
        mFIN[i] = fopen(inputFile[i], "rb");
        ASSERT(mFIN[i] != NULL);
    }

    allocParams.bank = MEM_DDR24;
    allocParams.trace_p = NULL;
    mOutputFile = vmalloc(strlen(outputFile)+1, &allocParams);
    strcpy(mOutputFile, outputFile);

    mNbChannels = nChannels;
}


void METH(fsmInit)(fsmInit_t initFsm) {
    int i;
    t_ExtendedAllocParams  allocParams;

	// trace init (mandatory before trace init)
    FSM_traceInit(&mTester, initFsm.traceInfoAddr, initFsm.id1);

    for(i=0 ; i<mNbInputToMix ; i++) {
        //Init Wrapper Buffer In
        mFifoOut[i][0] = &mInBuffer[i].buffer;
        allocParams.bank = MEM_DDR24;
        allocParams.trace_p = NULL;
        mInBuffer[i].buffer.data = vmalloc(mBufferSizeIn, &allocParams);
        ASSERT(mInBuffer[i].buffer.data != 0);
        mInBuffer[i].buffer.allocLen = mBufferSizeIn;
        mInBuffer[i].buffer.filledLen = 0;
        mInBuffer[i].buffer.byteInLastWord = 0;
        mInBuffer[i].buffer.flags = 0;
        //Initialize Tester Output Port (binded with Wrapper Input Buffer)
        Port_init(&mPorts[OUT+i], OutputPort, true, false, 0, &mFifoOut[i], 1, &outputport[i], OUT+i,
                (initFsm.portsDisabled & (1 << (OUT+i))), (initFsm.portsTunneled & (1 << (OUT+i))), &mTester);
    }

    //Init Wrapper Buffer Out
    mFifoIn[0] = &mOutBuffer;
    allocParams.bank = MEM_DDR24;
    allocParams.trace_p = NULL;
    mOutBuffer.data = vmalloc(mBufferSizeOut, &allocParams);
    ASSERT(mOutBuffer.data != 0);
    mOutBuffer.allocLen = mBufferSizeOut;
    mOutBuffer.filledLen = 0;
    mOutBuffer.byteInLastWord = 0;
    mOutBuffer.flags = 0;

    //Initialize Tester Input Port (binded with Wrapper Output Buffer)
    Port_init(&mPorts[IN], InputPort, true, false, 0, &mFifoIn, 1, &inputport, IN,
            (initFsm.portsDisabled & (1 << (IN))), (initFsm.portsTunneled & (1 << (IN))), &mTester);

    mTester.reset                   = reset;
    mTester.process                 = process;
    mTester.disablePortIndication   = disablePortIndication;
    mTester.enablePortIndication    = enablePortIndication;
    mTester.flushPortIndication     = flushPortIndication;

    Component_init(&mTester, mNbInputToMix + 1, mPorts, &proxy);
}

void METH(setTunnelStatus) (t_sint16 portIdx, t_uint16 isTunneled) {
    // This method should not be called : treatment done in controller only
    ASSERT(0);
    Port_setTunnelStatus(&mPorts[portIdx], isTunneled);
}

void METH(stopDataTransmission)(t_uint16 portIdx) {
    mInBuffer[portIdx-OUT].isEnabled = 0;
}

void METH(startDataTransmission)(t_uint16 portIdx) {
    mInBuffer[portIdx-OUT].isEnabled = 1;
    Component_scheduleProcessEvent(&mTester);
}

void METH(restartDataFlow)() {
    strcat(mOutputFile, "2");
    // Add missing parameters
    reset(&mTester);
    process(&mTester);
}

void METH(newFormat)(
        t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize)
{
    printf("Wrapper sent new PCM settings!\n");
    printf("freq = %d, chans_nb = %d, sample_bitsize = %d\n",freq,chans_nb,sample_bitsize);
}

void METH(processEvent)() {
    Component_processEvent(&mTester);
}

void METH(emptyThisBuffer)(Buffer_p buf) {
    Component_deliverBuffer(&mTester, IN, buf);
}

void METH(fillThisBuffer)(Buffer_p buf, t_uint8 idx) {
    Component_deliverBuffer(&mTester, OUT+idx, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    t_uint16 idx = 0;
    if (cmd == OMX_CommandStateSet && ((OMX_STATETYPE)param) == OMX_StateExecuting){
        if(mSendNewFormat){
            for(idx=0 ; idx<mNbInputToMix ; idx++) {
                outputsettings[idx].newFormat(FREQ_44_1KHZ, mNbChannels, 24);
            }
            mSendNewFormat = FALSE;
        }
    }
    Component_sendCommand(&mTester, cmd, param);
}



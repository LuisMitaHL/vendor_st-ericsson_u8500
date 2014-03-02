/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   tester.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "decoders/framealigned/tester/tester.nmf"
#include <stdio.h>
#include <stdlib.h>
#include "dbc.h"
#include "string.h"
#include "fsm/component/include/Component.inl"

#define MAX_NB_BUFFER 1

#define OUT 1
#define IN  0

////////////////////////////////////////////////////////////
//			Global Variables
////////////////////////////////////////////////////////////

static Buffer_t mInBuffer[MAX_NB_BUFFER]; 
static Buffer_t mOutBuffer[MAX_NB_BUFFER];
static void *   mFifoIn[MAX_NB_BUFFER];
static void *   mFifoOut[MAX_NB_BUFFER];
static Port     mPorts[2];
Component       mTester;

static FILE *   mFIN = NULL;
static char *   mOutputFile = NULL;
static FILE *   mFOUT = NULL;

static int      mFileSize;
static int      mNbWrapperBufIn;
static int      mNbWrapperBufOut;
static int      mNbsample;
static int      mBufferSize;
static int      mCurrentwriteBuf;
static int      mCurrentdecodedBuf;
static int      mEosReached;

static t_uint16 mOutputGrain;
static t_uint16 mNbChannel;
static t_uint16 mPcmSampleSize;

////////////////////////////////////////////////////////////
// Interfaces for fsm
////////////////////////////////////////////////////////////

static void 
ReadData(Buffer_t *Buf) {
    int i, bfi, frameSize;
    int nwordsread;
    unsigned char *arr;
    unsigned short *ptrdata;

    nwordsread = fread16(&frameSize, 1, 1, mFIN);
    if(nwordsread != 1){
        Buf->filledLen = 0;
        Buf->flags |= BUFFERFLAG_EOS;
        return;
    }
    nwordsread = fread16(&bfi, 1, 1, mFIN);
    if(nwordsread != 1){
        Buf->filledLen = 0;
        Buf->flags |= BUFFERFLAG_EOS;
        return;
    }

    if( bfi == 1)
        Buf->flags |= BUFFERFLAG_DATACORRUPT;
    else
        Buf->flags &= (!BUFFERFLAG_DATACORRUPT);

    arr = malloc_ext(frameSize);

    for(i=0; i<frameSize; i++)
    {
        arr[i] = fgetc(mFIN);
        if(feof(mFIN))
        {
            break;
        }
    }
    if(frameSize != i)
    {
        printf("Last data\n");
        Buf->byteInLastWord = 1;
    }

    Buf->filledLen = i/2;   // in 16 bit word

    ptrdata = (unsigned short *)Buf->data;
    for(i=0; i<frameSize; i+=2) {
        *ptrdata++ = arr[i]<<8 | arr[i+1] ;
    }

    if( (frameSize%2) != 0){
        ptrdata--;
        *ptrdata = *ptrdata & 0xFF00 ;
        Buf->filledLen += 1; //in 16 bit word
    }

    free(arr);

}

static void initWrapperBuffersIn()
{
    int i;
    
    for(i=0; i<mNbWrapperBufIn ; i++)
    {
        mFifoOut[i] = &mInBuffer[i];
        if(mInBuffer[i].data == NULL)
        {    
            mInBuffer[i].data = malloc_ext(mBufferSize);
            ASSERT(mInBuffer[i].data != 0);
        }
        mInBuffer[i].allocLen = mBufferSize;
        mInBuffer[i].filledLen = 0;
        mInBuffer[i].byteInLastWord = 0;
        mInBuffer[i].flags = 0;
    }
}

static void initWrapperBuffersOut()
{
    int i;

    for(i=0; i<mNbWrapperBufOut ; i++)
    {
        mFifoIn[i] = &mOutBuffer[i];
        if(mOutBuffer[i].data == NULL)
        {    
            mOutBuffer[i].data = malloc_ext(mOutputGrain*mNbChannel);
            ASSERT(mOutBuffer[i].data != 0);
        }
        mOutBuffer[i].allocLen = mOutputGrain*mNbChannel;
        mOutBuffer[i].filledLen = 0;
        mOutBuffer[i].byteInLastWord = 0;
        mOutBuffer[i].flags = 0;
    }
}

static bool
checkWrapperInputBuffer(Buffer_p buf) {
    if (buf->data != mInBuffer[mCurrentwriteBuf].data) { 
        return false;
    }
    if (buf->filledLen != 0) {
        return false;
    }
    return true;
}

static void
fillInputBuffer(Buffer_p buf) {
    ReadData(buf);
	mNbsample += buf->filledLen + 2;
	if( mNbsample >= mFileSize)
    {
		buf->flags |= BUFFERFLAG_EOS;
        mEosReached = 1;
	} 
}

static bool
checkWrapperOutputBuffer(Buffer_p buf) {
    int i;
    
    if (buf->data != mOutBuffer[mCurrentdecodedBuf].data) {
        return false;
    }
    if (buf->allocLen != mOutBuffer[mCurrentdecodedBuf].allocLen) {
        return false;
    }

    return true;
}

static void
fillOutputFile(Buffer_p buf) {
    int i;
   
    if(mPcmSampleSize == 24){   
        for (i = 0; i < buf->filledLen; i++) {
            buf->data[i] = wmsr(waddsat(buf->data[i], 0x80), 8);
        }
    }
   
    fwrite16(buf->data,1, buf->filledLen, mFOUT);
    buf->filledLen = 0;
    if(buf->flags & BUFFERFLAG_EOS){
        proxy.eventHandler(OMX_EventBufferFlag, IN, buf->flags);
        rewind(mFIN);
        fclose(mFOUT);
        mFOUT = 0;
    }
}

static void
tester_reset(void) {
    mNbsample = 0;

    

    if(mFOUT){
        rewind(mFIN);
        fclose(mFOUT);
        strcat(mOutputFile, "2");
        mFOUT = 0;
    }
    printf("Open new Output File : %s\n", mOutputFile);
    mFOUT = fopen(mOutputFile, "wb");
    ASSERT(mFOUT != NULL);
}

static void
reset(Component * this){
    tester_reset();
}

static void
disablePortIndication(t_uint32 portIdx) {
}

static void
enablePortIndication(t_uint32 portIdx) {
}

static void
flushPortIndication(t_uint32 portIdx) {
}

static void
process(Component * this) {

    if (Port_queuedBufferCount(&this->ports[0])) {
        Buffer_p buf = Port_dequeueBuffer(&this->ports[0]);
        ASSERT(checkWrapperOutputBuffer(buf));
        fillOutputFile(buf);
        //printf("Tester sends buffer idx=%d to wrapper dec output!\n", mCurrentdecodedBuf);
        Port_returnBuffer(&this->ports[0], buf);
        mCurrentdecodedBuf ++;
        mCurrentdecodedBuf = (mCurrentdecodedBuf < mNbWrapperBufOut ) ? mCurrentdecodedBuf : 0;
    }

    if (mEosReached) {
        return;
    }

    if (Port_queuedBufferCount(&this->ports[1])) {
        Buffer_p buf =  Port_dequeueBuffer(&this->ports[1]);
        ASSERT(checkWrapperInputBuffer(buf));
        fillInputBuffer(buf);
        //printf("Tester sends buffer idx=%d to wrapper dec input!\n", mCurrentwriteBuf);        
        Port_returnBuffer(&this->ports[1], buf);
        mCurrentwriteBuf ++;
        mCurrentwriteBuf = (mCurrentwriteBuf < mNbWrapperBufIn ) ? mCurrentwriteBuf : 0;
    }

}


////////////////////////////////////////////////////////////
// Provided Interfaces 
////////////////////////////////////////////////////////////


void METH(start)() {
    int i;

    mCurrentwriteBuf = 0;
    mCurrentdecodedBuf = 0;
    mEosReached = 0;
    
    for(i=0; i<MAX_NB_BUFFER ; i++)
    {
        mInBuffer[i].data = NULL;
        mInBuffer[i].allocLen = 0;
        mInBuffer[i].filledLen = 0;
        mInBuffer[i].byteInLastWord = 0;
        mInBuffer[i].flags = 0;
        mOutBuffer[i].data = NULL;
        mOutBuffer[i].allocLen = 0;
        mOutBuffer[i].filledLen = 0;
        mOutBuffer[i].byteInLastWord = 0;
        mOutBuffer[i].flags = 0;
    }
}

void METH(setParameter)(
        t_uint16 fileSize,
        t_uint16 buffer_size,
        char *inputFile,
        char *outputFile)
{
    mFileSize = fileSize;
    mBufferSize = buffer_size;
    mNbWrapperBufIn = 1;
    mNbWrapperBufOut = 1;
    
    mFIN = fopen(inputFile, "rb");
    ASSERT(mFIN != NULL);

    mOutputFile = malloc_ext(strlen(outputFile)+1);
    strcpy(mOutputFile, outputFile);

    codec.getOutputGrain(&mOutputGrain,&mNbChannel);

    initWrapperBuffersIn();
    initWrapperBuffersOut();

    //Initialize Tester Output Port (binded with Wrapper Input Buffer)
    Port_init(&mPorts[1], OutputPort, true, false, 0, &mFifoOut, mNbWrapperBufIn, &outputport, 0,0,1,&mTester);

    //Initialize Tester Input Port (binded with Wrapper Output Buffer)
    Port_init(&mPorts[0], InputPort, true, false, 0, &mFifoIn, mNbWrapperBufOut, &inputport, 1,0,1,&mTester);

    mTester.reset   = reset;
    mTester.process = process;
    mTester.disablePortIndication   = disablePortIndication;
    mTester.enablePortIndication    = enablePortIndication;
    mTester.flushPortIndication     = flushPortIndication;
    Component_init(&mTester, 2, mPorts, &proxy);
}

void METH(restartDataFlow)() {
    mEosReached = 0;
    strcat(mOutputFile, "2");
    tester_reset();
    process(&mTester);
}

void METH(newFormat)(
        t_sample_freq freq, t_uint16 chans_nb, t_uint16 sample_bitsize) 
{
    printf("Wrapper sent new PCM settings!\n");
    mPcmSampleSize = sample_bitsize;
    POSTCONDITION((sample_bitsize == 16) || (sample_bitsize == 24));
}

void METH(processEvent)() {
    Component_processEvent(&mTester);
}

void METH(emptyThisBuffer)(Buffer_p buf) {
    Component_deliverBuffer(&mTester, IN, buf);
}

void METH(fillThisBuffer)(Buffer_p buf) {
    Component_deliverBuffer(&mTester, OUT, buf);
}

void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param) {
    Component_sendCommand(&mTester, cmd, param);
}

void METH(displayMemPreset)(t_uint24 XMemUsed,t_uint24 YMemUsed,t_uint24 DDR24MemUsed,t_uint24 DDR16MemUsed,t_uint24 ESR24MemUsed,t_uint24 ESR16MemUsed)
{
    printf("\nMEM USAGE XMemUsed = %d\nMEM USAGE YMemUsed = %d\nMEM USAGE DDR24MemUsed= %d\nMEM USAGE DDR16MemUsed = %d\nMEM USAGE ESR24MemUsed = %d\nMEM USAGE ESR16MemUsed = %d\n\n",XMemUsed,YMemUsed,DDR24MemUsed,DDR16MemUsed,ESR24MemUsed,ESR16MemUsed);
    
}



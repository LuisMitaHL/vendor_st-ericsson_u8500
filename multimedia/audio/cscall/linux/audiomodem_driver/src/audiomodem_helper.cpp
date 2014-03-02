/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   audiomodem_helper.cpp
 * \author  ST-Ericsson
 */
/*****************************************************************************/
#include "audiomodem_helper.hpp"

audiomodem_helper * audiomodem_helper::theHandle ; // Used to let proxy do buffer allocation. Should be remove to use hwbuffer.

#define clearMem(VAR) {char *ptr=(char*)(&VAR);for(unsigned int i=0;i<sizeof(VAR);i++) *(ptr++)=0;}

audiomodem_helper::audiomodem_helper() {
    theHandle = this;
    clearMem(mRxCond);
    clearMem(mRxMutex);
    clearMem(mTxCond);
    clearMem(mTxMutex);
    clearMem(mAudioModemFd);
    clearMem(mDownlinkModemBufferList);
    clearMem(mRxThread);
    clearMem(mTxThread);
    clearMem(mUplinkModemBufferList);
#if defined(ANDROID)&& !defined(CSCALL_ENABLE_FEATURE_CAIF)
    clearMem(mShmSocketFd);
#endif
}

#if 0
void construct() {
  theHandle = this;
}

audiomodem_helper::audiomodem_helper(Iarmnmf_emptythisbuffer * outputport, Iarmnmf_fillthisbuffer  * inputport) {
	this->outputport = outputport ;
	this->inputport  = inputport  ;
	running = true ;
	rxBufferFifoIdx = 0 ;
	txBufferFifoIdx = 0 ;
} /* audiomodem_helper */


void audiomodem_helper::ConstructL() {
	// Create Semaphore to synchronize with new thread
	User::LeaveIfError(synchronizer.CreateLocal(0));

	// Spawn rx thread - thread shares the parent thread heap
	TThreadFunction threadFunction = (TThreadFunction) _init_rxthread_routine;
    User::LeaveIfError(rxThread.Create(_LIT_rx_threadname, threadFunction, (TInt) 8096, NULL, (TAny*) this));
    rxThread.SetPriority(RXThreadPriority);
    rxThread.Resume();

	// Spawn tx thread - thread shares the parent thread heap
	threadFunction = (TThreadFunction) _init_txthread_routine;
    User::LeaveIfError(txThread.Create(_LIT_tx_threadname, threadFunction, (TInt) 8096, NULL, (TAny*) this));
    txThread.SetPriority(TXThreadPriority);
    txThread.Resume();
} /* ConstructL */


void audiomodem_helper::startTransmit(OMX_BUFFERHEADERTYPE * buffer) {
	txBufferFifo[txBufferFifoIdx] = buffer ;
	*((TUint32 *)(buffer->pBuffer+KAMRUplinkBufferSize)) = (MODEM_AUD_LB_MSG | buffer->nFilledLen);
	iUpLink.Transmit(txStatus[txBufferFifoIdx], buffer->pBuffer, buffer->nFilledLen, buffer->pBuffer+KAMRUplinkBufferSize, KL2HeaderSize);

	if (++txBufferFifoIdx == AUDIOMODEM_HELPER_FIFOSIZE) txBufferFifoIdx = 0 ;
}


void audiomodem_helper::startReceive(OMX_BUFFERHEADERTYPE  * buffer) {
	rxBufferFifo[rxBufferFifoIdx] = buffer ;

	if (!nbRxBuffersTodo) { iDownLink.ReceiveComplete(buffer->pBuffer, (TInt)buffer->pOutputPortPrivate); }
	else nbRxBuffersTodo-- ;

	iDownLink.Receive(rxStatus[rxBufferFifoIdx], buffer->pBuffer, rxSize[rxBufferFifoIdx], (TUint8*)&iHeader, iHeaderSize);
	if (++rxBufferFifoIdx == AUDIOMODEM_HELPER_FIFOSIZE) rxBufferFifoIdx = 0 ;
}

audiomodem_helper::~audiomodem_helper() {
	running = false ;
	rxThread.RequestSignal() ;
	txThread.RequestSignal() ;

	rxThread.Close() ;
	txThread.Close() ;


	NMF_LOG("Waiting for audiomodem_helper thread\n") ;
	waitServerThreads() ;
	NMF_LOG("audiomodem_helper down\n") ;
	synchronizer.Close();
}
#endif

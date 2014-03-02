/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ModemBufferList.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ModemBufferList.hpp"

#include "audiomodem_driver.nmf"

#include <stdio.h>
#include <assert.h>
#include <string.h>

#define clearMem(VAR) {char *ptr=(char*)(&VAR);for(unsigned int i=0;i<sizeof(VAR);i++) *(ptr++)=0;}

ModemBufferList::ModemBufferList(unsigned int nbSlots, eBufferMode dir) {

  mNbElements = nbSlots;

  for (unsigned int i = 0; i < AUDIO_MODEM_NB_MAX_MESSAGES_IN_QUEUE; i++) {
    mState[i] = BUFFER_UNUSED;

    // Temporary
    mStateWatcher[i] = BUFFER_UNUSED;

    memset(&mOMXBuffers[i], 0, sizeof(OMX_BUFFERHEADERTYPE));
  }

  mToBeProcessedCurrId = -1;
  mProcessedCurrId = -1;
  mMode = dir;
  mNbReqReceived  = 0;
  mNbReqCompleted = 0;
  mNbAvailableElements = 0;

  // set to true if you want to activate Trace
  mTraceEnable = false;

  clearMem(mListMutex);
}

bool ModemBufferList::init(unsigned int bufferSize) {

  for (unsigned int i=0; i< mNbElements; i++) {
    // allocate the message buffers
    OMX_U8 * ptr = (OMX_U8 *) new char[bufferSize];
    if (ptr == NULL) {
      ALOGD("BuferList: Unable to allocate buffer\n");
      return false;
    }
    if (mMode == AUDIOMODEM_UL) {
      mOMXBuffers[i].pInputPortPrivate = (OMX_PTR)i;
    } else {
      mOMXBuffers[i].pOutputPortPrivate = (OMX_PTR)i;
    }

    mOMXBuffers[i].pBuffer = ptr;
  }

  pthread_mutexattr_t attr;
  int err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
  if (err != 0) return false;

  err = pthread_mutexattr_init(&attr);
  if (err != 0) return false;

  err = pthread_mutex_init(&mListMutex, &attr);
  if (err != 0) return false;

  return true;
}

void ModemBufferList::markAsToBeProcessed(OMX_BUFFERHEADERTYPE * buf) {

  unsigned int bufId;

  if (mMode == AUDIOMODEM_UL) {
    bufId = (unsigned int) buf->pInputPortPrivate;
  } else {
    bufId = (unsigned int) buf->pOutputPortPrivate;
  }

  // protect the update of the list
  pthread_mutex_lock(&mListMutex);
  assert(mState[bufId] == BUFFER_UNUSED);
  mNbReqReceived++;

  assert(bufId<mNbElements);
  mState[bufId] = BUFFER_TO_BE_PROCESSED;

  mStateWatcher[bufId] = BUFFER_TO_BE_PROCESSED;


  if (mToBeProcessedCurrId == -1) {
    mToBeProcessedCurrId = bufId;
  }

  if(mTraceEnable){
      if (mMode == AUDIOMODEM_DL) 
          ALOGD("matbp: s[%d]:%d b:%x\n",bufId,mState[bufId],(unsigned int)buf);
  }
  pthread_mutex_unlock(&mListMutex);
}

void ModemBufferList::markAsProcessed(OMX_BUFFERHEADERTYPE * buf) {

  unsigned int bufId;

  if (mMode == AUDIOMODEM_UL) {
    bufId = (unsigned int) buf->pInputPortPrivate;
  } else {
    bufId = (unsigned int) buf->pOutputPortPrivate;
  }

  // protect the update of the list
  pthread_mutex_lock(&mListMutex);
  // the lifecycle of a buffer should be unused->tobeprocessed>processed
  assert(mState[bufId] == BUFFER_TO_BE_PROCESSED);
  mNbReqCompleted++;

  mState[bufId] = BUFFER_PROCESSED;
  if (mProcessedCurrId == -1) {
    mProcessedCurrId = bufId;
  }
  if(mTraceEnable){
      if (mMode == AUDIOMODEM_DL) 
          ALOGD("map: s[%d]:%d b:%x\n",bufId,mState[bufId], (unsigned int)buf);
  }

  pthread_mutex_unlock(&mListMutex);
}

void ModemBufferList::markAsUnused(OMX_BUFFERHEADERTYPE * buf) {

  unsigned int bufId;

  if (mMode == AUDIOMODEM_UL) {
    bufId = (unsigned int) buf->pInputPortPrivate;
  } else {
    bufId = (unsigned int) buf->pOutputPortPrivate;
  }

  // protect the update of the list
  pthread_mutex_lock(&mListMutex);
  // the lifecycle of a buffer should be unused->tobeprocessed>processed
  assert(mState[bufId] == BUFFER_TO_BE_PROCESSED);
  mState[bufId] = BUFFER_UNUSED;

  if(mTraceEnable){
     ALOGD("mau: s[%d]:%d b:%x\n",bufId,mState[bufId], (unsigned int)buf);
  }

  pthread_mutex_unlock(&mListMutex);
}

OMX_BUFFERHEADERTYPE * ModemBufferList::getNextProcessedBuffer() {

  OMX_BUFFERHEADERTYPE * buff;
  unsigned int nextId;

  pthread_mutex_lock(&mListMutex);

  // circular index management
  nextId = mProcessedCurrId + 1;
  if (nextId >= mNbElements) {
    nextId = 0;
  }

  // does any process buffer exist
  if (mProcessedCurrId == -1) {
    buff = NULL;
  } else if (mState[mProcessedCurrId] != BUFFER_PROCESSED) {
    //  check if this slot is in the right state...
    ALOGD("ModemList::getNextProcessedBuffer is not processed %d [state: %d]\n", 
	   mProcessedCurrId,
	   mState[mProcessedCurrId]);
    buff = NULL;
    logBufferListInfo();
    mTraceEnable = true;

  } else {
    buff = &mOMXBuffers[mProcessedCurrId];
  }
  
  unsigned int index  = nextId, i;

  for (i = 0; i < mNbElements; i++, index++) {
    // update the index to the next buffer
    if (index >= mNbElements) {
      index = 0;
    }

    // check if we find another buffer of the right type 
    // (not the one we are asked for)
    if ((mState[index] == BUFFER_PROCESSED)  && (index !=  (unsigned int)mProcessedCurrId)) {
      break;
    }
  }

  if (i == mNbElements) {
    // no other buffer of this kind was found
    mProcessedCurrId = -1;
  } else {
    mProcessedCurrId = index;    
  }
  pthread_mutex_unlock(&mListMutex);
  return buff;
}

OMX_BUFFERHEADERTYPE * ModemBufferList::getNextToBeProcessedBuffer() {

  OMX_BUFFERHEADERTYPE * buff;
  unsigned int nextId;

  pthread_mutex_lock(&mListMutex);

  // circular index management
  nextId = mToBeProcessedCurrId + 1;
  if (nextId >= mNbElements) {
    nextId = 0;
  }

  // check if this slot is in the right state...
  if (mToBeProcessedCurrId == -1) {
    buff = NULL;
  } else if (mState[mToBeProcessedCurrId] != BUFFER_TO_BE_PROCESSED) {
    ALOGD("ModemList::getNextToBeProcessedBuffer is not to be processed %d  [state: %d]\n", 
	   mToBeProcessedCurrId, 
	   mState[mToBeProcessedCurrId]);
    buff = NULL;
    logBufferListInfo();
    mTraceEnable = true;
  } else {
    buff = &mOMXBuffers[mToBeProcessedCurrId];
  }
  
  unsigned int index  = nextId, i;

  for (i = 0; i < mNbElements; i++, index++) {
    // update the index to the next buffer
    if (index >= mNbElements) {
      index = 0;
    }

    // check if we find another buffer of the right type 
    // (not the one we are asked for)
    if ((mState[index] == BUFFER_TO_BE_PROCESSED) && (index !=  (unsigned int)mToBeProcessedCurrId)) {
      break;
    }
  }
  
  if (i == mNbElements) {
    // no other buffer of this kind was found
    mToBeProcessedCurrId = -1;
  } else {
    mToBeProcessedCurrId = index;    
  }

  pthread_mutex_unlock(&mListMutex);
  return buff;
}


OMX_BUFFERHEADERTYPE * ModemBufferList::getBuffer(unsigned int index) {
  return (&mOMXBuffers[index]);
}

void ModemBufferList::logBufferListInfo() {

  unsigned int i;

//  pthread_mutex_lock(&mListMutex);

  ALOGD("mMode=%d\n",mMode);
  ALOGD("mNbReqReceived=%ld mNbReqCompleted=%ld\n",mNbReqReceived,mNbReqCompleted);

  ALOGD("mToBeProcessedCurrId = %d\n",mToBeProcessedCurrId);
  ALOGD("mProcessedCurrId = %d\n",mProcessedCurrId);

  for (i = 0; i < mNbElements; i++)
  {
     ALOGD("mState[%d]=%d\n",i,mState[i]);
     ALOGD("mStateWatcher[%d]=%d\n",i,mStateWatcher[i]);
     ALOGD("&mOMXBuffers[%d]=0x%x\n",i,(unsigned int)&mOMXBuffers[i]);
  }

//  pthread_mutex_unlock(&mListMutex);
  return;
}

bool ModemBufferList::isAnyToBeProcessedBuffer() {

    unsigned int i, index = 0;
    bool isAnyBuf =  false;

    pthread_mutex_lock(&mListMutex);

    for (i = 0; i < mNbElements; i++, index++) {

        // check if we find another buffer of the right type 
        if (mState[index] == BUFFER_TO_BE_PROCESSED) {
            break;
        }
    }
    if(index == mNbElements)
        // No element found
        isAnyBuf = false;
    else
    {
        // Found one buffer to be sent
        mToBeProcessedCurrId = index;
        if (mMode == AUDIOMODEM_DL){
            if(mTraceEnable)
               ALOGD("iatbp: s[%d]:%d b:%x\n",index,mState[index], (unsigned int)&mOMXBuffers[index]);
        }

        isAnyBuf = true;
    }

    pthread_mutex_unlock(&mListMutex);

    return isAnyBuf;
}

unsigned int ModemBufferList::getNbToBeProcessedBuffer() {

    unsigned int i, index = 0;

    for (i = 0; i < mNbElements; i++) {

        // check if we find another buffer of the right type 
        if (mState[index] == BUFFER_TO_BE_PROCESSED) {
            index++;
        }
    }

    return index;
}

void ModemBufferList::updateNbAvailableToBeProcessedBuffer() {

    unsigned int i, index = 0;

    pthread_mutex_lock(&mListMutex);

    for (i = 0; i < mNbElements; i++) {

        // check if we find another buffer of the right type 
        if (mState[index] == BUFFER_TO_BE_PROCESSED) {
            index++;
        }
    }
    // New Available buffer value
    mNbAvailableElements = index;

    pthread_mutex_unlock(&mListMutex);

    return;
}




ModemBufferList::~ModemBufferList(){
  for (unsigned int i=0; i< mNbElements; i++) {
    if (mOMXBuffers[i].pBuffer != NULL)
      delete [] mOMXBuffers[i].pBuffer;
  }
}


#ifdef TEST

#define BUFFER_LIFECYLE(a) {\
    buf = mDownlinkModemBufferList->getNextToBeProcessedBuffer();	\
    assert(buf != NULL);						\
    assert(((int)buf->pOutputPortPrivate) == a);			\
    mDownlinkModemBufferList->markAsProcessed(buf);			\
    buf = mDownlinkModemBufferList->getNextProcessedBuffer();		\
    assert(((int)buf->pOutputPortPrivate) == a);			\
    mDownlinkModemBufferList->markAsUnused(buf);}

int main() {
  
  // create the OMX buffers and the room for modem messages
  ModemBufferList * mDownlinkModemBufferList = new ModemBufferList(DL_NB, AUDIOMODEM_DL);

  //  ModemBufferList mUplinkModemBufferList = new ModemBufferList(UL_NB, AUDIOMODEM_UL);

  if (!mDownlinkModemBufferList->init(AUDIO_MODEM_MAX_MESSAGE_SIZE)) return -1;
  //  if (!mUplinkModemBufferList->init(AUDIO_MODEM_MAX_MESSAGE_SIZE)) return -1;

  int nbBuffers = 4;
  OMX_BUFFERHEADERTYPE * buf;

  ALOGD("#######test 1\n");

  while (nbBuffers--) {
    // mark any dequeued buffer as to be processed
    mDownlinkModemBufferList->markAsToBeProcessed(mDownlinkModemBufferList->getBuffer(3-nbBuffers));
  }

  int id = 0;
  while ((buf = mDownlinkModemBufferList->getNextToBeProcessedBuffer()) != NULL) {
    assert(((int)buf->pOutputPortPrivate) == id);
    mDownlinkModemBufferList->markAsProcessed(buf);
    id++;
  }

  id = 0;
  while ((buf = mDownlinkModemBufferList->getNextProcessedBuffer()) != NULL) {
    assert(((int)buf->pOutputPortPrivate) == id);
    mDownlinkModemBufferList->markAsUnused(buf);
    id++;
  }

  assert(mDownlinkModemBufferList->getNextProcessedBuffer() == NULL);
  assert(mDownlinkModemBufferList->getNextToBeProcessedBuffer() == NULL);

  ALOGD("########test 2\n");

  nbBuffers = 3;
  while (nbBuffers--) {
    // mark any dequeued buffer as to be processed
    mDownlinkModemBufferList->markAsToBeProcessed(mDownlinkModemBufferList->getBuffer(2-nbBuffers));
  }

  id = 0;
  // get buffer 0
  buf = mDownlinkModemBufferList->getNextToBeProcessedBuffer();
  assert(buf != NULL);
  assert(((int)buf->pOutputPortPrivate) == 0);
  // insert buffer 3 in the loop
  mDownlinkModemBufferList->markAsToBeProcessed(mDownlinkModemBufferList->getBuffer(3));
  mDownlinkModemBufferList->markAsProcessed(buf);
  buf = mDownlinkModemBufferList->getNextProcessedBuffer();
  assert(((int)buf->pOutputPortPrivate) == 0);
  mDownlinkModemBufferList->markAsUnused(buf);

  buf = mDownlinkModemBufferList->getNextToBeProcessedBuffer();
  assert(buf != NULL);
  assert(((int)buf->pOutputPortPrivate) == 1);
  // emulate threads by marking the unused slot 0 as to be processed
  mDownlinkModemBufferList->markAsToBeProcessed(mDownlinkModemBufferList->getBuffer(0));
  // continue buffer 1 lifecycle
  mDownlinkModemBufferList->markAsProcessed(buf);
  buf = mDownlinkModemBufferList->getNextProcessedBuffer();
  assert(((int)buf->pOutputPortPrivate) == 1);
  mDownlinkModemBufferList->markAsUnused(buf);

  // buffer 2 lifecycle
  BUFFER_LIFECYLE(2);
  BUFFER_LIFECYLE(3);
  // check that we access the buffer 0
  BUFFER_LIFECYLE(0);

  ALOGD("########test 3\n");

  nbBuffers = 4;
  while (nbBuffers--) {
    // mark any dequeued buffer as to be processed
    mDownlinkModemBufferList->markAsToBeProcessed(mDownlinkModemBufferList->getBuffer(3-nbBuffers));
  }

  // buffers lifecycle
  BUFFER_LIFECYLE(0);
  BUFFER_LIFECYLE(1);
  BUFFER_LIFECYLE(2);

  buf = mDownlinkModemBufferList->getNextToBeProcessedBuffer();
  assert(buf != NULL);
  assert(((int)buf->pOutputPortPrivate) == 3);
  // emulate threads by marking the unused slot 0 as to be processed
  mDownlinkModemBufferList->markAsToBeProcessed(mDownlinkModemBufferList->getBuffer(0));
  mDownlinkModemBufferList->markAsToBeProcessed(mDownlinkModemBufferList->getBuffer(1));
  mDownlinkModemBufferList->markAsToBeProcessed(mDownlinkModemBufferList->getBuffer(2));
  // continue buffer 3 lifecycle
  mDownlinkModemBufferList->markAsProcessed(buf);
  buf = mDownlinkModemBufferList->getNextProcessedBuffer();
  assert(((int)buf->pOutputPortPrivate) == 3);
  mDownlinkModemBufferList->markAsUnused(buf);

  // check for previously inserted buffer lifecycle
  BUFFER_LIFECYLE(0);
  BUFFER_LIFECYLE(1);
  BUFFER_LIFECYLE(2);

  ALOGD("ModemBufferList tests ok\n");
  return 0;
}
#endif

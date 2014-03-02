/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ModemBufferList.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __MODEMBUFFERLIST_HPP__
#define __MODEMBUFFERLIST_HPP__

#include "audiomodem_driver_defines.h"
#include <pthread.h>
#include "OMX_Core.h"

class ModemBufferList {

public:
  ModemBufferList(unsigned int nbSlots, eBufferMode dir);
  ~ModemBufferList();

  bool init(unsigned int bufferSize);

  void markAsToBeProcessed(OMX_BUFFERHEADERTYPE * buff);
  void markAsProcessed(OMX_BUFFERHEADERTYPE * buff);
  void markAsUnused(OMX_BUFFERHEADERTYPE * buff);

  OMX_BUFFERHEADERTYPE * getNextProcessedBuffer();
  OMX_BUFFERHEADERTYPE * getNextToBeProcessedBuffer();

  OMX_BUFFERHEADERTYPE * getBuffer(unsigned int i);
  //  OMX_BUFFERHEADERTYPE * getBufferList() { return mOMXBuffers;};
  unsigned int           getBufferCount() { return mNbElements; };
  unsigned int           getAvailableBufferCount() { return mNbAvailableElements; };

  
  void                   logBufferListInfo();
  bool                   isAnyToBeProcessedBuffer();
  unsigned int           getNbToBeProcessedBuffer(); 
  void                   updateNbAvailableToBeProcessedBuffer(); 
  bool                   isTraceEnabled()  {return mTraceEnable;};
  void                   setTraceEnabled(bool val) { mTraceEnable = val;};

private:
  // modem buffer OMX headers
  OMX_BUFFERHEADERTYPE mOMXBuffers[AUDIO_MODEM_NB_MAX_MESSAGES_IN_QUEUE];
  // state of the buffers (true means to be processed, 
  // false means processed
  eBufferState mState[AUDIO_MODEM_NB_MAX_MESSAGES_IN_QUEUE];

  // state of the buffers, check buffer evolution 
  eBufferState mStateWatcher[AUDIO_MODEM_NB_MAX_MESSAGES_IN_QUEUE];

  // room allocated for each modem message content
  //  char ** buffers;
  // index to the latest buffer to be processed
  int mToBeProcessedCurrId;
  // index to the latest buffer to be processed
  int mProcessedCurrId;
  // number of buffers in the list
  unsigned int mNbElements;
  // number of available buffers in the list
  unsigned int mNbAvailableElements;
  // Number of requet received
  unsigned long mNbReqReceived;
  // Number of requet completed
  unsigned long mNbReqCompleted;
  // Dynamic Trace Activation
  bool mTraceEnable;
  pthread_mutex_t mListMutex;

  eBufferMode mMode;

};

#endif // __MODEMBUFFERLIST_HPP__

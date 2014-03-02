/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   audiomodem_helper.hpp
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#ifndef __audiomodem_helper
#define __audiomodem_helper

#include "ModemBufferList.hpp"

class audiomodem_helper {
public:

  static audiomodem_helper* getHandle() { return theHandle ; } // Used to let proxy do buffer allocation.

  audiomodem_helper();
  ~audiomodem_helper() { theHandle = NULL;};

  
  ModemBufferList * getDownlinkModemBufferList() { return mDownlinkModemBufferList;};
  ModemBufferList * getUplinkModemBufferList() { return mUplinkModemBufferList;};
  
  ModemBufferList * mDownlinkModemBufferList;
  ModemBufferList * mUplinkModemBufferList;

  pthread_t           mRxThread; 
  pthread_t           mTxThread; 

  pthread_cond_t      mTxCond,  mRxCond;
  pthread_mutex_t     mTxMutex,  mRxMutex;
  int                 mAudioModemFd;
#if defined(ANDROID) &&  !defined(CSCALL_ENABLE_FEATURE_CAIF)
  int32_t             mShmSocketFd;
#endif

private:
  static audiomodem_helper * theHandle;
};


#endif /* __audiomodem_helper */

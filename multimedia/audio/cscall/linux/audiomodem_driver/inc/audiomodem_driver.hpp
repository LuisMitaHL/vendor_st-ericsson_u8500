/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   audiomodem_driver.hpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __audiomodem_driver_hpp__
#define __audiomodem_driver_hpp__

#include "audiomodem_driver_defines.h"
#include "audiomodem_helper.hpp"

#include "ModemBufferList.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#define LOG_TAG "audiomodem"
#include "linux_utils.h"
#if defined(ANDROID) && !defined(CSCALL_ENABLE_FEATURE_CAIF)
#ifdef __cplusplus
  extern "C" {
    #include <shm_netlnk.h>
  }
#endif
#endif



class audiomodem_driver : public audiomodem_driverTemplate
{

public:
  audiomodem_driver() { mIsFinishing = false; mIsModemResetting = false; };
  virtual t_nmf_error construct();
  virtual void        destroy();

  virtual void cancelRequests();

  virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
  virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

  void signalDownlinkAction();
  void waitDownlinkAction();

  void signalUplinkAction();
  void waitUplinkAction();

  pthread_cond_t * getUplinkSignalingCond() { return &amh->mTxCond; };
  pthread_cond_t * getDownlinkSignalingCond() { return &amh->mRxCond; };

  ModemBufferList * getDownlinkModemBufferList() { return amh->mDownlinkModemBufferList;};
  ModemBufferList * getUplinkModemBufferList() { return amh->mUplinkModemBufferList;};
  pthread_mutex_t * getDownlinkSignalingMutex() { return &amh->mRxMutex;};
  pthread_mutex_t * getUplinkSignalingMutex() { return &amh->mTxMutex;};
  int getModemFd() { return amh->mAudioModemFd;};
  bool isFinishing() { return mIsFinishing;};
#if defined(ANDROID) && !defined(CSCALL_ENABLE_FEATURE_CAIF)
  int32_t getShmFd() { return amh->mShmSocketFd;};
#endif
  bool isModemResetting() { return mIsModemResetting; }
  void ModemResetting(bool reset) { mIsModemResetting = reset; }

  int openModemFd(bool modemReboot, bool checkStatus);
  void closeModemFd(bool modemReboot);

private:
  audiomodem_helper * amh;
  bool                mIsFinishing;
  bool                mIsModemResetting;
};

#endif /* __audiomodem_driver_hpp__ */

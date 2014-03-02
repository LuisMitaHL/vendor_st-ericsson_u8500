/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   audiomodem_driver.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "audiomodem_driver.nmf"
#include "audiomodem_driver_threads.h"
#include "cscall_config.h"

#include <signal.h>
#include <unistd.h>
#include <errno.h>
#if defined(ANDROID)  && !defined(CSCALL_ENABLE_FEATURE_CAIF)
#include <stdint.h>
#endif

#if defined(CSCALL_ENABLE_FEATURE_CAIF)
#define AF_CAIF 37
#define PF_CAIF AF_CAIF
#define SOL_CAIF 278
#include <linux/caif/caif_socket.h>
#endif

t_nmf_error METH(construct)() {
    ALOGD("audiomodem_driver construct\n") ;

    amh = new audiomodem_helper(/*&outputport, &inputport*/);
    if (amh == NULL) return NMF_INVALID_PARAMETER ;

    if(openModemFd(false, false) < 0)
    {
      return NMF_INVALID_PARAMETER;
    }

    // create the OMX buffers and the room for modem messages
    amh->mDownlinkModemBufferList = new ModemBufferList(NB_DL_MODEM_BUFFER, AUDIOMODEM_DL);

    amh->mUplinkModemBufferList = new ModemBufferList(NB_UL_MODEM_BUFFER, AUDIOMODEM_UL);

    if (!amh->mDownlinkModemBufferList->init(AUDIO_MODEM_MAX_MESSAGE_SIZE)) return NMF_INVALID_PARAMETER;
    if (!amh->mUplinkModemBufferList->init(AUDIO_MODEM_MAX_MESSAGE_SIZE)) return NMF_INVALID_PARAMETER;

    {
        int thread_error;
        pthread_attr_t attr;
        struct sched_param sched_param;
        pthread_attr_init(&attr);

        // init the condition variables
        pthread_cond_init(&amh->mTxCond, NULL);
        pthread_cond_init(&amh->mRxCond, NULL);

        pthread_mutexattr_t mattr;
        int err = pthread_mutexattr_init(&mattr);
        if (err !=0) return NMF_INVALID_PARAMETER;

        err = pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
        if (err !=0) return NMF_INVALID_PARAMETER;

        err = pthread_mutex_init(&amh->mRxMutex, &mattr);
        if (err !=0) return NMF_INVALID_PARAMETER;

        err = pthread_mutex_init(&amh->mTxMutex, &mattr);
        if (err !=0) return NMF_INVALID_PARAMETER;

        // get max priority from the scheduler
        int base_priority = sched_get_priority_max(SCHED_RR);
        err = pthread_attr_init(&attr);
        if (err !=0 ) return NMF_INVALID_PARAMETER;

#ifndef ANDROID
        err = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (err) {
            ALOGE("Error setting sched-inherit policy for thread: %d\n", err);
            return NMF_INVALID_PARAMETER;
        }
#endif

        err = pthread_attr_setschedpolicy(&attr, SCHED_RR);
        if (err !=0) return NMF_INVALID_PARAMETER;

        // set the priority to MAX_SCHEDULER_PRIORITY_NUMBER
        sched_param.sched_priority = base_priority ;

        err = pthread_attr_setschedparam(&attr, &sched_param);
        if (err !=0) return NMF_INVALID_PARAMETER;

        // create RX and TX threads
        thread_error = pthread_create(&amh->mRxThread, &attr,
                &RxThreadEntry, this);

        if (thread_error != 0) {
            ALOGE("Failed to create the RX thread\n");
            return NMF_INVALID_PARAMETER;
        }

        thread_error = pthread_create(&amh->mTxThread, &attr,
                &TxThreadEntry, this);

        if (thread_error != 0) {
            ALOGE("Failed to create the TX thread\n");
            return NMF_INVALID_PARAMETER;
        }
    }

    ALOGD("amc construct over...\n");
    return NMF_OK ;
}

void METH(destroy)() {
    ALOGD("audiomodem_driver destroy\n") ;


    /* cleanup the threads we've spawned...*/
    /* signal to the threads they can stop */
    /* isFinishing variable is not protected,
       but it should not provoke any race condition... */
    mIsFinishing = true;
    signalUplinkAction();
    signalDownlinkAction();

    /*  wait for the spawned threads to finish */
    pthread_join(amh->mRxThread, NULL);
    pthread_join(amh->mTxThread, NULL);

    delete amh->mUplinkModemBufferList;
    delete amh->mDownlinkModemBufferList;

    closeModemFd(false);

    delete amh;
}

void METH(fillThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer) {
    amh->mDownlinkModemBufferList->markAsToBeProcessed(buffer);
    if (amh->mDownlinkModemBufferList->isTraceEnabled() )
        ALOGD("amc process: signal the rx thread\n");

    // forward job to the transmit thread
    signalDownlinkAction();
}

void METH(emptyThisBuffer)(OMX_BUFFERHEADERTYPE_p buffer) {
    // mark any dequeued buffer as to be processed
    amh->mUplinkModemBufferList->markAsToBeProcessed(buffer);
    if(amh->mUplinkModemBufferList->isTraceEnabled() )
        ALOGD("amc process: signal the tx thread\n");
    // forward job to the receive thread
    signalUplinkAction();
}

void METH(cancelRequests)() {
    mIsFinishing = true;
    ALOGD("Cancel Request");
    // Signal to the ul and dl threads that we're done...
    signalUplinkAction();

    if(pthread_kill(amh->mRxThread, SIGUSR1))
    {
      ALOGE("Failed to send signal SIGUSR1 %d", errno);
    }
}

void METH(signalDownlinkAction)() {
    int err = pthread_mutex_lock(getDownlinkSignalingMutex());
    if (err !=0) {
        ALOGE("Error while unlocking the rx mutex: %d\n", err);
    }
    err = pthread_cond_signal(getDownlinkSignalingCond());
    if (err !=0) {
        ALOGE("Error while unlocking the rx mutex: %d\n", err);
    }
    err = pthread_mutex_unlock(getDownlinkSignalingMutex());
    if (err !=0) {
        ALOGE("Error while unlocking the rx mutex: %d\n", err);
    }
}

void METH(waitDownlinkAction)() {
    int err = pthread_mutex_lock(getDownlinkSignalingMutex());
    if (err !=0) {
        ALOGE("Error while locking the rx mutex: %d\n", err);
    }

    if (!amh->mDownlinkModemBufferList->isAnyToBeProcessedBuffer())
    {
      err = pthread_cond_wait(getDownlinkSignalingCond(), getDownlinkSignalingMutex());
      if (err !=0) {
        ALOGE("Error while waiting the rx cond: %d\n", err);
      }
    }

    // the signaling can be re-done
    err = pthread_mutex_unlock(getDownlinkSignalingMutex());
    if (err !=0) {
        ALOGE("Error while unlocking the rx mutex: %d\n", err);
    }
}

void METH(signalUplinkAction)() {
    int err = pthread_mutex_lock(getUplinkSignalingMutex());
    if (err !=0) {
        ALOGE("Error while locking the tx mutex: %d\n", err);
    }
    err = pthread_cond_signal(getUplinkSignalingCond());
    if (err !=0) {
        ALOGE("Error while signaling the tx cond: %d\n", err);
    }
    err = pthread_mutex_unlock(getUplinkSignalingMutex());
    if (err !=0) {
        ALOGE("Error while unlocking the rx mutex: %d\n", err);
    }
}

void METH(waitUplinkAction)() {
    int err = pthread_mutex_lock(getUplinkSignalingMutex());
    if (err !=0) {
        ALOGE("Error while locking the tx mutex: %d\n", err);
    }
    err = pthread_cond_wait(getUplinkSignalingCond(), getUplinkSignalingMutex());
    if (err !=0) {
        ALOGE("Error while waiting the tx cond: %d\n", err);
    }
    // the signaling can be re-done
    err = pthread_mutex_unlock(getUplinkSignalingMutex());
    if (err !=0) {
      ALOGE("Error while unlocking the tx mutex: %d\n", err);
    }
}



int audiomodem_driver::openModemFd(bool modemReboot, bool checkStatus)
{
#if defined(CSCALL_ENABLE_FEATURE_CAIF)
  struct sockaddr_caif addr;
  addr.family = AF_CAIF;
  strcpy(addr.u.util.service, "SPEECH");

  char initMessage[] = {0, 0};

  /* Create a CAIF socket for Utility Service */
  amh->mAudioModemFd = socket(PF_CAIF, SOCK_SEQPACKET, CAIFPROTO_UTIL);

  if(amh->mAudioModemFd < 0)
  {
    ALOGE("audiomodem_driver: Cannot open socket [%d] ", errno);
    return -1;
  }

  /* Connect socket */
  if(connect(amh->mAudioModemFd, (struct sockaddr *) & addr, sizeof(addr))==-1)
    ALOGE("audiomodem_driver: CAIF NOT connected !!!! : %d\n", amh->mAudioModemFd);
  else
    ALOGI("audiomodem_driver: CAIF connected : %d\n", amh->mAudioModemFd);

#else // CSCALL_ENABLE_FEATURE_CAIF
  // delay open on dev/audiomodem in rx thread
  // (i.e. call with modemReboot = true)
  if(modemReboot)
  {
    int audiomodem_fd = -1;
    int retval = 0;
#if defined(ANDROID)
    int shmFdMsg;

    while((audiomodem_fd < 0) && (retval >= 0) && !isFinishing())
    {
      if(checkStatus)
      {
        retval = netlnk_socket_send(amh->mShmSocketFd, MODEM_QUERY_STATUS_REQ);
        if(retval >= 0)
        {
          retval = netlnk_socket_recv(amh->mShmSocketFd, &shmFdMsg);
          if (retval < 0)
          {
            ALOGE("Rx Thread: netlnk_socket_recv() error=%d\n", retval);
            retval = 0;
          }
          else if (shmFdMsg == MODEM_STATUS_ONLINE)
          {
            checkStatus = false;
          }
        }
      }

      if(!checkStatus)
      {
        audiomodem_fd = open("/dev/modemaudio", O_RDWR);
        if(audiomodem_fd < 0) checkStatus = true;
      }
    }

    // This should never happen, but just in case
    // we also handled this error case
    if(retval < 0 && !isFinishing())
    {
      ALOGD("Rx Thread : fail to send message to socket, simply loop until we can open [%d]",errno);
      while((audiomodem_fd == -1) && !isFinishing())
      {
        audiomodem_fd = open("/dev/modemaudio", O_RDWR);
        if(audiomodem_fd < 0) sleep(2);
      }
    }

#else

    ALOGD("Rx Thread : waiting for modem");
    while((audiomodem_fd == -1) && !isFinishing())
    {
      audiomodem_fd = open("/dev/modemaudio", O_RDWR);
      if(audiomodem_fd < 0) sleep(2);
    }
#endif

    amh->mAudioModemFd = audiomodem_fd;
    ALOGD("Modem device opened");
  }
  else // if call from construct do nothing
  {
    amh->mAudioModemFd = -1;
  }

#if defined(ANDROID)
  if(!modemReboot)
  {
    // set up comunication with SHM_NETLNK driver for modem silent reboot
    int32_t netlnk_error = 0;
    int32_t ShmSocketFd = -1;
    netlnk_error = netlnk_socket_create(&ShmSocketFd);
    if (netlnk_error < 0 || ShmSocketFd ==-1)
    {
      ALOGE("audiomodem_driver: Error: Unable to open the SHM netlnk\n");
      close(amh->mAudioModemFd);
      return -1;
    }
    else
    {
      amh->mShmSocketFd = ShmSocketFd;
    }
  }
#endif
#endif

  return 0;
}


void audiomodem_driver::closeModemFd(bool modemReboot)
{
  int res;

  if(amh->mAudioModemFd >= 0)
  {
    res = close(amh->mAudioModemFd);
    if (res != 0)
    {
      ALOGE("Modem audio driver closure failed [%d]\n", errno);
    }
    amh->mAudioModemFd = -1;
  }

#if !defined(CSCALL_ENABLE_FEATURE_CAIF) && defined(ANDROID)
  if(!modemReboot)
  {
    int32_t netlnk_error;
    netlnk_error = netlnk_socket_close(amh->mShmSocketFd);

    if (netlnk_error != SHM_NETLNK_SUCCESS) {
      ALOGE("netlink socket to shared memory closure failed [%d]\n", netlnk_error);
    }
  }
#endif

}

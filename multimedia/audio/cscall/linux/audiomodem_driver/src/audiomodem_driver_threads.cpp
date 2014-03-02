/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   audiomodem_driver_threads.cpp
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "audiomodem_driver.nmf"
#include "audiomodem_driver_threads.h"
#include "cscall_time.h"
#include <unistd.h>
#include <assert.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <signal.h>
#include <errno.h>

#if defined(ANDROID)  && !defined(CSCALL_ENABLE_FEATURE_CAIF) && defined(CSCALL_ENABLE_SILENT_REBOOT_SUPPORT)
#include <cscall/nmf/host/protocolhandler/inc/IsiMessage.hpp>
#include "cscall/nmf/host/protocolhandler/codecreq.idt.h"
#ifdef __cplusplus
extern "C" {
#include <shm_netlnk.h>
}
#endif
#endif


static void usr_handler(int sig)
{
  // dummy signal that will stop blocking system call
  // and then cancel request will be handled immediately
  ALOGD("RX received signal to cancel request\n");
}

#if defined(ANDROID)  && !defined(CSCALL_ENABLE_FEATURE_CAIF) && defined(CSCALL_ENABLE_SILENT_REBOOT_SUPPORT)
static void generate_end_of_call_message(OMX_BUFFERHEADERTYPE * buf)
{
  IsiMessage isiMessage(buf->pBuffer);

  isiMessage.setType(MODEM_AUDIO_SPEECH_CODEC_REQ);
  isiMessage.setLength(sizeof(ModemAudioSpeechCodecReq));
  isiMessage.setMedia(0x26);//that is what modem sends
  isiMessage.setSenderDev(0x64);//dir of msg Modem ->APE sender = 0x64,rec = 0x0;
  isiMessage.setReceiverDev(0x0);
  isiMessage.setSenderObj(0x0);
  isiMessage.setReceiverObj(0x0);
  isiMessage.setTransactionId(1);
  isiMessage.setRes(PN_DSP_AUDIO);
  isiMessage.setNetwork(ISI_NETWORK_NONE);
  isiMessage.setCodec(ISI_CODEC_NONE);
}


static void wait_end_of_modem_reboot(int shm_fd)
{
  int modem_resetting = 1;
  int retval;

  while (modem_resetting)
  {
    int shmFdMsg = 0;
    retval = netlnk_socket_recv(shm_fd, &shmFdMsg);
    if (retval < 0)
    {
      ALOGE("Rx Thread: netlnk_socket_recv() error=%d\n", retval);
    }

    if (shmFdMsg == MODEM_STATUS_ONLINE)
    {
      modem_resetting = 0;
    }
    else
    {
      ALOGE("Rx Thread: unsupported shm_netlnk message %d, ignoring", shmFdMsg);
    }
  }
}
#endif

// store fd locally so that we don't have to ask them for each message
// also compute max_fd (use for select) just once
void get_fd(audiomodem_driver * amd, int *modem_fd, int *shm_fd, int *max_fd)
{
  int max;

  *modem_fd = amd->getModemFd();
  max = *modem_fd;
#if defined(ANDROID) && !defined(CSCALL_ENABLE_FEATURE_CAIF) && defined(CSCALL_ENABLE_SILENT_REBOOT_SUPPORT)
  *shm_fd = amd->getShmFd();
  if(*shm_fd > max) {max = *shm_fd;}
#endif
  max++;
  *max_fd = max;
}

/************************************************

  This thread handles the downlink part:
  It receives signals from the NMF Host component
  when some buffers are available on the input queue
  then dequeues the buffers and fills thenm with audio
  data from the modem. Once completed, it returns the
  buffers to the output queue.

*************************************************/
void * RxThreadEntry(void* arg)
{
  audiomodem_driver * amd = (audiomodem_driver *) arg;
  OMX_BUFFERHEADERTYPE * buf;
  int retval;
  long long int currentTime=0;
  long long int previousTime=0;
  ModemBufferList * bufferList = amd->getDownlinkModemBufferList();
  int modem_fd = -1;
  int shm_fd = -1;
  int max_fd;
  struct sigaction sigact;
  sigset_t sigset;

  ALOGD("Audio modem driver: in thread Rx\n");
#ifndef X86
  prctl(PR_SET_NAME, (unsigned long)"cscall-RxThread", 0, 0, 0);
#endif

  // set signal handler to signal SIGUSR1
  // This signal will be send by audiomodem_driver when closing
  // We use ppoll, rather than poll to avoid race condition,
  // then SIGUSR1 is blocked (will be unblocked only during ppoll)
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGUSR1);
  pthread_sigmask(SIG_BLOCK, &sigset, NULL);
  sigemptyset(&sigset);

  sigact.sa_handler = usr_handler;
  sigact.sa_flags   = 0;
  sigemptyset(&sigact.sa_mask);
  if(sigaction(SIGUSR1, &sigact, NULL))
  {
    ALOGE("Audio modem driver: Cannot set signal handler\n");
  }

#if !defined(CSCALL_ENABLE_FEATURE_CAIF)
  // open mode fd in this thread so that we can wait modem to be online
  // without blocking anybody
  amd->openModemFd(true, true);
#endif

  // retrieve fd's from amd
  get_fd(amd, &modem_fd, &shm_fd, &max_fd);

  while(!amd->isFinishing())
  {
    // wait for the NMF cmponent to trigger us
    amd->waitDownlinkAction();

    if(bufferList->isTraceEnabled()) { ALOGD("Rx Thread: got signal\n"); }

    while (((buf = bufferList->getNextToBeProcessedBuffer()) != NULL) && (!amd->isFinishing()))
    {
      fd_set rd;

      FD_ZERO(&rd);
      FD_SET(modem_fd, &rd);
#if defined(ANDROID)  && !defined(CSCALL_ENABLE_FEATURE_CAIF) && defined(CSCALL_ENABLE_SILENT_REBOOT_SUPPORT)
      FD_SET(shm_fd, &rd);
#endif

      retval = pselect(max_fd, &rd, NULL, NULL, NULL, &sigset);

      if (retval == -1)
      {
        if(errno == EINTR)
          ALOGD("Rx Thread: pselect interrupted by signal");
        else
          ALOGE("Rx Thread: pselect error %d\n", errno);
      }
      else if (retval)
      {
#if defined(ANDROID)  && !defined(CSCALL_ENABLE_FEATURE_CAIF) && defined(CSCALL_ENABLE_SILENT_REBOOT_SUPPORT)
        if(FD_ISSET(shm_fd, &rd))
        {
          int shmFdMsg = 0;
          retval = netlnk_socket_recv(shm_fd, &shmFdMsg);
          if (retval < 0)
          {
            ALOGE("Rx Thread: netlnk_socket_recv() error=%d\n", retval);
          }
          else if (shmFdMsg == MODEM_RESET_IND)
          {
            ALOGE("Rx Thread: Modem is rebooting");
            amd->ModemResetting(true);
            amd->closeModemFd(true);

            // generate dummy end of call message  and send it
            // to protocol handler to end call (if any is running)
            generate_end_of_call_message(buf);
            bufferList->markAsUnused(buf);
            amd->outputport.emptyThisBuffer(buf);

            // wait until modem is back online
            wait_end_of_modem_reboot(shm_fd);
            amd->ModemResetting(false);
            if(amd->openModemFd(true, false) < 0)
            {
              ALOGE("Rx Thread: Failed to reopen mode fd after reboot");
            }
            get_fd(amd, &modem_fd, &shm_fd, &max_fd);

            FD_CLR(modem_fd, &rd);
            ALOGE("Rx Thread: Modem is back online");
          }
          else
          {
            ALOGE("Rx Thread: unsupported shm_netlnk message %d, ignoring", shmFdMsg);
          }
        }
#endif
        if (FD_ISSET(modem_fd, &rd))
        {
          // fill this buffer with any received data...
          int nbBytesRead = read(modem_fd, buf->pBuffer, AUDIO_MODEM_MAX_MESSAGE_SIZE);

          currentTime=getTime();

          if ((previousTime) &&
              ((currentTime-previousTime) > 30000) &&
              ((currentTime-previousTime) < 5000000))
          {
            ALOGW("Rx Thread: read too long (time=%llu) (diffTime=%llu) \n",getTime(),currentTime-previousTime);
          }

          if (nbBytesRead >= 0)
          {
            buf->nTimeStamp = getTime();

            if(bufferList->isTraceEnabled())
              ALOGD("Rx Thread: read %d bytes from modem to buf %d\n", nbBytesRead, (int)buf->pOutputPortPrivate);

            if (nbBytesRead == 0)
            {
              /* Zero byte read corresponds to EOF. */
              ALOGD("Rx Thread: RXThreadEntry: Number of read bytes is null!\n");
              usleep(100000);
            }
            else
            {
              buf->nFilledLen = nbBytesRead;

              if(bufferList->isTraceEnabled())
                ALOGD("Rx Thread: Mark buffer %d as processed\n", (int)buf->pOutputPortPrivate);

              bufferList->markAsUnused(buf);

              amd->outputport.emptyThisBuffer(buf);
            }
          }
          else if(errno == EAGAIN)
          {
            ALOGE("Rx Thread: read error [EAGAIN]\n");
            usleep(100000);
          }
          else
          {
            ALOGE("Rx Thread: read error [%d]\n", errno);
          }
        }
      }
    }
  }

  while ((buf = bufferList->getNextToBeProcessedBuffer()) != NULL)
  {
    buf->nFilledLen = 0;
    bufferList->markAsUnused(buf);
    amd->outputport.emptyThisBuffer(buf);
  }
  ALOGD("Rx thread:exit\n");

  retval = 0;
  pthread_exit(&retval);

  return NULL;
}


/************************************************

      This thread handles the uplink part:
      It receives signals from the NMF Host component
      when some buffers are to be transmitted on the input queue
      then dequeues the buffers and sends to the modem.
      Once completed, it returns the
      buffers to the output queue.

*************************************************/

void * TxThreadEntry(void* arg)
{
  audiomodem_driver * amd = (audiomodem_driver *) arg;
  long long int currentTime=0;
  long long int writeTime=0;
  long long int previousTime=0;
  OMX_BUFFERHEADERTYPE * buf;
  int retval;
  ModemBufferList * bufferList = amd->getUplinkModemBufferList();

  ALOGD("Audio modem driver: in thread Tx\n");
#ifndef X86
  prctl(PR_SET_NAME, (unsigned long)"cscall-TxThread", 0, 0, 0);
#endif

  do
  {
    // wait for the NMF cmponent to trigger us
    amd->waitUplinkAction();

    if(bufferList->isTraceEnabled())
      ALOGD("Tx Thread: got signal\n");

    while (((buf = bufferList->getNextToBeProcessedBuffer()) != NULL) && (!amd->isFinishing()))
    {
      // then fill this buffer with any received data...
      int nbBytesWritten;

      assert(buf->nFilledLen < AUDIO_MODEM_MAX_MESSAGE_SIZE);

      if(bufferList->isTraceEnabled())
        ALOGD("Tx Thread: request write of %d bytes from buf %d to modem\n", (int)buf->nFilledLen, (int)(buf->pInputPortPrivate));

      if (!amd->isModemResetting())
      {
        // then write the buffer contents to the fill this buffer with any received data...
        // this is a blocking call
        writeTime=getTime();
        nbBytesWritten = write(amd->getModemFd(), (const void*) buf->pBuffer, buf->nFilledLen);

        currentTime=getTime();

        if(currentTime-writeTime > 5000)
        {
          ALOGW("Tx Thread: write too long (time=%llu) (diffTime=%llu) \n",getTime(),currentTime-writeTime);
        }
        currentTime=getTime();
        if (previousTime)
        {
          // Used to detect too long time between two UL-frames
          if((currentTime-previousTime > 30000) && (currentTime-previousTime) < 5000000)
            ALOGW("Tx Thread: too long time between two Tx frames at (time=%llu) (diff=%llu)\n",currentTime,currentTime-previousTime);
        }
        previousTime=getTime();

        //assert(buf->nFilledLen == nbBytesWritten);
        if ((int)buf->nFilledLen != nbBytesWritten)
        {
          ALOGE("Tx Thread: write error buf->nFilledLen=%d, nbBytesWritten=%d, is the modem rebooting?\n", (int)buf->nFilledLen, nbBytesWritten);
        }
      }
      else
      {
        ALOGD("Tx Thread: Modem is resetting no write to modem!\n");
      }

      if(bufferList->isTraceEnabled())
        ALOGD("Tx Thread: Mark buffer %d as processed\n", (int)buf->pInputPortPrivate);

      bufferList->markAsProcessed(buf);
      amd->inputport.fillThisBuffer(buf);
      bufferList->markAsUnused(buf);
    }

  } while(! amd->isFinishing());

  while ((buf = bufferList->getNextToBeProcessedBuffer()) != NULL)
  {
    buf->nFilledLen = 0;
    bufferList->markAsProcessed(buf);
    amd->inputport.fillThisBuffer(buf);
    bufferList->markAsUnused(buf);
  }

  ALOGD("Tx thread:exit\n");
  retval = 0;
  pthread_exit(&retval);

  return NULL;
}




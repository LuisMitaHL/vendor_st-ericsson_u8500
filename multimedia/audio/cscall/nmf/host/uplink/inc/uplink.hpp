/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef _uplink_hpp_
#define _uplink_hpp_

#include "Component.h"
#include "common_interface.h"
#include "cscall_codec.h"
#include "cscall_config.h"
#include "libeffects/libresampling/include/resample.h"

#define NB_BUFFER               2
#define BUFFER_MS_SIZE          INPUT_BUFFER_DURATION
#define MAX_BUFFER_SAMPLE_SIZE  (BUFFER_MS_SIZE * 16)
#define MISSING_UL_FRAME_THRESHOLD 2 // nb of missing frames allowed before reset/zeros the internal ring buffer

class cscall_nmf_host_uplink : public Component, public cscall_nmf_host_uplinkTemplate
{
public:

  cscall_nmf_host_uplink(void);

  //Component virtual functions
  virtual void process(void);
  virtual void reset(void) ;
  virtual void disablePortIndication(t_uint32 portIdx) ;
  virtual void enablePortIndication(t_uint32 portIdx) ;
  virtual void flushPortIndication(t_uint32 portIdx) ;

  // fsminit interface (fsminit)
  virtual void fsmInit(fsmInit_t initFsm);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);

  // sendcommand interface (sendcommand)
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param); }

  // postevent interface (postevent)
  virtual void processEvent(void)                              { Component::processEvent(); }

  // armnmf_emptythisbuffer interface (emptythisbuffer)
  virtual void emptyThisBuffer (OMX_BUFFERHEADERTYPE_p buffer) { Component::deliverBuffer(INPUT_PORT,  buffer); }

  // hst.common.pcmsettings interface (pcmsettings)
  // to be removed when really removed from AFM;
  virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size) {}

  // armnmf_fillthisbuffer interface (fillthisbuffer)
  virtual void fillThisBuffer  (OMX_BUFFERHEADERTYPE_p buffer) { mPorts[OUTPUT_PORT].queueBuffer((OMX_BUFFERHEADERTYPE *)buffer); }

  // cscall.nmf.host.protocolhandler.configure_code interface (configEncoder)
  virtual void configureCodec(CsCallCodec_t codecType, void* pCodecInfo);

  // timer.api.alarm interface (alarm)
  virtual void signal(void);

  // cscall.nmf.host.uplink.configure (configure)
  virtual void setParameter(t_bool real_time_input);
  virtual void setSampleFreq(t_sample_freq sample_freq);
  virtual void setMaxWaitTime(UlMaxWaitTime_t maxWaitTime);

private:
  typedef enum {INPUT_PORT, OUTPUT_PORT} portname;
  typedef enum {AMR_IDX, EFR_IDX, FR_IDX, HR_IDX, AMRWB_IDX, PCM_IDX} codec_idx;

  // Components variables
  Port                  mPorts[2];
  t_sample_freq			mInputSampleFreq;
  bool                  mRealTimeInput;


  // Encoder methods and variables
  void encoderOpen(void);
  void encoderClose(void);
  void setupEncoderBuffer(void);
  void encodeFrame(OMX_BUFFERHEADERTYPE * bufOut);

  CsCallCodec_t          mCodecInUse;
  codec_idx              mCodecIdx;
  CODEC_INTERFACE_T      mInterfaceFE;
  bool                   mCodecInitialized;
  t_sample_freq			 mCodecSampleFreq;
  Cscall_Codec_Config_t  mCscallCodecConfig;

  // samplerateconv methods
  void srcOpen(void);
  void srcClose(void);
  void convertSampleRate(OMX_BUFFERHEADERTYPE *bufIn);

  bool                  mSRCInitialized;
  ResampleContext       mResampleContext;

  // internal buffers
  void resetRingBuffer(void);
  void copyBuffer(OMX_BUFFERHEADERTYPE *bufIn);
  void updateBufferSize(void);

  t_uint16             mNbFreeBuffer;
  t_uint16             mBufferSampleSize;
  t_uint16             mReadBufferIdx;
  t_uint16             mWriteBufferIdx;
  t_sint16 *           mRingBuffer;
  t_sint16             mRingnExtraBuffer[NB_BUFFER * MAX_BUFFER_SAMPLE_SIZE];
  OMX_U32              mBufferFlags[NB_BUFFER];
  OMX_TICKS            mBufferTimeStamps[NB_BUFFER];
  OMX_BUFFERHEADERTYPE mEncoderBuffer;

  // timing and margins
  void reconfigureSource(t_uint32 drop_time=0);
  UlMaxWaitTime_t      mMaxWaitTime;
  t_uint16             mMaxTimeReached;
  unsigned long long   mLastReceivedBufferTime[NB_BUFFER];
  t_uint32             mSumWaitTime;

  /// Nb of time there was UL fake frame sent to modem during the call
  t_uint32             mULFakeGeneratedFrameNb;
  unsigned long        mNbFrameReceived;
  unsigned long        mNbFrameSent;
  unsigned long        mNbInputBufferTrashed;
  /// Nb of UL frame not received in time, this is used to reset/zeros internal buffer in order to generate silence during accesory change
  t_uint32             mMissingULFrameNb;
  bool                 mOutBufferReset;
  /// Nb of time there was UL lost frame (no available buffer on OUTPUT_PORT) during the call when it is time to send frame to modem
  t_uint32             mLostULFrameNb;
  void *               mHeap;

};

#endif // _uplink_hpp_





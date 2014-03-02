/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
#ifndef _downlink_hpp_
#define _downlink_hpp_

#include "Component.h"
#include "common_interface.h"
#include "cscall_codec.h"
#include "cscall/nmf/host/downlink/inc/FakeFrameGenerator.hpp"
#include "libeffects/libresampling/include/resample.h"

#define NB_BYTES_20MS_16KHZ_MONO (20*16*2)

class cscall_nmf_host_downlink : public Component, public cscall_nmf_host_downlinkTemplate
{
public:

  cscall_nmf_host_downlink();
  
  // Component virtual functions
  virtual void process(void);
  virtual void reset(void);
  virtual void disablePortIndication(t_uint32 portIdx);
  virtual void enablePortIndication(t_uint32 portIdx);
  virtual void flushPortIndication(t_uint32 portIdx);

  // lifecycle.stopper (stopper)
  virtual void stop(void);

  // fsminit interface (fsminit)
  virtual void fsmInit(fsmInit_t initFsm);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
  
  // sendcommand interface (sendcommand)
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param); }
  
  // postevent interface (postevent)
  virtual void processEvent(void)                              { Component::processEvent(); }
  
  // armnmf_emptythisbuffer interface (emptythisbuffer)
  virtual void emptyThisBuffer (OMX_BUFFERHEADERTYPE_p buffer) { Component::deliverBuffer(INPUT_PORT,  buffer); }  
  
  // armnmf_fillthisbuffer interface (fillthisbuffer)
  virtual void fillThisBuffer  (OMX_BUFFERHEADERTYPE_p buffer) { Component::deliverBuffer(OUTPUT_PORT,  buffer); } 

  //cscall.nmf.host.protocolhandler.configure_codec (configDecoder)
  virtual void configureCodec(CsCallCodec_t codecType, void* pCodecInfo);

  // cscall.nmf.host.downlink.configure (configure)
  virtual void setParameter(t_bool real_time_output);
  virtual void setSampleFreq(t_sample_freq sample_freq);
  virtual void setProtocol(CsCallProtocol_t protocol);

  // cscall.nmf.host.downlink.sendFakeFrame interface
  virtual void sendFakeFrame(t_sint64 nTimeStamp);
private:
  typedef enum {INPUT_PORT, OUTPUT_PORT} portname;
  typedef enum {AMR_IDX, EFR_IDX, FR_IDX, HR_IDX, AMRWB_IDX, PCM_IDX} codec_idx;
  typedef enum {CODEC_NOT_INITIALIZED, CODEC_INITIALIZED, TRANSFER_STARTED, PH_SENDING_FAKE_FRAME, CODEC_TERMINATING} codec_state;
 
  // Components variables
  Port           mPorts[2];
  t_sample_freq	 mOutputSampleFreq;
  bool           mRealTimeOutput;
  t_uint32       mDroppedFrameNb;
  
  // Decoder methods and variables 
  void checkSynchro(int ctx);
  void decodeFrame(OMX_BUFFERHEADERTYPE * bufIn, OMX_BUFFERHEADERTYPE * bufOut, int ctx, bool fakeFrame=false);
  void decoderOpen(int ctx);
  void decoderClose(int ctx);
  void returnOutputBuffer(void);

  int codecCtxInUse;

  typedef struct {
    CsCallCodec_t         codecInUse;
    codec_idx             codecIdx;
    CODEC_INTERFACE_T     interfaceFE; /// Interface with codec currently in use
    codec_state           codecInitialized; /// Is codec already initialized ?
    t_sample_freq		  codecSampleFreq;
  } codecCtx_t;

  BS_STRUCT_T           mBSRead;      /// Used as output decoder
  Cscall_Codec_Config_t mCscallCodecConfig; /// Configuration of all codecs (Stores configuration sent by the protocolhandler)
  codecCtx_t            mCodexCtx[2];
  int                   mCodecCtxInUse;
  FakeFrameGenerator   *mFakeFrameGenerator;

  int                   mDLFakeFrameGenerated;
  int                   mConsecutiveFakeFrameSent;

  float                 mGain;
  float                 mCoefGain;
  
  // samplerateconv methods
  void srcOpen(void);
  void srcClose(void);
  void convertSampleRate(OMX_BUFFERHEADERTYPE * bufOut);
  
  bool                  mSRCInitialized;
  ResampleContext       mResampleContext;
  
  // internal buffers
  inline bool                   availableFreeBuffer(void);
  inline OMX_BUFFERHEADERTYPE * getLatestBuffer(void);
  inline OMX_BUFFERHEADERTYPE * getDecoderBuffer(int ctx, bool *use_output_buffer);
  inline OMX_BUFFERHEADERTYPE * getSrcBuffer(bool *use_output_buffer);
  void copyBuffer(OMX_BUFFERHEADERTYPE *bufIn, OMX_BUFFERHEADERTYPE *bufOut);
  
  OMX_BUFFERHEADERTYPE mDecoderBufferHeader;
  OMX_U8               mDecoderBuffer[NB_BYTES_20MS_16KHZ_MONO];
  OMX_BUFFERHEADERTYPE mSrcBufferHeader;
  OMX_U8               mSrcBuffer[NB_BYTES_20MS_16KHZ_MONO];
  void *               mHeap;
  
  
};

#endif // _downlink_hpp_

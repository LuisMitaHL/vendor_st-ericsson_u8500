/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   protocolhandler.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef __protocolhandlerhpp
#define __protocolhandlerhpp


#include "Component.h"

#include "cscall_codec.h"
#include "cscall_config.h"
#include "OMX_Symbian_AudioExt_Ste.h"

#include "cscall/nmf/host/protocolhandler/inc/protocol.hpp"

#define CSCALL_PCM_FRAME_SIZE_16 (640)
#define CSCALL_PCM_FRAME_SIZE_8 (320)

#define CODEC_BUFFER_SIZE_IN_BYTE CSCALL_PCM_FRAME_SIZE_16

#define NO_NEW_CODEC_REQ_PENDING 0
#define NEW_CODEC_REQ_PENDING 1
#define NEW_CODEC_REQ_PENDING_BLOCKING 2

class cscall_nmf_host_protocolhandler : public Component, public cscall_nmf_host_protocolhandlerTemplate 
{
public:

  cscall_nmf_host_protocolhandler(void);

  // Component virtual functions
  virtual void process(void);
  virtual void reset(void) ;
  virtual void disablePortIndication(t_uint32 portIdx) ;
  virtual void enablePortIndication(t_uint32 portIdx) ;
  virtual void flushPortIndication(t_uint32 portIdx) ;

  // lifecycle.stopper (stopper)
  virtual void stop(void);
  
  // fsminit interface (fsminit)
  virtual void fsmInit(fsmInit_t initFsm);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled) { mPorts[portIdx].setTunnelStatus(isTunneled); }

  // sendcommand interface (sendcommand)
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) ;

  // postevent interface (postevent)
  virtual void processEvent(void)                                      { Component::processEvent(); }

  // armnmf_emptythisbuffer interface (emptythisbuffer)
  virtual void emptyThisBuffer         (OMX_BUFFERHEADERTYPE_p buffer) { receiveBuffer(ENCODER_PORT,  buffer); }  

  // armnmf_fillthisbuffer interface (fillthisbuffer)
  virtual void fillThisBuffer          (OMX_BUFFERHEADERTYPE_p buffer) { receiveBuffer(DECODER_PORT,  buffer); } 

  // armnmf_emptythisbuffer interface (downlink_emptythisbuffer)
  virtual void downlink_emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer) { receiveBuffer(DOWNLINK_PORT, buffer); } 

  // armnmf_fillthisbuffer interface (uplink_fillthisbuffer)
  virtual void uplink_fillThisBuffer   (OMX_BUFFERHEADERTYPE_p buffer) { receiveBuffer(UPLINK_PORT,   buffer); }  

  // cscall.nmf.host.protocolhandler.configure interface (configure)
  virtual void setProtocol(CsCallProtocol_t protocol);
  virtual void setModemLoopbackMode(Cscall_ModemLoop_Config_t * loop_config);
  virtual void setModemBuffer(t_uint16 uplinkBufferCount,   OMX_BUFFERHEADERTYPE_p *uplinkBuffer,
                              t_uint16 downlinkBufferCount, OMX_BUFFERHEADERTYPE_p *downlinkBuffer);
  virtual void initTimeMeasurement(void *ptr, t_uint32 duration_ms);
  virtual void resetTimeMeasurement(void);
  virtual void setTimingReqMargins(UlTimingReqMargin_t margins);
  
  // cscall.nmf.host.protocolhandler.newCodecApplied interface (encCodecApplied)
  virtual void encoder_newCodecApplied(void);

  // cscall.nmf.host.protocolhandler.newCodecApplied interface (decCodecApplied)
  virtual void decoder_newCodecApplied(void);
  
  virtual void signal();
  
private:
  
  ////////////////////////////////////
  // Ports
  ///////////////////////////////////
  /// For comprehensive use of ports
  typedef enum {ENCODER_PORT, DECODER_PORT, DOWNLINK_PORT, UPLINK_PORT} portname; 
       
  /// Ports of the protocol handler. One for decoder, one for encoder, two for modem driver.
  Port   mPorts[4]; 

  /// Used to store the "WaitForBuffer" state for each port.
  int    mWaitStatus;
  /**
   * Set that protocolhandler is waiting for some buffer on selected port
   * \param port is the selected port
   * */
  inline void setWaitOn  (portname port) {mWaitStatus |= (1<<port);}
  
  /**
   * Set that protocolhandler is no more waiting for buffer on selected port
   * \param port is the selected port
   * */
  inline void resetWaitOn(portname port) {mWaitStatus &= ~(1<<port);}
  
  /**
   * Is the protocolhanlder waiting for some buffer on selected port ?
   * \param port is the selected port
   * */
  inline bool isWaitingOn(portname port) {return (mWaitStatus & (1<<port));}

  
  ////////////////////////////////////
  // Buffers
  ///////////////////////////////////
  /// Buffer header for data to be decoded (used during init).
  OMX_BUFFERHEADERTYPE   mDecoderBufferHeader;
  /// Data for decoder.
  t_uint8                mDecoderBuffer[CODEC_BUFFER_SIZE_IN_BYTE]; 

  /// Buffer header for encoded data (used during init).
  OMX_BUFFERHEADERTYPE   mEncoderBufferHeader;
  /// Data for encoder.
  t_uint8                mEncoderBuffer[CODEC_BUFFER_SIZE_IN_BYTE];

  /// Buffers between encoder and protocol handler.
  OMX_BUFFERHEADERTYPE * mUplinkBuffers[NB_UL_MODEM_BUFFER];
  /// Number of buffers between encoder and protocol handler.
  t_uint16               mUplinkBufferCount; 
  
  /// Buffers between decoder and protocol handler.
  OMX_BUFFERHEADERTYPE * mDownlinkBuffers[NB_DL_MODEM_BUFFER];
  /// Number of buffers between decoder and protocol handler.
  t_uint16               mDownlinkBufferCount;

  ////////////////////////////////////
  // protocol
  ///////////////////////////////////
  /// pointer on protocol
  Protocol             * mProtocol;
  
  /// get protocol class instance
  void  autoDetectProtocol(OMX_BUFFERHEADERTYPE *msg);
    
  ////////////////////////////////////
  // Codec / network 
  ///////////////////////////////////
  /// Network currently in use
  CsCallNetwork_t       mNetwork;

  /// Next Network to use    
  CsCallNetwork_t       mPendingNetwork;  
  
  /// Codec currently in use.
  CsCallCodec_t         mCodecInUse;

  /// Next Codec to use.    
  CsCallCodec_t         mPendingCodecInUse;  
  
  /// Status of the codec (eg initialized), sent by the modem driver.
  Cscall_Codec_Status_t mCscallCodecStatus;
  
  /// Configuration of the codec, sent by the modem driver.
  Cscall_Codec_Config_t mCscallCodecConfig;

  /// Stores informations of codecReq event to send it to proxy after beeing applied by wrappers
  CsCallCodecReq_t      mCodecReq;

  /// NewCodecReq modem event is received (can be 0: no event pending, 1: event pending but no raise to proxy nor stop data,
  ///                                             2: stop data until pending event applied by both decoder and encoder)
  int                   mNewCodecReqReceived;

  /// NewCodecReq event have been applied by decoder
  int                   mNewCodecReqApplied_decoder;

  /// NewCodecReq event have been applied by decoder
  int                   mNewCodecReqApplied_encoder;

  /// dl security timer expired ?
  bool                  mDLTimerExpired;

  /// Number of dropped frame, when a frame to be decoded has been received from the modem driver but cannot be sent to the decoder (no buffer available).
  int                   mNbDropFrame; 
  /// Number of DL-frame received
  int                   mNbDLFrame; 
  /// Number of UL-frame sent
  int                   mNbULFrame;   
  /// Number of Timing report received
  int                   mNbTimingReport; 
  /// Number of Coding Format received
  int                   mNbCodingFormat; 

  void codecStatusReset (void);
  void newCodecApplied  (void); 
  void endOfCall        (void);
  
  ////////////////////////////////////
  // Time stamp stuff 
  ///////////////////////////////////
  /// Time when last downlink message was received 
  signed long long      mLastReceivedDownlinkMessage;

  /// Time when last downlink message was sent 
  signed long long      mLastSentDownlinkMessage;
  
  /// Time when last timing report was received from modem 
  signed long long      mLastReceivedTimingReport;
  
  /// Timing value that was received in last timing report from modem 
  t_uint32              mTimingReportTime;
  
  /// How many us after a decode is received that speech encode shall start 
  long                  mModemAdjustment;
  
  /// If a new timing report has been received and that shall trigger recalculation of mModemAdjustment
  int                   mPatternDetectionNeeded;

  /// Use to propagate Start Time. 
  bool                  mPropagateStartTime;

  /// Use to store current DL time stamp. 
  OMX_S64               mDLTimeStamp;

  /// Use for GSM
  t_sint16              mGSM445;
  
  /// To answer queries for uplink timing from OMX client
  OMX_SYMBIAN_AUDIO_CONFIG_UPLINKTIMINGINFOTYPE * mUplinkTimingInfo;

  /// Stores when the next uplink frame is needed
  unsigned long long                              mNextDeliveryTime;

  /// Is LoopBack mode on
  bool                  mUseLoopBackMode;

  /// How many dl did not update ul time
  int                                             mNbDlFrameDidNotUpdateUl;
  
  /// Forget factor for mean delivery time
  float                                           mAlpha;

  /// encoder margin
  UlTimingReqMargin_t mMargins;

  /// current margin
  t_uint32            mCurrentMargin;

  /// Use to start periodic timer at call start when no downlink message has been received
  bool                mTimerStarted;

  void chooseCurrentMargin(void);
  void downlinkMessageReceived(signed long long timestamp);
  bool calculateModemAdjustment(unsigned long long now);  
  void timingReport(signed long long timestamp, t_uint32 time);

  ////////////////////////////////////
  // Traces
  ///////////////////////////////////
  void traceULMsg(OMX_BUFFERHEADERTYPE *buffer);
  void traceDLMsg(OMX_BUFFERHEADERTYPE *buffer);

#ifdef MMPROBE_ENABLED
  // Used for speech probes
  typedef struct speech_data {
    short system;
    short codec;
    short dtx;
    char data[CODEC_BUFFER_SIZE_IN_BYTE];
  } SpeechData_t;
#endif        
  
  
  ////////////////////////////////////
  // Main functions
  ///////////////////////////////////
  bool processModemMessage(void);
  bool informationResponse(OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer);
  bool configureCodec     (OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer, OMX_BUFFERHEADERTYPE *decoder);
  bool configureDLTiming  (OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer);
  bool configureULTiming  (OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer);
  bool transmitCodecFrame (OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE *decoder);
  bool voiceCallStatus    (OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer);
  bool configureApeLoop   (OMX_BUFFERHEADERTYPE *msg, OMX_BUFFERHEADERTYPE **answer);
  
  void sendCodecFrame     (void);
  void receiveBuffer(portname port, OMX_BUFFERHEADERTYPE_p buffer);
  void applyCodecConfig(void);
};

#endif // __protocolhandlerhpp

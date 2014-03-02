/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _alsasink_wrapper_hpp_
#define _alsasink_wrapper_hpp_

#include "Component.h"
#include "common_interface.h"

class alsasink_nmfil_host_wrapper : public Component, public alsasink_nmfil_host_wrapperTemplate
{
public:
  alsasink_nmfil_host_wrapper(void);

  // NMF life-cycle methods...
  void start(void);
  void stop(void);

  // Component virtual methods...
  virtual void reset();
  virtual void process();
  virtual void disablePortIndication(t_uint32 portIdx);
  virtual void enablePortIndication(t_uint32 portIdx);
  virtual void flushPortIndication(t_uint32 portIdx);

  virtual void fsmInit(fsmInit_t initFsm);
  virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
  virtual void processEvent(void);
  virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
  virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

  // Methods provided by interface afm.nmf.host.pcmprocessings.wrapper.configure:
  void setParameter(alsasink_wrapper_config_t config);
  void bufferProcessed(void);

  // Methods provided by interface afm.nmf.host.common.pcmsettings:
  void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size);

private:
  alsasink_config_t m_alsasink_config;
  int mResetCount;
  bool mStartTime;
  int mTimeStamp;
  int mSentForProc;
  int mToBeReturned;
  Port mPort[2];
  typedef enum {
	PORT_IDX_INPUT,
	PORT_IDX_FEEDBACK
  } portIndex;
};

#endif // _alsasink_wrapper_hpp_

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
#ifndef _tonegen_wrapper_hpp_
#define _tonegen_wrapper_hpp_

#include "Component.h"
#include "common_interface.h"

class tonegen_nmfil_host_wrapper : public Component, public tonegen_nmfil_host_wrapperTemplate
{
public:

  // NMF life-cycle methods...
  t_nmf_error construct(void);
  void destroy(void);
  void start(void);
  void stop(void);

  // Component virtual methods...
  void reset();
  void process();
  void disablePortIndication(t_uint32 portIdx);
  void enablePortIndication(t_uint32 portIdx);
  void flushPortIndication(t_uint32 portIdx);

  void fsmInit(fsmInit_t initFsm);
  void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
  void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
  void processEvent(void)                    { Component::processEvent() ; }
  void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

  // Methods provided by interface afm.nmf.host.pcmprocessings.wrapper.configure:
  void setParameter(t_tonegen_wrapper_config config);

  // Methods provided by interface afm.nmf.host.common.pcmsettings:
  void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size);

private:
  typedef enum {OUTPUT_PORT_IDX} portname;
  Port mPort;
  t_source_config m_source_config;
  int mResetCount;
  bool mStartTime;
  int mTimeStamp;
  int mCount;
};

#endif // _tonegen_wrapper_hpp_

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief   ext_imgcontroller ARM-NMF component class header
*/
#ifndef __ext_imgcontrollerhpp
#define __ext_imgcontrollerhpp

#include "Controller.h"

typedef enum {
  GRABCTL_INDEX = 0,
  STAB_INDEX    = 1,
} ImgComponentIndex;

class ext_imgcontroller : public Controller, public ext_imgcontrollerTemplate
{
public:
    virtual void fsmInit(fsmInit_t inits);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
  virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Controller::sendCommand(cmd, param) ; }
  virtual void controllers_eventHandler(OMX_EVENTTYPE ev, t_uint32 data1, t_uint32 data2, t_uint8 collectionIndex) {
        Controller::controllersEventHandler(ev, data1, data2, collectionIndex) ;
    }

  virtual void forwardPortCommand(OMX_COMMANDTYPE cmd, t_uword id) ;
  virtual void forwardPortEvent(OMX_COMMANDTYPE cmd, t_uword id) ;

  virtual void grabctl_eventHandler (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);
  virtual void stab_eventHandler (OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2);

private:

};

#endif

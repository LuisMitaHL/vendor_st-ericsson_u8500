/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ispctl_eventclbk.h"
#include <string.h>
CIspctlEventClbks::CIspctlEventClbks()
{
    memset(mIspCtlEventHooks, 0, sizeof(mIspCtlEventHooks));
}

// FIXME: check pClbk name in function of type
 t_ispctlEventClbkErr CIspctlEventClbks::setEventClbk(t_sia_client_id clientID, enum e_ispctlInfo infoID, t_ispctlEventClbk pClbk, t_ispctlEventClbkCtxtHnd ctxtHnd)
 {
     t_ispctlEventClbk clbk;
     t_ispctlEventClbkCtxtHnd context;
     t_ispctlEventClbkErr err = getEventClbk(clientID, infoID, &clbk, &context);
     if (err != ISPCTL_EVENTCLBK_ERR_NOTFOUND) {
         // Error: A callback is already registered for this event and client ID.
         return ISPCTL_EVENTCLBK_ERR_BUSY;
     }
     for (int i=0; i < ISPCTL_EVENTCLBK_MAXCOUNT; i++) {
         if (mIspCtlEventHooks[i].pClbk == NULL) {
             mIspCtlEventHooks[i].infoID    = infoID;
             mIspCtlEventHooks[i].clientID  = clientID;
             mIspCtlEventHooks[i].pClbk     = pClbk;
             mIspCtlEventHooks[i].ctxtHnd   = ctxtHnd;
             return ISPCTL_EVENTCLBK_ERR_NONE;
         }
     }
     // Error: No room left for registering another callback.
     return ISPCTL_EVENTCLBK_ERR_OVERFLOW;
 }


 t_ispctlEventClbkErr CIspctlEventClbks::clearEventClbk(t_sia_client_id clientID, enum e_ispctlInfo infoID)
 {
     for (int i=0; i < ISPCTL_EVENTCLBK_MAXCOUNT; i++) {
         if ((mIspCtlEventHooks[i].infoID == infoID) && (mIspCtlEventHooks[i].clientID == clientID)) {
             mIspCtlEventHooks[i].infoID    = ISP_INF_NONE;
             mIspCtlEventHooks[i].clientID  = SIA_CLIENT_CAMERA;
             mIspCtlEventHooks[i].pClbk     = NULL;
             mIspCtlEventHooks[i].ctxtHnd   = NULL;
             return ISPCTL_EVENTCLBK_ERR_NONE;
         }
     }
     // Error: The specified callback is not registered.
     return ISPCTL_EVENTCLBK_ERR_NOTFOUND;
 }


 t_ispctlEventClbkErr CIspctlEventClbks::getEventClbk(t_sia_client_id clientID, enum e_ispctlInfo infoID, t_ispctlEventClbk *pClbk, t_ispctlEventClbkCtxtHnd *pCtxtHnd)
 {
     for (int i=0; i < ISPCTL_EVENTCLBK_MAXCOUNT; i++) {
          if ((mIspCtlEventHooks[i].infoID == infoID) && (mIspCtlEventHooks[i].clientID == clientID)) {
              *pClbk    = mIspCtlEventHooks[i].pClbk;
              *pCtxtHnd = mIspCtlEventHooks[i].ctxtHnd;
              return ISPCTL_EVENTCLBK_ERR_NONE;
          }
      }
     // Error: No callback matches the request.
      return ISPCTL_EVENTCLBK_ERR_NOTFOUND;
 }

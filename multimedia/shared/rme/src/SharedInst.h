/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file SharedInst.h
 * RME internal shared instances
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _SHAREDINST_H_
#define _SHAREDINST_H_

#include "NmfDomains.h"
#include "ORPCompNetwork.h"
#include "AGov.h"

class RMEimpl;
namespace rme {

/** RME internal shared instances */
class SharedInst {
 public:
  /// returns NMF domains manager
  static NmfDomains* GetNmfDomainsInst() { return mpNmfDomains; }
  /// returns ORPCompNetwork manager
  static ORPCompNetwork* GetORPCompNetwInst() { return mpORPCompNetw; }
  /// returns audio specific governor
  static AGov* GetAgov() { return mpAGov; }

  /// interconnect / bandwidth mode
  static void NotifyInterconnectBwMode(OMX_BOOL bUse1080pSettings) {
    if (mpNotifyInterconnectBwMode)
      mpNotifyInterconnectBwMode(bUse1080pSettings);
  }

 protected:
  friend class RMEimpl;

  /// NMF domains manager
  static NmfDomains* mpNmfDomains;
  /// ORPComponent network manager: aggregation of OMXIL Resource Proxy (ORP) components
  static ORPCompNetwork* mpORPCompNetw;
  /// audio specific governor
  static AGov* mpAGov;

  /// callback for interconnect / bandwidth mode
  static void (*mpNotifyInterconnectBwMode)(OMX_BOOL bUse1080pSettings);
};

}  // namespace

#endif


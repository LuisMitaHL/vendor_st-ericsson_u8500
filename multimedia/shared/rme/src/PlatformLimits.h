/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file PlatformLimits.h
 * Platform limits static class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _PLATFORMLIMITS_H_
#define _PLATFORMLIMITS_H_

namespace rme {

/** Platform limits static class */
class PlatformLimits {
 public:
  /// MCPS budget on SVA
  static const unsigned int MMDSP_SVA_MCPS = 200;

  /// MCPS budget on SIA / audio logical part
  static const unsigned int MMDSP_SIA_MCPS_AUDIO = 180;
  /// MCPS budget on SIA / visual logical part
  static const unsigned int MMDSP_SIA_MCPS_VISUAL = 20;

  /// Timeout value (in millisecs) for OMXIL component to react to RME's preemption command
  static const unsigned int PREEMPTION_ACKNOWLEDGE_TIMEOUTMS = 3000;

  /// Timeout value (in millisecs) for rechecking resources after ALP transition
  static const unsigned int ALP_TRANSITIONRECHECKRES_TIMEOUTMS = 1500;

  /// Delay time (in millisecs) for ALP Enter actual activation
  /// used to discard small audio streams from entering in ALP
  static const unsigned int ALP_DELAYACTIVATION_TIMEOUTMS = 4000;
};

}  // namespace

#endif

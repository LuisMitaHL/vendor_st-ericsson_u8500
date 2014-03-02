/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ResAlp.h
 * resource manager ALP support class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _RESALP_H_
#define _RESALP_H_

#include "uosal.h"
#include "ORPCompNetwork.h"

namespace rme {

/** resource manager ALP class */
class ResAlp {
 public:
  /// processes audio low power management; returns ALP timeout if any
  unsigned int ProcessAudioLowPower(const char* callerid, bool allow_timeout);

  /// checks state transition for component
  void CheckStateTransition(ORPComp* pcomp, DS_TSI_E transition_id);

  /// handles unsollicited low power enter
  void HandleUnsollicitedAlpEntered(ORPComp* pcomp);

  /// handles unsollicited low power exit
  void HandleUnsollicitedAlpExited(ORPComp* pcomp);

  /// ctor
  ResAlp() :
    mContextIsAlpValid(false),
    mAlpActivationTimeUs(0),
    mAlpSinkCompOmxhdl(NULL)
    {}

  /// dtor
  virtual ~ResAlp() {}

 private:
  /// audio low power network context validity
  bool mContextIsAlpValid;

  /// time in microseconds of next activation of low power
  OMX_U64 mAlpActivationTimeUs;

  /// current audio low power sink component handle
  OMX_PTR mAlpSinkCompOmxhdl;

  /// determines if network is in audio low power capable context
  /// analysis is based both on network topology and on components state
  /// returns ALP sink component in case of ALP compatible context
  ORPComp* IsNetworkAudioLowPowerCompatible(void);

  /// scans network sinks
  /// returns unique executing audio low power sink
  ORPComp* ScanNetworkSinks() const;

  /// scans up from ALP sink
  /// adds HAS_DOWNSTREAM_ALPSINK tag
  /// and sets unique executing audio low power source
  int ScanUpFromAlpSink(ORPCompHalfline* phalfline, ORPComp* *ppcomp_alpsource) const;

  /// scans down from ALP source
  /// adds HAS_UPSTREAM_ALPSOURCE tag
  int ScanDownFromAlpSource(ORPCompHalfline* phalfline);

  /// scans all components for ALP compatibility
  /// checks states, checks forbidden/authorized components
  int ScanNetworkComponentsForAlp(ORPComp* *ppcomp_alpsource_notlinked, const ORPComp* pcomp_alpsource_linked) const;

  /// handles ALP activation
  unsigned int ManageAlpActivation(ORPComp *pcomp_alpsink, bool allow_timeout, bool *penterexitcrossingdetected);

  /// handles ALP unactivation
  void ManageAlpUnactivation(bool *penterexitcrossingdetected);

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(ResAlp);
};

}  // namespace

#endif

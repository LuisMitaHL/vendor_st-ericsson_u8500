/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ResManager.h
 * resource manager class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _RESMANAGER_H_
#define _RESMANAGER_H_

#include "uosal.h"
#include "ORPComp.h"
#include "ResEstimator.h"
#include "ResAlp.h"

namespace rme {

/** resource manager class */
class ResManager {
 public:
  /// processes 'valid' transition related to a
  /// dual OMXIL state / Resource state notification
  void NewDualStatesTransition(ORPComp* pcomp, DS_TSI_E transition_id, bool initjustdone);

  /// processes notification (internal or external) of policy group priority and id from client
  void NewPolicyGP(ORPComp* pcomp,
      unsigned int new_gpid, unsigned int new_gpprio,
      bool internal = false);

  /// processes notification of NMF domain ids applied
  void NmfDomainIdsApplied(ORPComp* pcomp, RM_STATUS_E nErrorCode,
      int data);
  /// processes notification of audio low power enter applied
  void AudioLowPowerEnterApplied(ORPComp* pcomp, RM_STATUS_E nErrorCode,
      int udata);
  /// processes notification of audio low power exit applied
  void AudioLowPowerExitApplied(ORPComp* pcomp, RM_STATUS_E nErrorCode);

  /// works during 'idle' time
  unsigned int WorkTime(bool calledontimeout);

  /// ctor
  ResManager() : mAlp(), mResEstimator() {}
  /// dtor
  virtual ~ResManager() {}

 private:
  /// specific audio low power support
  ResAlp mAlp;

  /// resources estimator associated
  ResEstimator mResEstimator;

  /// retrieves capabilities and resources estimation from component
  /// possibly updates network following priority change
  int RetrieveCapabilitiesAndResEstim(ORPComp* pcomp, bool getcap);

  /// processes resources preemption-suspend
  void ProcessResourcesPremptSuspend(ORPComp* pcomp);
  /// processes resources preemption-unload
  void ProcessResourcesPreemptUnload(ORPComp* pcomp);

  /// processes resources requests of all types
  unsigned int ProcessResRequests();

  /// processes resources preemptions and marks resources grants
  void ProcessPreemptsMarkGrants(bool *pcandosvaresproc, bool *pcandosiaresproc);
  /// processes resources grants
  void ProcessGrants(bool candosvaresproc, bool candosiaresproc);

  /// sets and sends domains attribution
  bool SetSendNmfDomains(ORPComp* pcomp);
  /// checks NMF domains command acknowledge status <0:ko, 0=na, >0:ok
  int CheckNmfDomainsCmdAckStatus(ORPComp* pcomp);

  /// checks visual activity type
  void CheckVisualActivityType();

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(ResManager);
};

}  // namespace

#endif

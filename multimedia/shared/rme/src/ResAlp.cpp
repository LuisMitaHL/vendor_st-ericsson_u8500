/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ResAlp.cpp
 * resource manager ALP support class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "PlatformLimits.h"
#include "PfRoles.h"
#include "SharedInst.h"
#include "ORPCompNetwork.h"
#include "ORPCompHalfline.h"

#include "ResAlp.h"
#include "RMEimpl.h"

namespace rme {

/// processes Audio Low Power management; returns ALP timeout if any
unsigned int ResAlp::ProcessAudioLowPower(const char* callerid, bool allow_timeout) {
  unsigned int timeout_msec_alp = 0;

  if (callerid == NULL)
    callerid = "n.a.";

  // display and visual activity detector threads, if started,
  // will call *this* class' ProcessAudioLowPower
  // in case of change of display or visual context
  AGov* agovM = SharedInst::GetAgov();
  agovM->SetResAlp(this);

  // network topology and states analysis
  ORPComp *pcomp_alpsink = IsNetworkAudioLowPowerCompatible();
  bool contextisalpvalid = pcomp_alpsink ? true : false;

  if (contextisalpvalid) {
    // possibly spawn display and activity detector threads, if not done already,
    // when the ALP context is valid
    // thus making sure it is associated with 'audio' rme instance (in adm process in Android case);
    // if started at this time, detector threads wont be able to update tracker states (under lock..)
    // but not an issue since ALP activation is delayed and default display state is on anyway
    agovM->StartDetectorThreads();

    // valid ALP audio context but need additional condition:
    // enter ALP only if display if OFF and there is no SxA based visual activity
    // (separate RME process for audio & visual)
    if (agovM->IsDisplayOn()) {
      contextisalpvalid = false;
      RLOG_DEBUG("valid ALP context but display on\n");
      // no need to set timeout: display detector thread will call ALP processing when status changes
    } else if (agovM->IsActivityTrackerVisualOn()) {
      contextisalpvalid = false;
      RLOG_DEBUG("valid ALP context, display off, but visual on\n");
      // no need to set timeout: visual detector thread will call ALP processing when status changes
    } else if (agovM->IsActivityTrackerAdmOn()) {
      contextisalpvalid = false;
      RLOG_DEBUG("valid ALP context, display off, visual off, but ADM on\n");
      // no need to set timeout: visual detector thread will call ALP processing when status changes
    } else if (agovM->IsUcGovAlpAvailable() == false) {
      RLOG_DEBUG("valid ALP context, display off, visual off, ADM off, but no ALP UC gov\n");
#ifdef ANDROID
      // if audio low power UC gov is not present, then no low power (burst) activated
      contextisalpvalid = false;
#endif
    }
  }

  bool enterexitcrossingdetected = false;
  if (mContextIsAlpValid != contextisalpvalid) {
    // manage ALP context switch
    if (contextisalpvalid) {
      // ALP activaton
      RLOG_INFO("AudioLowPower::ENTER (%s)\n", callerid);
      timeout_msec_alp = ManageAlpActivation(pcomp_alpsink, allow_timeout, &enterexitcrossingdetected);
    } else {
      // ALP unactivaton
      RLOG_INFO("AudioLowPower::EXIT (%s)\n", callerid);
      ManageAlpUnactivation(&enterexitcrossingdetected);
    }

    if (enterexitcrossingdetected == false) {
      RLOG_DEBUG("AudioLowPower::ALP context changed to %d\n", contextisalpvalid);
      mContextIsAlpValid = contextisalpvalid;
    }
  } else {
      // same context: just check if delayed activation on-going ==> reattempt
      if (contextisalpvalid && mAlpActivationTimeUs) {
          RLOG_INFO("AudioLowPower::ENTER-retry (%s)\n", callerid);
          timeout_msec_alp = ManageAlpActivation(pcomp_alpsink, allow_timeout, NULL);
      }
  }

  return timeout_msec_alp;
}

/// determines if network is in audio low power capable context
/// analyses is based both on network topology and on components state
ORPComp* ResAlp::IsNetworkAudioLowPowerCompatible() {
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();

  // reset all external tags from halflines
  networkM->ClearExternTagHalflines();

  // Step1: scan network sinks
  // retrieve unique executing audio low power sink
  // candidate audio low power sink component
  ORPComp *pcomp_alpsink = ScanNetworkSinks();
  if (pcomp_alpsink == NULL) {
    // not in valid ALP context
    RLOG_DEBUG("non ALP context (from s1: sinks scan)\n");
    return NULL;
  }

  ORPCompHalfline* phalfline_sink = NULL;
  int nhidsink = pcomp_alpsink->NwHalflineId();
  if (nhidsink >= 0) {
    phalfline_sink = networkM->GetHalfline(static_cast<unsigned int>(nhidsink));
  }
  if (phalfline_sink == NULL) {
    RLOG_DEBUG("non ALP context (from s1: ALP sink halfline)\n");
    return NULL;
  }

  // Step2: recursively scan up from ALP sink
  // +add HAS_DOWNSTREAM_ALPSINK tag
  // +set unique executing audio low power 'linked' source
  networkM->ClearRecursiveMarkerHalflines();
  // candidate audio low power source 'linked-with-ALPsink' component
  ORPComp *pcomp_alpsource_linked = NULL;
  if ((ScanUpFromAlpSink(phalfline_sink, &pcomp_alpsource_linked) < 0) ||
      (pcomp_alpsource_linked == NULL)) {
    // not in valid ALP context
    RLOG_DEBUG("non ALP context (from s2: ALP sink scan-up)\n");
    return NULL;
  }

  ORPCompHalfline* phalfline_source = NULL;
  int nhidsource = pcomp_alpsource_linked->NwHalflineId();
  if (nhidsource >= 0) {
    phalfline_source = networkM->GetHalfline(static_cast<unsigned int>(nhidsource));
  }
  if (phalfline_source == NULL) {
    RLOG_DEBUG("non ALP context (from s2: ALP linked-source halfline)\n");
    return NULL;
  }

  // Step3: recursively scan down from ALP 'linked' source
  // +add HAS_UPSTREAM_ALPSOURCE tag
  networkM->ClearRecursiveMarkerHalflines();
  if (ScanDownFromAlpSource(phalfline_source) < 0) {
    // not in valid ALP context
    RLOG_DEBUG("non ALP context (from s3: ALP linked-source scan-down)\n");
    return NULL;
  }

  // Step4: scan all components
  // 'inside' ALP source/sink halflines or outside, including single components
  // candidate audio low power source 'not-linked-with-ALPsink' component
  ORPComp *pcomp_alpsource_notlinked = NULL;
  if (ScanNetworkComponentsForAlp(&pcomp_alpsource_notlinked, pcomp_alpsource_linked) < 0) {
    // not in valid ALP context
    RLOG_DEBUG("non ALP context (from s4: scan all)\n");
    return NULL;
  }

  if (pcomp_alpsource_notlinked) {
#ifdef NO_ALP_SINGLEDECODER_SOURCE
  // dont support not linked decoder ALP sources
    RLOG_DEBUG("non ALP context: (from s4: reject single ALP source)\n");
    return NULL;
#endif
  }

  return pcomp_alpsink;
}

/// scans network sinks
/// retrieves unique executing audio low power sink
ORPComp* ResAlp::ScanNetworkSinks() const {
  ORPComp *pcomp_alpsink_anystate = NULL;
  ORPComp *pcomp_alpsink_executing = NULL;

  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  for (const ORPCompHalfline* phalfline = networkM->FirstHalfline();
       phalfline != NULL;
       phalfline = networkM->NextHalfline(phalfline)) {
    ORPComp *pcomp_sink = phalfline->GetLastComponent();
    if (phalfline->CountOutputConnections() || (pcomp_sink == NULL) ) {
      // halfline is connected at output => not sink level, keep going
      // or last component is null (shall not happen anyway)
      continue;
    }

    // audio low power sink: needs valid capability role, flag..
    if ((PfRoles::IsAlpRoleSink(pcomp_sink->Role()) == false) ||
        ((pcomp_sink->CapBitmap() & RM_CAP_LOWPOWERCTRLSIG) == 0)) {
      if (StatesDuo::HasOnlyStatesWithNoActiveResourcesReq(pcomp_sink->DualStateOR()) == false) {
        // non ALP sink with 'active' state: not a valid ALP context (and no exceptions allowed)
        RLOG_DEBUG("sink comp[i:%d - r:%s] is active and non ALP\n", pcomp_sink->Index(), pcomp_sink->Role());
        return NULL;
      } else {
        // non ALP sink with 'unactive' state: ok accepted..
        // states of other components in halfline will be checked in a later step
        continue;
      }
    } else {
      if (pcomp_alpsink_anystate) {
        // shall be only ONE ALP capable sink, whatever the state (even if non active)
        RLOG_DEBUG("caution: multiple ALP capable sinks -- rejecting all\n");
        return NULL;
      } else {
        pcomp_alpsink_anystate = pcomp_sink;
        if (StatesDuo::HasOnlyStatesExecuting(pcomp_sink->DualStateOR()) == true) {
          // ALP sink executing candidate
          pcomp_alpsink_executing = pcomp_sink;
        } else {
          RLOG_DEBUG("ALP sink comp[i:%d - r:%s] not executing\n", pcomp_sink->Index(), pcomp_sink->Role());
          return NULL;
        }
      }
    }
  }

  return pcomp_alpsink_executing;
}

/// scans up from ALP sink
/// adds HAS_DOWNSTREAM_ALPSINK tag
/// and sets unique executing audio low power source
int ResAlp::ScanUpFromAlpSink(ORPCompHalfline* phalfline, ORPComp* *ppcomp_alpsource) const {
  if (phalfline == NULL) {
    return -1;
  }

  // mark current line
  phalfline->SetRecursiveMarker();

  // has ALP sink downstream
  phalfline->AddExternTag(ORPCompHalfline::HAS_DOWNSTREAM_ALPSINK);

  // at this stage, analyse/screen only 'sources' components;
  // others components will be analysed in a later step
  ORPComp *pcomp_source = phalfline->GetFirstComponent();
  if ((phalfline->CountInputConnections() == 0) && pcomp_source) {
    // halfline is not connected at input => source level
    // source component not NULL (shall not happen anyway)
    // consider only active source components at this step
    if (StatesDuo::HasOnlyStatesWithNoActiveResourcesReq(pcomp_source->DualStateOR()) == false) {
      // active source component: check whether or not it is an ALP source
      if (PfRoles::IsAlpRoleSource(pcomp_source->Role()) == false) {
        // not a valid ALP source and active: reject except if allowed to co-exist
        if (PfRoles::CanCoexistWithAlpUcc(pcomp_source->Role()) == false) {
          // not in ALP context: active non ALP source
          RLOG_DEBUG("linked source comp[i:%d - r:%s] is active and non ALP\n", pcomp_source->Index(), pcomp_source->Role());
          return -1;
        }  // else 'allowed' active (whatever the state) non-ALP source
      } else {
        // valid ALP source: shall be executing
        if (ppcomp_alpsource &&
            (StatesDuo::HasOnlyStatesExecuting(pcomp_source->DualStateOR()) == true)) {
          if (*ppcomp_alpsource) {
            // allow only ONE executing ALP source for valid ALP context
            // and multiple active ALP sources: not in ALP context
            RLOG_DEBUG("multiple executing ALP sources\n");
            return -1;
          } else {
            *ppcomp_alpsource = pcomp_source;
          }
        } else {
          // not in ALP context: active non executing ALP source
          RLOG_DEBUG("ALP linked source comp[i:%d - r:%s] active but not executing\n", pcomp_source->Index(), pcomp_source->Role());
          return -1;
        }
      }
    }  // else non active source
  } else {
    // not source level: recursive-up
    for (unsigned int k = 0; k < phalfline->CountInputConnections(); k++) {
      int peer_hlid = phalfline->PeerHalflineUp(k, NULL);
      if (peer_hlid >= 0) {
        ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
        ORPCompHalfline* phalfline_up = networkM->GetHalfline(static_cast<unsigned int>(peer_hlid));
        // recursive, if not already marked (loops protection)
        if ((phalfline_up) && (phalfline_up->RecursiveMarker() == false)) {
          if (ScanUpFromAlpSink(phalfline_up, ppcomp_alpsource) < 0) {
            // abort as soon as a non-ALP context is found
            return -1;
          }
        }
      }
    }
  }

  return 0;
}

/// scans down from ALP source
/// adds HAS_UPSTREAM_ALPSOURCE tag
int ResAlp::ScanDownFromAlpSource(ORPCompHalfline* phalfline) {
  if (phalfline == NULL) {
    return -1;
  }

  // mark current line
  phalfline->SetRecursiveMarker();

  // has ALP source upstream
  phalfline->AddExternTag(ORPCompHalfline::HAS_UPSTREAM_ALPSOURCE);

  // recursive-down
  for (unsigned int k = 0; k < phalfline->CountOutputConnections(); k++) {
    int peer_hlid = phalfline->PeerHalflineDown(k, NULL);
    if (peer_hlid >= 0) {
      ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
      ORPCompHalfline* phalfline_down = networkM->GetHalfline(static_cast<unsigned int>(peer_hlid));
      // recursive, if not already marked (loops protection)
      if ((phalfline_down) && (phalfline_down->RecursiveMarker() == false)) {
        if (ScanDownFromAlpSource(phalfline_down) < 0) {
          // abort as soon as a non-ALP context is found
          return -1;
        }
      }
    }
  }

  return 0;
}

/// scans all components for ALP compatibility
/// checks states, checks forbidden/authorized components
int ResAlp::ScanNetworkComponentsForAlp(ORPComp* *ppcomp_alpsource_notlinked, const ORPComp* pcomp_alpsource_linked) const {
  // scan all components of network
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  for (ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    int nhid = pcompiter->NwHalflineId();
    if (nhid >= 0) {
      // component is in a halfline
      ORPCompHalfline* phalfline = networkM->GetHalfline(static_cast<unsigned int>(nhid));
      if (phalfline == NULL) {
        return -1;  // shall not happen anyway
      }

      if ((phalfline->HasExternTag(ORPCompHalfline::HAS_UPSTREAM_ALPSOURCE) == false) ||
          (phalfline->HasExternTag(ORPCompHalfline::HAS_DOWNSTREAM_ALPSINK) == false)) {
        // not a valid ALP halfline => component shall be non active, except if allowed
        if (PfRoles::CanCoexistWithAlpUcc(pcompiter->Role()) == false) {
          if (StatesDuo::HasOnlyStatesWithNoActiveResourcesReq(pcompiter->DualStateOR()) == false) {
            // -> non ALP compatible state
            RLOG_DEBUG("comp[i:%d - r:%s] active but not in valid ALP halfline\n", pcompiter->Index(), pcompiter->Role());
            return -1;
          }
        }  // else 'allowed' (whatever the state)
      } else {
        // valid ALP halfline: has upstream ALP source and downstream ALP sink tags
        // => component shall be in executing
        if (StatesDuo::HasOnlyStatesExecuting(pcompiter->DualStateOR()) == false) {
          RLOG_DEBUG("comp[i:%d - r:%s] active but not executing in ALP halfline\n", pcompiter->Index(), pcompiter->Role());
          return -1;
        }

        // also discard blacklisted components
        if (PfRoles::IsAlpRoleForbidden(pcompiter->Role())) {
          RLOG_DEBUG("comp[i:%d - r:%s] in ALP halfline is marked as ALP incompatible\n", pcompiter->Index(), pcompiter->Role());
          return -1;
        }
      }
    } else {
      // component is not in a halfline: single
      if (StatesDuo::HasOnlyStatesWithNoActiveResourcesReq(pcompiter->DualStateOR()) == false) {
        // single with active resources:
        // compatible if allowed (whatever the state)
        // or if executing single decoder component found on top of ALP line source
        if (PfRoles::CanCoexistWithAlpUcc(pcompiter->Role()) == false) {
          if (ppcomp_alpsource_notlinked &&
              PfRoles::IsAlpRoleSingleSource(pcompiter->Role()) &&
              (StatesDuo::HasOnlyStatesExecuting(pcompiter->DualStateOR()) == true) &&
              pcomp_alpsource_linked && PfRoles::IsAlpRoleLineSource(pcomp_alpsource_linked->Role())) {
            if (*ppcomp_alpsource_notlinked) {
              // allow only ONE executing ALP non-linked source for valid ALP context
              // and multiple active ALP sources: not in ALP context
              RLOG_DEBUG("multiple executing non-linked ALP sources\n");
              return -1;
            } else {
              *ppcomp_alpsource_notlinked = pcompiter;
            }
          } else {
            RLOG_DEBUG("comp[i:%d - r:%s] is ALP incompatible active single\n", pcompiter->Index(), pcompiter->Role());
            return -1;
          }
        }  // else 'allowed' (whatever the state)
      }  // else single component with no active resources: ALP compatible
    }
  }  // scan all components

  return 0;
}

/// handles ALP activation
unsigned int ResAlp::ManageAlpActivation(ORPComp *pcomp_alpsink, bool allow_timeout, bool *penterexitcrossingdetected) {
  if (pcomp_alpsink == NULL) {
    // shall have been set (could use an assert)
    RLOG_ERROR("null sink for ALP enter\n");
    return 0;
  }

  // check handle used for ALP enter
  if ((mAlpSinkCompOmxhdl) &&
      (mAlpSinkCompOmxhdl != pcomp_alpsink->OmxilCompHdl())) {
      RLOG_WARNING("confused by different handles for ALP enter\n");
      // reset delayed activation time in that case
      mAlpActivationTimeUs = 0;
  }

  // check if need to send ALP enter signal
  bool proceed_alp_activation = false;
  if (pcomp_alpsink->CFlag(ORPComp::CF_ACTIVE_RMCFG_ALP) == 0) {
    if (pcomp_alpsink->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER) == 0) {
      if (pcomp_alpsink->CFlag(ORPComp::CF_FAILED_RMCFG_ALP) == 0) {
        proceed_alp_activation = true;
      }  // else nothing to do: ALP blocked after previous failure
    }  // else nothing to do: already sent ALP enter signal, waiting for ack
  } else {
    if (pcomp_alpsink->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT)) {
      // ALP active with pending ALP exit command sent
      // can not send ALP enter signal: not supported by ALP sink..
      // need to wait ALP enter acknowledge before sending new ALP enter command
      // done even if allow_timeout is false: network activity will wake RME up
      RLOG_INFO("postponing ALP enter: ALP exit currently pending\n");
      if (penterexitcrossingdetected) {
        *penterexitcrossingdetected = true;
      }
      return 0;
    }
  }

  if (proceed_alp_activation == false) {
    // nothing to do
    return 0;
  }

  // possibly delay ALP activation
  unsigned int timeout_msec = 0;
  unsigned int alpdelayactivationms = allow_timeout ? PlatformLimits::ALP_DELAYACTIVATION_TIMEOUTMS : 0;
#ifdef __RMEUNITARYDEV__
  if (alpdelayactivationms) { alpdelayactivationms = 2; }  // make it short for tests
#endif

  if (alpdelayactivationms) {
    OMX_U64 current_timeus = omxilosalservices::OmxILOsalTimer::GetSystemTime();

    if (mAlpActivationTimeUs) {
      // compare current time to previously set activation time
      if (current_timeus < mAlpActivationTimeUs) {
        // not in time yet
        OMX_U64 deltatimeus = mAlpActivationTimeUs - current_timeus;
        timeout_msec = static_cast<unsigned int>(deltatimeus/1000);
      }
    } else {
      // compute ALP activation time
      OMX_U64 activationmindeltatimeus = alpdelayactivationms * 1000;
      mAlpActivationTimeUs = current_timeus + activationmindeltatimeus;
      timeout_msec = alpdelayactivationms;
    }
  }

  if (timeout_msec == 0) {
    // remember handle used for ALP enter
    mAlpSinkCompOmxhdl = pcomp_alpsink->OmxilCompHdl();
    // reset activation time
    mAlpActivationTimeUs = 0;

    // set ALP tracker file as soon as pending ALP enter
    AGov* agovM = SharedInst::GetAgov();
    agovM->UccReactAlpEnterRequested();

    // command ALP enter
    pcomp_alpsink->CpCmdSetRmConfigALPenter();
  } else {
    RLOG_INFO("ALP enter postponed by %dms\n", timeout_msec);
  }

  return timeout_msec;
}

/// handles ALP unactivation
void ResAlp::ManageAlpUnactivation(bool *penterexitcrossingdetected) {
  // try to reuse component used for ALP enter
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();

  ORPComp* pcomp_lp = NULL;
  if (mAlpSinkCompOmxhdl) {
    pcomp_lp = networkM->GetComponent(mAlpSinkCompOmxhdl);
  }

  if (pcomp_lp == NULL) {
    // sink not detected (possibly because was disconnected/unloaded)
    RLOG_DEBUG("no valid sink found for ALP exit\n");
    // reset
    mAlpSinkCompOmxhdl = NULL;
    mAlpActivationTimeUs = 0;
    AGov* agovM = SharedInst::GetAgov();
    agovM->UccReactAlpExitRequested();
    agovM->UccReactAlpExitApplied();
    return;
  }

  // check if need to send ALP exit signal
  if (pcomp_lp->CFlag(ORPComp::CF_ACTIVE_RMCFG_ALP)) {
    if (pcomp_lp->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT) == 0) {
      if (pcomp_lp->CFlag(ORPComp::CF_FAILED_RMCFG_ALP) == 0) {
        // reset
        mAlpSinkCompOmxhdl = NULL;
        mAlpActivationTimeUs = 0;

        AGov* agovM = SharedInst::GetAgov();
        agovM->UccReactAlpExitRequested();

        // command ALP exit
        pcomp_lp->CpCmdSetRmConfigALPMexit();
      }  // else nothing to do: ALP blocked after previous failure
    }  // else nothing to do: already sent ALP exit signal, waiting for ack
  } else {
    if (pcomp_lp->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER)) {
      // ALP not active with pending ALP enter sent
      // can not send ALP exit signal: not supported by ALP sink..
      // need to wait ALP enter acknowledge before sending new ALP exit command
      // done even if allow_timeout is false: network activity will wake RME up
      RLOG_INFO("postponing ALP exit: ALP enter currently pending\n");
      if (penterexitcrossingdetected) {
        *penterexitcrossingdetected = true;
      }
      return;
    }
  }
}

/// checks state transition for component
void ResAlp::CheckStateTransition(ORPComp* pcomp, DS_TSI_E transition_id) {
  if (pcomp == NULL)
    return;

  if (((transition_id == DS_TSI_WAITREQ) || (transition_id == DS_TSI_LOADEDREQ)) &&
      (PfRoles::IsCsCallRole(pcomp->Role()))) {
    // possibly spawn display and activity detector threads, if not done already,
    // when the CsCall component is detected
    // thus making sure it is associated with 'audio' rme instance (in adm process in Android case)
    AGov* agovM = SharedInst::GetAgov();
    agovM->StartDetectorThreads();
  } else if ((mAlpSinkCompOmxhdl == pcomp->OmxilCompHdl()) &&
      (StatesDuo::HasOnlyStatesExecuting(pcomp->DualStateOR()) == false) &&
      (pcomp->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT) == 0) &&
      (pcomp->CFlag(ORPComp::CF_FAILED_RMCFG_ALP) == 0)) {
    // ALP sink component has transitioned to non executing state
    if (pcomp->CFlag(ORPComp::CF_ACTIVE_RMCFG_ALP)) {
      // ALP was on: switch to normal mode is made automatically
      // ALP exit signal shall be sent by component
      RLOG_INFO("transition from executing while ALP entered, waiting for ALP-auto-exit applied signal\n");
      pcomp->SetCFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT);
    } else if (pcomp->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER)) {
      // ALP was being entered: don't expect ALP auto exit applied signal
      RLOG_INFO("transition from executing while entering ALP\n");
      HandleUnsollicitedAlpExited(pcomp);
    }
  }
}

/// handles unsollicited low power enter
void ResAlp::HandleUnsollicitedAlpEntered(ORPComp* pcomp) {
  if (pcomp == NULL)
    return;
  // case is not supported
}

/// handles unsollicited low power exit
void ResAlp::HandleUnsollicitedAlpExited(ORPComp* pcomp) {
  if (pcomp == NULL)
    return;

  if (pcomp->OmxilCompHdl() == mAlpSinkCompOmxhdl) {
    RLOG_INFO("AudioLowPower::ALP context reset to 0 - non RME triggered\n");
    pcomp->ClearCFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT);
    pcomp->ClearCFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER);
    pcomp->ClearCFlag(ORPComp::CF_ACTIVE_RMCFG_ALP);
    mAlpSinkCompOmxhdl = NULL;
    mAlpActivationTimeUs = 0;
    mContextIsAlpValid = false;

    AGov* agovM = SharedInst::GetAgov();
    agovM->UccReactAlpExitRequested();
    agovM->UccReactAlpExitApplied();
  }
}

}  // namespace

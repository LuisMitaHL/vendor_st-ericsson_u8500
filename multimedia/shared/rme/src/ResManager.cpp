/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ResManager.cpp
 * resource manager class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "ResManager.h"
#include "ORPCompNetwork.h"
#include "ORPCompHalfline.h"
#include "NmfDomains.h"
#include "PfDomManager.h"
#include "PfRoles.h"
#include "SharedInst.h"
#include "AGov.h"

#define FORBIDDIRECTSUSPENDFAILEDREQ
// for direct suspension after failed request:
#ifdef ALLOWDIRECTSUSPENDFAILEDREQ
#undef FORBIDDIRECTSUSPENDFAILEDREQ
#endif

namespace rme {

/// processes 'valid' transition related to a
/// dual OMXIL state / Resource state notification
void ResManager::NewDualStatesTransition(ORPComp* pcomp, DS_TSI_E transition_id, bool initjustdone) {
  if (pcomp == NULL)
    return;
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();

  RLOG_DEBUG("NewStatesOMXRES comp[i:%d - r:%s] (%s/%s)\n", pcomp->Index(), pcomp->Role(), StatesDuo::StringifyOmx(pcomp->OmxState()), StatesDuo::StringifyRes(pcomp->ResState()));

  bool getcap;

  // initial registering/processing for transition
  // => most processing will be done during 'work' time
  switch (transition_id) {
    // resources unload transitions
    case DS_TSI_UNLOAD_ALL_RESOURCES:
      /* going to invalid or loaded with previous resource state 'active'
         => need to free resources previously associated/requested to/from the component
         and to do some reinits
         but DO keep network connections
         and transition id is kept (to force resources levels reevaluation) */

      /* +specific ALP if was active or pending enter/exit:
         no ALP exit on LP sink component when moving to loaded state
         if ALP was active or pending enter => exit to be done by component itself */
      if ( pcomp->CFlag(ORPComp::CF_ACTIVE_RMCFG_ALP) ||
           pcomp->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER) ||
           pcomp->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT) ) {
        // simulate ALP exit signal
        RLOG_DEBUG("emulating ALP exit signal on comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());
        pcomp->SetCFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT);
        AGov* agovM = SharedInst::GetAgov();
        agovM->UccReactAlpExitRequested();
        AudioLowPowerExitApplied(pcomp, RM_E_NONE);
      }

      // reinitialize component; clear all control flags, except CF_UNREGISTERED
      pcomp->ReInitializeUnload();
      break;

    // 'wait-required' state => wait-for-resources transition
    case DS_TSI_WAITREQ:
      pcomp->SetCFlag(ORPComp::CF_INREQUESTPHASE);
      pcomp->ClearCFlag(ORPComp::CF_INDYNREQUESTPHASE);

      // directly retrieve capabilities and resources estimation from component
      getcap = (initjustdone) ? false : true;
      if (RetrieveCapabilitiesAndResEstim(pcomp, getcap) < 0) {
        // on failure: no error notification, but deny resources access
        // this will automatically clear the inrequestphase flag
        pcomp->CpNotifyResReservationRejected();
      }
      break;

    // 'loaded-required' state => 'initial' resources request transition
    case DS_TSI_LOADEDREQ:
      pcomp->SetCFlag(ORPComp::CF_INREQUESTPHASE);
      pcomp->ClearCFlag(ORPComp::CF_INDYNREQUESTPHASE);

      // directly retrieve capabilities and resources estimation from component
      getcap = (initjustdone) ? false : true;
      if (RetrieveCapabilitiesAndResEstim(pcomp, getcap) < 0) {
        // on failure: no error notification, but deny resources access
        // this will automatically clear the inrequestphase flag
        pcomp->CpNotifyResReservationRejected();
      }
      break;

    // additional-'dynamic' resources request transitions
    case DS_TSI_IDLESATISF_TO_IDLEREQ:
    case DS_TSI_PAUSESATISF_TO_PAUSEREQ:
    case DS_TSI_EXECUTINGSATISF_TO_EXECUTINGREQ:
      pcomp->SetCFlag(ORPComp::CF_INREQUESTPHASE);
      pcomp->SetCFlag(ORPComp::CF_INDYNREQUESTPHASE);

      // get (dynamic) resources estimation from component
      // -previous resources estimation levels are kept automatically:
      //  to be used in case of failure
      if (RetrieveCapabilitiesAndResEstim(pcomp, false) < 0) {
        // on failure: no error notification, but deny resources access
        // this will automatically clear the inrequestphase flag
        pcomp->CpNotifyResReservationRejected();
      } else {
        // backup and reset resource grant date
        pcomp->BackupResetDateResourceGrant();
        // need sorting (date impact) -- done also on WorkTime
        networkM->SortComponents();
      }
      break;

    // 'suspended' resources transitions
    // => implicit resources requests unless in full preemption
    case DS_TSI_IDLESATISF_TO_IDLESUSPEND:
    case DS_TSI_PAUSESATISF_TO_PAUSESUSPEND:
    case DS_TSI_EXECUTINGSATISF_TO_IDLESUSPEND:
    case DS_TSI_EXECUTINGSATISF_TO_PAUSESUSPEND:
      if (pcomp->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_FULL) == 0) {
        // implicit resource request: waiting until resources become available
        pcomp->SetCFlag(ORPComp::CF_INREQUESTPHASE);
        pcomp->ClearCFlag(ORPComp::CF_INDYNREQUESTPHASE);
        // no need to get resources estimation in this case

        if (pcomp->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_PARTIAL)) {
          // suspension originated from RME
          pcomp->ClearCFlag(ORPComp::CF_PENDING_RESPREEMPTION_PARTIAL);
        } else {
          // suspension originated from component itself
          RLOG_DEBUG("component originated suspension-case1\n");
        }
      }

      // destroy scratch domains of component
      pcomp->ClearPndsAndDomains(true);
      // do NOT send updated domains ids to component:
      // component is assumed to automatically reset hwpipe domain ids
      // upon suspension notification to RME
      break;
    case DS_TSI_IDLEREQ_TO_IDLESUSPEND:
    case DS_TSI_PAUSEREQ_TO_PAUSESUSPEND:
    case DS_TSI_EXECUTINGREQ_TO_IDLESUSPEND:
    case DS_TSI_EXECUTINGREQ_TO_PAUSESUSPEND:
      if (pcomp->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_FULL) == 0) {
        // implicit resource request: waiting until resources become available
        pcomp->SetCFlag(ORPComp::CF_INREQUESTPHASE);
        pcomp->ClearCFlag(ORPComp::CF_INDYNREQUESTPHASE);
        // no need to get resources estimation in this case

        // clear previous flags related to 'dynamic' resources request;
        // possible on-going preemptions requests can not be cancelled..
        // possible on-going preemptions/suspensions might be cancelled during 'work' time
        pcomp->ClearCFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED);

        if (pcomp->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_PARTIAL)) {
          // suspension originated from RME
          pcomp->ClearCFlag(ORPComp::CF_PENDING_RESPREEMPTION_PARTIAL);
        } else {
          // suspension originated from component itself
          RLOG_DEBUG("component originated suspension-case2\n");
        }
      }

      // destroy scratch domains of component
      pcomp->ClearPndsAndDomains(true);
      // do NOT send updated domains ids to component:
      // component is assumed to automatically reset hwpipe domain ids
      // upon suspension notification to RME
      break;
    case DS_TSI_PAUSESUSPEND_TO_IDLESUSPEND:
    case DS_TSI_IDLESUSPEND_TO_PAUSESUSPEND:
      // nothing specific to be done
      break;

    // 'to satisfied' transitions
    case DS_TSI_LOADEDREQ_TO_IDLESATISF:
    case DS_TSI_WAITREQ_TO_IDLESATISF:
    case DS_TSI_IDLEREQ_TO_IDLESATISF:
    case DS_TSI_IDLESUSPEND_TO_IDLESATISF:
    case DS_TSI_PAUSESATISF_TO_IDLESATISF:
    case DS_TSI_EXECUTINGSATISF_TO_IDLESATISF:
    case DS_TSI_IDLESATISF_TO_PAUSESATISF:
    case DS_TSI_LOADEDREQ_TO_PAUSESATISF:
    case DS_TSI_EXECUTINGSATISF_TO_PAUSESATISF:
    case DS_TSI_PAUSEREQ_TO_PAUSESATISF:
    case DS_TSI_PAUSESUSPEND_TO_PAUSESATISF:
    case DS_TSI_IDLESATISF_TO_EXECUTINGSATISF:
    case DS_TSI_LOADEDREQ_TO_EXECUTINGSATISF:
    case DS_TSI_PAUSESATISF_TO_EXECUTINGSATISF:
    case DS_TSI_EXECUTINGREQ_TO_EXECUTINGSATISF:
      // for transitions from required or suspended to satisfied: clear transition flag
      pcomp->ClearCFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED);

      // nothing else to be done
      break;

    default:
      // other transitions: nothing to be done
      break;
  }

  // specific ALP support
  mAlp.CheckStateTransition(pcomp, transition_id);
}

/// processes notification (internal or external) of
/// policy group priority and id from client
void ResManager::NewPolicyGP(ORPComp* pcomp,
    unsigned int new_gpid, unsigned int new_gpprio, bool internal) {
  if (pcomp == NULL)
    return;
  bool sortcomp = false;

  unsigned int cur_gpid = pcomp->PolGroupId();
  if (cur_gpid != new_gpid) {
    // group id change
    pcomp->SetPolGroupId(new_gpid);

    RLOG_DEBUG("NewPolicyCfg with new group:%d for comp[i:%d - r:%s]\n", new_gpid, pcomp->Index(), pcomp->Role());
  }

  unsigned int cur_gpprio = pcomp->PolGroupPriorityClient();
  if ((cur_gpprio != new_gpprio)) {
    // client priority change
    pcomp->SetPolGroupPriorityClient(new_gpprio);
    sortcomp = true;

    RLOG_DEBUG("NewPolicyCfg with new prio:%d for comp[i:%d - r:%s] gid:%d\n", new_gpprio, pcomp->Index(), pcomp->Role(), pcomp->PolGroupId());
  }

  // handle group priority coherency =>
  // enforce all components of same group (gid+pid+tid)
  // to have same group priority (client+system):
  // using last received value
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  for (ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    // group is based on gid+pid+tid
    if ((pcompiter->PolGroupId() == pcomp->PolGroupId()) &&
        (pcompiter->PolProcessId() == pcomp->PolProcessId()) &&
        (pcompiter->PolThreadId() == pcomp->PolThreadId())) {
      if (pcompiter->PolGroupPriorityClient() != pcomp->PolGroupPriorityClient()) {
        pcompiter->SetPolGroupPriorityClient(pcomp->PolGroupPriorityClient());
        sortcomp = true;
        RLOG_DEBUG("adjusting new client prio:%d for comp[i:%d - r:%s] gid:%d\n", pcompiter->PolGroupPriorityClient(), pcompiter->Index(), pcompiter->Role(), pcompiter->PolGroupId());
      }
      if (pcompiter->PolGroupPrioritySystem() != pcomp->PolGroupPrioritySystem()) {
        pcompiter->SetPolGroupPrioritySystem(pcomp->PolGroupPrioritySystem());
        sortcomp = true;
        RLOG_DEBUG("adjusting new system prio:%d for comp[i:%d - r:%s] gid:%d\n", pcompiter->PolGroupPrioritySystem(), pcompiter->Index(), pcompiter->Role(), pcompiter->PolGroupId());
      }
    }
  }

  if ((internal == false) && (pcomp->ResState() == RM_RS_SUSPENDED)) {
      // change of priority on suspended component
      // => reset date of resource grant
      pcomp->ResetDateResourceGrant();
  }

  if (sortcomp || internal) {
    // re-sort list of components per priority and resource date
    // higher priority components come first
    networkM->SortComponents();
  }

  /* actual consequences of the updated group priority or id
     will be encountered during 'Work' time */
}

/// processes notification of NMF domain ids applied
void ResManager::NmfDomainIdsApplied(ORPComp* pcomp, RM_STATUS_E nErrorCode,
      int data) {
  if (pcomp == NULL)
    return;
  if (data) {}

  // NMF domains applied
  if (pcomp->CFlag(ORPComp::CF_PENDING_RMCFG_NMFD) == 0) {
    // unexpected notification => discard
    RLOG_WARNING("RMConfigApplied/NMF_DOMAINS unexpected comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());
    return;
  }
  pcomp->ClearCFlag(ORPComp::CF_PENDING_RMCFG_NMFD);

  if (nErrorCode != RM_E_NONE) {
    pcomp->SetCFlag(ORPComp::CF_FAILED_RMCFG_NMFD);
    RLOG_INFO("RMConfigApplied/NMF_DOMAINS KO comp[i:%d - r:%s] e:%d\n", pcomp->Index(), pcomp->Role(), static_cast<int>(nErrorCode));
  } else {
    pcomp->SetCFlag(ORPComp::CF_ACTIVE_RMCFG_NMFD);
    RLOG_DEBUG("RMConfigApplied/NMF_DOMAINS OK comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());
  }
}

/// processes notification of audio low power enter applied
void ResManager::AudioLowPowerEnterApplied(ORPComp* pcomp,
    RM_STATUS_E nErrorCode, int udata) {
  if (pcomp == NULL)
    return;

  if (pcomp->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER) == 0) {
    // unexpected notification => discard
    RLOG_WARNING("RMConfigApplied/ALP-ENTER unexpected comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());
    mAlp.HandleUnsollicitedAlpEntered(pcomp);
    return;
  }
  pcomp->ClearCFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER);

  if (nErrorCode != RM_E_NONE) {
    pcomp->SetCFlag(ORPComp::CF_FAILED_RMCFG_ALP);
    RLOG_INFO("RMConfigApplied/ALP-ENTER KO comp[i:%d - r:%s] e:%d\n", pcomp->Index(), pcomp->Role(), static_cast<int>(nErrorCode));
    RLOG_WARNING("will not retry ALP activation on this component\n");
  } else {
    pcomp->SetCFlag(ORPComp::CF_ACTIVE_RMCFG_ALP);

    // specific reaction
    AGov* agovM = SharedInst::GetAgov();
    agovM->UccReactAlpEnterApplied();

    RLOG_INFO("RMConfigApplied/ALP-ENTER OK comp[i:%d - r:%s] ud:%d\n", pcomp->Index(), pcomp->Role(), udata);
  }
}

/// processes notification of audio low power exit applied
void ResManager::AudioLowPowerExitApplied(ORPComp* pcomp,
    RM_STATUS_E nErrorCode) {
  if (pcomp == NULL)
    return;

  if (pcomp->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT) == 0) {
    // unexpected notification => might be related to port disabled
    RLOG_WARNING("RMConfigApplied/ALP-EXIT unexpected comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());
    mAlp.HandleUnsollicitedAlpExited(pcomp);
    return;
  }
  pcomp->ClearCFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT);

  if (nErrorCode != RM_E_NONE) {
    pcomp->SetCFlag(ORPComp::CF_FAILED_RMCFG_ALP);
    RLOG_INFO("RMConfigApplied/ALP-EXIT KO comp[i:%d - r:%s] e:%d\n", pcomp->Index(), pcomp->Role(), static_cast<int>(nErrorCode));
    RLOG_WARNING("will not retry ALP activation on this component\n");
  } else {
    // specific reaction
    AGov* agovM = SharedInst::GetAgov();
    agovM->UccReactAlpExitApplied();

    RLOG_INFO("RMConfigApplied/ALP-EXIT OK comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());
  }

  // whatever exit status: upon reception of this call, clear ALP status
  // cover specific crossing cases, especially for component initiated ALP exit
  pcomp->ClearCFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER);
  pcomp->ClearCFlag(ORPComp::CF_ACTIVE_RMCFG_ALP);
}

/// works during 'idle' time
unsigned int ResManager::WorkTime(bool calledontimeout) {
  RLOG_DEBUG("ResManager m-processing; coto:%d\n", calledontimeout);
  if (calledontimeout) {
    // no direct usage for the time being of this information:
    // indirect usage through systematic processing (ALP checks for instance)
    // -> conditional check kept in order to remove 'unused' warning
  }

  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  // cleanup: remove any pending unregistered component
  networkM->RemoveUnregComponents();
  // cleanup: update halflines when needed
  // and especially remove unused halflines
  networkM->UpdateHalflines();
  // resort network (especialy for components in loaded state: clear grant date)
  networkM->SortComponents();

  // check for visual activity type
  CheckVisualActivityType();

  // audio low power management
  // to be done BEFORE processing of resources requests
  unsigned int timeout_msec_alp = mAlp.ProcessAudioLowPower("Network", true);

  // process resources requests
  unsigned int timeout_msec_preempt = ProcessResRequests();

  // return 0 (no timeout) or minimal non null timeout value
  if (timeout_msec_alp && timeout_msec_preempt) {
    // both timeouts are activated: set minimal one
    if (timeout_msec_alp < timeout_msec_preempt) {
      return timeout_msec_alp;
    } else {
      return timeout_msec_preempt;
    }
  } else {
    if (timeout_msec_alp) {
      return timeout_msec_alp;
    } else {
      return timeout_msec_preempt;
    }
  }
}

/// retrieves capabilities and resources estimation from component
/// possibly updates network following priority change
int ResManager::RetrieveCapabilitiesAndResEstim(ORPComp* pcomp, bool getcap) {
  if (pcomp == NULL)
    return -1;

  RM_STATUS_E status;

  if ((getcap == true) || (pcomp->CFlag(ORPComp::CF_CAPABILITIES_VALID) == 0))  {
    // get capabilities
    // *even* if already done in LOADED/WAIT state on first use
    // (for notify tunnel or priority for instance) since parameters might have been changed
    status = pcomp->CpGetCapabilities();
    if (status == RM_E_NONE) {
      // internal notification for priority 'change'
      // needed for group priority coherency and network sorting (per prio)
      NewPolicyGP(pcomp, pcomp->PolGroupId(), pcomp->PolGroupPriorityClient(), true);
    } else {
      RLOG_ERROR("err:%d CpGetCapabilities comp[i:%d - r:%s] %s\n", status, pcomp->Index(), pcomp->Role(), pcomp->DualStateORstr());
      return -1;
    }
  }

  // get or re-get resources estimation from OMXIL component (might have changed since last time)
  status = pcomp->CpGetResourcesEstimation();
  if (status != RM_E_NONE) {
    RLOG_ERROR("CpGetResourcesEstimation status:%d comp[i:%d - r:%s] %s\n", status, pcomp->Index(), pcomp->Role(), pcomp->DualStateORstr());
    return -1;
  }

  return 0;
}

/// processes resources preemption-suspend
void ResManager::ProcessResourcesPremptSuspend(ORPComp* pcomp) {
  if (pcomp == NULL)
    return;

  if (pcomp->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_PARTIAL) == 0) {
    // trig resources suspension ..unless request already sent
    // if suspension is not supported by OMXIL component, unload is to be done instead
    if (pcomp->CpCmdSuspendResources() < 0) {
      // suspend command nok; switch to unload command
      ProcessResourcesPreemptUnload(pcomp);
    }
  }
}

/// processes resources preemption-unload
void ResManager::ProcessResourcesPreemptUnload(ORPComp* pcomp) {
  if (pcomp == NULL)
    return;

  if (pcomp->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_FULL)) {
    // request already sent or marked; being processed
    return;
  }

  // trig resources unload
  pcomp->CpCmdUnloadResources();
}

/// processes resources requests
unsigned int ResManager::ProcessResRequests() {
  bool candosvaresproc = true;
  bool candosiaresproc = true;

  bool has_pending_respreemption_full = false;
  bool has_pending_alp_transition = false;
  bool has_enteringoractive_alp = false;

  // scan ALL components
  // -clear working flags pending preempt-U/S, pending grant
  // -check component waiting for end of total preemptions
  // -check preemption pending components
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  for (ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    pcompiter->ClearCFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL);
    pcompiter->ClearCFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL);
    pcompiter->ClearCFlag(ORPComp::CF_RWL_CANDIDATE_RESGRANT);

    // check for components that were requesting preemption (full/partial)
    // but that didnot acknowledge yet the associated state transition(s)
    if ((pcompiter->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_FULL)) ||
        (pcompiter->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_PARTIAL))) {
      if (pcompiter->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_FULL)) {
        has_pending_respreemption_full = true;
      }

      bool skipcomp = false;
      // check for timeout..
      if (pcompiter->IsTimedOutReactResReleaseCmd() == true) {
        RLOG_INFO("preemption timeout for comp[i:%d - r:%s] %s\n", pcompiter->Index(), pcompiter->Role(), pcompiter->DualStateORstr());

        //+try error notification to OMXIL component:
        // shall abort (invalidate) it if it is not totally deadlocked..
        pcompiter->CpNotifyError(RM_E_TIMEOUT);

        if (pcompiter->OmxState() == OMX_StateIdle) {
          // work hypothesis / assumption: we assume that at least dynamic resources
          // are not in use after deadlock timeout and so that we can keep going
          // by attributing these resources to other components
          skipcomp = true;
        }
      }

      if (skipcomp == false) {
        // temporize resources processing for SVA and/or SIA and ESRAM
        // to make sure that when granting resources, these are really immediately available
        const ResLevels* pcompreslevels = pcompiter->ResourcesEstimLevels();
        if (pcompreslevels) {
          if (pcompreslevels->IsSvaBased() == true) {
            candosvaresproc = false;
          }
          if (pcompreslevels->IsSiaBased() == true) {
            candosiaresproc = false;
          }
          if (pcompreslevels->IsUsingEsram() == true) {
            // cross SxA usages protection
            candosvaresproc = false;
            candosiaresproc = false;
          }
        }  // else assume non SxA based component
      }
    }

    // check for component in pending state ALP exit or enter
    if (((pcompiter->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_EXIT)) ||
        (pcompiter->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER))) &&
        (pcompiter->CFlag(ORPComp::CF_FAILED_RMCFG_ALP) == 0)) {
      has_pending_alp_transition = true;
    }
    if (((pcompiter->CFlag(ORPComp::CF_PENDING_RMCFG_ALP_ENTER)) ||
        (pcompiter->CFlag(ORPComp::CF_ACTIVE_RMCFG_ALP))) &&
        (pcompiter->CFlag(ORPComp::CF_FAILED_RMCFG_ALP) == 0)) {
      has_enteringoractive_alp = true;
    }
  }

  if (candosvaresproc == false)
    RLOG_DEBUG("skip1 resources processing for SVA\n");
  if (candosiaresproc == false)
    RLOG_DEBUG("skip1 resources processing for SIA\n");
  if ((candosvaresproc == false) && (candosiaresproc == false)) {
    RLOG_DEBUG("ProcessResRequests postponed (p1) by %dms\n", PlatformLimits::PREEMPTION_ACKNOWLEDGE_TIMEOUTMS);
    return PlatformLimits::PREEMPTION_ACKNOWLEDGE_TIMEOUTMS;
  }

  // scan resources: mark candidates for preemption (full/partial)
  //  -SVA and SIA resources (MCPS)
  //    resolve conflicts through suspension markers
  //  -ESRAM scratch domains overlap conflicts between SVA and SIA
  //    resolve conflicts through suspension markers
  //  -ESRAM non scratch domain conflicts (ISP only currently) / ISP HW handling
  //    resolve conflicts through suspension markers
  //    (assuming ISP usage removed while in suspension)
  ResLevels reslev;
  mResEstimator.ScanResourcesMarkPreempts(&reslev);
  reslev.DumpLevels("targeted total");

  // process candidates
  // - for preemption:
  //    - full
  //    - partial
  // - and mark for resources grant when possible:
  //    - resources requests 'required'
  //    - resources requests 'waiting/suspended'
  ProcessPreemptsMarkGrants(&candosvaresproc, &candosiaresproc);

  // postpone any resources grant in case of ALP transition
  if (has_pending_alp_transition) {
    RLOG_INFO("ProcessResRequests postponed (ALP transition) by %dms\n", PlatformLimits::ALP_TRANSITIONRECHECKRES_TIMEOUTMS);
    return PlatformLimits::ALP_TRANSITIONRECHECKRES_TIMEOUTMS;
  } else if (has_enteringoractive_alp == false) {
#ifndef __RMEUNITARYDEV__
    // specific in case of multiple rme instantiations... (hack)
    AGov* agovM = SharedInst::GetAgov();
    if ((agovM->IsVisualProcess() == true) &&
        (agovM->IsActivityTrackerAlpOn() == true)) {
      // in rme 'visual instance': don't allow resources grant if ALP detected
      // this relies on good alp tracker file..
      RLOG_INFO("ProcessResRequests postponed (ALP activity detected) by %dms\n", PlatformLimits::ALP_TRANSITIONRECHECKRES_TIMEOUTMS);
      return PlatformLimits::ALP_TRANSITIONRECHECKRES_TIMEOUTMS;
    }
#endif
  }

  if (candosvaresproc == false)
    RLOG_DEBUG("skip2 resources processing for SVA\n");
  if (candosiaresproc == false)
    RLOG_DEBUG("skip2 resources processing for SIA\n");
  if ((candosvaresproc == false) && (candosiaresproc == false)) {
    // either ARM based component or ESRAM conflict
    RLOG_DEBUG("ProcessResRequests postponed (p2) by %dms\n", PlatformLimits::PREEMPTION_ACKNOWLEDGE_TIMEOUTMS);
    return PlatformLimits::PREEMPTION_ACKNOWLEDGE_TIMEOUTMS;
  }

  // specific activity for UCC: visualFHD context detection/mode trig
  mResEstimator.ScanActiveResourcesVisualMode(&reslev);
  if (reslev.VisualFHD() == RM_UCC_VISUAL_FHD) {
    if (mResEstimator.IcnVisualResMode() <= 0) {
      mResEstimator.SetIcnVisualResMode(1);
      RLOG_DEBUG("NotifyInterconnectBwMode FHD=true\n");
      // callback 1080p context
      SharedInst::NotifyInterconnectBwMode(OMX_TRUE);
    }
  } else {
    if (mResEstimator.IcnVisualResMode() >= 0) {
      mResEstimator.SetIcnVisualResMode(-1);
      RLOG_DEBUG("NotifyInterconnectBwMode FHD=false\n");
      // callback non 1080p context
      SharedInst::NotifyInterconnectBwMode(OMX_FALSE);
    }
  }

  // process resources grants (+domains attribution)
  ProcessGrants(candosvaresproc, candosiaresproc);

  // rescan ALL components
  // -clear working flags pending preempt-U/S, pending grant
  // -check preemption pending components: check if timeout needed
  bool haspendingcomps = false;
  for (ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    pcompiter->ClearCFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL);
    pcompiter->ClearCFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL);
    pcompiter->ClearCFlag(ORPComp::CF_RWL_CANDIDATE_RESGRANT);

    // check for components that were requesting preemption (full/partial)
    // but that didnot acknowledge yet the associated state transition(s)
    if ((pcompiter->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_FULL)) ||
        (pcompiter->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_PARTIAL))) {
      haspendingcomps = true;
    }
  }

  if (haspendingcomps == true) {
    RLOG_DEBUG("ProcessResRequests postponed (p3) by %dms\n", PlatformLimits::PREEMPTION_ACKNOWLEDGE_TIMEOUTMS);
    return PlatformLimits::PREEMPTION_ACKNOWLEDGE_TIMEOUTMS;
  } else {
    return 0;
  }
}

/// processes resources preemptions and marks resources grants
void ResManager::ProcessPreemptsMarkGrants(bool *pcandosvaresproc, bool *pcandosiaresproc) {
  if ((pcandosvaresproc == NULL) || (pcandosiaresproc == NULL))
    return;

  // - process candidates for preemption (full/partial)
  // - mark for resources grant when possible:
  //    - resources requests 'required'
  //    - resources requests 'waiting/suspended'
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  for (ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    if (mResEstimator.CanProceedResources(pcompiter) == NULL) {
      // skip item
      continue;
    }

    // check if a previous request for full preemption was done on the OMXIL component
    // (and not yet acknowledged)
    if (pcompiter->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_FULL)) {
      if ((pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL) == 0) &&
          ((pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL) == 0) ||
           (pcompiter->PolCanSuspend() == true))) {
        // too bad situation: full preemption request was done previously on component
        // that could now get resources back or at least be in suspend..
        // but no cancellation possible
        // => typically happens on components that didnot allowed suspension..
        RLOG_DEBUG("tbs.. unrecoverable full preemption for comp[i:%d - r:%s] %s\n", pcompiter->Index(), pcompiter->Role(), pcompiter->DualStateORstr());
      }
      // nothing to be done: can not revover fully preempted component
      continue;
    }

    // will either unload, suspend, grant resources or do nothing
    bool cmdunloadresources = false;
    bool cmdsuspendresources = false;
    bool candidategrantresources = false;

    // differenciation per resource state / omxil state
    // assuming no full preemption request previously done on component
    // (already checked above)
    if (pcompiter->ResState() == RM_RS_SATISFIED) {
      if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL)) {
        cmdunloadresources = true;
      } else if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL)) {
        cmdsuspendresources = true;
      } else {
        if (pcompiter->CFlag(ORPComp::CF_PENDING_RESPREEMPTION_PARTIAL)) {
          // partial preemption request was done previously on component
          // (and not yet acknowledged)
          // will grant resources to cancel this request ..unless already done
          candidategrantresources = true;
        }  // else nothing to do: keep satisfied
      }
    } else if (pcompiter->ResState() == RM_RS_SUSPENDED) {
      if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL)) {
        cmdunloadresources = true;
      } else if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL)) {
        // nothing to do: keep suspended
      } else {
        candidategrantresources = true;
      }
    } else if (pcompiter->ResState() == RM_RS_REQUIRED) {
      if (pcompiter->OmxState() == OMX_StateWaitForResources) {
        if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL)) {
          if (pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED)) {
            // grant for resources is pending: need to cancel it
            cmdunloadresources = true;
          }  // else nothing to do: no resources available, keep in wait for resources state
        } else if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL)) {
          if (pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED)) {
            // grant for resources is pending: need to cancel it
            cmdsuspendresources = true;
          }  // else nothing to do: no resources available, keep in wait for resources state
        } else {
          candidategrantresources = true;
        }
      } else if (pcompiter->OmxState() == OMX_StateLoaded) {
        bool rejectreq = false;
        if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL)) {
          if (pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED)) {
            // grant for resources is pending: need to cancel it
            cmdunloadresources = true;
          } else {
            rejectreq = true;
          }
        } else if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL)) {
          if (pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED)) {
            // grant for resources is pending: need to cancel it
            cmdsuspendresources = true;
          } else {
            // alternate behavior to have on 'partial' failure for L2I resources requests
            //  -direct failure
            //  OR
            //  -grant resources (allow transition to Idle) and then directly suspend
            //    => somewhat dangereous since allows for potential moves to executing
            //    but inverted commands (suspend first, allows second) would need specific processing
            //    in OMXIL component to reorder commands to OMXIL client (would get confused otherwise)
            //    => will hide away most loaded to idle failure
            //    since most cases are covered through suspend
            //  => second case assures coherent final state of network, independently of order of arrivals
            //     of requests to rme
#ifndef FORBIDDIRECTSUSPENDFAILEDREQ
            if (pcompiter->PolCanSuspend() == true) {
              candidategrantresources = true;
              // command to suspend will be sent only once grant resources is sent
            }
#endif
            if (candidategrantresources == false) {
              rejectreq = true;
            }
          }
        } else {
          candidategrantresources = true;
        }

        if (rejectreq == true) {
          // fail the 'required' resources state
          // note that this will revert internally the resources state to NONE
          // => request processing done once only
          pcompiter->CpNotifyResReservationRejected();
          // force uninits (as if moved to loaded,none from loaded,required)
          pcompiter->ReInitializeUnload();
        }
      } else {  // dynamic resources idle/pause/executing
        bool rejectdynreq = false;
        if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL)) {
          if (pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED)) {
            // grant for resources is pending: need to cancel it
            cmdunloadresources = true;
          } else {
            rejectdynreq = true;
          }
        } else if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL)) {
          if (pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED)) {
            // grant for resources is pending: need to cancel it
            cmdsuspendresources = true;
          } else {
            // alternate behavior to have on 'partial' failure for dynamic resources requests
            //  -direct failure (+revert resources estimation data to previous values)
            //  OR
            //  -grant resources and then directly suspend
            //    => somewhat dangereous since allows for potential moves to executing
            //    but inverted commands (suspend first, allows second) would need specific processing
            //    in OMXIL component to reorder commands to OMXIL client (would get confused otherwise)
            //    => will hide away most loaded to idle failure
            //    since most cases are covered through suspend
            //  => second case assures coherent final state of network, independently of order of arrivals
            //     of requests to rme
#ifndef FORBIDDIRECTSUSPENDFAILEDREQ
            if (pcompiter->PolCanSuspend() == true) {
              candidategrantresources = true;
              // command to suspend will be sent only once grant resources is sent
            }
#endif
            if (candidategrantresources == false) {
              rejectdynreq = true;
            }
          }
        } else {
          candidategrantresources = true;
        }

        if (rejectdynreq == true) {
          // fail the 'required' resources state
          // note that this will revert internally the resources state to SATISFIED
          // => request processing done once only
          pcompiter->CpNotifyResReservationRejected();

          // revert resources estimation and resource grant date to PREVIOUS state
          // otherwise on next call to idleT,
          // if component didnot change the dual state of the comp
          // or didnot make new resources request,
          // the component WOULD be commanded to unload or suspend
          // => reaction to dynamic request failure shall be done by OMXIL component
          pcompiter->RevertResEstimUccValues();
          pcompiter->RevertDateResourceGrant();
          // would be good to have a way to trig restart of resources processing
          // (with old/previous data) to avoid potential useless preemptions
          // => at least it will be done on next iteration call to IdleT()
          // so possibly bad for preempted components in the meantime..
        }
      }
    }

    bool checkpending = false;
    if (cmdunloadresources == true) {
      // trig resources unload ..unless request already sent
      ProcessResourcesPreemptUnload(pcompiter);
      checkpending = true;
    } else if (cmdsuspendresources == true) {
      // trig resources suspension ..unless request already sent
      // if suspension is not supported by OMXIL component, unload is done instead
      ProcessResourcesPremptSuspend(pcompiter);
      checkpending = true;
    } else if (candidategrantresources == true) {
      // will grant resources ..unless already done
      if (pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED) == 0) {
        pcompiter->SetCFlag(ORPComp::CF_RWL_CANDIDATE_RESGRANT);
      }
    }

    if (checkpending) {
      // need to temporize resources processing for SVA and/or SIA and ESRAM
      // to make sure that when granting resources, these are really immediately available
      const ResLevels* pcompreslevels = pcompiter->ResourcesEstimLevels();
      if (pcompreslevels) {
        if (pcompreslevels->IsSvaBased() == true) {
          *pcandosvaresproc = false;
        }
        if (pcompreslevels->IsSiaBased() == true) {
          *pcandosiaresproc = false;
        }
        if (pcompreslevels->IsUsingEsram() == true) {
          // cross SxA usages protection
          *pcandosvaresproc = false;
          *pcandosiaresproc = false;
        }
      }  // else assume non SxA based component
    }
  }
}

/// processes resources grants
void ResManager::ProcessGrants(bool candosvaresproc, bool candosiaresproc) {
  bool neednetworksort = false;
  // process components marked for resources grant
  // reverse order scan=> in order to keep current priority order
  // by having earliest date of resource grant given to component of highest priority
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
 for (ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    const ResLevels* pcompreslevels = mResEstimator.CanProceedResources(pcompiter);
    if (pcompreslevels == NULL) {
      // skip item; assume non SxA based component
      continue;
    }
    if (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESGRANT) == 0) {
      continue;
    }

    // check if SxA processing is forbiden vs resource usage of the component
    if ((candosvaresproc == false) && (pcompreslevels->IsSvaBased() == true))
      continue;
    if ((candosiaresproc == false) && (pcompreslevels->IsSiaBased() == true))
      continue;

    bool grantresources = false;

    // domains attribution (done in wait or loaded states or for un-suspension or for dynamic scratch domains);
    bool justsetsendnmfdomains = SetSendNmfDomains(pcompiter);

    if (justsetsendnmfdomains == false) {
      // domains were set already:
      // check if RM config for NMF domains acknowledge notification(s) was (were) received
      int nmfdomainscmdackstatus = CheckNmfDomainsCmdAckStatus(pcompiter);

      if (nmfdomainscmdackstatus < 0) {
        // NMF domains setting failed:
        // fail the 'required' resources state
        // note that this will revert internally the resources state to NONE
        // => request processing donce once only
        pcompiter->CpNotifyResReservationRejected();
        // force uninits (as if moved to loaded,none from loaded/wait,required)
        pcompiter->ReInitializeUnload();
      } else if (nmfdomainscmdackstatus > 0) {
        // NMF domains setting succeeded:
        if (pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED) == 0) {
          // grant resources ..unless grant already sent
          grantresources = true;
        }
      }  // else all acknowledge for nmf domains not yet received
    } else {
      // ok just set NMF domains:
      // wait for corresponding notification(s) before granting/rejecting resources
      // => will be done in a later call to idleT
    }

    if (grantresources) {
      pcompiter->CpNotifyResReservationGranted();

      // update resources grant date in ALL cases (initial, dynamic and unsuspend)
      pcompiter->UpdateDateResourceGrant();
      // date update has impacts on components sorting
      neednetworksort = true;

#ifndef FORBIDDIRECTSUSPENDFAILEDREQ
      // specific: granted resources to component candidate for partial preemption
      // (and component known/checked to support suspension)
      // (instead of failing the resource request)
      // => immediate suspension
      if ((pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL)) &&
          (pcompiter->PolCanSuspend() == true)) {
        ProcessResourcesPremptSuspend(pcompiter);
      }
#endif
    }
  }

  if (neednetworksort == true) {
    networkM->SortComponents();
  }
}

/// sets and sends domains attribution
bool ResManager::SetSendNmfDomains(ORPComp* pcomp) {
  if (pcomp == NULL)
    return false;

  if ((pcomp->CFlag(ORPComp::CF_PENDING_RMCFG_NMFD)) ||
      (pcomp->CFlag(ORPComp::CF_FAILED_RMCFG_NMFD))) {
    // domains set already: command configuration either pending or failed
    return false;
  }

  bool activereconfig = false;
  if (pcomp->CFlag(ORPComp::CF_ACTIVE_RMCFG_NMFD)) {
    if (pcomp->CFlag(ORPComp::CF_INDYNREQUESTPHASE) == 0) {
      // domains active (command configuration succeed) and not in dynamic request phase
      return false;
    } else {
      // domains active in dynamic request phase: reconfig allowed
      activereconfig = true;
    }
  }  // else no active domains: initial settings

  bool somepndset = false;

  // DDR-MPC domains attribution based on RM_CAP_DISTUSESVA / RM_CAP_DISTUSESIA flag

  // PlatformDomains::DHM_PROCSVA: generic SVA DDR-MPC domain
  if (pcomp->CapBitmap() & RM_CAP_DISTUSESVA) {
    if (pcomp->PndForDtype(PlatformDomains::DHM_PROCSVA) == PlatformDomains::PND_NA) {
      PlatformDomains::PNDS_E pndsva = PlatformDomains::PND_PROCSVA_DDR_GEN;
      RLOG_DEBUG("comp[i:%d - r:%s] ProcSVA pnd:%d\n", pcomp->Index(), pcomp->Role(), static_cast<int>(pndsva));
      pcomp->SetPndForDtype(pndsva, PlatformDomains::DHM_PROCSVA);
      somepndset = true;
    }
  }

  // PlatformDomains::DHM_PROCSIA: generic SVA DDR-MPC domain
  if ((pcomp->CapBitmap() & RM_CAP_DISTUSESIA)
#ifdef SW_VARIANT_ANDROID
      // audio hack: shall be suppressed once proper AFM used in Android with setting of RM_CAP_DISTUSESIA
      || (PfRoles::IsAudioRole(pcomp->Role()) == true)
#endif
      ) {
    if (pcomp->PndForDtype(PlatformDomains::DHM_PROCSIA) == PlatformDomains::PND_NA) {
      PlatformDomains::PNDS_E pndsia = PlatformDomains::PND_PROCSIA_DDR_GEN;
      // check for specific resource usage request
      if ((pcomp->UccBitmap() & RM_UCC_AUDIO_ESRAM) && (PfRoles::IsAudioRole(pcomp->Role()) == true)) {
        pndsia = PlatformDomains::PND_PROCSIA_DDRESRAM_AUDIO;
      }
      RLOG_DEBUG("comp[i:%d - r:%s] ProcSIA pnd:%d\n", pcomp->Index(), pcomp->Role(), static_cast<int>(pndsia));
      pcomp->SetPndForDtype(pndsia, PlatformDomains::DHM_PROCSIA);
      somepndset = true;
    }
  }

  int status = 0;

  // ESRAM HW domains attributions based on resources request
  const ResLevels* preslevels = pcomp->ResourcesEstimLevels();
  if (preslevels) {
    RM_ESRAMBUFID_E esramfid;
    PlatformDomains::PNDS_E pnd;

    // PlatformDomains::DHM_HWPIPESVA
    esramfid = preslevels->FidEsramBufSvaHwPipe();
    pnd = PfDomManager::GetPndAssociatedToEsramFidHwtyp(esramfid,
        PlatformDomains::DHM_HWPIPESVA);
    if (pnd != pcomp->PndForDtype(PlatformDomains::DHM_HWPIPESVA)) {
      RLOG_DEBUG("comp[i:%d - r:%s] HWSVA esramfid:%d, pnd:%d\n", pcomp->Index(), pcomp->Role(), static_cast<int>(esramfid), static_cast<int>(pnd));
      if (pcomp->SetPndForDtype(pnd, PlatformDomains::DHM_HWPIPESVA))
        status = -1;
      somepndset = true;
    }

    // PlatformDomains::DHM_HWPIPESIA
    esramfid = preslevels->FidEsramBufSiaHwPipe();
    pnd = PfDomManager::GetPndAssociatedToEsramFidHwtyp(esramfid,
        PlatformDomains::DHM_HWPIPESIA);
    if (pnd != pcomp->PndForDtype(PlatformDomains::DHM_HWPIPESIA)) {
      RLOG_DEBUG("comp[i:%d - r:%s] HWSIA esramfid:%d, pnd:%d\n", pcomp->Index(), pcomp->Role(), static_cast<int>(esramfid), static_cast<int>(pnd));
      if (pcomp->SetPndForDtype(pnd, PlatformDomains::DHM_HWPIPESIA))
        status = -1;
      somepndset = true;
    }

    // PlatformDomains::DHM_HWXP70SIA
    esramfid = preslevels->FidEsramBufSiaHwXP70();
    pnd = PfDomManager::GetPndAssociatedToEsramFidHwtyp(esramfid,
        PlatformDomains::DHM_HWXP70SIA);
    if (pnd != pcomp->PndForDtype(PlatformDomains::DHM_HWXP70SIA)) {
      RLOG_DEBUG("comp[i:%d - r:%s] HWXP70 esramfid:%d, pnd:%d\n", pcomp->Index(), pcomp->Role(), static_cast<int>(esramfid), static_cast<int>(pnd));
      pcomp->SetPndForDtype(pnd, PlatformDomains::DHM_HWXP70SIA);
      somepndset = true;
    }
  }

  if (status == 0) {
    // send RMConfig/NMFD command to OMXIL component
    // send only if needed
    if (somepndset) {
      if (activereconfig == true) {
        RLOG_DEBUG("dynamic reconfiguration of NMF Domains Ids\n");
        pcomp->ClearCFlag(ORPComp::CF_ACTIVE_RMCFG_NMFD);
      }
      pcomp->CpCmdSetRmConfigNmfDomainIds();
    } else {
      RLOG_DEBUG("(skipped CpCmdSetRmConfigNmfDomainIds for comp[i:%d - r:%s])\n", pcomp->Index(), pcomp->Role());
      pcomp->SetCFlag(ORPComp::CF_ACTIVE_RMCFG_NMFD);
    }

    // in case no domains needing to be sent: check if already available
    if (CheckNmfDomainsCmdAckStatus(pcomp) > 0)
      return false;
    else
      return true;
  } else {
    // simulate failure of NMF domains settings (even if config not sent to OMXIL component)
    // in order to trig rejection of resources request upon failure to create NMF scratch domain
    pcomp->SetCFlag(ORPComp::CF_FAILED_RMCFG_NMFD);
    return false;
  }
}

/// checks NMF domains command acknowledge status <0:ko, 0=na, >0:ok
int ResManager::CheckNmfDomainsCmdAckStatus(ORPComp* pcomp) {
  if (pcomp->CFlag(ORPComp::CF_FAILED_RMCFG_NMFD)) {
    // failed
    return -1;
  } else if (pcomp->CFlag(ORPComp::CF_ACTIVE_RMCFG_NMFD) == 0) {
    // not active (might or not be pending)
    return 0;
  } else {
    // active
    return 1;
  }
}

/// checks visual activity type
void ResManager::CheckVisualActivityType() {
  bool withvisualactivity = false;
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  for (ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    // need component with resources (possibly suspended)
    if (pcompiter->ResState() == RM_RS_NONE) {
      continue;
    }

    // detect visual activity
    // => assuming audio components have consistents roles..
    if (PfRoles::IsAudioRole(pcompiter->Role()) == false) {
      const ResLevels* pcompreslevels = pcompiter->ResourcesEstimLevels();
      if (pcompreslevels &&
          ((pcompreslevels->IsSvaBased() == true) || (pcompreslevels->IsSiaVisualBased() == true))) {
        withvisualactivity = true;
        break;
      }
    }
  }

  // specific reaction
  AGov* agovM = SharedInst::GetAgov();
  if (withvisualactivity) {
    agovM->UccReactVisualActivityOn();
  } else {
    agovM->UccReactVisualActivityOff();
  }
}

}  // namespace

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file RMEimpl.cpp
 * RME implementation class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#define _RMEIMPLCPP
#include "uosal.h"
#undef _RMEIMPLCPP

#include "RMEimpl.h"
#include "SharedInst.h"
#include "PfRoles.h"

#if defined(SW_VARIANT_ANDROID)
#include "linux_utils.h"
#include <stdlib.h>
#endif

namespace rme {

// Trace property
int Debug::mTraceEnabled = 0;
#ifdef ANDROID
#define TRACE_ENABLED_PROPERTY_NAME "ste.omx.rme"
#else
#define TRACE_ENABLED_PROPERTY_NAME "ste_omx_rme"
#endif

// RME interface class is a singleton
RMEimpl* RMEimpl::mInstance = NULL;

// mutex shared between all rme threads
omxilosalservices::OmxILOsalMutex* RMEimpl::mpMutexAsync = NULL;

/// acquires instance for RME interface / singleton class
/// has reference count support
RME* RME::getInstance() {
  if (RMEimpl::mpMutexAsync == NULL) {
    omxilosalservices::OmxILOsalMutex::MutexCreate(RMEimpl::mpMutexAsync);
    if (RMEimpl::mpMutexAsync == NULL) {
      RLOG_ERROR("OmxILOsalMutex::MutexCreate failed for RMEimpl\n");
      return NULL;
    }
  }
  RMEimpl::mpMutexAsync->MutexLock();

  if (RMEimpl::mInstance == NULL) {
#if defined(SW_VARIANT_ANDROID)
    GET_PROPERTY(TRACE_ENABLED_PROPERTY_NAME, value, "0");
    Debug::mTraceEnabled = atoi(value);
#endif
    RLOG_DEBUG("new RMEimpl instance\n");
    RMEimpl::mInstance = new RMEimpl();  // mRefCount set to 1
    if (RMEimpl::mInstance) {
      // init: will create NMF domains
      int status = RMEimpl::mInstance->Init();
      if (status < 0) {
        // refusing to keep going..
        // possible causes: platform domains table badly formed or
        // NMF CM domains failure
        RLOG_ERROR("RMEimpl init\n");
        delete RMEimpl::mInstance;
        RMEimpl::mInstance = NULL;
      }
    } else {
      RLOG_ERROR("RMEimpl instance\n");
    }
  } else {
    RMEimpl::mInstance->mRefCount++;
    RLOG_DEBUG("RME-refcount++ %d\n", RMEimpl::mInstance->mRefCount);
  }

  RMEimpl::mpMutexAsync->MutexUnlock();
  return RMEimpl::mInstance;
}

/// releases instance for RME interface / singleton class
/// has reference count support
void RME::delInstance() {
  if (RMEimpl::mpMutexAsync == NULL) {
    return;
  }
  RMEimpl::mpMutexAsync->MutexLock();
  if (RMEimpl::mInstance != NULL) {
    RMEimpl::mInstance->mRefCount--;
    RLOG_DEBUG("RME-refcount-- %d\n", RMEimpl::mInstance->mRefCount);
    if (RMEimpl::mInstance->mRefCount <= 0) {
      RLOG_DEBUG("delete RMEimpl instance\n");
      delete RMEimpl::mInstance;
      RMEimpl::mInstance = NULL;
    }
  }
  RMEimpl::mpMutexAsync->MutexUnlock();

  if (RMEimpl::mInstance == NULL) {
    omxilosalservices::OmxILOsalMutex::MutexFree(RMEimpl::mpMutexAsync);
    RMEimpl::mpMutexAsync = NULL;
  }
}

// RMEimpl is a 'facade' for the various submodules of the RME

// internal shared instances inits
NmfDomains* SharedInst::mpNmfDomains = NULL;
ORPCompNetwork* SharedInst::mpORPCompNetw = NULL;
AGov* SharedInst::mpAGov = NULL;

void (*SharedInst::mpNotifyInterconnectBwMode)(OMX_BOOL) = NULL;

/// inits
int RMEimpl::Init() {
  // init internal shared instances
  SharedInst::mpNmfDomains = &mNmfDomains;
  SharedInst::mpORPCompNetw = &mORPCompNetw;
  SharedInst::mpAGov = &mAGov;
  SharedInst::mpNotifyInterconnectBwMode = NULL;

  // create all 'non-scratch' platform NMF domains
  return mNmfDomains.CreateAllNonScratchPlatformDomains();
}

/// RME API: Register
/// call mode: SYNCHRONOUS from RME client (OMXIL component) context
RM_STATUS_E RMEimpl::Register(OMX_IN const OMX_PTR pCompHdl,
    OMX_IN const RM_CBK_T* pCompCbks) {
  // create and add a resource proxy component object (ORPComp)
  // comp will be in a waiting list until first use:
  // comp init will be done only on first call to GetComponent(pCompHdl)
  // this mecanism is meant to isolate this call made in client thread context
  // from processing calls made in RME thread context
  return mORPCompNetw.RegisterComponent(pCompHdl, pCompCbks);
}

/// RME API: Unregister
/// call mode: SYNCHRONOUS from RME client (OMXIL component) context
RM_STATUS_E RMEimpl::Unregister(OMX_IN const OMX_PTR pCompHdl) {
  // flag component as unregistered
  // actual component destruction and associated cleanups will be done
  // either on next call to GetComponent(pCompHdl)
  // (if messages are pending) or on next call to idleT()
  // this mecanism is meant to isolate this call made in client thread context
  // from processing calls made in RME thread context
  return mORPCompNetw.UnregisterComponent(pCompHdl);
}

/// RME API: Notify_NewStatesOMXRES
/// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
void RMEimpl::Notify_NewStatesOMXRES(OMX_IN const OMX_PTR pCompHdl,
    OMX_IN OMX_STATETYPE eOMXILState,
    OMX_IN RM_RESOURCESTATE_E eResourceState,
    OMX_IN OMX_U32 nReserved) {
  MutexAsyncMsgLock();
  mCounterAsync++;

  bool compinitisdone;
  ORPComp* pcomp = mORPCompNetw.GetComponent(pCompHdl, &compinitisdone);
  if (pcomp == NULL) {
    if (compinitisdone == false) {
      RLOG_WARNING("NewStatesOMXRES: unknown pCompHdl:%p\n", pCompHdl);
    }
    if (nReserved) {}  // to avoid 'unused' warning
    MutexAsyncMsgUnlock();
    return;
  }

  bool initjustdone = false;
  if (compinitisdone == false) {
    // time to init component on first use
    RLOG_DEBUG("NewStatesOMXRES: comp init\n");
    if (mORPCompNetw.InitComponentOnFirstUse(pcomp) < 0) {
      // failure: pcomp deleted..
      MutexAsyncMsgUnlock();
      return;
    }
    initjustdone = true;
    // internal notification for priority 'change'
    // needed for group priority coherency and network sorting (per prio)
    mResManager.NewPolicyGP(pcomp, pcomp->PolGroupId(), pcomp->PolGroupPriorityClient(), true);
  }

  // check validity of dual OMXIL/Resource state
  DS_OR_E ds_or_tgt = StatesDuo::GetDualStateOmxRes(eOMXILState, eResourceState);
  if (ds_or_tgt == DS_NA) {
    RLOG_WARNING("NewStatesOMXRES comp[i:%d - r:%s]: invalid states (%s/%s)\n", pcomp->Index(), pcomp->Role(), StatesDuo::StringifyOmx(eOMXILState), StatesDuo::StringifyRes(eResourceState));
    // if going to loaded state remove all resources anyway
    if (eOMXILState == OMX_StateLoaded) {
      eResourceState = RM_RS_NONE;  // forced
      ds_or_tgt = DS_LOADED_NONE;
    } else {
      // notify error and discard notification: dont take into account the notified states..
      pcomp->CpNotifyError(RM_E_STATES);
      MutexAsyncMsgUnlock();
      return;
    }
  }

  // check transition validity and set transition id
  DS_TSI_E transitionid = DS_TSI_NA;
  if (pcomp->IsDsORtransitionValidGetTSId(ds_or_tgt, &transitionid) == false) {
    RLOG_WARNING("NewStatesOMXRES comp[i:%d - r:%s]: invalid transition (%s/%s)\n", pcomp->Index(), pcomp->Role(), StatesDuo::StringifyOmx(eOMXILState), StatesDuo::StringifyRes(eResourceState));
    // if going to loaded state remove all resources anyway
    if (ds_or_tgt == DS_LOADED_NONE) {
      transitionid = DS_TSI_UNLOAD_ALL_RESOURCES;
    } else {
      pcomp->CpNotifyError(RM_E_STATESTR);
      MutexAsyncMsgUnlock();
      return;
    }
  }

  // transition is valid: change component Omxil & Resources states
  pcomp->SetORstates(eOMXILState, eResourceState);

  // resource manager work
  mResManager.NewDualStatesTransition(pcomp, transitionid, initjustdone);

  // the updated context will be checked/processed on next call to idleT

  MutexAsyncMsgUnlock();
}

/// RME API: Notify_NewPolicyCfg
/// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
void RMEimpl::Notify_NewPolicyCfg(OMX_IN const OMX_PTR pCompHdl,
    OMX_IN OMX_U32 nGroupPriority, OMX_IN OMX_U32 nGroupID,
    OMX_IN OMX_U32 nQosBitmap) {
  MutexAsyncMsgLock();
  mCounterAsync++;

  bool compinitisdone;
  ORPComp* pcomp = mORPCompNetw.GetComponent(pCompHdl, &compinitisdone);
  if (pcomp == NULL) {
    if (compinitisdone == false) {
      RLOG_WARNING("NewPolicyCfg: unknown pCompHdl:%p\n", pCompHdl);
    }
    MutexAsyncMsgUnlock();
    return;
  }

  if (compinitisdone == false) {
    // time to init component on first use
    RLOG_DEBUG("NewPolicyCfg: comp init\n");
    if (mORPCompNetw.InitComponentOnFirstUse(pcomp) < 0) {
      // failure: pcomp deleted..
      MutexAsyncMsgUnlock();
      return;
    }
  }

  if (nQosBitmap) {}

  // resource manager work
  mResManager.NewPolicyGP(pcomp,
      static_cast<unsigned int>(nGroupID), static_cast<unsigned int>(nGroupPriority));

  // the updated context will be checked/processed on next call to idleT

  MutexAsyncMsgUnlock();
}

/// RME API: Notify_RMConfigApplied
/// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
void RMEimpl::Notify_RMConfigApplied(OMX_IN const OMX_PTR pCompHdl,
    OMX_IN RM_RC_E eType,
    OMX_IN OMX_S32 nData,
    OMX_IN RM_STATUS_E nErrorCode) {
  MutexAsyncMsgLock();
  mCounterAsync++;

  bool compinitisdone;
  ORPComp* pcomp = mORPCompNetw.GetComponent(pCompHdl, &compinitisdone);
  if (pcomp == NULL) {
    if (compinitisdone == false) {
      RLOG_WARNING("RMConfigApplied: unknown pCompHdl:%p\n\n", pCompHdl);
    }
    MutexAsyncMsgUnlock();
    return;
  }

  if (compinitisdone == false) {
    // time to init component on first use
    RLOG_DEBUG("RMConfigApplied: comp init\n");
    if (mORPCompNetw.InitComponentOnFirstUse(pcomp) < 0) {
      // failure: pcomp deleted..
      MutexAsyncMsgUnlock();
      return;
    }
    // internal notification for priority 'change'
    // needed for group priority coherency and network sorting (per prio)
    mResManager.NewPolicyGP(pcomp, pcomp->PolGroupId(), pcomp->PolGroupPriorityClient(), true);
  }

  switch (eType) {
    // resource manager work: ->check status and possibly data
    case RM_RC_NMF_DOMAINS:
      mResManager.NmfDomainIdsApplied(pcomp, nErrorCode, static_cast<int>(nData));
      break;
    case RM_RC_AUDIOLOWPOWER_ENTER:
      mResManager.AudioLowPowerEnterApplied(pcomp, nErrorCode, static_cast<int>(nData));
      break;
    case RM_RC_AUDIOLOWPOWER_EXIT:
      mResManager.AudioLowPowerExitApplied(pcomp, nErrorCode);
      break;
    default:
      RLOG_WARNING("RMConfigApplied comp[i:%d - r:%s]: invalid type:%d\n", pcomp->Index(), pcomp->Role(),
          static_cast<int>(eType));
      break;
  }

  // the updated context will be checked/processed on next call to idleT

  MutexAsyncMsgUnlock();
}

/// RME API: Notify_Tunnel
/// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
void RMEimpl::Notify_Tunnel(
    OMX_IN const OMX_PTR pCompHdlOut, OMX_IN OMX_U32 nPortIndexOut,
    OMX_IN const OMX_PTR pCompHdlIn, OMX_IN OMX_U32 nPortIndexIn) {
  MutexAsyncMsgLock();
  mCounterAsync++;

  RM_STATUS_E status = RM_E_KO;
  bool aliencfg = false;
  bool compinitisdone;

  /* translate OMXIL component's handle to ORPComp and
     translate OMXIL component's port indexes (uncontrolled ranges)
     to ORPComp's port indexes (controlled ranges + metadata) */
  ORPComp* pcompOut = mORPCompNetw.GetComponent(pCompHdlOut, &compinitisdone);
  unsigned int orpportout = 0;
  if (pcompOut) {
    orpportout = pcompOut->GetAddORPport(nPortIndexOut, false);

    if (compinitisdone == false) {
      // time to init component on first use
      RLOG_DEBUG("Tunnel: comp init\n");
      if (mORPCompNetw.InitComponentOnFirstUse(pcompOut) < 0) {
        // failure: pcomp deleted..
        MutexAsyncMsgUnlock();
        return;
      }
      // internal notification for priority 'change'
      // needed for group priority coherency and network sorting (per prio)
      mResManager.NewPolicyGP(pcompOut,
                              pcompOut->PolGroupId(),
                              pcompOut->PolGroupPriorityClient(),
                              true);
    }
  }

  ORPComp* pcompIn = mORPCompNetw.GetComponent(pCompHdlIn, &compinitisdone);
  unsigned int orpportin = 0;
  if (pcompIn) {
    orpportin =  pcompIn->GetAddORPport(nPortIndexIn, true);

    if (compinitisdone == false) {
      // time to init component on first use
      RLOG_DEBUG("Tunnel: comp init\n");
      if (mORPCompNetw.InitComponentOnFirstUse(pcompIn) < 0) {
        // failure: pcomp deleted..
        MutexAsyncMsgUnlock();
        return;
      }
      // internal notification for priority 'change'
      // needed for group priority coherency and network sorting (per prio)
      mResManager.NewPolicyGP(pcompIn,
                              pcompIn->PolGroupId(),
                              pcompIn->PolGroupPriorityClient(),
                              true);
    }
  }

  // Network manager work
  if (pcompOut && pcompIn) {
    status = mORPCompNetw.TunnelComponents(pcompOut, orpportout,
                                             pcompIn, orpportin);
  } else if (pcompOut && (pcompIn == NULL)) {
    if (pCompHdlIn == NULL) {
      status = mORPCompNetw.UntunnelCompOut(pcompOut, orpportout);
    } else {
      /* hybrid connection between known component and 'alien'/unknown component;
         this 'alien' component may never register to the RME or may register later...
         =>not supported */
      RLOG_DEBUG("X tunneling for input comp[i:%d - r:%s]\n", pcompOut->Index(), pcompOut->Role());
      aliencfg = true;
    }
  } else if ((pcompOut == NULL) && pcompIn) {
    if (pCompHdlOut == NULL) {
      status = mORPCompNetw.UntunnelCompIn(pcompIn, orpportin);
    } else {
      /* same comment as above */
      RLOG_DEBUG("X tunneling for output comp[i:%d - r:%s]\n", pcompIn->Index(), pcompIn->Role());
      aliencfg = true;
    }
  } else {  // (pcompOut == NULL) && (pCompHdlIn == NULL)
    if (pCompHdlIn || pCompHdlOut) {
      /* pure 'alien' tunneling or untunneling: meaningless -> not supported */
      RLOG_DEBUG("pure X tunneling\n");
      aliencfg = true;
    } else {
      // nothing to do!
      status = RM_E_NONE;
      MutexAsyncMsgUnlock();
      return;
    }
  }

  if (status != RM_E_NONE) {
    if (aliencfg == false) {
      // error notification sent to components, when known
      if (pcompOut)
        pcompOut->CpNotifyError(status);
      if (pcompIn)
        pcompIn->CpNotifyError(status);
    }
  }

  // the updated context will be checked/processed on next call to idleT

  MutexAsyncMsgUnlock();
}

/// RME API: Get_DefaultDDRDomain
/// call mode: SYNCHRONOUS from RME client (OMXIL component) context
OMX_U32 RMEimpl::Get_DefaultDDRDomain(OMX_IN RM_NMFD_E nDomainType) {
  // OBSOLETE Code: will be removed;
  // no mutex lock needed since domain is built at RME init time
  switch (nDomainType) {
#ifndef NODEFAULTDOMAIN
    case RM_NMFD_NA:
      // TODO(pht) suppress this: not supported by NMF CM for ARM+SDRAM allocations anyway!
      return mNmfDomains.GetNonScratchPlDomId(PlatformDomains::PND_PROCARM_DDR_GEN);
    case RM_NMFD_PROCSVA:
      return mNmfDomains.GetNonScratchPlDomId(PlatformDomains::PND_PROCSVA_DDR_GEN);
    case RM_NMFD_PROCSIA:
      return mNmfDomains.GetNonScratchPlDomId(PlatformDomains::PND_PROCSIA_DDR_GEN);
#endif
    default:
      return 0;  // return not-set/not-valid domain
  }
}

/// RME API: InitSupport_PRM
/// call mode: SYNCHRONOUS from RME thread MESSAGE dispatcher
void RMEimpl::InitSupport_PRM(
    OMX_IN void (*NotifyInterconnectBwMode)(OMX_IN OMX_BOOL bUse1080pSettings)) {
  MutexAsyncMsgLock();
  mCounterAsync++;
  RLOG_DEBUG("InitSupport_PRM: %d\n", NotifyInterconnectBwMode ? 1 : 0);
  SharedInst::mpNotifyInterconnectBwMode = NotifyInterconnectBwMode;
  MutexAsyncMsgUnlock();
}

/// RME API: idleT
/// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
int RMEimpl::idleT() {
  MutexAsyncMsgLock();

#if defined(SW_VARIANT_ANDROID)
    GET_PROPERTY(TRACE_ENABLED_PROPERTY_NAME, value, "0");
    Debug::mTraceEnabled = atoi(value);
#endif

  // check if called on timeout
  // or after processing of all asynchronous messages in the message queue
  bool calledontimeout;
  if (mIdleCounterAsync == mCounterAsync) {
    // called on timeout
    calledontimeout = true;
  } else {
    calledontimeout = false;
  }

  // update idle counter
  mIdleCounterAsync = mCounterAsync;

  // resource manager works during 'idle' time
  int timeoutmsec = static_cast<int>(mResManager.WorkTime(calledontimeout));

  MutexAsyncMsgUnlock();

  return timeoutmsec;
}

}  // namespace

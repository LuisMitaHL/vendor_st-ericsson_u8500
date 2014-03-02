/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ORPComp.cpp
 * OMX IL component class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "ORPComp.h"
#include "NmfDomains.h"
#include "PlatformLimits.h"
#include "PfRoles.h"
#include "SharedInst.h"

namespace rme {

unsigned int ORPComp::msGlobalResourceDate = 1;
unsigned int ORPComp::msCompIndex = 0;

/////////////// network related: connection lines

/// sets network halfline id
void ORPComp::SetNwHalflineId(int id) {
  mNwHalflineId = id;
  if (id < 0) {
    // is a disconnection: make sure ports are also disconnected
    for (unsigned int j = 0; j < mCountPorts; j++)
      mPorts[j].peernwhlid = id;
  }
}

/// gets or adds ORP port entry associated to OMXIL port index
unsigned int ORPComp::GetAddORPport(unsigned int omxportindex, bool wayin) {
  unsigned int i;

  // check among valid ports (i < mCountPorts) if this port is known
  for (i = 0; i < mCountPorts; i++) {
    if ((mPorts[i].omxportindex == omxportindex) &&
        (mPorts[i].wayin == wayin)) {
      return i;  // got an existing port
    }
  }

  // else try to add a port entry to reference this port
  if (mCountPorts < msMAXCountPorts) {
    i = mCountPorts++;
    mPorts[i].omxportindex = omxportindex;
    mPorts[i].controlbmp = 0;
    mPorts[i].wayin = wayin;
    mPorts[i].peernwhlid = -1;
    return i;  // used a valid port
  } else {
    RLOG_ERROR("port overflow for comp[i:%d - r:%s]\n", Index(), Role());
    return msMAXCountPorts;  // overflow no more free ports
  }
}

/// returns ORP port index associated to OMXIL port index
unsigned int ORPComp::OrpPortIndex(unsigned int omxportindex) const {
  // check among valid ports (i < mCountPorts) if this port is known
  for (unsigned int i = 0; i < mCountPorts; i++) {
    if (mPorts[i].omxportindex == omxportindex)
      return i;
  }
  // not found: return limit
  return msMAXCountPorts;
}

/// returns OMXIL port index associated to ORP port index
int ORPComp::OmxPortIndex(unsigned int orpportindex) const {
  if (orpportindex < msMAXCountPorts)
    return mPorts[orpportindex].omxportindex;
  else
    return -1;
}

/// checks for input port associated to ORP port index
bool ORPComp::IsInputPort(unsigned int orpportindex) const {
  if ((orpportindex < msMAXCountPorts) && (mPorts[orpportindex].wayin)) {
    return true;
  } else {
    return false;
  }
}

/// checks for output port associated to ORP port index
bool ORPComp::IsOutputPort(unsigned int orpportindex) const {
  if ((orpportindex < msMAXCountPorts) && (mPorts[orpportindex].wayin == false)) {
    return true;
  } else {
    return false;
  }
}

/// returns network halfline id of peer for port associated to ORP port index
int ORPComp::PortPeerNwHlId(unsigned int orpportindex) const {
  if (orpportindex < msMAXCountPorts)
    return mPorts[orpportindex].peernwhlid;
  else
    return -1;
}

/// sets network halfline id of peer for port associated to ORP port index
void ORPComp::SetPortPeerNwHlId(unsigned int orpportindex, int value) {
  if (orpportindex < msMAXCountPorts)
    mPorts[orpportindex].peernwhlid = value;
}

/// counts number of input connections
unsigned int ORPComp::CountConnectionsOnPortIn() const {
  unsigned int count = 0;
  for (unsigned int j = 0; j < mCountPorts; j++) {
    if ((mPorts[j].wayin) && (mPorts[j].peernwhlid >= 0)) {
      count++;
    }
  }
  return count;
}

/// counts number of output connections
unsigned int ORPComp::CountConnectionsOnPortOut() const {
  unsigned int count = 0;
  for (unsigned int j = 0; j < mCountPorts; j++) {
    if ((mPorts[j].wayin == false) && (mPorts[j].peernwhlid >= 0)) {
      count++;
    }
  }
  return count;
}

/// counts number of connections
unsigned int ORPComp::CountConnectionsOnPort() const {
  unsigned int count = 0;
  for (unsigned int j = 0; j < mCountPorts; j++) {
    if (mPorts[j].peernwhlid >= 0) {
      count++;
    }
  }
  return count;
}

/// disconnects input ports that have a peer with given network halfline id
void ORPComp::DisconnectInputPortsWithPeerNwHlId(unsigned int id) {
  for (unsigned int j = 0; j < mCountPorts; j++) {
    if ((mPorts[j].wayin) &&
        (mPorts[j].peernwhlid == static_cast<int>(id)))
      mPorts[j].peernwhlid = -1;
  }
}

/// disconnects output ports that have a peer with given network halfline id
void ORPComp::DisconnectOutputPortsWithPeerNwHlId(unsigned int id) {
  for (unsigned int j = 0; j < mCountPorts; j++) {
    if ((mPorts[j].wayin == false) &&
        (mPorts[j].peernwhlid == static_cast<int>(id)))
      mPorts[j].peernwhlid = -1;
  }
}

/// replaces all occurences of old peer network halfline id by provided id
void ORPComp::ReplacePortsId(unsigned int old_id, unsigned int upd_id,
    bool replace_in, bool replace_out) {
  for (unsigned int j = 0; j < mCountPorts; j++) {
    if (mPorts[j].wayin) {
      if ((replace_in) && (mPorts[j].peernwhlid == static_cast<int>(old_id)))
        mPorts[j].peernwhlid = static_cast<int>(upd_id);
    } else {
      if ((replace_out) && (mPorts[j].peernwhlid == static_cast<int>(old_id)))
        mPorts[j].peernwhlid = static_cast<int>(upd_id);
    }
  }
}

/// sorting comparaison method: compares per priority and date
int ORPComp::ComparePerPriorityAndResourceDate(const ORPComp* pcompA,
                                               const ORPComp* pcompB) {
  if ((pcompA == NULL) || (pcompB == NULL))
    return 0;

  // if 'pcompA < (resp =, >) pcompB' then result is < (resp =, >) 0
  // for priority comparaison: highest priority == lowest priority value
  uint64_t priocompA = pcompA->PolGroupPriorityClient() + pcompA->PolGroupPrioritySystem();
  uint64_t priocompB = pcompB->PolGroupPriorityClient() + pcompB->PolGroupPrioritySystem();
  if (priocompA < priocompB) {
    return 1;
  } else if (priocompA == priocompB) {
    /* if components have same prio, check resource date:
     * the one with more recent resource date gets highest priority */
    if (pcompA->DateResourceGrant() > pcompB->DateResourceGrant())
      return 1;
    else if (pcompA->DateResourceGrant() == pcompB->DateResourceGrant())
      return 0;
    else
      return -1;
  } else {
    return -1;
  }
}

/////////////// states management

/// checks if a transition to given dual OR state is valid or not
/// and sets transition id associated
bool ORPComp::IsDsORtransitionValidGetTSId(DS_OR_E to_ds_or,
                                           DS_TSI_E *ptransitionid) const {
  bool check_reqtosatisf = false;
  bool isvalid = mStatesORp.IsValidTransitionDsOR(to_ds_or,
                                                  ptransitionid,
                                                  &check_reqtosatisf);
  if (isvalid && check_reqtosatisf) {
    /* check if comp was granted resources / authorized
       to move from REQUIRED to SATISFIED */
    if (CFlag(CF_INREQUESTPHASE)) {
      // resources not granted yet => invalidate the transition request to satisfied
      RLOG_WARNING("rejecting invalid transition to satisfied for comp[i:%d - r:%s] %s -- still in request phase\n", Index(), Role(), DualStateORstr());
      isvalid = false;
    }
  }
  return isvalid;
}

/// checks for timeout on OMXIL component reaction to preemption command
bool ORPComp::IsTimedOutReactResReleaseCmd() {
  if (mDateResReleaseCmd == 0)
    return false;
  if ((CFlag(CF_PENDING_RESPREEMPTION_FULL)) ||
      (CFlag(CF_PENDING_RESPREEMPTION_PARTIAL))) {
    OMX_U64 currenttime = omxilosalservices::OmxILOsalTimer::GetSystemTime();
    OMX_U64 limittime = mDateResReleaseCmd +
                        (PlatformLimits::PREEMPTION_ACKNOWLEDGE_TIMEOUTMS * 1000);
    if (currenttime >= limittime)
      return true;
  }
  return false;
}

/// reinitializes partially when unloading resources
void ORPComp::ReInitializeUnload() {
  // states
  mStatesORp.UpdateDsOR(OMX_StateLoaded, RM_RS_NONE);
  mDateResourceGrant =  static_cast<unsigned int>(-1);
  mDateResourceGrantBackup = static_cast<unsigned int>(-1);
  mDateResReleaseCmd = 0;

  // control flags
  MutexCpLock();
  // clear all control flags except UNREGISTERED
  unsigned int keepflag = mControlsBmp & (CF_UNREGISTERED);
  mControlsBmp = 0;
  mControlsBmp |= keepflag;
  MutexCpUnlock();

  for (unsigned int i = 0; i < msMAXCountPorts; i++)
        mPorts[i].controlbmp = 0;

  // pnds and scratch domains
  ClearPndsAndDomains();

  // clear resources levels estimation data
  mResEstimLevels.InitLevels(NULL, false);
  mUccBitmap = 0;
  mResEstimLevelsBackup.InitLevels(NULL, false);
  mUccBitmapBackup = 0;

  // no need to reset capabilities
}

///////////////////// activity & operational states

/// set a control flag for a port
void ORPComp::SetCFlagPort(CF_PUB_E flag, unsigned int orpportindex) {
  if (orpportindex < msMAXCountPorts) {
    mPorts[orpportindex].controlbmp |= flag;
  }
}

/// clear a control flag for a port
void ORPComp::ClearCFlagPort(CF_PUB_E flag, unsigned int orpportindex) {
  if (orpportindex < msMAXCountPorts) {
    mPorts[orpportindex].controlbmp &= ~flag;
  }
}

/// checks a control flag for a port
unsigned int ORPComp::CFlagPort(CF_PUB_E flag, unsigned int orpportindex) {
  if (orpportindex < msMAXCountPorts) {
    return mPorts[orpportindex].controlbmp & flag;
  } else {
    return 0;
  }
}

/////////////// platform domains

/// returns domain id for a domain type
unsigned int ORPComp::DomIdForDtype(PlatformDomains::DHM_E dtype) const {
  switch (dtype) {
    case PlatformDomains::DHM_PROCSVA: return mDomains.IdProcSva;
    case PlatformDomains::DHM_PROCSIA: return mDomains.IdProcSia;
    case PlatformDomains::DHM_HWPIPESVA: return mDomains.IdHwPipeSva;
    case PlatformDomains::DHM_HWPIPESIA: return mDomains.IdHwPipeSia;
    case PlatformDomains::DHM_HWXP70SIA: return mDomains.IdHwXp70Sia;
    case PlatformDomains::DHM_NA:
    default:
     RLOG_WARNING("dtype:%d not supported\n", dtype);
     return 0;
  }
}

/// returns PND for a domain type
PlatformDomains::PNDS_E ORPComp::PndForDtype(PlatformDomains::DHM_E dtype) const {
  switch (dtype) {
    case PlatformDomains::DHM_PROCSVA: return mDomains.PndProcSva;
    case PlatformDomains::DHM_PROCSIA: return mDomains.PndProcSia;
    case PlatformDomains::DHM_HWPIPESVA: return mDomains.PndHwPipeSva;
    case PlatformDomains::DHM_HWPIPESIA: return mDomains.PndHwPipeSia;
    case PlatformDomains::DHM_HWXP70SIA: return mDomains.PndHwXp70Sia;
    case PlatformDomains::DHM_NA:
    default:
     RLOG_WARNING("dtype:%d not supported\n", dtype);
     return PlatformDomains::PND_NA;
  }
}

/// sets PND for a domain type
/// and possibly creates scratch domain associated (depends on type)
int ORPComp::SetPndForDtype(PlatformDomains::PNDS_E pnd,
                            PlatformDomains::DHM_E dtype) {
  PlatformDomains::PNDS_E *ppnd = NULL;
  unsigned int *pdomid = NULL;

  if ((PfDomManager::IsPndValid(pnd) == false) && (pnd != PlatformDomains::PND_NA)) {
      RLOG_ERROR("pnd:%d invalid\n", pnd);
      return -1;
  }

  switch (dtype) {
    case PlatformDomains::DHM_PROCSVA:
      ppnd = &mDomains.PndProcSva;
      pdomid = &mDomains.IdProcSva;
      break;
    case PlatformDomains::DHM_PROCSIA:
      ppnd = &mDomains.PndProcSia;
      pdomid = &mDomains.IdProcSia;
      break;
    case PlatformDomains::DHM_HWPIPESVA:
      ppnd = &mDomains.PndHwPipeSva;
      pdomid = &mDomains.IdHwPipeSva;
      break;
    case PlatformDomains::DHM_HWPIPESIA:
      ppnd = &mDomains.PndHwPipeSia;
      pdomid = &mDomains.IdHwPipeSia;
      break;
    case PlatformDomains::DHM_HWXP70SIA:
      ppnd = &mDomains.PndHwXp70Sia;
      pdomid = &mDomains.IdHwXp70Sia;
      break;
    case PlatformDomains::DHM_NA:
    default:
      break;
  }

  if ((ppnd == NULL) || (pdomid == NULL)) {
    RLOG_ERROR("dtype:%d not supported\n", dtype);
    return -1;
  }

  NmfDomains* nmfdomainsM = SharedInst::GetNmfDomainsInst();
  // prev
  PlatformDomains::PNDS_E pndprev = *ppnd;
  unsigned int domidprev = *pdomid;
  // new -- init
  *ppnd = pnd;
  *pdomid = 0;

  // check if need to destroy previous domain
  if (domidprev) {
    // previous domain already created (has valid id)
    bool destroyprev = false;
    if (pnd == PlatformDomains::PND_NA) {
      destroyprev = true;
    } else if (pndprev != pnd) {
#ifndef NOCREATEDOMAINFORCLIENT
      // destroy previous domain, except if is parent or scratch
      if (PfDomManager::GetScratchId(pndprev) >= 0) {
        destroyprev = true;
      }
#else
      // destroy previous domain if is scratch
      if (PfDomManager::GetScratchId(pndprev) > 0) {
        destroyprev = true;
      }
#endif
    } else {
      // no need to destroy nor to create: same pnd
      *pdomid = domidprev;
      return 0;
    }

    if (destroyprev) {
      nmfdomainsM->DestroyDomain(pndprev, domidprev);
    }
  }

  if (pnd == PlatformDomains::PND_NA) {
    // nothing more to do
    return 0;
  }

  // different creation scheme based on scratch id
  // < 0: parent domain, 0: non scratch domain, > 0, scratch domain
  if (PfDomManager::GetScratchId(pnd) > 0) {
    // create scratch domain (no need for client in this case)
    if (nmfdomainsM->CreateDomain(pnd, pdomid, false, 0) < 0) {
      RLOG_ERROR("scratch CreateDomain for pnd:%d\n", pnd);
      return -1;
    }
  } else if (PfDomManager::GetScratchId(pnd) == 0) {
    // non parent non scratch
#ifndef NOCREATEDOMAINFORCLIENT
    if (nmfdomainsM->CreateDomain(pnd, pdomid, true, mPolicyGroup.pid) < 0) {
      RLOG_ERROR("client CreateDomain for pnd:%d\n", pnd);
      return -1;
    }
#else
    *pdomid = static_cast<unsigned int>(nmfdomainsM->GetNonScratchPlDomId(pnd));
#endif
  } else {
    // non scratch: scratch parent domain
    // => global way; RME owning the domains
    *pdomid = static_cast<unsigned int>(nmfdomainsM->GetNonScratchPlDomId(pnd));
  }

  return 0;
}

/// clears all PNDs, and destroy domains (if any)
void ORPComp::ClearPndsAndDomains(bool scratchdomainsonly) {
  NmfDomains* nmfdomainsM = SharedInst::GetNmfDomainsInst();
  int scratchid;
  bool upd = true;

  if (mDomains.PndProcSva != PlatformDomains::PND_NA) {
    scratchid = PfDomManager::GetScratchId(mDomains.PndProcSva);
    if (scratchid > 0) {
      // destroy scratch domain
      nmfdomainsM->DestroyDomain(mDomains.PndProcSva, mDomains.IdProcSva);
      mDomains.PndProcSva = PlatformDomains::PND_NA;
      mDomains.IdProcSva = 0;
      upd = false;
    } else if (scratchdomainsonly == false) {
      if (scratchid == 0) {
#ifndef NOCREATEDOMAINFORCLIENT
        // destroy non parent non scratch domains
        nmfdomainsM->DestroyDomain(mDomains.PndProcSva, mDomains.IdProcSva);
#endif
      }
      // reset in both non scratch cases (parent or not)
      mDomains.PndProcSva = PlatformDomains::PND_NA;
      mDomains.IdProcSva = 0;
    }
  }

  if (mDomains.PndProcSia != PlatformDomains::PND_NA) {
    scratchid = PfDomManager::GetScratchId(mDomains.PndProcSia);
    if (scratchid > 0) {
      // destroy scratch domain
      nmfdomainsM->DestroyDomain(mDomains.PndProcSia, mDomains.IdProcSia);
      mDomains.PndProcSia = PlatformDomains::PND_NA;
      mDomains.IdProcSia = 0;
      upd = false;
    } else if (scratchdomainsonly == false) {
      if (scratchid == 0) {
#ifndef NOCREATEDOMAINFORCLIENT
        // destroy non parent non scratch domains
        nmfdomainsM->DestroyDomain(mDomains.PndProcSia, mDomains.IdProcSia);
#endif
      }
      // reset in both non scratch cases (parent or not)
      mDomains.PndProcSia = PlatformDomains::PND_NA;
      mDomains.IdProcSia = 0;
    }
  }

  if (mDomains.PndHwPipeSva != PlatformDomains::PND_NA) {
    scratchid = PfDomManager::GetScratchId(mDomains.PndHwPipeSva);
    if (scratchid > 0) {
      // destroy scratch domain
      nmfdomainsM->DestroyDomain(mDomains.PndHwPipeSva, mDomains.IdHwPipeSva);
      mDomains.PndHwPipeSva = PlatformDomains::PND_NA;
      mDomains.IdHwPipeSva = 0;
      upd = false;
    } else if (scratchdomainsonly == false) {
      if (scratchid == 0) {
#ifndef NOCREATEDOMAINFORCLIENT
        // destroy non parent non scratch domains
        nmfdomainsM->DestroyDomain(mDomains.PndHwPipeSva, mDomains.IdHwPipeSva);
#endif
      }
      // reset in both non scratch cases (parent or not)
      mDomains.PndHwPipeSva = PlatformDomains::PND_NA;
      mDomains.IdHwPipeSva = 0;
    }
  }

  if (mDomains.PndHwPipeSia != PlatformDomains::PND_NA) {
    scratchid = PfDomManager::GetScratchId(mDomains.PndHwPipeSia);
    if (scratchid > 0) {
      // destroy scratch domain
      nmfdomainsM->DestroyDomain(mDomains.PndHwPipeSia, mDomains.IdHwPipeSia);
      mDomains.PndHwPipeSia = PlatformDomains::PND_NA;
      mDomains.IdHwPipeSia = 0;
      upd = false;
    } else if (scratchdomainsonly == false) {
      if (scratchid == 0) {
#ifndef NOCREATEDOMAINFORCLIENT
        // destroy non parent non scratch domains
        nmfdomainsM->DestroyDomain(mDomains.PndHwPipeSia, mDomains.IdHwPipeSia);
#endif
      }
      // reset in both non scratch cases (parent or not)
      mDomains.PndHwPipeSia = PlatformDomains::PND_NA;
      mDomains.IdHwPipeSia = 0;
    }
  }

  if (mDomains.PndHwXp70Sia != PlatformDomains::PND_NA) {
    scratchid = PfDomManager::GetScratchId(mDomains.PndHwXp70Sia);
    if (scratchid > 0) {
      // destroy scratch domain
      nmfdomainsM->DestroyDomain(mDomains.PndHwXp70Sia, mDomains.IdHwXp70Sia);
      mDomains.PndHwXp70Sia = PlatformDomains::PND_NA;
      mDomains.IdHwXp70Sia = 0;
      upd = false;
    } else if (scratchdomainsonly == false) {
      if (scratchid == 0) {
#ifndef NOCREATEDOMAINFORCLIENT
        // destroy non parent non scratch domains
        nmfdomainsM->DestroyDomain(mDomains.PndHwXp70Sia, mDomains.IdHwXp70Sia);
#endif
      }
      // reset in both non scratch cases (parent or not)
      mDomains.PndHwXp70Sia = PlatformDomains::PND_NA;
      mDomains.IdHwXp70Sia = 0;
    }
  }

  if (upd == false) {
    // specific flag reset in that case
    ClearCFlag(CF_ACTIVE_RMCFG_NMFD);
  }
}

/////////////// wrapping calls from RME to OMXIL component

/// fills capabilities structure used for interaction with policy server
void ORPComp::FillCapabilities(RM_CAPABILITIES_T *pcapabilities) const {
  if (pcapabilities == NULL)
    return;
  memcpy(&pcapabilities->sLoadTime.cRole[0], &mOmxilRole[0], sizeof(mOmxilRole));
  memcpy(&pcapabilities->sLoadTime.cName[0], &mOmxilName[0], sizeof(mOmxilName));
  pcapabilities->sLoadTime.eCapBitmap = mCapBitmap;
  pcapabilities->sPolicy.nGroupID = mPolicyGroup.gid;
  pcapabilities->sPolicy.nGroupPriority = mPolicyGroup.priorityclient;
  pcapabilities->sPolicy.bCanSuspend = mPolicyGroup.can_suspend ? OMX_TRUE : OMX_FALSE;
  pcapabilities->sIds.nProcessId = mPolicyGroup.pid;
  pcapabilities->sIds.nThreadId = mPolicyGroup.tid;
}

/// sets callbacks to OMXIL component and provides associated mutex
int ORPComp::SetCallbacks(const RM_CBK_T* pCompCbks) {
  if (pCompCbks)
    memcpy(&mCompCallbacks, pCompCbks, sizeof(mCompCallbacks));
  else
    memset(&mCompCallbacks, 0, sizeof(mCompCallbacks));

  // callbacks sanity check: all compulsory
  if (mCompCallbacks.CP_Get_Capabilities == NULL)
    return -1;
  if (mCompCallbacks.CP_Get_ResourcesEstimation == NULL)
    return -1;
  if (mCompCallbacks.CP_Cmd_SetRMConfig == NULL)
    return -1;
  if (mCompCallbacks.CP_Cmd_ReleaseResource == NULL)
    return -1;
  if (mCompCallbacks.CP_Notify_ResReservationProcessed == NULL)
    return -1;
  if (mCompCallbacks.CP_Notify_Error == NULL)
    return -1;

  return 0;
}

/// gets capabilities from the OMXIL component
RM_STATUS_E ORPComp::CpGetCapabilities() {
  if (mCompCallbacks.CP_Get_Capabilities == NULL)
    return RM_E_INVALIDPTR;

  /* check conditions for locking static/load-time part of capabilities */
  OMX_STATETYPE somxil = mStatesORp.OmxState();
  bool locked =
    ((somxil == OMX_StateLoaded) || (somxil == OMX_StateWaitForResources)) ?  false : true;

  RM_CAPABILITIES_T l_capabilities;
  RM_STATUS_E status = RM_E_KO;

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpGetCapabilities comp[i:%d - r:%s] locked:%u\n", Index(), Role(), static_cast<unsigned int>(locked));

    status = (mCompCallbacks.CP_Get_Capabilities)(
        mOmxilCompHdl,
        &l_capabilities);
    MutexCpUnlock();
  } else {
    MutexCpUnlock();
    return RM_E_INVALIDPTR;
  }

  if (status == RM_E_NONE) {
    // update 'static' part of capabilities only if lock not set
    if (locked == false) {
      memcpy(&mOmxilRole[0],
          reinterpret_cast<char*>(&l_capabilities.sLoadTime.cRole[0]),
          sizeof(mOmxilRole));
      memcpy(&mOmxilName[0],
          reinterpret_cast<char*>(&l_capabilities.sLoadTime.cName[0]),
          sizeof(mOmxilName));
      mCapBitmap = static_cast<unsigned int>(l_capabilities.sLoadTime.eCapBitmap);

      mPolicyGroup.can_suspend = l_capabilities.sPolicy.bCanSuspend ? true : false;
      mPolicyGroup.pid = static_cast<uint64_t>(l_capabilities.sIds.nProcessId);
      mPolicyGroup.tid = static_cast<uint64_t>(l_capabilities.sIds.nThreadId);
    }

    mPolicyGroup.priorityclient = static_cast<unsigned int>(l_capabilities.sPolicy.nGroupPriority);
    mPolicyGroup.gid = static_cast<unsigned int>(l_capabilities.sPolicy.nGroupID);

    SetCFlag(CF_CAPABILITIES_VALID);
  }

  return status;
}

/// gets resources estimation from the OMXIL component
RM_STATUS_E ORPComp::CpGetResourcesEstimation() {
  if (mCompCallbacks.CP_Get_ResourcesEstimation== NULL)
    return RM_E_INVALIDPTR;

  RM_STATUS_E status = RM_E_KO;
  RM_EMDATA_T emdata;
  memset(&emdata, 0, sizeof(RM_EMDATA_T));

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpGetResourcesEstimation comp[i:%d - r:%s]\n", Index(), Role());

    status = (mCompCallbacks.CP_Get_ResourcesEstimation)(mOmxilCompHdl, &emdata);
    MutexCpUnlock();
  } else {
    MutexCpUnlock();
    return RM_E_INVALIDPTR;
  }

  // if needed could handle specific direct RM-DB for some components here..

  if (status == RM_E_NONE) {
    // specific 'fast' identifications for ESRAM buffers:
    // => transform esram buffer sizes into 'fast' identifications
    // (if not already provided in that format)
    // + perform platform checks
    if (PfDomManager::SetEsramBufferFastId(
          reinterpret_cast<RM_ESRAMBUFID_E*>(&emdata.sEsramBufs.nSvaHwPipe),
          PlatformDomains::DHM_HWPIPESVA) < 0) {
      // invalid fast id / size detected
      return RM_E_EMDATA;
    }
    if (PfDomManager::SetEsramBufferFastId(
          reinterpret_cast<RM_ESRAMBUFID_E*>(&emdata.sEsramBufs.nSiaHwPipe),
          PlatformDomains::DHM_HWPIPESIA) < 0) {
      // invalid fast id / size detected
      return RM_E_EMDATA;
    }
    if (PfDomManager::SetEsramBufferFastId(
          reinterpret_cast<RM_ESRAMBUFID_E*>(&emdata.sEsramBufs.nSiaHwXP70),
          PlatformDomains::DHM_HWXP70SIA) < 0) {
      // invalid fast id / size detected
      return RM_E_EMDATA;
    }

    // backup (used in dynamic requests)
    memcpy(&mResEstimLevelsBackup, &mResEstimLevels, sizeof(mResEstimLevelsBackup));
    mUccBitmapBackup = mUccBitmap;

    // resources levels in internal format
    // need to distinguish audio vs visual on SIA
    bool siaforvisual = true;
    if (PfRoles::IsAudioRole(Role()) == true) {
      siaforvisual = false;
    }
    mResEstimLevels.InitLevels(&emdata, siaforvisual);

    // force capabilities bitmap for SxA usage
    // only if actual resources estimations report such usage
    // (ie dont clear flags in the other case)
    if (mResEstimLevels.IsSvaBased()) {
      RLOG_DEBUG("(SVA based)\n");
      mCapBitmap |= RM_CAP_DISTUSESVA;
    } else if ((emdata.sTcmSva.nSizeX) || (emdata.sTcmSva.nSizeY)) {
      // TCM support removed and component wrongly reported TCM usage but no other usage like MCPS
      RLOG_DEBUG("OBSOLETE SVA TCM resources usage reported and no other SVA resources provided\n");
      // *temporary* hack for backward compatibility
      mCapBitmap |= RM_CAP_DISTUSESVA;
    }

    if (mResEstimLevels.IsSiaBased()) {
      RLOG_DEBUG("(SIA based)\n");
      mCapBitmap |= RM_CAP_DISTUSESIA;
    } else if ((emdata.sTcmSia.nSizeX) || (emdata.sTcmSia.nSizeY)) {
      // TCM support removed and component wrongly reported TCM usage but no other usage like MCPS
      RLOG_DEBUG("OBSOLETE SIA TCM resources usage reported and no other SIA resources provided\n");
      // *temporary* hack for backward compatibility
      mCapBitmap |= RM_CAP_DISTUSESIA;
    }

    // specific: usecase context bitmap update
    mUccBitmap = emdata.eUccBitmap;

    // done: resources estimation data valid
    SetCFlag(CF_RESESTIM_VALID);
  }

  return status;
}

/// commands set NMF domain ids configuration to the OMXIL component
void ORPComp::CpCmdSetRmConfigNmfDomainIds() {
  if (mCompCallbacks.CP_Cmd_SetRMConfig == NULL)
    return;

  RM_SYSCTL_T RMcfg;
  RMcfg.eType = RM_RC_NMF_DOMAINS;
  memset(&RMcfg.ctl.sNMF, 0, sizeof(RMcfg.ctl.sNMF));
  RMcfg.ctl.sNMF.nPortIndex = -1;

  RMcfg.ctl.sNMF.nDidProcSVA = static_cast<OMX_U32>(mDomains.IdProcSva);
  RMcfg.ctl.sNMF.nDidProcSIA = static_cast<OMX_U32>(mDomains.IdProcSia);

  RMcfg.ctl.sNMF.nDidHWPipeSVA = static_cast<OMX_U32>(mDomains.IdHwPipeSva);
  RMcfg.ctl.sNMF.nDidHWPipeSIA = static_cast<OMX_U32>(mDomains.IdHwPipeSia);

  RMcfg.ctl.sNMF.nDidHWXP70SIA = static_cast<OMX_U32>(mDomains.IdHwXp70Sia);

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpCmdSetRmConfigNmfDomainIds comp[i:%d - r:%s]\n", Index(), Role());
    RLOG_DEBUG("--- sva:%d/sia:%d\n", mDomains.IdProcSva, mDomains.IdProcSia);
    RLOG_DEBUG("--- hwsva:%d/hwsia:%d/hwxp70:%d\n", mDomains.IdHwPipeSva, mDomains.IdHwPipeSia, mDomains.IdHwXp70Sia);

    // always send, even if domains are all 0

    // intercept
    SetCFlag(CF_PENDING_RMCFG_NMFD);

    (mCompCallbacks.CP_Cmd_SetRMConfig)(mOmxilCompHdl, &RMcfg);
  }
  MutexCpUnlock();
}

/// commands set audio lowpower enter configuration to the OMXIL component
void ORPComp::CpCmdSetRmConfigALPenter() {
  if (mCompCallbacks.CP_Cmd_SetRMConfig == NULL)
    return;

  // check that component supports ALP signals
  // (shall have been controlled before anyway)
  if (((mCapBitmap & RM_CAP_LOWPOWERCTRLSIG) == 0) ||
      (PfRoles::IsAlpRoleSink(Role()) == 0)) {
    // shall not get there
    RLOG_WARNING("ALPenter not supported for comp[i:%d - r:%s]\n", Index(), Role());
    return;
  }

  NmfDomains* nmfdomainsM = SharedInst::GetNmfDomainsInst();
  RM_SYSCTL_T RMcfg;
  RMcfg.eType = RM_RC_AUDIOLOWPOWER_ENTER;

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RMcfg.ctl.sLowPowerEnter.nDidAudLP = static_cast<OMX_U32>(nmfdomainsM->GetNonScratchPlDomId(PlatformDomains::PND_PROCSIA_DDRESRAM_AUDIOLOWPOWER));
    RLOG_DEBUG("CpCmdSetRmConfigALPenter comp[i:%d - r:%s] LPdomid=%d\n", Index(), Role(), static_cast<unsigned int>(RMcfg.ctl.sLowPowerEnter.nDidAudLP));

    // intercept
    SetCFlag(CF_PENDING_RMCFG_ALP_ENTER);

    (mCompCallbacks.CP_Cmd_SetRMConfig)(mOmxilCompHdl, &RMcfg);
  }
  MutexCpUnlock();
}

/// commands set audio lowpower exit configuration to the OMXIL component
void ORPComp::CpCmdSetRmConfigALPMexit() {
  if (mCompCallbacks.CP_Cmd_SetRMConfig == NULL)
    return;

  // check that component supports ALP signals
  // (shall have been controlled before anyway)
  if (((mCapBitmap & RM_CAP_LOWPOWERCTRLSIG) == 0) ||
      (PfRoles::IsAlpRoleSink(Role()) == 0)) {
    // shall not get there
    RLOG_WARNING("ALPexit not supported for comp[i:%d - r:%s]\n", Index(), Role());
    return;
  }

  RM_SYSCTL_T RMcfg;
  RMcfg.eType = RM_RC_AUDIOLOWPOWER_EXIT;

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpCmdSetRmConfigALPexit comp[i:%d - r:%s]\n", Index(), Role());

    // intercept
    SetCFlag(CF_PENDING_RMCFG_ALP_EXIT);

    (mCompCallbacks.CP_Cmd_SetRMConfig)(mOmxilCompHdl, &RMcfg);
  }
  MutexCpUnlock();
}

/// commands release partial resources (suspension) to the OMXIL component
int ORPComp::CpCmdSuspendResources() {
  if (mCompCallbacks.CP_Cmd_ReleaseResource == NULL)
    return 0;

  // check if suspension allowed
  if (mPolicyGroup.can_suspend == false) {
    RLOG_WARNING("CpCmdSuspendResources not allowed for comp[i:%d - r:%s]\n", Index(), Role());
    return -1;
  }

  if (CFlag(CF_PENDING_RESPREEMPTION_PARTIAL)) {
    RLOG_WARNING("CpCmdSuspendResources while partial-preemption pending -- discarded\n");
    return 0;
  }

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpCmdSuspendResources comp[i:%d - r:%s]\n", Index(), Role());

    // intercept
    SetCFlag(CF_PENDING_RESPREEMPTION_PARTIAL);
    mDateResReleaseCmd = omxilosalservices::OmxILOsalTimer::GetSystemTime();

    (mCompCallbacks.CP_Cmd_ReleaseResource)(mOmxilCompHdl, OMX_TRUE);
  }
  MutexCpUnlock();
  return 0;
}

/// commands release full resources (unload) to the OMXIL component
void ORPComp::CpCmdUnloadResources() {
  if (mCompCallbacks.CP_Cmd_ReleaseResource == NULL)
    return;

  if (CFlag(CF_PENDING_RESPREEMPTION_FULL)) {
    RLOG_WARNING("CpCmdUnloadResources while full-preemption pending -- discarded\n");
    return;
  }

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpCmdUnloadResources comp[i:%d - r:%s]\n", Index(), Role());

    // intercept
    SetCFlag(CF_PENDING_RESPREEMPTION_FULL);
    mDateResReleaseCmd = omxilosalservices::OmxILOsalTimer::GetSystemTime();

    (mCompCallbacks.CP_Cmd_ReleaseResource)(mOmxilCompHdl, OMX_FALSE);
  }
  MutexCpUnlock();
}

/// notifies resources reservation request granted OK to the OMXIL component
void ORPComp::CpNotifyResReservationGranted() {
  if (mCompCallbacks.CP_Notify_ResReservationProcessed == NULL)
    return;

  if (CFlag(CF_INREQUESTPHASE) == 0) {
    RLOG_WARNING("CpNotifyResReservationGranted while inrequestphase not set -- discarded\n");
    return;
  }
  if (CFlag(CF_POSTREQUESTPHASE_RESGRANTED)) {
    RLOG_WARNING("CpNotifyResReservationGranted while postrequestphase resgranted set -- discarded\n");
    return;
  }

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpNotifyResReservationGranted comp[i:%d - r:%s]\n", Index(), Role());

    // intercept to make sure RequestPhase flag cleared

    // resource request success:
    // set flag to be associated with resource states 'required'
    // until notification of new state 'satisfied' from OMXIL component;
    // dont change automatically resource state to 'satisfied':
    // need to wait upon physical instantiations
    SetCFlag(CF_POSTREQUESTPHASE_RESGRANTED);

    // let ResManager handle resource date update
    // since it has impacts on components sorting
    // UpdateDateResourceGrant();

    ClearCFlag(CF_INREQUESTPHASE);
    ClearCFlag(CF_INDYNREQUESTPHASE);
    mDateResReleaseCmd = 0;

    (mCompCallbacks.CP_Notify_ResReservationProcessed)(mOmxilCompHdl, OMX_TRUE);
  }
  MutexCpUnlock();
}

/// notifies resources reservation request rejected KO to the OMXIL component
void ORPComp::CpNotifyResReservationRejected() {
  if (mCompCallbacks.CP_Notify_ResReservationProcessed == NULL)
    return;

  if (CFlag(CF_INREQUESTPHASE) == 0) {
    RLOG_WARNING("CpNotifyResReservationRejected while inrequestphase not set -- discarded\n");
    return;
  }
  if (CFlag(CF_POSTREQUESTPHASE_RESGRANTED)) {
    RLOG_WARNING("CpNotifyResReservationRejected while postrequestphase resgranted set -- discarded\n");
    return;
  }

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpNotifyResReservationRejected comp[i:%d - r:%s]\n", Index(), Role());

    // intercept to make sure RequestPhase flag cleared

    // resource request failed: move back to origin dual state
    // to avoid new resources access processing (done once only)
    switch (mStatesORp.DsOR()) {
      case DS_WAIT_REQUIRED:
      case DS_LOADED_REQUIRED:
        mStatesORp.UpdateDsOR(OMX_StateLoaded, RM_RS_NONE);
        break;
      case DS_IDLE_REQUIRED:
        // that transition is originating only from DS_IDLE_SATISFIED
        mStatesORp.UpdateDsOR(OMX_StateIdle, RM_RS_SATISFIED);
        break;
      case DS_PAUSE_REQUIRED:
        // that transition is originating only from DS_PAUSE_SATISFIED
        mStatesORp.UpdateDsOR(OMX_StatePause, RM_RS_SATISFIED);
        break;
      case DS_EXECUTING_REQUIRED:
        // that transition is originating only from DS_EXECUTING_SATISFIED
        mStatesORp.UpdateDsOR(OMX_StateExecuting, RM_RS_SATISFIED);
        break;
      default:
        // shall not get there.. (for suspended cases, notably)
        break;
    }

    ClearCFlag(CF_INREQUESTPHASE);
    ClearCFlag(CF_INDYNREQUESTPHASE);
    // dont modify mDateResReleaseCmd

    (mCompCallbacks.CP_Notify_ResReservationProcessed)(mOmxilCompHdl, OMX_FALSE);
  }
  MutexCpUnlock();
}

/// notifies error to the OMXIL component
void ORPComp::CpNotifyError(RM_STATUS_E nError) {
  if (mCompCallbacks.CP_Notify_Error == NULL)
    return;

  MutexCpLock();
  if (CFlag(CF_UNREGISTERED) == 0) {
    RLOG_DEBUG("CpNotifyError comp[i:%d - r:%s] error:%d\n", Index(), Role(), static_cast<int>(nError));

    (mCompCallbacks.CP_Notify_Error)(mOmxilCompHdl, nError);
  }
  MutexCpUnlock();
}

/// ctor
ORPComp::ORPComp(const OMX_PTR pCompHdl)
  :
  mIndex(msCompIndex++),
  mOmxilRole(),  // CAUTION: no custom ctor
  mOmxilName(),  // CAUTION: no custom ctor
  mCapBitmap(0),
  mOmxilCompHdl(pCompHdl),
  mCompCallbacks(),  // CAUTION: no custom ctor
  mNwHalflineId(-1),
  mNwHalflinePlaceId(0),
  mNwHalfinePlaceType(0),
  mCountPorts(0),
  mPorts(),  /// will call msMAXCountPorts * ORPport() ctor
  mPolicyGroup(),
  mStatesORp(),
  mDateResourceGrant(static_cast<unsigned int>(-1)),
  mDateResourceGrantBackup(static_cast<unsigned int>(-1)),
  mDateResReleaseCmd(0),
  mControlsBmp(0),
  mpMutexCp(NULL),
  mDomains(),
  mResEstimLevels(),
  mResEstimLevelsBackup(),
  mUccBitmap(0),
  mUccBitmapBackup(0) {
    omxilosalservices::OmxILOsalMutex::MutexCreate(mpMutexCp);
    if (!mpMutexCp) {
      RLOG_ERROR("omxilosalservices::OmxILOsalMutex::MutexCreate failed for ORPComp\n");
    }
    // additional inits
    memset(&mOmxilRole, '\0', sizeof(mOmxilRole));
    mOmxilRole[0] = 'z';  // don't ask why :-)
    memset(&mOmxilName, '\0', sizeof(mOmxilName));
    mOmxilName[0] = 'z';  // don't ask why :-)
    memset(&mCompCallbacks, 0, sizeof(mCompCallbacks));
  }

/// dtor
ORPComp::~ORPComp() {
  if (mpMutexCp) {
    omxilosalservices::OmxILOsalMutex::MutexFree(mpMutexCp);
  }
}

}  // namespace


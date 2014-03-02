/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ORPCompNetwork.cpp
 * ORPcomponents network class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "ORPCompNetwork.h"
#include "ORPCompHalfline.h"
#include "SharedInst.h"
#include "ResManager.h"
#include "PfRoles.h"

namespace rme {

/// ctor
ORPCompNetwork::ORPCompNetwork()
  :
  mListCompWaitInit(LID_COMP_ICL),
  mListCompAll(LID_COMP_ACL),
  mListHalflines(0),
  mHalflineIdGen(0),
  mpMutexNw(NULL) {
    omxilosalservices::OmxILOsalMutex::MutexCreate(mpMutexNw);
    if (!mpMutexNw) {
      RLOG_ERROR("omxilosalservices::OmxILOsalMutex::MutexCreate failed for ORPCompNetwork\n");
    }
  }

/// dtor
ORPCompNetwork::~ORPCompNetwork() {
  // cleaning up remaining components
  ORPComp* pcomp;
  ORPComp* pcompiter = mListCompAll.FrontNode();
  while (pcompiter) {
    pcomp = pcompiter;
    pcompiter = mListCompAll.NextNode(pcompiter);
    // has lock protection done inside
    UninitDelComponent(pcomp);
  }
  MutexNwLock();
  pcompiter = mListCompWaitInit.FrontNode();
  while (pcompiter) {
    pcomp = pcompiter;
    pcompiter = mListCompWaitInit.NextNode(pcompiter);
    mListCompWaitInit.Remove(pcomp);
    delete pcomp;
  }
  MutexNwUnlock();

  // cleaning up remaining halflines
  UpdateHalflines();

  if (mpMutexNw) {
    omxilosalservices::OmxILOsalMutex::MutexFree(mpMutexNw);
  }
}


///////////////////// Components lifecycle

/// adds a OMXIL resource proxy component (ORPComp) object
/// in the list of components waiting for init (on first use)
/// specific call mode: SYNCHRONOUS from RME client (OMXIL component) context
RM_STATUS_E ORPCompNetwork::RegisterComponent(const OMX_PTR pCompHdl,
                                              const RM_CBK_T* pCompCbks) {
  if (pCompHdl == NULL)
    return RM_E_INVALIDPTR;

  // no need to look for unicity of pCompHdl at this point:
  // will be checked at init time
  // (so potentially allows for duplicates in mListCompWaitInit but dont care)

  // create component
  ORPComp* pcomp = new ORPComp(pCompHdl);
  if (pcomp == NULL)
    return RM_E_KO;

  // provide callbacks to component
  // and do sanity check on these callbacks
  if (pcomp->SetCallbacks(pCompCbks) < 0) {
    // sanity check detected bad callbacks
    RLOG_ERROR("Register pCompHdl:%p rejected: bad callbacks\n", pCompHdl);
    // forget this component right now
    delete pcomp;
    return RM_E_INVALIDPTR;
  }

  // add component to list of components waiting for init (on first use)
  // access protection for mListCompWaitInit
  // against read/write done in asynchronous mode / RME thread context
  // and against write done in synchronous mode
  MutexNwLock();
  mListCompWaitInit.PushBack(pcomp);
  MutexNwUnlock();

  RLOG_DEBUG("Register done comp[i:%d]\n", pcomp->Index());

  return RM_E_NONE;
}

/// marks component as unregistered
/// specific call mode: SYNCHRONOUS from RME client (OMXIL component) context
RM_STATUS_E ORPCompNetwork::UnregisterComponent(const OMX_PTR pCompHdl) {
  if (pCompHdl == NULL)
    return RM_E_INVALIDPTR;

  ORPComp* pcomp = NULL;
  RM_STATUS_E status = RM_E_NONE;

  // access protection for mListCompAll
  // against write done in asynchronous mode / RME thread context
  // access protection for mListCompWaitInit
  // against read/write done in asynchronous mode / RME thread context
  // and against write done in synchronous mode
  MutexNwLock();

  // look in list of all initialized components
  for (ORPComp *pcompiter = mListCompAll.FrontNode();
      pcompiter != NULL;
      pcompiter = mListCompAll.NextNode(pcompiter)) {
    if (pcompiter->OmxilCompHdl() == pCompHdl) {
      pcomp = pcompiter;
      break;
    }
  }

  // if not found then look in list components waiting for init (on first use)
  // (in case registered component never used)
  if (pcomp == NULL) {
    status = RM_E_INVALIDPTR;
    for (ORPComp *pcompiter = mListCompWaitInit.FrontNode();
        pcompiter != NULL;
        pcompiter = mListCompWaitInit.NextNode(pcompiter)) {
      if (pcompiter->OmxilCompHdl() == pCompHdl) {
        pcomp = pcompiter;
        status = RM_E_NONE;
        break;
      }
    }
  }

  AGov* agovM = SharedInst::GetAgov();
  if (pcomp) {
    // in mutex lock context against processing done on UccReactTunnel (sync vs async call context)
    agovM->UccReactUnregister(pcomp->Role());
  }

  MutexNwUnlock();

  if (pcomp) {
    // access protection against all calls to callbacks done in RME thread context
    pcomp->MutexCpLock();
    // mark component unregistered: will be destroyed later on
    pcomp->SetCFlag(ORPComp::CF_UNREGISTERED);
    RLOG_DEBUG("Unregister done for comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());
    pcomp->MutexCpUnlock();
#ifdef MAPPING_x540
#ifndef SYNCHRO_FROM_ADM
    // workaround for x540: ADM synchro not present
    // =>suboptimal hack to wakeup activity detector
    agovM->UccTrigsActivityDetector();
#endif
#endif
    // dont use pcomp after mutex unlock: might be destroyed
  } else {
    RLOG_DEBUG("Unregister skipped for unknown pCompHdl:%p\n", pCompHdl);
  }

  return status;
}

/// retrieves component from OMXIL handle
/// and if needed (on first use) initializes ORPComp
ORPComp* ORPCompNetwork::GetComponent(const OMX_PTR pCompHdl, bool *pcompinitisdone) {
  if (pCompHdl == NULL)
    return NULL;

  ORPComp* pcomp = NULL;
  bool foundininitlist = false;

  // look in list of all initialized components
  for (ORPComp *pcompiter = mListCompAll.FrontNode();
      pcompiter != NULL;
      pcompiter = mListCompAll.NextNode(pcompiter)) {
    if (pcompiter->OmxilCompHdl() == pCompHdl) {
      pcomp = pcompiter;
      // check if the component was unregistered in between
      // (synchronous unregister and asynchronous message based commands)
      if (pcomp->CFlag(ORPComp::CF_UNREGISTERED)) {
        // protection for unregister(): CF_UNREGISTERED flag set under lock
        pcomp->MutexCpLock();
        pcomp->MutexCpUnlock();
        // destroy component; has lists Nw lock protection done inside
        UninitDelComponent(pcomp);
        pcomp = NULL;
      }
      foundininitlist = true;
      break;
    }
  }

  if (pcompinitisdone) {
    *pcompinitisdone = foundininitlist;
  }
  if (foundininitlist == true) {
    return pcomp;
  }

  // not found: look in list of components waiting for init (on first use)
  // access protection for mListCompWaitInit
  // against write done in synchronous mode
  MutexNwLock();
  for (ORPComp *pcompiter = mListCompWaitInit.FrontNode();
      pcompiter != NULL;
      pcompiter = mListCompWaitInit.NextNode(pcompiter)) {
    if (pcompiter->OmxilCompHdl() == pCompHdl) {
      pcomp = pcompiter;
      // found: remove component from waiting list and break
      mListCompWaitInit.Remove(pcompiter);
      break;
    }
  }
  MutexNwUnlock();

  if (pcomp) {
    // check if the component was unregistered in between
    if (pcomp->CFlag(ORPComp::CF_UNREGISTERED)) {
        // protection for unregister() tail: CF_UNREGISTERED flag set under lock
        pcomp->MutexCpLock();
        pcomp->MutexCpUnlock();
        // so just destroy component (no init done, and not in any list)
        delete pcomp;
        pcomp = NULL;
        if (pcompinitisdone) {
          // init not done, but mark it to let know caller that component was found even though returning null
          *pcompinitisdone = true;
        }
    }
  }

  return pcomp;
}

/// inits ORPComp on first use; deletes it in case of init error (no retry)
int ORPCompNetwork::InitComponentOnFirstUse(ORPComp* pcomp) {
  if (pcomp == NULL)
    return -1;

  // check for unicity of component handle
  const OMX_PTR pcomphdl = pcomp->OmxilCompHdl();
  for (ORPComp *pcompiter = mListCompAll.FrontNode();
      pcompiter != NULL;
      pcompiter = mListCompAll.NextNode(pcompiter)) {
    if (pcompiter->OmxilCompHdl() == pcomphdl) {
      RLOG_ERROR("comp init: no handle unicity\n");
      delete pcomp;
      return -1;
    }
  }

  // get capabilities from OMXIL component
  RM_STATUS_E status = pcomp->CpGetCapabilities();
  if (status != RM_E_NONE) {
    RLOG_ERROR("comp init: CpGetCapabilities status:%d\n", static_cast<int>(status));
    delete pcomp;
    return -1;
  }

  // add component to list of all initialized components (sorted per prio & date)
  // and not in any halfline (single)
  pcomp->SetNwHalflineId(-1);

  // access protection for mListCompAll
  // against read done in synchronous call mode
  MutexNwLock();
  mListCompAll.PushBack(pcomp);
  MutexNwUnlock();

  RLOG_DEBUG("init done for comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());

  return 0;
}

/// removes from network and possibly destroy component proxy
void ORPCompNetwork::UninitDelComponent(ORPComp* pcomp) {
  if (pcomp == NULL)
    return;

  // force disconnections of any remaining connections
  // done so in order to update halflines
  for (unsigned int j = 0; j < pcomp->CountPorts(); j++) {
    if (pcomp->IsInputPort(j))
      UntunnelCompIn(pcomp, j, false);
    else
      UntunnelCompOut(pcomp, j, false);
  }

  // remove from lists

  // access protection for mListCompAll
  // against read done in synchronous call mode
  MutexNwLock();
  mListCompAll.Remove(pcomp);
  MutexNwUnlock();

  int netcnid = pcomp->NwHalflineId();
  if (netcnid >= 0) {
    ORPCompHalfline* phalfline = GetHalfline(static_cast<unsigned int>(netcnid));
    if (phalfline) {
      phalfline->ListORPcomps()->Remove(pcomp);
    }
  }

  // inform policy server if needed
  // and destroy domains if needed
  pcomp->ReInitializeUnload();

  // destroy
  RLOG_DEBUG("deleting comp[i:%d - r:%s]\n", pcomp->Index(), pcomp->Role());
  delete pcomp;
}

/// removes all unregistered components
void ORPCompNetwork::RemoveUnregComponents() {
  ORPComp* pcomp;

  // check big list
  ORPComp* pcompiter = mListCompAll.FrontNode();
  while (pcompiter) {
    // compute next node before working on current node
    pcomp = pcompiter;
    pcompiter = mListCompAll.NextNode(pcompiter);

    // remove component if marked unregistered
    if (pcomp->CFlag(ORPComp::CF_UNREGISTERED)) {
      // protection for unregister() tail: CF_UNREGISTERED flag set under lock
      pcomp->MutexCpLock();
      pcomp->MutexCpUnlock();
      // destroy component; has lists Nw lock protection done inside
      UninitDelComponent(pcomp);
    }
  }

  // check list of components waiting for init (on first use)
  // access protection for mListCompWaitInit
  // against write done in synchronous mode
  MutexNwLock();
  pcompiter = mListCompWaitInit.FrontNode();
  while (pcompiter) {
    // compute next node before working on current node
    pcomp = pcompiter;
    pcompiter = mListCompWaitInit.NextNode(pcompiter);

    // remove component if marked unregistered
    if (pcomp->CFlag(ORPComp::CF_UNREGISTERED)) {
      // protection for unregister() tail: CF_UNREGISTERED flag set under lock
      pcomp->MutexCpLock();
      pcomp->MutexCpUnlock();
      // simple remove and destroy component
      mListCompWaitInit.Remove(pcomp);
      delete pcomp;
    }
  }
  MutexNwUnlock();
}

////////////////////// Components networks lines management

/// sorts all components per priority & date
/// higher priority components come first
/// for identical priorities, components with more recent resource date come first
void ORPCompNetwork::SortComponents() {
  // access protection for mListCompAll
  // against read done in synchronous call mode
  MutexNwLock();
  mListCompAll.Sort(ORPComp::ComparePerPriorityAndResourceDate);
  MutexNwUnlock();
}

/// creates and returns a halfline
ORPCompHalfline* ORPCompNetwork::NewHalfline() {
  ORPCompHalfline* phalfline = new ORPCompHalfline(mHalflineIdGen++);
  if (phalfline) {
    RLOG_DEBUG("new halfline hlid:%d\n", phalfline->HlId());
    mListHalflines.PushBack(phalfline);
  }
  return phalfline;
}

/// retrieves halfline based on its (unique) id
ORPCompHalfline* ORPCompNetwork::GetHalfline(unsigned int hlid) const {
  for (ORPCompHalfline* phalfline_iter = mListHalflines.FrontNode();
      phalfline_iter != NULL;
      phalfline_iter = mListHalflines.NextNode(phalfline_iter)) {
    if (phalfline_iter->HlId() == hlid)
      return phalfline_iter;
  }
  return NULL;
}

/// returns next halfline
ORPCompHalfline* ORPCompNetwork::NextHalfline(const ORPCompHalfline* phalfline) const {
  return mListHalflines.NextNode(phalfline);
}

/// returns previous halfline
ORPCompHalfline* ORPCompNetwork::PrevHalfline(const ORPCompHalfline* phalfline) const {
  return mListHalflines.PrevNode(phalfline);
}

/// updates metadata on halflines when needed
/// and removes empty halflines
void ORPCompNetwork::UpdateHalflines(bool compute_depth) {
  if (compute_depth)
    ClearRecursiveMarkerHalflines();

  // dual pass

  // pass1: remove empty halflines
  // and merge halflines with only one connection (unless explicitely required)
  unsigned int maxretry = mListHalflines.Size();
  bool docleanuppass = true;
  while (docleanuppass && (maxretry > 0)) {
    docleanuppass = false;
    maxretry--;
    ORPCompHalfline* phalfline_iter = mListHalflines.FrontNode();
    while (phalfline_iter) {
      ORPCompHalfline* phalfline = phalfline_iter;
      phalfline_iter = mListHalflines.NextNode(phalfline_iter);

      if (phalfline->CountComponents() == 0) {
        RLOG_DEBUG("removing empty halfline hlid:%d (uh)\n", phalfline->HlId());
        int rr = mListHalflines.Remove(phalfline);
        if (rr) {
          RLOG_DEBUG("invalid remove %d\n", rr);
        }
        delete phalfline;
      } else {
        if (phalfline->CountOutputConnections() == 1) {
          ORPCompHalfline* phalfline_down =
            GetHalfline(static_cast<unsigned int>(phalfline->PeerHalflineDown(0, NULL)));
          if ((phalfline_down) && (phalfline_down->CountInputConnections() == 1)) {
            int status = MergeHalflines(phalfline, phalfline_down);
            if (status >= 0) {
              // mListHalflines modified: need to break and re-do the cleanup pass
              docleanuppass = true;
              phalfline_iter = NULL;
              compute_depth = true;  // depth needs update for halflines below
              break;
            }
          }
        }
      }
    }
  }

  // pass2: depth updates + components place updates
  for (ORPCompHalfline* phalfline_iter = mListHalflines.FrontNode();
      phalfline_iter != NULL;
      phalfline_iter = mListHalflines.NextNode(phalfline_iter)) {
    if (compute_depth) {
      // compute depth recursively down from 'input' halflines
      if (phalfline_iter->CountInputConnections() == 0) {
        UpdateHalflinesDepthDown(phalfline_iter);
      }
    }

    phalfline_iter->UpdateCompPlaceIds();
  }

  // sort halflines according to depths
  mListHalflines.Sort(ORPCompHalfline::ComparePerDepth);
}

/// clears extern tag on halflines
void ORPCompNetwork::ClearExternTagHalflines() {
  ORPCompHalfline* phalfline_iter = mListHalflines.FrontNode();
  while (phalfline_iter) {
    ORPCompHalfline* phalfline = phalfline_iter;
    phalfline_iter = mListHalflines.NextNode(phalfline_iter);

    if (phalfline->CountComponents() == 0) {
      // remove halfline if necessary => keep there
      RLOG_DEBUG("removing empty halfline hlid:%d (ceth)\n", phalfline->HlId());
      int rr = mListHalflines.Remove(phalfline);
      if (rr) {
        RLOG_DEBUG("invalid remove %d\n", rr);
      }
      delete phalfline;
    } else {
      phalfline->ResetExternTags();
    }
  }
}

/// clears recursive marker on halflines
void ORPCompNetwork::ClearRecursiveMarkerHalflines() {
  ORPCompHalfline* phalfline_iter = mListHalflines.FrontNode();
  while (phalfline_iter) {
    ORPCompHalfline* phalfline = phalfline_iter;
    phalfline_iter = mListHalflines.NextNode(phalfline_iter);

    if (phalfline->CountComponents() == 0) {
      // remove halfline if necessary => keep there
      RLOG_DEBUG("removing empty halfline hlid:%d (crmh)\n", phalfline->HlId());
      int rr = mListHalflines.Remove(phalfline);
      if (rr) {
        RLOG_DEBUG("invalid remove %d\n", rr);
      }
      delete phalfline;
    } else {
      phalfline->UnsetRecursiveMarker();
    }
  }
}

/// computes depth recursively down from given halfline
void ORPCompNetwork::UpdateHalflinesDepthDown(ORPCompHalfline* phalfline) {
  if (phalfline == NULL)
    return;
  if (phalfline->RecursiveMarker())
    return;
  else
    phalfline->SetRecursiveMarker();  // mark current line
  unsigned int depth = 0;

  // depth => takes greater value from input halflines
  unsigned int count_connections_in = phalfline->CountInputConnections();
  unsigned int count_connections_in_effective = 0;
  for (unsigned int k = 0; k < count_connections_in; k++) {
    int peer_hlid = phalfline->PeerHalflineUp(k, NULL);
    if (peer_hlid >= 0) {
      ORPCompHalfline* phalfline_up = GetHalfline(static_cast<unsigned int>(peer_hlid));
      if ((phalfline_up) && (phalfline_up != phalfline) &&
              (phalfline_up->Depth() > depth)) {
        count_connections_in_effective++;
        depth = phalfline_up->Depth();
      }
    }
  }

  // first halflines have depth 0; increments by 1 on each level
  if (count_connections_in_effective > 0) {
    phalfline->SetDepth(depth + 1);
  } else {
    phalfline->SetDepth(depth);
  }

  // propagate down recursively
  for (unsigned int k = 0; k < phalfline->CountOutputConnections(); k++) {
    int peer_hlid = phalfline->PeerHalflineDown(k, NULL);
    if (peer_hlid >= 0) {
      ORPCompHalfline* phalfline_down = GetHalfline(static_cast<unsigned int>(peer_hlid));
      if ((phalfline_down) && (phalfline_down != phalfline) &&
          (phalfline_down->RecursiveMarker() == false)) {
        UpdateHalflinesDepthDown(phalfline_down);
      }
    }
  }
}

/// processes tunneling of two components
RM_STATUS_E ORPCompNetwork::TunnelComponents(ORPComp* pcompout,
                                             unsigned int orpportout,
                                             ORPComp* pcompin,
                                             unsigned int orpportin) {
  if (pcompout == NULL)
    return RM_E_KO;
  if (pcompin == NULL)
    return RM_E_KO;
  if (orpportout >= ORPComp::msMAXCountPorts)
    return RM_E_NOMORE;
  if (orpportin >= ORPComp::msMAXCountPorts)
    return RM_E_NOMORE;

  RLOG_DEBUG("Tunnel comps [i:%d - r:%s] to [i:%d - r:%s]\n", pcompout->Index(), pcompout->Role(), pcompin->Index(), pcompin->Role());
  RLOG_DEBUG("--- co[orp:%d - omx:%d] ci[orp:%d - omx:%d]\n", orpportout, pcompout->OmxPortIndex(orpportout), orpportin, pcompin->OmxPortIndex(orpportin));
  RLOG_DEBUG("--- co[nhi:%d - pnhi:%d] ci[nhi:%d - pnhi:%d]\n", pcompout->NwHalflineId(), pcompout->PortPeerNwHlId(orpportout), pcompin->NwHalflineId(), pcompin->PortPeerNwHlId(orpportin));

  // check comps and if needed split or untunnel
  int rescheck = CheckCompsBeforeTunneling(pcompout, orpportout, pcompin, orpportin);
  if (rescheck < 0) {
    // not a failure: means that tunnelling already good => nothing to do
    return RM_E_NONE;
  }

  // get values (might have changed after split/untunnel)
  int compoutNwHlId = pcompout->NwHalflineId();
  // int portoutPeerNwHlId = pcompout->PortPeerNwHlId(orpportout);
  int compinNwHlId = pcompin->NwHalflineId();
  // int portinPeerNwHlId = pcompin->PortPeerNwHlId(orpportin);

  /* check values: now connection shall be possible in any case,
     and compin either single or at beginning of a halfline
     and compout either single or at end of a halfline */
  if ((compoutNwHlId < 0) && (compinNwHlId < 0)) {
    // both components singles: get free halfline(s) to store them
    ORPCompHalfline* phalfline1;
    ORPCompHalfline* phalfline2;

    phalfline1 = NewHalfline();
    phalfline2 = phalfline1;

    /* check for a priori knowledge forcing components
       to be in separate lines (mixer/splitter like cases) */
    /* not currently needed: keep for reference
    if (PfRoles::KeepAtHalflineStart(pcompin->Role()) ||
        PfRoles::KeepAtHalflineEnd(pcompout->Role())) {
      // different lines
      phalfline2 = NewHalfline();
    }
    */

    if ((phalfline1 == NULL) || (phalfline2 == NULL))
        return RM_E_KO;

    /* add components to the halfline -
       halfline sorted in flow direction, and set ids */

    phalfline1->ListORPcomps()->PushFront(pcompout);  // front
    pcompout->SetNwHalflineId(static_cast<int>(phalfline1->HlId()));
    pcompout->SetPortPeerNwHlId(orpportout, static_cast<int>(phalfline2->HlId()));

    phalfline2->ListORPcomps()->PushBack(pcompin);  // back
    pcompin->SetNwHalflineId(static_cast<int>(phalfline2->HlId()));
    pcompin->SetPortPeerNwHlId(orpportin, static_cast<int>(phalfline1->HlId()));
  } else if ((compoutNwHlId >= 0) && (compinNwHlId >= 0)) {
    // both in halflines and at extremities => interconnect those two
    pcompout->SetPortPeerNwHlId(orpportout, compinNwHlId);
    pcompin->SetPortPeerNwHlId(orpportin, compoutNwHlId);
  } else if (compoutNwHlId >= 0) /* && (compinNwHlId < 0) */ {
    ORPCompHalfline* phalfline;
    if ((pcompout->CountConnectionsOnPortOut() > 0)
        /* not currently needed: keep for reference
        || (PfRoles::KeepAtHalflineEnd(pcompout->Role()))
        || (PfRoles::KeepAtHalflineStart(pcompin->Role()))
        */
        ) {
      /* need to create halfline for compin
         (compout is of splitter type or compin of mixer type) */
      phalfline = NewHalfline();
    } else {
      phalfline = GetHalfline(static_cast<unsigned int>(compoutNwHlId));
    }
    if (phalfline == NULL)
      return RM_E_KO;

    // append pcompin to halfline of pcompout or to created halfline
    phalfline->ListORPcomps()->PushBack(pcompin);  // back
    pcompin->SetNwHalflineId(static_cast<int>(phalfline->HlId()));
    pcompin->SetPortPeerNwHlId(orpportin, compoutNwHlId);

    pcompout->SetPortPeerNwHlId(orpportout, static_cast<int>(phalfline->HlId()));
  } else if (compinNwHlId >= 0) /* && (compoutNwHlId < 0) */ {
    ORPCompHalfline* phalfline;
    if ((pcompin->CountConnectionsOnPortIn() > 0)
        /* not currently needed: keep for reference
        || (PfRoles::KeepAtHalflineStart(pcompin->Role()))
        || (PfRoles::KeepAtHalflineEnd(pcompout->Role()))
        */
        ) {
      /* need to create halfline for compout
         (compin is of mixer type or compout of splitter type) */
      phalfline = NewHalfline();
    } else {
      phalfline = GetHalfline(static_cast<unsigned int>(compinNwHlId));
    }
    if (phalfline == NULL)
      return RM_E_KO;

    // append pcompout to halfline of pcompin or to created halfline
    phalfline->ListORPcomps()->PushFront(pcompout);  // front
    pcompout->SetNwHalflineId(static_cast<int>(phalfline->HlId()));
    pcompout->SetPortPeerNwHlId(orpportout, compinNwHlId);

    pcompin->SetPortPeerNwHlId(orpportin, static_cast<int>(phalfline->HlId()));
  }

  // update halflines
  UpdateHalflines(true);

  AGov* agovM = SharedInst::GetAgov();
  // mutex protection against processing done on UccReactUnregister (sync vs async call context)
  MutexNwLock();
  agovM->UccReactTunnel(pcompin->Role(), pcompout->Role());
  MutexNwUnlock();

  return RM_E_NONE;
}

/// checks components before tunneling
int ORPCompNetwork::CheckCompsBeforeTunneling(ORPComp* pcompout,
                                               unsigned int orpportout,
                                               ORPComp* pcompin,
                                               unsigned int orpportin) {
  int compoutNwHlId = -1;
  int portoutPeerNwHlId = -1;
  ORPCompHalfline* phalflineout = NULL;
  if (pcompout) {
    compoutNwHlId = pcompout->NwHalflineId();
    if (compoutNwHlId >= 0)
      phalflineout = GetHalfline(static_cast<unsigned int>(compoutNwHlId));
    portoutPeerNwHlId = pcompout->PortPeerNwHlId(orpportout);
  }

  int compinNwHlId = -1;
  int portinPeerNwHlId = -1;
  ORPCompHalfline* phalflinein = NULL;
  if (pcompin) {
    compinNwHlId = pcompin->NwHalflineId();
    if (compinNwHlId >= 0)
      phalflinein = GetHalfline(static_cast<unsigned int>(compinNwHlId));
    portinPeerNwHlId = pcompin->PortPeerNwHlId(orpportin);
  }

  if ((compoutNwHlId >= 0) && (portoutPeerNwHlId >= 0) &&
      (compinNwHlId >= 0) && (portinPeerNwHlId >= 0)) {
    if (compoutNwHlId == compinNwHlId) {
      if ((phalflineout->ListORPcomps()->NextNode(pcompout) == pcompin) &&
          (phalflinein->ListORPcomps()->PrevNode(pcompin) == pcompout)) {
        // on same halfline and already connected..
        RLOG_DEBUG("skipping attempt to retunnel existing intra connection\n");
        // no need to work then
        return -1;
      }
    } else {
      if ((portoutPeerNwHlId == compinNwHlId) &&
          (portinPeerNwHlId == compoutNwHlId) &&
          (pcompout == phalflineout->ListORPcomps()->BackNode()) &&
          (pcompin == phalflinein->ListORPcomps()->FrontNode())) {
        // on different halflines and already connected
        RLOG_DEBUG("skipping attempt to retunnel existing inter connection\n");
        // no need to work then
        return -2;
      }
    }
  }

  // check comp_out if needs to split or untunnel
  if (compoutNwHlId >= 0) {
    // compout marked as already connected => check connection at port level
    if (portoutPeerNwHlId < 0) {
      if (pcompout->CountConnectionsOnPortOut() > 0) {
        /* compout was not connected on this output port,
           but has other output connections
           => multiconnection on output
           ==> splitter like component
           ===> check if comp is at end of halfline, if not split in two */
        SplitHalfLineAfter(compoutNwHlId, pcompout);
      }  // else: no connection on this port, dont split
    } else {
      // this port was connected: disconnect
      UntunnelCompOut(pcompout, orpportout, false);
    }
  }

  // check comp_in if needs to split or untunnel
  if (compinNwHlId >= 0) {
    // compin marked as already connected => check connection at port level
    if (portinPeerNwHlId < 0) {
      if (pcompin->CountConnectionsOnPortIn() > 0) {
        /* compin was not connected on this input port ,
           but has other input connections
           => multiconnection on input
           ==> mixer like component
           ===> check if comp is at start of halfline, if not split in two */
        SplitHalfLineBefore(compinNwHlId, pcompin);
      }  // else: no connection on this port, dont split
    } else {
      // this port was connected: disconnect
      UntunnelCompIn(pcompin, orpportin, false);
    }
  }

  return 0;
}

/// processes untunneling of a component on an output port
RM_STATUS_E ORPCompNetwork::UntunnelCompOut(ORPComp* pcompout,
                                            unsigned int orpportout,
                                            bool external) {
  if (pcompout == NULL)
    return RM_E_KO;
  if (orpportout >= ORPComp::msMAXCountPorts)
    return RM_E_NOMORE;

  int hlid = pcompout->NwHalflineId();
  if (hlid < 0) {
    // was already disconnected -- nothing more to do
    return RM_E_NONE;
  }

  RLOG_DEBUG("Untunnel compOUT [i:%d - r:%s] [nhi:%d - pnhi:%d]\n", pcompout->Index(), pcompout->Role(), pcompout->NwHalflineId(), pcompout->PortPeerNwHlId(orpportout));

  int peer_id = pcompout->PortPeerNwHlId(orpportout);
  if (peer_id >= 0) {
    // split halfline in two after pcompout
    RM_STATUS_E status = SplitHalfLineAfter(hlid, pcompout);
    if (status != RM_E_NONE)
      return status;

    // hlid possibly changed
    hlid = pcompout->NwHalflineId();
    peer_id = pcompout->PortPeerNwHlId(orpportout);

    /* after split pcompout is last element of halfline;
       disconnection to do between halflines */
    ORPCompHalfline* phalfline_peer = NULL;
    if (peer_id >= 0) {
        phalfline_peer = GetHalfline(static_cast<unsigned int>(peer_id));
    } else {
        RLOG_DEBUG("invalid peer id after SplitHalfLineAfter\n");
    }
    ORPComp* pcomp_peer = NULL;
    if (phalfline_peer)
      pcomp_peer = phalfline_peer->ListORPcomps()->FrontNode();

    if (pcomp_peer) {
      // update peer port disconnected: has id value at hlid
      pcomp_peer->DisconnectInputPortsWithPeerNwHlId(hlid);

      if (pcomp_peer->CountConnectionsOnPort() == 0) {
        // no more connections for peer : remove from halfline
        phalfline_peer->ListORPcomps()->Remove(pcomp_peer);
        pcomp_peer->SetNwHalflineId(-1);
      }
    }
    // update port disconnected
    pcompout->SetPortPeerNwHlId(orpportout, -1);
  }

  if (pcompout->CountConnectionsOnPort() == 0) {
    // no more connections (last element of halfline) : remove from halfline
    if (hlid >= 0) {
      ORPCompHalfline* phalfline = GetHalfline(static_cast<unsigned int>(hlid));
      if (phalfline) {
        phalfline->ListORPcomps()->Remove(pcompout);
      }
    }
    pcompout->SetNwHalflineId(-1);
  }

  // update halflines
  if (external)
    UpdateHalflines(true);

  return RM_E_NONE;
}

/// processes untunneling of a component on an input port
RM_STATUS_E ORPCompNetwork::UntunnelCompIn(ORPComp* pcompin,
                                           unsigned int orpportin,
                                           bool external) {
  if (pcompin == NULL)
    return RM_E_KO;
  if (orpportin >= ORPComp::msMAXCountPorts)
    return RM_E_NOMORE;

  int hlid = pcompin->NwHalflineId();
  if (hlid < 0) {
    // was already disconnected -- nothing more to do
    return RM_E_NONE;
  }

  RLOG_DEBUG("Untunnel compIN [i:%d - r:%s] [nhi:%d - pnhi:%d]\n", pcompin->Index(), pcompin->Role(), pcompin->NwHalflineId(), pcompin->PortPeerNwHlId(orpportin));

  int peer_id = pcompin->PortPeerNwHlId(orpportin);
  if (peer_id >= 0) {
    // split halfline in two before pcompin
    RM_STATUS_E status = SplitHalfLineBefore(hlid, pcompin);
    if (status != RM_E_NONE)
      return status;

    // hlid possibly changed
    hlid = pcompin->NwHalflineId();
    peer_id = pcompin->PortPeerNwHlId(orpportin);

    /* after split pcompin is first element of halfline;
       disconnection to do between halflines */
    ORPCompHalfline* phalfline_peer = NULL;
    if (peer_id >= 0) {
        phalfline_peer = GetHalfline(static_cast<unsigned int>(peer_id));
    } else {
        RLOG_DEBUG("invalid peer id after SplitHalfLineBefore\n");
    }
    ORPComp* pcomp_peer = NULL;
    if (phalfline_peer)
      pcomp_peer = phalfline_peer->ListORPcomps()->BackNode();

    if (pcomp_peer) {
      // update peer port disconnected: has id value at hlid
      pcomp_peer->DisconnectOutputPortsWithPeerNwHlId(hlid);

      if (pcomp_peer->CountConnectionsOnPort() == 0) {
        // no more connections for peer : remove from halfline
        phalfline_peer->ListORPcomps()->Remove(pcomp_peer);
        pcomp_peer->SetNwHalflineId(-1);
      }
    }
    // update port disconnected
    pcompin->SetPortPeerNwHlId(orpportin, -1);
  }

  if (pcompin->CountConnectionsOnPort() == 0) {
    // no more connections (last element of halfline) : remove from halfline
    if (hlid >= 0) {
      ORPCompHalfline* phalfline = GetHalfline(static_cast<unsigned int>(hlid));
      if (phalfline) {
        phalfline->ListORPcomps()->Remove(pcompin);
      }
    }
    pcompin->SetNwHalflineId(-1);
  }

  // update halflines
  if (external)
    UpdateHalflines(true);

  return RM_E_NONE;
}

/// splits a halfline after given component
RM_STATUS_E ORPCompNetwork::SplitHalfLineAfter(unsigned int hlid,
                                               ORPComp* pcompout) {
  if (pcompout == NULL)
    return RM_E_KO;

  ORPCompHalfline* phalfline = GetHalfline(hlid);
  ORPComp* pcomp_next = NULL;
  if (phalfline)
    pcomp_next = phalfline->ListORPcomps()->NextNode(pcompout);

  if (pcomp_next == NULL)
    return RM_E_NONE;  // was last element of halfline => nothing to do

  // splitting after pcompout is same as splitting before pcomp_next
  return SplitHalfLineBefore(hlid, pcomp_next);
}

/// splits a halfline before given component
RM_STATUS_E ORPCompNetwork::SplitHalfLineBefore(unsigned int hlid,
                                                ORPComp* pcompin) {
  if (pcompin == NULL)
    return RM_E_KO;

  ORPCompHalfline* phalfline = GetHalfline(hlid);
  ORPComp* pcomp_prev = NULL;
  if (phalfline)
    pcomp_prev = phalfline->ListORPcomps()->PrevNode(pcompin);

  if (pcomp_prev == NULL)
    return RM_E_NONE;  // was first element of halfline => nothing to do

  ORPCompHalfline* phalfline_split;
  phalfline_split = NewHalfline();
  if (phalfline_split == NULL)
    return RM_E_KO;
  int newhalfline_id = static_cast<int>(phalfline_split->HlId());

  // last element in halfline to be moved
  ORPComp* pcomp_last = phalfline->ListORPcomps()->BackNode();

  // split at pcompin level => move components in created halfline
  ORPComp* pcomp_move = pcompin;
  while (pcomp_move) {
    ORPComp* pcomp_move_next = phalfline->ListORPcomps()->NextNode(pcomp_move);

    // remove from old halfline
    phalfline->ListORPcomps()->Remove(pcomp_move);
    // add at end of created halfline (going downstream)
    phalfline_split->ListORPcomps()->PushBack(pcomp_move);
    pcomp_move->SetNwHalflineId(newhalfline_id);
    // for pcompin, dont replace input ports => connection to hlid kept
    if (pcomp_move == pcompin)
      pcomp_move->ReplacePortsId(hlid, newhalfline_id, false, true);
    // for pcomplast, dont replace output ports => connection to hlid kept (loop case)
    else if (pcomp_move == pcomp_last)
      pcomp_move->ReplacePortsId(hlid, newhalfline_id, true, false);
    else
      pcomp_move->ReplacePortsId(hlid, newhalfline_id, true, true);

    pcomp_move = pcomp_move_next;
  }

  /* for last element:
     has to update all peer ports in other halflines
     that were connected to this element */
  if (pcomp_last) {
    for (unsigned int j = 0; j < pcomp_last->CountPorts(); j++) {
      int peer_id = pcomp_last->PortPeerNwHlId(j);
      if ((peer_id >= 0) && (peer_id != newhalfline_id)) {
        ORPCompHalfline* phalfline_peer = GetHalfline(static_cast<unsigned int>(peer_id));
        ORPComp* pcomp_peer = NULL;
        if (phalfline_peer)
          pcomp_peer = phalfline_peer->ListORPcomps()->FrontNode();

        if (pcomp_peer) {
          // update peer port : replace id
          if (pcomp_last->IsOutputPort(j))
            pcomp_peer->ReplacePortsId(hlid, newhalfline_id, true, false);
          else
            pcomp_peer->ReplacePortsId(hlid, newhalfline_id, false, true);
        }
      }
    }
  }

  // for prev element: has to update output ports
  pcomp_prev->ReplacePortsId(hlid, newhalfline_id, false, true);

  return RM_E_NONE;
}

/// merges halflines if possible
int ORPCompNetwork::MergeHalflines(ORPCompHalfline* phalflineup, ORPCompHalfline* phalflinedown) {
  if ((phalflineup == NULL) || (phalflinedown == NULL) ||
      (phalflineup->CountOutputConnections() != 1) ||
      (phalflinedown->CountInputConnections() != 1) ||
      (phalflineup == phalflinedown) ||
      (phalflineup->PeerHalflineDown(0, NULL) < 0) ||
      (phalflinedown !=
       GetHalfline(static_cast<unsigned int>(phalflineup->PeerHalflineDown(0, NULL))))) {
    return -1;
  }
  ORPComp* pcompin = phalflineup->ListORPcomps()->BackNode();
  ORPComp *pcompout = phalflinedown->ListORPcomps()->FrontNode();

  if ((pcompin == NULL) || (pcompout == NULL)
    /* not currently needed: keep for reference
      || (PfRoles::KeepAtHalflineStart(pcompout->Role()) == true)
      || (PfRoles::KeepAtHalflineEnd(pcompin->Role()) == true)
     */
      ) {
    return -1;
  }

  RLOG_DEBUG("halfline reduction between hlid:%d (%s) and hlid:%d (%s)\n", phalflineup->HlId(), pcompin->Role(), phalflinedown->HlId(), pcompout->Role());
  // merge halfline_down in halfline

  // last element in halfline to be moved
  ORPComp* pcomp_last = phalflinedown->ListORPcomps()->BackNode();

  ORPComp* pcomp_move = pcompout;  // is first element of halfline to be moved
  while (pcomp_move) {
    ORPComp* pcomp_move_next = phalflinedown->ListORPcomps()->NextNode(pcomp_move);

    // remove from old halfline
    phalflinedown->ListORPcomps()->Remove(pcomp_move);
    // add at end of created halfline (going downstream)
    phalflineup->ListORPcomps()->PushBack(pcomp_move);
    pcomp_move->SetNwHalflineId(static_cast<int>(phalflineup->HlId()));
    pcomp_move->ReplacePortsId(static_cast<int>(phalflinedown->HlId()),
                               static_cast<int>(phalflineup->HlId()),
                               true, true);

    pcomp_move = pcomp_move_next;
  }

  /* for last element:
     has to update all peer ports in other halflines
     that were connected to this element */
  if (pcomp_last) {
    for (unsigned int j = 0; j < pcomp_last->CountPorts(); j++) {
      int peer_id = pcomp_last->PortPeerNwHlId(j);
      if (peer_id >= 0) {
        ORPCompHalfline* phalfline_peer = GetHalfline(static_cast<unsigned int>(peer_id));
        ORPComp* pcomp_peer = NULL;
        if (phalfline_peer) {
          pcomp_peer = phalfline_peer->ListORPcomps()->FrontNode();
        }

        if (pcomp_peer) {
          // update peer port : replace id
          if (pcomp_last->IsOutputPort(j))
            pcomp_peer->ReplacePortsId(static_cast<int>(phalflinedown->HlId()),
                static_cast<int>(phalflineup->HlId()),
                true, false);
          else
            pcomp_peer->ReplacePortsId(static_cast<int>(phalflinedown->HlId()),
                static_cast<int>(phalflineup->HlId()),
                false, true);
        }
      }
    }
  }

  // update compin port : replace id
  pcompin->ReplacePortsId(static_cast<int>(phalflinedown->HlId()),
                          static_cast<int>(phalflineup->HlId()),
                          false, true);

  if (phalflinedown->CountComponents() == 0) {
    RLOG_DEBUG("removing empty halfline hlid:%d (mh)\n", phalflinedown->HlId());
    int rr = mListHalflines.Remove(phalflinedown);
    if (rr) {
      RLOG_DEBUG("invalid remove %d\n", rr);
    }
    delete phalflinedown;
  }

  return 0;
}

}  // namespace

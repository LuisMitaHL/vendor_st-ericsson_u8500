/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ResEstimator.cpp
 * resource estimator class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "ResEstimator.h"
#include "ORPCompNetwork.h"
#include "PlatformLimits.h"
#include "PlatformDomains.h"
#include "SharedInst.h"

namespace rme {

/// checks if processing of resources can be done
const ResLevels* ResEstimator::CanProceedResources(const ORPComp *pcomp) {
  if (pcomp == NULL)
    return NULL;

  // resource state can be one of RM_RS_REQUIRED, RM_RS_SATISFIED, RM_RS_SUSPENDED
  if (pcomp->ResState() == RM_RS_NONE)
    return NULL;

  // note that for 'required' resources state, if initial resource check failed,
  // the required resources state is changed to none or satisfied (depending)
  // so no need to check required&loaded&inrequestphase=0&postrequestphase_granted=0

  // need valid resources estimation data
  if (pcomp->CFlag(ORPComp::CF_RESESTIM_VALID) == 0)
    return NULL;

  return pcomp->ResourcesEstimLevels();
}

/// scans active resources for visual mode
void ResEstimator::ScanActiveResourcesVisualMode(ResLevels *preslev) const {
  if (preslev == NULL)
    return;

  // scan ALL components, list is ordered: highest priority components come first
  // populate resources of various types until thresholds met
  // and mark candidates for preemption (full/partial)
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  for (const ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    // check resources & state validity
    // get stored resources estimation of current component
    const ResLevels *pcompreslevels = CanProceedResources(pcompiter);
    if (pcompreslevels == NULL)
      continue;

    // check components in satisfied state, even if going to be preempted,
    // in suspended or required states only if going to be granted resources,
    if ((pcompiter->ResState() == RM_RS_SATISFIED) ||
        ((pcompiter->CFlag(ORPComp::CF_POSTREQUESTPHASE_RESGRANTED)) ||
         (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESGRANT)))) {
      // simple aggregation
      preslev->mVisualFHD |= pcompreslevels->mVisualFHD;
    }
  }
}

/// scans and updates resources levels
/// and marks candidates for preemption (full/partial)
void ResEstimator::ScanResourcesMarkPreempts(ResLevels *preslev) const {
  if (preslev == NULL)
    return;

  // scan ALL components, list is ordered: highest priority components come first
  // populate resources of various types until thresholds met
  // and mark candidates for preemption (full/partial);
  // dont scan per resources then per ordered components,
  // but rather scan per ordered components then per resources, with dual pass:
  // if a given comp needs to be preempted for a given resource,
  // its resources usage for other resources shall not be accounted (thus need for dual pass);
  // this is done in order to prevent uneccessary preemption of lower priority components
  ORPCompNetwork* networkM = SharedInst::GetORPCompNetwInst();
  for (ORPComp *pcompiter = networkM->FirstComponent();
      pcompiter != NULL;
      pcompiter = networkM->NextComponent(pcompiter)) {
    // extended debug info
    RLOG_DEBUG("ScanRes comp[i:%d - r:%s] %s\n", pcompiter->Index(), pcompiter->Role(), pcompiter->DualStateORstr());
    if (pcompiter->NwHalflineId() >= 0) {
      RLOG_DEBUG("             hlid:%d plid:%d\n", pcompiter->NwHalflineId(), pcompiter->NwHalflinePlaceId());
      if ((pcompiter->NwHalfinePlaceType() & (1 << 0)) && (pcompiter->CountConnectionsOnPortIn())) {
        for (unsigned int j = 0; j < pcompiter->CountPorts(); j++) {
          if (pcompiter->IsInputPort(j)) {
            int peer_id = pcompiter->PortPeerNwHlId(j);
            if (peer_id >= 0) {
              RLOG_DEBUG("             connected to hlid:%d on input\n", peer_id);
            }
          }
        }
      }
      if ((pcompiter->NwHalfinePlaceType() & (1 << 1)) && (pcompiter->CountConnectionsOnPortOut()))  {
        for (unsigned int j = 0; j < pcompiter->CountPorts(); j++) {
          if (pcompiter->IsOutputPort(j)) {
            int peer_id = pcompiter->PortPeerNwHlId(j);
            if (peer_id >= 0) {
              RLOG_DEBUG("             connected to hlid:%d on output\n", peer_id);
            }
          }
        }
      }
    }
    RLOG_DEBUG("             gprio:%d gid:%d drg:%d\n", pcompiter->PolGroupPriorityClient(), pcompiter->PolGroupId(), pcompiter->DateResourceGrant());
#ifndef SW_VARIANT_ANDROID
    RLOG_DEBUG("             pid:%d tid:%d\n", static_cast<int>(pcompiter->PolProcessId()), static_cast<int>(pcompiter->PolThreadId()));
#endif

    // DUAL pass
    // 1st pass: check if adding the component would lead to overflow / incompatibilities,
    // thus needing to mark it as candidate for preemption (full/partial);
    // => check done for ALL resources types
    // 2nd pass: update resources levels if component is not candidate for preemption

    // MCPS, ESRAM-scratch, and ESRAM XP70/ISP:
    // accounted for only if component has active resources / requests
    // =>in SUSPEND, MCPS are supposed to be released/not used;
    // yet, for resources attribution or re-attribution, resources for component
    // in required and suspended states are added-up;

    // check resources & state validity
    // get stored resources estimation of current component
    const ResLevels *pcompreslevels = CanProceedResources(pcompiter);
    if (pcompreslevels == NULL)
      continue;

    // ***FIRST PASS: check for potential resources conflict
    // that the current component would bring for ALL resources types***
    int status = IterateScanResTestCheck(preslev, pcompreslevels);

    // mark candidate for preemption
    if (status == -1) {
      pcompiter->SetCFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL);
    } else if (status == -2) {
      pcompiter->SetCFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL);
    }

    // ***SECOND PASS: if not preemption candidate, then update aggregated resources levels***
    if ((pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_FULL) == 0) &&
        (pcompiter->CFlag(ORPComp::CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL) == 0)) {
      IterateScanResUpdate(preslev, pcompreslevels);
    }
  }
}

/// iterates resources scan : tests candidate for preemption checking resources levels
/// returned status; 0:ok, -1:partial preemption, -2:total preemption
int ResEstimator::IterateScanResTestCheck(const ResLevels *preslev,
                                          const ResLevels *pcompreslevels) const {
  if (preslev == NULL)
    return 0;
  if (pcompreslevels == NULL)
    return 0;

  // status; 0:ok, -1:partial preemption, -2 total preemption
  int status = 0;

  // SVA MCPS
  if (preslev->mSvaLevels.TestAddCheckOverflowMcps(&pcompreslevels->mSvaLevels,
        PlatformLimits::MMDSP_SVA_MCPS) < 0) {
    RLOG_DEBUG("SVA MCPS test-overflow positive: mark comp for suspension\n");
    status = -1;
  }

  // SIA-Visual MCPS
  if (preslev->mSiaLevelsVisual.TestAddCheckOverflowMcps(&pcompreslevels->mSiaLevelsVisual,
        PlatformLimits::MMDSP_SIA_MCPS_VISUAL) < 0) {
    RLOG_DEBUG("SIA-Visual MCPS test-overflow positive: mark comp for suspension\n");
    status = -1;
  }

  // SIA-Audio MCPS
  if (preslev->mSiaLevelsAudio.TestAddCheckOverflowMcps(&pcompreslevels->mSiaLevelsAudio,
        PlatformLimits::MMDSP_SIA_MCPS_AUDIO) < 0) {
    RLOG_DEBUG("SIA-Audio MCPS test-overflow positive: mark comp for suspension\n");
    status = -1;
  }

  // ESRAM scratch SxA hwpipes domains: control on domains overlap
  if (preslev->mEsramLevels.TestAddCheckOverlapSxaHwpEsramScratch(&pcompreslevels->mEsramLevels,
        NULL, NULL) < 0) {
    RLOG_DEBUG("ESRAM scratch hwp buffer test-conflict positive: mark comp for suspension\n");
    status = -1;
  }

  // ESRAM NON scratch XP70 SIA domain: no overlap and no overflow control in this case

  return status;
}

/// iterates resources scan : updates resources levels
void ResEstimator::IterateScanResUpdate(ResLevels *preslev,
                                        const ResLevels *pcompreslevels) const {
  if (preslev == NULL)
    return;
  if (pcompreslevels == NULL)
    return;

  // SVA MCPS
  preslev->mSvaLevels.AddCheckOverflowMcps(&pcompreslevels->mSvaLevels,
      PlatformLimits::MMDSP_SVA_MCPS);

  // SIA-Visual MCPS
  preslev->mSiaLevelsVisual.AddCheckOverflowMcps(&pcompreslevels->mSiaLevelsVisual,
      PlatformLimits::MMDSP_SIA_MCPS_VISUAL);

  // SIA-Audio MCPS
  preslev->mSiaLevelsAudio.AddCheckOverflowMcps(&pcompreslevels->mSiaLevelsAudio,
      PlatformLimits::MMDSP_SIA_MCPS_AUDIO);

  // ESRAM scratch SxA hwpipes domains update
  preslev->mEsramLevels.AddCheckOverlapSxaHwpEsramScratch(&pcompreslevels->mEsramLevels);

  // ESRAM NON scratch XP70 SIA / ISP domain update
  preslev->mEsramLevels.SiaHwxp70AddNoCheck(&pcompreslevels->mEsramLevels);
}

/// checks overlaps between ALL ESRAM domains
void ResEstimator::CheckAllEsramOverlaps() {
  unsigned int pndstart = PlatformDomains::PND_NA;
  for (unsigned int i = PlatformDomains::PND_NA; i < PlatformDomains::PND_COUNT; i++) {
    if (PfDomManager::IsPndValid(static_cast<PlatformDomains::PNDS_E>(i)) == false)
      continue;
    for (unsigned int j = pndstart; j < PlatformDomains::PND_COUNT; j++) {
      if (PfDomManager::IsPndValid(static_cast<PlatformDomains::PNDS_E>(j)) == false)
        continue;

      if (j == i)
        continue;

      // no need to get status; on overlap, dump done automatically
      PfDomManager::CheckEsramOverlap(
          static_cast<PlatformDomains::PNDS_E>(i),
          static_cast<PlatformDomains::PNDS_E>(j));
    }

    pndstart = i+1;
  }
}

}  // namespace

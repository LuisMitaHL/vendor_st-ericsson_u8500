/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file NmfDomains.cpp
 * NMF Domains class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "NmfDomains.h"

#include <cm/proxy/api/domain_proxy.h>

namespace rme {

/// creates NMF memory domain associated to provided descriptor
/// and sets domain id on success
int NmfDomains::CreateDomain(PlatformDomains::PNDS_E pnd,
                             unsigned int *pdomid,
                             bool createforclient, uint64_t clientpid) {
  const PlatformDomains::DomainDesc *pdomdesc =  PfDomManager::GetDomainDesc(pnd);
  if (pdomdesc == NULL) {
    RLOG_ERROR("CreateMemDom: invalid pnd:%d\n", pnd);
    return -1;
  }
  if (pdomid == NULL)
    return -1;

  t_cm_domain_memory ndmem;

  // core id
  switch (pdomdesc->type) {
    case PlatformDomains::DHM_HWPIPESVA:
    case PlatformDomains::DHM_PROCSVA:
      ndmem.coreId = SVA_CORE_ID;
      break;
    case PlatformDomains::DHM_HWPIPESIA:
    case PlatformDomains::DHM_HWXP70SIA:
    case PlatformDomains::DHM_PROCSIA:
      ndmem.coreId = SIA_CORE_ID;
      break;
    case PlatformDomains::DHM_NA:
    default:
      ndmem.coreId = ARM_CORE_ID;
      break;
  }

  // offsets and sizes for esram and sdram
  if (pdomdesc->scratchid > 0) {
    // scratch domain
    // *no* code section for scratch domains
    // sdram
    memset(&ndmem.sdramCode, 0, sizeof(t_cm_domain_segment));
    ndmem.sdramData.offset = pdomdesc->sdram.offset;
    ndmem.sdramData.size   = pdomdesc->sdram.size;
    // esram
    memset(&ndmem.esramCode, 0, sizeof(t_cm_domain_segment));
    ndmem.esramData.offset = pdomdesc->esram.offset;
    ndmem.esramData.size   = pdomdesc->esram.size;
  } else {
    // NON scratch domain
    // overlay code and data for both sdram and esram
    // sdram
    ndmem.sdramCode.offset = pdomdesc->sdram.offset;
    ndmem.sdramCode.size   = pdomdesc->sdram.size;
    ndmem.sdramData.offset = pdomdesc->sdram.offset;
    ndmem.sdramData.size   = pdomdesc->sdram.size;
    // esram
    ndmem.esramCode.offset = pdomdesc->esram.offset;
    ndmem.esramCode.size   = pdomdesc->esram.size;
    ndmem.esramData.offset = pdomdesc->esram.offset;
    ndmem.esramData.size   = pdomdesc->esram.size;
  }

  // create domain
  t_cm_error status = CM_OK;
  t_cm_domain_id ndid = 0;

#ifndef HOST_ONLY  // for platform x86 without CM

  // create domain
  if (pdomdesc->scratchid > 0) {
    // scratch domain
    // look for pnd parent
    PlatformDomains::PNDS_E pnd_parent = PfDomManager::GetDomainParentPnd(pdomdesc->scratchid);
    if (PfDomManager::IsPndValid(pnd_parent) == false) {
      RLOG_ERROR("CM_CreateMemDom s:%d '%s_%s': no parent found\n", pdomdesc->scratchid, PfDomManager::GetRootName(pdomdesc->pnd), PfDomManager::GetUsecaseName(pdomdesc->pnd));
      return -1;
    } else {
      // scratch domain creation
      // parent domain id is mTableNsPndDomainIds[pnd_parent]
      status = CM_CreateMemoryDomainScratch(mTableNsPndDomainIds[pnd_parent],
          &ndmem, &ndid);
    }
  } else if (pdomdesc->scratchid < 0) {
    // non scratch domain: scratch parent domain case => always owned by RME
    status = CM_CreateMemoryDomain(&ndmem, &ndid);
  } else {
    // non scratch domain: non scratch parent domain case
    if (createforclient == false) {
      status = CM_CreateMemoryDomain(&ndmem, &ndid);
    } else {
#ifndef NOCREATEDOMAINFORCLIENT
      // Create Domain *per process*
      int indexpiddomain = GetIndexPidDomain(clientpid, pnd);
      if ((indexpiddomain < 0) || (static_cast<unsigned int>(indexpiddomain) >= msMAXCountPidDomains)) {
        // not yet referenced: can create the domain (if enough index)
        indexpiddomain = GetNewIndexPidDomain();
        if ((indexpiddomain < 0) || (static_cast<unsigned int>(indexpiddomain) >= msMAXCountPidDomains)) {
          RLOG_ERROR("'%s_%s': no more index available for CM_CreateMemoryDomainForClient\n", PfDomManager::GetRootName(pdomdesc->pnd), PfDomManager::GetUsecaseName(pdomdesc->pnd));
          return -1;
        }
        status = CM_CreateMemoryDomainForClient(&ndmem, static_cast<t_nmf_client_id>(clientpid), &ndid);
        if (status == CM_OK) {
          // setup reference count + domain id
          mTablePidDomainIds[indexpiddomain].pid = clientpid;
          mTablePidDomainIds[indexpiddomain].pnd = pnd;
          mTablePidDomainIds[indexpiddomain].domid =  static_cast<unsigned int>(ndid);
          mTablePidDomainIds[indexpiddomain].refcount = 1;
          *pdomid = mTablePidDomainIds[indexpiddomain].domid;
          RLOG_DEBUG("CM_CreateMemoryDomainForClient '%s_%s' - dom-id:%u\n", PfDomManager::GetRootName(pdomdesc->pnd), PfDomManager::GetUsecaseName(pdomdesc->pnd), *pdomid);
          return 0;
        } else {
          RLOG_ERROR("CM_CreateMemoryDomainForClient s:%d '%s_%s': CMstatus:%d\n", pdomdesc->scratchid, PfDomManager::GetRootName(pdomdesc->pnd), PfDomManager::GetUsecaseName(pdomdesc->pnd), status);
          return -1;
        }
      } else {
        // index available: get domain id + update reference counter
        mTablePidDomainIds[indexpiddomain].refcount += 1;
        *pdomid = mTablePidDomainIds[indexpiddomain].domid;
        RLOG_DEBUG("CM_CreateMemoryDomainForClient '%s_%s' - dom-id:%u reused refcount:%d\n", PfDomManager::GetRootName(pdomdesc->pnd), PfDomManager::GetUsecaseName(pdomdesc->pnd), *pdomid, mTablePidDomainIds[indexpiddomain].refcount);
        return 0;
      }
#else
      status = CM_INVALID_PARAMETER;
#endif
    }
  }

#endif

  if (status != CM_OK) {
    RLOG_ERROR("CM_CreateMemDom s:%d '%s_%s': CMstatus:%d\n", pdomdesc->scratchid, PfDomManager::GetRootName(pdomdesc->pnd), PfDomManager::GetUsecaseName(pdomdesc->pnd), status);
    return -1;
  } else {
    RLOG_DEBUG("CM_CreateMemDom s:%d '%s_%s' - dom-id:%u\n", pdomdesc->scratchid, PfDomManager::GetRootName(pdomdesc->pnd), PfDomManager::GetUsecaseName(pdomdesc->pnd), static_cast<unsigned int>(ndid));
    *pdomid = static_cast<unsigned int>(ndid);
    return 0;
  }
}

/// destroys NMF memory domain associated to provided descriptor and domain id
void NmfDomains::DestroyDomain(PlatformDomains::PNDS_E pnd,
                               unsigned int domid) {
  if (domid == 0)
    return;

#ifndef NOCREATEDOMAINFORCLIENT
  for(unsigned int i = 0; i < msMAXCountPidDomains; i++) {
    if ((mTablePidDomainIds[i].domid == domid) && (mTablePidDomainIds[i].pnd == pnd)) {
      // this domain was created through CM_CreateMemoryDomainForClient => unref
      if (mTablePidDomainIds[i].refcount > 0) {
        mTablePidDomainIds[i].refcount -= 1;
      }
      if (mTablePidDomainIds[i].refcount > 0) {
        // don't actually destroy domain now; only done once refcount is 0
        RLOG_DEBUG("CM_DestroyMemDom dom-id:%u skipped; new refcount:%d\n", domid, mTablePidDomainIds[i].refcount);
        return;
      }
      break;
    }
  }
#endif

#ifndef HOST_ONLY  // for platform x86 without CM

  t_cm_error status = CM_DestroyMemoryDomain(domid);
  if (status != CM_OK) {
    if (PfDomManager::IsPndValid(pnd)) {
      RLOG_ERROR("CM_DestroyMemDom %u '%s_%s': CMstatus:%d\n", domid, PfDomManager::GetRootName(pnd), PfDomManager::GetUsecaseName(pnd), status);
    } else {
      RLOG_ERROR("CM_DestroyMemDom %u: invalid pnd:%d - CMstatus:%d\n", domid, pnd, status);
    }
  } else {
    if (PfDomManager::IsPndValid(pnd)) {
      RLOG_DEBUG("CM_DestroyMemDom dom-id:%u '%s_%s'\n", domid, PfDomManager::GetRootName(pnd), PfDomManager::GetUsecaseName(pnd));
    } else {
      RLOG_DEBUG("CM_DestroyMemDom dom-id:%u (bare)\n", domid);
    }
  }

#endif
}

/// creates all NMF memory domains associated to 'non scratch' platform named domain
int NmfDomains::CreateAllNonScratchPlatformDomains() {
  int status = 0;

  // create all NMF domains that have an associated descriptor in domains table
  for (unsigned int i = PlatformDomains::PND_NA; i < PlatformDomains::PND_COUNT; i++) {
    PlatformDomains::PNDS_E pnd = static_cast<PlatformDomains::PNDS_E>(i);
    if (PfDomManager::GetDomainDesc(pnd) == NULL) {
        continue;
    }

    const PlatformDomains::DomainDesc *pdomdesc =  PfDomManager::GetDomainDesc(pnd);
    if (pdomdesc) {
        RLOG_DEBUG("'%s_%s' ESRAM offset:%d ESRAM size:%d\n", PfDomManager::GetRootName(pnd), PfDomManager::GetUsecaseName(pnd), pdomdesc->esram.offset, pdomdesc->esram.size);
    }

    // discard scratch domains
    if (PfDomManager::GetScratchId(pnd) > 0) {
      RLOG_DEBUG("skipped '%s_%s' (scratch runtime creation)\n", PfDomManager::GetRootName(pnd), PfDomManager::GetUsecaseName(pnd));
      continue;
    }

#ifndef NOCREATEDOMAINFORCLIENT
#ifdef NODEFAULTDOMAIN
    // discard non parents non-scratch domains;
    if (PfDomManager::GetScratchId(pnd) == 0) {
      RLOG_DEBUG("skipped '%s_%s' (non-scratch runtime creation)\n", PfDomManager::GetRootName(pnd), PfDomManager::GetUsecaseName(pnd));
      continue;
    }
#endif
#endif

    // created once only
    if (mTableNsPndDomainIds[pnd] != 0) {
      RLOG_WARNING("trying to create several times '%s_%s'\n", PfDomManager::GetRootName(pnd), PfDomManager::GetUsecaseName(pnd));
      continue;
    }

    // domain creation
    unsigned int domid = 0;
    int cndstatus = CreateDomain(pnd, &domid, false, 0);
    if (cndstatus < 0) {
      // sticky failure status
      status = -1;
      // keep going anyway..
    } else {
      mTableNsPndDomainIds[pnd] = static_cast<unsigned int>(domid);
    }
  }

  return status;
}

#ifndef NOCREATEDOMAINFORCLIENT
int NmfDomains::GetIndexPidDomain(uint64_t pid, PlatformDomains::PNDS_E pnd) {
  for(unsigned int i = 0; i < msMAXCountPidDomains; i++) {
    if ((mTablePidDomainIds[i].pid == pid) && (mTablePidDomainIds[i].pnd == pnd)) {
      return i;
    }
  }
  return -1;
}

/// return new index for PidDomain entry; returns -1 if not found
int NmfDomains::GetNewIndexPidDomain(void) {
  for(unsigned int i = 0; i < msMAXCountPidDomains; i++) {
    if (mTablePidDomainIds[i].refcount == 0) {
      return i;
    }
  }
return -1;
}
#endif

}  // namespace


// ################################################################################
// ################################################################################
// ################################################################################
#ifdef __RMEUNITARYDEV__
// this is *STUB* code *ONLY* for RME unit development & tests
static t_cm_domain_id did = 101;  // dont ask why this number!
t_cm_error CM_CreateMemoryDomain(const t_cm_domain_memory *domain,
                                  t_cm_domain_id *handle) {
  if ((domain == NULL)||(handle == NULL))
    return CM_INVALID_PARAMETER;
  *handle = did++;
  return CM_OK;
}
t_cm_error CM_CreateMemoryDomainForClient(
        const t_cm_domain_memory *domain,
        t_nmf_client_id client,
        t_cm_domain_id *handle
        ) {
  if ((domain == NULL)||(handle == NULL))
    return CM_INVALID_PARAMETER;
  if (client) {
  }
  *handle = did++;
  return CM_OK;
}
t_cm_error CM_CreateMemoryDomainScratch(const t_cm_domain_id parentId,
                                        const t_cm_domain_memory *domain,
                                        t_cm_domain_id *handle) {
  if ((parentId == 0)||(domain == NULL)||(handle == NULL))
    return CM_INVALID_PARAMETER;
  *handle = did++;
  return CM_OK;
}
t_cm_error CM_DestroyMemoryDomain(t_cm_domain_id handle) {
  if (handle == 0)
    return CM_INVALID_PARAMETER;
  return CM_OK;
}
#endif

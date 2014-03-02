/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file PfDomManager.cpp
 * Platform domains manager class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "uosal.h"
#define _PLATFORMDOMAINSINIT_CPP_
#include "PfDomManager.h"
#undef _PLATFORMDOMAINSINIT_CPP_

namespace rme {

// inits
bool PfDomManager::mDoneMapFidPndInits = false;
PlatformDomains::PNDS_E PfDomManager::mMapFidPndHwpSva[] = {PlatformDomains::PND_NA};
PlatformDomains::PNDS_E PfDomManager::mMapFidPndHwpSia[] = {PlatformDomains::PND_NA};
PlatformDomains::PNDS_E PfDomManager::mMapFidPndHwxSia[] = {PlatformDomains::PND_NA};

/// checks if pnd is user valid
bool PfDomManager::IsPndValid(PlatformDomains::PNDS_E pnd) {
  if ((pnd <= PlatformDomains::PND_NA) ||
      (pnd >= PlatformDomains::PND_COUNT)) {
    return false;
  } else {
    return true;
  }
}

/// returns domain descriptor associated to a PND
const struct PlatformDomains::DomainDesc*
PfDomManager::GetDomainDesc(PlatformDomains::PNDS_E pnd) {
  if (IsPndValid(pnd) == false)
    return NULL;
  for (unsigned int j = PlatformDomains::PND_NA; j < PlatformDomains::PND_COUNT; j++) {
    if (PlatformDomains::msDomains[j].pnd == pnd) {
      // found: assuming unicity of pnd in msDomains
      return &PlatformDomains::msDomains[j];
    }
  }
  return NULL;
}

/// returns root name of a domain associated to a PND
const char* PfDomManager::GetRootName(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc == NULL) {
    return "na";
  }

  if (pdomaindesc->sdram.size && (pdomaindesc->esram.size == 0)) {
    // 'pure' DDR case
    switch (pdomaindesc->type) {
      case PlatformDomains::DHM_NA:      return "PND_PROCARM_DDR";
      case PlatformDomains::DHM_PROCSVA: return "PND_PROCSVA_DDR";
      case PlatformDomains::DHM_PROCSIA: return "PND_PROCSIA_DDR";
      default: return "wrongly-typed_PND_DDR";
    }
  } else if (pdomaindesc->sdram.size && pdomaindesc->esram.size) {
    //  'mixed' case
    // forbid scratch or scratch-parent esram domains
    if (pdomaindesc->scratchid) {
      return "invalid_scratch_mixed_PND_DDRESRAM";
    }
    switch (pdomaindesc->type) {
      case PlatformDomains::DHM_PROCSVA: return "PND_PROCSVA_DDRESRAM";
      case PlatformDomains::DHM_PROCSIA: return "PND_PROCSIA_DDRESRAM";
      default: return "wrongly-typed_PND-DDRESRAM";
    }
  } else if ((pdomaindesc->sdram.size == 0) && pdomaindesc->esram.size) {
    // 'pure' ESRAM case
    if ((pdomaindesc->esramfid < RM_ESBUFID_EMPTY) || (pdomaindesc->esramfid >= RM_ESBUFID_LAST)) {
      return "invalid_scratchfid_PND_ESRAM";
    }

    if ((pdomaindesc->scratchid < 0) && (pdomaindesc->esramfid == RM_ESBUFID_EMPTY)) {
      // scratch-parent domain
      switch (pdomaindesc->type) {
        case PlatformDomains::DHM_HWPIPESVA: return "PND_HWPIPESVA_ESRAM";
        case PlatformDomains::DHM_HWPIPESIA: return "PND_HWPIPESIA_ESRAM";
        case PlatformDomains::DHM_HWXP70SIA: return "PND_HWXP70SIA_ESRAM";
        default: return "wrongly-typedp_PND_ESRAM";
      }
    } else {
      // scratch or non scratch
      switch (pdomaindesc->type) {
        case PlatformDomains::DHM_HWPIPESVA:
          switch (pdomaindesc->esramfid) {
            case RM_ESBUFID_DEFAULT:  return "PND_HWPIPESVA_ESRAM_DEFAULT";
            case RM_ESBUFID_SMALL:    return "PND_HWPIPESVA_ESRAM_SMALL";
            case RM_ESBUFID_BIG:      return "PND_HWPIPESVA_ESRAM_BIG";
            case RM_ESBUFID_BIGGEST:  return "PND_HWPIPESVA_ESRAM_BIGGEST";
            default: return "invalid PND-ESRAM-HWPIPESVA";
          }
        case PlatformDomains::DHM_HWPIPESIA:
          switch (pdomaindesc->esramfid) {
            case RM_ESBUFID_DEFAULT:  return "PND_HWPIPESIA_ESRAM_DEFAULT";
            case RM_ESBUFID_SMALL:    return "PND_HWPIPESIA_ESRAM_SMALL";
            case RM_ESBUFID_BIG:      return "PND_HWPIPESIA_ESRAM_BIG";
            case RM_ESBUFID_BIGGEST:  return "PND_HWPIPESIA_ESRAM_BIGGEST";
            default: return "invalid PND-ESRAM-HWPIPESIA";
          }
        case PlatformDomains::DHM_HWXP70SIA:
          switch (pdomaindesc->esramfid) {
            case RM_ESBUFID_DEFAULT:  return "PND_HWXP70SIA_ESRAM_DEFAULT";
            case RM_ESBUFID_SMALL:    return "PND_HWXP70SIA_ESRAM_SMALL";
            case RM_ESBUFID_BIG:      return "PND_HWXP70SIA_ESRAM_BIG";
            case RM_ESBUFID_BIGGEST:  return "PND_HWXP70SIA_ESRAM_BIGGEST";
            default: return "invalid PND-ESRAM-HWXP70SIA";
          }
        default: return "wrongly-typed_PND_ESRAM";
      }
    }
  } else {
    //  'empty' case
    return "empty_PND";
  }
}

/// returns usecase name of a domain associated to a PND
const char* PfDomManager::GetUsecaseName(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc)
    return pdomaindesc->usecasename;
  else
    return "na";
}

/// returns RME domain type of a domain associated to a PND
PlatformDomains::DHM_E PfDomManager::GetDomainType(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc)
    return pdomaindesc->type;
  else
    return PlatformDomains::DHM_NA;
}

/// returns scratchid of a domain associated to a PND
int PfDomManager::GetScratchId(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc)
    return pdomaindesc->scratchid;
  else
    return 0;
}

/// returns ESRAM fast id of a domain associated to a PND
RM_ESRAMBUFID_E PfDomManager::GetEsramFastId(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc)
    return pdomaindesc->esramfid;
  else
    return RM_ESBUFID_EMPTY;
}
/// returns ESRAM size of a domain associated to a PND
unsigned int PfDomManager::GetDomainEsramSize(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc)
    return pdomaindesc->esram.size;
  else
    return 0;
}

/// returns ESRAM offset of a domain associated to a PND
unsigned int PfDomManager::GetDomainEsramOffset(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc)
    return pdomaindesc->esram.offset;
  else
    return 0;
}

/// returns SDRAM size of a domain associated to a PND
unsigned int PfDomManager::GetDomainSdramSize(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc)
    return pdomaindesc->sdram.size;
  else
    return 0;
}

/// returns SDRAM offset of a domain associated to a PND
unsigned int PfDomManager::GetDomainSdramOffset(PlatformDomains::PNDS_E pnd) {
  const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
  if (pdomaindesc)
    return pdomaindesc->sdram.offset;
  else
    return 0;
}

/// returns PND of parent domain associated to a scratch id > 0
PlatformDomains::PNDS_E PfDomManager::GetDomainParentPnd(int scratchid) {
  if (scratchid <= 0)
    return PlatformDomains::PND_NA;
  for (unsigned int j = PlatformDomains::PND_NA; j < PlatformDomains::PND_COUNT; j++) {
    if (PlatformDomains::msDomains[j].scratchid == -scratchid) {
      // found: assuming unicity of parent scratchid in msDomains
      return PlatformDomains::msDomains[j].pnd;
    }
  }
  return PlatformDomains::PND_NA;
}

/// checks if domains overlap on ESRAM (scratch or non scratch)
/// returns
/// -1 or -2:overlap, 0:no overlap, 1 or 2:controlled overlap (scratch domains / parent)
int PfDomManager::CheckEsramOverlap(PlatformDomains::PNDS_E pnd1, PlatformDomains::PNDS_E pnd2) {
  const struct PlatformDomains::DomainDesc *pdomaindesc1 = NULL;
  const struct PlatformDomains::DomainDesc *pdomaindesc2 = NULL;

  // skip self comparaison
  if (pnd1 == pnd2)
    return 0;

  // get descriptors
  for (unsigned int j = PlatformDomains::PND_NA; j < PlatformDomains::PND_COUNT; j++) {
    if (PlatformDomains::msDomains[j].pnd == pnd1) {
      // found: assuming unicity of pnd in msDomains
      pdomaindesc1 = &PlatformDomains::msDomains[j];
    }
    if (PlatformDomains::msDomains[j].pnd == pnd2) {
      // found: assuming unicity of pnd in msDomains
      pdomaindesc2 = &PlatformDomains::msDomains[j];
    }
  }

  if ((pdomaindesc1 == NULL) || (pdomaindesc2 == NULL)) {
    return 0;
  }

  // compare esram
  if ((pdomaindesc1->esram.size == 0) ||
      (pdomaindesc2->esram.size == 0)) {
    return 0;
  }

  if ((pdomaindesc1->scratchid == pdomaindesc2->scratchid) &&
      (pdomaindesc1->scratchid > 0)) {
    // domains are scratch with same parent
    // dont even care to check values: scratch overlap
    return 1;
  } else if ((pdomaindesc1->scratchid == -pdomaindesc2->scratchid) &&
      (pdomaindesc1->scratchid != 0)) {
    // one of the domains is parent of the other 'scratch' domain
    const struct PlatformDomains::DomainDesc *pdomaindescparent = NULL;
    const struct PlatformDomains::DomainDesc *pdomaindescscratch = NULL;
    if (pdomaindesc1->scratchid < 0) {
      pdomaindescparent = pdomaindesc1;
      pdomaindescscratch = pdomaindesc2;
    } else {
      pdomaindescparent = pdomaindesc2;
      pdomaindescscratch = pdomaindesc1;
    }
    // check that scratch domain is inside parent domain
    unsigned int firstaddP = pdomaindescparent->esram.offset;
    unsigned int lastaddP = pdomaindescparent->esram.offset + pdomaindescparent->esram.size - 1;
    unsigned int firstaddS = pdomaindescscratch->esram.offset;
    unsigned int lastaddS = pdomaindescscratch->esram.offset + pdomaindescscratch->esram.size - 1;
    if ((firstaddS < firstaddP) || (lastaddS > lastaddP)) {
      RLOG_WARNING("invalid ESRAM-overlap between {'%s_%s' / '%s_%s'} scratch vs parent\n", GetRootName(pnd1), GetUsecaseName(pnd1), GetRootName(pnd2), GetUsecaseName(pnd2));
      return -2;
    } else {
      return 2;
    }
  } else {
    unsigned int firstadd1 = pdomaindesc1->esram.offset;
    unsigned int lastadd1 = pdomaindesc1->esram.offset + pdomaindesc1->esram.size - 1;
    unsigned int firstadd2 = pdomaindesc2->esram.offset;
    unsigned int lastadd2 = pdomaindesc2->esram.offset + pdomaindesc2->esram.size - 1;
    if (((firstadd1 >= firstadd2) && (firstadd1 <= lastadd2)) ||
        ( (lastadd1 >= firstadd2) &&  (lastadd1 <= lastadd2))   ||
        ((firstadd2 >= firstadd1) && (firstadd2 <= lastadd1)) ||
        ( (lastadd2 >= firstadd1) &&  (lastadd2 <= lastadd1))) {
      RLOG_DEBUG("ESRAM-overlap between {'%s_%s' / '%s_%s'}\n", GetRootName(pnd1), GetUsecaseName(pnd1), GetRootName(pnd2), GetUsecaseName(pnd2));
      return -1;
    }
  }
  return 0;
}

/// performs mapping table inits
int PfDomManager::InitMappingFidPndTable(PlatformDomains::PNDS_E mapfidpnd[RM_ESBUFID_LAST],
                                         PlatformDomains::DHM_E type) {
  if (mapfidpnd == NULL)
    return -1;
  if ((type != PlatformDomains::DHM_HWPIPESVA) &&
      (type != PlatformDomains::DHM_HWPIPESIA) &&
      (type != PlatformDomains::DHM_HWXP70SIA))
    return -1;

  for (int j = 0; j < RM_ESBUFID_LAST; j++) {
    mapfidpnd[j] = PlatformDomains::PND_NA;
  }

  for (int i = PlatformDomains::PND_NA; i < PlatformDomains::PND_COUNT; i++) {
    PlatformDomains::PNDS_E pnd = static_cast<PlatformDomains::PNDS_E>(i);
    const struct PlatformDomains::DomainDesc *pdomaindesc = GetDomainDesc(pnd);
    if (pdomaindesc == NULL)
      continue;
    // want ESRAM based domains
    if (pdomaindesc->esram.size == 0)
      continue;
    // want given type
    if (pdomaindesc->type != type)
      continue;
    // skip parent domains: want non-scratch or purely-scratch domains
    if (pdomaindesc->scratchid < 0)
      continue;
    // want valid ESRAM fast id
    if ((pdomaindesc->esramfid <= RM_ESBUFID_EMPTY) || (pdomaindesc->esramfid >= RM_ESBUFID_LAST))
      continue;

    if (mapfidpnd[static_cast<int>(pdomaindesc->esramfid)] == PlatformDomains::PND_NA) {
      mapfidpnd[static_cast<int>(pdomaindesc->esramfid)] = pnd;
    } else {
      RLOG_WARNING("duplicate ESRAM domain for type:%s %d\n", StringifyHwType(type), static_cast<int>(pdomaindesc->esramfid));
      return -1;
    }
  }
  return 0;
}

/// transforms ESRAM buffer size into fast id
int PfDomManager::SetEsramBufferFastId(RM_ESRAMBUFID_E *pEsramBufId, PlatformDomains::DHM_E type) {
  if (pEsramBufId == NULL)
    return -1;
  if (*pEsramBufId == 0) {
    // size 0 corresponds to RM_ESBUFID_EMPTY => processed immediately
    *pEsramBufId = RM_ESBUFID_EMPTY;
    return 0;
  }

  // init mapping table ESRAM fast ids / pnds for ESRAM HW domains with given HW type
  // done once only (if success)
  if (mDoneMapFidPndInits == false) {
    if (InitMappingFidPndTable(mMapFidPndHwpSva, PlatformDomains::DHM_HWPIPESVA) < 0)
      return -1;
    if (InitMappingFidPndTable(mMapFidPndHwpSia, PlatformDomains::DHM_HWPIPESIA) < 0)
      return -1;
    if (InitMappingFidPndTable(mMapFidPndHwxSia, PlatformDomains::DHM_HWXP70SIA) < 0)
      return -1;
    mDoneMapFidPndInits = true;
  }

  const PlatformDomains::PNDS_E *mapfidpnd;
  if (type == PlatformDomains::DHM_HWPIPESVA)
    mapfidpnd = mMapFidPndHwpSva;
  else if (type == PlatformDomains::DHM_HWPIPESIA)
    mapfidpnd = mMapFidPndHwpSia;
  else if (type == PlatformDomains::DHM_HWXP70SIA)
    mapfidpnd = mMapFidPndHwxSia;
  else
    return -1;

  // check provided value: might either be ESRAM fast id to be checked
  // or size to be transformed in ESRAM fast id

  if ((*pEsramBufId > RM_ESBUFID_EMPTY) && (*pEsramBufId < RM_ESBUFID_LAST)) {
    // pEsramBufId already using fast id format: make checks depending on type
    if (mapfidpnd[*pEsramBufId] == PlatformDomains::PND_NA) {
      // no reference for this fast id in platform domain table found => invalid fast id
      RLOG_WARNING("invalid ESRAM fastid:%s for type:%s\n", StringifyEsramFid(*pEsramBufId), StringifyHwType(type));
      return -1;
    }
  } else  {
    // pEsramBufId based on size => check size / attribute fast id
    // look for smallest ESRAM domain fitting required size
    // or alternative choice (TBC)
    // attribute domain based on this size, just look for adequate parent domain ?
    unsigned int esramsize = static_cast<unsigned int>(*pEsramBufId);
    bool gotdomain = false;
    for (int j = 0; j < RM_ESBUFID_LAST; j++) {
      if (mapfidpnd[j] != PlatformDomains::PND_NA) {
        unsigned int itersize = GetDomainEsramSize(mapfidpnd[j]);
        if (esramsize <= itersize) {
          *pEsramBufId = static_cast<RM_ESRAMBUFID_E>(j);
          gotdomain = true;
          // stop on first match: since scanning from smallest to biggest domain
          break;
        }
      }
    }
    if (gotdomain == false) {
      RLOG_WARNING("invalid size:%u for ESRAM domain for type:%s\n", esramsize, StringifyHwType(type));
      return -1;
    }
  }

  return 0;
}

/// returns PND associated to ESRAM fast identification for a given HW type
PlatformDomains::PNDS_E PfDomManager::GetPndAssociatedToEsramFidHwtyp(
    RM_ESRAMBUFID_E esramfid, PlatformDomains::DHM_E hwtype) {
  // init mapping table ESRAM fast ids / pnds for ESRAM HW domains with given HW type
  // done once only (if success)
  if (mDoneMapFidPndInits == false) {
    if (InitMappingFidPndTable(mMapFidPndHwpSva, PlatformDomains::DHM_HWPIPESVA) < 0)
      return PlatformDomains::PND_NA;
    if (InitMappingFidPndTable(mMapFidPndHwpSia, PlatformDomains::DHM_HWPIPESIA) < 0)
      return PlatformDomains::PND_NA;
    if (InitMappingFidPndTable(mMapFidPndHwxSia, PlatformDomains::DHM_HWXP70SIA) < 0)
      return PlatformDomains::PND_NA;
    mDoneMapFidPndInits = true;
  }

  const PlatformDomains::PNDS_E *mapfidpnd;
  if (hwtype == PlatformDomains::DHM_HWPIPESVA) {
    mapfidpnd = mMapFidPndHwpSva;
  } else if (hwtype == PlatformDomains::DHM_HWPIPESIA) {
    mapfidpnd = mMapFidPndHwpSia;
  } else if (hwtype == PlatformDomains::DHM_HWXP70SIA) {
    mapfidpnd = mMapFidPndHwxSia;
  } else {
    return PlatformDomains::PND_NA;
  }

  if ((esramfid >= RM_ESBUFID_EMPTY) && (esramfid < RM_ESBUFID_LAST)) {
    return mapfidpnd[esramfid];
  } else {
    return PlatformDomains::PND_NA;
  }
}

}  // namespace


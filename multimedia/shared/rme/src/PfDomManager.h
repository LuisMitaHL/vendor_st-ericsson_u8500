/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file PfDomManager.h
 * Platform domains manager static class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _PFDOMMANAGER_H_
#define _PFDOMMANAGER_H_

#include "PlatformDomains.h"

namespace rme {

/** Platform Domains Manager static class */
class PfDomManager {
 public:
  /// checks if pnd is user valid
  static bool IsPndValid(PlatformDomains::PNDS_E pnd);
  /// returns domain descriptor associated to a PND
  static const struct PlatformDomains::DomainDesc* GetDomainDesc(PlatformDomains::PNDS_E pnd);
  /// returns root name of a domain associated to a PND
  static const char* GetRootName(PlatformDomains::PNDS_E pnd);
  /// returns usecase name of a domain associated to a PND
  static const char* GetUsecaseName(PlatformDomains::PNDS_E pnd);
  /// returns domain type of a domain associated to a PND
  static PlatformDomains::DHM_E GetDomainType(PlatformDomains::PNDS_E pnd);
  /// returns scratchid of a domain associated to a PND
  static int GetScratchId(PlatformDomains::PNDS_E pnd);
  /// returns ESRAM fast id of a domain associated to a PND
  static RM_ESRAMBUFID_E GetEsramFastId(PlatformDomains::PNDS_E pnd);

  /// returns PND of parent domain associated to a scratch id > 0
  static PlatformDomains::PNDS_E GetDomainParentPnd(int scratchid);

  /// checks if domains overlap on ESRAM (scratch or non scratch)
  /// returns
  /// -1 or -2:overlap, 0:no overlap, 1 or 2:controlled overlap (scratch domains / parent)
  static int CheckEsramOverlap(PlatformDomains::PNDS_E pnd1, PlatformDomains::PNDS_E pnd2);

  /// transforms ESRAM buffer size/fast id into fast id
  static int SetEsramBufferFastId(RM_ESRAMBUFID_E *pEsramBufId, PlatformDomains::DHM_E type);
  /// returns PND associated to ESRAM fast identification for a given HW type
  static PlatformDomains::PNDS_E GetPndAssociatedToEsramFidHwtyp(RM_ESRAMBUFID_E esramfid,
                                                                 PlatformDomains::DHM_E hwtype);

  /// returns ESRAM buffer id string
  static const char* StringifyEsramFid(RM_ESRAMBUFID_E esramfid) {
    switch (esramfid) {
      case RM_ESBUFID_EMPTY:    return "esbufid_empty";
      case RM_ESBUFID_DEFAULT:  return "esbufid_default";
      case RM_ESBUFID_SMALL:    return "esbufid_small";
      case RM_ESBUFID_BIG:      return "esbufid_big";
      case RM_ESBUFID_BIGGEST:  return "esbufid_biggest";
      default: return "inval";
    }
  }

  /// returns HW type string
  static const char* StringifyHwType(PlatformDomains::DHM_E hwtype) {
    switch (hwtype) {
      case PlatformDomains::DHM_NA:        return "na";
      case PlatformDomains::DHM_PROCSVA:   return "procsva";
      case PlatformDomains::DHM_PROCSIA:   return "procsia";
      case PlatformDomains::DHM_HWPIPESVA: return "hwpipesva";
      case PlatformDomains::DHM_HWPIPESIA: return "hwpipesia";
      case PlatformDomains::DHM_HWXP70SIA: return "hwxp70sia";
      default: return "unk";
    }
  }

 private:
  // mapping table init
  static bool mDoneMapFidPndInits;

  /// mapping table ESRAM fast ids / pnds for ESRAM HW domains for DHM_HWPIPESVA
  static PlatformDomains::PNDS_E mMapFidPndHwpSva[RM_ESBUFID_LAST];
  /// mapping table ESRAM fast ids / pnds for ESRAM HW domains for DHM_HWPIPESIA
  static PlatformDomains::PNDS_E mMapFidPndHwpSia[RM_ESBUFID_LAST];
  /// mapping table ESRAM fast ids / pnds for ESRAM HW domains for DHM_HWXP70SIA
  static PlatformDomains::PNDS_E mMapFidPndHwxSia[RM_ESBUFID_LAST];

  /// returns ESRAM size of a domain associated to a PND
  static unsigned int GetDomainEsramSize(PlatformDomains::PNDS_E pnd);
  /// returns ESRAM offset of a domain associated to a PND
  static unsigned int GetDomainEsramOffset(PlatformDomains::PNDS_E pnd);
  /// returns SDRAM size of a domain associated to a PND
  static unsigned int GetDomainSdramSize(PlatformDomains::PNDS_E pnd);
  /// returns SDRAM offset of a domain associated to a PND
  static unsigned int GetDomainSdramOffset(PlatformDomains::PNDS_E pnd);

  /// performs mapping table inits
  static int InitMappingFidPndTable(PlatformDomains::PNDS_E mapfidpnd[RM_ESBUFID_LAST],
                                    PlatformDomains::DHM_E type);
};

}  // namespace

#endif

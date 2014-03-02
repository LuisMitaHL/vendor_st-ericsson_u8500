/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ResLevels.cpp
 * resources levels class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "ResLevels.h"
#include "PfRoles.h"
#include "PfDomManager.h"

namespace rme {

/// SxA resources class

/// inits resources levels associated to a component
/// by processing its resources estimation retrieved from OMXIL component
void SxaRes::Init(RM_RES_MCPS_T *pmcps) {
  if (pmcps == NULL) {
    memset(&mMcps, 0, sizeof(mMcps));
  } else {
    mMcps.vmax = pmcps->nMax;
    mMcps.vavg = pmcps->nAvg;
    // ensure average value is smaller than max value
    if (mMcps.vavg > mMcps.vmax) {
      RLOG_DEBUG("limiting average value to max value\n");
      mMcps.vavg = mMcps.vmax;
    }
  }
}

/// tests add/check-overflow on MCPS: returns 0 if succeeded, -1 if overflow detected
int SxaRes::TestAddCheckOverflowMcps(const SxaRes *pref, unsigned int limitM) const {
  if (pref == NULL)
    return 0;

  if ((mMcps.vmax + pref->mMcps.vmax) > limitM) {
    return -1;
  } else {
    return 0;
  }
}

/// adds/checks-overflow on MCPS: returns 0 if succeeded, -1 if overflow detected
int SxaRes::AddCheckOverflowMcps(const SxaRes *pref, unsigned int limitM) {
  if (pref == NULL)
    return 0;

  if (TestAddCheckOverflowMcps(pref, limitM) < 0) {
    return -1;
  } else {
    mMcps.vmax += pref->mMcps.vmax;
    if (pref->mMcps.vavg < pref->mMcps.vmax) {
      mMcps.vavg += pref->mMcps.vavg;
    } else {
      mMcps.vavg += pref->mMcps.vmax;
    }
    return 0;
  }
}

/// dumps resources levels
void SxaRes::DumpLevels(const char* desc) const {
  if (desc == NULL)
    return;

  if (mMcps.vmax > 0) {
    RLOG_DEBUG("\t%sMCPS-ma:%u,%u\n", desc, mMcps.vmax, mMcps.vavg);
  }
}

/// ESRAM resources class

/// tests add/check-overlap on ESRAM scratch domains for Sxa HW pipes:
/// returns 0 if succeeded, < 0 if conflict detected requiring suspension or unload
int EsramRes::TestAddCheckOverlapSxaHwpEsramScratch(const EsramRes *pnewesramres,
    RM_ESRAMBUFID_E *pfidesrambufsvahwpipe, RM_ESRAMBUFID_E *pfidesrambufsiahwpipe) const {
  if (pnewesramres == NULL)
    return 0;

  PlatformDomains::PNDS_E pndsvahwp =
    PfDomManager::GetPndAssociatedToEsramFidHwtyp(mFidEsramBufSvaHwPipe,
        PlatformDomains::DHM_HWPIPESVA);
  PlatformDomains::PNDS_E pndsvahwpnew =
    PfDomManager::GetPndAssociatedToEsramFidHwtyp(pnewesramres->mFidEsramBufSvaHwPipe,
        PlatformDomains::DHM_HWPIPESVA);

  PlatformDomains::PNDS_E pndsiahwp =
    PfDomManager::GetPndAssociatedToEsramFidHwtyp(mFidEsramBufSiaHwPipe,
        PlatformDomains::DHM_HWPIPESIA);
  PlatformDomains::PNDS_E pndsiahwpnew =
    PfDomManager::GetPndAssociatedToEsramFidHwtyp(pnewesramres->mFidEsramBufSiaHwPipe,
        PlatformDomains::DHM_HWPIPESIA);

  // check for overlap between different parents in a given type
  if (PfDomManager::CheckEsramOverlap(pndsvahwp, pndsvahwpnew) < 0) {
    // overlap
    return -1;
  }
  if (PfDomManager::CheckEsramOverlap(pndsiahwp, pndsiahwpnew) < 0) {
    // overlap
    return -1;
  }

  RM_ESRAMBUFID_E FidEsramBufSvaHwPipe = mFidEsramBufSvaHwPipe;
  RM_ESRAMBUFID_E FidEsramBufSiaHwPipe = mFidEsramBufSiaHwPipe;
  // assumes Fids take greater value for each domain
  // warning: in the case of different parents for same type,
  // following might lead to 'hide' away such parents
  if (pnewesramres->mFidEsramBufSvaHwPipe > FidEsramBufSvaHwPipe) {
    FidEsramBufSvaHwPipe = pnewesramres->mFidEsramBufSvaHwPipe;
    pndsvahwp = pndsvahwpnew;
  }
  if (pnewesramres->mFidEsramBufSiaHwPipe > FidEsramBufSiaHwPipe) {
    FidEsramBufSiaHwPipe = pnewesramres->mFidEsramBufSiaHwPipe;
    pndsiahwp = pndsiahwpnew;
  }

  // check for overlap between SVA and SIA HW pipe ESRAM scratch domains;
  if (PfDomManager::CheckEsramOverlap(pndsvahwp, pndsiahwp) < 0) {
    return -1;
  }

  // possible update
  if (pfidesrambufsvahwpipe) {
    *pfidesrambufsvahwpipe = FidEsramBufSvaHwPipe;
  }
  if (pfidesrambufsiahwpipe) {
    *pfidesrambufsiahwpipe = FidEsramBufSiaHwPipe;
  }

  return 0;
}

/// adds/checks-overlap on ESRAM scratch domains for Sxa HW pipes:
/// returns 0 if succeeded, < 0 if conflict detected requiring suspension or unload
int EsramRes::AddCheckOverlapSxaHwpEsramScratch(const EsramRes *pnewesramres) {
  if (pnewesramres == NULL)
    return 0;

  RM_ESRAMBUFID_E FidEsramBufSvaHwPipe = mFidEsramBufSvaHwPipe;
  RM_ESRAMBUFID_E FidEsramBufSiaHwPipe = mFidEsramBufSiaHwPipe;
  if (TestAddCheckOverlapSxaHwpEsramScratch(pnewesramres,
        &FidEsramBufSvaHwPipe,
        &FidEsramBufSiaHwPipe) < 0) {
    return -1;
  }

  // possible update
  mFidEsramBufSvaHwPipe = FidEsramBufSvaHwPipe;
  mFidEsramBufSiaHwPipe = FidEsramBufSiaHwPipe;

  return 0;
}

/// does basic Sia HW XP70 'add' for NON scratch ESRAM xp70 domain(s):
/// no overlap checking and no overflow checking (from different users) since done by imaging RM
void EsramRes::SiaHwxp70AddNoCheck(const EsramRes *pref) {
  if (pref) {
    // assumes valid Fids for pref (checked before)
    if (pref->mFidEsramBufSiaHwXP70 > mFidEsramBufSiaHwXP70)
      mFidEsramBufSiaHwXP70 = pref->mFidEsramBufSiaHwXP70;
  }
}

/// dumps resources levels
void EsramRes::DumpLevels(const char* desc) const {
  if (desc == NULL)
    return;
  if (mFidEsramBufSvaHwPipe > RM_ESBUFID_EMPTY)
    RLOG_DEBUG("\t%sSvaHwPipe:%s\n", desc, PfDomManager::StringifyEsramFid(mFidEsramBufSvaHwPipe));
  if (mFidEsramBufSiaHwPipe > RM_ESBUFID_EMPTY)
    RLOG_DEBUG("\t%sSiaHwPipe:%s\n", desc, PfDomManager::StringifyEsramFid(mFidEsramBufSiaHwPipe));
  if (mFidEsramBufSiaHwXP70 > RM_ESBUFID_EMPTY)
    RLOG_DEBUG("\t%sSiaHwXP70:%s\n", desc, PfDomManager::StringifyEsramFid(mFidEsramBufSiaHwXP70));
}

/// ResLevels class

/// inits resources levels associated to a component
/// by processing its resources estimation retrieved from OMXIL component
void ResLevels::InitLevels(RM_EMDATA_T *pemdata, bool siaforvisual) {
  if (pemdata == NULL) {
    // reset case
    mSvaLevels.Init(NULL);
    mSiaLevelsAudio.Init(NULL);
    mSiaLevelsVisual.Init(NULL);
    memset(&mEsramLevels, 0, sizeof(mEsramLevels));
    mVisualFHD = 0;
    return;
  }

  // SVA usage
  mSvaLevels.Init(&pemdata->sMcpsSva);
  // SIA usage
  if (siaforvisual == true) {
    mSiaLevelsAudio.Init(NULL);
    mSiaLevelsVisual.Init(&pemdata->sMcpsSia);
  } else {
    mSiaLevelsAudio.Init(&pemdata->sMcpsSia);
    mSiaLevelsVisual.Init(NULL);
  }

  // ESRAM buffers usage
  // all sizes shall have been checked and transformed in fast ids beforehand
  mEsramLevels.mFidEsramBufSvaHwPipe =
    static_cast<RM_ESRAMBUFID_E>(pemdata->sEsramBufs.nSvaHwPipe);
  mEsramLevels.mFidEsramBufSiaHwPipe =
    static_cast<RM_ESRAMBUFID_E>(pemdata->sEsramBufs.nSiaHwPipe);
  mEsramLevels.mFidEsramBufSiaHwXP70 =
    static_cast<RM_ESRAMBUFID_E>(pemdata->sEsramBufs.nSiaHwXP70);

  // Visual Full HD usage
  if (pemdata->eUccBitmap & RM_UCC_VISUAL_FHD)
    mVisualFHD = RM_UCC_VISUAL_FHD;
}

/// checks if resources are SVA based
bool ResLevels::IsSvaBased() const {
  if ((mSvaLevels.mMcps.vmax > 0) ||
      (mEsramLevels.mFidEsramBufSvaHwPipe > RM_ESBUFID_EMPTY)) {
    return true;
  } else {
    return false;
  }
}

/// checks if resources are SIA based
bool ResLevels::IsSiaBased() const {
  if (IsSiaAudioBased() || IsSiaVisualBased()) {
    return true;
  } else {
    return false;
  }
}

/// checks if resources are SIA-audio based
bool ResLevels::IsSiaAudioBased() const {
  if (mSiaLevelsAudio.mMcps.vmax > 0) {
    return true;
  } else {
    return false;
  }
}

/// checks if resources are SIA-visual based
bool ResLevels::IsSiaVisualBased() const {
  if ((mSiaLevelsVisual.mMcps.vmax > 0) ||
      (mEsramLevels.mFidEsramBufSiaHwPipe > RM_ESBUFID_EMPTY) ||
      (mEsramLevels.mFidEsramBufSiaHwXP70 > RM_ESBUFID_EMPTY)) {
    return true;
  } else {
    return false;
  }
}

/// checks if resources ESRAM used (cross SxA usages)
bool ResLevels::IsUsingEsram() const {
  if ((mEsramLevels.mFidEsramBufSvaHwPipe > RM_ESBUFID_EMPTY) ||
      (mEsramLevels.mFidEsramBufSiaHwPipe > RM_ESBUFID_EMPTY) ||
      (mEsramLevels.mFidEsramBufSiaHwXP70 > RM_ESBUFID_EMPTY)) {
    return true;
  } else {
    return false;
  }
}

/// dumps resources levels
void ResLevels::DumpLevels(const char* name) const {
  if (name)
    RLOG_DEBUG("reslevels %s sva:%d sia:%d\n", name, IsSvaBased(), IsSiaBased());

  if (IsSvaBased()) {
    mSvaLevels.DumpLevels("SVA-");
  }
  if (IsSiaBased()) {
    mSiaLevelsAudio.DumpLevels("SIA-audio-");
    mSiaLevelsVisual.DumpLevels("SIA-visual-");
  }

  mEsramLevels.DumpLevels("ESRAM-");

  if (mVisualFHD)
    RLOG_DEBUG("\tVisualFHD mode:%d\n", mVisualFHD);
}

}  // namespace

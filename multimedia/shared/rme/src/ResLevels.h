/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ResLevels.h
 * resources levels class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _RESLEVELS_H_
#define _RESLEVELS_H_

#include <rme_types.h>

#include "uosal.h"

namespace rme {

/** SxA resources class */
class SxaRes {
 public:
  /// inits resources levels associated to a component
  /// by processing its resources estimation retrieved from OMXIL component
  void Init(RM_RES_MCPS_T *pmcps);

  /// tests add/check-overflow on MCPS: returns 0 if succeeded, -1 if overflow detected
  int TestAddCheckOverflowMcps(const SxaRes *pref, unsigned int limitM) const;
  /// adds/checks-overflow on MCPS: returns 0 if succeeded, -1 if overflow detected
  int AddCheckOverflowMcps(const SxaRes *pref, unsigned int limitM);

  /// dumps resources levels
  void DumpLevels(const char* desc) const;

  /// ctor
  SxaRes() : mMcps() {}

 private:
  friend class ResLevels;

  /// descriptor MCPS max-average
  struct McpsMA {
    unsigned int vmax;  /// maximum value
    unsigned int vavg;  /// average value
    /// ctor
    McpsMA() : vmax(0), vavg(0) {}
  };

  /// MCPS
  McpsMA mMcps;
};

/** ESRAM resources class */
class EsramRes {
 public:
  /// returns ESRAM fastid for SVA-HWPIPE scratch buffer
  RM_ESRAMBUFID_E FidEsramBufSvaHwPipe() const { return mFidEsramBufSvaHwPipe; }
  /// returns ESRAM fastid for SIA-HWPIPE scratch buffer
  RM_ESRAMBUFID_E FidEsramBufSiaHwPipe() const { return mFidEsramBufSiaHwPipe; }
  /// returns ESRAM fastid for SIA-XP70 buffer
  RM_ESRAMBUFID_E FidEsramBufSiaHwXP70() const { return mFidEsramBufSiaHwXP70; }

  /// tests add/check-overlap on ESRAM scratch domains for Sxa HW pipes:
  /// returns 0 if succeeded, < 0 if conflict detected requiring suspension or unload
  int TestAddCheckOverlapSxaHwpEsramScratch(const EsramRes *pnewesramres,
      RM_ESRAMBUFID_E *pfidesrambufsvahwpipe, RM_ESRAMBUFID_E *pfidesrambufsiahwpipe) const;
  /// adds/checks-overlap on ESRAM scratch domains for Sxa HW pipes:
  /// returns 0 if succeeded, < 0 if conflict detected requiring suspension or unload
  int AddCheckOverlapSxaHwpEsramScratch(const EsramRes *pnewesramres);

  /// does basic Sia HW XP70 'add' for NON scratch ESRAM xp70 domain(s):
  /// no overlap checking and no overflow checking (from different users) since done by imaging RM
  void SiaHwxp70AddNoCheck(const EsramRes *pref);

  /// dumps resources levels
  void DumpLevels(const char* desc) const;

  /// ctor
  EsramRes()
      :
        mFidEsramBufSvaHwPipe(RM_ESBUFID_EMPTY),
        mFidEsramBufSiaHwPipe(RM_ESBUFID_EMPTY),
        mFidEsramBufSiaHwXP70(RM_ESBUFID_EMPTY) {}

 private:
  friend class ResLevels;

  /// ESRAM fastid for SVA-HWPIPE scratch buffer
  RM_ESRAMBUFID_E mFidEsramBufSvaHwPipe;
  /// ESRAM fastid for SIA-HWPIPE scratch buffer
  RM_ESRAMBUFID_E mFidEsramBufSiaHwPipe;
  /// ESRAM fastid for SIA-XP70 buffer
  RM_ESRAMBUFID_E mFidEsramBufSiaHwXP70;
};

/** Resources levels class */
class ResLevels {
 public:
  /// inits resources levels associated to a component
  /// by processing its resources estimation retrieved from OMXIL component
  void InitLevels(RM_EMDATA_T *pemdata, bool siaforvisual);

  /// checks if resources are SVA based
  bool IsSvaBased() const;
  /// checks if resources are SIA based
  bool IsSiaBased() const;
  /// checks if resources are SIA-audio based
  bool IsSiaAudioBased() const;
  /// checks if resources are SIA-visual based
  bool IsSiaVisualBased() const;

  /// checks if resources ESRAM used (cross SxA usages)
  bool IsUsingEsram() const;

  /// dumps resources levels
  void DumpLevels(const char* name) const;

  /// returns ESRAM fastid for SVA-HWPIPE scratch buffer
  RM_ESRAMBUFID_E FidEsramBufSvaHwPipe() const {
    return mEsramLevels.FidEsramBufSvaHwPipe(); }
  /// returns ESRAM fastid for SIA-HWPIPE scratch buffer
  RM_ESRAMBUFID_E FidEsramBufSiaHwPipe() const {
    return mEsramLevels.FidEsramBufSiaHwPipe(); }
  /// returns ESRAM fastid for SIA-XP70 buffer
  RM_ESRAMBUFID_E FidEsramBufSiaHwXP70() const {
    return mEsramLevels.FidEsramBufSiaHwXP70(); }

  /// returns visual FHD
  int VisualFHD() const { return mVisualFHD; }

  /// ctor
  ResLevels() :
    mSvaLevels(),
    mSiaLevelsAudio(),
    mSiaLevelsVisual(),
    mEsramLevels(),
    mVisualFHD(0) {
    }

 private:
  friend class ResEstimator;

  /// SVA levels
  SxaRes mSvaLevels;
  /// SIA levels / Audio logical part
  SxaRes mSiaLevelsAudio;
  /// SIA levels / Visual logical part
  SxaRes mSiaLevelsVisual;

  /// ESRAM levels
  EsramRes mEsramLevels;

  /// Visual FHD (based on RM_UCCBF_E)
  int mVisualFHD;

  // DISALLOW_ASSIGN
  explicit ResLevels(ResLevels const&);
};

}  // namespace

#endif

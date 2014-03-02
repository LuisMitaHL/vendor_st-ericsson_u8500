/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ResEstimator.h
 * resource estimator class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _RESESTIMATOR_H_
#define _RESESTIMATOR_H_

#include "uosal.h"
#include "ORPComp.h"
#include "PlatformDomains.h"
#include "PlatformLimits.h"

namespace rme {

/** Resources estimator class */
class ResEstimator {
  public:
    /// checks if processing of resources can be done
    static const ResLevels* CanProceedResources(const ORPComp *pcomp);

    /// scans active resources for visual mode
    void ScanActiveResourcesVisualMode(ResLevels *preslev) const;

    /// scans and updates resources levels
    /// and marks candidates for preemption (full/partial)
    void ScanResourcesMarkPreempts(ResLevels *preslev) const;

    /// returns current interconnect visual mode
    int IcnVisualResMode() const { return mIcnVisualMode; }
    /// sets current interconnect visual mode
    void SetIcnVisualResMode(int mode) { mIcnVisualMode = mode; }

    /// ctor
    ResEstimator() :
      mIcnVisualMode(0) {
      CheckAllEsramOverlaps();
    }

  private:
    /// interconnect visual resolution mode: -1 non FHD, 0 unknown, 1 FHD
    int mIcnVisualMode;

    /// iterates resources scan : tests candidate for preemption checking resources levels
    /// returned status; 0:ok, -1:partial preemption, -2:total preemption
    int IterateScanResTestCheck(const ResLevels *preslev,
                                const ResLevels *pcompreslevels) const;
    /// iterates resources scan : updates resources levels
    void IterateScanResUpdate(ResLevels *preslev,
                              const ResLevels *pcompreslevels) const;

    /// checks overlaps between ALL ESRAM domains
    void CheckAllEsramOverlaps();

    // disallow copy and assign default constructors
    DISALLOW_COPY_AND_ASSIGN(ResEstimator);
};

}  // namespace

#endif

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file NmfDomains.h
 * NMF memory domains class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _NMFDOMAINS_H_
#define _NMFDOMAINS_H_

#include "uosal.h"
#include "PfDomManager.h"

namespace rme {

/** NMF memory domains manager class */
class NmfDomains {
 public:
  /// creates NMF memory domain associated to provided descriptor
  /// and sets domain id on success
  int CreateDomain(PlatformDomains::PNDS_E pnd, unsigned int *pdomid, bool createforclient, uint64_t clientpid);
  /// destroys NMF memory domain associated to provided descriptor and domain id
  void DestroyDomain(PlatformDomains::PNDS_E pnd, unsigned int domid);

  /// creates all NMF memory domains associated to 'non scratch' platform named domain
  int CreateAllNonScratchPlatformDomains();

  /// returns NMF memory domain id associated to 'non scratch' platform named domain
  OMX_U32 GetNonScratchPlDomId(PlatformDomains::PNDS_E pnd) const {
    if (PfDomManager::IsPndValid(pnd) == false)
      return 0;
    else
      return mTableNsPndDomainIds[pnd];
  }

  /// ctor
  NmfDomains() :
    mTableNsPndDomainIds()
#ifndef NOCREATEDOMAINFORCLIENT
    , mTablePidDomainIds()
#endif
  {
    memset(&mTableNsPndDomainIds, 0, sizeof(mTableNsPndDomainIds));
#ifndef NOCREATEDOMAINFORCLIENT
    memset(&mTablePidDomainIds, 0, sizeof(mTablePidDomainIds));
#endif
  }
  /// dtor
  virtual ~NmfDomains() {
    for (unsigned int i = PlatformDomains::PND_NA; i < PlatformDomains::PND_COUNT; i++) {
      PlatformDomains::PNDS_E pnd = static_cast<PlatformDomains::PNDS_E>(i);
      if (PfDomManager::IsPndValid(pnd) == false)
        continue;
      // destroy all remaining valid non scratch domains
      if (mTableNsPndDomainIds[i] != 0) {
        DestroyDomain(pnd, mTableNsPndDomainIds[i]);
      }
    }
#ifndef NOCREATEDOMAINFORCLIENT
    // destroy all remaining domains in mTablePidDomainIds
    for(unsigned int i = 0; i < msMAXCountPidDomains; i++) {
      if (mTablePidDomainIds[i].refcount > 0) {
        mTablePidDomainIds[i].refcount = 1;
        DestroyDomain(mTablePidDomainIds[i].pnd, mTablePidDomainIds[i].domid);
      }
    }
#endif
  }

 private:
  /// associative table {index::PND, value::NMF 'non scratch' domain id}
  OMX_U32 mTableNsPndDomainIds[PlatformDomains::PND_COUNT];

#ifndef NOCREATEDOMAINFORCLIENT
  static const unsigned int msMAXCountPidDomains = 128;
  struct PidDomain {
      uint64_t pid;
      PlatformDomains::PNDS_E pnd;
      unsigned int domid;
      unsigned int refcount;
  } mTablePidDomainIds[msMAXCountPidDomains];

  /// return index for PidDomain entry; returns -1 if not found
  int GetIndexPidDomain(uint64_t pid, PlatformDomains::PNDS_E pnd);
  /// return new index for PidDomain entry; returns -1 if not found
  int GetNewIndexPidDomain(void);
#endif

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(NmfDomains);
};

}  // namespace

#endif

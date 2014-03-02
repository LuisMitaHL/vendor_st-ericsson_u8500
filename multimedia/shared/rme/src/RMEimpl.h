/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file RMEimpl.h
 * RME implementation class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _RMEIMPL_H_
#define _RMEIMPL_H_

#include <rme.h>

#include "NmfDomains.h"
#include "ORPCompNetwork.h"
#include "ResManager.h"
#include "AGov.h"

namespace rme {

/** RME implementation class
  is a 'facade' for the various submodules implementing the RME */
class RMEimpl : public RME {
 protected:
  // for singleton management
  friend RME* RME::getInstance();
  friend void RME::delInstance();

  /// unique instance
  static RMEimpl* mInstance;
  /// reference counting
  int mRefCount;

  /// RME API: Register
  /// call mode: SYNCHRONOUS from RME client (OMXIL component) context
  RM_STATUS_E Register(
      OMX_IN const OMX_PTR pCompHdl,
      OMX_IN const RM_CBK_T* pCompCbks);

  /// RME API: Unregister
  /// call mode: SYNCHRONOUS from RME client (OMXIL component) context
  RM_STATUS_E Unregister(
      OMX_IN const OMX_PTR pCompHdl);

  /// RME API: Notify_NewStatesOMXRES
  /// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
  void Notify_NewStatesOMXRES(
      OMX_IN const OMX_PTR pCompHdl,
      OMX_IN OMX_STATETYPE eOMXILState,
      OMX_IN RM_RESOURCESTATE_E eResourceState,
      OMX_IN OMX_U32 nReserved);

  /// RME API: Notify_NewPolicyCfg
  /// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
  void Notify_NewPolicyCfg(
      OMX_IN const OMX_PTR pCompHdl,
      OMX_IN OMX_U32 nGroupPriority,
      OMX_IN OMX_U32 nGroupID,
      OMX_IN OMX_U32 nQosBitmap);

  /// RME API: Notify_RMConfigApplied
  /// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
  void Notify_RMConfigApplied(
      OMX_IN const OMX_PTR pCompHdl,
      OMX_IN RM_RC_E eType,
      OMX_IN OMX_S32 nData,
      OMX_IN RM_STATUS_E nErrorCode);

  /// RME API: Notify_Tunnel
  /// call mode: ASYNCHRONOUS from RME thread MESSAGE dispatcher
  void Notify_Tunnel(
      OMX_IN const OMX_PTR pCompHdlOut,
      OMX_IN OMX_U32 nPortIndexOut,
      OMX_IN const OMX_PTR pCompHdlIn,
      OMX_IN OMX_U32 nPortIndexIn);

  /// RME API: Get_DefaultDDRDomain
  /// call mode: SYNCHRONOUS from RME client (OMXIL component) context
  OMX_U32 Get_DefaultDDRDomain(
      OMX_IN RM_NMFD_E nDomainType);

  /// RME API: InitSupport_PRM
  /// call mode: SYNCHRONOUS from RME thread MESSAGE dispatcher
  void InitSupport_PRM(
      OMX_IN void (*NotifyInterconnectBwMode)(OMX_IN OMX_BOOL bUse1080pSettings));

  /// RME API: idleT
  /// call mode: SYNCHRONOUS from RME thread MESSAGE dispatcher
  int idleT();

  // for sharing mutex and instance with AGov threads
  friend void AGov::DisplayDetectorThreadImpl();
  friend void AGov::ActivityDetectorThreadImpl();
  friend unsigned int AGov::ProcessALP(const char*, bool);

  /// mutex on asynchronous RME API calls
  /// kind of enforces message-based RME wrapper
  static omxilosalservices::OmxILOsalMutex* mpMutexAsync;
  /// locks asynchronous messages mutex
  static bool MutexAsyncMsgLock()   { if (mpMutexAsync) { mpMutexAsync->MutexLock(); return true;} else { return false; } }
  /// unlocks  asynchronous messages mutex
  static void MutexAsyncMsgUnlock() { if (mpMutexAsync) { mpMutexAsync->MutexUnlock(); } }

  /// counts components registered
  int CountComponents() { return mORPCompNetw.CountComponents(); }

  /// ctor
  RMEimpl() :
    mRefCount(1),
    mNmfDomains(),
    mORPCompNetw(),
    mAGov(),
    mResManager(),
    mCounterAsync(0),
    mIdleCounterAsync(0)
  {}

  /// dtor
  virtual ~RMEimpl() {}

 private:
  /// NMF domains manager
  NmfDomains mNmfDomains;
  /// ORPComponent network manager: aggregation of OMXIL Resource Proxy (ORP) components
  ORPCompNetwork mORPCompNetw;
  /// audio specific governor
  AGov mAGov;
  /// core resources manager
  ResManager mResManager;

  /// counter on asynchronous call
  unsigned int mCounterAsync;
  /// value of counter on Idle asynchronous call
  unsigned int mIdleCounterAsync;

  /// inits
  int Init();

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(RMEimpl);
};

}  // namespace

#endif

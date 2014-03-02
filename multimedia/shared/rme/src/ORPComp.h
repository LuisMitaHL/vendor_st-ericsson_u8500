/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ORPComp.h
 * OMX IL component class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _ORPCOMP_H_
#define _ORPCOMP_H_

#include "uosal.h"
#include "TvdListNH.h"
#include "StatesDuo.h"
#include "PfDomManager.h"
#include "ResLevels.h"

namespace rme {

/** an ORPComp object can belong to several lists at the same time:
  following enum provides ids and count for these lists */
enum ORPC_LIDS_E {
  ORPC_LID_0 = 0,  /// important to start from 0: enforce it, just to make sure
  ORPC_LID_1,
  ORPC_LID_2,
  ORPC_LID_3,
  ORPC_LID_COUNT  /// number of lists supported by a single ORPComp object
};

// template to use for TvdListNode / TvdListHead related to ORPComp
#define ORPCOMPVTTEMPLATE ORPComp, ORPC_LID_COUNT

/** OMXIL resource-proxy (ORP) component class */
class ORPComp : public TvdListNode<ORPCOMPVTTEMPLATE> {
 public:
  ///////////////////// identification basis

  /// returns index : unique among all ORPcomponents
  unsigned int Index() const { return mIndex; }

  /// returns OMXIL role provided through capabilities
  const char* Role() const { return &mOmxilRole[0]; }
  /// returns OMXIL name provided through capabilities
  const char* OmxilName() const { return &mOmxilName[0]; }
  /// returns capability bitmap
  unsigned int CapBitmap() const { return mCapBitmap; }

  /// returns OMXIL component's handle provided at registration time
  OMX_PTR OmxilCompHdl() const { return mOmxilCompHdl; }

  ///////////////////// network related: connection lines

  /// returns network halfline id
  int NwHalflineId() const { return mNwHalflineId; }
  /// sets network halfline id
  void SetNwHalflineId(int id);

  /// returns place id
  int NwHalflinePlaceId() const { return mNwHalflinePlaceId; }
  /// returns place type
  int NwHalfinePlaceType() const { return mNwHalfinePlaceType; }
  /// sets place id
  void SetNwHalflinePlace(int placeid, int placetype) { mNwHalflinePlaceId = placeid; mNwHalfinePlaceType = placetype;}

  /// maximum number of ports per component
  static const unsigned int msMAXCountPorts = 16;

  /// counts active ports discovered mainly through tunneling requests
  unsigned int CountPorts() const { return mCountPorts; }
  /// gets or adds ORP port entry associated to OMXIL port index
  unsigned int GetAddORPport(unsigned int omxportindex, bool wayin);
  /// returns ORP port index associated to OMXIL port index
  unsigned int OrpPortIndex(unsigned int omxportindex) const;
  /// returns OMXIL port index associated to ORP port index
  int OmxPortIndex(unsigned int orpportindex) const;

  /// checks for input port associated to ORP port index
  bool IsInputPort(unsigned int orpportindex) const;
  /// checks for output port associated to ORP port index
  bool IsOutputPort(unsigned int orpportindex) const;

  /// returns network halfline id of peer for port associated to ORP port index
  int PortPeerNwHlId(unsigned int orpportindex) const;
  /// sets network halfline id of peer for port associated to ORP port index
  void SetPortPeerNwHlId(unsigned int orpportindex, int value);

  /// counts number of input connections
  unsigned int CountConnectionsOnPortIn() const;
  /// counts number of output connections
  unsigned int CountConnectionsOnPortOut() const;
  /// counts number of connections
  unsigned int CountConnectionsOnPort() const;

  /// disconnects input ports that have a peer with given network halfline id
  void DisconnectInputPortsWithPeerNwHlId(unsigned int id);
  /// disconnects output ports that have a peer with given network halfline id
  void DisconnectOutputPortsWithPeerNwHlId(unsigned int id);
  /// replaces all occurences of old peer network halfline id by provided id
  void ReplacePortsId(unsigned int old_id, unsigned int upd_id,
      bool replace_in = true, bool replace_out = true);

  ///////////////////// policy

  /// sorting comparaison method: compares per priority and date
  static int ComparePerPriorityAndResourceDate(const ORPComp* pcompA,
                                               const ORPComp* pcompB);

  /// returns policy group priority 'system'
  unsigned int PolGroupPrioritySystem() const { return mPolicyGroup.prioritysystem; }
  /// returns policy group priority 'client'
  unsigned int PolGroupPriorityClient() const { return mPolicyGroup.priorityclient; }
  /// returns policy group id
  unsigned int PolGroupId() const { return mPolicyGroup.gid; }
  /// returns policy process id
  uint64_t PolProcessId() const { return mPolicyGroup.pid; }
  /// returns policy thread id
  uint64_t PolThreadId() const { return mPolicyGroup.tid; }

  /// sets policy group priority 'system'
  void SetPolGroupPrioritySystem(unsigned int priority) {
    mPolicyGroup.prioritysystem = priority; }
  /// sets policy group priority 'client'
  void SetPolGroupPriorityClient(unsigned int priority) {
    mPolicyGroup.priorityclient = priority; }
  /// sets policy group id
  void SetPolGroupId(unsigned int gid) {
    mPolicyGroup.gid = gid; }

  /// checks if resources suspension is possible
  bool PolCanSuspend() const { return mPolicyGroup.can_suspend; }

  ///////////////////// states management

  /// returns OMXIL state
  OMX_STATETYPE OmxState() const { return mStatesORp.OmxState(); }
  /// returns resource state
  RM_RESOURCESTATE_E ResState() const { return mStatesORp.ResState(); }
  /// returns dual state OMXIL/Resource
  DS_OR_E DualStateOR() const { return mStatesORp.DsOR(); }
  /// returns string version of dual state OMXIL/Resource
  const char* DualStateORstr() const { return StatesDuo::StringifyDsOR(mStatesORp.DsOR()); }

  /// sets OMXIL/Resources states
  void SetORstates(OMX_STATETYPE somxil, RM_RESOURCESTATE_E sres) {
    mStatesORp.UpdateDsOR(somxil, sres); }

  /// checks if a transition to given dual OR state is valid or not
  /// and sets transition id associated
  bool IsDsORtransitionValidGetTSId(DS_OR_E to_ds_or,
                                    DS_TSI_E *ptransitionid) const;

  /// returns date of resources grant
  unsigned int DateResourceGrant() const { return mDateResourceGrant; }
  /// sets date of resources grant
  void UpdateDateResourceGrant() { mDateResourceGrant = msGlobalResourceDate++; }
  /// resets date of resources grant
  void ResetDateResourceGrant() { mDateResourceGrant = static_cast<unsigned int>(-1); }
  /// backups and resets date of resources grant
  void BackupResetDateResourceGrant() {
    mDateResourceGrantBackup = mDateResourceGrant;
    mDateResourceGrant = static_cast<unsigned int>(-1); }
  /// reverts backuped date of resources grant
  void RevertDateResourceGrant() { mDateResourceGrant = mDateResourceGrantBackup; }
  /// checks for timeout on OMXIL component reaction to preemption command
  bool IsTimedOutReactResReleaseCmd();

  /// reinitializes partially when unloading resources
  void ReInitializeUnload();

  ///////////////////// activity & operational states

  /// control & operational status flags
  enum CF_PUB_E {
    /// set if component is marked as unregistered
    CF_UNREGISTERED = (1UL << 0),

    /// set if capabilities retrieved
    CF_CAPABILITIES_VALID = (1UL << 1),
    /// set if resources estimation retrieved
    CF_RESESTIM_VALID     = (1UL << 2),

    /// set during any resource request phases
    CF_INREQUESTPHASE    = (1UL << 3),
    /// set during dynamic resource request phases
    CF_INDYNREQUESTPHASE = (1UL << 4),
    /// set after resources request phase granted, but
    /// before move out of REQUIRED state (by OMXIL comp)
    CF_POSTREQUESTPHASE_RESGRANTED = (1UL << 5),
    /// set if full preemption requested
    CF_PENDING_RESPREEMPTION_FULL    = (1UL << 6),
    /// set if partial preemption (suspension) requested
    CF_PENDING_RESPREEMPTION_PARTIAL = (1UL << 7),

    /// set during resources work loop
    CF_RWL_CANDIDATE_RESPREEMPTION_FULL    = (1UL << 8),
    /// set during resources work loop
    CF_RWL_CANDIDATE_RESPREEMPTION_PARTIAL = (1UL << 9),
    /// set during resources work loop
    CF_RWL_CANDIDATE_RESGRANT = (1UL << 10),

    /// set if RM config 'NMF domain' sent -- waiting for 'applied' notification
    CF_PENDING_RMCFG_NMFD = (1UL << 11),
    /// set if RM config 'NMF domain' applied (possibly using default value)
    CF_ACTIVE_RMCFG_NMFD  = (1UL << 12),
    /// set if RM config 'NMF domain' failed
    CF_FAILED_RMCFG_NMFD  = (1UL << 13),

    /// set if RM config 'ALP/enter' sent -- waiting for 'applied' notification
    CF_PENDING_RMCFG_ALP_ENTER = (1UL << 14),
    /// set if RM config 'ALP/exit' sent -- waiting for 'applied' notification
    CF_PENDING_RMCFG_ALP_EXIT  = (1UL << 15),
    /// set if Audio LowPower active
    CF_ACTIVE_RMCFG_ALP        = (1UL << 16),
    /// set if Audio LowPower failed
    CF_FAILED_RMCFG_ALP        = (1UL << 17)
  };
  /// sets a control flag
  void SetCFlag(CF_PUB_E flag)   { mControlsBmp |= flag; }
  /// clears a control flag
  void ClearCFlag(CF_PUB_E flag) { mControlsBmp &= ~flag; }
  /// checks a control flag
  unsigned int CFlag(CF_PUB_E flag) const { return (mControlsBmp & flag); }

  /// sets a control flag for ORP port
  void SetCFlagPort(CF_PUB_E flag, unsigned int orpportindex);
  /// clears a control flag for ORP port
  void ClearCFlagPort(CF_PUB_E flag, unsigned int orpportindex);
  /// checks a control flag for ORP port
  unsigned int CFlagPort(CF_PUB_E flag, unsigned int orpportindex);

  /// locks callbacks mutex
  void MutexCpLock()   { if (mpMutexCp) {mpMutexCp->MutexLock();} }
  /// unlocks callbacks mutex
  void MutexCpUnlock() { if (mpMutexCp) { mpMutexCp->MutexUnlock();} }

  ///////////////////// domains

  /// returns domain id for a domain type
  unsigned int DomIdForDtype(PlatformDomains::DHM_E dtype) const;

  /// returns PND for a domain type
  PlatformDomains::PNDS_E PndForDtype(PlatformDomains::DHM_E dtype) const;

  /// sets PND for a domain type
  /// and possibly creates scratch domain associated (depends on type)
  int SetPndForDtype(PlatformDomains::PNDS_E pnd,
                     PlatformDomains::DHM_E dtype);

  /// clears all PNDs, and destroy scratch domains (if any)
  void ClearPndsAndDomains(bool scratchdomainsonly = false);

  ///////////////////// Resources Estimation Tables

  /// returns resources estimations levels
  const ResLevels* ResourcesEstimLevels() const {
    if (mControlsBmp & CF_RESESTIM_VALID)
      return &mResEstimLevels;
    else
      return NULL;
  }

  /// returns usecase context bitmap
  unsigned int UccBitmap() const { return mUccBitmap; }

  /// revert backuped values
  void RevertResEstimUccValues() {
      memcpy(&mResEstimLevels, &mResEstimLevelsBackup, sizeof(mResEstimLevels));
      mUccBitmap = mUccBitmapBackup;
  }

  ///////////////////// wrapping calls to OMXIL component

  /// fills capabilities structure used for interaction with policy server
  void FillCapabilities(RM_CAPABILITIES_T *pcapabilities) const;

  /// sets callbacks to OMXIL component
  int SetCallbacks(const RM_CBK_T* pCompCbks);

  /// retrieves capabilities from the OMXIL component
  RM_STATUS_E CpGetCapabilities();
  /// retrieves resources estimation from the OMXIL component
  RM_STATUS_E CpGetResourcesEstimation();

  /// commands set NMF domain ids configuration to the OMXIL component
  void CpCmdSetRmConfigNmfDomainIds();
  /// commands set audio lowpower enter configuration to the OMXIL component
  void CpCmdSetRmConfigALPenter();
  /// commands set audio lowpower exit configuration to the OMXIL component
  void CpCmdSetRmConfigALPMexit();
  /// commands release partial resources (suspension) to the OMXIL component
  int CpCmdSuspendResources();
  /// commands release full resources (unload) to the OMXIL component
  void CpCmdUnloadResources();

  /// notifies resources reservation request granted OK to the OMXIL component
  void CpNotifyResReservationGranted();
  /// notifies resources reservation request rejected KO to the OMXIL component
  void CpNotifyResReservationRejected();
  /// notifies error to the OMXIL component
  void CpNotifyError(RM_STATUS_E nError);

  /////////////////////

  /// ctor
  explicit ORPComp(const OMX_PTR pCompHdl);
  /// dtor
  virtual ~ORPComp();

 private:
  ///////////////////// statics

  /// global counter-'date' incremented each time a resource is granted to a component
  static unsigned int msGlobalResourceDate;
  /// global index incremented for each component created
  /// => needed to distinguish components with same role
  static unsigned int msCompIndex;

  ///////////////////// identification basis

  /// component unique index
  unsigned int mIndex;

  /// OMXIL component role provided by capabilities
  char mOmxilRole[OMX_MAX_STRINGNAME_SIZE];
  /// OMXIL component name provided by capabilities
  char mOmxilName[OMX_MAX_STRINGNAME_SIZE];
  /// capabilities bitmap based on RM_CAPBF_E
  unsigned int mCapBitmap;

  /// OMXIL component handle provided at registration time
  const OMX_PTR mOmxilCompHdl;
  /// callbacks to OMXIL component provided at registration time
  RM_CBK_T mCompCallbacks;

  ///////////////////// network related: connection lines

  /// network halfline id of the component;
  /// set to -1 if component is 'single' (not connected on any ports)
  int mNwHalflineId;

  /// place id in network halfline (starting from 1)
  int mNwHalflinePlaceId;
  /// place type in network halfline; bit0 set: front, bit1 set: back
  int mNwHalfinePlaceType;

  /// number of known/activated/used ports for component (<= msMAXCountPorts)
  unsigned int mCountPorts;

  /// table of 'OMXIL Resource Proxy' ports of the component
  struct ORPport {
    /// unique index for this port as used by OMXIL component
    unsigned int omxportindex;
    /// port direction: true for input or false for output port
    bool wayin;
    /// network halfline id of peer component for this port; -1 if not connected
    int peernwhlid;
    /// port control bitmap based on CF_PUB_E & CF_PRIV_E
    int controlbmp;
    /// ctor
    ORPport() : omxportindex(0), wayin(false), peernwhlid(-1), controlbmp(0) {}
  } mPorts[msMAXCountPorts];

  ///////////////////// policy

  /// OMXIL component group policy
  struct PolicyGroup {
    /// system policy priority of the component group
    unsigned int prioritysystem;
    /// client (component's owner) priority of the component group
    unsigned int priorityclient;
    /// ID of the component group
    unsigned int gid;
    /// process id
    uint64_t pid;
    /// thread id
    uint64_t tid;
    /**< TRUE if suspension is authorized for the component
     * (=='forced' PAUSE/IDLE state initiated by the RME);
     * as per OMXIL 1.x standard, suspension is unactivated by default */
    bool can_suspend;
    /// ctor
    PolicyGroup()
      :
        prioritysystem(0), priorityclient(0), gid(0),
        pid(0), tid(0),
        can_suspend(false) {}
  } mPolicyGroup;

  ///////////////////// states

  /// dual OMXIL / Resource states
  StatesDuo  mStatesORp;

  /// counter-'date' of last resource grant
  unsigned int mDateResourceGrant;
  /// counter-'date' backup of last resource grant (used in dynamic requests)
  unsigned int mDateResourceGrantBackup;
  /// date of last resource release command (full/partial)
  OMX_U64 mDateResReleaseCmd;

  ///////////////////// activity & operational states (bitmap)

  /// control bitmap based on CF_PUB_E & CF_PRIV_E
  unsigned int mControlsBmp;

  /// mutex for callbacks
  omxilosalservices::OmxILOsalMutex* mpMutexCp;

  ///////////////////// domains

  /// platform domain ids associated to the component
  struct DomIds {
    /// DDR-MPC processor SVA domain name -- valid value if != PND_NA
    PlatformDomains::PNDS_E PndProcSva;
    /// DDR-MPC processor SVA domain id -- valid value if != 0
    unsigned int IdProcSva;

    /// DDR-MPC processor SIA domain name -- valid value if != PND_NA
    PlatformDomains::PNDS_E PndProcSia;
    /// DDR-MPC processor SIA domain id -- valid value if != 0
    unsigned int IdProcSia;

    /// ESRAM HWpipe SVA domain name -- valid value if != PND_NA
    PlatformDomains::PNDS_E PndHwPipeSva;
    /// ESRAM (non scratch) HWpipe SVA domain id -- valid value if != 0
    unsigned int IdHwPipeSva;

    /// ESRAM HWpipe SIA domain name -- valid value if != PND_NA
    PlatformDomains::PNDS_E PndHwPipeSia;
    /// ESRAM (non scratch) HWpipe SIA domain id -- valid value if != 0
    unsigned int IdHwPipeSia;

    /// ESRAM HwXP70SIA SIA domain name -- valid value if != PND_NA
    PlatformDomains::PNDS_E PndHwXp70Sia;
    /// ESRAM (non scratch) HwXP70SIA SIA domain id -- valid value if != 0
    unsigned int IdHwXp70Sia;

    /// ctor
    DomIds()
      :
        PndProcSva(PlatformDomains::PND_NA),
        IdProcSva(0),
        PndProcSia(PlatformDomains::PND_NA),
        IdProcSia(0),
        PndHwPipeSva(PlatformDomains::PND_NA),
        IdHwPipeSva(0),
        PndHwPipeSia(PlatformDomains::PND_NA),
        IdHwPipeSia(0),
        PndHwXp70Sia(PlatformDomains::PND_NA),
        IdHwXp70Sia(0) {}
  } mDomains;

  ///////////////////// Resources Estimation Table

  /// resources estimation levels
  ResLevels mResEstimLevels;
  /// resources estimation levels backup (used in dynamic requests)
  ResLevels mResEstimLevelsBackup;

  /// Usecase context bitmap based on RM_UCCBF_E
  unsigned int mUccBitmap;
  /// Usecase context bitmap  backup (used in dynamic requests)
  unsigned int mUccBitmapBackup;

  /////////////////////
  // disallow default constructor
  ORPComp();
  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(ORPComp);
};

}  // namespace

#endif

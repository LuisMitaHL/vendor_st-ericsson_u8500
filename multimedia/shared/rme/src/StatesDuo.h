/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file StatesDuo.h
 * dual state class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _STATESDUO_H_
#define _STATESDUO_H_

#include <rme_types.h>

namespace rme {

/// dual states OMXIL/RESOURCES bitfields
enum DS_OR_E {
  // DS_NA is a reserved value : not to be mistaken with DS_INVALID
  DS_NA                  = 0,

  DS_INVALID             = (1UL <<  0),
  DS_LOADED_NONE         = (1UL <<  1),
  DS_LOADED_REQUIRED     = (1UL <<  2),
  DS_WAIT_REQUIRED       = (1UL <<  3),

  DS_IDLE_SATISFIED      = (1UL <<  4),
  DS_IDLE_REQUIRED       = (1UL <<  5),
  DS_IDLE_SUSPENDED      = (1UL <<  6),

  DS_PAUSE_SATISFIED     = (1UL <<  7),
  DS_PAUSE_REQUIRED      = (1UL <<  8),
  DS_PAUSE_SUSPENDED     = (1UL <<  9),

  DS_EXECUTING_SATISFIED = (1UL << 10),
  DS_EXECUTING_REQUIRED  = (1UL << 11)
};

/// transition state ids
/// confer RME's dual state machine
enum DS_TSI_E {
  DS_TSI_NA = 0,  /// keep to 0
  DS_TSI_UNLOAD_ALL_RESOURCES,  /// to LOADED,NONE or to INVALID; => unload all resources

  DS_TSI_WAITREQ,    /// from LOADED,NONE to WAIT,REQUIRED => waiting for resources attribution
  DS_TSI_LOADEDREQ,  /// from LOADED,NONE to LOADED,REQUIRED => requesting resources to go in IDLE

  DS_TSI_LOADEDREQ_TO_IDLESATISF,    /// after resources granted while in LOADED,REQ
  DS_TSI_WAITREQ_TO_IDLESATISF,      /// after resources granted while in WAIT,REQ
  DS_TSI_IDLEREQ_TO_IDLESATISF,      /// after resources granted while in IDLE,REQ
  DS_TSI_IDLESUSPEND_TO_IDLESATISF,  /// after resources granted while in IDLE,SUSPEND
  DS_TSI_PAUSESATISF_TO_IDLESATISF,      /// from PAUSE to IDLE while resources satisfied
  DS_TSI_EXECUTINGSATISF_TO_IDLESATISF,  /// from EXECUTING to IDLE while resources satisfied

  DS_TSI_IDLESATISF_TO_IDLEREQ,  /// DYNAMIC resources request while in IDLE,SATISF

  DS_TSI_IDLESATISF_TO_IDLESUSPEND,  /// after resources suspend while in IDLE,SATISF
  DS_TSI_IDLEREQ_TO_IDLESUSPEND,     /// after resources suspend while in IDLE,REQ
  DS_TSI_PAUSESUSPEND_TO_IDLESUSPEND,  /// from PAUSE to IDLE while resources suspended
  DS_TSI_EXECUTINGREQ_TO_IDLESUSPEND,     /// after resources suspend while in EXECUTING,REQ
  DS_TSI_EXECUTINGSATISF_TO_IDLESUSPEND,  /// after resources suspend while in EXECUTING,SATISF

  DS_TSI_IDLESATISF_TO_PAUSESATISF,  /// from IDLE to PAUSE while resources satisfied
  DS_TSI_LOADEDREQ_TO_PAUSESATISF,   /// hack shortcut: skipping IDLE,SATISF
  DS_TSI_EXECUTINGSATISF_TO_PAUSESATISF,  /// from EXECUTING to PAUSE while resources satisfied
  DS_TSI_PAUSEREQ_TO_PAUSESATISF,      /// after resources granted while in PAUSE,REQ
  DS_TSI_PAUSESUSPEND_TO_PAUSESATISF,  /// after resources granted while in PAUSE,SUSPEND

  DS_TSI_PAUSESATISF_TO_PAUSEREQ,  /// DYNAMIC resources request while in PAUSE,SATISF

  DS_TSI_IDLESUSPEND_TO_PAUSESUSPEND,  /// from IDLE to PAUSE while resources suspended
  DS_TSI_PAUSESATISF_TO_PAUSESUSPEND,  /// after resources suspend while in PAUSE,SATISF
  DS_TSI_PAUSEREQ_TO_PAUSESUSPEND,     /// after resources suspend while in PAUSE,REQ
  DS_TSI_EXECUTINGSATISF_TO_PAUSESUSPEND,  /// after resources suspend while in EXECUTING,SATISF
  DS_TSI_EXECUTINGREQ_TO_PAUSESUSPEND,     /// after resources suspend while in EXECUTING,REQ

  DS_TSI_IDLESATISF_TO_EXECUTINGSATISF,  /// from IDLE to EXECUTING while resources satisfied
  DS_TSI_LOADEDREQ_TO_EXECUTINGSATISF,   /// hack shortcut: skipping IDLESTATISFIED
  DS_TSI_PAUSESATISF_TO_EXECUTINGSATISF,  /// from PAUSE to EXECUTING while resources satisfied
  DS_TSI_EXECUTINGREQ_TO_EXECUTINGSATISF,  /// after resources granted while in EXECUTING,REQ

  DS_TSI_EXECUTINGSATISF_TO_EXECUTINGREQ,  /// DYNAMIC resources request while in EXECUTING,SATISF

  // actionless TSIs:
  DS_TSI_PAUSEREQ_TO_IDLEREQ,  /// from PAUSE to IDLE while DYNAMIC resources request
  DS_TSI_EXECUTINGREQ_TO_IDLEREQ,  /// from EXECUTING to IDLE while DYNAMIC resources request
  DS_TSI_IDLEREQ_TO_PAUSEREQ,  /// from IDLE to PAUSE while DYNAMIC resources request
  DS_TSI_EXECUTINGREQ_TO_PAUSEREQ,  /// from EXECUTING to PAUSE while DYNAMIC resources request
  DS_TSI_IDLEREQ_TO_EXECUTINGREQ,  /// from IDLE to EXECUTING while DYNAMIC resources request
  DS_TSI_PAUSEREQ_TO_EXECUTINGREQ,  /// from PAUSE to EXECUTING while DYNAMIC resources request
};

/** dual {OMXIL, Resource} state class */
class StatesDuo {
 public:
  /// returns OMXIL state
  OMX_STATETYPE OmxState() const { return mStOmx; }
  /// returns Resources state
  RM_RESOURCESTATE_E ResState() const { return mStRes; }
  /// returns Dual OMXIL/Resources state
  DS_OR_E DsOR() const { return mDualStateOR; }

  /// returns OMXIL state string
  static const char* StringifyOmx(OMX_STATETYPE Omx) {
    switch (Omx) {
      case OMX_StateInvalid:    return "invalid";
      case OMX_StateLoaded:     return "loaded";
      case OMX_StateWaitForResources: return "waitforresources";
      case OMX_StateIdle:       return "idle";
      case OMX_StatePause:      return "pause";
      case OMX_StateExecuting:  return "executing";
      default: return "omx_";
    }
  }

  /// returns Resources state string
  static const char* StringifyRes(RM_RESOURCESTATE_E Res) {
    switch (Res) {
      case RM_RS_NONE:        return "none";
      case RM_RS_REQUIRED:    return "required";
      case RM_RS_SATISFIED:   return "satisfied";
      case RM_RS_SUSPENDED:   return "suspended";
      default: return "omx_";
    }
  }

  /// returns Dual OMXIL/Resources state string
  static const char* StringifyDsOR(DS_OR_E DualStateOR) {
    switch (DualStateOR) {
      case DS_NA:                  return "ds_na";
      case DS_INVALID:             return "ds_invalid";
      case DS_LOADED_NONE:         return "ds_loaded_none";
      case DS_LOADED_REQUIRED:     return "ds_loaded_required";
      case DS_WAIT_REQUIRED:       return "ds_wait_required";
      case DS_IDLE_SATISFIED:      return "ds_idle_satisfied";
      case DS_IDLE_REQUIRED:       return "ds_idle_required";
      case DS_IDLE_SUSPENDED:      return "ds_idle_suspended";
      case DS_PAUSE_SATISFIED:     return "ds_pause_satisfied";
      case DS_PAUSE_REQUIRED:      return "ds_pause_required";
      case DS_PAUSE_SUSPENDED:     return "ds_pause_suspended";
      case DS_EXECUTING_SATISFIED: return "ds_executing_satisfied";
      case DS_EXECUTING_REQUIRED:  return "ds_executing_required";
      default: return "ds_";
    }
  }

  /// updates all states
  void UpdateDsOR(OMX_STATETYPE somxil, RM_RESOURCESTATE_E sres) {
    DS_OR_E l_ds_or = GetDualStateOmxRes(somxil, sres);
    if (l_ds_or != DS_NA) {
      mStOmx = somxil;
      mStRes = sres;
      mDualStateOR = l_ds_or;
    }
  }

  /// checks if state couple OMXIL / RES is valid
  /// and returns dual state value
  static DS_OR_E GetDualStateOmxRes(OMX_STATETYPE somxil,
                                    RM_RESOURCESTATE_E sres) {
    if (somxil == OMX_StateInvalid)
      return DS_INVALID;
    else if (sres == RM_RS_NONE)
      return DS_LOADED_NONE;  // forced as soon as resource state is NONE, whatever somxil
    else if ((somxil == OMX_StateLoaded) && (sres == RM_RS_REQUIRED))
      return DS_LOADED_REQUIRED;
    else if ((somxil == OMX_StateWaitForResources) && (sres == RM_RS_REQUIRED))
      return DS_WAIT_REQUIRED;
    else if ((somxil == OMX_StateIdle) && (sres == RM_RS_SATISFIED))
      return DS_IDLE_SATISFIED;
    else if ((somxil == OMX_StateIdle) && (sres == RM_RS_REQUIRED))
      return DS_IDLE_REQUIRED;
    else if ((somxil == OMX_StateIdle) && (sres == RM_RS_SUSPENDED))
      return DS_IDLE_SUSPENDED;
    else if ((somxil == OMX_StatePause) && (sres == RM_RS_SATISFIED))
      return DS_PAUSE_SATISFIED;
    else if ((somxil == OMX_StatePause) && (sres == RM_RS_REQUIRED))
      return DS_PAUSE_REQUIRED;
    else if ((somxil == OMX_StatePause) && (sres == RM_RS_SUSPENDED))
      return DS_PAUSE_SUSPENDED;
    else if ((somxil == OMX_StateExecuting) && (sres == RM_RS_SATISFIED))
      return DS_EXECUTING_SATISFIED;
    else if ((somxil == OMX_StateExecuting) && (sres == RM_RS_REQUIRED))
      return DS_EXECUTING_REQUIRED;
    else
      return DS_NA;
  }

  /// checks if requested transition to given dual state OMXIL / RES is valid;
  /// sets corresponding transition id and
  /// sets flag indicating if transition shall be checked
  /// (especially if 'required to satisfied' transition was granted)
  bool IsValidTransitionDsOR(DS_OR_E to_ds_or,
                             DS_TSI_E *ptransitionid,
                             bool *pcheck_reqtosatisf) const {
    if (ptransitionid == NULL)
      return false;
    if (pcheck_reqtosatisf == NULL)
      return false;

    *ptransitionid = DS_TSI_NA;
    *pcheck_reqtosatisf = false;

    // check transition to same state: nothing else to do
    if (mDualStateOR == to_ds_or)
      return true;

    // check other transitions
    switch (to_ds_or) {
      case DS_INVALID:
        if (mDualStateOR != DS_LOADED_NONE)
          *ptransitionid = DS_TSI_UNLOAD_ALL_RESOURCES;
        return true;
      case DS_LOADED_NONE:
        *ptransitionid = DS_TSI_UNLOAD_ALL_RESOURCES;
        if ((mStOmx != OMX_StatePause) && ((mStOmx != OMX_StateExecuting)))
          return true;
        /* else invalid transition from pause or executing to loaded
           (shall go through idle first);
           yet if a component goes this way,
           its resources shall be freed nethertheless...
           so set *ptransitionid even if transition is declared unvalid... */
        break;
      case DS_WAIT_REQUIRED:
        if (mDualStateOR == DS_LOADED_NONE) {
          *ptransitionid = DS_TSI_WAITREQ;
          return true;
        }
        break;
      case DS_LOADED_REQUIRED:
        if (mDualStateOR == DS_LOADED_NONE) {
          *ptransitionid = DS_TSI_LOADEDREQ;
          return true;
        }
        break;
      case DS_IDLE_SATISFIED:
        if (mDualStateOR == DS_LOADED_REQUIRED) {
          *ptransitionid = DS_TSI_LOADEDREQ_TO_IDLESATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
        } else if (mDualStateOR == DS_WAIT_REQUIRED) {
          *ptransitionid = DS_TSI_WAITREQ_TO_IDLESATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
        } else if (mDualStateOR == DS_IDLE_REQUIRED) {
          *ptransitionid = DS_TSI_IDLEREQ_TO_IDLESATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
        } else if (mDualStateOR == DS_IDLE_SUSPENDED) {
          *ptransitionid = DS_TSI_IDLESUSPEND_TO_IDLESATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
        } else if (mDualStateOR == DS_PAUSE_SATISFIED) {
          *ptransitionid = DS_TSI_PAUSESATISF_TO_IDLESATISF;
          return true;
        } else if (mDualStateOR == DS_EXECUTING_SATISFIED) {
          *ptransitionid = DS_TSI_EXECUTINGSATISF_TO_IDLESATISF;
          return true;
        }
        break;
      case DS_IDLE_REQUIRED:
        if (mDualStateOR == DS_IDLE_SATISFIED) {
          *ptransitionid = DS_TSI_IDLESATISF_TO_IDLEREQ;
          return true;
        } else if (mDualStateOR == DS_PAUSE_REQUIRED) {
          *ptransitionid = DS_TSI_PAUSEREQ_TO_IDLEREQ;
          return true;
        } else if (mDualStateOR == DS_EXECUTING_REQUIRED) {
          *ptransitionid = DS_TSI_EXECUTINGREQ_TO_IDLEREQ;
          return true;
        }
        break;
      case DS_IDLE_SUSPENDED:
        if (mDualStateOR == DS_IDLE_SATISFIED) {
          *ptransitionid = DS_TSI_IDLESATISF_TO_IDLESUSPEND;
          return true;
        } else if  (mDualStateOR == DS_IDLE_REQUIRED) {
          *ptransitionid = DS_TSI_IDLEREQ_TO_IDLESUSPEND;
          return true;
        } else if (mDualStateOR == DS_PAUSE_SUSPENDED) {
          *ptransitionid = DS_TSI_PAUSESUSPEND_TO_IDLESUSPEND;
          return true;
        } else if  (mDualStateOR == DS_EXECUTING_REQUIRED) {
          *ptransitionid = DS_TSI_EXECUTINGREQ_TO_IDLESUSPEND;
          return true;
        } else if (mDualStateOR == DS_EXECUTING_SATISFIED) {
          *ptransitionid = DS_TSI_EXECUTINGSATISF_TO_IDLESUSPEND;
          return true;
        }
        break;
      case DS_PAUSE_SATISFIED:
        if (mDualStateOR == DS_IDLE_SATISFIED) {
          *ptransitionid = DS_TSI_IDLESATISF_TO_PAUSESATISF;
          return true;
        } else if (mDualStateOR == DS_LOADED_REQUIRED) {
          // hack shortcut: skipping transition to IDLE,SATISF
          *ptransitionid = DS_TSI_LOADEDREQ_TO_PAUSESATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
        } else if (mDualStateOR == DS_EXECUTING_SATISFIED) {
          *ptransitionid = DS_TSI_EXECUTINGSATISF_TO_PAUSESATISF;
          return true;
        } else if (mDualStateOR == DS_PAUSE_REQUIRED) {
          *ptransitionid = DS_TSI_PAUSEREQ_TO_PAUSESATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
       } else if (mDualStateOR == DS_PAUSE_SUSPENDED) {
          *ptransitionid = DS_TSI_PAUSESUSPEND_TO_PAUSESATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
        }
        break;
      case DS_PAUSE_REQUIRED:
        if (mDualStateOR == DS_PAUSE_SATISFIED) {
          *ptransitionid = DS_TSI_PAUSESATISF_TO_PAUSEREQ;
          return true;
        } else if (mDualStateOR == DS_IDLE_REQUIRED) {
          *ptransitionid = DS_TSI_IDLEREQ_TO_PAUSEREQ;
          return true;
        } else if (mDualStateOR == DS_EXECUTING_REQUIRED) {
          *ptransitionid = DS_TSI_EXECUTINGREQ_TO_PAUSEREQ;
          return true;
        }
        break;
      case DS_PAUSE_SUSPENDED:
        if (mDualStateOR == DS_IDLE_SUSPENDED) {
          *ptransitionid = DS_TSI_IDLESUSPEND_TO_PAUSESUSPEND;
          return true;
        } else if (mDualStateOR == DS_PAUSE_SATISFIED) {
          *ptransitionid = DS_TSI_PAUSESATISF_TO_PAUSESUSPEND;
          return true;
        } else if (mDualStateOR == DS_PAUSE_REQUIRED) {
          *ptransitionid = DS_TSI_PAUSEREQ_TO_PAUSESUSPEND;
          return true;
        } else if (mDualStateOR == DS_EXECUTING_SATISFIED) {
          *ptransitionid = DS_TSI_EXECUTINGSATISF_TO_PAUSESUSPEND;
          return true;
        } else if  (mDualStateOR == DS_EXECUTING_REQUIRED) {
          *ptransitionid = DS_TSI_EXECUTINGREQ_TO_PAUSESUSPEND;
          return true;
        }
        break;
      case DS_EXECUTING_SATISFIED:
        if (mDualStateOR == DS_IDLE_SATISFIED) {
          *ptransitionid = DS_TSI_IDLESATISF_TO_EXECUTINGSATISF;
          return true;
        } else if (mDualStateOR == DS_LOADED_REQUIRED) {
          // hack shortcut: skipping transition to IDLE,SATISF
          *ptransitionid = DS_TSI_LOADEDREQ_TO_EXECUTINGSATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
        } else if (mDualStateOR == DS_PAUSE_SATISFIED) {
          *ptransitionid = DS_TSI_PAUSESATISF_TO_EXECUTINGSATISF;
          return true;
        } else if (mDualStateOR == DS_EXECUTING_REQUIRED) {
          *ptransitionid = DS_TSI_EXECUTINGREQ_TO_EXECUTINGSATISF;
          // have to check RME authorized the transition
          *pcheck_reqtosatisf = true;
          return true;
        }
        break;
      case DS_EXECUTING_REQUIRED:
        if (mDualStateOR == DS_EXECUTING_SATISFIED) {
          *ptransitionid = DS_TSI_EXECUTINGSATISF_TO_EXECUTINGREQ;
          return true;
        } else if (mDualStateOR == DS_IDLE_REQUIRED) {
          *ptransitionid = DS_TSI_IDLEREQ_TO_EXECUTINGREQ;
          return true;
        } else if (mDualStateOR == DS_PAUSE_REQUIRED) {
          *ptransitionid = DS_TSI_PAUSEREQ_TO_EXECUTINGREQ;
          return true;
        }
        break;
      case DS_NA:
      default:  // shall not get here by construction
        break;
    }

    return false;
  }

  /// checks if dual state map provided has only Invalid, Loaded, Wait, states
  /// but no resources requests => excludes DS_LOADED_REQUIRED and SUSPENDED states
  static bool HasOnlyStatesWithNoActiveResourcesReq(unsigned int dualstatemap) {
    unsigned int ilwtmap = (DS_INVALID |
                            DS_LOADED_NONE |
                            DS_WAIT_REQUIRED);
    if (dualstatemap & ~ilwtmap)
      return false;
    else
      return true;
  }

  /// checks if dual state map provided has only Idle, Pause, Executing states
  static bool HasOnlyStatesWithResources(unsigned int dualstatemap) {
    unsigned int ipemap = (DS_IDLE_SATISFIED | DS_IDLE_REQUIRED | DS_IDLE_SUSPENDED |
                           DS_PAUSE_SATISFIED | DS_PAUSE_REQUIRED | DS_PAUSE_SUSPENDED |
                           DS_EXECUTING_SATISFIED | DS_EXECUTING_REQUIRED);
    if (dualstatemap & ~ipemap)
      return false;
    else
      return true;
  }

  /// checks if dual state map provided has only Executing states
  static bool HasOnlyStatesExecuting(unsigned int dualstatemap) {
    unsigned int emap = (DS_EXECUTING_SATISFIED | DS_EXECUTING_REQUIRED);
    if (dualstatemap & ~emap)
      return false;
    else
      return true;
  }

  /// ctor
  StatesDuo() :
    mStOmx(OMX_StateLoaded),
    mStRes(RM_RS_NONE),
    mDualStateOR(DS_LOADED_NONE)
  {}

 private:
  OMX_STATETYPE mStOmx;       /// OMXIL state
  RM_RESOURCESTATE_E mStRes;  /// Resources state
  DS_OR_E mDualStateOR;       /// Dual OMXIL/Resources state

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(StatesDuo);
};

}  // namespace

#endif


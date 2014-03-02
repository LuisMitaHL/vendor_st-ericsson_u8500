/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file  rme.h
 * RM engine class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _RME_H
#define _RME_H

#include "rme_types.h"

namespace rme {

/** Resource Management Engine :: INTERFACE+SINGLETON class */
class RME {
 public:
  //////////Component interface//////////

  /** registration of an OMXIL component with the RME
   * -call mode: SYNCHRONOUS
   *
   * @param[in] pCompHdl
   *      - 'handle' used to reference the OMX IL component; opaque data type for the RME
   *      - pCompHdl is provided back in all component callbacks
   *        for OS integration flexibility purposes
   * @param[in] pCompCbks
   *      - table of callbacks to be used by the RME to call the component
   *      - structure allocated & filled by the component, read by the RME
   *      - data life time: pCompCbks shall be valid along registration duration, i.e.
   *        until explicit return of Unregister()
   * @return status
   *      - RM_E_NONE if registered
   * */
  virtual RM_STATUS_E Register(OMX_IN const OMX_PTR pCompHdl,
      OMX_IN const RM_CBK_T* pCompCbks) = 0;

  /** unregistration of an OMXIL component from the RME
   * -call mode: SYNCHRONOUS
   * upon function return, callback functions provided to the RME
   * are guaranteed not to be used anymore
   * (i.e. can be safely destroyed along with the component)
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @return status
   *      - RM_E_NONE if unregistered
   * */
  virtual RM_STATUS_E Unregister(OMX_IN const OMX_PTR pCompHdl) = 0;

  /** notification to the RME of 2-uplet state of the component: OMXIL state AND resource state;
   * -implies resource request/wait from component to RME if the resource state is
   * 'required' or 'suspended' based
   * -call mode: ASYNCHRONOUS
   * -ASYNCHRONOUS processing completion signal:
   *      - if resource state is RM_RS_NONE or RM_RS_SATISFIED
   *        =>NONE
   *      - or if resource state is  RM_RS_REQUIRED, RM_RS_SUSPENDED
   *        => (*CP_Notify_ResReservationProcessed);
   *        -> the component has to wait for RME to grant or not the resources reservation;
   *          this waiting can  be interrupted by a compatible change state from the client
   *          (going to LOADED state usually)
   *
   * valid/expected state 2-uplets:
   *      {INVALID, ..}
   *      {LOADED, NONE}
   *      {LOADED, REQUIRED}
   *      {WAIT, REQUIRED}
   *      {IDLE, SATISFIED}
   *      {IDLE, REQUIRED}
   *      {IDLE, SUSPENDED}
   *      {PAUSE, SATISFIED}
   *      {PAUSE, REQUIRED}
   *      {PAUSE, SUSPENDED}
   *      {EXECUTING, SATISFIED}
   *      {EXECUTING, REQUIRED}
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @param[in] eOMXILState
   *      - OMXIL state of the component
   * @param[in] eResourceState
   *      - Resource state of the component
   * @param[in] nReserved
   *      - Reserved value
   * */
  virtual void Notify_NewStatesOMXRES(OMX_IN const OMX_PTR pCompHdl,
      OMX_IN OMX_STATETYPE eOMXILState, OMX_IN RM_RESOURCESTATE_E eResourceState,
      OMX_IN OMX_U32 nReserved) = 0;

  /** notification to the RME of new policy in the component;
   * following setting done by the OMX IL client on priority 'configuration' setting;
   * -call mode: ASYNCHRONOUS
   * -ASYNCHRONOUS processing completion signal: NONE
   *
   * @param[in] pCompHdl
   *      component 'handle' as provided at registration time
   * @param[in] nGroupPriority
   *      group priority
   * @param[in] nGroupID
   *      group id
   * @param[in] nQosBitmap
   *      QoS bitmap value: 0 (reserved value)
   * */
  virtual void Notify_NewPolicyCfg(OMX_IN const OMX_PTR pCompHdl,
      OMX_IN OMX_U32 nGroupPriority, OMX_IN OMX_U32 nGroupID,
      OMX_IN OMX_U32 nQosBitmap) = 0;

  /** notification to the RME of the result of a previous ASYNCHRONOUS command
   *  (*CP_Cmd_SetRMConfig) from the RME
   * -call mode: ASYNCHRONOUS
   * -ASYNCHRONOUS processing completion signal: NONE
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @param[in] eType
   *      - recall resource control type corresponding to the one provided
   *        in (*CP_Cmd_SetRMConfig) that is acknowledged
   * @param[in] nData
   *      - data: reserved value
   * @param[in] nErrorCode
   *      - if RM_E_NONE: no error => RM Config applied successfully
   *      else RM Config NOT applied
   * */
  virtual void Notify_RMConfigApplied(OMX_IN const OMX_PTR pCompHdl,
      OMX_IN RM_RC_E eType,
      OMX_IN OMX_S32 nData,
      OMX_IN RM_STATUS_E nErrorCode) = 0;

  //////////Cross/multi-components interface//////////

  /** notification to the RME of a successful tunnel between two ports
   * -notification of tunnel destruction is done through this call as well either with
   *   - peer component handle set to 0 or with
   *   - new peer component / port : tunnel reconfiguration
   * -call mode: ASYNCHRONOUS
   * -ASYNCHRONOUS processing completion signal: NONE
   *
   * @param[in] pCompHdlOut
   *      - component 'handle' out; for ST-Ericsson components:
   *        shall be same as provided at registration time
   * @param[in] nPortIndexOut
   *      - index of output port
   * @param[in] pCompHdlIn
   *      - component 'handle' in; for ST-Ericsson components:
   *        shall be same as provided at registration time
   * @param[in] nPortIndexIn
   *      - index of input port
   * */
  virtual void Notify_Tunnel(
      OMX_IN const OMX_PTR pCompHdlOut, OMX_IN OMX_U32 nPortIndexOut,
      OMX_IN const OMX_PTR pCompHdlIn, OMX_IN OMX_U32 nPortIndexIn) = 0;

  /** (deprecated API: WILL BE REMOVED)
   * -returns default NMF domains with DDR memory only : i.e. no ESRAM section associated
   * -valid domain types:
   *    - RM_NMFD_PROCSVA => SVA based domain,
   *    - RM_NMFD_PROCSIA => SIA based domain,
   * -call mode: SYNCHRONOUS
   * */
  virtual OMX_U32 Get_DefaultDDRDomain(OMX_IN RM_NMFD_E nDomainType) = 0;

  //////////Platform PRM service interface//////////

  /** platform specific: init PRM support
   * by providing callbacks for PRM services required by RME
   * -call mode: SYNCHRONOUS
   *
   * @param[in] NotifyInterconnectBwMode
   *      - callback to notify if interconnect settings for 1080p shall be used or not
   *      - parameter of NotifyInterconnectBwMode(): bUse1080pSettings
   *        if is OMX_TRUE: 1080p settings are REQUIRED
   * */
  virtual void InitSupport_PRM(
      OMX_IN void (*NotifyInterconnectBwMode)(OMX_IN OMX_BOOL bUse1080pSettings))
    = 0;

  //////////wrapper interface//////////

  /** RME proceeds much of its work during 'idle' time
   * this function shall be called by RME wrapper
   * before going to sleep / after processing of ALL immediately available messages
   * -call mode: SYNCHRONOUS
   *
   * @return timeout in millisecs
   *      - [timedpop] if timeout > 0 then wait with given timeout in millisecs
   *        for next message: on timeout, call idleT (no messages arrived)
   *      - [pop] if timeout == 0 then waits until new message(s) arrive
   *        process them all, until none left, then call idleT
   *      - [trypop] if timeout < 0 ==> not requested; fallback to pop
   * */
  virtual int idleT() = 0;

  /// get RME singleton instance and increment reference count (and create it on first call)
  static RME* getInstance();
  /// 'delete' RME singleton instance by decrementing reference count (and destroy it on last call)
  static void delInstance();

 protected:
  RME() {}
  virtual ~RME() {}

 private:
  // DISALLOW_COPY_AND_ASSIGN
  RME(const RME&);
  RME& operator=(const RME&);
};

}  // namespace rme

#endif


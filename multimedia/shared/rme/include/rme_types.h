/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file  rme_types.h
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _RMETYPES_H
#define _RMETYPES_H

#include "OMX_Types.h"
#include "OMX_Core.h"  // needed only for OMX_STATETYPE

// need to define followings, even if not used by rme: used by rme includers
#define IMPORT_C
#define EXPORT_C

/// RME namespace
namespace rme {

/********************************************************************************
 * //////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * ******************************************************************************/

/** @defgroup RMEd RME data
 */
/*@{*/

/** status and error codes */
typedef enum {
  RM_E_NONE = 0,    /// no error;
  RM_E_KO = -255,   /// failure -- generic
  RM_E_INVALIDPTR,  /// failure -- invalid pointer
  RM_E_NOMORE,      /// index overflow
  RM_E_STATES,      /// invalid states couple (omxil / resources)
  RM_E_STATESTR,    /// invalid states transition
  RM_E_EMDATA,      /// found some invalid parameters in estimation table
  RM_E_TIMEOUT,     /// component didnot acknowledge RME's config or command within some timeout
  RM_E_PRIORITY,    /// component priority setting error
  RM_E_INTERNAL = -128  /// RME internal error;
} RM_STATUS_E;

/********************************************************************************
 * //////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * ******************************************************************************/
/** @defgroup RMEst RME resources state machine
 * @ingroup RMEd
 */
/*@{*/

/** resources state machine,
 * used in correlation (more or less orthogonally) with OMXIL state machine */
typedef enum {
  /** No resources taken / required;
   * compatible OMX IL states:
   * - LOADED, INVALID : no resources taken / all resources freed
   * - IDLE, WAIT: freeing resources while moving to LOADED state */
  RM_RS_NONE,

  /** Resources required: TWO modes, STATIC/initial request and DYNAMIC request
   * - 'operational'/'static' (to reuse OMXIL specification wordings) resources required
   *   i.e. resources required to go in IDLE state;
   *   compatible OMX IL states:
   *   - WAIT, LOADED ('for transitioning to' IDLE)
   * - 'dynamic'/'new' resources request after a first previous resources request was satisfied;
   *   note that it might imply both releases and allocations of resources
   *   compatible OMX IL states:
   *   - IDLE, PAUSE, EXECUTING */
  RM_RS_REQUIRED,

  /** Resources satisfied:
   * can be set only after last resources requirement of the OMXIL component
   * was granted OK by the RME, through (*CP_Notify_ResReservationProcessed),
   * *AND* after resources were successfully allocated by the OMXIL component;
   * compatible OMX IL states:
   * - IDLE, PAUSE, EXECUTING */
  RM_RS_SATISFIED,

  /** Resources previously satisfied were suspended (and some possibly unallocated);
   * resources are required to go back in IDLE or in PAUSE in RM_RS_SATISFIED state
   * compatible OMX IL states:
   * - IDLE, PAUSE */
  RM_RS_SUSPENDED
} RM_RESOURCESTATE_E;

/*@}*/

/********************************************************************************
 * //////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * ******************************************************************************/
/** @defgroup RMEcap RME Capabilities
 * @ingroup RMEd
 */
/*@{*/

/** RM capabilities */
typedef struct RmCapabilities {
  /// STATIC/'LOAD TIME only' RM related capabilities
  /// => settings can be modified only in LOADED state (as per OMXIL standard)
  struct CapStatics {
    /// OMXIL component role: default one or one chosen/set by OMXIL client;
    OMX_U8 cRole[OMX_MAX_STRINGNAME_SIZE];
    /// OMXIL component name
    OMX_U8 cName[OMX_MAX_STRINGNAME_SIZE];

    /// specific capabilities bitmap based on RM_CAPBF_E bitfields
    OMX_U32 eCapBitmap;
  } sLoadTime;

  /// initial policy settings;
  /// can be dynamically updated during runtime through call to Notify_NewPolicyCfg()
  struct CapPolicy {
    /// ID of the component group
    OMX_U32 nGroupID;
    /// priority of the component group
    OMX_U32 nGroupPriority;

    /// TRUE if suspension is authorized for the component by the OMXIL client
    OMX_BOOL bCanSuspend;
  } sPolicy;

  /// additional STATIC identification to uniquely identify 'group id'
  /// in a multiprocess environment (group id unicity not assured then)
  struct CapIds {
    OMX_U64 nProcessId;  /// process id of component's client
    OMX_U64 nThreadId;   /// thread id of component's client
  } sIds;
} RM_CAPABILITIES_T;

/** capabilities bitfields */
typedef enum {
  /** (audio specific) low power control signal capability:
   * flag relevant only for OMXIL components with a
   * 'good' audio sink role (defined per platform);
   * it indicates that OMXIL component accepts/expects signals
   * from RME to set lowpower mode ON/OFF */
  RM_CAP_LOWPOWERCTRLSIG = (1UL << 0),

  /* distribution usage info: needed for domains id attribution
   * => OMXIL component can use SIA, SVA, both or none (ie ARM based) */

  /** SVA usage info: needed for SVA related domains id attribution */
  RM_CAP_DISTUSESVA = (1UL << 4),
  /** SIA usage info: needed for SIA related domains id attribution */
  RM_CAP_DISTUSESIA = (1UL << 5)
} RM_CAPBF_E;

// alias
typedef RM_CAPABILITIES_T::CapStatics RM_CAPSTA_T;
typedef RM_CAPABILITIES_T::CapPolicy  RM_CAPPOL_T;
typedef RM_CAPABILITIES_T::CapIds     RM_CAPIDS_T;

/*@}*/

/********************************************************************************
 * //////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * ******************************************************************************/
/** @defgroup RMEest RME estimation
 * @ingroup RMEd
 */
/*@{*/

/** MCPS unit resource description */
typedef struct McpsMA {
  OMX_U32 nMax;  /// maximum value
  OMX_U32 nAvg;  /// average value
} RM_RES_MCPS_T;

/** TCM unit resource description */
typedef struct TcmSizXY {
  OMX_U32 nSizeX;  /// TCM-X size in octets
  OMX_U32 nSizeY;  /// TCM-Y size in octets
} RM_RES_TCM_T;

/** ESRAM buffers 'fast' identification;
 *  exact understanding of buffers associated to the ids is platform dependant
 *  ==> confer related RME's platform domains header file: PlatformDomains.h */
typedef enum {
  /** 'empty' buffer (no buffer request)
   * => used for coherency and to force other values to be > 0 */
  RM_ESBUFID_EMPTY  = 0,

  RM_ESBUFID_DEFAULT,  /// 'default' buffer
  RM_ESBUFID_SMALL,    /// 'small' buffer
  RM_ESBUFID_BIG,      /// 'big' buffer
  RM_ESBUFID_BIGGEST,  /// 'biggest' buffer

  RM_ESBUFID_LAST,  /// TAG only => not refering to actual buffer; dont remove
} RM_ESRAMBUFID_E;

/** usecase context bitfields */
typedef enum {
  /** (visual specific) full HD resolution mode:
   * flag indicates that OMXIL component uses full HD resolution;
   * => information used for bandwidth limiter configuration / interconnect */
  RM_UCC_VISUAL_FHD = (1UL << 0),

  /** (audio specific) ESRAM usage for audio
   * flag indicates that OMXIL component requests audio ESRAM */
  RM_UCC_AUDIO_ESRAM = (1UL << 3)
} RM_UCCBF_E;

/** resources estimations from RM DB in the component */
typedef struct EmData {
  /* ******************* MCPS ********************
   * implicitely, MCPS resources are NOT taken during suspension (forced pause) of component */

  /// MCPS estimation on SVA
  RM_RES_MCPS_T sMcpsSva;
  /// MCPS estimation on SIA
  RM_RES_MCPS_T sMcpsSia;

  /* ******************* MEMORY: TCM banks X and Y ******************** */
  /// TCM size (X/Y) on SVA -- OBSOLETED : TCM no more supported
  RM_RES_TCM_T sTcmSva;
  /// TCM size (X/Y) on SIA -- OBSOLETED : TCM no more supported
  RM_RES_TCM_T sTcmSia;

  /* ******************* MEMORY: ESRAM ********************
   * */

  /** ESRAM 'HW' buffers: scratch and non scratch
   *  => a scratch buffer is used only when NMF component is executed on SxA;
   *    scratch buffer has following caracteristics:
   *    - on a given SxA, buffers can be used 'simultaneously' by multiple components
   *      (thanks to run-to-completion model)
   *    - section contains only DATA, and no CODE
   *    - only 2 types of scratch memory are defined corresponding to SIA-HWPIPE, SVA-HWPIPE
   *    - implicitely, resource is NOT taken during suspension (forced pause) of component
   *  => resource usage is given either 'classically' by providing a size
   *     OR is done by providing a 'fast' identification number based on RM_ESRAMBUFID_E;
   *     size and offset of each scratch buffer associated to a 'fast' id is platform specific
   *     and therefore not described here */
  struct EsramBufs {
    /// ESRAM SVA-HWPIPE 'SCRATCH' buffer:
    ///   size in octets (including alignment constraints)
    ///   or (prefered method) fast identification: one of the values in RM_ESRAMBUFID_E
    OMX_U32 nSvaHwPipe;
    /// ESRAM SIA-HWPIPE 'SCRATCH' buffer:
    ///   size in octets (including alignment constraints)
    ///   or (prefered method) fast identification: one of the values in RM_ESRAMBUFID_E
    OMX_U32 nSiaHwPipe;
    /// ESRAM SIA-XP70 (non-scratch) buffer:
    ///   size in octets (including alignment constraints)
    ///   or (prefered method) fast identification: one of the values in RM_ESRAMBUFID_E
    OMX_U32 nSiaHwXP70;
  } sEsramBufs;

  /* ******************* MEMORY: SDRAM ********************
   * DDR-MPC is *NOT* monitored by the RME;
   * */

  /* ******** specific resources usage ******** */

  /// resources usecase context bitmap => based on RM_UCCBF_E bitfields
  OMX_U32 eUccBitmap;

  /* */
} RM_EMDATA_T;

// alias
typedef RM_EMDATA_T::EsramBufs RM_ESRAMBUFS_T;

/*@}*/

/********************************************************************************
 * //////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * ******************************************************************************/
/** @defgroup RMErq RME resources request/control/release
 * @ingroup RMEd
 */
/*@{*/

/** Resource configuration control types */
typedef enum {
  RM_RC_NMF_DOMAINS,  /// command to set NMF domains ids to use
  RM_RC_AUDIOLOWPOWER_ENTER,  /// (audio specific) command to ENTER in LOW POWER mode
  RM_RC_AUDIOLOWPOWER_EXIT    /// (audio specific) command to EXIT from LOW POWER mode
} RM_RC_E;

/** RM control configurations set by RME to component */
typedef struct RmCtrlCfg {
  /// resource control type
  RM_RC_E eType;
  /// resource control parameters
  union Ctrl {
    /// RM_RC_NMF_DOMAINS related:
    /// provides NMF domain ids of different types;
    /// if a domain id value is 0, the domain is NOT valid and NOT to be used by the component
    struct NmfDomains {
      /// NMF SVA processor 'MMDSP' domain id;
      /// this domain shall be used for NMF components instantiation on SVA
      /// and for buffer allocations related to SVA in SDRAM or in TCM;
      /// this domain has SDRAM only (code and data)
      /// this domain has NO ESRAM
      OMX_U32 nDidProcSVA;
      /// NMF SIA processor 'MMDSP' domain id;
      /// this domain shall be used for NMF components instantiation on SVA
      /// and for buffer allocations related to SIA in SDRAM or in TCM;
      /// this domain has SDRAM only (code and data)
      /// this domain has NO ESRAM
      OMX_U32 nDidProcSIA;

      /// (visual specific) NMF SVA 'HWpipe' domain id;
      /// this domain shall be used for SCRATCH buffer allocations related to SVA HWpipe
      /// this domain has NO SDRAM
      /// this domain has ESRAM data only (no code!)
      OMX_U32 nDidHWPipeSVA;
      /// (visual specific) NMF SIA 'HWpipe' domain id;
      /// this domain shall be used for SCRATCH buffer allocations related to SIA HWpipe
      /// this domain has NO SDRAM
      /// this domain has ESRAM data only (no code!)
      OMX_U32 nDidHWPipeSIA;
      /// (visual specific) NMF SIA 'XP70' domain id;
      /// this domain shall be used for buffer allocations related to SIA XP70
      /// this domain has NO SDRAM
      /// this domain has ESRAM only (code and data)
      OMX_U32 nDidHWXP70SIA;

      /// reserved value (no more used; default value:-1)
      OMX_S32 nPortIndex;
    } sNMF;

    /// RM_RC_AUDIOLOWPOWER_ENTER related: (audio specific)
    struct LowPowerEnter {
      /// NMF ESRAM audio low power domain id to use (SIA+ESRAM only)
      /// if 0 (==invalid domain), migration SDRAM->ESRAM shall NOT be performed
      OMX_U32 nDidAudLP;
    } sLowPowerEnter;

    /// generic data
    OMX_S32 nData;
  } ctl;
} RM_SYSCTL_T;

// alias
typedef RM_SYSCTL_T::Ctrl::NmfDomains RM_NMFD_T;

/*@}*/

/********************************************************************************
 * //////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * ******************************************************************************/

/** callbacks used by the RME to call the component */
typedef struct RmCbks {
  /** retrieve component's current capabilities;
   * capabilities of the component can vary based on different parameters set by its OMXIL client
   * -call mode: synchronous (component shall minimize blocking time on this call)
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @param[out] pCapabilities
   *      - component capabilities;
   *      - structure allocated by the RME, filled by the component
   * @return status
   *      - if RM_E_NONE, command was successful (valid capabilities set)
   * */
  RM_STATUS_E (*CP_Get_Capabilities)(OMX_IN const OMX_PTR pCompHdl,
      OMX_INOUT RM_CAPABILITIES_T* pCapabilities);

  /** retrieve component's resources estimation
   * -call mode: synchronous (component shall minimize blocking time on this call)
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @param[out] pEstimationData
   *      - resources estimation data;
   *      - structure allocated by the RME, filled by the component
   * @return status
   *      - if RM_E_NONE, command was successful (valid resources data set)
   * */
  RM_STATUS_E (*CP_Get_ResourcesEstimation)(OMX_IN const OMX_PTR pCompHdl,
      OMX_INOUT RM_EMDATA_T* pEstimationData);

  /** (imperative) command to the component to apply some RM-system related configuration parameters;
   * - this command is used:
   *      - for loadtime setting of NMF domains handles
   *      - for runtime setting of audio lowpower mode (on/off)
   *        (when supported by the component)
   * - note that loadtime means transitions to IDLE state from LOADED, WAIT states
   * - note that runtime change of NMF domains handles (implying core and/or memory migration) is NOT supported
   * - note that the RME can send different requests in parallel with different types (pRMcfg->eType);
   * -call mode: asynchronous
   * -asynchronous completion signal: Notify_RMConfigApplied()
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @param[in] pRMcfg
   *      - RM parameters to be applied by the component
   *      - structure allocated and filled by the RME, read by the component;
   *      - data life time is limited to function call
   *      => component needs to copy information it needs before returning from this function
   * */
  void (*CP_Cmd_SetRMConfig)(OMX_IN const OMX_PTR pCompHdl,
      OMX_IN const RM_SYSCTL_T* pRMcfg);

  /** command to the component to release its resources
   *  fully or partially based on OMXIL standard:
   *  - either ALL component's resources (by moving to LOADED state) if 'suspend' parameter is FALSE,
   *  - or, else, only resources of 'dynamic' nature
   *    (by going in suspension mode -in PAUSE or IDLE states-);
   *
   * -this command results in forcing change of component's resource state
   * -the component can *not* discard this command request => 'imperative' command
   *
   * -call mode: asynchronous
   * -asynchronous processing completion signal: Notify_NewStatesOMXRES() with adequate resource state;
   * note that Notify_NewStatesOMXRES() might be sent several times,
   * for instance following preemption command on an executing component
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @param[in] bSuspend
   *     - if TRUE component shall suspend itself (=>partial resources release),
   *       else must release/unload all its resources (=>full resources release)
   *       and go to loaded state, through idle state if needed, as per OMXIL specifications;
   *       if suspension is not possible, the component shall relinquish all its resources
   *       (RME will be made aware of component's choice based on state change notification)
   * */
  void (*CP_Cmd_ReleaseResource)(OMX_IN const OMX_PTR pCompHdl,
      OMX_IN OMX_BOOL bSuspend);

  /** notification to the component that a reservation for 'required' or 'suspended' resources
   * has been processed by the RME;
   * this request was done by the component using Notify_NewStatesOMXRES() with resource state
   * of type RM_RS_REQUIRED or RM_RS_SUSPENDED
   * (no notification sent back by the RME for types RM_RS_NONE or RM_RS_SATISFIED)
   * -call mode: asynchronous
   * -asynchronous processing completion signal: NONE
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @param[in] bResourcesReservGranted
   *      - if TRUE resources reservation was granted, else was rejected
   * */
  void (*CP_Notify_ResReservationProcessed)(OMX_IN const OMX_PTR pCompHdl,
      OMX_IN OMX_BOOL bResourcesReservGranted);

  /** notification to the component of various errors detected by the RME;
   * the intent of this notification is more for integration purposes;
   * it is expected that the OMXIL component moves to INVALID state
   * upon reception of this notification
   * -call mode: asynchronous
   * -asynchronous processing completion signal: NONE
   *
   * @param[in] pCompHdl
   *      - component 'handle' as provided at registration time
   * @param[in] nErrorId
   *      - error id
   * */
  void (*CP_Notify_Error)(OMX_IN const OMX_PTR pCompHdl,
      OMX_IN RM_STATUS_E nError);
} RM_CBK_T;

/*@}*/

/** NMF domain types
  deprecated API, no more used by RME itself,
  but kept for rme clients' conveniance) */
typedef enum {
  RM_NMFD_NA = 0,
  RM_NMFD_PROCSVA,
  RM_NMFD_PROCSIA,
  RM_NMFD_HWPIPESVA,
  RM_NMFD_HWPIPESIA,
  RM_NMFD_HWXP70SIA
} RM_NMFD_E;

}  // namespace rme

#endif

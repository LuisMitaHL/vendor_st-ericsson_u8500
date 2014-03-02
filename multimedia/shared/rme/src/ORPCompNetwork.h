/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ORPCompNetwork.h
 * ORPcomponents network class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _ORPCOMPNETWORK_H_
#define _ORPCOMPNETWORK_H_

#include "uosal.h"
#include "ORPComp.h"

namespace rme {

// forward
class ORPCompHalfline;

/** OMXIL resource-proxy (ORP) component network manager class */
class ORPCompNetwork {
 public:
  /// ctor
  ORPCompNetwork();
  /// dtor
  virtual ~ORPCompNetwork();

  ///////////////////// Components lifecycle

  /// adds a OMXIL resource proxy component (ORPComp) object
  /// built from registration of an OMXIL component
  /// specific call mode: SYNCHRONOUS from RME client (OMXIL component) context
  RM_STATUS_E RegisterComponent(const OMX_PTR pCompHdl, const RM_CBK_T* pCompCbks);
  /// marks component as unregistered
  /// specific call mode: SYNCHRONOUS from RME client (OMXIL component) context
  RM_STATUS_E UnregisterComponent(const OMX_PTR pCompHdl);

  /// retrieves component from OMXIL handle
  /// and if needed (on first use) initializes ORPComp
  ORPComp* GetComponent(const OMX_PTR pCompHdl, bool *pcompinitisdone = NULL);

  /// returns first component
  ORPComp* FirstComponent() const { return mListCompAll.FrontNode(); }
  /// returns last component
  ORPComp* LastComponent() const { return mListCompAll.BackNode(); }
  /// returns next component
  ORPComp* NextComponent(const ORPComp* pcomp) const { return mListCompAll.NextNode(pcomp); }
  /// returns previous component
  ORPComp* PrevComponent(const ORPComp* pcomp) const { return mListCompAll.PrevNode(pcomp); }

  /// inits ORPComp on first use; deletes it in case of init error (no retry)
  int InitComponentOnFirstUse(ORPComp* pcomp);

  /// removes all unregistered components
  void RemoveUnregComponents();

  ///////////////////// Components networks lines management

  /// sorts all components (in 'all components' list) per priority & date
  /// higher priority components come first
  /// for identical priorities, components with more recent resource date come first
  void SortComponents();

  /// retrieves halfline based on its (unique) id
  ORPCompHalfline* GetHalfline(unsigned int hlid) const;

  /// returns first halfline
  ORPCompHalfline* FirstHalfline() const { return mListHalflines.FrontNode(); }
  /// returns last halfline
  ORPCompHalfline* LastHalfline() const { return mListHalflines.BackNode(); }

  /// returns next halfline
  ORPCompHalfline* NextHalfline(const ORPCompHalfline* phalfline) const;
  /// returns previous halfline
  ORPCompHalfline* PrevHalfline(const ORPCompHalfline* phalfline) const;

  /// processes tunneling of two components
  RM_STATUS_E TunnelComponents(ORPComp* pcompout, unsigned int orpportout,
                               ORPComp* pcompin, unsigned int orpportin);
  /// processes untunneling of a component on an output port
  RM_STATUS_E UntunnelCompOut(ORPComp* pcompout, unsigned int orpportout,
      bool external = true);
  /// processes untunneling of a component on an input port
  RM_STATUS_E UntunnelCompIn(ORPComp* pcompin, unsigned int orpportin,
      bool external = true);

  /// updates tags on halflines when needed
  /// and removes empty halflines
  void UpdateHalflines(bool compute_depth = false);
  /// clears extern-tag on halflines
  void ClearExternTagHalflines();
  /// clears recursive marker on halflines
  void ClearRecursiveMarkerHalflines();
  /// computes depth recursively down from given halfline
  void UpdateHalflinesDepthDown(ORPCompHalfline* phalfline);

  /// counts components registered
  int CountComponents() { return (mListCompAll.Size() + mListCompWaitInit.Size()); }

  /// Ids of lists to which an ORPComp can belong to;
  /// based on ORPC_LIDS_E values
  enum ORPCN_LIDS_E {
    /// Id of list of components waiting for init (on first use)
    LID_COMP_ICL = ORPC_LID_0,
    /// Id of list of all initialized components
    LID_COMP_ACL = ORPC_LID_1,
    /// Id of list of components forming a halfline
    LID_COMP_HCL = ORPC_LID_2,
    /// custom id
    LID_COMP_CUSTOM = ORPC_LID_3
  };

 private:
  /** Network lines management
    a registered component can be at a given time in THREE lists:
    1. wait for init list :
      registered components not yet initialized: done on first use
    2. global 'big' list :
      list of ALL registered components - sorted per prio/date
    3. connection network list:
    -  halfline[i] list :
      all components in a halfline are tunneled
      and sorted in flow direction (out to in);
      a component inside a halfline have no others connections
      than its peer previous and next in the halfline;
      components at extremities of halflines may be connected to
      several other halflines (halflines interconnections),
      at input (resp output) ports for first (resp last)
      component of the halfline;
   */

  /// list of components waiting for init; id = LID_COMP_ICL
  TvdListHead<ORPCOMPVTTEMPLATE> mListCompWaitInit;

  /// list of ALL initialized components; id = LID_COMP_ACL
  TvdListHead<ORPCOMPVTTEMPLATE> mListCompAll;

  /// list of halflines: list of list of components connected forming a line of components
  /// components inside the line has no tunnelled connections to components outside the halfline
  /// (list of components has id = LID_COMP_HCL)
  TvdListHead<ORPCompHalfline> mListHalflines;

  /// halfline id generator
  unsigned int mHalflineIdGen;

  /// mutex for some network services
  omxilosalservices::OmxILOsalMutex* mpMutexNw;
  /// locks mutex
  void MutexNwLock()   { if (mpMutexNw) { mpMutexNw->MutexLock(); } }
  /// unlocks mutex
  void MutexNwUnlock() { if (mpMutexNw) { mpMutexNw->MutexUnlock(); } }

  /// removes from network and possibly destroy component proxy
  void UninitDelComponent(ORPComp* pcomp);

  /// creates and returns a halfline
  ORPCompHalfline* NewHalfline();

  /// checks components before tunneling
  int CheckCompsBeforeTunneling(ORPComp* pcompout, unsigned int orpportout,
                                 ORPComp* pcompin, unsigned int orpportin);
  /// splits a halfline after given component
  RM_STATUS_E SplitHalfLineAfter(unsigned int hlid, ORPComp* pcompout);
  /// splits a halfline before given component
  RM_STATUS_E SplitHalfLineBefore(unsigned int hlid, ORPComp* pcompin);
  /// merges halflines if possible
  int MergeHalflines(ORPCompHalfline* phalflineup, ORPCompHalfline* phalflinedown);

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(ORPCompNetwork);
};

}  // namespace

#endif

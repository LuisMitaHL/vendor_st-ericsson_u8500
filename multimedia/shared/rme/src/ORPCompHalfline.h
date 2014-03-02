/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ORPCompHalfline.h
 * ORPcomponent halfline class interface
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _ORPCOMPHALFLINE_H_
#define _ORPCOMPHALFLINE_H_

#include "uosal.h"
#include "ORPComp.h"
#include "ORPCompNetwork.h"

namespace rme {

/** Halfline entry (node) class */
class ORPCompHalfline : public TvdListNode<ORPCompHalfline> {
 public:
  /// ctor no default value
  explicit ORPCompHalfline(unsigned int hlid) :
    mHlId(hlid),
    mListCompHl(ORPCompNetwork::LID_COMP_HCL),
    mDepth(0),
    mExternTags(0),
    mRecursiveMarker(false) {
  }

  /// returns halfline id
  unsigned int HlId() const { return mHlId; }

  /// returns list of components
  TvdListHead<ORPCOMPVTTEMPLATE>* ListORPcomps() { return &mListCompHl; }
  /// counts components
  unsigned int CountComponents() const { return mListCompHl.Size(); }
  /// counts number of input connections
  unsigned int CountInputConnections() const;
  /// counts number of output connections
  unsigned int CountOutputConnections() const;
  /// updates components' place id in halfline (starting from 1)
  void UpdateCompPlaceIds();

  /// returns kth peer halfline downstream and sets associated ORP port index
  int PeerHalflineDown(unsigned int k, unsigned int *porpportindex) const;
  /// returns kth peer halfline upstream and sets associated ORP port index
  int PeerHalflineUp(unsigned int k, unsigned int *porpportindex) const;

  /// returns first component of halfline
  ORPComp* GetFirstComponent() const { return mListCompHl.FrontNode(); }
  /// returns last component of halfline
  ORPComp* GetLastComponent() const { return mListCompHl.BackNode(); }

  /// returns next component of halfline
  ORPComp* GetNextComponent(const ORPComp *pcomp) const {
    if ((pcomp == NULL) ||
        ((pcomp->NwHalflineId() >0) && ((unsigned int)pcomp->NwHalflineId() != mHlId)))
      return NULL;
    else
      return mListCompHl.NextNode(pcomp);
  }
  /// returns previous component of halfline
  ORPComp* GetPrevComponent(const ORPComp *pcomp) const {
    if ((pcomp == NULL) ||
        ((pcomp->NwHalflineId() >0) && ((unsigned int)pcomp->NwHalflineId() != mHlId)))
      return NULL;
    else
      return mListCompHl.PrevNode(pcomp);
  }

  /// returns depth
  unsigned int Depth() const { return mDepth; }
  /// sets depth
  void SetDepth(unsigned int depth) { mDepth = depth; }

  /// sorting comparaison method: compares per depth
  static int ComparePerDepth(const ORPCompHalfline* phlA,
                             const ORPCompHalfline* phlB);

  /// extern tag bitfields
  enum TAGEXTERN_E {
    HAS_DOWNSTREAM_ALPSINK = (1UL << 0),  /// has ALP sink downstream
    HAS_UPSTREAM_ALPSOURCE = (1UL << 1),  /// has ALP source upstream
  };
  /// resets platform external tags
  void ResetExternTags() { mExternTags = 0; }
  /// adds platform external tag
  void AddExternTag(TAGEXTERN_E tag) { mExternTags |= tag; }
  /// checks platform external tag
  bool HasExternTag(TAGEXTERN_E tag) const { return (mExternTags & tag); }

  /// returns recursive marker
  bool RecursiveMarker() const { return mRecursiveMarker; }
  /// sets recursive marker
  void SetRecursiveMarker() { mRecursiveMarker = true; }
  /// unsets recursive marker
  void UnsetRecursiveMarker() { mRecursiveMarker = false; }

 private:
  /// unique halfline id
  unsigned int mHlId;

  /// list of components forming a halfline
  TvdListHead<ORPCOMPVTTEMPLATE> mListCompHl;

  /// "depth" of halfline:
  /// 0 for halfline with no upper connection
  /// halfine with upper connections, take biggest depth of above halflines, incremented by 1
  unsigned int mDepth;

  /// platform extern-tag for this halfline; based on TAGEXTERN_E
  /// extern==computed based on analysis on network around halfline
  unsigned int mExternTags;

  /// recursive marker: protecting during recursive scan against loop patterns
  bool mRecursiveMarker;

  // disallow default constructor
  ORPCompHalfline();
  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(ORPCompHalfline);
};

}  // namespace

#endif

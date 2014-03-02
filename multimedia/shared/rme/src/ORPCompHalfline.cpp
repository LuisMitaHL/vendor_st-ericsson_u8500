/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file ORPCompHalfline.cpp
 * ORPcomponents halfline class
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/

#include "ORPCompHalfline.h"

namespace rme {

/// counts number of input connections
unsigned int ORPCompHalfline::CountInputConnections() const {
  ORPComp* pcomp = mListCompHl.FrontNode();
  if (pcomp)
    return pcomp->CountConnectionsOnPortIn();
  else
    return 0;
}

/// counts number of output connections
unsigned int ORPCompHalfline::CountOutputConnections() const {
  ORPComp* pcomp = mListCompHl.BackNode();
  if (pcomp)
    return pcomp->CountConnectionsOnPortOut();
  else
    return 0;
}

/// updates components' place id in halfline (starting from 1)
void ORPCompHalfline::UpdateCompPlaceIds() {
  ORPComp* pcompfront = mListCompHl.FrontNode();
  ORPComp* pcompback = mListCompHl.BackNode();
  ORPComp* pcomp = pcompfront;
  int placeid = 1;
  while (pcomp) {
    int placetype = 0;
    if (pcomp == pcompfront) {
        placetype |= (1 << 0);
    }
    if (pcomp == pcompback) {
        placetype |= (1 << 1);
    }
    pcomp->SetNwHalflinePlace(placeid, placetype);
    pcomp = mListCompHl.NextNode(pcomp);
    placeid++;
  }
}

/// returns kth peer halfline downstream and sets associated ORP port index
int ORPCompHalfline::PeerHalflineDown(unsigned int k,
                                 unsigned int *porpportindex) const {
  ORPComp* pcomp = mListCompHl.BackNode();
  if (pcomp == NULL)
      return -1;
  unsigned int countpeers = 0;
  for (unsigned int j = 0; j < pcomp->CountPorts(); j++) {
    if (pcomp->IsOutputPort(j)) {
      int peerhlid = pcomp->PortPeerNwHlId(j);
      if (peerhlid >= 0) {
        if (countpeers == k) {
          if (porpportindex)
            *porpportindex = j;
          return peerhlid;
        }
        countpeers++;
      }
    }
  }
  return -1;
}

/// returns kth peer halfline upstream and sets associated ORP port index
int ORPCompHalfline::PeerHalflineUp(unsigned int k,
                                 unsigned int *porpportindex) const {
  ORPComp* pcomp = mListCompHl.FrontNode();
  if (pcomp == NULL)
      return -1;
  unsigned int countpeers = 0;
  for (unsigned int j = 0; j < pcomp->CountPorts(); j++) {
    if (pcomp->IsInputPort(j)) {
      int peerhlid = pcomp->PortPeerNwHlId(j);
      if (peerhlid >= 0) {
        if (countpeers == k) {
          if (porpportindex)
            *porpportindex = j;
          return peerhlid;
        }
        countpeers++;
      }
    }
  }
  return -1;
}

/// sorting comparaison method: compares per depth
int ORPCompHalfline::ComparePerDepth(const ORPCompHalfline* phlA,
                                const ORPCompHalfline* phlB) {
  if ((phlA == NULL) || (phlB == NULL))
    return 0;

  if (phlA->Depth() < phlB->Depth()) {
    return 1;
  } else if (phlA->Depth() == phlB->Depth()) {
    return 0;
  } else {
    return -1;
  }
}

}  // namespace

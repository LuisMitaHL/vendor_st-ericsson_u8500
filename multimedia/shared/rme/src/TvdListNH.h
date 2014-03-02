/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/*! ****************************************************************************
 * \file TvdListNH.h
 * custom list management: TvdListNode & TvdListHead classes interfaces
 *
 * \author philippe.tribolo@stericsson.com
 *
 * *****************************************************************************/
#ifndef _TVECTDLISTNH_H_
#define _TVECTDLISTNH_H_

namespace rme {

/** typed and vectorized double-linked (aka TvdL) list node class
 => this class shall be used as base class for class needing special(ized) list support;
 this class describes a node belonging simultaneously to up-to VSIZ lists;
 for each list, a node has previous node, next node and payload data */
template <typename T, unsigned int VSIZ = 1>
class TvdListNode {
 public:
  /// checks if list with id ipos is valid for node : < VSIZ
  inline bool IsValid(unsigned int ipos) const {
    if (ipos >= VSIZ) {
      return false;
    } else {
      return true;
    }
  }

  /// checks if node for list ipos is used
  inline bool IsActive(unsigned int ipos) const {
    if (mTabNode[ipos].payload)
      return true;
    else
      return false;
  }

  // followings are trusted calls: no check on ipos vs VSIZ:
  // IsValid supposed to have been called before use

  /// returns previous node for list ipos
  inline TvdListNode<T, VSIZ>* Prev(unsigned int ipos) const {
    return mTabNode[ipos].prev;
  }
  /// sets previous node for list ipos
  inline void SetPrev(unsigned int ipos, TvdListNode<T, VSIZ>* prev) {
    mTabNode[ipos].prev = prev;
  }

  /// returns next node for list ipos
  inline TvdListNode<T, VSIZ>* Next(unsigned int ipos) const {
    return mTabNode[ipos].next;
  }
  /// sets next node for list ipos
  inline void SetNext(unsigned int ipos, TvdListNode<T, VSIZ>* next) {
    mTabNode[ipos].next = next;
  }

  /// returns current payload for list ipos
  inline T* Payload(unsigned int ipos) const {
    return mTabNode[ipos].payload;
  }
  /// sets current payload for list ipos
  inline void SetPayload(unsigned int ipos, T* payload) {
    mTabNode[ipos].payload = payload;
  }

  /// ctor
  TvdListNode() : mTabNode() {}  // will call VSIZ * LNode ctor
  /// dtor
  virtual ~TvdListNode() {}

 private:
  struct LNode {
    /// previous node for this list
    TvdListNode<T, VSIZ>* prev;
    /// next node for this list
    TvdListNode<T, VSIZ>* next;
    /// holds node pointer when node is valid/active for this list
    T* payload;
    /// ctor
    LNode() : prev(NULL), next(NULL), payload(NULL) {}
  };
  LNode mTabNode[VSIZ];  /// table of lists associated to this node

  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(TvdListNode);
};

/** typed and vectorized double-linked list head class */
template <typename T, unsigned int VSIZ = 1>
class TvdListHead {
 public:
  /// returns list Id
  unsigned int ListId() const { return mListId; }
  /// returns list Size
  unsigned int Size()   const { return mSize; }
  /// returns list front (first) node
  T* FrontNode()        const { return mFront; }
  /// returns list back (last) node
  T* BackNode()         const { return mBack; }

  /// returns previous node for given node
  T* PrevNode(const T* node) const {
    if (node == NULL)
      return NULL;
    if (node->IsValid(mListId) == false)
      return NULL;  // bad node
    if (node->IsActive(mListId) == false)
      return NULL;  // bad node

    const TvdListNode<T, VSIZ>* prevnode = node->Prev(mListId);
    if (prevnode)
      return prevnode->Payload(mListId);
    else
      return NULL;
  }

  /// returns next node for given node
  T* NextNode(const T* node) const {
    if (node == NULL)
      return NULL;
    if (node->IsValid(mListId) == false)
      return NULL;  // bad node
    if (node->IsActive(mListId) == false)
      return NULL;  // bad node

    const TvdListNode<T, VSIZ>* nextnode = node->Next(mListId);
    if (nextnode)
      return nextnode->Payload(mListId);
    else
      return NULL;
  }

  /// pushes node at front (of list)
  void PushFront(T* node) {
    if (node == NULL)
      return;
    if (node->IsValid(mListId) == false)
      return;  // bad node
    if (node->IsActive(mListId) == true)
      return;  // bad node

    node->SetPrev(mListId, NULL);
    node->SetNext(mListId, NULL);
    node->SetPayload(mListId, node);
    mSize++;

    if (mFront == NULL) {
      mFront = node;
      mBack = node;
    } else {
      T* node_was_first = mFront;
      node_was_first->SetPrev(mListId, node);
      node->SetNext(mListId, node_was_first);
      mFront = node;
    }
  }

  /// pushes node at back (of list)
  void PushBack(T* node) {
    if (node == NULL)
      return;
    if (node->IsValid(mListId) == false)
      return;  // bad node
    if (node->IsActive(mListId) == true)
      return;  // bad node

    node->SetPrev(mListId, NULL);
    node->SetNext(mListId, NULL);
    node->SetPayload(mListId, node);
    mSize++;

    if (mBack == NULL) {
      mFront = node;
      mBack = node;
    } else {
      T* node_was_last = mBack;
      node_was_last->SetNext(mListId, node);
      node->SetPrev(mListId, node_was_last);
      mBack = node;
    }
  }

  /// removes node (from list)
  int Remove(T* node) {
    if (node == NULL)
      return -1;
    if (node->IsValid(mListId) == false)
      return -2;  // bad node
    if (node->IsActive(mListId) == false)
      return -3;  // bad node

    TvdListNode<T, VSIZ>* node_prev = node->Prev(mListId);
    TvdListNode<T, VSIZ>* node_next = node->Next(mListId);

    if ((node_prev == NULL) && (node_next == NULL)) {
      // no more nodes: was mFront & mBack
      mFront = NULL;
      mBack = NULL;
    } else if (node_prev == NULL) {
      // was mFront node
      mFront = node_next->Payload(mListId);
      node_next->SetPrev(mListId, NULL);
    } else if (node_next == NULL) {
      // was mBack node
      mBack = node_prev->Payload(mListId);
      node_prev->SetNext(mListId, NULL);
    } else {
      node_prev->SetNext(mListId, node_next);
      node_next->SetPrev(mListId, node_prev);
    }

    mSize--;
    node->SetPrev(mListId, NULL);
    node->SetNext(mListId, NULL);
    node->SetPayload(mListId, NULL);

    return 0;
  }

  /// removes front node (from list)
  T* PopFront() {
    T* node = mFront;
    Remove(node);
    return node;
  }

  /// removes back node (from list)
  T* PopBack() {
    T* node = mBack;
    Remove(node);
    return node;
  }

  /// 'Merge'-sorts the list based on provided comparaison function
  /// decreasing sort: higher elements come first
  void Sort(int (*cmp)(const T*, const T*)) {
    TvdListNode<T, VSIZ>* node_front = mFront;
    if ((node_front == NULL) || (node_front->Next(mListId) == NULL))
      return;  // 0 or 1 element in list

    int mergesiz = 1;
    while (1) {
      TvdListNode<T, VSIZ>* node_left = node_front;
      node_front = NULL;
      TvdListNode<T, VSIZ>* node_back = NULL;

      int nbmerges = 0;

      while (node_left) {
        nbmerges++;  // one more merge to do

        // shift `mergesiz' nodes from node_left
        TvdListNode<T, VSIZ>* node_right = node_left;
        int leftsize = 0;
        for (int i = 0; i < mergesiz; i++) {
          leftsize++;
          node_right = node_right->Next(mListId);
          if (node_right == NULL)
            break;
        }
        int rightsize = mergesiz;

        // merge the two partial lists (if possible)
        // by design, node_left can be shifted (leftsize-1) times without being NULL
        // so in following while-loop, node_left can never be NULL
        while ((leftsize > 0) || ((rightsize > 0) && node_right)) {
          // to remove coverity false positive warning on node_left
          if (node_left == NULL)
            break;

          // choose next element of merge: from node_left or node_right
          TvdListNode<T, VSIZ>* node_merge;
          if (leftsize <= 0) {
            node_merge = node_right;
            // we have ((rightsize > 0) && node_right))
            node_right = node_right->Next(mListId);
            rightsize--;
          } else if ((rightsize == 0) || (node_right == NULL)) {
            node_merge = node_left;
            node_left = node_left->Next(mListId);
            leftsize--;
          } else if (cmp(node_left->Payload(mListId), node_right->Payload(mListId)) > 0) {
            // node_left is higher than node_right
            node_merge = node_left;
            node_left = node_left->Next(mListId);
            leftsize--;
          } else {
            // node_left is lower or equal than node_right
            node_merge = node_right;
            node_right = node_right->Next(mListId);
            rightsize--;
          }

          // append the element to the end of the merged list
          if (node_back) {
            node_back->SetNext(mListId, node_merge);
          } else {
            node_front = node_merge;
          }

          if (node_merge)
            node_merge->SetPrev(mListId, node_back);
          node_back = node_merge;
        }

        // node_left and node_right have shifted from `mergesiz' nodes; iterate
        node_left = node_right;
      }

      if (node_back)
        node_back->SetNext(mListId, NULL);

      // check for end
      if (nbmerges <= 1) {
        if (node_front)
          mFront = node_front->Payload(mListId);
        if (node_back)
          mBack = node_back->Payload(mListId);
        // the end
        break;
      } else {
        // repeat: merging lists with twice the size
        mergesiz *= 2;
      }
    }
  }

  /// ctor; no default value
  explicit TvdListHead(unsigned int id) :
    mListId(id),
    mSize(0),
    mFront(NULL),
    mBack(NULL) {}
  /// dtor
  virtual ~TvdListHead() {}

 private:
  const unsigned int mListId;   /// list unique id
  unsigned int mSize;     /// list size
  T* mFront;  /// first element
  T* mBack;   /// last element

  // disallow default constructor
  TvdListHead();
  // disallow copy and assign default constructors
  DISALLOW_COPY_AND_ASSIGN(TvdListHead);
};

}  // namespace

#endif

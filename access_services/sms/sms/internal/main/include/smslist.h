/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _LIST_H_
#define _LIST_H_
/**************************************************************************
 *
 * DESCRIPTION:
 *
 *  @brief
 *
 *  MSG_List package interface.
 *
 *    Provides a list handling package where you can:
 *    - create lists.
 *    - destroy lists.
 *    - put items in lists as void pointers.
 *    - remove items from lists.
 *    - access items through references.
 *    - access items by position.
 *    - access items by value.
 *    - iterate through lists.
 *
 *    The list is non-intrusive - it does not store any information
 *    in the items it holds. That means that an item can be contained
 *    in more than one list, or more than once in the same list,
 *    without any trouble.
 *
 *    Items are simply void pointers, which means that any data type
 *    that can be safely typecast to a void pointer and back can be
 *    stored in a list. This means that ints, but not function pointers
 *    can be stored.
 *
 *    MSG_Lists are index based, i.e an item in a list is represented
 *    by an index into that list. That index is valid as long as
 *    the list remains unchanged. Note that the index is NOT necessarily
 *    the position of the item in the list in the sense that the first
 *    item has index zero etc. The index is simply a placeholder
 *    in the list. The MSG_ListIndex type is an unsigned integer type
 *    used to represent an index.
 *
 *    Note: in the current implementation of the list, the list index
 *          is in fact the position of the item in the list, but this
 *          is an implementation detail, and should not be relied upon.
 *
 *    The index-based access methods allow you for example to get the
 *    index of the item following an item with a known index, and
 *    of course to get the item from the index.
 *
 *    However, there are other ways to access the item in a list.
 *    You can access the list based on the items themselves -
 *    e.g. the MSG_ListGetNext function will give you the item following
 *    the first occurence of a given item.
 *
 *    Another way is to access the items by order through the
 *    MSG_ListGetOrder function. Note that a list index is used as
 *    the data type for the order, but that here zero really
 *    means the first item, one the second, and so on.
 *
 *    Iterators provide a convenient way of traversing a list with
 *    minimal dependency on the implementation details of the list.
 *    A typical list traversal will look like this:
 *
 *      MSG_ListIterator listIterator;
 *
 *        for (MSG_ListIteratorSetMSG_List (&listIterator, list, FALSE);
 *            ! MSG_ListIteratorDone (&listIterator);
 *              MSG_ListIteratorNext (&listIterator))
 *        {
 *           MyDataType *item;
 *           item = (MyDataType *) MSG_ListIteratorGetItem (&listIterator));
 *           DoSomething (item);
 *        }
 *
 *     For simple list traversal, there is the MSG_ListIterate method,
 *     which uses two functions: one match function which decides which
 *     elements in the list should be acted on, and one action function
 *     which performs the action itself, e.g like this:
 *
 *       boolean MyMatchFunc (void *elt) {
 *         return (boolean) ((MyDataType *) elt)->type == SomeType;
 *       }
 *
 *       void MyActionFunc (void *elt) {
 *         DoSomething ((MyDataType *) elt);
 *       }
 *
 *       ...
 *
 *          MSG_ListIterate (list, MyMatchFunc, MyActionFunc);
 *
 *       ...
 *
 *     Note that the MSG_ListIterate function returns the number of
 *     elements matched. If the match function is NULL, all elements
 *     are matched. If the action function is NULL, the method works
 *     only as a counter. Should both functions be NULL, the result is
 *     equivalent to calling MSG_ListGetCount.
 *
 *     Error Handling
 *
 *     Index based access functions will return the NOPOS value if
 *     an item is not found. Item based access functions will return
 *     NULL. If you wish to store the NULL value in a list, it is
 *     therefore unwise to test for the end of the list by comparing
 *     with NULL. Use iterators instead.
 *
 *
 **************************************************************************
 */


 ////////////////////////////////////////////////////////////
 // class MSG_List

 // > 0 means elt1 > elt2, 0 means equal, < 0 means elt1 < elt2
typedef int (
    *MSG_ListCompareFunc) (
    void *elt1,
    void *elt2);

typedef uint8_t(
    *MSG_ListMatchFunc) (
    void *elt);
typedef void (
    *MSG_ListActionFunc) (
    void *elt);

typedef unsigned short MSG_ListIndex;
#define NOPOS 0xffff

typedef struct MSG_ListItem {
    void *data;
} MSG_ListItem;

typedef struct MSG_List {
    MSG_ListIndex maxCount;
    MSG_ListIndex count;
    MSG_ListIndex lastIndex;
    MSG_ListItem *item;
} MSG_List;


MSG_List *MSG_ListCreate(
    void);
void MSG_ListDestroy(
    MSG_List *);

void MSG_ListInsertItem(
    MSG_List * this_p,
    MSG_ListIndex,
    const void *);
MSG_ListIndex MSG_ListInsertFirst(
    MSG_List *,
    const void *);
MSG_ListIndex MSG_ListInsertLast(
    MSG_List *,
    const void *);
MSG_ListIndex MSG_ListInsertSorted(
    MSG_List *,
    const void *,
    MSG_ListCompareFunc);

MSG_ListIndex MSG_ListGetCount(
    const MSG_List *);

void *MSG_ListGetLast(
    MSG_List *);
void *MSG_ListGet(
    MSG_List *,
    MSG_ListIndex);
void *MSG_ListGetFirst(
    MSG_List *);
void *MSG_ListGetItem(
    MSG_List *,
    MSG_ListIndex order);
void *MSG_ListGetNext(
    MSG_List *,
    const void *);
void *MSG_ListGetPrev(
    MSG_List *,
    const void *);

void *MSG_ListRemove(
    MSG_List *,
    const void *);
void *MSG_ListRemoveItem(
    MSG_List * this_p,
    MSG_ListIndex);
void MSG_ListClear(
    MSG_List *);
MSG_ListIndex MSG_ListIterate(
    const MSG_List *,
    MSG_ListMatchFunc,
    MSG_ListActionFunc);
MSG_ListIndex MSG_ListGetFirstIndex(
    const MSG_List *);
MSG_ListIndex MSG_ListGetLastIndex(
    const MSG_List *);
MSG_ListIndex MSG_ListGetNextIndex(
    const MSG_List *,
    MSG_ListIndex);
MSG_ListIndex MSG_ListGetPrevIndex(
    const MSG_List *,
    MSG_ListIndex);
uint8_t MSG_ListCheckItemData(
    const MSG_List *,
    const void *);
MSG_ListIndex MSG_ListFindItem(
    const MSG_List * this_p,
    const void *data);
MSG_ListIndex MSG_ListFind(
    const MSG_List *,
    const void *,
    MSG_ListCompareFunc);

/*************************************************************************/
/* UNUSED FUNCTIONS */
/*************************************************************************/
// The following function have been commented out as they are no longer
// Used.  However they have been left in a comment section to prevent
// them being lost and hence re-written if they are needed in the future.
/*************************************************************************/

/*

MSG_ListIndex MSG_ListInsertBefore (MSG_List *, const void *, MSG_ListIndex);
MSG_ListIndex MSG_ListInsertAfter (MSG_List *, const void *, MSG_ListIndex);

void *MSG_ListRemoveFirst (MSG_List *);
void *MSG_ListRemoveLast (MSG_List *);

void *MSG_ListRemoveIndex (MSG_List *, MSG_ListIndex);
void MSG_ListRemoveSubRange (MSG_List *,
                             MSG_ListIndex first,
                             MSG_ListIndex last,
                             MSG_List *);
void MSG_ListMoveSubRange (MSG_List *,
                           MSG_ListIndex first,
                           MSG_ListIndex last,
                           MSG_ListIndex beforeIndex);

MSG_ListIndex MSG_ListGetIndex (const MSG_List *, const void *);
MSG_ListIndex MSG_ListGetOrder (const MSG_List *, const void *);
void MSG_ListGetSubRange (const MSG_List *,
                                MSG_ListIndex first,
                                MSG_ListIndex last,
                                MSG_List *);

typedef struct MSG_ListIterator
{
  const MSG_List *list;
  MSG_ListIndex index;
  boolean reverse;
} MSG_ListIterator;

void MSG_ListIteratorSetMSG_List (MSG_ListIterator *, const MSG_List *,boolean reverse);
void MSG_ListIteratorReset (MSG_ListIterator *);
void MSG_ListIteratorNext (MSG_ListIterator *);
void MSG_ListIteratorSetIndex (MSG_ListIterator *, MSG_ListIndex);

boolean MSG_ListIteratorDone (const MSG_ListIterator *);
void *MSG_ListIteratorGetItem (const MSG_ListIterator *);
MSG_ListIndex MSG_ListIteratorGetIndex (const MSG_ListIterator *);
*/
#endif                          // _LIST_H_

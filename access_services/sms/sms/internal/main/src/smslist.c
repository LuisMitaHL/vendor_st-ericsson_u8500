/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
 *
 * DESCRIPTION:
 *
 * The SMS Server list object.
 *
 *************************************************************************/

/* Utility Include Files */
#include <stdio.h>

/* Mobile Station Include Files */
#include "r_smslinuxporting.h"


/* Message Module Include Files */
#include "smutil.h"
#include "smslist.h"


////////////////////////////////////////////////////////////
// Methods

/********************************************************************/
/**
 *
 * @function     *MSG_ListCreate
 *
 * @description  Creates a new list
 *
 * @param        void
 *
 * @return       MSG_List
 */
/********************************************************************/
MSG_List *MSG_ListCreate(
    void)
{
    MSG_List *this_p = (MSG_List *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_List) * 1);

    if (NULL == this_p) {
        SMS_A_(SMS_LOG_E("MSG_ListCreate failed to allocate memory for list"));
    } else {
        this_p->maxCount = 0;
        this_p->count = 0;
        this_p->lastIndex = NOPOS;
        this_p->item = NULL;
    }

    return this_p;
}


/********************************************************************/
/**
 *
 * @function     FindNextSize
 *
 * @description  <Detailed description of the method>
 *
 * @param        oldSize : <add description>.
 *
 * @return       static MSG_ListIndex
 */
/********************************************************************/
static MSG_ListIndex FindNextSize(
    MSG_ListIndex oldSize)
{
    switch (oldSize) {
    case 0:
        return 3;
    case 3:
        return 6;
    case 6:
        return 10;
    default:
        return oldSize + 10;
    }
}



/********************************************************************/
/**
 *
 * @function     MSG_ListDestroy
 *
 * @description  Destroy the list
 *
 * @param        *this_p : The list to destroy
 *
 * @return       void
 */
/********************************************************************/
void MSG_ListDestroy(
    MSG_List * this_p)
{
    if (this_p->item != NULL) {
        SMS_HEAP_FREE(&this_p->item);
    }
    SMS_HEAP_FREE(&this_p);

}


/********************************************************************/
/**
 *
 * @function     MSG_ListInsertItem
 *
 * @description  Insert an item in the list
 *
 * @param        *this_p : Identifies the list
 * @param        index :   The index to insert at
 * @param        *data :   The item to be inserted
 *
 * @return       void
 */
/********************************************************************/
void MSG_ListInsertItem(
    MSG_List * this_p,
    MSG_ListIndex index,
    const void *data)
{
    MSG_ListIndex i;

    // Make sure that there is space
    if (this_p->count == this_p->maxCount) {
        // allocate more space - this is only done here
        MSG_ListItem *newItem;
        MSG_ListIndex oldMaxCount;
        MSG_ListIndex newMaxCount;

        oldMaxCount = this_p->maxCount;
        newMaxCount = FindNextSize(oldMaxCount);

        // allocate a new list
        newItem = (MSG_ListItem *) SMS_HEAP_UNTYPED_ALLOC(sizeof(MSG_ListItem) * newMaxCount);

        // Copy the old list
        for (i = 0; i < oldMaxCount; i++) {
            newItem[i] = this_p->item[i];
        }

        // Free the old list
        if (this_p->item != NULL) {
            SMS_HEAP_FREE(&this_p->item);
        }
        // Set the list
        this_p->item = newItem;

        // Update list
        this_p->maxCount = newMaxCount;
    }

    for (i = this_p->count; i > index; i--) {
        this_p->item[i] = this_p->item[i - 1];
    }

    this_p->item[index].data = (void *) data;
    this_p->count++;

    if (this_p->lastIndex != NOPOS && this_p->lastIndex >= index) {
        // An item before the last indexed item has been added
        // so adjust the last index
        this_p->lastIndex++;
    }
}


/********************************************************************/
/**
 *
 * @function     *MSG_ListRemoveItem
 *
 * @description  Removes an item from the list
 *
 * @param        *this_p : Identifies the list itself
 * @param        index :   Index of the item to be delete from the list
 *
 * @return       void
 */
/********************************************************************/
void *MSG_ListRemoveItem(
    MSG_List * this_p,
    MSG_ListIndex index)
{
    MSG_ListIndex i = 0;
    void *data = NULL;

    // Store data before it is overwritten - support out-of-range index
    if (index < this_p->count) {
        data = this_p->item[index].data;
        for (i = index + 1; i < this_p->count; i++) {
            this_p->item[i - 1] = this_p->item[i];
        }
        this_p->count--;

        if (this_p->count == 0) {
            // List is empty so reset last index
            this_p->lastIndex = NOPOS;
        } else if (this_p->lastIndex != NOPOS && this_p->lastIndex != 0 && this_p->lastIndex >= index) {
            // An item before the last indexed item has been removed
            // so adjust the last index
            this_p->lastIndex--;
        }
    }

    return data;
}


/********************************************************************/
/**
 *
 * @function     MSG_ListFindItem
 *
 * @description  Seaches for an item in the list
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Identifies the item to find in the list
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListFindItem(
    const MSG_List * this_p,
    const void *data)
{
    MSG_ListIndex resultIndex = NOPOS;

    // First check if the item is at the last cached position
    if (this_p->lastIndex != NOPOS && this_p->count != 0 && this_p->item[this_p->lastIndex].data == data) {
        resultIndex = this_p->lastIndex;
    } else {
        MSG_ListIndex index;

        // Item not at cached position so search the whole list
        for (index = 0; index < this_p->count; index++) {
            if (this_p->item[index].data == data) {
                // Item found
                resultIndex = index;
                break;
            }
        }
    }

    return resultIndex;
}



/********************************************************************/
/**
 *
 * @function     MSG_ListClear
 *
 * @description  Clears the list
 *
 * @param        *this_p : Identifies the list
 *
 * @return       void
 */
/********************************************************************/
void MSG_ListClear(
    MSG_List * this_p)
{
    // ERROR? - should free the item storage to save RAM
    this_p->lastIndex = NOPOS;
    this_p->count = 0;
}


/********************************************************************/
/**
 *
 * @function     MSG_ListInsertFirst
 *
 * @description  Inserts an item at the start of the list
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Item to be inserted
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListInsertFirst(
    MSG_List * this_p,
    const void *data)
{
    MSG_ListInsertItem(this_p, 0, data);
    return 0;
}


/********************************************************************/
/**
 *
 * @function     MSG_ListInsertLast
 *
 * @description  Insert an item at the end of the list
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   The item to be inserted
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListInsertLast(
    MSG_List * this_p,
    const void *data)
{
    MSG_ListInsertItem(this_p, this_p->count, data);
    return (MSG_ListIndex) (this_p->count - 1);
}

/********************************************************************/
/**
 *
 * @function     MSG_ListInsertSorted
 *
 * @description  Inserts an item into the list
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Item to be inserted
 * @param        cmpFunc : Compare function to determine index to insert at
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListInsertSorted(
    MSG_List * this_p,
    const void *data,
    MSG_ListCompareFunc cmpFunc)
{
    MSG_ListIndex index;

    // Find the index to insert on
    for (index = 0; index < this_p->count; index++) {
        if (cmpFunc((void *) data, this_p->item[index].data) <= 0) {
            break;
        }
    }

    // Now, 'index' is the position to store 'data' on It is in the range [0, this_p->count].
    MSG_ListInsertItem(this_p, index, data);
    return index;
}

/********************************************************************/
/**
 *
 * @function     MSG_ListRemove
 *
 * @description  Removes an item from the list
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Item to be removed from the list
 *
 * @return       void*
 */
/********************************************************************/
void *MSG_ListRemove(
    MSG_List * this_p,
    const void *data)
{
    // Note: MSG_ListFindItem returns NOPOS if data not found.
    //       MSG_ListRemoveItem returns NULL if index is out of range (e.g. NOPOS).
    return MSG_ListRemoveItem(this_p, MSG_ListFindItem(this_p, data));
}



/********************************************************************/
/**
 *
 * @function     MSG_ListGetCount
 *
 * @description  Gets the number of items in the list
 *
 * @param        *this_p : Identifies the list
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListGetCount(
    const MSG_List * this_p)
{
    return this_p->count;
}


/********************************************************************/
/**
 *
 * @function     *MSG_ListGetFirst
 *
 * @description  Gets first item fro the list
 *
 * @param        *this_p : Identifies the list.
 *
 * @return       void
 */
/********************************************************************/
void *MSG_ListGetFirst(
    MSG_List * this_p)
{
    if (this_p->count == 0) {
        // List is empty so reset last index
        this_p->lastIndex = NOPOS;

        return NULL;
    } else {
        this_p->lastIndex = 0;

        return this_p->item[0].data;
    }
}


/********************************************************************/
/**
 *
 * @function     *MSG_ListGetLast
 *
 * @description  Gets last item from the list
 *
 * @param        *this_p : Identifies the list.
 *
 * @return       void
 */
/********************************************************************/
void *MSG_ListGetLast(
    MSG_List * this_p)
{
    if (this_p->count == 0) {
        // List is empty so reset last index
        this_p->lastIndex = NOPOS;

        return NULL;
    } else {
        this_p->lastIndex = this_p->count - 1;

        return this_p->item[this_p->count - 1].data;
    }
}


/********************************************************************/
/**
 *
 * @function     *MSG_ListGet
 *
 * @description  Gets item at specified index from the list
 *
 * @param        *this_p : Identifies the list.
 * @param        index :   Index of item within the list to get
 *
 * @return       void
 */
/********************************************************************/
void *MSG_ListGet(
    MSG_List * this_p,
    MSG_ListIndex index)
{
    if (index < this_p->count) {
        this_p->lastIndex = index;

        return this_p->item[index].data;
    } else {
        return NULL;
    }
}


/********************************************************************/
/**
 *
 * @function     *MSG_ListGetNext
 *
 * @description  Get the next item in a list given a previous item
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Item in the list to get the next one off
 *
 * @return       void
 */
/********************************************************************/
void *MSG_ListGetNext(
    MSG_List * this_p,
    const void *data)
{
    MSG_ListIndex index;

    index = MSG_ListFindItem(this_p, data);

    if (index != NOPOS && index < this_p->count - 1) {
        this_p->lastIndex = index + 1;

        return this_p->item[index + 1].data;
    } else {
        return NULL;
    }
}


/********************************************************************/
/**
 *
 * @function     *MSG_ListGetPrev
 *
 * @description  Get the previous item in a list
 *
 * @param        *this_p : Identifies the list
 * @param        *data :   Item to get the previous item for
 *
 * @return       void
 */
/********************************************************************/
void *MSG_ListGetPrev(
    MSG_List * this_p,
    const void *data)
{
    MSG_ListIndex index;

    index = MSG_ListFindItem(this_p, data);

    if (index != NOPOS && index != 0) {
        this_p->lastIndex = index - 1;

        return this_p->item[index - 1].data;
    } else {
        return NULL;
    }
}


/********************************************************************/
/**
 *
 * @function     MSG_ListCheckItem
 *
 * @description  Checks if item is in the list
 *
 * @param        *this_p : Identifies the list
 * @param        *Data :   The item to check for in the list
 *
 * @return       void
 */
/********************************************************************/
uint8_t MSG_ListCheckItemData(
    const MSG_List * this_p,
    const void *Data)
{
    MSG_ListIndex Index;
    uint8_t Result = FALSE;

    if (this_p != NULL) {
        for (Index = 0; (Index < this_p->count) && (Result == FALSE); Index++) {
            if (this_p->item[Index].data == Data) {
                Result = TRUE;
            }
        }
    } else {
        SMS_B_(SMS_LOG_E("MSG_ListCheckItemData this_p is NULL"));
    }

    return (Result);
}

/********************************************************************/
/**
 *
 * @function     MSG_ListGetFirstIndex
 *
 * @description  Get index of the first item in the list
 *
 * @param        *this_p : Identifies the list
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListGetFirstIndex(
    const MSG_List * this_p)
{
    if (this_p->count == 0) {
        return NOPOS;
    } else {
        return 0;
    }
}


/********************************************************************/
/**
 *
 * @function     MSG_ListGetLastIndex
 *
 * @description  Get index of last item in the list
 *
 * @param        *this_p : Identifies the list
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListGetLastIndex(
    const MSG_List * this_p)
{
    if (this_p->count == 0) {
        return NOPOS;
    } else {
        return this_p->count - 1;
    }
}


/********************************************************************/
/**
 *
 * @function     MSG_ListGetNextIndex
 *
 * @description  Gets next index in the list from a given index
 *
 * @param        *this_p : Identifies the list
 * @param        index :   Index of current item to get next one
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListGetNextIndex(
    const MSG_List * this_p,
    MSG_ListIndex index)
{
    if (this_p->count == 0 || index == this_p->count - 1) {
        return NOPOS;
    } else {
        return index + 1;
    }
}


/********************************************************************/
/**
 *
 * @function     MSG_ListGetPrevIndex
 *
 * @description  Gets item of previous index in the list
 *
 * @param        *this_p : Identifies the list
 * @param        index :   Index of current item to get previous for
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListGetPrevIndex(
    const MSG_List * this_p,
    MSG_ListIndex index)
{
    if (this_p->count == 0 || index == 0) {
        return NOPOS;
    } else {
        return index - 1;
    }
}

/********************************************************************/
/**
 *
 * @function     *MSG_ListGetItem
 *
 * @description  Gets and item from the list given the index
 *
 * @param        *this_p : Identifies the list
 * @param        index :   The index of the item to get
 *
 * @return       void
 */
/********************************************************************/
void *MSG_ListGetItem(
    MSG_List * this_p,
    MSG_ListIndex index)
{
    if (index < this_p->count) {
        this_p->lastIndex = index;

        return this_p->item[index].data;
    } else {
        return NULL;
    }
}

/********************************************************************/
/**
 *
 * @function     MSG_ListIterate
 *
 * @description  Iterates throgh the list
 *
 * @param        *this_p : Identifies the list
 * @param        matchFunc : Function to determine if items match criteria
 * @param        actionFunc : The action function if the items match
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListIterate(
    const MSG_List * this_p,
    MSG_ListMatchFunc matchFunc,
    MSG_ListActionFunc actionFunc)
{
    MSG_ListIndex index;
    MSG_ListIndex count = 0;

    // Iterate through the list
    for (index = 0; index < this_p->count; index++) {
        // compare
        if (matchFunc == (MSG_ListMatchFunc) NULL || matchFunc(this_p->item[index].data)) {
            count++;
            if (actionFunc != (MSG_ListActionFunc) NULL) {
                actionFunc(this_p->item[index].data);
            }
        }
    }
    // Return no. of matches
    return count;
}

/********************************************************************/
/**
 *
 * @function     MSG_ListFind
 *
 * @description  Find index of first matching element.
 *               The compare function will be called with a list item
 *               as the first parameter and the search data as the second
 *               (thus, they need not be of the same type).
 *
 * @param        *this_p : Identifies the list
 * @param        *data   : Identifies the item to find in the list
 * @param        cmpFunc : Compare function to determine match with list item
 *
 * @return       MSG_ListIndex
 */
/********************************************************************/
MSG_ListIndex MSG_ListFind(
    const MSG_List * this_p,
    const void *data,
    MSG_ListCompareFunc cmpFunc)
{
    MSG_ListIndex index;

    // Iterate through the list
    for (index = 0; index < this_p->count; index++) {
        // compare
        if (cmpFunc(this_p->item[index].data, (void *) data) == 0) {
            return index;
        }
    }

    // Not found
    return NOPOS;
}

/*************************************************************************/
/* UNUSED FUNCTIONS */
/*************************************************************************/
// The following function have been removed, as they are not needed
// However, they are available in version 2 of this element on the
// cnh160541_r3a_dev branch in Clearcase or label CNH160541_R3A005
/*************************************************************************/
/*
void *MSG_ListRemoveItem (MSG_List *this_p, MSG_ListIndex);

MSG_ListIndex MSG_ListInsertBefore (MSG_List *, const void *, MSG_ListIndex);
MSG_ListIndex MSG_ListInsertAfter (MSG_List *, const void *, MSG_ListIndex);

void *MSG_ListRemoveFirst (MSG_List *);
void *MSG_ListRemoveLast (MSG_List *);

void *MSG_ListRemoveIndex (MSG_List *, MSG_ListIndex);
void MSG_ListClear (MSG_List *);
void MSG_ListRemoveSubRange (MSG_List *,
                             MSG_ListIndex first,
                             MSG_ListIndex last,
                             MSG_List *);
void MSG_ListMoveSubRange (MSG_List *,
                           MSG_ListIndex first,
                           MSG_ListIndex last,
                           MSG_ListIndex beforeIndex);

MSG_ListIndex MSG_ListGetFirstIndex (const MSG_List *);
MSG_ListIndex MSG_ListGetLastIndex (const MSG_List *);
MSG_ListIndex MSG_ListGetNextIndex (const MSG_List *, MSG_ListIndex);
MSG_ListIndex MSG_ListGetPrevIndex (const MSG_List *, MSG_ListIndex);
MSG_ListIndex MSG_ListGetIndex (const MSG_List *, const void *);
MSG_ListIndex MSG_ListGetOrder (const MSG_List *, const void *);
void MSG_ListGetSubRange (const MSG_List *,
                                MSG_ListIndex first,
                                MSG_ListIndex last,
                                MSG_List *);
void MSG_ListIteratorSetMSG_List (MSG_ListIterator *, const MSG_List *,uint8_t reverse);
void MSG_ListIteratorReset (MSG_ListIterator *);
void MSG_ListIteratorNext (MSG_ListIterator *);
void MSG_ListIteratorSetIndex (MSG_ListIterator *, MSG_ListIndex);

uint8_t MSG_ListIteratorDone (const MSG_ListIterator *);
void *MSG_ListIteratorGetItem (const MSG_ListIterator *);
MSG_ListIndex MSG_ListIteratorGetIndex (const MSG_ListIterator *);
*/

/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_List.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _ENS_LIST_H_
#define _ENS_LIST_H_

#include "ENS_Redefine_Class.h"

#include "ENS_macros.h"

/// @defgroup ens_list_class ENS List 
/// @ingroup ens_utility_classes
/// utility class for handling lists in ENS
/// @{

class ENS_ListItem;

/// Pointer to an ENS_ListItem
typedef ENS_ListItem * ENS_ListItem_p;

/// Concrete class for a list item
class ENS_ListItem {
    public:
        ENS_ListItem(void* pData, ENS_ListItem_p prev, ENS_ListItem_p next)
            : mData(pData), mPrev(prev), mNext(next) {};
        
        ENS_API_IMPORT ~ENS_ListItem() {};

        void * getData() const {
            return mData;
        }

        ENS_ListItem_p getPrev() {
            return mPrev;
        }
        
        const ENS_ListItem * getPrev() const {
            return mPrev;
        }
        
        ENS_ListItem_p getNext() {
            return mNext;
        }
        
        const ENS_ListItem * getNext() const {
            return mNext;
        }
        
        ENS_ListItem_p setPrev(ENS_ListItem_p item) {
            mPrev = item;
            return mPrev;
        }
        
        ENS_ListItem_p setNext(ENS_ListItem_p item) {
            mNext = item;
            return mNext;
        }
    private:
        void* mData;
        ENS_ListItem_p mPrev;
        ENS_ListItem_p mNext;
};


class ENS_List;

/// Pointer to an ENS_List
typedef ENS_List * ENS_List_p;

/// Concrete class for a list
class ENS_List {
    public:
        ENS_List(): mFirst(0), mLast(0) {};

        ENS_API_IMPORT ~ENS_List();
       
        ENS_API_IMPORT void clear();

        ENS_API_IMPORT int getSize() const;

        bool isEmpty() const {
            return (mFirst == 0);
        }
        
        ENS_ListItem_p getFirst() const {
            return mFirst;
        }

        ENS_ListItem_p getLast() const {
            return mLast;
        }


        ENS_API_IMPORT ENS_ListItem_p pushFront(void* pData);
        ENS_API_IMPORT ENS_ListItem_p pushBack(void* pData);

        ENS_API_IMPORT void* popFront();
        ENS_API_IMPORT void* popBack();

        ENS_API_IMPORT ENS_ListItem_p search(void* pData);

    private:
        ENS_ListItem_p mFirst;
        ENS_ListItem_p mLast;

        // private copy-constructor and assignment operator
        // to forbid their use
        ENS_List(const ENS_List &);
        ENS_List & operator= (const ENS_List &);
};

/// @}

#endif // _ENS_LIST_H_

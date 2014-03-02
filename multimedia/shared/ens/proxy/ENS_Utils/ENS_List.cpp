/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   ENS_List.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "ENS_List.h"


ENS_API_EXPORT ENS_List::~ENS_List() {
    ENS_List::clear();
}

ENS_API_EXPORT void ENS_List::clear() {
    ENS_ListItem_p item = mFirst, next_item;

    while (item) {
        next_item = item->getNext();
        delete item;
        item = next_item;
    }
    mFirst = 0;
    mLast = 0;
}

ENS_API_EXPORT int ENS_List::getSize() const {
    ENS_ListItem_p item = mFirst;
    int size = 0;

    while (item) {
        item = item->getNext();
        size++;
    }

    return size;
}

ENS_API_EXPORT ENS_ListItem_p ENS_List::pushFront(void* pData) {
    ENS_ListItem_p item = new ENS_ListItem(pData, 0, mFirst);
    if (!item) {
        return 0;
    }

    if (mFirst) {
        mFirst->setPrev(item);
    } else {
        mLast = item;
    }
    
    mFirst = item;

    return item;
}

ENS_API_EXPORT ENS_ListItem_p ENS_List::pushBack(void* pData) {
    ENS_ListItem_p item = new ENS_ListItem(pData, mLast, 0);

    if (!item) {
        return 0;
    }

    if (mLast) {
        mLast->setNext(item);
    } else {
        mFirst = item;
    }

    mLast = item;

    return item;
}

ENS_API_EXPORT void* ENS_List::popFront() {
    if (mFirst) {
        ENS_ListItem_p tmp = mFirst;
        void* pData = mFirst->getData();
        
        mFirst = tmp->getNext();

        if (mFirst) {
            mFirst->setPrev(0);
        } else { 
            mLast = 0;
        }

        delete tmp;
        return pData;
    }
    
    return 0;
}

ENS_API_EXPORT void* ENS_List::popBack() {
    if (mLast) {
        ENS_ListItem_p tmp = mLast;
        void* pData = mLast->getData();
        
        mLast = tmp->getPrev();
        
        if (mLast) {
            mLast->setNext(0);
        } else {
            mFirst = 0;
        }

        delete tmp;
        return pData;
    }

    return 0;
}

ENS_API_EXPORT ENS_ListItem_p ENS_List::search(void* pData) {
    ENS_ListItem_p item = mFirst;

    while (item) {
        if (item->getData() == pData) {
            return item;
        }
        item = item->getNext();
    }

    return 0;
}


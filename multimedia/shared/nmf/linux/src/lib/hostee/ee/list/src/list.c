/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <ee/list.nmf>

typedef struct {
    hMutex mutex;
    t_list_link *pFIRST;
    t_list_link *pLAST;
} t_list_imp;

/* define matching function */
typedef t_bool (*listMatchingFuntion)(t_list_link *pElem, void *args);
typedef void (*listExecuteFuntion)(t_list_link *pElem, void *args);

/* private functions declaration */
static void addToList(t_list_imp *pList, t_list_link *pElem);
static void removeFromList(t_list_imp *pList, t_list_link *pElem);

/* implement ee.api.list.itf */
hList METH(create)(t_bool isProtected)
{
    t_list_imp *pList;
    
    pList = allocator.alloc(sizeof(t_list_imp));
    if (pList == 0) {return (hList)0;}
    if (isProtected)
    {
        pList->mutex = eeMutexCreate();
        if (pList->mutex == 0)
        {
            allocator.free((void *) pList);
            return (hList)0;
        }
    }
    else {pList->mutex = 0;}
    pList->pFIRST = 0;
    pList->pLAST = 0;
    
    return (hList) pList;
}

void METH(destroy)(hList listHandle)
{
    t_list_imp *pList = (t_list_imp *) listHandle;
    
    if (pList->mutex) {eeMutexDestroy(pList->mutex);}
    allocator.free((void *) pList);
}

void METH(add)(hList listHandle, t_list_link *pElem)
{
    t_list_imp *pList = (t_list_imp *) listHandle;
    
    if (pList->mutex) {eeMutexLock(pList->mutex);}
    
    addToList(pList, pElem);
    
    if (pList->mutex) {eeMutexUnlock(pList->mutex);}
}

void METH(remove)(hList listHandle, t_list_link *pElem)
{
    t_list_imp *pList = (t_list_imp *) listHandle;
   
    if (pList->mutex) {eeMutexLock(pList->mutex);}
    
    removeFromList(pList, pElem);
    
    if (pList->mutex) {eeMutexUnlock(pList->mutex);}
}

/* if pMatchingFunction return true for an elem then pElem is not add and search stop else pElem is add to list */
t_bool METH(addNotMatching)(hList listHandle, t_list_link *pElem, void *pMatchingFunction, void *pMatchingFunctionArgs)
{
    t_list_imp *pList = (t_list_imp *) listHandle;
    listMatchingFuntion matchingFunction = (listMatchingFuntion) pMatchingFunction;
    t_list_link *pCurrent;
    t_bool res = 0;
    
    if (pList->mutex) {eeMutexLock(pList->mutex);}
    
    pCurrent = pList->pFIRST;
    while(pCurrent)
    {
        res = (*matchingFunction)(pCurrent, pMatchingFunctionArgs);
        if (res) {break;}
        pCurrent = pCurrent->pNext;
    }
    if (!res) {addToList(pList, pElem);}
    
    if (pList->mutex) {eeMutexUnlock(pList->mutex);}
    
    return res;
}

/* if pMatchingFunction return true for an elem then this elem is remove and search stop */
t_bool METH(removeMatching)(hList listHandle, void *pMatchingFunction, void *pMatchingFunctionArgs)
{
    t_list_imp *pList = (t_list_imp *) listHandle;
    listMatchingFuntion matchingFunction = (listMatchingFuntion) pMatchingFunction;
    t_list_link *pCurrent;
    t_bool res = 0;
    
    if (pList->mutex) {eeMutexLock(pList->mutex);}
    
    pCurrent = pList->pFIRST;
    while(pCurrent)
    {
        res = (*matchingFunction)(pCurrent, pMatchingFunctionArgs);
        if (res) {break;}
        pCurrent = pCurrent->pNext;
    }
    if (res) {removeFromList(pList, pCurrent);}
    
    if (pList->mutex) {eeMutexUnlock(pList->mutex);}
    
    return res;
}

void METH(execute)(hList listHandle, void *pExecuteFunction, void *pExecuteFunctionArgs)
{
    t_list_imp *pList = (t_list_imp *) listHandle;
    listExecuteFuntion executeFunction = (listExecuteFuntion) pExecuteFunction;
    t_list_link *pCurrent;
    
    if (pList->mutex) {eeMutexLock(pList->mutex);}
    
    pCurrent = pList->pFIRST;
    while(pCurrent)
    {
        (*executeFunction)(pCurrent, pExecuteFunctionArgs);
        pCurrent = pCurrent->pNext;
    }
    
    if (pList->mutex) {eeMutexUnlock(pList->mutex);}
}

t_uint32 METH(getNumberOfElem)(hList listHandle)
{
    t_list_imp *pList = (t_list_imp *) listHandle;
    t_list_link *pCurrent;
    t_uint32 res = 0;
    
    if (pList->mutex) {eeMutexLock(pList->mutex);}
    pCurrent = pList->pFIRST;
    while(pCurrent)
    {
        res++;
        pCurrent = pCurrent->pNext;
    }
    if (pList->mutex) {eeMutexUnlock(pList->mutex);}
    
    return res;
}

t_bool METH(isElem)(hList listHandle, t_list_link *pElem)
{
    t_list_imp *pList = (t_list_imp *) listHandle;
    t_list_link *pCurrent;
    t_bool isElem = 0;
    
    if (pList->mutex) {eeMutexLock(pList->mutex);}
    pCurrent = pList->pFIRST;
    while(pCurrent)
    {
        if (pCurrent == pElem) {isElem = 1;break;}
        pCurrent = pCurrent->pNext;
    }
    if (pList->mutex) {eeMutexUnlock(pList->mutex);}
    
    return isElem;
}

/* private functions */
static void addToList(t_list_imp *pList, t_list_link *pElem)
{
    if (!pList->pFIRST) {pList->pFIRST = pElem;}
    else {pList->pLAST->pNext = pElem;}
    pElem->pNext = 0;
    pList->pLAST = pElem;
}

static void removeFromList(t_list_imp *pList, t_list_link *pElem)
{
    t_list_link *pCurrent = pList->pFIRST;
    t_list_link *pPrev = 0;
    
    while(pCurrent)
    {
        if (pCurrent == pElem)
        {
            if (pPrev == 0)
            {
                //elem is the first in list
                pList->pFIRST = pCurrent->pNext;
                if (pList->pFIRST == 0) {pList->pLAST = 0;} //and was the only one
            }
            else
            {
                //elem is not the first
                pPrev->pNext = pCurrent->pNext;
                if (pPrev->pNext == 0) {pList->pLAST = pPrev;} //but was the last
            }
            pCurrent->pNext = 0;
            break;
        }
        pPrev = pCurrent;
        pCurrent = pCurrent->pNext;
    }
}


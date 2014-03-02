/*******************************************************************************
*
*    File name: ObjectList.h
*     Language: Visual C++
*  Description: Template class for manage list of object
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                     File ObjectList.h

#ifndef __OBJECT_LIST_H__
#define __OBJECT_LIST_H__

#pragma once
#if defined(_WIN32)
#include "WinApiWrappers.h"
#elif (defined(__linux__) || defined(__APPLE__))
#include "LinuxApiWrappers.h"
#include <stdio.h>
#include <stdlib.h>
#else
#error "Unknown target"
#endif

#define ID_STRING_LEN   25

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            Template class CObjectList

template < class TemplateClass > class CObjectList
{
public:
    CObjectList();
    virtual ~CObjectList();

    int Add(TemplateClass *pObject, const char *szObjectId, int nSubPart = -1);
    TemplateClass *Find(const char *szObjectId, int nSubPart = -1);
    int Release(TemplateClass *pObject);
    int GetNumberOfInstances(TemplateClass *pObject);

protected:
    struct TObjectList {
        TemplateClass *pObject;
        char szObjectId[ 25 + 1 ];
        int nSubPart;
        int nCounter;
        TObjectList *pNextObject;
    } *m_pObjectList;
    CCriticalSectionObject m_CriticalSectionObject;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                         CObjectList()
// Constructor
//
template < class TemplateClass >
CObjectList< TemplateClass >::CObjectList()
{
    m_pObjectList = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                        ~CObjectList()
// Destructor
//
template < class TemplateClass >
CObjectList< TemplateClass >::~CObjectList()
{
    CLockCS Lock(m_CriticalSectionObject);
    TObjectList *pCurrentObject;

    // Delete all object that is left.
    while (m_pObjectList != NULL) {
        pCurrentObject = m_pObjectList;
        m_pObjectList = pCurrentObject->pNextObject;
        free(pCurrentObject);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                 Add()
// FUNCTION
//    Add() -- Add object ID to the object list
//
// INPUT
//    szId    - ID of the object
//    pObject - Object to be assigned to the ID
//
// RESULT
//     0: Success
//     1: ID string is to long
//    -1: Unable to expand the list.
//    -2: SubParts doesn't match each other.
//
// NOTES
//    If no record with the same ID string exist, a new record will be added and the object pointer will be set.
//    If a record with the same ID string already exists in the array, the instance counter will be incremented.
//

template < class TemplateClass >
int CObjectList< TemplateClass >::Add(TemplateClass *pObject, const char *szObjectId, int nSubPart)
{
    CLockCS Lock(m_CriticalSectionObject);
    TObjectList *pCurrentObject;

    if (m_pObjectList == NULL) {
        m_pObjectList = (TObjectList *)malloc(sizeof(TObjectList));

        if (m_pObjectList == NULL) {
            return -1;
        }

        m_pObjectList->pObject = pObject;
        strcpy_s(m_pObjectList->szObjectId, szObjectId);
        m_pObjectList->nSubPart = nSubPart;
        m_pObjectList->nCounter = 1;
        m_pObjectList->pNextObject = NULL;
    } else { // Is the object allready created ?
        pCurrentObject = m_pObjectList;

        while (((strcmp(pCurrentObject->szObjectId, szObjectId) != 0) || (pCurrentObject->nSubPart != nSubPart)) && (pCurrentObject->pNextObject != NULL)) {
            pCurrentObject = pCurrentObject->pNextObject;
        }

        if ((_stricmp(pCurrentObject->szObjectId, szObjectId) == 0) && (pCurrentObject->nSubPart = nSubPart)) {
            pCurrentObject->nCounter++;    // Found the object!
        } else {
            // Add a new object to the end of the list.
            pCurrentObject->pNextObject = (TObjectList *)malloc(sizeof(TObjectList));

            if (pCurrentObject->pNextObject == NULL) {
                return -1;
            }

            pCurrentObject->pNextObject->pObject = pObject;
            strcpy_s(pCurrentObject->pNextObject->szObjectId, szObjectId);
            pCurrentObject->pNextObject->nSubPart = nSubPart;
            pCurrentObject->pNextObject->nCounter = 1;
            pCurrentObject->pNextObject->pNextObject = NULL;
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                             Release()
// FUNCTION
//    Release() -- Release reference to object from the object list
//
// INPUT
//    pObject - Object to release
//
// RESULT
//    The number of instances left for this object.
//    If negative the object doesn't exist in the list.
//
// NOTES
//    The instance counter for the entry is decremented.
//    If the instance counter reaches 0, the entry will be deleted from the list.
//

template < class TemplateClass >
int CObjectList< TemplateClass >::Release(TemplateClass *pObject)
{
    CLockCS Lock(m_CriticalSectionObject);
    TObjectList *pCurrentObject, *pLastObject;

    // Find the right object to delete.
    if (m_pObjectList != NULL) {
        pCurrentObject = m_pObjectList;
        pLastObject = NULL;

        while ((pCurrentObject->pObject != pObject) && (pCurrentObject->pNextObject != NULL)) {
            pLastObject = pCurrentObject;
            pCurrentObject = pCurrentObject->pNextObject;
        }

        if (pCurrentObject->pObject == pObject) {
            if (pCurrentObject->nCounter > 1) {
                pCurrentObject->nCounter--;
                return pCurrentObject->nCounter;
            } else {
                // Remove the current record in the global object table.
                if (pLastObject == NULL) {
                    m_pObjectList = pCurrentObject->pNextObject;
                } else {
                    pLastObject->pNextObject = pCurrentObject->pNextObject;
                }

                free(pCurrentObject);
                return 0;
            }
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                Find()
// FUNCTION
//    Find() -- Search for an object ID in the object list
//
// INPUT
//    szObjectId - ID of object
//
// RESULT
//    The object pointer to the object assigned to the ID or
//    NULL if the ID was not found
//

template < class TemplateClass >
TemplateClass *CObjectList< TemplateClass >::Find(const char *szObjectId, int nSubPart)
{
    CLockCS Lock(m_CriticalSectionObject);
    TObjectList *pCurrentObject;

    // Find the right object
    if (m_pObjectList != NULL) {
        pCurrentObject = m_pObjectList;

        while (((strcmp(pCurrentObject->szObjectId, szObjectId) != 0) || (pCurrentObject->nSubPart != nSubPart)) && (pCurrentObject->pNextObject != NULL)) {
            pCurrentObject = pCurrentObject->pNextObject;
        }

        if ((_stricmp(pCurrentObject->szObjectId, szObjectId) == 0) && (pCurrentObject->nSubPart = nSubPart)) {
            return pCurrentObject->pObject;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                GetNumberOfInstances()
// FUNCTION
//    GetNumberOfInstances() -- Search for an object ID in the object list
//
// INPUT
//    szObjectId - ID of object
//
// RESULT
//    The number of instances of the object assigned to the ID.
//    0 if the ID was not found.
//

template < class TemplateClass >
int CObjectList< TemplateClass >::GetNumberOfInstances(TemplateClass *pObject)
{
    CLockCS Lock(m_CriticalSectionObject);
    TObjectList *pCurrentObject;

    // Find the right object
    if (m_pObjectList != NULL) {
        pCurrentObject = m_pObjectList;

        while ((pCurrentObject->pObject != pObject) && (pCurrentObject->pNextObject != NULL)) {
            pCurrentObject = pCurrentObject->pNextObject;
        }

        if (pCurrentObject->pObject == pObject) {
            return pCurrentObject->nCounter;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

#endif

//                                                                                              End of file ObjectList.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

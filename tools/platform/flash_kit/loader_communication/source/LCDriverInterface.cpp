/******************************************************************************
*
*    File name: LCDriverInterface.cpp
*      Project: LoaderCommunicationDriver
*     Language: Visual C++
*  Description: Interface class.
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
******************************************************************************/

#include "LCDriverInterface.h"

CCriticalSectionObject CLCDriverInterface::InitializationCS;
CObjectList<CLCDriverMethods> CLCDriverInterface::ObjectList;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLCDriverInterface::CLCDriverInterface()
{
    isStarted = false;
    m_pObject = NULL;
    m_szObjectId[0] = '\0';
}

CLCDriverInterface::~CLCDriverInterface()
{
    CLockCS CsLock(InitializationCS);

    // Disconnect from the CLCDriverMethods object
    if (m_pObject != NULL) {
        if (ObjectList.Release(m_pObject) == 0) {
            delete m_pObject;
            m_pObject = NULL;
        }
    }
}

CLCDriverMethods *CLCDriverInterface::FindObject(const char *interfaceId)
{
    return ObjectList.Find(interfaceId);
}

void CLCDriverInterface::ReleaseObject(CLCDriverMethods *object)
{
    ObjectList.Release(object);
}

int CLCDriverInterface::AddObject(CLCDriverMethods *object, const char *interfaceId)
{
    return ObjectList.Add(object, interfaceId);
}

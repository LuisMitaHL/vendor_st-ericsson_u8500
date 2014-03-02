/*******************************************************************************
*
*  File name: CEventObjectObject.h
*  Language: Visual C++
*  Description: CEventObjectObject class declarations
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

#ifndef _CEVENTOBJECT_H
#define _CEVENTOBJECT_H

#include <semaphore.h>

#include "Types.h"
#include "CWaitableObject.h"

//class used to wrap the OS methods for even signaling
//implements Wait() method used when waiting for event to occur

class CEventObject : public CWaitableObject
{
public:
    CEventObject();
    virtual ~CEventObject();
    void SetEvent();
    void UnsetEvent();
    DWORD Wait(DWORD dwTimeout = INFINITE);
private:
#if defined(__APPLE__)
    sem_t *m_sem;
#elif defined(__linux__)
    sem_t m_sem;
#endif
};

#endif /* _CEVENTOBJECT_H */


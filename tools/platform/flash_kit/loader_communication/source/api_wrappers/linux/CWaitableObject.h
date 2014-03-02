/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _CWAITABLEOBJECT_H
#define _CWAITABLEOBJECT_H

#include "Types.h"

class CWaitableObject
{
public:
    CWaitableObject();
    virtual ~CWaitableObject();
    virtual DWORD Wait(DWORD dwTimeout = INFINITE) = 0;
private:

};

typedef CWaitableObject *HANDLE;

#endif /* _CWAITABLEOBJECT_H */


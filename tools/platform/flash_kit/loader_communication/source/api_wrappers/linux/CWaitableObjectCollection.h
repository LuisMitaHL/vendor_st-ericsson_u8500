/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#ifndef _CWAITABLEOBJECTCOLLECTION_H
#define _CWAITABLEOBJECTCOLLECTION_H

#include <vector>
#include "CWaitableObject.h"

using namespace std;

class CWaitableObjectCollection
{
public:
    CWaitableObjectCollection();
    virtual ~CWaitableObjectCollection();
    void Add(CWaitableObject *obj);
    CWaitableObject *Wait(DWORD dwTimeout = INFINITE);
private:
    vector<CWaitableObject *>m_objs;
};

#endif /* _CWAITABLEOBJECTCOLLECTION_H */


/*******************************************************************************
*
*    File name: LCDriverInterface.h
*      Project: LCDriver
*     Language: Visual C++
*  Description: Interface class.
*  Revision   : R1A
*
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
*******************************************************************************/

#ifndef _CLCDRIVERINTERFACE_H_
#define _CLCDRIVERINTERFACE_H_

#include "LCDriverMethods.h"
#include "ObjectList.h"

/// <summary>
/// Interface class for functions agains loader command.
/// </summary>
struct CLCDriverInterface {
public:
    /// <summary>
    /// Constructor.
    /// </summary>
    CLCDriverInterface();

    /// <summary>
    /// Destructor.
    /// </summary>
    virtual ~CLCDriverInterface();

    // Public data members
    CLCDriverMethods *m_pObject;
    char m_szObjectId[25];
    bool isStarted;

    static CCriticalSectionObject InitializationCS;

    static CLCDriverMethods *FindObject(const char *interfaceId);
    static void ReleaseObject(CLCDriverMethods *object);
    static int AddObject(CLCDriverMethods *object, const char *interfaceId);
private:
    static CObjectList<CLCDriverMethods> ObjectList;
};

#endif // _CLCDRIVERINTERFACE_H_

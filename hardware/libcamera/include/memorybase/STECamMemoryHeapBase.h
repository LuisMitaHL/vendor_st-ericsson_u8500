/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STECAMMEMORYHEAPBASE_H
#define STECAMMEMORYHEAPBASE_H

//System include
#include <stdint.h>
#include <binder/MemoryHeapBase.h>

namespace android {

class CamMemoryHeapBase : public MemoryHeapBase
{
public:
	CamMemoryHeapBase( int aFd, uint32_t aLogicalAddress, size_t aSize ) : MemoryHeapBase()
    {
        int fd =  dup(aFd);

        //check for error
        if(fd == -1) {
            DBGT_CRITICAL("Dup failed with error: %s", strerror(errno));
        }

        //init can't fail
        init(fd, (void *) aLogicalAddress, aSize, 0 , NULL);
    }
};

}

#endif // STECAMMEMORYHEAPBASE_H


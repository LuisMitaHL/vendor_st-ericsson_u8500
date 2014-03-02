/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _FILEIO_HPP_
#define _FILEIO_HPP_

#include <fileio_types.idt.h>

class fileio: public fileioTemplate
{
    public:
        fileio();
        ~fileio();
        /* interface collection fileio.api.request */
        virtual void test(void);
        virtual void size(const char* aFileName, t_uint32* aSizePtr);
        virtual void read(const char* aFileName, void* aDestBuffer, t_uint32 aNumBytes);
        virtual void write(const char* aFileName, const void* aSrcBuffer, t_uint32 aNumBytes);
        /* interface collection fileio.api.error */
        virtual const char* errorCode2String(t_fileio_error aErr);
};

#endif /* _FILEIO_HPP_ */

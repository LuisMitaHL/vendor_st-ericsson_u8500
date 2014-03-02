/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __SW_3A_TRACE_HPP__
#define __SW_3A_TRACE_HPP__

#include "sw_3A_types.hpp"

class CSw3ATraceFile
{
    public:
        CSw3ATraceFile(char *fileNameBase);
        ~CSw3ATraceFile();
        sw3A_error_t append(sw3A_uint8_t *pu8_buf, sw3A_uint32_t size);
    private:
        char fileName[64];
        void *file;
};

#endif /* __SW_3A_TRACE_HPP__ */

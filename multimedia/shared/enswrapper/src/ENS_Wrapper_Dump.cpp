/*
* Copyright (C) ST-Ericsson SA 2012. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/
#include "ENS_Wrapper.h"
#include <string.h>
#include <linux_utils.h>
#include <errno.h>
#include <utils/Log.h>
#undef LOG_TAG
#define LOG_TAG "ENSWrapperDump"

void ENS_Wrapper::Dump(int action, const char* reason, ENS_Wrapper_p ensWrapperPointer)
{
    if (ENS_Wrapper_CtxTrace::enabled) {
        const char* fileName = ENS_Wrapper_CtxTrace::GetFileName();
        FILE *f = NULL;
        f = fopen(fileName, "w+");
        if(f == NULL) {
            LOGE("Could not open %s for writing : (%s)!\n",fileName,strerror(errno));
        } else {
            if (action & LOG_CURRENT) {
                if (ensWrapperPointer) {
                    LOGE("%s, dumping status of %s in logcat...", reason, ensWrapperPointer->name);
                    fprintf(f, ensWrapperPointer->name);
                }
            }
            if (action & LOG_OPENED) {
                LOGE("%s, dumping status of all currently opened OMX components in logcat...",reason);
                fprintf(f, "log-opened");
            }
            fclose(f);
        }
    }
    if (action & DUMP_FATAL) {
        //FIXME: There is a crash in "STELP_FATAL" call
        //ER - 457827 is created to track the same.
        //STELP_FATAL(reason);
    }
}

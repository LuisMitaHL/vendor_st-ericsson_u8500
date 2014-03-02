/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <sys/select.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/prctl.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ENS_Wrapper.h"
#include "ENS_WrapperLog.h"
#include "ENS_Index.h"

#undef LOG_TAG
#define LOG_TAG "ENSWDataDump"

int ENS_Wrapper::mDataDumpEnabled = 0;
unsigned long ENS_Wrapper::mCurrentDataDumpSize = 0;
void ENS_Wrapper::dumpData(OMX_BUFFERHEADERTYPE* pBuffer,
                            char *portType,
                            char *path,
                            int portIndex,
                            unsigned long target_size,
                            const char *comp_name,
                            OMX_HANDLETYPE hComponent)
{   int written = 0;
    char str[OMX_MAX_STRINGNAME_SIZE];
    struct stat st;
    FILE *fd;
    sprintf(str, "%s%s_%s_%d_0x%08x.bin", path, comp_name, portType, portIndex, hComponent);
    int ret = stat(str, &st);
    if (ret) {
        mDataDumpEnabled = 1;
        mCurrentDataDumpSize = 0;
        LOGI("OMX binary buffer dump started on port index %d of %s port of %s,output in file %s",
             OMX_dump_idx,
             OMX_dump_dir ? "output" : "input",
             OMX_dump_name,
             str);
    }
    if (!mDataDumpEnabled) {
        return;
    }
    if (mCurrentDataDumpSize >= target_size && target_size != 0) {
        LOGI("OMX binary buffer dump stopped after %d bytes (%d target size reached), remove %s file to restart dump.",
             mCurrentDataDumpSize, target_size, str);
        mDataDumpEnabled = 0;
        return;
    }
    fd = fopen(str,"ab");
    if (!fd) {
        LOGE("Unable to open %s file.Data Dump discarded",str);
        OMX_dump_enabled = 0;
        return;
    }
    written = fwrite(pBuffer->pBuffer,1,pBuffer->nFilledLen,fd);
    mCurrentDataDumpSize += written;
    if (written != pBuffer->nFilledLen) {
        LOGI("OMX binary buffer dump stopped after %d bytes (no space left on device), remove %s file to restart dump.",
             mCurrentDataDumpSize, str);
        mDataDumpEnabled = 0;
    }
    fclose(fd);
}

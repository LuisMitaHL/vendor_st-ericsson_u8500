/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include "MMIO_Camera.h"
#include "mmio_linux_api.h"
#define CALL_AND_CHECK_ERROR(func, ...)					\
    do {								\
	OMX_ERRORTYPE error = MMIO_Camera::func(__VA_ARGS__);		\
	if (error) {							\
	    printf("Error returned by " #func "\n");			\
	    exit(error);						\
	}								\
    } while(0)

#define SIZE_TEST	(0x10000)
unsigned char test_buf[SIZE_TEST];
int main()
{
        CALL_AND_CHECK_ERROR(initBoard);
        CALL_AND_CHECK_ERROR(powerSensor, OMX_TRUE);
        CALL_AND_CHECK_ERROR(setExtClk, OMX_TRUE);
        CALL_AND_CHECK_ERROR(setPrimaryCameraHWInterface);
        //CALL_AND_CHECK_ERROR(releasePrimaryCameraHWInterface);
        CALL_AND_CHECK_ERROR(setSecondaryCameraHWInterface);
        //CALL_AND_CHECK_ERROR(releaseSecondaryCameraHWInterface);
        MMIO_Camera::initMMDSPTimer();
        //MMIO_Camera::loadXP70FW(0,0,test_buf,SIZE_TEST,test_buf,0x100);
        //MMIO_Camera::loadXP70FW(test_buf,SIZE_TEST,0,0,test_buf,SIZE_TEST,test_buf,0x100);
        enable_xshutdown_from_host(OMX_TRUE,OMX_TRUE);
        CALL_AND_CHECK_ERROR(setPrimaryCameraHWInterface);
        enable_xshutdown_from_host(OMX_FALSE,OMX_TRUE);
        enable_xshutdown_from_host(OMX_TRUE,OMX_FALSE);
        CALL_AND_CHECK_ERROR(setPrimaryCameraHWInterface);
        CALL_AND_CHECK_ERROR(setSecondaryCameraHWInterface);
        enable_xshutdown_from_host(OMX_FALSE,OMX_FALSE);
        //MMIO_Camera::ispVectorWrite(0,(const long*)&test_buf,8);
        activate_i2c2(OMX_TRUE);
        activate_i2c2(OMX_FALSE);
        printf("All Passed\n");
        return 0;
}

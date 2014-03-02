/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "camera_flash.h"
#include <linux/camera_flash.h>

#define FLASH_DEV_FILE	"/dev/camera_flash"

#ifdef DEBUG
  #define DEBUG_LOG(fmt, args...) printf("flash-lib:%s:"fmt,__FUNCTION__, ## args)
#else
  #define DEBUG_LOG(...)
#endif


#define FLASH_DO_IOCTL(_ioctl_cmd,_arg) do{     \
    err = ioctl(driver->fd, _ioctl_cmd, _arg);          \
    if(err){                                    \
        DEBUG_LOG("ioctl failed, err %d",errno);\
        err = errno;                            \
        goto out;                               \
    }                                           \
}while(0)

LinuxCFlashDriver* driver; //global because of the signal handler

int LinuxCFlashDriver::mCount = 0;
pthread_mutex_t LinuxCFlashDriver::mMutex = PTHREAD_MUTEX_INITIALIZER;

void SIGIO_handler(int signum, siginfo_t *info, void *context)
{
	if (NULL != driver->aSelfTestCallback)
	{
		int err;
		flash_ioctl_args_t ioctl_arg;
		
		/* retrieve selftests status */
		ioctl_arg.cam = EPrimary;
		FLASH_DO_IOCTL(FLASH_GET_SELF_TEST_RESULT,&ioctl_arg);
		
		/* send callback to user */
		DEBUG_LOG("SIGIO_handler arg : 0x%lx\n", ioctl_arg.status);
		driver->aSelfTestCallback(ioctl_arg.status, (void*)driver->aSelfTestContext);
	}
out:
        return;
}

CFlashDriver* CFlashDriver::Open()
{
    static LinuxCFlashDriver drv;

    pthread_mutex_lock(&LinuxCFlashDriver::mMutex);
    if (0 == LinuxCFlashDriver::mCount)
        if (drv.openfd()) {
            pthread_mutex_unlock(&LinuxCFlashDriver::mMutex);
            return NULL;
        }

    LinuxCFlashDriver::mCount++;
    pthread_mutex_unlock(&LinuxCFlashDriver::mMutex);
    return &drv;
}

LinuxCFlashDriver::LinuxCFlashDriver(void)
{
    fd = -1;
 }


int LinuxCFlashDriver::openfd(void)
{
    int err = 0;

    fd = ::open(FLASH_DEV_FILE,O_RDWR);
    if(fd < 0) {
        DEBUG_LOG("unable to open %s, err %d\n",FLASH_DEV_FILE,errno);
        return errno;
    }

    //setup signal handler chain here
    struct sigaction sa;
    sa.sa_sigaction = SIGIO_handler;
    sigemptyset(&(sa.sa_mask));
    sa.sa_flags = SA_SIGINFO;
    err = sigaction(SIGIO, &sa, NULL);
    if (0 != err) {
        DEBUG_LOG("sigaction error, err %d\n",errno);
        return errno;
    }

    //activate asynchronous notification
    fcntl(fd, F_SETOWN, getpid( ));
    int oflags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, oflags | FASYNC);

    driver = this;

    DEBUG_LOG("LinuxCFlashDriver fd %d\n",fd);

    return 0;
}

void LinuxCFlashDriver::Close()
{
    pthread_mutex_lock(&LinuxCFlashDriver::mMutex);
    if (1 == LinuxCFlashDriver::mCount ) {
        assert( -1 != fd);
        ::close(fd);
        fd = -1;
        mCount--;
    }
    if( -1 != fd)
        mCount--; 
    pthread_mutex_unlock(&LinuxCFlashDriver::mMutex);
}

LinuxCFlashDriver::~LinuxCFlashDriver(void)
{
    Close();
}


TFlashReturnCode ConvertError(int driver_err){
    TFlashReturnCode err=FLASH_RET_UNKNOWN;
	if(!driver_err)
		return FLASH_RET_NONE;
	switch(driver_err){
		case -EINVAL:
			err = FLASH_RET_MODE_NOT_SUPPORTED;
		case -ENODEV:
			err = FLASH_RET_MODE_NOT_SUPPORTED;
		case -ENOMEM:
			err = FLASH_RET_OUT_OF_MEM;
	}
	return err;
}
void LinuxCFlashDriver::GetSupportedFlashModes(TFlashMode& aModes, TCameraId aCameraId)
{
    int err = 0;
    flash_ioctl_args_t ioctl_arg;
    ioctl_arg.cam = aCameraId;
    FLASH_DO_IOCTL(FLASH_GET_MODES,&ioctl_arg);
    aModes = ioctl_arg.flash_mode;
    DEBUG_LOG("supported modes %x\n",aModes);
out:
    return;
}

TFlashReturnCode LinuxCFlashDriver::GetFlashModeDetails(TFlashMode aFlashMode, TFlashDetails& aDetails,
        TCameraId aCameraId)
{
	int err=0;
	flash_ioctl_args_t ioctl_arg;
	struct flash_mode_details *details_p;	
	ioctl_arg.cam = aCameraId;
	ioctl_arg.flash_mode = aFlashMode;
	FLASH_DO_IOCTL(FLASH_GET_MODE_DETAILS,&ioctl_arg);
	details_p = &ioctl_arg.mode_arg.details;
	aDetails.FlashDriverType = 0x01;
	aDetails.FlashDriverVersion = 0x01;
	aDetails.FlashLedType = details_p->led_type;
	aDetails.MaxIntensity = details_p->max_intensity_uAmp ;
	aDetails.MinIntensity = details_p->min_intensity_uAmp ;
	aDetails.MaxStrobeDuration = details_p->max_strobe_duration_uSecs;
	aDetails.NbFaultRegisters = details_p->nbFaultRegisters;
	if(details_p->feature_bitmap & INTENSITY_PROGRAMMABLE)
		aDetails.IsIntensityProgrammable = true;
	else	
		aDetails.IsIntensityProgrammable = false;
		
	if(details_p->feature_bitmap & DURATION_PROGRAMMABLE)	
		aDetails.IsDurationProgrammable = true;
	else
		aDetails.IsDurationProgrammable = false;
	
	if(details_p->feature_bitmap & TIMEOUT_PROGRAMMABLE)	
		aDetails.IsTimeoutProgrammable = true;
	else
		aDetails.IsTimeoutProgrammable = false;
	aDetails.NbFaultRegisters = 0;
	aDetails.NVMSize = 0;
out:
	return ConvertError(err);
}

TFlashReturnCode LinuxCFlashDriver::ConfigureFlashMode(TFlashMode aFlashMode,uint32_t aDuration,
	uint32_t aIntensity, uint32_t aTimeout, TCameraId aCameraId)
{
	int err=0;
	flash_ioctl_args_t ioctl_arg;
	struct flash_mode_params *params_p;
	
	ioctl_arg.cam = aCameraId;
	ioctl_arg.flash_mode = aFlashMode;
	params_p = &ioctl_arg.mode_arg.params;
	params_p->duration_uSecs = aDuration;
	params_p->intensity_uAmp = (unsigned long) aIntensity;
	params_p->timeout_uSecs = (unsigned long) aTimeout;
	FLASH_DO_IOCTL(FLASH_CONFIGURE_MODE,&ioctl_arg);
out:
	return ConvertError(err);
}

TFlashReturnCode LinuxCFlashDriver::EnableFlashMode(TFlashMode aFlashMode, TCallbackFn aCallback,
        void *apContext,
        TCameraId aCameraId)
{
	int err=0;
	flash_ioctl_args_t ioctl_arg;
	ioctl_arg.cam = aCameraId;
	ioctl_arg.flash_mode = aFlashMode;
	if(aFlashMode)
		FLASH_DO_IOCTL(FLASH_ENABLE_MODE,&ioctl_arg);
	else
		FLASH_DO_IOCTL(FLASH_DISABLE_MODE,&ioctl_arg);
out:	
	return ConvertError(err);
}

TFlashReturnCode LinuxCFlashDriver::Strobe(TFlashMode aFlashMode, bool aEnable, TCameraId aCameraId)
{
	int err=0;
	flash_ioctl_args_t ioctl_arg;
	ioctl_arg.cam = aCameraId;
	ioctl_arg.flash_mode = aFlashMode;
	ioctl_arg.mode_arg.strobe_enable = aEnable;
	FLASH_DO_IOCTL(FLASH_TRIGGER_STROBE,&ioctl_arg);
out:
	return ConvertError(err);
}

TFlashReturnCode LinuxCFlashDriver::GetStatus(TFlashMode aFlashMode, TFlashStatus& aFlashStatus,
		TCameraId aCameraId)
{
	int err=0;
	flash_ioctl_args_t ioctl_arg;
	ioctl_arg.cam = aCameraId;
	ioctl_arg.flash_mode = aFlashMode;
	FLASH_DO_IOCTL(FLASH_GET_STATUS,&ioctl_arg);
	aFlashStatus = GET_FLASH_STATUS(ioctl_arg.status);
	DEBUG_LOG("Flash error %lx\n",GET_FLASH_ERROR(ioctl_arg.status));
out:
	return ConvertError(err);
}

TFlashReturnCode LinuxCFlashDriver::GetXenonLifeCounter(uint32_t &aCounter, TCameraId aCameraId)
{
    int err=0;
    return ConvertError(err);
}

void LinuxCFlashDriver::GetSupportedSelfTests(TFlashSelftest& aSelfTests,
        TCameraId aCameraId)
{
    int err=0;

    flash_ioctl_args_t ioctl_arg;
    ioctl_arg.cam = aCameraId;
    FLASH_DO_IOCTL(FLASH_GET_SELF_TEST_MODES,&ioctl_arg);
    aSelfTests = ioctl_arg.flash_mode;
out:
    return;
}

TFlashReturnCode LinuxCFlashDriver::DoSelfTest (TFlashSelftest aSelfTest,
        TSelfTestCallbackFn aSelfTestCb,
        void *apContext,
        TCameraId aCameraId)
{
        int err = 0;
	flash_ioctl_args_t ioctl_arg;
	
	aSelfTestCallback = aSelfTestCb;
	aSelfTestContext = (int)apContext;
        ioctl_arg.cam = aCameraId;
	ioctl_arg.flash_mode = aSelfTest;
        FLASH_DO_IOCTL(FLASH_SELF_TEST, &ioctl_arg);
out:
        return ConvertError(err);
}


TFlashReturnCode LinuxCFlashDriver::GetFaultRegisters( TFlashMode aFlashModes,
        flash_uint32_t *apFaultRegisters,
        TCameraId aCameraId)
{
        int err = 0;
	flash_ioctl_args_t ioctl_arg;
    
	ioctl_arg.cam = aCameraId;
	ioctl_arg.flash_mode = aFlashModes;
	FLASH_DO_IOCTL(FLASH_GET_FAULT_REGISTERS,&ioctl_arg);
	if (NULL != apFaultRegisters)
	{
		apFaultRegisters[0] = ioctl_arg.status;
	}
	else
	{
		err = -ENOMEM;
	}
out:
	return ConvertError(err);
}

TFlashReturnCode LinuxCFlashDriver::GetNVM(
        TFlashMode aFlashModes,
        flash_uint32_t BufferSize,
        void* NVMBuffer,
        TGetNVMCallbackFn cb,
        void *apContext,
        TCameraId aCameraId)
{
    return FLASH_RET_MODE_NOT_SUPPORTED;
}


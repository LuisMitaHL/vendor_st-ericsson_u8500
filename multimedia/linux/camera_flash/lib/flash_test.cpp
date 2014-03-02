
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "flash_api.h"

#define DEBUG_LOG(fmt, args...) printf("flash-test:%s:"fmt,__FUNCTION__, ## args)

int test_hpled(CFlashDriver *flash_p, TFlashMode mode,unsigned long max_strobe)
{
	int err=0, strobe_pass=0;
	unsigned long intensity,duration;
	TFlashDetails details;
	TFlashStatus status;
	flash_p->GetFlashModeDetails(mode,details,EPrimary);
	DEBUG_LOG("Mode %x, Intensity(uA):max %d, Min %d, MAX strobe(uSec): %d\n",
		mode, details.MaxIntensity,details.MinIntensity,details.MaxStrobeDuration);
	DEBUG_LOG("IP %d, DP %d, TP %d\n",details.IsIntensityProgrammable,
		details.IsDurationProgrammable,details.IsTimeoutProgrammable);
strobe_again:		
	if(!strobe_pass){
		intensity = details.MinIntensity;
		//intensity = details.MaxIntensity;
		duration = 60000;
	}else{	
		intensity = details.MaxIntensity;
		duration = details.MaxStrobeDuration - 200000;
	}
	DEBUG_LOG("mode %x, Strobing with intensity %lu, duration %lu\n",
		mode, intensity,duration);

#ifdef TEST_TIMEOUT_FAULT
	if(mode == FLASH_MODE_STILL_LED)
		duration = 0;
#endif		
	err = flash_p->ConfigureFlashMode(mode,duration,intensity,0,EPrimary);
	err = flash_p->EnableFlashMode(mode,NULL,NULL,EPrimary);
	err = flash_p->Strobe(mode,true,EPrimary);
	err = flash_p->GetStatus(mode,status,EPrimary);
	DEBUG_LOG("Mode:%x, status %x\n",mode,status);
	sleep(max_strobe);
	err = flash_p->Strobe(mode,false,EPrimary);
	if(!strobe_pass){
		strobe_pass = 1; 
		goto strobe_again;
	}
out:
	return err;
}

int main(int ){
	int err=0;
	TFlashMode modes;
	CFlashDriver *flash_p = CFlashDriver::Open();
	flash_p->GetSupportedFlashModes(modes,EPrimary);
	DEBUG_LOG("Supported Modes %x\n",modes);
	err = test_hpled(flash_p,FLASH_MODE_VIDEO_LED,2);
	if(err)
		DEBUG_LOG("video LED test failed, err %d\n",err);
	
	sleep(2);
	err = test_hpled(flash_p,FLASH_MODE_STILL_LED,1);
	if(err)
		DEBUG_LOG("Still LED test failed, err %d\n",err);
	err = test_hpled(flash_p,FLASH_MODE_INDICATOR,2);
	if(err)
		DEBUG_LOG("ILED test failed, err %d\n",err);
	{
	    TFlashDetails details;
	    flash_p->GetFlashModeDetails(FLASH_MODE_VIDEO_LED,details,EPrimary);
	    DEBUG_LOG("GetFlashModeDetails(FLASH_MODE_VIDEO_LED,...):\n"
                "  - FlashDriverType    = %.2X\n"
                "  - FlashDriverVersion = %.2X\n"
                "  - FlashLedType       = %.2X\n"
                "  - NbFaultRegisters = %d\n"
                "  - NVMSize          = %d\n",
                details.FlashDriverType, details.FlashDriverVersion, details.FlashLedType,
                details.NbFaultRegisters, details.NVMSize
	    );
	}
	return 0;
}

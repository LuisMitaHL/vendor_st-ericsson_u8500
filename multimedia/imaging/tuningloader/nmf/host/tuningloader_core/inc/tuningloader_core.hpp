/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNING_LOADER_CORE_HPP_
#define _TUNING_LOADER_CORE_HPP_

#include "tuning_data_base.h"
#include "tuning.h"
#include "TraceObject.h"
#define FILE_CACHING

#ifdef FILE_CACHING
static const char* gTuningPathsCache[] =
{
	"/data/tuning/GenTuning_Gridiron.dat",
};
static const char* gTuningPathsLinearCache[] ={ 
"/data/tuning/GenTuning_Linearization-RR.dat",
"/data/tuning/GenTuning_Linearization-GR.dat",
"/data/tuning/GenTuning_Linearization-GB.dat",
"/data/tuning/GenTuning_Linearization-BB.dat"
};

static const char* gTuningPathsGridIronCache[]={
	"/data/tuning/GenTuning_fCastData1.dat",
	"/data/tuning/GenTuning_fCastData2.dat",
	"/data/tuning/GenTuning_fCastData3.dat",
	"/data/tuning/GenTuning_fCastData4.dat",
};

typedef struct 
		{
			t_uint32 numberOfCasts;
			t_uint32 gridWidth;
			t_uint32 gridHeight;
			t_uint32 stride;
			t_uint32 size;		
			float fCastPositionsData[4];
		} gridData_struct;



#endif

class tuningloader_core: public tuningloader_coreTemplate, public TraceObject
{
    public:
        tuningloader_core();
        ~tuningloader_core();
        void                setTraceInfo(void*, t_uint32);
        virtual t_bool      hasFirmwareTuning();
        virtual t_bool      hasNvmTuning();
        virtual t_bool      hasImageQualityTuning();
        virtual t_bool      hasFlashTuning();
        virtual void        loadFirmwareTuning(t_camera_info, t_fw_blocks_info);
        virtual void        loadNvmTuning(t_camera_info);
        virtual void        saveNvmTuning(t_camera_info,t_uint32,t_uint32 );
        virtual void        loadImageQualityTuning(t_camera_info);
        virtual void        loadFlashTuning(t_flash_info);
        virtual const char* cameraConfigFileName();
        virtual const char* flashConfigFileName();
        virtual const char* errorCode2String(t_tuningloader_error);

    private:
        /* Private data */
        bool                   bImageQualityTuningLoaded;
        char                   iSensorIdStr[64];
        char*                  pWorkingBuffer;
        unsigned int           iWorkingBufferSize;

        /* Private methods */
        void                   makeSensorIdStr(char*, int, t_uint8, t_uint16, t_uint8);
        t_tuningloader_error   loadXmlData(CTuningDataBase*, const t_camera_info*);
        t_tuningloader_error   populateCalibrationData(CTuningDataBase*);
        t_tuningloader_error   loadGridironData(CTuningDataBase*, const t_camera_info*);
        t_tuningloader_error   loadLinearizationData(CTuningDataBase*, const t_camera_info*);
        t_tuningloader_error   loadSensorCharacterizationData(CTuningDataBase*, const t_camera_info*);
        char*                  allocateWorkingBuffer(const unsigned int);
        char*                  getWorkingBuffer(const unsigned int);
        void                   releaseWorkingBuffer();
        t_tuningloader_error   searchFile(const char*, const char*, const char**, unsigned int*);
        void                   doRespond(t_tuningloader_error);
};

#endif /* _TUNING_LOADER_CORE_HPP_ */

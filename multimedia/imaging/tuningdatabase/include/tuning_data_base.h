/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNING_DATA_BASE_H_
#define _TUNING_DATA_BASE_H_
#include "tuning.h"
#include "tuning_bin.h"

#ifndef __SYMBIAN32__
#define IMPORT_C
#define EXPORT_C
#endif

typedef enum
{
    TUNING_OBJ_STILL_PREVIEW,
    TUNING_OBJ_STILL_CAPTURE,
    TUNING_OBJ_VIDEO_PREVIEW,
    TUNING_OBJ_VIDEO_RECORD,
} t_tuning_object;

typedef enum
{
    TUNING_OBJ_IMAGE_QUALITY_DATA,
    TUNING_OBJ_CHARACTERIZATION_DATA,
    TUNING_OBJ_CALIBRATION_DATA,
    TUNING_OBJ_FLASH_DATA,
    TUNING_OBJ_FIRMWARE_DATA,
    TUNING_OBJ_SENSOR_NVM_DATA,
    TUNING_OBJ_FLASH_NVM_DATA,
    TUNING_OBJ_GRIDIRON,
    TUNING_OBJ_LINEARIZATION_RR,
    TUNING_OBJ_LINEARIZATION_GR,
    TUNING_OBJ_LINEARIZATION_GB,
    TUNING_OBJ_LINEARIZATION_BB
} t_tuning_bin_object;

class CTuningDataBase
{
    private:
        CTuningDataBase();
        ~CTuningDataBase();

    public:
        // Public methods
        IMPORT_C static CTuningDataBase* getInstance(const char*);
        IMPORT_C static void             releaseInstance(const char*);
        IMPORT_C CTuning*                getObject(t_tuning_object aObject);
        IMPORT_C CTuningBinData*         getObject(t_tuning_bin_object aObject);

    private:
        // Unique instance reference
        static CTuningDataBase* pInstance;
        static int              iNbClients;

        // Tuning data base entries
        CTuning        iTuningStillPreview;
        CTuning        iTuningStillCapture;
        CTuning        iTuningVideoPreview;
        CTuning        iTuningVideoRecord;
        CTuningBinData iTuningImageQualityData;
        CTuningBinData iTuningCharacterizationData;
        CTuningBinData iTuningCalibrationData;
        CTuningBinData iTuningFlashData;
        CTuningBinData iTuningFirmwareData;
        CTuningBinData iTuningSensorNvmData;
        CTuningBinData iTuningFlashNvmData;
        CTuningBinData iTuningGridiron;
        CTuningBinData iTuningLinearizationRR;
        CTuningBinData iTuningLinearizationGR;
        CTuningBinData iTuningLinearizationGB;
        CTuningBinData iTuningLinearizationBB;

        // Disallow copy and assign default constructors
        CTuningDataBase(const CTuningDataBase&);
        void operator=(const CTuningDataBase&);
};

#endif // _TUNING_DATA_BASE_H_

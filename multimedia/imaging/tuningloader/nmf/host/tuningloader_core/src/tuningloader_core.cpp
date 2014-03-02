/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Traces identifiers
 */
#define OMXCOMPONENT "TUNING_LOADER_CORE"
#define OMX_TRACE_UID 0x10
#define DECLARE_AUTOVAR

/*
 * Includes
 */
#include <tuningloader_core.nmf>
#include <tuningloader_types.idt.h>
#include "tuningloader_core.hpp"
#include "tuningloader_fileio.h"
#include "tuning_data_base.h"
#include "osi_trace.h"
#include "nmx.h"
#include "ccal.h"
#include "VhcElementDefs.h"
#include <stdio.h>
#include "ImgConfig.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tuningloader_nmf_host_tuningloader_core_src_tuningloader_coreTraces.h"
#endif

/*
 * Local definitions
 */

#define TUNING_LOADER_DATABASE_CLIENT_NAME "TUNING-LOADER"

typedef struct
{
    t_tuning_object    object;   // Database object identifier
    unsigned int       num;      // Max number of expected parameters
    const char*        prefix;   // File name prefix
} t_XmlFileDesc;

#define MAX_TUNING_ENTRIES 1024

static const t_XmlFileDesc KOpModeXmlDesc[] =
{
    { TUNING_OBJ_STILL_CAPTURE, MAX_TUNING_ENTRIES, "TunStiCap" },
    { TUNING_OBJ_STILL_PREVIEW, MAX_TUNING_ENTRIES, "TunStiPre" },
    { TUNING_OBJ_VIDEO_PREVIEW, MAX_TUNING_ENTRIES, "TunVidPre" },
    { TUNING_OBJ_VIDEO_RECORD,  MAX_TUNING_ENTRIES, "TunVidRec" }
};

static const int KOpModeXmlDescsNum = sizeof(KOpModeXmlDesc)/sizeof(KOpModeXmlDesc[0]);

#define HASH_ENTRY(id) {id,#id}

typedef struct
{
    t_tuningloader_error id;
    const char* name;
} t_tuning_loader_error_entry;

static const t_tuning_loader_error_entry KTuningLoaderErrorCodesList[] =
{
    HASH_ENTRY(TUNING_LOADER_OK),
    HASH_ENTRY(TUNING_LOADER_BAD_ARGUMENT),
    HASH_ENTRY(TUNING_LOADER_NOT_IMPLEMENTED),
    HASH_ENTRY(TUNING_LOADER_ALREADY_LOADED),
    HASH_ENTRY(TUNING_LOADER_INTERNAL_ERROR),
    HASH_ENTRY(TUNING_LOADER_MALLOC_FAILED),
    HASH_ENTRY(TUNING_LOADER_FILE_IO_ERROR),
    HASH_ENTRY(TUNING_LOADER_FILE_NOT_FOUND),
    HASH_ENTRY(TUNING_LOADER_TUNING_ERROR),
    HASH_ENTRY(TUNING_LOADER_CAMERA_ID_MISMATCH),
    HASH_ENTRY(TUNING_LOADER_REDUCTION_ERROR),
    HASH_ENTRY(TUNING_LOADER_TRANSLATION_ERROR),
    HASH_ENTRY(TUNING_LOADER_DECODE_ERROR)
};

#define KTuningLoaderErrorCodesListSize (sizeof(KTuningLoaderErrorCodesList)/sizeof(KTuningLoaderErrorCodesList[0]))

#if defined(CONFIG_DEBUG)
static const char* stringifyValue(const t_uint32 aVal) {
   static char str[48]="";
   snprintf(str, sizeof(str)-1, "%lu", aVal);
   return (const char*)str;
}
static const char* stringifyValue(const float aVal) {
   static char str[48]="";
   snprintf(str, sizeof(str)-1, "%f", aVal);
   return (const char*)str;
}
#endif

#define ADD_PARAM(t,i,u,a,v)                                \
   do                                                       \
   {                                                        \
      t_tuning_error_code e = TUNING_OK;                    \
      MSG4("[%s][%s] %s value=%s\n",t->GetInstanceName(),#i,#a,stringifyValue(v)); \
      e = t->AddParam( i, u, a, #a, v);                     \
      if(e!=TUNING_OK)                                      \
      {                                                     \
         MSG4("Failed to add param 0x%X (%s), err=%d (%s)\n",a,#a,e,CTuning::ErrorCode2String(e)); \
         eTlErr = TUNING_LOADER_TUNING_ERROR;                  \
         goto failed;                                       \
      }                                                     \
   }                                                        \
   while (0)

/*
 * Constructor
 */
tuningloader_core::tuningloader_core()
{
    GET_AND_SET_TRACE_LEVEL(tuningload);

    IN0("\n");
    iSensorIdStr[0] = '\0';
    bImageQualityTuningLoaded  = false;
    pWorkingBuffer = NULL;
    iWorkingBufferSize = 0;
    OUT0("\n");
    return;
}

/*
 * Destructor
 */
tuningloader_core::~tuningloader_core()
{
    IN0("\n");
    releaseWorkingBuffer();
    OUT0("\n");
    return;
}

/*
 * setTraceInfo method
 */
void tuningloader_core::setTraceInfo(void* pTraceInfo, t_uint32 aId1)
{
    TraceObject::setTraceInfo((TraceInfo_t*)pTraceInfo, (unsigned int)aId1);
    MSG0("pTraceInfo=%p, aId1=0x%x\n", pTraceInfo, (unsigned int)aId1);
    //OstTraceFiltInst2(TRACE_FLOW, "tuningloader_core::setTraceInfo pTraceInfo=0x%x, aId1=%x", (unsigned int)pTraceInfo, aId1);
}

/*
 * 'has' methods
 */
t_bool tuningloader_core::hasFirmwareTuning()
{
    return FALSE;
}

t_bool tuningloader_core::hasNvmTuning()
{
    return TRUE;
}

t_bool tuningloader_core::hasImageQualityTuning()
{
    return TRUE;
}

t_bool tuningloader_core::hasFlashTuning()
{
    return FALSE;
}

/*
 * loadFirmwareTuning interface entry point
 */
void tuningloader_core::loadFirmwareTuning(t_camera_info aCameraInfo, t_fw_blocks_info aFwBlocksInfo)
{
    ASYNC_IN0("\n");
    doRespond(TUNING_LOADER_NOT_IMPLEMENTED);
    ASYNC_OUT0("\n");
    return;
}

/*
 * loadNvmTuning interface entry point
 */
void tuningloader_core::loadNvmTuning(t_camera_info aCameraInfo)
{
    ASYNC_IN0("\n");
    //OstTraceFiltInst0(TRACE_FLOW, "Entry tuningloader_core::loadNvmTuning");

    CTuningDataBase*        pTuningDataBase = NULL;
    t_tuningloader_error    eTlErr = TUNING_LOADER_OK;
    CTuningBinData*         pNvmObj = NULL;
    t_tuning_bin_data_error eTbErr = TUNING_BIN_OK;
#if (IMG_CONFIG_GSAMPLE_TUNING_ALGO == IMG_CONFIG_TRUE)
    char                    iCalFileName[64] = "";
    tuningloader_calfile    iCalFile;
    t_cal_file_info         iCalFileInfo;
#endif
    char                    iNvmFileName[64] = "";
    int                     iNvmFileSize = 0;
    t_fileio_error          iFileIoErr = FILEIO_OK;
    tuningloader_nvmfile    iNvmFile;

    // Sensor Module information
    MSG1("Camera Manufacturer Id: 0x%.2X\n", aCameraInfo.manufId);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera Manufacturer Id: 0x%.2X", aCameraInfo.manufId);
    MSG1("Camera Model Id       : 0x%.4X\n", aCameraInfo.modelId);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera Model Id       : 0x%.4X", aCameraInfo.modelId);
    MSG1("Camera Module Revision: 0x%.2X\n", aCameraInfo.revNum);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera Module Revision: 0x%.2X", aCameraInfo.revNum);

#if (IMG_CONFIG_GSAMPLE_TUNING_ALGO == IMG_CONFIG_TRUE)
    MSG1("Camera FuseId1:         0x%.2X\n", aCameraInfo.Fuse_Id[0]);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera FuseId1:         0x%.2X", aCameraInfo.Fuse_Id[0]);
    MSG1("Camera FuseId2       :  0x%.2X\n", aCameraInfo.Fuse_Id[1]);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera FuseId2       :  0x%.2X", aCameraInfo.Fuse_Id[1]);
    MSG1("Camera FuseId3:         0x%.2X\n", aCameraInfo.Fuse_Id[2]);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera FuseId3:         0x%.2X", aCameraInfo.Fuse_Id[2]);
    MSG1("Camera FuseId4       :  0x%.4X\n", (unsigned int)aCameraInfo.Fuse_Id4);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera FuseId4       :  0x%.4X", (unsigned int)aCameraInfo.Fuse_Id4);

    // Now need to check Calibration File
    snprintf(iCalFileName, sizeof(iCalFileName), "CALIBRATION_%.2X%.4X.cal", aCameraInfo.manufId, aCameraInfo.modelId);
    MSG1("Opening Calibration file '%s'\n", iCalFileName);
    //OstTraceFiltInst1(TRACE_DEBUG, "Opening Calibration file '%s'", iCalFileName);
    iFileIoErr = iCalFile.open(iCalFileName);
    if( iFileIoErr != FILEIO_OK)
    {
        // Failed to open calibration file
		if(aCameraInfo.ePreset == ImageUnprocessed )
        {
            MSG0("Failed to open Calibration file\n");
            //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open Calibration file ");
            eTlErr = TUNING_LOADER_FILE_NOT_FOUND;
            goto failed;
        }
        else
        {
            MSG0("Calibration file not found\n");
            //OstTraceFiltInst0(TRACE_DEBUG, "Calibration file not found");
            // Select Golden Sample file to Load data
            snprintf(iNvmFileName, sizeof(iNvmFileName), "GSAMPLE_%.2X%.4X.cal", aCameraInfo.manufId, aCameraInfo.modelId);
        }
    }
    else
    {
        // Found calibration file
        MSG1("CALIBRATION file size: %u bytes\n", iCalFile.size());
        //OstTraceFiltInst1(TRACE_DEBUG, "CALIBRATION file size: %d bytes", iCalFile.size());
        // Clear calibration status and FuseId4
        iCalFileInfo.iCalStatus[0] = '\0';
        iCalFileInfo.Fuse_Id4=0x00;
        // Read Calibration Status and FuseID4 from calibration file
        iFileIoErr = iCalFile.process( &iCalFileInfo);
        MSG2("Calibration file status = %s, fuseId4 = 0x%x\n",iCalFileInfo.iCalStatus,(unsigned int)iCalFileInfo.Fuse_Id4);
        //OstTraceFiltInst2(TRACE_DEBUG, "Calibration file status = %s, fuseId4 = 0x%x",iCalFileInfo.iCalStatus,(unsigned int)iCalFileInfo.Fuse_Id4);
        if((strncmp(iCalFileInfo.iCalStatus,"Success",sizeof(iCalFileInfo.iCalStatus))== 0)&&(iCalFileInfo.Fuse_Id4==aCameraInfo.Fuse_Id4 ))
        {
            // Calibration passed read NVM File
            snprintf(iNvmFileName, sizeof(iNvmFileName), "NVM_%.2X%.4X.cal", aCameraInfo.manufId, aCameraInfo.modelId);
            // Check NVM file exist or not
            MSG1("Open NVM file '%s'\n", iNvmFileName);
            //OstTraceFiltInst1(TRACE_DEBUG, "Open NVM file '%s'", iNvmFileName);
            iFileIoErr = iNvmFile.open( iNvmFileName);
            if( iFileIoErr != FILEIO_OK)
            {
                // File does Not exist read golden sample file
                snprintf(iNvmFileName, sizeof(iNvmFileName), "GSAMPLE_%.2X%.4X.cal", aCameraInfo.manufId, aCameraInfo.modelId);
            }
            else
            {
                // Read NVM file
                goto construct_nvm;
            }
        }
        else
        {
            MSG0("Either status ='fail' or fuseId4 mismatch");
            //OstTraceFiltInst0(TRACE_DEBUG, "Either status ='fail' or fuseId4 mismatch");
            // Calibration failed read from Golden Sample File
            snprintf(iNvmFileName, sizeof(iNvmFileName), "GSAMPLE_%.2X%.4X.cal",  aCameraInfo.manufId, aCameraInfo.modelId);
        }
    }
#else
    snprintf(iNvmFileName, sizeof(iNvmFileName), "NVM_%.2X%.4X.cal", aCameraInfo.manufId, aCameraInfo.modelId);
#endif

    // Open NVM file
    MSG1("Opening Fully Qualified NVM file '%s'\n", iNvmFileName);
    //OstTraceFiltInst1(TRACE_DEBUG, "Opening Fully Qualified NVM file '0x%x'", (unsigned int)iNvmFileName); // Move to %s once supported by OST
    iFileIoErr = iNvmFile.open(iNvmFileName);
    if( iFileIoErr != FILEIO_OK)
    {
        MSG0("Failed to open NVM file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open NVM file");
        eTlErr = TUNING_LOADER_FILE_NOT_FOUND;
        goto failed;
    }

#if (IMG_CONFIG_GSAMPLE_TUNING_ALGO == IMG_CONFIG_TRUE)
construct_nvm:
#endif

    // NVM file found
    iNvmFileSize = iNvmFile.size();
    MSG1("NVM file size: %d bytes\n", iNvmFileSize);
    //OstTraceFiltInst1(TRACE_DEBUG, "NVM file size: %d bytes", iNvmFileSize);
    MSG1("NVM file name: %s\n", iNvmFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "NVM file name: 0x%x", (unsigned int)iNvmFile.fullyQualifiedFileName()); // Move to %s once supported by OST

    // Bind to the tuning Data Base
    MSG0("Binding to the Tuning Data Base\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "Binding to the Tuning Data Base");
    pTuningDataBase = CTuningDataBase::getInstance(TUNING_LOADER_DATABASE_CLIENT_NAME);
    if(pTuningDataBase==NULL)
    {
        MSG0("Could not bind to the Tuning Data Base\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Could not bind to the Tuning Data Base");
        eTlErr = TUNING_LOADER_INTERNAL_ERROR;
        goto failed;
    }
    MSG1("Bound to the Tuning Data Base: pTuningDataBase=%p\n", pTuningDataBase);
    //OstTraceFiltInst1(TRACE_DEBUG, "Bound to the Tuning Data Base: pTuningDataBase=0x%x", (unsigned int)pTuningDataBase);

    // Construct the NVM tuning object
    pNvmObj = pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA);
    MSG1("Constructing [%s] tuning object\n", pNvmObj->getName());
    //OstTraceFiltInst0(TRACE_DEBUG, "Constructing [TUNING_OBJ_SENSOR_NVM_DATA] tuning object");
    eTbErr = pNvmObj->construct(iNvmFileSize);
    if(eTbErr!=TUNING_BIN_OK)
    {
        MSG3("Failed to construct [%s] tuning object: err=%d (%s)\n", pNvmObj->getName(), eTbErr, CTuningBinData::errorCode2String(eTbErr));
        //OstTraceFiltInst1(TRACE_DEBUG, "Failed to construct [TUNING_OBJ_SENSOR_NVM_DATA] tuning object: err=%d", eTbErr);
        eTlErr = TUNING_LOADER_MALLOC_FAILED;
        goto failed;
    }

    // Launch NVM file read
    MSG1("Loading NVM file '%s'\n", iNvmFileName);
    //OstTraceFiltInst1(TRACE_DEBUG, "Loading NVM file '0x%x'", (unsigned int)iNvmFileName); // Move to %s once supported by OST
    iFileIoErr = iNvmFile.read( pNvmObj->getAddr(), iNvmFileSize);
    if( iFileIoErr != FILEIO_OK)
    {
        MSG0("Failed to load NVM file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to load NVM file");
        eTlErr = TUNING_LOADER_FILE_IO_ERROR;
        goto failed;
    }

    // Done
    MSG1("Unbinding from tuning database pTuningDataBase=%p\n", pTuningDataBase);
    //OstTraceFiltInst1(TRACE_DEBUG, "Unbinding from tuning database pTuningDataBase=0x%x", (unsigned int)pTuningDataBase);
    pTuningDataBase->releaseInstance(TUNING_LOADER_DATABASE_CLIENT_NAME);
    pTuningDataBase = NULL;
    iNvmFile.close();
    doRespond(TUNING_LOADER_OK);
    ASYNC_OUT0("\n");
    //OstTraceFiltInst0(TRACE_FLOW, "Exit tuningloader_core::loadNvmTuning (OK)");
    return;

failed:
    if(pTuningDataBase != NULL) {
       MSG1("Unbinding from tuning database pTuningDataBase=%p\n", pTuningDataBase);
       //OstTraceFiltInst1(TRACE_DEBUG, "Unbinding from tuning database pTuningDataBase=0x%x", (unsigned int)pTuningDataBase);
       pTuningDataBase->releaseInstance(TUNING_LOADER_DATABASE_CLIENT_NAME);
       pTuningDataBase = NULL;
    }
    if(iNvmFile.isOpen()) {
        iNvmFile.close();
    }
#if (IMG_CONFIG_GSAMPLE_TUNING_ALGO == IMG_CONFIG_TRUE)
    if(iCalFile.isOpen()) {
        iCalFile.isOpen();
    }
#endif
    doRespond(eTlErr);
    ASYNC_OUT0("\n");
    //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadNvmTuning (Failed, err=%d)", eTlErr);
    return;

}
void tuningloader_core::saveNvmTuning(t_camera_info aCameraInfo,t_uint32 pNvm,t_uint32 aSize )
{

	t_tuningloader_error    eTlErr = TUNING_LOADER_OK;
	char                    iNvmFileName[68] = "";
	t_fileio_mode           eMode = FILEIO_MODE_WRITE_ONLY;
    t_fileio_error          iFileIoErr = FILEIO_OK;
    tuningloader_nvmfile    iNvmFile;

	MSG1("Camera Manufacturer Id: 0x%.2X\n", aCameraInfo.manufId);
    MSG1("Camera Model Id       : 0x%.4X\n", aCameraInfo.modelId);
    MSG1("Camera Module Revision: 0x%.2X\n", aCameraInfo.revNum);

	snprintf(iNvmFileName, sizeof(iNvmFileName), "NVM_%.2X%.4X.cal", aCameraInfo.manufId, aCameraInfo.modelId);

	MSG1("saveNvmTuning Opening Fully Qualified NVM file '%s'\n", iNvmFileName);
    //OstTraceFiltInst1(TRACE_DEBUG, "Opening Fully Qualified NVM file '0x%x'", (unsigned int)iNvmFileName); // Move to %s once supported by OST
    iFileIoErr = iNvmFile.open(iNvmFileName,eMode);
    if( iFileIoErr != FILEIO_OK)
    {
        MSG0("Failed to open NVM file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open NVM file");
        eTlErr = TUNING_LOADER_FILE_NOT_FOUND;
        doRespond(eTlErr);
		ASYNC_OUT0("\n");
		//OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::saveNvmTuning (Failed, err=%d)", eTlErr);
		return;
    }
	iFileIoErr = iNvmFile.write((const void *) pNvm,aSize);
	if( iFileIoErr != FILEIO_OK)
	{
        MSG0("Failed to write NVM file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open NVM file");
        eTlErr = TUNING_LOADER_FILE_IO_ERROR;
        doRespond(eTlErr);
		ASYNC_OUT0("\n");
		//OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::saveNvmTuning (Failed, err=%d)", eTlErr);
		return;
    }
	doRespond(eTlErr);
	iNvmFile.close();
	ASYNC_OUT0("\n");
	//OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::saveNvmTuning (Failed, err=%d)", eTlErr);
	return;

}
/*
 * loadFlashTuning interface entry point
 */
void tuningloader_core::loadFlashTuning(t_flash_info aFlashInfo)
{
    ASYNC_IN0(" ");
    //OstTraceFiltInst0(TRACE_FLOW, "Entry tuningloader_core::loadFlashTuning");
    doRespond(TUNING_LOADER_NOT_IMPLEMENTED);
    //OstTraceFiltInst0(TRACE_DEBUG, "Not Implemented");
    ASYNC_OUT0(" ");
    //OstTraceFiltInst0(TRACE_FLOW, "Exit tuningloader_core::loadFlashTuning (Failed, err=TUNING_LOADER_NOT_IMPLEMENTED)");
    return;
}

/*
 * loadImageQualityTuning interface entry point
 */
void tuningloader_core::loadImageQualityTuning(t_camera_info aCameraInfo)
{
    ASYNC_IN0("\n");
    //OstTraceFiltInst0(TRACE_FLOW, "Entry tuningloader_core::loadImageQualityTuning");

    t_tuningloader_error    eTlErr = TUNING_LOADER_OK;
    CTuningDataBase*        pTuningDataBase = NULL;

    // Check whether image quality data are already loaded
    if( bImageQualityTuningLoaded )
    {
        MSG0("Image quality tuning data already loaded\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Image quality tuning data already loaded");
        eTlErr = TUNING_LOADER_ALREADY_LOADED;
        goto failed;
    }

    // Sensor Module information
    MSG1("Camera Manufacturer Id: 0x%.2X\n", aCameraInfo.manufId);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera Manufacturer Id: 0x%.2X", aCameraInfo.manufId);
    MSG1("Camera Model Id       : 0x%.4X\n", aCameraInfo.modelId);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera Model Id       : 0x%.4X", aCameraInfo.modelId);
    MSG1("Camera Module Revision: 0x%.2X\n", aCameraInfo.revNum);
    //OstTraceFiltInst1(TRACE_DEBUG, "Camera Module Revision: 0x%.2X", aCameraInfo.revNum);

    // Bind to the tuning Data Base
    MSG0("Binding to the Tuning Data Base\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "Binding to the Tuning Data Base");
    pTuningDataBase = CTuningDataBase::getInstance(TUNING_LOADER_DATABASE_CLIENT_NAME);
    if(pTuningDataBase==NULL)
    {
        MSG0("Could not bind to the Tuning Data Base\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Could not bind to the Tuning Data Base");
        eTlErr = TUNING_LOADER_INTERNAL_ERROR;
        goto failed;
    }
    MSG1("Bound to the Tuning Data Base: pTuningDataBase=%p\n", pTuningDataBase);
    //OstTraceFiltInst1(TRACE_DEBUG, "Bound to the Tuning Data Base: pTuningDataBase=0x%x", (unsigned int)pTuningDataBase);

    // Allocate a working buffer (large enough to avoid further reallocation as far as possible)
    if( allocateWorkingBuffer(150*1024)== NULL )
    {
        MSG0("Failed to allocate Working buffer\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to allocate Working buffer");
        eTlErr = TUNING_LOADER_MALLOC_FAILED;
        goto failed;
    }

    // Load XML data
    eTlErr = loadXmlData(pTuningDataBase, &aCameraInfo);
    if(eTlErr != TUNING_LOADER_OK)
    {
        MSG0("Failed to load XML data\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to load XML data");
        goto failed;
    }

    //populate calibration data
    eTlErr = populateCalibrationData(pTuningDataBase);
    if(eTlErr != TUNING_LOADER_OK)
    {
        MSG0("Failed to populate calibration data\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to populate calibration data");
        goto failed;
    }

    // Load Gridiron data
    eTlErr = loadGridironData(pTuningDataBase, &aCameraInfo);
    if(eTlErr != TUNING_LOADER_OK)
    {
        MSG0("Failed to load Gridiron data\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to load Gridiron data");
        goto failed;
    }

    // Load Linearization data
    eTlErr = loadLinearizationData(pTuningDataBase, &aCameraInfo);
    if(eTlErr != TUNING_LOADER_OK)
    {
        MSG0("Failed to load Linearization data\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to load Linearization data");
        goto failed;
    }

    // Load Sensor Characterization data
    eTlErr = loadSensorCharacterizationData(pTuningDataBase, &aCameraInfo);
    if(eTlErr != TUNING_LOADER_OK)
    {
        MSG0("Failed to Sensor Characterization data\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to Sensor Characterization data");
        goto failed;
    }

    // Everything is loaded
    bImageQualityTuningLoaded = true;
    MSG1("Unbinding from tuning database pTuningDataBase=%p\n", pTuningDataBase);
    //OstTraceFiltInst1(TRACE_DEBUG, "Unbinding from tuning database pTuningDataBase=0x%x", (unsigned int)pTuningDataBase);
    pTuningDataBase->releaseInstance(TUNING_LOADER_DATABASE_CLIENT_NAME);
    pTuningDataBase = NULL;
    MSG0("Releasing Working buffer\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "Releasing Working buffer");
    releaseWorkingBuffer();
    doRespond(TUNING_LOADER_OK);
    //OstTraceFiltInst0(TRACE_FLOW, "Exit tuningloader_core::loadImageQualityTuning (OK)");
    ASYNC_OUT0("\n");
    return;

failed:
    if(pTuningDataBase != NULL) {
       MSG1("Unbinding from tuning database pTuningDataBase=%p\n", pTuningDataBase);
       //OstTraceFiltInst1(TRACE_DEBUG, "Unbinding from tuning database pTuningDataBase=0x%x", (unsigned int)pTuningDataBase);
       pTuningDataBase->releaseInstance(TUNING_LOADER_DATABASE_CLIENT_NAME);
       pTuningDataBase = NULL;
    }
    MSG0("Releasing Working buffer\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "Releasing Working buffer");
    releaseWorkingBuffer();
    doRespond(eTlErr);
    ASYNC_OUT0("\n");
    //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadImageQualityTuning (Failed, err=%d)", eTlErr);
    return;
}

/*
 * This function loads XML files and populates the corresponding entries
 * of the tuning database
 */
t_tuningloader_error tuningloader_core::loadXmlData(CTuningDataBase*     pTuningDataBase,
                                                    const t_camera_info* pCameraInfo)
{
    IN0("\n");
    //OstTraceFiltInst0(TRACE_FLOW, "Entry tuningloader_core::loadXmlData");

    CTuning*                pTuning = NULL;
    t_tuningloader_error    eTlErr = TUNING_LOADER_OK;
    t_tuning_error_code     eTuErr = TUNING_OK;
    char                    iSensorIdStr[64]= "";
    char                    iXmlFileName[64] = "";
    char*                   pXmlBuffer = NULL;
    CTuning*                pTuningCommon = NULL;
    int                     iXmlSize = 0;
    t_tuning_sensor_id      iSensorId;
    t_fileio_error          iFileIoErr = FILEIO_OK;
    tuningloader_tuningfile iTuningFile;

    // Make Sensor ID string
    makeSensorIdStr(iSensorIdStr, sizeof(iSensorIdStr), pCameraInfo->manufId, pCameraInfo->modelId, pCameraInfo->revNum);

    // Load the Common XML file
    snprintf(iXmlFileName, sizeof(iXmlFileName), "TunCommon-%s.xml", iSensorIdStr);
    MSG1("Opening XML file: %s\n", iXmlFileName);
    //OstTraceFiltInst1(TRACE_DEBUG, "Opening XML file: 0x%x", (unsigned int)iXmlFileName); // Move to %s once supported by OST
    iFileIoErr = iTuningFile.open(iXmlFileName);
    if( iFileIoErr != FILEIO_OK) {
        MSG0("Failed to open XML file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open XML file");
        eTlErr = TUNING_LOADER_FILE_NOT_FOUND;
        goto failed;
    }

    // XML file found
    iXmlSize = iTuningFile.size();
    MSG1("XML file size: %d bytes\n", iXmlSize);
    //OstTraceFiltInst1(TRACE_DEBUG, "XML file size: %d bytes", iXmlSize);
    MSG1("XML file name: %s\n", iTuningFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "XML file name: 0x%x", (unsigned int)iTuningFile.fullyQualifiedFileName()); // Move to %s once supported by OST

    // Get Working buffer
    pXmlBuffer = getWorkingBuffer(iXmlSize);
    if( pXmlBuffer == NULL)
    {
        MSG0("Failed to get working buffer\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to get working buffer");
        eTlErr = TUNING_LOADER_MALLOC_FAILED;
        goto failed;
    }

    MSG1("Loading XML file '%s'\n", iTuningFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "Loading XML file '0x%x'", (unsigned int)iTuningFile.fullyQualifiedFileName()); // Move to %s once supported by OST
    iFileIoErr = iTuningFile.read(pXmlBuffer, iXmlSize);
    if( iFileIoErr != FILEIO_OK) {
        MSG0("Failed to load XML file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to load XML file");
        eTlErr = TUNING_LOADER_FILE_IO_ERROR;
        goto failed;
    }
    iTuningFile.close();

    // Common XML file loaded: populate a tuning object
    MSG0("XML file loaded\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "XML file loaded");
    pTuningCommon = new CTuning("Common");
    if(pTuningCommon==NULL)
    {
        MSG2("Failed to instantiate [Common] tuning object: err=%d (%s)\n", eTuErr, CTuning::ErrorCode2String(eTuErr));
        //OstTraceFiltInst1(TRACE_DEBUG, "Failed to instantiate [Common] tuning object: err=%d", eTuErr);
        eTlErr = TUNING_LOADER_MALLOC_FAILED;
        goto failed;
    }

    MSG2("Constructing [%s] tuning configuration (%u entries max)\n", pTuningCommon->GetInstanceName(), MAX_TUNING_ENTRIES);
    //OstTraceFiltInst1(TRACE_DEBUG, "Constructing [Common] tuning configuration (%d entries max)", MAX_TUNING_ENTRIES);
    eTuErr = pTuningCommon->Construct(MAX_TUNING_ENTRIES);
    if(eTuErr!=TUNING_OK)
    {
        MSG2("Failed to construct tuning object: err=%d (%s)\n", eTuErr, CTuning::ErrorCode2String(eTuErr));
        //OstTraceFiltInst1(TRACE_DEBUG, "Failed to construct tuning object: err=%d", eTuErr);
        eTlErr = TUNING_LOADER_MALLOC_FAILED;
        goto failed;
    }

    MSG1("Populating [%s] tuning configuration\n", pTuningCommon->GetInstanceName());
    //OstTraceFiltInst0(TRACE_DEBUG, "Populating [Common] tuning configuration");
    eTuErr = pTuningCommon->FetchTuningConfiguration( pXmlBuffer, iXmlSize);
    if(eTuErr!=TUNING_OK)
    {
        MSG2("Failed to populate tuning object: err=%d (%s)\n", eTuErr, CTuning::ErrorCode2String(eTuErr));
        //OstTraceFiltInst1(TRACE_DEBUG, "Failed to populate tuning object: err=%d", eTuErr);
        eTlErr = TUNING_LOADER_TUNING_ERROR;
        goto failed;
    }

    // Check Sensor ID
    eTuErr = pTuningCommon->GetSensorId(&iSensorId);
    if(eTuErr!=TUNING_OK)
    {
        MSG3("[%s] Failed to get Sensor ID: err=%d (%s)\n", pTuningCommon->GetInstanceName(), eTuErr, CTuning::ErrorCode2String(eTuErr));
        //OstTraceFiltInst1(TRACE_DEBUG, "[Common] Failed to get Sensor ID: err=%d", eTuErr);
        eTlErr = TUNING_LOADER_TUNING_ERROR;
        goto failed;
    }
    if(   pCameraInfo->manufId != (t_uint8)iSensorId.manuf
       || pCameraInfo->modelId != (t_uint16)iSensorId.model)
    {
        MSG1("[%s] Invalid sensor ID:\n", pTuningCommon->GetInstanceName());
        MSG3("[%s]    Manuf ID: expected 0x%.2X, found 0x%.2X\n", pTuningCommon->GetInstanceName(), pCameraInfo->manufId, (t_uint8)iSensorId.manuf);
        MSG3("[%s]    Model ID: expected 0x%.4X, found 0x%.4X\n", pTuningCommon->GetInstanceName(), pCameraInfo->modelId, (t_uint16)iSensorId.model);
        //OstTraceFiltInst0(TRACE_DEBUG, "[Common] Invalid sensor ID:");
        //OstTraceFiltInst2(TRACE_DEBUG, "[Common]    Manuf ID: expected 0x%x, found 0x%x", pCameraInfo->manufId, (t_uint8)iSensorId.manuf);
        //OstTraceFiltInst2(TRACE_DEBUG, "[Common]    Model ID: expected 0x%x, found 0x%x", pCameraInfo->modelId, (t_uint16)iSensorId.model);
        eTlErr = TUNING_LOADER_CAMERA_ID_MISMATCH;
        goto failed;
    }
    MSG2("[%s] Sensor Name: %s\n", pTuningCommon->GetInstanceName(), pTuningCommon->GetSensorName());
    MSG2("[%s] Manuf  ID  : 0x%.2X\n", pTuningCommon->GetInstanceName(), (t_uint8)iSensorId.manuf);
    MSG2("[%s] Sensor ID  : 0x%.4X\n", pTuningCommon->GetInstanceName(), (t_uint16)iSensorId.model );
    //OstTraceFiltInst1(TRACE_DEBUG, "[Common] Sensor Name: 0x%x", (unsigned int)pTuningCommon->GetSensorName()); // Move to %s once supported bu OST
    //OstTraceFiltInst1(TRACE_DEBUG, "[Common] Manuf  ID  : 0x%x", (t_uint8)iSensorId.manuf); // Move to %s once supported by OST
    //OstTraceFiltInst1(TRACE_DEBUG, "[Common] Sensor ID  : 0x%x", (t_uint16)iSensorId.model ); // Move to %s once supported by OST

    // Load the Operating Mode specific XML files
    for(int iFileIndex=0; iFileIndex<KOpModeXmlDescsNum; iFileIndex++)
    {
        // Open XML file
        snprintf(iXmlFileName, sizeof(iXmlFileName),"%s-%s.xml", KOpModeXmlDesc[iFileIndex].prefix, iSensorIdStr);
        MSG1("Opening XML file: %s\n", iXmlFileName);
        //OstTraceFiltInst1(TRACE_DEBUG, "Opening XML file: #%d", iFileIndex);
        iFileIoErr = iTuningFile.open(iXmlFileName);
        if( iFileIoErr != FILEIO_OK) {
            MSG0("Failed to open XML file\n");
            //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open XML file");
            eTlErr = TUNING_LOADER_FILE_NOT_FOUND;
            goto failed;
        }

        // XML file found
        iXmlSize = iTuningFile.size();
        MSG1("XML file size: %d bytes\n", iXmlSize);
        //OstTraceFiltInst1(TRACE_DEBUG, "XML file size: %d bytes", iXmlSize);
        MSG1("XML file name: %s\n", iTuningFile.fullyQualifiedFileName());
        //OstTraceFiltInst1(TRACE_DEBUG, "XML file name: 0x%x", (unsigned int)iTuningFile.fullyQualifiedFileName()); // Move to %s once supported by OST

        // Get new working buffer
        pXmlBuffer = getWorkingBuffer(iXmlSize);
        if( pXmlBuffer == NULL)
        {
            MSG0("Failed to get working buffer\n");
            //OstTraceFiltInst0(TRACE_DEBUG, "Failed to get working buffer");
            eTlErr = TUNING_LOADER_MALLOC_FAILED;
            goto failed;
        }

        // Launch XML file read
        MSG1("Loading XML file '%s'\n", iTuningFile.fullyQualifiedFileName());
        //OstTraceFiltInst1(TRACE_DEBUG, "Loading XML file '0x%x'", (unsigned int)iTuningFile.fullyQualifiedFileName()); // Move to %s once supported by OST
        iFileIoErr = iTuningFile.read(pXmlBuffer, iXmlSize);
        if( iFileIoErr != FILEIO_OK) {
            MSG0("Failed to load XML file\n");
            //OstTraceFiltInst0(TRACE_DEBUG, "Failed to load XML file");
            eTlErr = TUNING_LOADER_FILE_IO_ERROR;
            goto failed;
        }
        iTuningFile.close();

        // XML file loaded: populate the corresponding tuning object
        MSG0("XML file loaded\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "XML file loaded");
        pTuning = pTuningDataBase->getObject( KOpModeXmlDesc[iFileIndex].object);

        MSG2("Constructing [%s] tuning configuration (%u entries max)\n", pTuning->GetInstanceName(), KOpModeXmlDesc[iFileIndex].num);
        //OstTraceFiltInst2(TRACE_DEBUG, "Constructing [%d] tuning configuration (%d entries max)", iFileIndex, KOpModeXmlDesc[iFileIndex].num);
        eTuErr = pTuning->Construct(KOpModeXmlDesc[iFileIndex].num);
        if(eTuErr!=TUNING_OK)
        {
            MSG2("Failed to construct tuning object: err=%d (%s)\n", eTuErr, CTuning::ErrorCode2String(eTuErr));
            //OstTraceFiltInst1(TRACE_DEBUG, "Failed to construct tuning object: err=%d", eTuErr);
            eTlErr = TUNING_LOADER_MALLOC_FAILED;
            goto failed;
        }

        MSG1("Populating [%s] tuning configuration\n", pTuning->GetInstanceName());
        //OstTraceFiltInst1(TRACE_DEBUG, "Populating [%d] tuning configuration", iFileIndex);
        eTuErr = pTuning->FetchTuningConfiguration( pXmlBuffer, iXmlSize);
        if(eTuErr!=TUNING_OK)
        {
            MSG2("Failed to populate tuning object: err=%d (%s)\n", eTuErr, CTuning::ErrorCode2String(eTuErr));
            //OstTraceFiltInst1(TRACE_DEBUG, "Failed to populate tuning object: err=%d", eTuErr);
            eTlErr = TUNING_LOADER_TUNING_ERROR;
            goto failed;
        }

        // Check Sensor ID
        eTuErr = pTuning->GetSensorId(&iSensorId);
        if(eTuErr!=TUNING_OK)
        {
            MSG3("[%s] Failed to get Sensor ID: err=%d (%s)\n", pTuning->GetInstanceName(), eTuErr, CTuning::ErrorCode2String(eTuErr));
            //OstTraceFiltInst2(TRACE_DEBUG, "[%d] Failed to get Sensor ID: err=%d", iFileIndex, eTuErr);
            eTlErr = TUNING_LOADER_TUNING_ERROR;
            goto failed;
        }
        if(   pCameraInfo->manufId != (t_uint8)iSensorId.manuf
           || pCameraInfo->modelId != (t_uint16)iSensorId.model)
        {
            MSG1("[%s] Invalid sensor ID:\n", pTuningCommon->GetInstanceName());
            MSG3("[%s]    Manuf ID: expected 0x%.2X, found 0x%.2X\n", pTuning->GetInstanceName(), pCameraInfo->manufId, (t_uint8)iSensorId.manuf);
            MSG3("[%s]    Model ID: expected 0x%.4X, found 0x%.4X\n", pTuning->GetInstanceName(), pCameraInfo->modelId, (t_uint16)iSensorId.model);
            //OstTraceFiltInst1(TRACE_DEBUG, "[%d] Invalid sensor ID:", iFileIndex);
            //OstTraceFiltInst3(TRACE_DEBUG, "[%d]    Manuf ID: expected 0x%.2X, found 0x%.2X", iFileIndex, pCameraInfo->manufId, (t_uint8)iSensorId.manuf);
            //OstTraceFiltInst3(TRACE_DEBUG, "[%d]    Model ID: expected 0x%.4X, found 0x%.4X", iFileIndex, pCameraInfo->modelId, (t_uint16)iSensorId.model);
            eTlErr = TUNING_LOADER_CAMERA_ID_MISMATCH;
            goto failed;
        }

        // Merge the Common XML into this operative-mode specific one
        // Note: in case of conflict, the "Common" conflicting entries are ignored
        // i.e. Priority is given to the op-mode specific entries
        MSG2("Merging [%s] tuning object with [%s] tuning configuration\n", pTuningCommon->GetInstanceName(), pTuning->GetInstanceName());
        //OstTraceFiltInst1(TRACE_DEBUG, "Merging [Common] tuning object with [%d] tuning configuration", iFileIndex);
        eTuErr = pTuning->MergeWith( pTuningCommon, TUNING_IGNORE_DUPLICATE_ENTRIES);
        if(eTuErr!=TUNING_OK)
        {
            MSG2("Merge failed: err=%d (%s)\n", eTuErr, CTuning::ErrorCode2String(eTuErr));
            //OstTraceFiltInst1(TRACE_DEBUG, "Merge failed: err=%d", eTuErr);
            eTlErr = TUNING_LOADER_TUNING_ERROR;
            goto failed;
        }
    }

    // Done
    pTuningCommon->Destroy();
    delete pTuningCommon;
    pTuningCommon = NULL;
    OUTR(" ",TUNING_LOADER_OK);
    //OstTraceFiltInst0(TRACE_FLOW, "Exit tuningloader_core::loadXmlData (OK)");
    return TUNING_LOADER_OK;

failed:
    if(pTuningCommon!=NULL) {
        pTuningCommon->Destroy();
        delete pTuningCommon;
        pTuningCommon = NULL;
    }
    if(iTuningFile.isOpen()) {
        iTuningFile.close();
    }
    OUTR(" ",eTlErr);
    //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadXmlData (Failed, err=%d)", eTlErr);
    return eTlErr;
}

/*
 * This function loads the Gridiron data
 */
t_tuningloader_error tuningloader_core::loadGridironData(CTuningDataBase*     pTuningDataBase,
                                                         const t_camera_info* pCameraInfo)
{
    IN0("\n");
    //OstTraceFiltInst0(TRACE_FLOW, "Entry tuningloader_core::loadGridironData");

    t_tuningloader_error    eTlErr = TUNING_LOADER_OK;
    char                    cSensorIdStr[16] = "";
    char                    iGridironFileName[64] = "";
    char*                   pRawGridironData = NULL;
    unsigned int            iRawGridironSize = 0;
    nmxMatrix               iCalibratedGridironMatrix;
    float                   fCastPositionsData[4] = { 0.0, 0.0, 0.0, 0.0 };
    nmxMatrix               iCastPositionsMatrix;
    uint32_t                iTmpBufferSize = 0;
    char*                   pTmpBuffer = NULL;
    CTuningBinData*         pNvmObj = NULL;
    t_fileio_error          iFileIoErr = FILEIO_OK;
    tuningloader_tuningfile iGridironFile;
    CTuningBinData*         pCalibratedGridironObj = NULL;
    t_tuning_bin_data_error eTbErr = TUNING_BIN_OK;
    CTuning*                pTuning = NULL;
    bool                    bOk = true;
    const int               iNumberOfGridironElements = 26624;
    t_uint32                iNumberOfCasts = 0;
    t_uint32                nGridWidth = 0;
    t_uint32                nGridHeight = 0;
    const t_tuning_object   iTuningObjId[] = { TUNING_OBJ_STILL_CAPTURE, TUNING_OBJ_STILL_PREVIEW,
                                               TUNING_OBJ_VIDEO_PREVIEW, TUNING_OBJ_VIDEO_RECORD };

#ifdef FILE_CACHING
	tuningloader_fileio gridFile;
	gridData_struct gridDataStruct;
	int size;
	iFileIoErr = gridFile.open(gTuningPathsCache[0],FILEIO_MODE_READ_ONLY);
	if( iFileIoErr != FILEIO_OK)
	{
		MSG0("Could not open Gridiron file\n");
		//OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");	
		goto gridiron_read_failed;
	}
	size =gridFile.size();
	if(size>0)
	{
		iFileIoErr = gridFile.read(&gridDataStruct,size);
		gridFile.close();
		if( iFileIoErr != FILEIO_OK)
		{
			MSG0("Could read from Gridiron file\n");
			//OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");					
			goto gridiron_read_failed;
		}				
		//Read Data
		iFileIoErr = gridFile.open(gTuningPathsGridIronCache[0],FILEIO_MODE_READ_ONLY);
		if( iFileIoErr != FILEIO_OK)
		{
			MSG0("Could not open data Gridiron file\n");
			//OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");	
			goto gridiron_read_failed;
		}
		size =gridFile.size();
		if(size>0)
		{
			char *dataGrid = getWorkingBuffer(gridDataStruct.size);
			if( dataGrid == NULL)
			{
				MSG0("Failed to get working buffer\n");
				//OstTraceFiltInst0(TRACE_DEBUG, "Failed to get working buffer");
				goto gridiron_read_failed;
			}
			iFileIoErr = gridFile.read(dataGrid,gridDataStruct.size);
			if( iFileIoErr != FILEIO_OK)
			{
				MSG0("Could read from Gridiron file\n");
				//OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");	
				goto gridiron_read_failed;
			}	
			gridFile.close();
			nGridWidth=gridDataStruct.gridWidth;
			nGridHeight=gridDataStruct.gridHeight;
			memcpy(fCastPositionsData,gridDataStruct.fCastPositionsData, sizeof(float) * 4);
			iNumberOfCasts=gridDataStruct.numberOfCasts;
			// Construct the Calibrated Gridiron tuning object
			pCalibratedGridironObj = pTuningDataBase->getObject(TUNING_OBJ_GRIDIRON);
			MSG2("Constructing [%s] object (size=%d bytes)\n", pCalibratedGridironObj->getName(), iNumberOfGridironElements*sizeof(t_uint32));
			//OstTraceFiltInst1(TRACE_DEBUG, "Constructing [Gridiron] object (size=%d bytes)", iNumberOfGridironElements*sizeof(t_uint32));
			eTbErr = pCalibratedGridironObj->construct(iNumberOfGridironElements*sizeof(t_uint32),dataGrid);
			if(eTbErr!=TUNING_BIN_OK)
			{
				MSG2("Failed to construct object: err=%d (%s)\n", eTbErr, CTuningBinData::errorCode2String(eTbErr));
				//OstTraceFiltInst1(TRACE_DEBUG, "Failed to construct object: err=%d (%s)", eTbErr);
				eTlErr = TUNING_LOADER_TUNING_ERROR;
				goto gridiron_read_failed;
			}			
			for(unsigned int i=0; i<(sizeof(iTuningObjId)/sizeof(iTuningObjId[0])); i++)
			{
				pTuning = pTuningDataBase->getObject(iTuningObjId[i]);
				ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_u16_GridWidth_Byte0,                 nGridWidth);
				ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_u16_GridHeight_Byte0,                nGridHeight);
				ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_f_CastPosition0_Byte0,               fCastPositionsData[0]);
				ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_f_CastPosition1_Byte0,               fCastPositionsData[1]);
				ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_f_CastPosition2_Byte0,               fCastPositionsData[2]);
				ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_f_CastPosition3_Byte0,               fCastPositionsData[3]);
				ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_u8_Active_ReferenceCast_Count_Byte0, iNumberOfCasts);
			}
			pCalibratedGridironObj->setStride(gridDataStruct.stride);
		}
	}
	else
	{
		goto gridiron_read_failed;
	}
	releaseWorkingBuffer();
	goto gridiron_complete;
	gridiron_read_failed:
	releaseWorkingBuffer();
#endif


    // Make Sensor ID string
    makeSensorIdStr(cSensorIdStr, sizeof(cSensorIdStr), pCameraInfo->manufId, pCameraInfo->modelId, pCameraInfo->revNum);

    // Open the Gridiron file (Gridiron == Lense Shading Correction)
    snprintf(iGridironFileName, sizeof(iGridironFileName), "ChrLsc-%s.nmx", cSensorIdStr);
    MSG1("Open Gridiron file: %s\n", iGridironFileName);
    //OstTraceFiltInst1(TRACE_DEBUG, "Open Gridiron file: 0x%x", (unsigned int)iGridironFileName); // Move to %s once supported by OST
    iFileIoErr = iGridironFile.open(iGridironFileName);
    if( iFileIoErr != FILEIO_OK)
    {
        MSG0("Failed to open Gridiron file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open Gridiron file");
        eTlErr = TUNING_LOADER_FILE_NOT_FOUND;
        goto failed;
    }

    // Gridiron file found
    iRawGridironSize = iGridironFile.size();
    MSG1("Gridiron file size: %d bytes\n", iRawGridironSize);
    //OstTraceFiltInst1(TRACE_DEBUG, "Gridiron file size: %d bytes", iRawGridironSize);
    MSG1("Gridiron file name: %s\n", iGridironFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "Gridiron file name: 0x%x", (unsigned int)iGridironFile.fullyQualifiedFileName());

    // Get working buffer
    pRawGridironData = getWorkingBuffer(iRawGridironSize);
    if( pRawGridironData == NULL )
    {
        MSG0("Failed to get Gridiron Working buffer\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to get Gridiron Working buffer");
        eTlErr = TUNING_LOADER_INTERNAL_ERROR;
        goto failed;
    }

    // Launch Gridiron file read
    MSG1("Loading Gridiron file '%s'\n", iGridironFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "Loading Gridiron file '0x%x'", (unsigned int)iGridironFile.fullyQualifiedFileName()); // Move to %s once supported by OST
    iFileIoErr = iGridironFile.read( pRawGridironData, iRawGridironSize);
    if( iFileIoErr != FILEIO_OK)
    {
        MSG0("Could not load Gridiron file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");
        eTlErr = TUNING_LOADER_FILE_IO_ERROR;
        goto failed;
    }
    iGridironFile.close();

    // Construct the Calibrated Gridiron tuning object
    pCalibratedGridironObj = pTuningDataBase->getObject(TUNING_OBJ_GRIDIRON);
    MSG2("Constructing [%s] object (size=%d bytes)\n", pCalibratedGridironObj->getName(), iNumberOfGridironElements*sizeof(t_uint32));
    //OstTraceFiltInst1(TRACE_DEBUG, "Constructing [Gridiron] object (size=%d bytes)", iNumberOfGridironElements*sizeof(t_uint32));
    eTbErr = pCalibratedGridironObj->construct(iNumberOfGridironElements*sizeof(t_uint32));
    if(eTbErr!=TUNING_BIN_OK)
    {
        MSG2("Failed to construct object: err=%d (%s)\n", eTbErr, CTuningBinData::errorCode2String(eTbErr));
        //OstTraceFiltInst1(TRACE_DEBUG, "Failed to construct object: err=%d (%s)", eTbErr);
        eTlErr = TUNING_LOADER_TUNING_ERROR;
        goto failed;
    }

    // Initialize iCalibratedGridironMatrix matrix as a vector (the output will be reshape)
    MSG0("Initializing calibrated Gridiron NMX matrix\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "Initializing calibrated Gridiron NMX matrix");
    nmxSetMatrixP2D_u(&iCalibratedGridironMatrix, (uint32_t*)pCalibratedGridironObj->getAddr(), iNumberOfGridironElements, 1);

    // Initialize iCastPositionsMatrix vector of four float elements
    MSG0("Initializing cast positions NMX matrix\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "Initializing cast positions NMX matrix");
    nmxSetMatrixP2D_f(&iCastPositionsMatrix, fCastPositionsData, 4, 1);

    // Allocate temporary buffer
    MSG0("Getting LSC temporary buffer size\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "Getting LSC temporary buffer size");
    iTmpBufferSize = ccal_get_buffer_size_lsc_grids(pRawGridironData, iRawGridironSize);
    pTmpBuffer = new char[iTmpBufferSize];
    if(pTmpBuffer == NULL)
    {
        MSG0("Failed to allocate temporary buffer\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to allocate temporary buffer");
        eTlErr = TUNING_LOADER_MALLOC_FAILED;
        goto failed;
    }
    MSG1("LSC temporary buffer size: %u bytes\n", iTmpBufferSize);
    //OstTraceFiltInst1(TRACE_DEBUG, "LSC temporary buffer size: %d bytes", iTmpBufferSize);

    // Get NVM data
    pNvmObj = pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA);
    MSG1("NVM data: %p\n", pNvmObj->getAddr());
    MSG1("NVM size: %d\n", pNvmObj->getSize());
    //OstTraceFiltInst1(TRACE_DEBUG, "NVM data: 0x%x", (unsigned int)pNvmObj->getAddr());
    //OstTraceFiltInst1(TRACE_DEBUG, "NVM size: %d", pNvmObj->getSize());

    // Compute the nvm adaptation (Gridiron tuning object is directly populated with calibrated data)
    MSG0("Calibrating Gridiron data\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "Calibrating Gridiron data");
    bOk = ccal_calibrate_lsc_grids(pRawGridironData, iRawGridironSize, pNvmObj->getAddr(), pTmpBuffer, &iCalibratedGridironMatrix, &iCastPositionsMatrix);
    if(bOk!=true)
    {
        MSG0("Gridiron data calibration failed\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Gridiron data calibration failed");
        eTlErr = TUNING_LOADER_INTERNAL_ERROR;
        goto failed;
    }

    // Gridiron is actually made of 4 sub-buffers => set sub-buffers stride
    pCalibratedGridironObj->setStride((int)(iCalibratedGridironMatrix.strides[3] * NMX_SIZEOF(iCalibratedGridironMatrix.etype)));

    // Add ISP parameters to all operating modes
    nGridWidth = iCalibratedGridironMatrix.dims[0];
    nGridHeight = iCalibratedGridironMatrix.dims[2];
    iNumberOfCasts = iCalibratedGridironMatrix.dims[3];
    for(unsigned int i=0; i<(sizeof(iTuningObjId)/sizeof(iTuningObjId[0])); i++)
    {
        pTuning = pTuningDataBase->getObject(iTuningObjId[i]);
        ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_u16_GridWidth_Byte0,                 nGridWidth);
        ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_u16_GridHeight_Byte0,                nGridHeight);
        ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_f_CastPosition0_Byte0,               fCastPositionsData[0]);
        ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_f_CastPosition1_Byte0,               fCastPositionsData[1]);
        ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_f_CastPosition2_Byte0,               fCastPositionsData[2]);
        ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_f_CastPosition3_Byte0,               fCastPositionsData[3]);
        ADD_PARAM( pTuning, IQSET_OPMODE_SETUP, IQSET_USER_ISP, GridironControl_u8_Active_ReferenceCast_Count_Byte0, iNumberOfCasts);
    }
#ifdef FILE_CACHING
	gridDataStruct.gridHeight=nGridHeight;
	gridDataStruct.gridWidth=nGridWidth;
	gridDataStruct.numberOfCasts=iNumberOfCasts;		
	gridDataStruct.size=((pCalibratedGridironObj->getSize()+sizeof(int))/sizeof(int))*sizeof(int);
	gridDataStruct.stride=pCalibratedGridironObj->getStride();
	memcpy(gridDataStruct.fCastPositionsData,fCastPositionsData, sizeof(float) * 4);
	iFileIoErr = gridFile.open(gTuningPathsCache[0],FILEIO_MODE_WRITE_ONLY);
	if( iFileIoErr != FILEIO_OK)
	{
       	MSG0("Could not load Gridiron file for write\n");
       	//OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");	
       	goto gridiron_write_failed;
	}	
	iFileIoErr = gridFile.write(&gridDataStruct,sizeof(gridDataStruct));
	if( iFileIoErr != FILEIO_OK)
   	{
       	MSG0("Could not write to Gridiron file\n");
       	//OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");	
       	goto gridiron_write_failed;
	}	
	gridFile.close();
	iFileIoErr = gridFile.open(gTuningPathsGridIronCache[0],FILEIO_MODE_WRITE_ONLY);
	if( iFileIoErr != FILEIO_OK)
	{
       	MSG0("Could not load Gridiron data file for write\n");
       	//OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");	
       	goto gridiron_write_failed;
	}	
	iFileIoErr = gridFile.write(pCalibratedGridironObj->getAddr(),gridDataStruct.size);
	if( iFileIoErr != FILEIO_OK)
	{
       	MSG0("Could not write data to Gridiron file\n");
       	//OstTraceFiltInst0(TRACE_DEBUG, "Could not load Gridiron file");	
     	goto gridiron_write_failed;
	}	

	gridiron_write_failed:		
	gridFile.close();						
	gridiron_complete:
#endif
    // Done
    delete [] pTmpBuffer;
    pTmpBuffer = NULL;
    OUTR(" ",TUNING_LOADER_OK);
    //OstTraceFiltInst0(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (OK)");
    return TUNING_LOADER_OK;

failed:
    if(pTmpBuffer) {
        delete [] pTmpBuffer;
        pTmpBuffer = NULL;
    }
    if(iGridironFile.isOpen()) {
        iGridironFile.close();
    }
    OUTR(" ",eTlErr);
    //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", eTlErr);
    return eTlErr;
}


/*
 * This function loads the calibration data
 */
t_tuningloader_error tuningloader_core::populateCalibrationData(CTuningDataBase * pTuningDataBase)
{
    IN0("\n");

    t_tuning_bin_data_error tbErr = TUNING_BIN_OK;
    
    CTuningBinData * pOriginalCalibrationObj    = NULL;
    CTuningBinData * pTranslatedCalibrationObj  = NULL;

    pOriginalCalibrationObj = pTuningDataBase->getObject(TUNING_OBJ_SENSOR_NVM_DATA);
    
    MSG1("Original calibration data addr: %p\n", pOriginalCalibrationObj->getAddr());
    MSG1("Original calibration data size: %d\n", pOriginalCalibrationObj->getSize());
    
    ste3a_calibration_data TranslatedCalibrationData;
    
    ccal_generate_ste3a_calibration_data(pOriginalCalibrationObj->getAddr(), &TranslatedCalibrationData);

    pTranslatedCalibrationObj = pTuningDataBase->getObject(TUNING_OBJ_CALIBRATION_DATA);
    
    MSG2("Constructing [%s] tuning object, size=%d\n", pTranslatedCalibrationObj->getName(), sizeof(TranslatedCalibrationData));
    
    tbErr = pTranslatedCalibrationObj->construct(sizeof(TranslatedCalibrationData));
    
    if(tbErr!=TUNING_BIN_OK)
    {
        MSG3("Failed to construct [%s] tuning object: err=%d (%s)\n", pTranslatedCalibrationObj->getName(), tbErr, CTuningBinData::errorCode2String(tbErr));
        return TUNING_LOADER_MALLOC_FAILED;
    }
    
    MSG1("Populating [%s] tuning object\n", pTranslatedCalibrationObj->getName());
    
    tbErr = pTranslatedCalibrationObj->fill(&TranslatedCalibrationData);
    
    if(tbErr!=TUNING_BIN_OK)
    {
        MSG3("Failed to populate [%s] tuning object: err=%d (%s)\n", pTranslatedCalibrationObj->getName(), tbErr, CTuningBinData::errorCode2String(tbErr));
        return TUNING_LOADER_TUNING_ERROR;
    }

    // Done
    OUTR(" ",TUNING_LOADER_OK);
    return TUNING_LOADER_OK;
}

/*
 * This function loads the Linearization data
 */
t_tuningloader_error tuningloader_core::loadLinearizationData(CTuningDataBase*     pTuningDataBase,
                                                              const t_camera_info* pCameraInfo)
{
    IN0("\n");
    //OstTraceFiltInst0(TRACE_FLOW, "Entry tuningloader_core::loadLinearizationData");

    char                    iSensorIdStr[64]= "";
    char                    iLinFileName[64] = "";
    char*                   pLinBuffer = NULL;
    int                     iLinFileSize = 0;
    t_fileio_error          iFileIoErr = FILEIO_OK;
    tuningloader_tuningfile iLinFile;
	 t_uint8* pLinData =NULL;
     int      nChannelSize =0;
     int      nChannelStride ;
	 nmxRCode_t iNmxRet = NMX_RET_OK;
	 nmxMatrix	iLinMatrix;

    typedef struct
    {
         t_tuning_bin_object DbObj;
         t_uint8*            pData;
         int                 nSize;
    } t_LinearizationDesc;

    t_LinearizationDesc LinDesc[4]=     { 
	 	{ TUNING_OBJ_LINEARIZATION_RR,  pLinData, nChannelSize },
		{ TUNING_OBJ_LINEARIZATION_GR,  pLinData, nChannelSize },
		{ TUNING_OBJ_LINEARIZATION_GB,  pLinData, nChannelSize },
		{ TUNING_OBJ_LINEARIZATION_BB,  pLinData, nChannelSize }};
	#ifdef FILE_CACHING
	int size = 0;
	tuningloader_fileio iLinFileTun;
	t_tuning_bin_object bin_objs[]=	{TUNING_OBJ_LINEARIZATION_RR,
									 TUNING_OBJ_LINEARIZATION_GR,
									 TUNING_OBJ_LINEARIZATION_GB,
									 TUNING_OBJ_LINEARIZATION_BB};  
	for(int i=0; i<4; i++)
	{
		size=0;
		MSG0("Loading linearization files.");			
		iLinFileTun.open(gTuningPathsLinearCache[i],FILEIO_MODE_READ_ONLY);
		if( iFileIoErr != FILEIO_OK) 
		{
			MSG0("Failed to open for write Linearization file %d\n",i);
			//OstTraceFiltInst0(TRACE_DEBUG, "Failed to open Linearization file");
			OUTR(" ", TUNING_LOADER_FILE_NOT_FOUND);
			//OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_FILE_NOT_FOUND); 			
		}
		size = iLinFileTun.size();
		if(size>0)
		{
			pLinBuffer = getWorkingBuffer(size);
			if( pLinBuffer == NULL)
		    {
    	   		MSG0("Failed to get working buffer\n");
				releaseWorkingBuffer();
		        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to get working buffer");
		       	goto create_linear;
    		}
			iFileIoErr = iLinFileTun.read(pLinBuffer,size);
			if( iFileIoErr != FILEIO_OK) 
			{
				MSG0("Failed to read Linearization file %d\n",i);
				//OstTraceFiltInst0(TRACE_DEBUG, "Failed to open Linearization file");
				OUTR(" ", TUNING_LOADER_FILE_NOT_FOUND);
				//OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_FILE_NOT_FOUND); 	
				releaseWorkingBuffer();
				goto create_linear;		
			}
			CTuningBinData* pLinObj = pTuningDataBase->getObject(bin_objs[i]);
   			MSG2("Constructing [%s] tuning object, size=%d\n", pLinObj->getName(), size);     			 //OstTraceFiltInst2(TRACE_DEBUG, "Constructing [Linearization-%d] tuning object, size=%d", i, LinDesc[i].nSize);
      	  	t_tuning_bin_data_error eTbErr1 = TUNING_BIN_OK;
      		eTbErr1 = pLinObj->construct(size, pLinBuffer);
        	if(eTbErr1!=TUNING_BIN_OK)
        	{
        	    MSG3("Failed to construct [%s] tuning object: err=%d (%s)\n", pLinObj->getName(), eTbErr1, CTuningBinData::errorCode2String(eTbErr1));
          		 //OstTraceFiltInst2(TRACE_DEBUG, "Failed to construct [Linearization-%d] tuning object: err=%d", i, eTbErr);
         		OUTR(" ", TUNING_LOADER_MALLOC_FAILED);
				releaseWorkingBuffer();
         		   //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_MALLOC_FAILED);
         	    goto create_linear;
        	}
		}
		else 
		{
			goto create_linear; 	
		}
		iLinFileTun.close();
	}
	releaseWorkingBuffer();
	goto end_linear;	
	#endif
create_linear:
    // Make Sensor ID string
	MSG0("Creating linearization files.");
    makeSensorIdStr(iSensorIdStr, sizeof(iSensorIdStr), pCameraInfo->manufId, pCameraInfo->modelId, pCameraInfo->revNum);

    // Load the Linearization file
    snprintf(iLinFileName, sizeof(iLinFileName), "ChrLin-%s.nmx", iSensorIdStr);
    MSG1("Opening Linearization file: %s\n", iLinFileName);
    //OstTraceFiltInst1(TRACE_DEBUG, "Opening Linearization file: 0x%x", (unsigned int)iLinFileName); // Move to %s once supported by OST
    iFileIoErr = iLinFile.open(iLinFileName);
    if( iFileIoErr != FILEIO_OK) {
        MSG0("Failed to open Linearization file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open Linearization file");
        OUTR(" ", TUNING_LOADER_FILE_NOT_FOUND);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_FILE_NOT_FOUND);
        return TUNING_LOADER_FILE_NOT_FOUND;
    }

    // Linearization file found
    iLinFileSize = iLinFile.size();
    MSG1("Linearization file size: %d bytes\n", iLinFileSize);
    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization file size: %d bytes", iLinFileSize);
    MSG1("Linearization file name: %s\n", iLinFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization file name: 0x%x", (unsigned int)iLinFile.fullyQualifiedFileName()); // Move to %s once supported by OST

    // Get working buffer
    pLinBuffer = getWorkingBuffer(iLinFileSize);
    if( pLinBuffer == NULL )
    {
        MSG0("Failed to allocate Working buffer\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to allocate Working buffer");
        iLinFile.close();
        OUTR(" ", TUNING_LOADER_MALLOC_FAILED);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_MALLOC_FAILED);
        return TUNING_LOADER_MALLOC_FAILED;
    }

    // Launch Linearization file read
    MSG1("Loading Linearization file '%s'\n", iLinFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "Loading Linearization file '0x%x'", (unsigned int)iLinFile.fullyQualifiedFileName()); // Move to %s once supported by OST
    iFileIoErr = iLinFile.read( pLinBuffer, iLinFileSize);
    if( iFileIoErr != FILEIO_OK)
    {
        MSG0("Failed to read Linearization file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to read Linearization file");
        iLinFile.close();
        OUTR(" ", TUNING_LOADER_FILE_IO_ERROR);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_FILE_IO_ERROR);
        return TUNING_LOADER_FILE_IO_ERROR;
    }
    iLinFile.close();

    // Linearization file is in NMX format => need to decode it
   iNmxRet = NMX_RET_OK;

    MSG0("NmxDecoding the Linearization file\n");
    //OstTraceFiltInst0(TRACE_DEBUG, "NmxDecoding the Linearization file");
    iNmxRet = nmxDecode_a( (uint8_t*)pLinBuffer, iLinFileSize, &iLinMatrix);
    if(iNmxRet!=NMX_RET_OK)
    {
        MSG0("Failed to nmxDecode the Linearization file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to nmxDecode the Linearization file");
        OUTR(" ", TUNING_LOADER_DECODE_ERROR);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_DECODE_ERROR);
        return TUNING_LOADER_DECODE_ERROR;
    }

    MSG1("Linearization iNmxMatrix: etype        = %d\n", iLinMatrix.etype);
    MSG1("Linearization iNmxMatrix: sizeof(etype)= %d\n", NMX_SIZEOF(iLinMatrix.etype));
    MSG1("Linearization iNmxMatrix: ndims        = %d\n", iLinMatrix.ndims);
    MSG1("Linearization iNmxMatrix: dim[0]       = %d\n", iLinMatrix.dims[0]);
    MSG1("Linearization iNmxMatrix: dim[1]       = %d\n", iLinMatrix.dims[1]);
    MSG1("Linearization iNmxMatrix: strides[0]   = %d\n", iLinMatrix.strides[0]);
    MSG1("Linearization iNmxMatrix: strides[1]   = %d\n", iLinMatrix.strides[1]);

    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization iNmxMatrix: etype        = %d", iLinMatrix.etype);
    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization iNmxMatrix: sizeof(etype)= %d", NMX_SIZEOF(iLinMatrix.etype));
    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization iNmxMatrix: ndims        = %d", iLinMatrix.ndims);
    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization iNmxMatrix: dim[0]       = %d", iLinMatrix.dims[0]);
    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization iNmxMatrix: dim[1]       = %d", iLinMatrix.dims[1]);
    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization iNmxMatrix: strides[0]   = %d", iLinMatrix.strides[0]);
    //OstTraceFiltInst1(TRACE_DEBUG, "Linearization iNmxMatrix: strides[1]   = %d", iLinMatrix.strides[1]);

    // Sanity check
    if(iLinMatrix.etype != NMX_TYPE_U32 ) {
        MSG2("Invalid Linearization iNmxMatrix etype, expected %d, got %d\n", NMX_TYPE_U32, iLinMatrix.etype);
        //OstTraceFiltInst2(TRACE_DEBUG, "Invalid Linearization iNmxMatrix etype, expected %d, got %d", NMX_TYPE_U32, iLinMatrix.etype);
        OUTR(" ", TUNING_LOADER_DECODE_ERROR);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_DECODE_ERROR);
        return TUNING_LOADER_DECODE_ERROR;
    }
    if(iLinMatrix.ndims != 2) {
        MSG2("Invalid Linearization iNmxMatrix ndims, expected %d, got %d\n", 2, iLinMatrix.ndims);
        //OstTraceFiltInst2(TRACE_DEBUG, "Invalid Linearization iNmxMatrix ndims, expected %d, got %d", 2, iLinMatrix.ndims);
        OUTR(" ", TUNING_LOADER_DECODE_ERROR);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_DECODE_ERROR);
        return TUNING_LOADER_DECODE_ERROR;
    }
    if(iLinMatrix.dims[0] > 257 || iLinMatrix.dims[1] != 4) {
        MSG2("Invalid Linearization iNmxMatrix dims, expected %d max, got dims[0]=%d\n", 257, iLinMatrix.dims[0]);
        MSG2("Invalid Linearization iNmxMatrix dims, expected %d, got dims[1]=%d\n", 4, iLinMatrix.dims[1]);
        //OstTraceFiltInst2(TRACE_DEBUG, "Invalid Linearization iNmxMatrix dims, expected %d max, got dims[0]=%d", 257, iLinMatrix.dims[0]);
        //OstTraceFiltInst2(TRACE_DEBUG, "Invalid Linearization iNmxMatrix dims, expected %d, got dims[1]=%d", 4, iLinMatrix.dims[1]);
        OUTR(" ", TUNING_LOADER_DECODE_ERROR);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_DECODE_ERROR);
        return TUNING_LOADER_DECODE_ERROR;
    }

    // Populate the Linearization tuning objects
     pLinData = (t_uint8*)iLinMatrix.pdata;
     nChannelSize = iLinMatrix.dims[0]*NMX_SIZEOF(iLinMatrix.etype);
     nChannelStride = iLinMatrix.strides[1]*NMX_SIZEOF(iLinMatrix.etype);

 //LinDesc[0]=      { TUNING_OBJ_LINEARIZATION_RR,  pLinData+(0*nChannelStride), nChannelSize };
 //LinDesc[1]=       { TUNING_OBJ_LINEARIZATION_GR,  pLinData+(1*nChannelStride), nChannelSize };
 //LinDesc[2]=       { TUNING_OBJ_LINEARIZATION_GB,  pLinData+(2*nChannelStride), nChannelSize };
 //LinDesc[3]=       { TUNING_OBJ_LINEARIZATION_BB,  pLinData+(3*nChannelStride), nChannelSize };

     LinDesc[0].DbObj= TUNING_OBJ_LINEARIZATION_RR;
     LinDesc[1].DbObj= TUNING_OBJ_LINEARIZATION_GR;
     LinDesc[2].DbObj= TUNING_OBJ_LINEARIZATION_GB;
     LinDesc[3].DbObj= TUNING_OBJ_LINEARIZATION_BB;

     for (int i=0;i<4;i++)
     {
         LinDesc[i].pData= pLinData+(i*nChannelStride);
         LinDesc[i].nSize= nChannelSize;
     }


    for(int i=0; i<4; i++)
    {
        CTuningBinData* pLinObj = pTuningDataBase->getObject(LinDesc[i].DbObj);
        MSG2("Constructing [%s] tuning object, size=%d\n", pLinObj->getName(), LinDesc[i].nSize);
        //OstTraceFiltInst2(TRACE_DEBUG, "Constructing [Linearization-%d] tuning object, size=%d", i, LinDesc[i].nSize);
        t_tuning_bin_data_error eTbErr = TUNING_BIN_OK;
        eTbErr = pLinObj->construct(LinDesc[i].nSize, LinDesc[i].pData);
        if(eTbErr!=TUNING_BIN_OK)
        {
            MSG3("Failed to construct [%s] tuning object: err=%d (%s)\n", pLinObj->getName(), eTbErr, CTuningBinData::errorCode2String(eTbErr));
            //OstTraceFiltInst2(TRACE_DEBUG, "Failed to construct [Linearization-%d] tuning object: err=%d", i, eTbErr);
            OUTR(" ", TUNING_LOADER_MALLOC_FAILED);
            //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_MALLOC_FAILED);
            return TUNING_LOADER_MALLOC_FAILED;
        }
#ifdef FILE_CACHING		
		iFileIoErr=iLinFileTun.open(gTuningPathsLinearCache[i],FILEIO_MODE_WRITE_ONLY);
		if( iFileIoErr != FILEIO_OK) {
			MSG0("Failed to open for write Linearization file %d\n",i);
			//OstTraceFiltInst0(TRACE_DEBUG, "Failed to open Linearization file");
			OUTR(" ", TUNING_LOADER_FILE_NOT_FOUND);
			//OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_FILE_NOT_FOUND);       		
    }
		iFileIoErr = iLinFileTun.write(LinDesc[i].pData,LinDesc[i].nSize);
		if( iFileIoErr != FILEIO_OK) {
			MSG0("Failed to write Linearization file %d\n",i);
			//OstTraceFiltInst0(TRACE_DEBUG, "Failed to open Linearization file");
			OUTR(" ", TUNING_LOADER_FILE_NOT_FOUND);
			//OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", 	TUNING_LOADER_FILE_NOT_FOUND);       		
		}
		iLinFileTun.close();				
#endif		
    }	
end_linear:

    // Done
    OUTR(" ",TUNING_LOADER_OK);
    //OstTraceFiltInst0(TRACE_FLOW, "Exit tuningloader_core::loadLinearizationData (OK)");
    return TUNING_LOADER_OK;
}

/*
 * This function loads the Sensor Characterization data
 */
t_tuningloader_error tuningloader_core::loadSensorCharacterizationData( CTuningDataBase*     pTuningDataBase,
                                                                        const t_camera_info* pCameraInfo)
{
    IN0("\n");
    //OstTraceFiltInst0(TRACE_FLOW, "Entry tuningloader_core::loadSensorCharacterizationData");

    char                    iSensorIdStr[64]= "";
    char                    iCharactFileName[64] = "";
    int                     iCharactFileSize = 0;
    CTuningBinData*         pCharactDataObj = NULL;
    t_tuning_bin_data_error eTbErr = TUNING_BIN_OK;
    t_fileio_error          iFileIoErr = FILEIO_OK;
    tuningloader_tuningfile iCharactFile;

    // Make Sensor ID string
    makeSensorIdStr(iSensorIdStr, sizeof(iSensorIdStr), pCameraInfo->manufId, pCameraInfo->modelId, pCameraInfo->revNum);

    // Load the Sensor Characterization file
    snprintf(iCharactFileName, sizeof(iCharactFileName), "ChrOth-%s.bin", iSensorIdStr);
    MSG1("Opening Sensor Characterization file: %s\n", iCharactFileName);
    //OstTraceFiltInst1(TRACE_DEBUG, "Opening Sensor Characterization file: 0x%x", (unsigned int)iCharactFileName); // Move to %s once supported by OST
    iFileIoErr = iCharactFile.open(iCharactFileName);
    if( iFileIoErr != FILEIO_OK) {
        MSG0("Failed to open Sensor Characterization file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to open Sensor Characterization file");
        OUTR(" ", TUNING_LOADER_FILE_NOT_FOUND);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_FILE_NOT_FOUND);
        return TUNING_LOADER_FILE_NOT_FOUND;
    }

    // Sensor Characterization file found
    iCharactFileSize = iCharactFile.size();
    MSG1("Sensor Characterization file size: %d bytes\n", iCharactFileSize);
    //OstTraceFiltInst1(TRACE_DEBUG, "Sensor Characterization file size: %d bytes", iCharactFileSize);
    MSG1("Sensor Characterization file name: %s\n", iCharactFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "Sensor Characterization file name: 0x%x", (unsigned int)iCharactFile.fullyQualifiedFileName()); // Move to %s once supported by OST

    // Contruct the Sensor Characterization tuning object
    pCharactDataObj = pTuningDataBase->getObject( TUNING_OBJ_CHARACTERIZATION_DATA);
    MSG2("Constructing the [%s] tuning object, size=%d\n", pCharactDataObj->getName(), iCharactFileSize);
    //OstTraceFiltInst1(TRACE_DEBUG, "Constructing the [Characterization] tuning object, size=%d", iCharactFileSize);
    eTbErr = pCharactDataObj->construct(iCharactFileSize);
    if(eTbErr!=TUNING_BIN_OK)
    {
        MSG3("Failed to construct [%s] tuning object: err=%d (%s)\n", pCharactDataObj->getName(), eTbErr, CTuningBinData::errorCode2String(eTbErr));
        //OstTraceFiltInst1(TRACE_DEBUG, "Failed to construct [Characterization] tuning object: err=%d", eTbErr);
        iCharactFile.close();
        OUTR(" ", TUNING_LOADER_MALLOC_FAILED);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_MALLOC_FAILED);
        return TUNING_LOADER_MALLOC_FAILED;
    }

    // Launch Sensor Characterization file read
    // Note: data are directly copied to tuning object
    MSG1("Loading Sensor Characterization file '%s'\n", iCharactFile.fullyQualifiedFileName());
    //OstTraceFiltInst1(TRACE_DEBUG, "Loading Sensor Characterization file '0x%x'", (unsigned int)iCharactFile.fullyQualifiedFileName()); // Move to %s once supported by OST
    iFileIoErr = iCharactFile.read( pCharactDataObj->getAddr(), pCharactDataObj->getSize());
    if( iFileIoErr != FILEIO_OK)
    {
        MSG0("Failed to load Sensor Characterization file\n");
        //OstTraceFiltInst0(TRACE_DEBUG, "Failed to load Sensor Characterization file");
        iCharactFile.close();
        OUTR(" ", TUNING_LOADER_FILE_IO_ERROR);
        //OstTraceFiltInst1(TRACE_FLOW, "Exit tuningloader_core::loadGridironData (Failed, err=%d)", TUNING_LOADER_FILE_IO_ERROR);
        return TUNING_LOADER_FILE_IO_ERROR;
    }
    iCharactFile.close();

    // Done
    OUTR(" ",TUNING_LOADER_OK);
    //OstTraceFiltInst0(TRACE_FLOW, "Exit tuningloader_core::loadSensorCharacterizationData (OK)");
    return TUNING_LOADER_OK;
}

/*
 * cameraConfigFileName NMF interface
 */
const char* tuningloader_core::cameraConfigFileName()
{
    IN0("\n");
    // There is no camera config file in STE tuning loader context
    const char* pName = "";
    OUTR(" ", (int)pName);
    return pName;
}

/*
 * flashConfigFileName NMF interface
 */
const char* tuningloader_core::flashConfigFileName()
{
    IN0("\n");
    // There is no flash config file in STE tuning loader context
    const char* pName = "";
    OUTR(" ", (int)pName);
    return pName;
}

/*
 * Function that reallocates the working a buffer
 * If current buffer is large enough, then buffer is not reallocated
 * If current buffer is too small, then a bigger one is allocated
 */
char* tuningloader_core::allocateWorkingBuffer( const unsigned int aBufferSize)
{
    // Check whether current buffer is large enough
    if( aBufferSize > iWorkingBufferSize )
    {
        // Current buffer too small => need to reallocate a bigger one
        if( pWorkingBuffer != NULL)
        {
            MSG1("Deleting pWorkingBuffer=%p\n", pWorkingBuffer);
            //OstTraceFiltInst1(TRACE_DEBUG, "Deleting pWorkingBuffer=0x%x", (unsigned int)pWorkingBuffer);
            delete [] pWorkingBuffer;
            pWorkingBuffer = NULL;
            iWorkingBufferSize = 0;
        }
        pWorkingBuffer = new char[aBufferSize];
        if( pWorkingBuffer == NULL)
        {
            MSG1("Could not allocate %u bytes for Working Buffer\n", aBufferSize);
            //OstTraceFiltInst1(TRACE_DEBUG, "Could not allocate %d bytes for Working Buffer", aBufferSize);
            return NULL;
        }
        iWorkingBufferSize = aBufferSize;
        MSG2("Allocated pWorkingBuffer=%p (%u bytes)\n", pWorkingBuffer, iWorkingBufferSize);
        //OstTraceFiltInst2(TRACE_DEBUG, "Allocated pWorkingBuffer=0x%x (%d bytes)", (unsigned int)pWorkingBuffer, iWorkingBufferSize);
    }
    return pWorkingBuffer;
}

/*
 * Function that return the working buffer address
 */
char* tuningloader_core::getWorkingBuffer( const unsigned int aBufferSize)
{
    return allocateWorkingBuffer(aBufferSize);
}

/*
 * Function that releases the working buffer
 */
void tuningloader_core::releaseWorkingBuffer()
{
    // Current buffer too small => need to reallocate a bigger one
    if( pWorkingBuffer != NULL)
    {
        MSG1("Deleting pWorkingBuffer=%p\n", pWorkingBuffer);
        //OstTraceFiltInst1(TRACE_DEBUG, "Deleting pWorkingBuffer=0x%x", (unsigned int)pWorkingBuffer);
        delete [] pWorkingBuffer;
        pWorkingBuffer = NULL;
        iWorkingBufferSize = 0;
    }
}

/*
 * Function that sends the response to NMF client
 */
void tuningloader_core::doRespond(t_tuningloader_error aErr)
{
    MSG2("Sending response %s (%d)\n", tuningloader_core::errorCode2String(aErr), aErr);
    //OstTraceFiltInst1(TRACE_DEBUG, "Sending response %d", aErr);
    acknowledge.response(aErr);
}

/*
 * Function that builds a sensor ID string
 */
void tuningloader_core::makeSensorIdStr(char* aSensorIdStr, int aMaxLength, t_uint8 aManufId, t_uint16 aModelId, t_uint8 aRevNum)
{
    snprintf( aSensorIdStr, aMaxLength, "%.2X%.4X", aManufId, aModelId);
}

/*
 * This function converts an error code into a human readable string
 */
const char* tuningloader_core::errorCode2String(t_tuningloader_error aErr)
{
    for(unsigned int i=0; i<KTuningLoaderErrorCodesListSize; i++) {
        if(KTuningLoaderErrorCodesList[i].id == aErr)
            return KTuningLoaderErrorCodesList[i].name;
    }
    return "*** ERROR CODE NOT FOUND ***";
}

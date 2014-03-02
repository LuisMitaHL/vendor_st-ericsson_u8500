/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 *  @addtogroup ldr_flash_app
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "c_system.h"
#include <string.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "t_flash_applications.h"
#include "r_command_exec_service.h"
#include "r_io_subsystem.h"
#include "commands.h"
#include "r_serialization.h"
#include "r_basicdefinitions.h"
#include "r_path_utilities.h"
#include "flash_applications.h"
#include "r_service_management.h"
//#include "os_free_fs.h"
#include "flash_manager.h"
#include "flash_dump_area_repeat.h"

#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
#include "r_time_utilities.h"
#include "r_measurement_tool.h"
#endif

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
#ifdef  CFG_ENABLE_MEASUREMENT_TOOL
extern  Measurement_t *Measurement_p;
#endif

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static DumpAreaParameters_t *CreateInitDumpAreaParam(void);

static ErrorCode_e DumpArea_OpenSourceChannel(DumpInputParameters_t *InParam_p, DumpAreaParameters_t *Param_p);
static ErrorCode_e DumpArea_OpenDestinationChannel(DumpInputParameters_t *InParam_p, DumpAreaParameters_t *Param_p);
static ErrorCode_e Handle_DumpReadData(DumpAreaParameters_t *Param_p);
static ErrorCode_e Handle_DumpWriteData(DumpAreaParameters_t *Param_p);
static ErrorCode_e Handle_DumpReadNextData(DumpAreaParameters_t *Param_p);
static ErrorCode_e Handle_DumpWriteDataLoop(DumpAreaParameters_t *Param_p);

#ifdef MACH_TYPE_DB5700
static SourceType_t GetDeviceType(void *Device_p);
#else
extern SourceType_t GetDeviceType(void *Device_p);
#endif

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
ErrorCode_e Flash_DumpAreaRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 DeviceNumber = 0;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    static DumpAreaParameters_t *Param_p = NULL;
    static DumpInputParameters_t InParam = {0};
    static FPD_Config_t FPDConfig = {0};

    ASSERT(NULL != ExeContext_p);

    if (NULL == Param_p) {
        Param_p = CreateInitDumpAreaParam();
        VERIFY(NULL != Param_p, E_ALLOCATE_FAILED);
    }

    switch (Param_p->State) {
    case DUMP_INIT: {
        void *Data_p = NULL;

#ifdef CFG_ENABLE_MEASUREMENT_TOOL
        ReturnValue = MP(Measurement_p, 0, START_COMMAND_TIME);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#endif
        //
        // Fill input parameters structure with received parameters
        //
        Data_p = ExeContext_p->Received.Payload.Data_p;
        InParam.Source_p = skip_str(&Data_p);
        InParam.Start = get_uint64_le(&Data_p);
        InParam.Length = get_uint64_le(&Data_p);
        InParam.Target_p = skip_str(&Data_p);
        InParam.RedundantArea = get_uint32_le(&Data_p);
        InParam.Source_p = ExtractDevice(InParam.Source_p);
        VERIFY(NULL != InParam.Source_p, E_INVALID_INPUT_PARAMETERS);

        DeviceNumber = GetDeviceNumber(InParam.Source_p);
        ReturnValue = Do_FPD_GetInfo(DeviceNumber, &FPDConfig);
        VERIFY(E_SUCCESS == ReturnValue, E_INVALID_INPUT_PARAMETERS);

        //
        // Make basic initializations before start of data transfering
        //
        ReturnValue = DumpArea_OpenSourceChannel(&InParam, Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        ReturnValue = DumpArea_OpenDestinationChannel(&InParam, Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        BUFFER_FREE(InParam.Source_p);
        Param_p->State = DUMP_READ_DATA;
        break;
    }

    case DUMP_READ_DATA:
        ReturnValue = Handle_DumpReadData(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case DUMP_WRITE_DATA:
        ReturnValue = Handle_DumpWriteData(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case DUMP_READ_NEXT_DATA:
        ReturnValue = Handle_DumpReadNextData(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case DUMP_WRITE_DATA_LOOP:
        ReturnValue = Handle_DumpWriteDataLoop(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case DUMP_END:
        C_(printf("flash_dump_area_repeat.c(%d): Dump finished!!!\n\n", __LINE__);)

        if (((InParam.Length + InParam.Start) > FPDConfig.DeviceSize) && (COMPLETE_FLASH_MARKER != (uint32)InParam.Length)) {
            ReturnValue = E_DUMP_OUT_OF_FLASH_RANGE;
            goto Exit;
        }

        ReturnValue = E_SUCCESS;
        goto Exit;

    default:
        A_(printf("flash_dump_area_repeat.c (%d): ** Unpredicted behavior! **\n", __LINE__);)
        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    return ReturnValue;

ErrorExit:
    //
    // Returned error values are intentionally ignored in order we can keep
    // original error that caused problem in first place.
    //
    (void)Do_IO_DestroyVector(Param_p->VectorIndex);
    (void)Do_IO_DestroyVector(Param_p->NextVectorIndex);
    (void)Do_IO_Close(Param_p->ReadHandle);
    (void)Do_IO_Close(Param_p->WriteHandle);
    BUFFER_FREE(InParam.Source_p);

Exit:
    ReturnValue = Done_Flash_DumpArea(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    ExeContext_p->Running = FALSE;
    BUFFER_FREE(Param_p);

#ifdef CFG_ENABLE_MEASUREMENT_TOOL
    ReturnValue = MP(Measurement_p, 0, END_COMMAND_TIME);
    ReturnValue = MFINISH(Measurement_p);
    Do_Measurement_DestroyMeasurementData(&Measurement_p);
#endif

    return ReturnValue;
}


/*******************************************************************************
 * Definition of file internal functions
 ******************************************************************************/
static DumpAreaParameters_t *CreateInitDumpAreaParam(void)
{
    DumpAreaParameters_t *Par_p = NULL;

    Par_p = (DumpAreaParameters_t *)malloc(sizeof(DumpAreaParameters_t));
    ASSERT(NULL != Par_p);

    Par_p->State              = DUMP_INIT;          // DumpArea_t
    Par_p->ReadHandle         = IO_INVALID_HANDLE;  // IO_Handle_t
    Par_p->WriteHandle        = IO_INVALID_HANDLE;  // IO_Handle_t
    Par_p->VectorIndex        = IO_INVALID_HANDLE;  // IO_Handle_t
    Par_p->NextVectorIndex    = IO_INVALID_HANDLE;  // IO_Handle_t
    Par_p->VectorLength       = 0;                  // uint64
    Par_p->NextVectorLength   = 0;                  // uint64
    Par_p->SourceDataLength   = 0;                  // uint64
    Par_p->BytesWritten       = 0;                  // uint64
    Par_p->WritePosition      = 0;                  // uint64
    Par_p->ReadPosition       = 0;                  // uint64
    Par_p->VectorPending      = FALSE;              // boolean

    return Par_p;
}

static ErrorCode_e DumpArea_OpenSourceChannel(DumpInputParameters_t *InParam_p, DumpAreaParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 DeviceType = GetDeviceType(InParam_p->Source_p);
    uint32 DeviceNumber = 0;
    FPD_Config_t FPDConfig = {0,};

    DeviceNumber = GetDeviceNumber(InParam_p->Source_p);

    ReturnValue = Do_FPD_GetInfo(DeviceNumber, &FPDConfig);
    VERIFY(E_SUCCESS == ReturnValue, E_INVALID_INPUT_PARAMETERS);

    switch (DeviceType) {
    case DEVICE_BAM:
        Param_p->ReadHandle = Do_IOW_BAMOpen(&FPDConfig);
        VERIFY(IO_INVALID_HANDLE != Param_p->ReadHandle, E_IO_FAILED_TO_OPEN);
        C_(printf("flash_dump_area_repeat.c(%d): DeviceType DEVICE_BAM!\n", __LINE__);)
        break;

    case DEVICE_BDM:
        Param_p->ReadHandle = Do_IOW_BDMOpen(&FPDConfig);
        VERIFY(IO_INVALID_HANDLE != Param_p->ReadHandle, E_IO_FAILED_TO_OPEN);
        C_(printf("flash_dump_area_repeat.c(%d): DeviceType DEVICE_BDM!\n", __LINE__);)
        break;

    case DEVICE_FPD:
        VERIFY((INCLUDE_REDUNDANT_AREA == InParam_p->RedundantArea) ||
               (WITHOUT_REDUNDANT_AREA == InParam_p->RedundantArea), E_INVALID_INPUT_PARAMETERS);
        Param_p->ReadHandle = Do_IOW_FPDOpen(InParam_p->RedundantArea, DeviceNumber);
        VERIFY(IO_INVALID_HANDLE != Param_p->ReadHandle, E_IO_FAILED_TO_OPEN);
        C_(printf("flash_dump_area_repeat.c(%d): DeviceType DEVICE_FPD!\n", __LINE__);)
        break;

    default:
        A_(printf("flash_dump_area_repeat.c(%d): DeviceType DEVICE_UNKNOWN!\n", __LINE__);)
        ReturnValue = E_DEVICE_TYPE;
        goto ErrorExit;
    }

    Param_p->SourceDataLength = Do_IO_GetLength(Param_p->ReadHandle);
    VERIFY(IO_ERROR != Param_p->SourceDataLength, E_IO_FAILED_TO_GET_LENGTH);
    VERIFY(Param_p->SourceDataLength >= InParam_p->Start, E_INVALID_INPUT_PARAMETERS);

    Param_p->SourceDataLength = MIN(Param_p->SourceDataLength - InParam_p->Start, InParam_p->Length);
    Param_p->ReadPosition += InParam_p->Start;

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e DumpArea_OpenDestinationChannel(DumpInputParameters_t *InParam_p, DumpAreaParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint64 VectorChunkSize = 0;

    //
    // Open IO channel depending if data should be transported to PC side via bulk
    // protocol or they should be saved in file system.
    //
    if (TRUE == SearchBulkPath(InParam_p->Target_p)) {
        Param_p->WriteHandle = Do_IOW_BulkOpen(0, BULK_SEND, Param_p->SourceDataLength);
        VERIFY(IO_INVALID_HANDLE != Param_p->WriteHandle, E_IO_FAILED_TO_OPEN);
    } else {
        InParam_p->Target_p = ExtractPath(InParam_p->Target_p);

#ifdef MACH_TYPE_DB5700
        Param_p->WriteHandle = Do_IOW_FileOpen(InParam_p->Target_p, O_RDWR | O_CREAT | O_TRUNC, 0777);
#else
        Param_p->WriteHandle = IO_INVALID_HANDLE;
#endif
        BUFFER_FREE(InParam_p->Target_p);
        VERIFY(IO_INVALID_HANDLE != Param_p->WriteHandle, E_IO_FAILED_TO_OPEN);
    }

    //
    // Calculate vector parameters and create vector for first session
    //
    Do_IO_CalculateVectorParameters((uint32 *)&Param_p->VectorLength,
                                    (uint32 *)&VectorChunkSize,
                                    0,
                                    Param_p->SourceDataLength, FALSE);


    Param_p->VectorIndex = Do_IO_CreateVector(Param_p->WriteHandle, Param_p->VectorLength, VectorChunkSize);
    VERIFY(IO_INVALID_HANDLE != Param_p->VectorIndex, E_VECTOR_CREATE_FAIL);

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Handle_DumpReadData(DumpAreaParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint64 BytesRead = 0;

    BytesRead = Do_IO_ScatteredPositionedRead(Param_p->ReadHandle,
                Param_p->VectorIndex,
                Param_p->VectorLength,
                Param_p->ReadPosition);
    VERIFY(IO_ERROR != BytesRead, IO_ErrorCode);

    if (BytesRead == Param_p->VectorLength) {
        C_(printf("flash_dump_area_repeat.c(%d): %llu bytes successfuly readed.\n", __LINE__, BytesRead);)

        Param_p->ReadPosition += BytesRead;
        Param_p->SourceDataLength -= BytesRead;
        Param_p->State = DUMP_WRITE_DATA;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Handle_DumpWriteData(DumpAreaParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint64 VectorChunkSize = 0;

    Param_p->BytesWritten = Do_IO_GatheredPositionedWrite(Param_p->WriteHandle,
                            Param_p->VectorIndex,
                            Param_p->VectorLength,
                            Param_p->WritePosition);
    VERIFY(IO_ERROR != Param_p->BytesWritten, IO_ErrorCode);

    if (0 == Param_p->SourceDataLength) {
        Param_p->State = DUMP_WRITE_DATA_LOOP;
        Param_p->VectorPending = FALSE;
        ReturnValue = E_SUCCESS;
        goto ErrorExit;
    }

    Do_IO_CalculateVectorParameters((uint32 *)&Param_p->NextVectorLength,
                                    (uint32 *)&VectorChunkSize,
                                    0,
                                    Param_p->SourceDataLength, FALSE);

    Param_p->NextVectorIndex = Do_IO_CreateVector(Param_p->WriteHandle, Param_p->NextVectorLength, VectorChunkSize);
    VERIFY(IO_INVALID_HANDLE != Param_p->NextVectorIndex, E_VECTOR_CREATE_FAIL);

    Param_p->State = DUMP_READ_NEXT_DATA;
    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Handle_DumpReadNextData(DumpAreaParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint64 BytesRead = 0;

    BytesRead = Do_IO_ScatteredPositionedRead(Param_p->ReadHandle,
                Param_p->NextVectorIndex,
                Param_p->NextVectorLength,
                Param_p->ReadPosition);
    VERIFY(IO_ERROR != BytesRead, IO_ErrorCode);

    if (BytesRead == Param_p->NextVectorLength) {
        Param_p->ReadPosition += BytesRead;

        if (BytesRead == Param_p->SourceDataLength) {
            C_(printf("flash_dump_area_repeat.c (%d): *** VectorPending set as TRUE ***\n", __LINE__);)
            Param_p->VectorPending = TRUE;
        }

        Param_p->SourceDataLength -= BytesRead;
        Param_p->State = DUMP_WRITE_DATA_LOOP;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Handle_DumpWriteDataLoop(DumpAreaParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    Param_p->BytesWritten = Do_IO_GatheredPositionedWrite(Param_p->WriteHandle,
                            Param_p->VectorIndex,
                            Param_p->VectorLength,
                            Param_p->WritePosition);
    VERIFY(IO_ERROR != Param_p->BytesWritten, IO_ErrorCode);

    if (Param_p->BytesWritten == Param_p->VectorLength) {
        DESTROY_VECTOR(Param_p->VectorIndex);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Param_p->VectorIndex = Param_p->NextVectorIndex;
        Param_p->NextVectorIndex = IO_INVALID_HANDLE;

        Param_p->VectorLength = Param_p->NextVectorLength;
        Param_p->WritePosition += Param_p->BytesWritten;

        if ((0 == Param_p->SourceDataLength) && (FALSE == Param_p->VectorPending)) {
            CLOSE_HANDLE(Param_p->ReadHandle);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
            CLOSE_HANDLE(Param_p->WriteHandle);
            VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

            Param_p->State = DUMP_END;
            goto ErrorExit;
        }

        Param_p->State = DUMP_WRITE_DATA;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

#ifdef MACH_TYPE_DB5700
static SourceType_t GetDeviceType(void *Device_p)
{
    char *LastWord_p = NULL;
    SourceType_t ImageType = DEVICE_UNKNOWN;

    LastWord_p = GetLastWord(Device_p);

    if (NULL != LastWord_p) {
        if (0 == strncmp("mbbs", LastWord_p, strlen("mbbs"))) {
            ImageType = DEVICE_BAM;
        } else if (0 == strncmp("cabs", LastWord_p, strlen("cabs"))) {
            ImageType = DEVICE_BDM;
        } else if (0 == strncmp("flash", LastWord_p, strlen("flash"))) {
            ImageType = DEVICE_FPD;
        } else {
            ImageType = DEVICE_UNKNOWN;
        }

        BUFFER_FREE(LastWord_p);
    }

    return ImageType;
}
#endif

/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 * @addtogroup ldr_parameter_app
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "r_basicdefinitions.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "commands_impl.h"
#include "command_ids.h"
#include "commands.h"
#include "global_data_manager.h"
#include "r_serialization.h"
#include "t_communication_service.h"
#include "r_command_exec_service.h"
#include "r_service_management.h"
#include "t_flash_applications.h"
#include "parameter_applications.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
char *InsertedDevice_p = NULL;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static GD_ReadSetParameters_t *CreateInit_GD_ReadSetParameters(void);
static ErrorCode_e Handle_GD_ReadSet_CollectData(GD_ReadSetParameters_t *Param_p);
static ErrorCode_e Handle_GD_ReadSet_InitializeDataTransfer(GD_ReadSetParameters_t *Param_p);
static ErrorCode_e Handle_GD_ReadSet_TransferData(GD_ReadSetParameters_t *Param_p);

static GD_WriteSetParameters_t *CreateInit_GD_WriteSetParameters(void);
static ErrorCode_e Handle_GD_WriteSet_Initialize(GD_WriteSetParameters_t *Param_p, ExecutionContext_t *ExecutionContext_p);
static ErrorCode_e Handle_GD_WriteSet_GetData(GD_WriteSetParameters_t *Param_p);
static ErrorCode_e Handle_GD_WriteSet_ProcessData(GD_WriteSetParameters_t *Param_p);

/**
 * @brief Set GD Device that will be used for command (pointed by DevicePath).
 *
 * @param [in] DevicePath_p
 *
 * @return      @ref E_SUCCESS     - Success ending.
 * @return      @ref E_INVALID_INPUT_PARAMETERS - If invalid device path is passed.
 */
static ErrorCode_e Set_GD_Device(const char *DevicePath_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/**
 * @brief Reads a data from specified unit in Global Data (GD) storage area.
 *
 * Call source: PC
 *
 * @param [in]   DevicePath_p   - Identify the partition from where to read from (cspsa...).
 * @param [in]   Unit_id        - Unit id to read.
 *
 * @return       @ref E_SUCCESS - Success ending.
 *
 * @return       Function can also return some internal GD error codes.
 *
 * @remark       see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ParameterStorage_ReadGlobalDataUnitImpl(uint16 Session,
        const char *DevicePath_p,
        const uint32 Unit_id)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};
    void *Data_p = NULL;

    CmmData.CommandNr = COMMAND_PARAMETERSTORAGE_READGLOBALDATAUNIT;
    CmmData.ApplicationNr = GROUP_PARAMETERSTORAGE;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(NULL != DevicePath_p, E_INVALID_INPUT_PARAMETERS);

    ReturnValue = Set_GD_Device(DevicePath_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    //
    // Register command in CES (Command Execution Service)
    //
    CmmData.CommandNr = COMMAND_PARAMETERSTORAGE_READGLOBALDATAUNIT;
    CmmData.ApplicationNr = GROUP_PARAMETERSTORAGE;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    memset(CmmData.Payload.Data_p, 0x00, CmmData.Payload.Size);
    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Unit_id);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;

ErrorExit:
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);
    BUFFER_FREE(InsertedDevice_p);

    return ReturnValue;
}

/**
 * @brief Writes a specified unit to the Global Data (GD) storage area.
 *
 * Call source: PC
 *
 * @param [in]  DevicePath_p   - Path to destination partition (cspsa...)
 * @param [in]  Unit_id        - Unit id to write
 * @param [in]  DataBuffLength - Length of the Data buffer.
 * @param [in]  DataBuff_p     - The data to write.
 *
 * @return      @ref E_SUCCESS - Success ending.
 * @return      @ref E_INVALID_INPUT_PARAMETERS - If invalid input parameter is
 *                                                passed.
 *
 * @return      Function can also return some internal GD error codes.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ParameterStorage_WriteGlobalDataUnitImpl(uint16 Session,
        const char *DevicePath_p,
        const uint32 Unit_id,
        const uint32 DataBuffLength,
        void *DataBuff_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};
    void *Data_p = NULL;

    CmmData.CommandNr = COMMAND_PARAMETERSTORAGE_WRITEGLOBALDATAUNIT;
    CmmData.ApplicationNr = GROUP_PARAMETERSTORAGE;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(NULL != DataBuff_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(0 < DataBuffLength, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != DevicePath_p, E_INVALID_INPUT_PARAMETERS);

    ReturnValue = Set_GD_Device(DevicePath_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    //
    // Register command in CES (Command Execution Service)
    //
    CmmData.CommandNr = COMMAND_PARAMETERSTORAGE_WRITEGLOBALDATAUNIT;
    CmmData.ApplicationNr = GROUP_PARAMETERSTORAGE;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = sizeof(uint32) + sizeof(uint32) + DataBuffLength;
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    memset(CmmData.Payload.Data_p, 0x00, CmmData.Payload.Size);
    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Unit_id);
    put_uint32_le((void **)&Data_p, DataBuffLength);
    put_block(&Data_p, DataBuff_p, DataBuffLength);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;

ErrorExit:
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);
    BUFFER_FREE(InsertedDevice_p);

    return ReturnValue;
}

/**
 * @brief Reads complete Global Data storage area (all units).
 *
 * This function reads set of units and send them to client application
 * via bulk protocol.
 *
 * Call source: PC
 *
 * @param [in] DevicePath_p   - Path to source partition (cspsa ...)
 * @param [in] TargetPath_p   - not used
 * .
 * @return     @ref E_SUCCESS - Success ending.
 * @return     @ref E_GENERAL_FATAL_ERROR - If something unexpected happened.
 * @return     @ref E_INVALID_INPUT_PARAMETERS - If invalid input parameter
 *                                               is passed.
 *
 * @return     Function can also return some internal GD error codes.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ParameterStorage_ReadGlobalDataSetImpl(uint16 Session,
        const char *DevicePath_p,
        const char *TargetPath_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_PARAMETERSTORAGE_READGLOBALDATASET;
    CmmData.ApplicationNr = GROUP_PARAMETERSTORAGE;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = 0;
    CmmData.Payload.Data_p = NULL;

    VERIFY(NULL != TargetPath_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != DevicePath_p, E_INVALID_INPUT_PARAMETERS);

    ReturnValue = Set_GD_Device(DevicePath_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;

ErrorExit:
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    BUFFER_FREE(InsertedDevice_p);
    return ReturnValue;
}

/**
 * @brief Writes set of units in Global Data storage area.
 *
 * Source of operation is pointed by SourcePath. Currently only client
 * application can be source. DevicePath identify destination of data.
 * DataLength specify size of data that should be received.
 *
 * Received data should follow specific GD format, means that every single unit
 * must be described with BlockNumber, UnitNumber and UnitSize or:
 *
 *   - 2 bytes for Block Number
 *   - 4 bytes for Unit Number
 *   - 2 bytes for Unit Size
 *   - "UnitSize" bytes of Unit Data
 *
 * After successful receiving of source data they are parsed and written in GD
 * storage area.
 *
 * Call source: PC
 *
 * @param [in] DevicePath_p   - Path to destination partition (cspsa...).
 * @param [in] DataLength     - Length of the data that need to be received.
 * @param [in] SourcePath_p   - not used.
 *
 * @return     @ref E_SUCCESS - Success ending.
 * @return     @ref E_GENERAL_FATAL_ERROR - If something unexpected happened.
 * @return     @ref E_INVALID_INPUT_PARAMETERS - If invalid input parameter
 *                                               is passed.
 *
 * @return     Function can also return some internal GD error codes.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ParameterStorage_WriteGlobalDataSetImpl(uint16 Session,
        const char *DevicePath_p,
        const uint64 DataLength,
        const char *SourcePath_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint8 *GD_WriteDataSet_p = NULL;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_PARAMETERSTORAGE_WRITEGLOBALDATASET;
    CmmData.ApplicationNr = GROUP_PARAMETERSTORAGE;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY(NULL != SourcePath_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(NULL != DevicePath_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(0 != DataLength, E_INVALID_INPUT_PARAMETERS);

    ReturnValue = Set_GD_Device(DevicePath_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    GD_WriteDataSet_p = (uint8 *)malloc((uint32)DataLength);
    ASSERT(NULL != GD_WriteDataSet_p);

    CmmData.Payload.Size = (uint32)DataLength;
    CmmData.Payload.Data_p = GD_WriteDataSet_p;

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;

ErrorExit:
    BUFFER_FREE(GD_WriteDataSet_p);
    BUFFER_FREE(InsertedDevice_p);

    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);
    return ReturnValue;
}

/**
 * @brief Erases the complete Global Data storage area that is specified.
 *
 * Call source: PC
 *
 * @param [in] DevicePath_p   - Path to destination partition (cspsa...)
 *
 * @return     @ref E_SUCCESS - Success ending.
 *
 * @return     Function can also return some internal GD error codes.
 *
 * @remark     see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Do_ParameterStorage_EraseGlobalDataSetImpl(uint16 Session,
        const char *DevicePath_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData = {0};

    CmmData.CommandNr = COMMAND_PARAMETERSTORAGE_ERASEGLOBALDATASET;
    CmmData.ApplicationNr = GROUP_PARAMETERSTORAGE;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;
    CmmData.Payload.Size = 0;
    CmmData.Payload.Data_p = NULL;

    VERIFY(NULL != DevicePath_p, E_INVALID_INPUT_PARAMETERS);

    ReturnValue = Set_GD_Device(DevicePath_p);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = E_SUCCESS;
    return ReturnValue;

ErrorExit:
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);
    BUFFER_FREE(InsertedDevice_p);

    return ReturnValue;
}

/*******************************************************************************
 *  Long running commands
 ******************************************************************************/
ErrorCode_e ParameterStorage_ReadGlobalDataUnitRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    void *Data_p = NULL;
    uint8 *GD_ReadDataUnit_p = NULL;
    uint32 UnitSize = 0;
    uint32 Unit_id = 0;

    //
    // Sanity check of input parameters
    //
    ASSERT(NULL != ExeContext_p);

    Data_p = ExeContext_p->Received.Payload.Data_p;
    Unit_id = get_uint32_le((void **)&Data_p);

    ReturnValue = Do_GD_GetUnitSize(Unit_id, &UnitSize);

    if (E_SUCCESS != ReturnValue) {
        B_(printf("parameter_applications_functions.c (%d): Get unit size failed.\n", __LINE__);)
        UnitSize = 0;
        goto ErrorExit;
    }

    GD_ReadDataUnit_p = (uint8 *)malloc(UnitSize);
    ASSERT(NULL != GD_ReadDataUnit_p);

    ReturnValue = Do_GD_ReadUnit(Unit_id, GD_ReadDataUnit_p, UnitSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    ReturnValue = Done_ParameterStorage_ReadGlobalDataUnit(ExeContext_p->Received.SessionNr, ReturnValue, UnitSize, GD_ReadDataUnit_p);
    ASSERT(E_SUCCESS == ReturnValue);

    ExeContext_p->Running = FALSE;
    BUFFER_FREE(GD_ReadDataUnit_p);
    BUFFER_FREE(InsertedDevice_p);
    return ReturnValue;
}

ErrorCode_e ParameterStorage_WriteGlobalDataUnitRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    void *TempData_p = NULL;
    void *Data_p = NULL;
    uint32 DataLength = 0;
    uint32 Unit_id = 0;

    //
    // Sanity check of input parameters
    //
    ASSERT(NULL != ExeContext_p);

    TempData_p = ExeContext_p->Received.Payload.Data_p;
    Unit_id = get_uint32_le((void **)&TempData_p);
    DataLength = get_uint32_le((void **)&TempData_p);

    Data_p = malloc(DataLength);
    ASSERT(NULL != Data_p);
    get_block(((const void **)&TempData_p), Data_p, DataLength);

    ReturnValue = Do_GD_WriteUnit(Unit_id, Data_p, DataLength);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    //
    // Write all cashed data from RAM to flash.
    //
    ReturnValue = Do_GD_Flush();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    ReturnValue = Done_ParameterStorage_WriteGlobalDataUnit(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    ExeContext_p->Running = FALSE;
    BUFFER_FREE(Data_p);
    BUFFER_FREE(InsertedDevice_p);

    return ReturnValue;
}

ErrorCode_e ParameterStorage_ReadGlobalDataSetRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    static GD_ReadSetParameters_t *Param_p = NULL;

    //
    // Sanity check of input parameters
    //
    ASSERT(NULL != ExeContext_p);

    if (Param_p == NULL) {
        Param_p = CreateInit_GD_ReadSetParameters();
        VERIFY(NULL != Param_p, E_GENERAL_FATAL_ERROR);
    }

    switch (Param_p->State) {
    case GD_READ_SET_COLLECT_DATA:
        ReturnValue = Handle_GD_ReadSet_CollectData(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case GD_READ_SET_INIT_DATA_TRANSFER:
        ReturnValue = Handle_GD_ReadSet_InitializeDataTransfer(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case GD_READ_SET_TRANSFER_DATA:
        ReturnValue = Handle_GD_ReadSet_TransferData(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case GD_READ_SET_FINISH:
        C_(printf("parameter_applications_functions.c (%d): All data successfully transfered.\n", __LINE__);)
        ReturnValue = E_SUCCESS;
        goto Exit;

    default:
        A_(printf("parameter_applications_functions.c (%d): Unknown State!\n", __LINE__);)
        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    return ReturnValue;

ErrorExit:
    //
    // Returned error values are intentionally ignored in order we can keep
    // original error that caused problem in first place.
    //
    (void)Do_IO_Close(Param_p->BulkHandle);

Exit:
    ReturnValue = Done_ParameterStorage_ReadGlobalDataSet(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    BUFFER_FREE(Param_p->GD_ReadDataSet_p);
    BUFFER_FREE(Param_p);

    BUFFER_FREE(InsertedDevice_p);
    ExeContext_p->Running = FALSE;

    return ReturnValue;
}

ErrorCode_e ParameterStorage_WriteGlobalDataSetRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    static GD_WriteSetParameters_t *Param_p = NULL;

    //
    // Sanity check of input parameters
    //
    ASSERT(NULL != ExecutionContext_p);

    if (Param_p == NULL) {
        Param_p = CreateInit_GD_WriteSetParameters();
        VERIFY(NULL != Param_p, E_GENERAL_FATAL_ERROR);
    }

    switch (Param_p->State) {
    case GD_WRITE_SET_INITIALIZE:
        ReturnValue = Handle_GD_WriteSet_Initialize(Param_p, ExecutionContext_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case GD_WRITE_SET_GET_DATA:
        ReturnValue = Handle_GD_WriteSet_GetData(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
        break;

    case GD_WRITE_SET_PROCESS_DATA:
        ReturnValue = Handle_GD_WriteSet_ProcessData(Param_p);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        goto Exit;

    default:
        A_(printf("parameter_applications_functions.c (%d): Unknown State\n", __LINE__);)
        ReturnValue = E_GENERAL_FATAL_ERROR;
        goto ErrorExit;
    }

    return ReturnValue;

ErrorExit:
    //
    // Returned error values are intentionally ignored in order we can keep
    // original error that caused problem in first place.
    //
    (void)Do_IO_Close(Param_p->BulkHandle);

Exit:
    ReturnValue = Done_ParameterStorage_WriteGlobalDataSet(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    BUFFER_FREE(Param_p);

    BUFFER_FREE(InsertedDevice_p);
    ExeContext_p->Running = FALSE;

    return ReturnValue;
}

ErrorCode_e ParameterStorage_EraseGlobalDataSetRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;

    //
    // Sanity check of input parameters
    //
    ASSERT(NULL != ExeContext_p);

    ReturnValue = Do_GD_Format();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    ReturnValue = Done_ParameterStorage_EraseGlobalDataSet(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    BUFFER_FREE(InsertedDevice_p);
    ExeContext_p->Running = FALSE;
    return ReturnValue;
}

ErrorCode_e ParameterStorage_ErrorHandlerRepeat(void *ExecutionContext_p)
{
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint8 *Data_p = NULL;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 CommandNr = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    ReturnValue = (ErrorCode_e)get_uint32_le((void **)&Data_p);
    CommandNr = get_uint16_le((void **)&Data_p);

    switch (CommandNr) {
    case COMMAND_PARAMETERSTORAGE_READGLOBALDATAUNIT:
        ReturnValue = Done_ParameterStorage_ReadGlobalDataUnit(ExeContext_p->Received.SessionNr, ReturnValue, 0, NULL);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_PARAMETERSTORAGE_WRITEGLOBALDATAUNIT:
        ReturnValue = Done_ParameterStorage_WriteGlobalDataUnit(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_PARAMETERSTORAGE_READGLOBALDATASET:
        ReturnValue = Done_ParameterStorage_ReadGlobalDataSet(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_PARAMETERSTORAGE_WRITEGLOBALDATASET:
        ReturnValue = Done_ParameterStorage_WriteGlobalDataSet(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    case COMMAND_PARAMETERSTORAGE_ERASEGLOBALDATASET:
        ReturnValue = Done_ParameterStorage_EraseGlobalDataSet(ExeContext_p->Received.SessionNr, ReturnValue);
        ASSERT(E_SUCCESS == ReturnValue);
        break;

    default:
        break;
    }

    if (0 != ExeContext_p->Received.Payload.Size) {
        BUFFER_FREE(ExeContext_p->Received.Payload.Data_p);
    }

    ExeContext_p->Running = FALSE;
    ReturnValue = E_SUCCESS;
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
static GD_ReadSetParameters_t *CreateInit_GD_ReadSetParameters(void)
{
    GD_ReadSetParameters_t *Param_p = NULL;

    Param_p = (GD_ReadSetParameters_t *)malloc(sizeof(GD_ReadSetParameters_t));

    if (Param_p) {
        Param_p->State                 = GD_READ_SET_COLLECT_DATA;
        Param_p->BulkHandle            = IO_INVALID_HANDLE;
        Param_p->GD_ReadDataSet_p      = NULL;
        Param_p->GD_ReadDataSize       = 0;
    }

    return Param_p;
}

static ErrorCode_e Handle_GD_ReadSet_CollectData(GD_ReadSetParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 UnitSize = 0;
    uint32 UnitNr = 0;

    //
    // Get number and size of first unit in the list
    //
    ReturnValue = Do_GD_GetUnitSizeFirst(&UnitNr, &UnitSize);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    //
    // Read all units from specified partition and place them in buffer
    //
    do {
        Param_p->GD_ReadDataSet_p = (uint8 *)realloc(Param_p->GD_ReadDataSet_p, Param_p->GD_ReadDataSize + UnitSize + UNIT_DESCRIPTOR_SIZE);
        ASSERT(NULL != Param_p->GD_ReadDataSet_p);

        ReturnValue = Do_GD_ReadUnit(UnitNr, Param_p->GD_ReadDataSet_p + Param_p->GD_ReadDataSize + UNIT_DESCRIPTOR_SIZE, UnitSize);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        //
        // Write Block Number in output buffer
        //
        Param_p->GD_ReadDataSet_p[Param_p->GD_ReadDataSize++] = 0x00;
        Param_p->GD_ReadDataSet_p[Param_p->GD_ReadDataSize++] = 0x00;

        //
        // Write Unit Number in output buffer
        //
        Param_p->GD_ReadDataSet_p[Param_p->GD_ReadDataSize++] = (uint8)UnitNr;
        Param_p->GD_ReadDataSet_p[Param_p->GD_ReadDataSize++] = (uint8)(0x000000ff & (UnitNr >> 8));
        Param_p->GD_ReadDataSet_p[Param_p->GD_ReadDataSize++] = (uint8)(0x000000ff & (UnitNr >> 16));
        Param_p->GD_ReadDataSet_p[Param_p->GD_ReadDataSize++] = (uint8)(0x000000ff & (UnitNr >> 24));

        //
        // Write Unit Size in output buffer
        //
        Param_p->GD_ReadDataSet_p[Param_p->GD_ReadDataSize++] = (uint8)UnitSize;
        Param_p->GD_ReadDataSet_p[Param_p->GD_ReadDataSize++] = (uint8)(0x00ff & ((uint16)(UnitSize >> 8)));

        Param_p->GD_ReadDataSize += UnitSize;

        ReturnValue = Do_GD_GetUnitSizeNext(&UnitNr, &UnitSize);
    } while (E_SUCCESS == ReturnValue);

    //
    // If end of partition is reached carry on otherwise return GR with detected error.
    //
    if (E_CSPSA_RESULT_E_END_OF_DATA != ReturnValue) {
        B_(printf("parameter_applications_functions.c (%d): ** ERR: Error reading unit size! (%d) **\n\n", __LINE__, ReturnValue);)
        goto ErrorExit;
    }

    Param_p->State = GD_READ_SET_INIT_DATA_TRANSFER;
    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Handle_GD_ReadSet_InitializeDataTransfer(GD_ReadSetParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    Param_p->BulkHandle = Do_IOW_BulkOpen(0, BULK_SEND, (uint64)Param_p->GD_ReadDataSize);
    VERIFY(IO_INVALID_HANDLE != Param_p->BulkHandle, E_IO_FAILED_TO_OPEN);

    Param_p->State = GD_READ_SET_TRANSFER_DATA;
    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Handle_GD_ReadSet_TransferData(GD_ReadSetParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint64 BytesWritten = 0;

    BytesWritten = Do_IO_PositionedWrite(Param_p->BulkHandle,
                                         Param_p->GD_ReadDataSet_p, (uint64)Param_p->GD_ReadDataSize,
                                         (uint64)Param_p->GD_ReadDataSize, 0);
    VERIFY(IO_ERROR != BytesWritten, IO_ErrorCode);

    if (BytesWritten == (uint64)Param_p->GD_ReadDataSize) {
        CLOSE_HANDLE(Param_p->BulkHandle);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Param_p->State = GD_READ_SET_FINISH;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    return ReturnValue;
}

static GD_WriteSetParameters_t *CreateInit_GD_WriteSetParameters(void)
{
    GD_WriteSetParameters_t *Param_p = NULL;

    Param_p = (GD_WriteSetParameters_t *)malloc(sizeof(GD_WriteSetParameters_t));

    if (Param_p) {
        Param_p->State                 = GD_WRITE_SET_INITIALIZE;
        Param_p->BulkHandle            = IO_INVALID_HANDLE;
        Param_p->GD_WriteDataSet_p      = NULL;
        Param_p->GD_WriteDataSize       = 0;
    }

    return Param_p;
}

static ErrorCode_e Handle_GD_WriteSet_Initialize(GD_WriteSetParameters_t *Param_p, ExecutionContext_t *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;

    Param_p->GD_WriteDataSet_p = ExecutionContext_p->Received.Payload.Data_p;
    Param_p->GD_WriteDataSize = (uint64)ExecutionContext_p->Received.Payload.Size;

    Param_p->BulkHandle = Do_IOW_BulkOpen(0, BULK_RECEIVE, Param_p->GD_WriteDataSize);
    VERIFY(IO_INVALID_HANDLE != Param_p->BulkHandle, E_IO_FAILED_TO_OPEN);

    Param_p->State = GD_WRITE_SET_GET_DATA;
    ReturnValue = E_SUCCESS;

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Handle_GD_WriteSet_GetData(GD_WriteSetParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;
    uint64 BytesRead = 0;

    BytesRead = Do_IO_PositionedRead(Param_p->BulkHandle, Param_p->GD_WriteDataSet_p, Param_p->GD_WriteDataSize, Param_p->GD_WriteDataSize, 0);
    VERIFY(IO_ERROR != BytesRead, IO_ErrorCode);

    if (BytesRead == Param_p->GD_WriteDataSize) {
        CLOSE_HANDLE(Param_p->BulkHandle);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Param_p->State = GD_WRITE_SET_PROCESS_DATA;
    }

ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Handle_GD_WriteSet_ProcessData(GD_WriteSetParameters_t *Param_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 UnitNr = 0;
    uint16 UnitSize = 0;
    uint16 BlockNr = 0;
    uint32 Offset = 0;

    //
    // Sanity check of input parameters
    //
    VERIFY(NULL != Param_p->GD_WriteDataSet_p, E_INVALID_INPUT_PARAMETERS);
    VERIFY(0 != Param_p->GD_WriteDataSize, E_INVALID_INPUT_PARAMETERS);

    do {
        VERIFY((Param_p->GD_WriteDataSize - Offset) > UNIT_DESCRIPTOR_SIZE, E_INVALID_INPUT_PARAMETERS);

        //
        // Get block number from input buffer
        //
        BlockNr  = Param_p->GD_WriteDataSet_p[Offset++];
        BlockNr |= Param_p->GD_WriteDataSet_p[Offset++] << 8;

        //
        // Get unit number from input buffer
        //
        UnitNr  = Param_p->GD_WriteDataSet_p[Offset++];
        UnitNr |= Param_p->GD_WriteDataSet_p[Offset++] << 8;
        UnitNr |= Param_p->GD_WriteDataSet_p[Offset++] << 16;
        UnitNr |= Param_p->GD_WriteDataSet_p[Offset++] << 24;

        //
        // Get unit size from input buffer
        //
        UnitSize  = Param_p->GD_WriteDataSet_p[Offset++];
        UnitSize |= Param_p->GD_WriteDataSet_p[Offset++] << 8;

        VERIFY(Param_p->GD_WriteDataSize >= (UnitSize + Offset), E_INVALID_INPUT_PARAMETERS);

        ReturnValue = Do_GD_WriteUnit(UnitNr, Param_p->GD_WriteDataSet_p + Offset, (uint32)UnitSize);
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

        Offset += UnitSize;
    } while (Offset < Param_p->GD_WriteDataSize);

    //
    // Write all cashed data from RAM to flash
    //
    ReturnValue = Do_GD_Flush();
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    IDENTIFIER_NOT_USED(BlockNr);
ErrorExit:
    return ReturnValue;
}

static ErrorCode_e Set_GD_Device(const char *DevicePath_p)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 PathLength = get_uint32_le((void **)&DevicePath_p);
    uint8 *Path_p = NULL;
    uint32 Length = 0;
    uint32 i = 0;

    VERIFY(PathLength <= MAX_DEVICE_PATH_LENGTH, E_INVALID_INPUT_PARAMETERS);

    Path_p = (uint8 *)malloc(PathLength + sizeof(char) + sizeof(char));
    VERIFY(NULL != Path_p, E_ALLOCATE_FAILED);

    memcpy(Path_p, DevicePath_p, PathLength);
    Path_p[PathLength] = '\0';

    if (Path_p[PathLength - 1] != '/') {
        PathLength++;
        Path_p[PathLength - 1] = '/';
        Path_p[PathLength] = '\0';
    }

    for (i = 1; i < PathLength - 1; i++) {
        if (Path_p[i] == '/') {
            Length = i;
        }
    }

    if (NULL != strstr((const char *) Path_p, "/TOC/")) {
        InsertedDevice_p = (char *)malloc(TOC_ID_LENGTH + 1);
        VERIFY(NULL != InsertedDevice_p, E_ALLOCATE_FAILED);

        memset(InsertedDevice_p, 0, TOC_ID_LENGTH + 1);

        //
        // Get the last element of the path as TOC ID
        //
        strncpy(InsertedDevice_p, (const char *) Path_p + Length + 1, TOC_ID_LENGTH);

        //
        // Remove '/' character from end of name
        //
        if (InsertedDevice_p[strlen(InsertedDevice_p) - 1] == '/') {
            InsertedDevice_p[strlen(InsertedDevice_p) - 1] = '\0';
            A_(printf("Inserted Device: %s\n", InsertedDevice_p);)
        }
    } else {
        A_(printf("parameter_applications_functions.c(%d): Unknown GD device!!!\n", __LINE__);)
        ReturnValue = E_INVALID_INPUT_PARAMETERS;
        goto ErrorExit;
    }

    ReturnValue = E_SUCCESS;
ErrorExit:
    BUFFER_FREE(Path_p);
    return ReturnValue;
}


/* @} */

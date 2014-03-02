/*******************************************************************************
 * $Copyright Ericsson AB 2009 $
 ******************************************************************************/

/*
 *  @addtogroup ldr_security_app
 *  @{
 */
#include "c_system.h"

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_memory_utils.h"
#include "error_codes.h"
#include "commands.h"
#include "command_ids.h"
#include "commands_impl.h"
#include "r_serialization.h"

#include "r_command_exec_service.h"
#include "r_io_subsystem.h"
#include "t_communication_service.h"
#include "r_path_utilities.h"
#include "t_loader_sec_lib.h"
#include "r_loader_sec_lib.h"
#include "r_loader_sec_lib_port_funcs.h"
#include "ces_commands.h"

#include "cops.h"
#include "cops_data_manager.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * @brief Writes a security data unit.
 *
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Security_SetPropertiesRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint32 Unit_id = 0;
    uint32 DataLength = 0;
    void *TempData_p = NULL;

    TempData_p = ExeContext_p->Received.Payload.Data_p;
    Unit_id = get_uint32_le((void **)&TempData_p);
    DataLength = get_uint32_le((void **)&TempData_p);

    C_( {
        uint32 ii;
        uint8 *Data_p = TempData_p;

        printf("security_8500_repeat_functions.c (%d): Data content, DataLength 0x%08x\n", __LINE__, DataLength);

        for (ii = 0; ii < DataLength;) {
            printf("%02x ", Data_p[ii]);
            ii++;

            if (ii % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
    }
      )

    ReturnValue = COPS_SetParameter(Unit_id, TempData_p, DataLength);

    ExeContext_p->Running = FALSE;

    ReturnValue = Done_Security_SetProperties(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}


/*
 * @brief Reads a security data unit.
 *
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Security_GetPropertiesRepeat(void *ExecutionContext_p)
{
    ErrorCode_e ReturnValue = E_SUCCESS;//E_GENERAL_FATAL_ERROR;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    void *TempData_p = NULL;
    uint32 Unit_id = 0;
    uint8 *Data_p = NULL;
    uint32 DataLength = 0;
    boolean Exists;

    TempData_p = ExeContext_p->Received.Payload.Data_p;
    Unit_id = get_uint32_le((void **)&TempData_p);

    ReturnValue = COPS_GetParameter(Unit_id, &Exists, &Data_p, &DataLength);

    ExeContext_p->Running = FALSE;

    ReturnValue = Done_Security_GetProperties(ExeContext_p->Received.SessionNr, ReturnValue, DataLength, Data_p);
    ASSERT(E_SUCCESS == ReturnValue);

    BUFFER_FREE(Data_p);

    return ReturnValue;
}


/*
 * @brief Associates all security data units with the current ME.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Security_BindPropertiesRepeat(void *ExecutionContext_p)
{
    ErrorCode_e                 ReturnValue;
    ExecutionContext_t         *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;

    A_(printf("u8500_security_applications_functions.c (%d): BindPropertiesRepeat\n", __LINE__);)

    ReturnValue = COPS_BindProperties();

    ExeContext_p->Running = FALSE;

    ReturnValue = Done_Security_BindProperties(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}


/*
 * @brief Initialize SW version table, intended for checking the ARB functionality.
 *
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Security_InitARBTableRepeat(void *ExecutionContext_p)
{
    ErrorCode_e                 ReturnValue;
    ExecutionContext_t         *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;

    uint32 Type = 0;
    uint32 DataLength = 0;
    void *TempData_p = NULL;


    TempData_p = ExeContext_p->Received.Payload.Data_p;
    Type = get_uint32_le((void **)&TempData_p);
    DataLength = get_uint32_le((void **)&TempData_p);

    C_(printf("u8500_security_applications_functions.c (%d): InitARBTableRepeat\n", __LINE__);)
    C_(printf("u8500_security_applications_functions.c: DataLength 0x%08x, Type (%x)\n", DataLength, Type);)

    ReturnValue = COPS_InitARBTable(Type, DataLength, TempData_p);

    ExeContext_p->Running = FALSE;

    ReturnValue = Done_Security_InitARBTable(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*
 * @brief Invokes programming of RPMB authentication key.
 *
 * @remark      see \c ErrorCode_e for detail description of the error codes.
 */
ErrorCode_e Security_WriteRPMBKeyRepeat(void *ExecutionContext_p)
{
    ErrorCode_e         ReturnValue;
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;

    uint32 dev_id = 0;
    uint32 commercial = 0;
    void *TempData_p = NULL;

    TempData_p = ExeContext_p->Received.Payload.Data_p;
    dev_id = get_uint32_le((void **)&TempData_p);
    commercial = get_uint32_le((void **)&TempData_p);

    C_(printf("security_applications_repeat_functions.c(%d): WriteRPMBKeyRepeat\n", __LINE__);)

    ReturnValue = COPS_WriteRPMBKey(dev_id, commercial);

    ExeContext_p->Running = FALSE;

    ReturnValue = Done_Security_WriteRPMBKey(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/* @}*/


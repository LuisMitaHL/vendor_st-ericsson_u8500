/*******************************************************************************
 * $Copyright ST-Ericsson 2011 $
 ******************************************************************************/

/**
 * @file   otp_applications_functions.c
 * @brief  Application level functions for OTP functions
 *
 * @addtogroup ldr_otp_app
 * @{
 */

/*******************************************************************************
* Includes
*******************************************************************************/
#include "c_system.h"
#include <string.h>
#include <stdlib.h>

#include "commands.h"
#include "commands_impl.h"
#include "ces_commands.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "t_communication_service.h"
#include "r_command_exec_service.h"
#include "r_serialization.h"
#include "r_basicdefinitions.h"

#include "r_io_subsystem.h"
#include "r_path_utilities.h"
#include "r_memory_utils.h"
#include "ces_commands.h"
#include "otp_applications_functions.h"
#include "cops.h"
#include "cops_data_manager.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/* Defines word address used to access the fuse bits */
#define BIT2WORD(Bits) ((uint32)(Bits) / 32)
#define OTP_ROWS 46
#define OTP_FIELDS 2
#define OTP_AREA_SIZE (OTP_ROWS*OTP_FIELDS)  // 46 rows x 2 fields
#define START_BIT_MASK 0xFC0 // Mask used to ensure that start bit is aligned on beginning of a row in OTP area

#ifdef OTP_SIMULATOR
#define OTP_RAW_INDEX_MASK (0x0000FFFF)
static uint32 OTP_Simulator_Area[OTP_AREA_SIZE] = {0};
uint32 OTP_Row_Index = 0;
uint32 Counter = 0;
#endif


/*******************************************************************************
 * Declaration of internal functions
 ******************************************************************************/
#ifdef OTP_SUPPORT
static void Format_OTP_Data_Vector(uint32 *OTP_Data_p, uint32 Start_Word, uint32 Length_Words);
#endif


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/***************************************************************************//**
* @brief Reads bits from OTP memory.
*
* Reads all requested Length bits from OTP.
*
* @param[in]      Session   Command session.
* @param[in]      OTP_id    NOT used.
* @param[in]      Start     NOT used.
* @param[in]      Length    Number of bits to read.
*
* @return         E_INVALID_INPUT_PARAMETERS if input parameters are invalid.
*                 E_ALLOCATE_FAILED if memory allocation failed.
*                 E_READING_OTP_FAILED if reading from OTP failed.
*                 E_SUCCESS if reading was successful.
*
*******************************************************************************/
ErrorCode_e Do_OTP_ReadBitsImpl(uint16 Session, const uint32 OTP_id,
                                const uint32 Start, const uint32 Length)
{
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    CommandData_t CmmData   = {0};
    void *Data_p            = NULL;
    IDENTIFIER_NOT_USED(OTP_id);

    CmmData.CommandNr = COMMAND_OTP_READBITS;
    CmmData.ApplicationNr = GROUP_OTP;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY((Start & START_BIT_MASK) == Start, E_INVALID_INPUT_PARAMETERS);

    CmmData.Payload.Size = sizeof(uint32) + sizeof(uint32);
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Start);
    put_uint32_le((void **)&Data_p, Length);

    C_(printf("otp_applications_functions.c (%d): Do_OTP_ReadBitsImpl: OTP_AREA_SIZE = %d \n", __LINE__, OTP_AREA_SIZE);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_ReadBitsImpl: Start = %d\n", __LINE__, Start);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_ReadBitsImpl: Length = %d\n", __LINE__, Length);)

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;

ErrorExit:
    B_(printf("otp_applications_functions.c (%d): *** Do_OTP_ReadBitsImpl: ReturnValue = (%x) ***\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/***************************************************************************//**
* @brief Write OTP bits in OTP.
*
* Writes all requested DataBitsLength bits, in OTP.
*
* @param[in]      Session        Command session.
* @param[in]      OTP_id         NOT used.
* @param[in]      Start          NOT used.
* @param[in]      BitsLength     NOT used.
* @param[in]      DataBitsLength Number of bits from DataBits_p.
* @param[in]      DataBits_p     Buffer with bit data.
*
* @return         E_INVALID_INPUT_PARAMETERS if input parameters are invalid.
*                 E_WRITTING_OTP_FAILED if writing in OTP memory failed.
*                 E_SUCCESS if writing was successful.
*
*******************************************************************************/
ErrorCode_e Do_OTP_SetBitsImpl(uint16 Session, const uint32 OTP_id, const uint32 Start, const uint32 BitsLength, const uint32 DataBitsLength, void *DataBits_p)
{
    ErrorCode_e ReturnValue  = E_GENERAL_FATAL_ERROR;
    void *Data_p = NULL;
    CommandData_t CmmData    = {0};
    CmmData.Payload.Data_p   = NULL;
    uint32 Start_Bit    = Start;
    uint32 Start_Word   = 0;
    uint32 End_Word     = 0;
    uint32 Length_Words = 0;
    uint32 LengthBytes  = 0;

    IDENTIFIER_NOT_USED(OTP_id);

    CmmData.CommandNr = COMMAND_OTP_SETBITS;
    CmmData.ApplicationNr = GROUP_OTP;
    CmmData.SessionNr = Session;
    CmmData.Type = COMMAND_TYPE;

    VERIFY((Start_Bit & START_BIT_MASK) == Start_Bit, E_INVALID_INPUT_PARAMETERS);

    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: OTP_AREA_SIZE = %d\n", __LINE__, OTP_AREA_SIZE);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: Start = %d\n", __LINE__, Start);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: Start_Bit = %d\n", __LINE__, Start_Bit);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: BitsLength = %d\n", __LINE__, BitsLength);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: DataBitsLength = %d\n", __LINE__, DataBitsLength);)

    /* Input parameters are in bits, we have to switch in words */
    Start_Word   = BIT2WORD(Start_Bit);
    End_Word     = BIT2WORD(Start_Bit + BitsLength - 1);
    Length_Words = End_Word - Start_Word + 1;
    LengthBytes  = (Length_Words) * sizeof(uint32);

    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: Start_Word = %d\n", __LINE__, Start_Word);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: End_Word = %d\n", __LINE__, End_Word);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: Length_Words = %d\n", __LINE__, Length_Words);)
    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsImpl: LengthBytes = %d\n", __LINE__, LengthBytes);)

    VERIFY(Start_Word + Length_Words <= OTP_AREA_SIZE, E_INVALID_INPUT_PARAMETERS);

    CmmData.Payload.Size = sizeof(uint32) + sizeof(uint32) + LengthBytes;
    CmmData.Payload.Data_p = (uint8 *)malloc(CmmData.Payload.Size);
    ASSERT(NULL != CmmData.Payload.Data_p);

    Data_p = CmmData.Payload.Data_p;
    put_uint32_le((void **)&Data_p, Length_Words);
    put_uint32_le((void **)&Data_p, LengthBytes);
    put_block((void **)&Data_p, DataBits_p, LengthBytes);

    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);

    return ReturnValue;

ErrorExit:
    B_(printf("otp_applications_functions.c (%d): *** Do_OTP_SetBitsImpl: ReturnValue = (%x) ***\n", __LINE__, ReturnValue);)
    Do_CES_SetErrorHandlingData(&CmmData, ReturnValue);
    ReturnValue = Do_CES_RegisterRepeatCommand(&CmmData);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/*******************************************************************************
 *  Long running commands
 ******************************************************************************/

/**
 * @brief Reads bits from OTP memory.
 *
 * @param [in, out] ExecutionContext_p   is pointer to structure that holds
 *                                       information about a command that is
 *                                       placed (or will be placed) in the
 *                                       command execution queue.
 * @return   @ref E_SUCCESS              Successful ending.
 *
 * @return   @ref E_INVALID_INPUT_PARAMETERS - If CmdData_p is NULL.
 *
 * @return   @ref E_ALLOCATE_FAILED      If memory allocation failed.
 * @return   @ref E_READING_OTP_FAILED   If reading from OTP failed.
 */
ErrorCode_e OTP_ReadBitsRepeat(void *ExecutionContext_p)
{
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint32 LengthBits   = 0;
    uint32 LengthBytes  = 0;
    uint32 *OTP_Data_p  = NULL;
#ifndef OTP_SUPPORT
    ErrorCode_e ReturnValue = E_UNSUPPORTED_CMD;
#else
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    void *Data_p        = NULL;
    uint32 StartBit     = 0;
    uint32 Start_Word   = 0;
    uint32 End_Word     = 0;
    uint32 Length_Words = 0;


    Data_p = ExeContext_p->Received.Payload.Data_p;
    StartBit = get_uint32_le((void **)&Data_p);
    LengthBits = get_uint32_le((void **)&Data_p);

    C_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: StartBit = %d\n", __LINE__, StartBit);)
    C_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: LengthBits = %d\n", __LINE__, LengthBits);)

    /* Input parameters are in bits, we have to switch in words and bytes */
    Start_Word   = BIT2WORD(StartBit);
    End_Word     = BIT2WORD(StartBit + LengthBits - 1);
    Length_Words = End_Word - Start_Word + 1;
    LengthBytes  = (Length_Words) * sizeof(uint32);

    C_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: Start_Word = %d\n", __LINE__, Start_Word);)
    C_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: End_Word = %d\n", __LINE__, End_Word);)
    C_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: Length_Words = %d\n", __LINE__, Length_Words);)
    C_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: LengthBytes = %d\n", __LINE__, LengthBytes);)

    VERIFY(Start_Word + Length_Words <= OTP_AREA_SIZE, E_INVALID_INPUT_PARAMETERS);

    OTP_Data_p = (uint32 *)malloc(LengthBytes);
    VERIFY(OTP_Data_p != NULL, E_ALLOCATE_FAILED);
    memset(OTP_Data_p, 0x00, LengthBytes);
    Format_OTP_Data_Vector(OTP_Data_p, Start_Word, Length_Words);

#ifndef OTP_SIMULATOR
    /* Physical OTP Read */
    C_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: Reading from Physical OTP Area!\n", __LINE__);)
    ReturnValue = COPS_ReadOTP((uint32_t *)OTP_Data_p, LengthBytes);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
    C_(

    for (uint32 i = 0; i < Length_Words; i++) {
    printf("0x%08X ", OTP_Data_p[Start_Word + i]);

        if ((i + 1) % 8 == 0 && (i + 1) < Length_Words) {
            printf("\n");
        }
    }
    )
#else
    /* Simulated OTP Read */
    A_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: Reading from Simulated OTP Area!\n", __LINE__);)
    A_(printf("otp_applications_functions.c (%d): OTP_ReadBitsRepeat: OTP_Simulator_Area = \n", __LINE__);)

    /* Copy from Simulated OTP Area to output buffer */
    for (uint32 i = 0; i < Length_Words; i++) {
        OTP_Data_p[i] = OTP_Simulator_Area[Start_Word + i];
        A_(
            printf("0x%08X ", OTP_Simulator_Area[Start_Word + i]);

            if ((i + 1) % 8 == 0 && (i + 1) < Length_Words)
            printf("\n");
        )
        }

    A_(printf("\n");)

#endif
    ReturnValue = E_SUCCESS;
ErrorExit:
#endif //OTP_SUPPORT

    if (NULL == OTP_Data_p) {
        OTP_Data_p = (uint32 *)malloc(sizeof(uint32));
        ASSERT(NULL != OTP_Data_p);
        memset(OTP_Data_p, 0x00, sizeof(uint32));
        LengthBytes = 0;
    }

    ReturnValue = Done_OTP_ReadBits(ExeContext_p->Received.SessionNr, ReturnValue, LengthBits, LengthBytes, (void *)OTP_Data_p, 0, 0, (void *)NULL);
    ASSERT(E_SUCCESS == ReturnValue);
    ExeContext_p->Running = FALSE;

    BUFFER_FREE(OTP_Data_p);

    return ReturnValue;
}

/**
 * @brief Sets OTP bits in RAM OTP copy.
 *
 * @param [in, out] ExecutionContext_p   is pointer to structure that holds
 *                                       information about a command that is
 *                                       placed (or will be placed) in the
 *                                       command execution queue.
 * @return   @ref E_SUCCESS              Successful ending.
 *
 * @return   @ref E_INVALID_INPUT_PARAMETERS   If CmdData_p is NULL.
 *
 * @return   @ref E_WRITTING_OTP_FAILED  If writing in OTP memory failed.
 */
ErrorCode_e OTP_SetBitsRepeat(void *ExecutionContext_p)
{
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
#ifndef OTP_SUPPORT
    ErrorCode_e ReturnValue = E_UNSUPPORTED_CMD;
#else
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    void *Data_p = NULL;
    uint32 *OTP_Data_p = NULL;
    uint32 Length_Words = 0;
    uint32 LengthBytes  = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    VERIFY(Data_p != NULL, E_INVALID_INPUT_PARAMETERS);
    Length_Words = get_uint32_le((void **)&Data_p);
    LengthBytes = get_uint32_le((void **)&Data_p);

    C_(printf("otp_applications_functions.c (%d): OTP_SetBitsRepeat: Length_Words = %d\n", __LINE__, Length_Words);)
    C_(printf("otp_applications_functions.c (%d): OTP_SetBitsRepeat: LengthBytes = %d\n", __LINE__, LengthBytes);)

    OTP_Data_p = (uint32 *)malloc(LengthBytes);
    ASSERT(NULL != OTP_Data_p);
    get_block((const void **)&Data_p, OTP_Data_p, LengthBytes);

#ifndef OTP_SIMULATOR
    /* Physical OTP Write */
    C_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsRepeat: Writing to Physical OTP Area!\n", __LINE__);)
    C_(

    for (uint32 i = 0; i < Length_Words; i++) {
    printf("0x%08X ", OTP_Data_p[i]);

        if ((i + 1) % 8 == 0 && (i + 1) < Length_Words) {
            printf("\n");
        }
    }
    printf("\n");
    )
    ReturnValue = COPS_WriteOTP((uint32_t *)OTP_Data_p, LengthBytes);
    VERIFY(E_SUCCESS == ReturnValue, ReturnValue);
#else
    /* Simulated OTP Write */
    A_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsRepeat: Writing to Simulated OTP Area! \n", __LINE__);)
    A_(printf("otp_applications_functions.c (%d): Do_OTP_SetBitsRepeat: OTP_Simulator_Area = \n", __LINE__);)

    /* Copy from input buffer to Simulated OTP Area */
    while (Counter < Length_Words) {
        OTP_Row_Index = OTP_Data_p[Counter] & OTP_RAW_INDEX_MASK;
        OTP_Simulator_Area[OTP_Row_Index *OTP_FIELDS] = OTP_Data_p[Counter];
        OTP_Simulator_Area[OTP_Row_Index *OTP_FIELDS + 1] = OTP_Data_p[Counter + 1];
        Counter += 2;
    }

    Counter = 0;
#endif
    ReturnValue = E_SUCCESS;
ErrorExit:
    BUFFER_FREE(OTP_Data_p);
#endif //OTP_SUPPORT
    ExeContext_p->Running = FALSE;

    ReturnValue = Done_OTP_SetBits(ExeContext_p->Received.SessionNr, ReturnValue);
    ASSERT(E_SUCCESS == ReturnValue);

    return ReturnValue;
}

/**
* The function collects progress status
* form a buffer stored in the MEs RAM memory
*
* @param [in, out] ExecutionContext_p   is pointer to structure that holds
*                                       information about a command that is
*                                       placed (or will be placed) in the
*                                       command execution queue.
*
* @retval \c ErrorCode_e for more details.
*/
ErrorCode_e OTP_ErrorHandlerRepeat(void *ExecutionContext_p)
{
    ExecutionContext_t *ExeContext_p = (ExecutionContext_t *)ExecutionContext_p;
    uint8 *Data_p = NULL;
    ErrorCode_e ReturnValue = E_GENERAL_FATAL_ERROR;
    uint32 CommandNr = 0;

    Data_p = ExeContext_p->Received.Payload.Data_p;
    ReturnValue = (ErrorCode_e)get_uint32_le((void **)&Data_p);
    CommandNr = get_uint16_le((void **)&Data_p);

    switch (CommandNr) {
    case COMMAND_OTP_READBITS:
        ReturnValue = Done_OTP_ReadBits(ExeContext_p->Received.SessionNr, ReturnValue, 0, 0, NULL, 0, 0, NULL);
        ASSERT(E_SUCCESS == ReturnValue);
        break;
    case COMMAND_OTP_SETBITS:
        ReturnValue = Done_OTP_SetBits(ExeContext_p->Received.SessionNr, ReturnValue);
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

#ifdef OTP_SUPPORT
static void Format_OTP_Data_Vector(uint32 *OTP_Data_p, uint32 Start_Word, uint32 Length_Words)
{

    uint32 Counter = 0;
    uint32 RowIndex = Start_Word;

    while (Counter < Length_Words) {
        OTP_Data_p[Counter] = RowIndex;
        RowIndex++;
        Counter += 2;
    }
}
#endif

/** @}*/

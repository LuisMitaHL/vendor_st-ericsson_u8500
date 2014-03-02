/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 *  @addtogroup ldr_security_library
 *  @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "t_basicdefinitions.h"
#include "e_loader_sec_lib.h"
#include "r_loader_sec_lib.h"
#include "loader_sec_lib_internals.h"
#include "r_loader_sec_lib_port_funcs.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_copsrom.h"
#include "header_definitions.h"
#include "t_loader_sec_lib.h"
#include "jmptable.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/
/* Authentication level mask */
#define MASK_AUTHENTICATION_LEVEL 0x000000ff
/* Challenge data block version */
#define CB_VERSION  1

/* Challenge data block */
AuthenticationChallengeDataBlock_t CDBData;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static LoaderSecLib_Output_t LoaderSecLib_CreatePermissionVector(Authentication_Control_Data_t *Data_p, uint32 PermissionLevel, uint32 AllowedOperation, uint32 AllowedDomains);
static LoaderSecLib_Output_t LoaderSecLib_CreateDefaultPermissionLevel(Authentication_Control_Data_t *AccesData_p);
static LoaderSecLib_Output_t LoaderSecLib_VerifyAuthChallengeResponseBlock(uint8 *Data_p, uint32 DataLength);


/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 * Authentication data block Initialization.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success initialization.
 * @retval .
 */
LoaderSecLib_Output_t LoaderSecLib_InitAuthentication(void)
{
    LoaderSecLib_Output_t     ReturnValue = LOADER_SEC_LIB_FAILURE;
    Authentication_Control_Data_t *Data_p = NULL;

    ReturnValue = LoaderSecLib_CreateDataBlock((void **)&Data_p, sizeof(Authentication_Control_Data_t));
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = LoaderSecLib_CreateDefaultPermissionLevel(Data_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = LoaderSecLib_AddInternalDataBlock(INTERNAL_DATA_BLOCK_AUTHENTICATION, (void *)Data_p, sizeof(Authentication_Control_Data_t));
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/*
 * Authenticate loader.
 *
 * @param[in] Type    Type of authentication method.
 * @param[in] Data_p  Buffer with authentication data.
 * @param[in] Length  Length of the buffer Data_p.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success authentication.
 * @retval LOADER_SEC_LIB_INVALID_AUTHENTICATION_TYPE Invalid authentication type.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Authenticate(uint32 Type, void *Data_p, uint32 Length)
{
    LoaderSecLib_Output_t     ReturnValue;
    Authentication_Control_Data_t *AuthControlData_p = NULL;
    uint32 AuthPurpose = 0;
    uint32 PLevel = 0;
    uint32 AllowedOperation = 0;
    uint32 AllowedDomains = 0;

    VERIFY(((NULL != Data_p) && (Length != 0)), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    /*check the authentication counter */
    ReturnValue = LoaderSecLib_GetInternalDataBlock(INTERNAL_DATA_BLOCK_AUTHENTICATION, (void **)&AuthControlData_p);
    VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), ReturnValue);
    VERIFY(NULL != AuthControlData_p, LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    switch (Type) {
        /* Control Keys Authentication */
    case    LOADER_SEC_LIB_CONTROL_KEYS_AUTHENTICATION:

        if (AuthControlData_p->A1AuthCounter < MAX_NO_OF_A1_AUTHENTICATION) {
            ReturnValue = Do_LoaderSecLib_ControlKey_Authenticaion((uint8 *)Data_p, Length);

            if (ReturnValue == LOADER_SEC_LIB_SUCCESS) {
                // set allowed change operations
                ReturnValue = LoaderSecLib_CreatePermissionVector(AuthControlData_p, LOADER_SEC_LIB_PRODUCTION_MODE_LEVEL, AllowedOperation, AllowedDomains);
                VERIFY((ReturnValue == LOADER_SEC_LIB_SUCCESS), ReturnValue);
                AuthControlData_p->A1AuthCounter = 0;
            }
        } else {
            ReturnValue = LOADER_SEC_LIB_EXCEEDED_NUMBER_OF_AUTHENTICATION;
        }

        break;
        /* CA Authentication */
    case    LOADER_SEC_LIB_CERTIFICATE_AUTHENTICATION:

        if (AuthControlData_p->CAAuthCounter < MAX_NO_OF_CA_AUTHENTICATION) {
            ReturnValue = Do_LoaderSecLib_CA_Authenticaion((uint8 *)Data_p, Length, &AuthPurpose);

            if (ReturnValue == LOADER_SEC_LIB_SUCCESS) {
                // Verify Authentication challendge response block.
                ReturnValue = LoaderSecLib_VerifyAuthChallengeResponseBlock((uint8 *)Data_p, Length);
                VERIFY((ReturnValue == LOADER_SEC_LIB_SUCCESS), ReturnValue);
                // get Permission vector from the CA certificate
                PLevel = ((AuthPurpose >> 8) & MASK_AUTHENTICATION_LEVEL);
                AllowedOperation = AuthPurpose & MASK_ALLOWED_OPERATION;
                ReturnValue = LoaderSecLib_CreatePermissionVector(AuthControlData_p, PLevel, AllowedOperation, AllowedDomains);
                VERIFY((ReturnValue == LOADER_SEC_LIB_SUCCESS), ReturnValue);
                AuthControlData_p->CAAuthCounter = 0;
            }
        } else {
            ReturnValue = LOADER_SEC_LIB_EXCEEDED_NUMBER_OF_AUTHENTICATION;
        }

        break;
        /* Undefined Athentication */
    default:
        ReturnValue = LOADER_SEC_LIB_INVALID_AUTHENTICATION_TYPE;
        break;
    }

ErrorExit:

    if (LOADER_SEC_LIB_SUCCESS != ReturnValue) {
        if (Type == LOADER_SEC_LIB_CONTROL_KEYS_AUTHENTICATION) {
            if (NULL != AuthControlData_p) {
                AuthControlData_p->A1AuthCounter++;
            }
        } else {
            if (Type == LOADER_SEC_LIB_CERTIFICATE_AUTHENTICATION) {
                if (NULL != AuthControlData_p) {
                    AuthControlData_p->CAAuthCounter++;
                }
            }
        }
    }

    C_(printf("loader_sec_lib.c (%d):CAAuthCounter(%d) A1AuthCounter(%d)\n", __LINE__, AuthControlData_p->CAAuthCounter, AuthControlData_p->A1AuthCounter);)
    B_(printf("loader_sec_lib.c (%d): return... (%d)\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*
 * Audit permision level.
 *
 * @param[in]  CmdPermission_p  Pointer to structure that hold command
 *                              permission levels.
 *
 * @retval LOADER_SEC_LIB_SUCCESS        Successful unpacking.
 * @retval LOADER_SEC_LIB_FAILURE        Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Audit_Permission_Levels(CommandPermissionList_t *CmdPermission_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    Authentication_Control_Data_t *AuthControlData_p = NULL;
    int Index = 0;

    VERIFY(NULL != CmdPermission_p, ReturnValue);
    ReturnValue = LoaderSecLib_GetInternalDataBlock(INTERNAL_DATA_BLOCK_AUTHENTICATION, (void **)&AuthControlData_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);
    C_(printf("loader_sec_lib.c (%d): PLevel=0x%x\n", __LINE__, AuthControlData_p->PLevel);)

    ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;

    for (Index = 0; Index < CmdPermission_p->CommmandPermissionLength; Index++) {
        C_(printf("loader_sec_lib.c (%d): CmdPLevel[%d]=0x%x\n", __LINE__, Index, CmdPermission_p->CommmandPermmision_p[Index]);)

        if (AuthControlData_p->PLevel == CmdPermission_p->CommmandPermmision_p[Index]) {
            ReturnValue = LOADER_SEC_LIB_SUCCESS;
            goto ErrorExit;
        }
    }

ErrorExit:
    return ReturnValue;
}


/*
 * Create Authentication challendge data block
 *
 * @param[in] ChallengeBlock_p       Pointer to the created challendge block.
 * @param[in] ChallengeBlockLength_p Length of the created challendge block.
 *
 * @retvalue LOADER_SEC_LIB_SUCCESS
 * @retvalue LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC
 */
LoaderSecLib_Output_t Do_LoaderSecLib_CreateAuthChallengeDataBlock(AuthenticationChallengeDataBlock_t *ChallengeBlock_p, uint32 *ChallengeBlockLength_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;

    VERIFY((ChallengeBlock_p != NULL) && (ChallengeBlockLength_p != NULL), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    ChallengeBlock_p->CB_Version = CB_VERSION;
    ChallengeBlock_p->Device_Id = LoaderSecLib_Read_Chip_Id();
    ChallengeBlock_p->AddDataLenght = 0;
    *ChallengeBlockLength_p = ChallengeBlock_p->AddDataLenght + sizeof(ChallengeBlock_p->CB_Version) + sizeof(ChallengeBlock_p->Device_Id) + sizeof(ChallengeBlock_p->Random) + sizeof(ChallengeBlock_p->AddDataLenght);

    ReturnValue = Do_LoaderSecLib_GenerateRandomData((uint8 *)&ChallengeBlock_p->Random[0]);
    VERIFY(ReturnValue == LOADER_SEC_LIB_SUCCESS, ReturnValue);

    /* Save the created Challenge data block */
    memcpy(&CDBData, ChallengeBlock_p, *ChallengeBlockLength_p);
    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    B_(printf("loader_sec_lib.c (%d): return... (%d)\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/

/*
 * Create permission vector.
 *
 * @param[in] PermissionLevel  Requested permission level.
 * @param[in] AllowedOperation Allowed change operation.
 * @param[in] AllowedDomains   Alloed domains for allowed change operation.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Successfully created permission level.
 */
static LoaderSecLib_Output_t LoaderSecLib_CreatePermissionVector(Authentication_Control_Data_t *Data_p, uint32 PermissionLevel, uint32 AllowedOperation, uint32 AllowedDomains)
{
    LoaderSecLib_Output_t   ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;

    Data_p->PLevel = PermissionLevel;
    Data_p->AllowedOperation = AllowedOperation;
    Data_p->AllowedDomains = AllowedDomains;
    C_(printf("loader_sec_lib.c (%d):PermissionLevel(0x%x) AllowedOperation(0x%x) AllowedDomains(0x%x)\n", __LINE__, Data_p->PLevel, Data_p->AllowedOperation, Data_p->AllowedDomains);)

    ReturnValue = LoaderSecLib_UpdateInternalDataBlock(INTERNAL_DATA_BLOCK_AUTHENTICATION);
    B_(printf("loader_sec_lib.c (%d): return... (%d)\n", __LINE__, ReturnValue);)

    return ReturnValue;
}

/*
 * Create default permition level.
 *
 * @param[in] Data_p pointer to the auth. data block.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Successfully created default permission level.
 * @retval .
 */
static LoaderSecLib_Output_t LoaderSecLib_CreateDefaultPermissionLevel(Authentication_Control_Data_t *AccessData_p)
{
    LoaderSecLib_Output_t          ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    COPSROM_Security_Settings_t   *SecuritySettings_p = NULL;
    LOADER_SEC_LIB_OTP_t           OTP;
    DomainsData_t                 *Domain_Data_p = NULL;

    /* TODO: check DEBUG flag. */

    memset(&OTP, 0x00, sizeof(LOADER_SEC_LIB_OTP_t));

    ReturnValue = Do_LoaderSecLib_GetSecuritySettings(&SecuritySettings_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    /* Read OTP data from security library.*/
    ReturnValue = Do_LoaderSecLib_Read_OTP(&OTP);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    //Get written domain from domain internal data block.
    ReturnValue = LoaderSecLib_GetInternalDataBlock(INTERNAL_DATA_BLOCK_DOMAINS, (void **)&Domain_Data_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    if ((FALSE == SecuritySettings_p->SecuredSettings.OTPValues.MainSecurity) || (0 == OTP.OTP_Lock_Status) || (FALSE == OTP.PAF)) {
        // allow full access
        AccessData_p->PLevel = LOADER_SEC_LIB_PRODUCTION_MODE_LEVEL;
        AccessData_p->AllowedOperation = ALL_ALLOWED_CHANGE_OPERATION;
        AccessData_p->AllowedDomains = ALL_ALLOWED_DOMAINS;
    } else { // no debug
        if ((TRUE == SecuritySettings_p->SecuredSettings.OTPValues.MainSecurity) && (0 != OTP.OTP_Lock_Status) && (TRUE == OTP.PAF)) {
            if (Domain_Data_p->EffectiveDomain & (LOADER_SEC_LIB_DOMAIN_RnD | LOADER_SEC_LIB_DOMAIN_FACTORY)) {
                AccessData_p->PLevel = LOADER_SEC_LIB_PRODUCTION_MODE_LEVEL;
                AccessData_p->AllowedOperation = ALL_ALLOWED_CHANGE_OPERATION;
                AccessData_p->AllowedDomains = ALL_ALLOWED_DOMAINS;
            } else if (Domain_Data_p->EffectiveDomain & LOADER_SEC_LIB_DOMAIN_PRODUCT) {
                AccessData_p->PLevel = LOADER_SEC_LIB_FLASH_MODE_LEVEL;
                AccessData_p->AllowedOperation = 0;
                AccessData_p->AllowedDomains = 0;
            } else { // domain service
                AccessData_p->PLevel = LOADER_SEC_LIB_SERVICE_MODE_LEVEL;
                AccessData_p->AllowedOperation = 0;
                AccessData_p->AllowedDomains = 0;
            }
        }
    }

    AccessData_p->CAAuthCounter = 0;
    AccessData_p->A1AuthCounter = 0;
    C_(printf("loader_sec_lib.c (%d): PLevel=0x%x\n", __LINE__, AccessData_p->PLevel);)
    C_(printf("loader_sec_lib.c (%d): AllowedOperation=0x%x\n", __LINE__, AccessData_p->AllowedOperation);)
    C_(printf("loader_sec_lib.c (%d): AllowedDomains=0x%x\n", __LINE__, AccessData_p->AllowedDomains);)

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * Verify Authentication challendge response block.
 *
 * @param[in] Data_p       Pointer to the received buffer.
 * @param[in] DataLength   Length of the the received buffer.
 *
 * @retvalue LOADER_SEC_LIB_SUCCESS
 * @retvalue LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC
 */
static LoaderSecLib_Output_t LoaderSecLib_VerifyAuthChallengeResponseBlock(uint8 *Data_p, uint32 DataLength)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    AuthenticationChallengeDataBlock_t *RecAuthChData_p = NULL;
    COPSROM_Control_t *Header_p = (COPSROM_Control_t *)Data_p;
    uint32 HeaderLength = 0;

    VERIFY((Data_p != NULL) && (DataLength != 0), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    /*
     * The header length must be aligned to 8 bytes..
     * So if the header is unaligned, we need to add until we have even.
     */
    HeaderLength = Header_p->hd_length;

    if (0 != (HeaderLength & MASK_FIRST_3_BITS)) { // align to 64 bit
        HeaderLength = ((HeaderLength & CLEAR_FIRST_3_BITS) + sizeof(uint64));
    }

    RecAuthChData_p = (AuthenticationChallengeDataBlock_t *)(Data_p + HeaderLength);

    // verify CB_Version, Device_Id in the Authentication challendge response block
    VERIFY(RecAuthChData_p->CB_Version == CDBData.CB_Version, LOADER_SEC_LIB_INVALID_CHALLENGE_DATA_BLOCK);
    VERIFY(RecAuthChData_p->Device_Id == CDBData.Device_Id, LOADER_SEC_LIB_INVALID_CHALLENGE_DATA_BLOCK);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    B_(printf("loader_sec_lib.c (%d): return... (%d)\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/** @} */

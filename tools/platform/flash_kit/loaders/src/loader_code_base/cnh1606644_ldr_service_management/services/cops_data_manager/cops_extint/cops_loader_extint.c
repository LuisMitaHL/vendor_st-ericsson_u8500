/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*************************************************************************
 *
 * $Workfile: cops_loader_extint.c $
 *
 * DESCRIPTION:
 *
 * ORGANISATION OF THE FILE:
 *
 * 1. Includes
 * 2. External constants and variables
 * 3. File scope types, constants and variables
 * 4. Declaration of file local functions
 * 5. Definition of external functions
 * 6. Definition of internal functions
 *
 ************************************************************************/

#define SEC_FILE_NUMBER 4

/*************************************************************************
 *
 * 1. Includes
 *
 ************************************************************************/
#include "t_cops_extint_security.h"
#include "t_cops_extint_pmc.h"
#include "cops_extint.h"
#include "r_sec_flexstorage.h"
#include "r_hal_security.h"
#include "r_loader_sec_lib.h"
#include "r_os.h"
#include "r_cops.h"
#include "string.h"
#include "r_pd_esb.h"
#include "r_esbrom.h"
#include "r_copsrom.h"
#include "jmptable.h"
#include "r_debug_macro.h"
#include "t_hal_cryptoengine.h"
#include "r_bootparam.h"
#include "r_loader_sec_lib_port_funcs.h"  //xvslpi: only for test
#include "r_memory_utils.h"

/*************************************************************************
 *
 * 2. Definition of external constants and variables
 *
 ************************************************************************/
#define HEADER_HEADER_LENGTH_OFFSET    0x64
#define HEADER_PAYLOAD_SIZE_OFFSET     0x50

/*************************************************************************
 *
 * 3. File scope types, constants and variables
 *
 ************************************************************************/

/*************************************************************************
 *
 * 4. Declaration of file local functions
 *
 ************************************************************************/
static void COPS_Security_Data2Mac
(
    PD_ESB_MAC_Type_t                   *const PD_ESB_MacType_p,
    const COPS_ExtInt_Security_DataIntegrity_t       COPS_Security_DataIntegrity
);

/*************************************************************************
 *
 * 5. Definition of external functions
 *
 ************************************************************************/

/*************************************************************************
 *
 * 6. Definitions of internal functions
 *
 ************************************************************************/

/*
 * COPS_ExtInt_SHA256_Init
 *
 * Initializes a message digesting with the SHA-256 algorithm.
 *
 * @param [in]  Context_pp          Context pointer pointer *Context_pp has to be NULL
 *
 */
static COPS_RC_t COPS_ExtInt_SHA256_Init
(
    void                    **Context_pp
)
{
    COPS_RC_t ReturnValue = COPS_RC_OK;
    PD_ESB_ReturnValue_t PD_ESB_Return = PD_ESB_RETURN_VALUE_RANDOM_ERROR;
    boolean               Did_Malloc = FALSE;

    VERIFY(NULL != Context_pp, COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL == *Context_pp, COPS_RC_PARAMETER_ERROR);

    *Context_pp = malloc(sizeof(PD_ESB_MAC_SessionContext_t));
    ASSERT(NULL != *Context_pp);
    Did_Malloc = TRUE;

    PD_ESB_Return = R_Do_PD_ESB_MAC_GenerationInitialize(
                        (PD_ESB_MAC_SessionContext_t *) * Context_pp,
                        PD_ESB_MAC_TYPE_SHA256,
                        NULL,
                        0,
                        FALSE);

    VERIFY(PD_ESB_RETURN_VALUE_OK == PD_ESB_Return, COPS_RC_UNDEFINED_ERROR);

ErrorExit:

    if (ReturnValue != COPS_RC_OK && Context_pp != NULL && Did_Malloc) {
        BUFFER_FREE(*Context_pp);
    }

    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_SHA256_Init ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*
 * COPS_ExtInt_SHA256_Update
 *
 * Updates a message digesting with the SHA-256 algorithm.
 *
 * @param [in]  Context_pp          Context pointer pointer
 * @param [in]  Data_p              Data to calculate hash on.
 * @param [in]  Length              Size of data
 *
 */
static COPS_RC_t COPS_ExtInt_SHA256_Update
(
    void                    **Context_pp,
    const uint8              *const Data_p,
    const uint32                    Length
)
{
    COPS_RC_t ReturnValue = COPS_RC_OK;
    PD_ESB_ReturnValue_t PD_ESB_Return = PD_ESB_RETURN_VALUE_RANDOM_ERROR;

    VERIFY(NULL != Context_pp, COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != *Context_pp, COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != Data_p, COPS_RC_PARAMETER_ERROR);

    PD_ESB_Return = R_Do_PD_ESB_MAC_Update(
                        (PD_ESB_MAC_SessionContext_t *) * Context_pp,
                        0,
                        NULL,
                        Data_p,
                        Length);

    VERIFY(PD_ESB_RETURN_VALUE_OK == PD_ESB_Return, COPS_RC_UNDEFINED_ERROR);

ErrorExit:

    if (ReturnValue != COPS_RC_OK && Context_pp != NULL && *Context_pp != NULL) {
        BUFFER_FREE(*Context_pp);
    }

    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_SHA256_Update ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*
 * COPS_ExtInt_SHA256_Final
 *
 * Finalizes a message digesting with the SHA-256 algorithm.
 *
 * @param [in]  Context_pp          Context pointer pointer
 * @param [out] Calculated_Hash     Calculated hash
 *
 */
static COPS_RC_t COPS_ExtInt_SHA256_Final
(
    void                    **Context_pp,
    COPS_Storage_MAC_t *const Calculated_Hash_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_OK;
    PD_ESB_ReturnValue_t PD_ESB_Return = PD_ESB_RETURN_VALUE_RANDOM_ERROR;
    uint32 Length = sizeof(COPS_Storage_MAC_t);
    COPS_Storage_MAC_t    DummyHash;
    COPS_Storage_MAC_t   *Hash_p;

    VERIFY(NULL != Context_pp, COPS_RC_PARAMETER_ERROR);

    /*
     * Allow this function to be called to do cleanup even if there's nothing
     * to clean up.
     */
    if (*Context_pp == NULL) {
        goto ErrorExit;
    }

    if (Calculated_Hash_p == NULL) {
        Hash_p = &DummyHash;
    } else {
        Hash_p = Calculated_Hash_p;
    }

    PD_ESB_Return = R_Do_PD_ESB_MAC_GenerationFinalize(
                        (PD_ESB_MAC_SessionContext_t *) * Context_pp,
                        &Hash_p->Value[0],
                        &Length);
    VERIFY(PD_ESB_RETURN_VALUE_OK == PD_ESB_Return, COPS_RC_UNDEFINED_ERROR);

ErrorExit:
    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_SHA256_Final ... (%d).\n", __LINE__, ReturnValue);)

    if (Context_pp != NULL && NULL != *Context_pp) {
        free(*Context_pp);
        *Context_pp = NULL;
    }

    return ReturnValue;
}

/*
 * COPS_ExtInt_GetRNG
 *
 * TODO: Init random generator in COPS Init function!
 *
 * Generate a random number
 *
 * @param [out] Data_p     Random number
 * @param [in]  Length     Requested length
 *
 */
static COPS_RC_t COPS_ExtInt_GetRNG
(
    uint8              *const Data_p,
    const uint32                    Length
)
{
    COPS_RC_t ReturnValue = COPS_RC_OK;
    PD_ESB_ReturnValue_t PD_ESB_RetVal = PD_ESB_RETURN_VALUE_GENERAL_FAILURE;
    static PD_ESB_RandomSession_t RandomSession;
    static PD_ESB_RandomSeed_t Seed;
    static boolean IsInitializedOnce = FALSE;
    PD_ESB_Data_t Random = {0};
    uint32 GeneratedBytes = 0;

    VERIFY(NULL != Data_p, COPS_RC_PARAMETER_ERROR);

    Random.DataLength = PD_ESB_RANDOM_NUMBER_MAX_BYTE_SIZE;
    Random.Data_p = (uint8 *)malloc(Random.DataLength);
    ASSERT(NULL != Random.Data_p);

    if (FALSE == IsInitializedOnce) {
        PD_ESB_RetVal = R_Do_PD_ESB_RandomInitialize(&RandomSession,
                        PD_ESB_RANDOM_TYPE_EMP,
                        &Seed);
        VERIFY(PD_ESB_RetVal == PD_ESB_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);
        IsInitializedOnce = TRUE;
    }

    do {
        PD_ESB_RetVal = R_Do_PD_ESB_RandomGet(&RandomSession, &Random);
        VERIFY(PD_ESB_RetVal == PD_ESB_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);

        memcpy(Data_p + GeneratedBytes, Random.Data_p, MIN(Length, PD_ESB_RANDOM_NUMBER_MAX_BYTE_SIZE));
        GeneratedBytes += PD_ESB_RANDOM_NUMBER_MAX_BYTE_SIZE;
    } while (Length > GeneratedBytes);

ErrorExit:
    BUFFER_FREE(Random.Data_p);

    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_GetRNG ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}



static COPS_RC_t COPS_ExtInt_VerifyMAC
(
    const COPS_ExtInt_Security_DataIntegrity_t        MAC_Type,
    const COPS_Storage_MAC_t                   *const Reference_MAC_p,
    const uint8                                *const Data_p,
    uint32                                      Length,
    boolean                              *const VerifyOK_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_OK;
    PD_ESB_ReturnValue_t PD_ESB_Return = PD_ESB_RETURN_VALUE_GENERAL_FAILURE;
    PD_ESB_MAC_CompareResult_t PD_ESB_CompareResult = PD_ESB_MAC_COMPARE_RESULT_FAILED;
    PD_ESB_MAC_Type_t PD_ESB_MacType = PD_ESB_MAC_TYPE_UNDEFINED;

    VERIFY(NULL != Reference_MAC_p, COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != Data_p,          COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != VerifyOK_p,      COPS_RC_PARAMETER_ERROR);

    // Check that there is no possible reading over the buffer end
    VERIFY(sizeof(COPS_Storage_MAC_t) <= sizeof(PD_ESB_MAC_Value_t), COPS_RC_PARAMETER_ERROR);

    *VerifyOK_p = FALSE;

    COPS_Security_Data2Mac(&PD_ESB_MacType, MAC_Type);
    C_(printf("COPS_ExtInt_VerifyMAC %d: COPS try to verify MAC with PD_ESB_MacType = %d\n", __LINE__, (uint32)PD_ESB_MacType);)

    PD_ESB_Return = R_Do_PD_ESB_MAC_Compare(PD_ESB_MacType,
                                            NULL, // No key
                                            0,    // Keysize = 0
                                            Data_p,
                                            Length,
                                            (PD_ESB_MAC_Value_t *)Reference_MAC_p,
                                            &PD_ESB_CompareResult);
    VERIFY(PD_ESB_Return == PD_ESB_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);

    *VerifyOK_p = TRUE;

ErrorExit:
    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_VerifyMAC ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}


static COPS_RC_t COPS_ExtInt_CalculateMAC
(
    const COPS_ExtInt_Security_DataIntegrity_t        MAC_Type,
    const uint8                                *const Data_p,
    const uint32                                      Length,
    COPS_Storage_MAC_t                   *const Calculated_MAC_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_OK;
    PD_ESB_ReturnValue_t PD_ESB_Return  = PD_ESB_RETURN_VALUE_GENERAL_FAILURE;
    PD_ESB_MAC_Type_t PD_ESB_MacType = PD_ESB_MAC_TYPE_UNDEFINED;

    VERIFY(NULL != Calculated_MAC_p, COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != Data_p,           COPS_RC_PARAMETER_ERROR);

    // Check that buffer is big enough to take MAC
    VERIFY(sizeof(COPS_Storage_MAC_t) >= sizeof(PD_ESB_MAC_Value_t), COPS_RC_UNDEFINED_ERROR);

    COPS_Security_Data2Mac(&PD_ESB_MacType, MAC_Type);
    C_(printf("COPS_ExtInt_CalculateMAC %d: COPS try to calculate MAC with PD_ESB_MacType = %d\n", __LINE__, (uint32)PD_ESB_MacType);)

    PD_ESB_Return = R_Do_PD_ESB_MAC_Generate(PD_ESB_MacType,
                    NULL, // No key
                    0,    // Keysize = 0
                    Data_p,
                    Length,
                    (PD_ESB_MAC_Value_t *)Calculated_MAC_p);
    VERIFY(PD_ESB_Return == PD_ESB_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);

ErrorExit:
    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_CalculateMAC ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}


static COPS_RC_t COPS_ExtInt_VerifyFootprint
(
    const COPS_Storage_MAC_t *const HashOfSecret_p,
    const COPS_Storage_MAC_t *const Refference_Footprint_p,
    const uint8              *const Data_p,
    const uint32                    Length,
    boolean            *const VerifyOK_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;
    PD_ESB_ReturnValue_t PD_ESB_ReturnValue = PD_ESB_RETURN_VALUE_GENERAL_FAILURE;
    PD_ESB_MAC_Type_t PD_ESB_MacType = PD_ESB_MAC_TYPE_UNDEFINED;
    COPS_Storage_MAC_t CalculatedFootprint = {0};
    uint8 *Buffer_p = NULL;
    uint32 BufferLength = 0;

    /* Sanity check of input parameters.*/
    VERIFY(NULL != HashOfSecret_p,         COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != Refference_Footprint_p, COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != Data_p,                 COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != VerifyOK_p,             COPS_RC_PARAMETER_ERROR);

    *VerifyOK_p = FALSE;

    BufferLength = Length + sizeof(COPS_Storage_MAC_t);
    Buffer_p = (uint8 *)malloc(BufferLength);
    ASSERT(NULL != Buffer_p);

    memcpy(Buffer_p, HashOfSecret_p, sizeof(COPS_Storage_MAC_t));
    memcpy(Buffer_p + sizeof(COPS_Storage_MAC_t), Data_p, Length);

    COPS_Security_Data2Mac(&PD_ESB_MacType, COPS_EXTINT_SECURITY_DATA_INTEGRITY_MODEMLOCK);

    PD_ESB_ReturnValue = R_Do_PD_ESB_MAC_Generate(PD_ESB_MacType,
                         NULL, // No key
                         0,    // Keysize = 0
                         Buffer_p,
                         BufferLength,
                         (PD_ESB_MAC_Value_t *)&CalculatedFootprint);
    VERIFY(PD_ESB_ReturnValue == PD_ESB_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);

    if (0 == memcmp(&CalculatedFootprint, Refference_Footprint_p, sizeof(COPS_Storage_MAC_t))) {
        *VerifyOK_p = TRUE;
        ReturnValue = COPS_RC_OK;
    } else {
        B_(printf("COPS_ExtInt_VerifyFootprint %d: Verification of Footprint failed.\n", __LINE__);)
        *VerifyOK_p = FALSE;
        ReturnValue = COPS_RC_DATA_TAMPERED;
    }

ErrorExit:
    BUFFER_FREE(Buffer_p);

    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_VerifyFootprint ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}


static COPS_RC_t COPS_ExtInt_CalculateFootprint
(
    const COPS_Storage_MAC_t *const SecretKey_p,
    const uint8              *const Data_p,
    const uint32                    Length,
    COPS_Storage_MAC_t *const Calculated_Footprint_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;
    PD_ESB_ReturnValue_t PD_ESB_ReturnValue = PD_ESB_RETURN_VALUE_GENERAL_FAILURE;
    PD_ESB_MAC_Value_t HashOfSecret = {0};
    PD_ESB_MAC_Type_t PD_ESB_MacType = PD_ESB_MAC_TYPE_UNDEFINED;
    uint8 *Buffer_p = NULL;
    uint32 BufferLength = 0;

    VERIFY(NULL != SecretKey_p,            COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != Calculated_Footprint_p, COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != Data_p,                 COPS_RC_PARAMETER_ERROR);

    BufferLength = Length + sizeof(COPS_Storage_MAC_t);
    Buffer_p = (uint8 *)malloc(BufferLength);
    ASSERT(NULL != Buffer_p);

    memset(Buffer_p, 0x00, BufferLength);

    //Create hash from received secret
    PD_ESB_ReturnValue = R_Do_PD_ESB_MAC_Generate(PD_ESB_MAC_TYPE_SHA256,
                         NULL,
                         0,
                         SecretKey_p->Value,
                         COPS_STORAGE_MAC_LENGTH,
                         &HashOfSecret);
    VERIFY(PD_ESB_ReturnValue == PD_ESB_RETURN_VALUE_OK, ReturnValue);

    memcpy(Buffer_p, HashOfSecret.MAC, COPS_STORAGE_MAC_LENGTH);
    memcpy(Buffer_p + sizeof(COPS_Storage_MAC_t), Data_p, Length);

    COPS_Security_Data2Mac(&PD_ESB_MacType, COPS_EXTINT_SECURITY_DATA_INTEGRITY_MODEMLOCK);

    PD_ESB_ReturnValue = R_Do_PD_ESB_MAC_Generate(PD_ESB_MacType,
                         NULL, // No key
                         0,    // Keysize = 0
                         Buffer_p,
                         BufferLength,
                         (PD_ESB_MAC_Value_t *)Calculated_Footprint_p);
    VERIFY(PD_ESB_ReturnValue == PD_ESB_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);

    ReturnValue = COPS_RC_OK;
ErrorExit:
    BUFFER_FREE(Buffer_p);

    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_CalculateFootprint ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}


static ESBROM_Return_t   Wrap_ESBROM_MAC_Cmp(
    const ESBROM_MacType_t            MacType,
    const ESBROM_KeyUnknownSize_t        *const MacKey_p,
    const ESBROM_KeySize_t            MacKeySize,
    const uint8                  *Data_p,
    const uint32                  DataLength,
    const ESBROM_Mac_t             *const Mac_p,
    ESBROM_CompareResult_t           *const CompareResult_p)
{
    return R_Do_ESBROM_MAC_Cmp(MacType, MacKey_p, MacKeySize, Data_p, DataLength, Mac_p, CompareResult_p);
}

static ESBROM_Return_t   Wrap_ESBROM_MAC_Gen(
    const ESBROM_MacType_t                          MacType,
    const ESBROM_KeyUnknownSize_t                  *MacKey_p,
    const ESBROM_KeySize_t                          MacKeySize,
    const uint8                                    *Data_p,
    const uint32                                    DataLength,
    ESBROM_Mac_t                       *const Mac_p)
{
    return  R_Do_ESBROM_MAC_Gen(MacType, MacKey_p, MacKeySize, Data_p, DataLength, Mac_p);
}



//TODO: The HAL_Security interface will change to return CertificatePurpose and select verification type (loader SW ...)
static COPS_RC_t COPS_ExtInt_VerifySignature
(
    const uint8    *const SignedObject_p,
    const uint32          ObjectLength,
    uint32   *const PayloadOffset_p,
    uint32   *const CertificatePurpose_p,
    boolean  *const VerifyOK_p
)
{
    COPSROM_Return_t RetVal;
    uint8 *PKI_Pool_p = NULL;
    COPSROM_Error_Code_t *COPS_Error_p = NULL;
    COPSROM_Security_Settings_t *SecuritySettings_p;
    uint32 *SecData_p = NULL;
    uint32 HeaderLength = 0;
    SHA256_Hash_t HashListHash;
    SHA256_Hash_t PayloadHash;
    uint32 BP_Size = sizeof(COPSROM_Security_Settings_t);
    COPS_RC_t ReturnValue = COPS_RC_OK;

    VERIFY(NULL != SignedObject_p,       COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != PayloadOffset_p,      COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != CertificatePurpose_p, COPS_RC_PARAMETER_ERROR);
    VERIFY(NULL != VerifyOK_p,           COPS_RC_PARAMETER_ERROR);

    // Set default values
    *PayloadOffset_p = 0;
    *CertificatePurpose_p = 0x0f; //The authentication purpose should match as the authentication purpose in the certificate.
    *VerifyOK_p = FALSE;

    {
        // Allocate some memory work buffers
        PKI_Pool_p = (uint8 *)malloc(COPSROM_PKI_MEMORY_SIZE);
        ASSERT(PKI_Pool_p != NULL);

        COPS_Error_p = (COPSROM_Error_Code_t *)malloc(sizeof(COPSROM_Error_Code_t));
        ASSERT(COPS_Error_p != NULL);
        memset(COPS_Error_p, 0x00, sizeof(COPSROM_Error_Code_t));

        memcpy(&(HeaderLength), (void *)&SignedObject_p[HEADER_HEADER_LENGTH_OFFSET] , sizeof(uint32));
        SecData_p = (uint32 *)malloc(sizeof(COPSROM_Security_Settings_t));

        C_(printf("HeaderLength =%x\n", HeaderLength);)
        ReturnValue = (COPS_RC_t)Do_BOOTPARAM_ReadIndexed(BOOTPARAM_IDENTIFIER_SECURITY_SETTINGS, 0, SecData_p, &BP_Size);
        VERIFY(COPS_RC_OK == ReturnValue, COPS_RC_PARAMETER_ERROR);

        SecuritySettings_p = (COPSROM_Security_Settings_t *)SecData_p;
        C_(printf("- SecuritySettings -\n");)
        C_(
            printf("Domain=%d \n", SecuritySettings_p->SecuredSettings.Domain);
            printf("DomainDataPresent=%d \n", SecuritySettings_p->SecuredSettings.DomainDataPresent);
            printf("DomainDataValid=%d \n", SecuritySettings_p->SecuredSettings.DomainDataValid);
            printf("OTP.MainSecurity=%d \n", SecuritySettings_p->SecuredSettings.OTPValues.MainSecurity);
            printf("OTP.CID=%d \n", SecuritySettings_p->SecuredSettings.OTPValues.CID);
            printf("OTP.PAF=%d \n", SecuritySettings_p->SecuredSettings.OTPValues.PAF);
            printf("OTP.IMEI_Changeable=%d \n", SecuritySettings_p->SecuredSettings.OTPValues.IMEI_Changeable);
        )
        RetVal = R_Do_COPSROM_VerifyHeaderAll(COPS_Error_p,
                                              SecuritySettings_p,
                                              PKI_Pool_p,
                                              (uint8 *)SignedObject_p,
                                              HeaderLength,
                                              COPSROM_AUTHENTICATION,
                                              CertificatePurpose_p,
                                              PayloadHash,
                                              HashListHash,
                                              Wrap_ESBROM_MAC_Cmp,
                                              Wrap_ESBROM_MAC_Gen);
        B_(printf("cops_loader_extint.c (%d): R_Do_COPSROM_VerifyHeaderAll ... (%d).\n", __LINE__, RetVal);)
        B_(

        for (int i = 0; i < COPS_Error_p->pos; i++) {
        printf("M:%d Err:%d \n", COPS_Error_p->error[i].module, COPS_Error_p->error[i].value);
        }
        )
        C_(printf("AuthPurpose = 0x%x\n", *CertificatePurpose_p);)
        VERIFY(((RetVal == COPSROM_SUCCESS) || (RetVal == COPSROM_SUCCESS_HEADER_UPDATED)), COPS_RC_AUTHENTICATION_FAILED);
        *VerifyOK_p = TRUE;

        /*
        * The header length must be aligned to 8 bytes..
        * So if the header is unaligned, we need to add until we have even.
        */
        if (0 != (HeaderLength & MASK_FIRST_3_BITS)) { // align to 64 bit
            HeaderLength = (HeaderLength & CLEAR_FIRST_3_BITS) + sizeof(uint64);
        }

        *PayloadOffset_p = HeaderLength;
    }

ErrorExit:

    BUFFER_FREE(SecData_p);
    BUFFER_FREE(COPS_Error_p);
    BUFFER_FREE(PKI_Pool_p);

    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_VerifySignature ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}



static COPS_RC_t COPS_ExtInt_ReadOTP
(
    COPS_OTP_t *const OTP_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_OK;
    LoaderSecLib_Output_t Result = LOADER_SEC_LIB_FAILURE;

    LOADER_SEC_LIB_OTP_t Temp_OTP_Data = {0};

    VERIFY(NULL != OTP_p, COPS_RC_PARAMETER_ERROR);

    Result = Do_LoaderSecLib_Read_OTP(&Temp_OTP_Data);
    VERIFY(Result == LOADER_SEC_LIB_SUCCESS, COPS_RC_UNDEFINED_ERROR);

    OTP_p->PAF        = Temp_OTP_Data.PAF;
    OTP_p->PAF_Locked = (Temp_OTP_Data.OTP_Lock_Status) & OTP_PACKED_PAF_LOCK_MASK;

    OTP_p->CID        = Temp_OTP_Data.CID;
    OTP_p->CID_Locked = (Temp_OTP_Data.OTP_Lock_Status) & OTP_PACKED_CID_LOCK_MASK;

    OTP_p->IMEI.Length = 8;
    OTP_p->IMEI.TypeOfIdentity = 2;
    OTP_p->IMEI.OddEvenIndication = 1;
    memcpy(OTP_p->IMEI.Digits, (uint8 *)Temp_OTP_Data.IMEI, COPS_UNPACKED_IMEI_LENGTH);
    OTP_p->IMEI_Locked = (Temp_OTP_Data.OTP_Lock_Status) & OTP_PACKED_IMEI_LOCK_MASK;

    OTP_p->RewritableIMEI        = Temp_OTP_Data.RewritableIMEI;
    OTP_p->RewritableIMEI_Locked = (Temp_OTP_Data.OTP_Lock_Status) & OTP_PACKED_IMEI_CHANGEABLE_FLAG_LOCK_MASK;

ErrorExit:
    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_ReadOTP ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}


static COPS_RC_t COPS_ExtInt_ReadDomain
(
    HAL_Security_Domain_t      *const Domain_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;
    LoaderSecLib_Output_t Result = LOADER_SEC_LIB_FAILURE;
    uint32 EffectiveDomain = 0;
    uint32 WrittenDomain = 0;

    VERIFY(NULL != Domain_p, COPS_RC_PARAMETER_ERROR);

    Result = Do_LoaderSecLib_ReadDomain(&EffectiveDomain, &WrittenDomain);
    VERIFY(LOADER_SEC_LIB_SUCCESS == Result, COPS_RC_UNDEFINED_ERROR);

    switch (EffectiveDomain) {
    case LOADER_SEC_LIB_DOMAIN_FACTORY:
        *Domain_p = HAL_SECURITY_DOMAIN_FACTORY;
        break;

    case LOADER_SEC_LIB_DOMAIN_RnD:
        *Domain_p = HAL_SECURITY_DOMAIN_R_AND_D;
        break;

    case LOADER_SEC_LIB_DOMAIN_PRODUCT:
        *Domain_p = HAL_SECURITY_DOMAIN_PRODUCT;
        break;

    case LOADER_SEC_LIB_DOMAIN_SERVICE:
        *Domain_p = HAL_SECURITY_DOMAIN_SERVICE;
        break;

    default:
        *Domain_p = HAL_SECURITY_DOMAIN_UNDEFINED;
        ReturnValue = COPS_RC_UNDEFINED_ERROR;
        goto ErrorExit;
    }

    ReturnValue = COPS_RC_OK;

ErrorExit:
    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_ReadDomain ... (%d).\n", __LINE__, ReturnValue);)
    IDENTIFIER_NOT_USED(WrittenDomain);
    return ReturnValue;
}


static COPS_RC_t COPS_ExtInt_WriteDomain
(
    HAL_Security_Domain_t   Domain
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;
    LoaderSecLib_Output_t Result = LOADER_SEC_LIB_FAILURE;
    uint32 NewDomain = 0;

    switch (Domain) {
    case HAL_SECURITY_DOMAIN_FACTORY:
        NewDomain = LOADER_SEC_LIB_DOMAIN_FACTORY;
        break;

    case HAL_SECURITY_DOMAIN_R_AND_D:
        NewDomain = LOADER_SEC_LIB_DOMAIN_RnD;
        break;

    case HAL_SECURITY_DOMAIN_PRODUCT:
        NewDomain = LOADER_SEC_LIB_DOMAIN_PRODUCT;
        break;

    case HAL_SECURITY_DOMAIN_SERVICE:
        NewDomain = LOADER_SEC_LIB_DOMAIN_SERVICE;
        break;

    default:
        ReturnValue = COPS_RC_UNDEFINED_ERROR;
        goto ErrorExit;
    }

    Result = Do_LoaderSecLib_WriteDomain(NewDomain);
    VERIFY(LOADER_SEC_LIB_SUCCESS == Result, COPS_RC_UNDEFINED_ERROR);

    ReturnValue = COPS_RC_OK;

ErrorExit:
    B_(printf("cops_loader_extint.c (%d): COPS_ExtInt_WriteDomain ... (%d).\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

static void COPS_Security_Data2Mac
(
    PD_ESB_MAC_Type_t             *const PD_ESB_MacType_p,
    const COPS_ExtInt_Security_DataIntegrity_t       COPS_Security_DataIntegrity
)
{
    // Check input parameters, not needed in internal function since check done earlier.

    switch (COPS_Security_DataIntegrity) {
    case COPS_EXTINT_SECURITY_DATA_INTEGRITY_DOMAIN:
        *PD_ESB_MacType_p = PD_ESB_MAC_TYPE_INTERNAL_BLOCKING_00;
        break;

    case COPS_EXTINT_SECURITY_DATA_INTEGRITY_CONTROL_KEY_PROTECTION:
        *PD_ESB_MacType_p = PD_ESB_MAC_TYPE_INTERNAL_BLOCKING_01;
        break;

    case COPS_EXTINT_SECURITY_DATA_INTEGRITY_SIMLOCK_SETTINGS:
        *PD_ESB_MacType_p = PD_ESB_MAC_TYPE_INTERNAL_NONBLOCKING_07;
        break;

    case COPS_EXTINT_SECURITY_DATA_INTEGRITY_SIMLOCK_CONTROL:
        *PD_ESB_MacType_p = PD_ESB_MAC_TYPE_INTERNAL_NONBLOCKING_08;
        break;

    case COPS_EXTINT_SECURITY_DATA_INTEGRITY_MODEMLOCK:
        *PD_ESB_MacType_p = PD_ESB_MAC_TYPE_INTERNAL_NONBLOCKING_09;
        break;

    case COPS_EXTINT_SECURITY_DATA_INTEGRITY_SECURE_SETTINGS:
        *PD_ESB_MacType_p = PD_ESB_MAC_TYPE_INTERNAL_NONBLOCKING_05;
        break;

    default:
        *PD_ESB_MacType_p = PD_ESB_MAC_TYPE_UNDEFINED;
        break;
    }
}

// Definition of Function Pointers
const COPS_ExtInt_Security_t COPS_ExtInt_Security = {
    COPS_ExtInt_SHA256_Init,
    COPS_ExtInt_SHA256_Update,
    COPS_ExtInt_SHA256_Final,
    COPS_ExtInt_GetRNG,
    COPS_ExtInt_VerifyMAC,
    COPS_ExtInt_CalculateMAC,
    COPS_ExtInt_VerifyFootprint,
    COPS_ExtInt_CalculateFootprint,
    COPS_ExtInt_VerifySignature,
    COPS_ExtInt_ReadOTP,
    NULL,
    COPS_ExtInt_ReadDomain,
    COPS_ExtInt_WriteDomain
};


/* * * * * * * * * * * * * * * * *
 *                               *
 *    PMC External Functions     *
 *                               *
 * * * * * * * * * * * * * * * * */

/**
 * COPS_ExtInt_PMC_Get
 *
 * TODO: Implement this function!
 *
 * Returns the current value of a Protected Monotonic Counter
 *
 * @param [IN]  PMC_Type    Indentifying the PMC to be used
 * @param [OUT] PMC_Value   The Current PMC value
 *
 */
static COPS_RC_t COPS_ExtInt_PMC_Get
(
    COPS_ExtInt_PMC_Type_t        PMC_Type,
    uint32                 *const PMC_Value_p
)
{
    COPS_RC_t ReturnCode = COPS_RC_OK;

    *PMC_Value_p = 0;

    return ReturnCode;
}


/**
 * COPS_ExtInt_PMC_Increment
 *
 * TODO: Implement this function!
 *
 * Increments the value of a Protected Monotonic Counter
 *
 * @param [IN]  PMC_Type    Indentifying the PMC to be used
 * @param [OUT] PMC_Value   The PMC value after increment
 *
 */
static COPS_RC_t COPS_ExtInt_PMC_Increment
(
    COPS_ExtInt_PMC_Type_t        PMC_Type,
    uint32                 *const PMC_Value_p
)
{
    COPS_RC_t ReturnCode = COPS_RC_OK;

    return ReturnCode;
}

// Definition of Function Pointers
const COPS_ExtInt_PMC_t COPS_ExtInt_PMC = {
    COPS_ExtInt_PMC_Get,
    COPS_ExtInt_PMC_Increment
};

PD_ESB_ReturnValue_t R_Do_PD_ESB_RegisterRead
(
    const PD_ESB_ProtectedRegister_t       Register,
    uint8                     *const Data_p
)
{
    return HAL_CRYPTO_ENGINE_RETURN_VALUE_OK;
}


/*******************************************************************************
 *
 * 2. Definition of external constants and variables
 *
 ******************************************************************************/
#define COPS_EXTINT_FLEXSTORAGE_MAX_PATH_LENGTH 80

/*******************************************************************************
 *
 * 3. File scope types, constants and variables
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * 4. Declaration of file local functions
 *
 ******************************************************************************/
static COPS_RC_t COPS_ExtInt_FlexStorage_ReadData_Helper
(
    COPS_ExtInt_FlexStorage_Object_t        StorageObject,
    uint32                                  Offset,
    uint8                            *const Buffer_p,
    uint32                           *const BufferLen_p
);

static COPS_RC_t COPS_ExtInt_FlexStorage_GetFileName_FileSystem
(
    COPS_ExtInt_FlexStorage_Object_t        StorageObject,
    wchar_t                          *const FileName_p
);

/*******************************************************************************
 *
 * 5. Definition of external functions
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * 6. Definitions of internal functions
 *
 ******************************************************************************/

static COPS_RC_t COPS_ExtInt_FlexStorage_ReadData
(
    COPS_ExtInt_FlexStorage_Object_t        StorageObject,
    uint8                            *const Buffer_p,
    uint32                           *const BufferLen_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = COPS_ExtInt_FlexStorage_ReadData_Helper(StorageObject, 0, Buffer_p, BufferLen_p);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}


static COPS_RC_t COPS_ExtInt_FlexStorage_ReadPartialData
(
    COPS_ExtInt_FlexStorage_Object_t        StorageObject,
    uint32                                  Offset,
    uint8                            *const Buffer_p,
    uint32                                  BytesToRead
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;

    ReturnValue = COPS_ExtInt_FlexStorage_ReadData_Helper(StorageObject, Offset, Buffer_p, &BytesToRead);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

ErrorExit:

    return ReturnValue;
}

static COPS_RC_t COPS_ExtInt_FlexStorage_WriteData
(
    COPS_ExtInt_FlexStorage_Object_t        StorageObject,
    const uint8                            *const Buffer_p,
    uint32                                  BufferLen
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;
    Sec_FlexStorage_ReturnValue_t FlexStorage_ReturnValue;
    Sec_FlexStorage_File_t *Handle_p = NULL;
    wchar_t FileName[COPS_EXTINT_FLEXSTORAGE_MAX_PATH_LENGTH];
    uint32 BytesWritten = 0;

    VERIFY(NULL != Buffer_p, COPS_RC_PARAMETER_ERROR);

    ReturnValue = COPS_ExtInt_FlexStorage_GetFileName_FileSystem(StorageObject, FileName);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

    FlexStorage_ReturnValue = Sec_FlexStorage_FS_Open(&Handle_p, FileName, TRUE);
    VERIFY(FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_OK,  COPS_RC_COPS_DATA_NOT_PRESENT);

    // write everything in one chunk
    // TODO: may change to handle not complete reading
    FlexStorage_ReturnValue = Sec_FlexStorage_FS_Write(Handle_p, Buffer_p, BufferLen, &BytesWritten);
    VERIFY(FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_OK,  COPS_RC_COPS_DATA_NOT_PRESENT);
    VERIFY(BytesWritten == BufferLen,  COPS_RC_COPS_DATA_NOT_PRESENT);

    FlexStorage_ReturnValue = Sec_FlexStorage_FS_Close(&Handle_p);
    VERIFY(FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);

    ReturnValue = COPS_RC_OK;
ErrorExit:

    return ReturnValue;
}

static COPS_RC_t COPS_ExtInt_FlexStorage_EraseData
(
    COPS_ExtInt_FlexStorage_Object_t StorageObject
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;
    Sec_FlexStorage_ReturnValue_t FlexStorage_ReturnValue = SEC_FLEX_STORAGE_RETURN_VALUE_OK;
    wchar_t FileName[COPS_EXTINT_FLEXSTORAGE_MAX_PATH_LENGTH];

    ReturnValue = COPS_ExtInt_FlexStorage_GetFileName_FileSystem(StorageObject, FileName);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

    FlexStorage_ReturnValue = Sec_FlexStorage_FS_Delete(FileName);
    VERIFY(FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_OK,  COPS_RC_COPS_DATA_NOT_PRESENT);

    ReturnValue = COPS_RC_OK;
ErrorExit:
    return ReturnValue;
}



static COPS_RC_t COPS_ExtInt_FlexStorage_ReadData_Helper
(
    COPS_ExtInt_FlexStorage_Object_t        StorageObject,
    uint32                                  Offset,
    uint8                            *const Buffer_p,
    uint32                           *const BufferLen_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;
    Sec_FlexStorage_ReturnValue_t FlexStorage_ReturnValue = SEC_FLEX_STORAGE_RETURN_VALUE_OK;
    Sec_FlexStorage_File_t *Handle_p = NULL;
    wchar_t FileName[COPS_EXTINT_FLEXSTORAGE_MAX_PATH_LENGTH] = {0};
    uint32 FileSize = 0;
    uint32 BytesRead = 0;

    VERIFY(NULL != BufferLen_p, COPS_RC_PARAMETER_ERROR);

    ReturnValue = COPS_ExtInt_FlexStorage_GetFileName_FileSystem(StorageObject, FileName);
    VERIFY(COPS_RC_OK == ReturnValue, ReturnValue);

    FlexStorage_ReturnValue = Sec_FlexStorage_FS_Open(&Handle_p, FileName, FALSE);

    if (FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_FILE_NOT_FOUND) {
        B_(printf("cops_loader_extint.c (%d): File does not exitst. File %ls.\n", __LINE__, FileName);)
        ReturnValue =  COPS_RC_COPS_DATA_NOT_PRESENT;
        goto ErrorExit;
    } else if (FlexStorage_ReturnValue != SEC_FLEX_STORAGE_RETURN_VALUE_OK) {
        B_(printf("cops_loader_extint.c (%d): Error opening file. File %ls.\n", __LINE__, FileName);)
        ReturnValue = COPS_RC_UNDEFINED_ERROR;
        goto ErrorExit;
    }


    FlexStorage_ReturnValue = Sec_FlexStorage_FS_GetSize(Handle_p, &FileSize);
    VERIFY(FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);
    VERIFY(((Offset + *BufferLen_p) <= FileSize), COPS_RC_UNDEFINED_ERROR);

    //COPS module use this function to get file size
    if ((NULL == Buffer_p) && (0 == Offset)) {
        B_(printf("cops_loader_extint.c (%d): File size is %d.\n", __LINE__, FileSize);)
        *BufferLen_p = FileSize;
        return COPS_RC_OK;
    }

    FlexStorage_ReturnValue =  Sec_FlexStorage_FS_Seek(Handle_p, Offset, SEC_FLEX_STORAGE_SEEK_SET);
    VERIFY(FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);

    // read everything in one chunk
    // TODO: may change to handle not complete reading
    FlexStorage_ReturnValue = Sec_FlexStorage_FS_Read(Handle_p, Buffer_p, *BufferLen_p, &BytesRead);
    VERIFY(FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);
    VERIFY((BytesRead == *BufferLen_p), COPS_RC_UNDEFINED_ERROR);

    FlexStorage_ReturnValue = Sec_FlexStorage_FS_Close(&Handle_p);
    VERIFY(FlexStorage_ReturnValue == SEC_FLEX_STORAGE_RETURN_VALUE_OK, COPS_RC_UNDEFINED_ERROR);

    ReturnValue = COPS_RC_OK;
ErrorExit:

    return ReturnValue;
}


static COPS_RC_t COPS_ExtInt_FlexStorage_GetFileName_FileSystem
(
    COPS_ExtInt_FlexStorage_Object_t        StorageObject,
    wchar_t                          *const FileName_p
)
{
    COPS_RC_t ReturnValue = COPS_RC_UNDEFINED_ERROR;

    VERIFY(NULL != FileName_p, COPS_RC_PARAMETER_ERROR);

    memset(FileName_p, 0, sizeof(wchar_t) * COPS_EXTINT_FLEXSTORAGE_MAX_PATH_LENGTH);

    (void)swprintf(FileName_p, COPS_EXTINT_FLEXSTORAGE_MAX_PATH_LENGTH, L"COPS_DATA_%d.csd", StorageObject);

    ReturnValue = COPS_RC_OK;
ErrorExit:
    return ReturnValue;
}


// Definition of Function Pointers
const COPS_ExtInt_FlexStorage_t COPS_ExtInt_FlexStorage = {
    COPS_ExtInt_FlexStorage_ReadData,
    COPS_ExtInt_FlexStorage_ReadPartialData,
    COPS_ExtInt_FlexStorage_WriteData,
    COPS_ExtInt_FlexStorage_EraseData
};


/*************************************************************************
 * 2.4 SIM
 ************************************************************************/

/*Get SIMData from Sim-Card */
COPS_RC_t COPS_ExtInt_SIM_GetData
(
    COPS_SimData_t *const SimData_p
)
{
    return COPS_RC_OK;
}

/* Get SIMCNLData from Sim-Card */
COPS_RC_t COPS_ExtInt_SIM_GetCNLData
(
    COPS_SimCNLData_t *const SimCNLData_p
)
{
    return COPS_RC_OK;
}

/* Verify PIN used during reset of modem lock */
COPS_RC_t COPS_ExtInt_SIM_VerifyPin
(
    const uint8                *const Pin_p,
    boolean              *const VerifyOK_p
)
{
    return COPS_RC_OK;
}

/* Power on SIM */
COPS_RC_t COPS_ExtInt_SIM_PowerOn
(
    void
)
{
    return COPS_RC_OK;
}

/* Power off SIM */
COPS_RC_t COPS_ExtInt_SIM_PowerOff
(
    void
)
{
    return COPS_RC_OK;
}

/* Retrieving SIM-Status */
COPS_RC_t COPS_ExtInt_SIM_GetStatus
(
    boolean *const SimStateActive_p
)
{
    return COPS_RC_OK;
}

/* Subscribe  to sim state changed event */
COPS_RC_t COPS_ExtInt_SIM_SubscribeEvent(void)
{
    return COPS_RC_OK;
}

/* Subscribe  to sim state changed event */
COPS_RC_t COPS_ExtInt_SIM_UnSubscribeEvent(void)
{
    return COPS_RC_OK;
}


/*
 * Subscribe  to sim state changed event
 */
COPS_RC_t COPS_ExtInt_SIM_HandleSimEvent
(
    void          *SigStruct_p,
    boolean *const SimStateActive_p
)
{
    return COPS_RC_OK;
}

/*
  * Converts a IMSI formatted according to 3GPP TS 31.102 to
  * the internal used COPS format.
  *
  *
  * @param [in]  SIM_IMSI_p    IMSI according to 3GPP TS 31.102
  * @param [out] COPS_IMSI_p   IMSI in internal COPS format.
  */
COPS_RC_t COPS_ExtInt_SIM_Convert_IMSI
(
    const uint8            *const SIM_IMSI_p,
    COPS_IMSI_t      *const COPS_IMSI_p
)
{
    return COPS_RC_OK;
}


COPS_RC_t COPS_ExtInt_SIM_Convert_CNL
(
    const COPS_SIM_CNL_t     *const COPS_SIM_CNL_p,
    COPS_SimCNLData_t  *const COPS_SimCNLData_p
)
{
    return COPS_RC_OK;
}



// Definition of Function Pointers
const COPS_ExtInt_SIM_t COPS_ExtInt_Sim = {
    COPS_ExtInt_SIM_GetData,
    COPS_ExtInt_SIM_GetCNLData,
    COPS_ExtInt_SIM_VerifyPin,
    COPS_ExtInt_SIM_PowerOn,
    COPS_ExtInt_SIM_PowerOff,
    COPS_ExtInt_SIM_GetStatus,
    COPS_ExtInt_SIM_SubscribeEvent,
    COPS_ExtInt_SIM_UnSubscribeEvent,
    COPS_ExtInt_SIM_HandleSimEvent,
    COPS_ExtInt_SIM_Convert_IMSI,
    COPS_ExtInt_SIM_Convert_CNL

};

/*************************************************************************
 * 2.6 Timer
 ************************************************************************/

/*
 * COPS_ExtInt_Timer_Set
 *
 * Registers a timer on the timer server. When this timer timeouts (i.e. when
 * TimerValue * 1 ms has passed), a signal will be sent back to the calling
 * process.  If TimerValue is set to 0, a timeout
 * signal will be sent back instantly. The signal sent when the timer expires
 * also contains the client tag.
 *
 * @param [in] ClientTag         Client tag
 * @param [in] TimerValue        The timer value in ms (Max value: 1000 000 000)
 *
 *
 */
COPS_RC_t COPS_ExtInt_Timer_Set
(
    const uint32    ClientTag,
    uint32    TimerValue
)
{
    return COPS_RC_OK;
}

/*
 * COPS_ExtInt_Timer_Get
 *
 * Used to find out how much time the timer has left before timing out. The
 * result (in ms) is stored in the buffer provided by the caller. If
 * TimerValue_p is set to NULL, this function will just return OK if the timer
 * was found, and NOK otherwise (nothing will be written to *TimerValue_p).
 *
 * @param [in] ClientTag        Client tag
 * @param [out] TimerValue_p    Pointer to buffer where result will be stored
 *
 */
COPS_RC_t COPS_ExtInt_Timer_Get
(
    const uint32          ClientTag,
    uint32   *const TimerValue_p
)
{
    return COPS_RC_OK;
}

/*
 * COPS_ExtInt_Timer_Reset
 *
 * Used to to stop timer
 *
 * @param [in] ClientTag        Client tag
 *
 */
COPS_RC_t COPS_ExtInt_Timer_Reset
(
    const uint32          ClientTag
)
{
    return COPS_RC_OK;
}


const COPS_ExtInt_Timer_t COPS_ExtInt_Timer = {
    COPS_ExtInt_Timer_Set,
    COPS_ExtInt_Timer_Get,
    COPS_ExtInt_Timer_Reset
};



/*************************************************************************
 * 2.5 System control
 ************************************************************************/

/* Subscribe to sim state changed event */
COPS_RC_t COPS_ExtInt_SysCrtl_SubscribeEvent(void)
{
    return COPS_RC_OK;
}

/* Subscribe to sim state changed event */
COPS_RC_t COPS_ExtInt_SysCrtl_UnSubscribeEvent(void)
{
    return COPS_RC_OK;
}

/* Handle and decode event */
COPS_RC_t COPS_ExtInt_SysCrtl_HandleSimEvent
(
    void          *SigStruct_p,
    boolean *const ShuttingDown_p
)
{
    return COPS_RC_OK;
}

const COPS_ExtInt_SysCrtl_t COPS_ExtInt_SysCtrl = {
    COPS_ExtInt_SysCrtl_SubscribeEvent,
    COPS_ExtInt_SysCrtl_UnSubscribeEvent,
    COPS_ExtInt_SysCrtl_HandleSimEvent
};


/* Definition of Function Pointers */
const COPS_ExtInt_t COPS_ExtInt = {
    &COPS_ExtInt_Security,
    &COPS_ExtInt_FlexStorage,
    &COPS_ExtInt_Sim,
    &COPS_ExtInt_Timer,
    &COPS_ExtInt_PMC,
    &COPS_ExtInt_SysCtrl,
};


/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 *  @addtogroup ldr_security_library
 *  @{
 *      @addtogroup command_protocol
 *      @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "e_loader_sec_lib.h"
#include "r_loader_sec_lib.h"
#include "r_loader_sec_lib_port_funcs.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "loader_sec_lib_internals.h"
#include "r_bootparam.h"
#include "r_esbrom.h"
#include "r_bootrom.h"
#include "cops_data_manager.h"
#include "jmptable.h"

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * Domain data block Initialization.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success initialization.
 * @retval .
 */
LoaderSecLib_Output_t LoaderSecLib_InitDomainData(void)
{
    LoaderSecLib_Output_t     ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    DomainsData_t *Data_p;
    COPSROM_Security_Settings_t *SecuritySettings_p;

    ReturnValue = LoaderSecLib_CreateDataBlock((void **)&Data_p, sizeof(DomainsData_t));
    VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), ReturnValue);

    ReturnValue = Do_LoaderSecLib_GetSecuritySettings(&SecuritySettings_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    Data_p->EffectiveDomain = SecuritySettings_p->SecuredSettings.Domain;
    Data_p->WrittenDomain = Data_p->EffectiveDomain;

    ReturnValue = LoaderSecLib_AddInternalDataBlock(INTERNAL_DATA_BLOCK_DOMAINS, (void *)Data_p, sizeof(DomainsData_t));
    VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), ReturnValue);

ErrorExit:
    return ReturnValue;
}

/*
 * Read domains "Effective" and "Written".
 *
 * Efective domain is set by ROMcode, and Written domain is set by user. If user
 * did not write new domain, Written domain will be same as Effective domain.
 *
 * @param [out]  EffectiveDomain_p  Effective domain.
 * @param [out]  WrittenDomain_p    Written domain.
 *
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC In case of invalid input
 *                                                                                                  parameters.
 * @retval LOADER_SEC_LIB_SUCCESS                   In case of successful read.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_ReadDomain(uint32 *EffectiveDomain_p, uint32 *WrittenDomain_p)
{
    LoaderSecLib_Output_t     ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    DomainsData_t         *Domain_Data_p = NULL;

    VERIFY((EffectiveDomain_p != NULL) && (WrittenDomain_p != NULL), ReturnValue);

    ReturnValue = LoaderSecLib_GetInternalDataBlock(INTERNAL_DATA_BLOCK_DOMAINS, (void **)&Domain_Data_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    *EffectiveDomain_p = Domain_Data_p->EffectiveDomain;
    *WrittenDomain_p = Domain_Data_p->WrittenDomain;

ErrorExit:
    return ReturnValue;
}


/*
 * Write "Written" domain.
 *
 * Written domain is from the BootRecord in flash.
 *
 * @param [out]  Domain  Written domain from flash.
 *
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC In case of invalid input parameters.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                   In case of successful write.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_WriteDomain(uint32 Domain)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    DomainsData_t        *Domain_Data_p = NULL;

    VERIFY(Domain != 0, ReturnValue);

    ReturnValue = LoaderSecLib_GetInternalDataBlock(INTERNAL_DATA_BLOCK_DOMAINS, (void **)&Domain_Data_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    if (Domain_Data_p->WrittenDomain != Domain) {
        Domain_Data_p->WrittenDomain = Domain;
        ReturnValue = LoaderSecLib_UpdateInternalDataBlock(INTERNAL_DATA_BLOCK_DOMAINS);
        VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);
    } else {
        C_(printf(" Domain that is set is same as previous one.\n");)
    }

ErrorExit:
    return ReturnValue;
}

/*
 * Verify the domain data.
 *
 * @param [in]  DomainData_p  Domain data which should be verified.
 *
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC In case of invalid input
 *                                                  parameters.
 * @retval LOADER_SEC_LIB_SUCCESS                   In case of successful write.
 * @retval LOADER_SEC_LIB_FAILURE                   In case when verification
 *                                                  fail.
 * @retval LOADER_SEC_LIB_ESB_MAC_NOT_VERIFIED      In case when domain data is
 *                                                  not correct.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_VerifyDomainData(Domain_Data_t *DomainData_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;

    VERIFY(NULL != DomainData_p, ReturnValue);

    ReturnValue = LoaderSecLib_Verify_MAC(PD_ESB_MAC_TYPE_INTERNAL_BLOCKING_00,
                                          (uint8 *)&DomainData_p->Length,
                                          sizeof(Domain_Data_t) - PD_ESB_MAX_MAC_BYTE_SIZE,
                                          DomainData_p->MAC);
    VERIFY(ReturnValue == LOADER_SEC_LIB_SUCCESS, ReturnValue);

ErrorExit:
    return ReturnValue;
}

/*
 * Calculate domain data record and than MAC it.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                   In case of successful write.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC In case of invalid input
 *                                                  parameters.
 */
LoaderSecLib_Output_t LoaderSecLib_Calculate_And_MAC_DomainData(void)
{
    LoaderSecLib_Output_t           ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    //  ErrorCode_e                     BootRecH_RetVal = E_GENERAL_FATAL_ERROR;
    DomainsData_t                  *Domain_Data_p = NULL;
    LDR_SEC_LIB_Domain_BootRecord_t DomainBootRecord;

    memset(&DomainBootRecord, 0x00, sizeof(LDR_SEC_LIB_Domain_BootRecord_t));

    //Get written domain from domain internal data block.
    ReturnValue = LoaderSecLib_GetInternalDataBlock(INTERNAL_DATA_BLOCK_DOMAINS, (void **)&Domain_Data_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    DomainBootRecord.DomainID = LOADER_SEC_LIB_DOMAIN_DATA_ID;
    DomainBootRecord.DomainSize = sizeof(LDR_SEC_LIB_Domain_BootRecord_t);
    DomainBootRecord.Reserved = 0;
    DomainBootRecord.DomainData.Length = sizeof(Domain_Data_t);
    DomainBootRecord.DomainData.Domain = (uint32)Domain_Data_p->WrittenDomain;

    //Calculate MAC for Domain data
    ReturnValue = LoaderSecLib_Calculate_MAC(PD_ESB_MAC_TYPE_INTERNAL_BLOCKING_00,
                  (uint8 *)&DomainBootRecord.DomainData.Length,
                  sizeof(Domain_Data_t) - PD_ESB_MAX_MAC_BYTE_SIZE,
                  DomainBootRecord.DomainData.MAC);
    VERIFY(ReturnValue == LOADER_SEC_LIB_SUCCESS, ReturnValue);

    //Write data in boot records
    ReturnValue = Do_LoaderSecLib_Write_BootRecord(LOADER_SEC_LIB_DOMAIN_DATA_ID, (uint32 *)&DomainBootRecord, sizeof(LDR_SEC_LIB_Domain_BootRecord_t)); // TODO: should be removed in application layer
    VERIFY(E_SUCCESS == ReturnValue, LOADER_SEC_LIB_WRITING_BOOTRECORD_FAILED);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
ErrorExit:
    B_(printf("security_data.c %d: LoaderSecLib_Calculate_And_MAC_DomainData return ... (%d)\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*
 * Read the ASIC chip ID.
 *
 * @retval ChipId Chip Id.
 */
uint32 LoaderSecLib_Read_Chip_Id(void)
{
    uint32 ChipId = 0;

    (void)R_Do_BOOTROM_GetChipID((uint16 *)&ChipId);

    B_(printf("security_data.c %d: LoaderSecLib_Read_Chip_Id return ... (%d)\n", __LINE__, ChipId);)
    return ChipId;
}

/*
 * Get the random generated value.
 *
 * @retval RndData Random generated value.
 */
uint32 LoaderSecLib_GetRand(void)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    uint32 RndData = 0;
    ESBROM_Random_Session_t PRNG_Session;
    ESBROM_Random_t Random;
    ESBROM_Return_t ESBROM_RetVal;

    ESBROM_RetVal = R_Do_ESBROM_Random_Init(&PRNG_Session, ESBROM_RANDOM_PRNG_EMP);
    VERIFY(ESBROM_RETURN_OK == ESBROM_RetVal, ReturnValue);

    ESBROM_RetVal = R_Do_ESBROM_Random_Get(&PRNG_Session, &Random);
    VERIFY(ESBROM_RETURN_OK == ESBROM_RetVal, ReturnValue);
    memcpy(&RndData, Random.Rand, sizeof(uint32));

ErrorExit:
    B_(printf("security_data.c %d: LoaderSecLib_GetRand return ... (%d)\n", __LINE__, RndData);)
    return RndData;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/


/** @} */

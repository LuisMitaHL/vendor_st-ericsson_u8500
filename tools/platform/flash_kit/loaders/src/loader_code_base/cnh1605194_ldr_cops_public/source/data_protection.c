/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/**
 *  @addtogroup ldr_security_library
 *  @{
 *
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "r_loader_sec_lib.h"
#include "loader_sec_lib_internals.h"
#include "e_loader_sec_lib.h"
#include "r_loader_sec_lib_port_funcs.h"
#include "r_esbrom.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_pd_esb.h"
#include "jmptable.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/**
 * Internal data block
 */
typedef struct {
    Internal_Data_Block_Id_t    DataBlock_ID;
    void                        *Data_p;
    uint32                      Size;
    uint8                       DataBlockHash[LOADER_SEC_LIB_SHA256_HASH_LENGTH];
} LoaderSecLib_InternalData_t;


/**
 * Internal data node
 */
typedef struct LoaderSecLib_InternalDataNode_s {
    LoaderSecLib_InternalData_t Entry;
    struct LoaderSecLib_InternalDataNode_s *LinkDataBlock_p;
} LoaderSecLib_InternalDataBlock_t;

/**
 * General internal data structure
 */
typedef struct {
    LoaderSecLib_InternalDataBlock_t  *InternalData_p;
    uint8                            TotalHash[LOADER_SEC_LIB_SHA256_HASH_LENGTH];
} LoaderSecLib_InternalDataList_t;

/**
 * Maximal number of ChipID that Loader supports
 */
#define CHIP_ID_MAX_NUMBER 3

/**
 * Valid ChipID numbers that are supported by loader
 */
static uint32 ValidChipID_Filed[CHIP_ID_MAX_NUMBER] = {
    0x2000,
    0x2001,
    0xF001
};

/**  */
static LoaderSecLib_InternalDataList_t InternalList;

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static boolean LoaderSecLib_FindInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID,
        LoaderSecLib_InternalDataBlock_t **ReqDataBlock_pp,
        LoaderSecLib_InternalDataBlock_t **PreviousBlock_pp,
        LoaderSecLib_InternalDataBlock_t **NextBlock_pp);
static LoaderSecLib_InternalDataBlock_t *LoaderSecLib_FindLastInternalDataBlock(void);
static LoaderSecLib_Output_t LoaderSecLib_Internal_Init(void);
static LoaderSecLib_Output_t LoaderSecLib_CalculateAllInternalDataBlocks(void);
//static LoaderSecLib_Output_t LoaderSecLib_VerifyAllInternalDataBlocks(void);
//static LoaderSecLib_Output_t LoaderSecLib_VerifyInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/

/*
 * This routine initializes the data used by the Loader Security Library.
 *
 * Create list of the internal data structures. So that the other external
 * functions can be used. This function must be called before any other
 * security library functions.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute of
 *                                                    function.
 * @retval  LOADER_SEC_LIB_INIT_CALLED_TWICE          Sec library is already
 *                                                    initialized.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Init(void)
{
    LoaderSecLib_Output_t     ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    static boolean IsInitCalledOneTime = FALSE;

    if (!IsInitCalledOneTime) {
        // This will disable the possibility to call init 2 times..
        IsInitCalledOneTime = TRUE;

        // The internal variables are initialized by calling LoaderSecLib_Internal_Init().
        ReturnValue = LoaderSecLib_Internal_Init();
        VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), ReturnValue);

        // Make hardware checks
        ReturnValue = LoaderSecLib_Hw_Check();
        VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), ReturnValue);
    } else {
        A_(printf("data_protoection.c (%d): ** ERR: Called Init 2 times! **\n", __LINE__);)
        ReturnValue = LOADER_SEC_LIB_INIT_CALLED_TWICE;
        goto ErrorExit;
    }

    B_(if (ReturnValue == LOADER_SEC_LIB_SUCCESS) printf("data_protoection.c (%d): INFO: Security library initialized successful.! \n", __LINE__);)

ErrorExit:
        if (LOADER_SEC_LIB_SUCCESS != ReturnValue) {
            A_(printf("data_protoection.c (%d): ** ERR: Loader SecLib initialization failed! **\n", __LINE__);)
            Do_LoaderSecLib_ShutDown();
        }

    return ReturnValue;
}

/*
 * This routine free all memories allocated from loader security
 * library and stop the loader.
 */
void Do_LoaderSecLib_ShutDown(void)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    LoaderSecLib_InternalDataBlock_t *InternalDataBlock_p = NULL;

    //Find first internal data block starting from last list node.
    InternalDataBlock_p = LoaderSecLib_FindLastInternalDataBlock();

    while (NULL != InternalDataBlock_p) {
        //Remove last node from list.
        ReturnValue = LoaderSecLib_RemoveInternalDataBlock(InternalDataBlock_p->Entry.DataBlock_ID);
        B_(printf("LINE %d: Internal data block ID (%d) status of removing (%d)", __LINE__, InternalDataBlock_p->Entry.DataBlock_ID, ReturnValue);)

        InternalDataBlock_p = LoaderSecLib_FindLastInternalDataBlock();
    }

    memset(InternalList.TotalHash, 0x00, LOADER_SEC_LIB_SHA256_HASH_LENGTH);

    ReturnValue = LoaderSecLib_ESB_DownLock();

    if (LOADER_SEC_LIB_SUCCESS != ReturnValue) {
        B_(printf("LINE %d: ESB shut down status ... (%d)", __LINE__, ReturnValue);)
    }

    // Stop the loader
    while (1) {
        ;
    }
}

/*
 * This routine check if loader can be started on current hardware
 * configuration. Loaders can be executed only if ChipID is defined in
 * ValidChipID_Filed[] list.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS             Successful execute of function.
 * @retval  LOADER_SEC_LIB_FAILURE             If something unexpected hapened.
 * @retval  LOADER_SEC_LIB_CHIP_ID_INVALID     If Chip ID is invalid.
 */
LoaderSecLib_Output_t LoaderSecLib_Hw_Check(void)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_FAILURE;
    COPSROM_Security_Settings_t *SecuritySettings_p = NULL;
    uint32 ChipID = 0;
    uint32 Index = 0;

    ReturnValue = Do_LoaderSecLib_GetSecuritySettings(&SecuritySettings_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    // Loaders can be executed only if ChipID is defined in ValidChipID_Filed[] list
    ReturnValue = LOADER_SEC_LIB_CHIP_ID_INVALID;
    ChipID = LoaderSecLib_Read_Chip_Id();

    do {
        if (ChipID == ValidChipID_Filed[Index]) {
            ReturnValue = LOADER_SEC_LIB_SUCCESS;
        }

        Index++;
    } while ((ReturnValue == LOADER_SEC_LIB_CHIP_ID_INVALID) && (Index < CHIP_ID_MAX_NUMBER));

    // Testing if all values from ValidChipID_Filed[] are compared to ChipID
    if (Index > CHIP_ID_MAX_NUMBER) {
        A_(printf("data_protoection.c (%d): ** ERR: Invalid ChipID! **\n", __LINE__);)
        ReturnValue = LOADER_SEC_LIB_CHIP_ID_INVALID;
        goto ErrorExit;
    }

ErrorExit:
    B_(printf("data_protoection.c (%d): ** LoaderSecLib_Hw_Check return ... (%d) \n! **\n", __LINE__, ReturnValue);)
    return ReturnValue;
}

/*
 * ESB block downlock.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Successful executioin.
 */
LoaderSecLib_Output_t LoaderSecLib_ESB_DownLock(void)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_ESB_DOWNLOCK_FAILED;
    PD_ESB_BitMap64_t BitMap;

    BitMap.Map[0] = 0xffffffff;
    BitMap.Map[1] = 0xffffffff;
    VERIFY(PD_ESB_RETURN_VALUE_OK == R_Do_PD_ESB_MAC_LockMap(&BitMap), LOADER_SEC_LIB_ESB_DOWNLOCK_FAILED);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}


/*
 * This routine add new internal data block.
 * @param [in]  DataBlock_ID  Id of the internal data block
 * @param [in]  Data_p        Pointer to the new internal data block.
 * @param [in]  Size          Size of the new internal data block.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute of
 *                                                    function.
 */
LoaderSecLib_Output_t LoaderSecLib_AddInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID, void *Data_p, int Size)
{
    LoaderSecLib_Output_t             ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    LoaderSecLib_InternalDataBlock_t *NewDataBlock_p = NULL;
    LoaderSecLib_InternalDataBlock_t *LastDataBlock_p = NULL;
    LoaderSecLib_InternalDataBlock_t *Block_p = NULL;
    LoaderSecLib_InternalDataBlock_t *NextBlock_p = NULL;
    LoaderSecLib_InternalDataBlock_t *PreviousBlock_p = NULL;

    /* Check Input parameters */
    VERIFY(((NULL != Data_p) && (Size != 0)), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    /* check the internal data in the list of the internal data blocks */
    VERIFY((TRUE != LoaderSecLib_FindInternalDataBlock(DataBlock_ID, &Block_p, &PreviousBlock_p, &NextBlock_p)), LOADER_SEC_LIB_DATA_BLOCK_EXIST);

    NewDataBlock_p = (LoaderSecLib_InternalDataBlock_t *)LoaderSecLib_Allocate(sizeof(LoaderSecLib_InternalDataBlock_t));

    if (NewDataBlock_p) {
        /* add this new Data block */
        NewDataBlock_p->Entry.DataBlock_ID = DataBlock_ID;
        NewDataBlock_p->Entry.Data_p = Data_p;
        NewDataBlock_p->Entry.Size = Size;
        NewDataBlock_p->LinkDataBlock_p = NULL;
        ReturnValue = LoaderSecLib_Calculate_MAC(PD_ESB_MAC_TYPE_SHA256, (uint8 *)Data_p, Size, &NewDataBlock_p->Entry.DataBlockHash[0]);
        VERIFY((ReturnValue == LOADER_SEC_LIB_SUCCESS), ReturnValue);
    }

    /* add the new data block in the list of the data blocks */
    LastDataBlock_p = LoaderSecLib_FindLastInternalDataBlock();

    if (NULL == LastDataBlock_p) {
        InternalList.InternalData_p = NewDataBlock_p;
    } else {
        LastDataBlock_p->LinkDataBlock_p = NewDataBlock_p;
    }

    ReturnValue = LoaderSecLib_CalculateAllInternalDataBlocks();
    VERIFY((ReturnValue == LOADER_SEC_LIB_SUCCESS), ReturnValue);

ErrorExit:
    return ReturnValue;
}


/*
 * This routine remove the internal data block.
 *
 * @param [in]  DataBlock_ID  Id of the internal data block.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute of
 *                                                    function.
 */
LoaderSecLib_Output_t LoaderSecLib_RemoveInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID)
{
    LoaderSecLib_Output_t     ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    LoaderSecLib_InternalDataBlock_t *Block_p = NULL;
    LoaderSecLib_InternalDataBlock_t *NextBlock_p = NULL;
    LoaderSecLib_InternalDataBlock_t *PreviousBlock_p = NULL;

    /* check the internal data in the list of the internal data blocks */
    if (TRUE == LoaderSecLib_FindInternalDataBlock(DataBlock_ID, &Block_p, &PreviousBlock_p, &NextBlock_p)) {
        if (NULL != Block_p) {
            if (NULL == PreviousBlock_p) {
                InternalList.InternalData_p = NextBlock_p;
            } else {
                PreviousBlock_p->LinkDataBlock_p = NextBlock_p;
            }

            ReturnValue = LoaderSecLib_Free((uint8 **)&Block_p->Entry.Data_p);
            VERIFY(ReturnValue == LOADER_SEC_LIB_SUCCESS, ReturnValue);
            ReturnValue = LoaderSecLib_Free((uint8 **)&Block_p);
            VERIFY(ReturnValue == LOADER_SEC_LIB_SUCCESS, ReturnValue);
        } else {
            B_(printf("data ptotection.c (%d): Internal data block do not exist.\n", __LINE__);)
            ReturnValue = LOADER_SEC_LIB_DATA_BLOCK_DO_NOT_EXIST;
            goto ErrorExit;
        }
    }

    ReturnValue = LoaderSecLib_CalculateAllInternalDataBlocks();
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}


/*
 * This routine update the requested internal data block.
 *
 * @param [in]  DataBlock_ID Id of the requested internal data block.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                 Successful execute of function.
 * @retval LOADER_SEC_LIB_DATA_BLOCK_DO_NOT_EXIST If requested data block do not
 *                                                exist.
 */
LoaderSecLib_Output_t LoaderSecLib_UpdateInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID)
{
    LoaderSecLib_Output_t     ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    LoaderSecLib_InternalDataBlock_t *DataBlock_p = NULL;
    LoaderSecLib_InternalDataBlock_t *Block_p = NULL;
    LoaderSecLib_InternalDataBlock_t *NextBlock_p = NULL;
    LoaderSecLib_InternalDataBlock_t *PreviousBlock_p = NULL;

    VERIFY((TRUE == LoaderSecLib_FindInternalDataBlock(DataBlock_ID, &Block_p, &PreviousBlock_p, &NextBlock_p)), LOADER_SEC_LIB_DATA_BLOCK_DO_NOT_EXIST);
    DataBlock_p = Block_p;

    ReturnValue = LoaderSecLib_Calculate_MAC(PD_ESB_MAC_TYPE_SHA256, (uint8 *)DataBlock_p->Entry.Data_p, DataBlock_p->Entry.Size, &DataBlock_p->Entry.DataBlockHash[0]);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = LoaderSecLib_CalculateAllInternalDataBlocks();
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

ErrorExit:
    return ReturnValue;
}


/*
 * This routine search the internal data block.
 *
 * @param [in]  DataBlock_ID Id of the internal data block.
 * @param [out] DataBlock_pp Pointer to the requested data block.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                Successful execute of function.
 */
LoaderSecLib_Output_t LoaderSecLib_GetInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID, void **DataBlock_pp)
{
    LoaderSecLib_Output_t     ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    LoaderSecLib_InternalDataBlock_t *Block_p = NULL;
    LoaderSecLib_InternalDataBlock_t *NextBlock_p = NULL;
    LoaderSecLib_InternalDataBlock_t *DataBlock_p = NULL;
    LoaderSecLib_InternalDataBlock_t *PreviousBlock_p = NULL;
    boolean Res = FALSE;

    /* check the input parameters */
    VERIFY((NULL != DataBlock_pp), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    Res = LoaderSecLib_FindInternalDataBlock(DataBlock_ID, &Block_p, &PreviousBlock_p, &NextBlock_p);

    if (Res) {
        DataBlock_p = Block_p;
        *DataBlock_pp = DataBlock_p->Entry.Data_p;
        ReturnValue = LOADER_SEC_LIB_SUCCESS;
    } else {
        ReturnValue = LOADER_SEC_LIB_DATA_BLOCK_DO_NOT_EXIST;
    }

ErrorExit:
    return ReturnValue;
}

/*
 * Create data block with specified length.
 *
 * @param[in] Data_pp pointer to domains data block.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                  Successfully created data
 *                                                 block.
 * @retval LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED Failed to allocate memory for
 *                                                 data block.
 */
LoaderSecLib_Output_t LoaderSecLib_CreateDataBlock(void **Data_pp, uint32 Length)
{
    LoaderSecLib_Output_t   ReturnValue;

    /* Check the input parameters */
    VERIFY((NULL != Data_pp), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    *Data_pp = (void *)LoaderSecLib_Allocate(Length);
    VERIFY((NULL != *Data_pp), LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*
 * This routine calculates the hash values with specified hash algorithm.
 *
 * @param[in]  MAC_Type   Type of hash algorithm.
 * @param[in]  Data_p     Data for calculation.
 * @param[in]  Length     Length of the data for calculation.
 * @param[out] MAC_p      Result of the calculation.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute
 *                                                    of function.
 */
LoaderSecLib_Output_t LoaderSecLib_Calculate_MAC(PD_ESB_MAC_Type_t MAC_Type, uint8 *Data_p, uint32 Length, uint8 *MAC_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    PD_ESB_ReturnValue_t  PD_ESB_RetVal = PD_ESB_RETURN_VALUE_GENERAL_FAILURE;

    VERIFY(((NULL != Data_p) && (NULL != MAC_p) && (Length > 0)), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    PD_ESB_RetVal = R_Do_PD_ESB_MAC_Generate(MAC_Type, NULL, 0, Data_p, Length, (PD_ESB_MAC_Value_t *)MAC_p);
    VERIFY(PD_ESB_RETURN_VALUE_OK == PD_ESB_RetVal, LOADER_SEC_LIB_FAILURE);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/*
 * This routine verify the specified MAC value.
 *
 * @param[in] Type   Type of hash algorithm.
 * @param[in] Data_p Data for verification.
 * @param[in] Length Length of the data for verification.
 * @param[in] MAC_p Pointer to the MAC to compare against.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute
 *                                                    of function.
 */
LoaderSecLib_Output_t LoaderSecLib_Verify_MAC(PD_ESB_MAC_Type_t MAC_Type, uint8 *Data_p, uint32 Length, uint8 *MAC_p)
{
    LoaderSecLib_Output_t      ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
    PD_ESB_ReturnValue_t       PD_ESB_RetVal = PD_ESB_RETURN_VALUE_GENERAL_FAILURE;
    PD_ESB_MAC_CompareResult_t CompareResult = PD_ESB_MAC_COMPARE_RESULT_FAILED;

    VERIFY(((NULL != Data_p) && (NULL != MAC_p) && (Length > 0)), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    PD_ESB_RetVal = R_Do_PD_ESB_MAC_Compare(MAC_Type, NULL, 0, Data_p, Length, (PD_ESB_MAC_Value_t *)MAC_p, &CompareResult);
    VERIFY(PD_ESB_RETURN_VALUE_OK == PD_ESB_RetVal, LOADER_SEC_LIB_FAILURE);
    VERIFY(PD_ESB_MAC_COMPARE_RESULT_PASSED == CompareResult, LOADER_SEC_LIB_ESB_MAC_NOT_VERIFIED);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/*
 * Wrap function to the esb-rom functon "R_Do_ESBROM_MAC_Cmp".
 *
 * Makes a complete MAC compare operation with the specified data.
 *
 * @param[in]     MacType                             The type of Mac to use with this session.
 * @param[in]     MacKey_p                            Pointer to a Mac key.  If the MacType indicates that an external key is to be used,
 *                                                    this argument should point to the key. In other cases this parameter is ignored.
 * @param[in]     Data_p                              Pointer to data to process.
 * @param[in]     MacKeySize                          The size of the key, in bytes. Only used in external key mode. Ignored otherwise.
 * @param[in]     DataLength                          The length of the data in bytes.
 * @param[in]     Mac_p                               Pointer to the MAC to compare against.
 * @param[out]    CompareResult_p                     Pointer to the result of the compare.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ARGUMENT_ERROR               The MacType argument is out of bounds.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 * @retval ESBROM_RETURN_NOT_SUPPORTED                This return code means that this interface does not support a certain mode of operation.  See the c_esbrom.h file for configuration.
 *
 * @see    ESBROM_MAC_Gen
 */
ESBROM_Return_t   Wrap_R_Do_ESBROM_MAC_Cmp(
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

/*
 * Wrap function to the esb-rom functon "R_Do_ESBROM_MAC_Gen".
 *
 * Makes a complete MAC generation operation with the specified data.
 *
 * @param[in]     MacType                             The type of Mac to use with this session.
 * @param[in]     MacKey_p                            Pointer to a Mac key.  If the MacType indicates that an external key is to be used,
 *                                                    this argument should point to the key. In other cases this parameter is ignored.
 * @param[in]     MacKeySize                          The size of the key, in bytes. Only used in external key mode. Ignored otherwise.
 * @param[in]     Data_p                              Pointer to data to process.
 * @param[in]     DataLength                          The length of the data in bytes.
 * @param[out]    Mac_p                               Pointer to the resulting MAC.
 *
 * @retval ESBROM_RETURN_OK                           Operation succeeded.
 * @retval ESBROM_RETURN_ARGUMENT_ERROR               The MacType argument is out of bounds.
 * @retval ESBROM_RETURN_MAC_ERROR_LOCKED             Operation not possible because the MAC has been locked down.
 * @retval ESBROM_RETURN_NOT_IMPLEMENTED              This return code means that this interface is not implemented.  See the c_esbrom.h file for configuration.
 * @retval ESBROM_RETURN_NOT_SUPPORTED                This return code means that this interface does not support a certain mode of operation.  See the c_esbrom.h file for configuration.
 *
 * @see    ESBROM_MAC_Cmp
 */
ESBROM_Return_t   Wrap_R_Do_ESBROM_MAC_Gen(
    const ESBROM_MacType_t                          MacType,
    const ESBROM_KeyUnknownSize_t                  *MacKey_p,
    const ESBROM_KeySize_t                          MacKeySize,
    const uint8                                    *Data_p,
    const uint32                                    DataLength,
    ESBROM_Mac_t                       *const Mac_p)
{
    return  R_Do_ESBROM_MAC_Gen(MacType, MacKey_p, MacKeySize, Data_p, DataLength, Mac_p);
}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * Security library initialization.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Successful initialization.
 * @retval
 */
static LoaderSecLib_Output_t LoaderSecLib_Internal_Init(void)
{
    LoaderSecLib_Output_t         ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;

    memset((uint8 *)&InternalList.TotalHash[0], 0x00, LOADER_SEC_LIB_SHA256_HASH_LENGTH);

    ReturnValue = LoaderSecLib_InitDomainData();
    VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), ReturnValue);

    ReturnValue = LoaderSecLib_InitAuthentication();
    VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), ReturnValue);

ErrorExit:
    return ReturnValue;
}


/*
 * This routine verify the specified internal data block.
 *
 * @param[in] DataBlock_ID Id of the internal data block.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute of
 *                                                    function.
 */
//static LoaderSecLib_Output_t LoaderSecLib_VerifyInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID)
//{
//  LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC;
//  LoaderSecLib_InternalDataBlock_t *DataBlock_p = NULL;
//
//  ReturnValue = LoaderSecLib_GetInternalDataBlock(DataBlock_ID, (void*)&DataBlock_p);
//  VERIFY((ReturnValue == LOADER_SEC_LIB_SUCCESS), ReturnValue);
//
//  ReturnValue = LoaderSecLib_Verify_MAC(PD_ESB_MAC_TYPE_SHA256, DataBlock_p->Entry.Data_p, DataBlock_p->Entry.Size, DataBlock_p->Entry.DataBlockHash);
//  VERIFY(ReturnValue == LOADER_SEC_LIB_SUCCESS, ReturnValue);
//
// ReturnValue = LOADER_SEC_LIB_SUCCESS;
//ErrorExit:
//  return ReturnValue;
//}


/*
 * This routine verify all the internal data blocks.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute of
 *                                                    function.
 */
//static LoaderSecLib_Output_t LoaderSecLib_VerifyAllInternalDataBlocks(void)
//{
//  LoaderSecLib_Output_t ReturnValue;
//  PD_ESB_ReturnValue_t PD_ESB_RetVal = PD_ESB_RETURN_VALUE_GENERAL_FAILURE;
//  PD_ESB_MAC_SessionContext_t MacSessionCtx;
//  PD_ESB_MAC_CompareResult_t CompareResult;
//  LoaderSecLib_InternalDataBlock_t *NextInternalDataBlock_p = InternalList.InternalData_p;
//
//  VERIFY((NULL != NextInternalDataBlock_p),LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);
//
//  PD_ESB_RetVal = R_Do_PD_ESB_MAC_CompareInitialize(&MacSessionCtx, PD_ESB_MAC_TYPE_SHA256, NULL, 0, FALSE);
//  VERIFY((PD_ESB_RETURN_VALUE_OK == PD_ESB_RetVal), LOADER_SEC_LIB_ESB_MAC_INIT_FAILED);
//
//  do
//  {
//    PD_ESB_RetVal = R_Do_PD_ESB_MAC_Update(&MacSessionCtx, 0, NULL, NextInternalDataBlock_p->Entry.Data_p, NextInternalDataBlock_p->Entry.Size);
//    VERIFY((PD_ESB_RETURN_VALUE_OK == PD_ESB_RetVal), LOADER_SEC_LIB_ESB_MAC_UPDATE_FAILED);
//
//    NextInternalDataBlock_p = NextInternalDataBlock_p->LinkDataBlock_p;
//  }while(NextInternalDataBlock_p != NULL);
//
//  PD_ESB_RetVal = R_Do_PD_ESB_MAC_CompareFinalize(&MacSessionCtx, InternalList.TotalHash, (uint32)LOADER_SEC_LIB_SHA256_HASH_LENGTH, &CompareResult);
//  VERIFY(PD_ESB_RETURN_VALUE_OK == PD_ESB_RetVal, LOADER_SEC_LIB_FAILURE);
//  VERIFY(PD_ESB_MAC_COMPARE_RESULT_PASSED == CompareResult, LOADER_SEC_LIB_ESB_MAC_NOT_VERIFIED);
//
//  ReturnValue = LOADER_SEC_LIB_SUCCESS;
//ErrorExit:
//  return ReturnValue;
//}


/*
 * This routine calculate all the internal data blocks.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute of
 *                                                    function.
 */
static LoaderSecLib_Output_t LoaderSecLib_CalculateAllInternalDataBlocks(void)
{
    LoaderSecLib_Output_t       ReturnValue;
    PD_ESB_ReturnValue_t        PD_ESB_RetVal;
    PD_ESB_MAC_SessionContext_t MacSessionCtx;
    uint32                      MAC_Length = LOADER_SEC_LIB_SHA256_HASH_LENGTH; //xvlanau: Do not set to zero!!!
    LoaderSecLib_InternalDataBlock_t *NextInternalDataBlock_p = InternalList.InternalData_p;

    memset((uint8 *)&InternalList.TotalHash[0], 0x00, LOADER_SEC_LIB_SHA256_HASH_LENGTH);
    VERIFY((NULL != NextInternalDataBlock_p), LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC);

    PD_ESB_RetVal = R_Do_PD_ESB_MAC_GenerationInitialize(&MacSessionCtx, PD_ESB_MAC_TYPE_SHA256, NULL, 0, FALSE);
    VERIFY((PD_ESB_RETURN_VALUE_OK == PD_ESB_RetVal), LOADER_SEC_LIB_ESB_MAC_INIT_FAILED);

    do {
        PD_ESB_RetVal = R_Do_PD_ESB_MAC_Update(&MacSessionCtx, 0, NULL, NextInternalDataBlock_p->Entry.Data_p, NextInternalDataBlock_p->Entry.Size);
        VERIFY((PD_ESB_RETURN_VALUE_OK == PD_ESB_RetVal), LOADER_SEC_LIB_ESB_MAC_UPDATE_FAILED);

        NextInternalDataBlock_p = NextInternalDataBlock_p->LinkDataBlock_p;
    } while (NextInternalDataBlock_p != NULL);

    PD_ESB_RetVal = R_Do_PD_ESB_MAC_GenerationFinalize(&MacSessionCtx, (uint8 *)InternalList.TotalHash, &MAC_Length);
    VERIFY((PD_ESB_RETURN_VALUE_OK == PD_ESB_RetVal), LOADER_SEC_LIB_ESB_MAC_FINAL_FAILED);
    VERIFY(MAC_Length == LOADER_SEC_LIB_SHA256_HASH_LENGTH, LOADER_SEC_LIB_FAILURE);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
ErrorExit:
    return ReturnValue;
}

/*
 * Find data block from the linked list of internal data blocks.
 *
 * @param[in] DataBlock_ID     ID of requested data block.
 * @param[in] ReqDataBlock_pp  Pointer to the requested data block.
 * @param[in] NextBlock_pp     Pointer to the next data block.
 *
 * @retval TRUE  If data block exist.
 * @retval FALSE If data block don't exist.
 */
static boolean LoaderSecLib_FindInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID,
        LoaderSecLib_InternalDataBlock_t **ReqDataBlock_pp,
        LoaderSecLib_InternalDataBlock_t **PreviousBlock_pp,
        LoaderSecLib_InternalDataBlock_t **NextBlock_pp)
{
    LoaderSecLib_InternalDataBlock_t *DataBlock_p = InternalList.InternalData_p;
    boolean Status = FALSE;
    *ReqDataBlock_pp = NULL;
    *NextBlock_pp = NULL;
    *PreviousBlock_pp = NULL;

    if (NULL == DataBlock_p) {
        goto ErrorExit;
    }

    do {
        if (DataBlock_p->Entry.DataBlock_ID == DataBlock_ID) {
            *ReqDataBlock_pp = DataBlock_p;
            *NextBlock_pp = DataBlock_p->LinkDataBlock_p;
            Status = TRUE;
            break;
        }

        *PreviousBlock_pp = DataBlock_p;
        DataBlock_p = DataBlock_p->LinkDataBlock_p;
    } while (DataBlock_p != NULL);

ErrorExit:
    return Status;
}

/*
 * Find the last data block in the linked list of internal data blocks.
 *
 * @retval NULL    If is not crated list with data blocks.
 * @retval POINTER Pointer to the last data block in tne list of the
 *                 internal data blocks.
 */
static LoaderSecLib_InternalDataBlock_t *LoaderSecLib_FindLastInternalDataBlock(void)
{
    LoaderSecLib_InternalDataBlock_t *DataBlock_p = InternalList.InternalData_p;

    if (NULL == DataBlock_p) {
        goto ErrorExit;
    }

    while (DataBlock_p->LinkDataBlock_p != NULL) {
        DataBlock_p = DataBlock_p->LinkDataBlock_p;
    }

ErrorExit:
    return DataBlock_p;
}

/** @} */

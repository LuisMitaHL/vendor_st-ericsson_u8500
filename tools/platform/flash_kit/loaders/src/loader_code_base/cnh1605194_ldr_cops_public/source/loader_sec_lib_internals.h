/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSIONGUARD_LOADER_SEC_LIB_INTERNALS_H
#define _INCLUSIONGUARD_LOADER_SEC_LIB_INTERNALS_H
/**
 * @addtogroup ldr_security_library
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "t_loader_sec_lib.h"
#include "e_loader_sec_lib.h"
#include "t_esbrom.h"
#include "t_pd_esb.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * This routine verify the hash value.
 *
 * @param[in] MAC_Type   Type of hash algorithm.
 * @param[in] Data_p     Data for verification.
 * @param[in] Length     Length of the data for verification.
 * @param[in] MAC_p      Hash for verification.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful execute
 *                                                    of function.
 */
LoaderSecLib_Output_t LoaderSecLib_Verify_MAC(PD_ESB_MAC_Type_t MAC_Type, uint8 *Data_p, uint32 Length, uint8 *MAC_p);

/**
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
LoaderSecLib_Output_t LoaderSecLib_Calculate_MAC(PD_ESB_MAC_Type_t MAC_Type, uint8 *Data_p, uint32 Length, uint8 *MAC_p);

/**
 * This routine read new internal data block.
 * @param [in]  DataBlock_ID  Id of the internal data block
 * @param [in]  DataBlock_pp  Pointer to the requested internal data block.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS              Succesful execute of function.
 */
LoaderSecLib_Output_t LoaderSecLib_GetInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID, void **DataBlock_pp);

/**
 * Verify the static data.
 *
 * @param[in] StaticData_p Buffer with static data.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                   Successful verification.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC If input parameter is
 *                                                  not valid.
 */
LoaderSecLib_Output_t LoaderSecLib_Verify_StaticData(uint8 const *const StaticData_p);

/**
 * Create data block with specified length.
 *
 * @param[in] Data_pp pointer to data block.
 * @param[in] Length  lenfth of the data block.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                  Successfully created data
 *                                                 block.
 * @retval LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED Failed to allocate memory for
 *                                                 data block.
 */
LoaderSecLib_Output_t LoaderSecLib_CreateDataBlock(void **Data_pp, uint32 Length);

/**
 * Authentication data block Initialization.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success initialization.
 */
LoaderSecLib_Output_t LoaderSecLib_InitAuthentication(void);


/**
 * OTP data block Initialization.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success initialization.
 */
LoaderSecLib_Output_t LoaderSecLib_InitOTPData(void);

/**
 * Security data block Initialization.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success initialization.
 */
LoaderSecLib_Output_t LoaderSecLib_InitSecurityData(void);

/**
 * Domain data block Initialization.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success initialization.
 * @retval .
 */
LoaderSecLib_Output_t LoaderSecLib_InitDomainData(void);

/**
 * This routine add new internal data block.
 * @param [in]  DataBlock_ID Id of the internal data block
 * @param [in]  Data_p       Pointer to the new internal data block.
 * @param [in]  Size         Size of the new internal data block.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Succesful execute of function.
 */
LoaderSecLib_Output_t LoaderSecLib_AddInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID, void *Data_p, int Size);

/**
 * This routine remove the internal data block.
 *
 * @param [in]  DataBlock_ID  Id of the internal data block.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS               uccesful execute of function.
 */
LoaderSecLib_Output_t LoaderSecLib_RemoveInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID);

/**
 * This routine update the requested internal data block.
 *
 * @param [in]  DataBlock_ID Id of the requested internal data block.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                 Succesful execute of function.
 * @retval LOADER_SEC_LIB_DATA_BLOCK_DO_NOT_EXIST If requested data block do not
 *                                                exist.
 */
LoaderSecLib_Output_t LoaderSecLib_UpdateInternalDataBlock(Internal_Data_Block_Id_t  DataBlock_ID);

/**
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
    const ESBROM_MacType_t                          MacType,
    const ESBROM_KeyUnknownSize_t            *const MacKey_p,
    const ESBROM_KeySize_t                          MacKeySize,
    const uint8                                    *Data_p,
    const uint32                                    DataLength,
    const ESBROM_Mac_t                       *const Mac_p,
    ESBROM_CompareResult_t             *const CompareResult_p);


/**
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
    ESBROM_Mac_t                       *const Mac_p);


/**
 * This routine check if loader can be started on current hardware
 * configuration. Loaders can be executed only if ChipID is defined in
 * ValidChipID_Filed[] list.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS             Successful execute of function.
 * @retval  LOADER_SEC_LIB_FAILURE             If something unexpected hapened.
 * @retval  LOADER_SEC_LIB_CHIP_ID_INVALID     If Chip ID is invalid.
 */
LoaderSecLib_Output_t LoaderSecLib_Hw_Check(void);

/**
 * ESB block downlock.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Successful executioin.
 */
LoaderSecLib_Output_t LoaderSecLib_ESB_DownLock(void);

/**
 * Read the ASIC chip ID.
 *
 * @retval ChipId Chip Id.
 */
uint32 LoaderSecLib_Read_Chip_Id(void);

/** @} */
#endif /*_INCLUSIONGUARD_LOADER_SEC_LIB_INTERNALS_H*/

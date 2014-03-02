/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _INCLUSIONGUARD_R_LOADER_SEC_LIB_H
#define _INCLUSIONGUARD_R_LOADER_SEC_LIB_H
/**
 * @addtogroup ldr_security_library
 *
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_loader_sec_lib.h"
#include "e_loader_sec_lib.h"
#include "audit.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * This routine initializes the data used by the Loader Security Library.
 *
 * Create the list of the internal data structures. So that the other external
 * functions can be used. This function must be called before any other security
 * library functions.
 *
 * @retval  LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC  Ivalid input parameters.
 * @retval  LOADER_SEC_LIB_SUCCESS                    Succesful execute of function.
 * @retval  LOADER_SEC_LIB_INIT_CALLED_TWICE          Sec library is already initialized.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Init(void);

/**
 * This routine free all memories allocated from loader security
 * library and stop the loader.
 */
void Do_LoaderSecLib_ShutDown(void);

/* Verification */
/**
 * This function initilizes a verification context.
 *
 * @param [in]  Context_p Pointer to a context.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS          The operation was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE   The operation was unsuccessful.
 */

LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Initialize(VerifyContext_t *Context_p);



/**
 * This function verifies a signed header.
 *
 * @param [in]  Context_p         Pointer to a verification context .
 * @param [in]  HeaderBuffer_p    Pointer to a buffer containing the header.
 * @param [in]  HeaderBufferSize  Size of the header buffer.
 * @param [out] HeaderInfo_p      Header information. If set to NULL,
 *                                no header information will be returned.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS        The header verification was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE The header verification was
 *                                        unsuccessful.
 */

LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Header(VerifyContext_t *Context_p,
        uint8 *HeaderBuffer_p,
        uint32 HeaderBufferSize,
        HeaderInfo_t *HeaderInfo_p);

/**
 * This function verifies a payload block.
 *
 * @param [in]  Context_p      Pointer to a verification context.
 * @param [in]  Block_p        Pointer to the payload block.
 * @param [in]  HeaderInfo_p   Pointer to the Header information.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS          The block verification was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE   The block verification was
 *                                          unsuccessful.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_PayloadBlock(VerifyContext_t   *Context_p,
        const uint8 *const Block_p,
        HeaderInfo_t *HeaderInfo_p);

/**
 * This function verifies a payload block.
 *
 * @param [in]  Context_p    Pointer to a verification context.
 * @param [in]  BlockOffset  Offset of the block, relative to the start of the
 *                           payload. Must be the start of a verification block.
 * @param [in]  BlockHash    Pointer to the payload block hash.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS        The block verification was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE The block verification was
 *                                        unsuccessful.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_HashListBlockHash(VerifyContext_t *Context_p,
        uint64           BlockOffset,
        uint8           *BlockHash);

/**
 * This function retrieves information about a payload block.
 *
 * @param [in]  Context_p   Pointer to a verification context.
 * @param [in]  BlockOffset Offset of the block, relative to the start of the
 *                          payload. Must be the start of a verifiaction block.
 * @param [out] BlockInfo_p Block information (length, destination address), If
 *                          set to NULL, no block information will be returned.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS          The operation was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE   The operation was unsuccessful .
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_GetBlockInfo(VerifyContext_t *Context_p,
        uint64 BlockOffset,
        PayloadBlockInfo_t *BlockInfo_p);


/**
 * This function releases all resources allocated for the context.
 *
 * @param [in]  Context_p Pointer to a context.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS        The operation was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE The operation was unsuccessful.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Release(VerifyContext_t *Context_p);

/* Authentication */

/**
 * Authenticate loader.
 *
 * @param[in] Type    Type of authentication method.
 * @param[in] Data_p  Buffer with authentication data.
 * @param[in] Length  Length of the buffer Data_p.
 *
 * @retval LOADER_SEC_LIB_SUCCESS Success authentication.
 * @retval .
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Authenticate(uint32 Type, void *Data_p, uint32 Length);

/**
 * Create Authentication challendge data block
 *
 * @param[in] ChallengeBlock_p       Pointer to the created challendge block.
 * @param[in] ChallengeBlockLength_p Length of the created challendge block.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                   Successfull created
 *                                                  AuthChallengeDataBlock.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC In case of invalid input
 *                                                  parameters.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_CreateAuthChallengeDataBlock(AuthenticationChallengeDataBlock_t *ChallengeBlock_p, uint32 *ChallengeBlockLength_p);
/**
 * Read domains "Effective" and "Written".
 *
 * Efective domain is set by ROMcode, but Written domain is set from the
 * BootRecord in flash.
 *
 * @param [out]  EffectiveDomain_p  Effective domain.
 * @param [out]  WrittenDomain_p  Written domain.
 *
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC In case of invalid input
 *                                                  parameters.
 * @retval LOADER_SEC_LIB_SUCCESS                   In case of successful read.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_ReadDomain(uint32 *EffectiveDomain_p, uint32 *WrittenDomain_p);


/**
 * Write "Written" domain.
 *
 * Written domain is from the BootRecord in flash.
 *
 * @param [out]  Domain  Written domain from flash.
 *
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC In case of invalid input
 *                                                  parameters.
 * @retval LOADER_SEC_LIB_SUCCESS                   In case of successful write.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_WriteDomain(uint32 Domain);

/**
 * Calculate domain data and MAC them.
 *
 * Written domain is from the BootRecord in flash.
 *
 *
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC In case of invalid input
 *                                                  parameters.
 * @retval LOADER_SEC_LIB_WRITING_BOOTRECORD_FAILED If boot record writing failed.
 * @retval LOADER_SEC_LIB_SUCCESS                   In case of successful write.
 *
 *  * @remark  This function can return other errors beside above one, that
 *             depends from some internal sec lib states and processes.
 */
LoaderSecLib_Output_t LoaderSecLib_Calculate_And_MAC_DomainData(void);

/**
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
LoaderSecLib_Output_t Do_LoaderSecLib_VerifyDomainData(Domain_Data_t *DomainData_p);

/* OTP */

/**
 * Reads OTP data
 *
 * @param[out]  OTP_Data_p Buffer that conatins OTP data
 *
 * @retval LOADER_SEC_LIB_SUCCESS                            Successful reading.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC        Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Read_OTP(LOADER_SEC_LIB_OTP_t *OTP_Data_p);


/**
 * Audit permision level.
 *
 * @param[in]  CmdPermission_p  Pointer to structure that hold command
 *                              permission levels.
 *
 * @retval LOADER_SEC_LIB_SUCCESS                   Successful unpacking.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC Invalid input params.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Audit_Permission_Levels(CommandPermissionList_t *CmdPermission_p);

/** @} */
#endif /*_INCLUSIONGUARD_R_LOADER_SEC_LIB_H*/

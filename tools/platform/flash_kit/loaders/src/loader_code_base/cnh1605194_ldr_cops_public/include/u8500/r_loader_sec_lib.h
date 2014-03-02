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
 *
 * @retval  LOADER_SEC_LIB_SUCCESS        The header verification was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE The header verification was
 *                                        unsuccessful.
 */

LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Header(VerifyContext_t *Context_p,
        uint8 *HeaderBuffer_p,
        uint32 HeaderBufferSize);

/**
 * This function verifies the hashlist.
 *
 * @param [in]  Context_p    Pointer to a verification context.
 * @param [in]  HashlistBuffer_P Pointer to the haslist including header (complete file).
 * @param [in]  BufferSize Size of the buffer.
*
 * @retval  LOADER_SEC_LIB_SUCCESS        The block verification was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE   The block verification was
 *                                          unsuccessful.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Hashlist(VerifyContext_t   *Context_p,
        const uint8 *const HashlistBuffer_p,
        size_t BufferSize);

/**
 * This function returns the size of the hasblocks.
 *
 * @param [in]  Context_p    Pointer to a verification context.
 * @param [out]  HashBlockSize The size of the hashblocks
 *
 * @retval  LOADER_SEC_LIB_SUCCESS        The block verification was successful.
 * @retval  LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE   The block verification was
 *                                          unsuccessful.
 *
 * @note Determines the size by looking at the size of the first block
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Get_HashlistBlockSize(VerifyContext_t   *Context_p,
        uint64            *HashBlockSize);

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

/* OTP */

/**
 * Reads OTP data
 *
 * @param[out]  OTP_Data_p Buffer that conatins OTP data
 *
 * @retval LOADER_SEC_LIB_SUCCESS                            Successful reading.
 * @retval LOADER_SEC_LIB_INVALID_PARAMETER_TO_FUNC        Invalid input params.
 */
//LoaderSecLib_Output_t Do_LoaderSecLib_Read_OTP(LOADER_SEC_LIB_OTP_t *OTP_Data_p);


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

/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "t_basicdefinitions.h"
#include "r_loader_sec_lib.h"
#include "loader_sec_lib_internals.h"
#include "r_loader_sec_lib_port_funcs.h"
#include "header_definitions.h"
#include "e_loader_sec_lib.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_copsrom.h"
#include "r_esbrom.h"
#include "jmptable.h"
#include "r_pd_esb.h"

/*******************************************************************************
 * File scope types, constants and variables
 ******************************************************************************/

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static LoaderSecLib_Output_t VerifyHeaderAll(VerifyContext_t *Context_p, uint8 *Header_p);
static boolean GetHashlistEntryIndex(VerifyContext_t *Context_p, uint64 BlockOffset, uint32 *EntryIndex_p);
static LoaderSecLib_Output_t GetHashList(VerifyContext_t *Context_p, uint8 *Header_p);
static LoaderSecLib_Output_t GetHeaderInfo(uint8 *Header_p, VerifyContext_t *Context_p, HeaderInfo_t *HeaderInfo_p);

/*******************************************************************************
 * Definition of external functions
 ******************************************************************************/
/*
 * This function initilizes a verification context.
 *
 * @param [in]  Context_p   Pointer to a context.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS    The operation was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE   The operation was unsuccessful.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Initialize(VerifyContext_t *Context_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;

    VERIFY((Context_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    Context_p->PayloadOffset  = 0;
    Context_p->PayloadSize    = 0;
    Context_p->MaxBlockSize   = 0;
    Context_p->NumberOfBlocks = 0;
    Context_p->HashList_p     = NULL;
    Context_p->State          = INITIALIZED;
    ReturnValue = LOADER_SEC_LIB_SUCCESS;
    return ReturnValue;

ErrorExit:
    return ReturnValue;
}


/*
 * This function verifies a signed header.
 *
 * @param [in]  Context_p           Pointer to a verification context .
 * @param [in]  HeaderBuffer_p    Pointer to a buffer containing the header.
 * @param [in]  HeaderBufferSize  Size of the header buffer.
 * @param [out] HeaderInfo_p      Header information. If set to NULL,
 *                                no header information will be returned.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS       The header verification was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE The header verification was
 *                                        unsuccessful.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Header(VerifyContext_t *Context_p,
        uint8 *HeaderBuffer_p,
        uint32 HeaderBufferSize,
        HeaderInfo_t *HeaderInfo_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;

    VERIFY((Context_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((HeaderBuffer_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((NULL != HeaderInfo_p), LOADER_SEC_LIB_VERIFY_FAILURE);

    // check header size
    VERIFY(HeaderBufferSize >= SIGNED_SOFTWARE_HEADER_LENGTH, LOADER_SEC_LIB_VERIFY_FAILURE);

    // check the Context
    VERIFY(INITIALIZED == Context_p->State, LOADER_SEC_LIB_VERIFY_FAILURE);
    Context_p->State = ERROR;

    // verify the complete header
    ReturnValue = VerifyHeaderAll(Context_p, HeaderBuffer_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    ReturnValue = GetHeaderInfo(HeaderBuffer_p, Context_p, HeaderInfo_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    Context_p->State = HEADER_VERIFIED;
    ReturnValue = LOADER_SEC_LIB_SUCCESS;

    return ReturnValue;

ErrorExit:
    return ReturnValue;
}


/*
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
        HeaderInfo_t *HeaderInfo_p)
{

    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;
    SHA256_Hash_t HashListHash;
    VERIFY((HEADER_VERIFIED == Context_p->State), LOADER_SEC_LIB_VERIFY_FAILURE);

    (void)R_Do_PD_ESB_MAC_Generate(PD_ESB_MAC_TYPE_SHA256,
                                   NULL,
                                   0,
                                   Block_p,
                                   (uint32)Context_p->PayloadSize,
                                   (void *)HashListHash);

    VERIFY((0 == memcmp(HashListHash, Context_p->HashListHash, LOADER_SEC_LIB_SHA256_HASH_LENGTH)), LOADER_SEC_LIB_VERIFY_FAILURE);

    /* Check header type; If it is a Hash list unpack it; else do nothing */
    if ((HeaderInfo_p->HeaderType == LOADER_SEC_LIB_SIGNED_SOFTWARE) && (Context_p->MaxBlockSize != 0)) {
        /* SW image with hash list*/
        ReturnValue = GetHashList(Context_p, (uint8 *)Block_p);
        VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);
    }

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
    return ReturnValue;

ErrorExit:
    Context_p->State = ERROR;
    return ReturnValue;
}

/*
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
        uint8           *BlockHash)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;
    uint32 BlockIndex;

    VERIFY((HEADER_VERIFIED == Context_p->State), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((GetHashlistEntryIndex(Context_p, BlockOffset, &BlockIndex)), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((0 == memcmp(BlockHash, Context_p->HashList_p[BlockIndex].Hash, LOADER_SEC_LIB_SHA256_HASH_LENGTH)), LOADER_SEC_LIB_VERIFY_FAILURE);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
    return ReturnValue;

ErrorExit:
    Context_p->State = ERROR;
    return ReturnValue;
}

/*
 * This function retrieves information about a payload block.
 *
 * @param [in]  Context_p     Pointer to a verification context.
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
        PayloadBlockInfo_t *BlockInfo_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;
    uint32 BlockIndex;

    VERIFY(HEADER_VERIFIED == Context_p->State, LOADER_SEC_LIB_VERIFY_FAILURE);

    VERIFY(GetHashlistEntryIndex(Context_p, BlockOffset, &BlockIndex), LOADER_SEC_LIB_VERIFY_FAILURE);

    BlockInfo_p->Size = Context_p->HashList_p[BlockIndex].BlockInfo.Size;
    BlockInfo_p->DestinationAddress = Context_p->HashList_p[BlockIndex].BlockInfo.DestinationAddress;

    return LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    Context_p->State = ERROR;
    return ReturnValue;
}


/*
 * This function releases all resources allocated for the context.
 *
 * @param [in]  Context_p   Pointer to a context.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS          The operation was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE The operation was unsuccessful.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Release(VerifyContext_t *Context_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;

    Context_p->State = ERROR;
    Context_p->PayloadOffset = 0;
    Context_p->PayloadSize = 0;
    Context_p->MaxBlockSize = 0;
    Context_p->NumberOfBlocks = 0;

    ReturnValue = LoaderSecLib_Free((uint8 **) & (Context_p->HashList_p));
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);
    Context_p->State = SHUTDOWN;

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;

}


/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * This function verify complete header .
 *
 * @param [in]  Context_p      Pointer to a context.
 * @param [in]  Header_p     Pointer to header for verification.
 * @param [in]  HeaderLength Header length.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS                    Successful verification.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE           Unsuccessful verification.
 * @retval  LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED Memory allocation failed.
 * @retval  LOADER_SEC_LIB_MEMORY_RELEASE_FAILED    Memeory release failed.
 */
static LoaderSecLib_Output_t VerifyHeaderAll(VerifyContext_t *Context_p, uint8 *Header_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;
    COPSROM_Return_t COPSROMReturnValue;
    uint8 *PKI_Pool_p = NULL;
    COPSROM_Error_Code_t *COPS_Error_p = NULL;
    COPSROM_Security_Settings_t *SecuritySettings_p = NULL;

    //Test_ESB_Speed();
    // Allocate some memory work buffers
    PKI_Pool_p = (uint8 *)LoaderSecLib_Allocate(COPSROM_PKI_MEMORY_SIZE);
    ASSERT(PKI_Pool_p != NULL);

    COPS_Error_p = (COPSROM_Error_Code_t *)LoaderSecLib_Allocate(sizeof(COPSROM_Error_Code_t));
    ASSERT(PKI_Pool_p != NULL);

    memcpy(&(Context_p->PayloadOffset), (void *)&Header_p[HEADER_EXTENSION_HEADER_LENGTH_OFFSET] , sizeof(uint32));
    memcpy(&(Context_p->PayloadSize), (void *)&Header_p[HEADER_EXTENSION_PAYLOAD_SIZE_OFFSET] , 2 * sizeof(uint32));

    ReturnValue = Do_LoaderSecLib_GetSecuritySettings(&SecuritySettings_p);
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);

    COPSROMReturnValue = R_Do_COPSROM_VerifyHeaderAll(COPS_Error_p,
                         SecuritySettings_p,
                         PKI_Pool_p,
                         Header_p,
                         Context_p->PayloadOffset,
                         COPSROM_VERIFY_SW_LOADER,
                         NULL,
                         Context_p->PayloadHash,
                         Context_p->HashListHash,
                         Wrap_R_Do_ESBROM_MAC_Cmp,
                         Wrap_R_Do_ESBROM_MAC_Gen);

    VERIFY(((COPSROMReturnValue == COPSROM_SUCCESS) || (COPSROMReturnValue == COPSROM_SUCCESS_HEADER_UPDATED)), LOADER_SEC_LIB_VERIFY_FAILURE);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    (void)LoaderSecLib_Free((uint8 **)&COPS_Error_p);
    (void)LoaderSecLib_Free((uint8 **)&PKI_Pool_p);

    return ReturnValue;
}


/*
 * This function unpack the hash list hash from the received packet.
 *
 * @param [in]  Context_p      Pointer to a context.
 * @param [in]  Header_p       Poiner to the header data.
 * @param [in]  HeaderInfo_p Header information.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS                  Successful hash list
 *                                                verification.
 * @retval  LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE Hash list verification failed.
 */
static LoaderSecLib_Output_t GetHashList(VerifyContext_t *Context_p, uint8 *Header_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE;
    uint32 BlockIndex = 0;

    Context_p->HashList_p = (HashListEntry_t *)LoaderSecLib_Allocate(Context_p->NumberOfBlocks * sizeof(HashListEntry_t));
    VERIFY((Context_p->HashList_p != NULL), LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED);

    /* initialize the context with hash list info */
    for (BlockIndex = 0; BlockIndex < Context_p->NumberOfBlocks; BlockIndex++) {
        memcpy(&(Context_p->HashList_p[BlockIndex].PayloadOffset), (void *)&Header_p[BlockIndex *(HASH_LIST_BLOCK_OFFSET_LENGTH + HASH_LIST_BLOCK_LENGTH_LENGTH + SHA256_HASH_SIZE)] , HASH_LIST_BLOCK_OFFSET_LENGTH);
        memcpy(&(Context_p->HashList_p[BlockIndex].BlockInfo.Size), (void *)&Header_p[BlockIndex *(HASH_LIST_BLOCK_OFFSET_LENGTH + HASH_LIST_BLOCK_LENGTH_LENGTH + SHA256_HASH_SIZE) + HASH_LIST_BLOCK_OFFSET_LENGTH ] , HASH_LIST_BLOCK_LENGTH_LENGTH);
        memcpy(Context_p->HashList_p[BlockIndex].Hash, (void *)&Header_p[BlockIndex *(HASH_LIST_BLOCK_OFFSET_LENGTH + HASH_LIST_BLOCK_LENGTH_LENGTH + SHA256_HASH_SIZE) + HASH_LIST_BLOCK_OFFSET_LENGTH + HASH_LIST_BLOCK_LENGTH_LENGTH] , SHA256_HASH_SIZE);
    }

    ReturnValue = LOADER_SEC_LIB_SUCCESS;
    return ReturnValue;

ErrorExit:
    return ReturnValue;
}


/*
 * This function get header data from the received header.
 *
 * @param [in]  Header_p       Poiner to the header data.
 * @param [out] Context_p      Pointer to a context.
 * @param [out] HeaderInfo_p Header information.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS   ...
 */
static LoaderSecLib_Output_t GetHeaderInfo(uint8 *Header_p, VerifyContext_t *Context_p, HeaderInfo_t *HeaderInfo_p)
{

    memcpy(&(HeaderInfo_p->HeaderType), (void *)&Header_p[HEADER_EXTENSION_PAYLOAD_TYPE_OFFSET] , sizeof(uint32));
    memcpy(&(HeaderInfo_p->NumberOfBlocks), (void *)&Header_p[HEADER_EXTENSION_NUMBER_OF_BLOCKS_OFFSET] , sizeof(uint32));
    memcpy(&(HeaderInfo_p->MaxBlockSize), (void *)&Header_p[HEADER_EXTENSION_BLOCK_SIZE_OFFSET] , sizeof(uint32));

    HeaderInfo_p->HashAlgorithm = SHA_256;
    HeaderInfo_p->HashSize = SHA256_HASH_SIZE;

    Context_p->MaxBlockSize = HeaderInfo_p->MaxBlockSize;
    Context_p->NumberOfBlocks = HeaderInfo_p->NumberOfBlocks;

    return LOADER_SEC_LIB_SUCCESS;
}

// @TODO: this could be optimized..
/*
 * Get hash list ...
 *
 * @param [in]  Context_p      Payload length.
 * @param [in]  BlockOffset  Block Offset.
 * @param [out] EntryIndex_p ...
 *
 * @retval  TRUE  The operation was successful.
 * @retval  FALSE The operation was unsuccessful.
 */
static boolean GetHashlistEntryIndex(VerifyContext_t *Context_p, uint64 BlockOffset, uint32 *EntryIndex_p)
{
    uint32 BlockIndex;

    for (BlockIndex = 0; BlockIndex < Context_p->NumberOfBlocks; BlockIndex++) {
        if (Context_p->HashList_p[BlockIndex].PayloadOffset == BlockOffset) {
            *EntryIndex_p = BlockIndex;
            return TRUE;
        }
    }

    return FALSE;
}


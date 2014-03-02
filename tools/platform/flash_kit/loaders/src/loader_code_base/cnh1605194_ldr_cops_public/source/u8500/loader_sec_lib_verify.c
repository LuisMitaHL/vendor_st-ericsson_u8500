/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/**
 * @addtogroup u8500_seclib_functions
 *
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c_system.h"
#include "t_loader_sec_lib.h"
#include "r_loader_sec_lib.h"
#include "e_loader_sec_lib.h"
#include "r_debug.h"
#include "r_debug_macro.h"
#include "bass_app.h"
#include "r_loader_sec_lib_port_funcs.h"

// Each entry is a offset, a block size and a hash
#define HASHENTRYLENGTH       (HASHENTRY_OFFSETLENGTH+HASHENTRY_BLOCKSIZELENGTH+HASHENTRY_HASHLENGTH)
#define HASHENTRY_OFFSETLENGTH  8
#define HASHENTRY_BLOCKSIZELENGTH 8
#define HASHENTRY_HASHLENGTH 32

/*******************************************************************************
 * Declaration of file local functions
 ******************************************************************************/
static LoaderSecLib_Output_t CopyHashList(VerifyContext_t Context_p, const uint8 *const HashlistBuffer_p, size_t BufferSize);
static boolean GetHashlistEntryIndex(VerifyContext_t *Context_p, uint64 BlockOffset, uint64 *EntryIndex_p);
C_(static void PrintContext(VerifyContext_t Context_p);)

/*******************************************************************************
 * Declaration of file local types
 ******************************************************************************/
/** Status of the verification.*/
typedef enum {
    /** Initialized for verification of data block.*/
    INITIALIZED = 1,
    /** Header in the received data packet is verified.*/
    HEADER_VERIFIED,
    /** The hashlist has been verified.*/
    HASHLIST_VERIFIED,
    /** The hashlist has been copied.*/
    HASHLIST_RETRIEVED,
    /** Shut down of verification. */
    SHUTDOWN,
    /** Error in the verification.*/
    ERROR
} VerifyState_t;

/** Hash list info block.*/
typedef struct {
    /** Payload offset. */
    uint64 PayloadOffset;
    /** Payload block info. */
    uint64 PayloadBlockSize;
    /** Hash (SHA256) calculated for the specified payload with
     * payload offset.*/
    bass_hash_t Hash;
} HashListEntry_t;

/** Context used in the verification of the packet.*/
struct _VerifyContext {
    /** Verification state.*/
    VerifyState_t State;
    /** Info returned from security.*/
    bass_signed_header_info_t headerinfo;
    /** Number of Blocks in the hashlist.*/
    uint32 NumberOfBlocks;
    /** pointer to the hash list.*/
    HashListEntry_t *HashList_p;
};

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/* Verification */
/**
 * This function initilizes a verification context.
 *
 * @param [in]  Context_p Pointer to a context.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS          The operation was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE   The operation was unsuccessful.
 */

LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Initialize(VerifyContext_t *Context_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;
    VerifyContext_t RealContext_p = NULL;

    B_(printf("Do_LoaderSecLib_Verify_Initialize\n");)

    *Context_p = LoaderSecLib_Allocate(sizeof(struct _VerifyContext));
    VERIFY((*Context_p != NULL), LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED);

    RealContext_p = (*Context_p);

    memset(RealContext_p, 0, sizeof(struct _VerifyContext));
    RealContext_p->State = INITIALIZED;
    ReturnValue = LOADER_SEC_LIB_SUCCESS;

    C_(PrintContext(RealContext_p);)

ErrorExit:
    return ReturnValue;
}

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
        uint32 HeaderBufferSize)
{
    LoaderSecLib_Output_t ReturnValue    = LOADER_SEC_LIB_VERIFY_FAILURE;
    VerifyContext_t       RealContext_p  = NULL;
#ifndef DISABLE_SECURITY
    bass_return_code      SecurityResult = BASS_RC_FAILURE;
#endif

    B_(printf("Do_LoaderSecLib_Verify_Header\n");)

    VERIFY((HeaderBuffer_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((Context_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    RealContext_p = (*Context_p);
    VERIFY((RealContext_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    C_(PrintContext(RealContext_p);)

    // check the Context
    VERIFY((INITIALIZED == RealContext_p->State), LOADER_SEC_LIB_VERIFY_FAILURE);
    RealContext_p->State = ERROR;

#ifndef DISABLE_SECURITY
    // verify the header
    SecurityResult = verify_signedheader(HeaderBuffer_p, HeaderBufferSize,
                                         BASS_PL_TYPE_FLASH_ARCHIVE,
                                         &(RealContext_p->headerinfo));
    VERIFY((BASS_RC_SUCCESS == SecurityResult), LOADER_SEC_LIB_VERIFY_FAILURE);
#endif

    // It must be SHA 256 aaccording to Security documentation
    VERIFY((BASS_APP_SHA256_HASH == RealContext_p->headerinfo.ht), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((BASS_PL_TYPE_FLASH_ARCHIVE == RealContext_p->headerinfo.plt), LOADER_SEC_LIB_VERIFY_FAILURE);

    RealContext_p->State = HEADER_VERIFIED;
    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}

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
        size_t BufferSize)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;
    VerifyContext_t RealContext_p = NULL;
    const uint8_t *Payload_p = NULL;
#ifndef DISABLE_SECURITY
    bass_return_code HashCalcResult = BASS_RC_FAILURE;
    bass_hash_t calculatedhash = {{0}};
#endif

    B_(printf("Do_LoaderSecLib_Verify_Hashlist\n");)

    VERIFY((HashlistBuffer_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((Context_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    RealContext_p = *Context_p;
    VERIFY((RealContext_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY(HEADER_VERIFIED == RealContext_p->State, LOADER_SEC_LIB_VERIFY_FAILURE);
    C_(PrintContext(RealContext_p);)

    VERIFY((BufferSize > RealContext_p->headerinfo.pl_size), LOADER_SEC_LIB_VERIFY_FAILURE);

    RealContext_p->State = ERROR;

    Payload_p = HashlistBuffer_p + (BufferSize - (RealContext_p->headerinfo.pl_size));

#ifndef DISABLE_SECURITY
    HashCalcResult = bass_calcdigest(BASS_APP_SHA256_HASH, Payload_p, RealContext_p->headerinfo.pl_size, &calculatedhash);
    B_(printf("Do_LoaderSecLib_Verify_Hashlist: bass_calcdigest result: %x\n", HashCalcResult);)

    VERIFY((BASS_RC_SUCCESS == HashCalcResult), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((0 == memcmp(&calculatedhash, &(RealContext_p->headerinfo.ehash), sizeof(bass_hash_t))), LOADER_SEC_LIB_VERIFY_FAILURE);
#else
    A_(printf("loader_sec_lib_verify(%d): Skipping hash verification!\n", __LINE__);)
#endif

    RealContext_p->State = HASHLIST_VERIFIED;

    ReturnValue = CopyHashList(RealContext_p, HashlistBuffer_p, BufferSize);
    VERIFY((LOADER_SEC_LIB_SUCCESS == ReturnValue), LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}

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
uint64            *HashBlockSize_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE;
    VerifyContext_t RealContext_p = NULL;

    B_(printf("Do_LoaderSecLib_Get_HashlistBlockSize\n");)

    VERIFY((HashBlockSize_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((Context_p != NULL), LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);
    RealContext_p = (*Context_p);
    VERIFY((RealContext_p != NULL), LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);
    VERIFY(HASHLIST_RETRIEVED == RealContext_p->State, LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);
    VERIFY(RealContext_p->NumberOfBlocks > 0, LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);

    *HashBlockSize_p = RealContext_p->HashList_p[0].PayloadBlockSize;

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}

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
uint8           *BlockHash_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;
    VerifyContext_t RealContext_p = NULL;
    uint64 BlockIndex = 0;

    B_(printf("Do_LoaderSecLib_Verify_HashListBlockHash\n");)
    VERIFY((BlockHash_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY((Context_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    RealContext_p = (*Context_p);
    VERIFY((RealContext_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY(HASHLIST_RETRIEVED == RealContext_p->State, LOADER_SEC_LIB_VERIFY_FAILURE);

    VERIFY((GetHashlistEntryIndex(Context_p, BlockOffset, &BlockIndex)), LOADER_SEC_LIB_VERIFY_FAILURE);

    RealContext_p->State = ERROR;

#ifndef DISABLE_SECURITY
    VERIFY((0 == memcmp(BlockHash_p, (RealContext_p->HashList_p[BlockIndex].Hash.value), HASHENTRY_HASHLENGTH)), LOADER_SEC_LIB_VERIFY_FAILURE);
#else
    A_(printf("loader_sec_lib_verify(%d): Skipping hash verification!\n", __LINE__);)
#endif

    RealContext_p->State = HASHLIST_RETRIEVED;
    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/**
 * This function releases all resources allocated for the context.
 *
 * @param [in]  Context_p Pointer to a context.
 *
 * @retval  LOADER_SEC_LIB_SUCCESS        The operation was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE The operation was unsuccessful.
 */
LoaderSecLib_Output_t Do_LoaderSecLib_Verify_Release(VerifyContext_t *Context_p)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_VERIFY_FAILURE;
    VerifyContext_t RealContext_p = NULL;

    B_(printf("Do_LoaderSecLib_Verify_Release\n");)

    VERIFY((Context_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    RealContext_p = (*Context_p);
    VERIFY((RealContext_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);

    RealContext_p->State = ERROR;

    ReturnValue = LoaderSecLib_Free((uint8 **) & (RealContext_p->HashList_p));
    VERIFY(LOADER_SEC_LIB_SUCCESS == ReturnValue, ReturnValue);
    RealContext_p->HashList_p = NULL;

    RealContext_p->State = SHUTDOWN;

    LoaderSecLib_Free((uint8 **)Context_p);
    RealContext_p = NULL;
    *Context_p = NULL;

    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    return ReturnValue;
}

/*******************************************************************************
 * Definition of internal functions
 ******************************************************************************/
/*
 * This function copies the complete hashlist from the hashlist file to internal list.
 *
 * @param [in]  Context_p    Pointer to a verification context.
 * @param [in]  HashlistBuffer_P Pointer to the haslist including header (complete file).
 * @param [in]  BufferSize Size of the buffer.
*
 * @retval  LOADER_SEC_LIB_SUCCESS        The block verification was successful.
 * @retval  LOADER_SEC_LIB_VERIFY_FAILURE   The block verification was
 *                                          unsuccessful.
 */
static LoaderSecLib_Output_t CopyHashList(VerifyContext_t Context_p,
const uint8 *const HashlistBuffer_p,
size_t BufferSize)
{
    LoaderSecLib_Output_t ReturnValue = LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE;
    uint64 NumberOfBlocks = 0;
    uint64 BlockIndex = 0;
    const uint8_t *Payload_p = NULL;

    B_(printf("CopyHashList\n");)

    VERIFY((HashlistBuffer_p != NULL), LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);
    VERIFY((Context_p != NULL), LOADER_SEC_LIB_VERIFY_FAILURE);
    VERIFY(HASHLIST_VERIFIED == Context_p->State, LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);
    VERIFY((BufferSize > Context_p->headerinfo.pl_size), LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);

    Context_p->State = ERROR;

    Payload_p = HashlistBuffer_p + (BufferSize - (Context_p->headerinfo.pl_size));

    NumberOfBlocks = (Context_p->headerinfo.pl_size) / HASHENTRYLENGTH;
    VERIFY(0 != NumberOfBlocks, LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);
    VERIFY(0 == (Context_p->headerinfo.pl_size) % HASHENTRYLENGTH, LOADER_SEC_LIB_HASH_LIST_HASH_FAILURE);

    Context_p->HashList_p = (HashListEntry_t *)LoaderSecLib_Allocate(NumberOfBlocks * sizeof(HashListEntry_t));
    VERIFY((Context_p->HashList_p != NULL), LOADER_SEC_LIB_MEMORY_ALLOCATION_FAILED);

    /* initialize the context with hash list info */
    for (BlockIndex = 0; BlockIndex < NumberOfBlocks; BlockIndex++) {
        memcpy(&(Context_p->HashList_p[BlockIndex].PayloadOffset), (void *)&Payload_p[BlockIndex *(HASHENTRYLENGTH)], HASHENTRY_OFFSETLENGTH);
        memcpy(&(Context_p->HashList_p[BlockIndex].PayloadBlockSize), (void *)&Payload_p[BlockIndex *(HASHENTRYLENGTH) + HASHENTRY_OFFSETLENGTH], HASHENTRY_BLOCKSIZELENGTH);
        memcpy(&(Context_p->HashList_p[BlockIndex].Hash), (void *)&Payload_p[BlockIndex *(HASHENTRYLENGTH) + HASHENTRY_OFFSETLENGTH + HASHENTRY_BLOCKSIZELENGTH], HASHENTRY_HASHLENGTH);
    }

    Context_p->NumberOfBlocks = NumberOfBlocks;
    Context_p->State = HASHLIST_RETRIEVED;
    ReturnValue = LOADER_SEC_LIB_SUCCESS;

ErrorExit:
    C_(printf("loader_sec_lib_verify (%d): ", __LINE__);)
    C_(PrintContext(Context_p);)
    return ReturnValue;
}

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
static boolean GetHashlistEntryIndex(VerifyContext_t *Context_p, uint64 BlockOffset, uint64 *EntryIndex_p)
{
    VerifyContext_t RealContext_p = NULL;
    uint64 BlockIndex;
    uint64 HashBlockSize = 0;
    boolean ReturnValue = FALSE;

    B_(printf("GetHashlistEntryIndex\n");)
    VERIFY((Context_p != NULL), FALSE);
    RealContext_p = (*Context_p);
    VERIFY((RealContext_p != NULL), FALSE);
    VERIFY(HASHLIST_RETRIEVED == (*Context_p)->State, FALSE);
    VERIFY((EntryIndex_p != NULL), FALSE);

    if ((Do_LoaderSecLib_Get_HashlistBlockSize(Context_p, &HashBlockSize) == LOADER_SEC_LIB_SUCCESS) && (HashBlockSize != 0)) {
        BlockIndex = BlockOffset / HashBlockSize;

        if (RealContext_p->HashList_p[BlockIndex].PayloadOffset == BlockOffset) {
            *EntryIndex_p = BlockIndex;
            ReturnValue = TRUE;
        }
    }

ErrorExit:
    return ReturnValue;
}

C_(static void PrintContext(VerifyContext_t Context_p)
{
    int i = 0;

    printf("PrintContext:\n");

    printf("VerifyContext State: %d\n", Context_p->State);
    printf("VerifyContext headerinfo: %d\n", Context_p->headerinfo);

    printf("VerifyContext     Payload_size: %d\n", Context_p->headerinfo.pl_size);
    printf("VerifyContext     Hash type: 0x%x\n", Context_p->headerinfo.ht);
    printf("VerifyContext     Payload type: 0x%x\n", Context_p->headerinfo.plt);
    printf("VerifyContext     ehash: ");

    for (i = 0; i < BASS_HASH_LENGTH; i++) {
        printf("0x%x ", Context_p->headerinfo.ehash.Value[i]);
    }

    printf("\n");

    printf("VerifyContext  NumberOfBlocks: %d\n", Context_p->NumberOfBlocks);
    printf("VerifyContext  HashList_p: %d\n", Context_p->HashList_p);
})
/** @} */

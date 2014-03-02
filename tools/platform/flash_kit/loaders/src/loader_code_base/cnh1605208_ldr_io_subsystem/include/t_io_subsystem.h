/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_IO_SUBSYSTEM_H_
#define _T_IO_SUBSYSTEM_H_

/**
 *  @addtogroup ldr_IO_subsystem
 *  @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_bulk_protocol.h"
#include "error_codes.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Variable that contain appropriate error code in case of unsuccessful IO operation */
extern ErrorCode_e IO_ErrorCode;

/** The definition of error value for IO layer */
#define IO_ERROR 0xffffffffffffffff

/** The definition of error value when creating an IO handle */
#define IO_INVALID_HANDLE 0xffffffff

/** The definition of error value when creating an IO vector */
#define IO_VECTOR_INVALID_HANDLE 0xffffffff

/** The definition of error value when cached data are destroyed in Secure Wrap */
#define IO_CACHED_DATA_DESTROY_FAIL 0xffffffff

/** The IO Handle type definition */
typedef uint32 IO_Handle_t;

/* The size of hash buffer */
#define HASH_SIZE 32

/** The IO Vector type definition */
typedef uint32 IO_Vector_t;

/** The maximal number of open IO channels */
#define MAX_OPEN_IO 16

/** The optimal number of buffers for bulk transfer PC->ME */
#define OPTIMAL_NR_OF_BUFFERS_PROCESS (uint64)5

/** The optimal number of buffers for bulk transfer ME->PC */
#define OPTIMAL_NR_OF_BUFFERS_DUMP (uint64)6

/** The size of the block that hash is calculated on
 * Must be power of 2 */
#define HASHED_BLOCK_SIZE 0x00040000//256kB

/**
 * Replaces the 'complete' boolean in the model.
 */
typedef enum {
    IOV_UNMAPPED, /**< There is no buffer backing this area. */
    IOV_MAPPED,   /**< There is buffer backing this area. */
    IOV_VALID,    /**< A read operation or a user has filled this
                    area with data. */
    IOV_DONE,     /**< A write operation or a user has handled
                    previously IOV_VALID data. */
} IO_VectorStatus_t;

/**
 *  Defines IO Vector parameters
 */
typedef struct {
    IO_VectorStatus_t Status;   /**< Status of the IO vector */
    void *Buffer_p;             /**< Buffer containing packet data */
    uint8 *Hash_p;              /**< Buffer containing Hash data */
} IO_VectorEntry_t;

/**
 *  Cached data single entry
 */
typedef struct {
    /**< Pointer to cached data */
    uint8 *Buffer_p;
} IO_CachedDataEntry_t;

/**
 *  Cached data structure
 */
typedef struct {
    /**< Length of cached data. */
    uint64 CachedDataLength;
    /**< Internal offset of requested data inside cached data */
    uint64 IOffset;
    /**< Size of the cached buffers */
    uint32 ChunkSize;
    /**< Number of buffers in cached data entry */
    uint32 Buffers;
    /**< Index of vector that cached data originally belong */
    uint32 ParentVector;
    /**< Array with information for used buffers. */
    IO_CachedDataEntry_t Entries[MAX_OPEN_IO];
    /**< Parameter that indicate when cached data are valid */
    boolean DataValid;
} VectorCachedData_t;

/**
 *  Defines IO Vector parameters
 */
typedef struct {
    /**< Vector status. */
    uint32 Status;
    /**< Session State */
    uint32 State;
    /**< Length of the data for transfer. */
    uint64 Length;
    /**< Internal offset of requested data inside received data */
    uint64 IOffset;
    /**< Size of the chunks to be transfered over bulk protocol */
    uint32 ChunkSize;
    /**< Pointer to transport layer vector instance */
    TL_BulkVectorList_t *BulkVector_p;
    /**< Define vector for bulk transfer */
    uint32 BulkTransfer;
    /**< Structure that hold cached data information (if any) */
    VectorCachedData_t CachedData;
    /**< Parameters for specific buffer inside the vector. */
    IO_VectorEntry_t Entries[MAX_OPEN_IO];
    /**< Parameter that indicate if Vector cannot be destroyed even
        if is requested by user. Anyway this flag should be cleared
        in case to force vector to be destroyed. */
    boolean VectorProtected;
} IO_VectorList_t;

/** List of initialized IO vectors */
extern IO_VectorList_t *VectorList;

/** @} */

#endif /*_T_IO_SUBSYSTEM_H_*/

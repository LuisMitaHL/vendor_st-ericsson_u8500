#ifndef _TEE_CLIENT_API_H
#define _TEE_CLIENT_API_H
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef OS_FREE
#include <stddef.h>
typedef unsigned char t_uint8;
typedef signed char t_sint8;
typedef unsigned short t_uint16;
typedef signed short t_sint16;
typedef unsigned long t_uint32;
typedef signed long t_sint32;
#else
#include <stdlib.h>
#include <stdint.h>
typedef uint8_t t_uint8;
typedef uint16_t t_uint16;
typedef uint32_t t_uint32;
#endif

/*
 * Defines the number of available memory references in an open session or
 * invoke command operation payload.
 */
#define TEEC_CONFIG_PAYLOAD_REF_COUNT 4

/*
 * Flag constants indicating which of the memory references in an open session
 * or invoke command operation payload (TEEC_Operation) that are used. Type is
 * t_uint32.
 */
#define TEEC_MEMREF_0_USED  0x00000001
#define TEEC_MEMREF_1_USED  0x00000002
#define TEEC_MEMREF_2_USED  0x00000004
#define TEEC_MEMREF_3_USED  0x00000008

/*
 * Flag constants indicating the data transfer direction of memory in
 * TEEC_SharedMemory and TEEC_MemoryReference. TEEC_MEM_INPUT signifies data
 * transfer direction from the client application to the TEE. TEEC_MEM_OUTPUT
 * signifies data transfer direction from the TEE to the client application.
 * Type is t_uint32.
 */
#define TEEC_MEM_INPUT  0x00000001
#define TEEC_MEM_OUTPUT 0x00000002

/*
 * Return values. Type is TEEC_Result
 *
 * TEEC_SUCCESS                 The operation was successful.
 * TEEC_ERROR_GENERIC           Non-specific cause.
 * TEEC_ERROR_ACCESS_DENIED     Access privileges are not sufficient.
 * TEEC_ERROR_CANCEL            The operation was cancelled.
 * TEEC_ERROR_ACCESS_CONFLICT   Concurrent accesses caused conflict.
 * TEEC_ERROR_EXCESS_DATA       Too much data for the requested operation was
 *                              passed.
 * TEEC_ERROR_BAD_FORMAT        Input data was of invalid format.
 * TEEC_ERROR_BAD_PARAMETERS    Input parameters were invalid.
 * TEEC_ERROR_BAD_STATE         Operation is not valid in the current state.
 * TEEC_ERROR_ITEM_NOT_FOUND    The requested data item is not found.
 * TEEC_ERROR_NOT_IMPLEMENTED   The requested operation should exist but is not
 *                              yet implemented.
 * TEEC_ERROR_NOT_SUPPORTED     The requested operation is valid but is not
 *                              supported in this implementation.
 * TEEC_ERROR_NO_DATA           Expected data was missing.
 * TEEC_ERROR_OUT_OF_MEMORY     System ran out of resources.
 * TEEC_ERROR_BUSY              The system is busy working on something else.
 * TEEC_ERROR_COMMUNICATION     Communication with a remote party failed.
 * TEEC_ERROR_SECURITY          A security fault was detected.
 * TEEC_ERROR_SHORT_BUFFER      The supplied buffer is too short for the
 *                              generated output.
 * TEEC_ERROR_FILE              The trusted application file was not found
 */

/* Standard defined error codes */
#define TEEC_SUCCESS                0x00000000
#define TEEC_ERROR_GENERIC          0xFFFF0000
#define TEEC_ERROR_ACCESS_DENIED    0xFFFF0001
#define TEEC_ERROR_CANCEL           0xFFFF0002
#define TEEC_ERROR_ACCESS_CONFLICT  0xFFFF0003
#define TEEC_ERROR_EXCESS_DATA      0xFFFF0004
#define TEEC_ERROR_BAD_FORMAT       0xFFFF0005
#define TEEC_ERROR_BAD_PARAMETERS   0xFFFF0006
#define TEEC_ERROR_BAD_STATE        0xFFFF0007
#define TEEC_ERROR_ITEM_NOT_FOUND   0xFFFF0008
#define TEEC_ERROR_NOT_IMPLEMENTED  0xFFFF0009
#define TEEC_ERROR_NOT_SUPPORTED    0xFFFF000A
#define TEEC_ERROR_NO_DATA          0xFFFF000B
#define TEEC_ERROR_OUT_OF_MEMORY    0xFFFF000C
#define TEEC_ERROR_BUSY             0xFFFF000D
#define TEEC_ERROR_COMMUNICATION    0xFFFF000E
#define TEEC_ERROR_SECURITY         0xFFFF000F
#define TEEC_ERROR_SHORT_BUFFER     0xFFFF0010
/* Implementation defined error codes below*/
#define TEEC_ERROR_FILE     0xFFFFE000

/*
 * Function error origins, of type TEEC_ErrorOrigin. These indicate where in
 * the software stack a particular return value originates from.
 *
 * TEEC_ORIGIN_API          The error originated within the TEE Client API
 *                          implementation.
 * TEEC_ORIGIN_COMMS        The error originated within the underlying
 *                          communications stack linking the rich OS with
 *                          the TEE.
 * TEEC_ORIGIN_TEE          The error originated within the common TEE code.
 * TEEC_ORIGIN_TRUSTED_APP  The error originated within the Trusted Application
 *                          code.
 */
#define TEEC_ORIGIN_API         0x00000001
#define TEEC_ORIGIN_COMMS       0x00000002
#define TEEC_ORIGIN_TEE         0x00000003
#define TEEC_ORIGIN_TRUSTED_APP 0x00000004

/*
 * Session login methods, for use in TEEC_OpenSession() as parameter
 * connectionMethod. Type is t_uint32.
 *
 * TEEC_LOGIN_PUBLIC  No login data is provided.
 */
#define TEEC_LOGIN_PUBLIC 0x0

typedef t_uint32 TEEC_Result;
typedef t_uint32 TEEC_ErrorOrigin;

/**
 * This type contains a Universally Unique Resource Identifier (UUID) type as
 * defined in RFC4122. These UUID values are used to identify Trusted
 * Applications.
 */
typedef struct {
    t_uint32 timeLow;
    t_uint16 timeMid;
    t_uint16 timeHiAndVersion;
    t_uint8  clockSeqAndNode[8];
} TEEC_UUID;

/**
 * struct TEEC_SharedMemory - Memory to transfer data between a client
 * application and trusted code.
 * @buffer  The memory buffer which is to be, or has been, shared with the TEE.
 * @size    The size, in bytes, of the memory buffer.
 * @flags   Bit-vector which holds properties of buffer. The bit-vector can
 *          contain either or both of the TEEC_MEM_INPUT and TEEC_MEM_OUTPUT
 *          flags.
 *
 * A shared memory block is a region of memory allocated in the context of the
 * client application memory space that can be used to transfer data between
 * that client application and a trusted application. The user of this struct
 * is responsible to populate the buffer pointer.
 */
typedef struct {
    void    *buffer;
    size_t   size;
    t_uint32 flags;
} TEEC_SharedMemory;

/**
 * struct TEEC_Operation - Holds information and memory references used in
 * TEEC_InvokeCommand().
 * @memRefs Array of memory references used to transfer data related to the
 *          operation.
 * @flags   Bit-vector which holds information about which of the memory
 *          references in memRefs are used. Valid flags are TEEC_MEMREF_0_USED,
 *          TEEC_MEMREF_1_USED, TEEC_MEMREF_2_USED and TEEC_MEMREF_3_USED.
 *
 */
typedef struct {
    TEEC_SharedMemory     memRefs[TEEC_CONFIG_PAYLOAD_REF_COUNT];
    t_uint32              flags;
} TEEC_Operation;

/**
 * struct TEEC_Context - Represents a connection between a client application
 * and a TEE.
 */
typedef t_uint32 TEEC_Context;

/**
 * struct TEEC_Session - Represents a connection between a client application
 * and a trusted application.
 */
typedef struct {
    t_uint32 id;
    TEEC_Context ctx;
    TEEC_UUID uuid;
    void *ta;
} TEEC_Session;

/**
 * TEEC_InitializeContext() - Initializes a context holding connection
 * information on the specific TEE.
 * @name    A zero-terminated string identifying the TEE to connect to. If name
 *          is set to NULL, the default TEE is connected to. NULL is the only
 *          supported value in this version of the API implementation.
 * @context The context structure which is to be initialized.
 *
 * Initializes a context holding connection information between the calling
 * client application and the TEE designated by the name string.
 */
TEEC_Result TEEC_InitializeContext(
    const char   *name,
    TEEC_Context *context);

/**
 * TEEC_FinalizeContext() - Destroys a context holding connection information
 * on the specific TEE.
 * @context The context to be destroyed.
 *
 * This function destroys an initialized TEE context, closing the connection
 * between the client application and the TEE. This function must only be
 * called when all sessions related to this TEE context have been closed and
 * all shared memory blocks have been released, otherwise an error will be
 * returned.
 */
TEEC_Result TEEC_FinalizeContext(
    TEEC_Context *context);

/**
 * TEEC_OpenSession() - Opens a new session with the specified trusted
 * application.
 * @ context            The initialized TEE context structure in which scope to
 *                      open the session.
 * @ session            The session to initialize.
 * @ destination        A structure identifying the trusted application with
 *                      which to open a session. If this is set to NULL the
 *                      operation TEEC_MEMREF_0 is expected to contain the blob
 *                      which holds the Trusted Application.
 * @ connectionMethod   The connection method to use.
 * @ connectionData     Any data necessary to connect with the chosen
 *                      connection method. Not supported should be set to NULL.
 * @ operation          An operation structure to use in the session. May be
 *                      set to NULL to signify no operation structure needed.
 *                      If destination is set to NULL, TEEC_MEMREF_0 is
 *                      expected to hold the TA binary as described above.
 * @ errorOrigin        A parameter which will hold the error origin if this
 *                      function returns any value other than TEEC_SUCCESS.
 *
 * Opens a new session with the specified trusted application. Only
 * connectionMethod == TEEC_LOGIN_PUBLIC is supported. connectionData and
 * operation shall be set to NULL.
 */
TEEC_Result TEEC_OpenSession(
    TEEC_Context     *context,
    TEEC_Session     *session,
    const TEEC_UUID  *destination,
    t_uint32          connectionMethod,
    void             *connectionData,
    TEEC_Operation   *operation,
    TEEC_ErrorOrigin *errorOrigin);

/**
 * TEEC_CloseSession() - Closes the session which has been opened with the
 * specific trusted application.
 * @session The opened session to close.
 *
 * Closes the session which has been opened with the specific trusted
 * application.
 */
TEEC_Result TEEC_CloseSession(
    TEEC_Session *session);

/**
 * TEEC_InvokeCommand() - Executes a command in the specified trusted
 * application.
 * @destination A structure identifying the trusted application.
 * @commandID   Identifier of the command in the trusted application to invoke.
 * @operation   An operation structure to use in the invoke command. May be set
 *              to NULL to signify no operation structure needed.
 * @errorOrigin A parameter which will hold the error origin if this function
 *              returns any value other than TEEC_SUCCESS.
 *
 * Executes a command in the specified trusted application.
 */
TEEC_Result TEEC_InvokeCommand(
    TEEC_Session     *session,
    t_uint32          commandID,
    TEEC_Operation   *operation,
    TEEC_ErrorOrigin *errorOrigin);

/**
 * TEEC_AllocateSharedMemory() - Allocate shared memory for TEE.
 * @ context            The initialized TEE context structure in which scope to
 *                      open the session.
 * @ sharedMemory       pointer to the allocated shared memory.
 */
TEEC_Result TEEC_AllocateSharedMemory(TEEC_Context *context,
                                      TEEC_SharedMemory *sharedMemory);

/**
 * TEEC_ReleaseSharedMemory() - Free the shared memory.
 * @ sharedMemory       pointer to the shared memory to be freed.
 */
void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *sharedMemory);

#endif


/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_SECURITY_ALGORITHMS_H_
#define _R_SECURITY_ALGORITHMS_H_
/**
 * \addtogroup ldr_security_algorithms Security algorithms
 * This module include functions for hash device initialization,shutdown and
 * crypto calculation. As a function for calculation sholuld use the function
 * from ESB block.
 *
 * @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_security_algorithms.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Hash device initialization.
 *
 * @param [in]  Object_p  Initialized buffer context.
 *
 * retval Pointer Pointer to the initialized hash device descriptor.
 *
 */
HashDevice_t *Do_Hash_DeviceInit(void *Object_p);

/**
 * Hash device shutdown.
 *
 * @param [in]  Object_p       Initialized buffer context.
 * @param [in]  HashDevice_pp  Pointer to hash device descriptor.
 *
 * @return none.
 */
void Do_Hash_DeviceShutdown(void *Object_p, void **HashDevice_pp);

/*
 * Asynchronous hashing of data.
 *
 * Calculates a hash over the specified block of data, calling the call-
 * back function on completion. Note that the callback function might
 * execute in interrupt context, so due care should be taken when
 * writing it.
 *
 * The hashing might be delayed if another operation is currently
 * running.
 *
 * @param [in]  Object_p  Initialized buffer context.
 * @param [in]  Type      Hash type.
 * @param [in]  Data_p    Start of the block of data to hash.
 * @param [in]  Length    The length in bytes of the block of data to hash.
 * @param [out] Hash_p    Start of the buffer that should hold the hash
 *                                    value.
 * @param [in]  Callback  The function to call when the hash calculation
 *                                    completes.
 * @param [in,out] Param_p The value to use as parameter to Param_p.
 *
 *  @note Hash_p must be large enough to contain the selected hash type.
 *  Use the "HASH"_LENGTH macros when you know beforehand which hash
 *  type to use, or Do_Crypto_GetHashLength() for dynamically selected
 *  hashes.
 *
 */
void Do_Crypto_AsynchronousHash(void *Object_p, HashType_e Type, void *Data_p,
                                uint32 Length, uint8 *Hash_p,
                                HashCallback_fn Callback,
                                void *Param_p);

/**
 * Get the length of specified hash type.
 *
 * @param [in]  Object_p  Initialized buffer context.
 * @param [in]  Type      The hash type.
 *
 * @retval Lenth      The length in bytes of the hashes created by the specified
 *                    hashing function.
 * @retval HASH_ERROR if an invalid parameter was specified.
 */
uint32 Do_Crypto_GetHashLength(void *Object_p, HashType_e Type);

/** @} */
#endif /*_R_SECURITY_ALGORITHMS_H_*/

/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/
/**
 * \addtogroup ldr_security_algorithms
 * @{
 */

/*************************************************************************
* Includes
*************************************************************************/
#include "r_security_algorithms.h"
#include "r_basicdefinitions.h"
#include <stdlib.h>
#include <stdio.h>
#include "r_debug_macro.h"
#include <string.h>
#include "r_memory_utils.h"
#include "bass_app.h"

/***********************************************************************
 * Declaration of file local functions
 **********************************************************************/

static uint8 hashxor(const uint8 *Buf_p, uint32 Length);
static uint16 crc16(const uint8 *p, int count, uint16 crc_in);
/***********************************************************************
 * Definition of external functions
 **********************************************************************/
const uint32 red[16] = {
    0,
    (0x1 << 16) ^(0x1021 << 0), \
    (0x2 << 16) ^(0x1021 << 1), \
    (0x3 << 16) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0x4 << 16) ^(0x1021 << 2), \
    (0x5 << 16) ^(0x1021 << 2) ^(0x1021 << 0), \
    (0x6 << 16) ^(0x1021 << 2) ^(0x1021 << 1), \
    (0x7 << 16) ^(0x1021 << 2) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0x8 << 16) ^(0x1021 << 3), \
    (0x9 << 16) ^(0x1021 << 3) ^(0x1021 << 0), \
    (0xa << 16) ^(0x1021 << 3) ^(0x1021 << 1), \
    (0xb << 16) ^(0x1021 << 3) ^(0x1021 << 1) ^(0x1021 << 0), \
    (0xc << 16) ^(0x1021 << 3) ^(0x1021 << 2), \
    (0xd << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 0), \
    (0xe << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 1), \
    (0xf << 16) ^(0x1021 << 3) ^(0x1021 << 2) ^(0x1021 << 1) ^(0x1021 << 0)
};

/*
 * Hash device initialization.
 *
 * @param [in]  Object_p  Initialized buffer context.
 *
 * retval Pointer Pointer to the initialized hash device descriptor.
 */
HashDevice_t *Do_Hash_DeviceInit(void *Object_p)
{
    HashDevice_t *HashDevice_p = (HashDevice_t *) malloc(sizeof(HashDevice_t));

    ASSERT(NULL != HashDevice_p);

    HashDevice_p->Calculate = (DeviceCalculate_fn)Do_Crypto_AsynchronousHash;
    HashDevice_p->Cancel = Do_Hash_DeviceShutdown;

    IDENTIFIER_NOT_USED(Object_p);
    return HashDevice_p;
}

/*
 * Hash device shutdown.
 *
 * @param [in]  Object_p       Initialized buffer context.
 * @param [in]  HashDevice_pp  Pointer to hash device descriptor.
 *
 * @return none.
 */
void Do_Hash_DeviceShutdown(void *Object_p, void **HashDevice_pp)
{
    IDENTIFIER_NOT_USED(Object_p);
    ASSERT(NULL != HashDevice_pp);
    BUFFER_FREE(*HashDevice_pp);

    //  Do_Fifo_Destroy(NULL, &HashContextQueue_p);
}

/**
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
 *                        value.
 * @param [in]  Callback  The function to call when the hash calculation
 *                        completes.
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
                                void *Param_p)
{
    if (Type == HASH_SHA256) {
        memset(Hash_p, 0, SHA256_LENGTH);

        if (Length != 0) {
#ifdef _ONLY_HASHXOR
            Hash_p[0] = hashxor((const uint8 *)Data_p, Length); // TODO: should be changed when will be integrated ESB block!
#else

#ifndef SKIP_PAYLOAD_VERIFICATION
            bass_hash_t Hash;
            bass_return_code HashCalcResult = BASS_RC_ERROR_UNKNOWN;

            C_(printf("security_algorithms.c(%d): Call calcdigest Len %d\n", __LINE__, Length);)

            HashCalcResult = bass_calcdigest(BASS_APP_SHA256_HASH, Data_p, Length, &Hash);

            C_(printf("Result: 0x%x\n", HashCalcResult);)

            if (BASS_RC_SUCCESS == HashCalcResult) {
                memcpy(Hash_p, Hash.value, SHA256_LENGTH);
            } else {
                A_(printf("security_algorithms.c(%d): calcdigest failed!\n");)
            }

#endif

#endif
        }
    } else if (Type == HASH_CRC16) {
        uint16 CRC16;
        memset(Hash_p, 0, CRC16_LENGTH);

        if (Length != 0) {
            CRC16 = crc16((const uint8 *)Data_p, Length, 0);
            memcpy(Hash_p, &CRC16, sizeof(uint16));
        }
    } else if (Type == HASH_SIMPLE_XOR) {
        Hash_p[0] = hashxor((const uint8 *)Data_p, Length);
    }

    if (Callback != NULL) {
        Callback(Data_p, Length, Hash_p, Param_p);
    }

    IDENTIFIER_NOT_USED(Object_p);
}

/*
 * Get the length of specified hash type.
 *
 * @param [in]  Object_p  Initialized buffer context.
 * @param [in]  Type      The hash type.
 *
 * @retval Lenth      The length in bytes of the hashes created by the specified
 *                    hashing function.
 * @retval HASH_ERROR if an invalid parameter was specified.
 */
uint32 Do_Crypto_GetHashLength(void *Object_p, HashType_e Type)
{
    IDENTIFIER_NOT_USED(Object_p);

    if (Type == HASH_SHA256) {
        return SHA256_LENGTH;
    }

    if (Type == HASH_CRC16) {
        return CRC16_LENGTH;
    }

    return HASH_ERROR;
}

/***********************************************************************
 * Definition of internal functions
 **********************************************************************/

/*
 * This function is used for software emulation of XOR hashing
 * When hardware blocks will be used for hashing this function will be removed
 *
 * */

static uint8 hashxor(const uint8 *Buf_p, uint32 Length)
{
    uint8 Value = 0;
    const uint8 *Stop_p = Buf_p + Length;

    do {
        Value ^= *Buf_p++;
    } while (Buf_p < Stop_p);

    return Value;
}

static uint16 crc16(const uint8 *p, int count, uint16 crc_in)
{
    register uint32 crc = crc_in;

    while (--count >= 0) {
        crc = crc ^(uint8) * p++ << 8;
        crc ^= red[crc >> 16];
        crc = (crc << 4);
        crc ^= red[crc >> 16];
        crc = (crc << 4);
        crc ^= red[crc >> 16];

    }

    return (uint16) crc;
}
/** @} */


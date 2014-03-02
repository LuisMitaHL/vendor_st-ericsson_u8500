/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/

#include <stdlib.h>

#include <cops_common.h>
#include <cops_sipc.h>
#include <cops_shared.h>
#include <string.h>

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

/* Normal right shift operation. */
#define SHR(w, n) (w>>n)

/* The rotate right (circular right shift) operation,
   4 bytes per word according to SHA-256 spec. */
#define ROTR(w, n) ((w>>n) | (w << (32-n)))

/* Logical crypto function (NOTE for this function xor (^) can,
   according to spec, be replaced by bitor (|)) */
#define F1(x, y, z) ((x&y) | ((~x)&z))

/* Logical crypto function (NOTE for this function xor (^) can,
   according to spec, be replaced by bitor (|)) */
#define F2(x, y, z) ((x&y)|(x&z)|(y&z))

/* Sum_0 */
#define F3(w) (ROTR(w, 2) ^ ROTR(w, 13) ^ ROTR(w, 22))

/* Sum_1 */
#define F4(w) (ROTR(w, 6) ^ ROTR(w, 11) ^ ROTR(w, 25))

/* Sigma_0 */
#define F5(w) (ROTR(w, 7) ^ ROTR(w, 18) ^ SHR(w, 3))

/* Sigma_1 */
#define F6(w) (ROTR(w, 17) ^ ROTR(w, 19) ^ SHR(w, 10))

/* Hash size */
#define COPS_SHA256_SIZE (32)

/* HMAC block length */
#define COPS_HMAC_BLOCK_LENGTH (64)

/**
 * Dummy secret key.
 * Will be replaced with a proper key when available.
 */
static const char secret_key[64] =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-$";

/**
 * Size of secret key
 */
static const uint32_t secret_key_size = sizeof(secret_key);

/**
 * The initial hash value H(0), must be set.
 * The size and number of words in H(0) depends on the message digest size.
 * These words were obtained by taking the first thirty-two bits of the
 * fractional parts of the square roots of the first eight prime numbers.
 */
static const uint32_t vInit[8] = {
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
};

/**
 * The K values for SHA-256.
 * These words represent the first thirty-two bits of the fractional parts of
 * the cube roots of the first sixtyfour prime numbers.
 */
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/**
 * Context for sha256 calculations
 */
typedef struct {
    uint32_t v[8]; /* 8 words per round */

    struct {
      uint32_t hi;
      uint32_t lo;
    } n;

    uint8_t  buf[64];
    uint8_t  ncached;
} cops_sha256_context_t;

/**
 * Context for hmac calculations
 */
typedef struct {
    cops_sha256_context_t   *hash_context_p;
    cops_sha256_context_t   *inner_hash_context_p;
    cops_sha256_context_t   *outer_hash_context_p;
    uint32_t                digest_length;
} cops_hmac_context_t;

static cops_return_code_t cops_local_hmac_init(
                            cops_hmac_context_t **hmac_context_pp);

static cops_return_code_t cops_local_hmac_update(
                              cops_hmac_context_t *hmac_context_p,
                              const uint8_t *const data_p,
                              const uint32_t data_length);

static cops_return_code_t cops_local_hmac_final(
                            cops_hmac_context_t *hmac_context_p,
                            size_t hmac_len,
                            uint8_t *const hmac_p);

static cops_return_code_t cops_sha256_init(
                                     cops_sha256_context_t **sha256_context_pp);

static cops_return_code_t cops_sha256_update(
                                  cops_sha256_context_t *const sha256_context_p,
                                  const uint8_t  *const data_p,
                                  const uint32_t data_len);

static cops_return_code_t cops_sha256_final(
                                     cops_sha256_context_t **sha256_context_pp,
                                     struct cops_digest *hash_p);

static void cops_context_cleanup(cops_hmac_context_t **hmac_context_pp);

static void cops_byteswap(void *buf_p, uint32_t ul_num_bytes);

static void cops_sha256_process_block(
                                    cops_sha256_context_t *const sha256_ctx_p);

cops_return_code_t
cops_mac_calc(cops_mac_type_t mac_type, const uint8_t *data, size_t data_len,
              uint8_t *mac, size_t mac_len)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_hmac_context_t *hmac_context_p = NULL;

    /* We only support SIPC type */
    COPS_DEBUG_CHK_ASSERTION(COPS_MAC_TYPE_SIPC == mac_type);
    COPS_DEBUG_CHK_ASSERTION(NULL != data);
    COPS_DEBUG_CHK_ASSERTION(NULL != mac);

    COPS_CHK_RC(cops_local_hmac_init(&hmac_context_p));

    COPS_CHK_RC(cops_local_hmac_update(hmac_context_p,
                                       data,
                                       data_len));

    COPS_CHK_RC(cops_local_hmac_final(hmac_context_p,
                                      mac_len,
                                      mac));
function_exit:
    cops_context_cleanup(&hmac_context_p);
    return ret_code;
}

cops_return_code_t
cops_mac_verify(cops_mac_type_t mac_type, const uint8_t *data, size_t data_len,
                const uint8_t *mac, size_t mac_len, bool *mac_ok)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint8_t calculated_mac[COPS_SIPC_MAC_SIZE];

    /* We only support SIPC type */
    COPS_DEBUG_CHK_ASSERTION(COPS_MAC_TYPE_SIPC == mac_type);
    COPS_DEBUG_CHK_ASSERTION(NULL != data);
    COPS_DEBUG_CHK_ASSERTION(NULL != mac);
    COPS_DEBUG_CHK_ASSERTION(NULL != mac_ok);

    /* Init data */
    memset((void *)calculated_mac, 0, COPS_SIPC_MAC_SIZE);

    /* Calculate MAC for supplied data */
    COPS_CHK_RC(cops_mac_calc(mac_type,
                              (uint8_t *)data,
                              data_len,
                              calculated_mac,
                              COPS_SIPC_MAC_SIZE));

    /* Compare the MAC:s */
    *mac_ok = (bool)(mac_len == (size_t)COPS_SIPC_MAC_SIZE);

    if (*mac_ok) {
        *mac_ok = (memcmp((const void *)mac,
                          (const void *)calculated_mac,
                          mac_len) == 0);
    }

function_exit:
    return ret_code;
}

/**
 * Initiates the mac calculation.
 */


static cops_return_code_t
cops_local_hmac_init(cops_hmac_context_t **hmac_context_pp)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    struct cops_digest hash;
    uint8_t key[COPS_HMAC_BLOCK_LENGTH];
    uint8_t pad[COPS_HMAC_BLOCK_LENGTH];
    uint8_t i;

    if (NULL == hmac_context_pp  || NULL != *hmac_context_pp) {
        return COPS_RC_ARGUMENT_ERROR;
    }

    *hmac_context_pp = malloc(sizeof(cops_hmac_context_t));
    COPS_CHK_ALLOC(*hmac_context_pp);

    /* Init variables */
    memset((void *)(*hmac_context_pp), 0, sizeof(cops_hmac_context_t));
    (*hmac_context_pp)->digest_length = (uint32_t)COPS_HASH_LENGTH_SHA256;

    /* If the key is longer than block_length then digest the key */
    if (secret_key_size > (uint32_t)COPS_HMAC_BLOCK_LENGTH) {
        COPS_CHK_RC(cops_sha256_init(&((*hmac_context_pp)->hash_context_p)));

        COPS_CHK_RC(cops_sha256_update((*hmac_context_pp)->hash_context_p,
                                       (uint8_t *)secret_key,
                                       secret_key_size));

        COPS_CHK_RC(cops_sha256_final(&((*hmac_context_pp)->hash_context_p),
                                      &hash));

        memcpy((void *)key,
               (const void *)hash.value,
               (*hmac_context_pp)->digest_length);

        /* Unused part of key should be set to zeroes */
        memset((void *)&(key[(*hmac_context_pp)->digest_length]),
               0,
              (size_t)(COPS_HMAC_BLOCK_LENGTH -
                  (*hmac_context_pp)->digest_length));
    } else {
        memcpy((void *)key,
               (const void *)secret_key,
               secret_key_size);

        /* Unused part of key should be set to zeroes */
        if (secret_key_size < (uint32_t)COPS_HMAC_BLOCK_LENGTH) {
            memset((void *)&(key[secret_key_size]),
                   0,
                   (size_t)(COPS_HMAC_BLOCK_LENGTH - secret_key_size));
        }
    }

    /* Pad with the following according to standard */
    for (i = 0; i < (uint8_t)COPS_HMAC_BLOCK_LENGTH; i++) {
        pad[i] = 0x36 ^ key[i];
    }

    COPS_CHK_RC(cops_sha256_init(&((*hmac_context_pp)->inner_hash_context_p)));

    COPS_CHK_RC(cops_sha256_update((*hmac_context_pp)->inner_hash_context_p,
                                   pad,
                                   (uint32_t)COPS_HMAC_BLOCK_LENGTH));

    /* Pad with the following according to standard */
    for (i = 0; i < (uint8_t)COPS_HMAC_BLOCK_LENGTH; i++) {
        pad[i] = 0x5C ^ key[i];
    }

    COPS_CHK_RC(cops_sha256_init(&((*hmac_context_pp)->outer_hash_context_p)));

    COPS_CHK_RC(cops_sha256_update((*hmac_context_pp)->outer_hash_context_p,
                                   pad,
                                   (uint32_t)COPS_HMAC_BLOCK_LENGTH));

function_exit:
    if (COPS_RC_OK != ret_code) {
        /* Cleanup */
        cops_context_cleanup(hmac_context_pp);
    }

    return ret_code;
}

/**
 * Updates the mac calculation data.
 */
static cops_return_code_t
cops_local_hmac_update(cops_hmac_context_t *hmac_context_p,
                       const uint8_t *const data_p, const uint32_t data_length)
{
    return cops_sha256_update(hmac_context_p->inner_hash_context_p,
                              data_p,
                              data_length);
}

/**
 * Calculates and returns the mac.
 */
static cops_return_code_t
cops_local_hmac_final(cops_hmac_context_t *hmac_context_p,
                      size_t hmac_len, uint8_t *const hmac_p)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    struct cops_digest intermediate_digest;

    COPS_CHK_RC(cops_sha256_final(&(hmac_context_p->inner_hash_context_p),
                                  &intermediate_digest));

    COPS_CHK_RC(cops_sha256_update(hmac_context_p->outer_hash_context_p,
                                   intermediate_digest.value,
                                   (uint32_t)hmac_context_p->digest_length));

    COPS_CHK_RC(cops_sha256_final(&(hmac_context_p->outer_hash_context_p),
                                  &intermediate_digest));

    memcpy((void *)hmac_p, (const void *)intermediate_digest.value, hmac_len);

function_exit:
    return ret_code;
}

/**
 * Initiates the hash calculation.
 */
static cops_return_code_t
cops_sha256_init(cops_sha256_context_t **sha256_context_pp)
{
    cops_return_code_t ret_code = COPS_RC_OK;

    if (NULL == sha256_context_pp  || NULL != *sha256_context_pp) {
        return COPS_RC_ARGUMENT_ERROR;
    }

    /* Create sha256 context */
    *sha256_context_pp = (void *)malloc(sizeof(cops_sha256_context_t));
    COPS_CHK_ALLOC(*sha256_context_pp);

    /* Init sha256 context data */
    memcpy((void *)(*sha256_context_pp)->v, (const void *)vInit, sizeof(vInit));

    /* 2^64 max length, hence two 4-byte words needed */
    (*sha256_context_pp)->n.lo = 0;
    (*sha256_context_pp)->n.hi = 0;
    (*sha256_context_pp)->ncached = 0;

function_exit:
    return ret_code;
}

/**
 * Updates the hash calculation data.
 */
static cops_return_code_t
cops_sha256_update(cops_sha256_context_t *const sha256_context_p,
                   const uint8_t *const data_p, const uint32_t data_len)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    uint32_t n;
    uint32_t old;
    uint32_t remain_data_len = data_len;
    const uint8_t  *var_data_p = NULL;

    if (NULL == sha256_context_p  || NULL == data_p) {
        return COPS_RC_ARGUMENT_ERROR;
    }

    var_data_p = data_p;
    old = sha256_context_p->n.lo;

    /* Count bits, not chars, hence multiply by 2^3 */
    sha256_context_p->n.lo += (data_len << 3);

    /* There was an overflow */
    if (sha256_context_p->n.lo < old) {
        /* So carry into high order word */
        ++sha256_context_p->n.hi;
    }

    /* Shift 32-3 = 29 since it is counted in bits */
    sha256_context_p->n.hi += (data_len >> 29);

    /* While there are complete blocks left */
    while (remain_data_len > 0) {
        /* n = how much to fill in this block */
        n = MIN(remain_data_len, (uint32_t)(64 - sha256_context_p->ncached));

        /* Fill up block */
        memcpy((void *)&sha256_context_p->buf[sha256_context_p->ncached],
               (const void *)var_data_p,
               n);

        sha256_context_p->ncached += (uint8_t)n;

        /* Is this block full? */
        if (sha256_context_p->ncached  == 64) {
            /* Throughout the SHA specification, the big-endian convention
               is used when expressing both 32- and 64-bit words, so that
               within each word, the most significant bit is stored in the
               left-most bit position. We must however swap the bytes in ARM
               since little endian is used and, at the end of all blocks, swap
               back to big endian when returning the result. Swap the endian
               format for the 64 bytes. */
            cops_byteswap(sha256_context_p->buf, 64);

            /* Then hash this block */
            cops_sha256_process_block(sha256_context_p);

            /* Reset cache */
            sha256_context_p->ncached = 0;
        }

        remain_data_len -= n;
        var_data_p += n;
    }

    return ret_code;
}

/**
 * Calculates and returns the hash.
 */
static cops_return_code_t
cops_sha256_final(cops_sha256_context_t **sha256_context_pp,
                  struct cops_digest *hash_p)
{
    cops_return_code_t ret_code = COPS_RC_OK;
    cops_sha256_context_t *sha256_ctx_p = NULL;
    uint32_t *long_digest_p = NULL;

    if (NULL == sha256_context_pp
        || NULL == *sha256_context_pp
        || NULL == hash_p) {
        return COPS_RC_ARGUMENT_ERROR;
    }

    sha256_ctx_p = (cops_sha256_context_t *)*sha256_context_pp;

    memset((void *)hash_p, 0, sizeof(hash_p->value));

    long_digest_p = (uint32_t *)malloc(COPS_SHA256_SIZE);
    COPS_CHK_ALLOC(long_digest_p);

    /* A 1 bit must always be added */
    sha256_ctx_p->buf[sha256_ctx_p->ncached++] = (uint8_t)0x80;

    /* Is there room to pad with length */
    if (sha256_ctx_p->ncached > 56) {
        /* No, then pad with zeros */
        memset((void *)&sha256_ctx_p->buf[sha256_ctx_p->ncached],
               0,
               (size_t)(64 - sha256_ctx_p->ncached));

        /* Throughout the SHA specification, the big-endian convention is used
           when expressing both 32- and 64-bit words, so that within each word,
           the most significant bit is stored in the left-most bit position.
           We must however swap the bytes in ARM since little endian is used
           and, at the end of all blocks, swap back to big endian when
           returning the result. Swap the endian format for the 64 bytes. */
        cops_byteswap(sha256_ctx_p->buf, 64);

        /* And hash block */
        cops_sha256_process_block(sha256_ctx_p);

        /* Pad 56 zeroes */
        memset((void *)&sha256_ctx_p->buf[0], 0, 56);
    } else {
        /* Yes, pad first with zeros */
        memset((void *)&sha256_ctx_p->buf[sha256_ctx_p->ncached],
               0,
               (size_t)(56 - sha256_ctx_p->ncached));
    }

    /* Throughout the SHA specification, the big-endian convention is used
       when expressing both 32- and 64-bit words, so that within each word,
       the most significant bit is stored in the left-most bit position.
       We must however swap the bytes in ARM since little endian is used and,
       at the end of all blocks, swap back to big endian when returning the
       result. Swap the endian format for the first 56 bytes. */
    cops_byteswap(sha256_ctx_p->buf, 56);

    /* Pad last 8 bytes with the length in bits (These bytes
       are already in little endian, so no swap is needed)*/
    memcpy((void *)&sha256_ctx_p->buf[56],
           (const void *)&sha256_ctx_p->n,
           8);

    /* And hash the last block */
    cops_sha256_process_block(sha256_ctx_p);

    memcpy((void *)long_digest_p,
           (const void *)sha256_ctx_p->v,
           sizeof(sha256_ctx_p->v));

    /* Return digest in big-endian order by swapping back to big endian when
       returning the result. It has temporarly been in little endian format
       for the ARM. */
    cops_byteswap(long_digest_p, COPS_SHA256_SIZE);

    memcpy((void *)hash_p->value,
           (const void *)long_digest_p,
           COPS_SHA256_SIZE);

function_exit:
    if (long_digest_p != NULL) {
        free(long_digest_p);
    }

    /* Free context memory */
    if (sha256_context_pp != NULL && *sha256_context_pp != NULL) {
        free(*sha256_context_pp);
        *sha256_context_pp = NULL;
    }

    return ret_code;
}

/**
 * Makes sure dynamically allocated memory is freed for
 * the hmac context.
 */
static void
cops_context_cleanup(cops_hmac_context_t **hmac_context_pp)
{
    if (NULL != hmac_context_pp && NULL != *hmac_context_pp) {
        if (NULL != (*hmac_context_pp)->hash_context_p) {
            free((*hmac_context_pp)->hash_context_p);
            (*hmac_context_pp)->hash_context_p = NULL;
        }

        if (NULL != (*hmac_context_pp)->inner_hash_context_p) {
            free((*hmac_context_pp)->inner_hash_context_p);
            (*hmac_context_pp)->inner_hash_context_p = NULL;
        }

        if (NULL != (*hmac_context_pp)->outer_hash_context_p) {
            free((*hmac_context_pp)->outer_hash_context_p);
            (*hmac_context_pp)->outer_hash_context_p = NULL;
        }

        free(*hmac_context_pp);
        *hmac_context_pp = NULL;
    }
}

/**
 * Changes the byte order in a buffer when converting
 * from a uint8_t to a uint32_t buffer (little endian).
 * This function is used to be able to work on an big endian input
 * in the little endian ARM.
 */
static void
cops_byteswap(void *buf_p, uint32_t ul_num_bytes)
{
    union {
        uint32_t i;
        uint8_t  b[4];
    } y;

    uint32_t i = 0;

    while (ul_num_bytes > 0) {
        y.b[0] = ((uint8_t *)buf_p)[3+i*4];
        y.b[1] = ((uint8_t *)buf_p)[2+i*4];
        y.b[2] = ((uint8_t *)buf_p)[1+i*4];
        y.b[3] = ((uint8_t *)buf_p)[i*4];
        ((uint32_t *)buf_p)[i++] = y.i;
        ul_num_bytes -= 4;
    }
}

/*
 * Executes SHA-256 for a data block (the block must be in little endian).
 */
static void
cops_sha256_process_block(cops_sha256_context_t *const sha256_ctx_p)
{
    uint32_t     W[16], temp1, temp2, a, b, c, d, e, f, g, h;
    uint32_t     *ps_p;
    uint32_t     i, k;

    /* Copy first 16 words
       (16*32 = 512 bits/block = 64 bytes/block) directly. */
    memcpy((void *)W, (const void *)sha256_ctx_p->buf, 64);

    /* Copy old digest into a, b, c, d, e */
    ps_p = sha256_ctx_p->v;
    a = *ps_p++;
    b = *ps_p++;
    c = *ps_p++;
    d = *ps_p++;
    e = *ps_p++;
    f = *ps_p++;
    g = *ps_p++;
    h = *ps_p++;

    /* Perform the hash. */

    /* For the first 16 words */
    for (i = 0; i < 16; i++) {
        temp1 = h + F4(e) + F1(e, f, g) + K[i] + W[i];
        temp2 = F3(a) + F2(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    /* For the last 48 words */
    for (i = 16; i < 64; i++) {
        /* % 16 to reuse the 16 words of W */
        k = i % 16;
        W[k] = F6(W[(i-2)%16]) + W[(i-7)%16] + F5(W[(i-15)%16]) + W[(i-16)%16];
        temp1 = h + F4(e) + F1(e, f, g) + K[i] + W[k];
        temp2 = F3(a) + F2(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    /* Add new info into digest. */
    ps_p = sha256_ctx_p->v;
    *ps_p++ += a;
    *ps_p++ += b;
    *ps_p++ += c;
    *ps_p++ += d;
    *ps_p++ += e;
    *ps_p++ += f;
    *ps_p++ += g;
    *ps_p++ += h;
}

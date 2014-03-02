#ifndef BASS_APP_H
#define BASS_APP_H
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define BASS_HASH_LENGTH                       (64)

#define SMCL_OK                                0x0
#define SMCL_TEE_FAILURE                       0x1
#define SMCL_GENERAL_FAILURE                   0x2
#define SMCL_SERVICE_FAILURE                   0x3
#define SMCL_SERVICE_SECURITY_FAILED           0x4
#define SMCL_SERVICE_OUT_OF_MEMORY             0x5

/* ID's to use when calling smcl_get_modem_memory. */
#define TCM_L1                                 0x0
#define TCM_L2                                 0x1
#define GET_REGISTERS                          0x2
#define MODEM_SRAM                             0x3

#define TCM_L1_MAX_SIZE                        0x18000
#define TCM_L2_MAX_SIZE                        0x18000
#define GET_REGISTERS_SIZE                     0x200
#define MODEM_SRAM_SIZE                        0x2000

/*
 * Defines that tell whether it is encryption or decryption. Values should stay
 * in sync with the values defined in secure world.
 */
#define SMCL_DIR_ENCRYPT                      (0x8)
#define SMCL_DIR_DECRYPT                      (0x0)

#define SHA256_HASH_SIZE                        32

/* Backwards compatible. */
#define PRODUCT_ID_8500           PRODUCT_ID_8500B

/* Flags indicating run-time environment. */
#define TEE_RT_FLAGS_NORMAL             0x00000000
#define TEE_RT_FLAGS_MASK_ITP_PROD      0x00000001
#define TEE_RT_FLAGS_MODEM_DEBUG        0x00000002
#define TEE_RT_FLAGS_RNG_REG_PUBLIC     0x00000004
#define TEE_RT_FLAGS_JTAG_ENABLED       0x00000008
#define TEE_RT_FLAGS_POST_BOOT          0x00000010
#define TEE_RT_FLAGS_PERIPHERAL_BOOT    0x00000020
#define TEE_RT_FLAGS_ENGINEERING_STATE  0x00000040
#define TEE_RT_FLAGS_SECURE_BOOT        0x00000080
#define TEE_RT_FLAGS_PERM_AUTH          0x00000100
#define TEE_RT_FLAGS_ARB_ACTIVE         0x00000200
#define TEE_RT_FLAGS_DISABLE_SIML_AUTH  0x00000400
#define TEE_RT_FLAGS_ALLOW_MODEM_DUMP   0x00000800

/* Flags indicating fuses. */
#define TEE_FUSE_FLAGS_MODEM_DISABLE    0x00000001
#define TEE_FUSE_FLAGS_IS_PRODCHIP      0x00000002

/*
 * Structure for the signed header.
 *
 * @param magic           The magic header.
 * @size_of_signed_header Size of the signed header.
 * @size_of_signature     Size of the signature.
 * @sign_hash_type        The hash type for the signature, see t_hash_type.
 * @signature_type        Signature type, see t_signature_type.
 * @hash_type             The hash type, see t_hash_type.
 * @payload_type          The payload type, see enum payload_type_t.
 * @flags                 Reserved.
 * @size_of_payload       The size of the payload (except payload type
 *                        TAPP).
 * @sw_vers_nbr           Software version number.
 * @load_address          The load address.
 * @startup_address       The startup address.
 * @spare                 Reserved.
 *
 * This gives checkpatch warning which suggests __packed, but if you use that,
 * then arm-gcc complains. It's better to have no errors and warnings during
 * build than have a clean checkpatch output, so we keep it as it is here.
 */
typedef struct __attribute__ ((packed)) {
    uint32_t magic;
    uint16_t size_of_signed_header;
    uint16_t size_of_signature;
    uint32_t sign_hash_type;
    uint32_t signature_type;
    uint32_t hash_type;
    uint32_t payload_type;
    uint32_t flags;
    uint32_t size_of_payload;
    uint32_t sw_vers_nbr;
    uint32_t load_address;
    uint32_t startup_address;
    uint32_t spare;
} bass_app_signed_header_t;

/*
 * Structure for the anti-roll back (ARB) table info.
 *
 * @param toc_id1_size    Size of TOC ID 1.
 * @param toc_id1         Pointer to the TOC ID 1.
 * @param toc_id2_size    Size of TOC ID 2.
 * @param toc_id2         Pointer to TOC ID 2.
 * @param table1_size     Size of ARB table 1.
 * @param table1          Poiner to ARB table 1.
 * @param table2_size     Size of ARB table 2.
 * @param table2          Pointer to ARB table 2.
 */
struct bass_app_arb_table_info {
    size_t toc_id1_size;
    char *toc_id1;
    size_t toc_id2_size;
    char *toc_id2;
    size_t table1_size;
    uint8_t *table1;
    size_t table2_size;
    uint8_t *table2;
};

/*
 * structure for holding SHA256 Hash value.
 *
 * @param Value   Hash value
 */
enum payload_type_t {
    BASS_PL_TYPE_TAPP = 0,
    BASS_PL_TYPE_PRCMU,
    BASS_PL_TYPE_MEMINIT,
    BASS_PL_TYPE_X_LOADER,
    BASS_PL_TYPE_OS_LOADER,
    BASS_PL_TYPE_APE_NW_CODE,
    BASS_PL_TYPE_FC_LOADER,
    BASS_PL_TYPE_MODEM_CODE,
    BASS_PL_TYPE_FOTA,
    BASS_PL_TYPE_DNTCERT,
    BASS_PL_TYPE_AUTHCERT,
    BASS_PL_TYPE_IPL,
    BASS_PL_TYPE_FLASH_ARCHIVE,
    BASS_PL_TYPE_ITP,
    BASS_PL_TYPE_TVP,
    BASS_PL_TYPE_FRAC,
    BASS_PL_TYPE_AUTH_CHALLENGE = -1 /* 0xffffffff */
};

typedef enum {
    BASS_APP_SHA1_HASH   = 0x1UL,
    BASS_APP_SHA256_HASH = 0x2UL,
    BASS_APP_SHA384_HASH = 0x4UL,
    BASS_APP_SHA512_HASH = 0x8UL,
    BASS_APP_SHA1_HMAC   = 0x10UL,
    BASS_APP_SHA256_HMAC = 0x20UL,
    BASS_APP_SHA384_HMAC = 0x40UL,
    BASS_APP_SHA512_HMAC = 0x80UL
} t_bass_hash_type;

typedef struct {
    uint8_t value[BASS_HASH_LENGTH];
} bass_hash_t;

typedef enum {
    BASS_RC_SUCCESS,
    BASS_RC_FAILURE,
    BASS_RC_ERROR_UNKNOWN,
    BASS_RC_ERROR_INTERNAL,
    BASS_RC_ERROR_TEE_API,
    BASS_RC_ERROR_TEE_COMMS,
    BASS_RC_ERROR_TEE_CORE,
    BASS_RC_ERROR_TEE_TRUSTED_APP,
} bass_return_code;

/*
 * Product id numbers
 * Note: Needs to be replicated from isswapi_types.h.
 */
enum tee_product_id {
    PRODUCT_ID_UNKNOWN  =  0,
    PRODUCT_ID_8400     =  1, /* Obsolete */
    PRODUCT_ID_8500B    =  2, /* 1080p/1GHz/400MHz */
    PRODUCT_ID_9500     =  3, /* 1080p/1GHz/400MHz */
    PRODUCT_ID_5500     =  4, /* Obsolete */
    PRODUCT_ID_7400     =  5,
    PRODUCT_ID_8500C    =  6, /* 720p/1GHz/400MHz */
    PRODUCT_ID_8500A    =  7, /* 720p/800MHz/320MHz */
    PRODUCT_ID_8500E    =  8, /* 1080p/1.15GHz/533MHz */
    PRODUCT_ID_8520F    =  9, /* 720p/1.15GHz/533MHz */
    PRODUCT_ID_8520H    = 10, /* 720p/1GHz/200MHz */
    PRODUCT_ID_9540     = 11,
    PRODUCT_ID_9500C    = 12, /* 1080p/1.15GHz/533MHz */
    PRODUCT_ID_8500F    = 13, /* 720p/1.15GHz/533MHz */
    PRODUCT_ID_8540APE  = 14,
    PRODUCT_ID_8540XMIP = 15,
    PRODUCT_ID_8520E    = 16,
    PRODUCT_ID_8520J    = 17,
};

/**
 * struct tee_product_config - System configuration structure.
 *
 * @product_id: Product identification.
 * @rt_flags: Runtime configuration flags.
 * @fuse_flags: Fuse flags.
 *
 */
struct tee_product_config {
    uint32_t product_id;
    uint32_t rt_flags;
    uint32_t fuse_flags;
};

typedef uint32_t bass_payload_type_t;
typedef uint32_t bass_hash_type_t;

/*
 * @brief Segment descriptors used in access_image_descr.
 *
 * @param segemnt      Pointer to the segment.
 * @param hash         Hash of the segment.
 * @param size         Size of the segment.
 */
struct mcore_segment_descr {
    void *segment;
    void *hash;
    size_t size;
};

/*
 * @brief Access image data to be provided to the secure world.
 *
 * @param elf_hdr      Pointer to the elf header of the modem image.
 * @param pgm_hdr_tbl  Pointer to the program header table.
 * @param signature    Pointer to the signature.
 * @param nbr_segments Number of mcore_segment_desc.
 * @param descr        Segment descriptors. This is an array of pointers.
 *                     appended at the end of this structure.
 */
struct access_image_descr {
    void *elf_hdr;
    void *pgm_hdr_tbl;
    void *signature;
    unsigned long nbr_segment;
    struct mcore_segment_descr *descr;
};

/*
 * @brief Return data from the veirfy signed header call.
 *
 * This struct is used to provide information to the caller of
 * verify_signedheader.
 *
 * @param pl_size               The size of the payload.
 * @param ht                    The hash type used for calculating the expected
 *                              hash of the payload.
 * @param plt                   The payload type protected by the signed header.
 * @param ehash                 The expected payload hash.
 * @param size_of_signed_header The size of the signed header.
 */
typedef struct {
    size_t pl_size;
    bass_hash_type_t ht;
    bass_payload_type_t plt;
    bass_hash_t ehash;
    size_t size_of_signed_header;
} bass_signed_header_info_t;

/*
 * @brief Data for the handle_bootp_magic call.
 *
 * @param bootp_locked          Is the bootpartition locked?
 * @param auth_inbuffer_p       Hash data for engineering mode.
 * @param auth_inbuffer_size    Size of above hash data.
 * @param sec_inbuffer_p        Hash data for sec profile.
 * @param sec_inbuffer_size     Size of above hash data.
 */
typedef struct {
    uint32_t bootp_locked;
    uint8_t *eng_inbuffer_p;
    size_t eng_inbuffer_size;
    uint8_t *sec_inbuffer_p;
    size_t sec_inbuffer_size;
} bass_handle_bootp_magic_t;

/*
 * This function will unlock the modem by rearrange DDR regions for APE, Modem
 * and shared memory. The shared region (SECR_DDRSHAREHIGH) will be set to the
 * same as modem's upper region (SECR_DDRMODHIGH).
 *
 * @return SMCL_OK on success, otherwise SMCL_GENERAL_FAILURE, SMCL_TEE_FAILURE.
 */
int smcl_unlock_dbg_modem_mem(void);

/*
 * This function will unlock the modem memory.
 */
int smcl_reset_unlock_modem_mem(void);

/*
 * This function will restart the modem.
 */
int smcl_restart_modem(uint32_t ab8500cutid);

/*
 * Function that dump memory from TCM L1 and L2 if debug mode is enabled.
 *
 * Note that the user is responsible of allocating the provided buffer, the user
 * also have to free the buffer after calling this function.
 *
 * @param mem_id Id of the memory that should be dumped.
 * @param buffer Pointer to a buffer that the caller have allocated.
 * @param size The size of the allocated buffer.
 *
 * @return SMCL_OK on success, otherwise SMCL_GENERAL_FAILURE, SMCL_TEE_FAILURE.
 */
int smcl_get_modem_memory(uint8_t mem_id, uint8_t *buffer, uint32_t size);

/*
 * This function will encrypt 16 bytes of data using AES-ECB. The caller is
 * required to make space for 16 bytes in the pin variable. The reason for this
 * is that AES will produce at minimum 16 bytes of data for output. So even
 * though you might only have a 4 digit pin code, the result after encryption
 * will still be 16 bytes.
 *
 * @param pin           The pin code that should be encrypted or decrypted.
 * @param pin_length    The length of the buffer (should always be 16). The
 *                      reason for having this is that the user should be aware
 *                      that 16 bytes is needed in the buffer to call this
 *                      function.
 * @param encrypt_dir   SMCL_DIR_ENCRYPT or SMCL_DIR_DECRYPT.
 *
 * @return              SMCL_OK on success, otherwise failing SMCL error codes.
 */
uint32_t smcl_encrypt_pin(uint8_t *pin,
                          uint32_t pin_length,
                          uint32_t encrypt_dir);

/*
 * @brief verify_signedheader
 *
 * The following function verifies a signed header.
 *
 * Note that when calling this function in engineering mode (R&D) without a
 * valid header will give BASS_RC_SUCCESS in response for all payload types
 * except: A B C D E. When this happens, the information in the struct hinfo
 * will contain invalid values. I.e, the struct will be memset to zeroes
 * indicating that values have undefined behavior and also indicate that
 * size_of_signed_header is equal to zero.
 *
 * @param hdr        [in]   A pointer to the signed header to verify and return
 *                          the expected hash from.
 * @param hdr_length [in]   The length of the header.
 * @param pl_type    [in]   The type of the payload.
 * @param hinfo      [out]  The client should calculate a hash over the payload
 *                          accompanying the signed header and compare to the
 *                          expected hash included in the out param. The hash
 *                          _must_ be SHA-256.
 *
 * @return bool             true if function call succeeded otherwise false.
 */
bass_return_code verify_signedheader(const uint8_t *const hdr,
                                     const uint32_t hdr_length,
                                     enum payload_type_t pl_type,
                                     bass_signed_header_info_t *const hinfo);

/*
 * Init function for SHA256 calculations.
 *
 * @param context [in/out]  A pointer to the SHA context.
 *
 * @return true if OK, otherwise false.
 */
bool bass_sha256_init(void **context);

/*
 * Update function for SHA256 calculations.
 *
 * @param context [in/out]  A pointer to the SHA context.
 * @param data    [in]      Pointer to the data to be hashed.
 * @param length  [in]      Length of the data.
 *
 * @return true if OK, otherwise false.
 */
bool bass_sha256_update(void **context, const uint8_t *const data,
                        const uint32_t length);

/*
 * Final function for SHA256 calculations.
 *
 * @param context [in/out]  A pointer to the SHA context.
 * @param hash    [out]     The 32 byte hash as the result.
 *
 * @return true if OK, otherwise false.
 */
bool bass_sha256_final(void **context, bass_hash_t *const hash);

/**
 * @brief bass_calcdigest
 *
 * Calculates a hash over the data pointed out by the buffer and length
 * parameters according to the hash type parameter. The data must reside in
 * consecutive chunks in memory or the function will produce an incorrect hash
 * (i.e. the function will not be able to follow pages out-of-order).
 *
 * If ht is set to SHA1, the 20 first bytes of the hash will contain the
 * calculated hash value. The remaining will be set to 0.
 *
 * @param ht         [in]    The hash type to calculate the hash from.
 * @param data       [in]    The start address to calculate the hash from.
 * @param data_lengt [in]    The length of the data to calculate the hash from.
 * @param hash       [out]   The calculated hash.
 *
 * @return bass_return_code  BASS_RC_SUCCESS when successful, otherwise one
 *                           of the defined return codes.
 */
bass_return_code bass_calcdigest(const t_bass_hash_type ht,
                                 const uint8_t *data,
                                 const size_t data_length,
                                 bass_hash_t *const hash);

/*
 * @brief This function invokes the trusted application to start the modem.
 *
 * @param access_mem_start         Start of modem memory.
 * @param shared_mem_size          Shared memory size.
 * @param access_private_mem_size  Private memory size.
 * @param access_image_descr       Ptr to the modem image descriptor.
 */
int verify_start_modem(
    void *access_mem_start,
    size_t shared_mem_size,
    size_t access_private_mem_size,
    struct access_image_descr *access_image_descr);

/*
 * @brief This function verifies and start the modem. Only for U8500!
 *
 * @param ab8500cutid         The AB8500 cut id.
 *
 * @return bass_return_code  BASS_RC_SUCCESS when successful modem start,
 *                           otherwise one of the defined return codes.
 */
bass_return_code bass_u8500_verify_start_modem(uint32_t ab8500cutid);

/*
 * @brief get_productid
 *
 * The following function retrieves a product id for the chip. In combination
 * with cut_id this can be used to detect the 84xx and 95xx chip series.
 *
 * @param buffer [in]       A pointer to the buffer where product_id will
 *                          be stored.
 * @return bool             true if function call succeeded otherwise false.
 */
bass_return_code get_productid(uint32_t *const product_id);

/*
 * @brief get_product_config
 *
 * The following function retrieves a product config struct for the chip.
 *
 * @param product_config [in]   A pointer to struct where product_id,
 *                              runtime flags and fuse flags are stored.
 *
 * @return bool                 true if function call succeeded otherwise false.
 */
bass_return_code get_product_config(struct tee_product_config *product_config);

/*
 * @brief get_ta_key_hash
 *
 * The following function gives the hash of the key used when signing the ta.
 *
 * @param ta_key_hash [out]     A pointer to the ta key hash.
 * @param key_size    [out]     Size of the ta key hash.
 *
 * @return bool                 true if function call succeeded otherwise false.
 */
bass_return_code get_ta_key_hash(bass_hash_t *ta_key_hash, uint32_t *key_size);

/*
 * @brief Invokes a trusted application to apply the soc settings.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param avs_data     [in]     AVS data to be used instead of default.
 *                              Should be NULL if not available.
 * @param avs_data_len [in]     Length of supplied AVS data, not used
                                if avs_data is NULL.
 * @param ddr_regs     [out]    Buffer to hold contents of DDR registers.
 *                              Should be NULL output not requested.
 * @param ddr_regs_len [in/out] Size of buffer above, *ddr_regs_len will
 *                              on return hold the number of returned
 *                              registers.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when soc settings successfully
 *                              applied, otherwise one of the defined return
 *                              codes.
 */
bass_return_code bass_apply_soc_settings(const uint8_t *avs_data,
                                         const size_t avs_data_len,
                                         uint8_t *ddr_regs,
                                         size_t *ddr_regs_len);

/*
 * @brief Returns AVS fuses.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param fuses     [out]   Buffer to hold the AVS fuses.
 * @param fuses_len [in]    Size of above buffer. The size have to match
 *                          the expected size exacly. The required size
 *                          may differ in different platforms.
 *
 * @return bass_return_code BASS_RC_SUCCESS when AVS fuses successfully
 *                          extracted, otherwise one of the other defined return
 *                          codes.
 */
bass_return_code bass_get_avs_fuses(uint8_t *fuses, const size_t fuses_len);

/*
 * @brief Requests PRCMU to apply the AVS settings.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param avs_data     [in]     AVS data to be used.
 * @param avs_data_len [in]     Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when AVS settings successfully
 *                              applied, otherwise one of the other defined
 *                              return codes.
 */
bass_return_code bass_prcmu_apply_avs_settings(const uint8_t *avs_data,
                                               const size_t avs_data_len);

/*
 * @brief Request PRCMU to enter ApExec state.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @return bass_return_code  BASS_RC_SUCCESS when ApExec state successfully
 *                           entered, otherwise one of the other defined return
 *                           codes.
 */
bass_return_code bass_prcmu_set_ap_exec(void);

/*
 * @brief Request PRCMU to perform DDRPreInit.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param ddr_data          [in]    DDR data for initial configuration
 * @param ddr_data_len      [in]    Size of above buffer. The size have to match
 *                                  the expected size exacly. The required size
 *                                  may differ in different platforms.
 * @param ddr_read_regs     [in]    Buffer specifying registers to read.
 * @param ddr_read_regs_len [in]    Size of above buffer. The size have to match
 *                                  the expected size exacly. The required size
 *                                  may differ in different platforms.
 * @param  ddr_regs         [out]   Buffer to hold the value of specified
 *                                  registers.
 * @param ddr_regs_len      [in]    Size of above buffer. The size have to match
 *                                  the expected size exacly. The required size
 *                                  may differ in different platforms.
 *
 * @return bass_return_code         BASS_RC_SUCCESS when DDR register
 *                                  successfully extracted, otherwise one of the
 *                                  other defined return codes.
 */
bass_return_code bass_prcmu_ddr_pre_init(
    const uint8_t *ddr_data, const size_t ddr_data_len,
    const uint8_t *ddr_read_regs, size_t ddr_read_regs_len,
    uint8_t *ddr_regs, const size_t ddr_regs_len);

/*
 * @brief Request PRCMU to perform DDRInit.
 *
 * Restricted to only be called from Xloader or equivalent. Once this
 * function has returned all the functions involed in initialzation of
 * DDR are disabled.
 *
 * @param ddr_data     [in]     DDR data for configuration.
 * @param ddr_data_len [in]     Size of above buffer. The size have to match
 *                              the expected size exacly. The required size may
 *                              differ in different platforms.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when DDR successfully
 *                              initialized, otherwise one of the other defined
 *                              return codes.
 */
bass_return_code bass_prcmu_ddr_init(const uint8_t *ddr_data,
                                     const size_t ddr_data_len);

/*
 * @brief Request PRCMU to set DDR speed.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param speed_data     [in]   Data to set DDR speed, may be
 *                              interpreted different on different platforms.
 * @param speed_data_len [in]   Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when DDR frequency was
 *                              successfully set, otherwise one of the other
 *                              defined return codes.
 */
bass_return_code bass_prcmu_set_ddr_speed(const uint8_t *speed_data,
                                          const size_t speed_data_len);

/*
 * @brief Request PRCMU to set memory speed.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param speed_data     [in]   Data to set memory speed, may be
 *                              interpreted different on different platforms.
 * @param speed_data_len [in]   Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when memory speed was
 *                              successfully set, otherwise one of the other
 *                              defined return codes.
 */
bass_return_code bass_prcmu_set_memory_speed(const uint8_t *speed_data,
                                             const size_t speed_data_len);

/*
 * @brief Request PRCMU to do ABB initializations.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param abb_data     [in]     Data to initialize ABB with, may be
 *                              interpreted different on different platforms.
 * @param abb_data_len [in]     Size of above buffer. The size have to
 *                              less than or equal to the maximum size.
 *                              The maximum size may differ in different
 *                              platforms.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when DDR frequency was
 *                              successfully set, otherwise one of the other
 *                              defined return codes.
 */
bass_return_code bass_prcmu_abb_init(const uint8_t *abb_data,
                                     const size_t abb_data_len);

/*
 * @brief Sets L2CC filtering registers.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param start_reg [in]        New value for the L2CC Address Filtering
 *                              Start Register.
 * @param end_reg   [in]        New value for the L2CC Address Filtering
 *                              End Register.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when registers was successfully
 *                              updated, otherwise one of the other defined
 *                              return codes.
 */
bass_return_code bass_set_l2cc_filter_registers(const uint32_t start_reg,
                                                const uint32_t end_reg);

/*
 * @brief Store and verify signed indirect memory access descriptor in secure
 * memory.
 *
 * @param data [in]             Buffer holding the IMAD.
 * @param len  [in]             Length of the buffer above.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when data was stored
 *                              successfully, otherwise one of the other defined
 *                              return codes.
 */
bass_return_code bass_imad_store(const uint8_t *data, size_t data_len);

/*
 * @brief Release the previously stored indirect memory access descriptor
 *        and ending access privileges gained by the descriptor.
 *
 * @return bass_return_code  BASS_RC_SUCCESS when data was released OK,
 *                           otherwise one of the other defined return codes.
 */
bass_return_code bass_imad_release(void);

/*
 * @brief Define an operation on a indirect memory access descriptor.
 *
 * @param id        Sequence id in the IMAD to use.
 * @param paddr     Normal world physical address to use if it is an operation
 *                  requiring in-data or supplying out-data. Note that 0 is
 *                  allowed as a physical address.
 */
struct bass_imad_entry {
    uint32_t id;
    uint32_t paddr;
};

/*
 * @brief  Perform a set of sequences on an indirect memory access descriptor.
 *
 * @param entries      [in]     An array of entries that identifies the
 *                              sequence(s) to execute and supplies eventual
 *                              parameters.
 * @param num_ops      [in]     Number of elements in the array above.
 * @param failed_entry [out]    Failed entry when error is returned.
 *
 * Each of the operations in a sequence to execute has to be matched by one
 * entry in consecutive order, that is, the entries within a sequence has
 * to match one to one in the same order.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when complete sequence was
 *                              executed, otherwise
 *                              BASS_RC_ERROR_TEE_TRUSTED_APP Execution aborted
 *                              at failed_entry, no changes made by the failing
 *                              entry.
 */
bass_return_code bass_imad_exec(const struct bass_imad_entry *entries,
                                size_t num_entries, size_t *failed_entry);

/*
 * @brief Calls into secure world to perform the payload hash check there.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param hashtype     [in]     Inbuffer containing type of hash.
 * @param payload      [in]     Payload to calculate hash on.
 * @param payload_size [in]     Payload size (bytes).
 * @param ehash        [in]     Expected hash.
 * @param ehash_size   [in]     Size of expected hash (bytes).
 *
 * @return bass_return_code     BASS_RC_SUCCESS when successfully performed
 *                              check, otherwise one of the other defined return
 *                              codes.
 */
bass_return_code bass_check_payload_hash(bass_hash_type_t *hashtype,
                                         bass_payload_type_t *payloadtype,
                                         void *payload,
                                         size_t payload_size,
                                         void *ehash,
                                         size_t ehash_size);

/*
 * @brief Calls into secure world to handle bootpartition magic data.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param bootp_data [in]       Struct holding data to be sent.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when successfully handled data,
 *                              otherwise one of the other defined return codes.
 */
bass_return_code bass_handle_bootp_magic(bass_handle_bootp_magic_t *bootp_data);

/*
 * @brief Request PRCMU to do Thermal Sensors initializations.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param data     [in]         Data to initialize thermal sensors with, may be
 *                              interpreted different on different platforms.
 * @param data_len [in]         Size of above buffer. The size have to less than
 *                              or equal to the maximum size. The maximum size
 *                              may differ in different platforms.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when successfully initialized
 *                              thermal sensors, otherwise one of the other
 *                              defined return codes.
 */
bass_return_code bass_prcmu_set_thermal_sensors(const uint8_t *data,
                                                const size_t data_len);

/*
 * @brief Returns die id.
 *
 * @param  data    [out]        Buffer to hold the die id, may be
 *                              interpreted different on different platforms.
 * @param data_len [in]         Size of above buffer. The size have to
 *                              match the expected size exactly. The required
 *                              size may differ on different platforms.
 *
 * @return bass_return_code     BASS_RC_SUCCESS when die id was successfully
 *                              read, otherwise one of the other defined return
 *                              codes.
 */
bass_return_code bass_get_die_id(uint8_t *data, const size_t data_len);

/*
 * @brief Returns Anti-Roll Back (ARB) table info.
 *
 * For Little Kernel use.
 * Preferred usage:
 * 1. Get the toc_id_sizes and table_sizes (table_info members),
 *    by calling this function get_sizes = true.
 * 2. Call a 2nd time, with get_sizes = false and updated with "correct" sizes
 *    to get the toc_ids and tables.
 *
 * Note! If the requested sizes are zero, there is no table to be updated.
 *
 * @param get_sizes  [in]       True if request toc_id_sizes and table_sizes.
 * @param table_info [in/out]   Pointer to the ARB table info structure.
 *
 * @return bass_return_code     BASS_RC_SUCCESS for successful call(s),
                                otherwise one other defined return code.
 */
bass_return_code bass_get_arb_table_info(
    bool get_sizes, struct bass_app_arb_table_info *table_info);

/*
 * @brief Returns random value.
 *
 * @param random_data   [out]   Pointer to the expected random value.
 * @param random_size   [in]    Size in bytes of the expected random value.
 *
 * @return bass_return_code     BASS_RC_SUCCESS for successful call(s),
                                otherwise one other defined return code.
 */
bass_return_code drm_key_get_random(uint8_t *random_data,
                                    const size_t random_size);

/*
 * @brief Returns the device public ID.
 *
 * @param public_id      [out]   Pointer to the expected public ID.
 * @param public_id_size [in]    Size in bytes of the expected public ID.
 *
 * @return bass_return_code     BASS_RC_SUCCESS for successful call(s),
                                otherwise one other defined return code.
 */
bass_return_code drm_key_identify_device(uint8_t *public_id,
                                         const size_t public_id_size);

/*
 * @brief Sets the DRM state.
 * Note! Call only valid prior to post-boot,
 *       and only allowed to be set once per boot.
 *
 * @param drm_state_locked [in] True = state locked, false = state unlocked.
 *
 * @return bass_return_code     BASS_RC_SUCCESS for successful call(s),
                                otherwise one other defined return code.
 */
bass_return_code bass_set_drm_state(const bool drm_state_locked);

/*
 * @brief Encrypts and store the provided DRM key.
 *
 * @param key      [in]   Pointer to the DRM key to be encrypted and stored.
 * @param key_size [in]   Size in bytes of the provided DRM key.
 *
 * @return bass_return_code     BASS_RC_SUCCESS for successful call(s),
                                otherwise one other defined return code.
 */
bass_return_code drm_key_encrypt_and_store_key(uint8_t *key,
                                               const size_t key_size);

/*
 * @brief Returns the stored (in drm_key_encrypt_and_store_key) DRM key.
 *
 * @param key      [out]  Pointer to the expected DRM key.
 * @param key_size [in]   Size in bytes of the expected DRM key.
 *
 * @return bass_return_code     BASS_RC_SUCCESS for successful call(s),
                                otherwise one other defined return code.
 */
bass_return_code drm_key_get_key_data(uint8_t *key, const size_t key_size);

/**
 * @brief Sets permanent authentication if hash of data matches hash in
 *        STE-DNT-cert
 *
 * Restricted to only be called from Xloader or equivalent and is only
 * applicable for glacier customer seperation.
 *
 * @param [in]     inbuffer      The data matching the magic word.
 * @param [in]     inbuffer_size Size of the data above.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      Successfully checked the data
 * @retval BASS_RC_FAILURE      Failed check the data or data mismatch
 */
bass_return_code bass_auth_dnt_magic(void *inbuffer, size_t inbuffer_size);

#endif

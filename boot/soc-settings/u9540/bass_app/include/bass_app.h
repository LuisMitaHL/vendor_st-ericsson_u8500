/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef BASS_APP_H
#define BASS_APP_H

#include "isswapi_types.h"

#ifdef OS_FREE
#include <stddef.h>
#include <stdbool.h>

typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
#else
#include <stddef.h>
#include "stdint.h"
#include <stdbool.h>
#endif

#define BASS_HASH_LENGTH (64)

/**
 * structure for holding SHA256 Hash value
 *
 * @param Value   Hash value
 */

typedef enum
{
  BASS_APP_SHA1_HASH   = 0x1UL,
  BASS_APP_SHA256_HASH = 0x2UL,
  BASS_APP_SHA384_HASH = 0x4UL,
  BASS_APP_SHA512_HASH = 0x8UL,
  BASS_APP_SHA1_HMAC   = 0x10UL,
  BASS_APP_SHA256_HMAC = 0x20UL,
  BASS_APP_SHA384_HMAC = 0x40UL,
  BASS_APP_SHA512_HMAC = 0x80UL
} t_bass_hash_type;


typedef struct
{
  uint8_t Value[BASS_HASH_LENGTH];
} bass_hash_t;

enum bass_payload_type {
  PL_TYPE_TAPP = 0x00,
  PL_TYPE_PRCMU,
  PL_TYPE_MEMINIT,
  PL_TYPE_X_LOADER,
  PL_TYPE_OS_LOADER,
  PL_TYPE_APE_NW_CODE,
  PL_TYPE_FC_LOADER,
  PL_TYPE_MODEM_CODE,
  PL_TYPE_FOTA,
  PL_TYPE_RESERVED1,
  PL_TYPE_AUTH_CHALLENGE,
  PL_TYPE_IPL,
  PL_TYPE_FLASH_ARCHIVE,
  PL_TYPE_AUTHCERT = 0xFFFFFFF
};

typedef enum {
		BASS_RC_SUCCESS,
		BASS_RC_FAILURE,
		BASS_RC_ERROR_UNKNOWN,
		BASS_RC_ERROR_INTERNAL
} bass_return_code;

typedef uint32_t bass_payload_type_t;

typedef uint32_t bass_hash_type_t;

/*
 * @brief Segment descriptors used in access_image_descr
 *
 * @param segemnt      Pointer to the segment
 * @param hash         Hash of the segment
 * @param size         Size of the segment
 */
struct mcore_segment_descr {
    void *segment;
    void *hash;
    size_t size;
};

/*
 * @brief Access image data to be provided to the secure world
 *
 * @param elf_hdr      Pointer to the elf header of the modem image
 * @param pgm_hdr_tbl  Pointer to the program header table
 * @param signature    Pointer to the signature
 * @param nbr_segments Number of mcore_segment_desc
 * @param descr        Segment descriptors. This is an array of ponters
 *                     appended at the end of this structure
 */
struct access_image_descr {
    void *elf_hdr;
    void *pgm_hdr_tbl;
    void *signature;
    unsigned long nbr_segment;
    struct mcore_segment_descr *descr;
};

/*
 * @brief Return data from the veirfy signed header call
 *
 * @param pl_size	The size of the payload
 * @param ht		The hash type used for calculating the expected hash
 *                      of the payload
 * @param plt		The payload type protected by the signed header
 * @param ehash		The expected payload hash
 * @param hdr_size      The size of the header
 */

typedef struct {
  size_t pl_size;
  bass_hash_type_t ht;
  bass_payload_type_t plt;
  bass_hash_t ehash;
  size_t hdr_size;
  size_t hash_size;
} bass_vsh_t;


/*
 * @brief VerifySignedHeader
 *
 * The following function verifies a signed header.
 *
 * @param [in] hdr         A pointer to the signed header to verify and
 *                         return the expected hash from.
 * @param [in] hdr_length  The length of the header.
 * @param [out] hinfo      The client should calculate a hash
 *                         over the payload accompanying the
 *                         signed header and compare to the
 *                         expected hash included in the out param.
 *                         The hash _must_ be SHA-256.
 *
 * @return bool
 * @retval true The result succeeded.
 * @retval false Something failed.
 */

bass_return_code verify_signedheader(const uint8_t* const hdr,
				     enum issw_payload_type pl_type,
				     bass_vsh_t* const hinfo);


/**
 * @brief bass_calcdigest
 *
 * Calculates a hash over the data pointed out by the buffer
 * and length parameters according to the hash type parameter.
 * The data must reside in consecutive chunkcs in memory or the
 * function will produce an incorrect hash (i.e. the function
 * will not be able to follow pages out-of-order).
 *
 * If ht is set to SHA1, the 20 first bytes of the hash will contain the
 * calculated hash value. The remaining will be set to 0.
 *
 * @param [in]	ht          The hash type to calculate the hash from
 * @param [in]	start       The start address to calculate the hash from
 * @param [in]	length	    The length of the data to calculate the hash from
 * @param [out]	hash        The calculated hash.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS		Successful calculation of hash
 * @retval BASS_RC_FAILURE		Unsuccessful calculation of hash
 * @retval BASS_RC_ERROR_UNKNOWN	An unknown error accured
 */

bass_return_code bass_calcdigest(const t_bass_hash_type ht,
				 const uint8_t* data,
				 const size_t data_length,
				 bass_hash_t* const hash);

/*
 * @brief This function invokes the trusted application to start the modem.
 *
 * @param access_mem_start         Start of modem memory
 * @param shared_mem_size          Shared memory size
 * @param access_private_mem_size  Private memory size
 * @param access_image_descr       Ptr to the modem image descriptor
 */
int verify_start_modem(
    void *access_mem_start,
    size_t shared_mem_size,
    size_t access_private_mem_size,
    struct access_image_descr *access_image_descr);

#define BASS_APP_AVS_PARAMS_LEN     40

/**
 * @brief Returns AVS parameters used to calculate optimal settings.
 *
 * Restricted to only be called from Xloader or equivaltent.
 *
 * @param [out] avs_params  Pointer to a buffer to hold the AVS parameters.
 *                          The buffer has to be at least
 *                          BASS_APP_AVS_PARAMS_LEN bytes large.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      Soc settings successfully applied
 * @retval BASS_RC_FAILURE      Failed to apply soc settings
 */
bass_return_code bass_app_get_avs_parameters(uint8_t *avs_params);

#define BASS_APP_NUM_DDR_REGISTERS  16
/**
 * @brief Invokes a trusted application to apply the soc settings.
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param [in]     avs_data     AVS data to be used instead of default.
 *                              Should be NULL if not available.
 * @param [in]     avs_data_len Length of supplied AVS data, not used
                                if avs_data is NULL.
 * @param [out]    ddr_regs     Buffer to hold contents of DDR registers.
 *                              Should be NULL output not requested.
 * @param [in/out] ddr_regs_len Size of buffer above, *ddr_regs_len will
 *                              on return hold the number of returned
 *                              registers.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      Soc settings successfully applied
 * @retval BASS_RC_FAILURE      Failed to apply soc settings
 */
bass_return_code bass_apply_soc_settings(const uint8_t *avs_data,
                                         const size_t avs_data_len,
                                         uint8_t *ddr_regs,
                                         size_t *ddr_regs_len);

/**
 * @brief Returns AVS fuses
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param [out] fuses       Buffer to hold the AVS fuses
 * @param [in]  fuses_len   Size of above buffer. The size have to match
 *                          the expected size exacly. The required size
 *                          may differ in different platforms.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      AVS fuses successfully extracted
 * @retval BASS_RC_FAILURE      Failed to extract AVS fuses
 */
bass_return_code bass_get_avs_fuses(uint8_t *fuses,
                    const size_t fuses_len);

/**
 * @brief Requests PRCMU to apply the AVS settings
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param [in]  avs_data        AVS data to be used
 * @param [in]  avs_data_len    Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      AVS settings successfully applied
 * @retval BASS_RC_FAILURE      Failed to apply AVS settings
 */
bass_return_code bass_prcmu_apply_avs_settings(const uint8_t *avs_data,
                     const size_t avs_data_len);

/**
 * @brief Request PRCMU to enter ApExec state
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      ApExec state successfully entered
 * @retval BASS_RC_FAILURE      Failed to enter ApExec state
 */
bass_return_code bass_prcmu_set_ap_exec(void);

/**
 * @brief Request PRCMU to perform DDRPreInit
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param [in] ddr_data         DDR data for initial configuration
 * @param [in] ddr_data_len     Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 * @param [in] ddr_read_regs    Buffer specifying registers to read.
 * @param [in] ddr_read_regs_len Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 * @param [out] ddr_regs        Buffer to hold the value of specified
 *                              registers.
 * @param [in] ddr_regs_len     Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      DDR register successfully extracted
 * @retval BASS_RC_FAILURE      Failed to extract DDR registers
 */
bass_return_code bass_prcmu_ddr_pre_init(
                    const uint8_t *ddr_data, const size_t ddr_data_len,
                    const uint8_t *ddr_read_regs, size_t ddr_read_regs_len,
                    uint8_t *ddr_regs, const size_t ddr_regs_len);

/**
 * @brief Request PRCMU to perform DDRInit
 *
 * Restricted to only be called from Xloader or equivalent. Once this
 * function has returned all the functions involed in initialzation of
 * DDR are disabled.
 *
 * @param [in] ddr_data         DDR data for configuration
 * @param [in] ddr_data_len     Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      DDR successfully initialized
 * @retval BASS_RC_FAILURE      Failed to initialize DDR
 */
bass_return_code bass_prcmu_ddr_init(const uint8_t *ddr_data,
                    const size_t ddr_data_len);

/**
 * @brief Request PRCMU to set DDR speed
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param [in] speed_data       Data to set DDR speed, may be
 *                              interpreted different on different platforms.
 * @param [in] speed_data_len   Size of above buffer. The size have to match
 *                              the expected size exacly. The required size
 *                              may differ in different platforms.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      DDR frequency was successfully set
 * @retval BASS_RC_FAILURE      Failed to set DDR frequency
 */
bass_return_code bass_prcmu_set_ddr_speed(const uint8_t *speed_data,
                    const size_t speed_data_len);

/**
 * @brief Request PRCMU to do ABB initializations
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param [in] abb_data         Data to initialize ABB with, may be
 *                              interpreted different on different platforms.
 * @param [in] abb_data_len     Size of above buffer. The size have to
 *                              less than or equal to the maximum size.
 *                              The maximum size may differ in different
 *                              platforms.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      DDR frequency was successfully set
 * @retval BASS_RC_FAILURE      Failed to set DDR frequency
 */
bass_return_code bass_prcmu_abb_init(const uint8_t *abb_data,
                    const size_t abb_data_len);

/**
 * @brief Sets L2CC filtering registers
 *
 * Restricted to only be called from Xloader or equivalent.
 *
 * @param [in] start_reg        New value for the L2CC Address Filtering
 *                              Start Register.
 * @param [in] end_reg          New value for the L2CC Address Filtering
 *                              End Register.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      Registers was successfully updated
 * @retval BASS_RC_FAILURE      Failed to set update registers
 */
bass_return_code bass_set_l2cc_filter_registers(const uint32_t start_reg,
                    const uint32_t end_reg);

/*
 * @brief Store and verify signed indirect memory access descriptor in
 *        secure memory
 * @param data [in] Buffer holding the IMAD
 * @param len  [in] Length of the buffer above
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      Data was stored OK
 */
bass_return_code bass_imad_store(const uint8_t *data, size_t data_len);

/*
 * @brief Release the previously stored indirect memory access descriptor
 *        and ending access privileges gained by the descriptor
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS      Data was released OK
 */
bass_return_code bass_imad_release(void);

/*
 * @brief Define an operation on a indirect memory access descriptor
 * @param id    Sequence id in the IMAD to use
 * @param paddr Normal world physical address to use if it is an operation
 *              requiring in-data or supplying out-data.
 *
 * Note that 0 is allowed as a physical address.
 */
struct bass_imad_entry {
    uint32_t id;
    uint32_t paddr;
};

/*
 * @brief  Perform a set of sequences on an indirect memory access descriptor
 *
 * @param entries [in] An array of entries that identifies the sequence(s)
 *                     to execute and supplies eventual parameters
 * @param num_ops [in] Number of elements in the array above
 * @param failed_entry [out] Failed entry when error is returned
 *
 * Each of the operations in a sequence to execute has to be matched by one
 * entry in consecutive order, that is, the entries within a sequence has
 * to match one to one in the same order.
 *
 * @return bass_return_code
 * @retval BASS_RC_SUCCESS  Complete sequence was executed
 * @retval BASS_RC_ERROR_TEE_TRUSTED_APP
 *                          Execution aborted at failed_entry,
 *                          no changes made by the failing entry
 *
 */
bass_return_code bass_imad_exec(const struct bass_imad_entry *entries,
            size_t num_entries, size_t *failed_entry);

#endif /* BASS_APP_H */

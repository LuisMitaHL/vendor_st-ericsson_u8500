/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 */
#ifndef UTEE_SYSCALLS_H
#define UTEE_SYSCALLS_H

#include <stddef.h>
#include <stdint.h>

#include <utee_types.h>
#include <tee_api_types.h>

void utee_return(uint32_t ret) __attribute__((noreturn));

void utee_log(const void *buf, size_t len);

void utee_panic(uint32_t code) __attribute__((noreturn));

uint32_t utee_dummy(uint32_t *a);

uint32_t utee_dummy_7args(uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4,
                          uint32_t a5, uint32_t a6, uint32_t a7);

uint32_t utee_nocall(void);

uint32_t utee_sla_3args(uint32_t id, uint32_t a1, uint32_t a2, uint32_t a3);
uint32_t utee_sla_8args(uint32_t id, uint32_t a1, uint32_t a2, uint32_t a3,
                        uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7,
                        uint32_t a8);
uint32_t utee_sla_16args(uint32_t id, uint32_t a1, uint32_t a2, uint32_t a3,
                        uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7,
                        uint32_t a8, uint32_t a9, uint32_t a10, uint32_t a11,
                        uint32_t a12, uint32_t a13, uint32_t a14, uint32_t a15,
                        uint32_t a16);

TEE_Result utee_get_property(enum utee_property prop, void *buf, uint32_t len);

TEE_Result utee_open_ta_session(const TEE_UUID *dest,
                    uint32_t cancel_req_to, uint32_t param_types,
                    TEE_Param params[4], TEE_TASessionHandle *sess,
                    uint32_t *ret_orig);

TEE_Result utee_close_ta_session(TEE_TASessionHandle sess);

TEE_Result utee_invoke_ta_command(TEE_TASessionHandle sess,
                    uint32_t cancel_req_to, uint32_t cmd_id,
                    uint32_t param_types, TEE_Param params[4],
                    uint32_t *ret_orig);

TEE_Result utee_check_access_rights(uint32_t flags, const void *buf,
                    size_t len);

bool utee_get_cancellation_flag(void);

bool utee_unmask_cancellation(void);

bool utee_mask_cancellation(void);

TEE_Result utee_wait(uint32_t timeout);

TEE_Result utee_get_time(enum utee_time_category cat, TEE_Time *time);

TEE_Result utee_set_ta_time(const TEE_Time *time);

TEE_Result utee_cryp_state_alloc(uint32_t algo, uint32_t op_mode,
                uint32_t key1, uint32_t key2, uint32_t *state);
TEE_Result utee_cryp_state_copy(uint32_t dst, uint32_t src);
TEE_Result utee_cryp_state_free(uint32_t state);

/* iv and iv_len are ignored for some algorithms */
TEE_Result utee_hash_init(uint32_t state, const void *iv, size_t iv_len);
TEE_Result utee_hash_update(uint32_t state, const void *chunk,
                size_t chunk_size);
TEE_Result utee_hash_final(uint32_t state, const void *chunk,
                size_t chunk_size, void *hash, size_t *hash_len);

TEE_Result utee_cipher_init(uint32_t state, const void *iv, size_t iv_len);
TEE_Result utee_cipher_update(uint32_t state, const void *src, size_t src_len,
                void *dest, size_t *dest_len);
TEE_Result utee_cipher_final(uint32_t state, const void *src, size_t src_len,
                void *dest, size_t *dest_len);


/* Generic Object Functions */
TEE_Result utee_cryp_obj_get_info(uint32_t obj, TEE_ObjectInfo *info);
TEE_Result utee_cryp_obj_restrict_usage(uint32_t obj, uint32_t usage);
TEE_Result utee_cryp_obj_get_attr(uint32_t obj, uint32_t attr_id,
                void *buffer, size_t *size);

/* Transient Object Functions */
TEE_Result utee_cryp_obj_alloc(TEE_ObjectType type, uint32_t max_size,
                uint32_t *obj);
TEE_Result utee_cryp_obj_close(uint32_t obj);
TEE_Result utee_cryp_obj_reset(uint32_t obj);
TEE_Result utee_cryp_obj_populate(uint32_t obj, TEE_Attribute *attrs,
                uint32_t attr_count);
TEE_Result utee_cryp_obj_copy(uint32_t dst_obj, uint32_t src_obj);

TEE_Result utee_cryp_derive_key(uint32_t state, const TEE_Attribute *params,
                uint32_t param_count, TEE_ObjectHandle derived_key);

TEE_Result utee_cryp_random_number_generate(void *buf, size_t blen);

#endif /*UTEE_SYSCALLS_H*/

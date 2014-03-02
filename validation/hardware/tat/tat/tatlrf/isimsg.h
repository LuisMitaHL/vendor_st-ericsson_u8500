/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   isimsg.h
* \brief   routines to manipulate ISI messages and subblocks
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef ISIMSG_H_
#define ISIMSG_H_

#include "modem_test_msg_ext.h"	/* for MODEM_TEST_RUN_REQ and the like */
#include "dth_tools.h"

/* CONSTANTS */

/* char pattern to fill the ISI buffer with when unused */
#define ISI_BUFFER_FILL_PATTERN         0xFE

/* TYPES */

typedef t_ModemTest_RunReq MODEM_TEST_RUN_REQ_STR;
typedef t_ModemTest_RunResp MODEM_TEST_RUN_RESP_STR;

typedef enum {
	client  = 0x0001,
	modem   = 0x0002,
	mixed   = 0x0003 /* result of failed attempt to code or decode a message */
} t_encoding;

/* these information are used internally.
 * Initialize it by tatl17_00init then pass it to the functions */
struct tatrf_isi_msg_t {
	/* address of the buffer to map message */
	u16 *msg;

	/* capacity of buffer in bytes. set once */
	size_t size;

	/* message encoding */
	t_encoding encoding;

	/* isi_common_get_msg_length does not return the value passed to 
	 * isi_common_add_isi_header() so this variable tells how many bytes are not
	 * counted */
	size_t diff_len;

	/* addres of a u16: nb subblocks in message. if NULL then the messsage does
	 * not accept subblocks */
	u16 *nb_sb;

	/* last subblock or NULL if none */
	u16 *sb;

	/* resource ID of last request sent. negative if none */
	s16 resource;

	/* This flags tells if the buffer is locked or not.
	 * When buffer is in use, a call to tatl17_01msg() will always fail.
	 * The buffer is unlocked on tatl17_05send() call or if the flag is forced
     * unlocked */
	u16 locked;
};

/* MACROS */

#define SIZE_MODEM_TEST_RUN_REQ_STR     MODEM_TEST_RUN_REQ_LEN
#define SIZE_MODEM_TEST_RUN_RESP_STR    MODEM_TEST_RUN_RESP_LEN

#define tatrf_isi_init(pp_st)  \
    tatl17_00init(pp_st)

#define tatrf_isi_msg_begin_no_sb(pp_st, rsrc, test) \
    (test##_STR *)tatl17_01msg(pp_st, rsrc, test, SIZE_##test##_STR, NULL)

#define tatrf_isi_msg_begin(pp_st, rsrc, test, attr_nb_sb) \
    (test##_STR *)tatl17_01msg((pp_st), rsrc, test, SIZE_##test##_STR, \
    &((test##_STR *)((u8 *)(pp_st)->msg + sizeof(t_isi_header)))->attr_nb_sb)

#define tatrf_isi_msg_header(pp_st) \
    ((pp_st)->msg ? (void *)((u8 *)(pp_st)->msg + sizeof(t_isi_header))\
     : (void *)NULL)

#define tatrf_isi_msg_add_sb(pp_st, sb_id)    \
    (sb_id##_STR *)tatl17_02addSubblock((pp_st), sb_id, SIZE_##sb_id##_STR)

#define tatrf_isi_msg_add_sb_copy(pp_st, sb_id, cpy, cpy_size)    \
    (sb_id##_STR *)tatl17_03addSubblockCopy((pp_st), sb_id, \
    SIZE_##sb_id##_STR, (u16 *)cpy, cpy_size)

#define tatrf_isi_append_data(pp_st, pp_data, vp_nbr_of_words)    \
    tatl17_04appendData((pp_st), (pp_data), vp_nbr_of_words)

#define tatrf_isi_msg_length(pp_st)    \
    ((pp_st)->msg ? isi_common_get_message_length((u8 *)(pp_st)->msg) : 0)

#define tatrf_isi_total_length(pp_st)    \
    ((pp_st)->msg ? isi_common_get_message_length((u8 *)(pp_st)->msg)\
     + (pp_st)->diff_len : 0)

#define tatrf_isi_send(st, name)  \
    tatl17_05send(st, name)

#define tatrf_isi_read_simple(pp_st, vp_msg_id, pp_desc, pp_error)    \
    (vp_msg_id##_STR *)tatl17_06read(pp_st, vp_msg_id, NULL, pp_desc, pp_error)

#define tatrf_isi_read(pp_st, vp_msg_id, attr_nb_sb, pp_desc, pp_error)    \
    (vp_msg_id##_STR *)tatl17_06read(pp_st, vp_msg_id, &((vp_msg_id##_STR *)\
    ((u8 *)(pp_st)->msg + sizeof(t_isi_header)))->attr_nb_sb, pp_desc, pp_error)

#define tatrf_isi_find_sb(pp_st, vp_sb_id, pp_start) \
    (vp_sb_id##_STR *)tatl17_08find_sb(pp_st, vp_sb_id, pp_start)

#define tatrf_isi_set_buffer(pp_st, pp_buf, vp_size) \
    (tatl17_09setBuffer((pp_st), (u8 *)(pp_buf), (size_t)(vp_size))

/* PROTOTYPES */

/**
 * Initialize a tatrf_isi_msg_t structure to build an ISI message.
 * The function assigns a global allocated buffer to the structure.
 * @param st [in] address of a tatrf_isi_msg_t structure
 */
void tatl17_00init(struct tatrf_isi_msg_t *pp_st);

/**
 * Set the message information as its resource and test IDs.
 * @param st [in/out] address of an initialized tatrf_isi_msg_t structure. The
 * value passed should be the one passed first to \tatl17_00init()
 * @param resource [in] resource ID of the message
 * @param msg_id [in] message ID of the message
 * @param msg_len [in] size in bytes of the message
 * @param nb_sb [in] address of a u16 containing the number of subblocks in
 * the message or NULL if the message is not composed of subblocks.
 * @return address of the element next to the ISI header or NULL on error. The
 * address is part of st->msg buffer.
 */
u16 *tatl17_01msg(struct tatrf_isi_msg_t *pp_st, u8 vp_resource, u16 vp_msg_id,
		  size_t vp_msg_len, u16 * pp_nb_sb);

/**
 * Append a subblock to the message.
 * The message's subblock count and size and message length are incremented.
 * Fillers are added to the previous element, if any, if not 32 bits-aligned.
 * @param st [in/out] address of an initialized tatrf_isi_msg_t structure
 * @param sb_id [in] sub-block ID
 * @param sb_len [in] sub-block size in bytes
 * @return if the message accepts subblocks then the function returns the
 * address of the sub-block, which is part of st->msg. NULL is returned on error
 */
u16 *tatl17_02addSubblock(struct tatrf_isi_msg_t *pp_st, u16 vp_sb_id,
			  size_t vp_sb_len);

/**
 * Copy and append a subblock to the message.
 * @param st [in/out] address of an initialized tatrf_isi_msg_t structure
 * @param sb_id [in] sub-block ID
 * @param sb_len [in] sub-block size in bytes
 * @param cpy [in] address of the subblock to copy
 * @param cpy_len [in] size in bytes of the copy
 * @return if the message accepts subblocks then the function returns the
 * address of the sub-block, which is part of st->msg. NULL is returned on error
 */
u16 *tatl17_03addSubblockCopy(struct tatrf_isi_msg_t *pp_st, u16 vp_sb_id,
			      size_t vp_sb_len, u16 *pp_cpy,
			      size_t vp_cpy_len);

/* Append data to the last element pushed in the message.
 * Update the message and/or subblock length accordingly */
int tatl17_04appendData(struct tatrf_isi_msg_t *pp_st, const u16 *pp_words,
			size_t vp_nbr_of_words);

/* Send current message if any and ready */
int tatl17_05send(struct tatrf_isi_msg_t *pp_st, const char *pp_msg);

/* Read the response to the request sent. */
u16 *tatl17_06read(struct tatrf_isi_msg_t *pp_st, u16 vp_msg_id, u16 *pp_nb_sb,
    const char *pp_desc, int *pp_error);

int tatl17_07incrementLength(struct tatrf_isi_msg_t *pp_st, size_t vp_by);

void *tatl17_08find_sb(struct tatrf_isi_msg_t *pp_st, u16 vp_sb_id,
    void *pp_start);

int tatl17_09setBuffer(struct tatrf_isi_msg_t *pp_st, u8 *buffer,
    size_t buf_size);

int tatl17_11get_id(struct tatrf_isi_msg_t *pp_st, u8 *pp_trans_id,
    u8 *pp_msg_id);

/* encode or decode a valid ISI message and its subblocks */
int tatl17_12marsh_msg(struct tatrf_isi_msg_t *pp_st,
    TAT_MODEM_ISI_MARSH vp_op);

#ifdef _UNIT_TEST
int tatl17_get_sb_hdr(u16 *pp_sb, u16 *pp_sb_id, u16 *pp_sb_len);
int tatl17_set_sb_hdr(u16 *pp_sb, u16 vp_sb_id, u16 vp_sb_len);
#endif

#endif /* ISIMSG_H_ */

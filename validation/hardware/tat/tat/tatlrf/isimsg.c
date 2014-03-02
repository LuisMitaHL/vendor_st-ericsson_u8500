/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   isimsg.c
* \brief   routines to manipulate ISI messages and subblocks
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "misc.h"

#include "isimsg.h"
#include "isidump.h"
#include "rftuning.h"

#include "tatmodemlib.h"
#include "isi_driver_lib.h"

/* GLOBALS */

/* this module provides a general use buffer for ISI message */
u8 v_tatrf_isi_buffer[ISI_DRIVER_MAX_MESSAGE_LENGTH];

/* TYPES */

typedef int (*tatrf_isi_marsh_proc) (struct tatrf_isi_msg_t *,
				     TAT_MODEM_ISI_MARSH);

/* MACROS */

#define BUFFER_IS_LARGE_ENOUGH(pp_st, vp_increment)   \
    ((tatrf_isi_total_length((pp_st)) + (vp_increment)) <= (pp_st)->size)

/* FORWARD DECLARATIONS */

int tatl17_set_id(struct tatrf_isi_msg_t *pp_st, u8 vp_trans_id, u8 vp_msg_id);

/* FUNCTIONS */

int tatl17_get_sb_hdr(u16 * pp_sb, u16 * pp_sb_id, u16 * pp_sb_len)
{
	int vl_Error = TAT_ERROR_CASE;
	if (pp_sb != NULL) {
		/* first 16 bits are subblock ID */
		*pp_sb_id = *pp_sb;

		/* next 16 bits are the subblock length */
		*pp_sb_len = *(pp_sb + 1);

		vl_Error = TAT_ERROR_OFF;
	}

	return vl_Error;
}

int tatl17_set_sb_hdr(u16 * pp_sb, u16 vp_sb_id, u16 vp_sb_len)
{
	int vl_Error = TAT_ERROR_CASE;
	if (pp_sb != NULL) {
		/* first 16 bits are subblock ID */
		*pp_sb = vp_sb_id;

		/* next 16 bits are the subblock length */
		*(pp_sb + 1) = vp_sb_len;

		vl_Error = TAT_ERROR_OFF;
	}

	return vl_Error;
}

void tatl17_00init(struct tatrf_isi_msg_t *pp_st)
{
	pp_st->msg = (u16 *) v_tatrf_isi_buffer;
	pp_st->size = ISI_DRIVER_MAX_MESSAGE_LENGTH;
	pp_st->encoding = client;
	pp_st->diff_len = 0;
	pp_st->nb_sb = NULL;
	pp_st->sb = NULL;
	pp_st->resource = -1;
	pp_st->locked = 0;	/* unlocked */
}

u16 *tatl17_01msg(struct tatrf_isi_msg_t *pp_st, u8 vp_resource, u16 vp_msg_id,
		  size_t vp_msg_len, u16 * pp_nb_sb)
{
	u16 *pl_test = NULL;

	if ((pp_st->encoding != client) && (pp_st->encoding != modem)) {
		return NULL;
	}

	/* pp_st must have been initialized */
	if ((NULL == pp_st->msg)) {
		return NULL;
	}

	/* cannot reuse buffer if locked */
	if (pp_st->locked) {
		return NULL;
	}

	/* set resource, transaction and test IDs */
	size_t vl_total_len = sizeof(t_isi_header) + vp_msg_len;
	if (BUFFER_IS_LARGE_ENOUGH(pp_st, vl_total_len)) {
		/* clear the buffer */
		memset(pp_st->msg, ISI_BUFFER_FILL_PATTERN, pp_st->size);

		pp_st->nb_sb = pp_nb_sb;
		if (pp_st->nb_sb)
			*pp_st->nb_sb = 0;
		pp_st->sb = NULL;

		pp_st->resource = vp_resource;

		/* buffer is now locked until beeing sent or released */
		pp_st->locked = 1;

		isi_common_add_isi_header((uint8 *) pp_st->msg, vl_total_len,
					  vp_resource);

		/* isi_common_get_msg_length() does not return the value passed to 
		 * isi_common_add_isi_header() so we need to remember how many bytes
		 * are not counted */
		pp_st->diff_len = vl_total_len - tatrf_isi_msg_length(pp_st);

		/* return address of structure next to isi header */
		pl_test = tatrf_isi_msg_header(pp_st);

		/* initialize message */
		tatl17_set_id(pp_st, 0, vp_msg_id);
	} else {
		/* error: buffer is not large enough */
		SYSLOG(LOG_ERR, "buffer is not large enough");
	}

	return pl_test;
}

/* add padding to align on 32 bits, if necessary */
int tatl17_padding(struct tatrf_isi_msg_t *pp_st)
{
	if (NULL == pp_st->msg)
		return -1;

	size_t vl_len = tatrf_isi_total_length(pp_st);
	int vl_nb_padding = vl_len % 4;	/* nb words to paddle */
	if (vl_nb_padding) {
		vl_nb_padding = 4 - vl_nb_padding;
		/*SYSLOG(LOG_DEBUG, "append %d padding bytes", vl_nb_padding); */

		/* fill with padding */
		if (BUFFER_IS_LARGE_ENOUGH(pp_st, vl_nb_padding)) {
			/* enough capacity */
			isi_common_fill_padding_byte((u8 *) pp_st->msg, vl_len,
						     vl_nb_padding);
			tatl17_07incrementLength(pp_st, vl_nb_padding);
		} else {
			/* error: would overflow */
			return -1;
		}
	}

	return 0;
}

u16 *tatl17_02addSubblock(struct tatrf_isi_msg_t * pp_st, u16 vp_sb_id,
			  size_t vp_sb_len)
{
	u16 *pl_sb = NULL;
	if ((NULL != pp_st->msg) && (NULL != pp_st->nb_sb)) {
		/* align structure on 32 bits */
		size_t vl_old_len = tatrf_isi_total_length(pp_st);
		if (0 == tatl17_padding(pp_st)) {
			size_t vl_new_len = tatrf_isi_total_length(pp_st);
			if (vl_old_len < vl_new_len) {
				/* padding was added */
				if (pp_st->sb != NULL) {
					/* increment the subblock length by the number of bytes padded */
					*(pp_st->sb + 1) +=
					    vl_new_len - vl_old_len;
				}
			}

			if (BUFFER_IS_LARGE_ENOUGH(pp_st, vp_sb_len)) {
				pl_sb =
				    (u16 *) ((u8 *) pp_st->msg + vl_new_len);

				/* update last subblock reference */
				pp_st->sb = pl_sb;

				/* first 16 bits are subblock ID */
				*pl_sb = vp_sb_id;
				/* next 16 bits are the subblock length */
				*(pl_sb + 1) = vp_sb_len;

				/* one more subblock in message */
				(*pp_st->nb_sb)++;

				/* increment message length by subblock length */
				tatl17_07incrementLength(pp_st, vp_sb_len);
			} else {
				/* not enough capacity in buffer for subblock */
			}
		} else {
			/* error: overflow? */
		}
	} else if (NULL == pp_st->nb_sb) {
		SYSLOG(LOG_ERR,
		       "cannot add a subblock to a 'no-subblock' message");
	}

	return pl_sb;
}

int tatl17_setSubblock(struct tatrf_isi_msg_t *pp_st, u16 * pp_sb,
		       size_t vp_sb_len)
{
	int vl_ret = -1;

	if (pp_st->sb) {
		memcpy((u16 *) pp_st->sb + 2, pp_sb + 2, vp_sb_len - 4);
		vl_ret = 0;
	}

	return vl_ret;
}

u16 *tatl17_03addSubblockCopy(struct tatrf_isi_msg_t * pp_st, u16 vp_sb_id,
			      size_t vp_sb_len, u16 * pp_cpy, size_t vp_cpy_len)
{
	u16 *pl_sb = NULL;
	if (vp_cpy_len >= vp_sb_len) {
		pl_sb = tatl17_02addSubblock(pp_st, vp_sb_id, vp_sb_len);
		if (NULL != pl_sb) {
			tatl17_setSubblock(pp_st, pp_cpy, vp_cpy_len);
		}
	}

	return pl_sb;
}

int tatl17_04appendData(struct tatrf_isi_msg_t *pp_st, const u16 * pp_words,
			size_t vp_nbr_of_words)
{
	int vl_ret = TAT_ERROR_OFF;
	/*SYSLOG(LOG_DEBUG, "appendData(st=0x%p, data_addr=0x%p, nb_words=%u)", pp_st, pp_words, vp_nbr_of_words); */

	size_t vl_size_in_bytes = vp_nbr_of_words * sizeof(uint16);
	if (BUFFER_IS_LARGE_ENOUGH(pp_st, vl_size_in_bytes)) {
		if (NULL != pp_st->sb) {
			/* append data to current subblock */
			u16 vl_sb_id, vl_sb_len;
			tatl17_get_sb_hdr(pp_st->sb, &vl_sb_id, &vl_sb_len);

			memcpy(((u8 *) pp_st->sb) + vl_sb_len, pp_words,
			       vl_size_in_bytes);

			/* incrementing subblock length by the number of bytes appended */
			vl_sb_len += vl_size_in_bytes;
			tatl17_set_sb_hdr(pp_st->sb, vl_sb_id, vl_sb_len);
		} else {
			/* appending data to message */
			size_t vl_total_len = tatrf_isi_total_length(pp_st);
			memcpy(((u8 *) pp_st->msg) + vl_total_len, pp_words,
			       vl_size_in_bytes);
		}

		tatl17_07incrementLength(pp_st, vl_size_in_bytes);
	} else {
		SYSLOG(LOG_ERR,
		       "buffer would overflow: %u bytes to write, current size=%u, (capacity=%u)",
		       vl_size_in_bytes, tatrf_isi_total_length(pp_st),
		       pp_st->size);
		vl_ret = TAT_ERROR_CASE;
	}

	return vl_ret;
}

int tatl17_05send(struct tatrf_isi_msg_t *pp_st, const char *pp_msg)
{
	int vl_ret = TAT_ERROR_CASE;

	if (pp_st->msg != NULL) {
		if (pp_st->encoding == client) {
			/* dump original message */
			tatl17_10dump(pp_st, pp_msg, LOG_DEBUG);

			/* encode message */
			vl_ret = tatl17_12marsh_msg(pp_st, ISI_MARSH);
		} else if (pp_st->encoding == modem) {
			/* dumping the message will have no sense to user and the message
			 * is already encoded so continue */
			vl_ret = TAT_ERROR_OFF;
		}
		/* else endianess is undetermined. Nothing can be done */

		if (TAT_OK(vl_ret)) {
			/* send encoded message */
			u32 vl_rsrc_id = 0;
			vl_ret = tat_modem_send(RF_FEATURE, pp_st->msg,
						tatrf_isi_total_length(pp_st),
						&vl_rsrc_id);

			if (0 != vl_ret) {
				vl_ret = TAT_ISI_HANDLER_ERROR;
			}

			/* unlock buffer */
			pp_st->locked = 0;
		}
	}

	return vl_ret;
}

u16 *tatl17_06read(struct tatrf_isi_msg_t * pp_st, u16 vp_msg_id,
		   u16 * pp_nb_sb, const char *pp_desc, int *pp_error)
{
	if ((pp_st == NULL) || (pp_error == NULL)) {
		SYSLOG(LOG_ERR, "null pointer");
		return NULL;
	}

	u16 *pl_msg = NULL;

	/* reuse request buffer for response */
	memset(pp_st->msg, ISI_BUFFER_FILL_PATTERN, pp_st->size);
	/* size unchanged */
	pp_st->encoding = modem;
	/* diff_len unchanged */
	pp_st->nb_sb = pp_nb_sb;
	pp_st->sb = NULL;	/* not relevant for response */
	/* resource is unchanged */

	size_t vl_max_len = pp_st->size;
	int vl_ret = tat_modem_read(RF_FEATURE, pp_st->resource, pp_st->msg,
				    &vl_max_len);
	if (0 == vl_ret) {
		pl_msg = (u16 *) tatrf_isi_msg_header(pp_st);

		/* unmarsh message whatever its type */
		vl_ret = tatl17_12marsh_msg(pp_st, ISI_UNMARSH);
		if (TAT_OK(vl_ret)) {
			tatl17_10dump(pp_st, pp_desc, LOG_DEBUG);

			/* test the response has the right message id */
			u8 vl_trans_id, vl_msg_id;
			tatl17_11get_id(pp_st, &vl_trans_id, &vl_msg_id);

			if (vl_msg_id == vp_msg_id) {
				*pp_error = TAT_ERROR_OFF;
			} else {
				SYSLOG(LOG_ERR,
				       "read message of id %u but expected %u. trans id = %u",
				       vl_msg_id, vp_msg_id, vl_trans_id);
				*pp_error = TAT_ERROR_NOT_MATCHING_MSG;
				pl_msg = NULL;
			}
		} else {
			*pp_error = vl_ret;
			pl_msg = NULL;
		}
	} else {
		*pp_error = TAT_ISI_HANDLER_ERROR;
	}

	return pl_msg;
}

int tatl17_07incrementLength(struct tatrf_isi_msg_t *pp_st, size_t vp_by)
{
	int vl_ret = TAT_ERROR_CASE;

	if ((pp_st->msg != NULL) && BUFFER_IS_LARGE_ENOUGH(pp_st, vp_by)) {
		size_t vl_len = tatrf_isi_msg_length(pp_st);
		vl_len += vp_by;

		/* update message size in isi header */
		isi_common_set_message_length((u8 *) pp_st->msg, vl_len);

		vl_ret = TAT_ERROR_OFF;
	}

	return vl_ret;
}

void *tatl17_08find_sb(struct tatrf_isi_msg_t *pp_st, u16 vp_sb_id,
		       void *pp_start)
{
	if ((pp_st == NULL) || (pp_st->msg == NULL)) {
		SYSLOG(LOG_ERR, "null pointer");
		return NULL;
	}

	/* we are only supporting client-encoded messages. This means responses have
	   to be decoded before passed to this function */
	if (pp_st->encoding != client) {
		SYSLOG(LOG_ERR, "not client encoded");
		return NULL;
	}

	void *pl_sb_found = NULL;
	uint8 *pl_start = NULL;
	uint16 vl_sb_id, vl_sb_len;

	if (NULL != pp_start) {
		/* pp_start should be a valid pointer to a subblock.
		 * We are starting at the next subblock, if any
		 */
		tatl17_get_sb_hdr((uint16 *) pp_start, &vl_sb_id, &vl_sb_len);
		pl_start = (uint8 *) pp_start + vl_sb_len;
	} else if (pp_st->nb_sb) {
		/* start from first subblock if any */
		pl_start = ((uint8 *) pp_st->nb_sb) + sizeof(*pp_st->nb_sb);
	}

	const uint8 *pl_eof_msg =
	    (uint8 *) pp_st->msg + tatrf_isi_total_length(pp_st);
	while ((NULL == pl_sb_found) && (pl_start < pl_eof_msg)) {
		/* test current subblock */
		tatl17_get_sb_hdr((uint16 *) pl_start, &vl_sb_id, &vl_sb_len);

		/* if it matches sb id then return it */
		if (vl_sb_id == vp_sb_id) {
			pl_sb_found = pl_start;
		}

		/* else test next subblock if any */
		else {
			pl_start += vl_sb_len;
		}
	}

#if defined(_UNIT_TEST)
	SYSLOG(LOG_DEBUG, "tatrf_find_sb(msg=%p, sb_id=0x%2X, from=%p): %p",
	       pp_st->msg, vp_sb_id, pp_start, pl_sb_found);
#endif

	return pl_sb_found;
}

int tatl17_09setBuffer(struct tatrf_isi_msg_t *pp_st, u8 * buffer,
		       size_t buf_size)
{
	pp_st->msg = (u16 *) buffer;
	pp_st->size = buf_size;

	/* initialize isi header so the length field is zero. If not doing this,
	 * we may probably experience bound check problems later */
	memset(buffer, 0, sizeof(t_isi_header));

	return 0;
}

int tatl17_11get_id(struct tatrf_isi_msg_t *pp_st, u8 * pp_trans_id,
		    u8 * pp_msg_id)
{
	const u16 *const vl_id_ptr = tatrf_isi_msg_header(pp_st);

	if (pp_st->encoding == modem) {
		*pp_trans_id = GetU8InU16(LSB_POSS, *vl_id_ptr);
		*pp_msg_id = GetU8InU16(MSB_POSS, *vl_id_ptr);
	} else if (pp_st->encoding == client) {
		*pp_trans_id = GetU8InU16(MSB_POSS, *vl_id_ptr);
		*pp_msg_id = GetU8InU16(LSB_POSS, *vl_id_ptr);
	} else {
		return TAT_ERROR_CASE;
	}

	return TAT_ERROR_OFF;
}

int tatl17_set_id(struct tatrf_isi_msg_t *pp_st, u8 vp_trans_id, u8 vp_msg_id)
{
	u16 *pl_id_ptr = tatrf_isi_msg_header(pp_st);

	if (pp_st->encoding == modem) {
		*pl_id_ptr = SetU8InU16(LSB_POSS, *pl_id_ptr, vp_trans_id);
		*pl_id_ptr = SetU8InU16(MSB_POSS, *pl_id_ptr, vp_msg_id);
	} else if (pp_st->encoding == client) {
		*pl_id_ptr = SetU8InU16(MSB_POSS, *pl_id_ptr, vp_trans_id);
		*pl_id_ptr = SetU8InU16(LSB_POSS, *pl_id_ptr, vp_msg_id);
	} else {
		return TAT_ERROR_CASE;
	}

	return TAT_ERROR_OFF;
}

void tatl17_marsh_GSM_TX_PWR_SELFTUNING_CTRL_DATA(int vp_nb_tuning_data,
						  GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR
						  * pp_data,
						  TAT_MODEM_ISI_MARSH vp_op)
{
	int i;
	for (i = 0; i < vp_nb_tuning_data; i++) {
		tat_modem_marsh16(&pp_data->gsm_pwr_level, vp_op);
		tat_modem_marsh16((uint16_t *) & pp_data->target_dbm, vp_op);
		tat_modem_marsh16(&pp_data->fill1, vp_op);
		tat_modem_marsh16(&pp_data->pa_data_valid, vp_op);
		tat_modem_marsh16(&pp_data->pa_ctrl_data.pa_voltage_ctrl,
				  vp_op);
		tat_modem_marsh16(&pp_data->pa_ctrl_data.pa_ictrl, vp_op);
		pp_data++;
	}
}

void tatl17_marsh_TX_PWR_TUNING_DATA(int vp_nb_data,
				     TX_PWR_TUNING_DATA_STR * pp_data,
				     TAT_MODEM_ISI_MARSH vp_op)
{
	int i;
	for (i = 0; i < vp_nb_data; i++) {
		tat_modem_marsh16((uint16_t *) & pp_data->measured_pwr, vp_op);
		tat_modem_marsh16((uint16_t *) & pp_data->pwr_ctrl, vp_op);
		pp_data++;
	}
}

void tatl17_marsh_WCDMA_TX_PWR_CTRL_DATA(int vp_nb_data,
					 WCDMA_TX_PWR_CTRL_DATA_STR * pp_data,
					 TAT_MODEM_ISI_MARSH vp_op)
{
	int i;
	for (i = 0; i < vp_nb_data; i++) {
		tat_modem_marsh16((uint16_t *) & pp_data->pa_voltage_ctrl,
				  vp_op);
		tat_modem_marsh16((uint16_t *) & pp_data->pa_ictrl, vp_op);
		pp_data++;
	}
}

int tatl17_marsh_sb_common(u16 * pp_block, TAT_MODEM_ISI_MARSH vp_op,
			   u16 * pp_id, u16 * pp_len)
{
	int vl_err = TAT_ERROR_OFF;
	u16 pl_sb_id;
	u16 pl_sb_len;
	u16 *pl_ptr16 = (u16 *) pp_block;

	/* Marsh/unmarsh both sb_id and sb_len */
	switch (vp_op) {
	case ISI_MARSH:

		pl_sb_id = DEREF_PTR(pl_ptr16, u16);
		pl_ptr16 = tat_modem_marsh16(pl_ptr16, vp_op);

		pl_sb_len = DEREF_PTR(pl_ptr16, u16);
		pl_ptr16 = tat_modem_marsh16(pl_ptr16, vp_op);
		break;

	case ISI_UNMARSH:

		pl_ptr16 = tat_modem_marsh16(pl_ptr16, vp_op);
		pl_sb_id = DEREF_PTR(pl_ptr16 - 1, u16);

		pl_ptr16 = tat_modem_marsh16(pl_ptr16, vp_op);
		pl_sb_len = DEREF_PTR(pl_ptr16 - 1, u16);
		break;

	default:

		SYSLOG(LOG_ERR, "invalid marshalling operation!");
		return TAT_BAD_REQ;
	}

	if (NULL != pp_id)
		*pp_id = pl_sb_id;
	if (NULL != pp_len)
		*pp_len = pl_sb_len;

	switch (pl_sb_id) {
    case C_TEST_SB_PA_INFO:
        {
            C_TEST_SB_PA_INFO_STR *pl_sb = (C_TEST_SB_PA_INFO_STR *)pp_block;
            tat_modem_marsh16(&pl_sb->status, vp_op);
            tat_modem_marsh16(&pl_sb->system_info, vp_op);
            tat_modem_marsh16(&pl_sb->version, vp_op);
            /* Manufacturer of PA in ASCII string */
        }
        break;

    case C_TEST_SB_PA_BIAS_INFO:
        {
            C_TEST_SB_PA_BIAS_INFO_STR *pl_sb =
                (C_TEST_SB_PA_BIAS_INFO_STR *)pp_block;
            tat_modem_marsh16(&pl_sb->status, vp_op);
            tat_modem_marsh16(&pl_sb->system_info, vp_op);
            tat_modem_marsh16(&pl_sb->bias1_const, vp_op);
            tat_modem_marsh16(&pl_sb->bias1_current, vp_op);
            tat_modem_marsh16(&pl_sb->bias2_default_dac, vp_op);
            tat_modem_marsh16(&pl_sb->target_value, vp_op);
        }
        break;

	case C_TEST_SB_GSM_RX_CONTROL:
		{
			C_TEST_SB_GSM_RX_CONTROL_STR *pl_sb =
			    (C_TEST_SB_GSM_RX_CONTROL_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->mode, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->rf_band, vp_op);
			tat_modem_marsh16(&pl_sb->channel, vp_op);
			tat_modem_marsh16(&pl_sb->mon_ch, vp_op);
			tat_modem_marsh16((uint16 *) & pl_sb->afc, vp_op);
			tat_modem_marsh16(&pl_sb->agc, vp_op);
			tat_modem_marsh16(&pl_sb->operationMode, vp_op);
		}
		break;

	case C_TEST_SB_GSM_RX_CONTROL_RESP:
		{
			C_TEST_SB_GSM_RX_CONTROL_RESP_STR *pl_sb =
			    (C_TEST_SB_GSM_RX_CONTROL_RESP_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->status, vp_op);
		}
		break;

    case C_TEST_SB_AFC_MEAS_AND_ADJUST:
        {
            C_TEST_SB_AFC_MEAS_AND_ADJUST_STR *pl_sb = (C_TEST_SB_AFC_MEAS_AND_ADJUST_STR *)pp_block;
            tat_modem_marsh16(&pl_sb->measurementMode, vp_op);
            tat_modem_marsh16(&pl_sb->afcValue, vp_op);
            tat_modem_marsh16(&pl_sb->coarseValue, vp_op);
            tat_modem_marsh16(&pl_sb->adjustTimeout, vp_op);
            tat_modem_marsh16(&pl_sb->adjustAccuracy, vp_op);
            tat_modem_marsh16(&pl_sb->filler, vp_op);
        }
        break;

    case C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS:
        {
            C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS_STR *pl_sb = (C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS_STR *)pp_block;
            tat_modem_marsh16(&pl_sb->status, vp_op);
            tat_modem_marsh16((uint16 *)&pl_sb->freqErrorRad, vp_op);
            tat_modem_marsh16(&pl_sb->usedAfc, vp_op);
            tat_modem_marsh16(&pl_sb->usedCoarse, vp_op);
            tat_modem_marsh16(&pl_sb->remainingTimeout, vp_op);
            tat_modem_marsh16(&pl_sb->filler, vp_op);
        }
        break;

	case C_TEST_SB_LINKO_AFC_TUNING:
		{
			C_TEST_SB_LINKO_AFC_TUNING_STR *pl_sb =
			    (C_TEST_SB_LINKO_AFC_TUNING_STR *) pp_block;
			tat_modem_marsh16((uint16 *) & pl_sb->defaultAfc,
					  vp_op);
			tat_modem_marsh16(&pl_sb->defaultCoarse, vp_op);
		}
		break;

	case C_TEST_SB_LINKO_AFC_TUNING_RESULTS:
		{
			C_TEST_SB_LINKO_AFC_TUNING_RESULTS_STR *pl_sb =
			    (C_TEST_SB_LINKO_AFC_TUNING_RESULTS_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->status, vp_op);	/* Status of the operation, 0 if ok, otherwise test specific error code */
			tat_modem_marsh16(&pl_sb->c_coarse, vp_op);	/* C_coarse tuning value */
			tat_modem_marsh16(&pl_sb->i_bias_core, vp_op);	/* I_Core tuning value */
			tat_modem_marsh16((uint16 *) & pl_sb->afc0_value, vp_op);	/* Afc0, initial afc tuning value */
			tat_modem_marsh16(&pl_sb->temperature, vp_op);	/* Calibration temperature, DAC value */
			tat_modem_marsh16((uint16 *) & pl_sb->c_temp_k, vp_op);	/* C_temp k factor */
			tat_modem_marsh16((uint16 *) & pl_sb->c_temp_b, vp_op);	/* C_temp b factor */
			tat_modem_marsh16((uint16 *) & pl_sb->afc1_reading, vp_op);	/* Afc1 phase/freq error */
			tat_modem_marsh16((uint16 *) & pl_sb->afc2_reading, vp_op);	/* Afc2 phase/freq error */
			tat_modem_marsh16((uint16 *) & pl_sb->afc3_reading, vp_op);	/* Afc3 phase/freq error */
			tat_modem_marsh16((uint16 *) & pl_sb->afc4_reading, vp_op);	/* Afc4 phase/freq error */
			tat_modem_marsh16((uint16 *) & pl_sb->afc5_reading, vp_op);	/* Afc5 phase/freq error */
			tat_modem_marsh16(&pl_sb->afc1_value, vp_op);	/* Afc1 tuning value */
			tat_modem_marsh16(&pl_sb->afc2_value, vp_op);	/* Afc2 tuning value */
			tat_modem_marsh16(&pl_sb->afc3_value, vp_op);	/* Afc3 tuning value */
			tat_modem_marsh16(&pl_sb->afc4_value, vp_op);	/* Afc4 tuning value */
			tat_modem_marsh16(&pl_sb->afc5_value, vp_op);	/* Afc5 tuning value */
		}
		break;

	case C_HAL_SB_GSM_RX_CONTROL:
		{
			C_HAL_SB_GSM_RX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_GSM_RX_CONTROL_STR *) pp_block;
			tat_modem_marsh32((uint32_t *) & pl_sb->gsm_band_info, vp_op);	/* Values from the bitmask table INFO_GSM_BAND */
			tat_modem_marsh16(&pl_sb->rx_channel, vp_op);
			tat_modem_marsh16(&pl_sb->mon_channel, vp_op);
			/* xxxxxxxx--------  Rx slots
			   --------xxxxxxxx  AFC table selection
			 */
			tat_modem_marsh16(&pl_sb->rx_slot_mask_afc_table,
					  vp_op);
			tat_modem_marsh16(&pl_sb->fill1, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->manualAfc,
					  vp_op);
			/* xxxxxxxx--------  Rx AGC mode selection
			   --------xxxxxxxx  Filler
			 */
			tat_modem_marsh16(&pl_sb->agc_mode_fill2, vp_op);
			tat_modem_marsh16((uint16 *) & pl_sb->rx_level, vp_op);
		}
		break;

	case C_HAL_SB_GSM_TX_CONTROL:
		{
			C_HAL_SB_GSM_TX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_GSM_TX_CONTROL_STR *) pp_block;
			tat_modem_marsh32((uint32_t *) & pl_sb->gsm_band_info, vp_op);	/* Values from the bitmask table INFO_GSM_BAND */
			tat_modem_marsh16(&pl_sb->tx_channel, vp_op);
			/* xxxxxxxx--------  Tx slots
			   --------xxxxxxxx  AFC table selection
			 */
			tat_modem_marsh16(&pl_sb->tx_slot_mask_afc_table,
					  vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->manualAfc,
					  vp_op);
			/* xxxxxxxx--------  8PSK slots
			   --------xxxxxxxx  Data type
			 */
			tat_modem_marsh16(&pl_sb->tx_8psk_mask_data_type,
					  vp_op);
			/* xxxxxxxx--------  RACH slots
			   --------xxxxxxxx  Tx power level unit
			 */
			tat_modem_marsh16(&pl_sb->tx_rach_mask_tx_power_unit,
					  vp_op);
			tat_modem_marsh16(&pl_sb->power_level, vp_op);
		}
		break;

	case C_HAL_SB_WCDMA_RX_CONTROL:
		{
			C_HAL_SB_WCDMA_RX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_WCDMA_RX_CONTROL_STR *) pp_block;
			tat_modem_marsh32((uint32_t *) & pl_sb->band_info, vp_op);	/* Values from the bitmask table INFO_WCDMA_BAND */
			tat_modem_marsh16(&pl_sb->dl_uarfcn, vp_op);
			/* xxxxxxxx--------  AFC table selection
			   --------xxxxxxxx  Filler
			 */
			tat_modem_marsh16(&pl_sb->afc_table_fill1, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->manualAfc,
					  vp_op);
			/* xxxxxxxx--------  Rx AGC mode selection
			   --------xxxxxxxx  Filler
			 */
			tat_modem_marsh16(&pl_sb->agc_mode_fill2, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->rx_level,
					  vp_op);
		}
		break;

	case C_HAL_SB_WCDMA_TX_CONTROL:
		{
			C_HAL_SB_WCDMA_TX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_WCDMA_TX_CONTROL_STR *) pp_block;
			tat_modem_marsh32((uint32_t *) & pl_sb->band_info, vp_op);	/* Values from the bitmask table INFO_WCDMA_BAND */
			tat_modem_marsh16(&pl_sb->ul_uarfcn, vp_op);
			tat_modem_marsh16(&pl_sb->power_level, vp_op);
			/* xxxxxxxx--------  AFC table selection
			   --------xxxxxxxx  Filler
			 */
			tat_modem_marsh16(&pl_sb->afc_table_fill1, vp_op);
			tat_modem_marsh16(&pl_sb->fill2, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->manualAfc,
					  vp_op);
			tat_modem_marsh16(&pl_sb->dpdch_enable_acknack_enable,
					  vp_op);
			tat_modem_marsh16(&pl_sb->cqi_enable_ul_modulation,
					  vp_op);
			tat_modem_marsh16(&pl_sb->num_edpdch_edpdch_tti, vp_op);
			tat_modem_marsh16(&pl_sb->beta_c_beta_d, vp_op);
			tat_modem_marsh16(&pl_sb->beta_hs, vp_op);
			tat_modem_marsh16(&pl_sb->beta_ec, vp_op);
			tat_modem_marsh16(&pl_sb->beta_ed1, vp_op);
			tat_modem_marsh16(&pl_sb->beta_ed2, vp_op);
			tat_modem_marsh16(&pl_sb->beta_ed3, vp_op);
			tat_modem_marsh16(&pl_sb->beta_ed4, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->scrambling_code,
					  vp_op);
			tat_modem_marsh16(&pl_sb->dpdch_sf, vp_op);
			tat_modem_marsh16(&pl_sb->edpdch_sf, vp_op);
			tat_modem_marsh16(&pl_sb->hsdpa_delay_fill3, vp_op);
			tat_modem_marsh16(&pl_sb->fill4, vp_op);
		}
		break;

	case C_HAL_SB_GSM_RXTX_CONTROL:
		{
			C_HAL_SB_GSM_RXTX_CONTROL_STR *pl_sb =
			    (C_HAL_SB_GSM_RXTX_CONTROL_STR *) pp_block;
			tat_modem_marsh32((uint32_t *) & pl_sb->gsm_band_info,
					  vp_op);
			tat_modem_marsh16(&pl_sb->rf_channel, vp_op);
			tat_modem_marsh16(&pl_sb->rx_slot_mask_tx_slot_mask,
					  vp_op);
			tat_modem_marsh16(&pl_sb->tx_8psk_mask_mon_rach_mask,
					  vp_op);
			tat_modem_marsh16(&pl_sb->data_type_afc_type, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->manualAfc,
					  vp_op);
			tat_modem_marsh16(&pl_sb->tx_timing_advance, vp_op);
			tat_modem_marsh16(&pl_sb->agc_mode_tx_power_unit,
					  vp_op);

			int vl_slot, vl_nb_slot =
			    sizeof(pl_sb->slot_level) /
			    sizeof(pl_sb->slot_level[0]);
			for (vl_slot = 0; vl_slot < vl_nb_slot; vl_slot++) {
				tat_modem_marsh16((u16 *) pl_sb->slot_level +
						  vl_slot, vp_op);
			}
		}
		break;

	case C_HAL_RF_SB_TUNING_VALUES_GET_REQ:
		{
			C_HAL_RF_SB_TUNING_VALUES_GET_REQ_STR *pl_sb =
			    (C_HAL_RF_SB_TUNING_VALUES_GET_REQ_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->tuning_id, vp_op);
			pl_sb->fill1 = ISIFILLER8;
		}
		break;

	case C_HAL_RF_SB_TUNING_VALUES_GET_RESP:
		{
			C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR *pl_sb =
			    (C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->tuning_id, vp_op);
			tat_modem_marsh16(&pl_sb->fill1_status, vp_op);
			pl_sb->fill2 = ISIFILLER8;
			tat_modem_marsh16(&pl_sb->nbr_of_words, vp_op);
			/* NB: does not handle the data themselves */
		}
		break;

	case C_HAL_RF_SB_TUNING_VALUES_SET_REQ:
		{
			C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *pl_sb =
			    (C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->tuning_id, vp_op);
			tat_modem_marsh16(&pl_sb->nbr_of_words, vp_op);
			/* NB: does not handle the data themselves */
		}
		break;

	case C_HAL_RF_SB_TUNING_VALUES_SET_RESP:
		{
			C_HAL_RF_SB_TUNING_VALUES_SET_RESP_STR *pl_sb =
			    (C_HAL_RF_SB_TUNING_VALUES_SET_RESP_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->tuning_id, vp_op);
			tat_modem_marsh16(&pl_sb->fill1_status, vp_op);
		}
		break;

	case C_TEST_SB_PWR_SEQ_MEASURE:
		{
			C_TEST_SB_PWR_SEQ_MEASURE_STR *pl_sb =
			    (C_TEST_SB_PWR_SEQ_MEASURE_STR *) pp_block;
            union tx_pwr_seq_data vl_pwr_seq_data;
            vl_pwr_seq_data.wcdma = pl_sb->system_dep_data;

			switch (pl_sb->system_info) {
			case C_TEST_GSM:
				{
					tat_modem_marsh16(&pl_sb->system_info, vp_op);
					tat_modem_marsh16(&pl_sb->rx_attenuation_mode_and_fill1,
					    vp_op);
                    tat_modem_marsh32(&vl_pwr_seq_data.gsm.rf_band, vp_op);
                    tat_modem_marsh16(&vl_pwr_seq_data.gsm.pwr_level_nbr,
                        vp_op);
					tat_modem_marsh16(&vl_pwr_seq_data.gsm.fill1, vp_op);
                    pl_sb->system_dep_data = vl_pwr_seq_data.wcdma;

					tat_modem_marsh16(&pl_sb->channel_nbr, vp_op);
					tat_modem_marsh16(&pl_sb->pwr_ctrl_value, vp_op);
					tat_modem_marsh16(&pl_sb->rf_rx_attenuation_value, vp_op);
					tat_modem_marsh16(&pl_sb->lna_state, vp_op);
					tat_modem_marsh16(&pl_sb->rf_lna_step_size, vp_op);
					tat_modem_marsh16(&pl_sb->nbr_empty_slots, vp_op);
				}
				break;
			case C_TEST_WCDMA:
				{
					tat_modem_marsh16(&pl_sb->system_info, vp_op);
					tat_modem_marsh16(&pl_sb->rx_attenuation_mode_and_fill1,
					    vp_op);
					tat_modem_marsh32((uint32_t *)
                        &vl_pwr_seq_data.wcdma.rf_band, vp_op);
					tat_modem_marsh16(&vl_pwr_seq_data.wcdma.fill1, vp_op);
					tat_modem_marsh16(&vl_pwr_seq_data.wcdma.fill2, vp_op);
                    pl_sb->system_dep_data = vl_pwr_seq_data.wcdma;

					tat_modem_marsh16(&pl_sb->channel_nbr, vp_op);
					tat_modem_marsh16(&pl_sb->pwr_ctrl_value, vp_op);
					tat_modem_marsh16(&pl_sb->rf_rx_attenuation_value, vp_op);
					tat_modem_marsh16(&pl_sb->lna_state, vp_op);
					tat_modem_marsh16(&pl_sb->rf_lna_step_size, vp_op);
					tat_modem_marsh16(&pl_sb->nbr_empty_slots, vp_op);
				}
				break;

			default:
				vl_err = TAT_ISI_HANDLER_ERROR;
			}
		}
		break;

	case C_TEST_SB_PWR_SEQ_MEAS_RESULTS:
		{
			C_TEST_SB_PWR_SEQ_MEAS_RESULTS_STR *pl_sb =
			    (C_TEST_SB_PWR_SEQ_MEAS_RESULTS_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->
					  rx_attenuation_mode_and_status,
					  vp_op);
			tat_modem_marsh16(&pl_sb->system_info, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->rf_band, vp_op);
			tat_modem_marsh16(&pl_sb->channel_nbr, vp_op);
			tat_modem_marsh16((u16 *) & pl_sb->int_measured_pwr1,
					  vp_op);
			tat_modem_marsh16((u16 *) & pl_sb->int_measured_pwr2,
					  vp_op);
			tat_modem_marsh16(&pl_sb->lna_state, vp_op);
		}
		break;

	case C_TEST_SB_FREQ_RESP_CALIBRATE:
		{
			C_TEST_SB_FREQ_RESP_CALIBRATE_STR *pl_sb =
			    (C_TEST_SB_FREQ_RESP_CALIBRATE_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->system_info, vp_op);
			tat_modem_marsh16(&pl_sb->fill1, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->rf_band, vp_op);
			tat_modem_marsh16(&pl_sb->fill2, vp_op);
			u16 vl_nb_data = 0;
			if (ISI_MARSH == vp_op) {
				vl_nb_data = pl_sb->nbr_of_data;
				tat_modem_marsh16(&pl_sb->nbr_of_data, vp_op);
			} else {
				tat_modem_marsh16(&pl_sb->nbr_of_data, vp_op);
				vl_nb_data = pl_sb->nbr_of_data;
			}

			int i;
			for (i = 0; i < vl_nb_data; i++) {
				C_TEST_FREQ_RESP_DATA_STR *data =
				    &pl_sb->freq_resp_data[i];
				tat_modem_marsh16(&data->channel_nbr, vp_op);
				tat_modem_marsh16((u16 *) & data->ext_data,
						  vp_op);
				tat_modem_marsh16((u16 *) & data->int_data,
						  vp_op);
				tat_modem_marsh16(&data->fill1, vp_op);
			}
		}
		break;

	case C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS:
		{
			C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS_STR *pl_sb =
			    (C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS_STR *)
			    pp_block;
			tat_modem_marsh16(&pl_sb->status, vp_op);
			tat_modem_marsh16(&pl_sb->system_info, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->rf_band, vp_op);
			tat_modem_marsh16((u16 *) & pl_sb->slope, vp_op);
			tat_modem_marsh16((u16 *) & pl_sb->coeff_b, vp_op);
			tat_modem_marsh16((u16 *) & pl_sb->coeff_c, vp_op);
			u16 vl_nb_data = 0;
			if (ISI_MARSH == vp_op) {
				vl_nb_data = pl_sb->nbr_of_data;
				tat_modem_marsh16(&pl_sb->nbr_of_data, vp_op);
			} else {
				tat_modem_marsh16(&pl_sb->nbr_of_data, vp_op);
				vl_nb_data = pl_sb->nbr_of_data;
			}

			int i;
			for (i = 0; i < vl_nb_data; i++) {
				C_TEST_FREQ_RESP_DATA_STR *data =
				    &pl_sb->freq_resp_data[i];
				tat_modem_marsh16(&data->channel_nbr, vp_op);
				tat_modem_marsh16((u16 *) & data->ext_data,
						  vp_op);
				tat_modem_marsh16((u16 *) & data->int_data,
						  vp_op);
				tat_modem_marsh16(&data->fill1, vp_op);
			}
		}
		break;

	case C_TEST_SB_GSM_TX_PWR_TUNING:
		{
			C_TEST_SB_GSM_TX_PWR_TUNING_STR *pl_sb =
			    (C_TEST_SB_GSM_TX_PWR_TUNING_STR *) pp_block;
			tat_modem_marsh32((uint32_t *) & pl_sb->rf_band, vp_op);
			tat_modem_marsh16(&pl_sb->channel_nbr, vp_op);
			tat_modem_marsh16(&pl_sb->fill1, vp_op);
			tat_modem_marsh16(&pl_sb->edge_on, vp_op);

			/* warning: do not use for C_TEST_SB_GSM_TX_PWR_TUNING_STR allocated: it have very limited memory allocated for pp_data->selftun_ctrl */
			GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR *pl_data =
			    (GSM_TX_PWR_SELFTUNING_CTRL_DATA_STR *) pl_sb->
			    selftun_ctrl;
			if (ISI_MARSH == vp_op) {
				tatl17_marsh_GSM_TX_PWR_SELFTUNING_CTRL_DATA
				    (pl_sb->nbr_of_ctrl_data, pl_data, vp_op);
				tat_modem_marsh16(&pl_sb->nbr_of_ctrl_data,
						  vp_op);
			} else {
				tat_modem_marsh16(&pl_sb->nbr_of_ctrl_data,
						  vp_op);
				tatl17_marsh_GSM_TX_PWR_SELFTUNING_CTRL_DATA
				    (pl_sb->nbr_of_ctrl_data, pl_data, vp_op);
			}
		}
		break;

	case C_TEST_SB_WCDMA_TX_TUNING:
		{
			C_TEST_SB_WCDMA_TX_TUNING_STR *pl_sb =
			    (C_TEST_SB_WCDMA_TX_TUNING_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->step_length, vp_op);
			tat_modem_marsh16(&pl_sb->tuning_ctrl_opt, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->rf_band, vp_op);
			tat_modem_marsh16(&pl_sb->channel_nbr, vp_op);
			tat_modem_marsh16(&pl_sb->tuning_target, vp_op);
			tat_modem_marsh16(&pl_sb->pwr_meter_att, vp_op);
			tat_modem_marsh16(&pl_sb->pwr_ctrl_value, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->pwr_ctrl_step,
					  vp_op);
			tat_modem_marsh16(&pl_sb->dpdch_weight, vp_op);
			tat_modem_marsh16(&pl_sb->dpcch_weight, vp_op);

			/* warning: do not use for C_TEST_SB_WCDMA_TX_TUNING_STR allocated: it have very limited memory allocated for pp_data->pa_ctrl */
			WCDMA_TX_PWR_CTRL_DATA_STR *pl_data =
			    (WCDMA_TX_PWR_CTRL_DATA_STR *) pl_sb->pa_ctrl;
			if (ISI_MARSH == vp_op) {
				tatl17_marsh_WCDMA_TX_PWR_CTRL_DATA(pl_sb->
								    pa_ctrl_data_size,
								    pl_data,
								    vp_op);
				tat_modem_marsh16(&pl_sb->pa_ctrl_data_size,
						  vp_op);
			} else {
				tat_modem_marsh16(&pl_sb->pa_ctrl_data_size,
						  vp_op);
				tatl17_marsh_WCDMA_TX_PWR_CTRL_DATA(pl_sb->
								    pa_ctrl_data_size,
								    pl_data,
								    vp_op);
			}
		}
		break;

	case C_TEST_SB_TX_TUNING_RESULTS:
		{
			C_TEST_SB_TX_TUNING_RESULTS_STR *pl_sb =
			    (C_TEST_SB_TX_TUNING_RESULTS_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->status, vp_op);
			tat_modem_marsh16(&pl_sb->system_info, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->system_dep_data,
					  vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->max_pwr, vp_op);

			/* warning: do not use for C_TEST_SB_TX_TUNING_RESULTS_STR allocated: it have very limited memory allocated for pp_data->measured_pwr */
			TX_PWR_TUNING_DATA_STR *pl_data =
			    (TX_PWR_TUNING_DATA_STR *) pl_sb->measured_pwr;
			if (ISI_MARSH == vp_op) {
				tatl17_marsh_TX_PWR_TUNING_DATA(pl_sb->
								amount_of_values,
								pl_data, vp_op);
				tat_modem_marsh16(&pl_sb->amount_of_values,
						  vp_op);
			} else {
				tat_modem_marsh16(&pl_sb->amount_of_values,
						  vp_op);
				tatl17_marsh_TX_PWR_TUNING_DATA(pl_sb->
								amount_of_values,
								pl_data, vp_op);
			}
		}
		break;

	case C_TEST_SB_GSM_TX_IQ_SETUP:
		{
			C_TEST_SB_GSM_TX_IQ_SETUP_STR *pl_sb =
			    (C_TEST_SB_GSM_TX_IQ_SETUP_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->band, vp_op);
			tat_modem_marsh16(&pl_sb->pa_loop_mode, vp_op);
			tat_modem_marsh16(&pl_sb->power_level, vp_op);
			tat_modem_marsh16(&pl_sb->channel_nbr, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->dc_offset_i,
					  vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->dc_offset_q,
					  vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->ampl_offset,
					  vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->fill1, vp_op);
		}
		break;

	case C_TEST_SB_WCDMA_TX_IQ_SETUP:
		{
			C_TEST_SB_WCDMA_TX_IQ_SETUP_STR *pl_sb =
			    (C_TEST_SB_WCDMA_TX_IQ_SETUP_STR *) pp_block;
			tat_modem_marsh32(&pl_sb->band_info, vp_op);
			tat_modem_marsh16(&pl_sb->ul_uarfcn, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->fill1, vp_op);
		}
		break;

	case C_TEST_SB_DC_OFFSET_RESULT:
		{
			C_TEST_SB_DC_OFFSET_RESULT_STR *pl_sb =
			    (C_TEST_SB_DC_OFFSET_RESULT_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->status, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->i_tuning_value,
					  vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->i_result,
					  vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->q_tuning_value,
					  vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->q_result,
					  vp_op);
			tat_modem_marsh16(&pl_sb->fill1, vp_op);
		}
		break;

	case C_TEST_SB_AMPLITUDE_RESULT:
		{
			C_TEST_SB_AMPLITUDE_RESULT_STR *pl_sb =
			    (C_TEST_SB_AMPLITUDE_RESULT_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->status, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->
					  best_tuning_value, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->best_result,
					  vp_op);
			tat_modem_marsh16(&pl_sb->vector_length, vp_op);
		}
		break;

	case C_TEST_SB_PHASE_RESULT:
		{
			C_TEST_SB_PHASE_RESULT_STR *pl_sb =
			    (C_TEST_SB_PHASE_RESULT_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->status, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->
					  best_tuning_value, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->best_result,
					  vp_op);
			tat_modem_marsh16(&pl_sb->vector_length, vp_op);
		}
		break;

	case C_TEST_SB_GSM_PA_COMPRESSION_TUNING:
		{
			C_TEST_SB_GSM_PA_COMPRESSION_TUNING_STR *pl_sb =
			    (C_TEST_SB_GSM_PA_COMPRESSION_TUNING_STR *)
			    pp_block;
			tat_modem_marsh32((uint32_t *) & pl_sb->rf_band, vp_op);
			tat_modem_marsh16(&pl_sb->channel_nbr, vp_op);
			tat_modem_marsh16(&pl_sb->control_word, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->
					  target_power_dBm_q6, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->
					  compression_level_dB_q6, vp_op);
		}
		break;

	case C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS:
		{
			C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS_STR *pl_sb =
			    (C_TEST_SB_GSM_PA_COMPRESSION_TUNING_RESULTS_STR *)
			    pp_block;
			tat_modem_marsh16(&pl_sb->status, vp_op);
			tat_modem_marsh16(&pl_sb->fill1, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->rf_band, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->
					  pa_compensation_dB_q6, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->
					  final_tx_power_dBm_q6, vp_op);
			tat_modem_marsh16(&pl_sb->iteration_count, vp_op);
			tat_modem_marsh16(&pl_sb->fill2, vp_op);
		}
		break;

	case C_TEST_SB_TUNING_ERROR:
		{
			C_TEST_SB_TUNING_ERROR_STR *sb =
			    (C_TEST_SB_TUNING_ERROR_STR *) pp_block;
			tat_modem_marsh16((uint16 *) & sb->errorCode, vp_op);
			/* xxxxxxxx--------  Id of failed operation
			   --------xxxxxxxx  First char of error message
			 */
			/* nothing to do for status */
			/* Error message in ASCII string : nothing to do */
			/* 0 to 3 Filler : skip it */
		}
		break;

	case C_TEST_SB_ANTENNA_TEST:
		{
			C_TEST_SB_ANTENNA_TEST_STR *pl_sb =
			    (C_TEST_SB_ANTENNA_TEST_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->fill1, vp_op);
			tat_modem_marsh16(&pl_sb->system_info, vp_op);
			tat_modem_marsh32((uint32_t *) & pl_sb->system_dep_data,
					  vp_op);
			tat_modem_marsh16(&pl_sb->channel_nbr, vp_op);
			tat_modem_marsh16((uint16_t *) & pl_sb->pwr_level,
					  vp_op);
			tat_modem_marsh16(&pl_sb->att_value, vp_op);
			tat_modem_marsh16(&pl_sb->fill2, vp_op);
		}
		break;

	case C_TEST_SB_ANTENNA_TEST_RESULT:
		{
			C_TEST_SB_ANTENNA_TEST_RESULT_STR *pl_sb =
			    (C_TEST_SB_ANTENNA_TEST_RESULT_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->fill1_substatus, vp_op);
			tat_modem_marsh16(&pl_sb->vswr_value, vp_op);
		}
		break;

	case C_TEST_SB_READ_RX_IQ_SAMPLES:
		{
			C_TEST_SB_READ_RX_IQ_SAMPLES_STR *pl_sb =
			    (C_TEST_SB_READ_RX_IQ_SAMPLES_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->system_info, vp_op);
			tat_modem_marsh16(&pl_sb->is_new_samples_rx_path,
					  vp_op);
		}
		break;

	case C_TEST_SB_RECEIVED_RX_IQ_SAMPLES:
		{
			C_TEST_SB_RECEIVED_RX_IQ_SAMPLES_STR *pl_sb =
			    (C_TEST_SB_RECEIVED_RX_IQ_SAMPLES_STR *) pp_block;
			C_TEST_IQ_PAIR_DATA_STR *pl_data;
			int i;

			/* xxxxxxxx--------  Filler
			   --------xxxxxxxx  Status of operation
			 */
			tat_modem_marsh16(&pl_sb->status, vp_op);
			/* System information
			   Values from the bitmask table SYSTEM
			 */
			tat_modem_marsh16(&pl_sb->system_info, vp_op);
			/* xxxxxxxx--------  Filler
			   --------xxxxxxxx  RF HW path
			 */
			tat_modem_marsh16(&pl_sb->fill2_rx_path, vp_op);
			tat_modem_marsh16(&pl_sb->sample_freq, vp_op);	/* Sampling frequency, kHz */
			tat_modem_marsh16(&pl_sb->rx_gain, vp_op);	/* Receiver gain index */
			tat_modem_marsh16(&pl_sb->reserved1, vp_op);	/* Recerved for later use */
			tat_modem_marsh16(&pl_sb->reserved2, vp_op);	/* Recerved for later use */
			tat_modem_marsh16(&pl_sb->nbr_of_data, vp_op);

			for (i = 0; i < pl_sb->nbr_of_data; i++) {
				pl_data =
				    (C_TEST_IQ_PAIR_DATA_STR *) (pl_sb->
								 rx_iq_data +
								 i);
				tat_modem_marsh16((uint16_t *) & pl_data->
						  i_data, vp_op);
				tat_modem_marsh16((uint16_t *) & pl_data->
						  q_data, vp_op);
			}
		}
		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR *sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR *)
			    pp_block;

			GET_INFO_CTRL_DATA_STR *pl_get_info =
			    (GET_INFO_CTRL_DATA_STR *) ((u8 *) sb +
							SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR);
			tat_modem_marsh16(&pl_get_info->requested_action,
					  vp_op);
			tat_modem_marsh16(&pl_get_info->fill_index, vp_op);
		}
		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR *sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR *)
			    pp_block;

			RETURN_INFO_PARAM_RESP_STR *pl_ret_info =
			    (RETURN_INFO_PARAM_RESP_STR *) ((u8 *) sb +
							    SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR);
			tat_modem_marsh16(&pl_ret_info->requested_action,
					  vp_op);
			tat_modem_marsh16(&pl_ret_info->action_status, vp_op);
			tat_modem_marsh16(&pl_ret_info->nb_of_param, vp_op);
			tat_modem_marsh16(&pl_ret_info->index, vp_op);

			tat_modem_marsh16(&pl_ret_info->return_info_param.
					  paramID, vp_op);
			int vl_size_of_param_in_words =
			    sizeof(pl_ret_info->return_info_param.string) /
			    sizeof(u16);
			int vl_i;
			for (vl_i = 0; vl_i < vl_size_of_param_in_words; vl_i++) {
				tat_modem_marsh16((pl_ret_info->
						   return_info_param.string +
						   vl_i), vp_op);
			}
		}
		break;

	case C_TEST_SB_GSM_SINGLE_BER:
		{
			C_TEST_SB_GSM_SINGLE_BER_STR *pl_sb =
			    (C_TEST_SB_GSM_SINGLE_BER_STR *) pp_block;

			tat_modem_marsh16(&pl_sb->
					  rx_slot_pattern_tx_slot_pattern,
					  vp_op);
			tat_modem_marsh16(&pl_sb->psk_slots_channel_type,
					  vp_op);
			tat_modem_marsh16(&pl_sb->fill2_codec, vp_op);
			tat_modem_marsh16(&pl_sb->ARFCN_val, vp_op);
			tat_modem_marsh16(&pl_sb->coded_mode, vp_op);
			tat_modem_marsh16(&pl_sb->num_of_tested_bits, vp_op);
			tat_modem_marsh16(&pl_sb->fill3_transmit_data_type,
					  vp_op);

			int vl_nb_data = pl_sb->num_of_freq;
			tat_modem_marsh16(&pl_sb->num_of_freq, vp_op);
			if (vp_op == ISI_UNMARSH) {
				vl_nb_data = pl_sb->num_of_freq;
			}

			int i, j;
			GSM_SBER_INFO_PER_FREQ_STR *pl_data;
			for (i = 0; i < vl_nb_data; i++) {
				pl_data =
				    (GSM_SBER_INFO_PER_FREQ_STR *) pl_sb->
				    array_of_freq_info;
				pl_data += i;

				for (j = 0; j < 8; j++) {
					tat_modem_marsh16(&pl_data->
							  tx_level_pcl[j].
							  tx_level, vp_op);
					tat_modem_marsh16((uint16 *) & pl_data->
							  init_rx_level[j].
							  rx_pwr_level, vp_op);
				}
				tat_modem_marsh16(&pl_data->arfcn, vp_op);
				tat_modem_marsh32(&pl_data->rf_band, vp_op);
			}
		}
		break;

	case C_TEST_SB_GSM_SINGLE_BER_SYNC:
		{
			C_TEST_SB_GSM_SINGLE_BER_SYNC_STR *pl_sb =
			    (C_TEST_SB_GSM_SINGLE_BER_SYNC_STR *) pp_block;
			tat_modem_marsh32(&pl_sb->rf_band, vp_op);
			tat_modem_marsh16(&pl_sb->tester_tool, vp_op);
			tat_modem_marsh16(&pl_sb->arfcn_bcch, vp_op);
			tat_modem_marsh16(&pl_sb->fill1_data_type, vp_op);
			tat_modem_marsh16(&pl_sb->arfcn_tch, vp_op);
			tat_modem_marsh16(&pl_sb->FN_end_of_sync, vp_op);
			tat_modem_marsh16(&pl_sb->chan_type_codec, vp_op);
			tat_modem_marsh16(&pl_sb->fill2_time_slot_pat, vp_op);
			tat_modem_marsh16(&pl_sb->coded_mode, vp_op);
            tat_modem_marsh16((uint16 *)&pl_sb->init_rx_level, vp_op);
            tat_modem_marsh16(&pl_sb->fill3, vp_op);
		}
		break;

	case C_TEST_SB_GSM_SINGLE_BER_RESULTS:
		{
			C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *pl_sb =
			    (C_TEST_SB_GSM_SINGLE_BER_RESULTS_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->fill1_status, vp_op);

			int vl_nb_result = pl_sb->num_of_res;
			tat_modem_marsh16(&pl_sb->num_of_res, vp_op);
			if (vp_op == ISI_UNMARSH) {
				vl_nb_result = pl_sb->num_of_res;
			}

			int i;
			GSM_BER_RES_INFO_STR *pl_data;
			for (i = 0; i < vl_nb_result; i++) {
				pl_data =
				    (GSM_BER_RES_INFO_STR *) pl_sb->
				    array_of_results;
				pl_data += i;

				tat_modem_marsh16(&pl_data->
						  num_of_rber_Ib_coded_bits,
						  vp_op);
				tat_modem_marsh16(&pl_data->
						  num_of_rber_II_coded_bits,
						  vp_op);
				tat_modem_marsh16(&pl_data->
						  num_of_ber_uncoded_bits,
						  vp_op);
				tat_modem_marsh16(&pl_data->
						  rber_Ib_coded_error_bits,
						  vp_op);
				tat_modem_marsh16(&pl_data->
						  rber_II_coded_error_bits,
						  vp_op);
				tat_modem_marsh16(&pl_data->
						  ber_uncoded_error_bits,
						  vp_op);
				tat_modem_marsh16(&pl_data->meas_blocks, vp_op);
				tat_modem_marsh16(&pl_data->false_blocks,
						  vp_op);
				tat_modem_marsh16((uint16 *) & pl_data->avg_snr,
						  vp_op);
				tat_modem_marsh16((uint16 *) & pl_data->avg_pwr,
						  vp_op);
			}
		}
		break;

	case C_TEST_SB_WCDMA_SINGLE_BER:
		{
			C_TEST_SB_WCDMA_SINGLE_BER_STR *pl_sb =
			    (C_TEST_SB_WCDMA_SINGLE_BER_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->no_bits_to_be_tested, vp_op);

			int vl_nb_data = pl_sb->num_of_freq;
			tat_modem_marsh16(&pl_sb->num_of_freq, vp_op);
			if (vp_op == ISI_UNMARSH) {
				vl_nb_data = pl_sb->num_of_freq;
			}

			int i;
			WCDMA_SBER_INFO_PER_FREQ_STR *pl_data;
			for (i = 0; i < vl_nb_data; i++) {
				pl_data =
				    (WCDMA_SBER_INFO_PER_FREQ_STR *) pl_sb->
				    array_of_freq_info;
				pl_data += i;

				tat_modem_marsh16((uint16 *) & pl_data->
						  tx_level, vp_op);
				tat_modem_marsh16(&pl_data->dl_uarfcn, vp_op);
				tat_modem_marsh32(&pl_data->rf_band, vp_op);
			}
		}
		break;

	case C_TEST_SB_WCDMA_SINGLE_BER_SYNC:
		{
			C_TEST_SB_WCDMA_SINGLE_BER_SYNC_STR *pl_sb =
			    (C_TEST_SB_WCDMA_SINGLE_BER_SYNC_STR *) pp_block;
			tat_modem_marsh32(&pl_sb->rf_band, vp_op);
			tat_modem_marsh16(&pl_sb->dl_uarfcn, vp_op);
			tat_modem_marsh16(&pl_sb->tester_tool, vp_op);
			tat_modem_marsh16(&pl_sb->fill1_2_channelization_code,
					  vp_op);
			tat_modem_marsh16(&pl_sb->fill1, vp_op);
			tat_modem_marsh32(&pl_sb->ul_scrambling_code, vp_op);
			tat_modem_marsh16(&pl_sb->pri_scrambling_code, vp_op);
			tat_modem_marsh16(&pl_sb->frame_no, vp_op);
			tat_modem_marsh16(&pl_sb->fill2_rx_data_type, vp_op);
			tat_modem_marsh16((uint16 *) & pl_sb->tx_pwr_level,
					  vp_op);
			tat_modem_marsh16(&pl_sb->fill3, vp_op);
			tat_modem_marsh16(&pl_sb->fill3_2_rf_rx_path, vp_op);

		}
		break;

	case C_TEST_SB_WCDMA_SINGLE_BER_RESULTS:
		{
			C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR *pl_sb =
			    (C_TEST_SB_WCDMA_SINGLE_BER_RESULTS_STR *) pp_block;
			tat_modem_marsh16(&pl_sb->fill1_status, vp_op);

			int vl_nb_data = pl_sb->num_of_result;
			tat_modem_marsh16(&pl_sb->num_of_result, vp_op);
			if (vp_op == ISI_UNMARSH) {
				vl_nb_data = pl_sb->num_of_result;
			}

			int i;
			WCDMA_SBER_RES_INFO_STR *pl_data;
			for (i = 0; i < vl_nb_data; i++) {
				pl_data =
				    (WCDMA_SBER_RES_INFO_STR *) pl_sb->
				    array_of_results;
				pl_data += i;

				tat_modem_marsh16(&pl_data->num_bits_meas,
						  vp_op);
				tat_modem_marsh16(&pl_data->num_err_bits,
						  vp_op);
				tat_modem_marsh16((uint16 *) & pl_data->
						  avg_rcvd_pwr, vp_op);
				tat_modem_marsh16((uint16 *) & pl_data->
						  avg_rscp_res, vp_op);
			}
		}
		break;

	case C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL:
		{
			C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR *pl_sb =
			    (C_HAL_SB_WCDMA_TX_SWEEP_TEST_CONTROL_STR *) pp_block;

			int vl_nb_chnl = pl_sb->number_of_channels;
			tat_modem_marsh16(&pl_sb->number_of_channels, vp_op);
			if (vp_op == ISI_UNMARSH) {
				vl_nb_chnl = pl_sb->number_of_channels;
			}
			tat_modem_marsh16(&pl_sb->tx_setup, vp_op);
			tat_modem_marsh32(&pl_sb->duration, vp_op);

			int i,j;
			WCDMA_TX_SWEEP_TEST_CHANNEL_STR *pl_data1;
			int16 *pl_data2;
			for (i = 0; i < vl_nb_chnl; i++) {
				pl_data1 = (WCDMA_TX_SWEEP_TEST_CHANNEL_STR *)pl_sb->channel_subblock;
				pl_data1 += i;

				tat_modem_marsh32(&pl_data1->band_info, vp_op);
				tat_modem_marsh16(&pl_data1->ul_uarfcn, vp_op);

				int vl_nb_step = pl_data1->number_of_steps;
				tat_modem_marsh16(&pl_data1->number_of_steps, vp_op);
				if (vp_op == ISI_UNMARSH) {
					vl_nb_step = pl_data1->number_of_steps;
				}

				for (j = 0; j < vl_nb_step; j++) {
					pl_data2 = (int16 *)pl_data1->power_level;
					pl_data2 += j;
					tat_modem_marsh16(pl_data2, vp_op);
				}
			}
		}
		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR *pl_sb_hdr =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR *) pp_block;

			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ *) ((u8 *) pl_sb_hdr +
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_STR);

			tat_modem_marsh32(&pl_sb->band, vp_op);
			tat_modem_marsh16(&pl_sb->channel, vp_op);
			tat_modem_marsh16(&pl_sb->control, vp_op);
			tat_modem_marsh16((uint16 *)&pl_sb->power, vp_op);

			tat_modem_marsh16(&pl_sb->filler01, vp_op);
			tat_modem_marsh16(&pl_sb->filler02, vp_op);
			tat_modem_marsh16(&pl_sb->filler03, vp_op);
			tat_modem_marsh16(&pl_sb->filler04, vp_op);
			tat_modem_marsh16(&pl_sb->filler05, vp_op);
			tat_modem_marsh16(&pl_sb->filler06, vp_op);
			tat_modem_marsh16(&pl_sb->filler07, vp_op);
			tat_modem_marsh16(&pl_sb->filler08, vp_op);
			tat_modem_marsh16(&pl_sb->filler09, vp_op);
			tat_modem_marsh16(&pl_sb->filler10, vp_op);
			tat_modem_marsh16(&pl_sb->filler11, vp_op);
			tat_modem_marsh16(&pl_sb->filler12, vp_op);
			tat_modem_marsh16(&pl_sb->filler13, vp_op);
			tat_modem_marsh16(&pl_sb->filler14, vp_op);
			tat_modem_marsh16(&pl_sb->filler15, vp_op);
			tat_modem_marsh16(&pl_sb->filler16, vp_op);
			tat_modem_marsh16(&pl_sb->filler17, vp_op);
			tat_modem_marsh16(&pl_sb->filler18, vp_op);
			tat_modem_marsh16(&pl_sb->filler19, vp_op);
			tat_modem_marsh16(&pl_sb->filler20, vp_op);
 		}
 		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR *pl_sb_hdr =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR *) pp_block;

			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ *) ((u8 *) pl_sb_hdr +
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_STR);

			tat_modem_marsh32(&pl_sb->band, vp_op);
			tat_modem_marsh16(&pl_sb->channel, vp_op);
			tat_modem_marsh16(&pl_sb->status, vp_op);
			tat_modem_marsh16((uint16 *)&pl_sb->power_in_test, vp_op);
			tat_modem_marsh16((uint16 *)&pl_sb->power_last_request, vp_op);
			tat_modem_marsh16((uint16 *)&pl_sb->power_last_report, vp_op);
			tat_modem_marsh16((uint16 *)&pl_sb->power_delta, vp_op);
			tat_modem_marsh16(&pl_sb->filler1, vp_op);
			tat_modem_marsh16(&pl_sb->filler2, vp_op);
 		}
 		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR *pl_sb_hdr =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR *) pp_block;

			C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ *) ((u8 *) pl_sb_hdr +
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_STR);

			tat_modem_marsh16(&pl_sb->system, vp_op);

			uint32 band;
			band = SetU16InU32(MSB_POSS, band, pl_sb->band_MSB);
			band = SetU16InU32(LSB_POSS, band, pl_sb->band_LSB);

			tat_modem_marsh32(&band, vp_op);
			pl_sb->band_MSB = GetU16InU32(MSB_POSS, band);
			pl_sb->band_LSB = GetU16InU32(LSB_POSS, band);

			tat_modem_marsh16((uint16 *)&pl_sb->powerlevel, vp_op);
			tat_modem_marsh16(&pl_sb->mode_filler, vp_op);
			tat_modem_marsh16(&pl_sb->channel, vp_op);
		}
		break;

	case C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2:
		{
			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_STR *pl_sb_hdr =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_STR *) pp_block;

			C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ *pl_sb =
			    (C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ *) ((u8 *) pl_sb_hdr +
				SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_STR);

			tat_modem_marsh16(&pl_sb->filler_status, vp_op);
			tat_modem_marsh16((uint16 *)&pl_sb->transpower, vp_op);
			tat_modem_marsh16(&pl_sb->transpower_inraw, vp_op);
			tat_modem_marsh16((uint16 *)&pl_sb->recvpower, vp_op);
			tat_modem_marsh16(&pl_sb->recvpower_inraw, vp_op);
			tat_modem_marsh16(&pl_sb->filler, vp_op);
		}
		break;

	case C_HAL_SB_GSM_TX_SWEEP_TEST_CONTROL:
		{
			C_HAL_SB_GSM_TX_SWEEP_TEST_CONTROL_STR *pl_sb_hdr =
				(C_HAL_SB_GSM_TX_SWEEP_TEST_CONTROL_STR *) pp_block;

			tat_modem_marsh16((uint16*)&pl_sb_hdr->tx_setup, vp_op);

			uint16 number = pl_sb_hdr->number_of_tx_opers;
			tat_modem_marsh16((uint16*)&pl_sb_hdr->number_of_tx_opers, vp_op);
			if (vp_op == ISI_UNMARSH) {
				number = pl_sb_hdr->number_of_tx_opers;
			}

			GSM_TX_SWEEP_TEST_OPER_STR *pl_sb_seq =
				(GSM_TX_SWEEP_TEST_OPER_STR *) ((u8*)pl_sb_hdr +
				SIZE_C_HAL_SB_GSM_TX_SWEEP_TEST_CONTROL_STR);
			int i;
			for (i = 0; i < number; i++ )
			{
				tat_modem_marsh32((uint32*)&pl_sb_seq->band_info, vp_op);
				tat_modem_marsh16((uint16*)&pl_sb_seq->channel_nbr, vp_op);
				tat_modem_marsh16((uint16*)&pl_sb_seq->off_time, vp_op);
				tat_modem_marsh16((uint16*)&pl_sb_seq->number_of_pcl, vp_op);
				tat_modem_marsh16((uint16*)&pl_sb_seq->power_level[0], vp_op);
				tat_modem_marsh16((uint16*)&pl_sb_seq->power_level[1], vp_op);
				tat_modem_marsh16((uint16*)&pl_sb_seq->fill1, vp_op);
				pl_sb_seq += 1;
			}
		}
		break;

	default:

		SYSLOG(LOG_ERR,
		       "no method defined to encode/decode block of id=0x%X",
		       pl_sb_id);
		vl_err = TAT_ERROR_NOT_MATCHING_MSG;
	}

	return vl_err;
}

int tatl17_marsh_msg_wcdma(struct tatrf_isi_msg_t *pp_st,
			   TAT_MODEM_ISI_MARSH vp_op)
{
	int vl_err = TAT_ERROR_OFF;
	u8 vl_trans_id, vl_msg_id;

	tatl17_11get_id(pp_st, &vl_trans_id, &vl_msg_id);

	/* transaction id and message id */
	void *pl_msg = tatrf_isi_msg_header(pp_st);
	tat_modem_marsh16((u16 *) pl_msg, vp_op);

	switch (vl_msg_id) {
	case W_TEST_AGC_LNA_ALIGN_REQ:
		{
			W_TEST_AGC_LNA_ALIGN_REQ_STR *req =
			    (W_TEST_AGC_LNA_ALIGN_REQ_STR *) pl_msg;
			/* xxxxxxxx--------  Filler
			   --------xxxxxxxx Length of one AGC step
			 */
			tat_modem_marsh16(&req->filler1_agc_step_length, vp_op);
			tat_modem_marsh16(&req->mid_chan, vp_op);
			tat_modem_marsh16(&req->afc_dac_value, vp_op);	/* not used */
			tat_modem_marsh16(&req->rx_chain_gain, vp_op);
			tat_modem_marsh16(&req->mid_gain1, vp_op);	/* not used */
			tat_modem_marsh16(&req->mid_gain2, vp_op);	/* not used */
			tat_modem_marsh16(&req->low_gain1, vp_op);	/* not used */
			tat_modem_marsh16(&req->low_gain2, vp_op);	/* not used */
			tat_modem_marsh16(&req->agc_slope_align1, vp_op);	/* not used */
			tat_modem_marsh16(&req->agc_slope_align2, vp_op);	/* not used */
			/* xxxxxxxx--------  Filler
			   --------xxxxxxxx Options for RX alignment control
			 */
			tat_modem_marsh16(&req->filler2_rx_align_ctrl, vp_op);
			tat_modem_marsh16(&req->low_chan, vp_op);
			tat_modem_marsh16(&req->high_chan, vp_op);
		}
		break;

	case W_TEST_AGC_LNA_ALIGN_RESP:
		{
			W_TEST_AGC_LNA_ALIGN_RESP_STR *req =
			    (W_TEST_AGC_LNA_ALIGN_RESP_STR *) pl_msg;
			/* xxxxxxxx--------  Filler
			   --------xxxxxxxx Alignment status (W_TEST_STATUS)
			 */
			tat_modem_marsh16(&req->filler1_align_status, vp_op);
			tat_modem_marsh16((u16 *) & req->rx_chain_gain, vp_op);
			tat_modem_marsh16((u16 *) & req->mid_gain_error, vp_op);
			tat_modem_marsh16((u16 *) & req->low_gain_error, vp_op);
			tat_modem_marsh16(&req->agc_gain_error_slope, vp_op);
			tat_modem_marsh16(&req->bias_error, vp_op);
			tat_modem_marsh16(&req->low_freq_comp, vp_op);
			tat_modem_marsh16(&req->high_freq_comp, vp_op);
		}
		break;



	default:

		SYSLOG(LOG_ERR,
		       "no method available to encode/decode WCDMA message 0x%2X",
		       vl_msg_id);
		vl_err = TAT_ERROR_NOT_MATCHING_MSG;
	}

	return vl_err;
}

int tatl17_marsh_msg_modem(struct tatrf_isi_msg_t *pp_st,
			   TAT_MODEM_ISI_MARSH vp_op)
{
	int vl_err = TAT_ERROR_OFF;
	u8 vl_trans_id, vl_msg_id;

	tatl17_11get_id(pp_st, &vl_trans_id, &vl_msg_id);

	/* transaction id and message id */
	u16 *pl_msg = tatrf_isi_msg_header(pp_st);
	tat_modem_marsh16(pl_msg, vp_op);

	switch (vl_msg_id) {
	case MODEM_TEST_RUN_REQ:
		{
			MODEM_TEST_RUN_REQ_STR *pl_req =
			    (MODEM_TEST_RUN_REQ_STR *) pl_msg;
			tat_modem_marsh16(&pl_req->group_id, vp_op);
			tat_modem_marsh16(&pl_req->test_id, vp_op);
			tat_modem_marsh16(&pl_req->case_ID, vp_op);
			tat_modem_marsh8(&pl_req->attrib, vp_op);
			tat_modem_marsh8(&pl_req->fillbyte, vp_op);
			tat_modem_marsh16(&pl_req->inp_data_length, vp_op);
			/* pl_req->inp_data is to be processed by caller */
		}
		break;

	case MODEM_TEST_RUN_RESP:
		{
			MODEM_TEST_RUN_RESP_STR *pl_resp =
			    (MODEM_TEST_RUN_RESP_STR *) pl_msg;
			tat_modem_marsh16(&pl_resp->group_id, vp_op);
			tat_modem_marsh16(&pl_resp->test_id, vp_op);
			tat_modem_marsh16(&pl_resp->case_ID, vp_op);
			tat_modem_marsh8(&pl_resp->fillbyte, vp_op);
			tat_modem_marsh8(&pl_resp->status, vp_op);
			tat_modem_marsh16(&pl_resp->out_data_length, vp_op);
			/* pl_resp->out_data is to be processed by caller */
		}
		break;

	default:

		SYSLOG(LOG_ERR,
		       "no method available to encode/decode MODEM message 0x%2X",
		       vl_msg_id);
		vl_err = TAT_ERROR_NOT_MATCHING_MSG;
	}

	return vl_err;
}

int tatl17_marsh_msg_common(struct tatrf_isi_msg_t *pp_st,
			    TAT_MODEM_ISI_MARSH vp_op)
{
	int vl_err = TAT_ERROR_OFF, vl_i = 0;
	const size_t vl_msg_size = tatrf_isi_total_length(pp_st);
	u16 *pl_msg = pp_st->msg;
	u8 vl_trans_id, vl_msg_id;
	u16 vl_sb_id, vl_sb_len;
	u16 vl_sb_ndx, vl_num_sb;
	u16 *pl_ptr16 = NULL;
	u16 *pl_msg_end = (u16 *) ((u8 *) pl_msg + vl_msg_size);

	tatl17_11get_id(pp_st, &vl_trans_id, &vl_msg_id);
	/*SYSLOG(LOG_DEBUG, "message: id=0x%0X, trans=%u\n", vl_msg_id, vl_trans_id); */

	/* transaction id and message id */
	pl_ptr16 = tatrf_isi_msg_header(pp_st);
	tat_modem_marsh16(pl_ptr16, vp_op);

	/* do marshal all members except trans_id, msg_id and nbr_of_sb
	 * TODO: add any missing message definition in this switch case */
	switch (vl_msg_id) {
    case C_TEST_PA_INFO_REQ:
        {
            C_TEST_PA_INFO_REQ_STR *pl_req = (C_TEST_PA_INFO_REQ_STR *)pl_ptr16;
            tat_modem_marsh16(&pl_req->system_info, vp_op);
            tat_modem_marsh16(&pl_req->update_pa_type_to_pm, vp_op);
            u16 vl_num_sb_id = 0;
            if ( ISI_MARSH == vp_op )
            {
                vl_num_sb_id = DEREF_PTR(&pl_req->num_sb_id, u16);
                tat_modem_marsh16(&pl_req->num_sb_id, vp_op);
            }
            else
            {
                tat_modem_marsh16(&pl_req->num_sb_id, vp_op);
                vl_num_sb_id = DEREF_PTR(&pl_req->num_sb_id, u16);
            }

            pl_ptr16 = &pl_req->num_sb_id + 1;
            for (vl_i = 0; vl_i < pl_req->num_sb_id; vl_i++)
                tat_modem_marsh16(pl_ptr16++, vp_op);
        }
        break;

    case C_TEST_PA_INFO_RESP:
        {
            C_TEST_PA_INFO_RESP_STR *pl_resp =
                (C_TEST_PA_INFO_RESP_STR *)pl_ptr16;
            tat_modem_marsh16(&pl_resp->fill1, vp_op);
            tat_modem_marsh16(&pl_resp->status, vp_op);
        }
        break;

	case C_TEST_TUNING_REQ:
		{
			C_TEST_TUNING_REQ_STR *pl_req =
			    (C_TEST_TUNING_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->fill1, vp_op);
			tat_modem_marsh16(&pl_req->fill2, vp_op);
		}
		break;

	case C_TEST_TUNING_RESP:
		{
			C_TEST_TUNING_RESP_STR *pl_resp =
			    (C_TEST_TUNING_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->status, vp_op);
			tat_modem_marsh16(&pl_resp->fill2, vp_op);
		}
		break;

	case C_TEST_IQ_SELF_TUNING_REQ:
		{
			C_TEST_IQ_SELF_TUNING_REQ_STR *pl_req =
			    (C_TEST_IQ_SELF_TUNING_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->tx_iq_mode, vp_op);
			tat_modem_marsh16(&pl_req->fill1, vp_op);
		}
		break;

	case C_TEST_IQ_SELF_TUNING_RESP:

		/* this message has no practical member */
		break;

	case C_HAL_RF_TEST_INFO_REQ:
		{
			C_HAL_RF_TEST_INFO_REQ_STR *pl_req =
			    (C_HAL_RF_TEST_INFO_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->info_type, vp_op);
		}
		break;

	case C_HAL_RF_TEST_INFO_RESP:
		{
			C_HAL_RF_TEST_INFO_RESP_STR *pl_resp =
			    (C_HAL_RF_TEST_INFO_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->info_type, vp_op);
			tat_modem_marsh16(&pl_resp->data, vp_op);
		}
		break;

	case C_HAL_RF_TEST_CONTROL_REQ:

		/* this message has no practical member */
		break;

	case C_HAL_RF_TEST_CONTROL_RESP:
		{
			C_HAL_RF_TEST_CONTROL_RESP_STR *pl_resp =
			    (C_HAL_RF_TEST_CONTROL_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->status, vp_op);
		}
		break;

	case C_HAL_RF_TEST_CONTROL_STOP_REQ:
		{
			C_HAL_RF_TEST_CONTROL_STOP_REQ_STR *pl_req =
			    (C_HAL_RF_TEST_CONTROL_STOP_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->fill1, vp_op);
		}
		break;

	case C_HAL_RF_TEST_CONTROL_STOP_RESP:
		{
			C_HAL_RF_TEST_CONTROL_STOP_RESP_STR *pl_resp =
			    (C_HAL_RF_TEST_CONTROL_STOP_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1, vp_op);
		}
		break;

	case C_TEST_RFIC_READ_REQ:
		{
			C_TEST_RFIC_READ_REQ_STR *pl_req =
			    (C_TEST_RFIC_READ_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->accs_incr, vp_op);
			tat_modem_marsh16(&pl_req->address_msw, vp_op);
			tat_modem_marsh16(&pl_req->address_lsw, vp_op);
			tat_modem_marsh16(&pl_req->data_length, vp_op);
			tat_modem_marsh16(&pl_req->fill1_fill1, vp_op);
		}
		break;

	case C_TEST_RFIC_READ_RESP:
		{
			C_TEST_RFIC_READ_RESP_STR *pl_resp =
			    (C_TEST_RFIC_READ_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->mode_status, vp_op);
			tat_modem_marsh16(&pl_resp->data_length, vp_op);
			/* data MUST be processed by the calling code */
		}
		break;

	case C_TEST_RFIC_WRITE_REQ:
		{
			C_TEST_RFIC_WRITE_REQ_STR *pl_req =
			    (C_TEST_RFIC_WRITE_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->accs_incr, vp_op);
			tat_modem_marsh16(&pl_req->address_msw, vp_op);
			tat_modem_marsh16(&pl_req->address_lsw, vp_op);
			tat_modem_marsh16(&pl_req->data_length, vp_op);
			/* data MUST be processed by the calling code */
		}
		break;

	case C_TEST_RFIC_WRITE_RESP:
		{
			C_TEST_RFIC_WRITE_RESP_STR *pl_resp =
			    (C_TEST_RFIC_WRITE_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->mode_status, vp_op);
		}
		break;

	case C_HAL_RF_TUNING_VALUES_GET_REQ:
		{
			C_HAL_RF_TUNING_VALUES_GET_REQ_STR *pl_req =
			    (C_HAL_RF_TUNING_VALUES_GET_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->source, vp_op);
			tat_modem_marsh16(&pl_req->fill1, vp_op);
		}
		break;

	case C_HAL_RF_TUNING_VALUES_GET_RESP:
		{
			C_HAL_RF_TUNING_VALUES_GET_RESP_STR *pl_resp =
			    (C_HAL_RF_TUNING_VALUES_GET_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1_status, vp_op);
			tat_modem_marsh16(&pl_resp->fill2, vp_op);
		}
		break;

	case C_HAL_RF_TUNING_VALUES_SET_REQ:
		{
			C_HAL_RF_TUNING_VALUES_SET_REQ_STR *pl_req =
			    (C_HAL_RF_TUNING_VALUES_SET_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->destination, vp_op);
			tat_modem_marsh16(&pl_req->fill1, vp_op);
		}
		break;

	case C_HAL_RF_TUNING_VALUES_SET_RESP:
		{
			C_HAL_RF_TUNING_VALUES_SET_RESP_STR *pl_resp =
			    (C_HAL_RF_TUNING_VALUES_SET_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1_status, vp_op);
			tat_modem_marsh16(&pl_resp->fill2, vp_op);
		}
		break;

	case C_TEST_ANTENNA_CONTROL_REQ:
		{
			C_TEST_ANTENNA_CONTROL_REQ_STR *pl_req =
			    (C_TEST_ANTENNA_CONTROL_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->fill1, vp_op);
			tat_modem_marsh16(&pl_req->ant_ctrl_mode, vp_op);
		}
		break;

	case C_TEST_ANTENNA_CONTROL_RESP:
		{
			C_TEST_ANTENNA_CONTROL_RESP_STR *pl_resp =
			    (C_TEST_ANTENNA_CONTROL_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1, vp_op);
			tat_modem_marsh16(&pl_resp->fill2_status, vp_op);
		}
		break;

	case STORE_COMMAND_LIST_REQ:
		{
			STORE_COMMAND_LIST_REQ_STR *pl_req =
			    (STORE_COMMAND_LIST_REQ_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_req->num_rem_sub_blocks, vp_op);
			tat_modem_marsh16(&pl_req->fill1, vp_op);
		}
		break;

	case STORE_COMMAND_LIST_RESP:
		{
			STORE_COMMAND_LIST_RESP_STR *pl_resp =
			    (STORE_COMMAND_LIST_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1_status, vp_op);
		}
		break;

	case START_COMMAND_LIST_REQ:
		/* empty message */
		break;

	case START_COMMAND_LIST_RESP:
		{
			START_COMMAND_LIST_RESP_STR *pl_resp =
			    (START_COMMAND_LIST_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1_status, vp_op);
		}
		break;

	case STOP_COMMAND_LIST_REQ:
		/* empty message */
		break;

	case STOP_COMMAND_LIST_RESP:
		{
			STOP_COMMAND_LIST_RESP_STR *pl_resp =
			    (STOP_COMMAND_LIST_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1_status, vp_op);
		}
		break;

	case GET_STATUS_REQ:
		/* empty message */
		break;

	case GET_STATUS_RESP:
		{
			GET_STATUS_RESP_STR *pl_resp =
			    (GET_STATUS_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1_status, vp_op);
		}
		break;

	case GET_RESULTS_REQ:
		/* empty message */
		break;

	case GET_RESULTS_RESP:
		{
			GET_RESULTS_RESP_STR *pl_resp =
			    (GET_RESULTS_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1_status, vp_op);
			tat_modem_marsh16(&pl_resp->fill2_is_remaining_sb,
					  vp_op);
		}
		break;

	case SEQUENCER_RESET_REQ:
		/* empty message */
		break;

	case SEQUENCER_RESET_RESP:
		{
			SEQUENCER_RESET_RESP_STR *pl_resp =
			    (SEQUENCER_RESET_RESP_STR *) pl_ptr16;
			tat_modem_marsh16(&pl_resp->fill1_status, vp_op);
		}
		break;

	default:

		SYSLOG(LOG_ERR,
		       "no method available to encode/decode message 0x%2X",
		       vl_msg_id);
		vl_err = TAT_ERROR_NOT_MATCHING_MSG;
	}

	/* process subblocks if the message has any */
	if ((TAT_OK(vl_err)) && (NULL != pp_st->nb_sb)) {
		/* nb_sb_id : theorical number of subblocks in message but not always the real one.
		   Espacially, when response message buffer size is not sufficient, then the 
		   theorical number of subblock exceeds the number of subblocks which can be stored. */

		if (ISI_MARSH == vp_op) {
			vl_num_sb = DEREF_PTR(pp_st->nb_sb, u16);
			tat_modem_marsh16(pp_st->nb_sb, vp_op);
		} else {
			tat_modem_marsh16(pp_st->nb_sb, vp_op);
			vl_num_sb = DEREF_PTR(pp_st->nb_sb, u16);
		}

		/*SYSLOG(LOG_DEBUG, "Message has %u subblocks", vl_num_sb); */

		pl_ptr16 = pp_st->nb_sb + 1;

		/* process (un)marshalling of the subblock sequence */
		for (vl_sb_ndx = 0; (vl_sb_ndx < vl_num_sb) && TAT_OK(vl_err);
		     vl_sb_ndx++) {
			/* error case: insufficient buffer size for the number of subblocks expected */
			if (pl_msg_end <= pl_ptr16) {
				SYSLOG(LOG_WARNING,
				       "stopped at block %u due to buffer limit",
				       vl_sb_ndx + 1);

				/* overwrite the number of subblocks the structure is really containing */
				*pp_st->nb_sb = vl_sb_ndx;

				/* marshall the new sb count, if message is going out */
				if (ISI_MARSH == vp_op) {
					tat_modem_marsh16(pp_st->nb_sb, vp_op);
				}

				/* end marshalling here */
				break;
			}

			/* block is ok to be marshalled: do it */
			/*SYSLOG(LOG_DEBUG, "marshalling subblock %d at %p\n", pl_sb_ndx + 1, pl_ptr8); */
			vl_sb_len = 0;
			vl_err =
			    tatl17_marsh_sb_common(pl_ptr16, vp_op, &vl_sb_id,
						   &vl_sb_len);
			if (TAT_OK(vl_err)) {
				pl_ptr16 =
				    (u16 *) ((u8 *) pl_ptr16 + vl_sb_len);
				/*SYSLOG(LOG_DEBUG, "subblock %d was : id = %02X, len = %u. Next subblock begins at %p\n", pl_sb_ndx + 1, pl_sb_id, pl_sb_len, pl_ptr8); */
			}
		}
	}

	return vl_err;
}

int tatl17_12marsh_msg(struct tatrf_isi_msg_t *pp_st, TAT_MODEM_ISI_MARSH vp_op)
{
	int vl_err = TAT_ERROR_OFF;

	if (pp_st->encoding == mixed)
		return -1;	/* message is not usable due to previous encoding problem */

	if ((ISI_MARSH == vp_op) && (pp_st->encoding == modem))
		return 0;

	if ((ISI_UNMARSH == vp_op) && (pp_st->encoding == client))
		return 0;

	/* process the message according to its test handler */
	tatrf_isi_marsh_proc pl_proc = NULL;
	u8 vl_resource_id = ((t_isi_header *) pp_st->msg)->resourceID;

	/*SYSLOG(LOG_DEBUG, "message encoding: %d, transform:%d", pp_st->encoding, vp_op); */

	switch (vl_resource_id) {
	case PN_DSP_COMMON_TEST:
		pl_proc = tatl17_marsh_msg_common;
		break;

	case PN_DSP_WCDMA_TEST:
		pl_proc = tatl17_marsh_msg_wcdma;
		break;

	case PN_MODEM_TEST:
		pl_proc = tatl17_marsh_msg_modem;
		break;
	}

	if (pl_proc) {
		vl_err = pl_proc(pp_st, vp_op);
		/* if failed, the message encoding is undetermined, and thus the message is
		 * not usable */
		pp_st->encoding = mixed;
		if (TAT_OK(vl_err)) {
			if (ISI_MARSH == vp_op) {
				pp_st->encoding = modem;
			} else {
				pp_st->encoding = client;
			}
		}
	} else {
		SYSLOG(LOG_CRIT,
		       "no method available to process message of resource ID 0x%X",
		       vl_resource_id);
		vl_err = TAT_ERROR_NOT_MATCHING_MSG;
	}

	return vl_err;
}

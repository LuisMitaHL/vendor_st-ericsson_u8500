/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    pmmacces.c
* \brief   routines to access Permanent Modem Memory
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "pmmacces.h"

#include "misc.h"
#include "isimsg.h"
#include "pmmidmap.h"
#include "rfparams.h"

#include "isi_driver_lib.h"

/* TYPES */
enum { PMM_READ, PMM_WRITE };

union ISI_BASIC_VARIANT {
	s8 _char;
	u8 _byte;
	s16 _short;
	u16 _ushort;
	s32 _int;
	u32 _uint;
	s64 _int64;
	u64 _uint64;
	float _float;
};

/* GLOBALS */
u32 v_tatrf_AccessType = PMM_READ;
u32 v_tatrf_PmmType = C_HAL_RF_TUNING_VALUE_PERM;
u32 v_tatrf_ReadFromPmmErrorState = 0U;

static int tatl5_readDataFromSb(struct dth_element *elem,
				C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR
				* sb, void *Value, float mult, int ptype);

static int tatl5_readArrayFromSb(struct dth_element *elem,
				 C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR
				 * sb, void *Value, float mult, int ptype);

static int tatl5_writeData(struct dth_element *elem,
			   struct tatrf_isi_msg_t *msg, void *Value,
			   int vp_quot, int ptype);

static int tatl5_writeArrayToSb(struct dth_element *elem,
				struct tatrf_isi_msg_t *msg,
				void *Value, int vp_quot, int ptype);

static int tatl5_appendDataToSb(struct dth_element *elem, void *value,
				struct tatrf_isi_msg_t *msg, float mult,
				int ptype);

/* DTH functions callbacks for PMM access */

int DthRf_PMM_Get(struct dth_element *elem, void *value)
{
	return tatl5_01Pmm_GetAndResult(elem, value);
}

int DthRf_PMM_Set(struct dth_element *elem, void *value)
{
	return tatl5_02Pmm_Set(elem, value);
}

int DthRf_Get_PMM_Parameter(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 0);
}

int DthRf_Get_PMM_Parameter_Q1(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 1);
}

int DthRf_Get_PMM_Parameter_Q2(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 2);
}

int DthRf_Get_PMM_Parameter_Q3(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 3);
}

int DthRf_Get_PMM_Parameter_Q6(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 6);
}

int DthRf_Get_PMM_Parameter_Q8(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 8);
}

int DthRf_Get_PMM_Parameter_Q10(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 10);
}

int DthRf_Get_PMM_Parameter_Q12(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 12);
}

int DthRf_Get_PMM_Parameter_Q14(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 14);
}

int DthRf_Get_PMM_Parameter_Q16(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 16);
}

int DthRf_Get_PMM_Parameter_Q23(struct dth_element *elem, void *value)
{
	return tatl5_03Pmm_Read(elem, value, 23);
}

int DthRf_Set_PMM_Parameter(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 0);
}

int DthRf_Set_PMM_Parameter_Q1(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 1);
}

int DthRf_Set_PMM_Parameter_Q2(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 2);
}

int DthRf_Set_PMM_Parameter_Q3(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 3);
}

int DthRf_Set_PMM_Parameter_Q6(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 6);
}

int DthRf_Set_PMM_Parameter_Q8(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 8);
}

int DthRf_Set_PMM_Parameter_Q10(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 10);
}

int DthRf_Set_PMM_Parameter_Q12(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 12);
}

int DthRf_Set_PMM_Parameter_Q14(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 14);
}

int DthRf_Set_PMM_Parameter_Q16(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 16);
}

int DthRf_Set_PMM_Parameter_Q23(struct dth_element *elem, void *value)
{
	return tatl5_04Pmm_Write(elem, value, 23);
}

int tatl5_01Pmm_GetAndResult(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;
	u32 *data = Value;

	switch (elem->user_data) {
	case ACT_PMM_TYPE:	/* PMM or RAM access */
		*data = v_tatrf_PmmType;
		SYSLOG(LOG_DEBUG, "ACT_PMM_TYPE case");
		break;

	case ACT_PMM_ACCESS:	/* READ or WRITE access */
		*data = v_tatrf_AccessType;
		SYSLOG(LOG_DEBUG, "ACT_PMM_ACCESS case");
		break;

	default:		/* error : bad request */
		SYSLOG(LOG_DEBUG, "Error case access memory management");
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl5_02Pmm_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;
	u32 *data = Value;

	switch (elem->user_data) {
	case ACT_PMM_TYPE:	/* PMM or RAM access */
		switch (*data) {
		case 0:	/*RAM ACCESS */
			v_tatrf_PmmType = C_HAL_RF_TUNING_VALUE_RAM;
			SYSLOG(LOG_DEBUG, "RAM access");
			break;

		case 1:	/*PMM ACCESS */
			v_tatrf_PmmType = C_HAL_RF_TUNING_VALUE_PERM;
			SYSLOG(LOG_DEBUG, "PMM access");
			break;

		default:	/* error bad request */
			SYSLOG(LOG_ERR, "Error case Type memory (RAM/PMM)");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_PMM_ACCESS:	/* READ or WRITE access */
		switch (*data) {
		case 0:
			v_tatrf_AccessType = PMM_READ;
			SYSLOG(LOG_DEBUG, "reading access memory");
			break;

		case 1:
			v_tatrf_AccessType = PMM_WRITE;
			SYSLOG(LOG_DEBUG, "writing access memory");
			break;

		default:
			vl_Error = TAT_BAD_REQ;
			SYSLOG(LOG_ERR, "Error case R/W access memory");
		}
		break;

	default:		/* error : bad request */
		SYSLOG(LOG_ERR, "Error case access memory management");
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl5_03Pmm_Read(struct dth_element *pp_elem, void *pp_value, int vp_quot)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	float vl_mult = RF_Q_MULT(vp_quot);
	SYSLOG(LOG_DEBUG, "PMM read %s Q%d multiplier=%f", pp_elem->path,
	       vp_quot, vl_mult);

	int vl_ptype = 0;
	int vl_Error = get_param_store_type(pp_elem->user_data, &vl_ptype);
	if (!TAT_OK(vl_Error))
		goto end;

	/* IMPORTANT: this function is called when writting one element of an
	 * array.
	 * The problem is the calling function also uses tatrf_isi resources
	 * including the shared buffer! If we use it here, we will overwrite
	 * what was done!
	 *
	 * So, it is imperative to have tatl5_03Pmm_Read use a specific
	 * buffer for its ISI message, NOT the one provided by tatl17isi
	 * toolkit.
	 */
	u8 al_readBuffer[ISI_DRIVER_MAX_MESSAGE_LENGTH];
	memset(al_readBuffer, 0, sizeof(t_isi_header));
	vl_msg_info.msg = (u16 *) al_readBuffer;
	vl_msg_info.size = ISI_DRIVER_MAX_MESSAGE_LENGTH;

	C_HAL_RF_TUNING_VALUES_GET_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_HAL_RF_TUNING_VALUES_GET_REQ, nbr_of_sb);
	if (NULL == pl_req)
		goto isierr;

	pl_req->source = v_tatrf_PmmType;

	/* add a subblock to tell the data to write */
	C_HAL_RF_SB_TUNING_VALUES_GET_REQ_STR *pl_sb =
	    tatrf_isi_msg_add_sb(&vl_msg_info,
				 C_HAL_RF_SB_TUNING_VALUES_GET_REQ);
	if (NULL == pl_sb)
		goto isierr;

	pl_sb->tuning_id = pp_elem->user_data;

	/*Send ISI message - Blocking function */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Read PMM");
	if (!TAT_OK(vl_Error))
		goto end;

	/* message successfully sent. waiting for response */
	C_HAL_RF_TUNING_VALUES_GET_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_HAL_RF_TUNING_VALUES_GET_RESP,
			   nbr_of_sb, "Read PMM response", &vl_Error);
	if (NULL == pl_resp)
		goto end;

	/* should have returned one subblock */
	C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR *pl_sbr =
	    tatrf_isi_find_sb(&vl_msg_info, C_HAL_RF_SB_TUNING_VALUES_GET_RESP,
			      NULL);
	if (NULL == pl_sbr)
		goto resperr;

	/* subblock is present */
	if (LOWBYTE(pl_sbr->fill1_status) == C_TEST_OK) {
		if (pl_sbr->tuning_id != pp_elem->user_data)
			goto resperr;

		vl_Error =
		    tatl5_readDataFromSb
		    (pp_elem, pl_sbr, pp_value, vl_mult, vl_ptype);
		if (TAT_OK(vl_Error)) {
			v_tatrf_ReadFromPmmErrorState =
			    LOWBYTE(pl_sbr->fill1_status);
		} else {
			/* TODO: not very coherent: should return a
			 * C_TEST_STATUS code */
			v_tatrf_ReadFromPmmErrorState = vl_Error;
		}
	}

end:
	return vl_Error;

isierr:
	return TAT_ISI_HANDLER_ERROR;

resperr:
	return TAT_ERROR_NOT_MATCHING_MSG;
}

int tatl5_sizeOfType(int type)
{
	int vl_size = 0;
	switch (type) {
	case DTH_TYPE_U8:
	case DTH_TYPE_S8:
		vl_size = sizeof(u8);
		break;

	case DTH_TYPE_U16:
	case DTH_TYPE_S16:
		vl_size = sizeof(u16);
		break;

	case DTH_TYPE_U32:
	case DTH_TYPE_S32:
		vl_size = sizeof(u32);
		break;

	case DTH_TYPE_U64:
	case DTH_TYPE_S64:
		vl_size = sizeof(u64);
		break;

		/* remember this one has meaning only for DTH type, not for PMM
		 * encoding */
	case DTH_TYPE_FLOAT:
		vl_size = sizeof(float);
		break;

	default:
		SYSLOG(LOG_ERR, "unknown type");
	}

	return vl_size;
}

static int tatl5_marshPmmValue(int type,
			       u16 *data,
			       int offset, int size, TAT_MODEM_ISI_MARSH op)
{
	int vl_error = TAT_ERROR_OFF;
	int vl_size = tatl5_sizeOfType(type);
	if (size - offset < vl_size) {
		SYSLOG(LOG_ERR, "r/w overflow");
		return TAT_ERROR_BAD_REQ;
	}

	u16 *pl_data = (u16 *) ((u8 *) data + offset);

	/* marshall value from subblock according to its encoding type */
	switch (type) {
		/* 8 bits is stored in a 16 bits */
	case DTH_TYPE_U8:
	case DTH_TYPE_S8:
		/* no need to marshall */
		break;

	case DTH_TYPE_U16:
	case DTH_TYPE_S16:
		ASSERT(size >= 2, "buffer overflow");
		tat_modem_marsh16(pl_data, op);
		break;

	case DTH_TYPE_U32:
	case DTH_TYPE_S32:
		ASSERT(size >= 4, "buffer overflow");
		tat_modem_marsh32((u32 *) pl_data, op);
		break;

	default:
		/* other physical types are not supported */
		SYSLOG(LOG_ERR, "unsupported encoding type: %d", type);
		vl_error = TAT_ERROR_PARAMETERS;
	}

	return vl_error;
}

static int tatl5_readPmmValue(
		int type,	/* in: PMM storage type */
		u16 *data,	/* in: address of raw data */
		int offset,	/* in: offset from data */
		int size,	/* in: size of data in bytes */
		float mult,	/* in: multiplier or 1.0 if Q0 */
		float *output)	/* out: variable to receive the final value */
{
	int vl_error = TAT_ERROR_OFF;
	int vl_size = tatl5_sizeOfType(type);
	if (size - offset < vl_size) {
		SYSLOG(LOG_ERR, "read overflow");
		return TAT_ERROR_PARAMETERS;
	}

	u16 *pl_data = (u16 *) ((u8 *) data + offset);

	switch (type) {
	case DTH_TYPE_U8:
		*output = DEREF_PTR(pl_data, u8) / mult;
		SYSLOG(LOG_DEBUG,
		       "decode {type=8u, base=%p, offset=%d, mult=%f, value=%u (0x%X)}: %f",
		       data, offset, mult, DEREF_PTR(pl_data, u8),
		       DEREF_PTR(pl_data, u8), *output);
		break;

	case DTH_TYPE_S8:
		*output = DEREF_PTR(pl_data, s8) / mult;
		SYSLOG(LOG_DEBUG,
		       "decode {type=8s, base=%p, offset=%d, mult=%f, value=%d (0x%X)}: %f",
		       data, offset, mult, DEREF_PTR(pl_data, u8),
		       DEREF_PTR(pl_data, s8), *output);
		break;

	case DTH_TYPE_U16:
		*output = DEREF_PTR(pl_data, u16) / mult;
		SYSLOG(LOG_DEBUG,
		       "decode {type=16u, base=%p, offset=%d, mult=%f, value=%u (0x%02X)}: %f",
		       data, offset, mult, DEREF_PTR(pl_data, u16),
		       DEREF_PTR(pl_data, s16), *output);
		break;

	case DTH_TYPE_S16:
		*output = DEREF_PTR(pl_data, s16) / mult;
		SYSLOG(LOG_DEBUG,
		       "decode {type=16s, base=%p, offset=%d, mult=%f, value=%u (0x%02X)}: %f",
		       data, offset, mult, DEREF_PTR(pl_data, s16),
		       DEREF_PTR(pl_data, s16), *output);
		break;

	case DTH_TYPE_U32:
		*output = DEREF_PTR(pl_data, u32) / mult;
		SYSLOG(LOG_DEBUG,
		       "decode {type=32u, base=%p, offset=%d, mult=%f, value=%lu (0x%04X)}: %f",
		       data, offset, mult, DEREF_PTR(pl_data, u32),
		       DEREF_PTR(pl_data, u32), *output);
		break;

	case DTH_TYPE_S32:
		*output = DEREF_PTR(pl_data, s32) / mult;
		SYSLOG(LOG_DEBUG,
		       "decode {type=32s, base=%p, offset=%d, mult=%f, value=%ld (0x%04X)}: %f",
		       data, offset, mult, DEREF_PTR(pl_data, s32),
		       DEREF_PTR(pl_data, s32), *output);
		break;

	case DTH_TYPE_U64:
		*output = DEREF_PTR(pl_data, u64) / mult;
		SYSLOG(LOG_DEBUG,
		       "decode {type=64u, base=%p, offset=%d, mult=%f, value=%llu (0x%08X)}: %f",
		       data, offset, mult, DEREF_PTR(pl_data, u64),
		       DEREF_PTR(pl_data, u64), *output);
		break;

	case DTH_TYPE_S64:
		*output = DEREF_PTR(pl_data, s64) / mult;
		SYSLOG(LOG_DEBUG,
		       "decode {type=64s, base=%p, offset=%d, mult=%f, value=%lld (0x%08X)}: %f",
		       data, offset, mult, DEREF_PTR(pl_data, s64),
		       DEREF_PTR(pl_data, s64), *output);
		break;

	default:
		SYSLOG(LOG_ERR, "unsupported type: %u", type);
		vl_error = TAT_ERROR_PARAMETERS;
	}

	return vl_error;
}

static int tatl5_getTypedValue(int type,
		float value,
		void *output)
{
	int vl_error = TAT_ERROR_OFF;

	switch (type) {
	case DTH_TYPE_U8:
		DEREF_PTR_SET(output, u8, value);
		SYSLOG(LOG_DEBUG, "write u8 addr=%p, value=%f: %u", output,
		       value, DEREF_PTR(output, u8));
		break;

	case DTH_TYPE_S8:
		DEREF_PTR_SET(output, s8, value);
		SYSLOG(LOG_DEBUG, "write s8 addr=%p, value=%f: %d", output,
		       value, DEREF_PTR(output, s8));
		break;

	case DTH_TYPE_U16:
		DEREF_PTR_SET(output, u16, value);
		SYSLOG(LOG_DEBUG, "write u16 addr=%p, value=%f: %u", output,
		       value, DEREF_PTR(output, u16));
		break;

	case DTH_TYPE_S16:
		DEREF_PTR_SET(output, s16, value);
		SYSLOG(LOG_DEBUG, "write s16 addr=%p, value=%f: %d", output,
		       value, DEREF_PTR(output, s16));
		break;

	case DTH_TYPE_U32:
		DEREF_PTR_SET(output, u32, value);
		SYSLOG(LOG_DEBUG, "write u32 addr=%p, value=%f: %lu", output,
		       value, DEREF_PTR(output, u32));
		break;

	case DTH_TYPE_S32:
		DEREF_PTR_SET(output, s32, value);
		SYSLOG(LOG_DEBUG, "write s32 addr=%p, value=%f: %ld", output,
		       value, DEREF_PTR(output, s32));
		break;

	case DTH_TYPE_U64:
		DEREF_PTR_SET(output, u64, value);
		SYSLOG(LOG_DEBUG, "write u64 addr=%p, value=%f: %llu", output,
		       value, DEREF_PTR(output, u64));
		break;

	case DTH_TYPE_S64:
		DEREF_PTR_SET(output, s64, value);
		SYSLOG(LOG_DEBUG, "write s64 addr=%p, value=%f: %lld", output,
		       value, DEREF_PTR(output, s64));
		break;

	case DTH_TYPE_FLOAT:
		DEREF_PTR_SET(output, float, value);
		SYSLOG(LOG_DEBUG, "write float addr=%p, value=%f: %f", output,
		       value, DEREF_PTR(output, float));
		break;

	default:
		SYSLOG(LOG_ERR, "unsupported DTH type: %d", type);
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

static int tatl5_typeValue(int type,	/* in: DTH element's type */
			   void *value,	/* in: user value */
			   float mult,	/* in: multiplier or 1.0 if Q0 */
			   float *result	/* out: typed value */
    )
{
	int vl_error = TAT_ERROR_OFF;

	switch (type) {
	case DTH_TYPE_U8:
		*result = DEREF_PTR(value, u8) * mult;
		break;

	case DTH_TYPE_S8:
		*result = DEREF_PTR(value, s8) * mult;
		break;

	case DTH_TYPE_U16:
		*result = DEREF_PTR(value, u16) * mult;
		break;

	case DTH_TYPE_S16:
		*result = DEREF_PTR(value, s16) * mult;
		break;

	case DTH_TYPE_U32:
		*result = DEREF_PTR(value, u32) * mult;
		break;

	case DTH_TYPE_S32:
		*result = DEREF_PTR(value, s32) * mult;
		break;

	case DTH_TYPE_U64:
		*result = DEREF_PTR(value, u64) * mult;
		break;

	case DTH_TYPE_S64:
		*result = DEREF_PTR(value, s64) * mult;
		break;

	case DTH_TYPE_FLOAT:
		*result = DEREF_PTR(value, float) * mult;
		break;

	default:
		SYSLOG(LOG_ERR, "unsupported DTH type: %d", type);
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

static int tatl5_writePmmValue(
		int type,
		float value,
		union ISI_BASIC_VARIANT *data,
		int size
		)
{
	int vl_error = TAT_ERROR_OFF;
	int vl_size = tatl5_sizeOfType(type);
	if (size < vl_size) {
		SYSLOG(LOG_ERR, "write overflow");
		return TAT_ERROR_PARAMETERS;
	}

	switch (type) {
	case DTH_TYPE_U8:
	case DTH_TYPE_S8:
		SYSLOG(LOG_ERR,
		       "modem expects word encoding. 8 bits does not fit.");
		vl_error = TAT_ERROR_CASE;
		break;

	case DTH_TYPE_U16:
		data->_ushort = value;
		break;

	case DTH_TYPE_S16:
		data->_short = value;
		break;

	case DTH_TYPE_U32:
		data->_uint = value;
		break;

	case DTH_TYPE_S32:
		data->_int = value;
		break;

	case DTH_TYPE_U64:
		data->_uint64 = value;
		break;

	case DTH_TYPE_S64:
		data->_int64 = value;
		break;

	default:
		SYSLOG(LOG_ERR, "unsupported type: %u", type);
		vl_error = TAT_ERROR_CASE;
	}

	return vl_error;
}

int tatl5_readArrayFromSb(struct dth_element *elem,
			  C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR *
			  sb, void *Value, float mult, int ptype)
{
	int vl_Error = TAT_ERROR_OFF;

	/* marsh the entire array */
	int vl_nbElem = elem->rows * elem->cols;
	int vl_sbSize = tatl5_sizeOfType(ptype);
	/* what if the data is not long enough? */
	if ((vl_nbElem * vl_sbSize) > (int)(sb->nbr_of_words * sizeof(u16)))
		/* warn and read only data provided */
		vl_nbElem = (sb->nbr_of_words * sizeof(u16)) / vl_sbSize;

	/* retrieve the entire table or one of its cell? */
	struct dth_array *pl_array = (struct dth_array *)Value;
	if ((pl_array->row < elem->rows) && (pl_array->col < elem->cols))
		vl_nbElem = 1;

	int vl_dthSize = tatl5_sizeOfType(elem->type);
	float vl_value;
	int vl_sbOffset, vl_dthOffset;
	int vl_sbTotalSize;

	if (vl_nbElem == 1) {
		SYSLOG(LOG_DEBUG, "get PMM %s(r=%d, c=%d)",
		       elem->path, pl_array->row, pl_array->col);

		vl_sbOffset =
		    (pl_array->row * elem->cols + pl_array->col) * vl_sbSize;
		vl_sbTotalSize = sb->nbr_of_words * sizeof(u16);

		vl_Error =
		    tatl5_marshPmmValue(ptype, sb->data, vl_sbOffset,
					vl_sbTotalSize, ISI_UNMARSH);

		if (TAT_OK(vl_Error))
			vl_Error =
			    tatl5_readPmmValue(ptype, sb->data,
					       vl_sbOffset, vl_sbTotalSize,
					       mult, &vl_value);

		if (TAT_OK(vl_Error))
			/* NB: the value is returned at the begin of the
			 * array */
			vl_Error = tatl5_getTypedValue(elem->type, vl_value,
				(u8 *) pl_array->array);
	} else {
		vl_sbOffset = vl_dthOffset = 0;
		vl_sbTotalSize = sb->nbr_of_words * sizeof(u16);

		int ielem;
		for (ielem = 0; (ielem < vl_nbElem) && TAT_OK(vl_Error);
		     ielem++) {
			SYSLOG(LOG_DEBUG, "get PMM %s(r=%d, c=%d)", elem->path,
			       ielem / elem->cols, ielem % elem->cols);

			vl_Error =
			    tatl5_marshPmmValue(ptype, sb->data,
						vl_sbOffset, vl_sbTotalSize,
						ISI_UNMARSH);

			if (TAT_OK(vl_Error))
				vl_Error =
				    tatl5_readPmmValue(ptype, sb->data,
						       vl_sbOffset,
						       vl_sbTotalSize, mult,
						       &vl_value);

			if (TAT_OK(vl_Error))
				vl_Error =
				    tatl5_getTypedValue(elem->type, vl_value,
							(u8 *) pl_array->array +
							vl_dthOffset);

			vl_sbOffset += vl_sbSize;
			vl_dthOffset += vl_dthSize;
		}
	}

	return vl_Error;
}

int tatl5_readDataFromSb(struct dth_element *elem,
			 C_HAL_RF_SB_TUNING_VALUES_GET_RESP_STR *sb,
			 void *Value, float mult, int ptype)
{
	int vl_Error = TAT_ERROR_OFF;
	float vl_value;

	if (elem->rows == 0 && elem->cols == 0) {
		/* marshall value from subblock according to its encoding
		 * type */
		vl_Error = tatl5_marshPmmValue(ptype, sb->data, 0,
					       sb->nbr_of_words * sizeof(u16),
					       ISI_UNMARSH);

		if (TAT_OK(vl_Error))
			vl_Error = tatl5_readPmmValue(ptype, sb->data, 0,
						      sb->nbr_of_words *
						      sizeof(u16), mult,
						      &vl_value);
		if (TAT_OK(vl_Error))
			vl_Error =
			    tatl5_getTypedValue(elem->type, vl_value, Value);
	} else
		vl_Error = tatl5_readArrayFromSb(elem, sb, Value, mult, ptype);

	return vl_Error;
}

/******************************************************************/
/* Write In PMM (Permanente Memory)                               */
/******************************************************************/

int tatl5_appendDataToSb(struct dth_element *elem, void *value,
			 struct tatrf_isi_msg_t *msg, float mult, int ptype)
{
	int vl_Error = TAT_ERROR_OFF;

	float vl_value;
	vl_Error = tatl5_typeValue(elem->type, value, mult, &vl_value);

	union ISI_BASIC_VARIANT vl_pmmValue;
	if (TAT_OK(vl_Error))
		vl_Error =
		    tatl5_writePmmValue(ptype, vl_value,
					&vl_pmmValue,
					sizeof(union ISI_BASIC_VARIANT));

	if (TAT_OK(vl_Error))
		vl_Error =
		    tatl5_marshPmmValue(ptype, (u16 *) &vl_pmmValue, 0,
					sizeof(u64), ISI_MARSH);

	/* size in words */
	int vl_pmmSize = tatl5_sizeOfType(ptype) / sizeof(u16);
	if (TAT_OK(vl_Error))
		vl_Error =
		    tatrf_isi_append_data(msg, (const u16 *)&vl_pmmValue,
					  vl_pmmSize);

	if (TAT_OK(vl_Error)) {
		C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *pl_sb =
		    (C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *) msg->sb;
		pl_sb->nbr_of_words += vl_pmmSize;
	}

	return vl_Error;
}

/* \reentrant if assigning a single cell of the array */
int tatl5_writeArrayToSb(struct dth_element *elem,
			 struct tatrf_isi_msg_t *msg, void *Value, int vp_quot,
			 int ptype)
{
	int vl_Error = TAT_ERROR_OFF;
	int vl_nbElem = elem->rows * elem->cols;
	int vl_sbSize = tatl5_sizeOfType(ptype);
	float vl_mult = RF_Q_MULT(vp_quot);

	/* write the entire table or one of its cell? */
	struct dth_array *pl_array = (struct dth_array *)Value;
	if ((pl_array->row < elem->rows) && (pl_array->col < elem->cols))
		vl_nbElem = 1;

	SYSLOG(LOG_DEBUG, "nb elem to write: %d", vl_nbElem);

	/* allocate subblock data */
	int vl_sbTotalSize, vl_sbAlignSize;
	vl_sbTotalSize = vl_sbAlignSize = elem->cols * elem->rows * vl_sbSize;

	/* subblock must be aligned on 32 bits, so we'll have to add padding */
	if (vl_sbTotalSize % sizeof(u32))
		vl_sbAlignSize = (vl_sbTotalSize / sizeof(u32) + 1) * vl_sbSize;

	int vl_dthSize = tatl5_sizeOfType(elem->type);
	if (vl_nbElem == 1) {
		/* can only write the entire array. So here we need to read the
		 * array, overwrite the cell value and then write the updated
		 * array */
		struct dth_array vl_array;
		vl_array.col = elem->cols;
		vl_array.row = elem->rows;
		vl_array.array =
		    (s64 *) calloc(elem->cols * elem->rows, vl_dthSize);
		vl_Error = tatl5_03Pmm_Read(elem, &vl_array, vp_quot);
		int vl_dthOffset =
		    (pl_array->row * elem->cols + pl_array->col) * vl_dthSize;

		memcpy((u8 *) vl_array.array + vl_dthOffset,
		       (u8 *) pl_array->array, vl_dthSize);

		vl_Error =
		    tatl5_writeArrayToSb(elem, msg, &vl_array, vp_quot, ptype);

		free(vl_array.array);
	} else {
		int i;
		int vl_sbOffset, vl_dthOffset;
		vl_sbOffset = vl_dthOffset = 0;

		for (i = 0; (i < vl_nbElem) && TAT_OK(vl_Error); i++) {
			vl_Error = tatl5_appendDataToSb(elem, (u8 *)
							pl_array->array +
							vl_dthOffset, msg,
							vl_mult, ptype);

			vl_dthOffset += vl_dthSize;
		}
	}

	return vl_Error;
}

int tatl5_writeData(struct dth_element *elem,
		    struct tatrf_isi_msg_t *msg, void *Value,
		    int vp_quot, int ptype)
{
	int vl_Error = TAT_ERROR_OFF;

	C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *pl_sb =
	    (C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *) msg->sb;
	pl_sb->nbr_of_words = 0;

	if (elem->rows == 0 && elem->cols == 0) {
		SYSLOG(LOG_DEBUG, "write scalar");
		vl_Error = tatl5_appendDataToSb(elem, Value, msg,
						RF_Q_MULT(vp_quot), ptype);
	} else {
		SYSLOG(LOG_DEBUG, "write array");
		vl_Error =
		    tatl5_writeArrayToSb(elem, msg, Value, vp_quot, ptype);
	}

	return vl_Error;
}

int tatl5_pmmWrite(struct dth_element *pp_elem, void *pp_value, int vp_quot)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	float vl_mult = RF_Q_MULT(vp_quot);
	SYSLOG(LOG_DEBUG, "PMM write %s Q%d multiplier=%f", pp_elem->path,
	       vp_quot, vl_mult);

	int vl_ptype = 0;
	int vl_Error = get_param_store_type(pp_elem->user_data, &vl_ptype);
	if (!TAT_OK(vl_Error))
		goto end;

	C_HAL_RF_TUNING_VALUES_SET_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_HAL_RF_TUNING_VALUES_SET_REQ, nbr_of_sb);
	if (NULL == pl_req)
		goto isierr;

	pl_req->destination = v_tatrf_PmmType;

	/* add a subblock to tell the data to write */
	C_HAL_RF_SB_TUNING_VALUES_SET_REQ_STR *pl_sb =
	    tatrf_isi_msg_add_sb(&vl_msg_info,
				 C_HAL_RF_SB_TUNING_VALUES_SET_REQ);
	if (NULL == pl_sb)
		goto isierr;

	pl_sb->tuning_id = pp_elem->user_data;

	/* fill data member of the subblock */
	tatl5_writeData(pp_elem, &vl_msg_info, pp_value, vp_quot, vl_ptype);

	/*Send ISI message - Blocking function */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Write PMM");
	if (!TAT_OK(vl_Error))
		goto end;

	/* message successfully sent. waiting for response */
	C_HAL_RF_TUNING_VALUES_SET_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_HAL_RF_TUNING_VALUES_SET_RESP,
			   nbr_of_sb, "Write PMM response", &vl_Error);
	if (NULL == pl_resp)
		goto msgerr;

	/* should have returned one subblock */
	C_HAL_RF_SB_TUNING_VALUES_SET_RESP_STR *pl_sbr =
	    tatrf_isi_find_sb(&vl_msg_info,
			      C_HAL_RF_SB_TUNING_VALUES_SET_RESP, NULL);
	if (NULL == pl_sbr)
		goto msgerr;

	/* subblock is present */
	u8 vl_test_status = LOWBYTE(pl_sbr->fill1_status);

	/*  data should be related to the parameter specified */
	if (C_TEST_OK == vl_test_status)
		if (pl_sbr->tuning_id != pp_elem->user_data)
			goto msgerr;

	v_tatrf_ReadFromPmmErrorState = vl_test_status;
	vl_Error = TAT_ERROR_OFF;

end:
	return vl_Error;

isierr:
	return TAT_ISI_HANDLER_ERROR;

msgerr:
	return TAT_ERROR_NOT_MATCHING_MSG;
}

int tatl5_04Pmm_Write(struct dth_element *elem, void *Value, int q)
{
	int vl_Error = TAT_ERROR_OFF;

	if (v_tatrf_AccessType == PMM_WRITE)
		vl_Error = tatl5_pmmWrite(elem, Value, q);
	else
		SYSLOG(LOG_WARNING, "not currently in PMM WRITE access mode");

	return vl_Error;
}

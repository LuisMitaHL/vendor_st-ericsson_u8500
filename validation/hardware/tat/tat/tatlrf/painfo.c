/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   painfo.c
* \brief   this module implements HATS RF service related to PA INFO
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "painfo.h"
#include "isimsg.h"
#include "misc.h"

static C_TEST_PA_INFO_REQ_STR v_tatrf_painfo;
static C_TEST_PA_INFO_RESP_STR v_tatrf_painfo_resp;

/* buffer size should be enough to cover for SIZE_C_TEST_SB_PA_INFO_STR and the
   manufacturer information string */
#define PA_SB_INFO_MAX_SIZE         512

#define PA_INFO_INIT_NOT_DONE       0xEA
#define PA_INFO_INIT_DONE           0x47

static char v_tatrf_painfo_buffer[PA_SB_INFO_MAX_SIZE];
static C_TEST_SB_PA_INFO_STR *v_tatrf_sb_painfo =
    (C_TEST_SB_PA_INFO_STR *) v_tatrf_painfo_buffer;
static size_t v_tatrf_sb_painfo_size = SIZE_C_TEST_SB_PA_INFO_STR;

static C_TEST_SB_PA_BIAS_INFO_STR v_tatrf_sb_pa_bias_info;

static int v_tatrf_painfo_available = PA_INFO_TYPE_NONE;

static void painfo_init();

int DthRf_PaInfo_set(struct dth_element *elem, void *value)
{
	int vl_result = TAT_ERROR_OFF;

	painfo_init();

	switch (elem->user_data) {
	case ACT_PA_INFO:
		/* just return OK */
		break;

	case ACT_PA_INFO_SYSTEM:

		DTH_SET_UNSIGNED(elem, value, v_tatrf_painfo.system_info,
				 vl_result);
		break;

	case ACT_PA_INFO_SAVE_OR_UPDATE:

		DTH_SET_L8(elem, value, v_tatrf_painfo.update_pa_type_to_pm,
			   vl_result);
		break;

	default:
		vl_result = TAT_BAD_REQ;
	}

	return vl_result;
}

int DthRf_PaInfo_get(struct dth_element *elem, void *value)
{
	int vl_result = TAT_ERROR_OFF;
	char *pl_str = NULL;

	painfo_init();

	switch (elem->user_data) {
	case ACT_PA_INFO_SYSTEM:

		DTH_GET_UNSIGNED(v_tatrf_painfo.system_info, elem, value,
				 vl_result);
		break;

	case ACT_PA_INFO_STATUS:

		DTH_GET_UNSIGNED(v_tatrf_painfo_resp.status, elem, value,
				 vl_result);
		break;

	case ACT_PA_INFO_PA_INFO_STATUS:

		DTH_GET_UNSIGNED(v_tatrf_sb_painfo->status, elem, value,
				 vl_result);
		break;

	case ACT_PA_INFO_PA_INFO_SYSTEM:

		DTH_GET_UNSIGNED(v_tatrf_sb_painfo->system_info, elem, value,
				 vl_result);
		break;

	case ACT_PA_INFO_PA_INFO_VERSION:

		DTH_GET_UNSIGNED(v_tatrf_sb_painfo->version, elem, value,
				 vl_result);
		break;

	case ACT_PA_INFO_PA_INFO_VENDOR:

		pl_str = (char *)value;
		strcpy(pl_str, (char *)v_tatrf_sb_painfo + SIZE_C_TEST_SB_PA_INFO_STR);
		break;

	case ACT_PA_INFO_PA_BIAS_STATUS:

		DTH_GET_UNSIGNED(v_tatrf_sb_pa_bias_info.status, elem, value,
				 vl_result);
		break;

	case ACT_PA_INFO_PA_BIAS_SYSTEM:

		DTH_GET_UNSIGNED(v_tatrf_sb_pa_bias_info.system_info, elem,
				 value, vl_result);
		break;

	case ACT_PA_INFO_PA_BIAS_BIAS1_VALUE:

		DTH_GET_UNSIGNED(v_tatrf_sb_pa_bias_info.bias1_const, elem,
				 value, vl_result);
		break;

	case ACT_PA_INFO_PA_BIAS_BIAS1_CURRENT:

		DTH_GET_UNSIGNED(v_tatrf_sb_pa_bias_info.bias1_current, elem,
				 value, vl_result);
		break;

	case ACT_PA_INFO_PA_BIAS_BIAS2_SETTING:

		DTH_GET_UNSIGNED(v_tatrf_sb_pa_bias_info.bias2_default_dac,
				 elem, value, vl_result);
		break;

	case ACT_PA_INFO_PA_BIAS_TUNING_TARGET:

		DTH_GET_UNSIGNED(v_tatrf_sb_pa_bias_info.target_value, elem,
				 value, vl_result);
		break;

	default:
		vl_result = TAT_BAD_REQ;
	}

	return vl_result;
}

int DthRf_PaInfo_exec(struct dth_element *elem)
{
	int vl_result = TAT_ERROR_OFF;
	int vl_type = 0;

	painfo_init();

	switch (elem->user_data) {
	case ACT_PA_INFO:

		vl_type = PA_INFO_TYPE_PA_ALL;
		vl_result = painfo_saveOrUpdatePAInfo(&vl_type,
						      v_tatrf_painfo.
						      system_info,
						      v_tatrf_painfo.
						      update_pa_type_to_pm);
		if (!TAT_OK(vl_result))
			goto end;

		v_tatrf_painfo_available = vl_type;

		painfo_getPAInfo(v_tatrf_sb_painfo, PA_SB_INFO_MAX_SIZE);

		painfo_getPABiasInfo(&v_tatrf_sb_pa_bias_info);
		break;

	default:
		vl_result = TAT_BAD_REQ;
	}

end:
	return vl_result;
}

static void painfo_init()
{
	static int v_tatrf_painfo_init_done = PA_INFO_INIT_NOT_DONE;

	if (PA_INFO_INIT_NOT_DONE == v_tatrf_painfo_init_done) {
		/* first init of PA info data */

		memset(&v_tatrf_painfo, 0, SIZE_C_TEST_PA_INFO_REQ_STR);
		v_tatrf_painfo.id = MAKE16(0, C_TEST_PA_INFO_REQ);
		v_tatrf_painfo.system_info = C_TEST_GSM;
		v_tatrf_painfo.update_pa_type_to_pm =
		    MAKE16(ISIFILLER8, C_TEST_FALSE);
		v_tatrf_painfo.num_sb_id = 0;

		memset(&v_tatrf_painfo_resp, 0, SIZE_C_TEST_PA_INFO_RESP_STR);
		v_tatrf_painfo_resp.id = MAKE16(0, C_TEST_PA_INFO_RESP);
		v_tatrf_painfo_resp.fill1 = ISIFILLER16;
		v_tatrf_painfo_resp.status =
		    MAKE16(ISIFILLER8, C_TEST_NOT_VALID);
		v_tatrf_painfo_resp.num_sb_id = 0;

		memset(v_tatrf_painfo_buffer, 0, PA_SB_INFO_MAX_SIZE);
		v_tatrf_sb_painfo_size = SIZE_C_TEST_SB_PA_INFO_STR;
		v_tatrf_sb_painfo->sb_id = C_TEST_SB_PA_INFO;
		v_tatrf_sb_painfo->sb_len = SIZE_C_TEST_SB_PA_INFO_STR;
		v_tatrf_sb_painfo->status =
		    MAKE16(ISIFILLER8, C_TEST_NOT_VALID);
		v_tatrf_sb_painfo->system_info = C_TEST_GSM;
		v_tatrf_sb_painfo->version = 0;

		memset(&v_tatrf_sb_pa_bias_info, 0,
		       SIZE_C_TEST_SB_PA_BIAS_INFO_STR);
		v_tatrf_sb_pa_bias_info.sb_id = C_TEST_SB_PA_BIAS_INFO;
		v_tatrf_sb_pa_bias_info.sb_len =
		    SIZE_C_TEST_SB_PA_BIAS_INFO_STR;
		v_tatrf_sb_pa_bias_info.status =
		    MAKE16(ISIFILLER8, C_TEST_NOT_VALID);
		v_tatrf_sb_pa_bias_info.system_info = C_TEST_GSM;
		v_tatrf_sb_pa_bias_info.bias1_const = 0;
		v_tatrf_sb_pa_bias_info.bias1_current = 0;
		v_tatrf_sb_pa_bias_info.bias2_default_dac = 0;
		v_tatrf_sb_pa_bias_info.target_value = 0;

		v_tatrf_painfo_available = PA_INFO_TYPE_NONE;
	}

	v_tatrf_painfo_init_done = PA_INFO_INIT_DONE;
}

int
painfo_saveOrUpdatePAInfo(int *pp_type, uint16 vp_system,
			  uint16 vp_saveOrUpdate)
{
	int vl_result = TAT_ERROR_OFF;
	int vl_type = *pp_type;
	uint16 vl_sb_pa_type = 0;
	C_TEST_PA_INFO_RESP_STR *pl_resp = NULL;
	C_TEST_SB_PA_INFO_STR *pl_sb_info = NULL;
	C_TEST_SB_PA_BIAS_INFO_STR *pl_sb_bias_info = NULL;

	painfo_init();

	/* Send PA_INFO_REQ for update */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_PA_INFO_REQ_STR *pl_req = tatrf_isi_msg_begin_no_sb(&vl_msg_info,
								   PN_DSP_COMMON_TEST,
								   C_TEST_PA_INFO_REQ);
	if (NULL == pl_req) {
		vl_result = TAT_ISI_HANDLER_ERROR;
		goto end;
	}

	pl_req->system_info = vp_system;
	pl_req->update_pa_type_to_pm = vp_saveOrUpdate;
	pl_req->num_sb_id = 0;

	if ((vl_type & PA_INFO_TYPE_PA_INFO) != 0) {
		pl_req->num_sb_id++;
		vl_sb_pa_type = C_TEST_SB_PA_INFO;
		tatl17_04appendData(&vl_msg_info, &vl_sb_pa_type, 1);
	}

	if ((vl_type & PA_INFO_TYPE_PA_BIAS_INFO) != 0) {
		pl_req->num_sb_id++;
		vl_sb_pa_type = C_TEST_SB_PA_BIAS_INFO;
		tatl17_04appendData(&vl_msg_info, &vl_sb_pa_type, 1);
	}

	vl_result = tatrf_isi_send(&vl_msg_info, "Update PA INFO");
	if (!TAT_OK(vl_result))
		goto end;

	/* message successfully sent. waiting for response */
	pl_resp = tatrf_isi_read(&vl_msg_info, C_TEST_PA_INFO_RESP, num_sb_id,
				 "Update PA INFO response", &vl_result);
	if (!TAT_OK(vl_result))
		goto end;

	if (pl_resp == NULL) {
		vl_result = TAT_ERROR_NOT_MATCHING_MSG;
		goto end;
	}

	memcpy(&v_tatrf_painfo_resp, pl_resp, SIZE_C_TEST_PA_INFO_RESP_STR);

	if (C_TEST_FALSE == vp_saveOrUpdate) {
		/* updating PA's information: search for the wanted information */

		*pp_type = PA_INFO_TYPE_NONE;
		memset(v_tatrf_painfo_buffer, 0, PA_SB_INFO_MAX_SIZE);
		v_tatrf_sb_painfo_size = 0;
		v_tatrf_sb_painfo->status = C_TEST_NOT_VALID;

		memset(&v_tatrf_sb_pa_bias_info, 0,
		       SIZE_C_TEST_SB_PA_BIAS_INFO_STR);
		v_tatrf_sb_pa_bias_info.status = C_TEST_NOT_VALID;

		if ((vl_type & PA_INFO_TYPE_PA_INFO) != 0) {
			pl_sb_info =
			    tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_PA_INFO,
					      NULL);
			if (NULL != pl_sb_info) {
				v_tatrf_sb_painfo_size = pl_sb_info->sb_len;
				memcpy(v_tatrf_painfo_buffer, pl_sb_info,
				       v_tatrf_sb_painfo_size);
				*pp_type |= PA_INFO_TYPE_PA_INFO;
			}
		}

		if ((vl_type & PA_INFO_TYPE_PA_BIAS_INFO) != 0) {
			pl_sb_bias_info = tatrf_isi_find_sb(&vl_msg_info,
							    C_TEST_SB_PA_BIAS_INFO,
							    NULL);
			if (NULL != pl_sb_bias_info) {
				memcpy(&v_tatrf_sb_pa_bias_info,
				       pl_sb_bias_info,
				       SIZE_C_TEST_SB_PA_BIAS_INFO_STR);
				*pp_type |= PA_INFO_TYPE_PA_BIAS_INFO;
			}
		}
	}

end:
	return vl_result;
}

int painfo_infoAvailable()
{
	painfo_init();

	return v_tatrf_painfo_available;
}

int painfo_getPAInfo(C_TEST_SB_PA_INFO_STR *pp_info, size_t vp_size)
{
	int vl_res = -1;

	painfo_init();

	if (vp_size < v_tatrf_sb_painfo_size) {
		vl_res = -2;	/* would overflow */
	} else if ((v_tatrf_painfo_available & PA_INFO_TYPE_PA_INFO) != 0) {
		memcpy(pp_info, v_tatrf_painfo_buffer, v_tatrf_sb_painfo_size);
		vl_res = 0;
	}

	return vl_res;
}

int painfo_getPABiasInfo(C_TEST_SB_PA_BIAS_INFO_STR *pp_info)
{
	int vl_res = -1;

	painfo_init();

	if ((v_tatrf_painfo_available & PA_INFO_TYPE_PA_INFO) != 0) {
		memcpy(pp_info, &v_tatrf_sb_pa_bias_info,
		       SIZE_C_TEST_SB_PA_BIAS_INFO_STR);
		vl_res = 0;
	}

	return vl_res;
}

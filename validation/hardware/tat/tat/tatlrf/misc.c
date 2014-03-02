/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   misc.c
* \brief   generic utilities
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"

#include "isimsg.h"
#include "misc.h"

u16 v_RxRssiInfoPath = 0U;
C_HAL_RF_TEST_INFO_RESP_STR v_tatmisc_TestInfo_conf;	/* ISI message in response of a GetRfInfo request */

/* Function tide to RF Get Info */
/********************************/
int DthRf_GetRfInfo_exec(struct dth_element *elem)
{
	return tatl3_00GetRfInfo_exec(elem);
}

int DthRf_GetRfInfoParam_set(struct dth_element *elem, void *value)
{
	return tatl3_04GetRfInfo_ParamSet(elem, value);
}

int DthRf_GetRfInfoParam_get(struct dth_element *elem, void *value)
{
	return tatl3_02GetRfInfoParam_get(elem, value);
}

typedef struct {
	int band_index;
	u32 gsm_band;
	u32 wcdma_band;
} RF_BAND_INFO;

#define COUNT_RF_TUNING_BANDS (sizeof(v_tatrf_bands)/sizeof(RF_BAND_INFO))

const RF_BAND_INFO v_tatrf_bands[] = {
	{.band_index = RF_BAND_0,.gsm_band = INFO_NO_GSM,.wcdma_band =
	 INFO_NO_WCDMA},
	{.band_index = RF_BAND_1,.gsm_band = INFO_GSM850,.wcdma_band =
	 INFO_WCDMA_FDD_BAND1},
	{.band_index = RF_BAND_2,.gsm_band = INFO_GSM900,.wcdma_band =
	 INFO_WCDMA_FDD_BAND2},
	{.band_index = RF_BAND_3,.gsm_band = INFO_GSM1800,.wcdma_band =
	 INFO_WCDMA_FDD_BAND3},
	{.band_index = RF_BAND_4,.gsm_band = INFO_GSM1900,.wcdma_band =
	 INFO_WCDMA_FDD_BAND4},
	{.band_index = RF_BAND_5,.gsm_band = INFO_NO_GSM,.wcdma_band =
	 INFO_WCDMA_FDD_BAND5},
	{.band_index = RF_BAND_6,.gsm_band = INFO_NO_GSM,.wcdma_band =
	 INFO_WCDMA_FDD_BAND6},
	{.band_index = RF_BAND_7,.gsm_band = INFO_NO_GSM,.wcdma_band =
	 INFO_WCDMA_FDD_BAND7},
	{.band_index = RF_BAND_8,.gsm_band = INFO_NO_GSM,.wcdma_band =
	 INFO_WCDMA_FDD_BAND8},
	{.band_index = RF_BAND_9,.gsm_band = INFO_NO_GSM,.wcdma_band =
	 INFO_WCDMA_FDD_BAND9},
	{.band_index = RF_NO_BAND,.gsm_band = INFO_NO_GSM,.wcdma_band =
	 INFO_NO_WCDMA}
};

u16 SetU8InU16(t_position vp_position, u16 vp_dest, u8 vp_srce)
{
	u16 temp;

	switch (vp_position) {
	case LSB_POSS:
		temp = 0x0000 | vp_srce;
		vp_dest = vp_dest & 0xFF00;
		vp_dest = vp_dest | temp;
		break;

	case MSB_POSS:
		temp = (u16) (vp_srce << 8);
		vp_dest = vp_dest & 0x00FF;
		vp_dest = vp_dest | temp;
		break;
	}

	return (vp_dest);
}

u8 GetU8InU16(t_position vp_position, u16 vp_dest)
{
	u8 vl_u8 = 0xFF;
	switch (vp_position) {
	case LSB_POSS:
		vl_u8 = (u8) vp_dest;
		break;

	case MSB_POSS:
		vl_u8 = (u8) (vp_dest >> 8);
		break;

	default:
		SYSLOG(LOG_ERR, "error: invalid position in GetU8InU16");
	}

	return vl_u8;
}

u32 SetU16InU32(t_position vp_position, u32 vp_dest, u16 vp_srce)
{
	u32 temp;

	switch (vp_position) {
	case LSB_POSS:
		temp = 0x00000000 | vp_srce;
		vp_dest = vp_dest & 0xFFFF0000;
		vp_dest = vp_dest | temp;
		break;

	case MSB_POSS:
		temp = (u32) (vp_srce << 16);
		vp_dest = vp_dest & 0x0000FFFF;
		vp_dest = vp_dest | temp;
		break;
	}

	return (vp_dest);
}

u16 GetU16InU32(t_position vp_position, u32 vp_dest)
{
	u16 vl_u16 = 0xFFFF;
	switch (vp_position) {
	case LSB_POSS:
		vl_u16 = (u16) vp_dest;
		break;

	case MSB_POSS:
		vl_u16 = (u16) (vp_dest >> 16);
		break;

	default:
		SYSLOG(LOG_ERR, "error: invalid position in GetU8InU16");
	}

	return vl_u16;
}

void PrintBinary(void *pp_buf, size_t vp_size)
{
	size_t i;
	u8 *pl_bytes = (u8 *) pp_buf;

	SYSLOGSTR(LOG_DEBUG, "\n");

	for (i = 0; i < vp_size; i++) {
		if (i == 0) {
			SYSLOGSTR(LOG_DEBUG, "%8p - ", pl_bytes + i);
		} else if ((i > 0) && ((i % 16) == 0)) {
			SYSLOGSTR(LOG_DEBUG, "\n%8p - ", pl_bytes + i);
		} else if ((i > 0) && ((i % 4) == 0)) {
			SYSLOGSTR(LOG_DEBUG, " ");
		}

		SYSLOGSTR(LOG_DEBUG, "%02X", pl_bytes[i]);
	}

	SYSLOGSTR(LOG_DEBUG, "\n%u bytes.\n", vp_size);
}

void GetDthArray(struct dth_element *pl_elem,
		 struct dth_array *pl_array,
		 int pl_array_size,
		 void *pl_data, int pl_data_size, int pl_nb_data)
{
	int col, row;
	u8 *vl_dest8 = (u8 *) pl_array->array;
	u8 *vl_src8 = (u8 *) pl_data;
	u8 *vl_src_end8 = (u8 *) pl_data + (pl_nb_data * pl_data_size);

	if ((pl_array->col == pl_elem->cols)
	    && (pl_array->row == pl_elem->rows)) {
		/* fill array with all data elements */
		for (row = 0; (row < pl_elem->rows) && (vl_src8 <= vl_src_end8);
		     row++) {
			for (col = 0;
			     (col < pl_elem->cols) && (vl_src8 <= vl_src_end8);
			     col++, vl_dest8 += pl_array_size, vl_src8 +=
			     pl_data_size) {
				memset(vl_dest8, 0, pl_array_size);
				memcpy(vl_dest8, vl_src8,
				       min(pl_array_size, pl_data_size));
			}
		}
	} else if ((pl_array->col < pl_elem->cols)
		   && (pl_array->row < pl_elem->rows)) {
		/* fill the cell (row, col) at array[0] */
		int32 offset_data =
		    (pl_array->row * pl_elem->cols +
		     pl_array->col) * pl_data_size;
		if (offset_data <= (pl_nb_data * pl_data_size)) {
			memset(vl_dest8, 0, pl_array_size);
			memcpy(vl_dest8, vl_src8 + offset_data,
			       min(pl_array_size, pl_data_size));
		}
	} else {
		/* error: array col or/and row are out of range. */
		SYSLOG(LOG_ERR, "error: cell (c:%d, r:%d) is out of range!\n",
		       pl_array->col, pl_array->row);
	}
}

void SetDthArray(struct dth_element *pl_elem,
		 struct dth_array *pl_array,
		 int pl_array_size,
		 void *pl_data, int pl_data_size, int pl_nb_data)
{
	int col, row;
	u8 *vl_src8 = (u8 *) pl_array->array;
	u8 *vl_dest8 = (u8 *) pl_data;
	u8 *vl_dest_end8 = (u8 *) pl_data + (pl_nb_data * pl_data_size);

	if ((pl_array->col == pl_elem->cols)
	    && (pl_array->row == pl_elem->rows)) {
		/* copy all elements from array to data */
		for (row = 0;
		     (row < pl_elem->rows) && (vl_dest8 <= vl_dest_end8);
		     row++) {
			for (col = 0;
			     (col < pl_elem->cols)
			     && (vl_dest8 <= vl_dest_end8);
			     col++, vl_src8 += pl_array_size, vl_dest8 +=
			     pl_data_size) {
				memset(vl_dest8, 0, pl_data_size);
				memcpy(vl_dest8, vl_src8,
				       min(pl_array_size, pl_data_size));
			}
		}
	} else if ((pl_array->col < pl_elem->cols)
		   && (pl_array->row < pl_elem->rows)) {
		/* copy cell (row, col) value into data */
		int32 offset_data =
		    (pl_array->row * pl_elem->cols +
		     pl_array->col) * pl_data_size;
		if (offset_data <= (pl_nb_data * pl_data_size)) {
			memset(vl_dest8, 0, pl_data_size);
			memcpy(vl_dest8 + offset_data, vl_src8,
			       min(pl_array_size, pl_data_size));
		}
	} else {
		/* error: array col or/and row are out of range. */
		SYSLOG(LOG_ERR, "error: cell (c:%d, r:%d) is out of range!\n",
		       pl_array->col, pl_array->row);
	}
}

int SetDthQuot(struct dth_element *elem, void *pp_value, void *pp_var,
	       int vp_quot)
{
	int vl_result = TAT_ERROR_TYPE;
	if (elem->type == DTH_TYPE_FLOAT) {
		DEREF_PTR_SET(pp_var, s16,
			      RF_Q(DEREF_PTR(pp_value, float), vp_quot));

		SYSLOGSTR(LOG_INFO, "set %s to %f (0x%04x Q%d)\n",
			  elem->path,
			  DEREF_PTR(pp_value, float),
			  DEREF_PTR(pp_var, s16), vp_quot);

		vl_result = 0;
	}

	return vl_result;
}

int GetDthQuot(struct dth_element *elem, void *pp_value, void *pp_var,
	       int vp_quot)
{
	int vl_result = TAT_ERROR_TYPE;
	if (elem->type == DTH_TYPE_FLOAT) {
		DEREF_PTR_SET(pp_value, float,
			      RF_UNQ(DEREF_PTR(pp_var, s16), vp_quot));

		SYSLOGSTR(LOG_INFO, "%s: %f (0x%04x Q%d)\n",
			  elem->path,
			  DEREF_PTR(pp_value, float),
			  DEREF_PTR(pp_var, s16), vp_quot);

		vl_result = 0;
	}

	return vl_result;
}

int tatl3_getRfInfo(void);

int tatl3_00GetRfInfo_exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_CASE;

	switch (elem->user_data) {
	case ACT_GETINFO:	/* use stored arguments values to execute action */

		vl_Error = tatl3_getRfInfo();
		break;

	default:

		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl3_04GetRfInfo_ParamSet(struct dth_element *elem, void *Value)
{
	int vl_Error;
	u16 *vp_Value;

	vl_Error = TAT_ERROR_OFF;
	vp_Value = (u16 *) Value;

	switch (elem->user_data) {
	case ACT_SET_INFO_RSSI:

		switch (*vp_Value) {
		case ACT_RSSI_PATH1:

			v_RxRssiInfoPath = C_HAL_RX_PATH1_RSSI_INFO;
			break;

		case ACT_RSSI_PATH2:

			v_RxRssiInfoPath = C_HAL_RX_PATH2_RSSI_INFO;
			break;

		default:
			vl_Error = TAT_BAD_REQ;
		}
		break;

	default:
		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
		break;
	}

	return vl_Error;
}

int tatl3_02GetRfInfoParam_get(struct dth_element *elem, void *Value)
{
	int vl_Error;
	s16 *vp_s16_Value;
	float *vp_float_Value;
	u16 *vp_u16_Value;

	vl_Error = TAT_ERROR_OFF;
	vp_s16_Value = (s16 *) Value;
	vp_u16_Value = (u16 *) Value;
	vp_float_Value = (float *)Value;

	switch (elem->user_data) {
	case ACT_GET_INFO_RSSI:
		{
			int16 *pl_rx_level =
			    (int16 *) (&v_tatmisc_TestInfo_conf.data);
			*vp_float_Value = RF_UNQ(*pl_rx_level, 8);
			SYSLOG(LOG_DEBUG, "Get RSSIvalue: %f\n",
			       *vp_float_Value);
		}
		break;

	case ACT_GET_PATH_RSSI:

		*vp_u16_Value = v_tatmisc_TestInfo_conf.info_type;
		break;

	default:
		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
		break;
	}

	return vl_Error;
}

int tatl3_03StopRfActivity(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_HAL_RF_TEST_CONTROL_STOP_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
				      C_HAL_RF_TEST_CONTROL_STOP_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/*Send ISI message - Blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Stop RX/TX");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_HAL_RF_TEST_CONTROL_STOP_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_CONTROL_STOP_RESP,
				  "Stop RX/TX response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
	}

	return vl_Error;
}

u32 tatl3_04GetGsmRfBand(int band_index)
{
	u32 vl_rf_band = INFO_NO_GSM;

	int i;
	for (i = 0; i < (int) COUNT_RF_TUNING_BANDS; i++) {
		if (v_tatrf_bands[i].band_index == band_index) {
			/* band item found */
			vl_rf_band = v_tatrf_bands[i].gsm_band;
			break;
		}
	}

	return vl_rf_band;
}

int tatl3_05GetGsmRfBandIndex(u32 rf_band)
{
	int vl_band_index = RF_NO_BAND;

	int i;
	for (i = 0; i < (int) COUNT_RF_TUNING_BANDS; i++) {
		if (v_tatrf_bands[i].gsm_band == rf_band) {
			/* band item found */
			vl_band_index = v_tatrf_bands[i].band_index;
			break;
		}
	}

	return vl_band_index;
}

u32 tatl3_06GetWcdmaRfBand(int band_index)
{
	u32 vl_rf_band = INFO_NO_WCDMA;

	int i;
	for (i = 0; i < (int) COUNT_RF_TUNING_BANDS; i++) {
		if (v_tatrf_bands[i].band_index == band_index) {
			/* band item found */
			vl_rf_band = v_tatrf_bands[i].wcdma_band;
			break;
		}
	}

	return vl_rf_band;
}

int tatl3_07GetWcdmaRfBandIndex(u32 rf_band)
{
	int vl_band_index = RF_NO_BAND;

	int i;
	for (i = 0; i < (int) COUNT_RF_TUNING_BANDS; i++) {
		if (v_tatrf_bands[i].wcdma_band == rf_band) {
			/* band item found */
			vl_band_index = v_tatrf_bands[i].band_index;
			break;
		}
	}

	return vl_band_index;
}

int tatl3_08ForceStopRfActivity(void)
{
	int vl_Error = TAT_ERROR_OFF;
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	/*Fill Isi header information */
	C_HAL_RF_TEST_CONTROL_STOP_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
				      C_HAL_RF_TEST_CONTROL_STOP_REQ);
	if (NULL != pl_req) {
		vl_Error = tatrf_isi_send(&vl_msg_info, "Stop RF (force)");
	} else {
		vl_Error = TAT_ISI_HANDLER_ERROR;
	}

	return (vl_Error);
}

u16 tatl3_09GetGsmMonChannel(u32 rf_band, u16 channel)
{
	u16 mon_channel = 0;
	u16 max_channel = 0;

	switch (rf_band) {
	case INFO_GSM850:
		/* range: 128-251 */
		mon_channel = 189;
		max_channel = 251;
		break;

	case INFO_GSM900:
		/* range: 0-124 or 975-1023 */
		mon_channel = 62;
		max_channel = 124;
		break;

	case INFO_GSM1800:
		/* range: 512-885 */
		mon_channel = 698;
		max_channel = 885;
		break;

	case INFO_GSM1900:
		/* range: 512-810 */
		mon_channel = 600;
		max_channel = 810;
		break;

	case INFO_NO_GSM:
	default:
		mon_channel = 0;
	}

	/* to be the most effective, there should be at least 10 channels between the two channels */
	if ((channel > (mon_channel - 10)) && (channel < (mon_channel + 10))) {
		/* monitoring channel and RF channels are to close to each other */
		/* but adjusted monitoring channel still must be in the RF band channel range */
		if (channel < (max_channel - 10)) {
			mon_channel = channel + 10;
		} else {
			mon_channel = channel - 10;
		}
	}

	return mon_channel;
}

int tatl3_10GetGsmBandNumber(u32 vp_rfBand)
{
	int vl_bandNbr;
	switch (vp_rfBand) {
	case INFO_NO_GSM:
		vl_bandNbr = 0;
		break;

	case INFO_GSM850:
		vl_bandNbr = 850;
		break;

	case INFO_GSM900:
		vl_bandNbr = 900;
		break;

	case INFO_GSM1800:
		vl_bandNbr = 1800;
		break;

	case INFO_GSM1900:
		vl_bandNbr = 1900;
		break;

	default:
		vl_bandNbr = -1;
	}

	return vl_bandNbr;
}

int tatl3_11GetWcdmaBandNumber(u32 vp_rfBand)
{
	int vl_bandNbr;
	switch (vp_rfBand) {
	case INFO_NO_WCDMA:
		vl_bandNbr = 0;
		break;

	case INFO_WCDMA_FDD_BAND1:
		vl_bandNbr = 1;
		break;

	case INFO_WCDMA_FDD_BAND2:
		vl_bandNbr = 2;
		break;

	case INFO_WCDMA_FDD_BAND3:
		vl_bandNbr = 3;
		break;

	case INFO_WCDMA_FDD_BAND4:
		vl_bandNbr = 4;
		break;

	case INFO_WCDMA_FDD_BAND5:
		vl_bandNbr = 5;
		break;

	case INFO_WCDMA_FDD_BAND6:
		vl_bandNbr = 6;
		break;

	case INFO_WCDMA_FDD_BAND7:
		vl_bandNbr = 7;
		break;

	case INFO_WCDMA_FDD_BAND8:
		vl_bandNbr = 8;
		break;

	case INFO_WCDMA_FDD_BAND9:
		vl_bandNbr = 9;
		break;
/*
    case INFO_WCDMA_FDD_BAND10:
        vl_bandNbr = 10; break;

    case INFO_WCDMA_FDD_BAND11:
        vl_bandNbr = 11; break;

    case INFO_WCDMA_FDD_BAND12:
        vl_bandNbr = 12; break;

    case INFO_WCDMA_FDD_BAND13:
        vl_bandNbr = 13; break;

    case INFO_WCDMA_FDD_BAND14:
        vl_bandNbr = 14; break;

    case INFO_WCDMA_FDD_BAND15:
        vl_bandNbr = 15; break;

    case INFO_WCDMA_FDD_BAND16:
        vl_bandNbr = 16; break;

    case INFO_WCDMA_FDD_BAND17:
        vl_bandNbr = 17; break;

    case INFO_WCDMA_FDD_BAND18:
        vl_bandNbr = 18; break;

    case INFO_WCDMA_FDD_BAND19:
        vl_bandNbr = 19; break;

    case INFO_WCDMA_FDD_BAND20:
        vl_bandNbr = 20; break;

    case INFO_WCDMA_FDD_BAND21:
        vl_bandNbr = 21; break;

    case INFO_WCDMA_FDD_BAND22:
        vl_bandNbr = 22; break;

    case INFO_WCDMA_FDD_BAND23:
        vl_bandNbr = 23; break;

    case INFO_WCDMA_FDD_BAND24:
        vl_bandNbr = 24; break;

    case INFO_WCDMA_FDD_BAND25:
        vl_bandNbr = 25; break;

    case INFO_WCDMA_FDD_BAND26:
        vl_bandNbr = 26; break;

    case INFO_WCDMA_FDD_BAND27:
        vl_bandNbr = 27; break;

    case INFO_WCDMA_FDD_BAND28:
        vl_bandNbr = 28; break;

    case INFO_WCDMA_FDD_BAND29:
        vl_bandNbr = 29; break;

    case INFO_WCDMA_FDD_BAND30:
        vl_bandNbr = 30; break;

    case INFO_WCDMA_FDD_BAND31:
        vl_bandNbr = 31; break;

    case INFO_WCDMA_FDD_BAND32:
        vl_bandNbr = 32; break;
*/
	default:
		vl_bandNbr = -1;
	}

	return vl_bandNbr;
}

int tatl3_getRfInfo(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	/* Set ISI message header */
	C_HAL_RF_TEST_INFO_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
				      C_HAL_RF_TEST_INFO_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	pl_req->info_type = v_RxRssiInfoPath;

	/* Send ISI message - Blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Get RF info");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_HAL_RF_TEST_INFO_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_INFO_RESP,
				  "Get RF Info response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		/* copy returned information */
		v_tatmisc_TestInfo_conf = *pl_resp;
	}

	return vl_Error;
}

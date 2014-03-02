/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rftuning.c
* \brief   routines to tune RF: AFC DCXO, GSM RSSI, WCDMA RSSI and internal
*          power meter.
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "rftuning.h"

#include "misc.h"
#include "isimsg.h"

/* TYPES */

enum {
	RF_MODE_GSM,
	RF_MODE_WCDMA,
	RF_NO_MODE
} RF_MODE;

enum {
	ON,
	OFF,
	NO_ACTIVATION
} RF_ACTIVATION;

/* CONSTS */

/* C_STATE_DCXO table */
#define C_RF_STATE_DCXO_INACTIVE              0
#define C_RF_STATE_DCXO_STARTED               1
#define C_RF_STATE_DCXO_AFC_TUNED             2

/* C_STATE_GSM_RSSI */
#define C_RF_STATE_GSM_RSSI_INACTIVE          0
#define C_RF_STATE_GSM_RSSI_STARTED           1

/*
* In WCDMA, there are 6 channel/level couples per band and up-to 5 bands
* available (I, II, IV, V and VIII)
* In GSM there are up to 2 channel/level couples (but only one channel) per band
* and 4 bands available (850MHz, 900MHz, 1800MHz and 1900MHz)
*/
#define TAT_RF_NB_MEASURES                       6
#define TAT_RF_NB_PWR_TX_CHANNELS                6
#define TAT_RF_NB_PWR_TX_LEVELS                  6
#define TAT_RF_NB_PWR_CAL_CHANNELS               6
#define TAT_RF_NB_CAL_MEASURES                   6
#define TAT_RF_NB_PWR_CAL_RESP                   6

/* Quotients */
#define QUOT_TUNING_AFC_FREQ_ERROR              11
#define QUOT_TUNING_AFC_B0                      -1
#define QUOT_TUNING_AFC_B1                      14
#define QUOT_TUNING_AFC_B2                      23
#define QUOT_TUNING_GSM_RSSI_LEVEL               8
#define QUOT_TUNING_WCDMA_RSSI_COMP_LOW          6
#define QUOT_TUNING_WCDMA_RSSI_COMP_HIGH         6
#define QUOT_TUNING_WCDMA_RSSI_GAIN              6
#define QUOT_TUNING_PWR_CAL_EXT_MEAS             6
#define QUOT_TUNING_PWR_CAL_SLOPE                3
#define QUOT_TUNING_PWR_CAL_COEFF_B              0
#define QUOT_TUNING_PWR_CAL_COEFF_C              6
#define QUOT_TUNING_PWR_CAL_RESP_INT             6
#define QUOT_TUNING_PWR_CAL_RESP_EXT             6
#define QUOT_TUNING_GSM_AFC_ADJUST_ACCURACY     11
#define QUOT_TUNING_GSM_AFC_ADJUST_FREQ_ERROR   0
#define QUOT_TUNING_GSM_AFC_ADJUST_VALUE	10

#define DEFAULT_TUNING_GSM_RSSI_LEVEL        (-60.0)

/* GLOBALS */

/* DCXO tuning automatom state. See table C_STATE_DCXO */
int v_tatrf_dcxo_state;
/* DCXO RX CONTROL request block data */
C_TEST_SB_GSM_RX_CONTROL_STR v_tatrf_gsm_rx_ctrl;
/* DCXO RX CONTROL response message */
C_TEST_SB_GSM_RX_CONTROL_RESP_STR v_tatrf_dcxo_rx_resp;
/* DCXO RX AFC adjustment */
C_TEST_SB_AFC_MEAS_AND_ADJUST_STR v_tatrf_gsm_afc_adjust;
/* DCXO RX AFC adjustment results */
C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS_STR v_tatrf_gsm_afc_adjust_result;
/* AFC tuning request block data */
C_TEST_SB_LINKO_AFC_TUNING_STR v_tatrf_afc_tuning;
/* AFC tuning response message */
C_TEST_SB_LINKO_AFC_TUNING_RESULTS_STR v_tatrf_afc_resp;

/* GSM RSSI requested action */
int v_tatrf_gsm_rssi_action;
/* GSM RSSI tuning automatom state. See table C_STATE_GSM_RSSI */
int v_tatrf_gsm_rssi_state;
/* GSM RSSI tuning request block data. */
C_HAL_SB_GSM_RX_CONTROL_STR v_tatrf_gsm_rssi;
/* GSM RSSI tuning start status. */
uint16 v_tatrf_gsm_rssi_status;
/* GSM RSSI tuning measure value. */
float v_tatrf_gsm_rssi_meas;

/* WCDMA RSSI tuning inputs */
W_TEST_AGC_LNA_ALIGN_REQ_STR v_tatrf_wcdma_rssi;
/* WCDMA RSSI tuning response */
W_TEST_AGC_LNA_ALIGN_RESP_STR v_tatrf_wcdma_rssi_resp;

/* Power detector TX tuning */
int v_tatrf_pwr_tx_band;
int v_tatrf_pwr_tx_mode;
u8 v_tatrf_pwr_tx_amount_of_data;
u16 v_tatrf_pwr_tx_channel[TAT_RF_NB_PWR_TX_CHANNELS];
s16 v_tatrf_pwr_tx_level[TAT_RF_NB_PWR_TX_LEVELS];
s16 v_tatrf_pwr_tx_results[TAT_RF_NB_MEASURES];	/* Q0 value stored */
u8 v_tatrf_pwr_tx_slots;
u8 v_tatrf_pwr_tx_err_code;

/* Power detector calibration tuning */
int v_tatrf_pwr_cal_band;
int v_tatrf_pwr_cal_mode;
u8 v_tatrf_pwr_cal_amount_of_data;
u16 v_tatrf_pwr_cal_channel[TAT_RF_NB_PWR_CAL_CHANNELS];
s16 v_tatrf_pwr_cal_int_level[TAT_RF_NB_CAL_MEASURES];
float v_tatrf_pwr_cal_ext_level[TAT_RF_NB_CAL_MEASURES];
float v_tatrf_pwr_cal_slope;
float v_tatrf_pwr_cal_b;
float v_tatrf_pwr_cal_c;
u16 v_tatrf_pwr_cal_resp_channel[TAT_RF_NB_PWR_CAL_RESP];
float v_tatrf_pwr_cal_resp_tx[TAT_RF_NB_PWR_CAL_RESP];
float v_tatrf_pwr_cal_resp_detector[TAT_RF_NB_PWR_CAL_RESP];
u8 v_tatrf_pwr_cal_err_code;


/**
 * Build and send an ISI message to open or close DCXO RX.
 * The operation done depends of an input set before exec is called.
 * @param[in] elem not used.
 * @retval 0 success.
 * @retval an errno code if an error occured while processing.
 */
static int tat12_04Dcxo_ActivateRx(u16 mode);

/**
 * Build and send an ISI message to start AFC tuning.
 * @retval 0 success.
 * @retval an errno code if an error occured while processing.
 */
static int tatl12_05Afc_exec(void);

/**
 * Start GSM RX or stop active GSM RX.
 * @retval 0 success.
 * @retval an errno code if an error occured while processing.
 */
static int tatl12_06GsmRssi_exec(struct dth_element *elem);

/**
 * Start WCDMA RX or stop active WCDMA RX.
 * Initialize, sends a W_TEST_AGC_LNA_ALIGN_REQ message and process the response.
 * @retval 0 success
 * @retval an errno code if an error occured while processing.
 */
static int tatl12_07WcdmaRssi_exec(void);

/**
 * Start power detector TX for both GSM or WCDMA.
 * The mode depends of the control ID given by \a elem->user_data.
 * The function creates, initializes and sends a C_TEST_TUNING_REQ message
 * with C_TEST_SB_PWR_SEQ_MEASURE subblock and then processes the response.
 * @retval 0 success.
 * @retval an errno code if an error occured while processing.
 */
static int tatl12_08PowerTx_exec(void);

/**
 * Start power detector CAL for both GSM or WCDMA.
 * The mode depends of the control ID given by \a elem->user_data.
 * The function creates, initializes and sends a C_TEST_TUNING_REQ message
 * with C_TEST_SB_FREQ_RESP_CALIBRATE and C_TEST_FREQ_RESP_DATA subblocks and
 * then processes the response.
 * @retval 0 success.
 * @retval an errno code if an error occured while processing.
 */
static int tatl12_09PowerCal_exec(void);

/**
 * Build and send an ISI message to set AFC fine adjustment.
 * @retval 0 success.
 * @retval an errno code if an error occured while processing.
 */
static int tatl12_13AfcAdjust(void);

/* Function tide to RF Tuning                     */
/**************************************************/
int DthRf_Tuning_Get(struct dth_element *elem, void *value)
{
	return tatl12_01TuningGet(elem, value);
}

int DthRf_Tuning_Set(struct dth_element *elem, void *value)
{
	return tatl12_02TuningSet(elem, value);
}

int DthRf_Tuning_Exec(struct dth_element *elem)
{
	return tatl12_03TuningExec(elem);
}

void tatl12_00Init(void)
{
	static int vl_tatrf_init_done = 0xEF;
	if (0xEF == vl_tatrf_init_done) {
		vl_tatrf_init_done = 1;

		/* Init DCXO tuning data */
		v_tatrf_dcxo_state = C_RF_STATE_DCXO_INACTIVE;
		memset(&v_tatrf_gsm_rx_ctrl, 0, sizeof(v_tatrf_gsm_rx_ctrl));
		memset(&v_tatrf_afc_tuning, 0, sizeof(v_tatrf_afc_tuning));
		memset(&v_tatrf_dcxo_rx_resp, 0, sizeof(v_tatrf_dcxo_rx_resp));
		memset(&v_tatrf_afc_resp, 0, sizeof(v_tatrf_afc_resp));
		memset(&v_tatrf_gsm_afc_adjust, 0,
		       sizeof(v_tatrf_gsm_afc_adjust));
		memset(&v_tatrf_gsm_afc_adjust_result, 0,
		       sizeof(v_tatrf_gsm_afc_adjust_result));
		v_tatrf_gsm_afc_adjust_result.status = 0xFF;

		/* Init GSM RSSI tuning data */
		v_tatrf_gsm_rssi_action = NO_ACTIVATION;
		v_tatrf_gsm_rssi_state = C_RF_STATE_GSM_RSSI_INACTIVE;
		memset(&v_tatrf_gsm_rssi, 0, sizeof(v_tatrf_gsm_rssi));
		v_tatrf_gsm_rssi.rx_level =
		    RF_Q(DEFAULT_TUNING_GSM_RSSI_LEVEL,
			 QUOT_TUNING_GSM_RSSI_LEVEL);
		v_tatrf_gsm_rssi_status = 0xFF;
		v_tatrf_gsm_rssi_meas = 0;

		/* Init WCDMA RSSI tuning data */
		memset(&v_tatrf_wcdma_rssi, 0, sizeof(v_tatrf_wcdma_rssi));
		v_tatrf_wcdma_rssi.filler1_agc_step_length = MAKE16(0x00, 1);
		memset(&v_tatrf_wcdma_rssi_resp, 0,
		       sizeof(v_tatrf_wcdma_rssi_resp));

		/* Init Power detector TX tuning data */
		v_tatrf_pwr_tx_band = RF_NO_BAND;
		v_tatrf_pwr_tx_mode = RF_NO_MODE;
		memset(v_tatrf_pwr_tx_channel, 0,
		       sizeof(v_tatrf_pwr_tx_channel));
		memset(v_tatrf_pwr_tx_level, 0, sizeof(v_tatrf_pwr_tx_level));
		memset(v_tatrf_pwr_tx_results, 0,
		       sizeof(v_tatrf_pwr_tx_results));
		v_tatrf_pwr_tx_slots = 0;
		v_tatrf_pwr_tx_err_code = 0;

		/* Init Power detector calibration tuning data */
		v_tatrf_pwr_cal_band = RF_NO_BAND;
		v_tatrf_pwr_cal_mode = RF_NO_MODE;
		v_tatrf_pwr_cal_amount_of_data = 0;
		memset(v_tatrf_pwr_cal_channel, 0,
		       sizeof(v_tatrf_pwr_cal_channel));
		memset(v_tatrf_pwr_cal_int_level, 0,
		       sizeof(v_tatrf_pwr_cal_int_level));
		memset(v_tatrf_pwr_cal_ext_level, 0,
		       sizeof(v_tatrf_pwr_cal_ext_level));
		v_tatrf_pwr_cal_slope = 0.;
		v_tatrf_pwr_cal_b = 0.;
		v_tatrf_pwr_cal_c = 0.;
		memset(v_tatrf_pwr_cal_resp_channel, 0,
		       sizeof(v_tatrf_pwr_cal_resp_channel));
		memset(v_tatrf_pwr_cal_resp_tx, 0,
		       sizeof(v_tatrf_pwr_cal_resp_tx));
		memset(v_tatrf_pwr_cal_resp_detector, 0,
		       sizeof(v_tatrf_pwr_cal_resp_detector));
		v_tatrf_pwr_cal_err_code = 0;
	}
}

int tatl12_01TuningGet(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;	/* errno return code */
	struct dth_array *vl_array = NULL;
	int i;

	/* Initialize module's data the first time */
	tatl12_00Init();

	/* Switch on elem user data and fill Value with the corresponding current value */
	switch (elem->user_data) {
		/*
		 * DCXO tuning outputs
		 */

	case ACT_TUNING_DCXO_ERROR_CODE:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_DCXO_ERROR_CODE");
		DEREF_PTR_SET(Value, u8, v_tatrf_dcxo_rx_resp.status);
		SYSLOG(LOG_DEBUG, "get: DCXO error code = %u",
		       v_tatrf_dcxo_rx_resp.status);
		break;

	case ACT_TUNING_DCXO_STATUS:	/* TYPE U32 */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_DCXO_STATUS");
		DEREF_PTR_SET(Value, u32, v_tatrf_dcxo_state);
		SYSLOG(LOG_DEBUG, "get: DCXO status = %u", v_tatrf_dcxo_state);
		break;

	case ACT_TUNING_AFC_ADJUST_MODE:

		DTH_GET_UNSIGNED(v_tatrf_gsm_afc_adjust.measurementMode, elem,
				 Value, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_AFC:

		DTH_GET_UNSIGNED(v_tatrf_gsm_afc_adjust.afcValue, elem,
				Value, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_COARSE:

		DTH_GET_UNSIGNED(v_tatrf_gsm_afc_adjust.coarseValue, elem,
				 Value, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_TIMEOUT:

		DTH_GET_UNSIGNED(v_tatrf_gsm_afc_adjust.adjustTimeout, elem,
				 Value, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_ACCURACY:

		DTH_GET_QUOT(elem, Value, v_tatrf_gsm_afc_adjust.adjustAccuracy,
			     QUOT_TUNING_GSM_AFC_ADJUST_ACCURACY, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_STATUS:

		DTH_GET_UNSIGNED(v_tatrf_gsm_afc_adjust_result.status, elem,
				 Value, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_FREQ_ERROR:

		DTH_GET_QUOT(elem, Value,
			     v_tatrf_gsm_afc_adjust_result.freqErrorRad,
			     QUOT_TUNING_GSM_AFC_ADJUST_FREQ_ERROR, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_FINAL_AFC:

		DTH_GET_FLOAT(RF_UNQ(DEREF_PTR(&v_tatrf_gsm_afc_adjust_result.usedAfc,
				int16),
				QUOT_TUNING_GSM_AFC_ADJUST_VALUE),
				elem,
				Value,
				vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_COARSE_USED:

		DTH_GET_UNSIGNED(v_tatrf_gsm_afc_adjust_result.usedCoarse, elem,
				 Value, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_TIMEOUT_USED:

		DTH_GET_UNSIGNED(v_tatrf_gsm_afc_adjust_result.remainingTimeout,
				 elem, Value, vl_Error);
		break;

		/*
		 * AFC tuning outputs
		 */

	case ACT_TUNING_AFC_ERROR_CODE:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_AFC_ERROR_CODE");
		DEREF_PTR_SET(Value, u8, v_tatrf_afc_resp.status);
		SYSLOG(LOG_DEBUG, "get: AFC error code = %u",
		       v_tatrf_afc_resp.status);
		break;

	case ACT_TUNING_AFC_C_COARSE:	/* TYPE U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_AFC_C_COARSE");
		DEREF_PTR_SET(Value, u16, v_tatrf_afc_resp.c_coarse);
		SYSLOG(LOG_DEBUG, "get: AFC c coarse = %u",
		       v_tatrf_afc_resp.c_coarse);
		break;

	case ACT_TUNING_AFC_I_CORE:	/* TYPE U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_AFC_I_CORE");
		DEREF_PTR_SET(Value, u16, v_tatrf_afc_resp.i_bias_core);
		SYSLOG(LOG_DEBUG, "get: AFC i core = %u",
		       v_tatrf_afc_resp.i_bias_core);
		break;

	case ACT_TUNING_AFC_CAL_TEMP:	/* TYPE U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_AFC_CAL_TEMP");
		DEREF_PTR_SET(Value, u16, v_tatrf_afc_resp.temperature);
		SYSLOG(LOG_DEBUG, "get: AFC cal. temperature = %u",
		       v_tatrf_afc_resp.temperature);
		break;

	case ACT_TUNING_AFC_AFC0:	/* TYPE S16 */

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_AFC_AFC0");
		DEREF_PTR_SET(Value, int16, v_tatrf_afc_resp.afc0_value);
		SYSLOG(LOG_DEBUG, "get: AFC 0 = %u",
		       v_tatrf_afc_resp.afc0_value);
		break;

	case ACT_TUNING_AFC_VALUE:	/* TYPE U16[C:5] */
		{
			ASSERT(elem->type == DTH_TYPE_U16,
			       "unexpected type for dth element ACT_TUNING_AFC_VALUE");
			vl_array = (struct dth_array *)Value;
			SYSLOG(LOG_DEBUG,
			       "get: AFC values (cols = %d, rows = %d)...",
			       vl_array->col, vl_array->row);
			GetDthArray(elem, vl_array, sizeof(u16),
				    &v_tatrf_afc_resp.afc1_value,
				    sizeof(uint16), 5);
			if (vl_array->col == elem->cols
			    && vl_array->row == elem->rows) {
				for (i = 0; i < 5; i++) {
					SYSLOGSTR(LOG_DEBUG,
						  "acf%d_value = %u\n", i,
						  DEREF_PTR((u16 *)
							    vl_array->array + i,
							    u16));
				}
			} else {
				i = vl_array->col;
				SYSLOGSTR(LOG_DEBUG, "acf%d_value = %u\n", i,
					  DEREF_PTR((u16 *) vl_array->array,
						    u16));
			}
		}
		break;

	case ACT_TUNING_AFC_FREQ_ERROR:	/* TYPE S16[R:5] */
		{
			ASSERT(elem->type == DTH_TYPE_S16,
			       "unexpected type for dth element ACT_TUNING_AFC_FREQ_ERROR");
			vl_array = (struct dth_array *)Value;

			GetDthArray(elem, vl_array, sizeof(s16),
				    &v_tatrf_afc_resp.afc1_reading,
				    sizeof(int16), 5);
			if (vl_array->col == elem->cols
			    && vl_array->row == elem->rows) {
				for (i = 0; i < 5; i++) {
					SYSLOGSTR(LOG_DEBUG,
						  "get: AFC freq. error %d = %d\n",
						  i, DEREF_PTR((s16 *)
							       vl_array->array +
							       i, s16));
				}
			} else if ((vl_array->row < elem->rows)
				   && (vl_array->col < elem->cols)) {
				i = vl_array->col;
				SYSLOGSTR(LOG_DEBUG,
					  "get: AFC freq. error %d = %d\n", i,
					  DEREF_PTR((s16 *) vl_array->array + i,
						    s16));
			} else {
				/* error: array col or/and row are out of range. */
				SYSLOG(LOG_ERR,
				       "cell (c:%d, r:%d) is out of range!",
				       vl_array->col, vl_array->row);
				vl_Error = TAT_BAD_REQ;
			}
		}
		break;

	case ACT_TUNING_AFC_B0:	/* PMM, Q-1, TYPE S16 */

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_AFC_B0");
		/* v_tatrf_afc_resp..c_temp_b? */
		SYSLOG(LOG_DEBUG, "get: AFC b0 = ?");
		break;

	case ACT_TUNING_AFC_B1:	/* PMM, Q14, TYPE S16 */

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_AFC_B1");
		/* v_tatrf_afc_resp..c_temp_b? */
		SYSLOG(LOG_DEBUG, "get: AFC b1 = ?");
		break;

	case ACT_TUNING_AFC_B2:	/* PMM, Q23, TYPE S16 */

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_AFC_B2");
		/* v_tatrf_afc_resp..c_temp_b? */
		SYSLOG(LOG_DEBUG, "get: AFC b2 = ?");
		break;

		/*
		 * GSM RSSI tuning outputs
		 */

	case ACT_TUNING_GSM_RSSI_ERROR_CODE:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_GSM_RSSI_ERROR_CODE");
		DEREF_PTR_SET(Value, u8, v_tatrf_gsm_rssi_status);
		SYSLOG(LOG_DEBUG, "get: GSM RSSI error code = %u",
		       v_tatrf_gsm_rssi_status);
		break;

	case ACT_TUNING_GSM_RSSI_STATUS:	/* TYPE U32 */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_GSM_RSSI_STATUS");
		DEREF_PTR_SET(Value, u32, v_tatrf_gsm_rssi_state);
		SYSLOG(LOG_DEBUG, "get: GSM RSSI status = %u",
		       v_tatrf_gsm_rssi_state);
		break;

	case ACT_TUNING_GSM_RSSI_MEAS_VALUE:	/* TYPE FLOAT */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_GSM_RSSI_MEAS_VALUE");
		DEREF_PTR_SET(Value, float, v_tatrf_gsm_rssi_meas);
		SYSLOG(LOG_DEBUG, "get: GSM RSSI measure = %lf",
		       v_tatrf_gsm_rssi_meas);
		break;

		/*
		 * Diversity WCDMA/WCDMA RSSI tuning outputs
		 */

	case ACT_TUNING_WCDMA_RSSI_ERROR_CODE:	/* DTH_TYPE_U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_ERROR_CODE");
		DEREF_PTR_SET(Value, u8,
			      LOWBYTE
			      (v_tatrf_wcdma_rssi_resp.filler1_align_status));
		SYSLOG(LOG_DEBUG, "get: WCDMA RSSI error code = %u",
		       DEREF_PTR(Value, u8));
		break;

       case ACT_TUNING_WCDMA_RSSI_SAMPLE_NBR:   /* DTH_TYPE_U8 */
               ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_SAMPLE_NBR");
               DEREF_PTR_SET(Value, u8,
			      LOWBYTE
			      (v_tatrf_wcdma_rssi.filler1_agc_step_length));
	       SYSLOG(LOG_DEBUG, "get: WCDMA RSSI Sample Number = %u",
		       DEREF_PTR(Value, u8));
	       break;

	case ACT_TUNING_WCDMA_RSSI_COMP_LOW:	/* DTH_TYPE_FLOAT */

		/* Warning: is coded as uint16 in W_TEST_AGC_LNA_ALIGN_RESP_STR but is signed */
		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_COMP_LOW");
		DEREF_PTR_SET(Value, float,
			      RF_UNQ(DEREF_PTR
				     (&v_tatrf_wcdma_rssi_resp.low_freq_comp,
				      int16), QUOT_TUNING_WCDMA_RSSI_COMP_LOW));
		SYSLOG(LOG_DEBUG, "get: WCDMA RSSI RX comp. (low ch) = %f",
		       DEREF_PTR(Value, float));
		break;

	case ACT_TUNING_WCDMA_RSSI_COMP_HIGH:	/* DTH_TYPE_FLOAT */

		/* Warning: is coded as uint16 in W_TEST_AGC_LNA_ALIGN_RESP_STR but is signed */
		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_COMP_HIGH");
		DEREF_PTR_SET(Value, float,
			      RF_UNQ(DEREF_PTR
				     (&v_tatrf_wcdma_rssi_resp.high_freq_comp,
				      int16),
				     QUOT_TUNING_WCDMA_RSSI_COMP_HIGH));
		SYSLOG(LOG_DEBUG, "get: WCDMA RSSI RX comp. (high ch) = %f",
		       DEREF_PTR(Value, float));
		break;

	case ACT_TUNING_WCDMA_RSSI_GAIN:	/* DTH_TYPE_FLOAT */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_GAIN");
		DEREF_PTR_SET(Value, float,
			      RF_UNQ(v_tatrf_wcdma_rssi_resp.rx_chain_gain,
				     QUOT_TUNING_WCDMA_RSSI_GAIN));
		SYSLOG(LOG_DEBUG, "get: WCDMA RSSI RX chain gain = %f",
		       DEREF_PTR(Value, float));
		break;

		/*
		 * Internal power meter tuning outputs
		 */
	case ACT_TUNING_POWER_TX_MODE:	/* TYPE ENUM */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_MODE");
		DEREF_PTR_SET(Value, u32, v_tatrf_pwr_tx_mode);
		SYSLOG(LOG_DEBUG, "get: power TX mode = %u",
		       DEREF_PTR(Value, u32));
		break;

	case ACT_TUNING_POWER_TX_CODE:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_CODE");
		DEREF_PTR_SET(Value, u8, v_tatrf_pwr_tx_err_code);
		SYSLOG(LOG_DEBUG, "get: power TX error code = %u",
		       DEREF_PTR(Value, u8));
		break;

	case ACT_TUNING_POWER_TX_MEAS:	/* TYPE S16[C:6] */

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_MEAS");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG, "get: power TX, internal power meas...");
		GetDthArray(elem, vl_array, sizeof(s16), v_tatrf_pwr_tx_results,
			    sizeof(s16), TAT_RF_NB_MEASURES);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_MEASURES; i++) {
				SYSLOGSTR(LOG_DEBUG, "tx result %d = %d\n", i,
					  v_tatrf_pwr_tx_results[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "tx result %d = %d\n", i,
				  v_tatrf_pwr_tx_results[i]);
		}
		break;

	case ACT_TUNING_POWER_CAL_CODE:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_CODE");
		DEREF_PTR_SET(Value, u8, v_tatrf_pwr_cal_err_code);
		SYSLOG(LOG_DEBUG, "get: power CAL error code = %u",
		       DEREF_PTR(Value, u8));
		break;

	case ACT_TUNING_POWER_CAL_SLOPE:	/* TYPE FLOAT */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_SLOPE");
		DEREF_PTR_SET(Value, float, v_tatrf_pwr_cal_slope);
		SYSLOG(LOG_DEBUG, "get: power CAL slope = %f",
		       DEREF_PTR(Value, float));
		break;

	case ACT_TUNING_POWER_CAL_COEFB:	/* TYPE FLOAT */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_COEFB");
		DEREF_PTR_SET(Value, float, v_tatrf_pwr_cal_b);
		SYSLOG(LOG_DEBUG, "get: power CAL coef b = %f",
		       DEREF_PTR(Value, float));
		break;

	case ACT_TUNING_POWER_CAL_COEFC:	/* TYPE FLOAT */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_COEFC");
		DEREF_PTR_SET(Value, float, v_tatrf_pwr_cal_c);
		SYSLOG(LOG_DEBUG, "get: power CAL coef c = %f",
		       DEREF_PTR(Value, float));
		break;

	case ACT_TUNING_POWER_CAL_BAND_RESP_CHANNELS:	/* TYPE U16[C:5] */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_BAND_RESP_CHANNELS");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG, "get: power CAL, band response channels...");
		GetDthArray(elem, vl_array, sizeof(u16),
			    v_tatrf_pwr_cal_resp_channel, sizeof(u16),
			    TAT_RF_NB_PWR_CAL_RESP);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_PWR_CAL_RESP; i++) {
				SYSLOGSTR(LOG_DEBUG,
					  "response channel %d = %u\n", i,
					  v_tatrf_pwr_cal_resp_channel[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "response channel %d = %u\n", i,
				  v_tatrf_pwr_cal_resp_channel[i]);
		}
		break;

	case ACT_TUNING_POWER_CAL_BAND_RESP_TX:	/* TYPE FLOAT[C:5] */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_BAND_RESP_TX");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG, "get: power CAL, TX band responses...");
		GetDthArray(elem, vl_array, sizeof(float),
			    v_tatrf_pwr_cal_resp_tx, sizeof(float),
			    TAT_RF_NB_PWR_CAL_RESP);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_PWR_CAL_RESP; i++) {
				SYSLOGSTR(LOG_DEBUG, "response tx %d = %lf\n",
					  i, v_tatrf_pwr_cal_resp_tx[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "response tx %d = %lf\n", i,
				  v_tatrf_pwr_cal_resp_tx[i]);
		}
		break;

	case ACT_TUNING_POWER_CAL_BAND_RESP_DETECTOR:	/* TYPE FLOAT[C:5] */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_BAND_RESP_DETECTOR");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG, "get: power CAL, detector band responses...");
		GetDthArray(elem, vl_array, sizeof(float),
			    v_tatrf_pwr_cal_resp_detector, sizeof(float),
			    TAT_RF_NB_PWR_CAL_RESP);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_PWR_CAL_RESP; i++) {
				SYSLOGSTR(LOG_DEBUG,
					  "response detector %d = %lf\n", i,
					  v_tatrf_pwr_cal_resp_detector[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "response detector %d = %lf\n", i,
				  v_tatrf_pwr_cal_resp_detector[i]);
		}
		break;

		/* Raise error if elem user data is not an valid output */
	default:
		SYSLOG(LOG_ERR, "invalid output for get:%d", elem->user_data);
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl12_02TuningSet(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;	/* errno return code */
	struct dth_array *vl_array = NULL;
	int i;

	/* Initialize module's data the first time */
	tatl12_00Init();

	/* Switch on elem user data, cast Value and fill the corresponding internal value */
	switch (elem->user_data) {

		/*
		 * DCXO tuning inputs
		 */

	case ACT_TUNING_DCXO_RFBAND:

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_DCXO_RFBAND");
		v_tatrf_gsm_rx_ctrl.mon_ch = 0;
		v_tatrf_gsm_rx_ctrl.rf_band =
		    tatl3_04GetGsmRfBand(DEREF_PTR(Value, u32));
		SYSLOG(LOG_DEBUG, "set: param DCXO RX RF band=%lu",
		       v_tatrf_gsm_rx_ctrl.rf_band);
		break;

	case ACT_TUNING_DCXO_CHANNEL:

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_DCXO_CHANNEL");
		v_tatrf_gsm_rx_ctrl.channel = DEREF_PTR(Value, u16);
		SYSLOG(LOG_DEBUG, "set: param DCXO RX channel=%u",
		       v_tatrf_gsm_rx_ctrl.channel);
		break;

	case ACT_TUNING_DCXO_DEFAULT_AFC:

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_DCXO_DEFAULT_AFC");
		v_tatrf_gsm_rx_ctrl.afc = DEREF_PTR(Value, int16);
		SYSLOG(LOG_DEBUG, "set: param DCXO RX default AFC=%d",
		       v_tatrf_gsm_rx_ctrl.afc);
		break;

	case ACT_TUNING_DCXO_DEFAULT_AGC:

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_DCXO_DEFAULT_AGC");
		v_tatrf_gsm_rx_ctrl.agc = DEREF_PTR(Value, u16);
		SYSLOG(LOG_DEBUG, "set: param DCXO RX default AGC=%u",
		       v_tatrf_gsm_rx_ctrl.agc);
		break;

	case ACT_TUNING_AFC_ADJUST_MODE:

		DTH_SET_UNSIGNED(elem, Value,
				 v_tatrf_gsm_afc_adjust.measurementMode,
				 vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_AFC:

		DTH_SET_UNSIGNED(elem, Value, v_tatrf_gsm_afc_adjust.afcValue,
				 vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_COARSE:

		DTH_SET_UNSIGNED(elem, Value,
				 v_tatrf_gsm_afc_adjust.coarseValue, vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_TIMEOUT:

		DTH_SET_UNSIGNED(elem, Value,
				 v_tatrf_gsm_afc_adjust.adjustTimeout,
				 vl_Error);
		break;

	case ACT_TUNING_AFC_ADJUST_ACCURACY:

		DTH_SET_QUOT(elem, Value, vl_Error,
			     v_tatrf_gsm_afc_adjust.adjustAccuracy,
			     QUOT_TUNING_GSM_AFC_ADJUST_ACCURACY);
		break;

		/*
		 * AFC tuning inputs
		 */

	case ACT_TUNING_AFC_DEFAULT_AFC:	/* TYPE S16 */

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_AFC_DEFAULT_AFC");
		v_tatrf_afc_tuning.defaultAfc = DEREF_PTR(Value, int16);
		SYSLOG(LOG_DEBUG, "set: param DCXO AFC default AFC=%d",
		       v_tatrf_afc_tuning.defaultAfc);
		break;

	case ACT_TUNING_AFC_DEFAULT_COARSE:	/* TYPE U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_AFC_DEFAULT_COARSE");
		v_tatrf_afc_tuning.defaultCoarse = DEREF_PTR(Value, uint16);
		SYSLOG(LOG_DEBUG, "set: param DCXO AFC default Coarse=%u",
		       v_tatrf_afc_tuning.defaultCoarse);
		break;

		/*
		 * GSM RSSI tuning inputs
		 */

	case ACT_TUNING_GSM_RSSI:

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_GSM_RSSI");
		switch (DEREF_PTR(Value, u32)) {
		case 0:
			v_tatrf_gsm_rssi_action = ON;
			SYSLOG(LOG_DEBUG, "set: action GSM RSSI mode=%u",
			       v_tatrf_gsm_rssi_action);
			break;

		case 1:
			v_tatrf_gsm_rssi_action = OFF;
			SYSLOG(LOG_DEBUG, "set: action GSM RSSI mode=%u",
			       v_tatrf_gsm_rssi_action);
			break;

		default:
			SYSLOG(LOG_ERR, "invalid GSM RSSI action mode");
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TUNING_GSM_RSSI_RFBAND:

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_GSM_RSSI_RFBAND");
		v_tatrf_gsm_rssi.mon_channel = 0;
		v_tatrf_gsm_rssi.gsm_band_info =
		    tatl3_04GetGsmRfBand(DEREF_PTR(Value, u32));
		SYSLOG(LOG_DEBUG, "set: param GSM RSSI RF Band=%lu",
		       v_tatrf_gsm_rssi.gsm_band_info);
		break;

	case ACT_TUNING_GSM_RSSI_CHANNEL:	/* TYPE U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_GSM_RSSI_CHANNEL");
		v_tatrf_gsm_rssi.rx_channel = DEREF_PTR(Value, uint16);
		SYSLOG(LOG_DEBUG, "set: param GSM RSSI RX Channel=%u",
		       v_tatrf_gsm_rssi.rx_channel);
		break;

	case ACT_TUNING_GSM_RSSI_LEVEL:	/* TYPE FLOAT */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_GSM_RSSI_LEVEL");
		v_tatrf_gsm_rssi.rx_level =
		    RF_Q(DEREF_PTR(Value, float), QUOT_TUNING_GSM_RSSI_LEVEL);
		SYSLOG(LOG_DEBUG, "set: param GSM RSSI RX Level=%d",
		       v_tatrf_gsm_rssi.rx_level);
		break;

		/*
		 * Diversity WCDMA/WCDMA RSSI tuning inputs
		 */

	case ACT_TUNING_WCDMA_RSSI_CHANEL_MID:	/* DTH_TYPE_U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_CHANEL_MID");
		v_tatrf_wcdma_rssi.mid_chan = DEREF_PTR(Value, u16);
		SYSLOG(LOG_DEBUG, "set: param WCDMA RSSI mid channel=%u",
		       v_tatrf_wcdma_rssi.mid_chan);
		break;

	case ACT_TUNING_WCDMA_RSSI_CHANEL_LOW:	/* DTH_TYPE_U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_CHANEL_LOW");
		v_tatrf_wcdma_rssi.low_chan = DEREF_PTR(Value, u16);
		SYSLOG(LOG_DEBUG, "set: param WCDMA RSSI low channel=%u",
		       v_tatrf_wcdma_rssi.low_chan);
		break;

	case ACT_TUNING_WCDMA_RSSI_CHANEL_HIGH:	/* DTH_TYPE_U16 */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_CHANEL_HIGH");
		v_tatrf_wcdma_rssi.high_chan = DEREF_PTR(Value, u16);
		SYSLOG(LOG_DEBUG, "set: param WCDMA RSSI high channel=%u",
		       v_tatrf_wcdma_rssi.high_chan);
		break;

	case ACT_TUNING_WCDMA_RSSI_SAMPLE_NBR:	/* DTH_TYPE_U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_SAMPLE_NBR");
		v_tatrf_wcdma_rssi.filler1_agc_step_length =
		    MAKE16(0x00, DEREF_PTR(Value, u8));
		SYSLOG(LOG_DEBUG, "set: param WCDMA RSSI sample nbr=%u",
		       v_tatrf_wcdma_rssi.filler1_agc_step_length);
		break;

	case ACT_TUNING_WCDMA_RSSI_PATH_SEL:

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_WCDMA_RSSI_PATH_SEL");
		switch (DEREF_PTR(Value, u32)) {
		case 0:

			SYSLOG(LOG_DEBUG,
			       "set: param WCDMA RSSI path selection main");
			v_tatrf_wcdma_rssi.rx_chain_gain = 0x0014;
			break;

		case 1:

			SYSLOG(LOG_DEBUG,
			       "set: param WCDMA RSSI path selection diversity");
			v_tatrf_wcdma_rssi.rx_chain_gain = 0x0018;
			break;

		default:
			SYSLOG(LOG_ERR,
			       "invalid WCDMA RSSI path selection (0=main, 1=diversity)");
			vl_Error = TAT_BAD_REQ;
		}
		break;

		/*
		 * Internal power meter tuning inputs
		 */

	case ACT_TUNING_POWER_TX_BAND:	/* TYPE ENUM */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_BAND");
		v_tatrf_pwr_tx_band = DEREF_PTR(Value, u32);
		SYSLOG(LOG_DEBUG, "set: power tuning band set to %d",
		       v_tatrf_pwr_tx_band);
		break;

	case ACT_TUNING_POWER_TX_MODE:	/* TYPE ENUM */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_MODE");
		switch (DEREF_PTR(Value, u32)) {
		case 0:	/* GSM */

			SYSLOG(LOG_DEBUG,
			       "set: power TX tuning mode set to GSM");
			v_tatrf_pwr_tx_mode = RF_MODE_GSM;
			break;

		case 1:	/* WCDMA */

			SYSLOG(LOG_DEBUG,
			       "set: power TX tuning mode set to WCDMA");
			v_tatrf_pwr_tx_mode = RF_MODE_WCDMA;
			break;

		default:
			SYSLOG(LOG_ERR,
			       "invalid power TX mode for set: %u (0=gsm; 1=wcdma)",
			       DEREF_PTR(Value, u32));
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TUNING_POWER_TX_AMOUNT_OF_DATA:

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_AMOUNT_OF_DATA");
		SYSLOG(LOG_DEBUG, "set: setting power TX amount of data...");
		switch (DEREF_PTR(Value, u32)) {
		case 0:	/* 2 */

			v_tatrf_pwr_tx_amount_of_data = 2;
			break;

		case 1:	/* 6 */

			v_tatrf_pwr_tx_amount_of_data = 6;
			break;

		default:
			SYSLOG(LOG_ERR,
			       "invalid power TX amount of data for set: %u (0=gsm; 1=wcdma)",
			       DEREF_PTR(Value, u32));
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TUNING_POWER_TX_CHANNELS:	/* TYPE U16[C:6] */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_CHANNELS");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG, "set: setting power TX channels...");
		SetDthArray(elem, vl_array, sizeof(u16), v_tatrf_pwr_tx_channel,
			    sizeof(u16), TAT_RF_NB_PWR_TX_CHANNELS);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_PWR_TX_CHANNELS; i++) {
				SYSLOGSTR(LOG_DEBUG, "channel %d = %u\n", i,
					  v_tatrf_pwr_tx_channel[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "channel %d = %u\n", i,
				  v_tatrf_pwr_tx_channel[i]);
		}
		break;

	case ACT_TUNING_POWER_TX_LEVELS:	/* TYPE S16[C:6] */

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_LEVELS");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG, "set: setting power TX levels...");
		SetDthArray(elem, vl_array, sizeof(int16), v_tatrf_pwr_tx_level,
			    sizeof(int16), TAT_RF_NB_PWR_TX_LEVELS);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_PWR_TX_LEVELS; i++) {
				SYSLOGSTR(LOG_DEBUG, "power level %d = %d\n", i,
					  v_tatrf_pwr_tx_level[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "power level %d = %d\n", i,
				  v_tatrf_pwr_tx_level[i]);
		}
		break;

	case ACT_TUNING_POWER_TX_SLOT:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_POWER_TX_SLOT");
		v_tatrf_pwr_tx_slots = DEREF_PTR(Value, u8);
		SYSLOG(LOG_DEBUG,
		       "set: power TX tuning empty slot number to %u",
		       v_tatrf_pwr_tx_slots);
		break;

	case ACT_TUNING_POWER_CAL_BAND:	/* TYPE ENUM */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_BAND");
		v_tatrf_pwr_cal_band = DEREF_PTR(Value, u32);
		SYSLOG(LOG_DEBUG,
		       "set: power Calibration tuning band set to %d",
		       v_tatrf_pwr_cal_band);
		break;

	case ACT_TUNING_POWER_CAL_MODE:	/* TYPE ENUM */

		ASSERT(elem->type == DTH_TYPE_U32,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_MODE");
		switch (DEREF_PTR(Value, u32)) {
		case 0:	/* GSM */

			SYSLOG(LOG_DEBUG,
			       "set: power CAL tuning mode set to GSM");
			v_tatrf_pwr_cal_mode = RF_MODE_GSM;
			break;

		case 1:	/* WCDMA */

			SYSLOG(LOG_DEBUG,
			       "set: power CAL tuning mode set to WCDMA");
			v_tatrf_pwr_cal_mode = RF_MODE_WCDMA;
			break;

		default:
			SYSLOG(LOG_ERR,
			       "invalid power CAL mode for set: %u (0=gsm; 1=wcdma)",
			       DEREF_PTR(Value, u32));
			vl_Error = TAT_BAD_REQ;
		}
		break;

	case ACT_TUNING_POWER_CAL_CHANNEL:	/* TYPE U16[C:6] */

		ASSERT(elem->type == DTH_TYPE_U16,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_CHANNEL");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG,
		       "set: setting power Calibration tuning channels...");
		SetDthArray(elem, vl_array, sizeof(u16),
			    v_tatrf_pwr_cal_channel, sizeof(u16),
			    TAT_RF_NB_PWR_CAL_CHANNELS);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_PWR_CAL_CHANNELS; i++) {
				SYSLOGSTR(LOG_DEBUG, "channel %d = %u\n", i,
					  v_tatrf_pwr_cal_channel[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "channel %d = %u\n", i,
				  v_tatrf_pwr_cal_channel[i]);
		}
		break;

	case ACT_TUNING_POWER_CAL_INT_MEAS:	/* TYPE S16[C:6] */

		ASSERT(elem->type == DTH_TYPE_S16,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_INT_MEAS");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG,
		       "set: setting power cal. tuning internal power measures...");
		SetDthArray(elem, vl_array, sizeof(s16),
			    v_tatrf_pwr_cal_int_level, sizeof(s16),
			    TAT_RF_NB_MEASURES);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_MEASURES; i++) {
				SYSLOGSTR(LOG_DEBUG, "int meas. %d = %d\n", i,
					  v_tatrf_pwr_cal_int_level[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "int meas. %d = %d\n", i,
				  v_tatrf_pwr_cal_int_level[i]);
		}
		break;

	case ACT_TUNING_POWER_CAL_EXT_MEAS:	/* TYPE FLOAT[C:6] */

		ASSERT(elem->type == DTH_TYPE_FLOAT,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_EXT_MEAS");
		vl_array = (struct dth_array *)Value;
		SYSLOG(LOG_DEBUG,
		       "set: setting power cal. tuning external power measures...");
		SetDthArray(elem, vl_array, sizeof(float),
			    v_tatrf_pwr_cal_ext_level, sizeof(float),
			    TAT_RF_NB_MEASURES);
		if (vl_array->col == elem->cols && vl_array->row == elem->rows) {
			for (i = 0; i < TAT_RF_NB_MEASURES; i++) {
				SYSLOGSTR(LOG_DEBUG, "ext meas. %d = %lf\n", i,
					  v_tatrf_pwr_cal_ext_level[i]);
			}
		} else {
			i = vl_array->col;
			SYSLOGSTR(LOG_DEBUG, "ext meas. %d = %lf\n", i,
				  v_tatrf_pwr_cal_ext_level[i]);
		}
		break;

	case ACT_TUNING_POWER_CAL_AMOUNT_DATA:	/* TYPE U8 */

		ASSERT(elem->type == DTH_TYPE_U8,
		       "unexpected type for dth element ACT_TUNING_POWER_CAL_AMOUNT_DATA");
		switch (DEREF_PTR(Value, u8)) {
		case 0:	/* 2 */

			v_tatrf_pwr_cal_amount_of_data = 2;
			break;

		case 1:	/* 6 */

			v_tatrf_pwr_cal_amount_of_data = 6;
			break;

		default:
			SYSLOG(LOG_ERR,
			       "invalid power CAL amount of data for set: %u (0->2; 1->6)",
			       DEREF_PTR(Value, u8));
			vl_Error = TAT_BAD_REQ;
		}

		SYSLOG(LOG_DEBUG, "set: power CAL amount of data set to %u",
		       v_tatrf_pwr_cal_amount_of_data);
		break;

		/* Raise error if elem user data is not an valid input */
	default:
		SYSLOG(LOG_ERR, "invalid input for set: %d", elem->user_data);
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl12_03TuningExec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;

	/* Initialize module's data the first time */
	tatl12_00Init();

	switch (elem->user_data) {
	case ACT_TUNING_DCXO_START_RX:

		vl_Error = tat12_04Dcxo_ActivateRx(C_TEST_RX_OFF);
		if (0 == vl_Error)
			vl_Error = tat12_04Dcxo_ActivateRx(C_TEST_RX_ON);
		break;

	case ACT_TUNING_DCXO_STOP_RX:

		vl_Error = tat12_04Dcxo_ActivateRx(C_TEST_RX_OFF);
		break;

	case ACT_TUNING_AFC_ADJUST:

		vl_Error = tatl12_13AfcAdjust();
		break;

	case ACT_TUNING_AFC:

		vl_Error = tatl12_05Afc_exec();
		break;

	case ACT_TUNING_GSM_RSSI:

		vl_Error = tatl12_06GsmRssi_exec(elem);
		break;

	case ACT_TUNING_GSM_RSSI_MEAS:

		vl_Error = tatl12_06GsmRssi_exec(elem);
		break;

	case ACT_TUNING_WCDMA_RSSI:

		vl_Error = tatl12_07WcdmaRssi_exec();
		break;

	case ACT_TUNING_POWER_TX:

		vl_Error = tatl12_08PowerTx_exec();
		break;

	case ACT_TUNING_POWER_CAL:

		vl_Error = tatl12_09PowerCal_exec();
		break;

	default:
		SYSLOG(LOG_ERR, "invalid action code: %d", elem->user_data);
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl12_05Afc_exec(void)
{
	/* AFC tuning is available once RX is started otherwise it was observed that the modem do not respond to the request. */
	if (C_RF_STATE_DCXO_INACTIVE >= v_tatrf_dcxo_state) {
		v_tatrf_afc_resp.status = C_TEST_NOT_AVAILABLE;
		return TAT_BAD_REQ;
	}

	v_tatrf_afc_resp.status = RF_TEST_MODEM_NOT_READY;

	/* construct the correct C_TEST_TUNING_REQ message */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_TEST_TUNING_REQ, numberOfSubBlocks);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	C_TEST_SB_LINKO_AFC_TUNING_STR *pl_sb =
	    tatrf_isi_msg_add_sb(&vl_msg_info, C_TEST_SB_LINKO_AFC_TUNING);
	if (NULL == pl_sb)
		return TAT_ISI_HANDLER_ERROR;

	pl_sb->defaultAfc = v_tatrf_afc_tuning.defaultAfc;
	pl_sb->defaultCoarse = v_tatrf_afc_tuning.defaultCoarse;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "DCXO Afc");
	if (!TAT_OK(vl_Error)) {
		v_tatrf_afc_resp.status = RF_TEST_MODEM_NOT_READY;
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	v_tatrf_afc_resp.status = RF_TEST_MODEM_NOT_READY;

	C_TEST_TUNING_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_TEST_TUNING_RESP, numerOfSubBlocks,
			   "DCXO Afc response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		C_TEST_SB_LINKO_AFC_TUNING_RESULTS_STR *pl_afc_result =
		    tatrf_isi_find_sb(&vl_msg_info,
				      C_TEST_SB_LINKO_AFC_TUNING_RESULTS, NULL);
		if (NULL != pl_afc_result) {
			v_tatrf_afc_resp = *pl_afc_result;

			if (C_TEST_OK == v_tatrf_afc_resp.status){
				/* AFC is active */
				v_tatrf_dcxo_state = C_RF_STATE_DCXO_AFC_TUNED;
			}
			else{
				SYSLOG(LOG_DEBUG,
					"AFC tuning failed with C_TEST_STATUS = 0x%X",
					v_tatrf_afc_resp.status);
				/* AFC is inactive */
				//v_tatrf_afc_resp.status =
				//    RF_TEST_MODEM_NOT_READY;
			}
		} else
			vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
	}

	return vl_Error;
}

int tatl12_p10StartGsmRx(void)
{
	/* default outputs */
	v_tatrf_gsm_rssi_status =
	    SetU8InU16(LSB_POSS, v_tatrf_gsm_rssi_status,
		       RF_TEST_MODEM_NOT_READY);

	/* construct the correct  message */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_HAL_RF_TEST_CONTROL_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_HAL_RF_TEST_CONTROL_REQ, num_sb_id);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	C_HAL_SB_GSM_RX_CONTROL_STR *pl_sb =
	    tatrf_isi_msg_add_sb(&vl_msg_info, C_HAL_SB_GSM_RX_CONTROL);
	if (NULL == pl_sb)
		return TAT_ISI_HANDLER_ERROR;

	pl_sb->gsm_band_info = v_tatrf_gsm_rssi.gsm_band_info;
	pl_sb->mon_channel =
	    tatl3_09GetGsmMonChannel(pl_sb->gsm_band_info,
				     v_tatrf_gsm_rssi.rx_channel);
	pl_sb->rx_channel = v_tatrf_gsm_rssi.rx_channel;
	/* RX Slots: default = 0x01, Setting all 8 bits to 1 enables continuous Rx mode. */
	/* AFC table mode: use tuned AFC value */
	pl_sb->rx_slot_mask_afc_table = MAKE16(0x01, CTRL_AFC_TUNED);
	pl_sb->fill1 = 0x0000;
	pl_sb->manualAfc = 0x0000;	/* CTRL_AFC_TUNED -> filler */
	pl_sb->agc_mode_fill2 = MAKE16(CTRL_RX_AGC_MANUAL, 0x00);
	pl_sb->rx_level = v_tatrf_gsm_rssi.rx_level;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "GSM RSSI");
	if (!TAT_OK(vl_Error)) {
		v_tatrf_gsm_rssi_status = RF_TEST_MODEM_NOT_READY;
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_HAL_RF_TEST_CONTROL_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_CONTROL_RESP,
				  "GSM RSSI response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatrf_gsm_rssi_status = pl_resp->status;

		SYSLOG(LOG_DEBUG,
		       "GSM RSSI tuning ends by C_TEST_STATUS = 0x%X",
		       v_tatrf_gsm_rssi_status);

		/* GSM RSSI is active */
		v_tatrf_gsm_rssi_state = C_RF_STATE_GSM_RSSI_STARTED;
	}

	return vl_Error;
}

int tatl12_p11StopGsmRx(void)
{
	v_tatrf_gsm_rssi_status =
	    SetU8InU16(LSB_POSS, v_tatrf_gsm_rssi_status,
		       RF_TEST_MODEM_NOT_READY);

	/* construct the correct  message */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_HAL_RF_TEST_CONTROL_STOP_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
				      C_HAL_RF_TEST_CONTROL_STOP_REQ);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "DCXO stop RX");
	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* message successfully sent. waiting for response */
	C_HAL_RF_TEST_CONTROL_STOP_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_CONTROL_STOP_RESP,
				  "DCXO stop RX response", &vl_Error);
	if (NULL != pl_resp)
		/* this what we expected */

		/* no status in response */
		/* GSM RSSI is inactive */
		v_tatrf_gsm_rssi_state = C_RF_STATE_GSM_RSSI_INACTIVE;

	return vl_Error;
}

int tatl12_p12MeasureGsmRx(void)
{
	/* construct the correct  message */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_HAL_RF_TEST_INFO_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
				      C_HAL_RF_TEST_INFO_REQ);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	pl_req->info_type = C_HAL_RX_PATH1_RSSI_INFO;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "RSSI measure");
	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* message successfully sent. waiting for response */
	C_HAL_RF_TEST_INFO_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_HAL_RF_TEST_INFO_RESP,
				  "RSSI measure response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		C_HAL_RX_RSSI_INFO_STR *pl_rssi_info =
		    (C_HAL_RX_RSSI_INFO_STR *) &pl_resp->data;
		v_tatrf_gsm_rssi_meas =
		    RF_UNQ(pl_rssi_info->rx_level, QUOT_TUNING_GSM_RSSI_LEVEL);
	}

	return vl_Error;
}

int tatl12_06GsmRssi_exec(struct dth_element *elem)
{
	int vl_Error = TAT_BAD_REQ;	/* errno return code */

	switch (elem->user_data) {
	case ACT_TUNING_GSM_RSSI:

		switch (v_tatrf_gsm_rssi_action) {
		case ON:

			/* always stop RX before starting it */
			vl_Error = tatl12_p11StopGsmRx();
			if (TAT_OK(vl_Error))
				vl_Error = tatl12_p10StartGsmRx();
			break;

		case OFF:

			vl_Error = tatl12_p11StopGsmRx();
			break;

		default:
			SYSLOG(LOG_ERR, "invalid GSM RSSI action mode.");
		}

		break;

	case ACT_TUNING_GSM_RSSI_MEAS:

		vl_Error = tatl12_p12MeasureGsmRx();
		break;

	default:
		SYSLOG(LOG_ERR, "invalid GSM RSSI action code.");
	}

	return vl_Error;
}

int tatl12_07WcdmaRssi_exec(void)
{
	/* construct the correct  message */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	v_tatrf_wcdma_rssi_resp.filler1_align_status =
	    SetU8InU16(LSB_POSS, v_tatrf_wcdma_rssi_resp.filler1_align_status,
		       RF_TEST_MODEM_NOT_READY);

	W_TEST_AGC_LNA_ALIGN_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_WCDMA_TEST,
				      W_TEST_AGC_LNA_ALIGN_REQ);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	/* xxxxxxxx--------  Filler
	   --------xxxxxxxx Length of one AGC step
	 */
	pl_req->filler1_agc_step_length =
	    v_tatrf_wcdma_rssi.filler1_agc_step_length;
	pl_req->mid_chan = v_tatrf_wcdma_rssi.mid_chan;
	pl_req->afc_dac_value = 0;	/* not used */
	pl_req->rx_chain_gain = v_tatrf_wcdma_rssi.rx_chain_gain;
	pl_req->mid_gain1 = 0;	/* not used */
	pl_req->mid_gain2 = 0;	/* not used */
	pl_req->low_gain1 = 0;	/* not used */
	pl_req->low_gain2 = 0;	/* not used */
	pl_req->agc_slope_align1 = 0;	/* not used */
	pl_req->agc_slope_align2 = 0;	/* not used */
	/* xxxxxxxx--------  Filler
	   --------xxxxxxxx Options for RX alignment control
	 */
	pl_req->filler2_rx_align_ctrl = 0x0001;	/* W_TEST_RX_ALIGN_CTRL */
	pl_req->low_chan = v_tatrf_wcdma_rssi.low_chan;
	pl_req->high_chan = v_tatrf_wcdma_rssi.high_chan;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "RSSI Wcdma");
	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* message successfully sent. waiting for response */
	W_TEST_AGC_LNA_ALIGN_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, W_TEST_AGC_LNA_ALIGN_RESP,
				  "RSSI Wcdma response", &vl_Error);
	if (NULL != pl_resp)
		/* this what we expected */
		v_tatrf_wcdma_rssi_resp = *pl_resp;

	return vl_Error;
}

int SetPwrTxMeasure(struct tatrf_isi_msg_t *pp_st, u16 vp_channel,
		    int16 vp_level)
{
	C_TEST_SB_PWR_SEQ_MEASURE_STR vl_pwr_measure;
	union tx_pwr_seq_data vl_pwr_seq_data;

	vl_pwr_measure.rx_attenuation_mode_and_fill1 =
	    MAKE16(C_TEST_ATTENUATOR_COARSE, 0x00);
	vl_pwr_measure.channel_nbr = vp_channel;
	vl_pwr_measure.rf_rx_attenuation_value = 0;	/* to be confirmed */
	vl_pwr_measure.lna_state = C_TEST_RF_LNA_OFF;	/* to be confirmed */
	vl_pwr_measure.rf_lna_step_size = 0;	/* to be confirmed */
	vl_pwr_measure.nbr_empty_slots = v_tatrf_pwr_tx_slots;	/* ok because codes match value. See field description in struct declaration. */

	int vl_Error = TAT_BAD_REQ;
	switch (v_tatrf_pwr_tx_mode) {
	case RF_MODE_GSM:

		vl_pwr_measure.system_info = C_TEST_GSM;
		vl_pwr_seq_data.gsm.rf_band =
		    tatl3_04GetGsmRfBand(v_tatrf_pwr_tx_band);
		vl_pwr_seq_data.gsm.pwr_level_nbr = vp_level;
		vl_pwr_seq_data.gsm.fill1 = ISIFILLER16;
		vl_pwr_measure.system_dep_data = vl_pwr_seq_data.wcdma;
		vl_pwr_measure.pwr_ctrl_value = 0x0000;
		vl_Error = TAT_ERROR_OFF;
		break;

	case RF_MODE_WCDMA:

		vl_pwr_measure.system_info = C_TEST_WCDMA;
		vl_pwr_measure.system_dep_data.rf_band =
		    tatl3_06GetWcdmaRfBand(v_tatrf_pwr_tx_band);
		vl_pwr_measure.system_dep_data.fill1 = ISIFILLER16;
		vl_pwr_measure.system_dep_data.fill2 = ISIFILLER16;
		vl_pwr_measure.pwr_ctrl_value = vp_level;
		vl_Error = TAT_ERROR_OFF;
		break;

	default:
		SYSLOG(LOG_ERR, "invalid tuning power TX mode");
	}

	if (TAT_OK(vl_Error)) {
		if (NULL ==
		    tatrf_isi_msg_add_sb_copy(pp_st, C_TEST_SB_PWR_SEQ_MEASURE,
					      &vl_pwr_measure,
					      sizeof(vl_pwr_measure)))
			vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
	}

	return vl_Error;
}

int tatl12_08PowerTx_exec(void)
{
	/* default outputs */
	v_tatrf_pwr_tx_err_code = RF_TEST_MODEM_NOT_READY;

	/* construct the correct  message */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_TEST_TUNING_REQ, numberOfSubBlocks);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	/* Fill 2 subblocks in GSM and 6 in WCDMA with the temporary stored data */
	int vl_Error = TAT_BAD_REQ;
	switch (v_tatrf_pwr_tx_mode) {
	case RF_MODE_GSM:

		/* one channel combined with two levels */
		SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[0],
				v_tatrf_pwr_tx_level[0]);
		SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[1],
				v_tatrf_pwr_tx_level[1]);
		vl_Error = TAT_ERROR_OFF;
		break;

	case RF_MODE_WCDMA:

		switch (v_tatrf_pwr_tx_amount_of_data) {
		case 2:
			SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[0],
					v_tatrf_pwr_tx_level[0]);
			SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[1],
					v_tatrf_pwr_tx_level[1]);
			vl_Error = TAT_ERROR_OFF;
			break;

		case 6:
			SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[0],
					v_tatrf_pwr_tx_level[0]);
			SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[1],
					v_tatrf_pwr_tx_level[1]);
			SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[2],
					v_tatrf_pwr_tx_level[2]);
			SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[3],
					v_tatrf_pwr_tx_level[3]);
			SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[4],
					v_tatrf_pwr_tx_level[4]);
			SetPwrTxMeasure(&vl_msg_info, v_tatrf_pwr_tx_channel[5],
					v_tatrf_pwr_tx_level[5]);
			vl_Error = TAT_ERROR_OFF;
			break;

		default:
			SYSLOG(LOG_ERR, "Bad number of amount of data!");
		}
		break;
	}

	/* send ISI message - blocking function */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Power TX");
	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* message successfully sent. waiting for response */
	C_TEST_TUNING_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_TEST_TUNING_RESP, numerOfSubBlocks,
			   "Power TX response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		v_tatrf_pwr_tx_err_code = pl_resp->status;

		/* erase last results */
		memset(v_tatrf_pwr_tx_results, 0,
		       sizeof(v_tatrf_pwr_tx_results));

		/* read result */
		int vl_nb_result = 0;
		C_TEST_SB_PWR_SEQ_MEAS_RESULTS_STR *pl_result =
		    tatrf_isi_find_sb(&vl_msg_info,
				      C_TEST_SB_PWR_SEQ_MEAS_RESULTS, NULL);
		while (pl_result) {
			if (vl_nb_result >= TAT_RF_NB_MEASURES) {
				/* too many subblocks returned! */
				SYSLOG(LOG_WARNING,
				       "too many results returned!");
				break;
			}

			v_tatrf_pwr_tx_results[vl_nb_result] =
			    pl_result->int_measured_pwr1;

			pl_result =
			    tatrf_isi_find_sb(&vl_msg_info,
					      C_TEST_SB_PWR_SEQ_MEAS_RESULTS,
					      pl_result);

			vl_nb_result++;
		}
	}

	return vl_Error;
}

int tatl12_09PowerCal_exec(void)
{
	/* default outputs */
	v_tatrf_pwr_cal_err_code = RF_TEST_MODEM_NOT_READY;

	memset(v_tatrf_pwr_cal_resp_channel, 0,
	       sizeof(v_tatrf_pwr_cal_resp_channel));
	memset(v_tatrf_pwr_cal_resp_tx, 0, sizeof(v_tatrf_pwr_cal_resp_tx));
	memset(v_tatrf_pwr_cal_resp_detector, 0,
	       sizeof(v_tatrf_pwr_cal_resp_detector));

	v_tatrf_pwr_cal_slope = 0.;
	v_tatrf_pwr_cal_b = 0.;
	v_tatrf_pwr_cal_c = 0.;

	/* construct the correct  message */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_TEST_TUNING_REQ, numberOfSubBlocks);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	C_TEST_SB_FREQ_RESP_CALIBRATE_STR *pl_sb =
	    tatrf_isi_msg_add_sb(&vl_msg_info, C_TEST_SB_FREQ_RESP_CALIBRATE);
	if (NULL == pl_sb)
		return TAT_ISI_HANDLER_ERROR;

	int vl_Error = TAT_BAD_REQ;
	int vl_nb_data = v_tatrf_pwr_cal_amount_of_data;
	switch (v_tatrf_pwr_cal_mode) {
	case RF_MODE_GSM:

		pl_sb->system_info = C_TEST_GSM;
		pl_sb->rf_band = tatl3_04GetGsmRfBand(v_tatrf_pwr_cal_band);
		vl_nb_data = 2;
		vl_Error = TAT_ERROR_OFF;
		break;

	case RF_MODE_WCDMA:

		pl_sb->system_info = C_TEST_WCDMA;
		pl_sb->rf_band = tatl3_06GetWcdmaRfBand(v_tatrf_pwr_cal_band);
		vl_Error = TAT_ERROR_OFF;
		break;

	default:
		SYSLOG(LOG_ERR, "invalid tuning power CAL mode");
	}

	if (TAT_OK(vl_Error)) {
		vl_nb_data = min(TAT_RF_NB_CAL_MEASURES, vl_nb_data);
		pl_sb->nbr_of_data = (uint16) vl_nb_data;

		C_TEST_FREQ_RESP_DATA_STR vl_data;
		int i;
		for (i = 0; i < vl_nb_data; i++) {
			vl_data.channel_nbr = v_tatrf_pwr_cal_channel[i];
			vl_data.int_data = v_tatrf_pwr_cal_int_level[i];
			vl_data.ext_data =
			    RF_Q(v_tatrf_pwr_cal_ext_level[i],
				 QUOT_TUNING_PWR_CAL_EXT_MEAS);
			vl_data.fill1 = 0;

			tatrf_isi_append_data(&vl_msg_info, (u16 *) &vl_data,
					      sizeof(vl_data) / sizeof(u16));
		}

		/* send ISI message - blocking function */
		vl_Error = tatrf_isi_send(&vl_msg_info, "Power Calibration");
		if (!TAT_OK(vl_Error))
			return vl_Error;

		/* message successfully sent. waiting for response */
		C_TEST_TUNING_RESP_STR *pl_resp =
		    tatrf_isi_read(&vl_msg_info, C_TEST_TUNING_RESP,
				   numerOfSubBlocks,
				   "Power Calibration response", &vl_Error);
		if (NULL != pl_resp) {
			/* this what we expected */

			/* save status. */
			v_tatrf_pwr_cal_err_code = LOWBYTE(pl_resp->status);

			C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS_STR *pl_sbr =
			    tatrf_isi_find_sb(&vl_msg_info,
					      C_TEST_SB_LINKO2_FREQ_RESP_CAL_RESULTS,
					      NULL);
			if (NULL != pl_sbr) {
				v_tatrf_pwr_cal_slope =
				    RF_UNQ(pl_sbr->slope,
					   QUOT_TUNING_PWR_CAL_SLOPE);
				v_tatrf_pwr_cal_b =
				    RF_UNQ(pl_sbr->coeff_b,
					   QUOT_TUNING_PWR_CAL_COEFF_B);
				v_tatrf_pwr_cal_c =
				    RF_UNQ(pl_sbr->coeff_c,
					   QUOT_TUNING_PWR_CAL_COEFF_C);

				vl_nb_data =
				    min(pl_sbr->nbr_of_data,
					TAT_RF_NB_PWR_CAL_RESP);
				for (i = 0; i < vl_nb_data; i++) {
					v_tatrf_pwr_cal_resp_channel[i] =
					    pl_sbr->freq_resp_data[i].
					    channel_nbr;
					v_tatrf_pwr_cal_resp_tx[i] =
					    RF_UNQ(pl_sbr->freq_resp_data[i].
						   ext_data,
						   QUOT_TUNING_PWR_CAL_RESP_EXT);
					v_tatrf_pwr_cal_resp_detector[i] =
					    RF_UNQ(pl_sbr->freq_resp_data[i].
						   int_data,
						   QUOT_TUNING_PWR_CAL_RESP_INT);
				}
			} else
				vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
		}
	}

	return vl_Error;
}

int tatl12_13AfcAdjust(void)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *pl_req = tatrf_isi_msg_begin(&vl_msg_info,
		PN_DSP_COMMON_TEST, C_TEST_TUNING_REQ, numberOfSubBlocks);

	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	C_TEST_SB_AFC_MEAS_AND_ADJUST_STR *pl_sb2 =
		tatrf_isi_msg_add_sb_copy(&vl_msg_info,
		C_TEST_SB_AFC_MEAS_AND_ADJUST, &v_tatrf_gsm_afc_adjust,
		sizeof(v_tatrf_gsm_afc_adjust));

	if (NULL == pl_sb2)
		return TAT_ISI_HANDLER_ERROR;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "AFC adjustment");

	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* message successfully sent. waiting for response */
	C_TEST_TUNING_RESP_STR *pl_resp = tatrf_isi_read(&vl_msg_info,
		C_TEST_TUNING_RESP, numerOfSubBlocks,
		"AFC adjustment response", &vl_Error);

	if (NULL == pl_resp)
		return TAT_ISI_HANDLER_ERROR;

	C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS_STR *pl_sbr = tatrf_isi_find_sb(
		&vl_msg_info, C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS, NULL);

	if (NULL == pl_sbr)
		return TAT_ERROR_NOT_MATCHING_MSG;

	memcpy(&v_tatrf_gsm_afc_adjust_result, pl_sbr,
		SIZE_C_TEST_SB_AFC_MEAS_AND_ADJUST_RESULTS_STR);

	return TAT_ERROR_OFF;
}

int tat12_04Dcxo_ActivateRx(u16 vp_mode)
{
	v_tatrf_dcxo_rx_resp.status = RF_TEST_MODEM_NOT_READY;

	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_TUNING_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
			C_TEST_TUNING_REQ, numberOfSubBlocks);
	if (NULL == pl_req)
		return TAT_ISI_HANDLER_ERROR;

	C_TEST_SB_GSM_RX_CONTROL_STR *pl_sb =
	    tatrf_isi_msg_add_sb(&vl_msg_info, C_TEST_SB_GSM_RX_CONTROL);
	if (NULL == pl_sb)
		return TAT_ISI_HANDLER_ERROR;

	pl_sb->mode = vp_mode;
	pl_sb->operationMode = C_TEST_CONTINUOUS;
	pl_sb->rf_band = v_tatrf_gsm_rx_ctrl.rf_band;
	pl_sb->channel = v_tatrf_gsm_rx_ctrl.channel;
	pl_sb->mon_ch = v_tatrf_gsm_rx_ctrl.mon_ch;
	pl_sb->afc = v_tatrf_gsm_rx_ctrl.afc;
	pl_sb->agc = v_tatrf_gsm_rx_ctrl.agc;

	int vl_Error = TAT_BAD_REQ;
	if (C_TEST_RX_ON == vp_mode) {
		/* send ISI message - blocking function */
		vl_Error = tatrf_isi_send(&vl_msg_info, "DCXO start RX");

	} else if (C_TEST_RX_OFF == vp_mode)
		/* send ISI message - blocking function */
		vl_Error = tatrf_isi_send(&vl_msg_info, "DCXO stop RX");

	if (!TAT_OK(vl_Error))
		return vl_Error;

	/* message successfully sent. waiting for response */
	C_TEST_TUNING_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_TEST_TUNING_RESP, numerOfSubBlocks,
			   "DCXO RX response", &vl_Error);
	if (NULL == pl_resp)
		return TAT_ISI_HANDLER_ERROR;

	/* this what we expected */
	C_TEST_SB_GSM_RX_CONTROL_RESP_STR *pl_sbr =
		tatrf_isi_find_sb(&vl_msg_info, C_TEST_SB_GSM_RX_CONTROL_RESP, NULL);

	if (NULL == pl_sbr)
		return TAT_ERROR_NOT_MATCHING_MSG;

	v_tatrf_dcxo_rx_resp = *pl_sbr;

	if (C_TEST_OK == v_tatrf_dcxo_rx_resp.status) {
		if (vp_mode == C_TEST_RX_ON)
			v_tatrf_dcxo_state = C_RF_STATE_DCXO_STARTED;
		else
			v_tatrf_dcxo_state = C_RF_STATE_DCXO_INACTIVE;
	}

	return TAT_ERROR_OFF;
}

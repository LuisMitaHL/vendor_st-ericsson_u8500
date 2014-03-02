/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   selftest.c
* \brief   routines to run RF selftest
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "selftest.h"

#include "test_handler_modem_ext.h"
#include "rfdd_st_ext.h"		/* for self-test constants and typedefs */

#include "misc.h"
#include "isimsg.h"


/* Start antenna connection test (returns a VSWR value) */
#define ANTENNA_VSWR                        0x0009
#define	SELF_TEST_RESOURCE_ID			    0x91
#define	SELF_TEST_TESTCASE_ID			    0x0000
#define FILLER_PADDING					0x55AA

#define QUOT_SELFTEST_ANTENNA_PWR_LEVEL     6
#define QUOT_SELFTEST_ANTENNA_VSWR          6


/**
 * List of available orders supported by a process.
 */
typedef enum {
	PASSED = 0,
	FAILED,
	UNDEFINED,
	CASE_NOT_SUPPORTED,
	DATA_INVALID,
	BUSY,
	TIMEOUT,
	DONE,
	UNKNOWN
} SELF_TEST_STATUS;

/**
 * Sequence: Output parameters for WCDMA Tx Self-test.
 */
typedef struct {
    /* WCDMA/GSM high band that was tested. */
	uint16 highBand;
    /* WCDMA/GSM Tx error code in high band. */
	uint16 highBandErrorCode;
    /* WCDMA/GSM Tx measured level in high band. */
	uint16 highBandPwrValue;
    /* WCDMA/GSM low band that was tested. */
	uint16 lowBand;
    /* WCDMA/GSM Tx error code in low band. */
	uint16 lowBandErrorCode;
    /* WCDMA/GSM Tx measured level in low band. */
	uint16 lowBandPwrValue;
} WCDMA_GSM_TX_OUTPUT;

/**
 * Sequence: Output parameters for PA ID & PIN Self-test.
 */
typedef struct {
	uint16 errorCode;		/**< PA ID & PIN error code. */
	uint16 voltageValue;	/**< PA ID & PIN voltage measured value. */
} PA_ID_PIN_OUTPUT;

/**
 * Sequence: Output parameters for RF Supply Self-test.
 */
typedef struct {
	uint16 stw3530ErrorCode;
	uint16 stw3550ErrorCode;	/* not used (Diversity RF Supply is here) */
	uint16 stw3500ErrorCode;  /**< STw3500 error code. */
	uint16 regulators[RF_SUPPLY_REGULATOR_COUNT];	/* supply regulators measures */
} RF_SUPPLY_OUTPUT;

/* Diversity RF supply regulator indexes in DIVERSITY_RF_SUPPLY_OUTPUT.regulators array */
typedef enum {
	DIVERSITY_RF_SUPPLY_REGULATOR_VDIG,
	DIVERSITY_RF_SUPPLY_REGULATOR_VLPF_RX,
	DIVERSITY_RF_SUPPLY_REGULATOR_VVCO_RX,
	DIVERSITY_RF_SUPPLY_REGULATOR_VABM_RX,
	DIVERSITY_RF_SUPPLY_REGULATOR_VFE_RX,
	DIVERSITY_RF_SUPPLY_REGULATOR_VLO_RX,
	DIVERSITY_RF_SUPPLY_REGULATOR_VXO,
	DIVERSITY_RF_SUPPLY_REGULATOR_COUNT	/* always last in enum */
} DIVERSITY_RF_SUPPLY_REGULATOR_INDEX;

/**
 * Sequence: Output parameters for diversity RF Supply Self-test.
 */
typedef struct {
	uint16 stw3530ErrorCode;	/* not used */
	uint16 stw3550ErrorCode;	/* the only one error code for Diversity Supply is here */
	uint16 stw3500ErrorCode;	/* not used */
	uint16 regulators[DIVERSITY_RF_SUPPLY_REGULATOR_COUNT];	/* supply regulators measures */
} DIVERSITY_RF_SUPPLY_OUTPUT;

typedef union {
    WCDMA_GSM_TX_OUTPUT                 wcdma_gsm_tx;
    PA_ID_PIN_OUTPUT                    pa_id_pin;
    RF_SUPPLY_OUTPUT                    rf_supply;
    DIVERSITY_RF_SUPPLY_REGULATOR_INDEX div_rf_supply_regulator;
    DIVERSITY_RF_SUPPLY_OUTPUT          div_rf_supply;
} RF_SELFTEST_OUTPUT;

/*-----------------------------------------------------------------------------
Global variables
-----------------------------------------------------------------------------*/
/* RF Bus Selft-test error code. */
uint16 v_tatrf_rfbus_error_code = 0x0000;

/* RF Bus Selft-test status. */
SELF_TEST_STATUS v_tatrf_rfbus_status = UNKNOWN;

/* Strobe Selft-test error code. */
uint16 v_tatrf_strobe_error_code = 0x0000;

/* Strobe Selft-test status. */
SELF_TEST_STATUS v_tatrf_strobe_status = UNKNOWN;

/* Digital Tx/Rx interface Selft-test error code. */
uint16 v_tatrf_digi_error_code = 0x0000;

/* Digital Tx/Rx interface Selft-test status. */
SELF_TEST_STATUS v_tatrf_digi_status = UNKNOWN;

/* WCDMA Tx Self-test output. */
WCDMA_GSM_TX_OUTPUT v_tatrf_wcdma_tx_output = {
	.highBand = 0u,
    .highBandErrorCode = UNKNOWN,
	.highBandPwrValue = 0u,
	.lowBand = 0u,
	.lowBandErrorCode = UNKNOWN,
	.lowBandPwrValue = 0u
};

/* WCDMA Tx Selft-test status. */
SELF_TEST_STATUS v_tatrf_wcdma_tx_status = UNKNOWN;

/* PA ID & PIN output. */
PA_ID_PIN_OUTPUT v_tatrf_pa_id_pin_output = {
	.errorCode = 0u,
	.voltageValue = 0u
};

/* PA ID & PIN Selft-test status. */
SELF_TEST_STATUS v_tatrf_pa_id_pin_status = UNKNOWN;

/* RF Supply output parameters. */
RF_SUPPLY_OUTPUT v_tatrf_rf_supply_output = {
	.stw3530ErrorCode = 0u,
	.stw3550ErrorCode = 0u,
	.stw3500ErrorCode = 0u,
	.regulators = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/* RF Supply Selft-test status. */
SELF_TEST_STATUS v_tatrf_rf_supply_status = UNKNOWN;

/* Antenna Selft-test status. */
SELF_TEST_STATUS v_tatrf_rf_ant_status = UNKNOWN;

/* Antenna Selft-test error code. */
uint16 v_tatrf_rf_ant_error_code = 0x0000;

/* Q6 VSWR measured value. */
uint16 v_tatrf_rf_ant_vswr = 0x0000;

/* Bitfield SYSTEM. */
uint16 v_tatrf_rf_ant_sysinfo = 0x0000;

/* GSM or WCDMA band. */
uint32 v_tatrf_rf_ant_band = 0x0000;

/* Tx channel number. */
uint16 v_tatrf_rf_ant_channel = 0x0000;

/* Q6 Tx signal power level (dBm). */
int16 v_tatrf_rf_ant_pwr_level = 0;

/* Diversity RF Bus Selft-test error code. */
uint16 v_tatrf_div_rfbus_error_code = 0x0000;

/* Diversity RF Bus Selft-test status. */
SELF_TEST_STATUS v_tatrf_div_rfbus_status = UNKNOWN;

/* Diversity Digital Tx/Rx interface Selft-test error code. */
uint16 v_tatrf_div_digi_error_code = 0x0000;

/* Diversity Digital Tx/Rx interface Selft-test status. */
SELF_TEST_STATUS v_tatrf_div_digi_status = UNKNOWN;

/* Diversity RF Supply output parameters. */
DIVERSITY_RF_SUPPLY_OUTPUT v_tatrf_div_rf_supply_output = {
    .stw3530ErrorCode = 0u,
	.stw3550ErrorCode = 0u,
	.stw3500ErrorCode = 0u,
	.regulators = { 0, 0, 0, 0, 0, 0, 0 }
};

/* Diversity RF Supply Selft-test status. */
SELF_TEST_STATUS v_tatrf_div_rf_supply_status = UNKNOWN;

/* Function tide to Start SelfTest                */
/**************************************************/
int DthRf_SelfTest_Get(struct dth_element *elem, void *value)
{
	return tatl10_02SelfTest_Get(elem, value);
}

int DthRf_SelfTest_Set(struct dth_element *elem, void *value)
{
	return tatl10_03SelfTest_Set(elem, value);
}

int DthRf_SelfTest_Exec(struct dth_element *elem)
{
	return tatl10_00SelfTest_Exec(elem);
}

/* should have DIVERSITY_RF_SUPPLY_REGULATOR_COUNT elements and be ordered the 
   same way DIVERSITY_RF_SUPPLY_REGULATOR_INDEX is. */
const char *DIVERSITY_RF_SUPPLY_REGULATORS[DIVERSITY_RF_SUPPLY_REGULATOR_COUNT]
    = {
	"VDIG", "VLPF_RX", "VVCO_RX", "VABM_RX", "VFE_RX", "VLO_RX", "VXO"
};

/* should have RF_SUPPLY_REGULATOR_COUNT elements and be ordered the same way RF_SUPPLY_REGULATOR_INDEX is. */
const char *RF_SUPPLY_REGULATORS[RF_SUPPLY_REGULATOR_COUNT] = {
	"VREG50BD",
	"VREG50S",
	"VHI",
	"VBATT",
	"VDIG_RX",
	"VLPF_RX",
	"VVCO_RX",
	"VBAM",
	"VFE",
	"VLO_RX",
	"VXO",
	"VLPF_TX",
	"VOPA",
	"VVCO_TX",
	"VDIG_TX",
	"VSEL",
	"VPAP",
	"VDP_TX",
	"VLO_TX",
	"VFB_TX"
};

int tatl10_selfTestAntenna();
int tatl10_commonSelfTest(struct dth_element *elem);

int tatl10_00SelfTest_Exec(struct dth_element *elem)
{
	int vl_error = TAT_ERROR_CASE;

	switch (elem->user_data) {
	case ACT_ANTENNA_START:
		vl_error = tatl10_selfTestAntenna();
		break;

	default:
		vl_error = tatl10_commonSelfTest(elem);
	}

	return vl_error;
}

int tatl10_02SelfTest_Get(struct dth_element *elem, void *value)
{
	int vl_error = TAT_ERROR_OFF;

	switch (elem->user_data) {
		/* Get RF Bus status. */
	case ACT_RFBUS_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_rfbus_status;
		}
		break;

		/* Get RF Bus error code. */
	case ACT_RFBUS_START_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_rfbus_error_code;
		}
		break;

		/* Get Strobe status. */
	case ACT_STROBE_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_strobe_status;
		}
		break;

		/* Get Strobe error code. */
	case ACT_STROBE_START_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_strobe_error_code;
		}
		break;

		/* Get Digi RX/TX status. */
	case ACT_DIGI_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_digi_status;
		}
		break;

		/* Get Digi RX/TX interface error code. */
	case ACT_DIGI_START_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_digi_error_code;
		}
		break;

		/* Get WCDMA TX status. */
	case ACT_WCDMATX_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_wcdma_tx_status;
		}
		break;

		/* Get WCDMA TX error code in low band. */
	case ACT_WCDMATX_START_LOW_BAND_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_wcdma_tx_output.lowBandErrorCode;
		}
		break;

		/*Get measured power in low band. */
	case ACT_WCDMATX_START_LOW_BAND_PWR_VAL:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_wcdma_tx_output.lowBandPwrValue;
		}
		break;

		/* Get WCDMA TX error code in low band. */
	case ACT_WCDMATX_START_HIGH_BAND_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_wcdma_tx_output.highBandErrorCode;
		}
		break;

		/*Get measured power in high band. */
	case ACT_WCDMATX_START_HIGH_BAND_PWR_VAL:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_wcdma_tx_output.highBandPwrValue;
		}
		break;

		/* Get PA ID & PIN status. */
	case ACT_PAIDPIN_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_pa_id_pin_status;
		}
		break;

		/*Get PA ID & PIN error code. */
	case ACT_PAIDPIN_START_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_pa_id_pin_output.errorCode;
		}
		break;

		/*Get PA ID & PIN voltage value code. */
	case ACT_PAIDPIN_START_VOLTAGE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_pa_id_pin_output.voltageValue;
		}
		break;

		/* Get RF Supply status. */
	case ACT_RFSUPPLY_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_rf_supply_status;
		}
		break;

		/*Get RF Supply STw3530 error code. */
	case ACT_RFSUPPLY_START_STW3530_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_rf_supply_output.stw3530ErrorCode;
		}
		break;

		/*Get RF Supply STw3500 error code. */
	case ACT_RFSUPPLY_START_STW3500_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_rf_supply_output.stw3500ErrorCode;
		}
		break;

		/* Get RF Supply status. */
	case ACT_ANTENNA_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_rf_ant_status;
		}
		break;

		/*Get Antenna error code. */
	case ACT_ANTENNA_START_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_rf_ant_error_code;
		}
		break;

		/*Get Antenna VSWR value. */
	case ACT_ANTENNA_START_VSWR:
		{
			float *data = (float *)value;
			*data =
			    RF_UNQ(v_tatrf_rf_ant_vswr,
				   QUOT_SELFTEST_ANTENNA_VSWR);
		}
		break;

		/* Get RF Supply input parameters. */
	case ACT_RFSUPPLY_START_REGULATORS:	/* TYPE U16[C:20] */
		{
			int i;
			struct dth_array *vl_array = (struct dth_array *)value;

			SYSLOG(LOG_DEBUG,
			       "get: RF Supply regulators (cols = %d, rows = %d)...",
			       vl_array->col, vl_array->row);
			GetDthArray(elem, vl_array, sizeof(u16),
				    v_tatrf_rf_supply_output.regulators,
				    sizeof(uint16), RF_SUPPLY_REGULATOR_COUNT);
			if (vl_array->col == elem->cols
			    && vl_array->row == elem->rows) {
				for (i = 0; i < RF_SUPPLY_REGULATOR_COUNT; i++) {
					SYSLOGSTR(LOG_DEBUG,
						  "regulator %d = %u\n", i,
						  DEREF_PTR((u16 *) vl_array->
							    array + i, u16));
				}
			} else {
				i = vl_array->col;
				SYSLOGSTR(LOG_DEBUG, "regulator %d = %u\n", i,
					  DEREF_PTR((u16 *) vl_array->array,
						    u16));
			}
		}
		break;

	case ACT_DIV_RFBUS_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_div_rfbus_status;
		}
		break;

	case ACT_DIV_RFBUS_START_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_div_rfbus_error_code;
		}
		break;

	case ACT_DIV_DIGI_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_div_digi_status;
		}
		break;

	case ACT_DIV_DIGI_START_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_div_digi_error_code;
		}
		break;

	case ACT_DIV_RFSUPPLY_START_STATUS:
		{
			uint8 *data = (uint8 *) value;
			*data = v_tatrf_div_rf_supply_status;
		}
		break;

	case ACT_DIV_RFSUPPLY_START_STW3550_ERROR_CODE:
		{
			uint16 *data = (uint16 *) value;
			*data = v_tatrf_div_rf_supply_output.stw3550ErrorCode;
		}
		break;

		/* RF Diversity power supply regulators outputs */
	case ACT_DIV_RFSUPPLY_START_VDIG:
	case ACT_DIV_RFSUPPLY_START_VLPF_RX:
	case ACT_DIV_RFSUPPLY_START_VVCO_RX:
	case ACT_DIV_RFSUPPLY_START_VABM_RX:
	case ACT_DIV_RFSUPPLY_START_VFE_RX:
	case ACT_DIV_RFSUPPLY_START_VLO_RX:
	case ACT_DIV_RFSUPPLY_START_VXO:
		{
			int vl_regulator_index = -1;

			switch (elem->user_data) {
			case ACT_DIV_RFSUPPLY_START_VDIG:

				vl_regulator_index =
				    DIVERSITY_RF_SUPPLY_REGULATOR_VDIG;
				break;

			case ACT_DIV_RFSUPPLY_START_VLPF_RX:

				vl_regulator_index =
				    DIVERSITY_RF_SUPPLY_REGULATOR_VLPF_RX;
				break;

			case ACT_DIV_RFSUPPLY_START_VVCO_RX:

				vl_regulator_index =
				    DIVERSITY_RF_SUPPLY_REGULATOR_VVCO_RX;
				break;

			case ACT_DIV_RFSUPPLY_START_VABM_RX:

				vl_regulator_index =
				    DIVERSITY_RF_SUPPLY_REGULATOR_VABM_RX;
				break;

			case ACT_DIV_RFSUPPLY_START_VFE_RX:

				vl_regulator_index =
				    DIVERSITY_RF_SUPPLY_REGULATOR_VFE_RX;
				break;

			case ACT_DIV_RFSUPPLY_START_VLO_RX:

				vl_regulator_index =
				    DIVERSITY_RF_SUPPLY_REGULATOR_VLO_RX;
				break;

			case ACT_DIV_RFSUPPLY_START_VXO:

				vl_regulator_index =
				    DIVERSITY_RF_SUPPLY_REGULATOR_VXO;
				break;
			}

			if ((vl_regulator_index >= 0)
			    && (vl_regulator_index <
				DIVERSITY_RF_SUPPLY_REGULATOR_COUNT)) {
				*((uint16 *) value) =
				    v_tatrf_div_rf_supply_output.
				    regulators[vl_regulator_index];
			} else {
				SYSLOG(LOG_ERR,
				       "Invalid RF Diversity supply regulator code: %d",
				       elem->user_data);
				vl_error = TAT_BAD_REQ;
			}
		}
		break;

	default:
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

int tatl10_03SelfTest_Set(struct dth_element *elem, void *value)
{
	int vl_error;

	vl_error = TAT_ERROR_OFF;

	switch (elem->user_data) {
		/*Set Antenna parameters. */
	case ACT_ANTENNA_START_SELECT:
		{
			uint16 *data = (uint16 *) value;
			switch (*data) {
			case ENUM_SYSTEM_GSM:

				v_tatrf_rf_ant_sysinfo = C_TEST_GSM;
				break;

			case ENUM_SYSTEM_WCDMA:

				v_tatrf_rf_ant_sysinfo = C_TEST_WCDMA;
				break;

			default:
				vl_error = TAT_BAD_REQ;
			}
		}
		break;
	case ACT_ANTENNA_START_BAND:
		{
			uint32 *data = (uint32 *) value;
			v_tatrf_rf_ant_band = *data;
		}
		break;
	case ACT_ANTENNA_START_CHANNEL:
		{
			uint16 *data = (uint16 *) value;
			v_tatrf_rf_ant_channel = *data;
		}
		break;
	case ACT_ANTENNA_START_PWR_LEVEL:
		{
			float *data = (float *)value;
			v_tatrf_rf_ant_pwr_level =
			    RF_Q(*data, QUOT_SELFTEST_ANTENNA_PWR_LEVEL);
		}
		break;
	default:
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

/**
 * Decode the ISI message sent by modem in response of starting RF Self-test.
 * @param[in] pp_req ISI message to be filled. 
 * @param[in] element specifies the DTH element. 
 * @retval 0 success.
 * @retval errorCode if the self-test id is unknown.
 */
int tatl10_decodeSelfTest(struct tatrf_isi_msg_t *pp_st,
			  struct dth_element *elem)
{
	int vl_error = TAT_ERROR_OFF;
	MODEM_TEST_RUN_RESP_STR *pl_resp =
	    (MODEM_TEST_RUN_RESP_STR *) tatrf_isi_msg_header(pp_st);
    RF_SELFTEST_OUTPUT *pl_output;
    size_t vl_st_size;

	switch (elem->user_data)
    {
		/* If the test is not passed, set the error code. */
	case ACT_RFBUS_START:

		if (pl_resp->out_data_length >= sizeof(uint16)) {
			v_tatrf_rfbus_error_code = *pl_resp->out_data;
		} else {
			v_tatrf_rfbus_error_code = 0x0000;
		}

		v_tatrf_rfbus_status = pl_resp->status;
		break;

		/* If the test is not passed, set the error code. */
	case ACT_STROBE_START:

		if (pl_resp->out_data_length >= sizeof(uint16)) {
			v_tatrf_strobe_error_code = *pl_resp->out_data;
		} else {
			v_tatrf_strobe_error_code = 0x0000;
		}

		v_tatrf_strobe_status = pl_resp->status;
		break;

		/* If the test is not passed, set the error code. */
	case ACT_DIGI_START:

		if (pl_resp->out_data_length >= sizeof(uint16)) {
			v_tatrf_digi_error_code = *pl_resp->out_data;
		} else {
			v_tatrf_digi_error_code = 0x0000;
		}

		v_tatrf_digi_status = pl_resp->status;
		break;

		/* Set WCDMA Tx output parameters. */
	case ACT_WCDMATX_START:

        vl_st_size = sizeof(WCDMA_GSM_TX_OUTPUT);
        memset(&v_tatrf_wcdma_tx_output, 0, vl_st_size);

        if (pl_resp->out_data_length == vl_st_size)
        {
            pl_output = (RF_SELFTEST_OUTPUT *)pl_resp->out_data;
            memcpy(&v_tatrf_wcdma_tx_output, &pl_output->wcdma_gsm_tx,
                pl_resp->out_data_length);
		}
        else
        {
			SYSLOG(LOG_ERR, "incorrect output data");
            vl_error = TAT_ERROR_NOT_MATCHING_MSG;
		}
        v_tatrf_wcdma_tx_status = pl_resp->status;
		break;

		/* Set PA ID & PIN output parameters. */
	case ACT_PAIDPIN_START:

        vl_st_size = sizeof(PA_ID_PIN_OUTPUT);
        memset(&v_tatrf_pa_id_pin_output, 0, vl_st_size);

        if (pl_resp->out_data_length == vl_st_size)
        {
            pl_output = (RF_SELFTEST_OUTPUT *)pl_resp->out_data;
            memcpy(&v_tatrf_pa_id_pin_output, &pl_output->pa_id_pin,
                pl_resp->out_data_length);
		}
        else
        {
			SYSLOG(LOG_ERR, "incorrect output data");
            vl_error = TAT_ERROR_NOT_MATCHING_MSG;
		}

		v_tatrf_pa_id_pin_status = pl_resp->status;
		break;

		/* Set RF Supply output parameters. */
	case ACT_RFSUPPLY_START:

        vl_st_size = sizeof(RF_SUPPLY_OUTPUT);
        memset(&v_tatrf_rf_supply_output, 0, vl_st_size);

        if (pl_resp->out_data_length == vl_st_size)
        {
            pl_output = (RF_SELFTEST_OUTPUT *)pl_resp->out_data;
            memcpy(&v_tatrf_rf_supply_output, &pl_output->rf_supply,
                pl_resp->out_data_length);
		}
        else
        {
			SYSLOG(LOG_ERR, "incorrect output data");
            vl_error = TAT_ERROR_NOT_MATCHING_MSG;
		}

		v_tatrf_rf_supply_status = pl_resp->status;
		break;

	case ACT_DIV_RFBUS_START:

		if (pl_resp->out_data_length >= sizeof(uint16)) {
			v_tatrf_div_rfbus_error_code = *pl_resp->out_data;
		} else {
			v_tatrf_div_rfbus_error_code = 0x0000;
		}
		v_tatrf_div_rfbus_status = pl_resp->status;
		break;

	case ACT_DIV_DIGI_START:

		if (pl_resp->out_data_length >= sizeof(uint16)) {
			v_tatrf_div_digi_error_code = *pl_resp->out_data;
		} else {
			v_tatrf_div_digi_error_code = 0x0000;
		}
		v_tatrf_div_digi_status = pl_resp->status;
		break;

	case ACT_DIV_RFSUPPLY_START:

        vl_st_size = sizeof(DIVERSITY_RF_SUPPLY_OUTPUT);
        memset(&v_tatrf_div_rf_supply_output, 0, vl_st_size);

        if (pl_resp->out_data_length == vl_st_size)
        {
            pl_output = (RF_SELFTEST_OUTPUT *)pl_resp->out_data;
            memcpy(&v_tatrf_div_rf_supply_output, &pl_output->div_rf_supply,
                pl_resp->out_data_length);
		}
        else
        {
			SYSLOG(LOG_ERR, "incorrect output data");
            vl_error = TAT_ERROR_NOT_MATCHING_MSG;
		}

		v_tatrf_div_rf_supply_status = pl_resp->status;
		break;

	default:
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

void tatl10_displaySelfTestResult(struct dth_element *elem)
{
	int i = 0;

	switch (elem->user_data) {
	case ACT_RFBUS_START:
		{
			SYSLOGSTR(LOG_DEBUG, "RFBus error code: 0x%X\n",
				  v_tatrf_rfbus_error_code);
		}
		break;

	case ACT_STROBE_START:
		{
			SYSLOGSTR(LOG_DEBUG, "Strobe error code: 0x%X\n",
				  v_tatrf_strobe_error_code);
		}
		break;

	case ACT_DIGI_START:
		{
			SYSLOGSTR(LOG_DEBUG, "Digital TX/TX error code: 0x%X\n",
				  v_tatrf_digi_error_code);
		}
		break;

	case ACT_WCDMATX_START:
		{
			SYSLOGSTR(LOG_DEBUG,
				  "WCDMA low band tested: 0x%X, %d\n",
				  v_tatrf_wcdma_tx_output.lowBand,
				  v_tatrf_wcdma_tx_output.lowBand);
			SYSLOGSTR(LOG_DEBUG,
				  "WCDMA Error code in low band: 0x%X\n",
				  v_tatrf_wcdma_tx_output.lowBandErrorCode);
			SYSLOGSTR(LOG_DEBUG,
				  "WCDMA measured power value in low band: 0x%X, %d\n",
				  v_tatrf_wcdma_tx_output.lowBandPwrValue,
				  v_tatrf_wcdma_tx_output.lowBandPwrValue);
			SYSLOGSTR(LOG_DEBUG,
				  "WCDMA high band tested: 0x%X, %d\n",
				  v_tatrf_wcdma_tx_output.highBand,
				  v_tatrf_wcdma_tx_output.highBand);
			SYSLOGSTR(LOG_DEBUG,
				  "WCDMA error code in high band: 0x%X\n",
				  v_tatrf_wcdma_tx_output.highBandErrorCode);
			SYSLOGSTR(LOG_DEBUG,
				  "WCDMA measured power value in high band: 0x%X, %d\n",
				  v_tatrf_wcdma_tx_output.highBandPwrValue,
				  v_tatrf_wcdma_tx_output.highBandPwrValue);
		}
		break;

	case ACT_PAIDPIN_START:
		{
			SYSLOGSTR(LOG_DEBUG, "PA/ID pin error code: 0x%X\n",
				  v_tatrf_pa_id_pin_output.errorCode);
			SYSLOGSTR(LOG_DEBUG,
				  "PA/ID pin voltage value: 0x%X, %d\n",
				  v_tatrf_pa_id_pin_output.voltageValue,
				  v_tatrf_pa_id_pin_output.voltageValue);
		}
		break;

	case ACT_RFSUPPLY_START:
		{
			int i;

			SYSLOGSTR(LOG_DEBUG, "RF STw3530 error code: 0x%X\n",
				  v_tatrf_rf_supply_output.stw3530ErrorCode);
			SYSLOGSTR(LOG_DEBUG, "RF STw3500 error code: 0x%X\n",
				  v_tatrf_rf_supply_output.stw3500ErrorCode);
			SYSLOGSTR(LOG_DEBUG, "regulators: \n");
			for (i = 0; i < RF_SUPPLY_REGULATOR_COUNT; i++) {
				SYSLOGSTR(LOG_DEBUG, "%s = %u (0x%2X) ;",
					  RF_SUPPLY_REGULATORS[i],
					  v_tatrf_rf_supply_output.
					  regulators[i],
					  v_tatrf_rf_supply_output.regulators[i]
				    );
			}
			SYSLOGSTR(LOG_DEBUG, "\n");
		}
		break;

	case ACT_DIV_RFSUPPLY_START:

		SYSLOGSTR(LOG_DEBUG, "RF STw3500 error code: 0x%X\n",
			  v_tatrf_div_rf_supply_output.stw3550ErrorCode);
		SYSLOGSTR(LOG_DEBUG, "regulators: \n");
		for (i = 0; i < DIVERSITY_RF_SUPPLY_REGULATOR_COUNT; i++) {
			SYSLOGSTR(LOG_DEBUG, "%s = %u (0x%2X) ;",
				  DIVERSITY_RF_SUPPLY_REGULATORS[i],
				  v_tatrf_div_rf_supply_output.regulators[i],
				  v_tatrf_div_rf_supply_output.regulators[i]
			    );
		}
		SYSLOGSTR(LOG_DEBUG, "\n");
		break;

	default:

		SYSLOG(LOG_WARNING, "No description available for message %d",
		       elem->user_data);
	}

	SYSLOGSTR(LOG_DEBUG, "--SELF TEST RESPONSE--\n\n\n");
}

/**
 * Fill the ISI message sent to modem to perform a self-test.
 * @param[in] pp_req ISI message to be filled. 
 * @param[in] element specifies the DTH element. 
 * @retval 0 success.
 * @retval errorCode if the self-test id is unknown.
 */
int tatl10_initSelfTestCase(struct tatrf_isi_msg_t *pp_st,
			    struct dth_element *elem)
{
	int vl_error = TAT_ERROR_OFF;
	MODEM_TEST_RUN_REQ_STR *pl_req =
	    (MODEM_TEST_RUN_REQ_STR *) tatrf_isi_msg_header(pp_st);

	switch (elem->user_data) {
	case ACT_RFBUS_START:	/* Lauch RFBus Self-test. */
		{
			pl_req->test_id = LICENSEE1_RF_BB_IF_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	case ACT_STROBE_START:	/* Lauch Strobe Self-test. */
		{
			pl_req->test_id = LICENSEE1_STROBE_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	case ACT_DIGI_START:	/* Lauch Digi RX/TX interface Self-test. */
		{
			pl_req->test_id = LICENSEE1_DIGI_RXTX_IF_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	case ACT_WCDMATX_START:	/* Lauch WCDMA TX Self-test. */
		{
			pl_req->test_id = LICENSEE1_WCDMA_TX_POWER_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	case ACT_PAIDPIN_START:	/* Lauch PA & ID PIN Self-test. */
		{
			pl_req->test_id = LICENSEE1_PA_ID_PIN_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	case ACT_RFSUPPLY_START:	/* Lauch RF Supply Self-test. */
		{
			pl_req->test_id = LICENSEE1_RF_SUPPLY_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	case ACT_DIV_RFBUS_START:
		{
			pl_req->test_id = LICENSEE1_DIV_RF_BB_IF_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	case ACT_DIV_DIGI_START:
		{
			pl_req->test_id = LICENSEE1_DIV_DIGI_RXTX_IF_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	case ACT_DIV_RFSUPPLY_START:
		{
			pl_req->test_id = LICENSEE1_DIV_RF_SUPPLY_TEST;
			pl_req->inp_data_length = 0x0000;
		}
		break;

	default:
		vl_error = TAT_BAD_REQ;
	}

	return vl_error;
}

int tatl10_commonSelfTest(struct dth_element *elem)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	/* Set information common to all self test cases */
	MODEM_TEST_RUN_REQ_STR *pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, SELF_TEST_RESOURCE_ID,
				      MODEM_TEST_RUN_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}
	pl_req->group_id = MODEM_TEST_GRP_SELFTEST_L1;
	pl_req->case_ID = SELF_TEST_TESTCASE_ID;
	pl_req->attrib = 0x00;	/* TODO: which value here? */
	pl_req->fillbyte = 0x55;

	/* Set specific part of ISI message. */
	int vl_Error = tatl10_initSelfTestCase(&vl_msg_info, elem);
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* send ISI message - blocking function */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Self test");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	MODEM_TEST_RUN_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, MODEM_TEST_RUN_RESP,
				  "Self test response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		vl_Error = tatl10_decodeSelfTest(&vl_msg_info, elem);

		tatl10_displaySelfTestResult(elem);
	}

	return vl_Error;
}

int tatl10_selfTestAntenna()
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	v_tatrf_rf_ant_status = UNKNOWN;

	C_TEST_ANTENNA_CONTROL_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
				C_TEST_ANTENNA_CONTROL_REQ, sub_block_count);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}
	pl_req->ant_ctrl_mode = ANTENNA_VSWR;

	C_TEST_SB_ANTENNA_TEST_STR *pl_sb =
	    tatrf_isi_msg_add_sb(&vl_msg_info, C_TEST_SB_ANTENNA_TEST);
	if (NULL == pl_sb) {
		return TAT_ISI_HANDLER_ERROR;
	}
	pl_sb->fill1 = 0x0000;
	pl_sb->system_info = v_tatrf_rf_ant_sysinfo;

	switch (pl_sb->system_info) {
	case C_TEST_GSM:
		pl_sb->system_dep_data =
		    tatl3_04GetGsmRfBand(v_tatrf_rf_ant_band);
		break;

	case C_TEST_WCDMA:
		pl_sb->system_dep_data =
		    tatl3_06GetWcdmaRfBand(v_tatrf_rf_ant_band);
		break;

	default:
		SYSLOG(LOG_ERR, "Wrong system info: 0x%X", pl_sb->system_info);
		return TAT_BAD_REQ;
	}

	pl_sb->channel_nbr = v_tatrf_rf_ant_channel;
	pl_sb->pwr_level = v_tatrf_rf_ant_pwr_level;
	pl_sb->att_value = 0x0000;
	pl_sb->fill2 = 0x0000;

	/* send ISI message - blocking function */
	int vl_Error = tatrf_isi_send(&vl_msg_info, "Self test antenna");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_TEST_ANTENNA_CONTROL_RESP_STR *pl_resp =
	    tatrf_isi_read(&vl_msg_info, C_TEST_ANTENNA_CONTROL_RESP,
			   sub_block_count, "Self test antenna response",
			   &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		C_TEST_SB_ANTENNA_TEST_RESULT_STR *pl_sbr =
		    tatrf_isi_find_sb(&vl_msg_info,
				      C_TEST_SB_ANTENNA_TEST_RESULT, NULL);
		if (NULL != pl_sbr) {
			v_tatrf_rf_ant_error_code =
			    GetU8InU16(LSB_POSS, pl_sbr->fill1_substatus);
			v_tatrf_rf_ant_status = v_tatrf_rf_ant_error_code;

			v_tatrf_rf_ant_vswr = pl_sbr->vswr_value;
			vl_Error = TAT_ERROR_OFF;
		} else {
			vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
		}
	}

	return vl_Error;
}


/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rfic.c
* \brief   routines to access RFIC registers
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "rfic.h"

#include "misc.h"
#include "isimsg.h"


/* TYPES */
enum { REG_READ, REG_WRITE };


/* GLOBALS */

/* Register adress. */
u16 v_tatrf_rfic_RegisterAddress = 0U;
/* Data R/W from/to a register. */
u16 v_tatrf_rfic_RegisterValue = 0U;
/* Register Operation Mode : Read or Write. */
int v_tatrf_rfic_RegisterOpMode = 0;
/* RFIC deviceID index */
u32 v_tatrf_rfic_DeviceID = C_TEST_RFIC_DEV_RFIC;


/* Function tide to access to 2G5 and 3G registers */
/**************************************************/
int DthRf_Register_Get(struct dth_element *elem, void *value)
{
	return tatl8_02Register_get(elem, value);
}

int DthRf_Register_Set(struct dth_element *elem, void *value)
{
	return tatl8_03Register_set(elem, value);
}

int DthRf_Register_Exec(struct dth_element *elem)
{
	return tatl8_01Register_exec(elem);
}


/**
 * Read register value from Modem.
 * @param[in] vp_AdressRegister specifies the register address to be accessed.
 * @param[in] pp_ReadValue specifies the location where data read from register must be stored.
 * @param[in] vp_access_mode specifies RFIC access mode.
 * @param[in] vp_increment_mode specifies RFIC increment mode.
 */
int tatl8_readRegister(u16 vp_AdressRegister, u16 *pp_ReadValue,
		       u8 vp_access_mode, u8 vp_increment_mode);

/**
 * Write register value to Modem.
 * @param[in] vp_AdressRegister specifies the register address to be accessed.
 * @param[in] pp_WriteValue specifies the value to be written into the RFIC register.
 * @param[in] vp_access_mode specifies RFIC access mode.
 * @param[in] vp_increment_mode specifies RFIC increment mode.
 */
int tatl8_writeRegister(u16 vp_AdressRegister, u16 vp_WriteValue,
			u8 vp_access_mode, u8 vp_increment_mode);

int tatl8_makeAddressLsw(u32 vp_devId, u16 vp_regAddr, uint16 *pp_addrLsw);

int tatl8_01Register_exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_CASE;

	/* Variable declaration. */
	u8 vl_access_mode = 0U;
	u8 vl_increment_mode = 0U;

	switch (elem->user_data) {
	case ACT_REGISTER:	/* use stored arguments values to execute action */
		{
			switch (v_tatrf_rfic_RegisterOpMode) {
			case REG_READ:	/* Reading request. */
				{
					vl_access_mode = C_TEST_SINGLE;
					vl_increment_mode = C_TEST_STEP_FORWARD;
					vl_Error =
					    tatl8_readRegister
					    (v_tatrf_rfic_RegisterAddress,
					     &v_tatrf_rfic_RegisterValue,
					     vl_access_mode, vl_increment_mode);
				}
				break;

			case REG_WRITE:	/* Writing request. */
				{
					vl_access_mode = C_TEST_SINGLE;
					vl_increment_mode = C_TEST_STEP_FORWARD;
					vl_Error =
					    tatl8_writeRegister
					    (v_tatrf_rfic_RegisterAddress,
					     v_tatrf_rfic_RegisterValue,
					     vl_access_mode, vl_increment_mode);
				}
				break;

			default:	/* Invalid request code. */

				vl_Error = TAT_BAD_REQ;
			}
		}
		break;

	default:		/* Invalid request code. */

		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl8_02Register_get(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_CASE;
	u16 *vl_Value = (u16 *)value;

	switch (elem->user_data) {
	case ACT_REGISTER_DATA:
		{
			*vl_Value = v_tatrf_rfic_RegisterValue;
			SYSLOG(LOG_DEBUG, "Get register data: %d \n",
			       *vl_Value);
			vl_Error = TAT_ERROR_OFF;
		}
		break;

	default:
		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl8_03Register_set(struct dth_element *elem, void *value)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data) {
		/* Set operation mode (R/W). */
	case ACT_REGISTER:
		{
			v_tatrf_rfic_RegisterOpMode = *((u32 *) value);
			SYSLOG(LOG_DEBUG,
			       "Set register operation mode Read(0) Write(1): 0x%X\n",
			       v_tatrf_rfic_RegisterOpMode);
		}
		break;

		/* Set address register. */
	case ACT_REGISTER_ADDRESS:
		{
			v_tatrf_rfic_RegisterAddress = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set register adress: 0x%X\n",
			       v_tatrf_rfic_RegisterAddress);
		}
		break;

		/* Set data to write into register. */
	case ACT_REGISTER_DATA:
		{
			v_tatrf_rfic_RegisterValue = *((u16 *) value);
			SYSLOG(LOG_DEBUG,
			       "Set data to write into register: %u\n",
			       v_tatrf_rfic_RegisterValue);
		}
		break;

		/* Set device ID to read/write from/into register. */
	case ACT_REGISTER_DEVICE:
		{
			v_tatrf_rfic_DeviceID = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set RFIC Device ID index: %u\n",
			       v_tatrf_rfic_DeviceID);
		}
		break;

	default:
		/* Invalid request code */
		vl_Error = TAT_BAD_REQ;
	}

	return vl_Error;
}

int tatl8_readRegister(u16 vp_AdressRegister, u16 * pp_ReadValue,
		       u8 vp_access_mode, u8 vp_increment_mode)
{
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_TEST_RFIC_READ_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
				      C_TEST_RFIC_READ_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* Add specific data part of RFIC Read Request Message. */
	pl_req->accs_incr = MAKE16(vp_access_mode, vp_increment_mode);
	pl_req->address_msw = 0x0000;
	int vl_Error =
	    tatl8_makeAddressLsw(v_tatrf_rfic_DeviceID, vp_AdressRegister,
				 &pl_req->address_lsw);
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	pl_req->data_length = 1;	/* we read only one word (in any case???) */
	pl_req->fill1_fill1 = ISIFILLER8;	/* this is a filler */

	/* Send the ISI message and wait for response. */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Read IC register");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_TEST_RFIC_READ_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_TEST_RFIC_READ_RESP,
				  "Read IC register response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */
		/* response decoding */
		if (pl_resp->data_length != 1) {
			return TAT_ERROR_NOT_MATCHING_MSG;
		}

		/* extract data, which has not been yet decoded */
		tat_modem_marsh16(pl_resp->data, ISI_UNMARSH);
		*pp_ReadValue = pl_resp->data[0];
	}

	return TAT_ERROR_OFF;
}

int tatl8_writeRegister(u16 vp_AdressRegister, u16 vp_WriteValue,
			u8 vp_access_mode, u8 vp_increment_mode)
{
	/* Local variable declaration. */
	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	SYSLOG(LOG_DEBUG,
	       "write register(addr=0x%X, value=%u, access=0x%X, increment=0x%X)",
	       vp_AdressRegister, vp_WriteValue, vp_access_mode,
	       vp_increment_mode);

	C_TEST_RFIC_WRITE_REQ_STR *pl_req;
	pl_req =
	    tatrf_isi_msg_begin_no_sb(&vl_msg_info, PN_DSP_COMMON_TEST,
				      C_TEST_RFIC_WRITE_REQ);
	if (NULL == pl_req) {
		return TAT_ISI_HANDLER_ERROR;
	}

	/* Add specific data part of RFIC Write Request Message. */
	pl_req->accs_incr = MAKE16(vp_access_mode, vp_increment_mode);
	pl_req->address_msw = 0x0000;
	int vl_Error =
	    tatl8_makeAddressLsw(v_tatrf_rfic_DeviceID, vp_AdressRegister,
				 &pl_req->address_lsw);
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	pl_req->data_length = 1;	/* pushing one word */

	/* SIZE_C_TEST_RFIC_WRITE_REQ_STR includes the data member of 16 bits so we
	 * must set it directly */
	pl_req->data[0] = vp_WriteValue;
	tat_modem_marsh16(pl_req->data, ISI_MARSH);

	/* will append a padding by himself, if necessary. Don't care of it here */

	/* Send the ISI message and wait for response. */
	tat_modem_print_isi((uint8_t *) vl_msg_info.msg,
			    tatrf_isi_total_length(&vl_msg_info), LOG_DEBUG);

	vl_Error = tatrf_isi_send(&vl_msg_info, "Write IC register");
	if (!TAT_OK(vl_Error)) {
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	C_TEST_RFIC_WRITE_RESP_STR *pl_resp =
	    tatrf_isi_read_simple(&vl_msg_info, C_TEST_RFIC_WRITE_RESP,
				  "Write IC register response", &vl_Error);
	if (NULL != pl_resp) {
		/* this what we expected */

		/* TODO: something to with mode_status returned in response? */
	}

	return vl_Error;
}

int tatl8_makeAddressLsw(u32 vp_devId, u16 vp_regAddr, uint16 * pp_addrLsw)
{
	int vl_error = TAT_ERROR_OFF;
	*pp_addrLsw = 0;

	/* Device ID and Register address.
	   Bits 15:14 Device ID */
	u16 vl_realDevId;
	switch (vp_devId) {
	case RF_RFIC_DEV_DRFIC:
		vl_realDevId = C_TEST_RFIC_DEV_DRFIC;
		break;

	case RF_RFIC_DEV_RFIC:
		vl_realDevId = C_TEST_RFIC_DEV_RFIC;
		break;

	case RF_RFIC_DEV_PWRIC:
		vl_realDevId = C_TEST_RFIC_DEV_PWRIC;
		break;

	default:
		/* error: invalid RFIC device. Note: "Not used" is not proposed. */
		SYSLOG(LOG_ERR, "Invalid RFIC device index: %d", vp_devId);
		vl_error = EBADRQC;
	}

	if (TAT_OK(vl_error)) {
		*pp_addrLsw = ((vl_realDevId << 14) & (0x0003 << 14)) |
		    /* Bits 13:10 Reserved */
		    /* Bits 9:0 register address
		       See register map of corresponding IC. */
		    (vp_regAddr & 0x02FF);

		/* Note that access to nonexistent registers is not limited by SW and the 
		 * result is thus HW implementation dependent */
	}

	return vl_error;
}


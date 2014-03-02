/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "tatlcharge.h"

char CHARGE_param_array[CHARGE_ELEMENT_COUNT] = {0};
float CHARGE_float_array[CHARGE_ELEMENT_COUNT] = {0};
char *CHARGE_string_array[CHARGE_ELEMENT_COUNT] = {NULL};

Match_Table_string MT_UsbLinkStatus[] = {
	{0x00, "Not_configured"}
	,
	{0x01, "Standard_Host,not_charging"}
	,
	{0x02, "Standard_Host,charging,not_suspended"}
	,
	{0x03, "Standard_Host,charging,suspended"}
	,
	{0x04, "Host_charger,normal_mode"}
	,
	{0x05, "Host_charger,HS_mode"}
	,
	{0x06, "Host_charger,HS_Chirp_mode"}
	,
	{0x07, "Dedicated_USx_charger"}
	,
	{0x08, "ACA_RID_A_configuration"}
	,
	{0x09, "ACA_RID_B_configuration"}
	,
	{0x0a, "ACA_RID_C_configuration,normal_mode"}
	,
	{0x0b, "ACA_RID_C_configuration,HS_mode"}
	,
	{0x0c, "ACA_RID_C_configuration,HS_Chirp_mode"}
	,
	{0x0d, "Host_mode(IDGND)"}
	,
	{0x0e, "Reserved"}
	,
	{0x0f, "USB_link_not_valid"}
	,
	{0xff, "0xff"}
	,
};

Match_Table_string MT_AutoMainChCurr[] = {
	{0x00, "100mA"}
	,
	{0x01, "200mA"}
	,
	{0x02, "300mA"}
	,
	{0x03, "400mA"}
	,
	{0x04, "500mA"}
	,
	{0x05, "600mA"}
	,
	{0x06, "700mA"}
	,
	{0x07, "800mA"}
	,
	{0x08, "900mA"}
	,
	{0x09, "1000mA"}
	,
	{0x0a, "1100mA"}
	,
	{0x0b, "1200mA"}
	,
	{0x0c, "1300mA"}
	,
	{0x0d, "1400mA"}
	,
	{0x0e, "1500mA"}
	,
	{0x0f, "1500mA"}
	,
	{0xff, "0xff"}
	,
};

Match_Table_string MT_AutoVBusChCurr[] = {
	{0x00, "50mA"}
	,
	{0x01, "98mA"}
	,
	{0x02, "193mA"}
	,
	{0x03, "290mA"}
	,
	{0x04, "380mA"}
	,
	{0x05, "450mA"}
	,
	{0x06, "500mA"}
	,
	{0x07, "600mA"}
	,
	{0x08, "700mA"}
	,
	{0x09, "800mA"}
	,
	{0x0a, "900mA"}
	,
	{0x0b, "1000mA"}
	,
	{0x0c, "1100mA"}
	,
	{0x0d, "1300mA"}
	,
	{0x0e, "1400mA"}
	,
	{0x0f, "1500mA"}
	,
	{0xff, "0xff"}
	,
};

Match_Table_float MT_BattOkfalling[] = {
	{0x00, 2.36}
	,
	{0x01, 2.41}
	,
	{0x02, 2.46}
	,
	{0x03, 2.51}
	,
	{0x04, 2.56}
	,
	{0x05, 2.61}
	,
	{0x06, 2.66}
	,
	{0x07, 2.71}
	,
	{0x08, 2.76}
	,
	{0x09, 2.81}
	,
	{0x0a, 2.86}
	,
	{0x0b, 2.91}
	,
	{0x0c, 2.96}
	,
	{0x0d, 3.01}
	,
	{0x0e, 3.06}
	,
	{0x0f, 3.06}
	,
	{0xff, 0xff}
	,
};

Match_Table_float MT_OTP_BattOKF[] = {
	{0x00, 0.5}
	,
	{0x01, 0.4}
	,
	{0x02, 0.3}
	,
	{0x03, 0.2}
	,
	{0xff, 0xff}
	,
};

Match_Table_float MT_ChVoltLevel[] = {
	{0x00, 0x00}, /* 3.5V */
	{0x01, 0x04}, /* 3.6V */
	{0x02, 0x08}, /* 3.7V */
	{0x03, 0x0c}, /* 3.8V */
	{0x04, 0x10}, /* 3.9V */
	{0x05, 0x14}, /* 4.0V */
	{0x06, 0x16}, /* 4.05V */
	{0x07, 0x1b}, /* 4.10V */
	{0x08, 0x20}, /* 4.15V */
	{0x09, 0x25}, /* 4.2V */
	{0x0a, 0x2a}, /* 4.25V */
	{0x0b, 0x2f}, /* 4.3V */
	{0x0c, 0x39}, /* 4.4V */
	{0x0d, 0x43}, /* 4.5V */
	{0x0e, 0x4d}, /* 4.6V */
	{0xff, 0xff},
};

Match_Table_string MT_Str_OTP[] = {
	{0x00, "0mA"}
	,
	{0x01, "100mA"}
	,
	{0x02, "300mA"}
	,
	{0x03, "500mA"}
	,
	{0x04, "Not300mA"}
	,
	{0x05, "seeOTP11"}
	,
	{0xff, "0xff"}
	,
};

Match_Table_string MT_Str_Algo[] = {
	{0x00, "Disabled"},
	{0x01, "Enabled"},
};


static int tatl1_04_Get_MatchValue(void *item1, void *item2, char M_table)
{
	Match_Table_float *p_MTfloat = NULL;
	Match_Table_string *p_MTstring = NULL;
	char **str;

	switch (M_table) {
	case MT_BATTOKFALLING:
		p_MTfloat = MT_BattOkfalling;
		break;
	case MT_OTPBATTOKF:
		p_MTfloat = MT_OTP_BattOKF;
		break;
	case MT_STR_OTP:
		p_MTstring = MT_Str_OTP;
		break;
	case MT_AUTOMAINCHCURR:
		p_MTstring = MT_AutoMainChCurr;
		break;
	case MT_AUTOVBUSCHCURR:
		p_MTstring = MT_AutoVBusChCurr;
		break;
	case MT_USBLINKSTATUS:
		p_MTstring = MT_UsbLinkStatus;
		break;
	case MT_CHVOLTLEVEL:
		p_MTfloat = MT_ChVoltLevel;
		break;
	case MT_ALGOSTATUS:
		p_MTstring = MT_Str_Algo;
		break;
	default:
		printf("tatl1_04_Get_MatchValue CMD AVAILABLE\n");
		return TAT_ERROR_CASE;
		break;
	}

	if (p_MTfloat != NULL) {
		while (p_MTfloat->regvalue != 0xff) {
			if (p_MTfloat->regvalue == *((u8 *) item1)) {
				*((float *)item2) = p_MTfloat->value;
				return TAT_ERROR_OFF;
			}
			p_MTfloat++;
		}
	}

	if (p_MTstring != NULL) {
		while (p_MTstring->regvalue != 0xff) {
			if (p_MTstring->regvalue == *((u8 *) item1)) {
				str = (char **)item2;
				*str = p_MTstring->str;
				return TAT_ERROR_OFF;
			}
			p_MTstring++;
		}
	}

	return TAT_ERROR_CASE;
}

/*---------------------------------------------------------------------------*
 * Procedure    : tatl1_00ActCHARGE_exec
 *---------------------------------------------------------------------------*
 * Description  : Execute the Cmd Action sent by DTH
 *---------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, the  DTH element
 *---------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_00ActCHARGE_exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;
	u8 vl_VAL = 0;
	u8 vl_OTP150f = 0;
	u8 vl_OTP1503 = 0;
	u8 vl_2Rsel1 = 0;
	u8 vl_1Rsel1 = 0;
	u8 vl_2Rsel0 = 0;
	u8 vl_1Rsel0 = 0;
	u8 u8MV;
	char *p_strMV;
	char *p_strAlgo;
	float floatMV;
	FILE *fd = NULL;

	switch (elem->user_data) {

	case ACT_INITCHARGE:
		/* Enable or disable Algo charge */
		fd = fopen(CHARGE_SYSFS_PATH, "w");
		fprintf(fd, "%d", CHARGE_param_array[ACT_INITCHARGE]);
		fclose(fd);
		tatl1_04_Get_MatchValue(&CHARGE_param_array[ACT_INITCHARGE], &p_strAlgo, MT_ALGOSTATUS);
		CHARGE_string_array[ACT_ALGO] = p_strAlgo;
		break;

	case ACT_BATOK:
		abxxxx_read(BATTERY_OK_THRESHOLD_REG, &vl_VAL);

		u8MV = ((vl_VAL >> 4) & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &floatMV, MT_BATTOKFALLING);
		CHARGE_float_array[OUT_BATOKFSEL1] = floatMV;

		u8MV = (vl_VAL & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &floatMV, MT_BATTOKFALLING);
		CHARGE_float_array[OUT_BATOKFSEL0] = floatMV;

		abxxxx_read(0x150F, &vl_OTP150f);
		abxxxx_read(0x1503, &vl_OTP1503);

		vl_2Rsel1 = ((vl_OTP150f >> 6) & 0x03);
		tatl1_04_Get_MatchValue(&vl_2Rsel1, &floatMV, MT_OTPBATTOKF);
		CHARGE_float_array[OUT_BATOK2RSEL1] =
		    floatMV + CHARGE_float_array[OUT_BATOKFSEL1];

		vl_1Rsel1 = ((vl_OTP1503 >> 1) & 0x03);
		tatl1_04_Get_MatchValue(&vl_1Rsel1, &floatMV, MT_OTPBATTOKF);
		CHARGE_float_array[OUT_BATOK1RSEL1] =
		    floatMV + CHARGE_float_array[OUT_BATOKFSEL1];

		vl_2Rsel0 = ((vl_OTP150f >> 4) & 0x03);
		tatl1_04_Get_MatchValue(&vl_2Rsel0, &floatMV, MT_OTPBATTOKF);
		CHARGE_float_array[OUT_BATOK2RSEL0] =
		    floatMV + CHARGE_float_array[OUT_BATOKFSEL0];

		vl_1Rsel0 = ((vl_OTP150f >> 2) & 0x03);
		tatl1_04_Get_MatchValue(&vl_1Rsel1, &floatMV, MT_OTPBATTOKF);
		CHARGE_float_array[OUT_BATOK1RSEL0] =
		    floatMV + CHARGE_float_array[OUT_BATOKFSEL0];

		break;

	case ACT_OTPTEMP:
		abxxxx_read(0x1502, &vl_VAL);

		/* OTP2_BtempCmpLow bit3 */
		if ((vl_VAL & 0x08) == 0x08)
			u8MV = 1;
		else
			u8MV = 0;

		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_STR_OTP);
		CHARGE_string_array[OUT_OTP2] = p_strMV;

		/* OTP3_BtempCmpHigh bit4 */
		if ((vl_VAL & 0x10) == 0x10)
			u8MV = 2;
		else
			u8MV = 3;

		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_STR_OTP);
		CHARGE_string_array[OUT_OTP3] = p_strMV;

		/* OTP4_BtempCmpLow bit5 */
		if ((vl_VAL & 0x20) == 0x20)
			u8MV = 2;
		else
			u8MV = 4;

		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_STR_OTP);
		CHARGE_string_array[OUT_OTP4] = p_strMV;

		abxxxx_read(0x1504, &vl_VAL);

		/* OTP11_BtempCmpMedium bit6 */
		if ((vl_VAL & 0x40) == 0x40)
			u8MV = 1;
		else
			u8MV = 0;

		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_STR_OTP);
		CHARGE_string_array[OUT_OTP11] = p_strMV;

		/* OTP12_BtempCmpMedium bit7 */
		if ((vl_VAL & 0x80) == 0x80)
			u8MV = 2;
		else
			u8MV = 5;

		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_STR_OTP);
		if (!strcmp(p_strMV, "seeOTP11"))
			CHARGE_string_array[OUT_OTP12] = CHARGE_string_array[OUT_OTP11];
		else
			CHARGE_string_array[OUT_OTP12] = p_strMV;

		break;

	case ACT_GCHINFO:
		/* read AutoMainChInputCurr[3:0] bits
		 * in MainChStatus1 register @0x0B00 [7:4] */
		abxxxx_read(MAIN_CHARGER_STATUS_REG, &vl_VAL);

		u8MV = ((vl_VAL >> 4) & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_AUTOMAINCHCURR);
		CHARGE_string_array[OUT_AUTOMAINCHINCURR] = p_strMV;

		/* read AutoVbusChInputCurr[3:0] bits in
		 * UsbChStatus2 register @0x0B03 [7:4] */
		abxxxx_read(USB_CHARGER_STATUS_2_REG, &vl_VAL);

		u8MV = ((vl_VAL >> 4) & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_AUTOVBUSCHCURR);
		CHARGE_string_array[OUT_AUTOVBUSCHINCURR] = p_strMV;

		break;

	case ACT_GUSBINFO:
		/* read UsbLinkStatus[3:0] in register UsbLineStatus @0x0580 [5:2] */
		abxxxx_read(USB_LINE_STATUS_REG, &vl_VAL);
		u8MV = ((vl_VAL & 0x3c) >> 2);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_USBLINKSTATUS);
		CHARGE_string_array[OUT_USBLINKSTATUS] = p_strMV;
		break;

	case ACT_SMAINCHINFO:

		/* ACTION ENABLE */
		if (CHARGE_param_array[elem->user_data] == CHARGE_ENABLE) {
			/* write in  ChOutputCurrentLevel[3:0]
			 * in ChOutCurrLevel register @0x0B42 bit [3:0] */
			abxxxx_read(CHANNEL_OUTPUT_CURRENT_LEVEL_REG, &vl_VAL);
			vl_VAL = ((vl_VAL & 0xf0) | CHARGE_param_array[IN_CHOUTCUR]);
			abxxxx_write(CHANNEL_OUTPUT_CURRENT_LEVEL_REG, vl_VAL);

			/* write in  ChOutputCurrentLevelMax[3:0]
			 * in ChOutCurrLevelMax  register @0x0B43 bit [3:0] */
			abxxxx_read(0x0B43, &vl_VAL);
			vl_VAL = ((vl_VAL & 0xf0) | CHARGE_param_array[IN_CHOUTCURMAX]);
			abxxxx_write(0x0B43, vl_VAL);

			/* write value in ChVoltLevel[6:0]
			 * bit register ChVoltLevel @0x0B40 bits [6:0] */
			tatl1_04_Get_MatchValue(&CHARGE_param_array[IN_CHVOLTLEVEL], &floatMV, MT_CHVOLTLEVEL);
			abxxxx_read(CH_VOLT_LEVEL_REG, &vl_VAL);
			vl_VAL = ((vl_VAL & 0x80) | (char)floatMV);
			abxxxx_write(CH_VOLT_LEVEL_REG, vl_VAL);

#ifndef HATSCONF_AB_LIB_8505
			/* write value in MainChInputCurr[3:0]  in MainChInputCurrLevel
			 * register @0x0B82  bit [7:4] */
			abxxxx_read(MAIN_CHARGER_CURRENT_CONTROL_REG, &vl_VAL);
			vl_VAL = ((vl_VAL & 0x0f) | ((CHARGE_param_array[IN_CHINCUR] << 4) & 0xf0));
			abxxxx_write(MAIN_CHARGER_CURRENT_CONTROL_REG, vl_VAL);
#endif
			/* write value in MainAutoInCurrLimEna in MainChCtrl2
			 * register @0x0B81   bit [2] */
			abxxxx_read(0x0B81, &vl_VAL);
			vl_VAL = ((vl_VAL & 0xfb) | ((CHARGE_param_array[IN_AUTOINCURENA] << 2) & 0x04));
			abxxxx_write(0x0B81, vl_VAL);

			/* Write "1" in bit BattOVVth in
			 * register BattOVV @0x0B55 bit 0 BattOVV Threshold = 4,75v */
			abxxxx_read(BATTERY_THRESHOLD_CONTROL_REG, &vl_VAL);
			vl_VAL = (vl_VAL | 0x01);
			abxxxx_write(BATTERY_THRESHOLD_CONTROL_REG, vl_VAL);

			/* watchdog */
			abxxxx_write(MAIN_WATCH_DOG_CONTROL_REG, 0x03);
			abxxxx_write(MAIN_WATCH_DOG_CONTROL_REG, 0x00);

			/* Write "1" in bit MainChEna  in
			 * register MainChCtrl1  @0x0B80  bit 0 Enable Charge */
			abxxxx_read(MAIN_CHARGER_CONTROL_REG, &vl_VAL);
			vl_VAL = (vl_VAL | 0x01);
			abxxxx_write(MAIN_CHARGER_CONTROL_REG, vl_VAL);

		}
		/* DISABLE */
		else {
			/* Write "0" in bit MainChEna in register
			 * MainChCtrl1 @0x0B80 bit 0 Enable Charge */
			abxxxx_read(MAIN_CHARGER_CONTROL_REG, &vl_VAL);
			vl_VAL = (vl_VAL & 0xfe);
			abxxxx_write(MAIN_CHARGER_CONTROL_REG, vl_VAL);
		}
		break;

	case ACT_SUSBCHINFO:
		/* ACTION ENABLE */
		if (CHARGE_param_array[elem->user_data] == CHARGE_ENABLE) {

			/* write in  ChOutputCurrentLevel[3:0]  in ChOutCurrLevel
			 * register @0x0B42 bit [3:0] */
			abxxxx_read(CHANNEL_OUTPUT_CURRENT_LEVEL_REG, &vl_VAL);
			vl_VAL = ((vl_VAL & 0xf0) | CHARGE_param_array[IN_CHOUTCUR]);
			abxxxx_write(CHANNEL_OUTPUT_CURRENT_LEVEL_REG, vl_VAL);

			/* write in  ChOutputCurrentLevelMax[3:0]  in ChOutCurrLevelMax
			 * register @0x0B43 bit [3:0] */
			abxxxx_read(0x0B43, &vl_VAL);
			vl_VAL = ((vl_VAL & 0xf0) | CHARGE_param_array[IN_CHOUTCURMAX]);
			abxxxx_write(0x0B43, vl_VAL);

			/* write value in ChVoltLevel[6:0] bit
			 * register ChVoltLevel @0x0B40 bits [6:0] */
			tatl1_04_Get_MatchValue(&CHARGE_param_array[IN_CHVOLTLEVEL], &floatMV, MT_CHVOLTLEVEL);
			abxxxx_read(CH_VOLT_LEVEL_REG, &vl_VAL);
			vl_VAL = ((vl_VAL & 0x80) | (char)floatMV);
			abxxxx_write(CH_VOLT_LEVEL_REG, vl_VAL);

			/* write value in VbusInCurrLim[3:0]  in UsbChCurrLevel
			 * register @0x0BC2   bit [7:4] */
			abxxxx_read(USB_CHARGER_CONTROL_3_REG, &vl_VAL);
			vl_VAL = ((vl_VAL & 0x0f) | ((CHARGE_param_array[IN_CHINCUR] << 4) & 0xf0));
			abxxxx_write(USB_CHARGER_CONTROL_3_REG, vl_VAL);

			/* write value in VbusAutoInCurrLimEna  in UsbChCtrl2
			 * register @0x0BC1    bit [2] */
			abxxxx_read(USB_CHARGER_CONTROL_2_REG, &vl_VAL);
			vl_VAL = ((vl_VAL & 0xfb) | ((CHARGE_param_array[IN_AUTOINCURENA] << 2) & 0x04));
			abxxxx_write(USB_CHARGER_CONTROL_2_REG, vl_VAL);

			/*Write "1" in bit BattOVVth in register BattOVV
			*@0x0B55 bit 0 BattOVV Threshold = 4,75v */
			abxxxx_read(BATTERY_THRESHOLD_CONTROL_REG, &vl_VAL);
			vl_VAL = (vl_VAL | 0x01);
			abxxxx_write(BATTERY_THRESHOLD_CONTROL_REG, vl_VAL);

			/* watchdog */
			abxxxx_write(MAIN_WATCH_DOG_CONTROL_REG, 0x03);
			abxxxx_write(MAIN_WATCH_DOG_CONTROL_REG, 0x00);

			/* Write "1" in bit UsbChEna in register UsbChCtrl1
			 * @0x0BC0   bit 0 Enable Charge */
			abxxxx_read(USB_CHARGER_CONTROL_REG, &vl_VAL);
			vl_VAL = (vl_VAL | 0x01);
			abxxxx_write(USB_CHARGER_CONTROL_REG, vl_VAL);
		}
		/* DISABLE */
		else {
			/* Write "0" in bit UsbChEna  in register UsbChCtrl1
			*@0x0BC0 bit 0 Enable Charge */
			abxxxx_read(USB_CHARGER_CONTROL_REG, &vl_VAL);
			vl_VAL = (vl_VAL & 0xfe);
			abxxxx_write(USB_CHARGER_CONTROL_REG, vl_VAL);
		}
		break;

	case ACT_CHWDAK:
		/* every 10s write 1 in bit ChargWDKick of register ChargWDCtrl
		 * @0x0581 bit 0 to disable Charge watchdog. */
		abxxxx_read(USB_PHY_MMANAGEMENT_REG, &vl_VAL);
		vl_VAL = (vl_VAL | 0x01);
		printf("tatl1_00ActCHARGE_exec: ChargWDKick WRITE 1 in bit 0 0x0581=0x%x\n", vl_VAL);
		abxxxx_write(USB_PHY_MMANAGEMENT_REG, vl_VAL);
		break;
	case ACT_GMAINCHSETT:

		/* read ChOutputCurrentLevel[3:0]  in ChOutCurrLevel
		 * register @0x0B42 bit [3:0] */
		abxxxx_read(CHANNEL_OUTPUT_CURRENT_LEVEL_REG, &vl_VAL);
		u8MV = (vl_VAL & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_AUTOMAINCHCURR);
		CHARGE_string_array[OUT_CHOUTCUR] = p_strMV;

		/* read ChOutputCurrentLevelMax[3:0]  in ChOutCurrLevelMax
		*register @0x0B43 bit [3:0] */
		abxxxx_read(0x0B43, &vl_VAL);
		u8MV = (vl_VAL & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_AUTOMAINCHCURR);
		CHARGE_string_array[OUT_CHOUTCURMAX] = p_strMV;

		/* read ChVoltLevel[6:0] in ChVoltLevel register @0x0B40 bit [6:0] */
		abxxxx_read(CH_VOLT_LEVEL_REG, &vl_VAL);
		vl_VAL = (vl_VAL & 0x7f);
		if (vl_VAL <= 0x16)
			floatMV = 3.5 + vl_VAL * 0.025;
		else if (vl_VAL > 0x16 && vl_VAL <= 0x4d)
			floatMV = (vl_VAL - 0x16) * 0.01 + 4.05;
		else if (vl_VAL == 0x4e)
			floatMV = 3.39;
		else
			floatMV = 3.5;

		CHARGE_float_array[OUT_CHVOLTLEVEL] = floatMV;

#ifndef HATSCONF_AB_LIB_8505
		/* read MainChInputCurr[3:0]  in MainChInputCurrLevel
		register @0x0B82 bit [7:4] */
		abxxxx_read(MAIN_CHARGER_CURRENT_CONTROL_REG, &vl_VAL);
		u8MV = ((vl_VAL >> 4) & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_AUTOMAINCHCURR);
		CHARGE_string_array[OUT_CHINCUR] = p_strMV;
#endif
		/* write value in MainAutoInCurrLimEna in MainChCtrl2
		*register @0x0B81   bit [2] */
		abxxxx_read(0x0B81, &vl_VAL);
		if ((vl_VAL | 0xfb) == 0xff)
			CHARGE_string_array[OUT_AUTOINCURENA] = "ENABLE";
		else
			CHARGE_string_array[OUT_AUTOINCURENA] = "DISABLE";

		break;

	case ACT_GUSBCHSETT:
		/* read ChOutputCurrentLevel[3:0]  in ChOutCurrLevel
		 * register @0x0B42 bit [3:0] */
		abxxxx_read(CHANNEL_OUTPUT_CURRENT_LEVEL_REG, &vl_VAL);
		u8MV = (vl_VAL & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_AUTOMAINCHCURR);
		CHARGE_string_array[OUT_CHOUTCUR] = p_strMV;

		/* read ChOutputCurrentLevelMax[3:0]
		*in ChOutCurrLevelMax register @0x0B43 bit [3:0] */
		abxxxx_read(0x0B43, &vl_VAL);
		u8MV = (vl_VAL & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_AUTOMAINCHCURR);
		CHARGE_string_array[OUT_CHOUTCURMAX] = p_strMV;

		/* read ChVoltLevel[6:0] in ChVoltLevel register @0x0B40 bit [6:0] */
		abxxxx_read(CH_VOLT_LEVEL_REG, &vl_VAL);
		vl_VAL = (vl_VAL & 0x7f);
		if (vl_VAL <= 0x16)
			floatMV = 3.5 + vl_VAL * 0.025;
		else if (vl_VAL > 0x16 && vl_VAL <= 0x4d)
			floatMV = (vl_VAL - 0x16) * 0.01 + 4.05;
		else if (vl_VAL == 0x4e)
			floatMV = 3.39;
		else
			floatMV = 3.5;

		CHARGE_float_array[OUT_CHVOLTLEVEL] = floatMV;

		/* read VbusInCurrLim[3:0]  in UsbChCurrLevel register
		 * @0x0BC2 bit [7:4] */
		abxxxx_read(USB_CHARGER_CONTROL_3_REG, &vl_VAL);
		u8MV = ((vl_VAL >> 4) & 0x0f);
		tatl1_04_Get_MatchValue(&u8MV, &p_strMV, MT_AUTOVBUSCHCURR);
		CHARGE_string_array[OUT_CHINCUR] = p_strMV;

		/* write value in VbusAutoInCurrLimEna in UsbChCtrl2
		 * register @0x0BC1 bit [2] */
		abxxxx_read(USB_CHARGER_CONTROL_2_REG, &vl_VAL);
		if ((vl_VAL | 0xfb) == 0xff)
			CHARGE_string_array[OUT_AUTOINCURENA] = "ENABLE";
		else
			CHARGE_string_array[OUT_AUTOINCURENA] = "DISABLE";

		break;

	default:
		printf("tatl1_00ActCHARGE_exec USERDATA not AVAILABLE\n");
		vl_Error = TAT_ERROR_CASE;
		break;
	}
	return vl_Error;
}

/*----------------------------------------------------------------------------*
 * Procedure    : tatl1_02ActCHARGEParam_Set
 *----------------------------------------------------------------------------*
 * Description  : Set the DTH Element Value Input Parameters
 *----------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *----------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_02ActCHARGEParam_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->type) {
	case DTH_TYPE_U8:
		CHARGE_param_array[elem->user_data] = *(u8 *) Value;
		break;
	case DTH_TYPE_S8:
		CHARGE_param_array[elem->user_data] = *(s8 *) Value;
		break;
	case DTH_TYPE_U16:
		CHARGE_param_array[elem->user_data] = *(u16 *) Value;
		break;
	case DTH_TYPE_S16:
		CHARGE_param_array[elem->user_data] = *(s16 *) Value;
		break;
	case DTH_TYPE_U32:
		CHARGE_param_array[elem->user_data] = *(u32 *) Value;
		break;
	case DTH_TYPE_S32:
		CHARGE_param_array[elem->user_data] = *(s32 *) Value;
		break;
	case DTH_TYPE_U64:
		CHARGE_param_array[elem->user_data] = *(u64 *) Value;
		break;
	case DTH_TYPE_S64:
		CHARGE_param_array[elem->user_data] = *(s64 *) Value;
		break;
	case DTH_TYPE_STRING:
		if ((char *)Value != NULL) {
			int len = strlen((char *)Value);
			if (CHARGE_string_array[elem->user_data] != NULL)
				free(CHARGE_string_array[elem->user_data]);

			CHARGE_string_array[elem->user_data] =
			    (char *)calloc(len + 1, sizeof(char));
			strncpy(CHARGE_string_array[elem->user_data], (char *)Value, len);
		} else {
			printf(" tatl1_02ActCHARGEParam_Set (char*)Value) ==NULL\n");
		}
		break;
	default:
		printf("tatl1_02ActCHARGEParam_Set TYPE not AVAILABLE\n");
		vl_Error = TAT_ERROR_CASE;
		break;
	}
	return vl_Error;

}

/*----------------------------------------------------------------------------*
 * Procedure    : tatl1_03ActCHARGEParam_Get
 *----------------------------------------------------------------------------*
 * Description  : Get the DTH ELement Value Output Parameters
 *----------------------------------------------------------------------------*
 * Parameter(s) : dth_element *, Structure of DTH element
 *		  void *, Value of Dth Element
 *----------------------------------------------------------------------------*
 * Return Value : u32, TAT error
 *---------------------------------------------------------------------------*/
int tatl1_03ActCHARGEParam_Get(struct dth_element *elem, void *Value)
{

	int vl_Error = TAT_ERROR_OFF;

	switch (elem->type) {

	case DTH_TYPE_U8:
		*((u8 *) Value) = CHARGE_param_array[elem->user_data];
		break;
	case DTH_TYPE_S8:
		*((s8 *) Value) = CHARGE_param_array[elem->user_data];
		break;
	case DTH_TYPE_U16:
		*((u16 *) Value) = CHARGE_param_array[elem->user_data];
		break;
	case DTH_TYPE_S16:
		*((s16 *) Value) = CHARGE_param_array[elem->user_data];
		break;
	case DTH_TYPE_U32:
		*((u32 *) Value) = CHARGE_param_array[elem->user_data];
		break;
	case DTH_TYPE_S32:
		*((s32 *) Value) = CHARGE_param_array[elem->user_data];
		break;
	case DTH_TYPE_U64:
		*((u64 *) Value) = CHARGE_param_array[elem->user_data];
		break;
	case DTH_TYPE_S64:
		*((s64 *) Value) = CHARGE_param_array[elem->user_data];
		break;
	case DTH_TYPE_FLOAT:
		*(float *)Value = CHARGE_float_array[elem->user_data];
		break;
	case DTH_TYPE_STRING:
		if (CHARGE_string_array[elem->user_data] != NULL) {
			strncpy((char *)Value,
				CHARGE_string_array[elem->user_data],
				strlen(CHARGE_string_array[elem->user_data]));
		} else {
			strncpy((char *)Value, "NULL", 5);
		}
		break;
	default:
		printf("tatl1_03ActCHARGEParam_Get TYPE not AVAILABLE\n");
		vl_Error = TAT_ERROR_CASE;
		break;
	}

	CHARGE_param_array[elem->user_data] = 0;

	return vl_Error;

}


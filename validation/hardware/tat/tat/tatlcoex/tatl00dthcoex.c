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

#include "tatlcoex.h"
#include "dthcoex.h"
#include <string.h>

void find_mmcblk(char *mc0_source, char *mc0_fstype,
				 char *mc2_source, char *mc2_fstype,
				 char *mc4_source, char *mc4_fstype)
{
	FILE *file_fstab;
	char line[100];
	char *pos;

	file_fstab=fopen ("/etc/fstab","r");
	while(fgets(line,100,file_fstab) != NULL){
		pos = strstr(line,"/mnt/SDCard");
		if (pos)
		{ /* found sdcard */
			sscanf(line,"%s %*s %s\n",mc0_source,mc0_fstype);
			if (!strcmp(mc0_fstype,"auto"))
			{
				/* auto not supported by mount function.
				Must set filesystem type to a value */
				strcpy(mc0_fstype,"vfat");
			}
			continue;
		}
		pos = strstr(line,"/system");
		if (pos)
		{ /* found mc2 */
			sscanf(line,"%s %*s %s\n",mc2_source,mc2_fstype);
			if (!strcmp(mc2_fstype,"auto"))
			{
				/* auto not supported by mount function.
				Must set filesystem type to a value */
				strcpy(mc2_fstype,"ext4");
			}
			continue;
		}
		pos = strstr(line,"/mnt/UserFS1");
		if (pos)
		{ /* found mc4 */
			sscanf(line,"%s %*s %s\n",mc4_source,mc4_fstype);
			if (!strcmp(mc4_fstype,"auto"))
			{
				/* auto not supported by mount function.
				Must set filesystem type to a value */
				strcpy(mc4_fstype,"ext4");
			}
			continue;
		}
	}
	fclose(file_fstab);
}

void dth_init_data()
{
	/* Init MC0. */

	find_mmcblk(block_name_mc0,block_type_mc0,
				block_name_mc2,block_type_mc2,
				block_name_mc4,block_type_mc4);

	tatl02_03Init_Mc_Data(&v_tatcoex_mc0_data, block_name_mc0, MC0_TARGET,
			      MC0_FILE, block_type_mc0);

	/* Init MC2. */
	tatl02_03Init_Mc_Data(&v_tatcoex_mc2_data, block_name_mc2, MC2_TARGET,
			      MC2_FILE, block_type_mc2);

	/* Init MC4. */
	tatl02_03Init_Mc_Data(&v_tatcoex_mc4_data, block_name_mc4, MC4_TARGET,
			      MC4_FILE, block_type_mc4);

	/* Init DSI0. */
	tatl03_03Init_Dsi_Ycbcr_Data(&v_tatcoex_dsi0_data, ACT_MAINDISPLAY_INIT,
				     POWER_MODE_ON, DSI0_FILE, DSI0_WIDTH,
				     DSI0_HEIGHT, DSI0_PICTURE_FORMAT,
				     DSI0_LOOP_MODE, DSI0_LOOP_RATE,
				     DSI0_FREQUENCY, DSI0_SCAN_MODE);

	/* Init DSI2. */
	tatl03_03Init_Dsi_Ycbcr_Data(&v_tatcoex_dsi2_data, ACT_HDMIDISPLAY_INIT,
				     POWER_MODE_ON, DSI2_FILE, DSI2_WIDTH,
				     DSI2_HEIGHT, DSI2_PICTURE_FORMAT,
				     DSI2_LOOP_MODE, DSI2_LOOP_RATE,
				     DSI2_FREQUENCY, DSI2_SCAN_MODE);

	/* Init PWM1. */
	tatl04_03Init_Pwm_Data(&v_tatcoex_pwm1_data, IN_MAINDIS_BLPWMVAL);

	/* Init CSI0. */
	tatl05_03Init_Csi_Data(&v_tatcoex_csi0_data, PRIMARY_SENSOR);

	/* Init CSI1. */
	tatl05_03Init_Csi_Data(&v_tatcoex_csi1_data, SECONDARY_SENSOR);

	/* Init I2C0. */
	tatl06_03Init_I2C_Data(&v_tatcoex_i2c0_data, I2C0_DEVICE_NAME,
			       I2C0_DEVICE_ADRESS, I2C0_REGISTER_ADRESS);

	/* Init I2C2. */
	tatl06_03Init_I2C_Data(&v_tatcoex_i2c2_data, I2C2_DEVICE_NAME,
			       I2C2_DEVICE_ADRESS, I2C2_REGISTER_ADRESS);

	/* Init I2C3. */
	tatl06_03Init_I2C_Data(&v_tatcoex_i2c3_data, I2C3_DEVICE_NAME,
			       I2C3_DEVICE_ADRESS, I2C3_REGISTER_ADRESS);

	/* Init Victim feature. */
	tatl07_03Init_Victim_Data();

	/* Init DMICs. */
	tatl08_03Init_Audio_Data(&v_tatcoex_mic_data, START_MIC, STOP_MIC);

	/* Init VIBs. */
	tatl08_03Init_Audio_Data(&v_tatcoex_vib_data, START_VIB, STOP_VIB);

	/* Init MSP1. */
	tatl08_03Init_Audio_Data(&v_tatcoex_msp1_data, START_MSP1, STOP_MSP1);

	/* Init MSP2. */
	tatl08_03Init_Audio_Data(&v_tatcoex_msp2_data, START_MSP2, STOP_MSP2);

	/* Init MOD_I2C. */
	tatl09_03Init_ModI2C_Data();

	/* Init APE_I2C. */
	tatl10_03Init_ApeI2C_Data();

	/* Init CG2900. */
	tatl12_03Conn_Init_Data(&v_tatcoex_cg2900_data);

	/* Init CW1200. */
	tatl12_03Conn_Init_Data(&v_tatcoex_cw1200_data);

	/* Init BT IP. */
	tatl12_03Conn_Init_Data(&v_tatcoex_btip_data);

	/* Init FM IP. */
	tatl12_03Conn_Init_Data(&v_tatcoex_fmip_data);

#ifdef CONNECTIVITY_PANEL
	/* Init GPS IP. */
	tatl12_03Conn_Init_GPS_Data(&v_tatcoex_gpsip_data);
	/* Init CGVER Data. */
	tatl12_03Conn_Init_CGVER_Data(&v_tatcoex_cgver_data);
	/* Init BLEDUT DUT. */
	tatl12_03Conn_Init_BLEDUT_Data(&v_tatcoex_bledut_data);
#endif

	/* Init UART0. */
	tatl12_03Conn_Init_Data(&v_tatcoex_uart0_data);

	/* Init I2S. */
	tatl12_03Conn_Init_Data(&v_tatcoex_i2s_data);

	/* Init MSP0. */
	tatl12_03Conn_Init_Data(&v_tatcoex_msp0_data);

	/* Init MC1. */
	tatl12_03Conn_Init_Data(&v_tatcoex_mc1_data);

	/* Init BT TX. */
	tatl12_03Conn_Init_BTTX_Data(&v_tatcoex_bttx_data);

	/* Init FM TX. */
	tatl12_03Conn_Init_FMTX_Data(&v_tatcoex_fmtx_data);

	/* Init BT DUT. */
	tatl12_03Conn_Init_Data(&v_tatcoex_btdut_data);

	/* Init WLAN TX Data. */
	tatl12_03Conn_Init_WLANTX_Data(&v_tatcoex_wlantx_data);

	/* Init WLAN RX Data. */
	tatl12_03Conn_Init_WLANRX_Data(&v_tatcoex_wlanrx_data);

}

int dth_init_service()
{
	int result = 0;

	dth_init_data();

	result = tatlcoex_register_dth_struct();

	return result;
}

/* Function tide to I2C buses management */
/****************************************/
int DthCoex_I2C_Set(struct dth_element *elem, void *Value)
{
	return tatl06_02I2C_Set(elem, Value);
}

int DthCoex_I2C_Get(struct dth_element *elem, void *Value)
{
	return tatl06_01I2C_Get(elem, Value);
}

int DthCoex_I2C_Exec(struct dth_element *elem)
{
	return tatl06_00I2C_Exec(elem);
}

/* Function tide to DSI and YCBCR buses management */
/***************************************************/
int DthCoex_Display_Set(struct dth_element *elem, void *Value)
{
	return tatl03_02Display_Set(elem, Value);
}

int DthCoex_Display_Get(struct dth_element *elem, void *Value)
{
	return tatl03_01Display_Get(elem, Value);
}

int DthCoex_Display_Exec(struct dth_element *elem)
{
	return tatl03_00Display_Exec(elem);
}

/* Function tide to PWM buses management */
/****************************************/
int DthCoex_Pwm_Set(struct dth_element *elem, void *Value)
{
	return tatl04_02Pwm_Set(elem, Value);
}

int DthCoex_Pwm_Get(struct dth_element *elem, void *Value)
{
	return tatl04_01Pwm_Get(elem, Value);
}

int DthCoex_Pwm_Exec(struct dth_element *elem)
{
	return tatl04_00Pwm_Exec(elem);
}

/* Function tide to CSI buses management */
/****************************************/
int DthCoex_Sensor_Set(struct dth_element *elem, void *Value)
{
	return tatl05_02Sensor_Set(elem, Value);
}

int DthCoex_Sensor_Get(struct dth_element *elem, void *Value)
{
	return tatl05_01Sensor_Get(elem, Value);
}

int DthCoex_Sensor_Exec(struct dth_element *elem)
{
	return tatl05_00Sensor_Exec(elem);
}

/* Function tide to MSP, VIB and DMIC buses management */
/******************************************************/
int DthCoex_Audio_Set(struct dth_element *elem, void *Value)
{
	return tatl08_02Audio_Set(elem, Value);
}

int DthCoex_Audio_Get(struct dth_element *elem, void *Value)
{
	return tatl08_01Audio_Get(elem, Value);
}

int DthCoex_Audio_Exec(struct dth_element *elem)
{
	return tatl08_00Audio_Exec(elem);
}

/* Function tide to MC buses management */
/******************************************************/
int DthCoex_Memory_Set(struct dth_element *elem, void *Value)
{
	return tatl02_02Memory_Set(elem, Value);
}

int DthCoex_Memory_Get(struct dth_element *elem, void *Value)
{
	return tatl02_01Memory_Get(elem, Value);
}

int DthCoex_Memory_Exec(struct dth_element *elem)
{
	return tatl02_00Memory_Exec(elem);
}

/* Function tide to MOD_I2C buses management */
/******************************************************/
int DthCoex_ModI2C_Set(struct dth_element *elem, void *Value)
{
	return tatl09_02ModI2C_Set(elem, Value);
}

int DthCoex_ModI2C_Get(struct dth_element *elem, void *Value)
{
	return tatl09_01ModI2C_Get(elem, Value);
}

int DthCoex_ModI2C_Exec(struct dth_element *elem)
{
	return tatl09_00ModI2C_Exec(elem);
}

/* Function tide to APE_I2C buses management */
/******************************************************/
int DthCoex_ApeI2C_Set(struct dth_element *elem, void *Value)
{
	return tatl10_02ApeI2C_Set(elem, Value);
}

int DthCoex_ApeI2C_Get(struct dth_element *elem, void *Value)
{
	return tatl10_01ApeI2C_Get(elem, Value);
}

int DthCoex_ApeI2C_Exec(struct dth_element *elem)
{
	return tatl10_00ApeI2C_Exec(elem);
}

/* Function tide to Connectivity buses management */
/******************************************************/
int DthCoex_Conn_Set(struct dth_element *elem, void *Value)
{
	return tatl12_02Conn_Set(elem, Value);
}

int DthCoex_Conn_Get(struct dth_element *elem, void *Value)
{
	return tatl12_01Conn_Get(elem, Value);
}

int DthCoex_Conn_Exec(struct dth_element *elem)
{
	return tatl12_00Conn_Exec(elem);
}

/* Function tide to victim management */
/******************************************************/
int DthCoex_Victim_Set(struct dth_element *elem, void *Value)
{
	return tatl07_02Victim_Set(elem, Value);
}

int DthCoex_Victim_Get(struct dth_element *elem, void *Value)
{
	return tatl07_01Victim_Get(elem, Value);
}

int DthCoex_Victim_Exec(struct dth_element *elem)
{
	return tatl07_00Victim_Exec(elem);
}

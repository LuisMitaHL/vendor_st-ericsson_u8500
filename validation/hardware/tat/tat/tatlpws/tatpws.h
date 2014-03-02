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

#ifndef TATPWS_H_
#define TATPWS_H_

#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>		/* close */

#include <dthsrvhelper/dthsrvhelper.h>
/*#include <libdb.h>*/

#ifdef HATSCONF_AB_LIB_8505
#include "ab9540_GPIO_types.h"
#define GPIO_PAD_CONTROL_1_REG GPIO_PAD_SELECTION_1_REG
#define GPIO_PAD_CONTROL_2_REG GPIO_PAD_SELECTION_2_REG
#define GPIO_PAD_CONTROL_3_REG GPIO_PAD_SELECTION_3_REG
#define GPIO_PAD_CONTROL_4_REG GPIO_PAD_SELECTION_4_REG
#define GPIO_PAD_CONTROL_5_REG GPIO_PAD_SELECTION_5_REG
#define GPIO_PAD_CONTROL_6_REG GPIO_PAD_SELECTION_6_REG

#define GPIO_SET_OUPUT_1_REG GPIO_OUTPUT_CONTROL_1_REG
#define GPIO_SET_OUPUT_2_REG GPIO_OUTPUT_CONTROL_2_REG
#define GPIO_SET_OUPUT_3_REG GPIO_OUTPUT_CONTROL_3_REG
#define GPIO_SET_OUPUT_4_REG GPIO_OUTPUT_CONTROL_4_REG
#define GPIO_SET_OUPUT_5_REG GPIO_OUTPUT_CONTROL_5_REG
#define GPIO_SET_OUPUT_6_REG GPIO_OUTPUT_CONTROL_6_REG

#define GPIO_PULL_DOWN_1_REG GPIO_PULL_CONTROL_1_REG
#define GPIO_PULL_DOWN_2_REG GPIO_PULL_CONTROL_2_REG
#define GPIO_PULL_DOWN_3_REG GPIO_PULL_CONTROL_3_REG
#define GPIO_PULL_DOWN_4_REG GPIO_PULL_CONTROL_4_REG
#define GPIO_PULL_DOWN_5_REG GPIO_PULL_CONTROL_5_REG
#define GPIO_PULL_DOWN_6_REG GPIO_PULL_CONTROL_6_REG

#else
#include "ab8500_GPIO_types.h"
#endif

#include <libab.h>
#include <libdb.h>
#include "hats_power.h"
#include "tatpwsdth.h"

#define TATPWS_NO_ERROR 0
#define TATPWS_ERROR_BAD_VALUE EINVAL
#define TATPWS_ERROR_BAD_REQ EBADMSG
#define TATPWS_ERROR_EXEC ENOEXEC

#define TATPWS_GPIO_DAT		0
#define TATPWS_GPIO_DIR     4

#define TATPWS_MAX_GPIO     (TATPWS_GPIO_FWMSC + 1)
#define TATPWS_NB_BANKS		9
#define TATPWS_NB_GPIO	    (TATPWS_NB_BANKS * TATPWS_MAX_GPIO)
#define TATPWS_NB_AB_GPIOS  (TATPWS_GPIO_ALT_FCT + 1)
#define TATPWS_SEP          ";"

#define TATPWS_IO_FILE "/var/local/tat/tatlpws/DBgpios.csv"
#define TATPWS_IO_FILE_FOR_AB "/var/local/tat/tatlpws/ABgpios.csv"

typedef struct Reg {
	unsigned int v_Address;
	unsigned int v_Value;
	unsigned char v_PreventReading;
	unsigned char v_PreventWriting;
} GpioReg;

int dth_init_service();

void tatpws1_GPIO_Settings_Init();
int tatpws1_GPIO_Read(GpioReg *Reg, int Nb_Regs);
int tatpws1_GPIO_Write(GpioReg *Reg, int Nb_Regs);
int tatpws1_GPIO_Settings_Result(struct dth_element *elem, void *Value);
int tatpws1_GPIO_Settings_Execute(struct dth_element *elem);
int tatpws1_GPIO_Settings_SetMode(struct dth_element *elem, void *Value);
int tatpws1_GPIO_Settings_Set(struct dth_element *elem, void *Value);
int tatpws1_GPIO_Settings_Get(struct dth_element *elem, void *Value);

int tatpws2_Set_Mode(struct dth_element *elem, void *Value);
int tatpws2_SetMode_Execute(struct dth_element *elem);
int tatpws2_SetMode_Result(struct dth_element *elem, void *Value);
int tatpws2_Get_Status(struct dth_element *elem, void *Value);
int tatpws2_Set_HwAcc(struct dth_element *elem, void *Value);

int tatpws3_SetOpp_Result(struct dth_element *elem, void *Value);
int tatpws3_Set_Opp(struct dth_element *elem, void *Value);
int tatpws3_SetOpp_Execute(struct dth_element *elem);
int tatpws3_Get_Status(struct dth_element *elem, void *Value);

void tatpws4_ABGPIO_Settings_Init();
int tatpws4_ABGPIO_Read(GpioReg *Reg, int Nb_Regs);
int tatpws4_ABGPIO_Write(GpioReg *Reg, int Nb_Regs);
int tatpws4_ABGPIO_Settings_Execute(struct dth_element *elem);
int tatpws4_ABGPIO_Settings_SetMode(struct dth_element *elem,
					   void *Value);
int tatpws4_ABGPIO_Settings_Set(struct dth_element *elem, void *Value);
int tatpws4_ABGPIO_Settings_Get(struct dth_element *elem, void *Value);
int tatpws4_ABGPIO_Settings_Result(struct dth_element *elem, void *Value);

int tatpws5_Reset_Execute(struct dth_element *elem);
int tatpws5_Get_Status(struct dth_element *elem, void *Value);
int tatpws5_Set_ResetMode(struct dth_element *elem, void *Value);

#endif /*TATPWS_H_ */


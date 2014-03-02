/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cli.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <los/api/los_api.h>

#include "ite_darky_box_cmd.h"

#ifdef __ARM_THINK
	#include "ssc.h"
	STSSC_Handle_t i2DarkBoxHandle = 0;
#endif

#ifdef __ARM_LINUX
	#include <linux/i2c.h>
	#include <linux/i2c-dev.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif

#ifdef __ARM_SYMBIAN
#    include "i2c-symbian_osal.h"
#    include <e32std.h>
#    include <studebug.h>
static RI2C i2c;
#endif

#define dark_box_lux_min 0
#define dark_box_lux_max 3000
#define dark_box_color_min 0
#define dark_box_color_max 255
#define dark_box_chart_min 0
#define dark_box_chart_max 5
#define dark_box_shuttlepos_min 0
#define dark_box_shuttlepos_max 550

#define dark_box_sleep_after_writting 1000

#define ITE_DB_TRING_SIZE 32


//=========================================================================
/**
 *
 */
//=========================================================================
void ITE_Darky_Box_Sleep(int timeToSleep)
{
#ifdef __ARM_SYMBIAN
	User::After(timeToSleep*1000);
#else
 			// sleep is needed in order to give hand to other threads
			LOS_Sleep(timeToSleep);
#endif
}

//=========================================================================
/**
 *
 */
//=========================================================================
t_uint16 ITE_Convert_To_Int16(char STR[6])
{
	t_uint16 tmp;

	if (STR[1] == 'x')
	{
	   sscanf(STR,"%x",(int*)&tmp);
	   //_snscanf(STR,6, "%x",(int*)&tmp); // error: _snscanf? was not declared in this sco
	}
	else
	{
	   tmp = atoi(STR);
	}
	return(tmp);
}

//=========================================================================
/**
 *
 */
//=========================================================================
void ITE_initBoardForDarkBox(void)
{
#ifdef __EPOC32__
	TInt err=KErrNone;
	err = RI2C::LoadLogicalDevice();
	STU_ASSERT_ALWAYS(err==KErrNone);

	err=i2c.Open();
	STU_ASSERT_ALWAYS(err==KErrNone);

	err=i2c.Init(1, 0x54);	//address of Darky box
	STU_ASSERT_ALWAYS(err==KErrNone);
#endif

#ifdef __ARM_THINK
	STSSC_InitParams_t i2cparams;
	t_uint32 gpioBaseAdresse = 0;
	ST_ErrorCode_t	ApiError = ST_NO_ERROR;

	gpioBaseAdresse = LOS_Remap((t_los_physical_address)0x8012e000, 0x1000, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
	i2cparams.SDAPIOBase = gpioBaseAdresse;
	i2cparams.SDAPIOBit = 17;
	i2cparams.SCKPIOBase = gpioBaseAdresse;
	i2cparams.SCKPIOBit = 16;
	i2cparams.BaudRate = 40000;

	ApiError = STSSC_Init(&i2cparams, &i2DarkBoxHandle);
    if ( ApiError != ST_NO_ERROR )
	{
		CLI_disp_error((char*)"Error in %s\n", __FUNCTION__);
		return;
	}
#endif

}

//=========================================================================
/**
 *
 */
//=========================================================================
void ITE_freeBoardForDarkBox(void)
{
#ifdef __ARM_SYMBIAN
	i2c.Close();
	TInt err = RI2C::FreeLogicalDevice();
	STU_ASSERT_ALWAYS(err==KErrNone);
#endif

#ifdef __ARM_THINK
	STSSC_Term(i2DarkBoxHandle);
#endif
}

//=========================================================================
/**
 *
 */
//=========================================================================
t_uint16 ITE_Read_Darky_Box_Reg(t_uint16 index, t_uint16 *errorReturn)
{
    t_uint8			tab_i2c_data[2];
    t_uint16		res = 0;
#ifdef __ARM_THINK
    t_uint32		Written,Read;
    ST_ErrorCode_t	ApiError = ST_NO_ERROR;
#endif

#ifdef __ARM_LINUX
	char i2cname[ITE_DB_TRING_SIZE];
	int i2cfd = -1;
#endif

#ifndef __ARM_SYMBIAN
    t_uint8 i2c_reg = index & 0xff;
#endif	
	
	if(errorReturn != NULL)
	{
		*errorReturn = 0;
	}
	else
	{
		CLI_disp_error((char*)"Error in %s\n",__FUNCTION__);
		return res;
	}

#ifdef __ARM_THINK
    ApiError = STSSC_Write(i2DarkBoxHandle, 0x54, &i2c_reg, 1, &Written,1000,FALSE);
    if ( ApiError == ST_NO_ERROR )
    {
        ApiError = STSSC_Read(i2DarkBoxHandle, 0x55, tab_i2c_data, 2, &Read, 1000);
		if ( ApiError != ST_NO_ERROR )
		{
			CLI_disp_error((char*)"Error in %s\n", __FUNCTION__);
			return res;
		}
    }
	else
	{
		CLI_disp_error((char*)"Error in %s\n", __FUNCTION__);
		return res;
	}
#endif

#ifdef __ARM_LINUX
    //Open I2C1 device
	strncpy(i2cname,"/dev/i2c-1", ITE_DB_TRING_SIZE-1);
	i2cfd = open(i2cname, O_RDWR);
	if (i2cfd < 0) {
		LOS_Log("Error: failed to open %s.\n", i2cname);
	return 0;
	}
	//Set sensor address : 0x54 --> 0x2A due to a 1bit right-shift with thi I2C interface
	if (ioctl(i2cfd, I2C_SLAVE, 0x2A) < 0) {
		//LOS_Log("Error: failed to access slave address 0x20\n");
		return 0;
	}
	if (write(i2cfd, &i2c_reg, 1) !=1) {
		//LOS_Log("Error: failed to write I2C commands\n");
	}
	if (read(i2cfd, tab_i2c_data, 2) != 2) {
		//LOS_Log("Error: failed to read I2C respond\n");
	}

	close(i2cfd);
#endif

#ifdef __ARM_SYMBIAN
	TInt err=i2c.ReadRegister16(index, (TUint16*)&tab_i2c_data[0]);
	STU_ASSERT_ALWAYS(err==KErrNone);
#endif

    res = ((tab_i2c_data[0]&0xff) << 8)  + (tab_i2c_data[1]&0xff);

	*errorReturn = 1;
	return res;
}

//=========================================================================
/**
 *
 */
//=========================================================================
// return 0 in case of failure, 1 in case of succes
t_uint16 ITE_Read_Until_Darky_Box(t_uint16 index, t_uint16 expectedValue)
{
	t_uint16 result = 0;
	t_uint16 errorReturn = 0;
	t_uint16 nbTry = 0;

	while(1)
	{
		nbTry++;
		if(expectedValue == ITE_Read_Darky_Box_Reg(index, &errorReturn))
		{
			result = 1;
			CLI_disp_msg((char*)"Read_Until_Darky_Box good result after %d tries\n", nbTry);
			goto label_end;
		}
		if( 0 == errorReturn)
		{
			CLI_disp_error((char*)"Error in %s\n", __FUNCTION__);
			goto label_end;
		}
		if( nbTry == 5)
		{
			goto label_error;
		}
	}
label_error:

	CLI_disp_msg((char*)"Read_Until_Darky_Box after %d tries\n", nbTry);

label_end :
	return result;
}

//=========================================================================
/**
 *
 */
//=========================================================================
void ITE_Write_Darky_Box_Reg(t_uint16 index, t_uint16 data)
{
#ifndef __ARM_SYMBIAN
    t_uint8             tab_i2c_data[3];
    tab_i2c_data[0]     = (index&0xff);
    tab_i2c_data[1]     = (data>>8)     ;
    tab_i2c_data[2]     = (data&0xFF)   ;
#endif

#ifdef __ARM_THINK
    ApiError = STSSC_Write(i2DarkBoxHandle, 0x54, tab_i2c_data, 3, &Written, 1000, FALSE);
	if ( ApiError != ST_NO_ERROR )
	{
		CLI_disp_error((char*)"Error in %s\n", __FUNCTION__);
		return;
	}
#endif

#ifdef __ARM_LINUX
	char i2cname[ITE_DB_TRING_SIZE];
	int i2cfd = -1;
	//Open I2C1 device
	strncpy(i2cname,"/dev/i2c-1", ITE_DB_TRING_SIZE-1);
	i2cfd = open(i2cname, O_RDWR);
	if (i2cfd < 0) {
		LOS_Log("Error: failed to open %s.\n", i2cname);
	return;
	}
	//Set sensor address : 0x54 --> 0x2A due to a 1bit right-shift with thi I2C interface
	if (ioctl(i2cfd, I2C_SLAVE, 0x2A) < 0) {
		//LOS_Log("Error: failed to access slave address 0x20\n");
		return;
	}
	if (write(i2cfd, tab_i2c_data, 3) !=3) {
		//LOS_Log("Error: failed to write I2C commands\n");
	}
	close(i2cfd);
#endif

#ifdef __ARM_SYMBIAN
	//Convert endianness
	data = ((data&0xFF00)>>8) | ((data&0x00FF)<<8);
	TInt err=i2c.WriteRegister16(index, data);
	STU_ASSERT_ALWAYS(err==KErrNone);
#endif
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_init_cmd
   PURPOSE  : darky Box initialisation
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dark_box_init_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_init_cmd);
	}
	else
	{
		if (a_nb_args == 1)
		{
			ITE_initBoardForDarkBox();
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_free_cmd
   PURPOSE  : darky Box free
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dark_box_free_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_free_cmd);
	}
	else
	{
		if (a_nb_args == 1)
		{
			ITE_freeBoardForDarkBox();
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_reset_cmd
   PURPOSE  : darky Box reset
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dark_box_reset_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_reset_cmd);
	}
	else
	{
		if (a_nb_args == 1)
		{
			// all leds off
			LOS_Log("Darkbox : all leds off : start\n");
			ITE_Write_Darky_Box_Reg(0x2,0);
			ITE_Write_Darky_Box_Reg(0x4,0);
			ITE_Write_Darky_Box_Reg(0x6,0);
			ITE_Write_Darky_Box_Reg(0x8,0);
			ITE_Write_Darky_Box_Reg(0xa,0);
			LOS_Log("Darkbox : all leds off : end\n");
			  // shuttle at pos 0
			LOS_Log("Darkbox : shuttle at pos 0 : start\n");
			ITE_Write_Darky_Box_Reg(0x10,0);
			LOS_Log("Darkbox : shuttle at pos 0 : end\n");
			if( 0 == ITE_Read_Until_Darky_Box(0x12, 0xa1) )
			{
				CLI_disp_error((char*)"Error in %s\n", __FUNCTION__);
				return CMD_COMPLETE;
			}
			LOS_Log("Darkbox : shuttle at pos 0: check\n");

			  // all chart up
			LOS_Log("Darkbox : all chart up: start\n");
			ITE_Write_Darky_Box_Reg(0x14,0);
			LOS_Log("Darkbox : all chart up: end\n");
			if( 0 == ITE_Read_Until_Darky_Box(0x16, 0xffff) )
			{
				CLI_disp_error((char*)"Error in %s\n", __FUNCTION__);
				return CMD_COMPLETE;
			}
			LOS_Log("Darkbox : all chart up: check\n");

			LOS_Log("Darkbox : reset: end\n");
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_read_cmd
   PURPOSE  : darky box read
   ------------------------------------------------------------------------ */
/*
CMD_COMPLETION C_ite_dark_box_read_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_read_cmd);
	}
	else
	{
		if (a_nb_args == 2)
		{
			t_uint16 errorReturn = 0;
			t_uint16 data = 0;
			t_uint16 tmp_1 = 0;
			tmp_1 = ITE_Convert_To_Int16(ap_args[1]);
			data = ITE_Read_Darky_Box_Reg(tmp_1, &errorReturn);
			if( 0 == errorReturn)
			{
				CLI_disp_error((char*)"Error in fucntion %s\n", __FUNCTION__);
			}
			else
			{
				CLI_disp_msg((char*)"Darky_Box data = %d \n", data);
			}
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}
*/
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_write_cmd
   PURPOSE  : darky Box write
   ------------------------------------------------------------------------ */
/*
CMD_COMPLETION C_ite_dark_box_write_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_write_cmd);
	}
	else
	{
		if (a_nb_args == 3)
		{
			t_uint16 tmp_1 = 0, tmp_2 = 0;
			tmp_1 = ITE_Convert_To_Int16(ap_args[1]);
			tmp_2 = ITE_Convert_To_Int16(ap_args[2]);
			ITE_Write_Darky_Box_Reg(tmp_1,tmp_2);
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}
*/

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_lux_cmd
   PURPOSE  : darky Box lux setting
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dark_box_lux_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_lux_cmd);
	}
	else
	{
		if (a_nb_args == 2)
		{
			int value = ITE_Convert_To_Int16(ap_args[1]);
			if( (value >= dark_box_lux_min) && (value <= dark_box_lux_max) )
			{
				ITE_Write_Darky_Box_Reg(0xa, value);
				ITE_Darky_Box_Sleep(dark_box_sleep_after_writting);
			}
			else
			{
				CLI_disp_error((char*)"Error, param is not in range [%d ... %d]\n", dark_box_lux_min, dark_box_lux_max);
			}
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_color_cmd
   PURPOSE  : darky Box color setting
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dark_box_color_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_color_cmd);
	}
	else
	{
		if (a_nb_args == 4)
		{
			int valueR = ITE_Convert_To_Int16(ap_args[1]);
			int valueG = ITE_Convert_To_Int16(ap_args[2]);
			int valueB = ITE_Convert_To_Int16(ap_args[3]);

			if(
				(valueR >= dark_box_color_min) && (valueR <= dark_box_color_max) &&
				(valueG >= dark_box_color_min) && (valueG <= dark_box_color_max) &&
				(valueB >= dark_box_color_min) && (valueB <= dark_box_color_max)
				)
			{
				ITE_Write_Darky_Box_Reg(0x2, valueR);
				ITE_Write_Darky_Box_Reg(0x4, valueG);
				ITE_Write_Darky_Box_Reg(0x6, valueB);
				ITE_Darky_Box_Sleep(dark_box_sleep_after_writting);
			}
			else
			{
				CLI_disp_error((char*)"Error, param is not in range [%d ... %d]\n", dark_box_color_min, dark_box_color_max);
			}
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_chartpos_cmd
   PURPOSE  : darky Box chart positionning
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dark_box_chartpos_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_chartpos_cmd);
	}
	else
	{
		if (a_nb_args == 2)
		{
			t_uint16 tmp = 0;
			int value = ITE_Convert_To_Int16(ap_args[1]);
			if( (value >= dark_box_chart_min) && (value <= dark_box_chart_max) )
			{
				ITE_Write_Darky_Box_Reg(0x14, value);
				tmp = ( (0xffff << (1 + value) ) + 1 );
				if( 0 == ITE_Read_Until_Darky_Box(0x16, tmp) )
				{
					CLI_disp_error((char*)"Error Read_Until_Darky_Box\n");
					return CMD_COMPLETE;
				}
			}
			else
			{
				CLI_disp_error((char*)"Error, param is not in range [%d ... %d]\n", dark_box_chart_min, dark_box_chart_max);
			}
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dark_box_shuttlepos_cmd
   PURPOSE  : darky Box shuttle positionning
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dark_box_shuttlepos_cmd(int a_nb_args, char ** ap_args)
{
	if (ap_args[1]==NULL){ap_args[1]=(char*)"";} // needed for linux strcmp
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_dark_box_shuttlepos_cmd);
	}
	else
	{
		if (a_nb_args == 2)
		{
			t_uint16 tmp = 0;
			int value = ITE_Convert_To_Int16(ap_args[1]);
			if( (value >= dark_box_shuttlepos_min) && (value <= dark_box_shuttlepos_max) )
			{
				ITE_Write_Darky_Box_Reg(0x10, value);
				ITE_Darky_Box_Sleep(dark_box_sleep_after_writting);
				if (ITE_Convert_To_Int16(ap_args[1])==0)
				{
					tmp = 0xa1;
				}
				else
				{
					tmp = 1;
				}
				if( 0 == ITE_Read_Until_Darky_Box(0x12, tmp) )
				{
					CLI_disp_error((char*)"Error Read_Until_Darky_Box\n");
					return CMD_COMPLETE;
				}
			}
			else
			{
				CLI_disp_error((char*)"Error, param is not in range [%d ... %d]\n", dark_box_shuttlepos_min, dark_box_shuttlepos_max);
			}
		}
		else
		{
			CLI_disp_error((char*)"Error Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}


t_cmd_list ite_cmd_list_darky_box[] =
{
	{ "dbox_init", C_ite_dark_box_init_cmd,
      (char*)"dbox_init: dbox_init\n"
    },
	{ "dbox_free", C_ite_dark_box_free_cmd,
      (char*)"dbox_free: dbox_free\n"
    },
	{ "dbox_reset", C_ite_dark_box_reset_cmd,
      (char*)"dbox_reset: dbox_reset\n"
    },
/*	{ "dbox_read", C_ite_dark_box_read_cmd,
      (char*)"dbox_read: dbox_read  <index> <B>\n"
    },
	{ "dbox_write", C_ite_dark_box_write_cmd,
      (char*)"dbox_write: dbox_write <dbox index> <data> <B> \n"
    },*/
	{ "dbox_lux", C_ite_dark_box_lux_cmd,
      (char*)"dbox_lux: dbox_lux <[0..3000]>\n"
    },
	{ "dbox_color", C_ite_dark_box_color_cmd,
      (char*)"dbox_color: dbox_color R G B (value: [0..255])\n"
    },
	{ "dbox_chart", C_ite_dark_box_chartpos_cmd,
      (char*)"dbox_chart: dbox_chart <[0..5]>\n"
    },
	{ "dbox_shuttle", C_ite_dark_box_shuttlepos_cmd,
      (char*)"dbox_shuttle: dbox_shuttle <[0..550]>\n"
    },
};

void Init_Darky_Box_ITECmdList(void)
{
    if(CLI_register_interface((char*)"DARKY_BOX_ITE_CMB", sizeof(ite_cmd_list_darky_box)/(sizeof(ite_cmd_list_darky_box[0])), ite_cmd_list_darky_box, 1))
	{
		LOS_Log("CLI_register_interface DARKY_BOX_ITE_CMB\n");
	}
	else
	{
		CLI_disp_error((char*)"Error CLI_register_interface DARKY_BOX_ITE_CMB\n");
	}
}


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


#ifdef __ARM_LINUX
#ifndef ANDROID 
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#else
#define I2C_SLAVE	0x0703	/* Use this slave address */
#endif
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h> //__NO_WARNING__
#endif

#define DarkBox_sleep_after_writting 1000

void ITE_DarkyBoxSleep(int timeToSleep)
{
	LOS_Sleep(timeToSleep);
}

t_uint16 ITE_ConvertToInt16(char STR[6])
{
volatile t_uint16 tmp;

if (STR[1] == 'x')
 {
   sscanf(STR,"%x",(int*)&tmp);
 }
else
 {
   tmp = CLI_atoi(STR);
 }
 return(tmp);
}

/*###########################################################*/
t_uint16 ITE_ReadDarkyBoxReg(t_uint16 index)
{	
    //__NO_WARNING__ t_uint32		Written,Read;
    
    t_uint8		i2c_reg;
    t_uint8		tab_i2c_data[2];
    t_uint16		res;
    char i2cname[32];
    int i2cfd = -1;
    
    i2c_reg = index & 0xff;

    //Open I2C1 device
    strcpy(i2cname,"/dev/i2c-1");
    i2cfd = open(i2cname, O_RDWR);
    if (i2cfd < 0) 
    {
	LOS_Log("Error: failed to open %s.\n", i2cname);
	return -1; //__NO_WARNING__
    }
    //Set sensor address : 0x54 --> 0x2A due to a 1bit right-shift with thi I2C interface
    if (ioctl(i2cfd, I2C_SLAVE, 0x2A) < 0) 
    {
	 LOS_Log("Error: failed to access slave address 0x20\n");
	return -1; //__NO_WARNING__
    }
    if (write(i2cfd, &i2c_reg, 1) !=1) 
    {
	//LOS_Log("Error: failed to write I2C commands\n");
    }
    if (read(i2cfd, tab_i2c_data, 2) != 2) 
    {
	//LOS_Log("Error: failed to read I2C respond\n");
    }
	
    close(i2cfd);

    res = ((tab_i2c_data[0]&0xff) << 8)  + (tab_i2c_data[1]&0xff);
    return(res);
}

/*################################################################*/
void ITE_WriteDarkyBoxReg(t_uint16 index, t_uint16 data)
{	
    t_uint8             tab_i2c_data[3];
    //__NO_WARNING__ t_uint32            Written;
    char i2cname[32];
    int i2cfd = -1;
    
    tab_i2c_data[0]     = (index&0xff);
    tab_i2c_data[1]     = (data>>8)     ;
    tab_i2c_data[2]     = (data&0xFF)   ;
	
    //Open I2C1 device
    strcpy(i2cname,"/dev/i2c-1");
    i2cfd = open(i2cname, O_RDWR);
    if (i2cfd < 0)
     {
        LOS_Log("Error: failed to open %s.\n", i2cname);
	return;
     }
     //Set sensor address : 0x54 --> 0x2A due to a 1bit right-shift with thi I2C interface
     if (ioctl(i2cfd, I2C_SLAVE, 0x2A) < 0) 
     {
	 //LOS_Log("Error: failed to access slave address 0x20\n");
	 return;
     }
     if (write(i2cfd, tab_i2c_data, 3) !=3) 
     {
	//LOS_Log("Error: failed to write I2C commands\n");
     }

     close(i2cfd);

}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_darkbox_init_cmd
   PURPOSE  : darky Box initialisation  
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_darkbox_init_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";	//For Linux, strcmp doesn't support NULL parameter
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_darkbox_init_cmd);
	}
	else
	{
		if (a_nb_args == 1)
		{       
			// all leds off
			ITE_WriteDarkyBoxReg(0x2,0);
			ITE_WriteDarkyBoxReg(0x4,0);
			ITE_WriteDarkyBoxReg(0x6,0);
			ITE_WriteDarkyBoxReg(0x8,0);
			ITE_WriteDarkyBoxReg(0xa,0);
			  // shuttle at pos 0
			ITE_WriteDarkyBoxReg(0x10,0);
			while ( 0xa1 != ITE_ReadDarkyBoxReg(0x12));
			  // all chart up
			ITE_WriteDarkyBoxReg(0x14,0);
			while ( 0xffff != ITE_ReadDarkyBoxReg(0x16));
		}
		else
		{
			CLI_disp_error("Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_darkbox_read_cmd
   PURPOSE  : darky box read  
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_darkbox_read_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";	//For Linux, strcmp doesn't support NULL parameter
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_darkbox_read_cmd);
	}
	else
	{
		if (a_nb_args == 2)
		{
			CLI_disp_msg("darkyBox data = %d \n",ITE_ReadDarkyBoxReg(ITE_ConvertToInt16(ap_args[1])));
		}
		else
		{
			CLI_disp_error("Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_darkbox_write_cmd
   PURPOSE  : darky Box write  
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_darkbox_write_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";	//For Linux, strcmp doesn't support NULL parameter
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_darkbox_write_cmd);
	}
	else
	{
		if (a_nb_args == 3)
		{
			ITE_WriteDarkyBoxReg(ITE_ConvertToInt16(ap_args[1]),ITE_ConvertToInt16(ap_args[2]));
		}
		else
		{
			CLI_disp_error("Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_darkbox_lux_cmd
   PURPOSE  : darky Box lux setting  
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_darkbox_lux_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";	//For Linux, strcmp doesn't support NULL parameter
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_darkbox_lux_cmd);
	}
	else
	{
		if (a_nb_args == 2)
		{
			ITE_WriteDarkyBoxReg(0xa,ITE_ConvertToInt16(ap_args[1]));
			ITE_DarkyBoxSleep(DarkBox_sleep_after_writting);
		}
		else
		{
			CLI_disp_error("Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_darkbox_color_cmd
   PURPOSE  : darky Box lux setting  
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_darkbox_color_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";	//For Linux, strcmp doesn't support NULL parameter
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_darkbox_color_cmd);
	}
	else
	{
		if (a_nb_args == 4)
		{
			ITE_WriteDarkyBoxReg(0x2,ITE_ConvertToInt16(ap_args[1]));
			ITE_WriteDarkyBoxReg(0x4,ITE_ConvertToInt16(ap_args[2]));
			ITE_WriteDarkyBoxReg(0x6,ITE_ConvertToInt16(ap_args[3]));
			ITE_DarkyBoxSleep(DarkBox_sleep_after_writting);
		}
		else
		{
			CLI_disp_error("Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_darkbox_chartpos_cmd
   PURPOSE  : darky Box chart positioning  
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_darkbox_chartpos_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";	//For Linux, strcmp doesn't support NULL parameter
	t_uint16 tmp;	

	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_darkbox_chartpos_cmd);
	}
	else
	{
		if (a_nb_args == 2)
		{
			ITE_WriteDarkyBoxReg(0x14,ITE_ConvertToInt16(ap_args[1]));
			tmp = ((0xffff << (1+ITE_ConvertToInt16(ap_args[1])))+1);
			while (  tmp != ITE_ReadDarkyBoxReg(0x16));
		}
		else
		{
			CLI_disp_error("Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_darkbox_shuttlepos_cmd
   PURPOSE  : darky Box shuttle positioning  
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_darkbox_shuttlepos_cmd(int a_nb_args, char ** ap_args)
{if (ap_args[1]==NULL) ap_args[1]="";	//For Linux, strcmp doesn't support NULL parameter
	t_uint16 tmp;
	if (0== strcmp(ap_args[1],"help"))
	{
		CLI_disp_usage(C_ite_darkbox_shuttlepos_cmd);
	}
	else
	{
		if (a_nb_args == 2)
		{
			ITE_WriteDarkyBoxReg(0x10,ITE_ConvertToInt16(ap_args[1]));
			ITE_DarkyBoxSleep(DarkBox_sleep_after_writting);
			if (ITE_ConvertToInt16(ap_args[1])==0)
				tmp = 0xa1;
			else
				tmp = 1;
			while (  tmp != ITE_ReadDarkyBoxReg(0x12));
		}
		else
		{
			CLI_disp_error("Not correct command arguments\n");
		}
	}
	return CMD_COMPLETE;
}


t_cmd_list ite_cmd_list_darkybox[] =
{
	{ "dboxinit", C_ite_darkbox_init_cmd,
      "dboxinit: dboxinit\n"
    },
	{ "dboxread", C_ite_darkbox_read_cmd,
      "dbox: dboxread  <index> <B>\n"
    },
	{ "dboxwrite", C_ite_darkbox_write_cmd,
      "dboxwrite: dboxwrite <dbox index> <data> <B> \n"
    },
	{ "dboxlux", C_ite_darkbox_lux_cmd,
      "dboxlux: dboxlux <[0..3000]>\n"
    },
	{ "dboxcolor", C_ite_darkbox_color_cmd,
      "dboxcolor: dboxcolor R G B (value: [0..255])\n"
    },
	{ "dboxchart", C_ite_darkbox_chartpos_cmd,
      "dboxchart: chart <[0..5]>\n"
    },
	{ "dboxshuttle", C_ite_darkbox_shuttlepos_cmd,
      "dboxshuttle: shuttle <[0..550]>\n"
    },
};

void Init_DarkyBox_ITECmdList(void)
{
    CLI_register_interface("DARKYBOX_ITE_CMB", sizeof(ite_cmd_list_darkybox)/(sizeof(ite_cmd_list_darkybox[0])), ite_cmd_list_darkybox, 1);
}


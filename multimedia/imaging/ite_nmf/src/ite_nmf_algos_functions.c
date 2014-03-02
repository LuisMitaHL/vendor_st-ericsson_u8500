/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_nmf_algos_functions.h"
#include "ite_nmf_standard_functions.h"

#include "ite_testenv_utils.h"

#include "ite_boardinfo.h"
#include "ite_platform.h"
#include "VhcElementDefs.h"

//for NMF
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_init.h"
//#include "ite_alloc.h"
#include "ite_display.h"
#include "ite_event.h"
#include "ite_pageelements.h"
#include "ite_grab.h"
#include "ite_vpip.h"
#include "ite_sia_buffer.h"
#include "ite_host2sensor.h"
#include "ite_sensorinfo.h"


#include <cm/inc/cm_macros.h>
#include <los/api/los_api.h>
#include "ite_main.h"
#include "sia.h"
#include "pictor_full.h"
#include "sia_register_fct.h"
#include "hi_register_acces.h"
#include "ite_sia_bootcmd.h"

void ITE_NMF_gammacontrol1enablepe(){
   int isp_state;

   isp_state = ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0);

   if (isp_state == HostInterfaceHighLevelState_e_RUNNING){ 
	STOPVPIP();
   	ITE_writePE(CE_GammaControl_1_e_GammaCurve_Byte0,GammaCurve_Custom);
   	STARTVPIP();
   }else {
   	ITE_writePE(CE_GammaControl_1_e_GammaCurve_Byte0,GammaCurve_Custom);
   }

}

void //__NO_WARNING__
ITE_NMF_gammacontrol1enablehw(){
   	ISP_R_W(ISP_CE1_FLEXTF_SHARP_ENABLE,0x01);
	ISP_R_W(ISP_CE1_FLEXTF_UNSHARP_ENABLE,0x01);
}

void //__NO_WARNING__
ITE_NMF_gammacontrol0enablepe(){
   int isp_state;

   isp_state = ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0);
   if (isp_state == HostInterfaceHighLevelState_e_RUNNING){ 
	STOPVPIP();
   	ITE_writePE(CE_GammaControl_0_e_GammaCurve_Byte0,GammaCurve_Custom);
   	STARTVPIP();
   }else {
	ITE_writePE(CE_GammaControl_0_e_GammaCurve_Byte0,GammaCurve_Custom);
   }
}

void //__NO_WARNING__
ITE_NMF_gammacontrol0enablehw(){
  	ISP_R_W(ISP_CE0_FLEXTF_SHARP_ENABLE,0x01);
	ISP_R_W(ISP_CE0_FLEXTF_UNSHARP_ENABLE,0x01);
}

void //__NO_WARNING__
ITE_NMF_gammacontrol1disablepe(){
   int isp_state;

   isp_state = ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0);
   if (isp_state == HostInterfaceHighLevelState_e_RUNNING){ 
	STOPVPIP();
   	ITE_writePE(CE_GammaControl_1_e_GammaCurve_Byte0,GammaCurve_Disable);
   	STARTVPIP();
   }else {
   	ITE_writePE(CE_GammaControl_1_e_GammaCurve_Byte0,GammaCurve_Disable);
        ITE_ActivePipe(GRBPID_PIPE_LR, 1);
	STARTVPIP();
	STOPVPIP();
	ITE_ActivePipe(GRBPID_PIPE_LR,0);
   }
}

void //__NO_WARNING__
ITE_NMF_gammacontrol1disablehw(){
   	ISP_R_W(ISP_CE1_FLEXTF_SHARP_ENABLE,0x00);
	ISP_R_W(ISP_CE1_FLEXTF_UNSHARP_ENABLE,0x00);
}

void //__NO_WARNING__
ITE_NMF_gammacontrol0disablepe(){
   int isp_state;

   isp_state = ITE_readPE(HostInterface_Status_e_HostInterfaceHighLevelState_Byte0);
   if (isp_state == HostInterfaceHighLevelState_e_RUNNING){ 
	STOPVPIP();
   	ITE_writePE(CE_GammaControl_1_e_GammaCurve_Byte0,GammaCurve_Disable);
   	STARTVPIP();
   }else {
   	ITE_writePE(CE_GammaControl_1_e_GammaCurve_Byte0,GammaCurve_Disable);
        ITE_ActivePipe(GRBPID_PIPE_HR, 1);
	STARTVPIP();
	STOPVPIP();
	ITE_ActivePipe(GRBPID_PIPE_HR, 0);
   }
}

void //__NO_WARNING__
ITE_NMF_gammacontrol0disablehw(){
   	ISP_R_W(ISP_CE0_FLEXTF_SHARP_ENABLE,0x00);
	ISP_R_W(ISP_CE0_FLEXTF_UNSHARP_ENABLE,0x00);
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_gammacontrolhw_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_gammacontrolhw_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: gammacontrolhw <enable/disable> <LR/HR/>\n");}
 else {
   if (a_nb_args == 3)
      {
        if (0== strcmp(ap_args[1],"enable") && 0== strcmp(ap_args[2],"LR"))
	      	ITE_NMF_gammacontrol1enablehw();
        if (0== strcmp(ap_args[1],"enable") && 0== strcmp(ap_args[2],"HR"))
	      	ITE_NMF_gammacontrol0enablehw();
        if (0== strcmp(ap_args[1],"disable")&& 0== strcmp(ap_args[2],"LR"))
		ITE_NMF_gammacontrol1disablehw();
	if (0== strcmp(ap_args[1],"disable")&& 0== strcmp(ap_args[2],"HR"))
		ITE_NMF_gammacontrol0disablehw();
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_gammacontrol_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_gammacontrolpe_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: gammacontrol <enable/disable> <LR/HR/>\n");}
 else {
   if (a_nb_args == 3)
      {
        if (0== strcmp(ap_args[1],"enable") && 0== strcmp(ap_args[2],"LR"))
	      	ITE_NMF_gammacontrol1enablepe();
        if (0== strcmp(ap_args[1],"enable") && 0== strcmp(ap_args[2],"HR"))
	      	ITE_NMF_gammacontrol0enablepe();
        if (0== strcmp(ap_args[1],"disable")&& 0== strcmp(ap_args[2],"LR"))
		ITE_NMF_gammacontrol1disablepe();
	if (0== strcmp(ap_args[1],"disable")&& 0== strcmp(ap_args[2],"HR"))
		ITE_NMF_gammacontrol0disablepe();
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

void //__NO_WARNING__
ITE_NMF_linearcontrolenablehw(){
  	ISP_R_W(ISP_FLEXTF_LINEAR_ENABLE,0x01);
}

void //__NO_WARNING__
ITE_NMF_linearcontroldisablehw(){
   	ISP_R_W(ISP_FLEXTF_LINEAR_ENABLE,0x00);
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_linearcontrolhw_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_linearcontrolhw_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: linearcontrolhw <enable/disable>\n");}
 else {
   if (a_nb_args == 2)
      {
        if (0== strcmp(ap_args[1],"enable"))
	      	ITE_NMF_linearcontrolenablehw();
        if (0== strcmp(ap_args[1],"disable"))
		ITE_NMF_linearcontroldisablehw();
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

void //__NO_WARNING__
ITE_NMF_linearcontrolenablepe(){
	ITE_writePE(SDL_Control_e_SDLMode_Control_Byte0, SDL_Custom);
      while (ITE_readPE(SDL_Status_e_SDLMode_Status_Byte0) != SDL_Custom){};
}

void //__NO_WARNING__
ITE_NMF_linearcontroldisablepe(){
	ITE_writePE(SDL_Control_e_SDLMode_Control_Byte0, SDL_Disable);
        while (ITE_readPE(SDL_Status_e_SDLMode_Status_Byte0) != SDL_Disable){};
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_linearcontrol_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_linearcontrolpe_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: linearcontrol <enable/disable>\n");}
 else {
   if (a_nb_args == 2)
      {
        if (0== strcmp(ap_args[1],"enable"))
	      	ITE_NMF_linearcontrolenablepe();
        if (0== strcmp(ap_args[1],"disable"))
		ITE_NMF_linearcontroldisablepe();
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

void //__NO_WARNING__
ITE_NMF_dustercontrolenablehw(){
  	ISP_R_W(ISP_DUSTER_ENABLE,0x01);
}

void //__NO_WARNING__
ITE_NMF_dustercontroldisablehw(){
   	ISP_R_W(ISP_DUSTER_ENABLE,0x00);
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_dustercontrolhw_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_dustercontrolhw_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: dustercontrolhw <enable/disable>\n");}
 else {
   if (a_nb_args == 2)
      {
        if (0== strcmp(ap_args[1],"enable"))
	      	ITE_NMF_dustercontrolenablehw();
        if (0== strcmp(ap_args[1],"disable"))
		ITE_NMF_dustercontroldisablehw();
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

void //__NO_WARNING__
ITE_NMF_gridironcontrolenablehw(){
  	ISP_R_W(ISP_GRIDIRON_ENABLE,0x01);
}

void //__NO_WARNING__
ITE_NMF_gridironcontroldisablehw(){
   	ISP_R_W(ISP_GRIDIRON_ENABLE,0x00);
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_gridironcontrolhw_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_gridironcontrolhw_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: gridironcontrolhw <enable/disable>\n");}
 else {
   if (a_nb_args == 2)
      {
        if (0== strcmp(ap_args[1],"enable"))
	      	ITE_NMF_gridironcontrolenablehw();
        if (0== strcmp(ap_args[1],"disable"))
		ITE_NMF_gridironcontroldisablehw();
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

void ITE_NMF_gridironcontrolenablepe(){
	ITE_writePE(GridironControl_e_Flag_Enable_Byte0,Flag_e_TRUE);
}

void ITE_NMF_gridironcontroldisablepe(){
	ITE_writePE(GridironControl_e_Flag_Enable_Byte0,Flag_e_FALSE);
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_gridironcontrolpe_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_gridironcontrolpe_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: gridironcontrolpe <enable/disable>\n");}
 else {
   if (a_nb_args == 2)
      {
        if (0== strcmp(ap_args[1],"enable"))
	      	ITE_NMF_gridironcontrolenablepe();
        if (0== strcmp(ap_args[1],"disable"))
		ITE_NMF_gridironcontroldisablepe();
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


void ITE_NMF_gammacopy1(t_los_logical_address addr, int hw ){
    int i;
    t_uint16 *r_gamma_table= (t_uint16 *)addr;
    t_uint16 *g_gamma_table= (t_uint16 *)addr+CE1_SHARP_MEM_LUT_RED_ELT_DIMENSION;
    t_uint16 *b_gamma_table= (t_uint16 *)addr+CE1_SHARP_MEM_LUT_RED_ELT_DIMENSION+CE1_SHARP_MEM_LUT_GREEN_ELT_DIMENSION;

    for(i=CE1_SHARP_MEM_LUT_GREEN_ELT_OFFSET; i<(CE1_SHARP_MEM_LUT_GREEN_ELT_OFFSET + CE1_SHARP_MEM_LUT_GREEN_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*g_gamma_table++) );} 	//Vert
    for(i=CE1_SHARP_MEM_LUT_RED_ELT_OFFSET; i<(CE1_SHARP_MEM_LUT_RED_ELT_OFFSET + CE1_SHARP_MEM_LUT_RED_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*r_gamma_table++) );} 	//Bleu
    for(i=CE1_SHARP_MEM_LUT_BLUE_ELT_OFFSET; i<(CE1_SHARP_MEM_LUT_BLUE_ELT_OFFSET + CE1_SHARP_MEM_LUT_BLUE_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i), (*b_gamma_table++) );} 	//Rouge

    r_gamma_table= (t_uint16 *)addr;
    g_gamma_table= (t_uint16 *)addr+CE1_UNSHARP_MEM_LUT_RED_ELT_DIMENSION;
    b_gamma_table= (t_uint16 *)addr+CE1_UNSHARP_MEM_LUT_RED_ELT_DIMENSION+CE1_UNSHARP_MEM_LUT_GREEN_ELT_DIMENSION;
    for(i=CE1_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET; i<(CE1_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET + CE1_UNSHARP_MEM_LUT_GREEN_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*g_gamma_table++) );} 
    for(i=CE1_UNSHARP_MEM_LUT_RED_ELT_OFFSET; i<(CE1_UNSHARP_MEM_LUT_RED_ELT_OFFSET + CE1_UNSHARP_MEM_LUT_RED_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*r_gamma_table++) );} 
    for(i=CE1_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET; i<(CE1_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET + CE1_UNSHARP_MEM_LUT_BLUE_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*b_gamma_table++) );} 
    if (hw){
	ISP_R_W(ISP_CE1_FLEXTF_SHARP_LAST_ELT_GREEN_GIR,0x3ff);
	ISP_R_W(ISP_CE1_FLEXTF_SHARP_LAST_ELT_RED,0x3ff);
	ISP_R_W(ISP_CE1_FLEXTF_SHARP_LAST_ELT_BLUE,0x3ff);
	ISP_R_W(ISP_CE1_FLEXTF_UNSHARP_LAST_ELT_GREEN_GIR,0x3ff);
	ISP_R_W(ISP_CE1_FLEXTF_UNSHARP_LAST_ELT_RED,0x3ff);
	ISP_R_W(ISP_CE1_FLEXTF_UNSHARP_LAST_ELT_BLUE,0x3ff);
   	ISP_R_W(ISP_CE1_FLEXTF_SHARP_PIXELIN_SHIFT,0x00);
	ISP_R_W(ISP_CE1_FLEXTF_UNSHARP_PIXELIN_SHIFT,0x00);
    } else {
    ITE_writePE(CE_GammaControl_1_u8_GammaPixelInShift_Sharp_Byte0,0x00);
    ITE_writePE(CE_GammaControl_1_u8_GammaPixelInShift_UnSharp_Byte0,0x00);
    }
}
void ITE_NMF_gammacopy0(t_los_logical_address addr, int hw ){
    int i;
    t_uint16 *r_gamma_table= (t_uint16 *)addr;
    t_uint16 *g_gamma_table= (t_uint16 *)addr+CE0_SHARP_MEM_LUT_RED_ELT_DIMENSION;
    t_uint16 *b_gamma_table= (t_uint16 *)addr+CE0_SHARP_MEM_LUT_RED_ELT_DIMENSION+CE0_SHARP_MEM_LUT_GREEN_ELT_DIMENSION;

    for(i=CE0_SHARP_MEM_LUT_GREEN_ELT_OFFSET; i<(CE0_SHARP_MEM_LUT_GREEN_ELT_OFFSET + CE0_SHARP_MEM_LUT_GREEN_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*g_gamma_table++) );} 
    for(i=CE0_SHARP_MEM_LUT_RED_ELT_OFFSET; i<(CE0_SHARP_MEM_LUT_RED_ELT_OFFSET + CE0_SHARP_MEM_LUT_RED_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*r_gamma_table++) );} 
    for(i=CE0_SHARP_MEM_LUT_BLUE_ELT_OFFSET; i<(CE0_SHARP_MEM_LUT_BLUE_ELT_OFFSET + CE0_SHARP_MEM_LUT_BLUE_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i), (*b_gamma_table++) );} 

    r_gamma_table= (t_uint16 *)addr;
    g_gamma_table= (t_uint16 *)addr+CE0_UNSHARP_MEM_LUT_RED_ELT_DIMENSION;
    b_gamma_table= (t_uint16 *)addr+CE0_UNSHARP_MEM_LUT_RED_ELT_DIMENSION+CE0_UNSHARP_MEM_LUT_GREEN_ELT_DIMENSION;
    for(i=CE0_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET; i<(CE0_UNSHARP_MEM_LUT_GREEN_ELT_OFFSET + CE0_UNSHARP_MEM_LUT_GREEN_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*g_gamma_table++) );} 
    for(i=CE0_UNSHARP_MEM_LUT_RED_ELT_OFFSET; i<(CE0_UNSHARP_MEM_LUT_RED_ELT_OFFSET + CE0_UNSHARP_MEM_LUT_RED_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*r_gamma_table++) );} 
    for(i=CE0_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET; i<(CE0_UNSHARP_MEM_LUT_BLUE_ELT_OFFSET + CE0_UNSHARP_MEM_LUT_BLUE_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*b_gamma_table++) );} 
   if (hw){
	ISP_R_W(ISP_CE0_FLEXTF_SHARP_LAST_ELT_GREEN_GIR,0x3ff);
	ISP_R_W(ISP_CE0_FLEXTF_SHARP_LAST_ELT_RED,0x3ff);
	ISP_R_W(ISP_CE0_FLEXTF_SHARP_LAST_ELT_BLUE,0x3ff);
	ISP_R_W(ISP_CE0_FLEXTF_UNSHARP_LAST_ELT_GREEN_GIR,0x3ff);
	ISP_R_W(ISP_CE0_FLEXTF_UNSHARP_LAST_ELT_RED,0x3ff);
	ISP_R_W(ISP_CE0_FLEXTF_UNSHARP_LAST_ELT_BLUE,0x3ff);
  	ISP_R_W(ISP_CE0_FLEXTF_SHARP_PIXELIN_SHIFT,0x00);
	ISP_R_W(ISP_CE0_FLEXTF_UNSHARP_PIXELIN_SHIFT,0x00);
    } else {
    ITE_writePE(CE_GammaControl_0_u8_GammaPixelInShift_Sharp_Byte0,0x00);
    ITE_writePE(CE_GammaControl_0_u8_GammaPixelInShift_UnSharp_Byte0,0x00);
    }
}

int ITE_NMF_gammaload (char *filename, char *pipe,int hw){
#define GAMMA_SIZE_BYTE ((CE0_SHARP_MEM_LUT_RED_ELT_DIMENSION+CE0_SHARP_MEM_LUT_GREEN_ELT_DIMENSION+CE0_SHARP_MEM_LUT_BLUE_ELT_DIMENSION)*2)

   t_los_file *fd;
   size_t read;
   size_t size;
   t_los_memory_handle mem_handle;
   t_los_logical_address logicalAddr;
   //t_los_physical_address physicalAddr;
 
   fd = LOS_fopen(filename, "rb");
   if (fd == NULL)
   {
      LOS_Log("unable to open gamma file %s\n", (void*)filename, NULL, NULL, NULL, NULL, NULL);
      return 1;
   }

   LOS_fseek(fd, 0, LOS_SEEK_END);
   size = (size_t)LOS_ftell(fd);
   LOS_fseek(fd, 0, LOS_SEEK_SET);
   LOS_Log("gamma size is %i bytes\n",  (void*)size, NULL, NULL, NULL, NULL, NULL);
   if (size != GAMMA_SIZE_BYTE){
	  LOS_Log("Size %i bytes instead of 768\n", (void*)size, NULL, NULL, NULL, NULL, NULL);
          LOS_fclose(fd);
          return 1;
   }

   mem_handle=LOS_Alloc(size, 4, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
   logicalAddr = LOS_GetLogicalAddress(mem_handle) ;
   //physicalAddr = ILOS_GetPhysicalAddress(mem_handle) ;

   read = LOS_fread((void *)logicalAddr, 1, size, fd);
   if (read != size)
    {
          LOS_Log("Read %i bytes instead of %i\n", (void*)read, (void*)size, NULL, NULL, NULL, NULL);
	  LOS_Free(mem_handle);
          LOS_fclose(fd);
          return 1;
   }
   LOS_fclose(fd);

  
   if (0== strcmp(pipe,"LR"))
    	ITE_NMF_gammacopy1(logicalAddr,hw);
   if (0== strcmp(pipe,"HR"))
      	ITE_NMF_gammacopy0(logicalAddr,hw);

   LOS_Free(mem_handle);

   return 0;
}

int ITE_NMF_gammaloadnips (char *filename, char *pipe){
#define GAMMA_SIZE_BYTE ((CE0_SHARP_MEM_LUT_RED_ELT_DIMENSION+CE0_SHARP_MEM_LUT_GREEN_ELT_DIMENSION+CE0_SHARP_MEM_LUT_BLUE_ELT_DIMENSION)*2)

   t_los_file *fd;
   size_t read;
   size_t size;
   t_los_memory_handle mem_handle,nips_mem_handle;
   t_los_logical_address logicalAddr,nips_logicalAddr;
   //t_los_physical_address physicalAddr;
   t_uint8 *nips_buffer;
   t_uint16 *buffer;
   float value;
   int i;

   fd = LOS_fopen(filename, "rb");
   if (fd == NULL)
   {
      LOS_Log("unable to open gamma file %s\n", (void*)filename, NULL, NULL, NULL, NULL, NULL);
      return 1;
   }

   LOS_fseek(fd, 0, LOS_SEEK_END);
   size = (size_t)LOS_ftell(fd);
   LOS_fseek(fd, 0, LOS_SEEK_SET);
   LOS_Log("gamma size is %i bytes\n",  (void*)size, NULL, NULL, NULL, NULL, NULL);
   if (size != 256){
	  LOS_Log("Size %i bytes instead of 256\n", (void*)size, NULL, NULL, NULL, NULL, NULL);
          LOS_fclose(fd);
          return 1;
   }

   nips_mem_handle=LOS_Alloc(size, 4, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
   nips_logicalAddr = LOS_GetLogicalAddress(nips_mem_handle) ;
   nips_buffer= (t_uint8 *)nips_logicalAddr;
   //physicalAddr = ILOS_GetPhysicalAddress(mem_handle) ;

   read = LOS_fread((void *)nips_logicalAddr, 1, size, fd);
   if (read != size)
    {
          LOS_Log("Read %i bytes instead of %i\n", (void*)read, (void*)size, NULL, NULL, NULL, NULL);
	  LOS_Free(nips_mem_handle);
          LOS_fclose(fd);
          return 1;
   }
   LOS_fclose(fd);
   
   mem_handle=LOS_Alloc(GAMMA_SIZE_BYTE, 4, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
   logicalAddr = LOS_GetLogicalAddress(mem_handle) ;
   buffer = (t_uint16 *)logicalAddr;

   for (i=0;i<127;i++){
	value = ((*nips_buffer+i*2)+(*nips_buffer+i*2+1))*((float)1023/(float)510);
   	*(buffer+i)= (t_uint16)value;
	*(buffer+i+128) = (t_uint16)value;
	*(buffer+i+256) = (t_uint16)value;
    }
   
   if (0== strcmp(pipe,"LR"))
    	ITE_NMF_gammacopy1(logicalAddr,1);
   if (0== strcmp(pipe,"HR"))
      	ITE_NMF_gammacopy0(logicalAddr,1);

   LOS_Free(mem_handle);
   LOS_Free(nips_mem_handle);

   return 0;
}


/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_gammaloadhw_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_gammaloadhw_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: gammaloadhw <FILENAME> <LR/HR/>\n");}
 else {
   if (a_nb_args == 3)
      {
          ITE_NMF_gammaload(ap_args[1],ap_args[2],1);
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_gammaload_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_gammaloadpe_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: gammaload <FILENAME> <LR/HR/>\n");}
 else {
   if (a_nb_args == 3)
      {
          ITE_NMF_gammaload(ap_args[1],ap_args[2],0);
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_gammaloadnips_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_gammaloadnips_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: gammaloadnips <FILENAME> <LR/HR/>\n");}
 else {
   if (a_nb_args == 3)
      {
          ITE_NMF_gammaloadnips(ap_args[1],ap_args[2]);
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}


int ITE_NMF_linearload (char *filename, int hw){
#define LINEAR_SIZE_BYTE ((LINEAR_MEM_LUT_GINR_ELT_DIMENSION+LINEAR_MEM_LUT_RED_ELT_DIMENSION+LINEAR_MEM_LUT_BLUE_ELT_DIMENSION+LINEAR_MEM_LUT_GINB_ELT_DIMENSION)*2)

   t_los_file *fd;
   size_t read;
   size_t size;
   t_los_memory_handle mem_handle;
   t_los_logical_address logicalAddr;
//   t_los_physical_address physicalAddr;
   int i;
   t_uint16 *gr_gamma_table;
   t_uint16 *r_gamma_table;
   t_uint16 *b_gamma_table;
   t_uint16 *gb_gamma_table;

   fd = LOS_fopen(filename, "rb");
   if (fd == NULL)
   {
      LOS_Log("unable to open linearisation file %s\n", (void*)filename, NULL, NULL, NULL, NULL, NULL);
      return 1;
   }

   LOS_fseek(fd, 0, LOS_SEEK_END);
   size = (size_t)LOS_ftell(fd);
   LOS_fseek(fd, 0, LOS_SEEK_SET);
   LOS_Log("linear size is %i bytes\n",  (void*)size, NULL, NULL, NULL, NULL, NULL);
   if (size != LINEAR_SIZE_BYTE){
	  LOS_Log("Size %i bytes instead of 2048\n", (void*)size, NULL, NULL, NULL, NULL, NULL);
          LOS_fclose(fd);
          return 1;
   }

   mem_handle=LOS_Alloc(size, 4, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
   logicalAddr = LOS_GetLogicalAddress(mem_handle) ;
//   physicalAddr = ILOS_GetPhysicalAddress(mem_handle) ;

   read = LOS_fread((void *)logicalAddr, 1, size, fd);
   if (read != size)
    {
          LOS_Log("Read %i bytes instead of %i\n", (void*)read, (void*)size, NULL, NULL, NULL, NULL);
	  LOS_Free(mem_handle);
          LOS_fclose(fd);
          return 1;
   }
   LOS_fclose(fd);

   gr_gamma_table= (t_uint16 *)logicalAddr;
   r_gamma_table= (t_uint16 *)logicalAddr+LINEAR_MEM_LUT_GINR_ELT_DIMENSION;
   b_gamma_table= (t_uint16 *)logicalAddr+LINEAR_MEM_LUT_GINR_ELT_DIMENSION+LINEAR_MEM_LUT_RED_ELT_DIMENSION;
   gb_gamma_table= (t_uint16 *)logicalAddr+LINEAR_MEM_LUT_GINR_ELT_DIMENSION+LINEAR_MEM_LUT_RED_ELT_DIMENSION+LINEAR_MEM_LUT_BLUE_ELT_DIMENSION;

   for(i=LINEAR_MEM_LUT_GINR_ELT_OFFSET; i<(LINEAR_MEM_LUT_GINR_ELT_OFFSET + LINEAR_MEM_LUT_GINR_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*gr_gamma_table++) );}
   for(i=LINEAR_MEM_LUT_RED_ELT_OFFSET; i<(LINEAR_MEM_LUT_RED_ELT_OFFSET + LINEAR_MEM_LUT_RED_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*r_gamma_table++) );}
   for(i=LINEAR_MEM_LUT_BLUE_ELT_OFFSET; i<(LINEAR_MEM_LUT_BLUE_ELT_OFFSET + LINEAR_MEM_LUT_BLUE_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*b_gamma_table++) );}
   for(i=LINEAR_MEM_LUT_GINB_ELT_OFFSET; i<(LINEAR_MEM_LUT_GINB_ELT_OFFSET + LINEAR_MEM_LUT_GINB_ELT_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*gb_gamma_table++) );}

   if (hw){
	ISP_R_W(ISP_FLEXTF_LINEAR_LAST_ELT_GREEN_GIR,0xfff);
	ISP_R_W(ISP_FLEXTF_LINEAR_LAST_ELT_RED,0xfff);
	ISP_R_W(ISP_FLEXTF_LINEAR_LAST_ELT_BLUE,0xfff);
	ISP_R_W(ISP_FLEXTF_LINEAR_LAST_ELT_GIB,0xfff);
	ISP_R_W(ISP_FLEXTF_LINEAR_PIXELIN_SHIFT,0x00);
   } else {
	/*NO PE interface */
   }

   LOS_Free(mem_handle);

   return 0;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_linearloadhw_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_linearloadhw_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: linearloadhw <FILENAME>\n");}
 else {
   if (a_nb_args == 2)
      {
          ITE_NMF_linearload(ap_args[1],1);
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_linearloadpe_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_linearloadpe_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: linearloadpe <FILENAME>\n");}
 else {
   if (a_nb_args == 2)
      {
          ITE_NMF_linearload(ap_args[1],0);
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

int ITE_NMF_dusterload (char *filename, int hw){
#define DUSTER_SIZE_BYTE (DUSTER_MEM_POSITION_DEFECT_DIMENSION*4)

   t_los_file *fd;
   size_t read;
   size_t size;
   t_los_memory_handle mem_handle;
   t_los_logical_address logicalAddr;
 //  t_los_physical_address physicalAddr;
   int i;
   t_uint32 *duster_table;

   fd = LOS_fopen(filename, "rb");
   if (fd == NULL)
   {
      LOS_Log("unable to open duster file %s\n", (void*)filename, NULL, NULL, NULL, NULL, NULL);
      return 1;
   }

   LOS_fseek(fd, 0, LOS_SEEK_END);
   size = (size_t)LOS_ftell(fd);
   LOS_fseek(fd, 0, LOS_SEEK_SET);
   LOS_Log("duster size is %i bytes\n",  (void*)size, NULL, NULL, NULL, NULL, NULL);
   if (size != DUSTER_SIZE_BYTE){
	  LOS_Log("Size %i bytes instead of 2048\n", (void*)size, NULL, NULL, NULL, NULL, NULL);
          LOS_fclose(fd);
          return 1;
   }

   mem_handle=LOS_Alloc(size, 4, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
   logicalAddr = LOS_GetLogicalAddress(mem_handle) ;
//   physicalAddr = ILOS_GetPhysicalAddress(mem_handle) ;

   read = LOS_fread((void *)logicalAddr, 1, size, fd);
   if (read != size)
    {
          LOS_Log("Read %i bytes instead of %i\n", (void*)read, (void*)size, NULL, NULL, NULL, NULL);
	  LOS_Free(mem_handle);
          LOS_fclose(fd);
          return 1;
   }
   LOS_fclose(fd);

   duster_table= (t_uint32 *)logicalAddr;

   for(i=DUSTER_MEM_POSITION_DEFECT_OFFSET; i<(DUSTER_MEM_POSITION_DEFECT_OFFSET + DUSTER_SIZE_BYTE); i=i+4){ hv_SetReg(t1_to_arm(i),(*duster_table++) );}

   if (hw){
   } else {
   }

   LOS_Free(mem_handle);

   return 0;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_dusterloadhw_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_dusterloadhw_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: dusterloadhw <FILENAME>\n");}
 else {
   if (a_nb_args == 2)
      {
          ITE_NMF_dusterload(ap_args[1],1);
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

int ITE_NMF_gridironload (char *filename, int hw){
#define GRIDIRON_SIZE_BYTE ((GRIDIRON_MEM_ELT_CAST0_DIMENSION+GRIDIRON_MEM_ELT_CAST1_DIMENSION+GRIDIRON_MEM_ELT_CAST2_DIMENSION+GRIDIRON_MEM_ELT_CAST3_DIMENSION)*4)

   t_los_file *fd;
   size_t read;
   size_t size;
   t_los_memory_handle mem_handle;
   t_los_logical_address logicalAddr;
//   t_los_physical_address physicalAddr;
   int i;
   t_uint32 *cast0_gridiron_table;
   t_uint32 *cast1_gridiron_table;
   t_uint32 *cast2_gridiron_table;
   t_uint32 *cast3_gridiron_table;

   fd = LOS_fopen(filename, "rb");
   if (fd == NULL)
   {
      LOS_Log("unable to open gridiron file %s\n", (void*)filename, NULL, NULL, NULL, NULL, NULL);
      return 1;
   }

   LOS_fseek(fd, 0, LOS_SEEK_END);
   size = (size_t)LOS_ftell(fd);
   LOS_fseek(fd, 0, LOS_SEEK_SET);
   LOS_Log("gridiron size is %i bytes\n",  (void*)size, NULL, NULL, NULL, NULL, NULL);
   if (size != GRIDIRON_SIZE_BYTE){
	  LOS_Log("Size %i bytes instead of 106496\n", (void*)size, NULL, NULL, NULL, NULL, NULL);
          LOS_fclose(fd);
          return 1;
   }

   mem_handle=LOS_Alloc(size, 4, LOS_MAPPING_CREATE_UNCACHED_UNBUFFERED);
   logicalAddr = LOS_GetLogicalAddress(mem_handle) ;
//   physicalAddr = ILOS_GetPhysicalAddress(mem_handle) ;

   read = LOS_fread((void *)logicalAddr, 1, size, fd);
   if (read != size)
    {
          LOS_Log("Read %i bytes instead of %i\n", (void*)read, (void*)size, NULL, NULL, NULL, NULL);
	  LOS_Free(mem_handle);
          LOS_fclose(fd);
          return 1;
   }
   LOS_fclose(fd);

   ISP_R_W(ISP_GRIDIRON_CTRL,ISP_R_R(ISP_GRIDIRON_CTRL)|1<<8 );
 
   cast0_gridiron_table= (t_uint32 *)logicalAddr;
   cast1_gridiron_table= (t_uint32 *)logicalAddr+GRIDIRON_MEM_ELT_CAST0_DIMENSION;
   cast2_gridiron_table= (t_uint32 *)logicalAddr+GRIDIRON_MEM_ELT_CAST0_DIMENSION+GRIDIRON_MEM_ELT_CAST1_DIMENSION;
   cast3_gridiron_table= (t_uint32 *)logicalAddr+GRIDIRON_MEM_ELT_CAST0_DIMENSION+GRIDIRON_MEM_ELT_CAST1_DIMENSION+GRIDIRON_MEM_ELT_CAST2_DIMENSION;
   

   for(i=GRIDIRON_MEM_ELT_CAST0_OFFSET; i<(GRIDIRON_MEM_ELT_CAST0_OFFSET + GRIDIRON_MEM_ELT_CAST0_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*cast0_gridiron_table++) );}
   for(i=GRIDIRON_MEM_ELT_CAST1_OFFSET; i<(GRIDIRON_MEM_ELT_CAST1_OFFSET + GRIDIRON_MEM_ELT_CAST1_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*cast1_gridiron_table++) );}
   for(i=GRIDIRON_MEM_ELT_CAST2_OFFSET; i<(GRIDIRON_MEM_ELT_CAST2_OFFSET + GRIDIRON_MEM_ELT_CAST2_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*cast2_gridiron_table++) );}
   for(i=GRIDIRON_MEM_ELT_CAST3_OFFSET; i<(GRIDIRON_MEM_ELT_CAST3_OFFSET + GRIDIRON_MEM_ELT_CAST3_DIMENSION*4); i=i+4){ hv_SetReg(t1_to_arm(i),(*cast3_gridiron_table++) );}

   ISP_R_W(ISP_GRIDIRON_CTRL,ISP_R_R(ISP_GRIDIRON_CTRL)&(~(1L<<8)) );

   if (hw){

   } else {
	
   }

   LOS_Free(mem_handle);

   return 0;
}
/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_gridironloadhw_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */
CMD_COMPLETION C_ite_dbg_gridironloadhw_cmd(int a_nb_args, char ** ap_args)
{if (0== strcmp(ap_args[1],"help")) {
	CLI_disp_error("syntax: gridironloadhw <FILENAME>\n");}
 else {
   if (a_nb_args == 2)
      {
          ITE_NMF_gridironload(ap_args[1],1);
      }
   else
      {CLI_disp_error("Not correct command arguments\n"); }
   }
   return CMD_COMPLETE;
}

/* -----------------------------------------------------------------------
   FUNCTION : C_ite_dbg_algohelp_cmd
   PURPOSE  : 
   ------------------------------------------------------------------------ */


CMD_COMPLETION C_ite_dbg_algoshelp_cmd(int a_nb_args, char ** ap_args)
{  
 UNUSED(ap_args);
 if (a_nb_args == 1)
      { 
       	CLI_disp_error("\n");
       	CLI_disp_error("gammacontrolhw    : control gamma HW block, registers acces implementation\n");
 	CLI_disp_error(" 	            gammacontrolhw <ENABLE/DISABLE> <LR/HR>\n");
       	CLI_disp_error("gammaloadhw       : load gamma data file, registers acces implementation\n");
	CLI_disp_error("	 	    gammaloadhw <FILENAME> <LR/HR>\n");
       	CLI_disp_error("gammacontrol      : control gamma HW block, PE implementation\n");
 	CLI_disp_error(" 	            gammacontrolhw <ENABLE/DISABLE> <LR/HR>\n");
       	CLI_disp_error("gammaload         : load gamma data file, PE implementation\n");
	CLI_disp_error("	 	    gammaloadhw <FILENAME> <LR/HR>\n");
       	CLI_disp_error("gammaloadnips     : load gamma nips data file, register acces implementation\n");
	CLI_disp_error("	 	    gammaloadnips <FILENAME> <LR/HR>\n");
    	CLI_disp_error("linearcontrolhw   : control linearisation HW block, registers acces implementation\n");
 	CLI_disp_error(" 	            linearcontrolhw <ENABLE/DISABLE>\n");
       	CLI_disp_error("linearloadhw      : load linearisation data file, registers acces implementation\n");
	CLI_disp_error("	 	    linearloadhw <FILENAME>\n");
     	CLI_disp_error("dustercontrolhw   : control duster HW block, registers acces implementation\n");
 	CLI_disp_error(" 	            dustercontrolhw <ENABLE/DISABLE>\n");
      	CLI_disp_error("dusterloadhw      : load duster data file, registers acces implementation\n");
	CLI_disp_error("	 	    dusterloadhw <FILENAME>\n");
     	CLI_disp_error("gridironcontrolhw : control duster HW block, registers acces implementation\n");
 	CLI_disp_error(" 	            dustercontrolhw <ENABLE/DISABLE>\n");
      	CLI_disp_error("gridironloadhw    : load duster data file, registers acces implementation\n");
	CLI_disp_error("	 	    dusterloadhw <FILENAME>\n");


      } 
   else
      { CLI_disp_error("Not correct command arguments\n"); }
 return CMD_COMPLETE;
}

t_cmd_list ite_cmd_list_algos[] =
{
      { "algoshelp", C_ite_dbg_algoshelp_cmd,
        "algoshelp: algoshelp\n"
    },{ "gammacontrolhw", C_ite_dbg_gammacontrolhw_cmd,
        "gammacontrolhw: gammacontrolhw <enable/disable> <LR/HR> \n"
    },{ "gammaloadhw", C_ite_dbg_gammaloadhw_cmd,
        "gammaloadhw: gamaloadhw <FILE_NAME> <LR/HR>\n"
    },{ "gammacontrol", C_ite_dbg_gammacontrolpe_cmd,
        "gammacontrol: gammacontrol <enable/disable> <LR/HR> \n"
    },{ "gammaload", C_ite_dbg_gammaloadpe_cmd,
        "gammaload: gamaload <FILE_NAME> <LR/HR>\n"
    },{ "gammaloadnips", C_ite_dbg_gammaloadnips_cmd,
        "gammaloadnips: gamaloadnips <FILE_NAME> <LR/HR>\n"

    },{ "linearcontrolhw", C_ite_dbg_linearcontrolhw_cmd,
        "linearcontrolhw: linearcontrol <enable/disable>\n"
    },{ "linearcontrol", C_ite_dbg_linearcontrolpe_cmd,
        "linearcontrol: linearcontrol <enable/disable>\n"
    },{ "linearloadhw", C_ite_dbg_linearloadhw_cmd,
        "linearloadhw: linearloadhw <FILE_NAME>\n"
    },{ "linearload", C_ite_dbg_linearloadpe_cmd,
        "linearload: linearload <FILE_NAME>\n"

    },{ "dustercontrolhw", C_ite_dbg_dustercontrolhw_cmd,
        "dustercontrolhw: dustercontrolhw <enable/disable>\n"
    },{ "dusterloadhw", C_ite_dbg_dusterloadhw_cmd,
        "dusterloadhw: dusterloadhw <FILE_NAME>\n"

    },{ "gridironcontrolhw", C_ite_dbg_gridironcontrolhw_cmd,
        "gridironcontrolhw: gridironcontrol <enable/disable>\n"
    },{ "gridironcontrol", C_ite_dbg_gridironcontrolpe_cmd,
        "gridironcontrol: gridironcontrol <enable/disable>\n"
    },{ "gridironloadhw", C_ite_dbg_gridironloadhw_cmd,
        "gridironloadhw: gridironloadhw <FILE_NAME>\n"

    },
};

void Init_algos_ITECmdList(void)
{
    CLI_register_interface("ALGOS_ITE_CMB", sizeof(ite_cmd_list_algos)/(sizeof(ite_cmd_list_algos[0])), ite_cmd_list_algos, 1);
}




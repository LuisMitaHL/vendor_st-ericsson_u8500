/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DSI services
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "hcl_defs.h"
#include "services.h"
#include "memory_mapping.h"
#include "dsi.h"
#include "gic.h"
#include "dsi_irq.h"
#include "ab8500_core_services.h"
#include "dsi_services.h"
#ifndef __PEPS_8500
#include "i2c_services.h"
#endif

/****************************************************************************/
/*		 NAME :	SER_DSI_InterruptHandler()			        		    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Display Interface ISR                                 		*/
/* PARAMETERS :																*/
/*       IN   :	unsigned int irq,                                   		*/
/*     InOut  :None                                                         */
/* 		OUT   :None                                             	        */
/*                                                                          */
/* RETURN	  :None                         						   	    */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
void SER_DSI_InterruptHandler(IN t_uint32 irq)
{
    t_gic_error         gic_error;
    t_dsi_error         dsi_error;
    t_uint32            irq_src;
    t_dsi_irq_type      irq_type;


    gic_error = GIC_DisableItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_DISP_LINE \n");
        return;
    }
/*DSI_IRQ_TYPE_MCTL_MAIN*/
    irq_type = DSI_IRQ_TYPE_MCTL_MAIN;

    irq_src = DSI_GetIRQSrc(irq_type);
    irq_src = irq_src;

    DSI_ClearIRQSrc(irq_type,irq_src);


/*DSI_IRQ_TYPE_CMD_MODE*/
    irq_type = DSI_IRQ_TYPE_CMD_MODE;

    irq_src = DSI_GetIRQSrc(irq_type);
    irq_src = irq_src;

    DSI_ClearIRQSrc(irq_type,irq_src);


/*DSI_IRQ_TYPE_DIRECT_CMD_MODE*/
    irq_type = DSI_IRQ_TYPE_DIRECT_CMD_MODE;

    irq_src = DSI_GetIRQSrc(irq_type);
    irq_src = irq_src;

    DSI_ClearIRQSrc(irq_type,irq_src);

/*DSI_IRQ_TYPE_DIRECT_CMD_RD_MODE*/
    irq_type = DSI_IRQ_TYPE_DIRECT_CMD_RD_MODE;

    irq_src = DSI_GetIRQSrc(irq_type);
    irq_src = irq_src;

    DSI_ClearIRQSrc(irq_type,irq_src);

/*DSI_IRQ_TYPE_VID_MODE*/
    irq_type = DSI_IRQ_TYPE_VID_MODE;

    irq_src = DSI_GetIRQSrc(irq_type);
    irq_src = irq_src;

    DSI_ClearIRQSrc(irq_type,irq_src);

/*DSI_IRQ_TYPE_TG*/
    irq_type = DSI_IRQ_TYPE_TG;

    irq_src = DSI_GetIRQSrc(irq_type);
    irq_src = irq_src;

    DSI_ClearIRQSrc(irq_type,irq_src);

/*DSI_IRQ_TYPE_DPHY_ERROR*/
    irq_type = DSI_IRQ_TYPE_DPHY_ERROR;

    irq_src = DSI_GetIRQSrc(irq_type);
    irq_src = irq_src;

    DSI_ClearIRQSrc(irq_type,irq_src);


/*DSI_IRQ_TYPE_DPHY_CLK_TRIM_RD*/
    irq_type = DSI_IRQ_TYPE_DPHY_CLK_TRIM_RD;

    irq_src = DSI_GetIRQSrc(irq_type);
    irq_src = irq_src;

    DSI_ClearIRQSrc(irq_type,irq_src);

  /*  gic_error = GIC_AcknowledgeItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error During Acknowledge IT line the GIC_DISP_LINE \n");
        return(DSI_INTERNAL_ERROR);
    }*/

    gic_error = GIC_EnableItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_DISP_LINE \n");
        return;
    }

    dsi_error = dsi_error;
    return;
}


/****************************************************************************/
/*		 NAME :	SER_DSI_Init()										    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize DSI services using hcl api   		*/
/* PARAMETERS :																*/
/*       IN   :	t_uint8 mask, Display interface mask                		*/
/*     InOut  :None                                                         */
/* 		OUT   :None                                             	        */
/*                                                                          */
/* RETURN	  :None                         						   	    */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_DSI_Init(IN t_uint8 mask)
{
    t_dsi_error         dsi_error = DSI_OK;
    t_gic_error         gic_error;
    t_gic_func_ptr      old_handler;
    t_uint8 TransmitTab[2];
    t_uint8 ReceiveTab[2];
    #ifndef __PEPS_8500
    t_i2c_error     err_status;
    t_uint8         data_tx = 0,data;
/*    t_uint32        i;*/
    #endif


    dsi_error = DSI_Init(DSI_LINK0,DSI1_LINK_CFG_REG_BASE_ADDR);

    dsi_error = DSI_Init(DSI_LINK1,DSI2_LINK_CFG_REG_BASE_ADDR);

    dsi_error = DSI_Init(DSI_LINK2,DSI3_LINK_CFG_REG_BASE_ADDR);
    if (DSI_OK != dsi_error)
    {
        PRINT("Error in DSI_Init\n");
    }

    DSI_SetBaseAddress(DSI1_LINK_CFG_REG_BASE_ADDR);

    DSI_SetBaseAddress(DSI2_LINK_CFG_REG_BASE_ADDR);

    DSI_SetBaseAddress(DSI3_LINK_CFG_REG_BASE_ADDR);
    
    #ifndef __PEPS_8500
#if 0 /*Different expander for V1*/
    SER_I2C_Init(INIT_I2C0);

    SER_I2C_ConfigureDefault();

    I2C_SetTransferMode(I2C0, I2C_TRANSFER_MODE_POLLING, I2C_TRANSFER_MODE_POLLING);

       data_tx = 0x03;

       err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0x8B, data_tx);
       err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0x85, data_tx);


    /*for(i=0;i<1000000;i++);*/

    err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0x88, data_tx);

    /*for(i=0;i<1000000;i++);*/

    err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0x85, data_tx);

    err_status = err_status;
    
    
#endif
    SER_I2C_Init(INIT_I2C0);

        SER_I2C_ConfigureDefault();

        I2C_SetTransferMode(I2C0, I2C_TRANSFER_MODE_POLLING, I2C_TRANSFER_MODE_POLLING);

           data_tx = 0xC0;
           
           TransmitTab[0] = 0xC0;
           TransmitTab[1] = 0xC0;

           err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0xC7, data_tx);
           err_status = I2C_WriteMultipleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0xC2, TransmitTab,0x2);
           err_status = I2C_ReadMultipleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0xC2, ReceiveTab,0x2);
           


       /* for(i=0;i<1000000;i++);*/
        
        TransmitTab[0] = 0x0;
        TransmitTab[1] = 0xC0;
        
        ReceiveTab[0] = 0x0;
        ReceiveTab[1] = 0x0;

        err_status = I2C_WriteMultipleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0xC2, TransmitTab,0x2);
        err_status = I2C_ReadMultipleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0xC2, ReceiveTab,0x2);

      /*  for(i=0;i<1000000;i++);*/
        
        TransmitTab[0] = 0xC0;
        TransmitTab[1] = 0xC0;
        
        ReceiveTab[0] = 0x0;
        ReceiveTab[1] = 0x0;

        err_status = I2C_WriteMultipleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0xC2, TransmitTab,0x2);
        err_status = I2C_ReadMultipleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0xC2, ReceiveTab,0x2);

        err_status = err_status;
    
    
   #endif
		data=0x02;
			SER_AB8500_CORE_Write(0x03,0x00,1,&data);

		data = 0x05;
        
		SER_AB8500_CORE_Write(0x04,0x06,1,&data);
		data = 0x05;
		SER_AB8500_CORE_Write(0x04,0x09,1,&data);
		data = 0x08;
		SER_AB8500_CORE_Write(0x04,0x1F,1,&data);
		data = 0x08;
		SER_AB8500_CORE_Write(0x04,0x20,1,&data);
	

    gic_error = GIC_DisableItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error in disable the It line  GIC_DISP_LINE\n");
    }

    gic_error = GIC_ChangeDatum(GIC_DISP_LINE, SER_DSI_InterruptHandler, &old_handler);
    if (GIC_OK != gic_error)
    {
        PRINT("Error in GIC_ChangeDatum\n");
    }

    gic_error = GIC_EnableItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error in enable the It line  GIC_DISP_LINE\n");
    }

    /*enable interrupts tbd*/



}


/****************************************************************************/
/*		 NAME :	SER_DSI_Close()										        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine resets MCDE services using hcl api.     	    */
/* PARAMETERS :																*/
/*       IN   :	None                                                		*/
/*     InOut  :None                                                         */
/* 		OUT   :None                                             	        */
/*                                                                          */
/* RETURN	  :None                         						   	    */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_DSI_Close(void)
{

 //TBD

}
/****************************************************************************/
/*		 NAME :	SER_DSI_Configure()									        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures DSI for TAAL panel          	    */
/* PARAMETERS :																*/
/*       IN   :	None                                                		*/
/*     InOut  :None                                                         */
/* 		OUT   :None                                             	        */
/*                                                                          */
/* RETURN	  :None                         						   	    */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error SER_DSI_Configure(t_dsi_platform platform)
{
    volatile t_dsi_pll_ctl         pll_ctl;
    t_uint8               link_sts;
    t_uint32              time_out= 1000;
    volatile t_dsi_dphy_timeout    timeout;
    volatile t_dphy_static         dhy_static;
    t_dsi_error           error;
    
    #ifdef ST_8500ED

    pll_ctl.pll_master = DSI_PLL_SLAVE;
    pll_ctl.pll_in_sel = DSI_PLL_IN_CLK_27;
    pll_ctl.pll_out_sel = DSI_INTERNAL_PLL;
    pll_ctl.multiplier  = 32;
    pll_ctl.division_ratio = 1;

	#else
	
/*	pll_ctl.pll_master = DSI_PLL_SLAVE;
    pll_ctl.pll_in_sel = DSI_PLL_IN_CLK_27;
    pll_ctl.pll_out_sel = DSI_INTERNAL_PLL;
    pll_ctl.multiplier  = 32;
    pll_ctl.division_ratio = 1;*/
    
    #endif

	

    dhy_static.hs_invert_clk = DSI_DISABLE;
    dhy_static.hs_invert_dat1 = DSI_DISABLE;
    dhy_static.hs_invert_dat2 = DSI_DISABLE;
    dhy_static.swap_pins_clk = DSI_DISABLE;
    dhy_static.swap_pins_dat1 = DSI_DISABLE;
    dhy_static.swap_pins_dat2 = DSI_DISABLE;
    dhy_static.ui_x4          = 12;/* 4*clock period in nanoseconds */

  *((volatile t_uint32 *) (0xA0351008)) = 0x01;

  *((volatile t_uint32 *) (0xA0353008)) = 0x01;

      /*MCTL_MAIN_DATA_CTL*/
   	DSI_EnableLink(DSI_LINK2);

    error = DSI_StartLane(DSI_LINK2,DSI_DATA_LANE1);
    error = DSI_StartLane(DSI_LINK2,DSI_DATA_LANE2);
    error = DSI_StartLane(DSI_LINK2,DSI_CLK_LANE);

    error = DSI_GetLinkStatus(DSI_LINK2,&link_sts);

    if(!(link_sts & 0xE))
        return(DSI_NOT_CONFIGURED);/*PLL programming failed*/

      DSI_SetPLLControl(DSI_LINK2,pll_ctl);


      DSI_StartPLL(DSI_LINK2);


    DSI_EnableLink(DSI_LINK0);


    error = DSI_StartLane(DSI_LINK0,DSI_DATA_LANE1);
    error = DSI_StartLane(DSI_LINK0,DSI_DATA_LANE2);
    error = DSI_StartLane(DSI_LINK0,DSI_CLK_LANE);

    error = DSI_GetLinkStatus(DSI_LINK0,&link_sts);

    if(!(link_sts & 0xE))
        return(DSI_INTERNAL_ERROR);/*PLL programming failed*/

    DSI_SetPLLControl(DSI_LINK0,pll_ctl);

    DSI_StartPLL(DSI_LINK0);


    if(platform ==  DSI_MOP500)
    {

  	DSI_SetDPHY_Static(DSI_LINK0,dhy_static);
    DSI_EnableCLK_HS_SendingMode(DSI_LINK0);
    }
    else
    {

    /*  DPHY programming*/

    *((volatile t_uint32 *) (0xA0351A00)) = 0x14080C00;
    *((volatile t_uint32 *) (0xA0351A04)) = 0xA8800240;
    *((volatile t_uint32 *) (0xA0351A08)) = 0x041000AA;
    *((volatile t_uint32 *) (0xA0351A0C)) = 0x00000024;
    *((volatile t_uint32 *) (0xA0351A24)) = 0x10000000;
    *((volatile t_uint32 *) (0xA0351A28)) = 0x00000700;
    *((volatile t_uint32 *) (0xA0351A44)) = 0x0000E002;
    *((volatile t_uint32 *) (0xA0351A48)) = 0x20000100;
    *((volatile t_uint32 *) (0xA0351A50)) = 0x20000001;
    }

 	timeout.clk_div = 0xF;
    timeout.hs_tx_timeout = 0xFFFF;
    timeout.lp_rx_timeout = 0xFFFF;

    DSI_SetDPHY_TimeOut(DSI_LINK0,timeout);


    error = DSI_EnableInterface(DSI_LINK0,DSI_INTERFACE_2);

    while(time_out > 0)
        time_out--;

    *((volatile t_uint32 *) (0xA0351050)) = 0x800000;
    
    return(error);
}

t_uint32 SER_DSI_SendDirectCommand_HS(t_uint32 Head_size,t_uint32 Cmd_size,t_uint32 cmd1,t_uint32 cmd2,t_uint32 cmd3,t_uint32  cmd4)
{
    volatile t_dsi_cmd_main_setting cmd_settings;

    cmd_settings.cmd_id     = DSI_VIRTUAL_CHANNEL_0;
    cmd_settings.cmd_lp_enable = DSI_DISABLE;
    cmd_settings.cmd_nature = DSI_CMD_WRITE;
    cmd_settings.cmd_size = Cmd_size;
    cmd_settings.cmd_trigger_val = 0x0;
  
    switch(Head_size)
    {
    case 0x39:
        cmd_settings.cmd_header = 0x39;
        cmd_settings.packet_type = DSI_CMD_LONG;
        break;

    case 0x15:
        cmd_settings.cmd_header = 0x15;
        cmd_settings.packet_type = DSI_CMD_SHORT;
        break;

    case 0x05:
        cmd_settings.cmd_header = 0x05;
        cmd_settings.packet_type = DSI_CMD_SHORT;
        break;

    default:
        return(1);
    }

    DSI_SetDirectCmdSettings(DSI_LINK0,cmd_settings);

    DSI_SetDirectCommand(DSI_LINK0,cmd1,cmd2,cmd3,cmd4);

   /* DSI_DirectCommandSend(0);*/

    *((volatile t_uint32 *) (0xA0351060)) = 0x1;

    return(0);
}


t_uint32 SER_DSI_SendDirectCommand_LP(t_uint32 Head_size,t_uint32 Cmd_size,t_uint32 cmd1,t_uint32 cmd2,t_uint32 cmd3,t_uint32  cmd4)
{
    volatile t_dsi_cmd_main_setting cmd_settings;

    cmd_settings.cmd_id     = DSI_VIRTUAL_CHANNEL_0;
    cmd_settings.cmd_lp_enable = DSI_ENABLE;
    cmd_settings.cmd_nature = DSI_CMD_WRITE;
    cmd_settings.cmd_size = Cmd_size;
    cmd_settings.cmd_trigger_val = 0x0;
  
    switch(Head_size)
    {
    case 0x39:
        cmd_settings.cmd_header = 0x39;
        cmd_settings.packet_type = DSI_CMD_LONG;
        break;

    case 0x15:
        cmd_settings.cmd_header = 0x15;
        cmd_settings.packet_type = DSI_CMD_SHORT;
        break;

    case 0x05:
        cmd_settings.cmd_header = 0x05;
        cmd_settings.packet_type = DSI_CMD_SHORT;
        break;

    default:
        return(1);
    }

    DSI_SetDirectCmdSettings(DSI_LINK0,cmd_settings);

    DSI_SetDirectCommand(DSI_LINK0,cmd1,cmd2,cmd3,cmd4);

   /* DSI_DirectCommandSend(0);*/

    *((volatile t_uint32 *) (0xA0351060)) = 0x1;

    return(0);
}


PUBLIC void SER_DSI_TAALDisplayInit(void)
{
    t_uint32 ret_val;
/*    t_uint32 i;*/
        
    /* 
    *! Display Settings for initialization
    */
    

     ret_val = SER_DSI_SendDirectCommand_LP(0x05,0x01,0x00000011,0x00000000,0x00000000,0x00000000);/*sleepout*/
    
    ret_val = SER_DSI_SendDirectCommand_LP(0x39,0x10,0x00000000,0x00000000,0x00000000,0x00000000);
    
    /*   for (i=0; i<120000; i++);*/
        
    ret_val = SER_DSI_SendDirectCommand_LP(0x15,0x02,0x0000F73A,0x00000000,0x00000000,0x00000000);    /*color mode*/
    

    ret_val = SER_DSI_SendDirectCommand_LP(0x05,0x01,0x00000029,0x00000000,0x00000000,0x00000000);   /*display on*/

    ret_val = SER_DSI_SendDirectCommand_HS(0x39,0x04,0xFFFFFF2C,0x000000,0x0,0x0);
    
    ret_val = ret_val;
          
}


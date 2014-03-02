/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   MCDE Services File
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "hcl_defs.h"
#include "memory_mapping.h"
#include "mcde.h"
#include "mcde_irq.h"
#include "gic.h"

#include "services.h"
#include "mcde_services.h"


#define NO_FILTER_MODE  0

/****************************************************************************/
/*		 NAME :	SER_MCDE_InterruptHandler()			        		    	*/
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
PUBLIC void SER_MCDE_InterruptHandler(IN t_uint32 irq)
{
    t_gic_error         gic_error;
    t_uint32            irq_src;
   
    gic_error = GIC_DisableItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error During disable the GIC_DISP_LINE \n");
        return;
    }
	
	#ifdef ST_8500ED
	irq_src = MCDE_GetIRQSrc();
	#else
    irq_src = MCDE_GetIRQSrc(MCDE_IRQ_TYPE_PIXELPROCESSING);
    #endif
	/*coverity[self_assign]*/
	irq_src = irq_src;

  //  MCDE_ClearIRQSrc(0x08);

 /*   gic_error = GIC_AcknowledgeItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error During Acknowledge IT line the GIC_DISP_LINE \n");
        return(MCDE_INTERNAL_ERROR);
    }*/

    gic_error = GIC_EnableItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error During enable the GIC_DISP_LINE \n");
        return;
    }
    
    return;
}

/****************************************************************************/
/*		 NAME :	SER_MCDE_Init()										    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize MCDE services using hcl api   		*/
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
PUBLIC void SER_MCDE_Init(IN t_uint8 mask)
{
   
    t_mcde_error        mcde_error = MCDE_OK;
    t_gic_error         gic_error;
    t_gic_func_ptr      old_handler;
    
   #if 0
    
      *((volatile t_uint32 *) (0x80113004)) = 0xFF;
    
    while(*(unsigned *)(0x80113000) != 0x01)
	 *((volatile t_uint32 *) (0x80113000))   = 0x01; /* Work around for making PWL high*/
	#endif 
   

   mcde_error = MCDE_Init(DISPLAY_CTRL_REG_BASE_ADDR);
    if (MCDE_OK != mcde_error)
    {
        PRINT("Error in MCDE_Init\n");
    }

    MCDE_SetBaseAddress(DISPLAY_CTRL_REG_BASE_ADDR);

    gic_error = GIC_ChangeDatum(GIC_DISP_LINE, SER_MCDE_InterruptHandler, &old_handler);
    if (GIC_OK != gic_error)
    {
        PRINT("Error in GIC_ChangeDatum\n");
    }

    gic_error = GIC_EnableItLine(GIC_DISP_LINE);
    if (GIC_OK != gic_error)
    {
        PRINT("Error in enable the It line  GIC_DISP_LINE\n");
    }
    
 
}


/****************************************************************************/
/*		 NAME :	SER_MCDE_Close()										    */
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
PUBLIC void SER_MCDE_Close(void)
{

 //TBD
	
}
#if 0
/****************************************************************************/
/*		 NAME :	SER_MCDE_SmartPanel_Init()								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes backlight and panel reset for smart*/
/*                panel     	                                            */
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
PUBLIC void SER_MCDE_SmartPanel_Init(void)
{
  /* 	t_uint8         data_tx = 0;*/
	t_i2c_error     err_status;
	#if ( defined(NDB20) && (NDB20 == 10))
	t_gpio_config   config;
	#endif
	t_gpio_config   config1;

	
#if ( defined(NDB20) && (NDB20 == 10))	
	config.mode = GPIO_MODE_SOFTWARE;
	config.direction = GPIO_DIR_OUTPUT;
	config.trig = GPIO_TRIG_LEAVE_UNCHANGED;
	#endif
	
	config1.mode = GPIO_MODE_ALT_FUNCTION_B;
	config1.direction = GPIO_DIR_INPUT;
	config1.trig = GPIO_TRIG_LEAVE_UNCHANGED;
	
	GPIO_SetPinConfig(GPIO_PIN_46,config1);
	
	
	*((volatile t_uint32 *) (0x90360804)) = 0x40000000;
	
	*((volatile t_uint32 *) (0x90360008 )) = 0x000000C0;

 /* SER_I2C_Init(INIT_I2C0);
    
    SER_I2C_ConfigureDefault();
    
    I2C_SetTransferMode(I2C0, I2C_TRANSFER_MODE_POLLING, I2C_TRANSFER_MODE_POLLING);*/
    
    SER_GPIO_Expander_Init();
  
    #if ( defined(NDB20) && (NDB20 == 10))
/*    data_tx = 0x02;
    
    err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0x89, data_tx);
    err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0x83, data_tx);*/
    
    SER_GPIO_Expander_Set_PinDirection(GPIO_EXPANDER_PIN_17,GPIO_EXPANDER_DIR_OUTPUT);
    SER_GPIO_Expander_Set_Pin(GPIO_EXPANDER_PIN_17);
    
    err_status=err_status;/*to remove warning*/
    GPIO_SetPinConfig(GPIO_PIN_43,config);
    
    GPIO_SetGpioPin(GPIO_PIN_43);
    
    GPIO_SetPinConfig(GPIO_PIN_33,config);
    
    GPIO_SetGpioPin(GPIO_PIN_33);
    
 #endif   
    #if ( defined(NVB20) && (NVB20 == 10))
/*    data_tx = 0x0A;
    
    err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0x89, data_tx);
    err_status = I2C_WriteSingleData(I2C0, (0x84 >>1), I2C_BYTE_INDEX, 0x83, data_tx);*/
    
    SER_GPIO_Expander_Set_PinDirection(GPIO_EXPANDER_PIN_17,GPIO_EXPANDER_DIR_OUTPUT);
    SER_GPIO_Expander_Set_Pin(GPIO_EXPANDER_PIN_17);
    
    SER_GPIO_Expander_Set_PinDirection(GPIO_EXPANDER_PIN_19,GPIO_EXPANDER_DIR_OUTPUT);
    SER_GPIO_Expander_Set_Pin(GPIO_EXPANDER_PIN_19);
    
    
    
    err_status=err_status;/*to remove warning*/
    #endif
	
}
/****************************************************************************/
/*		 NAME :	SER_MCDE_LCDDefaultConfigure()								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures MCDE for default LCD configuration  */
/* PARAMETERS :																*/
/*       IN   :	t_mcde_lcd_panel : Panel type (VGA/QVGA)                    */
/*            : t_mcde_bpp_ctrl  : bits per pixel                           */
/*            : t_uint32 buffer_address : Buffer address                    */
/*     InOut  :None                                                         */
/* 		OUT   :None                                             	        */
/*                                                                          */
/* RETURN	  :t_mcde_error                         						*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/

PUBLIC t_mcde_error SER_MCDE_LCDDefaultConfigure(t_mcde_lcd_panel panel,t_mcde_bpp_ctrl bpp,t_uint32 buffer_address)
{
    t_mcde_control   sys_control;
    t_mcde_ch_id channel = MCDE_CH_A;

    t_mcde_ext_conf    config;
   	t_mcde_ext_src_ctrl control;
   	t_mcde_ovr_control  control1;
   	t_mcde_ovr_config   config1;
    t_mcde_ovr_conf2    conf;
	t_mcde_ovr_comp     comp;
	t_mcde_chx_config  chx_config;
	t_mcde_chsyncconf  chx_conf;
	t_mcde_chsyncmod   chx_mod;
	t_mcde_ch_bckgrnd_col color;

    t_mcde_chx_control1 chx_control;
    t_mcde_tv_control   tv_ctrl;
    t_mcde_tv_timing1 tim;
    t_mcde_tv_lbalw_timing tim1;
    t_mcde_tv_dvo_offset offset;
    t_mcde_tv_blanking_field blank;
    t_mcde_lcd_timing1 lcd_time;
    
    sys_control.data_lsb0 = MCDE_CH_A_LSB;
    sys_control.data_lsb1 = MCDE_CH_A_MID;
    sys_control.data_mid =  MCDE_CH_A_LSB;
    sys_control.data_msb0 = MCDE_CH_A_LSB;
    sys_control.data_msb1 = MCDE_CH_A_MSB;
    sys_control.panel_mcdeaps = MCDE_CHIPSELECT_C0;
    sys_control.panel_mcdeapl = MCDE_CHIPSELECT_C0;
    sys_control.panel_mcdealp = MCDE_CH_A_OUT_ENABLE;
    sys_control.panel_mcdeacp = MCDE_CH_A_CLK_PANEL;
    sys_control.cdi_enable = MCDE_CDI_DISABLE;
    sys_control.data_cdiblock = MCDE_CDI_CH_A;
    
    MCDE_SetControlConfig(sys_control);

    MCDE_SetBufferBaseAddress(MCDE_EXT_SRC_0,MCDE_BUFFER_ID_0,(t_uint32)buffer_address);
   
    config.bpp = bpp; 
    config.buf_id = MCDE_BUFFER_ID_0;
    config.buf_num = MCDE_BUFFER_USED_1;
    config.endianity = MCDE_BYTE_LITTLE;
    config.ovr_id = MCDE_OVERLAY_0;
    config.ovr_pxlorder =MCDE_PIXEL_ORDER_LITTLE;
    config.rgb_format = MCDE_COL_RGB;
   
   
   MCDE_SetExtSrcConfig(MCDE_EXT_SRC_0,config);
      
   control.fs_ctrl = MCDE_FS_FREQ_DIV_ENABLE;
   control.fs_div = MCDE_FS_FREQ_UNCHANGED;
   control.ovr_ctrl = MCDE_MULTI_CH_CTRL_PRIMARY_OVR;
   control.sel_mode = /*MCDE_BUFFER_AUTO_TOGGLE;*/MCDE_BUFFER_SOFTWARE_SELECT;
   
   MCDE_SetExtSrcControl(MCDE_EXT_SRC_0,control);
   
   
   /*Overlay*/
   	control1.rot_burst_req = MCDE_ROTATE_BURST_WORD_4;
	control1.outstnd_req = MCDE_OUTSTND_REQ_4;
	control1.burst_req = MCDE_BURST_WORD_HW_8;
	control1.priority = 0x0;
	control1.color_key = MCDE_COLOR_KEY_DISABLE;
	control1.pal_control = MCDE_PAL_GAMA_DISABLE;
	control1.col_ctrl = MCDE_COL_CONV_DISABLE;
	control1.ovr_state = MCDE_OVERLAY_ENABLE;
    
    MCDE_SetOverlayCtrl(MCDE_OVERLAY_0,control1);
    
	config1.line_per_frame = 480;
	config1.ovr_ppl = 640;
	config1.src_id = MCDE_EXT_SRC_0;
	
	MCDE_SetOverlayConfig(MCDE_OVERLAY_0,config1);
	
	conf.ovr_blend = MCDE_CONST_ALPHA_SOURCE;
	conf.alpha_value = 0x64;
	conf.ovr_opaq = MCDE_OVR_OPAQUE_ENABLE;
	conf.pixoff = 0x0;
	conf.watermark_level = 0x20; 
	
	MCDE_SetOverlayConf2(MCDE_OVERLAY_0,conf);

    if (panel == MCDE_PANEL_VGA) {
    
    switch(bpp)
    {
    case MCDE_ARGB_16_BIT:
    case MCDE_IRGB1555_16_BIT:
    case MCDE_RGB565_16_BIT:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x500);
        break;
    case MCDE_RGB_PACKED_24_BIT:
    case MCDE_RGB_UNPACKED_24_BIT:
    case MCDE_ARGB_32_BIT:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0xA00);
        break;
    default:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x280);
        break;
    }
    }
    else if(panel == MCDE_PANEL_QVGA){
        switch(bpp){
        case MCDE_ARGB_16_BIT:
    case MCDE_IRGB1555_16_BIT:
    case MCDE_RGB565_16_BIT:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x1E0);
        break;
    case MCDE_RGB_PACKED_24_BIT:
    case MCDE_RGB_UNPACKED_24_BIT:
    case MCDE_ARGB_32_BIT:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x3C0);
        break;
    default:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0xF0);
        break;
        }
    }
    else
        return(MCDE_INVALID_PARAMETER);
	
/*	MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x500);*/
	
	MCDE_SetOvrTopMargin(MCDE_OVERLAY_0,0);
	
	MCDE_SetOvrLeftMargin(MCDE_OVERLAY_0,0);
	
	comp.ch_id = channel;            
	comp.ovr_xpos = 0x0;
	comp.ovr_ypos = 0x0;
	comp.ovr_zlevel = 0xF;
	
	MCDE_SetOvrComposition(MCDE_OVERLAY_0,comp);
	
	if (panel == MCDE_PANEL_VGA) {
	
	chx_config.chx_lpf = 480-1;
	chx_config.chx_ppl =  640-1;
    }
    else if (panel == MCDE_PANEL_QVGA) {
        chx_config.chx_lpf = 320 -1;
        chx_config.chx_ppl =  480 -1;
    }
    else
        return(MCDE_INVALID_PARAMETER);
	
	color.blue = 0x00;                                                    
	color.green = 0x00;
	color.red = 0x00;
	
	MCDE_SetChXConfig(channel,chx_config);
	
	MCDE_SetChBckGndCol(channel,color);
	
	chx_conf.autoframe_delay = 0x1E0;
	chx_conf.frame_intrevent = MCDE_FRONT_PORCH;
	chx_conf.frame_synchroevnt =MCDE_ACTIVE_VIDEO;
	chx_conf.swframe_intrdelay =0x1E0;
	
	MCDE_SetChSyncConf(channel,chx_conf);
	
	chx_mod.ch_synch_src =MCDE_SYNCHRO_OUTPUT_SOURCE;
	chx_mod.out_synch_interface = MCDE_LCD_TV_0;    
	
	MCDE_SetChSyncSource(channel,chx_mod);

    chx_control.tv_clk = MCDE_TVCLK_INTERNAL;
    chx_control.bcd_ctrl = MCDE_PCD_BYPASS;
    chx_control.out_bpp = MCDE_BPP_16;
    chx_control.clk_per_line = 0x27F;
    chx_control.lcd_bus = MCDE_BUS_16_CONF1;//MCDE_BUS_18_CONF1;
    chx_control.dpi2_clk = MCDE_CLK_42;
    chx_control.pcd = 0x0;
   
   MCDE_SetPanelControl(channel,chx_control);
   
   tv_ctrl.num_lines = 0x1E0;
   tv_ctrl.tv_mode = MCDE_TVMODE_NOTUSED;
   tv_ctrl.ifield = MCDE_ACTIVE_HIGH;
   tv_ctrl.interlace_enable = MCDE_DISABLE;
   tv_ctrl.sel_mode = MCDE_MODE_LCD;
   
   MCDE_SetTVControl(channel,tv_ctrl);
   
   tim.src_window_width = 0x280;
   tim.destination_hor_offset = 0x6;
   
   MCDE_SetTVSourceWindowWidth(channel,tim);
   
   tim1.active_line_width = 0x6;
   tim1.line_blanking_width = 0x25;
   
   MCDE_SetTVModeLineBlanking(channel,tim1);
   
   
   offset.field1_window_offset = 0x10;
   offset.field2_window_offset = 0;
   
   MCDE_SetTVVerticalOffset(channel,offset);
   
   blank.blanking_start = 0x5;
   blank.blanking_end = 0x12;
   
   MCDE_SetField1Blanking(channel,blank);
   
   
   lcd_time.io_enable = MCDE_ACTIVE_HIGH;
   lcd_time.ipc = MCDE_DATA_RISING_EDGE;
   lcd_time.ihs = MCDE_ACTIVE_LOW;
   lcd_time.ivs = MCDE_ACTIVE_LOW;
   lcd_time.ivp = MCDE_ACTIVE_HIGH;
   lcd_time.iclspl = MCDE_ACTIVE_HIGH;
   lcd_time.iclrev = MCDE_ACTIVE_HIGH;
   lcd_time.iclsp = MCDE_ACTIVE_HIGH;
   lcd_time.mcde_spl = MCDE_VERTICAL_ACTIVE_FRAME_DISABLE;
   lcd_time.spltgen = MCDE_OUTPUT_NORMAL;
   lcd_time.spl_sync_sel = MCDE_SYNCHRO_HBP;
   lcd_time.spl_delay1 = 0x0;
   lcd_time.spl_delay0 = 0x0;
   
   MCDE_SetLCDTimingControl(channel,lcd_time);

   *((volatile t_uint32 *) (0x90360860)) = 0x00300000; /* work around for 8820 cut A0*/

   return(MCDE_OK);
   
}

/****************************************************************************/
/*		 NAME :	SER_MCDE_LCDRun()								            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures enables display on LCD              */
/* PARAMETERS :																*/
/*       IN   :None                                                         */
/*     InOut  :None                                                         */
/* 		OUT   :None                                             	        */
/*                                                                          */
/* RETURN	  :none                                   						*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void SER_MCDE_LCDRun(void)
{
    t_mcde_ch_id channel = MCDE_CH_A;

    MCDE_SetState(MCDE_ENABLE);
    
    MCDE_ChannelX_FlowEnable(channel);
 
 	MCDE_ChannelX_PowerEnable(channel);
}
#endif
/****************************************************************************/
/*		 NAME :	SER_MCDE_DefaultConfigureUIB()								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures MCDE for default LCD configuration  */
/* PARAMETERS :																*/
/*       IN   :	t_mcde_lcd_panel : Panel type (VGA/QVGA)                    */
/*            : t_mcde_bpp_ctrl  : bits per pixel                           */
/*            : t_uint32 buffer_address : Buffer address                    */
/*     InOut  :None                                                         */
/* 		OUT   :None                                             	        */
/*                                                                          */
/* RETURN	  :t_mcde_error                         						*/
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/

PUBLIC t_mcde_error SER_MCDE_DefaultConfigureUIB(t_mcde_lcd_panel panel,t_mcde_bpp_ctrl bpp,t_uint32 buffer_address)
{
	t_mcde_fifo_ctrl fifo_ctrl;
	#ifdef ST_8500ED 
	t_mcde_dsi_clk_config clk_config;
	#endif
    t_mcde_ch_id channel = MCDE_CH_C0;

    t_mcde_ext_conf    config;
   	t_mcde_ext_src_ctrl control;
   	t_mcde_ovr_control  control1;
   	t_mcde_ovr_config   config1;
    t_mcde_ovr_conf2    conf;
	t_mcde_ovr_comp     comp;
	t_mcde_chx_config  chx_config;
	t_mcde_chsyncmod   chx_mod;
	t_mcde_ch_bckgrnd_col color;
	t_mcde_chc_config     configuration;

    t_mcde_dsi_conf dsi_conf;
    t_mcde_error error;
    
    #ifdef ST_8500ED
    clk_config.pllout_divsel2 = MCDE_PLL_OUT_2;
    clk_config.pllout_divsel1 = MCDE_PLL_OUT_2;
    clk_config.pllout_divsel0 = MCDE_PLL_OUT_2;
    clk_config.txescdiv_sel = MCDE_DSI_MCDECLK;
    clk_config.txescdiv     = 16;
    clk_config.pll4in_sel = MCDE_HDMICLK;
	#endif


    /*Enable the various Clocks*/
    *((volatile t_uint32 *) (0x80157064)) = 0x125;   /* MCDE Clock=160 MHz*/
    *((volatile t_uint32 *) (0x80157058)) = 0x145;   /* HDMI Clock=76.8 MHz*/
    *((volatile t_uint32 *) (0x8015707C)) = 0x14E;   /* TVCLK = 27.4 MHz*/

	#ifdef ST_8500ED    
    /* Set MCDE Clock selection for DSI link*/
    MCDE_SetDSIClock(&clk_config);
    #endif
    
    *((volatile t_uint32 *) (0xA0350000)) =0x387B1020; //ch C mcde clk
    
    fifo_ctrl.out_fifo0 = MCDE_DSI_CMD0;
    fifo_ctrl.out_fifo1 = MCDE_DBI_C1;
    fifo_ctrl.out_fifoa = MCDE_DPI_A;
    fifo_ctrl.out_fifob = MCDE_DPI_B;

    MCDE_SetFIFOControl(&fifo_ctrl);
    

    MCDE_SetBufferBaseAddress(MCDE_EXT_SRC_0,MCDE_BUFFER_ID_0,(t_uint32)buffer_address);
   
    config.bpp = bpp; 
    config.buf_id = MCDE_BUFFER_ID_0;
    config.buf_num = MCDE_BUFFER_USED_1;
    config.endianity = MCDE_BYTE_LITTLE;
    config.ovr_id = MCDE_OVERLAY_0;
    config.ovr_pxlorder =MCDE_PIXEL_ORDER_LITTLE;
    config.rgb_format = MCDE_COL_RGB;
   
   
    MCDE_SetExtSrcConfig(MCDE_EXT_SRC_0,&config);
      
    control.fs_ctrl = MCDE_FS_FREQ_DIV_ENABLE;
    control.fs_div = MCDE_FS_FREQ_UNCHANGED;
    control.ovr_ctrl = MCDE_MULTI_CH_CTRL_ALL_OVR;
    control.sel_mode = /*MCDE_BUFFER_AUTO_TOGGLE;*/MCDE_BUFFER_SOFTWARE_SELECT;
   
    MCDE_SetExtSrcControl(MCDE_EXT_SRC_0,&control);
   
   
   /*Overlay*/
   	control1.rot_burst_req = MCDE_ROTATE_BURST_WORD_4;
	control1.outstnd_req = MCDE_OUTSTND_REQ_4;
	control1.burst_req = MCDE_BURST_WORD_HW_8;
	control1.priority = 0x0;
	control1.color_key = MCDE_COLOR_KEY_DISABLE;
	control1.col_ctrl = MCDE_COL_CONV_DISABLE;
	control1.ovr_state = MCDE_OVERLAY_ENABLE;
    
    MCDE_SetOverlayCtrl(MCDE_OVERLAY_0,&control1);
	
	conf.ovr_blend = MCDE_CONST_ALPHA_SOURCE;
	conf.alpha_value = 0x0;
	conf.ovr_opaq = MCDE_OVR_OPAQUE_ENABLE;
	conf.pixoff = 0x0;
	conf.watermark_level = 240*4; 
	
	MCDE_SetOverlayConf2(MCDE_OVERLAY_0,&conf);

    if (panel == MCDE_PANEL_VGA) {
   	config1.line_per_frame = 480;
	config1.ovr_ppl = 640;
	config1.src_id = MCDE_EXT_SRC_0;
	
	MCDE_SetOverlayConfig(MCDE_OVERLAY_0,&config1);
    
    switch(bpp)
    {
    case MCDE_ARGB_16_BIT:
    case MCDE_IRGB1555_16_BIT:
    case MCDE_RGB565_16_BIT:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x500);
        break;
    case MCDE_RGB_PACKED_24_BIT:
    case MCDE_RGB_UNPACKED_24_BIT:
    case MCDE_ARGB_32_BIT:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0xA00);
        break;
    default:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x280);
        break;
    }
    }
    
    if(panel == MCDE_PANEL_QVGA){
   	config1.line_per_frame = 240;
	config1.ovr_ppl = 320;
	config1.src_id = MCDE_EXT_SRC_0;
	
	MCDE_SetOverlayConfig(MCDE_OVERLAY_0,&config1);
        switch(bpp){
        case MCDE_ARGB_16_BIT:
    case MCDE_IRGB1555_16_BIT:
    case MCDE_RGB565_16_BIT:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x1E0);
        break;
    case MCDE_RGB_PACKED_24_BIT:
    case MCDE_RGB_UNPACKED_24_BIT:
    case MCDE_ARGB_32_BIT:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0x3C0);
        break;
    default:
        MCDE_SetOvrLineIncrement(MCDE_OVERLAY_0,0xF0);
        break;
        }
    }
    
	MCDE_SetOvrTopMargin(MCDE_OVERLAY_0,0);
	
	MCDE_SetOvrLeftMargin(MCDE_OVERLAY_0,0);
	
	comp.ch_id = channel;            
	comp.ovr_xpos = 0x0;
	comp.ovr_ypos = 0x0;
	comp.ovr_zlevel = 0x0;
	
	MCDE_SetOvrComposition(MCDE_OVERLAY_0,&comp);
	
	if (panel == MCDE_PANEL_VGA) {
	
	chx_config.chx_lpf = 480-1;
	chx_config.chx_ppl =  864-1;
    }
    if (panel == MCDE_PANEL_QVGA) {
        chx_config.chx_lpf = 320 -1;
        chx_config.chx_ppl =  480 -1;
    }
    
	color.blue = 0x00;                                                    
	color.green = 0x00;
	color.red = 0x00;
	
	MCDE_SetChXConfig(channel,&chx_config);
	
	MCDE_SetChBckGndCol(channel,&color);
	
	chx_mod.ch_synch_src =MCDE_SYNCHRO_SOFTWARE;
	chx_mod.out_synch_interface = MCDE_LCD_TV_0;    
	
	MCDE_SetChSyncSource(channel,&chx_mod);
	
    configuration.fifo_watermark = MCDE_FIFO_WMLVL_8;
    configuration.res_pol = MCDE_SIG_INACTIVE_POL_HIGH;
    configuration.rd_pol = MCDE_SIG_INACTIVE_POL_LOW;
    configuration.wr_pol = MCDE_SIG_INACTIVE_POL_LOW;
    configuration.cd_pol = (t_mcde_cd_polarity)MCDE_SIG_INACTIVE_POL_HIGH;
    configuration.cs_pol = MCDE_SIG_INACTIVE_POL_HIGH;
    configuration.csen  = MCDE_CSEN_ACTIVATED;
    configuration.bus_size = MCDE_BUS_SIZE_8;
    configuration.chcen = MCDE_CHANEL_C_ENABLE;
    configuration.inband_mode = MCDE_SELECT_OUTBAND;
    configuration.syncen = MCDE_SYNCHRO_CAPTURE_ENABLE;
    error = MCDE_SetChCConfig(MCDE_PANEL_C0,&configuration);
    
 	MCDE_ChC_FlowEnable(MCDE_FLOW_ENABLE);
 
 	MCDE_ChC_PowerEnable(MCDE_POWER_ENABLE);
 	
 	MCDE_SetState(MCDE_ENABLE);
 	
 	dsi_conf.bit_swap = MCDE_DSI_NO_SWAP;
    dsi_conf.blanking = 0;
    dsi_conf.byte_swap = MCDE_DSI_NO_SWAP;
    dsi_conf.cmd_mode = MCDE_DSI_CMD_8;
    dsi_conf.packing  = MCDE_PACKING_RGB888_R ;
    dsi_conf.synchro  =  MCDE_DSI_OUT_VIDEO_DCS ;
    dsi_conf.vid_mode = MCDE_DSI_CMD_MODE ;
    dsi_conf.words_per_frame = 480*2593;/* */
    dsi_conf.words_per_packet =2593;/*1+864*3*/
    
    MCDE_SetDSIConfig(MCDE_DSI_CH_CMD0,&dsi_conf);
    
    MCDE_SetDSICommandWord(MCDE_DSI_CH_CMD0,0x3C,0x00);
    
    return(error);
	
}





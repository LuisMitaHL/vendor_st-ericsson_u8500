/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DIF panel Driver
* \author  ST-Ericsson
*/
/*****************************************************************************/


#include "mcde.h"
#include "dif_panel_c3_tpo.h"
#include "dif_panel_c3_tpo_p.h"
#include "hcl_defs.h"
#include "debug.h"

#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_DIF
#define MY_DEBUG_ID             myDebugID_DIF

/* For debug HCL */
#ifdef __DEBUG
extern t_dbg_level  MY_DEBUG_LEVEL_VAR_NAME;
extern t_dbg_id     MY_DEBUG_ID;
#endif
PRIVATE t_dif_error Dif_panel_send_cmd_data(t_uint16, t_uint16);

/****************************************************************************/
/*		 NAME :	PRIVATE t_dif_error   Dif_panel_send_cmd_data(); 	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_uint32, t_uint32                                          */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PRIVATE t_dif_error Dif_panel_send_cmd_data(t_uint16 command, t_uint16 data)
{
    t_dif_error error = DIF_OK;
    t_uint16    var1, var2;
    var1 = (t_uint16) (((command << 8) & 0xff00) | ((command >> 8) & 0x00ff));
    error = (t_dif_error)MCDE_WriteTxFIFO(MCDE_PANEL_C0,MCDE_TXFIFO_WRITE_COMMAND, var1);
    if (error != DIF_OK)
    {
        return(error);
    }

    var2 = (t_uint16) (((data << 8) & 0xff00) | ((data >> 8) & 0x00ff));
    error = (t_dif_error)MCDE_WriteTxFIFO(MCDE_PANEL_C0,MCDE_TXFIFO_WRITE_DATA, var2);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_ConfigEntryMode(); 	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_entry_mode_config  entrymode_config      */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  : t_dif_error                           				        */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_ConfigEntryMode(t_dif_panel_c3_tpo_entry_mode_config entrymode_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* Entry Mode Settings */
    /* AM:: Specifies the direction in which the address is updated*/
    switch (entrymode_config.am_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.am_status, DIF_PANEL_AM_POS, MASK_BIT3);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*ID0*/
    switch (entrymode_config.id0_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.id0_status, DIF_PANEL_ID0_POS, MASK_BIT4);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*ID1*/
    switch (entrymode_config.id1_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.id1_status, DIF_PANEL_ID1_POS, MASK_BIT5);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*HWM*/
    switch (entrymode_config.hwm_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.hwm_status, DIF_PANEL_HWM_POS, MASK_BIT8);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*IF18*/
    switch (entrymode_config.if18_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.if18_status, DIF_PANEL_IF18_POS, MASK_BIT11);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*BGR */
    switch (entrymode_config.bgr_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.bgr_status, DIF_PANEL_BGR_POS, MASK_BIT12);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*DFM0 */
    switch (entrymode_config.dfm0_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.dfm0_status, DIF_PANEL_DFM0_POS, MASK_BIT13);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*DFM1 */
    switch (entrymode_config.dfm1_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.dfm1_status, DIF_PANEL_DFM1_POS, MASK_BIT14);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*TRI*/
    switch (entrymode_config.tri_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, entrymode_config.tri_status, DIF_PANEL_TRI_POS, MASK_BIT15);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_ENTRY_MODE, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_SetFR_Period();      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_fr_period_config   fr_period_config      */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  : t_dif_error                           					    */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_SetFR_Period(t_dif_panel_c3_tpo_fr_period_config fr_period_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* FR period adjustment setting */
    /* DIVI*/
    switch (fr_period_config.onchip_osc_divisor)
    {
        case DIF_PANEL_C3_TPO_ONCHIP_DOTCLK_DIV_1:
        case DIF_PANEL_C3_TPO_ONCHIP_DOTCLK_DIV_2:
        case DIF_PANEL_C3_TPO_ONCHIP_DOTCLK_DIV_4:
        case DIF_PANEL_C3_TPO_ONCHIP_DOTCLK_DIV_8:
            DIF_PANEL_PUT_BITS(test_reg, fr_period_config.onchip_osc_divisor, DIF_PANEL_DIVI_POS, DIF_PANEL_DIVI_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*RTNI Values */
    DIF_PANEL_PUT_BITS(test_reg, fr_period_config.lines_per_1h, DIF_PANEL_RTNI_POS, DIF_PANEL_RTNI_MASK);
    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_FR_FREQ_ADJ_SET, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_AutoSeqCtrl();      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_autoseq_set   auto_seq_ctrl                    */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  : t_dif_error                           					    */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_AutoSeqCtrl(t_dif_panel_c3_tpo_autoseq_set autoseq_set)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    switch (autoseq_set.ctrl)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, autoseq_set.ctrl, DIF_PANEL_AUTO_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_AUTO_SEQ_CTRL, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_OscillatorSetting();      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_state   auto_seq_ctrl                    */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  : t_dif_error                           					    */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_OscillatorSetting(t_dif_panel_c3_tpo_state osc_set)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    switch (osc_set)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, osc_set, DIF_PANEL_OSC_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_OSCILLATION_SET, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_LCD_DrvSetting();      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_state   auto_seq_ctrl                    */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  : t_dif_error                           					    */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_LCD_DrvSetting(t_dif_panel_c3_tpo_lcd_drv_signal lcd_drv_sig)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    switch (lcd_drv_sig)
    {
        case DIF_PANEL_C3_TPO_FRAME_AC_WAVEFORM:
        case DIF_PANEL_C3_TPO_LINE_AC_WAVEFORM:
            DIF_PANEL_PUT_BITS(test_reg, lcd_drv_sig, DIF_PANEL_LCD_DRV_POS, MASK_BIT9);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LCD_DRV_SIGNAL_SET, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_LCD_DrvSetting();      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_state   auto_seq_ctrl                    */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  : t_dif_error                           					    */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_AmpCapSetting(t_dif_panel_c3_tpo_amp_cap_set amp_cap_set)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    switch (amp_cap_set)
    {
        case DIF_PANEL_C3_TPO_MAX:
        case DIF_PANEL_C3_TPO_OFF:
        case DIF_PANEL_C3_TPO_OPTIMUM:
        case DIF_PANEL_C3_TPO_MINIMUM:
            DIF_PANEL_PUT_BITS(test_reg, amp_cap_set, DIF_PANEL_LCD_AMP_POS, DIF_PANEL_LCD_AMP_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_AMP_CAP_SET, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_ConfigOSD();      	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_osd_set   osd_set                        */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  : t_dif_error                           					    */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_ConfigOSD(t_dif_panel_c3_tpo_osd_set osd_set)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* OSD Function ON/OFF Settings */
    /* OSDON*/
    switch (osd_set.feature)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, osd_set.feature, DIF_PANEL_OSDON_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*OSDW*/
    switch (osd_set.addr_mode)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, osd_set.addr_mode, DIF_PANEL_OSDW_POS, MASK_BIT8);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_OSD_ON_OFF, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_ConfigDisplayMode();  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_disp_mode_config  disp_mode_config       */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :  t_dif_error                          						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_ConfigDisplayMode(t_dif_panel_c3_tpo_disp_mode_config disp_mode_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* Display Mode Setting(1) */
    /* REV*/
    switch (disp_mode_config.rev_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.rev_status, DIF_PANEL_REV_POS, MASK_BIT2);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* NBW*/
    switch (disp_mode_config.nbw_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.nbw_status, DIF_PANEL_NBW_POS, MASK_BIT4);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* PT0*/
    switch (disp_mode_config.pt0_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.pt0_status, DIF_PANEL_PT0_POS, MASK_BIT6);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* PT1*/
    switch (disp_mode_config.pt1_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.pt1_status, DIF_PANEL_PT1_POS, MASK_BIT7);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* SPT*/
    switch (disp_mode_config.split_screen_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.split_screen_status, DIF_PANEL_SPT_POS, MASK_BIT8);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* VLE1*/
    switch (disp_mode_config.screen1)
    {
        case DIF_PANEL_C3_TPO_FIXED_DISP:
        case DIF_PANEL_C3_TPO_SCROLL_DISP:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.screen1, DIF_PANEL_VLE1_POS, MASK_BIT9);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* VLE2*/
    switch (disp_mode_config.screen2)
    {
        case DIF_PANEL_C3_TPO_FIXED_DISP:
        case DIF_PANEL_C3_TPO_SCROLL_DISP:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.screen2, DIF_PANEL_VLE2_POS, MASK_BIT10);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* COL0,COL1 */
    switch (disp_mode_config.color_mode)
    {
        case DIF_PANEL_C3_TPO_262K_COLORS:
        case DIF_PANEL_C3_TPO_65K_COLORS:
        case DIF_PANEL_C3_TPO_8_COLORS:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.color_mode, DIF_PANEL_COL_POS, DIF_PANEL_COL_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_DISP_MODE_1, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0x0808;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* Display Mode Setting(2) */
    /* Vertical Back Porch*/
    DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.vertical_back_porch, DIF_PANEL_BP_POS, DIF_PANEL_BP_MASK);
    data_reg |= test_reg;

    /* Vertical Front Porch*/
    test_reg = 0;
    DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.vertical_front_porch, DIF_PANEL_FP_POS, DIF_PANEL_FP_MASK);
    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_DISP_MODE_2, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0x1;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* Display Mode Setting(3) */
    /* RSE*/
    switch (disp_mode_config.refresh_state)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.refresh_state, DIF_PANEL_RSE_POS, MASK_BIT7);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*RSB:: Magnification Factor*/
    switch (disp_mode_config.magn_factor)
    {
        case DIF_PANEL_C3_TPO_MAGN_FACTOR_2:
        case DIF_PANEL_C3_TPO_MAGN_FACTOR_4:
        case DIF_PANEL_C3_TPO_MAGN_FACTOR_8:
        case DIF_PANEL_C3_TPO_MAGN_FACTOR_16:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.magn_factor, DIF_PANEL_RSB_POS, DIF_PANEL_RSB_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*RSH:: No. of Refresh Fields*/
    test_reg = 0;
    DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.refresh_fields, DIF_PANEL_RSH_POS, DIF_PANEL_RSH_MASK);
    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_DISP_MODE_3, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* Display Mode Setting(4) */
    /* SIP*/
    switch (disp_mode_config.sip_stat)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.sip_stat, DIF_PANEL_SIP_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*X2 Y2 :Specifies the direction in which the image is expanded*/
    switch (disp_mode_config.xy_expansion)
    {
        case DIF_PANEL_C3_TPO_XY_EXP_DISABLED:
        case DIF_PANEL_C3_TPO_Y_DIR_EXP:
        case DIF_PANEL_C3_TPO_X_DIR_EXP:
        case DIF_PANEL_C3_TPO_BOTH_DIR_EXP:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.xy_expansion, DIF_PANEL_X2Y2_POS, DIF_PANEL_X2Y2_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*XYON*/
    switch (disp_mode_config.xy_expansion_state)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, disp_mode_config.xy_expansion_state, DIF_PANEL_XYON_POS, MASK_BIT7);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_DISP_MODE_4, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_ConfigExtDisplayMode(); 	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_ext_disp_config  ext_disp_config         */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :    t_dif_error             						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_ConfigExtDisplayMode(t_dif_panel_c3_tpo_ext_disp_config ext_disp_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* External Display Signal Setting(1) */
    /* RM*/
    switch (ext_disp_config.ram_access_interface)
    {
        case DIF_PANEL_C3_TPO_MPU_OR_VSYNC:
        case DIF_PANEL_C3_TPO_RGB:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.ram_access_interface, DIF_PANEL_RM_POS, MASK_BIT8);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* RIM0 , RIM1*/
    switch (ext_disp_config.interface_width)
    {
        case DIF_PANEL_C3_TPO_18_BIT:
        case DIF_PANEL_C3_TPO_16_BIT:
        case DIF_PANEL_C3_TPO_6_BIT:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.interface_width, DIF_PANEL_RIM_POS, DIF_PANEL_RIM_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* DM0 , DM1*/
    switch (ext_disp_config.sync_signal)
    {
        case DIF_PANEL_C3_TPO_INTERNAL_CLK_OPER:
        case DIF_PANEL_C3_TPO_RGB_INTERFACE:
        case DIF_PANEL_C3_TPO_VSYNC_INTERFACE:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.sync_signal, DIF_PANEL_DM_POS, DIF_PANEL_DM_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_EXT_DISP_SIGNAL_SET_1, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* External Display Signal Setting(2) */
    /* DIVE0, DIVE1 */
    switch (ext_disp_config.dotclk_divisor)
    {
        case DIF_PANEL_C3_TPO_DOTCLK_DIV_4:
        case DIF_PANEL_C3_TPO_DOTCLK_DIV_8:
        case DIF_PANEL_C3_TPO_DOTCLK_DIV_16:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.dotclk_divisor, DIF_PANEL_DIVE_POS, DIF_PANEL_DIVE_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*RTNE*/
    DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.line_period, DIF_PANEL_RTNE_POS, DIF_PANEL_RTNE_MASK);
    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_EXT_DISP_SIGNAL_SET_2, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* External Display Signal Setting(3) */
    /* DPL*/
    switch (ext_disp_config.dotclk_polarity)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.dotclk_polarity, DIF_PANEL_DPL_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*EPL*/
    switch (ext_disp_config.enable_polarity)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.enable_polarity, DIF_PANEL_EPL_POS, MASK_BIT1);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*VPL*/
    switch (ext_disp_config.vld_polarity)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.vld_polarity, DIF_PANEL_VPL_POS, MASK_BIT2);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*HSPL*/
    switch (ext_disp_config.hsync_polarity)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.hsync_polarity, DIF_PANEL_HSPL_POS, MASK_BIT3);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*VSPL*/
    switch (ext_disp_config.vsync_polarity)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, ext_disp_config.vsync_polarity, DIF_PANEL_VSPL_POS, MASK_BIT4);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_EXT_DISP_SIGNAL_SET_3, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_SetResolution(); 	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the PBC with default values         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_resolution_set    resolution_set         */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  : t_dif_error                            			 	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PRIVATE                                             */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_SetResolution(t_dif_panel_c3_tpo_resolution_set resolution_set)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* Horizontal Back Porch Setting */
    /* HBP4, HBP3, HBP2, HBP1, HBP0*/
    DIF_PANEL_PUT_BITS(test_reg, resolution_set.horizontal_back_porch, DIF_PANEL_HBP_POS, DIF_PANEL_HBP_MASK);
    data_reg |= test_reg;

    /* Horizontal Valid Width Setting */
    test_reg = 0;
    DIF_PANEL_PUT_BITS(test_reg, resolution_set.horizontal_width, DIF_PANEL_HWS_POS, DIF_PANEL_HWS_MASK);
    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_HOR_VALID_SET, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* Driver Output Control Setting */
    /* SS*/
    switch (resolution_set.driver_shift)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, resolution_set.driver_shift, DIF_PANEL_SS_POS, MASK_BIT8);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* NL*/
    switch (resolution_set.disp_size)
    {
        case DIF_PANEL_C3_TPO_DISP_256_16_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_24_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_32_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_40_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_48_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_56_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_64_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_72_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_80_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_88_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_96_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_104_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_112_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_120_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_128_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_136_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_144_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_152_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_160_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_168_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_176_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_184_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_192_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_200_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_208_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_216_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_224_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_232_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_240_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_248_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_256_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_264_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_272_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_280_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_288_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_296_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_304_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_312_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_320_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_328_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_336_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_344_SIZE:
        case DIF_PANEL_C3_TPO_DISP_256_352_SIZE:
            DIF_PANEL_PUT_BITS(test_reg, resolution_set.disp_size, DIF_PANEL_NL_POS, DIF_PANEL_NL_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_DRV_OP_CTRL_SET, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*		 NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_LTPS_Config(); 	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_ltps_ctrl_set                            */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_LTPS_Config(t_dif_panel_c3_tpo_ltps_ctrl_set ltps_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* LTPS control setting(1) */
    /* CLT1-0 :: Specifies the position at which ASW 1 rises*/
    data_reg = 0x100;
    switch (ltps_config.ltps_asw_rise_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_asw_rise_pos, DIF_PANEL_CLTI_POS, DIF_PANEL_CLTI_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg = test_reg;
    test_reg = 0;

    /* CLWI0-3 :: Specifies high width for ASW1,ASW2,ASW3 */
    DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_asw_high_width, DIF_PANEL_CLWI_POS, DIF_PANEL_CLWI_MASK);

    /*-------- write control register of panel---- */
    data_reg |= test_reg;
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LTPS_CTRL_SET_1, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* LTPS control setting(2) */
    /* OEVFI */
    switch (ltps_config.ltps_oe_rise_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_oe_rise_pos, DIF_PANEL_OEVFI_POS, DIF_PANEL_OEVFI_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg = test_reg;
    test_reg = 0;

    /* OEVBI*/
    switch (ltps_config.ltps_oe_rise_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_oe_rise_pos, DIF_PANEL_OEVBI_POS, DIF_PANEL_OEVBI_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /*-------- write control register of panel---- */
    data_reg |= test_reg;

    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LTPS_CTRL_SET_2, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* LTPS control setting(3) */
    /* SHI0-1*/
    switch (ltps_config.ltps_asw_fall_hold_time)
    {
        case DIF_PANEL_C3_TPO_HALF_CLK:
        case DIF_PANEL_C3_TPO_ONE_CLK:
        case DIF_PANEL_C3_TPO_ONE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_TWO_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_asw_fall_hold_time, DIF_PANEL_SHI_POS, DIF_PANEL_SHI_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LTPS_CTRL_SET_3, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;

    /*-------------------------------------------------------------------*/
    /* LTPS control setting(4) */
    /* CKBI */
    switch (ltps_config.ltps_ckv_rise_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ckv_rise_pos, DIF_PANEL_CKBI_POS, DIF_PANEL_CKBI_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg = test_reg;
    test_reg = 0;

    /* CKFI*/
    switch (ltps_config.ltps_ckv_fall_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ckv_fall_pos, DIF_PANEL_CKFI_POS, DIF_PANEL_CKFI_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /*-------- write control register of panel---- */
    data_reg |= test_reg;

    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LTPS_CTRL_SET_4, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0x100;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* LTPS control setting(5) */
    /* CLTE3-0 */
    switch (ltps_config.ltps_ext_asw_rise_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ext_asw_rise_pos, DIF_PANEL_CLTE_POS, DIF_PANEL_CLTE_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg = test_reg;
    test_reg = 0;

    /* CLWE */
    DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ext_asw_high_width, DIF_PANEL_CLWE_POS, DIF_PANEL_CLWE_MASK);

    /*-------- write control register of panel---- */
    data_reg |= test_reg;

    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LTPS_CTRL_SET_5, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* LTPS control setting(6) */
    /* OEVBE */
    switch (ltps_config.ltps_ext_oe_rise_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ext_oe_rise_pos, DIF_PANEL_OEVBE_POS, DIF_PANEL_OEVBE_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg = test_reg;
    test_reg = 0;

    /* OEVFE */
    switch (ltps_config.ltps_ext_oe_fall_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ext_oe_fall_pos, DIF_PANEL_OEVFE_POS, DIF_PANEL_OEVFE_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /*-------- write control register of panel---- */
    data_reg |= test_reg;

    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LTPS_CTRL_SET_6, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* LTPS control setting(7) */
    /* SHE */
    switch (ltps_config.ltps_ext_asw_fall_hold_time)
    {
        case DIF_PANEL_C3_TPO_HALF_CLK:
        case DIF_PANEL_C3_TPO_ONE_CLK:
        case DIF_PANEL_C3_TPO_ONE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_TWO_CLK:
        case DIF_PANEL_C3_TPO_TWO_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_THREE_CLK:
        case DIF_PANEL_C3_TPO_THREE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_FOUR_CLK:
        case DIF_PANEL_C3_TPO_FOUR_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_FIVE_CLK:
        case DIF_PANEL_C3_TPO_FIVE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_SIX_CLK:
        case DIF_PANEL_C3_TPO_SIX_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_SEVEN_CLK:
        case DIF_PANEL_C3_TPO_SEVEN_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_EIGHT_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ext_asw_fall_hold_time, DIF_PANEL_SHE_POS, DIF_PANEL_SHE_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /*-------- write control register of panel---- */
    data_reg |= test_reg;
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LTPS_CTRL_SET_7, data_reg);
    if (error != DIF_OK)
    {
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* LTPS control setting(8) */
    /* CKBE */
    switch (ltps_config.ltps_ext_ckv_rise_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ext_oe_rise_pos, DIF_PANEL_CKBE_POS, DIF_PANEL_CKBE_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg = test_reg;
    test_reg = 0;

    /* CKFE */
    switch (ltps_config.ltps_ext_ckv_fall_pos)
    {
        case DIF_PANEL_C3_TPO_POS_ZERO_CLK:
        case DIF_PANEL_C3_TPO_POS_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_CLK:
        case DIF_PANEL_C3_TPO_POS_ONE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_CLK:
        case DIF_PANEL_C3_TPO_POS_TWO_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_CLK:
        case DIF_PANEL_C3_TPO_POS_THREE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_CLK:
        case DIF_PANEL_C3_TPO_POS_FOUR_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_CLK:
        case DIF_PANEL_C3_TPO_POS_FIVE_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_CLK:
        case DIF_PANEL_C3_TPO_POS_SIX_AND_HALF_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_CLK:
        case DIF_PANEL_C3_TPO_POS_SEVEN_AND_HALF_CLK:
            DIF_PANEL_PUT_BITS(test_reg, ltps_config.ltps_ext_oe_fall_pos, DIF_PANEL_CKFE_POS, DIF_PANEL_CKFE_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /*-------- write control register of panel---- */
    data_reg |= test_reg;
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_LTPS_CTRL_SET_8, data_reg);
    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_Display_Config(); 	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:         */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_disp_config                            */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_Display_Config(t_dif_panel_c3_tpo_disp_config display_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*-------------------------------------------------------------------*/
    /* DISPLAY  Control  */
    /* ASW1-0 :: Turns on/off ASW signal generated from TPO panel and varies the level of the ASW*/
    switch (display_config.asw_level)
    {
        case DIF_PANEL_C3_TPO_FIXED_VSS_0:
        case DIF_PANEL_C3_TPO_FIXED_VSS_1:
        case DIF_PANEL_C3_TPO_FIXED_VDD:
        case DIF_PANEL_C3_TPO_NORMAL_DRIVE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.asw_level, DIF_PANEL_ASW_POS, DIF_PANEL_ASW_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* D1-0, specifies the state of the output */
    switch (display_config.output_pin_state)
    {
        case DIF_PANEL_C3_TPO_OP_VSS:
        case DIF_PANEL_C3_TPO_OP_WHITE_DATA:
        case DIF_PANEL_C3_TPO_OP_NORMAL_DRIVE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.output_pin_state, DIF_PANEL_OP_PIN_POS, DIF_PANEL_OP_PIN_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*PO :triggers sleep mode*/
    switch (display_config.po_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.po_status, DIF_PANEL_PO_PIN_POS, MASK_BIT15);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*CONT : Turns on/off internal signals generated from panel*/
    switch (display_config.cont_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.cont_status, DIF_PANEL_CONT_POS, MASK_BIT14);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* PEV : Turns on/off voltage booster for LCD */
    switch (display_config.pev_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.pev_status, DIF_PANEL_PEV_POS, MASK_BIT13);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* DCEV : Turns on/off DCCK,/DCCK and DCEV signals  */
    /*These signals are used for voltage booster ckt */
    switch (display_config.dcev_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.dcev_status, DIF_PANEL_DCEV_POS, MASK_BIT12);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* UD : Setting of gate scan direction,which uses gate ckt on lcd  */
    switch (display_config.ud_scan)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.ud_scan, DIF_PANEL_UD_POS, MASK_BIT11);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* CON : Turns on/off the STV,CKV1 and CKV2 signals generated from panel  */
    switch (display_config.con_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.con_status, DIF_PANEL_CON_POS, MASK_BIT10);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* OEV : Turns on/off the OEV signal  */
    switch (display_config.oev_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.oev_status, DIF_PANEL_OEV_POS, MASK_BIT9);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* VCS : Turns on/off the VCS signal    */
    switch (display_config.vcs_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.vcs_status, DIF_PANEL_VCS_POS, MASK_BIT8);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* FR : Turns on/off the FR signal    */
    switch (display_config.fr_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.fr_status, DIF_PANEL_FR_POS, MASK_BIT3);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* FDON : Turns on/off the FDON signal    */
    switch (display_config.fdon_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.fdon_status, DIF_PANEL_FDON_POS, MASK_BIT2);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* VGAM : Turns on/off the VGAM power supply    */
    switch (display_config.vgam_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.vgam_status, DIF_PANEL_VGAM_POS, MASK_BIT1);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* DCG : Turns on/off the DCG signal generated from panel    */
    switch (display_config.dcg_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, display_config.dcg_status, DIF_PANEL_DCG_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /*-------- write control register of panel---- */
    data_reg |= test_reg;
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_DISP_CTRL, data_reg);
    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_Power_OffLine_Setting(); 	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                       */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_power_offline_set                            */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_Power_OffLine_Setting(t_dif_panel_c3_tpo_power_offline_set offline_power)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /* Power offline settings  */
    switch (offline_power)
    {
        case DIF_PANEL_C3_TPO_2H_TIME:
        case DIF_PANEL_C3_TPO_3H_TIME:
        case DIF_PANEL_C3_TPO_4H_TIME:
        case DIF_PANEL_C3_TPO_5H_TIME:
        case DIF_PANEL_C3_TPO_6H_TIME:
        case DIF_PANEL_C3_TPO_7H_TIME:
        case DIF_PANEL_C3_TPO_8H_TIME:
        case DIF_PANEL_C3_TPO_9H_TIME:
        case DIF_PANEL_C3_TPO_10H_TIME:
        case DIF_PANEL_C3_TPO_11H_TIME:
        case DIF_PANEL_C3_TPO_12H_TIME:
        case DIF_PANEL_C3_TPO_13H_TIME:
        case DIF_PANEL_C3_TPO_14H_TIME:
        case DIF_PANEL_C3_TPO_15H_TIME:
        case DIF_PANEL_C3_TPO_16H_TIME:
            DIF_PANEL_PUT_BITS(test_reg, offline_power, DIF_PANEL_POFH_POS, DIF_PANEL_POFH_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /*-------- write control register of panel---- */
    data_reg |= test_reg;
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_POW_OFF_LINE_COUNT_SET, data_reg);
    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_Power_Config(); 	        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                       */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_power_config                            */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_Power_Config(t_dif_panel_c3_tpo_power_config power_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /* Power Supply  Control(1)  */
    /* VCD3-0 Specifies the VCOND output voltage */
    switch (power_config.vcond_output_voltage)
    {
        case DIF_PANEL_C3_TPO_VCOMD_1_4V:
        case DIF_PANEL_C3_TPO_VCOMD_1_5V:
        case DIF_PANEL_C3_TPO_VCOMD_1_6V:
        case DIF_PANEL_C3_TPO_VCOMD_1_7V:
        case DIF_PANEL_C3_TPO_VCOMD_1_8V:
        case DIF_PANEL_C3_TPO_VCOMD_1_9V:
        case DIF_PANEL_C3_TPO_VCOMD_2_0V:
        case DIF_PANEL_C3_TPO_VCOMD_2_1V:
        case DIF_PANEL_C3_TPO_VCOMD_2_2V:
        case DIF_PANEL_C3_TPO_VCOMD_2_3V:
        case DIF_PANEL_C3_TPO_VCOMD_2_4V:
        case DIF_PANEL_C3_TPO_VCOMD_2_5V:
        case DIF_PANEL_C3_TPO_VCOMD_2_6V:
        case DIF_PANEL_C3_TPO_VCOMD_2_7V:
        case DIF_PANEL_C3_TPO_VCOMD_2_8V:
        case DIF_PANEL_C3_TPO_VCOMD_2_9V:
            DIF_PANEL_PUT_BITS(test_reg, power_config.vcond_output_voltage, DIF_PANEL_VCOND_POS, DIF_PANEL_VCOND_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* VCS3-0 Specifies the VCS output voltage */
    switch (power_config.vcs_output_voltage)
    {
        case DIF_PANEL_C3_TPO_VCS_3_0V:
        case DIF_PANEL_C3_TPO_VCS_3_1V:
        case DIF_PANEL_C3_TPO_VCS_3_2V:
        case DIF_PANEL_C3_TPO_VCS_3_3V:
        case DIF_PANEL_C3_TPO_VCS_3_4V:
        case DIF_PANEL_C3_TPO_VCS_3_5V:
        case DIF_PANEL_C3_TPO_VCS_3_6V:
        case DIF_PANEL_C3_TPO_VCS_3_7V:
        case DIF_PANEL_C3_TPO_VCS_3_8V:
        case DIF_PANEL_C3_TPO_VCS_3_9V:
        case DIF_PANEL_C3_TPO_VCS_4_0V:
        case DIF_PANEL_C3_TPO_VCS_4_1V:
        case DIF_PANEL_C3_TPO_VCS_4_2V:
        case DIF_PANEL_C3_TPO_VCS_4_3V:
        case DIF_PANEL_C3_TPO_VCS_4_4V:
        case DIF_PANEL_C3_TPO_VCS_4_5V:
            DIF_PANEL_PUT_BITS(test_reg, power_config.vcs_output_voltage, DIF_PANEL_VCS_OP_POS, DIF_PANEL_VCS_OP_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* VGM3-0 Specifies the VGM output voltage */
    switch (power_config.vgm_output_voltage)
    {
        case DIF_PANEL_C3_TPO_VGM_3_7V:
        case DIF_PANEL_C3_TPO_VGM_3_8V:
        case DIF_PANEL_C3_TPO_VGM_3_9V:
        case DIF_PANEL_C3_TPO_VGM_4_0V:
        case DIF_PANEL_C3_TPO_VGM_4_1V:
        case DIF_PANEL_C3_TPO_VGM_4_2V:
        case DIF_PANEL_C3_TPO_VGM_4_3V:
        case DIF_PANEL_C3_TPO_VGM_4_4V:
        case DIF_PANEL_C3_TPO_VGM_4_5V:
        case DIF_PANEL_C3_TPO_VGM_4_6V:
        case DIF_PANEL_C3_TPO_VGM_4_7V:
        case DIF_PANEL_C3_TPO_VGM_4_8V:
        case DIF_PANEL_C3_TPO_VGM_4_9V:
        case DIF_PANEL_C3_TPO_VGM_5_0V:
        case DIF_PANEL_C3_TPO_VGM_5_1V:
        case DIF_PANEL_C3_TPO_VGM_5_2V:
            DIF_PANEL_PUT_BITS(test_reg, power_config.vgm_output_voltage, DIF_PANEL_VGM_POS, DIF_PANEL_VGM_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_POW_SUPPLY_CTRL_1, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-----------------------------------------------------------------------------------*/
    /* Power Supply  Control(2)  */
    /* XVD2-0 : XVDD output voltage */
    switch (power_config.xvdd_output_voltage)
    {
        case DIF_PANEL_C3_TPO_XVDD_4_7V:
        case DIF_PANEL_C3_TPO_XVDD_4_8V:
        case DIF_PANEL_C3_TPO_XVDD_4_9V:
        case DIF_PANEL_C3_TPO_XVDD_5_0V:
        case DIF_PANEL_C3_TPO_XVDD_5_1V:
        case DIF_PANEL_C3_TPO_XVDD_5_2V:
        case DIF_PANEL_C3_TPO_XVDD_5_3V:
        case DIF_PANEL_C3_TPO_XVDD_5_4V:
            DIF_PANEL_PUT_BITS(test_reg, power_config.xvdd_output_voltage, DIF_PANEL_XVDD_POS, DIF_PANEL_XVDD_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* WSEL1 */
    switch (power_config.wsel_avdd)
    {
        case DIF_PANEL_C3_TPO_DUAL_MODE:
        case DIF_PANEL_C3_TPO_SINGLE_MODE:
            DIF_PANEL_PUT_BITS(test_reg, power_config.wsel_avdd, DIF_PANEL_WSEL1_POS, MASK_BIT12);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*WSEL2 */
    switch (power_config.wsel_xvdd)
    {
        case DIF_PANEL_C3_TPO_DUAL_MODE:
        case DIF_PANEL_C3_TPO_SINGLE_MODE:
            DIF_PANEL_PUT_BITS(test_reg, power_config.wsel_xvdd, DIF_PANEL_WSEL2_POS, MASK_BIT13);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_POW_SUPPLY_CTRL_2, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-----------------------------------------------------------------------------------*/
    /* Power Supply  Control(3)  */
    /* BAV */
    switch (power_config.bav_avdd)
    {
        case DIF_PANEL_C3_TPO_X2_MODE:
        case DIF_PANEL_C3_TPO_X3_MODE:
            DIF_PANEL_PUT_BITS(test_reg, power_config.bav_avdd, DIF_PANEL_BAV_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*BXV */
    switch (power_config.bxv_xvdd)
    {
        case DIF_PANEL_C3_TPO_X2_MODE:
        case DIF_PANEL_C3_TPO_X3_MODE:
            DIF_PANEL_PUT_BITS(test_reg, power_config.bxv_xvdd, DIF_PANEL_BXV_POS, MASK_BIT1);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_POW_SUPPLY_CTRL_3, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    data_reg = 0;
    test_reg = 0;

    /*-----------------------------------------------------------------------------------*/
    /* Power Supply  Control(4)  */
    /* DCW1-0: Set mask period of DCCLK signal */
    switch (power_config.dcclk_msk_period)
    {
        case DIF_PANEL_C3_TPO_HALF:
        case DIF_PANEL_C3_TPO_ONE:
        case DIF_PANEL_C3_TPO_ONE_HALF:
        case DIF_PANEL_C3_TPO_TWO:
            DIF_PANEL_PUT_BITS(test_reg, power_config.dcclk_msk_period, DIF_PANEL_DCW_POS, DIF_PANEL_DCW_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* CK11-10 : Set the divided level of regulate clock for AVDD ckt */
    switch (power_config.avdd_regulate_ckt)
    {
        case DIF_PANEL_C3_TPO_REGULATE_HALF:
        case DIF_PANEL_C3_TPO_REGULATE_ONE:
        case DIF_PANEL_C3_TPO_REGULATE_TWO:
        case DIF_PANEL_C3_TPO_REGULATE_FOUR:
            DIF_PANEL_PUT_BITS(test_reg, power_config.avdd_regulate_ckt, DIF_PANEL_AVDD_REG_POS, DIF_PANEL_AVDD_REG_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    /* CK21-20 : Set the divided level of regulate clock for XVDD ckt */
    switch (power_config.xvdd_regulate_ckt)
    {
        case DIF_PANEL_C3_TPO_REGULATE_HALF:
        case DIF_PANEL_C3_TPO_REGULATE_ONE:
        case DIF_PANEL_C3_TPO_REGULATE_TWO:
        case DIF_PANEL_C3_TPO_REGULATE_FOUR:
            DIF_PANEL_PUT_BITS(test_reg, power_config.xvdd_regulate_ckt, DIF_PANEL_XVDD_REG_POS, DIF_PANEL_XVDD_REG_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* CK31-30 : Set the divided level of regulate clock for XVDD ckt */
    switch (power_config.ext_regulate_ckt)
    {
        case DIF_PANEL_C3_TPO_REGULATE_HALF:
        case DIF_PANEL_C3_TPO_REGULATE_ONE:
        case DIF_PANEL_C3_TPO_REGULATE_TWO:
        case DIF_PANEL_C3_TPO_REGULATE_FOUR:
            DIF_PANEL_PUT_BITS(test_reg, power_config.ext_regulate_ckt, DIF_PANEL_EXT_REG_POS, DIF_PANEL_EXT_REG_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel---- */
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_POW_SUPPLY_CTRL_4, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*-----------------------------------------------------------------------------------*/
    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_ExtPolarityConfig(); 	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_ext_polarity_ctrl                        */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_ExtPolarityConfig(t_dif_panel_c3_tpo_ext_polarity_ctrl polarity_ctrl)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /* External polarity control */
    /* EXTC1 */
    switch (polarity_ctrl.extc1_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, polarity_ctrl.extc1_status, DIF_PANEL_EXTC1_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* EXTC2 */
    switch (polarity_ctrl.extc2_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, polarity_ctrl.extc1_status, DIF_PANEL_EXTC2_POS, MASK_BIT1);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*-------- write control register of panel---- */
    data_reg |= test_reg;
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_EXT_POLARITY_CTRL, data_reg);
    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_AddrConfig(); 	    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_addr_config                              */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_AddrConfig(t_dif_panel_c3_tpo_addr_config addr_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;

    /* Horizontal /vertical RAM address location */
    /*Horizontal window Start address*/
    data_reg = addr_config.hor_start_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_HOR_RAM_START_ADDR, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* Horizontal window End address*/
    data_reg = 0;
    data_reg = addr_config.hor_end_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_HOR_RAM_END_ADDR, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* Vertical window Start address*/
    data_reg = 0;
    data_reg = addr_config.ver_start_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_VER_RAM_START_ADDR, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* Vertical window End address*/
    data_reg = 0;
    data_reg = addr_config.ver_end_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_VER_RAM_END_ADDR, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* First Second screen drive position */
    /*Screen1   Start address*/
    data_reg = 0;
    data_reg = addr_config.screen1_start_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_FIRST_SCREEN_START_ADDR, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* Screen1   End address*/
    data_reg = 0;
    data_reg = addr_config.screen1_end_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_FIRST_SCREEN_END_ADDR, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* Screen2 Start address*/
    data_reg = 0;
    data_reg = addr_config.screen2_start_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_SECOND_SCREEN_START_ADDR, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* Screen2 End address*/
    data_reg = 0;
    data_reg = addr_config.screen2_end_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_SECOND_SCREEN_END_ADDR, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* RAM address settings */
    /* X page address*/
    data_reg = 0;
    data_reg = addr_config.y_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_RAM_ADD_SET_1, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* Y-page address*/
    data_reg = 0;
    data_reg = addr_config.x_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_RAM_ADD_SET_2, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* OSD first/second screen start addresses */
    /* OSD first screen addr*/
    data_reg = 0;
    data_reg = addr_config.osd_screen1_start_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_OSD_SCREEN_1_START_ADD, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*SD Second screen addr*/
    data_reg = 0;
    data_reg = addr_config.osd_screen2_start_addr;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_OSD_SCREEN_2_START_ADD, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*-----------------------------------------------------------------------------*/
    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error   DIF_PANEL_C3_TPO_GrayScaleAmtConfig();     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	t_dif_panel_c3_tpo_gray_scale_amt_config                    */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_GrayScaleAmtConfig(t_dif_panel_c3_tpo_gray_scale_amt_config offset)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /* Setting of gray scale offset amounts*/
    /*BLON: Select offset on/off of gray scale offset */
    switch (offset.blon_status)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, offset.blon_status, DIF_PANEL_BLON_POS, MASK_BIT8);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*BUP2-0 : Adjust volatage between L63 and L62, L1 and L0*/
    switch (offset.bup_amount)
    {
        case DIF_PANEL_C3_TPO_0_0V:
        case DIF_PANEL_C3_TPO_0_1V:
        case DIF_PANEL_C3_TPO_0_2V:
        case DIF_PANEL_C3_TPO_0_3V:
        case DIF_PANEL_C3_TPO_0_4V:
        case DIF_PANEL_C3_TPO_0_5V:
        case DIF_PANEL_C3_TPO_0_6V:
        case DIF_PANEL_C3_TPO_0_7V:
            DIF_PANEL_PUT_BITS(test_reg, offset.bup_amount, DIF_PANEL_BUP_POS, DIF_PANEL_BUP_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /*BOFS2-0 : Set gray scale offset amounts */
    switch (offset.bofs_amount)
    {
        case DIF_PANEL_C3_TPO_0_0V:
        case DIF_PANEL_C3_TPO_0_1V:
        case DIF_PANEL_C3_TPO_0_2V:
        case DIF_PANEL_C3_TPO_0_3V:
        case DIF_PANEL_C3_TPO_0_4V:
        case DIF_PANEL_C3_TPO_0_5V:
        case DIF_PANEL_C3_TPO_0_6V:
        case DIF_PANEL_C3_TPO_0_7V:
            DIF_PANEL_PUT_BITS(test_reg, offset.bofs_amount, DIF_PANEL_BOFS_POS, DIF_PANEL_BOFS_MASK);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_GRAY_SCALE_OFFSET_SET, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*-----------------------------------------------------------------------------*/
    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error  DIF_PANEL_C3_TPO_CtrlVertScroll             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	    t_dif_panel_c3_tpo_vert_scroll                          */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_CtrlVertScroll(t_dif_panel_c3_tpo_vert_scroll scroll_lines)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;

    //t_uint16    test_reg=0;
    /* Vertical scroll control*/
    /*First Screen */
    data_reg = scroll_lines.screen1_lines;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_VER_SCROLL_CTRL_1, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*-----------------------------------------------------------------------------*/
    data_reg = 0;

    /*Second Screen */
    data_reg = scroll_lines.screen2_lines;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_VER_SCROLL_CTRL_2, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error  DIF_PANEL_C3_TPO_RAM_DataWrite             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	    t_uint16                                                */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_RAM_DataWrite(t_uint16 data)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;

    //t_uint16    test_reg=0;
    data_reg = data;

    /* RAM data write */
    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_RAM_DATA_WRITE, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error  DIF_PANEL_C3_TPO_DisplayRAM_Mask             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	  t_uint8,t_uint8,t_uint8                                       */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_DisplayRAM_Mask(t_uint8 blue_mask, t_uint8 green_mask, t_uint8 red_mask)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /* Graphics operation 1 */
    DIF_PANEL_PUT_BITS(test_reg, blue_mask, DIF_PANEL_BLUE_POS, DIF_PANEL_BLUE_MASK);
    data_reg |= test_reg;
    test_reg = 0;
    DIF_PANEL_PUT_BITS(test_reg, green_mask, DIF_PANEL_GREEN_POS, DIF_PANEL_GREEN_MASK);

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_GRAPHIC_OP_1, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /* Graphics operation 2 */
    data_reg = red_mask;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_GRAPHIC_OP_2, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error  DIF_PANEL_C3_TPO_StandBy                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	  t_uint8,t_uint8,t_uint8                                   */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_StandBy(t_dif_panel_c3_tpo_state deep_standby, t_dif_panel_c3_tpo_state standby)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /* Mode Setting */
    /*DSTB: Specifies the deep standby mode */
    switch (deep_standby)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, deep_standby, DIF_PANEL_DSTB_POS, MASK_BIT2);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;
    test_reg = 0;

    /* STB : */
    switch (standby)
    {
        case DIF_PANEL_C3_TPO_BIT_DISABLE:
        case DIF_PANEL_C3_TPO_BIT_ENABLE:
            DIF_PANEL_PUT_BITS(test_reg, standby, DIF_PANEL_STB_POS, MASK_BIT0);
            break;

        default:
            {
                error = DIF_INVALID_PARAMETER;
                return(error);
            }
    }

    data_reg |= test_reg;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_MODE_SET, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error  DIF_PANEL_C3_TPO_GrayScaleConfig            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	  t_dif_panel_c3_tpo_gray_scale                             */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : None                                             	        */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_GrayScaleConfig(t_dif_panel_c3_tpo_gray_scale gray_scale_config)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;
    t_uint16    test_reg = 0;

    /*------------------------------------------------------------------------------------------*/
    /* Gray scale setting(1) */
    /* PK0 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.pk0_config, DIF_PANEL_PK0_POS, DIF_PANEL_PK0_MASK);

    data_reg |= test_reg;
    test_reg = 0;

    /* PK1 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.pk1_config, DIF_PANEL_PK1_POS, DIF_PANEL_PK1_MASK);

    data_reg |= test_reg;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_GRAY_SCALE_SET_1, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*------------------------------------------------------------------------------------------*/
    /* Gray scale setting(2) */
    test_reg = 0;
    data_reg = 0;

    /* PK2 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.pk2_config, DIF_PANEL_PK2_POS, DIF_PANEL_PK2_MASK);

    data_reg |= test_reg;
    test_reg = 0;

    /* PK3 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.pk3_config, DIF_PANEL_PK3_POS, DIF_PANEL_PK3_MASK);

    data_reg |= test_reg;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_GRAY_SCALE_SET_2, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*------------------------------------------------------------------------------------------*/
    /* Gray scale setting(3) */
    test_reg = 0;
    data_reg = 0;

    /* PK4 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.pk4_config, DIF_PANEL_PK2_POS, DIF_PANEL_PK4_MASK);

    data_reg |= test_reg;
    test_reg = 0;

    /* PK5 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.pk5_config, DIF_PANEL_PK3_POS, DIF_PANEL_PK5_MASK);

    data_reg |= test_reg;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_GRAY_SCALE_SET_3, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*------------------------------------------------------------------------------------------*/
    /* Gray scale setting(4) */
    test_reg = 0;
    data_reg = 0;

    /* PR0 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.pr0_config, DIF_PANEL_PR0_POS, DIF_PANEL_PR0_MASK);

    data_reg |= test_reg;
    test_reg = 0;

    /* PR1 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.pr1_config, DIF_PANEL_PR1_POS, DIF_PANEL_PR1_MASK);

    data_reg |= test_reg;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_GRAY_SCALE_SET_4, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*------------------------------------------------------------------------------------------*/
    /* Gray scale setting(5) */
    test_reg = 0;
    data_reg = 0;

    /* VR0 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.vr0_config, DIF_PANEL_VR0_POS, DIF_PANEL_VR0_MASK);

    data_reg |= test_reg;
    test_reg = 0;

    /* VR1 */
    DIF_PANEL_PUT_BITS(test_reg, gray_scale_config.vr1_config, DIF_PANEL_VR1_POS, DIF_PANEL_VR1_MASK);

    data_reg |= test_reg;

    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_GRAY_SCALE_SET_5, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    /*------------------------------------------------------------------------------------------*/
    return(error);
}

/****************************************************************************/
/*NAME :	PUBLIC t_dif_error  DIF_PANEL_C3_TPO_Nop                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*                                                                          */
/*																			*/
/* PARAMETERS :																*/
/*         IN :	None                                                        */
/*     InOut  : None                                                        */
/*                                                                          */
/* 		OUT   : Error type                                             	    */
/*                                                                          */
/* RETURN	  :                            						   	        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dif_error DIF_PANEL_C3_TPO_Nop(void)
{
    t_dif_error error = DIF_OK;
    t_uint16    data_reg = 0;

    data_reg = 0x0000;

    /* RAM data write */
    /*-------- write control register of panel----*/
    error = Dif_panel_send_cmd_data(DIF_PANEL_C3_TPO_NOP, data_reg);
    if (error != DIF_OK)
    {
        error = DIF_INVALID_PARAMETER;
        return(error);
    }

    return(error);
}


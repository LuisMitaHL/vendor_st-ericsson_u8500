/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DSI IRQ file
* \author  ST-Ericsson
*/
/*****************************************************************************/


/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "dsi_irq.h"
#include "dsi.h"
#include "dsi_p.h"
#include "hcl_defs.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*
 * Private variables                                                        *
 *--------------------------------------------------------------------------*/
PRIVATE t_dsi_link_registers    *gp_dsi_registers;

/****************************************************************************
* NAME:	 DSI_SetBaseAddress()												
*---------------------------------------------------------------------------
* DESCRIPTION   : This routine initializes DSI HCL.		
* PARAMETERS    :											
* IN            : t_logical_address  base_address 	
* INOUT         : None
* OUT           : None
* RETURN VALUE  : none											
* TYPE       : Public
*---------------------------------------------------------------------------
* REENTRANCY: NA														
*****************************************************************************/
PUBLIC void DSI_SetBaseAddress(t_logical_address base_address)
{
    /* initializating the MCDE base address */
    gp_dsi_registers = (t_dsi_link_registers *) base_address;
}

/***************************************************************************
* NAME:     DSI_EnableIRQSrc()
*---------------------------------------------------------------------------
* DESCRIPTION   :This routine allows to enable a specific interrupt
* PARAMETERS    : 											
* IN            : irq_src: ORed value of interrupt sources to be enabled.
*							(can also be used to enable all interrupts)
*                 t_dsi_irq_type: Block of DSI link
* INOUT         : None
* OUT           : None
* RETURN VALUE  : none
* TYPE          : Public
*--------------------------------------------------------------------------
*REENTRANCY: NA														
*****************************************************************************/
PUBLIC void DSI_EnableIRQSrc(t_dsi_irq_type irq_type, t_uint32 irq_src)
{
    switch (irq_type)
    {
        case DSI_IRQ_TYPE_MCTL_MAIN:
            gp_dsi_registers->mctl_main_sts_ctl |= (t_uint8) irq_src;
            break;

        case DSI_IRQ_TYPE_CMD_MODE:
            gp_dsi_registers->cmd_mode_sts_ctl |= (t_uint8) irq_src;
            break;

        case DSI_IRQ_TYPE_DIRECT_CMD_MODE:
            gp_dsi_registers->direct_cmd_sts_ctl |= (t_uint16) irq_src;
            break;

        case DSI_IRQ_TYPE_DIRECT_CMD_RD_MODE:
            gp_dsi_registers->direct_cmd_rd_sts_ctl |= (t_uint16) irq_src;
            break;

        case DSI_IRQ_TYPE_VID_MODE:
            gp_dsi_registers->vid_mode_sts_ctl |= (t_uint16) irq_src;
            break;

        case DSI_IRQ_TYPE_TG:
            gp_dsi_registers->tg_sts_ctl |= (t_uint8) irq_src;
            break;

        case DSI_IRQ_TYPE_DPHY_ERROR:
            gp_dsi_registers->mctl_dphy_err_ctl |= (t_uint16) irq_src;
            break;

        case DSI_IRQ_TYPE_DPHY_CLK_TRIM_RD:
            gp_dsi_registers->dphy_clk_trim_rd_ctl |= (t_uint8) irq_src;
            break;

        default:
            break;
    }
}

/***************************************************************************
* NAME:     DSI_DisableIRQSrc()
*---------------------------------------------------------------------------
* DESCRIPTION   :This routine allows to disable a specific interrupt
* PARAMETERS    : 											
* IN            : irq_src: ORed value of interrupt sources to be disabled.
*							(can also be used to enable all interrupts)
*                 t_dsi_irq_type: Block of DSI link
* INOUT         : None
* OUT           : None
* RETURN VALUE  : none
* TYPE          : Public
*--------------------------------------------------------------------------
*REENTRANCY: NA														
*****************************************************************************/
PUBLIC void DSI_DisableIRQSrc(t_dsi_irq_type irq_type, t_uint32 irq_src)
{
    switch (irq_type)
    {
        case DSI_IRQ_TYPE_MCTL_MAIN:
            gp_dsi_registers->mctl_main_sts_ctl |= (t_uint8)~irq_src;
            break;

        case DSI_IRQ_TYPE_CMD_MODE:
            gp_dsi_registers->cmd_mode_sts_ctl |= (t_uint8)~irq_src;
            break;

        case DSI_IRQ_TYPE_DIRECT_CMD_MODE:
            gp_dsi_registers->direct_cmd_sts_ctl |= (t_uint16)~irq_src;
            break;

        case DSI_IRQ_TYPE_DIRECT_CMD_RD_MODE:
            gp_dsi_registers->direct_cmd_rd_sts_ctl |= (t_uint16)~irq_src;
            break;

        case DSI_IRQ_TYPE_VID_MODE:
            gp_dsi_registers->vid_mode_sts_ctl |= (t_uint16)~irq_src;
            break;

        case DSI_IRQ_TYPE_TG:
            gp_dsi_registers->tg_sts_ctl |= (t_uint8)~irq_src;
            break;

        case DSI_IRQ_TYPE_DPHY_ERROR:
            gp_dsi_registers->mctl_dphy_err_ctl |= (t_uint16)~irq_src;
            break;

        case DSI_IRQ_TYPE_DPHY_CLK_TRIM_RD:
            gp_dsi_registers->dphy_clk_trim_rd_ctl |= (t_uint8)~irq_src;
            break;

        default:
            break;
    }
}

/***************************************************************************
*NAME:     DSI_GetIRQSrc()												   *
*--------------------------------------------------------------------------*
*DESCRIPTION   :This routine allows to read it status					   *
*(only valid for enabled IT)											   *
*PARAMETERS    : 														   *
*IN            : t_dsi_irq_type: Block of DSI link                         *
*INOUT         : None													   *
*OUT           : itstatus: it status for all interrupts					   *
*RETURN VALUE  :ORed value of all the active interrupt sources			   *
*TYPE          : Public													   *
*--------------------------------------------------------------------------*
*REENTRANCY: NA															   *
****************************************************************************/
PUBLIC t_uint32 DSI_GetIRQSrc(t_dsi_irq_type irq_type)
{
    t_uint32    irq_src = DSI_NO_INTERRUPT;

    switch (irq_type)
    {
        case DSI_IRQ_TYPE_MCTL_MAIN:
            irq_src = (t_uint32) gp_dsi_registers->mctl_main_sts_flag;
            break;

        case DSI_IRQ_TYPE_CMD_MODE:
            irq_src = (t_uint32) gp_dsi_registers->cmd_mode_sts_flag;
            break;

        case DSI_IRQ_TYPE_DIRECT_CMD_MODE:
            irq_src = (t_uint32) gp_dsi_registers->direct_cmd_sts_flag;
            break;

        case DSI_IRQ_TYPE_DIRECT_CMD_RD_MODE:
            irq_src = (t_uint32) gp_dsi_registers->direct_cmd_rd_sts_flag;
            break;

        case DSI_IRQ_TYPE_VID_MODE:
            irq_src = (t_uint32) gp_dsi_registers->vid_mode_sts_flag;
            break;

        case DSI_IRQ_TYPE_TG:
            irq_src = (t_uint32) gp_dsi_registers->tg_sts_flag;
            break;

        case DSI_IRQ_TYPE_DPHY_ERROR:
            irq_src = (t_uint32) gp_dsi_registers->mctl_dphy_err_flag;
            break;

        case DSI_IRQ_TYPE_DPHY_CLK_TRIM_RD:
            irq_src = (t_uint32) gp_dsi_registers->dphy_clk_trim_rd_flag;
            break;

        default:
            break;
    }

    return(irq_src);
}

/****************************************************************************
* NAME:     DSI_ClearIRQSrc()												*
*---------------------------------------------------------------------------*
* DESCRIPTION   :This routine allows to clear interrupt status 				*
*(clear in fact raw status)													*
* PARAMETERS    : 															*
* IN            : irq_src: ORed value of interrupt sourcesto be cleared 	*
*					(all interrupts can also be cleared together)			*
*                 t_dsi_irq_type: Block of DSI link                         *
* INOUT         : None														*
* OUT           : None														*
* RETURN VALUE  : none: 													*
* TYPE          : Public													*
*---------------------------------------------------------------------------*
*REENTRANCY: Non Reentrant													*
*****************************************************************************/
PUBLIC void DSI_ClearIRQSrc(t_dsi_irq_type irq_type, t_uint32 irq_src)
{
    switch (irq_type)
    {
        case DSI_IRQ_TYPE_MCTL_MAIN:
            gp_dsi_registers->mctl_main_sts_flag |= (t_uint8) irq_src;
            break;

        case DSI_IRQ_TYPE_CMD_MODE:
            gp_dsi_registers->cmd_mode_sts_flag |= (t_uint8) irq_src;
            break;

        case DSI_IRQ_TYPE_DIRECT_CMD_MODE:
            gp_dsi_registers->direct_cmd_sts_flag |= (t_uint16) irq_src;
            break;

        case DSI_IRQ_TYPE_DIRECT_CMD_RD_MODE:
            gp_dsi_registers->direct_cmd_rd_sts_flag |= (t_uint16) irq_src;
            break;

        case DSI_IRQ_TYPE_VID_MODE:
            gp_dsi_registers->vid_mode_sts_flag |= (t_uint16) irq_src;
            break;

        case DSI_IRQ_TYPE_TG:
            gp_dsi_registers->tg_sts_flag |= (t_uint8) irq_src;
            break;

        case DSI_IRQ_TYPE_DPHY_ERROR:
            gp_dsi_registers->mctl_dphy_err_flag |= (t_uint16) irq_src;
            break;

        case DSI_IRQ_TYPE_DPHY_CLK_TRIM_RD:
            gp_dsi_registers->dphy_clk_trim_rd_flag |= (t_uint8) irq_src;
            break;

        default:
            break;
    }
}


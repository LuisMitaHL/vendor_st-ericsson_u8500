/*
* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma_base.c
* This file contains the HCL base functions
* --------------------------------------------------------------------------------------------*/
#include "dma_base.h"
#include "dma_p.h"

/*------------------------------------------------------------------------
 * Global Variables                                   
 *------------------------------------------------------------------------*/
/* Global DMA system context variable */
PRIVATE t_dma_system_context    g_dma_system_context;

PRIVATE void dmaBase_ConfigSrcHalfChan(IN t_dma_channel dma_channel, IN t_dma_pr_mode dma_mode, IN t_dma_sxcfg_config *p_dma_sxcfg_config);
PRIVATE void dmaBase_ConfigDestHalfChan(IN t_dma_channel dma_channel, IN t_dma_pr_mode dma_mode, IN t_dma_sxcfg_config *p_dma_sxcfg_config);
PRIVATE void dmaBase_ConfigSrcHalfChanPhyMode(IN t_dma_channel dma_channel, IN t_dma_sxcfg_config *p_dma_sxcfg_config);
PRIVATE void dmaBase_ConfigSrcHalfChanLogMode(IN t_dma_channel dma_channel, IN t_dma_sxcfg_config *p_dma_sxcfg_config);
PRIVATE void dmaBase_ConfigDestHalfChanPhyMode(IN t_dma_channel dma_channel, IN t_dma_sxcfg_config *p_dma_sxcfg_config);
PRIVATE void dmaBase_ConfigDestHalfChanLogMode(IN t_dma_channel dma_channel, IN t_dma_sxcfg_config *p_dma_sxcfg_config);


/******************************************************************************************/
/* NAME:  t_bool dmaBase_Init                                                             */
/*----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is called for Initialisation of DMA Hardware base address   */
/* PARAMETERS:                                                                            */
/* IN:    dma_base_address: Base address of DMA                                           */
/* INOUT: None                                                                            */
/* OUT:   None                                                                            */
/*                                                                                        */
/* RETURN:         t_bool  which can take any of following values -                       */
/*              FALSE  :  If DMA peripheral IDs are incorrect                             */
/*              TRUE   :  If DMA peripheral IDs are correct                               */
/*----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                   */

/******************************************************************************************/
PUBLIC t_bool dmaBase_Init(IN t_logical_address dma_base_address)
{
    t_bool  status;

    /* Saving DMA base address to global dma system context variable */
    g_dma_system_context.p_dma_register = (t_dma_register *) dma_base_address;

    /* Checking DMA Peripheral IDs & PCellID */
    if
    (
        DMA_PERIPH_ID_0 != (g_dma_system_context.p_dma_register->dma_periphid0)
    ||  (DMA_PERIPH_ID_1 != (g_dma_system_context.p_dma_register->dma_periphid1))
    ||  (DMA_PERIPH_ID_2 != (g_dma_system_context.p_dma_register->dma_periphid2))
    ||  (DMA_PERIPH_ID_3 != (g_dma_system_context.p_dma_register->dma_periphid3))
    ||  (DMA_P_CELL_ID_0 != (g_dma_system_context.p_dma_register->dma_cellid0))
    ||  (DMA_P_CELL_ID_1 != (g_dma_system_context.p_dma_register->dma_cellid1))
    ||  (DMA_P_CELL_ID_2 != (g_dma_system_context.p_dma_register->dma_cellid2))
    ||  (DMA_P_CELL_ID_3 != (g_dma_system_context.p_dma_register->dma_cellid3))
    )
    {
        status = FALSE;
    }
    else
    {
        status = TRUE;
    }

    return(status);
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetClockControl                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables or disables the various clock parts of the DMA        */
/* NOTE       : The CPU must do secure read or write accesses to this register             */
/*              otherwise an error is generated on CPU bus interface by the DMAC           */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_evt_grp: Event group clock of the DMA including entire DMA Kernel Clock      */
/*                     Event group input can be one or a combination by giving like        */
/*                     (DMA_SRC_EVT_GRP_1|DMA_SRC_EVT_GRP_3|DMA_DEST_EVT_GRP_1) as input   */
/*        dma_ctrl: Enable or Disable                                                      */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetClockControl(IN t_dma_evt_grp dma_evt_grp, IN t_dma_ctrl dma_ctrl)
{
    if (dma_ctrl == DMA_ENABLE)
    {
        g_dma_system_context.p_dma_register->dmac_gcc |= (t_uint32) dma_evt_grp;
    }
    else
    {
        g_dma_system_context.p_dma_register->dmac_gcc &= ~((t_uint32) dma_evt_grp);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetClockControl                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the enabled or disabled status of the various clock      */
/*              parts of the DMA.                                                          */
/* NOTE :- The CPU must do secure read or write accesses to this register otherwise to     */
/*         this register otherwise an error is generated on CPU bus interface by the DMAC. */
/*                               CPU bus interface by the DMAC.                            */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_evt_grp: Event group clock of the DMA including entire DMA Kernel Clock      */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   *p_dma_ctrl: Enabled or Disabled                                                 */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetClockControl(IN t_dma_evt_grp dma_evt_grp, OUT t_dma_ctrl *p_dma_ctrl)
{
    if (DMA_GLOBAL == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) (g_dma_system_context.p_dma_register->dmac_gcc & DMA_CLKEN_MASK);
    }
    else if (DMA_SRC_EVT_GRP_0 == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) ((g_dma_system_context.p_dma_register->dmac_gcc & DMA_SE_0_MASK) >> DMA_SE_0_SHIFT);
    }
    else if (DMA_SRC_EVT_GRP_1 == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) ((g_dma_system_context.p_dma_register->dmac_gcc & DMA_SE_1_MASK) >> DMA_SE_1_SHIFT);
    }
    else if (DMA_SRC_EVT_GRP_2 == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) ((g_dma_system_context.p_dma_register->dmac_gcc & DMA_SE_2_MASK) >> DMA_SE_2_SHIFT);
    }
    else if (DMA_SRC_EVT_GRP_3 == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) ((g_dma_system_context.p_dma_register->dmac_gcc & DMA_SE_3_MASK) >> DMA_SE_3_SHIFT);
    }
    else if (DMA_DEST_EVT_GRP_0 == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) ((g_dma_system_context.p_dma_register->dmac_gcc & DMA_DE_0_MASK) >> DMA_DE_0_SHIFT);
    }
    else if (DMA_DEST_EVT_GRP_1 == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) ((g_dma_system_context.p_dma_register->dmac_gcc & DMA_DE_1_MASK) >> DMA_DE_1_SHIFT);
    }
    else if (DMA_DEST_EVT_GRP_2 == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) ((g_dma_system_context.p_dma_register->dmac_gcc & DMA_DE_2_MASK) >> DMA_DE_2_SHIFT);
    }
    else if (DMA_DEST_EVT_GRP_3 == dma_evt_grp)
    {
        *p_dma_ctrl = (t_dma_ctrl) ((g_dma_system_context.p_dma_register->dmac_gcc & DMA_DE_3_MASK) >> DMA_DE_3_SHIFT);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetPRType                                                                */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the physical resource type.                              */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:  dma_channel  :     Physical Resource(0,1,2,4,..31)                                 */
/*        pr_type         :     Standard / Extended                                        */
/*                                                                                         */
/*  NOTE: If PR type is of concatenated give corresponding Even PR(0,2,4,..30) as input    */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                             */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetPRType(IN t_dma_channel dma_channel, IN t_dma_pr_type pr_type)
{
   t_uint32    pr_mask = DMA_ACTIVE_MASK;
    t_uint8		dma_active_state = 0x1;
    t_uint32    active_odd_bit = 0, active_even_bit = 0, reg_val_odd = 0, reg_val_even=0;
    
   t_uint8 shift = ((t_uint8) dma_channel/2) * 2;
   
   
    reg_val_odd = g_dma_system_context.p_dma_register->dmac_activo;
    reg_val_even = g_dma_system_context.p_dma_register->dmac_active;
    active_odd_bit = reg_val_odd & (dma_active_state<<shift);
    active_even_bit = reg_val_even & (dma_active_state<<shift);
    
    
    if (0 == (active_odd_bit ||active_even_bit))
    {
   
   	pr_mask &= ~(t_uint32) (DMA_CP_TYP_MASK << shift);
    pr_mask |= (t_uint32) pr_type << shift;
    g_dma_system_context.p_dma_register->dmac_prtyp = pr_mask ;
    }
   
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetPRType                                                                */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads the physical resource type                               */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN    :   dma_channel:     Physical Resource(0,1,2,4,..31)                              */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None:                                                                            */
/*                                                                                         */
/* RETURN:     t_dma_pr_type  : Standard / Extended                                        */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_pr_type dmaBase_GetPRType(IN t_dma_channel dma_channel)
{
    t_dma_pr_type   pr_type;
    t_uint8         shift = ((t_uint8) dma_channel / 2) * 2;

    pr_type = (t_dma_pr_type) ((g_dma_system_context.p_dma_register->dmac_prtyp & (DMA_CP_TYP_MASK << shift)) >> shift);

    return(pr_type);
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetPRSecurity                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the physical resource security                           */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel: Physical Resource(0,1,2,4,..31)                                     */
/*          pr_security : SECURE / NON_SECURE                                              */
/*                                                                                         */
/* NOTE: If PR type is of concatenated give corresponding even PR(0,2,4,..30) as input     */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetPRSecurity(IN t_dma_channel dma_channel, IN t_dma_pr_security pr_security)
{
    t_uint32    pr_mask;
    t_uint8     shift = ((t_uint8) dma_channel / 2) * 2;

    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    {
        pr_mask = g_dma_system_context.p_dma_register->dmac_prsmo;
        pr_mask &= ~((t_uint32) (DMA_PR_SEC_MASK << shift));
        pr_mask |= (t_uint32) pr_security << shift;
        g_dma_system_context.p_dma_register->dmac_prsmo = pr_mask;
    }
    else                            /* Even PR */
    {
        pr_mask = g_dma_system_context.p_dma_register->dmac_prsme;
        pr_mask &= ~(t_uint32) (DMA_PR_SEC_MASK << shift);
        pr_mask |= (t_uint32) pr_security << shift;
        g_dma_system_context.p_dma_register->dmac_prsme = pr_mask;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetPRSecurity                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads the physical resource security                          */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN    :   dma_channel:     Physical Resource(0,1,2,4,..31)                              */
/*                                                                                         */
/* INOUT`:      None                                                                       */
/* OUT     :   None:                                                                       */
/*                                                                                         */
/* RETURN:     t_dma_pr_security                                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_pr_security dmaBase_GetPRSecurity(IN t_dma_channel dma_channel)
{
    t_dma_pr_security   pr_security;
    t_uint8             shift = ((t_uint8) dma_channel / 2) * 2;

    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    {
        pr_security = (t_dma_pr_security) ((g_dma_system_context.p_dma_register->dmac_prsmo & (DMA_PR_SEC_MASK << shift)) >> shift);
    }
    else                            /* Even PR */
    {
        pr_security = (t_dma_pr_security) ((g_dma_system_context.p_dma_register->dmac_prsme & (DMA_PR_SEC_MASK << shift)) >> shift);
    }

    return(pr_security);
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetPRModeSelect                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the physical resource mode.                              */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel: Physical Resource(0,1,2,4,..31)                                     */
/*          pr_mode    :     BASIC / LOGICAL / OPERATION                                   */
/*                                                                                         */
/* NOTE: If PR type is of concatenated give corresponding Even PR(0,2,4,..30) as input     */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetPRModeSelect(IN t_dma_channel dma_channel, IN t_dma_pr_mode pr_mode)
{
    t_uint32    pr_mask = 0x0;
    t_uint8     shift = ((t_uint8) dma_channel / 2) * 2;

    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    {
    
               
        pr_mask |= (t_uint32) pr_mode << shift;
        g_dma_system_context.p_dma_register->dmac_prmso = pr_mask;
    }
    else                            /* Even PR */
    {
    
        pr_mask |= (t_uint32) pr_mode << shift;
        g_dma_system_context.p_dma_register->dmac_prmse = pr_mask;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetPRModeSelect                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads the physical resource mode                              */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN    :   dma_pr:     Physical Resource(0,1,2,4,..31)                                   */
/*                                                                                         */
/* INOUT`:      None                                                                       */
/* OUT     :   None:                                                                       */
/*                                                                                         */
/* RETURN:     t_dma_pr_mode                                                               */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_pr_mode dmaBase_GetPRModeSelect(IN t_dma_channel dma_channel)
{
    t_dma_pr_mode   pr_mode;
    t_uint8         shift = ((t_uint8) dma_channel / 2) * 2;

    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    {
        pr_mode = (t_dma_pr_mode) ((g_dma_system_context.p_dma_register->dmac_prmso & (DMA_PR_MOD_MASK << shift)) >> shift);
    }
    else                            /* Even PR */
    {
        pr_mode = (t_dma_pr_mode) ((g_dma_system_context.p_dma_register->dmac_prmse & (DMA_PR_MOD_MASK << shift)) >> shift);
    }

    return(pr_mode);
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetPRModeOption                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the physical resource type,mode,mode option,activation   */
/*              and secure mode                                                            */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        : Physical Resource(0,1,2,4,..31)                             */
/*        pr_mode_option    :  Physical Mode   1. BASIC                                    */
/*                                                2. MODULO ADDRESSING                     */
/*                                                3. DOUBLE DESTINATION                    */
/*                                                                                         */
/*                               Logical Mode     1. SRC_PHY_DEST_LOG                      */
/*                                                2. SRC_LOG_DEST_PHY                      */
/*                                                3. SRC_LOG_DEST_LOG                      */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetPRModeOption(IN t_dma_channel dma_channel, IN t_dma_pr_mode_option pr_mode_option)
{
    t_uint32    pr_mask = 0x0;
    t_uint8     shift = ((t_uint8) dma_channel / 2) * 2;

    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    {
    
        pr_mask |= (t_uint32) pr_mode_option << shift;
        g_dma_system_context.p_dma_register->dmac_prmoo = pr_mask;
    }
    else                            /* Even PR */
    {
        pr_mask |= (t_uint32) pr_mode_option << shift;
        g_dma_system_context.p_dma_register->dmac_prmoe = pr_mask;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetPRModeOption                                                          */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads the physical resource type mode option                  */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN    :   dma_channel:     Physical Resource(0,1,2,4,..31)                              */
/*                                                                                         */
/* INOUT`:      None                                                                       */
/* OUT     :   None:                                                                       */
/*                                                                                         */
/* RETURN:     t_dma_pr_mode_option    :  Physical Mode    1. BASIC                        */
/*                                                         2. MODULO ADDRESSING            */
/*                                                         3. DOUBLE DESTINATION           */
/*                                                                                         */
/*                                      :     Logical Mode     1. SRC_PHY_DEST_LOG         */
/*                                                            2. SRC_LOG_DEST_PHY          */
/*                                                            3. SRC_LOG_DEST_LOG          */
/*                                                                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_pr_mode_option dmaBase_GetPRModeOption(IN t_dma_channel dma_channel)
{
    t_uint8                 shift;
    t_dma_pr_mode_option    pr_mode_option;

    shift = ((t_uint8) dma_channel / 2) * 2;

    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    {
        pr_mode_option = (t_dma_pr_mode_option) ((g_dma_system_context.p_dma_register->dmac_prmoo & (DMA_PR_OPT_MASK << shift)) >> shift);
    }
    else                            /* Even PR */
    {
        pr_mode_option = (t_dma_pr_mode_option) ((g_dma_system_context.p_dma_register->dmac_prmoe & (DMA_PR_OPT_MASK << shift)) >> shift);
    }

    return(pr_mode_option);
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetPRActiveStatus                                                        */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the physical resource activation                         */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        :     Physical Resource(0,1,2,4,..31)                         */
/*          pr_active_state   :      STOP                                                  */
/*                                RUN                                                      */
/*                                SUSPEND_REQ                                              */
/*                                SUSPENDED                                                */
/*                                                                                         */
/* NOTE: If PR type is of concatenated give corresponding Even PR(0,2,4,..30) as input     */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetPRActiveStatus(IN t_dma_channel dma_channel, IN t_dma_active_state pr_active_state)
{
    t_uint32    pr_mask = DMA_ACTIVE_MASK;
    t_uint8     shift = ((t_uint8) dma_channel / 2) * 2;

    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    {
           	  
        pr_mask &= ~(t_uint32) (DMA_PR_AS_MASK << shift);
        pr_mask |= (t_uint32) pr_active_state << shift;
        g_dma_system_context.p_dma_register->dmac_activo = pr_mask;
    }
    else                            /* Even PR */
    {
        pr_mask &= ~(t_uint32) (DMA_PR_AS_MASK << shift);
        pr_mask |= (t_uint32) pr_active_state << shift;
        g_dma_system_context.p_dma_register->dmac_active = pr_mask;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetPRActiveStatus                                                        */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine reads the physical resource activation                        */
/*              NOTE: Only 2n and 2n+1 can be concatenated (n = 0,1,2..15)                 */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN    :   dma_channel:     Physical Resource(0,1,2,4,..31)                              */
/*                                                                                         */
/* INOUT`:      None                                                                       */
/* OUT     :   None:                                                                       */
/*                                                                                         */
/* RETURN:     t_dma_active_state    :    STOP                                             */
/*                                    RUN                                                  */
/*                                    SUSPEND_REQ                                          */
/*                                    SUSPENDED                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_active_state dmaBase_GetPRActiveStatus(IN t_dma_channel dma_channel)
{
    t_uint8             shift;
    t_dma_active_state  pr_active_state;

    shift = ((t_uint8) dma_channel / 2) * 2;

    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    {
        pr_active_state = (t_dma_active_state) ((g_dma_system_context.p_dma_register->dmac_activo & (DMA_PR_AS_MASK << shift)) >> shift);
    }
    else                            /* Even PR */
    {
        pr_active_state = (t_dma_active_state) ((g_dma_system_context.p_dma_register->dmac_active & (DMA_PR_AS_MASK << shift)) >> shift);
    }

    return(pr_active_state);
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetLogicalChanBaseAddr                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the secure/non-secure logical channel Parameter          */
/*                and Link base addresses                                                  */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    secure: TRUE : if want to configure secure Logical Channel base address          */
/*                FALSE: if want to configure non-secure Logical Channel base address      */
/*                                                                                         */
/*        *p_dma_base_addr_val: Sets Both Parameter base address and Link base address     */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetLogicalChanBaseAddr(IN t_dma_pr_security security, IN t_dma_base_addr *p_dma_base_addr)
{
    if (DMA_PR_SECURE == security)
    {
        g_dma_system_context.p_dma_register->dmac_slcpa = p_dma_base_addr->param_base_addr;
        g_dma_system_context.p_dma_register->dmac_slcla = p_dma_base_addr->link_base_addr;
    }
    else
    {
        g_dma_system_context.p_dma_register->dmac_lcpa = p_dma_base_addr->param_base_addr;
        g_dma_system_context.p_dma_register->dmac_lcla = p_dma_base_addr->link_base_addr;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetLogicalChanBaseAddr                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the secure/non-secure logical channel Parameter          */
/*                and Link base addresses                                                  */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    secure: TRUE : if want to get secure Logical Channel base address                */
/*                FALSE: if want to get non-secure Logical Channel base address            */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   *p_dma_base_addr_val: Parameter and Link Base Addresses                          */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetLogicalChanBaseAddr(IN t_dma_pr_security security, OUT t_dma_base_addr *p_dma_base_addr)
{
    if (DMA_PR_SECURE == security)
    {
        p_dma_base_addr->param_base_addr = (t_uint32) (g_dma_system_context.p_dma_register->dmac_slcpa);
        p_dma_base_addr->link_base_addr = (t_uint32) (g_dma_system_context.p_dma_register->dmac_slcla);
    }
    else
    {
        p_dma_base_addr->param_base_addr = (t_uint32) (g_dma_system_context.p_dma_register->dmac_lcpa);
        p_dma_base_addr->link_base_addr = (t_uint32) (g_dma_system_context.p_dma_register->dmac_lcla);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetEventLineSecurity                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables secure mode for the event lines                       */
/*              There are (0-63) source event line and (0-63) destination event lines      */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    evt_security        :    Secure or non_secure event line                         */
/*                                                                                         */
/*          dma_evt_line_type: Specify whether source or destination event line            */
/*                                                                                         */
/*        evt_line: The event line which needs mode enabling/disabling                     */
/*                       NOTE :- The CPU must do secure read or write accesses             */
/*                               to this register otherwise an error is generated on       */
/*                               CPU bus interface by the DMAC.                            */
/*                                                                                         */
/*                             - Secured event lines are used by                           */
/*                               secured physical resources only while Not secured lines   */
/*                               are used by secure and not-secure physical resources      */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetEventLineSecurity
(
    IN t_dma_pr_security    evt_security,
    IN t_dma_evt_line_type  dma_evt_line_type,
    IN t_dma_event          evt_line
)
{
    t_uint8 reg_sel, shift;

    reg_sel = (t_uint8) evt_line / 16;

    shift = (DMA_SRC_EVT_LINE == dma_evt_line_type) ? ((t_uint8) evt_line % 16) : ((t_uint8) evt_line % 16) + 16;

    if (DMA_PR_SECURE == evt_security)
    {
        g_dma_system_context.p_dma_register->dmac_sseg[reg_sel] |= DMA_EVT_SEL_MASK << shift;
    }
    else
    {
        g_dma_system_context.p_dma_register->dmac_sceg[reg_sel] |= DMA_EVT_SEL_MASK << shift;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetEventLineSecurity                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the SECURE/NON SECURE status of the event lines       */
/*              There are (0-63) source event line and (0-63) destination event lines      */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_evt_line_type: Specify whether source or destination event line              */
/*                                                                                         */
/*        evt_line: The event line which needs secure mode enabling/disabling              */
/*                       NOTE :- The CPU must do secure read or write accesses             */
/*                               to this register otherwise an error is generated on       */
/*                               CPU bus interface by the DMAC.                            */
/*                                                                                         */
/*                             - Secured event lines are used by                           */
/*                               secured physical resources only while Not secured lines   */
/*                               are used by secure and not-secure physical resources      */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   *p_evt_security: secure or non-secure                                            */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetEventLineSecurity
(
    IN t_dma_evt_line_type  dma_evt_line_type,
    IN t_dma_event          evt_line,
    OUT t_dma_pr_security   *p_evt_security
)
{
    t_uint8 reg_sel, shift;

    reg_sel = (t_uint8) evt_line / 16;
    shift = (DMA_SRC_EVT_LINE == dma_evt_line_type) ? ((t_uint8) evt_line % 16) : ((t_uint8) evt_line % 16) + 16;

    if (g_dma_system_context.p_dma_register->dmac_sseg[reg_sel] & (DMA_EVT_SEL_MASK << shift))
    {
        *p_evt_security = DMA_PR_SECURE;
    }
    else
    {
        *p_evt_security = DMA_PR_NONSECURE;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_ClearEventLineSecurity                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables  secure mode for the event lines                     */
/*              There are (0-63) source event line and (0-63) destination event lines      */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    evt_security        :    Secure or non_secure event line                         */
/*                                                                                         */
/*          dma_evt_line_type: Specify whether source or destination event line            */
/*                                                                                         */
/*        evt_line: The event line which needs mode enabling/disabling                     */
/*                       NOTE :- The CPU must do secure read or write accesses             */
/*                               to this register otherwise an error is generated on       */
/*                               CPU bus interface by the DMAC.                            */
/*                                                                                         */
/*                             - Secured event lines are used by                           */
/*                               secured physical resources only while Not secured lines   */
/*                               are used by secure and not-secure physical resources      */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_ClearEventLineSecurity
(
    IN t_dma_pr_security    evt_security,
    IN t_dma_evt_line_type  dma_evt_line_type,
    IN t_dma_event          evt_line
)
{
    t_uint8 reg_sel, shift;

    reg_sel = (t_uint8) evt_line / 16;

    shift = (DMA_SRC_EVT_LINE == dma_evt_line_type) ? ((t_uint8) evt_line % 16) : ((t_uint8) evt_line % 16) + 16;

    if (DMA_PR_SECURE == evt_security)
    {
        g_dma_system_context.p_dma_register->dmac_sceg[reg_sel] |= ~(DMA_EVT_SEL_MASK << shift);
    }
    else
    {
        g_dma_system_context.p_dma_register->dmac_sseg[reg_sel] |= (DMA_EVT_SEL_MASK << shift);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetStatistics                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets input statistic parameters for getting results           */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/*                                                                                         */
/* p_dma_stat_config    : Input structure which need to contain the value of statistic     */
/*                          parameter                                                      */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetStatistics(IN t_dma_stat_config *p_dma_stat_config)
{
    /* Enable Statistic Feature    */
    if (p_dma_stat_config->stat_feature)
    {
        g_dma_system_context.p_dma_register->dmac_stfu |= DMA_STAT_ENA_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dmac_stfu &= ~DMA_STAT_ENA_MASK;
    }

    /* Enable Statistic Mode */
    g_dma_system_context.p_dma_register->dmac_stfu &= ~DMA_STAT_MODE_MASK;
    g_dma_system_context.p_dma_register->dmac_stfu |= (t_uint32) p_dma_stat_config->stat_mode;

    /* Enable Master Selection */
    g_dma_system_context.p_dma_register->dmac_stfu &= ~DMA_STAT_MSTSEL_MASK;
    g_dma_system_context.p_dma_register->dmac_stfu |= (t_uint32) p_dma_stat_config->dma_master;

    /* Physical Resource Selection */
    g_dma_system_context.p_dma_register->dmac_stfu &= ~DMA_STAT_PRSEL_MASK;
    g_dma_system_context.p_dma_register->dmac_stfu |= (t_uint32) p_dma_stat_config->dma_channel;

    /* Source or Destination Half Channel Selection */
    if (p_dma_stat_config->half_chan_type)
    {
        g_dma_system_context.p_dma_register->dmac_stfu |= DMA_STAT_SD_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dmac_stfu &= ~DMA_STAT_SD_MASK;
    }

    /* Logical Channel Source/Destination Event Line Number    */
    g_dma_system_context.p_dma_register->dmac_stfu &= ~DMA_STAT_LCELNB_MASK;
    g_dma_system_context.p_dma_register->dmac_stfu |= (t_uint32) p_dma_stat_config->logical_chan_evt_line;
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetStatistics                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the statistic result as well as the input parameters     */
/*              already set                                                                */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:   None                                                                              */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   *p_dma_stat_value: Structure  contains statistics input parameter already set    */
/*        *result: The statistic result                                                    */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetStatistics(OUT t_dma_stat_config *p_dma_stat_value, OUT t_uint16 *result)
{
    p_dma_stat_value->stat_feature = (t_dma_stat_feature) ((g_dma_system_context.p_dma_register->dmac_stfu & DMA_STAT_ENA_MASK) >> DMA_STAT_ENA_SHIFT);

    p_dma_stat_value->stat_mode = (t_dma_stat_mode) ((g_dma_system_context.p_dma_register->dmac_stfu & DMA_STAT_MODE_MASK) >> DMA_STAT_MODE_SHIFT);

    p_dma_stat_value->dma_master = (t_dma_master_port) ((g_dma_system_context.p_dma_register->dmac_stfu & DMA_STAT_MSTSEL_MASK) >> DMA_STAT_MSTSEL_SHIFT);

    p_dma_stat_value->dma_channel = (t_dma_channel) ((g_dma_system_context.p_dma_register->dmac_stfu & DMA_STAT_PRSEL_MASK) >> DMA_STAT_PRSEL_SHIFT);

    p_dma_stat_value->half_chan_type = (t_dma_half_chan_type) ((g_dma_system_context.p_dma_register->dmac_stfu & DMA_STAT_SD_MASK) >> DMA_STAT_SD_SHIFT);

    p_dma_stat_value->logical_chan_evt_line = (t_uint8) ((g_dma_system_context.p_dma_register->dmac_stfu & DMA_STAT_LCELNB_MASK) >> DMA_STAT_LCELNB_SHIFT);

    *result = (t_uint16) (g_dma_system_context.p_dma_register->dmac_stfu & DMA_STAT_RESULT_MASK);
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetHwConfig                                                              */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the DMA hardware implemented configuration (ICFG)        */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:   None                                                                              */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   *t_dma_icfg_config: Structure  contains hardware implemented configuration       */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetHwConfig(OUT t_dma_icfg_config *p_icfg_config)
{
    p_icfg_config->master1_modulo = (t_dma_modulo_enable) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MODEN1_MASK) >> DMA_ICFG_MODEN1_SHIFT);

    p_icfg_config->master2_modulo = (t_dma_modulo_enable) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MODEN2_MASK) >> DMA_ICFG_MODEN2_SHIFT);

    p_icfg_config->master3_modulo = (t_dma_modulo_enable) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MODEN3_MASK) >> DMA_ICFG_MODEN3_SHIFT);

    p_icfg_config->master4_modulo = (t_dma_modulo_enable) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MODEN4_MASK) >> DMA_ICFG_MODEN4_SHIFT);

    p_icfg_config->ptyp_asymm_cmd = (t_dma_ptype_mngmt) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_PTYP_MASK) >> DMA_ICFG_PTYP_SHIFT);

    p_icfg_config->master1_if_data_width = (t_dma_data_bus_width) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MSZ1_MASK) >> DMA_ICFG_MSZ1_SHIFT);

    p_icfg_config->master2_if_data_width = (t_dma_data_bus_width) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MSZ2_MASK) >> DMA_ICFG_MSZ2_SHIFT);

    p_icfg_config->master3_if_data_width = (t_dma_data_bus_width) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MSZ3_MASK) >> DMA_ICFG_MSZ3_SHIFT);

    p_icfg_config->master4_if_data_width = (t_dma_data_bus_width) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MSZ4_MASK) >> DMA_ICFG_MSZ4_SHIFT);

    p_icfg_config->cpu_if_data_width = (t_dma_data_bus_width) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_CPSZ_MASK) >> DMA_ICFG_CPSZ_SHIFT);

    p_icfg_config->data_buf_size = (t_dma_data_buf_size) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_DBUFSZ_MASK) >> DMA_ICFG_DBUFSZ_SHIFT);

    p_icfg_config->num_of_master_if = (t_dma_master_port_number) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_MNB_MASK) >> DMA_ICFG_MNB_SHIFT);

    p_icfg_config->std_chan_num = (t_dma_std_chan_num) ((g_dma_system_context.p_dma_register->dmac_icfg & DMA_ICFG_SCHNB_MASK));
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetMasterPlugStatus                                                      */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets Hardware connection status between Physcial Resource     */
/*              & Master units                                                             */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:   dma_master     :     DMA Master interface                                         */
/*         dma_channel    :    Physical Resource(0,1,2,4,..31)                             */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   *p_dma_ctrl: Hardware connection status between Physcial Resource & Master units */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetMasterPlugStatus
(
    IN t_dma_master_port    dma_master,
    IN t_dma_channel        dma_channel,
    OUT t_dma_ctrl          *p_dma_ctrl
)
{
    *p_dma_ctrl = (t_dma_ctrl)
        (
            (g_dma_system_context.p_dma_register->dmac_mplug[dma_master] & (DMA_MPLUG_MASK << (t_uint8) dma_channel)) >>
                (t_uint8) dma_channel
        );
}

/*******************************************************************************************/
/* NAME:  DMA_SetForceSrcEvtBurst()                                                        */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine forcefully sets the Burst request for the source event line   */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    event_line: Source Event line                                                    */
/* NOTE: Secure CPU access is required if the source event lines are secured.              */
/*                                                                                         */
/* INOUT: None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetForceSrcEvtBurst(IN t_uint8 event_line)
{
    t_uint32    shift, line_no;

    shift = event_line % 32;
    line_no = 0x01 << shift;

    if (0 == (event_line / 32))
    {
        g_dma_system_context.p_dma_register->dmac_fsebs1 |= line_no;
    }
    else
    {
        g_dma_system_context.p_dma_register->dmac_fsebs2 |= line_no;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetForceSrcEvtBurst()                                                    */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the Burst/Single request status of the source event   */
/*              lines                                                                      */
/* PARAMETERS:                                                                             */
/* IN:    event_line: Source Event line                                                    */
/* NOTE: Secure CPU access is required if the source event lines  are secured.             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT :     status: status of the source burst request                                    */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetForceSrcEvtBurst(IN t_uint8 event_line, OUT t_dma_ctrl *status)
{
    t_uint32    shift, line_no;

    shift = event_line % 32;
    line_no = 0x01 << shift;

    if (0 == (event_line / 32))
    {
        *status = (t_dma_ctrl) (g_dma_system_context.p_dma_register->dmac_fsebs1 & line_no);
    }
    else
    {
        *status = (t_dma_ctrl) (g_dma_system_context.p_dma_register->dmac_fsebs2 & line_no);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetChannelConfig()                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets channel config parameters                                */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        : DMA Physical Resource                                       */
/*        dma_half_chan_type: Source or Destination half channel to be configured          */
/*        dma_mode            :         PHYSICAL/LOGICAL/OPERATION                         */
/*        p_dma_sxcfg_config: Input structure which need to contain the value of           */
/*                            channel parameters to be set.                                */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetChannelConfig
(
    IN t_dma_channel        dma_channel,
    IN t_dma_pr_mode        dma_mode,
    IN t_dma_half_chan_type half_channel,
    IN t_dma_sxcfg_config   *p_dma_sxcfg_config
  
)
{
    if (DMA_SRC_HALF_CHAN == half_channel)
    {
        dmaBase_ConfigSrcHalfChan(dma_channel, dma_mode, p_dma_sxcfg_config);
    }
    else
    {   /* (DMA_DEST_HALF_CHAN == half_channel)*/
        dmaBase_ConfigDestHalfChan(dma_channel, dma_mode, p_dma_sxcfg_config);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetChannelElement()                                                      */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets channel element parameters                               */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel            : DMA Physical Resource                                   */
/*        dma_half_chan_type    : Source or Destination half channel to be configured      */
/*        dma_mode                : PHYSICAL/LOGICAL/OPERATION                             */
/*        p_dma_sxelt_config    : Input structure which contains element parameters.       */
/*                                                                                         */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetChannelElement
(
    IN t_dma_channel        dma_channel,
    IN t_dma_pr_mode        dma_mode,
    IN t_dma_half_chan_type half_channel,
    IN t_dma_sxelt_config   *p_dma_sxelt_config
)
{

 	t_uint8     shift = ((t_uint8) dma_channel / 2) * 2;
    t_uint8		dma_active_state = 0x1;
    t_uint32    active_odd = 0, active_even = 0, reg_val = 0;
    
    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    { 
    reg_val = g_dma_system_context.p_dma_register->dmac_activo;
    active_odd = reg_val & (dma_active_state<<shift);
    }
    
    else
    {
   
    reg_val = g_dma_system_context.p_dma_register->dmac_active;
    active_even = reg_val & (dma_active_state<<shift);
    
    }
    
    
    if (0 == (active_odd || active_even))
    {

    if (DMA_SRC_HALF_CHAN == half_channel)
    {
        if (DMA_PR_MODE_BASIC == dma_mode)
        {
            /* Load the Element Counter */
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sselt_exelt = p_dma_sxelt_config->
                    element_counter <<
                DMA_SxELT_ECNT_SHIFT;

            /* Configure Element Index */
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sselt_exelt |= p_dma_sxelt_config->element_index;
        }
        else if (DMA_PR_MODE_LOGICAL == dma_mode)   /*    Logical channel Mode */
        {
            /* Configure Link Index */
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sselt_exelt = p_dma_sxelt_config->
                    link_index <<
                DMA_SxELT_LIDX_SHIFT;
        }
    }
    else    /* (DMA_DEST_HALF_CHAN == half_channel)*/
    {
        if (DMA_PR_MODE_BASIC == dma_mode)
        {
            /* Load the Element Counter */
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdelt_exfrm = p_dma_sxelt_config->
                    element_counter <<
                DMA_SxELT_ECNT_SHIFT;

            /* Configure Element Index */
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdelt_exfrm |= p_dma_sxelt_config->element_index;
        }
        else if (DMA_PR_MODE_LOGICAL == dma_mode)   /*    Logical channel Mode */
        {
            /* Configure Link Index */
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdelt_exfrm = p_dma_sxelt_config->
                    link_index <<
                DMA_SxELT_LIDX_SHIFT;
        }
    }
    }
 }

/*******************************************************************************************/
/* NAME:  dmaBase_SetChannelPointer()                                                      */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the channel pointer for the source and dest half chan.   */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel            : DMA Physical Resource                                   */
/*        dma_half_chan_type    : Source or Destination half channel to be configured      */
/*        data_pointer            : Data Pointer of the data                               */
//*                                                                                        */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetChannelPointer
(
    IN t_dma_channel        dma_channel,
    IN t_dma_half_chan_type half_channel,
    IN t_uint32             data_pointer
)
{
    if (DMA_SRC_HALF_CHAN == half_channel)
    {
        /* load 32 bit address of the current element to be read */
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_ssptr_exptr = data_pointer;
    }
    else
    {
        /* load 32 bit address of the current element to be written */
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdptr_exrld = data_pointer;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetChannelLink()                                                         */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets link address in the channel link register.               */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel            : DMA Physical Resource                                   */
/*        dma_half_chan_type    : Source or Destination half channel to be configured      */
/*        p_dma_sxlnk_config    : Input structure which contains link parameters.          */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetChannelLink
(
    IN t_dma_channel        dma_channel,
    IN t_dma_half_chan_type half_channel,
    IN t_dma_sxlnk_config   *p_dma_sxlnk_config
)
{
    if (DMA_SRC_HALF_CHAN == half_channel)
    {
        /* Load Destination link address */
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk = p_dma_sxlnk_config->link_addr & DMA_SxLNK_LINK_MASK;

        /* Configure pre-link */
        if (p_dma_sxlnk_config->pre_link)
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk |= DMA_SxLNK_PRE_MASK;
        }
        else
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk &= ~DMA_SxLNK_PRE_MASK;
        }

        /* Configure Link Master Port */
        if (p_dma_sxlnk_config->link_master_port)
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk |= DMA_SxLNK_LMP_MASK;
        }
        else
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk &= ~DMA_SxLNK_LMP_MASK;
        }

        /* Configure Terminal Count Pulse */
        if (p_dma_sxlnk_config->tcp_assert)
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk |= DMA_SxLNK_TCP_MASK;
        }
        else
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk &= ~DMA_SxLNK_TCP_MASK;
        }
    }
    else
    {
        /* Load Destination link address */
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk = p_dma_sxlnk_config->link_addr & DMA_SxLNK_LINK_MASK;

        /* Configure pre-link */
        if (p_dma_sxlnk_config->pre_link)
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk |= DMA_SxLNK_PRE_MASK;
        }
        else
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk &= ~DMA_SxLNK_PRE_MASK;
        }

        /* Configure Link Master Port */
        if (p_dma_sxlnk_config->link_master_port)
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk |= DMA_SxLNK_LMP_MASK;
        }
        else
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk &= ~DMA_SxLNK_LMP_MASK;
        }

        /* Configure Terminal Count Pulse */
        if (p_dma_sxlnk_config->tcp_assert)
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk |= DMA_SxLNK_TCP_MASK;
        }
        else
        {
            g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk &= ~DMA_SxLNK_TCP_MASK;
        }
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetPhyLinkStatus()                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the physical link status  of the channel.                */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel            : DMA Physical Resource                                   */
/*        dma_half_chan_type    : Source or Destination half channel to be configured      */
/*        p_dma_sxlnk_config    : Input structure which contains link parameters.          */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetPhyLinkStatus
(
    IN t_dma_channel        dma_channel,
    IN t_dma_half_chan_type half_channel,
    IN t_uint32             *p_dma_link_status
)
{
    if (DMA_SRC_HALF_CHAN == half_channel)
    {
        /* Get Source link status */
        *p_dma_link_status = g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk & DMA_SxLNK_LINK_MASK;
    }
    else
    {
        /* Get Destination link status */
        *p_dma_link_status = g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk & DMA_SxLNK_LINK_MASK;
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_ResetChannel()                                                           */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine resets physical channel parameters                            */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel            : DMA Physical Resource                                   */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_ResetChannel(IN t_dma_channel dma_channel)
{


    t_uint8     shift = ((t_uint8) dma_channel / 2) * 2;
    t_uint8		dma_active_state = 0x1;
    t_uint32    active_odd = 0, active_even = 0, reg_val = 0;
    
    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    { 
    reg_val = g_dma_system_context.p_dma_register->dmac_activo;
    active_odd = reg_val & (dma_active_state<<shift);
    }
    
    else
    {
   
    reg_val = g_dma_system_context.p_dma_register->dmac_active;
    active_even = reg_val & (dma_active_state<<shift);
    
    }
    
    
    if (0 == (active_odd ||active_even))
    {
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg = 0x0;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sselt_exelt = 0x0;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_ssptr_exptr = 0x0;
/*    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk = 0x0; */
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc = 0x0;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdelt_exfrm = 0x0;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdptr_exrld = 0x0;
  /*  g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk = 0x0; */
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_SetLogChannelEvent()                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables logical channel                                       */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel: DMA Physical Resource                                               */
/*        dma_half_chan_type: Source or Destination half channel to be configured          */
/*        dma_event : device which is requesting the exchange                              */
/*          evt_ctrl : DISABLE/ENABLE/ROUND                                                */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_SetLogChannelEvent
(
    IN t_dma_channel        dma_channel,
    IN t_dma_evt_line_type  event_type,
    IN t_dma_event          dma_event,
    IN t_dma_evt_ctrl       evt_ctrl
)
{
    t_uint32    evt_line, shift;
    t_uint32    pr_mask = DMA_ACTIVE_MASK;

    evt_line = (t_uint8) dma_event % 16;
    shift = (t_uint8) evt_line * 2;

    if (DMA_SRC_EVT_LINE == event_type)
    {
        pr_mask = (DMA_ACTIVE_MASK) &~(DMA_SxLNK_EE_MASK << shift);
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk = pr_mask | ((t_uint32) evt_ctrl << shift);
    }
    else
    {
        pr_mask = (DMA_ACTIVE_MASK) &~(DMA_SxLNK_EE_MASK << shift);
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk = pr_mask | ((t_uint32) evt_ctrl << shift);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_GetLogChannelEvent()                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables logical channel                                       */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel: DMA Physical Resource                                               */
/*        dma_half_chan_type: Source or Destination half channel to be configured          */
/*        device : device which is requesting the exchange                                 */
/*          *p_evt_ctrl : DISABLE/ENABLE/ROUND                                             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PUBLIC void dmaBase_GetLogChannelEvent
(
    IN t_dma_channel        dma_channel,
    IN t_dma_evt_line_type  event_type,
    IN t_dma_event          dma_event,
    OUT t_dma_evt_ctrl      *p_evt_ctrl
)
{
    t_uint8 evt_line, shift;

    evt_line = (t_uint8) dma_event % 16;
    shift = (t_uint8) evt_line * 2;

    if (DMA_SRC_EVT_LINE == event_type)
    {
        *p_evt_ctrl = (t_dma_evt_ctrl)
            (
                (
                    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sslnk_exlnk &
                        (DMA_SxLNK_EE_MASK << shift)
                ) >> shift
            );
    }
    else
    {
        *p_evt_ctrl = (t_dma_evt_ctrl)
            (
                (
                    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdlnk_exblk &
                        (DMA_SxLNK_EE_MASK << shift)
                ) >> shift
            );
    }
}

/****************************************************************************/
/* NAME: t_uint32 dmaBase_GetChannelTCIntStatus()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*      This routine returns the TC interrupt status of the given DMA Ctrl. */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN  :                                                                    */
/*       secure:    TRUE or FALSE                                           */
/*                                                                          */
/* OUT :  none                                                              */
/*                                                                          */
/* RETURN:                                                                  */
/*        t_uint32: DMA Controller TC Interrupt Status                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_uint32 dmaBase_GetChannelTCIntStatus(t_bool secure)
{
    if (secure == TRUE)
    {
        return(g_dma_system_context.p_dma_register->dmac_spctis);
    }
    else
    {
        return(g_dma_system_context.p_dma_register->dmac_pctis);
    }
}

/****************************************************************************/
/* NAME: t_uint32 dmaBase_GetChannelErrorIntStatus()                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*      This routine returns the Error interrupt status                     */
/*      of the given DMA Controller.                                        */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN  :                                                                    */
/*      secure:    TRUE or FALSE                                            */
/*                                                                          */
/* OUT :  none                                                              */
/*                                                                          */
/* RETURN:                                                                  */
/*        t_uint32: DMA Controller Error Interrupt Status                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_uint32 dmaBase_GetChannelErrorIntStatus(t_bool secure)
{
    if (secure == TRUE)
    {
        return(g_dma_system_context.p_dma_register->dmac_spceis);
    }
    else
    {
        return(g_dma_system_context.p_dma_register->dmac_pceis);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_ConfigSrcHalfChan()                                                      */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configs source half channel parameters.                       */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        : DMA Physical Resource                                       */
/*        dma_mode           : PHYSICAL/LOGICAL/OPERATION                                  */
/*        p_dma_sxcfg_config : Input structure which need to contain the value of          */
/*                             channel parameters to be set.                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PRIVATE void dmaBase_ConfigSrcHalfChan
(
    IN t_dma_channel        dma_channel,
    IN t_dma_pr_mode        dma_mode,
    IN t_dma_sxcfg_config   *p_dma_sxcfg_config
)
{
    if (DMA_PR_MODE_BASIC == dma_mode)
    {
        dmaBase_ConfigSrcHalfChanPhyMode(dma_channel, p_dma_sxcfg_config);
    }
    else if (DMA_PR_MODE_LOGICAL == dma_mode)
    {   /* Logical Channel Mode */
        dmaBase_ConfigSrcHalfChanLogMode(dma_channel, p_dma_sxcfg_config);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_ConfigSrcHalfChanPhyMode()                                               */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configs source half channel parameters in Physical Mode       */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        : DMA Physical Resource                                       */
/*        p_dma_sxcfg_config : Input structure which need to contain the value of          */
/*                             channel parameters to be set.                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PRIVATE void dmaBase_ConfigSrcHalfChanPhyMode(IN t_dma_channel dma_channel, IN t_dma_sxcfg_config *p_dma_sxcfg_config)
{
       	
    /* Configure Master Port selection */
    if (p_dma_sxcfg_config->master_port)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg = DMA_SxCFG_MST_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg = ~DMA_SxCFG_MST_MASK;
    }

    /* Configure Terminal Count Interrupt Mask */
    if (p_dma_sxcfg_config->tc_intr_mask)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_TIM_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_TIM_MASK;
    }

    /* Configure Error Interrupt Mask */
    if (p_dma_sxcfg_config->err_intr_mask)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_EIM_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_EIM_MASK;
    }

    /* Configure Packet Enable */
    if (p_dma_sxcfg_config->pen_incr)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_PEN_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_PEN_MASK;
    }

    /* Configure Packet Size */
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_PSIZE_MASK;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |=
            (t_uint8) p_dma_sxcfg_config->packet_size <<
        DMA_SxCFG_PSIZE_SHIFT;

    /* Configure Element Size */
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~(DMA_SxCFG_ESIZE_MASK);
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |=
            (t_uint32) p_dma_sxcfg_config->element_size <<
        DMA_SxCFG_ESIZE_SHIFT;

    /* Configure the Priority level of Physical resource */
    if (p_dma_sxcfg_config->priority_level)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_PRI_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_PRI_MASK;
    }

    /* Configure Little/Big Endianess */
    if (p_dma_sxcfg_config->switch_endian)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_LBE_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_LBE_MASK;
    }

    /* Configure Transfer mode */
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_TM_MASK;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |=
            (t_uint32) p_dma_sxcfg_config->transfer_mode <<
        DMA_SxCFG_TM_SHIFT;

    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_EVTL_MASK;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= p_dma_sxcfg_config->event_line;
    
       
   
    
}

/*******************************************************************************************/
/* NAME:  dmaBase_ConfigSrcHalfChanLogMode()                                               */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configs source half channel parameters in Logical Mode        */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        : DMA Physical Resource                                       */
/*        p_dma_sxcfg_config : Input structure which need to contain the value of          */
/*                             channel parameters to be set.                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PRIVATE void dmaBase_ConfigSrcHalfChanLogMode(IN t_dma_channel dma_channel, IN t_dma_sxcfg_config *p_dma_sxcfg_config)
{
   
    t_uint8     shift = ((t_uint8) dma_channel / 2) * 2;
    t_uint8		dma_active_state = 0x1;
    t_uint32    active_odd = 0, active_even = 0, reg_val = 0;
    
    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    { 
    reg_val = g_dma_system_context.p_dma_register->dmac_activo;
    active_odd = reg_val & (dma_active_state<<shift);
    }
    
    else
    {
   
    reg_val = g_dma_system_context.p_dma_register->dmac_active;
    active_even = reg_val & (dma_active_state<<shift);
    
    }
    
    
    if (0 == (active_odd ||active_even))
    {
     /* Configure the Priority level of Physical resource */
    if (p_dma_sxcfg_config->priority_level)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_PRI_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_PRI_MASK;
    }

    /* Configure Little/Big Endianess */
    if (p_dma_sxcfg_config->switch_endian)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_LBE_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_LBE_MASK;
    }

    /* Global Interrupt Mask */
    if (p_dma_sxcfg_config->global_intr_mask)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_GIM_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_GIM_MASK;
    }

    /* Master for Parameters Update/fetch */
    if (p_dma_sxcfg_config->master_par_fetch)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg |= DMA_SxCFG_MFU_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sscfg_excfg &= ~DMA_SxCFG_MFU_MASK;
    }
    
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_ConfigDestHalfChan()                                                     */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configs Destination half channel parameters.                  */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        : DMA Physical Resource                                       */
/*        dma_mode           : PHYSICAL/LOGICAL/OPERATION                                  */
/*        p_dma_sxcfg_config : Input structure which need to contain the value of          */
/*                             channel parameters to be set.                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PRIVATE void dmaBase_ConfigDestHalfChan
(
    IN t_dma_channel        dma_channel,
    IN t_dma_pr_mode        dma_mode,
    IN t_dma_sxcfg_config   *p_dma_sxcfg_config
)
{
    if (DMA_PR_MODE_BASIC == dma_mode)
    {
        dmaBase_ConfigDestHalfChanPhyMode(dma_channel, p_dma_sxcfg_config);
    }
    else if (DMA_PR_MODE_LOGICAL == dma_mode)
    {   /* Logical Channel Mode */
        dmaBase_ConfigDestHalfChanLogMode(dma_channel, p_dma_sxcfg_config);
    }
}

/*******************************************************************************************/
/* NAME:  dmaBase_ConfigDestHalfChanPhyMode()                                              */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configs destination half channel parameters in Physical Mode  */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        : DMA Physical Resource                                       */
/*        p_dma_sxcfg_config : Input structure which need to contain the value of          */
/*                             channel parameters to be set.                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PRIVATE void dmaBase_ConfigDestHalfChanPhyMode(IN t_dma_channel dma_channel, IN t_dma_sxcfg_config *p_dma_sxcfg_config)
{
    /* Configure Master Port selection */
    if (p_dma_sxcfg_config->master_port)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc = DMA_SxCFG_MST_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc = ~DMA_SxCFG_MST_MASK;
    }

    /* Configure Terminal Count Interrupt Mask */
    if (p_dma_sxcfg_config->tc_intr_mask)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_TIM_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_TIM_MASK;
    }

    /* Configure Error Interrupt Mask */
    if (p_dma_sxcfg_config->err_intr_mask)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_EIM_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_EIM_MASK;
    }

    /* Configure Packet Enable */
    if (p_dma_sxcfg_config->pen_incr)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_PEN_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_PEN_MASK;
    }

    /* Configure Packet Size */
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_PSIZE_MASK;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |=
            (t_uint32) p_dma_sxcfg_config->packet_size <<
        DMA_SxCFG_PSIZE_SHIFT;

    /* Configure Element Size */
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~(DMA_SxCFG_ESIZE_MASK);
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |=
            (t_uint32) p_dma_sxcfg_config->element_size <<
        DMA_SxCFG_ESIZE_SHIFT;

    /* Configure the Priority level of Physical resource */
    if (p_dma_sxcfg_config->priority_level)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_PRI_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_PRI_MASK;
    }

    /* Configure Little/Big Endianess */
    if (p_dma_sxcfg_config->switch_endian)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_LBE_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_LBE_MASK;
    }

    /* Configure Transfer mode */
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_TM_MASK;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |=
            (t_uint32) p_dma_sxcfg_config->transfer_mode <<
        DMA_SxCFG_TM_SHIFT;

    /* Select Event Line */
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_EVTL_MASK;
    g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= p_dma_sxcfg_config->event_line;
}

/*******************************************************************************************/
/* NAME:  dmaBase_ConfigDestHalfChanLogMode()                                              */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configs destination half channel parameters in Logical Mode  */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    dma_channel        : DMA Physical Resource                                       */
/*        p_dma_sxcfg_config : Input structure which need to contain the value of          */
/*                             channel parameters to be set.                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: void                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */

/*******************************************************************************************/
PRIVATE void dmaBase_ConfigDestHalfChanLogMode(IN t_dma_channel dma_channel, IN t_dma_sxcfg_config *p_dma_sxcfg_config)
{
    t_uint8     shift = ((t_uint8) dma_channel / 2) * 2;
    t_uint8		dma_active_state = 0x1;
    t_uint32    active_odd = 0, active_even = 0, reg_val = 0;
    
    if ((t_uint8) dma_channel % 2)  /* Odd PR */
    { 
    reg_val = g_dma_system_context.p_dma_register->dmac_activo;
    active_odd = reg_val & (dma_active_state<<shift);
    }
    
    else
    {
   
    reg_val = g_dma_system_context.p_dma_register->dmac_active;
    active_even = reg_val & (dma_active_state<<shift);
    
    }
    
    
    if (0 == (active_odd ||active_even))
    {
    
    
     /* Configure the Priority level of Physical resource */
    if (p_dma_sxcfg_config->priority_level)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_PRI_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_PRI_MASK;
    }

    /* Configure Little/Big Endianess */
    if (p_dma_sxcfg_config->switch_endian)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_LBE_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_LBE_MASK;
    }

    /* Global Interrupt Mask */
    if (p_dma_sxcfg_config->global_intr_mask)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_GIM_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_GIM_MASK;
    }

    /* Master for Parameters Update/fetch */
    if (p_dma_sxcfg_config->master_par_fetch)
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc |= DMA_SxCFG_MFU_MASK;
    }
    else
    {
        g_dma_system_context.p_dma_register->dma_channel_param[dma_channel].dmac_sdcfg_exexc &= ~DMA_SxCFG_MFU_MASK;
    }
    
    }
}

/* End of file - dma_base.c */


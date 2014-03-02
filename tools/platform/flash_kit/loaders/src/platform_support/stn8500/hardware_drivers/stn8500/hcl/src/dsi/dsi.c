/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DSI driver
* \author  ST-Ericsson
*/
/*****************************************************************************/
#include "dsi.h"
#include "dsi_p.h"
#include "hcl_defs.h"
#include "debug.h"

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
PRIVATE volatile t_dsi_system_context   g_dsi_system_context[3];

/*-----------------------------------------------------------------------------
 * DEBUG STUFF
 *---------------------------------------------------------------------------*/
#ifdef __DEBUG
PRIVATE t_dbg_level             myDebugLevel_DSI = DEBUG_LEVEL0;
PRIVATE t_dbg_id                myDebugID_DSI = DSI_HCL_DBG_ID;
#endif
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_DSI
#define MY_DEBUG_ID             myDebugID_DSI


/*---------------------------------------------------------------------------
*  Public Functions                                                     
*---------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME			:	DSI_SetIntegrationmode()							    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables/disables integartion mode for DSI Link*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_int_mode : Integration mode enum                     */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*               DSI_INVALID_PARAMETER :if input argument is invalid        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetIntegrationmode(IN t_dsi_link dsi_id, IN t_dsi_int_mode mode)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_integration_mode = ((t_uint32) mode & DSI_MCTL_INTMODE_MASK);

    g_dsi_system_context[dsi_id].dsi_int_mode = mode;

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_Init()							                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine initializes the DSI Link                   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_logical_address : DSI base address                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*               DSI_INVALID_PARAMETER :if input argument is invalid        */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_Init(IN t_dsi_link dsi_id, IN t_logical_address dsi_base_address)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER1("Setting Base Address for registers to %lx", dsi_base_address);

    if (NULL == dsi_base_address)
    {
        return(DSI_INVALID_PARAMETER);
    }

    g_dsi_system_context[dsi_id].dsi_register = (t_dsi_link_registers *) dsi_base_address;

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_EnableLink()							            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the DSI Link                       */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableLink(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_LINKEN_MASK) |
            ((t_uint32) DSI_SET_BIT & DSI_MCTL_LINKEN_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_link_state = DSI_ENABLE;

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableLink()							            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the DSI Link                      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableLink(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_LINKEN_MASK) |
            ((t_uint32) DSI_CLEAR_BIT & DSI_MCTL_LINKEN_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_link_state = DSI_DISABLE;
    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetInterface1_Mode()							    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the interface1 mode of the DSI Link   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_interface1_mode : command mode or video mode         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetInterface1_Mode(IN t_dsi_link dsi_id, IN t_dsi_interface1_mode mode)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_INTERFACE1_MODE_MASK) |
            (((t_uint32) mode << DSI_MCTL_INTERFACE1_MODE_SHIFT) & DSI_MCTL_INTERFACE1_MODE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_if1_mode = mode;

    DBGEXIT0(DSI_OK);
}

PUBLIC void DSI_SetDPHY_Static(IN t_dsi_link dsi_id, IN t_dphy_static dhy_static)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static &~DSI_HS_INVERT_CLK_MASK) |
            (((t_uint32) dhy_static.hs_invert_clk << DSI_HS_INVERT_CLK_SHIFT) & DSI_HS_INVERT_CLK_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static &~DSI_SWAP_PINS_DAT1_MASK) |
            (((t_uint32) dhy_static.swap_pins_dat1 << DSI_SWAP_PINS_DAT1_SHIFT) & DSI_SWAP_PINS_DAT1_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static &~DSI_HS_INVERT_DAT1_MASK) |
            (((t_uint32) dhy_static.hs_invert_dat1 << DSI_HS_INVERT_DAT1_SHIFT) & DSI_HS_INVERT_DAT1_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static &~DSI_SWAP_PINS_DAT2_MASK) |
            (((t_uint32) dhy_static.swap_pins_dat2 << DSI_SWAP_PINS_DAT2_SHIFT) & DSI_SWAP_PINS_DAT2_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static &~DSI_HS_INVERT_DAT2_MASK) |
            (((t_uint32) dhy_static.hs_invert_dat2 << DSI_HS_INVERT_DAT2_SHIFT) & DSI_HS_INVERT_DAT2_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static &~DSI_UI_X4_MASK) |
            (((t_uint32) dhy_static.ui_x4 << DSI_UI_X4_SHIFT) & DSI_UI_X4_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_static &~DSI_SWAP_PINS_CLK_MASK) |
            (((t_uint32) dhy_static.swap_pins_clk) & DSI_SWAP_PINS_CLK_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableVideoStreamGenerator()				        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the Video Stream Generator         */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableVideoStreamGenerator(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_VID_EN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_VID_EN_SHIFT) & DSI_MCTL_VID_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableVideoStreamGenerator()				        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the Video Stream Generator        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableVideoStreamGenerator(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_VID_EN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_VID_EN_SHIFT) & DSI_MCTL_VID_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableTestVideoGenerator()	    			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the Test Video Generator           */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_EnableTestVideoGenerator(IN t_dsi_link dsi_id)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if
    (
        (g_dsi_system_context[dsi_id].dsi_if1_mode == DSI_COMMAND_MODE)
    ||  (g_dsi_system_context[dsi_id].dsi_if1_state == DSI_IF1_ENABLE)
    )
    {
        return(DSI_REQUEST_NOT_APPLICABLE);
    }

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_TVG_SEL_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_TVG_SEL_SHIFT) & DSI_MCTL_TVG_SEL_MASK)
        );

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_DisableTestVideoGenerator()	    			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the Test Video Generator          */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableTestVideoGenerator(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_TVG_SEL_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_TVG_SEL_SHIFT) & DSI_MCTL_TVG_SEL_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableTestByteGenerator()	    			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the Test Byte Generator            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableTestByteGenerator(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_TBG_SEL_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_TBG_SEL_SHIFT) & DSI_MCTL_TBG_SEL_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableTestByteGenerator()	    			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disbles the Test Byte Generator            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableTestByteGenerator(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_TBG_SEL_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_TBG_SEL_SHIFT) & DSI_MCTL_TBG_SEL_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ReadEnable()	    			                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the Read operation                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ReadEnable(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_READEN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_READEN_SHIFT) & DSI_MCTL_READEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ReadDisable()	    			                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the Read operation                */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ReadDisable(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_READEN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_READEN_SHIFT) & DSI_MCTL_READEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_BTAEnable()	    			                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the BTA                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_BTAEnable(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_BTAEN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_BTAEN_SHIFT) & DSI_MCTL_BTAEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_BTADisable()	    			                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the BTA                           */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_BTADisable(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_BTAEN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_BTAEN_SHIFT) & DSI_MCTL_BTAEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableDisplayECCGen()	    			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables display to generate ECC on response*/
/*                  packets                                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableDisplayECCGen(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_DISPECCGEN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_DISPECCGEN_SHIFT) & DSI_MCTL_DISPECCGEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableDisplayECCGen()	    			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables display to generate ECC on response*/
/*                  packets                                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableDisplayECCGen(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_DISPECCGEN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_DISPECCGEN_SHIFT) & DSI_MCTL_DISPECCGEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableDisplayCheksumGen()	    			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables display to generate checksum on response*/
/*                  packets                                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableDisplayCheksumGen(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_DISPCHECKSUMGEN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_DISPCHECKSUMGEN_SHIFT) & DSI_MCTL_DISPCHECKSUMGEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableDisplayCheksumGen()	    			        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables display to generate checksum on response*/
/*                  packets                                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableDisplayCheksumGen(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_DISPCHECKSUMGEN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_DISPCHECKSUMGEN_SHIFT) & DSI_MCTL_DISPCHECKSUMGEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableDisplayEOTGen()	    			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables display to generate EOT on response*/
/*                  packets                                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableDisplayEOTGen(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_DISPEOTGEN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_DISPEOTGEN_SHIFT) & DSI_MCTL_DISPEOTGEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableDisplayEOTGen()	    			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables display to generate EOT on response*/
/*                  packets                                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableDisplayEOTGen(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_DISPEOTGEN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_DISPEOTGEN_SHIFT) & DSI_MCTL_DISPEOTGEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableHostEOTGen()	    			                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables host to generate EOT on response   */
/*                  packets                                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableHostEOTGen(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_HOSTEOTGEN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_MCTL_HOSTEOTGEN_SHIFT) & DSI_MCTL_HOSTEOTGEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableHostEOTGen()	    			                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables host to generate EOT on response  */
/*                  packets                                                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableHostEOTGen(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_HOSTEOTGEN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_MCTL_HOSTEOTGEN_SHIFT) & DSI_MCTL_HOSTEOTGEN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableTearingEffect()	    			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables TE from register or from interface */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_te_en : Register or Interface                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER : if parameter passed was invalid    */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_EnableTearingEffect(IN t_dsi_link dsi_id, IN t_dsi_te_en tearing)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    switch (tearing.te_sel)
    {
        case DSI_REG_TE:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_REG_TE_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_REG_TE_SHIFT) & DSI_REG_TE_MASK)
                );
            break;

        case DSI_IF_TE:
            if (tearing.interface == DSI_INTERFACE_1)
            {
                g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
                    (
                        (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_IF1_TE_MASK) |
                        (((t_uint32) DSI_SET_BIT << DSI_IF1_TE_SHIFT) & DSI_IF1_TE_MASK)
                    );
            }
            else if (tearing.interface == DSI_INTERFACE_2)
            {
                g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
                    (
                        (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_IF2_TE_MASK) |
                        (((t_uint32) DSI_SET_BIT << DSI_IF2_TE_SHIFT) & DSI_IF2_TE_MASK)
                    );
            }
            else
            {
                dsi_error = DSI_INVALID_PARAMETER;
            }
            break;

        default:
            dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_DisableTearingEffect()	    			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables TE from register or from interface */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_te_en : Register or Interface                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER : if parameter passed was invalid    */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_DisableTearingEffect(IN t_dsi_link dsi_id, IN t_dsi_te_en tearing)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    switch (tearing.te_sel)
    {
        case DSI_REG_TE:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_REG_TE_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_REG_TE_SHIFT) & DSI_REG_TE_MASK)
                );
            break;

        case DSI_IF_TE:
            if (tearing.interface == DSI_INTERFACE_1)
            {
                g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
                    (
                        (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_IF1_TE_MASK) |
                        (((t_uint32) DSI_CLEAR_BIT << DSI_IF1_TE_SHIFT) & DSI_IF1_TE_MASK)
                    );
            }
            else if (tearing.interface == DSI_INTERFACE_2)
            {
                g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
                    (
                        (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_IF2_TE_MASK) |
                        (((t_uint32) DSI_CLEAR_BIT << DSI_IF2_TE_SHIFT) & DSI_IF2_TE_MASK)
                    );
            }
            else
            {
                dsi_error = DSI_INVALID_PARAMETER;
            }
            break;

        default:
            dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_SetPLLControl()	    			                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the PLL control parameters            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_pll_ctl :  PLL control structure                     */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetPLLControl(IN t_dsi_link dsi_id, t_dsi_pll_ctl pll_ctl)
{
    DBGENTER0();
    
    #ifdef ST_8500ED

    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_OUT_SEL_MASK) |
            (((t_uint32) pll_ctl.pll_out_sel << DSI_PLL_OUT_SEL_SHIFT) & DSI_PLL_OUT_SEL_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_DIV_MASK) |
            (((t_uint32) pll_ctl.division_ratio << DSI_PLL_DIV_SHIFT) & DSI_PLL_DIV_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_IN_SEL_MASK) |
            (((t_uint32) pll_ctl.pll_in_sel << DSI_PLL_IN_SEL_SHIFT) & DSI_PLL_IN_SEL_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_MASTER_MASK) |
            (((t_uint32) pll_ctl.pll_master << DSI_PLL_MASTER_SHIFT) & DSI_PLL_MASTER_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_MULT_MASK) |
            ((t_uint32) pll_ctl.multiplier & DSI_PLL_MULT_MASK)
        );

	#else
	
	 g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_OUT_SEL_MASK) |
            (((t_uint32) pll_ctl.pll_out_sel << DSI_PLL_OUT_SEL_SHIFT) & DSI_PLL_OUT_SEL_MASK)
        );
	
	 g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_MASTER_MASK) |
            (((t_uint32) pll_ctl.pll_master << DSI_PLL_MASTER_SHIFT) & DSI_PLL_MASTER_MASK)
        );
        
    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_OUT_DIV_MASK) |
            (((t_uint32) pll_ctl.pll_out_div << DSI_PLL_OUT_DIV_SHIFT) & DSI_PLL_OUT_DIV_MASK)
        );
        
    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_IN_DIV_MASK) |
            (((t_uint32) pll_ctl.pll_in_div << DSI_PLL_IN_DIV_SHIFT) & DSI_PLL_IN_DIV_MASK)
        );
        
    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_SEL_DIV2_MASK) |
            (((t_uint32) pll_ctl.pll_sel_div2 << DSI_PLL_SEL_DIV2_SHIFT) & DSI_PLL_SEL_DIV2_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_pll_ctl &~DSI_PLL_MULT_MASK) |
            ((t_uint32) pll_ctl.multiplier & DSI_PLL_MULT_MASK)
        );
        
    #endif  
	
    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableLane2()	    			                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables Lane2 of DSI link                  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableLane2(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_LANE2_EN_MASK) |
            ((t_uint32) DSI_SET_BIT & DSI_LANE2_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableLane2()	    			                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables Lane2 of DSI link                 */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableLane2(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_LANE2_EN_MASK) |
            ((t_uint32) DSI_CLEAR_BIT & DSI_LANE2_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableForceStopMode()	    			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine forces back data lanes in STOP mode        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableForceStopMode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_FORCE_STOP_MODE_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_FORCE_STOP_MODE_SHIFT) & DSI_FORCE_STOP_MODE_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableForceStopMode()	    			            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables force back STOP mode on data lanes*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableForceStopMode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_FORCE_STOP_MODE_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_FORCE_STOP_MODE_SHIFT) & DSI_FORCE_STOP_MODE_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableCLK_HS_SendingMode()	    		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables HS sending mode on clock lanes     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableCLK_HS_SendingMode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_CLK_CONTINUOUS_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_CLK_CONTINUOUS_SHIFT) & DSI_CLK_CONTINUOUS_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableCLK_HS_SendingMode()	    		            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables HS sending mode on clock lanes    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableCLK_HS_SendingMode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_CLK_CONTINUOUS_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_CLK_CONTINUOUS_SHIFT) & DSI_CLK_CONTINUOUS_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableCLK_ULPM_Mode()	    		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies that clk lane can be switched in ULP mode*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableCLK_ULPM_Mode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_CLK_ULPM_EN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_CLK_ULPM_EN_SHIFT) & DSI_CLK_ULPM_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableCLK_ULPM_Mode()	    		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies that clk lane cannot be switched in ULP mode*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableCLK_ULPM_Mode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_CLK_ULPM_EN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_CLK_ULPM_EN_SHIFT) & DSI_CLK_ULPM_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableDAT1_ULPM_Mode()	    		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies that data lane1 can be switched in ULP mode*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableDAT1_ULPM_Mode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_DAT1_ULPM_EN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_DAT1_ULPM_EN_SHIFT) & DSI_DAT1_ULPM_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableDAT1_ULPM_Mode()	    		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies that data lane1 cannot be switched in ULP mode*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableDAT1_ULPM_Mode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_DAT1_ULPM_EN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_DAT1_ULPM_EN_SHIFT) & DSI_DAT1_ULPM_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_EnableDAT2_ULPM_Mode()	    		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies that data lane2 can be switched in ULP mode*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_EnableDAT2_ULPM_Mode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_DAT2_ULPM_EN_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_DAT2_ULPM_EN_SHIFT) & DSI_DAT2_ULPM_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DisableDAT2_ULPM_Mode()	    		                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies that data lane2 cannot be switched in ULP mode*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DisableDAT2_ULPM_Mode(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_DAT2_ULPM_EN_MASK) |
            (((t_uint32) DSI_CLEAR_BIT << DSI_DAT2_ULPM_EN_SHIFT) & DSI_DAT2_ULPM_EN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetWaitBurstTime()	    		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies the delay between two HS bursts  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_uint8 delay: delay value                                 */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetWaitBurstTime(IN t_dsi_link dsi_id, IN t_uint8 delay)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_WAIT_BURST_MASK) |
            (((t_uint32) delay << DSI_WAIT_BURST_SHIFT) & DSI_WAIT_BURST_MASK)
        );

    DBGEXIT0(DSI_OK);
}

#ifndef ST_8500ED
/****************************************************************************/
/* NAME			:	DSI_ForceClkStop()	           		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine forces the clock lanes in STOP mode        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_link dsi_id: DSI ID                                  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                	    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ForceClkStop(IN t_dsi_link dsi_id )
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_CLK_FORCESTOP_MASK) |
            (((t_uint32) DSI_SET_BIT << DSI_CLK_FORCESTOP_SHIFT) & DSI_CLK_FORCESTOP_MASK)
        );

    DBGEXIT0(DSI_OK);
}
#endif


/****************************************************************************/
/* NAME			:	DSI_GetCLKLaneState()	    		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the state of the clock lane           */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_dsi_clk_lane_state : clocklane state enum                */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER: if parameter is invalid             */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetCLKLaneState(IN t_dsi_link dsi_id, OUT t_dsi_clk_lane_state *clklane_state)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((NULL == clklane_state))
    {
        return(DSI_INVALID_PARAMETER);
    }

    *clklane_state = (t_dsi_clk_lane_state) (g_dsi_system_context[dsi_id].dsi_register->mctl_lane_sts & DSI_CLKLANESTS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetDataLaneState()	    		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the state of the data lanes           */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	: t_dsi_data_lane: data lane 1/2                            */
/*     InOut    :None                                                       */
/* 		OUT 	:t_dsi_data_lane_state : datalane state enum                */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER: if parameter is invalid             */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetDataLaneState
(
    IN t_dsi_link               dsi_id,
    IN t_dsi_data_lane          data_lane,
    OUT t_dsi_data_lane_state   *datalane_state
)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((NULL == datalane_state))
    {
        return(DSI_INVALID_PARAMETER);
    }

    if (data_lane == DSI_DAT_LANE1)
    {
        *datalane_state = (t_dsi_data_lane_state)
            (
                (g_dsi_system_context[dsi_id].dsi_register->mctl_lane_sts & DSI_DATALANE1STS_MASK) >>
                DSI_DATALANE1STS_SHIFT
            );
    }
    else
    {
        *datalane_state = (t_dsi_data_lane_state)
            (
                (g_dsi_system_context[dsi_id].dsi_register->mctl_lane_sts & DSI_DATALANE2STS_MASK) >>
                DSI_DATALANE2STS_SHIFT
            );
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_SetDPHY_TimeOut()	    		                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the timeout values of High Speed      */
/*                  Transmission and Low Speed Reception                    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:  t_dsi_dphy_timeout: timeout structure                    */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetDPHY_TimeOut(IN t_dsi_link dsi_id, IN t_dsi_dphy_timeout timeout)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_timeout =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_timeout &~DSI_HSTX_TO_MASK) |
            (((t_uint32) timeout.hs_tx_timeout << DSI_HSTX_TO_SHIFT) & DSI_HSTX_TO_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_timeout =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_timeout &~DSI_LPRX_TO_MASK) |
            (((t_uint32) timeout.lp_rx_timeout << DSI_LPRX_TO_SHIFT) & DSI_LPRX_TO_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_timeout =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_timeout &~DSI_CLK_DIV_MASK) |
            ((t_uint32) timeout.clk_div & DSI_CLK_DIV_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetDataLaneULP_OutTime()	                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies the time to leave ULP mode for data lanes*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:  t_uint16 timeout: timeout value                          */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetDataLaneULP_OutTime(IN t_dsi_link dsi_id, IN t_uint16 timeout)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_ulpout_time =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_ulpout_time &~DSI_DATA_ULPOUT_MASK) |
            (((t_uint32) timeout << DSI_DATA_ULPOUT_SHIFT) & DSI_DATA_ULPOUT_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetCLKLaneULP_OutTime()	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine specifies the time to leave ULP mode for clock lane*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:  t_uint16 timeout: timeout value                          */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetCLKLaneULP_OutTime(IN t_dsi_link dsi_id, IN t_uint16 timeout)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_ulpout_time =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_ulpout_time &~DSI_CLK_ULPOUT_MASK) |
            ((t_uint32) timeout & DSI_DATA_ULPOUT_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_StartPLL()	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine starts the PLL                             */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_StartPLL(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_PLL_START_MASK) |
            ((t_uint32) DSI_SET_BIT & DSI_PLL_START_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_StopPLL()	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine stops the PLL                              */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_StopPLL(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_PLL_START_MASK) |
            ((t_uint32) DSI_CLEAR_BIT & DSI_PLL_START_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_StartLane()	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine starts the DSI Link lanes                  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_lane : data lane 1/2 or clock lane                   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_StartLane(IN t_dsi_link dsi_id, IN t_dsi_lane lane)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((lane > DSI_DATA_LANE2))
    {
        return(DSI_INVALID_PARAMETER);
    }

    switch (lane)
    {
        case DSI_CLK_LANE:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_CKLANE_EN_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_CKLANE_EN_SHIFT) & DSI_CKLANE_EN_MASK)
                );
            break;

        case DSI_DATA_LANE1:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT1_EN_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_DAT1_EN_SHIFT) & DSI_DAT1_EN_MASK)
                );
            break;

        case DSI_DATA_LANE2:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT2_EN_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_DAT2_EN_SHIFT) & DSI_DAT2_EN_MASK)
                );
            break;

        default:
            dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_StopLane()	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine stops the DSI Link lanes                  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_lane : data lane 1/2 or clock lane                   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_StopLane(IN t_dsi_link dsi_id, IN t_dsi_lane lane)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((lane > DSI_DATA_LANE2))
    {
        return(DSI_INVALID_PARAMETER);
    }

    switch (lane)
    {
        case DSI_CLK_LANE:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_CKLANE_EN_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_CKLANE_EN_SHIFT) & DSI_CKLANE_EN_MASK)
                );
            break;

        case DSI_DATA_LANE1:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT1_EN_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_DAT1_EN_SHIFT) & DSI_DAT1_EN_MASK)
                );
            break;

        case DSI_DATA_LANE2:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT2_EN_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_DAT2_EN_SHIFT) & DSI_DAT2_EN_MASK)
                );
            break;

        default:
            dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_StartULPM_Mode()	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine switches the DSI Link lanes in ULPM mode   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_lane : data lane 1/2 or clock lane                   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_StartULPM_Mode(IN t_dsi_link dsi_id, IN t_dsi_lane lane)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    switch (lane)
    {
        case DSI_CLK_LANE:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_CLK_ULPM_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_CLK_ULPM_SHIFT) & DSI_CLK_ULPM_MASK)
                );
            break;

        case DSI_DATA_LANE1:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT1_ULPM_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_DAT1_ULPM_SHIFT) & DSI_DAT1_ULPM_MASK)
                );
            break;

        case DSI_DATA_LANE2:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT2_ULPM_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_DAT2_ULPM_SHIFT) & DSI_DAT2_ULPM_MASK)
                );
            break;

        default:
            dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_StopULPM_Mode()	                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine switches the DSI Link lanes back from ULPM mode */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_lane : data lane 1/2 or clock lane                   */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_StopULPM_Mode(IN t_dsi_link dsi_id, IN t_dsi_lane lane)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((lane > DSI_DATA_LANE2))
    {
        return(DSI_INVALID_PARAMETER);
    }

    switch (lane)
    {
        case DSI_CLK_LANE:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_CLK_ULPM_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_CLK_ULPM_SHIFT) & DSI_CLK_ULPM_MASK)
                );
            break;

        case DSI_DATA_LANE1:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT1_ULPM_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_DAT1_ULPM_SHIFT) & DSI_DAT1_ULPM_MASK)
                );
            break;

        case DSI_DATA_LANE2:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT2_ULPM_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_DAT2_ULPM_SHIFT) & DSI_DAT2_ULPM_MASK)
                );
            break;

        default:
            dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_EnableInterface()	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the interface                      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_interface : interface1/2                             */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_EnableInterface(IN t_dsi_link dsi_id, IN t_dsi_interface interface)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    switch (interface)
    {
        case DSI_INTERFACE_1:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_IF1_EN_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_IF1_EN_SHIFT) & DSI_IF1_EN_MASK)
                );
            break;

        case DSI_INTERFACE_2:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_IF2_EN_MASK) |
                    (((t_uint32) DSI_SET_BIT << DSI_IF2_EN_SHIFT) & DSI_IF2_EN_MASK)
                );
            break;

        default:
            dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_DisableInterface()	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the interface                     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_interface : interface1/2                             */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_DisableInterface(IN t_dsi_link dsi_id, IN t_dsi_interface interface)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    switch (interface)
    {
        case DSI_INTERFACE_1:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_IF1_EN_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_IF1_EN_SHIFT) & DSI_IF1_EN_MASK)
                );
            break;

        case DSI_INTERFACE_2:
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_IF2_EN_MASK) |
                    (((t_uint32) DSI_CLEAR_BIT << DSI_IF2_EN_SHIFT) & DSI_IF2_EN_MASK)
                );
            break;

        default:
            dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetLinkStatus()	                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine reads the Link status                      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint8 *p_status : pointer to link status                 */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetLinkStatus(IN t_dsi_link dsi_id, OUT t_uint8 *p_status)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((p_status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *p_status = (t_uint8) (g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts & DSI_MAIN_STS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetDPHYError()	                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine reads the Errors on DPHY link              */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint16 *p_error : pointer to error status                */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetDPHYError(IN t_dsi_link dsi_id, OUT t_uint16 *p_error)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((p_error == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *p_error = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err & DSI_DPHY_ERROR_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetStreamObservation()	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine reads the Video and Command Stream observation */
/*                  in integration mode                                     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_interface1_mode: interface mode                      */
/*     InOut    :None                                                       */
/* 		OUT 	:t_dsi_int_read *p_rd_data : observation data               */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetStreamObservation
(
    IN t_dsi_link               dsi_id,
    IN t_dsi_interface1_mode    mode,
    OUT t_dsi_int_read          *p_rd_data
)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((p_rd_data == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    if (g_dsi_system_context[dsi_id].dsi_int_mode != DSI_INT_MODE_ENABLE)
    {
        return(DSI_REQUEST_NOT_APPLICABLE);
    }

    switch (mode)
    {
        case DSI_VIDEO_MODE:
            p_rd_data->if_data = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->int_vid_rddata & DSI_IF_DATA_MASK);

            p_rd_data->if_valid = (t_dsi_signal_state) ((g_dsi_system_context[dsi_id].dsi_register->int_vid_rddata & DSI_IF_VALID_MASK) >> DSI_IF_VALID_SHIFT);

            p_rd_data->if_start = (t_dsi_signal_state) ((g_dsi_system_context[dsi_id].dsi_register->int_vid_rddata & DSI_IF_START_MASK) >> DSI_IF_START_SHIFT);

            p_rd_data->if_frame_sync = (t_dsi_signal_state)
                (
                    (g_dsi_system_context[dsi_id].dsi_register->int_vid_rddata & DSI_IF_FRAME_SYNC_MASK) >>
                    DSI_IF_FRAME_SYNC_SHIFT
                );

            break;

        case DSI_COMMAND_MODE:
            p_rd_data->if_data = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->int_cmd_rddata & DSI_IF_DATA_MASK);

            p_rd_data->if_valid = (t_dsi_signal_state) ((g_dsi_system_context[dsi_id].dsi_register->int_cmd_rddata & DSI_IF_VALID_MASK) >> DSI_IF_VALID_SHIFT);

            p_rd_data->if_start = (t_dsi_signal_state) ((g_dsi_system_context[dsi_id].dsi_register->int_cmd_rddata & DSI_IF_START_MASK) >> DSI_IF_START_SHIFT);

            p_rd_data->if_frame_sync = (t_dsi_signal_state)
                (
                    (g_dsi_system_context[dsi_id].dsi_register->int_cmd_rddata & DSI_IF_FRAME_SYNC_MASK) >>
                    DSI_IF_FRAME_SYNC_SHIFT
                );

            break;

        default:
            DBGEXIT0(DSI_INVALID_PARAMETER);
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_SetStallSignal()      	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine drives the stall signal                    */
/*                  in integration mode                                     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_interface1_mode: interface mode                      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_SetStallSignal(IN t_dsi_link dsi_id, IN t_dsi_interface1_mode mode)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if (g_dsi_system_context[dsi_id].dsi_int_mode != DSI_INT_MODE_ENABLE)
    {
        return(DSI_REQUEST_NOT_APPLICABLE);
    }

    switch (mode)
    {
        case DSI_VIDEO_MODE:
            g_dsi_system_context[dsi_id].dsi_register->int_vid_gnt =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->int_vid_gnt &~DSI_IF_STALL_MASK) |
                    ((t_uint32) DSI_SET_BIT & DSI_IF_STALL_MASK)
                );

            break;

        case DSI_COMMAND_MODE:
            g_dsi_system_context[dsi_id].dsi_register->int_cmd_gnt =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->int_cmd_gnt &~DSI_IF_STALL_MASK) |
                    ((t_uint32) DSI_SET_BIT & DSI_IF_STALL_MASK)
                );

            break;

        default:
            DBGEXIT0(DSI_INVALID_PARAMETER);
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_ResetStallSignal()	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine resets the stall signal                    */
/*                  in integration mode                                     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_interface1_mode: interface mode                      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_ResetStallSignal(IN t_dsi_link dsi_id, IN t_dsi_interface1_mode mode)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if (g_dsi_system_context[dsi_id].dsi_int_mode != DSI_INT_MODE_ENABLE)
    {
        return(DSI_REQUEST_NOT_APPLICABLE);
    }

    switch (mode)
    {
        case DSI_VIDEO_MODE:
            g_dsi_system_context[dsi_id].dsi_register->int_vid_gnt =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->int_vid_gnt &~DSI_IF_STALL_MASK) |
                    ((t_uint32) DSI_CLEAR_BIT & DSI_IF_STALL_MASK)
                );

            break;

        case DSI_COMMAND_MODE:
            g_dsi_system_context[dsi_id].dsi_register->int_cmd_gnt =
                (
                    (g_dsi_system_context[dsi_id].dsi_register->int_cmd_gnt &~DSI_IF_STALL_MASK) |
                    ((t_uint32) DSI_CLEAR_BIT & DSI_IF_STALL_MASK)
                );

            break;

        default:
            DBGEXIT0(DSI_INVALID_PARAMETER);
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_SetInterruptActive()	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine drives the signal interrupt active         */
/*                  in integration mode                                     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_SetInterruptActive(IN t_dsi_link dsi_id)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if (g_dsi_system_context[dsi_id].dsi_int_mode != DSI_INT_MODE_ENABLE)
    {
        return(DSI_REQUEST_NOT_APPLICABLE);
    }

    g_dsi_system_context[dsi_id].dsi_register->int_interrupt_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->int_interrupt_ctl &~DSI_INT_VAL_MASK) |
            ((t_uint32) DSI_SET_BIT & DSI_INT_VAL_MASK)
        );

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_ResetInterruptActive()	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine stops the signal interrupt active          */
/*                  in integration mode                                     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_ResetInterruptActive(IN t_dsi_link dsi_id)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if (g_dsi_system_context[dsi_id].dsi_int_mode != DSI_INT_MODE_ENABLE)
    {
        return(DSI_REQUEST_NOT_APPLICABLE);
    }

    g_dsi_system_context[dsi_id].dsi_register->int_interrupt_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->int_interrupt_ctl &~DSI_INT_VAL_MASK) |
            ((t_uint32) DSI_CLEAR_BIT & DSI_INT_VAL_MASK)
        );

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_DirectCommandSend()	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine starts the sending of direct commands      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DirectCommandSend(IN t_dsi_link dsi_id)
{
    DBGENTER0();

     g_dsi_system_context[dsi_id].dsi_register->direct_cmd_send =
        (
            ((t_uint32) DSI_SET_BIT & DSI_INT_VAL_MASK)
        );
    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_GetDirectCmdReadStatus()	                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the status of read comands received   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint16 *p_read_status: status pointer                    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetDirectCmdReadStatus(IN t_dsi_link dsi_id, OUT t_uint16 *p_read_status)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((NULL == p_read_status))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *p_read_status = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts & DSI_DIRECT_CMD_RD_STS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetCmdModeStatus()	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the status of generic commands        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint8  *p_read_status: status pointer                    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetCmdModeStatus(IN t_dsi_link dsi_id, OUT t_uint8 *p_read_status)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((NULL == p_read_status))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *p_read_status = (t_uint8) (g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts & DSI_CMD_MODE_STS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_WriteDirectCommand()	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine writes the direct commands to the Direct   */
/*                  Command registers                                       */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_uint8 data_num : number of commands                      */
/*               t_uint8 *data : pointer to commands                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_WriteDirectCommand(IN t_dsi_link dsi_id, IN t_uint8 data_num, IN t_uint8 *data)
{
    t_dsi_error dsi_error = DSI_OK;
    t_uint32    write_data[4];
    t_uint8     num;
    t_uint8     data_remain;
    t_uint8     icount1 = 0, icount2 = 0;
    t_uint8     data0,data1,data2,data3;

    DBGENTER0();

    if ((NULL == data) || (data_num == 0))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    write_data[0] = 0;
    write_data[1] = 0;
    write_data[2] = 0;
    write_data[3] = 0;

    if (data_num > 16)
    {
        data_num = 16;
    }

    num = data_num / 4;

    data_remain = data_num % 4;
    


    for (icount1 = 0; icount1 < num; icount1++)
    {
    	data0 = *data;
    
    	data1 = *(++data);
    
    	data2 = *(++data);
    
    	data3 = *(++data);
    	
        write_data[icount1] = (data0 | (data1 << 8) | (data2 << 16) | (data3 << 24));
        data++;
    }

    if(data_remain != 0)
    {
    	write_data[icount1] = 0;

    	for (icount2 = 0; icount2 < data_remain; icount2++)
    	{
    	
        	write_data[icount1] = write_data[icount1] | ((*data) << (icount2 * 8));

        	data++;
    	}

    }
    
    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_wrdat0 = write_data[0];

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_wrdat1 = write_data[1];

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_wrdat2 = write_data[2];

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_wrdat3 = write_data[3];

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

PUBLIC void DSI_SetDirectCommand
(
    IN t_dsi_link   dsi_id,
    IN t_uint32     cmd0,
    IN t_uint32     cmd1,
    IN t_uint32     cmd2,
    IN t_uint32     cmd3
)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_wrdat0 = cmd0;

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_wrdat1 = cmd1;

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_wrdat2 = cmd2;

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_wrdat3 = cmd3;

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ReadCmdCharacteristics()	                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the charecteristics of command read   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_dsi_cmd_rd_property *p_cmd_rd: charecteristic pointer    */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_ReadCmdCharacteristics(IN t_dsi_link dsi_id, OUT t_dsi_cmd_rd_property *p_cmd_rd)
{
    t_dsi_error dsi_error = DSI_OK;

    DBGENTER0();

    if ((NULL == p_cmd_rd))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    p_cmd_rd->rd_size = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_property & MASK_HALFWORD0);

    p_cmd_rd->rd_id = (t_dsi_virtual_ch) ((g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_property & DSI_RD_ID_MASK) >> DSI_RD_ID_SHIFT);

    p_cmd_rd->cmd_type = (t_dsi_cmd_type)
        (
            (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_property & DSI_RD_DCSNOTGENERIC_MASK) >>
            DSI_RD_DCSNOTGENERIC_SHIFT
        );

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_Read_CmdDat()              	                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine reads the receive fifo output              */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_dsi_cmd_rddat *p_rddat: pointer to read data             */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_Read_CmdDat(IN t_dsi_link dsi_id, OUT t_dsi_cmd_rddat *p_rddat)
{
    t_dsi_error dsi_error = DSI_OK;
    t_uint32    read_data;

    DBGENTER0();

    if ((NULL == p_rddat))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    read_data = (t_uint32) (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rddat);

    p_rddat->rddat0 = (t_uint8) (read_data & MASK_BYTE0);

    p_rddat->rddat1 = (t_uint8) ((read_data & MASK_BYTE1) >> 8);

    p_rddat->rddat2 = (t_uint8) ((read_data & MASK_BYTE2) >> 16);

    p_rddat->rddat3 = (t_uint8) ((read_data & MASK_BYTE3) >> 24);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_SetDirectCmdSettings()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the property of direct commands       */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_cmd_main_setting cmd_settings                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_INVALID_PARAMETER                                      */
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetDirectCmdSettings(IN t_dsi_link dsi_id, IN t_dsi_cmd_main_setting cmd_settings)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings =
        (
            (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings &~DSI_CMD_NAT_MASK) |
            ((t_uint32) cmd_settings.cmd_nature & DSI_CMD_NAT_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings =
        (
            (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings &~DSI_CMD_LONGNOTSHORT_MASK) |
            (((t_uint32) cmd_settings.packet_type << DSI_CMD_LONGNOTSHORT_SHIFT) & DSI_CMD_LONGNOTSHORT_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings =
        (
            (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings &~DSI_CMD_HEAD_MASK) |
            (((t_uint32) cmd_settings.cmd_header << DSI_CMD_HEAD_SHIFT) & DSI_CMD_HEAD_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings =
        (
            (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings &~DSI_CMD_ID_MASK) |
            (((t_uint32) cmd_settings.cmd_id << DSI_CMD_ID_SHIFT) & DSI_CMD_ID_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings =
        (
            (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings &~DSI_CMD_SIZE_MASK) |
            (((t_uint32) cmd_settings.cmd_size << DSI_CMD_SIZE_SHIFT) & DSI_CMD_SIZE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings =
        (
            (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings &~DSI_CMD_LP_EN_MASK) |
            (((t_uint32) cmd_settings.cmd_lp_enable << DSI_CMD_LP_EN_SHIFT) & DSI_CMD_LP_EN_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings =
        (
            (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_main_settings &~DSI_TRIGGER_VAL_MASK) |
            (((t_uint32) cmd_settings.cmd_trigger_val << DSI_TRIGGER_VAL_SHIFT) & DSI_TRIGGER_VAL_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetTE_Timeout()	                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the TE timeout value                  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_uint32 te_timeout : timeout value                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetTE_Timeout(IN t_dsi_link dsi_id, IN t_uint32 te_timeout)
{
    t_uint8             te_lowerbits;
    t_dsi_te_timeout    te_upperbits;

    DBGENTER0();

    if (te_timeout / 256 == 1)
    {
        te_lowerbits = (t_uint8) (te_timeout % 256);
        te_upperbits = DSI_TE_256;
    }
    else if (te_timeout / 512 == 1)
    {
        te_lowerbits = (t_uint8) (te_timeout % 512);
        te_upperbits = DSI_TE_512;
    }
    else if (te_timeout / 1024 == 1)
    {
        te_lowerbits = (t_uint8) (te_timeout % 1024);
        te_upperbits = DSI_TE_1024;
    }
    else
    {
        te_lowerbits = (t_uint8) (te_timeout % 2048);
        te_upperbits = DSI_TE_2048;
    }

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl &~DSI_TE_LOWERBIT_MASK) |
            (((t_uint32) te_lowerbits << DSI_TE_LOWERBIT_SHIFT) & DSI_TE_LOWERBIT_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl &~DSI_TE_UPPERBIT_MASK) |
            (((t_uint32) te_upperbits << DSI_TE_UPPERBIT_SHIFT) & DSI_TE_UPPERBIT_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetPaddingVal()	                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets value to use to fill packet during    */
/*                  data underrun or to complete unterminated               */
/*                  packet (referred as padding value)                      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_uint8 padding : padding value                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetPaddingVal(IN t_dsi_link dsi_id, IN t_uint8 padding)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl &~DSI_FIL_VAL_MASK) |
            (((t_uint32) padding << DSI_FIL_VAL_SHIFT) & DSI_FIL_VAL_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetArbCtl()	                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the arbitration mode and in fixed mode*/
/*                  its priority                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_arb_ctl : arbitration control structure              */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetArbCtl(IN t_dsi_link dsi_id, IN t_dsi_arb_ctl arb_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl &~DSI_ARB_MODE_MASK) |
            (((t_uint32) arb_ctl.arb_mode << DSI_ARB_MODE_SHIFT) & DSI_ARB_MODE_MASK)
        );

    if (arb_ctl.arb_mode == DSI_ARB_MODE_FIXED)
    {
        g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl =
            (
                (g_dsi_system_context[dsi_id].dsi_register->cmd_mode_ctl &~DSI_ARB_PRI_MASK) |
                (((t_uint32) arb_ctl.arb_mode << DSI_ARB_PRI_SHIFT) & DSI_ARB_PRI_MASK)
            );
    }

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetVideoMainControl()	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the control parameters of Video stream*/
/*                 generator                                                */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_vid_main_ctl : video mode control structure          */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetVideoMainControl(IN t_dsi_link dsi_id, IN t_dsi_vid_main_ctl vid_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_STOP_MODE_MASK) |
            (((t_uint32) vid_ctl.vid_stop_mode << DSI_STOP_MODE_SHIFT) & DSI_STOP_MODE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_VID_ID_MASK) |
            (((t_uint32) vid_ctl.vid_id << DSI_VID_ID_SHIFT) & DSI_VID_ID_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_HEADER_MASK) |
            (((t_uint32) vid_ctl.header << DSI_HEADER_SHIFT) & DSI_HEADER_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_PIXEL_MODE_MASK) |
            (((t_uint32) vid_ctl.vid_pixel_mode << DSI_PIXEL_MODE_SHIFT) & DSI_PIXEL_MODE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_BURST_MODE_MASK) |
            (((t_uint32) vid_ctl.vid_burst_mode << DSI_BURST_MODE_SHIFT) & DSI_BURST_MODE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_SYNC_PULSE_ACTIVE_MASK) |
            (((t_uint32) vid_ctl.sync_pulse_active << DSI_SYNC_PULSE_ACTIVE_SHIFT) & DSI_SYNC_PULSE_ACTIVE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_SYNC_PULSE_HORIZONTAL_MASK) |
            (((t_uint32) vid_ctl.sync_pulse_horizontal << DSI_SYNC_PULSE_HORIZONTAL_SHIFT) & DSI_SYNC_PULSE_HORIZONTAL_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_BLKLINE_MASK) |
            (((t_uint32) vid_ctl.blkline_mode << DSI_BLKLINE_SHIFT) & DSI_BLKLINE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_BLKEOL_MASK) |
            (((t_uint32) vid_ctl.blkeol_mode << DSI_BLKEOL_SHIFT) & DSI_BLKEOL_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_RECOVERY_MODE_MASK) |
            (((t_uint32) vid_ctl.recovery_mode << DSI_RECOVERY_MODE_SHIFT) & DSI_RECOVERY_MODE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_main_ctl &~DSI_START_MODE_MASK) |
            ((t_uint32) vid_ctl.vid_start_mode & DSI_START_MODE_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetVideoVerticalSize()	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the image vertical setting            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_img_vertical_size : image size structure             */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetVideoVerticalSize(IN t_dsi_link dsi_id, IN t_dsi_img_vertical_size vid_vsize)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_vsize =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vsize &~DSI_VACT_LENGTH_MASK) |
            (((t_uint32) vid_vsize.vact_length << DSI_VACT_LENGTH_SHIFT) & DSI_VACT_LENGTH_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_vsize =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vsize &~DSI_VFP_LENGTH_MASK) |
            (((t_uint32) vid_vsize.vfp_length << DSI_VFP_LENGTH_SHIFT) & DSI_VFP_LENGTH_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_vsize =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vsize &~DSI_VBP_LENGTH_MASK) |
            (((t_uint32) vid_vsize.vbp_length << DSI_VBP_LENGTH_SHIFT) & DSI_VBP_LENGTH_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_vsize =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vsize &~DSI_VSA_LENGTH_MASK) |
            ((t_uint32) vid_vsize.vsa_length & DSI_VSA_LENGTH_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetVideoHorizontalSize()	                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the image horizontal setting          */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_img_horizontal_size : image size structure           */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetVideoHorizontalSize(IN t_dsi_link dsi_id, IN t_dsi_img_horizontal_size vid_hsize)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_hsize1 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_hsize1 &~DSI_HBP_LENGTH_MASK) |
            (((t_uint32) vid_hsize.hbp_length << DSI_HBP_LENGTH_SHIFT) & DSI_HBP_LENGTH_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_hsize1 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_hsize1 &~DSI_HFP_LENGTH_MASK) |
            (((t_uint32) vid_hsize.hfp_length << DSI_HFP_LENGTH_SHIFT) & DSI_HFP_LENGTH_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_hsize1 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_hsize1 &~DSI_HSA_LENGTH_MASK) |
            ((t_uint32) vid_hsize.hsa_length & DSI_HSA_LENGTH_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_hsize2 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_hsize2 &~DSI_RGB_SIZE_MASK) |
            ((t_uint32) vid_hsize.rgb_size & DSI_RGB_SIZE_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetVideoPosition()	                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the image horizontal and vertical position*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_img_position : image position structure              */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetVideoPosition(IN t_dsi_link dsi_id, IN t_dsi_img_position vid_pos)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_vpos =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vpos &~DSI_LINE_POS_MASK) |
            ((t_uint32) vid_pos.line_pos & DSI_LINE_POS_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_vpos =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vpos &~DSI_LINE_VAL_MASK) |
            (((t_uint32) vid_pos.line_val << DSI_LINE_VAL_SHIFT) & DSI_LINE_VAL_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_hpos =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_hpos &~DSI_HORI_POS_MASK) |
            ((t_uint32) vid_pos.horizontal_pos & DSI_HORI_POS_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_hpos =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_hpos &~DSI_HORI_VAL_MASK) |
            (((t_uint32) vid_pos.horizontal_val << DSI_HORI_VAL_SHIFT) & DSI_HORI_VAL_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_GetVideoModeStatus()	                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the status and error of video mode    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint16 *p_vid_mode_sts: pointer to status                */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetVideoModeStatus(IN t_dsi_link dsi_id, OUT t_uint16 *p_vid_mode_sts)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((p_vid_mode_sts == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *p_vid_mode_sts = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts & DSI_VID_MODE_STS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_SetVCAControl()	                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets Video Command arbitartor control parameters*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_vca_setting : VCA settings structure                 */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetVCAControl(IN t_dsi_link dsi_id, IN t_dsi_vca_setting vca_setting)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_vca_setting1 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vca_setting1 &~DSI_BURST_LP_MASK) |
            (((t_uint32) vca_setting.burst_lp << DSI_BURST_LP_SHIFT) & DSI_BURST_LP_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_vca_setting1 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vca_setting1 &~DSI_MAX_BURST_LIMIT_MASK) |
            ((t_uint32) vca_setting.max_burst_limit & DSI_MAX_BURST_LIMIT_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_vca_setting2 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vca_setting2 &~DSI_MAX_LINE_LIMIT_MASK) |
            (((t_uint32) vca_setting.max_line_limit << DSI_MAX_LINE_LIMIT_SHIFT) & DSI_MAX_LINE_LIMIT_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_vca_setting2 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_vca_setting2 &~DSI_EXACT_BURST_LIMIT_MASK) |
            ((t_uint32) vca_setting.exact_burst_limit & DSI_EXACT_BURST_LIMIT_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetBlankingControl()                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets blanking packets length and events    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_vid_blanking : video blanking structure              */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetBlankingControl(IN t_dsi_link dsi_id, IN t_dsi_vid_blanking blksize)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_blksize1 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_blksize1 &~DSI_BLKEOL_PCK_MASK) |
            (((t_uint32) blksize.blkeol_pck << DSI_BLKEOL_PCK_SHIFT) & DSI_BLKEOL_PCK_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_blksize1 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_blksize1 &~DSI_BLKLINE_EVENT_MASK) |
            ((t_uint32) blksize.blkline_event_pck & DSI_BLKLINE_EVENT_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_blksize2 =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_blksize2 &~DSI_BLKLINE_PULSE_PCK_MASK) |
            ((t_uint32) blksize.blkline_pulse_pck & DSI_BLKLINE_PULSE_PCK_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_pck_time =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_pck_time &~DSI_VERT_BLANK_DURATION_MASK) |
            (((t_uint32) blksize.vert_balnking_duration << DSI_VERT_BLANK_DURATION_SHIFT) & DSI_VERT_BLANK_DURATION_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_pck_time =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_pck_time &~DSI_BLKEOL_DURATION_MASK) |
            ((t_uint32) blksize.blkeol_duration & DSI_BLKEOL_DURATION_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetVID_ERRColor()                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the color to be used in case of error */
/*                  in video data                                           */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_vid_err_color : error color structure                */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetVID_ERRColor(IN t_dsi_link dsi_id, IN t_dsi_vid_err_color err_color)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_err_color =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_err_color &~DSI_COL_GREEN_MASK) |
            (((t_uint32) err_color.col_green << DSI_COL_GREEN_SHIFT) & DSI_COL_GREEN_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_err_color =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_err_color &~DSI_COL_BLUE_MASK) |
            (((t_uint32) err_color.col_blue << DSI_COL_BLUE_SHIFT) & DSI_COL_BLUE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_err_color =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_err_color &~DSI_PAD_VAL_MASK) |
            (((t_uint32) err_color.pad_val << DSI_PAD_VAL_SHIFT) & DSI_PAD_VAL_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_err_color =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_err_color &~DSI_COL_RED_MASK) |
            ((t_uint32) err_color.col_red & DSI_COL_RED_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetTVGControl()                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets Test Video Generator Control Settings */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_tvg_control : TVG control structure                  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetTVGControl(IN t_dsi_link dsi_id, IN t_dsi_tvg_control tvg_control)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tvg_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->tvg_ctl &~DSI_TVG_STRIPE_MASK) |
            (((t_uint32) tvg_control.tvg_stripe_size << DSI_TVG_STRIPE_SHIFT) & DSI_TVG_STRIPE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->tvg_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->tvg_ctl &~DSI_TVG_MODE_MASK) |
            (((t_uint32) tvg_control.tvg_mode << DSI_TVG_MODE_SHIFT) & DSI_TVG_MODE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->tvg_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->tvg_ctl &~DSI_TVG_STOPMODE_MASK) |
            (((t_uint32) tvg_control.stop_mode << DSI_TVG_STOPMODE_SHIFT) & DSI_TVG_STOPMODE_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_TVGStart()                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine triggers the TVG starts                    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_TVGStart(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tvg_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->tvg_ctl &~DSI_TVG_RUN_MASK) |
            ((t_uint32) DSI_ENABLE & DSI_TVG_RUN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_TVGStop()                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine triggers the TVG stop                      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_TVGStop(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tvg_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->tvg_ctl &~DSI_TVG_RUN_MASK) |
            ((t_uint32) DSI_DISABLE & DSI_TVG_RUN_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetTVGImgSize()                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets Test Video Generator Image Size       */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_tvg_img_size : TVG image size                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetTVGImgSize(IN t_dsi_link dsi_id, IN t_dsi_tvg_img_size img_size)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tvg_img_size =
        (
            (g_dsi_system_context[dsi_id].dsi_register->tvg_img_size &~DSI_TVG_NBLINE_MASK) |
            (((t_uint32) img_size.tvg_nbline << DSI_TVG_NBLINE_SHIFT) & DSI_TVG_NBLINE_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->tvg_img_size =
        (
            (g_dsi_system_context[dsi_id].dsi_register->tvg_img_size &~DSI_TVG_LINE_SIZE_MASK) |
            ((t_uint32) img_size.tvg_line_size & DSI_TVG_LINE_SIZE_MASK)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetTVGColor()                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets Test Video Generator Image Color type */
/*                  and color                                               */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_color_type : color type enum                         */
/*               t_dsi_frame_color : frame color                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_SetTVGColor(IN t_dsi_link dsi_id, IN t_dsi_color_type color_type, IN t_dsi_frame_color color)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if (color_type == DSI_TVG_COLOR1)
    {
        g_dsi_system_context[dsi_id].dsi_register->tvg_color1 =
            (
                (g_dsi_system_context[dsi_id].dsi_register->tvg_color1 &~DSI_COL_RED_MASK) |
                ((t_uint32) color.col_red & DSI_COL_RED_MASK)
            );

        g_dsi_system_context[dsi_id].dsi_register->tvg_color1 =
            (
                (g_dsi_system_context[dsi_id].dsi_register->tvg_color1 &~DSI_COL_GREEN_MASK) |
                (((t_uint32) color.col_green << DSI_COL_GREEN_SHIFT) & DSI_COL_GREEN_MASK)
            );

        g_dsi_system_context[dsi_id].dsi_register->tvg_color1 =
            (
                (g_dsi_system_context[dsi_id].dsi_register->tvg_color1 &~DSI_COL_BLUE_MASK) |
                (((t_uint32) color.col_blue << DSI_COL_BLUE_SHIFT) & DSI_COL_BLUE_MASK)
            );
    }
    else if (color_type == DSI_TVG_COLOR2)
    {
        g_dsi_system_context[dsi_id].dsi_register->tvg_color2 =
            (
                (g_dsi_system_context[dsi_id].dsi_register->tvg_color2 &~DSI_COL_RED_MASK) |
                ((t_uint32) color.col_red & DSI_COL_RED_MASK)
            );

        g_dsi_system_context[dsi_id].dsi_register->tvg_color2 =
            (
                (g_dsi_system_context[dsi_id].dsi_register->tvg_color2 &~DSI_COL_GREEN_MASK) |
                (((t_uint32) color.col_green << DSI_COL_GREEN_SHIFT) & DSI_COL_GREEN_MASK)
            );

        g_dsi_system_context[dsi_id].dsi_register->tvg_color2 =
            (
                (g_dsi_system_context[dsi_id].dsi_register->tvg_color2 &~DSI_COL_BLUE_MASK) |
                (((t_uint32) color.col_blue << DSI_COL_BLUE_SHIFT) & DSI_COL_BLUE_MASK)
            );
    }
    else
    {
        dsi_error = DSI_INVALID_PARAMETER;
    }

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetTVGState()                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the TVG state                         */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_dsi_tvg_state *p_tvg_state: pointer to TVG state         */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetTVGState(IN t_dsi_link dsi_id, OUT t_dsi_tvg_state *p_tvg_state)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((p_tvg_state == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *p_tvg_state = (t_dsi_tvg_state) (g_dsi_system_context[dsi_id].dsi_register->tvg_sts & DSI_MAIN_STS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetTBGState()                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the TBG state                         */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_dsi_tbg_state *p_tbg_state: pointer to TBG state         */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetTBGState(IN t_dsi_link dsi_id, OUT t_dsi_tbg_state *p_tbg_state)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((p_tbg_state == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *p_tbg_state = (t_dsi_tbg_state) (g_dsi_system_context[dsi_id].dsi_register->tbg_sts & DSI_MAIN_STS_MASK);

    DBGEXIT0(DSI_OK);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_MCTLMainSTSControlEnable()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the MCTL Main Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_link_status : interrupt enum                         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_MCTLMainSTSControlEnable(IN t_dsi_link dsi_id, IN t_dsi_link_status sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_ctl |
            (t_uint32) sts_ctl
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_MCTLMainSTSControlDisable()                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the MCTL Main Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_link_status : interrupt enum                         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_MCTLMainSTSControlDisable(IN t_dsi_link dsi_id, IN t_dsi_link_status sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_ctl |
            ~((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetFallingEdgeMCTLMainSTSControl()                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the MCTL Main Status interrupts as    */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_link_status : interrupt enum                         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetFallingEdgeMCTLMainSTSControl(IN t_dsi_link dsi_id, IN t_dsi_link_status sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_ctl |
            ((t_uint32) sts_ctl << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetRisingEdgeMCTLMainSTSControl()                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the MCTL Main Status interrupts as    */
/*                  rising  edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_link_status : interrupt enum                         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetRisingEdgeMCTLMainSTSControl(IN t_dsi_link dsi_id, IN t_dsi_link_status sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_ctl |
            (~((t_uint32) sts_ctl) << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_CommandModeSTSControlEnable()                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the Command Mode Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_cmd_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_CommandModeSTSControlEnable(IN t_dsi_link dsi_id, IN t_dsi_cmd_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_ctl |
            (t_uint32) sts_ctl
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_CommandModeSTSControlDisable()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the Command Mode Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_cmd_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_CommandModeSTSControlDisable(IN t_dsi_link dsi_id, IN t_dsi_cmd_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_ctl |
            ~((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetFallingEdgeCmdModeControl()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the Command Mode Status interrupts as */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_cmd_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetFallingEdgeCmdModeControl(IN t_dsi_link dsi_id, IN t_dsi_cmd_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_ctl |
            ((t_uint32) sts_ctl << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetRisingEdgeCmdModeControl()                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the Command Mode Status interrupts as */
/*                  rising  edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_cmd_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetRisingEdgeCmdModeControl(IN t_dsi_link dsi_id, IN t_dsi_cmd_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_ctl |
            (~((t_uint32) sts_ctl) << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DirectCmdSTSControlEnable()                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the Direct Command Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_cmd_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DirectCmdSTSControlEnable(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_ctl |
            (t_uint32) sts_ctl
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DirectCmdSTSControlDisable()                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the Direct Command Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_cmd_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DirectCmdSTSControlDisable(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_ctl |
            ~((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetFallingEdgeDirectCmdControl()                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the Direct Command Status interrupts as */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_sts : interrupt enum                      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetFallingEdgeDirectCmdControl(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_ctl |
            ((t_uint32) sts_ctl << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetRisingEdgeDirectCmdControl()                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the Direct Command Status interrupts as */
/*                   rising edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_sts : interrupt enum                      */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetRisingEdgeDirectCmdControl(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_ctl |
            (~((t_uint32) sts_ctl) << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DirectCmdRdSTSEnable()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the Direct Command Read Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_rd_sts_ctl : interrupt enum               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DirectCmdRdSTSEnable(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_ctl |
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DirectCmdRdSTSDisable()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the Direct Command Read Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_rd_sts_ctl : interrupt enum               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DirectCmdRdSTSDisable(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_ctl |
            (~((t_uint32) sts_ctl))
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetFallingEdgeDirectCmdRdSTS()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the Direct Command Read Status interrupts as */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_rd_sts_ctl : interrupt enum               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetFallingEdgeDirectCmdRdSTS(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_ctl |
            ((t_uint32) sts_ctl << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetRisingEdgeDirectCmdRdSTS()                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the Direct Command Read Status interrupts as */
/*                   rising edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_rd_sts_ctl : interrupt enum               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetRisingEdgeDirectCmdRdSTS(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_ctl |
            (~((t_uint32) sts_ctl) << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_VID_MODESTSEnable()                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the Video Mode Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_vid_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_VID_MODESTSEnable(IN t_dsi_link dsi_id, IN t_dsi_vid_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_ctl |
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_VID_MODESTSDisable()                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the Video Mode Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_vid_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_VID_MODESTSDisable(IN t_dsi_link dsi_id, IN t_dsi_vid_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_ctl |
            (~((t_uint32) sts_ctl))
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetFallingEdgeVIDModeSTS()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the Video Mode Status interrupts as   */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_vid_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetFallingEdgeVIDModeSTS(IN t_dsi_link dsi_id, IN t_dsi_vid_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_ctl |
            ((t_uint32) sts_ctl << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetRisingEdgeVIDModeSTS()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the Video Mode Status interrupts as   */
/*                   rising edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_vid_mode_sts : interrupt enum                        */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetRisingEdgeVIDModeSTS(IN t_dsi_link dsi_id, IN t_dsi_vid_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_ctl |
            (~((t_uint32) sts_ctl) << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_TG_STS_Enable()                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the TVG and TBG Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_tg_sts_ctl : interrupt enum                          */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_TG_STS_Enable(IN t_dsi_link dsi_id, IN t_dsi_tg_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tg_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->tg_sts_ctl |
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_TG_STS_Disable()                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the TVG and TBG Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_tg_sts_ctl : interrupt enum                          */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_TG_STS_Disable(IN t_dsi_link dsi_id, IN t_dsi_tg_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tg_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->tg_sts_ctl |
            (~((t_uint32) sts_ctl))
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetFallingEdgeTGSTS()                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the TVG and TBG Status interrupts as  */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_tg_sts_ctl : interrupt enum                          */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetFallingEdgeTGSTS(IN t_dsi_link dsi_id, IN t_dsi_tg_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tg_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->tg_sts_ctl |
            ((t_uint32) sts_ctl << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetRisingEdgeTGSTS()                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the TVG and TBG Status interrupts as  */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_tg_sts_ctl : interrupt enum                          */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetRisingEdgeTGSTS(IN t_dsi_link dsi_id, IN t_dsi_tg_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tg_sts_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->tg_sts_ctl |
            (~((t_uint32) sts_ctl) << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_MCTL_DPHY_ERR_Enable()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the DPHY Error Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_dphy_err : interrupt enum                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_MCTL_DPHY_ERR_Enable(IN t_dsi_link dsi_id, IN t_dsi_dphy_err err_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_ctl |
            ((t_uint32) err_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_MCTL_DPHY_ERR_Disable()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine disables the DPHY Error Status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_dphy_err : interrupt enum                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_MCTL_DPHY_ERR_Disable(IN t_dsi_link dsi_id, IN t_dsi_dphy_err err_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_ctl |
            (~((t_uint32) err_ctl))
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetFallingEdgeDPHYERR()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the DPHY Error Status interrupts as   */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_dphy_err : interrupt enum                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetFallingEdgeDPHYERR(IN t_dsi_link dsi_id, IN t_dsi_dphy_err err_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_ctl |
            ((t_uint32) err_ctl << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetRisingEdgeDPHYERR()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the DPHY Error Status interrupts as   */
/*                  rising  edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_dphy_err : interrupt enum                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetRisingEdgeDPHYERR(IN t_dsi_link dsi_id, IN t_dsi_dphy_err err_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_ctl |
            (~((t_uint32) err_ctl << 16))
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DPHY_CLK_TRIM_RD_Enable()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the DPHY CLK TRIM RD status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DPHY_CLK_TRIM_RD_Enable(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_ctl |
            ((t_uint32) DSI_DPHY_Z_CALIB_OUT_VALID)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_DPHY_CLK_TRIM_RD_Disable()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine enables the DPHY CLK TRIM RD status interrupt generation*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_DPHY_CLK_TRIM_RD_Disable(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_ctl |
            (~((t_uint32) DSI_DPHY_Z_CALIB_OUT_VALID))
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetFallingEdgeDPHYCLKTRIM()                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the DPHY CLK RD Status interrupts as  */
/*                  falling edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetFallingEdgeDPHYCLKTRIM(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_ctl |
            (((t_uint32) DSI_DPHY_Z_CALIB_OUT_VALID) << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_SetRisingEdgeDPHYCLKTRIM()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine sets the DPHY CLK RD Status interrupts as  */
/*                  rising  edge                                            */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_SetRisingEdgeDPHYCLKTRIM(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_ctl =
        (
            g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_ctl |
            ~(((t_uint32) DSI_DPHY_Z_CALIB_OUT_VALID) << 16)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ClearMCTL_Main_Status()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine clears the MCTL Main status interrupts     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_link_status:intrrupt enum                            */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ClearMCTL_Main_Status(IN t_dsi_link dsi_id, IN t_dsi_link_status sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_clr =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_clr |
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ClearCmd_Mode_Status()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine clears the Command Mode status interrupts  */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_cmd_mode_sts:intrrupt enum                           */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ClearCmd_Mode_Status(IN t_dsi_link dsi_id, IN t_dsi_cmd_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_clr =
        (
            g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_clr |
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ClearDirectCmd_Status()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine clears the Direct Command Mode status interrupts*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_sts:intrrupt enum                         */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ClearDirectCmd_Status(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_clr =
        (
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ClearDirectCmdRD_Status()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine clears the Direct Command Read Mode status interrupts*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_rd_sts_ctl:intrrupt enum                  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ClearDirectCmdRD_Status(IN t_dsi_link dsi_id, IN t_dsi_direct_cmd_rd_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_clr =
        (
            g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_clr |
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ClearVIDMode_Status()                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine clears the Direct Command Read Mode status interrupts*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_direct_cmd_rd_sts_ctl:intrrupt enum                  */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ClearVIDMode_Status(IN t_dsi_link dsi_id, IN t_dsi_vid_mode_sts sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_clr =
        (
            g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_clr |
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ClearTG_Status()                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine clears the TVG and TBG status interrupts   */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_tg_sts_ctl:intrrupt enum                             */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ClearTG_Status(IN t_dsi_link dsi_id, IN t_dsi_tg_sts_ctl sts_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->tg_sts_clr =
        (
            g_dsi_system_context[dsi_id].dsi_register->tg_sts_clr |
            ((t_uint32) sts_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ClearDPHY_ERR()                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine clears the DPHY Error status interrupts    */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:t_dsi_dphy_err:intrrupt enum                               */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ClearDPHY_ERR(IN t_dsi_link dsi_id, IN t_dsi_dphy_err err_ctl)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_clr =
        (
            g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_clr |
            ((t_uint32) err_ctl)
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_ClearDPHY_CLK_TRIM_RD()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine clears the DPHY Clock Trim RD status interrupts*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:None                                                       */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC void DSI_ClearDPHY_CLK_TRIM_RD(IN t_dsi_link dsi_id)
{
    DBGENTER0();

    g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_clr =
        (
            g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_clr |
            (t_uint32) DSI_DPHY_Z_CALIB_OUT_VALID
        );

    DBGEXIT0(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_GetMCTL_Main_Status()                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the MCTL Main Interrupt Status        */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint8 *status: status pointer                            */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetMCTL_Main_Status(IN t_dsi_link dsi_id, OUT t_uint8 *status)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = (t_uint8) (g_dsi_system_context[dsi_id].dsi_register->mctl_main_sts_flag);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetCmd_Mode_Status()                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the Command Mode Interrupt Status     */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint8 *status: status pointer                            */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetCmd_Mode_Status(IN t_dsi_link dsi_id, OUT t_uint8 *status)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = (t_uint8) (g_dsi_system_context[dsi_id].dsi_register->cmd_mode_sts_flag | (t_uint32) DSI_CMD_MODE_STATUS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}




/****************************************************************************/
/* NAME			:	DSI_GetDirect_Cmd_ReadStatus()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the Direct Command 				 Status*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint32 *status: status pointer                           */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetDirect_Cmd_ReadStatus(IN t_dsi_link dsi_id, OUT t_uint32 *status)
{
    DBGENTER0();
    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = ( g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts );

    DBGEXIT0(DSI_OK);
    return(DSI_OK);
}

/****************************************************************************/
/* NAME			:	DSI_GetDirect_Cmd_Status()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the Direct Command Mode Interrupt Status*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint16 *status: status pointer                           */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetDirect_Cmd_Status(IN t_dsi_link dsi_id, OUT t_uint16 *status)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_sts_flag | (t_uint32) DSI_DIRECT_CMD_STS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetDirect_Cmd_Rd_Status()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the Direct Command Read Mode Interrupt Status*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint16 *status: status pointer                           */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetDirect_Cmd_Rd_Status(IN t_dsi_link dsi_id, OUT t_uint16 *status)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->direct_cmd_rd_sts_flag | (t_uint32) DSI_DIRECT_CMD_RD_STATUS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetVID_Mode_Status()                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the Video  Mode Interrupt Status      */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint16 *status: status pointer                           */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetVID_Mode_Status(IN t_dsi_link dsi_id, OUT t_uint16 *status)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->vid_mode_sts_flag | (t_uint32) DSI_VID_MODE_STATUS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetVID_Mode_Status()                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the TBG and TVG  Mode Interrupt Status*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint8 *status: status pointer                            */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetTG_Status(IN t_dsi_link dsi_id, OUT t_uint8 *status)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = (t_uint8) (g_dsi_system_context[dsi_id].dsi_register->tg_sts_flag | (t_uint32) DSI_TG_STS_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetDPHYErr_Status()                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the DPHY Error Interrupt Status       */
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint16 *status: status pointer                           */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetDPHYErr_Status(IN t_dsi_link dsi_id, OUT t_uint16 *status)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = (t_uint16) (g_dsi_system_context[dsi_id].dsi_register->mctl_dphy_err_flag);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

/****************************************************************************/
/* NAME			:	DSI_GetDPHY_CLK_TRIM_RDStatus()                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION	: 	This routine gets the DPHY Clock Trim Rd Interrupt Status*/
/*                                                                          */
/* PARAMETERS	:                                                           */
/* 		IN  	:None                                                       */
/*     InOut    :None                                                       */
/* 		OUT 	:t_uint8 *status: status pointer                            */
/*                                                                          */
/* RETURN		:t_dsi_error	: DSI error code						   	*/
/*               DSI_OK                                                     */
/*--------------------------------------------------------------------------*/
/* Type              :  PUBLIC                                              */
/* REENTRANCY 	     :	Non Re-entrant                                      */
/* REENTRANCY ISSUES :														*/

/****************************************************************************/
PUBLIC t_dsi_error DSI_GetDPHY_CLK_TRIM_RDStatus(IN t_dsi_link dsi_id, OUT t_uint8 *status)
{
    t_dsi_error dsi_error = DSI_OK;
    DBGENTER0();

    if ((status == NULL))
    {
        DBGEXIT0(DSI_INVALID_PARAMETER);
        return(DSI_INVALID_PARAMETER);
    }

    *status = (t_uint8) (g_dsi_system_context[dsi_id].dsi_register->dphy_clk_trim_rd_flag | DSI_CLK_TRIM_RD_MASK);

    DBGEXIT0(dsi_error);
    return(dsi_error);
}

PUBLIC t_dsi_error DSI_SetVID_dphy_time(IN t_dsi_link dsi_id, IN t_vid_dphy_time vid_dphy_time_reg)
{
    t_dsi_error dsi_error = DSI_OK;

    g_dsi_system_context[dsi_id].dsi_register->vid_dphy_time =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_dphy_time &~DSI_VID_REG_LINE_DURATION_MASK) |
            (((t_uint32) vid_dphy_time_reg.reg_line_duration << DSI_VID_REG_LINE_DURATION_SHIFT) & DSI_VID_REG_LINE_DURATION_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->vid_dphy_time =
        (
            (g_dsi_system_context[dsi_id].dsi_register->vid_dphy_time &~DSI_VID_REG_WAKEUP_TIME_MASK) |
            (((t_uint32) vid_dphy_time_reg.reg_wakeup_time << DSI_VID_REG_WAKEUP_TIME_SHIFT) & DSI_VID_REG_WAKEUP_TIME_MASK)
        );


    return(dsi_error);
}

PUBLIC t_dsi_error DSI_SetMCTL_main_en(IN t_dsi_link dsi_id, IN t_mctl_main_en mctl_main_en_reg)
{
    t_dsi_error dsi_error = DSI_OK;

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_IF2_EN_MASK) |
            (((t_uint32) mctl_main_en_reg.interface2 << DSI_IF2_EN_SHIFT) & DSI_IF2_EN_MASK)
        );

     g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_IF1_EN_MASK) |
            (((t_uint32) mctl_main_en_reg.interface1 << DSI_IF1_EN_SHIFT) & DSI_IF1_EN_MASK)
        );

     g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT2_ULPM_MASK) |
            (((t_uint32) mctl_main_en_reg.data2_ulpm_req << DSI_DAT2_ULPM_SHIFT) & DSI_DAT2_ULPM_MASK)
        );

     g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT1_ULPM_MASK) |
            (((t_uint32) mctl_main_en_reg.data1_ulpm_req << DSI_DAT1_ULPM_SHIFT) & DSI_DAT1_ULPM_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_CLK_ULPM_MASK) |
            (((t_uint32) mctl_main_en_reg.clk_ulpm_req << DSI_CLK_ULPM_SHIFT) & DSI_CLK_ULPM_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT2_EN_MASK) |
            (((t_uint32) mctl_main_en_reg.data2_lane << DSI_DAT2_EN_SHIFT) & DSI_DAT2_EN_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_DAT1_EN_MASK) |
            (((t_uint32) mctl_main_en_reg.data1_lane << DSI_DAT1_EN_SHIFT) & DSI_DAT1_EN_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_en =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_en &~DSI_CKLANE_EN_MASK) |
            (((t_uint32) mctl_main_en_reg.clk_lane << DSI_CKLANE_EN_SHIFT) & DSI_CKLANE_EN_MASK)
        );

    return(dsi_error);
}


PUBLIC t_dsi_error DSI_SetMCTL_main_data_ctl(IN t_dsi_link dsi_id, IN t_mctl_main_data_ctl mctl_main_data_ctl_reg)
{
    t_dsi_error dsi_error = DSI_OK;

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_BTAEN_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.bta << DSI_MCTL_BTAEN_SHIFT) & DSI_MCTL_BTAEN_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_READEN_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.read_operation << DSI_MCTL_READEN_SHIFT) & DSI_MCTL_READEN_MASK)
        );

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_REG_TE_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.te_from_reg << DSI_REG_TE_SHIFT) & DSI_REG_TE_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_IF2_TE_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.te_on_interface2 << DSI_IF2_TE_SHIFT) & DSI_IF2_TE_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_IF1_TE_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.te_on_interface1 << DSI_IF1_TE_SHIFT) & DSI_IF1_TE_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_TBG_SEL_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.test_byte_gen << DSI_MCTL_TBG_SEL_SHIFT) & DSI_MCTL_TBG_SEL_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_TVG_SEL_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.test_video_gen << DSI_MCTL_TVG_SEL_SHIFT) & DSI_MCTL_TVG_SEL_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_VID_EN_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.test_video_gen << DSI_MCTL_VID_EN_SHIFT) & DSI_MCTL_VID_EN_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_INTERFACE1_MODE_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.if1_mode << DSI_MCTL_INTERFACE1_MODE_SHIFT) & DSI_MCTL_INTERFACE1_MODE_MASK)
        );

      g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_data_ctl &~DSI_MCTL_LINKEN_MASK) |
            (((t_uint32) mctl_main_data_ctl_reg.test_video_gen) & DSI_MCTL_LINKEN_MASK)
        );

    return(dsi_error);
}

PUBLIC t_dsi_error DSI_SetMCTL_main_phy_ctl(IN t_dsi_link dsi_id, IN t_mctl_main_phy_ctl mctl_main_phy_ctl_reg)
{
    t_dsi_error dsi_error = DSI_OK;

    g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_WAIT_BURST_MASK) |
            (((t_uint32) mctl_main_phy_ctl_reg.wait_burst_time << DSI_WAIT_BURST_SHIFT) & DSI_WAIT_BURST_MASK)
        );

     g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_DAT2_ULPM_EN_MASK) |
            (((t_uint32) mctl_main_phy_ctl_reg.dat2_ulpm_en << DSI_DAT2_ULPM_EN_SHIFT) & DSI_DAT2_ULPM_EN_MASK)
        );

     g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_DAT1_ULPM_EN_MASK) |
            (((t_uint32) mctl_main_phy_ctl_reg.dat1_ulpm_en << DSI_DAT1_ULPM_EN_SHIFT) & DSI_DAT1_ULPM_EN_MASK)
        );

     g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_CLK_ULPM_EN_MASK) |
            (((t_uint32) mctl_main_phy_ctl_reg.clk_ulpm_en << DSI_CLK_ULPM_EN_SHIFT) & DSI_CLK_ULPM_EN_MASK)
        );

     g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_CLK_CONTINUOUS_MASK) |
            (((t_uint32) mctl_main_phy_ctl_reg.clk_continuous << DSI_CLK_CONTINUOUS_SHIFT) & DSI_CLK_CONTINUOUS_MASK)
        );

     g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl =
        (
            (g_dsi_system_context[dsi_id].dsi_register->mctl_main_phy_ctl &~DSI_LANE2_EN_MASK) |
            (((t_uint32) mctl_main_phy_ctl_reg.data2_lane_en) & DSI_LANE2_EN_MASK)
        );

    return(dsi_error);
}

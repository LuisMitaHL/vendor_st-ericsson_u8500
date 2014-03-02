/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Generic Interrupt Controller (GIC) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "gic.h"
#include "gic_p.h"

/*--------------------------------------------------------------------------*
 * Global Variables															*
 *--------------------------------------------------------------------------*/
/*System context*/
PRIVATE t_gic_system_context    g_gic_system_context;   /* GIC System variables */

PRIVATE t_gic_func_ptr          gic_int_handler[GIC_MAX_IT_LINES];

PRIVATE void                    gic_configurespi(t_gic_line, t_uint32, t_uint32, t_uint32, t_uint32);
PRIVATE void                    gic_configuresti(t_gic_line, t_uint32, t_uint32);

/* Debug stuffs */
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_GIC
#define MY_DEBUG_ID             myDebugID_GIC
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE const t_dbg_id          MY_DEBUG_ID = GIC_HCL_DBG_ID;
#endif

/*-------------------------------------------------------------------------*
 * Public functions														   *
 *-------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME: t_gic_error GIC_Init												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine Initialise the GIC							    */
/*																			*/
/* PARAMETERS:																*/
/* IN  :	gic_base_address: base address of gic 							*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:	Checks for supported hardware functionality		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */
/****************************************************************************/
PUBLIC t_gic_error GIC_Init(IN t_logical_address gic_base_address)
{
    t_gic_register      *gic_reg;
    t_gic_config_cntrl  gic_config;
    t_uint32            reg_offset;

    DBGENTER1(" (%lx)", gic_base_address);

    gic_reg = (t_gic_register *) gic_base_address;

    g_gic_system_context.p_gic_register = (t_gic_register *) gic_base_address;

    gic_config.it_en = GIC_IT_ENABLE;

    if
    (
        (GIC_P_ID0 == gic_reg->periph_id_0)
    &&  (GIC_P_ID1 == gic_reg->periph_id_1)
    &&  (GIC_P_ID2 == gic_reg->periph_id_2)
    &&  (GIC_P_ID3 == gic_reg->periph_id_3)
    &&  (GIC_CELL_ID0 == gic_reg->pcell_id_0)
    &&  (GIC_CELL_ID1 == gic_reg->pcell_id_1)
    &&  (GIC_CELL_ID2 == gic_reg->pcell_id_2)
    &&  (GIC_CELL_ID3 == gic_reg->pcell_id_3)
    )
    {
        /* disable all interrupts (32 private and 128 shared) */
        for (reg_offset = 0; reg_offset < MASK_REG_OFFSET; reg_offset++)
        {
            (*((g_gic_system_context.p_gic_register->it_en_clr) + reg_offset)) = MASK_ALL32;
        }

        /* clear all pending interrupts.*/
        for (reg_offset = 0; reg_offset < MASK_REG_OFFSET; reg_offset++)
        {
            (*((g_gic_system_context.p_gic_register->it_pen_clr) + reg_offset)) = MASK_ALL32;
        }

        /* enable_s, enable_ns (only secure ITs are enabled in DIST if ARM is running in secure mode) */
        g_gic_system_context.p_gic_register->it_dis_ctr_type |= (t_uint32) (gic_config.it_en);
    }
    else
    {
        DBGEXIT(GIC_UNSUPPORTED_HW);
        return(GIC_UNSUPPORTED_HW);
    }

    DBGEXIT(GIC_OK);
    return(GIC_OK);
}

/****************************************************************************/
/* NAME: t_gic_error GIC_ConfigureIrqLine									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the configuration parameters for IRQ      */
/*																			*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line it_line:	 existing line number(0-127 possible lines) */
/*			t_uint32 datum    :  ISR address								*/
/*			t_gic_config_cntrl *p_gic_config: Pointer to t_gic_config_cntrl */
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number 			*/
/*									is valid or not. 						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PUBLIC t_gic_error GIC_ConfigureIrqLine
(
    IN t_gic_line           it_line,
    IN t_gic_func_ptr       datum,
    IN t_gic_config_cntrl   *p_gic_config
)
{
    t_uint32    intid;
    t_uint32    reg_offset;
    t_uint32    bit_offset;

    intid = (t_uint32) it_line;

    DBGENTER3(" (%d %d %lx)", (p_gic_config->prio_level), it_line, datum);

    /* Checks the line number consistency */
    if (GIC_NO_LINE == it_line || GIC_ALL_LINES == it_line)
    {
        DBGEXIT(GIC_LINE_NOT_AUTHORIZED);
        return(GIC_LINE_NOT_AUTHORIZED);
    }
 
 	/* Check for priority */
    if (p_gic_config->prio_level > GIC_PR_LEVEL_31)
    {
        return(GIC_INVALID_PARAMETER);
    }

    reg_offset = (intid / MASK_BIT5);
    bit_offset = (intid % MASK_BIT5);

    /* Check for line enable status for SPI */
    if (it_line > GIC_STI_LINE_15)
    {
        if ((*((g_gic_system_context.p_gic_register->it_en_set) + reg_offset) & (MASK_BIT0 << bit_offset)))
        {
            DBGPRINT(DBGL_ERROR, "Can't Configure Line enabled");
            DBGEXIT(GIC_LINE_IS_ENABLED);
            return(GIC_LINE_IS_ENABLED);
        }
    }

    /* Set default isr */
    gic_int_handler[intid] = (t_gic_func_ptr) datum;

    if (it_line <= GIC_STI_LINE_15)
    {
        /* For Sti Config */
        gic_configuresti(it_line, (t_uint32) (p_gic_config->prio_level), (t_uint32) (p_gic_config->it_sec));
    }
    else
    {
        /* Use generic function for Spi config*/
        gic_configurespi
        (
            it_line,
            (t_uint32) (p_gic_config->prio_level),
            (t_uint32) (p_gic_config->cpu_num),
            (t_uint32) (p_gic_config->it_config),
            (t_uint32) (p_gic_config->it_sec)
        );
    }

    DBGEXIT(GIC_OK);
    return(GIC_OK);
}

/****************************************************************************/
/* NAME: t_gic_error GIC_ConfigureFiqLine									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the configuration parameters for FIQ      */
/*																			*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line it_line:	 existing line number(0-127 possible lines) */
/*			t_uint32 datum    :  ISR address								*/
/*			t_gic_config_cntrl *p_gic_config: Pointer to t_gic_config_cntrl */
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number 			*/
/*									is valid or not. 						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PUBLIC t_gic_error GIC_ConfigureFiqLine
(
    IN t_gic_line           it_line,
    IN t_gic_func_ptr       datum,
    IN t_gic_config_cntrl   *p_gic_config
)
{
    t_uint32    intid;
    t_uint32    reg_offset;
    t_uint32    bit_offset;
    t_gic_error error_val = GIC_OK;

    intid = (t_uint32) it_line;

    DBGENTER3(" (%d %d %lx)", (p_gic_config->prio_level), it_line, datum);

    /* Checks the line number consistency */
    if (GIC_NO_LINE == it_line || GIC_ALL_LINES == it_line)
    {
        DBGEXIT(GIC_LINE_NOT_AUTHORIZED);
        return(GIC_LINE_NOT_AUTHORIZED);
    }

    reg_offset = (intid / MASK_BIT5);
    bit_offset = (intid % MASK_BIT5);

    /* Check for line enable status for SPI */
    if (it_line > GIC_STI_LINE_15)
    {
        if ((*((g_gic_system_context.p_gic_register->it_en_set) + reg_offset) & (MASK_BIT0 << bit_offset)))
        {
            DBGPRINT(DBGL_ERROR, "Can't Configure Line enabled");
            DBGEXIT(GIC_LINE_IS_ENABLED);
            return(GIC_LINE_IS_ENABLED);
        }
    }

    gic_int_handler[intid] = (t_gic_func_ptr) datum;

    if (it_line <= GIC_STI_LINE_15)
    {
        /* For Sti Config */
        gic_configuresti(it_line, (t_uint32) (p_gic_config->prio_level), (t_uint32) (p_gic_config->it_sec));
    }
    else
    {
        /* Use generic function for Spi config*/
        gic_configurespi
        (
            it_line,
            (t_uint32) (p_gic_config->prio_level),
            (t_uint32) (p_gic_config->cpu_num),
            (t_uint32) (p_gic_config->it_config),
            (t_uint32) (p_gic_config->it_sec)
        );
    }

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME: void gic_configurespi												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:Enable an SPI intid and sets its parameters					*/
/*				 (security, target, priority, etc.) 						*/
/*																			*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line it_line: existing line number(0-127 possible lines)  */
/*			spi_num    domain: [0 .. 127].									*/
/* 			rising_or_level  : 1 = high-level , 3 = edge sensitive. 		*/
/* 			sec   	   domain: [0 .. 1] : 0 = secure, 1 = non-secure.       */
/* 			spi_target domain: [0x0 .. 0xFF]  							    */
/*	 ( possible values are 0x0 = no target, 0x1 = target Core0,			*/
/*                        0x2 = target Core1, 0x3 = target Core0 and Core1) */
/*			prio 	   domain: [0x0 .. 0x1F] five bits field. 				*/
/*								0x1F is the lowest priority					*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN: 	none															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PRIVATE void gic_configurespi
(
    IN t_gic_line   it_line,
    IN t_uint32     prio,
    IN t_uint32     spi_target,
    IN t_uint32     config,
    IN t_uint32     sec
)
{
    t_uint32    reg_offset;
    t_uint32    bit_offset;

	if(GIC_IT_SEC == sec)
	{
		
	reg_offset = ((t_uint32) it_line / MASK_BIT5);
    bit_offset = ((t_uint32) it_line % MASK_BIT5);

    /* Set the interrupt security register */
    (*((g_gic_system_context.p_gic_register->it_sec) + reg_offset)) |= ((MASK_BIT0 & sec) << bit_offset);
	
	}
    
    reg_offset = ((t_uint32) it_line / MASK_BIT4);
    bit_offset = ((t_uint32) it_line % MASK_BIT4);

    /* Set the config (level/edge) */
    (*((g_gic_system_context.p_gic_register->it_config) + reg_offset)) |= ((config) << (2 * bit_offset));

    reg_offset = ((t_uint32) it_line / MASK_BIT2);
    bit_offset = ((t_uint32) it_line % MASK_BIT2);

    /* Set the cpu_num in spi target register */
    (*((g_gic_system_context.p_gic_register->it_spi_target) + reg_offset)) |= (spi_target << (MASK_BIT3 * bit_offset));

    reg_offset = ((t_uint32) it_line / MASK_BIT2);
    bit_offset = ((t_uint32) it_line % MASK_BIT2);

    /* Set the priority level */
    (*((g_gic_system_context.p_gic_register->it_prio_level) + reg_offset)) |= ((prio << 3) << (MASK_BIT3 * bit_offset));
}

/****************************************************************************/
/* NAME: void gic_configuresti												*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:Enable an STI intid and sets its parameters					*/
/*				 (security, target, priority, etc.) 						*/
/*																			*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line it_line: existing line number(0-16 possible lines)   */
/* 			prio 	   domain: [0x0 .. 0x1F] five bits field. 				*/
/*								0x1F is the lowest priority					*/
/*			target_list 0-1: [0 is cpu interface0 , 1 is cpu interface1]    */
/*			list_filter 0-3: [ 0 is cpu target specified in list            */
/*							   1 is all cpu target except the requested one	*/
/*							   2 is cpu that requested the interrupt        */
/*							   3 is reserved. ]								*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN: 	none															*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PRIVATE void gic_configuresti(IN t_gic_line it_line, IN t_uint32 prio, IN t_uint32 sec)
{
    t_uint32    reg_offset;
    t_uint32    bit_offset;

	if(GIC_IT_SEC == sec)
	{
		
	*(g_gic_system_context.p_gic_register->it_sec) |= ((MASK_BIT0 & sec) << (t_uint32) it_line);    /* Security Register is RW */

	}
	
	reg_offset = ((t_uint32) it_line / MASK_BIT2);
    bit_offset = ((t_uint32) it_line % MASK_BIT2);

    /* Setting the priority level */
    (*((g_gic_system_context.p_gic_register->it_prio_level) + reg_offset)) |= ((prio << 3) << (MASK_BIT3 * bit_offset));
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_SetItPending									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the interrupt status as pending		*/
/*								 for GIC testing purpose                    */
/*																			*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line Line:		existing line number(128 possible lines)*/
/*																			*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number 			*/
/*									is valid or not. 						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PUBLIC t_gic_error GIC_SetItPending(IN t_gic_line it_line)
{
    t_gic_error error_val = GIC_OK;
    t_uint32    reg_offset;
    t_uint32    bit_offset;

    DBGENTER1(" (%d)", it_line);

    if ((GIC_NO_LINE == it_line) || (GIC_ALL_LINES == it_line))
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    reg_offset = ((t_uint32) it_line / MASK_BIT5);
    bit_offset = ((t_uint32) it_line % MASK_BIT5);

    /* Set the it_line as pending */
    (*((g_gic_system_context.p_gic_register->it_pen_set) + reg_offset)) |= (MASK_BIT0 << bit_offset);

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_ClrItPending									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine clears the pending status of a spi interrupt   */
/*															                */
/* PARAMETERS:																*/
/* IN  :	t_gic_line Line:		existing line number(128 possible lines)*/
/*																			*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number 			*/
/*									is valid or not. 						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PUBLIC t_gic_error GIC_ClrItPending(IN t_gic_line it_line)
{
    t_gic_error error_val = GIC_OK;
    t_uint32    reg_offset;
    t_uint32    bit_offset;

    DBGENTER1(" (%d)", it_line);

    if ((GIC_NO_LINE == it_line) || (GIC_ALL_LINES == it_line))
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    reg_offset = ((t_uint32) it_line / MASK_BIT5);
    bit_offset = ((t_uint32) it_line % MASK_BIT5);

    /* Clear the pending it_line */
    (*((g_gic_system_context.p_gic_register->it_pen_clr) + reg_offset)) = (MASK_BIT0 << bit_offset);

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_IsActiveStatus									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the interrupt INTID is Active 			*/
/*													or Inactive/Pending     */
/*																			*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line Line:		existing line number(128 possible lines)*/
/*																			*/
/* OUT : 	t_bool *p_active_line	Status of the interrupt 0: Inactive     */
/*															1: Active		*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number 			*/
/*									is valid or not 						*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PUBLIC t_gic_error GIC_IsActiveStatus(IN t_gic_line it_line, OUT t_bool *p_active_line)
{
    /* for spi's 32-224 */
    t_gic_error error_val = GIC_OK;
    t_uint32    reg_offset;
    t_uint32    bit_offset;

    DBGENTER1(" (%d)", it_line);

    if ((GIC_NO_LINE == it_line) || (GIC_ALL_LINES == it_line))
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    reg_offset = ((t_uint32) it_line / MASK_BIT5);
    bit_offset = ((t_uint32) it_line % MASK_BIT5);

    /* Check for the status */
    if ((*((g_gic_system_context.p_gic_register->it_active_status) + reg_offset) & (MASK_BIT0 << bit_offset)))
    {
        *p_active_line = TRUE;
    }
    else
    {
        *p_active_line = FALSE;
    }

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_GetSpiStatus								    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns returns 1 if the input corresponding   */
/*											to the SPI spi_num is HIGH      */
/*															                */
/*																			*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line Line:		existing line number(128 possible lines)*/
/*																			*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number is already 	*/
/*									configured.								*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PUBLIC t_gic_error GIC_GetSpiStatus(IN t_gic_line it_line, OUT t_bool *p_active_spi_line)
{
    t_uint32    reg_offset;
    t_uint32    bit_offset;
    t_gic_error error_val = GIC_OK;
    t_uint32    intid;

    intid = (t_uint32) it_line;

    DBGENTER1(" (%d)", it_line);

    if (it_line < GIC_RESERVED_0_LINE)
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    if ((GIC_NO_LINE == it_line) || (GIC_ALL_LINES == it_line))
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    reg_offset = (intid / MASK_BIT5);
    bit_offset = (intid % MASK_BIT5);

    /* Check for the status*/
    if ((*((g_gic_system_context.p_gic_register->it_spi_status) + reg_offset) & (MASK_BIT0 << bit_offset)))
    {
        *p_active_spi_line = TRUE;
    }
    else
    {
        *p_active_spi_line = FALSE;
    }

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_ChangeDatum										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine changes the configuration of one interrupt		*/
/*				line by modifying the datum linked to it.If it_line is    	*/
/*              VIC_NO_LINE,the Datum is default ISR                        */
/*																			*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line Line:		existing line number(128 possible lines)*/
/*			t_uint32 new_datum:		Datum to be linked to the IT line.		*/
/*									Can be the ISR address.					*/
/*			t_uint32 *p_old_datum	old datum linked						*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number is already 	*/
/*									configured.								*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PUBLIC t_gic_error GIC_ChangeDatum(IN t_gic_line it_line, IN t_gic_func_ptr new_datum, OUT t_gic_func_ptr *p_old_datum)
{
    t_uint32    intid;
    t_gic_error error_val;
    t_uint32    reg_offset;
    t_uint32    bit_offset;

    intid = (t_uint32) it_line;

    DBGENTER2(" (%d %lx)", it_line, new_datum);

    if (GIC_ALL_LINES == it_line)
    {
        DBGEXIT(GIC_INVALID_PARAMETER); /* Invalid Line */
        return(GIC_INVALID_PARAMETER);
    }

    if (NULL == p_old_datum)
    {
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* Invalid handler */
    }

    reg_offset = (intid / MASK_BIT5);
    bit_offset = (intid % MASK_BIT5);

    /* Check if the it_line is already enabled */
    if (it_line > GIC_STI_LINE_15)
    {
        if ((*((g_gic_system_context.p_gic_register->it_en_set) + reg_offset) & (MASK_BIT0 << bit_offset)))
        {
            DBGPRINT(DBGL_ERROR, "Can't Configure Line enabled");
            DBGEXIT(GIC_LINE_IS_ENABLED);
            return(GIC_LINE_IS_ENABLED);
        }
    }

    *p_old_datum = gic_int_handler[intid];

    gic_int_handler[intid] = new_datum;

    error_val = GIC_OK;

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:    t_gic_error GIC_SetDbgLevel										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the debug level for the GIC HCL.			*/
/*																			*/
/* PARAMETERS:																*/
/* IN:		t_dbg_level		DebugLevel from DEBUG_LEVEL0 to DEBUG_LEVEL4.	*/
/* OUT:		none															*/
/*																			*/
/* RETURN:																	*/
/*  t_gic_error:		Checks that the line number is already configured	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA															*/

/****************************************************************************/
PUBLIC t_gic_error GIC_SetDbgLevel(IN t_dbg_level dbg_level)
{
    DBGENTER0();
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = dbg_level;
#endif
    /*coverity[self_assign]*/
    dbg_level = dbg_level;
    DBGEXIT0(GIC_OK);
    return(GIC_OK);
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_GetVersion(t_version)							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Returns the version information of the HCL					*/
/*																			*/
/* PARAMETERS:																*/
/* IN  :																	*/
/*																			*/
/* OUT : 	p_version: Variable to return the HCL version					*/
/* 																			*/
/* RETURN: t_gic_error:	Checks that the line number is already configured	*/
/*																			*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA															*/

/****************************************************************************/
PUBLIC t_gic_error GIC_GetVersion(IN t_version *p_version)
{
    t_gic_error error_status = GIC_OK;
    DBGENTER0();
    p_version->version = GIC_HCL_VERSION;
    p_version->major = GIC_HCL_MAJOR;
    p_version->minor = GIC_HCL_MINOR;

    DBGEXIT0(error_status);
    return(error_status);
}

/****************************************************************************/
/* NAME:     void GIC_SaveDeviceContext(void ) 								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Saves the GIC register and global variables in memory		*/
/* PARAMETERS:                                                              */
/* IN : 	none                                                            */
/* OUT : 	none                                                            */
/*                                                                          */
/* RETURN: none															    */
/*	  																		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant                                                   */

/****************************************************************************/
PUBLIC void GIC_SaveDeviceContext(void)
{
    g_gic_system_context.gic_device_context.it_cpu_ens = (g_gic_system_context.p_gic_register->cpu_icr) & MASK_BIT0;
    g_gic_system_context.gic_device_context.it_cpu_enns = (g_gic_system_context.p_gic_register->cpu_icr) & MASK_BIT1;
    g_gic_system_context.gic_device_context.acktl = (g_gic_system_context.p_gic_register->cpu_icr) & MASK_BIT2;
    g_gic_system_context.gic_device_context.fiqen = (g_gic_system_context.p_gic_register->cpu_icr) & MASK_BIT3;
    g_gic_system_context.gic_device_context.sbpr = (g_gic_system_context.p_gic_register->cpu_icr) & MASK_BIT4;
}

/****************************************************************************/
/* NAME:     void GIC_RestoreDeviceContext(void )							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Restore the gic register and global variables from memory	*/
/* PARAMETERS:                                                              */
/* IN : 	none                                                            */
/* OUT : 	none                                                            */
/*                                                                          */
/* RETURN: t_gic_error: none 												*/
/*	  																		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant                                                   */

/****************************************************************************/
PUBLIC void GIC_RestoreDeviceContext(void)
{
    t_uint32    cpu_enns;
    t_uint32    cpu_ens;
    t_uint32    cpu_acktl;
    t_uint32    cpu_fiqen;
    t_uint32    cpu_sbpr;

    cpu_enns = (g_gic_system_context.gic_device_context.it_cpu_ens);
    cpu_ens = ((g_gic_system_context.gic_device_context.it_cpu_enns) << GIC_ICR_SHIFT_ENNS);
    cpu_acktl = ((g_gic_system_context.gic_device_context.acktl) << GIC_ICR_SHIFT_ACKTL);
    cpu_fiqen = ((g_gic_system_context.gic_device_context.fiqen) << GIC_ICR_SHIFT_FIQEN);
    cpu_sbpr = ((g_gic_system_context.gic_device_context.sbpr) << GIC_ICR_SHIFT_SBPR);

    g_gic_system_context.p_gic_register->cpu_icr = cpu_enns | cpu_ens | cpu_acktl | cpu_fiqen | cpu_sbpr;
}

/****************************************************************************/
/* NAME:    t_gic_error GIC_GetAckId								    	*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:  Returns the ack id of the interrupt ready to be served		*/
/* PARAMETERS:                                                              */
/* IN : 	none                                                            */
/* OUT : 	t_uint32 *ack_id : Interrupt ID                                 */
/*                                                                          */
/* RETURN: t_gic_error:	Checks that the interrupt id is valid or not. 		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant                                                   */

/****************************************************************************/
PUBLIC t_gic_error GIC_GetItAckId(OUT t_uint32 *ack_id, OUT t_uint32 *src_cpu_id)
{
    t_uint32    interrupt_ack;
    t_sint32    timout_cnt;
    t_gic_error error_val;

    /* Read Interrupt ACK register from processor interface */
    interrupt_ack = (g_gic_system_context.p_gic_register->it_ack) & GIC_IT_ACK_EOI_MASK;

    /*  Interrupt Handler Called */
    timout_cnt = ACK_TIMECOUNT;
    while (((interrupt_ack & GIC_IT_ACK_ID) == GIC_IT_ACK_ID) && (timout_cnt > 0))
    {
        /* Waiting for Interrupt Ack  */
        timout_cnt--;
        interrupt_ack = (g_gic_system_context.p_gic_register->it_ack) & GIC_IT_ACK_EOI_MASK;
    } 

    if (timout_cnt > 0)
    {
        *ack_id = (interrupt_ack & GIC_IT_ACK_ID);
        *src_cpu_id = (interrupt_ack >> GIC_IT_ACK_SHIFT_SRC_CPU);
    }
    else
    {
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);
    }

    error_val = GIC_OK;

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:    t_gic_error GIC_ItHandler										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:  Set the end of interrupt.									*/
/* PARAMETERS:                                                              */
/* IN : 	ack_id  : Interrupt Id of the interrupt got handled				*/
/* OUT : 																	*/
/*                                                                          */
/* RETURN:t_gic_error:	Checks that the interrupt id is valid or not		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant                                                   */

/****************************************************************************/
PUBLIC t_gic_error GIC_ItHandler(IN t_uint32 ack_id, IN t_uint32 src_cpu_id)
{
    t_gic_error error_val = GIC_INVALID_PARAMETER;
    t_uint32    src_cpu;

    if (ack_id < (t_uint32) GIC_MAX_IT_LINES)
    {
        /* Calling Corresponding Interrupt Handler */
        (*gic_int_handler[ack_id]) (ack_id);

        src_cpu = (src_cpu_id << GIC_IT_ACK_SHIFT_SRC_CPU);

        /* Set end of Interrupt */
        g_gic_system_context.p_gic_register->eoi = ack_id | src_cpu;

        error_val = GIC_OK;
    }

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_EnableItLine()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables the interrupt given in parameter.		*/
/*				First it checks that the line is configured.				*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line it_line:		interrupt line to be enabled.			*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error error_val:	checks that IT line has already been	*/
/*									configured.								*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant													*/

/****************************************************************************/
PUBLIC t_gic_error GIC_EnableItLine(IN t_gic_line it_line)
{
    t_gic_error error_val = GIC_OK;
    t_uint32    reg_offset;
    t_uint32    bit_offset;

    DBGENTER1(" (%d)", it_line);

    if (GIC_NO_LINE == it_line)
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    reg_offset = ((t_uint32) it_line / MASK_BIT5);
    bit_offset = ((t_uint32) it_line % MASK_BIT5);

    /* For enabling all lines */
    if (GIC_ALL_LINES == it_line)
        {
    	    
    		    (*((g_gic_system_context.p_gic_register->it_en_set) + 1)) = GIC_EN_TEST_MASK1;
		    
		}
        
    else
    {
        (*((g_gic_system_context.p_gic_register->it_en_set) + reg_offset)) |= MASK_BIT0 << (bit_offset);
    }

    error_val = GIC_OK;
    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:	t_gic__error GIC_DisableItLine()								*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine disables the interrupt given in parameter.		*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line it_line:		interrupt line to be disabled.			*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error error_val:	checks that IT line has already been	*/
/*									configured.								*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_gic_error GIC_DisableItLine(IN t_gic_line it_line)
{
    t_gic_error error_val = GIC_OK;
    t_uint32    reg_offset;
    t_uint32    bit_offset;

    DBGENTER1(" (%d)", it_line);

    if (GIC_NO_LINE == it_line)
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    /* For disabling all the lines */
    if (GIC_ALL_LINES == it_line)
    {
        for (reg_offset = 0; reg_offset < MASK_REG_OFFSET; reg_offset++)
        {
            (*((g_gic_system_context.p_gic_register->it_en_clr) + reg_offset)) = GIC_EN_ALL_LINES_MASK_1;
        }
    }
    else
    {
        reg_offset = ((t_uint32) it_line / MASK_BIT5);
        bit_offset = ((t_uint32) it_line % MASK_BIT5);
        (*((g_gic_system_context.p_gic_register->it_en_clr) + reg_offset)) = (MASK_BIT0 << (bit_offset));
    }

    DBGEXIT(error_val);
    return(error_val);
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_IsLineEnabled()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine computes the enable status of the interrupt	*/
/*				line given as an argument.									*/
/* PARAMETERS:																*/
/* IN  :	t_gic_line it_line:		interrupt line to be checked			*/
/* OUT : 	bool *p_enabled_line:  	True if IT line is enabled, else FALSE.	*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number given as	*/
/*									 argument is a valid line 				*/
/*																			*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_gic_error GIC_IsLineEnabled(IN t_gic_line it_line, OUT t_bool *p_enabled_line)
{
    t_uint32    reg_offset;
    t_uint32    bit_offset;

    DBGENTER1(" (%d )", it_line);

    if (GIC_NO_LINE == it_line)
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    if ((GIC_NO_LINE == it_line) || (GIC_ALL_LINES == it_line))
    {
        DBGPRINT(DBGL_ERROR, " IT line not valid");
        DBGEXIT(GIC_INVALID_PARAMETER);
        return(GIC_INVALID_PARAMETER);  /* line not valid */
    }

    reg_offset = ((t_uint32) it_line / MASK_BIT5);
    bit_offset = ((t_uint32) it_line % MASK_BIT5);

    /* Check for the line enabled */
    if ((*((g_gic_system_context.p_gic_register->it_en_set) + reg_offset) & (MASK_BIT0 << bit_offset)))
    {
        *p_enabled_line = TRUE;
    }
    else
    {
        *p_enabled_line = FALSE;
    }

    DBGEXIT(GIC_OK);
    return(GIC_OK);
}

/****************************************************************************/
/* NAME:	t_gic_error GIC_SetSoftSti()									*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine triggers the sti line given as parameter.		*/
/*			                                                                */
/* PARAMETERS:																*/
/* IN  :	t_gic_line it_line:		interrupt line to be checked			*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:			Checks that the line number given as	*/
/*									 argument is a valid line 				*/
/*																			*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/

/****************************************************************************/
PUBLIC t_gic_error GIC_SetSoftSti
(
    t_gic_line                  it_line,
    t_gic_target_list           target_list,
    t_gic_it_cpu_list_filter    cpu_list_filter
)
{
    t_gic_error error_value = GIC_INVALID_PARAMETER;

    t_uint32    reg_val = 0;

    reg_val = (((t_uint32) (target_list) << TARG_LIST_BIT) | ((t_uint32) (cpu_list_filter) << TARG_LIST_FILTER_BIT));

    if (it_line <= GIC_STI_LINE_15)
    {
        /* Banked per CPU , 
	   PPIs, STIs are always enabled:  RO register */
        g_gic_system_context.p_gic_register->it_sti_trig = (((t_uint32) it_line) | reg_val);    /* bit 16 is set to set CPU0 as STI intid target.	*/

        error_value = GIC_OK;
    }

    return(error_value);
}

/****************************************************************************/
/* NAME: t_gic_error GIC_SetCpuInterfaceParameters							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the cpu interface parameters.				*/
/*																			*/
/* PARAMETERS:																*/
/* IN  :  t_gic_interface_cntrl *p_gic_cntrl: pointer to the cpu interface  */
/*												control structure.			*/
/* OUT : 	none															*/
/* 																			*/
/* RETURN:																	*/
/*			t_gic_error:	Checks for supported hardware functionality		*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */

/****************************************************************************/
PUBLIC t_gic_error GIC_SetCpuInterfaceParameters(t_gic_interface_cntrl *p_gic_cntrl)
{
    t_gic_error error_val = GIC_OK;

    /* Set the Priority mask register, allow all priority levels to go */
       (g_gic_system_context.p_gic_register->prio_mask) = ((t_uint32) (p_gic_cntrl->prio_mask) << 3); 

    /* Set the cpu interface control register */
    (g_gic_system_context.p_gic_register->cpu_icr) |=
        (
            (t_uint32) (p_gic_cntrl->fiqen) |
            (t_uint32) p_gic_cntrl->ackctl |
            (t_uint32) p_gic_cntrl->en_set_ns |
            (t_uint32) p_gic_cntrl->en_set_s
        );

    return(error_val);
}


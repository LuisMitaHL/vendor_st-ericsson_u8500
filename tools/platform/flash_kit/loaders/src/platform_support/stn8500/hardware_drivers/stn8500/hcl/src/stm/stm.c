/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the System Trace Module
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "stm_p.h"

/*--------------------------------------------------------------------------*
 * Global Variables															*
 *--------------------------------------------------------------------------*/
/*System context*/


PRIVATE t_stm_system_context    g_stm_system_context;   /* STM System variables */

/* Debug stuffs */
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_STM
#define MY_DEBUG_ID             myDebugID_STM
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE const t_dbg_id          MY_DEBUG_ID = STM_HCL_DBG_ID;
#endif


/*-------------------------------------------------------------------------*
 * Public functions														   *
 *-------------------------------------------------------------------------*/

/****************************************************************************/
/* NAME : STM_SetBaseAddress                                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine initializes STM register base address         */
/* PARAMETERS :                                                             */
/* IN : t_logical_address stm_base_address : STM registers base address     */
/* OUT :    None                                                            */
/* RETURN : None                                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY : Non Re-entrant                                              */
/****************************************************************************/

PUBLIC void STM_SetBaseAddress(IN t_logical_address stm_base_address)
{
    /* Initializes the stm Base Address */
    g_stm_system_context.p_stm_register = (t_stm_register *) stm_base_address;   
    
}

/****************************************************************************/
/* NAME : STM_SetInitiatorBaseAddress                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine initializes STM OSMO register base address    */
/* PARAMETERS :                                                             */
/* IN : t_stm_osmo stm_osmo_type : STM registers base address               */
/* OUT :    None                                                            */
/* RETURN : None                                                            */
/*--------------------------------------------------------------------------*/
/* REENTRANCY : Non Re-entrant                                              */
/****************************************************************************/

PUBLIC void STM_SetInitiatorBaseAddress(IN t_stm_initiator *p_stm_initiator)
{
    /* Initializes the stm Base Address */
    g_stm_system_context.p_stm_initiator =  p_stm_initiator;   
    
}


/****************************************************************************/
/* NAME:	t_stm_error STM_Init()			     							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the STM registers.					*/
/*																			*/
/* PARAMETERS:																*/
/* IN :			stm_base_address:STM registers base address					*/
/* OUT : 		none														*/
/* RETURN:																	*/
/*				void														*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non - Reentrant												*/
/****************************************************************************/
PUBLIC t_stm_error STM_Init(t_logical_address stm_base_address)
{
    t_stm_error     error_stm = STM_UNSUPPORTED_HW;
    DBGENTER1(" (%lx)", stm_base_address);
	g_stm_system_context.p_stm_register = (t_stm_register *)stm_base_address;

    if
    (
        (STM_P_ID0 == g_stm_system_context.p_stm_register->periph_id_0[0])
    &&  (STM_P_ID1 == g_stm_system_context.p_stm_register->periph_id_1[0])
    &&  (STM_P_ID2 == g_stm_system_context.p_stm_register->periph_id_2[0])
    &&  (STM_P_ID3 == g_stm_system_context.p_stm_register->periph_id_3[0])
    &&  (STM_CELL_ID0 == g_stm_system_context.p_stm_register->pcell_id_0[0])
    &&  (STM_CELL_ID1 == g_stm_system_context.p_stm_register->pcell_id_1[0])
    &&  (STM_CELL_ID2 == g_stm_system_context.p_stm_register->pcell_id_2[0])
    &&  (STM_CELL_ID3 == g_stm_system_context.p_stm_register->pcell_id_3[0])
    )
        /* Initializing register pointer */
    {
            error_stm = STM_OK;
    }

    DBGEXIT(error_stm);
    return(error_stm);
}

/****************************************************************************/
/* NAME:    STM_SetDbgLevel													*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the debug level for the STM HCL.			*/
/*																			*/
/* PARAMETERS:																*/
/* IN:		t_dbg_level		DebugLevel from DEBUG_LEVEL0 to DEBUG_LEVEL4.	*/
/* OUT:		none															*/
/*																			*/
/* RETURN:																	*/
/*  t_dbg_error																*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA															*/

/****************************************************************************/
PUBLIC t_stm_error STM_SetDbgLevel(t_dbg_level dbg_level)
{
    DBGENTER0();
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = dbg_level;
#endif
    
    DBGEXIT0(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_GetVersion(t_version)							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Returns the version information of the HCL					*/
/*																			*/
/* PARAMETERS:																*/
/* IN  :																	*/
/* OUT : 	p_version: Variable to return the HCL version					*/
/* RETURN:																	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA															*/
/****************************************************************************/
PUBLIC t_stm_error STM_GetVersion(t_version *p_version)
{
    t_stm_error error_status = STM_OK;
    DBGENTER0();
    p_version->version = STM_HCL_VERSION;
    p_version->major = STM_HCL_MAJOR;
    p_version->minor = STM_HCL_MINOR;

    DBGEXIT0(error_status);
    return(error_status);
}


/**********************************************************************************************/
/* NAME:	t_stm_error STM_ConfigureFrequency()							     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures STM time stamp and clock frequency.		              */
/* PARAMETERS:																                  */
/* IN : 		t_stm_xckdiv:Configures the STM signal clock frequency          	          */
/* OUT:         none																		  */
/* RETURN:		t_stm_error :						STM error code				          	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_ConfigureFrequency(IN t_stm_xckdiv stm_xckdiv)
{
    /* STM clock divider */
    DBGENTER1(" STM Time Stamp frequency (%x)", stm_xckdiv);
    
    STM_SET_XCKDIV(g_stm_system_context.p_stm_register->stm_cr[0],(t_uint32) stm_xckdiv);
    
    DBGEXIT0(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_Configuration()      	    					*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to configure the priority and frequecy */
/*              of operation for STM                                        */
/* PARAMETERS:																*/
/* IN  :	t_stm_configuration stm_configure:COnfigure the STM             */
/* OUT : 	none															*/
/* RETURN:																	*/
/*			t_stm_error: Returns the correct error code.                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant												*/
/****************************************************************************/

PUBLIC t_stm_error STM_Configuration(IN t_stm_configuration stm_configuration)
{
    
    /* Set the priority for Port 0 */
    STM_SET_PLP0(g_stm_system_context.p_stm_register->stm_cr[0],(t_uint32)stm_configuration.stm_plp0);

    /* Set the priority for Port 1 */
    STM_SET_PLP1(g_stm_system_context.p_stm_register->stm_cr[0],(t_uint32)stm_configuration.stm_plp1);

    /* Set the downgradable STM Data lines */

    /* Ensure that the correct value for Data output is loaded. */
    if( STM_UNUSED == stm_configuration.stm_dwng )
    {
        stm_configuration.stm_dwng = STM_4_BIT_DATA_OUTPUT;
     }
    STM_SET_DWNG(g_stm_system_context.p_stm_register->stm_cr[0],(t_uint32)stm_configuration.stm_dwng);

	STM_SET_PRCMU(g_stm_system_context.p_stm_register->stm_cr[0],(t_uint32)stm_configuration.stm_swap_prcmu);
    g_stm_system_context.stm_configuration = stm_configuration;
    
    DBGEXIT(STM_OK);
    return(STM_OK);

}

/****************************************************************************/
/* NAME:	t_stm_error STM_MIPIModeControl()   							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to configure the MAsters as HW or SW   */
/* PARAMETERS:																*/
/* IN  :	t_stm_mcc: The number of masters that can be configured         */
/* OUT : 	none															*/
/* RETURN:																	*/
/*			t_stm_error:  Return the correct error code                 	*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Non-Rentrant												    */
/****************************************************************************/
PUBLIC t_stm_error STM_MIPIModeControl(IN t_stm_mmc stm_mmc)
{
    /* Configure the Masters as HW or SW */

    /* Configure Master 0 */
    STM_SET_HWNSW0(g_stm_system_context.p_stm_register->stm_mmcr[0],(t_uint32)stm_mmc.stm_hwnsw0);

    /* Configure Master 1 */
    STM_SET_HWNSW1(g_stm_system_context.p_stm_register->stm_mmcr[0],(t_uint32)stm_mmc.stm_hwnsw1);

    /* Configure Master 2 */
    STM_SET_HWNSW2(g_stm_system_context.p_stm_register->stm_mmcr[0],(t_uint32)stm_mmc.stm_hwnsw2);

    /* Configure Master 3 */
    STM_SET_HWNSW3(g_stm_system_context.p_stm_register->stm_mmcr[0],(t_uint32)stm_mmc.stm_hwnsw3);

    /* Configure Master 4 */
    STM_SET_HWNSW4(g_stm_system_context.p_stm_register->stm_mmcr[0],(t_uint32)stm_mmc.stm_hwnsw4);

    /* Configure Master 5 */
    STM_SET_HWNSW5(g_stm_system_context.p_stm_register->stm_mmcr[0],(t_uint32)stm_mmc.stm_hwnsw5);


    DBGEXIT(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_TraceEnable()       							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to enable the trace for various masters */
/* PARAMETERS:																*/
/* IN  :	t_stm_ter:The structure of all the masters                      */
/*                    that are enabled\disabled								*/
/* OUT :    None                                                            */
/* RETURN:																	*/
/*			t_stm_error: Return the correct error code                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC t_stm_error STM_TraceEnable(IN t_stm_ter stm_ter)
{
    /* Configure Master 0 Trace */
    STM_SET_TE0(g_stm_system_context.p_stm_register->stm_ter[0],(t_uint32)stm_ter.stm_te0);

    /* Configure Master 2 Trace */
    STM_SET_TE2(g_stm_system_context.p_stm_register->stm_ter[0],(t_uint32)stm_ter.stm_te2);

    /* Configure Master 3 Trace */
    STM_SET_TE3(g_stm_system_context.p_stm_register->stm_ter[0],(t_uint32)stm_ter.stm_te3);

    /* Configure Master 4 Trace */
    STM_SET_TE4(g_stm_system_context.p_stm_register->stm_ter[0],(t_uint32)stm_ter.stm_te4);


	/* Configure Master 5 Trace */
	STM_SET_TE5(g_stm_system_context.p_stm_register->stm_ter[0],(t_uint32)stm_ter.stm_te5);


	/* Configure Master 9 Trace */
	STM_SET_TE9(g_stm_system_context.p_stm_register->stm_ter[0],(t_uint32)stm_ter.stm_te9);

    DBGEXIT(STM_OK);
    return(STM_OK);
}


/****************************************************************************/
/* NAME:	t_stm_error STM_GetTraceStatus()       							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to read the status of various masters   */
/*             weather the trace is enabled\disabled for them.              */
/* PARAMETERS:																*/
/* IN   :   None                                                            */
/* OUT  :	t_stm_tdsr:The structure of all the masters                     */
/*                    that are enabled\disabled								*/
/* RETURN:																	*/
/*			t_stm_error: Return the correct error code                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC t_stm_error STM_GetTraceStatus(OUT t_stm_tdsr *stm_tdsr)
{
	/* Get the current status i.e. enable\disable of the trace master 0 */
    stm_tdsr->stm_td0 = (t_bool)STM_GET_TD0((g_stm_system_context.p_stm_register->stm_tdsr[0]));

	/* Get the current status i.e. enable\disable of the trace master 1 */
	stm_tdsr->stm_td1 = (t_bool)STM_GET_TD1((g_stm_system_context.p_stm_register->stm_tdsr[0]));

	/* Get the current status i.e. enable\disable of the trace master 2 */
	stm_tdsr->stm_td2 = (t_bool)STM_GET_TD2((g_stm_system_context.p_stm_register->stm_tdsr[0]));

	/* Get the current status i.e. enable\disable of the trace master 3 */
	stm_tdsr->stm_td3 = (t_bool)STM_GET_TD3((g_stm_system_context.p_stm_register->stm_tdsr[0]));

	/* Get the current status i.e. enable\disable of the trace master 4 */
	stm_tdsr->stm_td4 = (t_bool)STM_GET_TD4((g_stm_system_context.p_stm_register->stm_tdsr[0]));

	/* Get the current status i.e. enable\disable of the trace master 5 */
	stm_tdsr->stm_td5 = (t_bool)STM_GET_TD5((g_stm_system_context.p_stm_register->stm_tdsr[0]));

	/* Get the current status i.e. enable\disable of the trace master 9 */
	stm_tdsr->stm_td9 = (t_bool)STM_GET_TD9((g_stm_system_context.p_stm_register->stm_tdsr[0]));

    DBGEXIT(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_GetOverflowStatus()    							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to read the status of various masters   */
/*             weather the overflow is set for them.                        */
/* PARAMETERS:																*/
/* IN   :   None                                                            */
/* OUT  :	t_stm_ofsr:The structure of all the masters                     */
/*                    that are overflown or not								*/
/* RETURN:																	*/
/*			t_stm_error: Return the correct error code                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC t_stm_error STM_GetOverflowStatus(OUT t_stm_ofsr *stm_ofsr)
{

	/* Get the current status weather there is atleast 1 overflow for master 0 */
        stm_ofsr->stm_ovf0 = (t_bool)STM_GET_OV0(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status weather there is atleast 1 overflow for master 1 */
        stm_ofsr->stm_ovf1 = (t_bool)STM_GET_OV1(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status weather there is atleast 1 overflow for master 2 */
        stm_ofsr->stm_ovf2 = (t_bool)STM_GET_OV2(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status weather there is atleast 1 overflow for master 3 */
        stm_ofsr->stm_ovf3 = (t_bool)STM_GET_OV3(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status weather there is atleast 1 overflow for master 4 */
        stm_ofsr->stm_ovf4 = (t_bool)STM_GET_OV4(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status weather there is atleast 1 overflow for master 5 */
        stm_ofsr->stm_ovf5 = (t_bool)STM_GET_OV5(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status weather there is atleast 1 overflow for master 9 */
        stm_ofsr->stm_ovf9 = (t_bool)STM_GET_OV9(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status of illegal address error */
        stm_ofsr->stm_adderr = (t_uint8)STM_GET_ADDERR(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status of Decode Error */
        stm_ofsr->stm_decerr = (t_bool)STM_GET_DECERR(g_stm_system_context.p_stm_register->stm_ofsr[0]);

		/* Get the current status of secure Error */
		stm_ofsr->stm_secerr = (t_bool)STM_GET_SECERR(g_stm_system_context.p_stm_register->stm_ofsr[0]);

    DBGEXIT(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_GetTransmitFIFOStatus() 						*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to read the status of transmit FIFO     */
/* PARAMETERS:																*/
/* IN   :   None                                                            */
/* OUT : t_stm_tfsr:The structure of transmit FIFO is empty and full status */
/* RETURN:																	*/
/*			t_stm_error: Return the correct error code                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC t_stm_error STM_GetTransmitFIFOStatus(OUT t_stm_tfsr  *stm_tfsr)
{

    /* Check the Flag for RAM is Empty or not */
    stm_tfsr->stm_rame = (t_bool)STM_GET_RAM_EMPTY_STATUS(g_stm_system_context.p_stm_register->stm_tfsr[0]);

	/* Check the Flag for RAM is Full or not */
	stm_tfsr->stm_ramf = (t_bool)STM_GET_RAM_FULL_STATUS(g_stm_system_context.p_stm_register->stm_tfsr[0]);

	/* Check the Configuration change Flag */
	stm_tfsr->stm_chgconf = (t_bool)STM_GET_CHGCONF_STATUS(g_stm_system_context.p_stm_register->stm_tfsr[0]);

	/* Check the Flag for PRCMU FIFO is Empty or not */
	stm_tfsr->stm_prcmue = (t_bool)STM_GET_PRCMU_EMPTY_STATUS(g_stm_system_context.p_stm_register->stm_tfsr[0]);

	/* Check the Flag for PRCMU FIFO is FULL or not */
	stm_tfsr->stm_prcmuf = (t_bool)STM_GET_PRCMU_FULL_STATUS(g_stm_system_context.p_stm_register->stm_tfsr[0]);

    DBGEXIT(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_GetSBAGStatus()     							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to read the status of SBAG FIFO         */
/* PARAMETERS:																*/
/* IN   :   None                                                            */
/* OUT  :	t_stm_sbsr:The structure of the status of the SBAG FIFO         */
/* RETURN:																	*/
/*			t_stm_error: Return the correct error code                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC t_stm_error STM_GetSBAGStatus(OUT t_stm_sbsr  *stm_sbsr)
{

	/* Get the amount of byes recieved beyond SBAG I/F acceptance */
	stm_sbsr->stm_bytenb = (t_bool)STM_GET_SBAG_BYTENB(g_stm_system_context.p_stm_register->stm_sbsr[0]);

	/* Get the Miscount Value */
	stm_sbsr->stm_miscnt = (t_bool)STM_GET_SBAG_MISCOUNT(g_stm_system_context.p_stm_register->stm_sbsr[0]);

	/* Get the FIFO Empty Status of the SBAG FIFO */
	stm_sbsr->stm_sbfe = (t_bool)STM_GET_SBAG_FIFOEMPTY_FLAG(g_stm_system_context.p_stm_register->stm_sbsr[0]);

	/* Get the FIFO Full Status of the SBAG FIFO */
	stm_sbsr->stm_sbff = (t_bool)STM_GET_SBAG_FIFOFULL_FLAG(g_stm_system_context.p_stm_register->stm_sbsr[0]);

    /* Get the Overflow status for SBAG */
	stm_sbsr->stm_sbov = (t_bool)STM_GET_SBAG_OVERFOW_FLAG(g_stm_system_context.p_stm_register->stm_sbsr[0]);

	/* Check weather there is any overflow */
	if(stm_sbsr->stm_sbov) {
		stm_sbsr->stm_sbovn = STM_GET_SBAG_OVERFLOW_NUM(g_stm_system_context.p_stm_register->stm_sbsr[0]);
	}
	else
		stm_sbsr->stm_sbovn = STM_NO_OVERFLOW;
    DBGEXIT(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_SetDebug()           							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to set the debug register              */
/* PARAMETERS:																*/
/* IN   :   None                                                            */
/* OUT  :	t_stm_sbsr:The structure of the status of the SBAG FIFO         */
/* RETURN:																	*/
/*			t_stm_error: Return the correct error code                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC t_stm_error STM_SetDebug(IN t_stm_dbg  stm_dbg)
{

    /* Set the Debug Enable Register */
    STM_SET_DBGEN(g_stm_system_context.p_stm_register->stm_dbg[0],(t_uint32)stm_dbg.stm_dbgen);

    /* Set the Debug Mode Control Register */
    STM_SET_DBGM(g_stm_system_context.p_stm_register->stm_dbg[0],(t_uint32)stm_dbg.stm_dbgm);

    DBGEXIT(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_GetDebug()           							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to get the debug register              */
/* PARAMETERS:																*/
/* IN   :   None                                                            */
/* OUT  :	t_stm_sbsr:The structure of the status of the SBAG FIFO         */
/* RETURN:																	*/
/*			t_stm_error: Return the correct error code                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC t_stm_error STM_GetDebug(OUT t_stm_dbg  *stm_dbg)
{

    /* Get the Debug Enable Register */
	stm_dbg->stm_dbgen = (t_bool)STM_GET_DBGEN(g_stm_system_context.p_stm_register->stm_dbg[0]);

    /* Get the Debug Mode Control Register */
	stm_dbg->stm_dbgm = (t_bool)STM_GET_DBGM(g_stm_system_context.p_stm_register->stm_dbg[0]);

    /* Get the PTI Calibration Sequence Selection */
	stm_dbg->stm_pticssel = (t_uint8)STM_GET_PTICSSEL(g_stm_system_context.p_stm_register->stm_dbg[0]);

    /* Get the Force STP FIFO Full Selection */
	stm_dbg->stm_ftfsel = (t_uint8)STM_GET_FTFSEL(g_stm_system_context.p_stm_register->stm_dbg[0]);

	/* Get the Force SBAG FIFO Full Selection */
	stm_dbg->stm_fsbff = (t_bool)STM_GET_FSBFF(g_stm_system_context.p_stm_register->stm_dbg[0]);

	/* Get the Force PTI State Selection */
	stm_dbg->stm_ptistate = (t_uint8)STM_GET_PTISTATE(g_stm_system_context.p_stm_register->stm_dbg[0]);
	DBGEXIT(STM_OK);
    return(STM_OK);
}

/****************************************************************************/
/* NAME:	t_stm_error STM_GetTimeStamp()       							*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:This routine used to read the status of Time Stamp Register  */
/* PARAMETERS:																*/
/* IN   :   None                                                            */
/* OUT  :	None                                                            */
/* RETURN:																	*/
/*			t_stm_error: Return the correct error code                      */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													*/
/****************************************************************************/
PUBLIC t_stm_error STM_GetTimeStamp(t_uint32 *stm_ts)
{
    /* Store teh value of the free running time stamp counter */
	stm_ts[0] = STM_GET_TIMESTAMP(g_stm_system_context.p_stm_register->stm_tsr[0]);
	stm_ts[1] = STM_GET_TIMESTAMP(g_stm_system_context.p_stm_register->stm_tsr[1]);

    DBGEXIT(STM_OK);
    return(STM_OK);

}

/******************************************************************************/
/* NAME:	t_stm_error STM_GetSleepClockCounter() 							  */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:This routine is used to read the status of Sleep Clock Counter */
/* PARAMETERS:																  */
/* IN   :   None                                                              */
/* OUT  :	None                                                              */
/* RETURN:																	  */
/*			t_stm_error: Return the correct error code                        */
/*----------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant													  */
/******************************************************************************/
PUBLIC t_stm_error STM_GetSleepClockCounter(t_uint32 *stm_scc)
{
    /* Store teh value of the free running time stamp counter */
	stm_scc[0] = STM_GET_SLEEPCLOCK_COUNTER(g_stm_system_context.p_stm_register->stm_sccr[0]);
	stm_scc[1] = STM_GET_SLEEPCLOCK_COUNTER(g_stm_system_context.p_stm_register->stm_sccr[1]);
    DBGEXIT(STM_OK);
    return(STM_OK);

}



/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendMessage8()   					     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 8 bit message through STM			              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint8   data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendMessage8
(
IN t_uint32          channel_no,
IN t_uint8          data
)
{
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("STM channel selected is : %d and value is : %d",channel_no,data);
   
           *(t_uint8 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator =  data;
		   stm_error = STM_OK;
      

    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendMessage16()   					     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 16 bit message through STM			              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint16  data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendMessage16
(
IN t_uint32          channel_no,
IN t_uint16          data
)
{
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);

			
           *(t_uint16 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator =  data;


           stm_error = STM_OK;
      

    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendMessage32()   					     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 32 bit message through STM			              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint32  data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendMessage32
(
IN t_uint32          channel_no,
IN t_uint32          data
)
{
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
   
           *(t_uint32 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator =  data;
           stm_error = STM_OK;
      
    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendMessage64()   					     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 32 bit message through STM			              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint32  *data: The data that is to be written ,                             */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendMessage64
(
IN t_uint32          channel_no,
IN t_uint32          *data
)
{
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
   
           g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator[1] =  *data++;
		   
           g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator[0]=  *data;

           stm_error = STM_OK;
      
    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendTimeStampMessage8()				     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 8 bit time stamp message through STM		      */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint08  data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendTimeStampMessage8
(
IN t_uint32          channel_no,
IN t_uint8           data
)
{
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO Time stamp channel selected is : %d and value is : %d",channel_no,data);
    
           *(t_uint8 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp =  data;
           stm_error = STM_OK;
          
    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendTimeStampMessage16()			     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 16 bit time stamp message through STM		      */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint16  data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendTimeStampMessage16
(
IN t_uint32          channel_no,
IN t_uint16          data
)
{
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO Time stamp channel selected is : %d and value is : %d",channel_no,data);
    
           *(t_uint16 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp =  data;
           stm_error = STM_OK;
           
    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendTimeStampMessage32()				   		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send time stamp message through STM		              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint32  data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendTimeStampMessage32
(
IN t_uint32          channel_no,
IN t_uint32          data
)
{
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO Time stamp channel selected is : %d and value is : %d",channel_no,data);
    
           *(t_uint32 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp =  data;
           stm_error = STM_OK;
           
    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendTimeStampMessage64()				   		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send time stamp message through STM		              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint32  data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendTimeStampMessage64
(
IN t_uint32          channel_no,
IN t_uint32          *data
)
{
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO Time stamp channel selected is : %d and value is : %d",channel_no,data);
    
           g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp[1] =  *data++;
		   g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp[0] =  *data;
           stm_error = STM_OK;
           
    DBGEXIT0(stm_error);
    return(stm_error);
}


/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendBlockMessage8()   				     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 8 bit message through STM			              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint8   data: The data that is to be written ,                              */
/*              t_uint32  data_length:The length of the message block to be sent              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendBlockMessage8
(
IN t_uint32         channel_no,
IN t_uint8          *data,
IN t_uint32         data_length 
)
{
    t_uint32 index;
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
    for(index = 0x00;index < data_length;index++)
    {
      *(t_uint8 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator =  *data++;
    }

    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendBlockMessage16()  				     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 16 bit message through STM			              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint16  data: The data that is to be written ,                              */
/*              t_uint32  data_length:The length of the message block to be sent              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendBlockMessage16
(
IN t_uint32          channel_no,
IN t_uint16          *data,
IN t_uint32          data_length
)
{
    t_uint32 index;
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
    for(index = 0x00;index < data_length;index++)
    {
       *(t_uint16 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator =  *data++;
    }

    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendBlockessage32()   				     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 32 bit message through STM			              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint32  data: The data that is to be written ,                              */
/*              t_uint32  data_length:The length of the message block to be sent              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendBlockMessage32
(
IN t_uint32          channel_no,
IN t_uint32          *data,
IN t_uint32          data_length
)
{
    t_uint32 index;
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
    for(index = 0x00;index < data_length;index++)
    {
       *(t_uint32 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator =  *data++;
    }

    DBGEXIT0(stm_error);
    return(stm_error);
}



/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendBlockessage64()   				     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 64 bit message through STM			              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint32  data: The data that is to be written ,                              */
/*              t_uint32  data_length:The length of the message block to be sent              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendBlockMessage64
(
IN t_uint32          channel_no,
IN t_uint32          *data,
IN t_uint32          data_length
)
{
    t_uint32 index;
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
    for(index = 0x00;index < data_length;index++)
    {
	   g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator[1] =  *data++;
	   g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator[0]=  *data++;
    }

    DBGEXIT0(stm_error);
    return(stm_error);
}




/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendBlockTimeStampMessage8()		     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 8 bit time stamp message through STM		      */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint08  data: The data that is to be written ,                              */
/*              t_uint32  data_length:The length of the message block to be sent              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendBlockTimeStampMessage8
(
IN t_uint32          channel_no,
IN t_uint8           *data,
IN t_uint32          data_length
)
{
    t_uint32 index;
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
    for(index = 0x00;index < data_length;index++)
    {
	   *(t_uint8 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp =  *data++;       
    }

    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendBlockTimeStampMessage16()		     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send 16 bit time stamp message through STM		      */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint16  data: The data that is to be written ,                              */
/*              t_uint32  data_length:The length of the message block to be sent              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendBlockTimeStampMessage16
(
IN t_uint32          channel_no,
IN t_uint16          *data,
IN t_uint32          data_length
)
{
   t_uint32 index;
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
    for(index = 0x00;index < data_length;index++)
    {
      *(t_uint16 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp =  *data++;
    }

    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendBlockTimeStampMessage32()			   		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send time stamp message through STM		              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint32  data: The data that is to be written ,                              */
/*              t_uint32  data_length:The length of the message block to be sent              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendBlockTimeStampMessage32
(
IN t_uint32          channel_no,
IN t_uint32          *data,
IN t_uint32          data_length
)
{
    t_uint32 index;
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
    for(index = 0x00;index < data_length;index++)
    {
	   *(t_uint32 *)g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp =  *data++;
    }

    DBGEXIT0(stm_error);
    return(stm_error);
}

/**********************************************************************************************/
/* NAME:	t_stm_error STM_SendBlockTimeStampMessage64()			   		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send time stamp message through STM		              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint32  data: The data that is to be written ,                              */
/*              t_uint32  data_length:The length of the message block to be sent              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error STM_SendBlockTimeStampMessage64
(
IN t_uint32          channel_no,
IN t_uint32          *data,
IN t_uint32          data_length
)
{
    t_uint32 index;
    t_stm_error    stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    DBGENTER2("OSMO channel selected is : %d and value is : %d",channel_no,data);
    for(index = 0x00;index < data_length;index++)
    {
	   g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp[1] =  *data++;
	   g_stm_system_context.p_stm_initiator->stm_initiator_init[channel_no].stm_initiator_timestamp[0] =  *data++;
    }

    DBGEXIT0(stm_error);
    return(stm_error);
}




/* End of file - stm.c */





/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the TIMER(MTU) 
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*--------------------------------------------------------------------------*
 * Includes                                                                 *
 *--------------------------------------------------------------------------*/
#include "tmr.h"
#include "tmr_p.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/
/*For debug HCL */
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_TMR
#define MY_DEBUG_ID             myDebugID_TMR
PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id                MY_DEBUG_ID = TMR_HCL_DBG_ID;
#endif

/*Global definitions for the TIMER driver*/
PRIVATE t_tmr_system_context    g_tmr_system_context;

PRIVATE t_uint32 tmr_TestIrqSrc(t_tmr_irq_src tmr_irq_src)
{
    t_uint32    src;
    switch (tmr_irq_src)
    {
        case TMR_ID_0:
            src = 0;
            break;

        case TMR_ID_1:
            src = 1;
            break;

        case TMR_ID_2:
            src = 2;
            break;

        case TMR_ID_3:
            src = 3;
            break;

        case TMR_ID_4:
            src = 4;
            break;

        case TMR_ID_5:
            src = 5;
            break;

        case TMR_ID_6:
            src = 6;
            break;

        case TMR_ID_7:
            src = 7;
            break;

        default:
            DBGEXIT0(TMR_INTERNAL_EVENT);
            return((t_uint32) TMR_INTERNAL_EVENT);
    }

    return(src);
}

/*PUBLIC API FUNCTIONS*/
/********************************************************************************/
/* NAME:	 TMR_Init()                                                      	*/
/*------------------------------------------------------------------------------*/
/* DESCRIPTION   : This routine initializes the TMR units. This function is  	*/
/*                 called twice, once for each timer unit.                   	*/
/* PARAMETERS    :											                	*/
/* IN            : t_logical_address                                         	*/
/* INOUT         : None                                                      	*/
/* OUT           : None                                                      	*/
/* RETURN VALUE  : void												        	*/
/* TYPE          : Public                                                    	*/
/*------------------------------------------------------------------------------*/
/* REENTRANCY: NA														    	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_Init(t_tmr_device_id tmr_device_id, t_logical_address tmr_base_address)
{
    t_uint32        tmr_device_id_index = 0;
    t_tmr_register  *p_tmr_register;
    DBGENTER2("(%d,%lx)", tmr_device_id, tmr_base_address);

    switch (tmr_device_id)
    {
        case TMR_DEVICE_ID_0:
            tmr_device_id_index = 0;
            break;

        case TMR_DEVICE_ID_1:
            tmr_device_id_index = 1;
            break;

        case TMR_DEVICE_ID_INVALID:
        default:
            DBGEXIT0(TMR_INVALID_PARAMETER);
            return(TMR_INVALID_PARAMETER);
    }

    if (NULL != tmr_base_address)
    {
        /* Initializing the registers structure
        ---------------------------------------*/
        p_tmr_register = g_tmr_system_context.p_tmr_register[tmr_device_id_index] = (t_tmr_register *) tmr_base_address;    /* TBC TBD */

        /* Checking Peripheral Ids *
         *-------------------------*/
        if
        (
            (TMR_PERIPHID0 == p_tmr_register->tmr_periph_id0)
        &&  (TMR_PERIPHID1 == p_tmr_register->tmr_periph_id1)
        &&  (TMR_PERIPHID2 == p_tmr_register->tmr_periph_id2)
        &&  (TMR_PERIPHID3 == p_tmr_register->tmr_periph_id3)
        &&  (TMR_PCELLID0 == p_tmr_register->tmr_pcell0)
        &&  (TMR_PCELLID1 == p_tmr_register->tmr_pcell1)
        &&  (TMR_PCELLID2 == p_tmr_register->tmr_pcell2)
        &&  (TMR_PCELLID3 == p_tmr_register->tmr_pcell3)
        )
        {
            if (TMR_OK == TMR_Reset(tmr_device_id))
            {
                DBGEXIT0(TMR_OK);
                return(TMR_OK);
            }
        }
    }   /* end if */

    DBGEXIT0(TMR_INVALID_PARAMETER);
    return(TMR_INVALID_PARAMETER);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_Reset(t_tmr_device_id tmr_device_id)               	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This routine initializes the TMR units. This function is     	*/
/*              called twice, one for each timer unit.It should be called    	*/
/*              after SetBaseAddress.                                        	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/* IN : t_tmr_device_id tmr_device_id   	                                 	*/
/*                                                                           	*/
/* OUT :                                                                     	*/
/*                                                                           	*/
/* RETURN: t_tmr_error                 : TMR error code                      	*/
/*                     TMR_UNSUPPORTED_HW in case the						 	*/
/*                     hw unit is not congruent with the ID code             	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY: NA                                                            	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_Reset(t_tmr_device_id tmr_device_id)
{
    t_tmr_error tmr_error = TMR_OK;
    t_uint32    mask;

    DBGENTER1(" (%d)", tmr_device_id);

    if (TMR_DEVICE_ID_0 == tmr_device_id)
    {
        TMR_ClearIRQSrc(TMR_TIMER_IDS_IN_TMR_UNIT0);
    }
    else if (TMR_DEVICE_ID_1 == tmr_device_id)
    {
        TMR_ClearIRQSrc(TMR_TIMER_IDS_IN_TMR_UNIT1);
    }
    else
    {
        tmr_error = TMR_INVALID_PARAMETER;
        DBGEXIT0(tmr_error);
        return(tmr_error);
    }

    g_tmr_system_context.number_of_total_timers = TMR_TOTAL_NUMBER_OF_TIMERS;

    /*initialize the masks*/
    g_tmr_system_context.tmr_mask[0] = TMR_ID_0;
    g_tmr_system_context.tmr_mask[1] = TMR_ID_1;
    g_tmr_system_context.tmr_mask[2] = TMR_ID_2;
    g_tmr_system_context.tmr_mask[3] = TMR_ID_3;
    g_tmr_system_context.tmr_mask[4] = TMR_ID_4;
    g_tmr_system_context.tmr_mask[5] = TMR_ID_5;
    g_tmr_system_context.tmr_mask[6] = TMR_ID_6;
    g_tmr_system_context.tmr_mask[7] = TMR_ID_7;

    if (TMR_DEVICE_ID_0 == tmr_device_id)
    {
        g_tmr_system_context.tmr_temp_ptr[0] = (t_tmr_register_subset *) &(g_tmr_system_context.p_tmr_register[0]->tmr_load1);
        g_tmr_system_context.tmr_temp_ptr[1] = (t_tmr_register_subset *) &(g_tmr_system_context.p_tmr_register[0]->tmr_load2);
        g_tmr_system_context.tmr_temp_ptr[2] = (t_tmr_register_subset *) &(g_tmr_system_context.p_tmr_register[0]->tmr_load3);
        g_tmr_system_context.tmr_temp_ptr[3] = (t_tmr_register_subset *) &(g_tmr_system_context.p_tmr_register[0]->tmr_load4);
    }
    else if (TMR_DEVICE_ID_1 == tmr_device_id)
    {
        g_tmr_system_context.tmr_temp_ptr[4] = (t_tmr_register_subset *) &(g_tmr_system_context.p_tmr_register[1]->tmr_load1);
        g_tmr_system_context.tmr_temp_ptr[5] = (t_tmr_register_subset *) &(g_tmr_system_context.p_tmr_register[1]->tmr_load2);
        g_tmr_system_context.tmr_temp_ptr[6] = (t_tmr_register_subset *) &(g_tmr_system_context.p_tmr_register[1]->tmr_load3);
        g_tmr_system_context.tmr_temp_ptr[7] = (t_tmr_register_subset *) &(g_tmr_system_context.p_tmr_register[1]->tmr_load4);
    }

    switch (tmr_device_id)
    {
        case TMR_DEVICE_ID_0:
            tmr_EnterCriticalSection(&mask);
            g_tmr_system_context.tmr_state[0] = TMR_STATE_FREE;
            g_tmr_system_context.tmr_state[1] = TMR_STATE_FREE;
            g_tmr_system_context.tmr_state[2] = TMR_STATE_FREE;
            g_tmr_system_context.tmr_state[3] = TMR_STATE_FREE;
            tmr_ExitCriticalSection(mask);

            /*the only bit to be initialized is the Timer Interrupt Enable    */
            /*It is enabled at reset, but for safety We reprogram it          */
            MTU_TIMER_SIZE_SET_0(TIMER_SIZE_32);
            MTU_TIMER_SIZE_SET_1(TIMER_SIZE_32);
            MTU_TIMER_SIZE_SET_2(TIMER_SIZE_32);
            MTU_TIMER_SIZE_SET_3(TIMER_SIZE_32);
            TMR_EnableIRQSrc(TMR_TIMER_IDS_IN_TMR_UNIT0);
            break;

        case TMR_DEVICE_ID_1:
            tmr_EnterCriticalSection(&mask);
            g_tmr_system_context.tmr_state[4] = TMR_STATE_FREE;
            g_tmr_system_context.tmr_state[5] = TMR_STATE_FREE;
            g_tmr_system_context.tmr_state[6] = TMR_STATE_FREE;
            g_tmr_system_context.tmr_state[7] = TMR_STATE_FREE;
            tmr_ExitCriticalSection(mask);

            /*the only bit to be initialized is the Timer Interrupt Enable    */
            /*It is enabled at reset, but for safety We reprogram it          */
            MTU_TIMER_SIZE_SET_4(TIMER_SIZE_32);
            MTU_TIMER_SIZE_SET_5(TIMER_SIZE_32);
            MTU_TIMER_SIZE_SET_6(TIMER_SIZE_32);
            MTU_TIMER_SIZE_SET_7(TIMER_SIZE_32);
            TMR_EnableIRQSrc(TMR_TIMER_IDS_IN_TMR_UNIT1);

            break;

        
    }

    DBGEXIT0(tmr_error);
    return(tmr_error);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_Configure(t_tmr_id  tmr_id,     		             	*/
/*               		    t_tmr_mode tmr_mode,                            	*/
/*               		    t_uint32 load_value,                             	*/
/*               		    t_tmr_clk_prescaler tmr_clk_prescaler)            	*/
/*               		                                                     	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This routine initializes the timer parameters		         	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/* IN :      t_tmr_id   tmr_id:the variable gives the timer ID to initialize	*/
/* IN :      t_tmr_mode tmr_mode :gives the timer mode(Periodic,Free-Running	*/
/*	  			or one shot		           	                                 	*/
/* IN :      t_uint32   load_value :from which to start counting		     	*/
/* IN :      t_uint32   tmr_clk_prescaler :the variable gives the timer ID to 	*/
/*                                     initialize                            	*/
/* OUT :                       		                             	         	*/
/*                                                                           	*/
/* RETURN: t_tmr_error : TMR error code. This function will return  	     	*/
/*                       TMR_OK 				   	                         	*/
/* 		         TMR_ERROR_INVALID_TIMER_ID invalid tmr_id	                 	*/
/* 		         TMR_ERROR_TIMER_ALREADY_STARTED timer already runing        	*/
/* 		         TMR_INVALID_PARAMETER One parameter is invalid        	     	*/
/*                       tmr_clk_prescaler has not been set to 1, 16 or256    	*/
/*                                		                                     	*/
/* COMMENTS: The Timer must have been allocated with TMR_AllocTimer and not  	*/
/*           started             					                         	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY:The function is non-reentrant as it modifies global 				*/
/*						g_tmr_system_context.tmr_state  						*/
/*            Hence it has been put into critical section                    	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_Configure
(
    t_tmr_id            tmr_id,
    t_tmr_mode          tmr_mode,
    t_uint32            load_value,
    t_tmr_clk_prescaler tmr_clk_prescaler
)
{
    t_tmr_error             tmr_error = TMR_OK;
    t_tmr_register_subset   *p_tmr_register_subset;
    t_uint32                index;
    t_uint32                mask = 0;

    DBGENTER4
    (
        "tmr_id %d, tmr_mode %d,  load_value %ld,   tmr_clk_prescaler %d\n",
        tmr_id,
        tmr_mode,
        load_value,
        tmr_clk_prescaler
    );

    if (0 == load_value)
    {
        tmr_error = TMR_INVALID_PARAMETER;
        DBGEXIT0(tmr_error);
        return(tmr_error);
    }

    /*Check that the timer ID is a valid Timer ID */
    for (index = 0; index < g_tmr_system_context.number_of_total_timers; index++)
    {
        if (g_tmr_system_context.tmr_mask[index] == tmr_id)
        {
            /*check if the timer is in use or not */
            tmr_EnterCriticalSection(&mask);
            switch (g_tmr_system_context.tmr_state[index])
            {
                /*check that the timer has been initialized*/
                case TMR_STATE_BUSY:        /*initialize the timer*/
                    break;

                case TMR_STATE_INITIALIZED: /*re initialize the timer*/
                    g_tmr_system_context.tmr_state[index] = TMR_STATE_BUSY;
                    break;

                case TMR_STATE_FREE:
                    tmr_ExitCriticalSection(mask);
                    return(TMR_ERROR_INVALID_TIMER_ID);

                case TMR_STATE_RUNNING:
                    tmr_ExitCriticalSection(mask);
                    return(TMR_ERROR_TIMER_ALREADY_STARTED);

                default:
                    return(TMR_INVALID_PARAMETER);
            }

            tmr_ExitCriticalSection(mask);
            break;
        }
    }

    if (index == g_tmr_system_context.number_of_total_timers)
    {
        return(TMR_ERROR_INVALID_TIMER_ID);
    }

    /*End Check that the timer ID is a valid Timer ID */
    /*Get the ptr to the structure in use             */
    p_tmr_register_subset = g_tmr_system_context.tmr_temp_ptr[index];   /*address of a specific timer area*/

    /*End Get the ptr to the structure in use         */
    /*Set the timer mode to the passed value*/
    switch (tmr_mode)
    {
        case TMR_MODE_FREE_RUNNING:
            tmr_EnterCriticalSection(&mask);
            g_tmr_system_context.tmr_mode[index] = TMR_MODE_FREE_RUNNING;
            tmr_ExitCriticalSection(mask);

            TMR_PERIODIC_TIMER_DISABLE;
            TMR_TIMER_ONE_SHOT_SET(TIMER_WRAPPING);
            break;

        case TMR_MODE_PERIODIC:
            tmr_EnterCriticalSection(&mask);
            g_tmr_system_context.tmr_mode[index] = TMR_MODE_PERIODIC;
            tmr_ExitCriticalSection(mask);

            TMR_PERIODIC_TIMER_ENABLE;
            TMR_TIMER_ONE_SHOT_SET(TIMER_WRAPPING);
            break;

        case TMR_MODE_ONE_SHOT:
            tmr_EnterCriticalSection(&mask);
            g_tmr_system_context.tmr_mode[index] = TMR_MODE_ONE_SHOT;
            tmr_ExitCriticalSection(mask);

            TMR_TIMER_ONE_SHOT_SET(TIMER_ONE_SHOT);
            break;

        default:
            return(TMR_INVALID_PARAMETER);
    }

    /*END Set the timer mode to the passed value*/
    /*set the passed loadvalue in the down counter*/
    TMR_SET_LOAD_REG(load_value);

    /*END set the passed loadvalue in the down counter*/
    /*set the clk division*/
    switch (tmr_clk_prescaler)
    {
        case TMR_CLK_PRESCALER_1:
            TMR_PRESCALER_SET(0x0);
            break;

        case TMR_CLK_PRESCALER_16:
            TMR_PRESCALER_SET(0x1);
            break;

        case TMR_CLK_PRESCALER_256:
            TMR_PRESCALER_SET(0x2);
            break;

        default:
            return(TMR_INVALID_PARAMETER);
    }

    /*END set the clk division*/
    tmr_EnterCriticalSection(&mask);
    g_tmr_system_context.tmr_state[index] = TMR_STATE_INITIALIZED;
    tmr_ExitCriticalSection(mask);
    DBGEXIT0(TMR_OK);
    return(TMR_OK);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_AllocTimer(t_tmr_id  *p_tmr_id)     		         	*/
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allocates a timer and returns its number	     	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/* IN :      	                  	                  	                     	*/
/*                                                                           	*/
/* OUT :    t_tmr_id  *p_tmr_id   the variable points to a new timer ID if   	*/
/*                                available. This Timer ID is the physical   	*/
/*                                timer number                               	*/
/*                                                                           	*/
/* RETURN: t_tmr_error            :TMR error code. This function will return 	*/
/*                                TMR_OK or TMR_ERROR_NO_MORE_TIMER_AVAILABLE	*/
/*                                if no more timer can be allocated          	*/
/*                                                                           	*/
/* COMMENTS: Before to be used a timer ID must be allocated using this function	*/
/*           TimerIDs are always integers with only one bit set, to allow    	*/
/*           ProcessIT to return a timer ID list using a 32 bit mask         	*/
/*           The returned value must be used to check the interrupt mask when	*/
/*           an interrupt occurs                                             	*/
/*------------------------------------------------------------------------------*/
/* REENTRANCY: The function is non-reentrant as it modifies global 				*/
/*												g_tmr_system_context.tmr_state 	*/
/*             Hence it has been put into critical section                   	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_AllocTimer(t_tmr_id *p_tmr_id)
{
    t_uint32    index;
    t_tmr_error tmr_error = TMR_OK;
    t_uint32    mask;

    DBGENTER0();
    for (index = 0; index < g_tmr_system_context.number_of_total_timers; index++)
    {
        tmr_EnterCriticalSection(&mask);
        if (TMR_STATE_FREE == g_tmr_system_context.tmr_state[index])
        {
            g_tmr_system_context.tmr_state[index] = TMR_STATE_BUSY;
            *p_tmr_id = g_tmr_system_context.tmr_mask[index];
            tmr_ExitCriticalSection(mask);
            DBGEXIT1(tmr_error, "tmr_id=%d\n", *p_tmr_id);
            return(tmr_error);
        }

        tmr_ExitCriticalSection(mask);
    }

    if (index == g_tmr_system_context.number_of_total_timers)
    {
        tmr_error = TMR_ERROR_NO_MORE_TIMER_AVAILABLE;
    }

    DBGEXIT1(tmr_error, "tmr_id=%d\n", *p_tmr_id);
    return(tmr_error);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_StartTimer(t_tmr_id tmr_id)     		         	 	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This routine starts a virtual  timer decounting 	         	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/*                                                                           	*/
/* IN :    t_tmr_id   tmr_id    the variable gives the timer ID to start     	*/
/*                                                                           	*/
/* OUT :      	                  	                  	                     	*/
/*                                                                           	*/
/* RETURN: t_tmr_error  TMR error code. This function will return            	*/
/*                      TMR_OK  			             	                 	*/
/*                      TMR_ERROR_INVALID_TIMER_ID   if the given timer is   	*/
/*                       invalid			                                 	*/
/* 		        TMR_ERROR_TIMER_ALREADY_STARTED timer already running        	*/
/*                                                                           	*/
/* COMMENTS: A Timer ID must be queried and initialized before starting it.  	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY:The function is non-reentrant as it modifies global 			 	*/
/*	g_tmr_system_context.tmr_state  										 	*/
/*            Hence it has been put into critical section                    	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_StartTimer(t_tmr_id tmr_id)
{
    t_tmr_register_subset   *p_tmr_register_subset;
    t_uint32                index;
    t_uint32                mask = 0;

    /*check that the timer has been queried*/
    DBGENTER1("tmr_id %d\n", tmr_id);

    for (index = 0; index < g_tmr_system_context.number_of_total_timers; index++)
    {
        if (g_tmr_system_context.tmr_mask[index] == tmr_id)
        {
            tmr_EnterCriticalSection(&mask);

            /*check the state of the timer                    */
            switch (g_tmr_system_context.tmr_state[index])
            {
                case TMR_STATE_FREE:
                case TMR_STATE_BUSY:
                    tmr_ExitCriticalSection(mask);
                    return(TMR_ERROR_INVALID_STATE);

                case TMR_STATE_RUNNING:
                    tmr_ExitCriticalSection(mask);
                    return(TMR_ERROR_TIMER_ALREADY_STARTED);

                /*check that the timer has been initialized*/
                case TMR_STATE_INITIALIZED:
                    /*start the initialized timer*/
                    /*Get the ptr to the structure in use             */
                    p_tmr_register_subset = g_tmr_system_context.tmr_temp_ptr[index];   /*address of a specific timer area*/
                    g_tmr_system_context.tmr_state[index] = TMR_STATE_RUNNING;

                    /*start the previously initialized timer, by setting the enable bit in the */
                    /*CTRL register     		         		                      */
                    TMR_TIMER_ENABLE;
                    break;

                default:
                    return(TMR_INVALID_PARAMETER);
            }

            tmr_ExitCriticalSection(mask);
            break;
        }
    }

    if (index == g_tmr_system_context.number_of_total_timers)
    {
        return(TMR_ERROR_INVALID_TIMER_ID);
    }

    DBGEXIT0(TMR_OK);
    return(TMR_OK);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_LoadBackGround(t_tmr_id  tmr_id,     	         	 	*/
/*               		    t_uint32 load_value                              	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This routine initializes the background register in Hex      	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/* IN : t_tmr_id   tmr_id:the variable gives the timer ID to be initialized  	*/
/* IN : t_uint32   load_value :value in hex to be written in Background 	 	*/
/* 					register												 	*/
/* OUT :                       		                             	         	*/
/*                                                                           	*/
/* RETURN: t_tmr_error : TMR error code. This function will return  	     	*/
/*                       TMR_OK 				   	                         	*/
/* 		         TMR_ERROR_INVALID_VALUE invalid value	                     	*/
/* COMMENTS: The Timer must have been allocated with TMR_AllocTimer ,        	*/
/*           initialized and started             					         	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY: NA                                                            	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_LoadBackGround(t_tmr_id tmr_id, t_uint32 load_value)
{
    t_tmr_error             tmr_error = TMR_OK;
    t_tmr_register_subset   *p_tmr_register_subset;
    t_uint32                index;

    DBGENTER2("tmr_id %d,   load_value %ld \n", tmr_id, load_value);

    if (0 == load_value)
    {
        tmr_error = TMR_INVALID_PARAMETER;
        DBGEXIT0(tmr_error);
        return(tmr_error);
    }

    /*Check that the timer ID is a valid Timer ID */
    for (index = 0; index < g_tmr_system_context.number_of_total_timers; index++)
    {
        if (g_tmr_system_context.tmr_mask[index] == tmr_id)
        {
            /*check if the timer is in use or not */
            switch (g_tmr_system_context.tmr_state[index])
            {
                case TMR_STATE_BUSY:
                    return(TMR_ERROR_TIMER_ALREADY_STOPPED);

                case TMR_STATE_INITIALIZED: /*re initialize the timer*/
                    return(TMR_ERROR_INVALID_TIMER_ID);

                case TMR_STATE_FREE:
                    return(TMR_ERROR_INVALID_TIMER_ID);

                case TMR_STATE_RUNNING:
                    break;

                default:
                    return(TMR_ERROR_INVALID_TIMER_ID);
            }
            break;
        }
    }

    if (index == g_tmr_system_context.number_of_total_timers)
    {
        DBGEXIT0(TMR_ERROR_INVALID_TIMER_ID);
        return(TMR_ERROR_INVALID_TIMER_ID);
    }

    p_tmr_register_subset = g_tmr_system_context.tmr_temp_ptr[index];
    TMR_SET_BGLOAD_REG(load_value);
    DBGEXIT0(TMR_OK);
    return(TMR_OK);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_StopTimer(t_tmr_id  tmr_id)     		             	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This routine stops a timer			 	                     	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/*                                                                           	*/
/* IN :    t_tmr_id   tmr_id    the variable gives the timer ID to stop      	*/
/*                                                                           	*/
/* OUT :      	                  	                  	                     	*/
/*                                                                           	*/
/* RETURN: t_tmr_error TMR error code. This function will return             	*/
/*                     TMR_OK  			             	                     	*/
/*                     TMR_ERROR_INVALID_TIMER_ID   if the given timer is    	*/
/*                     invalid or unasable			                         	*/
/* 		       TMR_ERROR_TIMER_ALREADY_STOPPED timer already stopped         	*/
/*                                                                           	*/
/* COMMENTS: A Timer ID must be queried and started before to this call.     	*/
/*           It is possible to free a running timer without stopping it      	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY:The function is non-reentrant as it modifies global 			 	*/
/* 				g_tmr_system_context.tmr_state  							 	*/
/*            Hence it has been put into critical section                    	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_StopTimer(t_tmr_id tmr_id)
{
    t_tmr_register_subset   *p_tmr_register_subset;
    t_uint32                index;
    t_uint32                mask = 0;

    /*Check that the timer ID is a valid Timer ID */
    DBGENTER1("tmr_id %d\n", tmr_id);

    for (index = 0; index < g_tmr_system_context.number_of_total_timers; index++)
    {
        if (g_tmr_system_context.tmr_mask[index] == tmr_id)
        {
            /*check if the timer is in use or not */
            tmr_EnterCriticalSection(&mask);
            switch (g_tmr_system_context.tmr_state[index])
            {
                case TMR_STATE_RUNNING: /*stop the running timer*/
                    /*Get the ptr to the structure in use             */
                    p_tmr_register_subset = g_tmr_system_context.tmr_temp_ptr[index];   /*address of a specific timer area*/
                    TMR_TIMER_DISABLE;
                    g_tmr_system_context.tmr_state[index] = TMR_STATE_BUSY;
                    break;

                case TMR_STATE_BUSY:
                    tmr_ExitCriticalSection(mask);
                    return(TMR_ERROR_TIMER_ALREADY_STOPPED);

                case TMR_STATE_INITIALIZED:
                case TMR_STATE_FREE:
                    tmr_ExitCriticalSection(mask);
                    return(TMR_ERROR_INVALID_TIMER_ID);

                default:
                    return(TMR_INVALID_PARAMETER);
            }

            tmr_ExitCriticalSection(mask);
            break;
        }
    }

    if (index == g_tmr_system_context.number_of_total_timers)
    {
        return(TMR_ERROR_INVALID_TIMER_ID);
    }

    /*End Check that the timer ID is a valid Timer ID */
    DBGEXIT0(TMR_OK);
    return(TMR_OK);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_ReStartTimer()     		                         	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This is to restart the ONE SHOT timer(initialize as well as  	*/
/*              start it.                                                    	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/*                                                                           	*/
/* IN :    t_tmr_id   tmr_id    the variable gives the timer ID to start     	*/
/*         t_uint32   load_value  in Hexadecimal                             	*/
/*         t_tmr_clk_prescaler   tmr_clk_prescaler                           	*/
/*                                                                           	*/
/* OUT :      	                  	                  	                     	*/
/*                                                                           	*/
/* RETURN: t_tmr_error  TMR error code. This function will return            	*/
/*                      TMR_OK  			             	                 	*/
/*                      TMR_ERROR_INVALID_TIMER_ID   if the given timer is   	*/
/*                       invalid			                                 	*/
/* 		        TMR_ERROR_TIMER_ALREADY_STARTED timer already running        	*/
/*                                                                           	*/
/* COMMENTS: This API is only for ONE SHOT TIMER ,it loads count and starts  	*/
/*           the ONE SHOT Timer.                                             	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY:The function is non-reentrant as it modifies global 			 	*/
/* 									g_tmr_system_context.tmr_state  		 	*/
/*            Hence it has been put into critical section                    	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_ReStartTimer(t_tmr_id tmr_id, t_uint32 load_value, t_tmr_clk_prescaler tmr_clk_prescaler)
{
    t_tmr_error             tmr_error;
    t_tmr_register_subset   *p_tmr_register_subset;
    t_uint32                index;
    t_uint32                mask = 0;

    DBGENTER3("tmr_id %d, load_value %ld, tmr_clk_prescaler %d\n", tmr_id, load_value, tmr_clk_prescaler);
    if (0 == load_value)
    {
        tmr_error = TMR_INVALID_PARAMETER;
        DBGEXIT0(tmr_error);
        return(tmr_error);
    }

    /*Check that the timer ID is a valid Timer ID */
    for (index = 0; index < g_tmr_system_context.number_of_total_timers; index++)
    {
        if (g_tmr_system_context.tmr_mask[index] == tmr_id)
        {
            /*check if the timer is in use or not */
            if (g_tmr_system_context.tmr_mode[index] != TMR_MODE_ONE_SHOT)
            {
                DBGEXIT0(TMR_ERROR_NOT_ONE_SHOT);
                return(TMR_ERROR_NOT_ONE_SHOT);
            }

            tmr_EnterCriticalSection(&mask);
            switch (g_tmr_system_context.tmr_state[index])
            {
                case TMR_STATE_FREE:
                    tmr_ExitCriticalSection(mask);
                    return(TMR_ERROR_INVALID_STATE);

                case TMR_STATE_RUNNING:
                    tmr_ExitCriticalSection(mask);
                    return(TMR_ERROR_TIMER_ALREADY_STARTED);

                case TMR_STATE_INITIALIZED:
                case TMR_STATE_BUSY:
                    /* start the initialized timer */
                    /* Get the ptr to the structure in use             */
                    p_tmr_register_subset = g_tmr_system_context.tmr_temp_ptr[index];   /* address of a specific timer area */
                    g_tmr_system_context.tmr_state[index] = TMR_STATE_INITIALIZED;

                    /* start the previously initialized timer, by setting the enable bit in the */
                    /* CTRL register */
                    break;

                default:
                    return(TMR_INVALID_PARAMETER);
            }

            tmr_ExitCriticalSection(mask);
            break;
        }
    }

    if (index == g_tmr_system_context.number_of_total_timers)
    {
        return(TMR_ERROR_INVALID_TIMER_ID);
    }

    /*End Check that the timer ID is a valid Timer ID */
    /*Get the ptr to the structure in use             */
    p_tmr_register_subset = g_tmr_system_context.tmr_temp_ptr[index];                   /*address of a specific timer area*/

    /*End Get the ptr to the structure in use         */
    /*set the passed loadvalue in the down counter*/
    TMR_SET_LOAD_REG(load_value);

    /*END set the passed loadvalue in the down counter*/
    /*set the clk division*/
    switch (tmr_clk_prescaler)
    {
        case TMR_CLK_PRESCALER_1:
            TMR_PRESCALER_SET(0x0);
            break;

        case TMR_CLK_PRESCALER_16:
            TMR_PRESCALER_SET(0x1);
            break;

        case TMR_CLK_PRESCALER_256:
            TMR_PRESCALER_SET(0x2);
            break;

        default:
            return(TMR_INVALID_PARAMETER);
    }

    tmr_EnterCriticalSection(&mask);
    g_tmr_system_context.tmr_state[index] = TMR_STATE_RUNNING;
    TMR_TIMER_ENABLE;
    tmr_ExitCriticalSection(mask);

    DBGEXIT(TMR_OK);
    return(TMR_OK);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_GetCounterValue(t_tmr_id tmr_id,  		         	*/
/*               		          t_uint32 *p_current_value)                 	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION:This routine gets the current counter value.                  	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/*                                                                           	*/
/* IN :     t_tmr_id  tmr_id  The tmr_id from where to get the value       	 	*/
/*                                                                           	*/
/* OUT :    t_uint32  *p_current_value   The current value		             	*/
/*                                                                           	*/
/* RETURN: t_tmr_error  TMR error code. This function will return            	*/
/*                      TMR_OK  			             	                 	*/
/*                      TMR_ERROR_INVALID_TIMER_ID   if the given timer is   	*/
/*                      invalid or unasable			                         	*/
/* 		        TMR_ERROR_TIMER_ALREADY_STOPPED timer already stopped        	*/
/*                                                                           	*/
/* COMMENTS:                                                                 	*/
/*                                                                           	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY: Reentrant                                                     	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_GetCounterValue(t_tmr_id tmr_id, t_uint32 *p_current_value)
{
    t_tmr_error             tmr_error = TMR_OK;
    t_uint32                index;
    t_tmr_register_subset   *p_tmr_register_subset;

    DBGENTER1("t_tmr_id %d\n", tmr_id);

    /*check that the timer has been queried*/
    for (index = 0; index < g_tmr_system_context.number_of_total_timers; index++)
    {
        if (g_tmr_system_context.tmr_mask[index] == tmr_id)
        {
            /*check the state of the timer*/
            switch (g_tmr_system_context.tmr_state[index])
            {
                case TMR_STATE_FREE:
                case TMR_STATE_BUSY:
                    return(TMR_ERROR_INVALID_TIMER_ID);

                case TMR_STATE_INITIALIZED:
                    return(TMR_ERROR_TIMER_ALREADY_STOPPED);

                case TMR_STATE_RUNNING:
                    /*return the counter*/
                    /*Get the ptr to the structure in use    */
                    p_tmr_register_subset = g_tmr_system_context.tmr_temp_ptr[index];   /*address of a specific timer area*/

                    /* return the content of the timerXvalue register*/
                    TMR_GET_VALUE_REG(*p_current_value);
                    break;

                default:
                    return(TMR_INVALID_PARAMETER);
            }
            break;
        }
    }

    if (index == g_tmr_system_context.number_of_total_timers)
    {
        return(TMR_ERROR_INVALID_TIMER_ID);
    }

    DBGEXIT1(tmr_error, "CurrentValue of timer= %ld\n", *p_current_value);
    return(tmr_error);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_FreeTimer(t_tmr_id  tmr_id)     		             	*/
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine frees a precedently allocated timer	         	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/* IN :    t_tmr_id   tmr_id    the variable gives the timer ID to free      	*/
/*                                                                           	*/
/* OUT :      	                  	                  	                     	*/
/*                                                                           	*/
/* RETURN: t_tmr_error           : TMR error code. This function will return 	*/
/*                                 TMR_OK or TMR_ERROR_INVALID_TIMER_ID	     	*/
/*                                 if the given timer is invalid             	*/
/*                                                                           	*/
/* COMMENTS: The Timer must be valid. It is possible to free a running timer 	*/
/*           without stopping it					                         	*/
/*------------------------------------------------------------------------------*/
/* REENTRANCY:The function is non-reentrant as it modifies global 				*/
/*											g_tmr_system_context.tmr_state		*/
/*            Hence it has been put into critical section                    	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_FreeTimer(t_tmr_id tmr_id)
{
    t_uint32    index;
    t_tmr_error err_status = TMR_OK;
    t_uint32    mask = 0;
    DBGENTER1("tmr_id %d\n", tmr_id);

    /*check that the timer to free is really busy */
    for (index = 0; index < g_tmr_system_context.number_of_total_timers; index++)
    {
        if (g_tmr_system_context.tmr_mask[index] == tmr_id)
        {
            tmr_EnterCriticalSection(&mask);
            switch (g_tmr_system_context.tmr_state[index])
            {
                case TMR_STATE_FREE:
                case TMR_STATE_BUSY:
                case TMR_STATE_INITIALIZED:
                    break;

                case TMR_STATE_RUNNING:
                    /*stop the running timer*/
                    err_status = TMR_StopTimer(tmr_id);
                    break;

                default:
                    return(TMR_INVALID_PARAMETER);
            }

            g_tmr_system_context.tmr_state[index] = TMR_STATE_FREE;
            tmr_ExitCriticalSection(mask);
            break;
        }
    }

    if (index == g_tmr_system_context.number_of_total_timers)
    {
        err_status = TMR_ERROR_INVALID_TIMER_ID;
    }

    DBGEXIT0(err_status);
    return(err_status);
}

/********************************************************************************/
/* NAME:	t_tmr_error TMR_SetDbgLevel(t_dbg_level dbg_level)          	 	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This routine enables to choose between different debug       	*/
/*              comments levels                                              	*/
/*                                                                           	*/
/* PARAMETERS:                                                               	*/
/* IN :  t_dbg_level dbg_level:          identify TMR debug level            	*/
/*                                                                           	*/
/*                                                                           	*/
/* RETURN:         t_tmr_error :          TMR error code                     	*/
/*                                                                           	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY: NA                                                            	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_SetDbgLevel(t_dbg_level dbg_level)
{
    DBGENTER1("Setting Debug Level to %d", dbg_level);
    /*coverity[self_assign]*/
    dbg_level = dbg_level;  /*for PC LINT*/
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = dbg_level;
#endif
    DBGEXIT0(TMR_OK);
    return(TMR_OK);
}

/********************************************************************************/
/* NAME:	t_tmr_error TMR_GetDbgLevel(t_dbg_level *dbg_level)              	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This routine enables to choose between different debug       	*/
/*              comments levels                                              	*/
/*                                                                           	*/
/* PARAMETERS:                                                               	*/
/* IN :                                                                      	*/
/*                                                                           	*/
/* OUT: t_dbg_level dbg_level:           identify TMR debug level            	*/
/*                                                                           	*/
/* RETURN:        t_tmr_error :           TMR error code                     	*/
/*                                                                           	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY: NA                                                            	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_GetDbgLevel(t_dbg_level *dbg_level)
{
#ifdef __DEBUG
    * dbg_level = myDebugLevel_TMR;
#endif
   /*coverity[self_assign]*/
    dbg_level = dbg_level;  /*for PC LINT*/
    return(TMR_OK);
}

/********************************************************************************/
/* NAME:  t_tmr_error TMR_GetVersion(t_version *p_version)     		         	*/
/*---------------------------------------------------------------------------	*/
/* DESCRIPTION: This routine returns the TMR HCL version		             	*/
/*               		                                                     	*/
/* PARAMETERS:                                                               	*/
/* IN :      	                  	                  	                     	*/
/*                                                                           	*/
/* OUT :    t_version *p_version   The Timer HCL version                     	*/
/*                                                                           	*/
/* RETURN: t_tmr_error                  : TMR error code                     	*/
/*                                                                           	*/
/*---------------------------------------------------------------------------	*/
/* REENTRANCY: NA                                                            	*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_GetVersion(t_version *p_version)
{
    t_tmr_error tmr_error = TMR_OK;

    DBGENTER1("p_version @=%p\n", (void *) p_version);
    p_version->version = TMR_HCL_VERSION_ID;
    p_version->major = TMR_MAJOR_VERSION;
    p_version->minor = TMR_MINOR_VERSION;

    DBGEXIT3
    (
        tmr_error,
        "major=%08d, minor=%08d, version=%08d\n",
        p_version->major,
        p_version->minor,
        p_version->version
    );

    return(tmr_error);
}

/********************************************************************************/
/* NAME:	void TMR_SaveDeviceContext()										*/
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine saves current values of TMR hardware for power 	*/
/*																management.		*/
/*																		        */
/* PARAMETERS:																    */
/* IN			NONE											 	  			*/
/* OUT			NONE															*/
/* RETURN:		void															*/
/*																		        */
/*------------------------------------------------------------------------------*/
/* REENTRANCY: NA   												            */

/********************************************************************************/
PUBLIC void TMR_SaveDeviceContext(void)
{
    DBGENTER0();

    /*UNIT 0*/
    g_tmr_system_context.tmr_device_context[0] = g_tmr_system_context.p_tmr_register[0]->tmr_imsc;
    g_tmr_system_context.tmr_device_context[1] = g_tmr_system_context.p_tmr_register[0]->tmr_load1;
    g_tmr_system_context.tmr_device_context[2] = g_tmr_system_context.p_tmr_register[0]->tmr_value1;
    g_tmr_system_context.tmr_device_context[3] = g_tmr_system_context.p_tmr_register[0]->tmr_control1;
    g_tmr_system_context.tmr_device_context[4] = g_tmr_system_context.p_tmr_register[0]->tmr_bgload1;
    g_tmr_system_context.tmr_device_context[5] = g_tmr_system_context.p_tmr_register[0]->tmr_load2;
    g_tmr_system_context.tmr_device_context[6] = g_tmr_system_context.p_tmr_register[0]->tmr_value2;
    g_tmr_system_context.tmr_device_context[7] = g_tmr_system_context.p_tmr_register[0]->tmr_control2;
    g_tmr_system_context.tmr_device_context[8] = g_tmr_system_context.p_tmr_register[0]->tmr_bgload2;
    g_tmr_system_context.tmr_device_context[9] = g_tmr_system_context.p_tmr_register[0]->tmr_load3;
    g_tmr_system_context.tmr_device_context[10] = g_tmr_system_context.p_tmr_register[0]->tmr_value3;
    g_tmr_system_context.tmr_device_context[11] = g_tmr_system_context.p_tmr_register[0]->tmr_control3;
    g_tmr_system_context.tmr_device_context[12] = g_tmr_system_context.p_tmr_register[0]->tmr_bgload3;
    g_tmr_system_context.tmr_device_context[13] = g_tmr_system_context.p_tmr_register[0]->tmr_load4;
    g_tmr_system_context.tmr_device_context[14] = g_tmr_system_context.p_tmr_register[0]->tmr_value4;
    g_tmr_system_context.tmr_device_context[15] = g_tmr_system_context.p_tmr_register[0]->tmr_control4;
    g_tmr_system_context.tmr_device_context[16] = g_tmr_system_context.p_tmr_register[0]->tmr_bgload4;

    /*UNIT 1*/
    g_tmr_system_context.tmr_device_context[17] = g_tmr_system_context.p_tmr_register[1]->tmr_imsc;
    g_tmr_system_context.tmr_device_context[18] = g_tmr_system_context.p_tmr_register[1]->tmr_load1;
    g_tmr_system_context.tmr_device_context[19] = g_tmr_system_context.p_tmr_register[1]->tmr_value1;
    g_tmr_system_context.tmr_device_context[20] = g_tmr_system_context.p_tmr_register[1]->tmr_control1;
    g_tmr_system_context.tmr_device_context[21] = g_tmr_system_context.p_tmr_register[1]->tmr_bgload1;
    g_tmr_system_context.tmr_device_context[22] = g_tmr_system_context.p_tmr_register[1]->tmr_load2;
    g_tmr_system_context.tmr_device_context[23] = g_tmr_system_context.p_tmr_register[1]->tmr_value2;
    g_tmr_system_context.tmr_device_context[24] = g_tmr_system_context.p_tmr_register[1]->tmr_control2;
    g_tmr_system_context.tmr_device_context[25] = g_tmr_system_context.p_tmr_register[1]->tmr_bgload2;
    g_tmr_system_context.tmr_device_context[26] = g_tmr_system_context.p_tmr_register[1]->tmr_load3;
    g_tmr_system_context.tmr_device_context[27] = g_tmr_system_context.p_tmr_register[1]->tmr_value3;
    g_tmr_system_context.tmr_device_context[28] = g_tmr_system_context.p_tmr_register[1]->tmr_control3;
    g_tmr_system_context.tmr_device_context[29] = g_tmr_system_context.p_tmr_register[1]->tmr_bgload3;
    g_tmr_system_context.tmr_device_context[30] = g_tmr_system_context.p_tmr_register[1]->tmr_load4;
    g_tmr_system_context.tmr_device_context[31] = g_tmr_system_context.p_tmr_register[1]->tmr_value4;
    g_tmr_system_context.tmr_device_context[32] = g_tmr_system_context.p_tmr_register[1]->tmr_control4;
    g_tmr_system_context.tmr_device_context[33] = g_tmr_system_context.p_tmr_register[1]->tmr_bgload4;

    DBGEXIT0(TMR_OK);
}

/********************************************************************************/
/* NAME:	void TMR_RestoreDeviceContext()										*/
/*------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine restore values of TMR hardware.					*/
/*              Counting resumes from where it was halted when request for sleep*/
/*								mode came                                     	*/
/*																		        */
/* PARAMETERS:																    */
/* IN			NONE											 	  			*/
/* OUT			NONE															*/
/* RETURN:		void															*/
/*																		        */
/*------------------------------------------------------------------------------*/
/* REENTRANCY:NA   													            */

/********************************************************************************/
PUBLIC void TMR_RestoreDeviceContext(void)
{
    DBGENTER0();

    /*load the IMSC registers*/
    g_tmr_system_context.p_tmr_register[0]->tmr_imsc = g_tmr_system_context.tmr_device_context[0];
    g_tmr_system_context.p_tmr_register[1]->tmr_imsc = g_tmr_system_context.tmr_device_context[17];

    /*restore the load and Value registers first*/
    g_tmr_system_context.p_tmr_register[0]->tmr_load1 = g_tmr_system_context.tmr_device_context[2];
    g_tmr_system_context.p_tmr_register[0]->tmr_value1 = g_tmr_system_context.tmr_device_context[2];
    g_tmr_system_context.p_tmr_register[0]->tmr_load2 = g_tmr_system_context.tmr_device_context[6];
    g_tmr_system_context.p_tmr_register[0]->tmr_value2 = g_tmr_system_context.tmr_device_context[6];
    g_tmr_system_context.p_tmr_register[0]->tmr_load3 = g_tmr_system_context.tmr_device_context[10];
    g_tmr_system_context.p_tmr_register[0]->tmr_value3 = g_tmr_system_context.tmr_device_context[10];
    g_tmr_system_context.p_tmr_register[0]->tmr_load4 = g_tmr_system_context.tmr_device_context[14];
    g_tmr_system_context.p_tmr_register[0]->tmr_value4 = g_tmr_system_context.tmr_device_context[14];
    g_tmr_system_context.p_tmr_register[1]->tmr_load1 = g_tmr_system_context.tmr_device_context[19];
    g_tmr_system_context.p_tmr_register[1]->tmr_value1 = g_tmr_system_context.tmr_device_context[19];
    g_tmr_system_context.p_tmr_register[1]->tmr_load2 = g_tmr_system_context.tmr_device_context[23];
    g_tmr_system_context.p_tmr_register[1]->tmr_value2 = g_tmr_system_context.tmr_device_context[23];
    g_tmr_system_context.p_tmr_register[1]->tmr_load3 = g_tmr_system_context.tmr_device_context[27];
    g_tmr_system_context.p_tmr_register[1]->tmr_value3 = g_tmr_system_context.tmr_device_context[27];
    g_tmr_system_context.p_tmr_register[1]->tmr_load4 = g_tmr_system_context.tmr_device_context[31];
    g_tmr_system_context.p_tmr_register[1]->tmr_value4 = g_tmr_system_context.tmr_device_context[31];

    /*now restore the control registers*/
    g_tmr_system_context.p_tmr_register[0]->tmr_control1 = g_tmr_system_context.tmr_device_context[3];
    g_tmr_system_context.p_tmr_register[0]->tmr_control2 = g_tmr_system_context.tmr_device_context[7];
    g_tmr_system_context.p_tmr_register[0]->tmr_control3 = g_tmr_system_context.tmr_device_context[11];
    g_tmr_system_context.p_tmr_register[0]->tmr_control4 = g_tmr_system_context.tmr_device_context[15];

    g_tmr_system_context.p_tmr_register[1]->tmr_control1 = g_tmr_system_context.tmr_device_context[20];
    g_tmr_system_context.p_tmr_register[1]->tmr_control2 = g_tmr_system_context.tmr_device_context[24];
    g_tmr_system_context.p_tmr_register[1]->tmr_control3 = g_tmr_system_context.tmr_device_context[28];
    g_tmr_system_context.p_tmr_register[1]->tmr_control4 = g_tmr_system_context.tmr_device_context[32];

    /*Now restore the BG registers*/
    g_tmr_system_context.p_tmr_register[0]->tmr_bgload1 = g_tmr_system_context.tmr_device_context[4];
    g_tmr_system_context.p_tmr_register[0]->tmr_bgload2 = g_tmr_system_context.tmr_device_context[8];
    g_tmr_system_context.p_tmr_register[0]->tmr_bgload3 = g_tmr_system_context.tmr_device_context[12];
    g_tmr_system_context.p_tmr_register[0]->tmr_bgload4 = g_tmr_system_context.tmr_device_context[16];

    g_tmr_system_context.p_tmr_register[1]->tmr_bgload1 = g_tmr_system_context.tmr_device_context[21];
    g_tmr_system_context.p_tmr_register[1]->tmr_bgload2 = g_tmr_system_context.tmr_device_context[25];
    g_tmr_system_context.p_tmr_register[1]->tmr_bgload3 = g_tmr_system_context.tmr_device_context[29];
    g_tmr_system_context.p_tmr_register[1]->tmr_bgload4 = g_tmr_system_context.tmr_device_context[33];

    DBGEXIT0(TMR_OK);
}

/********************************************************************************/
/* NAME:	 TMR_GetIRQSrcStatus()                                              */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   : This function updates a <t_tmr_irq_status> structure         */
/*				  according to the <t_tmr_irq_src> parameter. It allows to      */
/*				  store the reason of the interrupt within the                  */
/*				  <t_tmr_irq_status> parameter.                                 */
/* PARAMETERS    :											                    */
/* IN            : id       : irqsource number                                  */
/*	              p_status :pointer to the structure to be updated              */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : void												            */
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC void TMR_GetIRQSrcStatus(t_tmr_irq_src tmr_irq_src, t_tmr_irq_status *p_status)
{
    t_tmr_irq_src   tmrstat = 0;
    t_tmr_device_id tmr_device_id = TMR_DEVICE_ID_0;

    DBGENTER1("%d", tmr_irq_src);

    tmr_device_id = TMR_GetDeviceID(tmr_irq_src);

    if (TMR_DEVICE_ID_INVALID == tmr_device_id)
    {
        return;
    }

    tmrstat = TMR_GetIRQSrc(tmr_device_id); /* Take care TBC TBD*/

    p_status->pending_irq = tmrstat;        /* (*p_tmr_irq_status)->pending_irq []= tmrstat; */
    p_status->initial_irq = tmrstat;
    p_status->interrupt_state = TMR_IRQ_STATE_NEW;
}

/********************************************************************************/
/* NAME:	 TMR_FilterProcessIRQSrc()                                          */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   :This iterative routine allows to process the device           */
/*              interrupt sources identified through the tmr_error structure.   */
/*				It processes the interrupt sources one by one, updates tmr_error*/
/*               structure and keeps an internal history of the events generated*/
/* PARAMETERS    :											                    */
/* IN            : p_status 		pointer to interrupt tmr_error register     */
/*				  p_event		pointer to store multiple events generated in   */
/*								the current iteration of TMR_FilterProcessIRQSrc*/
/*				  filter_mode   optional parameters which allows to filter      */
/*		                    	the processing and the generation of events     */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  :  												    */
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Non -Reentrant as it modifies global variable                    */
/*			g_tmr_system_context.tmr_state.Hence                                */
/*             it should be called from within a critical section.				*/

/********************************************************************************/
PUBLIC t_tmr_error TMR_FilterProcessIRQSrc
(
    t_tmr_irq_status    *p_tmr_irq_status,
    t_tmr_event         *p_tmr_event,
    t_tmr_filter_mode   filter_mode
)
{
    t_tmr_irq_src   tmr_irq_src = 0;
    t_uint32        temp_filter;

    DBGENTER1("%ld", filter_mode);

    /*t_tmr_irq_status L_status = {TMR_IRQ_STATE_NEW, 0, 0};*/
    if (((p_tmr_irq_status)->interrupt_state == TMR_IRQ_STATE_NEW) && ((p_tmr_irq_status)->pending_irq == 0))
    {
        DBGEXIT0(TMR_NO_PENDING_EVENT_ERROR);
        return(TMR_NO_PENDING_EVENT_ERROR);
    }
    (p_tmr_irq_status)->interrupt_state = TMR_IRQ_STATE_OLD;
    if (filter_mode == TMR_NO_FILTER_MODE)
    {
        temp_filter = (p_tmr_irq_status)->pending_irq;  /*temp_filter = (*p_tmr_irq_status)->EventStatus;*/
        if (0 == temp_filter)
        {
            if (0 == (g_tmr_system_context.tmr_event))
            {
                DBGEXIT0(TMR_NO_MORE_PENDING_EVENT);
                return(TMR_NO_MORE_PENDING_EVENT);
            }
            else
            {
                DBGEXIT0(TMR_INTERNAL_EVENT);
                return(TMR_INTERNAL_EVENT);
            }
        }
    }
    else
    {
        temp_filter = filter_mode & (p_tmr_irq_status)->pending_irq;    /*temp_filter = filter_mode & (*p_tmr_irq_status)->EventStatus;*/
        if (0 == temp_filter)
        {
            if (0 == ((g_tmr_system_context.tmr_event) & filter_mode))
            {
                if (0 == (g_tmr_system_context.tmr_event))
                {
                    DBGEXIT0(TMR_NO_MORE_PENDING_EVENT);
                    return(TMR_NO_MORE_PENDING_EVENT);
                }
                else
                {
                    DBGEXIT0(TMR_NO_MORE_FILTER_PENDING_EVENT);
                    return(TMR_NO_MORE_FILTER_PENDING_EVENT);
                }
            }
            else
            {
                DBGEXIT0(TMR_INTERNAL_EVENT);
                return(TMR_INTERNAL_EVENT);
            }
        }
    }
    {
        t_uint8     count;
        t_uint32    mask = 1;
        for (count = 0; count < g_tmr_system_context.number_of_total_timers; count++)
        {
            if (temp_filter & mask)
            {
                tmr_irq_src = mask;
                break;
            }

            mask = mask << 1;
        }
    }

    /*(*p_tmr_irq_status)->EventStatus &= ~(L_status.EventStatus);*/
    (p_tmr_irq_status)->pending_irq &= ~(tmr_irq_src);

    /*(g_tmr_system_context.tmr_event) |=L_status.EventStatus;*/
    /*assigning the p_tmr_event to the global variable ,it shd b cleared in Acknowledge fn*/
    (g_tmr_system_context.tmr_event) |= tmr_irq_src;

    /* **p_tmr_event=(t_tmr_event)L_status.EventStatus;*/
    *p_tmr_event = (t_tmr_event) tmr_irq_src;

    /*switch(L_status.EventStatus)*/
    TMR_ClearIRQSrc(tmr_irq_src);
    {
        t_uint32    index = 0;

        /* Inline function to reduce complexity */
        index = tmr_TestIrqSrc(tmr_irq_src);

        if (TMR_STATE_RUNNING == g_tmr_system_context.tmr_state[index])
        {
            if (TMR_MODE_ONE_SHOT == g_tmr_system_context.tmr_mode[index])
            {
                t_tmr_register_subset   *p_tmr_register_subset;
                p_tmr_register_subset = g_tmr_system_context.tmr_temp_ptr[index];
                TMR_TIMER_DISABLE;

                /*TMR_HCL_FIELDSET(g_tmr_system_context.p_tmr_register[0]->tmr_control1,  TIMER_DISABLE, 0)*/
                g_tmr_system_context.tmr_state[index] = TMR_STATE_BUSY;
            }
        }
    }

    DBGEXIT0(TMR_REMAINING_PENDING_EVENTS);
    return(TMR_REMAINING_PENDING_EVENTS);
}

/********************************************************************************/
/* NAME:	 TMR_AcknowledgeEvent()                                             */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   :This routine allows to acknowledge the related internal       */
/*				p_tmr_event                                                     */
/* PARAMETERS    :                                                              */
/* IN            : p_event		pointer to acknowledge multiple events generated*/
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : void                                                         */
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant                                                       */

/********************************************************************************/
PUBLIC void TMR_AcknowledgeEvent(const t_tmr_event *p_event)
{
    DBGENTER0();
    if (p_event != NULL)
    {
        g_tmr_system_context.tmr_event &= ~(*p_event);
    }

    DBGEXIT0(TRUE);
}

/********************************************************************************/
/* NAME:	 TMR_IsEventActive()                                                */
/*---------------------------------------------------------------------------   */
/* DESCRIPTION   :This routine allows to check whether the given event is       */
/*				 active or not by checking the global event variable of the HCL.*/
/* PARAMETERS    :											                    */
/* IN            : t_tmr_idevent *p_event:the event for which its status has    */
/*													to be checked               */
/* INOUT         : None                                                         */
/* OUT           : None                                                         */
/* RETURN VALUE  : t_bool	 TRUE if the event is active.						*/
/* TYPE          : Public                                                       */
/*---------------------------------------------------------------------------   */
/* REENTRANCY: Re-entrant														*/

/********************************************************************************/
PUBLIC t_bool TMR_IsEventActive(const t_tmr_irq_src *p_event)
{
    DBGENTER1("(%lx)", (t_uint32) * p_event);
    if (((t_tmr_event) (*p_event) & g_tmr_system_context.tmr_event) != 0)
    {
        DBGEXIT1(TMR_OK, "(%s)", "TRUE");
        return(TRUE);
    }
    else
    {
        DBGEXIT1(TMR_OK, "(%s)", "FALSE");
        return(FALSE);
    }
}

/********************************************************************************/
/*End of PUBLIC functions, start PRIVATE area of functions declaration       	*/

/********************************************************************************/
PRIVATE void tmr_EnterCriticalSection(t_uint32 *maskstat)
{
    *maskstat = 0;
    if (g_tmr_system_context.tmr_initialized[0])
    {
        TMR_DisableIRQSrc(TMR_TIMER_IDS_IN_TMR_UNIT0);
        *maskstat = TMR_TIMER_IDS_IN_TMR_UNIT0;
    }

    if (g_tmr_system_context.tmr_initialized[1])
    {
        TMR_DisableIRQSrc(TMR_TIMER_IDS_IN_TMR_UNIT0);
        *maskstat = TMR_TIMER_IDS_IN_TMR_UNIT0;
    }
}

PRIVATE void tmr_ExitCriticalSection(t_tmr_irq_src tmr_irq_src_maskstat)
{
    if (g_tmr_system_context.tmr_initialized[0])
    {
        TMR_EnableIRQSrc(tmr_irq_src_maskstat & TMR_TIMER_IDS_IN_TMR_UNIT0);
    }

    if (g_tmr_system_context.tmr_initialized[1])
    {
        TMR_EnableIRQSrc(tmr_irq_src_maskstat & TMR_TIMER_IDS_IN_TMR_UNIT1);
    }
}

/* End of file - tmr.c*/


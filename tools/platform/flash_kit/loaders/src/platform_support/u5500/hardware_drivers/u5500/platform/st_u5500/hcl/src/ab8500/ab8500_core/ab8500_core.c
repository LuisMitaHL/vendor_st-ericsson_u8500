/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This file provides generic HAL routines for Ab8500 CORE (U4500)
* \author  ST-Ericsson
*/
/*****************************************************************************/


/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "debug.h"
#include "ab8500_core.h"
#include "ab8500_core_p.h"

extern t_ab8500_core_error SER_AB8500_CORE_Read(t_uint8 bank_addr, t_uint8 register_offset, t_uint32 count, t_uint8 *dummy_data, t_uint8 *data_out);
extern t_ab8500_core_error SER_AB8500_CORE_Write(t_uint8 bank_addr, t_uint8 register_offset, t_uint32 count, t_uint8 *data_in);
/*--------------------------------------------------------------------------*
 * debug stuff                                                              *
 *--------------------------------------------------------------------------*/
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_AB8500_CORE
#define MY_DEBUG_ID             myDebugID_AB8500_CORE
PRIVATE t_dbg_level     MY_DEBUG_LEVEL_VAR_NAME = DEBUG_LEVEL0;
PRIVATE t_dbg_id        MY_DEBUG_ID = AB8500_CORE_HCL_DBG_ID;
#endif

/*--------------------------------------------------------------------------*
 * Private API's                                                            *
 *--------------------------------------------------------------------------*/

/* PRIVATE API: For HCL internal use Only!
 * Parameters: bank_addr      : Register Bank to write to
 *             register_offset: Offset of register within the power block
 *             value          : Value to write in the register
 *             mask           : Mask with value
 */              

PRIVATE t_ab8500_core_error AB8500_CORE_WriteBits(IN t_uint8 bank_addr, IN t_uint8 register_offset, IN t_uint8 value, IN t_uint8 mask)
{
    t_ab8500_core_error error;
    t_uint8 temp_reg=0;    
    t_uint8 dummy_data = 0xFF;
    
    DBGENTER4("BANK Address = %x, Register Offset = %d, data = %x, Mask = %x\r\n", bank_addr, register_offset, value, mask);

    
    error = (t_ab8500_core_error)SER_AB8500_CORE_Read(bank_addr, register_offset, ONE_BYTE, &dummy_data, &temp_reg);
    if(AB8500_CORE_OK != error)
    {
        DBGEXIT(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
        return (AB8500_CORE_TRANSACTION_ON_SPI_FAILED);     
    }

    temp_reg =  (t_uint8)(  (temp_reg & (~mask) ) | (value & mask) );
   
    error = (t_ab8500_core_error)SER_AB8500_CORE_Write(bank_addr, register_offset, ONE_BYTE, &temp_reg);
    if(AB8500_CORE_OK != error)
    {
        DBGEXIT(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
        return (AB8500_CORE_TRANSACTION_ON_SPI_FAILED);     
    }

    DBGEXIT(AB8500_CORE_OK);
    return (AB8500_CORE_OK);
    
}

PRIVATE t_ab8500_core_error Ab8500_CORE_InterpretTurnOnCause(IN t_uint8 reg_value, OUT t_ab8500_core_turnon_cause *p_ab8500_turnon_cause)
{
    DBGENTER2("REG_VALUE = %x, OUT = %p", reg_value, p_ab8500_turnon_cause);
    
    if(reg_value & AB8500_CORE_START_WITH_PORnVBAT)
        *p_ab8500_turnon_cause = AB8500_CORE_TURN_ON_BY_PORnVBAT;
    else if(reg_value & AB8500_CORE_START_WITH_PONKEY1dbF)
        *p_ab8500_turnon_cause = AB8500_CORE_TURN_ON_BY_PONKEY1dbF;
    else if(reg_value & AB8500_CORE_START_WITH_PONKEY2dbF)
        *p_ab8500_turnon_cause = AB8500_CORE_TURN_ON_BY_PONKEY2dbF;
    else if(reg_value & AB8500_CORE_START_WITH_RTCALARM)
        *p_ab8500_turnon_cause = AB8500_CORE_TURN_ON_BY_RTCALARM;
    else if(reg_value & AB8500_CORE_START_WITH_MAIN_CHRG_DET)
        *p_ab8500_turnon_cause = AB8500_CORE_TURN_ON_BY_MAINCHDETECT;
    else if(reg_value & AB8500_CORE_START_WITH_VBUS_DETECT)
        *p_ab8500_turnon_cause = AB8500_CORE_TURN_ON_BY_VBUSDETECTR;    
    else if(reg_value & AB8500_CORE_START_WITH_VUSB_ID_DETECT)
        *p_ab8500_turnon_cause = AB8500_CORE_TURN_ON_BY_USBIDDETECTR;
    else 
        *p_ab8500_turnon_cause = AB8500_CORE_NO_TURNON_CAUSE;
    
    DBGEXIT0(AB8500_CORE_OK);
    return(AB8500_CORE_OK);
    
}

/**********************************************************************************************/
/* NAME:                    AB8500_CORE_GetVersion()                                          */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns version for ab8500 HCL.                                  */
/*                                                                                            */
/* PARAMETERS :                                                                               */
/* IN         : NONE                                                                          */
/* OUT        : t_version*   : p_version    Version for ab8500 HCL                            */
/*                                                                                            */
/* RETURN     : t_ab8500_core_error :            ab8500 error code                            */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Re-entrant                                                                     */
/**********************************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_GetVersion(OUT t_version *p_version)
{
    DBGENTER0();
    if (p_version == NULL)
    {
        DBGEXIT0(AB8500_CORE_INVALID_PARAMETER);
        return(AB8500_CORE_INVALID_PARAMETER);
    }

    p_version->minor = AB8500_CORE_HCL_MINOR_ID;
    p_version->major = AB8500_CORE_HCL_MAJOR_ID;
    p_version->version = AB8500_CORE_HCL_VERSION_ID;

    DBGEXIT0(AB8500_CORE_OK);
    return(AB8500_CORE_OK);
}

/**********************************************************************************************/
/* NAME:     AB8500_CORE_SetDbgLevel()                                                        */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug comments levels        */
/*                                                                                            */
/* PARAMETERS:                                                                                */
/* IN :         t_dbg_level :            DebugLevel from DEBUG_LEVEL0 to DEBUG_LEVEL4.        */
/* OUT:         none                                                                          */
/*                                                                                            */
/* RETURN:        t_ab8500_core_error :            ab8500 error code                          */
/*                                                                                            */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY:    Non-reentrant                                                               */

/**********************************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetDbgLevel(IN t_dbg_level ab8500_dbg_level)
{
    DBGENTER1(" (%x)", ab8500_dbg_level);
#ifdef __DEBUG
    MY_DEBUG_LEVEL_VAR_NAME = ab8500_dbg_level;
#endif
    DBGEXIT0(AB8500_CORE_OK);
    return(AB8500_CORE_OK);
}

/****************************************************************************/
/* NAME        :      AB8500_CORE_GetTurnOnCause()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine gets the Turn On reason of ab8500             */
/* PARAMETERS  :                                                            */
/*         IN  : None                                                       */
/*         OUT : p_ab8500_turnon_cause: defines what caused Turn-On         */
/*                                                                          */
/* RETURN      :    AB8500_CORE_INVALID_PARAMETER        : If OUT parameter */
/*                                                          is NULL         */
/*                  AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or */
/*                                                           Error Occurs   */
/*                  AB8500_CORE_OK                       : If no error is   */
/*                                                               detected   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_GetTurnOnCause(OUT t_ab8500_core_turnon_cause *p_ab8500_turnon_cause)
{
    t_uint8 reg_turn_on=0x0;
    t_ab8500_core_error error;
    t_ab8500_core_error err_status;
    t_uint8 dummy_data = 0xFF;
    
    DBGENTER1("(%p)", p_ab8500_turnon_cause);

    if (NULL != p_ab8500_turnon_cause)
    {

       error = (t_ab8500_core_error)SER_AB8500_CORE_Read(AB8500_CORE_SYSTEM_CTRL1_BANK, AB8500_CORE_TURN_ON_STATUS_REG, ONE_BYTE, &dummy_data, &reg_turn_on);
       if(AB8500_CORE_OK != error)
       {
           DBGEXIT(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
           return (AB8500_CORE_TRANSACTION_ON_SPI_FAILED);     
       }
       
       err_status=Ab8500_CORE_InterpretTurnOnCause(reg_turn_on, p_ab8500_turnon_cause);
    
       DBGEXIT(err_status);
       return (err_status);     
       
    }   /* end if */

    DBGEXIT0(AB8500_CORE_INVALID_PARAMETER);
    return(AB8500_CORE_INVALID_PARAMETER);
}

/****************************************************************************/
/* NAME        :      AB8500_CORE_GetPonKey1PressDuration()                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine gets the duration for which PonKey1 is pressed*/
/* PARAMETERS  :                                                            */
/*         IN  : None                                                       */
/*         OUT : p_msecs: duration of pressure in millisecs                 */
/*                                                                          */
/* RETURN      :     AB8500_CORE_INVALID_PARAMETER       : If OUT parameter */
/*                                                          is NULL         */
/*                  AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or */
/*                                                          Error Occurs    */
/*                  AB8500_CORE_OK                       : If no error is   */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_GetPonKey1PressDuration(OUT t_uint32 *p_msecs)
{
    t_uint8 reg_msecs=0;
    t_ab8500_core_error error;
    t_uint8 dummy_data = 0xFF;
    
    DBGENTER1("(%p)", p_msecs);

    if (NULL != p_msecs)
    {
        error = (t_ab8500_core_error)SER_AB8500_CORE_Read(AB8500_CORE_SYSTEM_CTRL1_BANK, AB8500_CORE_PON_KEYPRESS_STA_REG, 
			                                                                  ONE_BYTE, &dummy_data, &reg_msecs);
        if(AB8500_CORE_OK != error)
        {
            DBGEXIT(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
            return (AB8500_CORE_TRANSACTION_ON_SPI_FAILED);     
        }
    
        *p_msecs = 125*reg_msecs;
	if(AB8500_CORE_PONKEY1_MAX_PRESS_DURATION > *p_msecs)
	{
             *p_msecs = AB8500_CORE_PONKEY1_MAX_PRESS_DURATION;
	}
        	
        DBGEXIT0(AB8500_CORE_OK);
        return(AB8500_CORE_OK);
    }   /* end if */

    DBGEXIT0(AB8500_CORE_INVALID_PARAMETER);
    return(AB8500_CORE_INVALID_PARAMETER);
}

/****************************************************************************/
/* NAME        :      AB8500_CORE_TurnOff()                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Turns-off the STw4500                         */
/* PARAMETERS  :                                                            */
/*         IN  : None                                                       */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_TurnOff(void)
{
    t_ab8500_core_error error;
    DBGENTER0();

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL1_BANK, AB8500_CORE_CTRL1_REG, AB8500_CORE_SWITCHOFF_CMD, AB8500_CORE_SWITCHOFF_CMD);

    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/* NAME        :      AB8500_CORE_ResetPlatform()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Turns-off the MOP/HREF Platform after x secs  */
/*  PARAMETERS :                                                            */
/*         IN  : secs: Seconds till restart                                 */
/*         OUT : None                                                       */
/*                                                                          */
/*      RETURN : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*    COMMENTS : Reset takes place after nearest secs that can be set on H/W*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ResetPlatform(IN t_uint8 secs)
{
    t_uint8 data_buffer =0x0;

    DBGENTER1("(%d)", secs);

    if(secs >= AB8500_CORE_MAIN_WDTMR_MAX_VAL)              /* Max 127 secs duration */
    {
        data_buffer = AB8500_CORE_MAIN_WDTMR_MAX_VAL;
    }
    else
    {
        data_buffer=secs;
    }
	    
    /* write value 0x01 to the MainWDogTimer */        
    if(AB8500_CORE_OK != AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_TMR_REG, data_buffer, MASK_ONE_BYTE) )
    {
        DBGEXIT0(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
        return(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
    }
	      
    /* set WDExpTurnOnValid bit in MainWDogCtrl register */
    if(AB8500_CORE_OK != AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_CTRL_REG, 
				                          AB8500_CORE_WDEXP_TURNONVALID, AB8500_CORE_WDEXP_TURNONVALID) )
    {
        DBGEXIT0(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
        return(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
    }


    /* Enable main watchdog timer */
    /* SET bit[0] to 1 in  MainWDogCtrl register  */
    if(AB8500_CORE_OK != AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_MAIN_WDOG_CTRL_REG, 
				                             AB8500_CORE_MAINWDENABLE|AB8500_CORE_MAINWDKICK, 
							     AB8500_CORE_MAINWDENABLE|AB8500_CORE_MAINWDKICK) )
    {
        DBGEXIT0(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
        return(AB8500_CORE_TRANSACTION_ON_SPI_FAILED);
    }
    
    DBGEXIT0(AB8500_CORE_OK);
    return(AB8500_CORE_OK);
}

/****************************************************************************/
/* NAME        :      AB8500_CORE_ResetRegisters()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Resets the STw4500 Registers                  */
/* PARAMETERS  :                                                            */
/*         IN  : t_ab8500_core_bank: AUDIO/DENC/STw4500                     */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ResetRegisters(IN t_ab8500_core_bank bank)
{
    t_ab8500_core_error error;
    DBGENTER0();

    switch(bank)
    {
         case AB8500_CORE_STW4500_ALL:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL1_BANK, AB8500_CORE_CTRL1_REG, 
			                                 ~AB8500_CORE_RESETn_4500_REG, AB8500_CORE_RESETn_4500_REG);
              break;

         case AB8500_CORE_AUDIO:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_CTRL3_REG, 
			                                 ~AB8500_CORE_RESETn_AUDIO, AB8500_CORE_RESETn_AUDIO);
              break;

         case AB8500_CORE_DENC:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_CTRL3_REG, 
			                                 ~AB8500_CORE_RESETn_DENC, AB8500_CORE_RESETn_DENC);
              break;
  
         default:
              error = AB8500_CORE_INVALID_PARAMETER;
    }

    DBGEXIT0(error);
    return(error);    
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_ReleaseRegistersFromReset()               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine releases the STw4500 Registers from Reset     */
/* PARAMETERS  :                                                            */
/*         IN  : t_ab8500_core_bank: AUDIO/DENC/STw4500                     */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ReleaseRegistersFromReset(IN t_ab8500_core_bank bank)
{
    t_ab8500_core_error error;
    DBGENTER0();

    switch(bank)
    {
         case AB8500_CORE_STW4500_ALL:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL1_BANK, AB8500_CORE_CTRL1_REG, 
			                                 AB8500_CORE_RESETn_4500_REG, AB8500_CORE_RESETn_4500_REG);
              break;
	      
         case AB8500_CORE_AUDIO:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_CTRL3_REG, 
			                                 AB8500_CORE_RESETn_AUDIO, AB8500_CORE_RESETn_AUDIO);
              break;
	      
         case AB8500_CORE_DENC:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_CTRL3_REG, 
			                                 AB8500_CORE_RESETn_DENC, AB8500_CORE_RESETn_DENC);
              break;

	 default:
              error = AB8500_CORE_INVALID_PARAMETER;
    }	 

    DBGEXIT0(error);
    return(error);    
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_Clk32kOut2Control()                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine enables/disables CLK32kOut2 O/P buffer        */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_clk32kout2_ctrl: Enable/Disable CLK32kOut2     */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_Clk32kOut2Control(IN t_ab8500_core_clk32kout2_ctrl ab8500_core_clk32kout2_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER1(" Clk32kOut2 Control Enable/Disable = %d\r\n", (t_uint8)ab8500_core_clk32kout2_ctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_CTRL3_REG,
		                       ( (t_uint8)ab8500_core_clk32kout2_ctrl<<sbAB8500_CORE_CLK32_OUT2_DIS), AB8500_CORE_CLK32_OUT2_DIS);
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SetClkStrength()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine defines the ULPCLK/CLK27MHz output digital    */
/*               buffer strength                                            */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_clk_strength: Strength set to 0/1              */
/*             : ab8500_core_clk_str     : Clock to set: ULPCLK/CLK27MHz    */ 
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If CLK is invalid           */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetClkStrength(IN t_ab8500_core_clk_strength ab8500_core_clk_strength, 
		                                        IN t_ab8500_core_clk_str ab8500_core_clk_str  )
{
    t_ab8500_core_error error;
    DBGENTER2("ULPCLK/CLK27MHz select = %d, O/P digital Buffer strength (0/1) = %d\r\n", ab8500_core_clk_str, ab8500_core_clk_strength);

    switch(ab8500_core_clk_str)
    {
        case AB8500_CORE_ULPCLK:
                  error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CONF_REG, 
				                 (t_uint8)ab8500_core_clk_strength<<sbAB8500_CORE_ULPCLK_STRENGTH, AB8500_CORE_ULPCLK_STRENGTH);
                  break;

        case AB8500_CORE_CLK27MHZ:
                  error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CONF_REG, 
				                 (t_uint8)ab8500_core_clk_strength<<sbAB8500_CORE_CLK_27MHZ_STRENGTH, AB8500_CORE_CLK_27MHZ_STRENGTH);
                  break;

        default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }

    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_ControlTvoutClkDelay()                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine controls the STw4500 TVoutClk Delay           */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_tvout_clk_delay: Enable/Disable Delay          */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ControlTvoutClkDelay(IN t_ab8500_core_tvout_clk_delay ab8500_core_tvout_clk_delay)
{
    t_ab8500_core_error error;
    DBGENTER1(" No Delay = %d\r\n", (t_uint8)ab8500_core_tvout_clk_delay);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CONF_REG,
		                  ( (t_uint8)ab8500_core_tvout_clk_delay<<sbAB8500_CORE_TVOUT_CLK_DELAY), AB8500_CORE_TVOUT_CLK_DELAY);
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_ControlTvoutClkInv()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine controls the STw4500 TVout Inversion          */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_tvout_clk_inv: Enable/Disable TVoutClk Inv     */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ControlTvoutClkInv(IN t_ab8500_core_tvout_clk_inv ab8500_core_tvout_clk_inv)
{
    t_ab8500_core_error error;
    DBGENTER1(" Inversion = %d\r\n", (t_uint8)ab8500_core_tvout_clk_inv);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CONF_REG,
		                   ( (t_uint8)ab8500_core_tvout_clk_inv<<sbAB8500_CORE_TVOUT_CLK_INV), AB8500_CORE_TVOUT_CLK_INV);
    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/* NAME        :      AB8500_CORE_ConfigClk27Mhz()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Configures the CLK27MHz Buffer & PD Direction */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_clk27_ctrl: CLK27MHz O/p Buffer Disable/Enable */
/*               ab8500_core_clk27_pud : Pulldown Disable/Enable            */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigClk27Mhz(IN t_ab8500_core_clk27_ctrl ab8500_core_clk27_ctrl, 
		                                  IN t_ab8500_core_clk27_pud ab8500_core_clk27_pud)
{
    t_ab8500_core_error error;
    DBGENTER2(" CLK27Mhz O/P Buf disable/Enable = %d, Clk27MHz Pulldown disabled/enabled = %d\r\n", ab8500_core_clk27_ctrl, ab8500_core_clk27_pud);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CONF_REG,
	       ( ((t_uint8)ab8500_core_clk27_ctrl<<sbAB8500_CORE_CLK27MHZ_BUF_ENA) | ((t_uint8)ab8500_core_clk27_pud<<sbAB8500_CORE_CLK27MHZ_PD_ENA) ), 
				    (AB8500_CORE_CLK27MHZ_BUF_ENA | AB8500_CORE_CLK27MHZ_PD_ENA) );
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_ConfigULPClkPad()                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Configures the ULPCLK Pad                     */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_ulpclk_conf: Inactive/Input/Output             */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigULPClkPad(IN t_ab8500_core_ulpclk_conf ab8500_core_ulpclk_conf)
{
    t_ab8500_core_error error;
    DBGENTER1(" ULP CLK CONF Struct = %x\r\n", ab8500_core_ulpclk_conf);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CONF_REG,
		                       ((t_uint8)ab8500_core_ulpclk_conf<<sbAB8500_CORE_ULPCLK_CONF), AB8500_CORE_ULPCLK_CONF);
    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_ConfigSysUlpClkReq()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Configures SysClkREq, UlpClkReq and internal  */
/*               switching between sysclk & ulpclk                          */
/* PARAMETERS  :                                                            */
/*         IN  : t_ab8500_core_sysulpclkreq: config structure               */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigSysUlpClkReq(IN t_ab8500_core_sysulpclkreq ab8500_core_sysulpclkreq)
{
    t_ab8500_core_error error;
    DBGENTER1(" Sys ULP CLK Req Struct = %p\r\n", &ab8500_core_sysulpclkreq);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CTRL1_REG,
	                                (     ((t_uint8)(ab8500_core_sysulpclkreq.sysclkreq)<<sbAB8500_CORE_SYSCLK_REQ) 
					    | ((t_uint8)(ab8500_core_sysulpclkreq.ulpclkreq)<<sbAB8500_CORE_ULPCLK_REQ)
					    | ((t_uint8)(ab8500_core_sysulpclkreq.sysulpintclksel)<<sbAB8500_CORE_SYSULPCLK_INTSEL)), 
					     (AB8500_CORE_SYSCLK_REQ|AB8500_CORE_ULPCLK_REQ|AB8500_CORE_SYSULPCLK_INTSEL));
    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_EnableInternalClk()                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine enables the internal clock of TVOUT/AUDIO/USB */
/* PARAMETERS  :                                                            */
/*         IN  : t_ab8500_core_device_clk: USB/AUDIO/TVOUT internal clk sel */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If device is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_EnableInternalClk(IN t_ab8500_core_device_clk ab8500_core_device_clk)
{
    t_ab8500_core_error error;
    DBGENTER1("DEVICE %d \r\n", ab8500_core_device_clk);

    switch(ab8500_core_device_clk)
    {
        case AB8500_CORE_TVOUT_INTERNAL_CLK:
                  error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKCTRL_REG, 
				                     AB8500_CORE_TVOUT_CLK_ENA, AB8500_CORE_TVOUT_CLK_ENA );
                  break;

        case AB8500_CORE_USB_INTERNAL_CLK:
                  error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKCTRL_REG, 
				                     AB8500_CORE_USB_CLK_ENA, AB8500_CORE_USB_CLK_ENA );
                  break;

        case AB8500_CORE_AUDIO_INTERNAL_CLK:
                  error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CTRL1_REG, 
		                                  AB8500_CORE_AUDIO_CLK_ENABLE, AB8500_CORE_AUDIO_CLK_ENABLE); 
                  break;
  
        default:
                  error = AB8500_CORE_INVALID_PARAMETER;
    }
    
    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_DisableInternalClk()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine disables the internal clock of TVOUT/AUDIO/USB*/
/* PARAMETERS  :                                                            */
/*         IN  : t_ab8500_core_device_clk: USB/AUDIO/TVOUT internal clk sel */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If device is invalid        */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_DisableInternalClk(IN t_ab8500_core_device_clk ab8500_core_device_clk)
{
    t_ab8500_core_error error;
    DBGENTER1("DEVICE %d\r\n", ab8500_core_device_clk);

    switch(ab8500_core_device_clk)
    {
        case AB8500_CORE_TVOUT_INTERNAL_CLK:
                  error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKCTRL_REG, 
				                     ~AB8500_CORE_TVOUT_CLK_ENA, AB8500_CORE_TVOUT_CLK_ENA );
                  break;

        case AB8500_CORE_USB_INTERNAL_CLK:
                  error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKCTRL_REG, 
				                     ~AB8500_CORE_USB_CLK_ENA, AB8500_CORE_USB_CLK_ENA );
                  break;

        case AB8500_CORE_AUDIO_INTERNAL_CLK:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CTRL1_REG, 
		                                  ~AB8500_CORE_AUDIO_CLK_ENABLE, AB8500_CORE_AUDIO_CLK_ENABLE); 
                  break;
  
        default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }
    
    DBGEXIT0(error);
    return(error);
}




/****************************************************************************/
/* NAME        :      AB8500_CORE_ControlTvoutPll()                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Enables-Disables the STw4500 TVout PLL        */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_pll_control: Enable/Disable TVout PLL          */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ControlTvoutPll(IN t_ab8500_core_pll_control ab8500_core_pll_control)
{
    t_ab8500_core_error error;
    DBGENTER1(" Enable TVOUT PLL = %d\r\n", (t_uint8)ab8500_core_pll_control);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKCTRL_REG,
		                 ((t_uint8)ab8500_core_pll_control<<sbAB8500_CORE_TVOUT_PLL_ENA), AB8500_CORE_TVOUT_PLL_ENA);
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_ConfigInt8500n()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine configures INT8500n pin as push-pull o/p      */
/*               or open-drain o/p                                          */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_int8500n_conf: Push-Pull/Open-Drain O/P        */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigInt8500n(IN t_ab8500_core_int8500n_conf ab8500_core_int8500n_conf)
{
    t_ab8500_core_error error;
    DBGENTER1(" INT8500n Config = %d\r\n", (t_uint8)ab8500_core_int8500n_conf);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCTRL_SPARE_REG,
		                 ((t_uint8)ab8500_core_int8500n_conf<<sbAB8500_CORE_INT8500N), AB8500_CORE_INT8500N);
    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_ConfigSysClkReq1Input()                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine configures ULP/APE SysClkReq1valid request    */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_sysclkreq1_input: no-request/request           */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigSysClkReq1Input(IN t_ab8500_core_sysclkreq1_input ab8500_core_sysclkreq1_input)
{
    t_ab8500_core_error error;
    DBGENTER1(" SysClkReq1 I/P = %p\r\n", &ab8500_core_sysclkreq1_input);

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ1_VALID_REG,
		                 (  (t_uint8)(ab8500_core_sysclkreq1_input.req_ulpclk)<<sbAB8500_CORE_ULPCLKREQ1_VALID 
				  | (t_uint8)(ab8500_core_sysclkreq1_input.req_sysclk)<<sbAB8500_CORE_SYSCLKREQ1_VALID), 
				    (AB8500_CORE_ULPCLKREQ1_VALID|AB8500_CORE_SYSCLKREQ1_VALID) );
    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_ConfigSysClkReqRfClkBuf()                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine to be defined in V1                           */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_pll_control: Enable/Disable TVout PLL          */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigSysClkReqRfClkBuf(IN t_ab8500_core_sysclkreq_sel ab8500_core_sysclkreq_sel, 
		                                               IN t_ab8500_core_rfclkbuf_conf ab8500_core_rfclkbuf_conf)
{
    t_ab8500_core_error error;
    DBGENTER2(" SysClkReq Select = %p, RfClkBufConf Struct = %p\r\n", &ab8500_core_sysclkreq_sel, &ab8500_core_rfclkbuf_conf);

    switch(ab8500_core_sysclkreq_sel)
    {
         case AB8500_CORE_SYSCLKREQ_1:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ1_RFCLKBUF_REG,
		                 (   ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_2)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2) 
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_3)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3)
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_4)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4)), 
	                             (AB8500_CORE_SYSCLKREQ_RFCLKBUF_2|AB8500_CORE_SYSCLKREQ_RFCLKBUF_3|AB8500_CORE_SYSCLKREQ_RFCLKBUF_4) );
              break;

         case AB8500_CORE_SYSCLKREQ_2:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ2_RFCLKBUF_REG,
		                 (   ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_2)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2) 
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_3)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3)
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_4)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4)), 
	                             (AB8500_CORE_SYSCLKREQ_RFCLKBUF_2|AB8500_CORE_SYSCLKREQ_RFCLKBUF_3|AB8500_CORE_SYSCLKREQ_RFCLKBUF_4) );
              break;


         case AB8500_CORE_SYSCLKREQ_3:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ3_RFCLKBUF_REG,
		                 (   ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_2)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2) 
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_3)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3)
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_4)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4)), 
	                             (AB8500_CORE_SYSCLKREQ_RFCLKBUF_2|AB8500_CORE_SYSCLKREQ_RFCLKBUF_3|AB8500_CORE_SYSCLKREQ_RFCLKBUF_4) );
              break;


         case AB8500_CORE_SYSCLKREQ_4:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ4_RFCLKBUF_REG,
		                 (   ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_2)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2) 
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_3)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3)
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_4)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4)), 
	                             (AB8500_CORE_SYSCLKREQ_RFCLKBUF_2|AB8500_CORE_SYSCLKREQ_RFCLKBUF_3|AB8500_CORE_SYSCLKREQ_RFCLKBUF_4) );
              break;


         case AB8500_CORE_SYSCLKREQ_5:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ5_RFCLKBUF_REG,
		                 (   ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_2)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2) 
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_3)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3)
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_4)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4)), 
	                             (AB8500_CORE_SYSCLKREQ_RFCLKBUF_2|AB8500_CORE_SYSCLKREQ_RFCLKBUF_3|AB8500_CORE_SYSCLKREQ_RFCLKBUF_4) );
              break;


         case AB8500_CORE_SYSCLKREQ_6:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ6_RFCLKBUF_REG,
		                 (   ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_2)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2) 
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_3)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3)
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_4)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4)), 
	                             (AB8500_CORE_SYSCLKREQ_RFCLKBUF_2|AB8500_CORE_SYSCLKREQ_RFCLKBUF_3|AB8500_CORE_SYSCLKREQ_RFCLKBUF_4) );
              break;


         case AB8500_CORE_SYSCLKREQ_7:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ7_RFCLKBUF_REG,
		                 (   ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_2)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2) 
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_3)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3)
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_4)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4)), 
	                             (AB8500_CORE_SYSCLKREQ_RFCLKBUF_2|AB8500_CORE_SYSCLKREQ_RFCLKBUF_3|AB8500_CORE_SYSCLKREQ_RFCLKBUF_4) );
              break;


         case AB8500_CORE_SYSCLKREQ_8:
              error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKREQ8_RFCLKBUF_REG,
		                 (   ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_2)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_2) 
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_3)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_3)
				   | ((t_uint8)(ab8500_core_rfclkbuf_conf.clk_buf_4)<<sbAB8500_CORE_SYSCLKREQ_RFCLKBUF_4)), 
	                             (AB8500_CORE_SYSCLKREQ_RFCLKBUF_2|AB8500_CORE_SYSCLKREQ_RFCLKBUF_3|AB8500_CORE_SYSCLKREQ_RFCLKBUF_4) );
              break;

	 default:
	      error=AB8500_CORE_INVALID_PARAMETER;
    
    }


    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SwatControl()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Enables-Disables the Swat control             */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_swat_ctrl: Disable/Enable SWAT                 */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SwatControl(IN t_ab8500_core_swat_ctrl ab8500_core_swat_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER1(" Swat Disable/Enable = %x\r\n", (t_uint8)ab8500_core_swat_ctrl);

    error = AB8500_CORE_WriteBits( AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SWAT_CTRL_REG,
     		                     ((t_uint8)ab8500_core_swat_ctrl<<sbAB8500_CORE_SWAT_ENABLE), AB8500_CORE_SWAT_ENABLE );

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SelLeviDieselRFIC()                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine selects LEVI or DIESEL as the RF IC           */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_levidiesel_sel: LEVI/DIESEl Select             */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SelLeviDieselRFIC(IN t_ab8500_core_levidiesel_sel ab8500_core_levidiesel_sel)
{
    t_ab8500_core_error error;
    DBGENTER1("RF IC Levi/Diesel = %x\r\n", (t_uint8)ab8500_core_levidiesel_sel);

    error = AB8500_CORE_WriteBits( AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SWAT_CTRL_REG,
     		                     ((t_uint8)ab8500_core_levidiesel_sel<<sbAB8500_CORE_LEVI_DIESEL_SEL), AB8500_CORE_LEVI_DIESEL_SEL );

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SelRfOffTmr()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets the RF-Off timer                         */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_rfoff_period: timer period                     */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SelRfOffTmr(IN t_uint8 ab8500_core_rfoff_period)
{
    t_ab8500_core_error error;
    DBGENTER1("RF Off TMR Period = %x\r\n", (t_uint8)ab8500_core_rfoff_period);

    error = AB8500_CORE_WriteBits( AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SWAT_CTRL_REG,
     		                     (t_uint8)( (ab8500_core_rfoff_period & 0x07)<<sbAB8500_CORE_RF_OFF_TMR), AB8500_CORE_RF_OFF_TMR );

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVintCore12Voltage()                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets the VintCore12 output Voltage            */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vintcore12_voltage: Voltage                    */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVintCore12Voltage(IN t_ab8500_core_vintcore12_voltage ab8500_core_vintcore12_voltage)
{
    t_ab8500_core_error error;
    DBGENTER1("VintCore12 Voltage = %x\r\n", ab8500_core_vintcore12_voltage);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_REGU_MISC1_REG,
		                  (t_uint8)ab8500_core_vintcore12_voltage<<sbAB8500_CORE_VINTCORE12_SEL, AB8500_CORE_VINTCORE12_SEL);
   
    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_VintCore12VTVoutControl()                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine enables/Disables the VTVout/VintCore12        */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vintvtvout_select: Vint/VTVout Select          */
/*               ab8500_core_vintvtvout_control: Disable/Enable             */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_VintVTVoutControl(IN t_ab8500_core_vintvtvout_select ab8500_core_vintvtvout_select, 
                                                     IN t_ab8500_core_vintvtvout_control ab8500_core_vintvtvout_control)
{
    t_ab8500_core_error error;
    DBGENTER2("Vint/VTVout  = %d, Disable/enable = %d\r\n", ab8500_core_vintvtvout_select, ab8500_core_vintvtvout_control);

    switch(ab8500_core_vintvtvout_select)
    {
        case AB8500_CORE_VINTCORE12_SELECT:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_REGU_MISC1_REG, 
				                 ( (t_uint8)ab8500_core_vintvtvout_control<<sbAB8500_CORE_VINT_VCORE12_ENA), AB8500_CORE_VINT_VCORE12_ENA);
                  break;

        case AB8500_CORE_VTVOUT_SELECT:
                  error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_REGU_MISC1_REG, 
				                 ( (t_uint8)ab8500_core_vintvtvout_control<<sbAB8500_CORE_VTVOUT_ENA), AB8500_CORE_VTVOUT_ENA);
                  break;

        default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }

    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_ForceVintToLP()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Enables/Disables the Vint to LP when enabled  */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vint_lp_select: Disable/Enable LP              */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ForceVintToLP(IN t_ab8500_core_vint_lp_select ab8500_core_vint_lp_select)
{
    t_ab8500_core_error error;
    DBGENTER1("Vint LP Disable/Enable  = %d\r\n", ab8500_core_vint_lp_select);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_REGU_MISC1_REG, 
		                   (t_uint8)ab8500_core_vint_lp_select<<sbAB8500_CORE_VINTCORE12_LP, AB8500_CORE_VINTCORE12_LP);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlVaudio()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Enables/Disables the Vaudio Supply            */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vaudio_supplyctrl: Disable/Enable              */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVaudio(IN t_ab8500_core_vaudio_hs_mic_supplyctrl ab8500_core_vaudio_supplyctrl)
{
    t_ab8500_core_error error;
    DBGENTER1("Vaudio disable/enable  = %d\r\n", ab8500_core_vaudio_supplyctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_VAUDIO_SUPPLY_REG, 
		                   (t_uint8)ab8500_core_vaudio_supplyctrl<<sbAB8500_CORE_VAUDIO_ENA, AB8500_CORE_VAUDIO_ENA);

    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlVdmic()                               */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Enables/Disables the Vdmic supply             */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vdmic_supplyctrl: Disable/Enable               */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVdmic(IN t_ab8500_core_vaudio_hs_mic_supplyctrl ab8500_core_vdmic_supplyctrl)
{
    t_ab8500_core_error error;
    DBGENTER1("Vdmic disable/enable  = %d\r\n", ab8500_core_vdmic_supplyctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_VAUDIO_SUPPLY_REG, 
		                   (t_uint8)ab8500_core_vdmic_supplyctrl<<sbAB8500_CORE_VDMIC_ENA, AB8500_CORE_VDMIC_ENA);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlVamic1()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Enables/Disables the Vamic1 supply            */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vamic1_supplyctrl: Disable/Enable              */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVamic1(IN t_ab8500_core_vaudio_hs_mic_supplyctrl ab8500_core_vamic1_supplyctrl)
{
    t_ab8500_core_error error;
    DBGENTER1("Vamic1 disable/enable  = %d\r\n", ab8500_core_vamic1_supplyctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_VAUDIO_SUPPLY_REG, 
		                   (t_uint8)ab8500_core_vamic1_supplyctrl<<sbAB8500_CORE_VAMIC1_ENA, AB8500_CORE_VAMIC1_ENA);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlVamic2()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Enables/Disables the Vamic2 supply            */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vamic2_supplyctrl: Disable/Enable              */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVamic2(IN t_ab8500_core_vaudio_hs_mic_supplyctrl ab8500_core_vamic2_supplyctrl)
{
    t_ab8500_core_error error;
    DBGENTER1("Vamic2 disable/enable  = %d\r\n", ab8500_core_vamic2_supplyctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_VAUDIO_SUPPLY_REG, 
		                   (t_uint8)ab8500_core_vamic2_supplyctrl<<sbAB8500_CORE_VAMIC2_ENA, AB8500_CORE_VAMIC2_ENA);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlVamicZout()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets the Vamic1&2 o/p impedance when disabled */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vamic_zout: High/Grounded                      */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVamicZout(IN t_ab8500_core_vamic_zout ab8500_core_vamic_zout)
{
    t_ab8500_core_error error;
    DBGENTER1("Vamic Zout struct  = %p\r\n", &ab8500_core_vamic_zout);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_REGCTRL1_SPARE_REG, 
		                   ( ((t_uint8)(ab8500_core_vamic_zout.vamic1_zout)<<sbAB8500_CORE_VAMIC1_DZOUT)|
				     ((t_uint8)(ab8500_core_vamic_zout.vamic2_zout)<<sbAB8500_CORE_VAMIC2_DZOUT)  ), 
				      (sbAB8500_CORE_VAMIC1_DZOUT|sbAB8500_CORE_VAMIC2_DZOUT) );

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlTVLoadRC()                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine configures TVLoad as R or R+C                 */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_tvload_rc: 750Ohm/&50Ohm+Capacitance           */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlTVLoadRC(IN t_ab8500_core_tvload_rc ab8500_core_tvload_rc)
{
    t_ab8500_core_error error;
    DBGENTER1("TVLOAD R/R+C  = %x\r\n", ab8500_core_tvload_rc);

    error = AB8500_CORE_WriteBits(AB8500_CORE_TVOUT_BANK, AB8500_CORE_TVOUT_CTRL_REG, 
		                   (t_uint8)ab8500_core_tvload_rc<<sbAB8500_CORE_TVLOAD_RC, AB8500_CORE_TVLOAD_RC);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlPlugTV()                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Enables/Disables the TVPlug detection         */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_plugtv_ctrl: Disable/Enable TVPlug detection   */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlPlugTV(IN t_ab8500_core_plugtv_ctrl ab8500_core_plugtv_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER1("Plug Tv detection Disabled/Enabled = %x\r\n", ab8500_core_plugtv_ctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_TVOUT_BANK, AB8500_CORE_TVOUT_CTRL_REG, 
		                   (t_uint8)ab8500_core_plugtv_ctrl<<sbAB8500_CORE_PLUG_TVON, AB8500_CORE_PLUG_TVON);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlTVoutDAC()                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine configures the TvOut DAC                      */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_tvout_dac_ctr: config struct                   */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlTVoutDAC(IN t_ab8500_core_tvout_dac_ctrl ab8500_core_tvout_dac_ctr)
{
    t_ab8500_core_error error;
    DBGENTER1("TvOut DAC Disabled/Enabled = %x\r\n", ab8500_core_tvout_dac_ctr);

    error = AB8500_CORE_WriteBits(AB8500_CORE_TVOUT_BANK, AB8500_CORE_TVOUT_CTRL_REG, 
		                   (t_uint8)ab8500_core_tvout_dac_ctr<<sbAB8500_CORE_TVOUT_DACCTRL_1_0, AB8500_CORE_TVOUT_DACCTRL_1_0);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SetPlugTVTime()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets the PlugTV Time                          */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_plugtv_time: Time                              */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetPlugTVTime(IN t_ab8500_core_plugtv_time ab8500_core_plugtv_time)
{
    t_ab8500_core_error error;
    DBGENTER1("Plug TV Time = %x\r\n", ab8500_core_plugtv_time);

    error = AB8500_CORE_WriteBits(AB8500_CORE_TVOUT_BANK, AB8500_CORE_TVOUT_CTRL_REG, 
		                   (t_uint8)ab8500_core_plugtv_time<<sbAB8500_CORE_PLUG_TV_TIME, AB8500_CORE_PLUG_TV_TIME);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVoltReguPWMMode()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine enables/Disables the PWM mode for Varm, Vape  */
/*               Vsmps1, Vsmps2, Vsmps3                                     */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_pwm_regu_sel: Varm/Vape/Vsmps1/Vsmps2/Vsmps3   */
/*               ab8500_core_pwm_ctrl    : Disable/Enable PWM output        */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVoltReguPWMMode(IN t_ab8500_core_regu_sel ab8500_core_pwm_regu_sel, 
                                                          IN t_ab8500_core_pwm_ctrl  ab8500_core_pwm_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER2("Voltage Regulator  = %d, Disable/enable = %d\r\n", ab8500_core_pwm_regu_sel, ab8500_core_pwm_ctrl);

    switch(ab8500_core_pwm_regu_sel)
    {
        case AB8500_CORE_REGU_VARM_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_ARM_REGU1_REG, 
				                 ( (t_uint8)ab8500_core_pwm_ctrl<<sbAB8500_CORE_VARM_PWM_MODE), AB8500_CORE_VARM_PWM_MODE);
                  break;

        case AB8500_CORE_REGU_VAPE_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAPE_REGU_REG, 
				                 ( (t_uint8)ab8500_core_pwm_ctrl<<sbAB8500_CORE_VAPE_PWM_MODE), AB8500_CORE_VAPE_PWM_MODE);
                  break;

        case AB8500_CORE_REGU_VSMPS1_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS1_REGU_REG, 
				                 ( (t_uint8)ab8500_core_pwm_ctrl<<sbAB8500_CORE_VSMPS_PWM_MODE), AB8500_CORE_VSMPS_PWM_MODE);
                  break;

        case AB8500_CORE_REGU_VSMPS2_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS2_REGU_REG, 
				                 ( (t_uint8)ab8500_core_pwm_ctrl<<sbAB8500_CORE_VSMPS_PWM_MODE), AB8500_CORE_VSMPS_PWM_MODE);
                  break;

        case AB8500_CORE_REGU_VSMPS3_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS3_REGU_REG, 
				                 ( (t_uint8)ab8500_core_pwm_ctrl<<sbAB8500_CORE_VSMPS_PWM_MODE), AB8500_CORE_VSMPS_PWM_MODE);
                  break;

        default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }

    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVoltReguAUTOMode()                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine enables/Disables the Auto mode for Varm, Vape */
/*               Vsmps1, Vsmps2, Vsmps3                                     */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_auto_regu_sel: Varm/Vape/Vsmps1/Vsmps2/Vsmps3  */
/*               ab8500_core_auto_ctrl    : Disable/Enable Auto             */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVoltReguAUTOMode(IN t_ab8500_core_regu_sel ab8500_core_auto_regu_sel, 
                                                          IN t_ab8500_core_auto_ctrl  ab8500_core_auto_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER2("Voltage Regulator  = %d, Disable/enable = %d\r\n", ab8500_core_auto_regu_sel, ab8500_core_auto_ctrl);

    switch(ab8500_core_auto_regu_sel)
    {
        case AB8500_CORE_REGU_VARM_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_ARM_REGU1_REG, 
				                 ( (t_uint8)ab8500_core_auto_ctrl<<sbAB8500_CORE_VARM_AUTO_MODE), AB8500_CORE_VARM_AUTO_MODE);
                  break;

        case AB8500_CORE_REGU_VAPE_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAPE_REGU_REG, 
				                 ( (t_uint8)ab8500_core_auto_ctrl<<sbAB8500_CORE_VAPE_AUTO_MODE), AB8500_CORE_VAPE_AUTO_MODE);
                  break;

        case AB8500_CORE_REGU_VSMPS1_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS1_REGU_REG, 
				                 ( (t_uint8)ab8500_core_auto_ctrl<<sbAB8500_CORE_VSMPS_AUTO_MODE), AB8500_CORE_VSMPS_AUTO_MODE);
                  break;

        case AB8500_CORE_REGU_VSMPS2_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS2_REGU_REG, 
				                 ( (t_uint8)ab8500_core_auto_ctrl<<sbAB8500_CORE_VSMPS_AUTO_MODE), AB8500_CORE_VSMPS_AUTO_MODE);
                  break;

        case AB8500_CORE_REGU_VSMPS3_SEL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS3_REGU_REG, 
				                 ( (t_uint8)ab8500_core_auto_ctrl<<sbAB8500_CORE_VSMPS_AUTO_MODE), AB8500_CORE_VSMPS_AUTO_MODE);
                  break;

        default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_ConfigVoltReguMode()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine configs the regulator mode for Varm/Vape/     */
/*               Vsmps1/Vsmps2/Vsmps3/Vbbn/Vbbp/Vpll/Vana/VextSupply1/      */
/*               VextSupply2/VextSupply3/Vaux1/Vaux2/Vaux3/Vrf1             */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_regu_mode_sel: Voltage domain to set           */
/*               ab8500_core_regu_mode: Mode set as Disable/HP/HW or HP/LP  */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_ConfigVoltReguMode(IN t_ab8500_core_regu_mode_sel ab8500_core_regu_mode_sel, 
                                                          IN t_ab8500_core_regu_mode  ab8500_core_regu_mode)
{
    t_ab8500_core_error error=AB8500_CORE_OK;
    DBGENTER2("Voltage Regulator  = %d, Disable/enable = %d\r\n", ab8500_core_regu_mode_sel, ab8500_core_regu_mode);

    switch(ab8500_core_regu_mode_sel)
    {
        case AB8500_CORE_REGU_VARM:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_ARM_REGU1_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VARM_REGU), AB8500_CORE_VARM_REGU);
                  break;

        case AB8500_CORE_REGU_VAPE:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAPE_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VAPE_REGU), AB8500_CORE_VAPE_REGU);
                  break;

        case AB8500_CORE_REGU_VSMPS1:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS1_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VSMPS_REGU), AB8500_CORE_VSMPS_REGU);
                  break;

        case AB8500_CORE_REGU_VSMPS2:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS2_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VSMPS_REGU), AB8500_CORE_VSMPS_REGU);
                  break;

        case AB8500_CORE_REGU_VSMPS3:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS3_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VSMPS_REGU), AB8500_CORE_VSMPS_REGU);
                  break;

        case AB8500_CORE_REGU_VBBN:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_ARM_REGU2_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VBBN_REGU), sbAB8500_CORE_VBBN_REGU);
                  break;

        case AB8500_CORE_REGU_VBBP:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_ARM_REGU2_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VBBP_REGU), AB8500_CORE_VBBP_REGU);
                  break;

        case AB8500_CORE_REGU_VPLL:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VPLL_VANA_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VPLL_REGU), AB8500_CORE_VPLL_REGU);
                  break;

        case AB8500_CORE_REGU_VANA:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VPLL_VANA_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VANA_REGU), AB8500_CORE_VANA_REGU);
                  break;


        case AB8500_CORE_REGU_VEXT_SUPPLY_1:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_EXT_SUPPLY_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VEXT_SUPPLY_1_REGU), AB8500_CORE_VEXT_SUPPLY_1_REGU);
                  break;


        case AB8500_CORE_REGU_VEXT_SUPPLY_2:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_EXT_SUPPLY_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VEXT_SUPPLY_2_REGU), AB8500_CORE_VEXT_SUPPLY_2_REGU);
                  break;


        case AB8500_CORE_REGU_VEXT_SUPPLY_3:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_EXT_SUPPLY_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VEXT_SUPPLY_3_REGU), AB8500_CORE_VEXT_SUPPLY_3_REGU);
                  break;


        case AB8500_CORE_REGU_VAUX_1:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAUX12_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VAUX_1_REGU), AB8500_CORE_VAUX_1_REGU);
                  break;


        case AB8500_CORE_REGU_VAUX_2:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAUX12_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VAUX_2_REGU), AB8500_CORE_VAUX_2_REGU);
                  break;		  
		  
        case AB8500_CORE_REGU_VAUX_3:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_RF1_VAUX3_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VAUX_3_REGU), AB8500_CORE_VAUX_3_REGU);
                  break;

        case AB8500_CORE_REGU_VRF_1:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_RF1_VAUX3_REGU_REG, 
				                 ( (t_uint8)ab8500_core_regu_mode<<sbAB8500_CORE_VRF_1_REGU), AB8500_CORE_VRF_1_REGU);
                  break;

	default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }

    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_SetArmVoltCtrlReg()                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine selects Varm voltage control register from    */
/*               Varmsel1/VarmSel2/VarmSel3                                 */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_arm_volt_ctrl_reg: Select the register         */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetArmVoltCtrlReg(IN t_ab8500_core_volt_ctrl_reg  ab8500_core_arm_volt_ctrl_reg)
{
    t_ab8500_core_error error;
    DBGENTER1("Voltage Ctrl by VarmSel1/VarmSel2/VarmSel3  = %d\r\n", ab8500_core_arm_volt_ctrl_reg);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_ARM_REGU1_REG, 
				                 ( (t_uint8)ab8500_core_arm_volt_ctrl_reg<<sbAB8500_CORE_VARM_CTRL_SEL), AB8500_CORE_VARM_CTRL_SEL);
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlVrefDDR()                             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine Disables/Enables VrefDDR                      */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vref_ddr_ctrl: Disable/Enable                  */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVrefDDR(IN t_ab8500_core_vref_ddr_ctrl  ab8500_core_vref_ddr_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER1("VrefDDR Disable/Enable  = %d\r\n", ab8500_core_vref_ddr_ctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VREF_DDR_REG, 
				                 ( (t_uint8)ab8500_core_vref_ddr_ctrl<<sbAB8500_CORE_VREF_DDR_ENA), AB8500_CORE_VREF_DDR_ENA);
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_CtrlVrefDDRSleepMode()                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine enables/Disables the VrefDDr Sleep Mode       */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vref_ddr_ctrl: Disable/Enable sleep mode       */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_CtrlVrefDDRSleepMode(IN t_ab8500_core_vref_ddr_ctrl  ab8500_core_vref_ddr_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER1("VrefDDR Disable/Enable  = %d\r\n", ab8500_core_vref_ddr_ctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VREF_DDR_REG, 
  		                   ( (t_uint8)ab8500_core_vref_ddr_ctrl<<sbAB8500_CORE_VREF_DDR_SLEEPMODE), AB8500_CORE_VREF_DDR_SLEEPMODE);
    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVarmVoltage()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine set Varm Voltage                              */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_volt_ctrl_reg: Varm1/Varm2/Varm3 register sel  */
/*               ab8500_core_varm_voltage : Voltage to set                  */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVarmVoltage(IN t_ab8500_core_volt_ctrl_reg  ab8500_core_volt_ctrl_reg,
		                                      IN t_uint8                      ab8500_core_varm_voltage)
{
    t_ab8500_core_error error;
    DBGENTER2("Ab8500 Reg=%d, Varm=%d\r\n", ab8500_core_volt_ctrl_reg, ab8500_core_varm_voltage);

    switch(ab8500_core_volt_ctrl_reg)
    {
	    case AB8500_CORE_VOLT_CTRL_SEL1_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VARM_SEL1_REG, 
  		                   (ab8500_core_varm_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                 break;

	    case AB8500_CORE_VOLT_CTRL_SEL2_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VARM_SEL2_REG, 
  		                   (ab8500_core_varm_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                 break;

	    case AB8500_CORE_VOLT_CTRL_SEL3_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VARM_SEL3_REG, 
  		                   (ab8500_core_varm_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                 break;

            default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }

    DBGEXIT0(error);
    return(error);    
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVapeVoltage()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets Vape Voltage                             */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_volt_ctrl_reg: Vape1/Vape2 register select     */
/*               ab8500_core_vape_voltage : Voltage to set                  */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVapeVoltage(IN t_ab8500_core_volt_ctrl_reg  ab8500_core_volt_ctrl_reg,
		                                      IN t_uint8                      ab8500_core_vape_voltage)
{
    t_ab8500_core_error error;
    DBGENTER2("Ab8500 Reg=%d, Vape=%d\r\n", ab8500_core_volt_ctrl_reg, ab8500_core_vape_voltage);

    switch(ab8500_core_volt_ctrl_reg)
    {
	    case AB8500_CORE_VOLT_CTRL_SEL1_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAPE_SEL1_REG, 
  		                   (ab8500_core_vape_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                 break;

	    case AB8500_CORE_VOLT_CTRL_SEL2_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAPE_SEL2_REG, 
  		                   (ab8500_core_vape_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
		 break;

	    case AB8500_CORE_VOLT_CTRL_SEL3_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAPE_SEL3_REG, 
  		                   (ab8500_core_vape_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
		 break;

            default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }

    DBGEXIT0(error);
    return(error);    
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVauxVoltage()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets Vaux Voltage                             */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vaux         : Vaux1/Vaux2/Vaux3 register sel  */
/*               ab8500_core_vaux_voltage : Voltage to set                  */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVauxVoltage(IN t_ab8500_core_vaux  ab8500_core_vaux,
		                                      IN t_uint8             ab8500_core_vaux_voltage)
{
    t_ab8500_core_error error;
    DBGENTER2("Vuax%d, Volt=%d\r\n", ab8500_core_vaux, ab8500_core_vaux_voltage);

    switch(ab8500_core_vaux)
    {
	    case AB8500_CORE_VAUX_1:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAUX1_SEL_REG, 
  		                   (ab8500_core_vaux_voltage<<sbAB8500_CORE_VAUX_SEL), AB8500_CORE_VAUX_SEL);
		 break;

	    case AB8500_CORE_VAUX_2:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAUX2_SEL_REG, 
  		                   (ab8500_core_vaux_voltage<<sbAB8500_CORE_VAUX_SEL), AB8500_CORE_VAUX_SEL);
		 break;

	    case AB8500_CORE_VAUX_3:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VRF1_VAUX3_SEL_REG, 
  		                   (ab8500_core_vaux_voltage<<sbAB8500_CORE_VAUX_SEL), AB8500_CORE_VAUX_SEL);
		 break;

            default:
                  error= AB8500_CORE_INVALID_PARAMETER;

    }		

    DBGEXIT0(error);
    return(error); 
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVbbpVoltage()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets Vbbp voltage                             */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_volt_ctrl_reg: Vbbp1/Vbbp2 select              */
/*               ab8500_core_vbbp_voltage: Voltage to set                   */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVbbpVoltage(IN t_ab8500_core_volt_ctrl_reg  ab8500_core_volt_ctrl_reg,
		                                      IN t_uint8                      ab8500_core_vbbp_voltage)
{
    t_ab8500_core_error error;
    DBGENTER2("Ab8500 Reg=%d, Vbbp=%d\r\n", ab8500_core_volt_ctrl_reg, ab8500_core_vbbp_voltage);

    switch(ab8500_core_volt_ctrl_reg)
    {
	    case AB8500_CORE_VOLT_CTRL_SEL1_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VBB_SEL1_REG, 
  		                   (ab8500_core_vbbp_voltage<<sbAB8500_CORE_VBBP_SEL)&0xFF, AB8500_CORE_VBBP_SEL);
		 break;

	    case AB8500_CORE_VOLT_CTRL_SEL2_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VBB_SEL2_REG, 
  		                   (ab8500_core_vbbp_voltage<<sbAB8500_CORE_VBBP_SEL)&0xFF, AB8500_CORE_VBBP_SEL);
		 break;

	    case AB8500_CORE_VOLT_CTRL_SEL3_REG:
	    default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }		

    DBGEXIT0(error);
    return(error);
}



/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVbbnVoltage()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets Vbbn voltage                             */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_volt_ctrl_reg: Vbbn1/Vbbn2 select              */
/*               ab8500_core_vbbn_voltage: Voltage to set                   */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid       */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVbbnVoltage(IN t_ab8500_core_volt_ctrl_reg  ab8500_core_volt_ctrl_reg,
		                                      IN t_uint8                      ab8500_core_vbbn_voltage)
{
    t_ab8500_core_error error;
    DBGENTER2("Ab8500 Reg=%d, Vbbn=%d\r\n", ab8500_core_volt_ctrl_reg, ab8500_core_vbbn_voltage);

    switch(ab8500_core_volt_ctrl_reg)
    {
	    case AB8500_CORE_VOLT_CTRL_SEL1_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VBB_SEL1_REG, 
  		                   (ab8500_core_vbbn_voltage<<sbAB8500_CORE_VBBN_SEL), AB8500_CORE_VBBN_SEL);
		 break;
		 
	    case AB8500_CORE_VOLT_CTRL_SEL2_REG:
                 error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VBB_SEL2_REG, 
  		                   (ab8500_core_vbbn_voltage<<sbAB8500_CORE_VBBN_SEL), AB8500_CORE_VBBN_SEL);
		 break;

	    case AB8500_CORE_VOLT_CTRL_SEL3_REG:
            default:
                  error = AB8500_CORE_INVALID_PARAMETER;

    }		

    DBGEXIT0(error);
    return(error); 
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVrf1Voltage()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets Vrf1 Voltage                             */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vrf1_voltage: voltage to set                   */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVrf1Voltage(IN t_uint8 ab8500_core_vrf1_voltage)
{
    t_ab8500_core_error error;
    DBGENTER1("VRF1=%d\r\n", ab8500_core_vrf1_voltage);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VRF1_VAUX3_SEL_REG, 
  		                   (ab8500_core_vrf1_voltage<<sbAB8500_CORE_VRF1_SEL)&0xFF, AB8500_CORE_VRF1_SEL);
    DBGEXIT0(error);
    return(error);

}



/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVsmpsVoltage()                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine sets the Vsmps1/Vsmps2/Vsmps3 voltage         */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_smps_sel      : Vsmps1/Vsmps2/Vsmps3 Select    */
/*               ab8500_core_smps_voltage  : Voltage to set                 */
/*               ab8500_core_volt_ctrl_reg :VsmpsSel1/VsmpsSel2/VsmpsSel3   */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If select is invalid        */
/*             : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVsmpsVoltage(IN t_ab8500_core_smps_sel ab8500_core_smps_sel,
		               IN t_ab8500_core_volt_ctrl_reg  ab8500_core_volt_ctrl_reg, IN t_uint8 ab8500_core_smps_voltage)
{
    t_ab8500_core_error error=AB8500_CORE_OK;
    DBGENTER3("SMPS = %d, Ab8500 Reg=%d, Vasmps=%d\r\n", ab8500_core_smps_sel, ab8500_core_volt_ctrl_reg, ab8500_core_smps_voltage);

    switch(ab8500_core_smps_sel)
    {
         case AB8500_CORE_SMPS_1:
	 {
   	      switch(ab8500_core_volt_ctrl_reg)
              {
	          case AB8500_CORE_VOLT_CTRL_SEL1_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS1_SEL1_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

	          case AB8500_CORE_VOLT_CTRL_SEL2_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS1_SEL2_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

                  case AB8500_CORE_VOLT_CTRL_SEL3_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS1_SEL3_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

                  default:
                     error = AB8500_CORE_INVALID_PARAMETER;
	      }
	      break;
	 }

         case AB8500_CORE_SMPS_2:
	 {
   	      switch(ab8500_core_volt_ctrl_reg)
              {
	          case AB8500_CORE_VOLT_CTRL_SEL1_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS2_SEL1_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

	          case AB8500_CORE_VOLT_CTRL_SEL2_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS2_SEL2_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

                  case AB8500_CORE_VOLT_CTRL_SEL3_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS2_SEL3_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

                  default:
                     error = AB8500_CORE_INVALID_PARAMETER;
	      }
	      break;	      
	 }

         case AB8500_CORE_SMPS_3:
	 {
   	      switch(ab8500_core_volt_ctrl_reg)
              {
	          case AB8500_CORE_VOLT_CTRL_SEL1_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS3_SEL1_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

	          case AB8500_CORE_VOLT_CTRL_SEL2_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS3_SEL2_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

                  case AB8500_CORE_VOLT_CTRL_SEL3_REG:
                     error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS3_SEL3_REG, 
  		                   (ab8500_core_smps_voltage<<sbAB8500_CORE_VARM_APE_SMPS_SEL), AB8500_CORE_VARM_APE_SMPS_SEL);
                     break;

                  default:
                     error = AB8500_CORE_INVALID_PARAMETER;
	      }
	      break;
	 }

         default:
              error = AB8500_CORE_INVALID_PARAMETER;
   
    }
  
    DBGEXIT0(error);
    return(error);    

}

/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVapeCtrlReg()                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine selects Vape voltage control register from    */
/*               Vapesel1/VapeSel2                                          */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_ape_volt_ctrl_reg: Select the register         */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVapeCtrlReg(IN t_ab8500_core_volt_ctrl_reg  ab8500_core_ape_volt_ctrl_reg)
{
    t_ab8500_core_error error=AB8500_CORE_ERROR;
    DBGENTER1("Voltage Ctrl by VapeSel1/VapeSel2/VapeSel3  = %d\r\n", ab8500_core_ape_volt_ctrl_reg);

    if(AB8500_CORE_VOLT_CTRL_SEL3_REG == ab8500_core_ape_volt_ctrl_reg)
    {
        error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAPE_REGU_REG, 
				                  (t_uint8)AB8500_CORE_VAPE3_CTRL_SEL, AB8500_CORE_VAPE3_CTRL_SEL);
    }
    else
    {
        error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VAPE_REGU_REG, 
				                 ( (t_uint8)ab8500_core_ape_volt_ctrl_reg<<sbAB8500_CORE_VAPE_CTRL_SEL), 
						 (AB8500_CORE_VAPE_CTRL_SEL | AB8500_CORE_VAPE3_CTRL_SEL));
    }

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVSmpsCtrlReg()                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine selects Vsmps1/2/3 voltage control register   */
/*               as VsmpsSel1/VsmpsSel2/VsmpsSel3                           */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_smps: Vsmps1/Vsmps2/Vsmps3                     */
/*               ab8500_core_smps_volt_ctrl_reg: Select Reg 1/2/3           */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVSmpsCtrlReg(IN t_ab8500_core_smps_sel       ab8500_core_smps, 
		                                      IN t_ab8500_core_volt_ctrl_reg  ab8500_core_smps_volt_ctrl_reg)
{
    t_ab8500_core_error error;
    DBGENTER2("Voltage Ctrl for SMPS%d by Sel1/Sel2/Sel3  = %d\r\n", ab8500_core_smps, ab8500_core_smps_volt_ctrl_reg);

    switch(ab8500_core_smps)
    {
         case AB8500_CORE_SMPS_1: 
              error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS1_REGU_REG, 
				            ( (t_uint8)ab8500_core_smps_volt_ctrl_reg<<sbAB8500_CORE_VSMPS_CTRL_SEL), AB8500_CORE_VSMPS_CTRL_SEL);
	      break;

         case AB8500_CORE_SMPS_2: 
              error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS2_REGU_REG, 
				            ( (t_uint8)ab8500_core_smps_volt_ctrl_reg<<sbAB8500_CORE_VSMPS_CTRL_SEL), AB8500_CORE_VSMPS_CTRL_SEL);
	      break;

         case AB8500_CORE_SMPS_3: 
              error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_VSMPS3_REGU_REG, 
				            ( (t_uint8)ab8500_core_smps_volt_ctrl_reg<<sbAB8500_CORE_VSMPS_CTRL_SEL), AB8500_CORE_VSMPS_CTRL_SEL);
	      break;      
      
	 default: 
                 
	      error = AB8500_CORE_INVALID_PARAMETER;
    }

    DBGEXIT0(error);
    return(error);
	      
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_SetVBBCtrlReg()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine selects Vbbp & Vbbn voltage control register  */
/*               as Varmsel1/VarmSel2/VarmSel3                              */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_vbb_volt_ctrl_reg: Select register             */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_SetVBBCtrlReg(IN t_ab8500_core_vbbvolt_ctrl_reg  ab8500_core_vbb_volt_ctrl_reg)
{
    t_ab8500_core_error error;
    DBGENTER1("VBB Select Struct   = %p\r\n", &ab8500_core_vbb_volt_ctrl_reg);

    if( (AB8500_CORE_VOLT_CTRL_SEL3_REG==ab8500_core_vbb_volt_ctrl_reg.vbbp_sel_ctrl) || 
        (AB8500_CORE_VOLT_CTRL_SEL3_REG==ab8500_core_vbb_volt_ctrl_reg.vbbn_sel_ctrl)    )
    {
         DBGEXIT0(AB8500_CORE_INVALID_PARAMETER);
         return(AB8500_CORE_INVALID_PARAMETER);
    }

    error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_ARM_REGU1_REG, 
				     ( (t_uint8)(ab8500_core_vbb_volt_ctrl_reg.vbbp_sel_ctrl)<<sbAB8500_CORE_VBBP_SEL_CTRL), AB8500_CORE_VBBP_SEL_CTRL);
    if(AB8500_CORE_OK != error)
    {
         DBGEXIT(error);
         return (error);     
    }

    error = AB8500_CORE_WriteBits(AB8500_CORE_REG_CTRL2_SIM_BANK, AB8500_CORE_ARM_REGU1_REG, 
				     ( (t_uint8)(ab8500_core_vbb_volt_ctrl_reg.vbbn_sel_ctrl)<<sbAB8500_CORE_VBBN_SEL_CTRL), AB8500_CORE_VBBN_SEL_CTRL);

    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_USBOTGSupplyCtrl()                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine controls the OTG Supply (Disable/Enable)      */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_ctrl: Enable/Disable OTG Supply                */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_USBOTGSupplyCtrl(IN t_ab8500_core_ctrl ab8500_core_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER1(" Off/ON = %d\r\n", (t_uint32)ab8500_core_ctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_OTG_SUPPLY_CTRL_REG,
		                  ( (t_uint8)ab8500_core_ctrl<<sbAB8500_CORE_OTG_SUPPLY_ENA), AB8500_CORE_OTG_SUPPLY_ENA);
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_USBOTGIDDeviceCtrl()                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine controls the STw4500 OTG Device ID Detection  */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_ctrl: Enable/Disable Device ID detection       */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_USBOTGIDDeviceCtrl(IN t_ab8500_core_ctrl ab8500_core_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER1(" Off/ON = %d\r\n", (t_uint32)ab8500_core_ctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_USB_BANK, AB8500_CORE_USB_OTG_CTRL_REG,
		                  ( (t_uint8)ab8500_core_ctrl<<sbAB8500_CORE_ID_DEV_ENA), AB8500_CORE_ID_DEV_ENA);
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_USBOTGIDHostCtrl()                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine controls the STw4500 OTG Host ID Detection    */
/* PARAMETERS  :                                                            */
/*         IN  : ab8500_core_ctrl: Enable/Disable Host ID detection         */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_TRANSACTION_ON_SPI_FAILED: If read fails or    */
/*                                                          Error Occurs    */
/*               AB8500_CORE_OK                       : If no error is      */
/*                                                          detected        */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_USBOTGIDHostCtrl(IN t_ab8500_core_ctrl ab8500_core_ctrl)
{
    t_ab8500_core_error error;
    DBGENTER1(" Off/ON = %d\r\n", (t_uint32)ab8500_core_ctrl);

    error = AB8500_CORE_WriteBits(AB8500_CORE_USB_BANK, AB8500_CORE_USB_OTG_CTRL_REG,
		                  ( (t_uint8)ab8500_core_ctrl<<sbAB8500_CORE_ID_HOST_ENA), AB8500_CORE_ID_HOST_ENA);
    DBGEXIT0(error);
    return(error);
}

/****************************************************************************/
/* NAME        :      AB8500_CORE_TvoutDefInit()                            */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine enables TVout with default & optimal config   */
/* PARAMETERS  :                                                            */
/*         IN  : None                                                       */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : AB8500_CORE_INVALID_PARAMETER: If TVOut mode is invalid    */
/*             : ERROR CODE: If Error Occurs                                */
/*             : AB8500_CORE_OK: If no error is detected                    */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_TvoutDefInit(void)
{
    t_ab8500_core_error error;
    DBGENTER0();

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSCLKCTRL_REG, (AB8500_CORE_TVOUT_PLL_ENA|AB8500_CORE_TVOUT_CLK_ENA), 
		                           (AB8500_CORE_TVOUT_PLL_ENA|AB8500_CORE_TVOUT_CLK_ENA) ); /* Enable TVout PLL and TVOUT ClK 27MHz */
    if(AB8500_CORE_OK != error)
    {
          DBGEXIT(error);
          return (error);     
    }

    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_REGU_MISC1_REG, AB8500_CORE_VTVOUT_ENA, 
		                           AB8500_CORE_VTVOUT_ENA );                                 /* Enable VTVout Supply */
    if(AB8500_CORE_OK != error)
    {
          DBGEXIT(error);
          return (error);     
    }
    
    /* Release DENC Registers from RESET */
    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_CTRL3_REG, AB8500_CORE_RESETn_DENC, AB8500_CORE_RESETn_DENC);
  
    DBGEXIT0(error);
    return(error);
}


/****************************************************************************/
/* NAME        :      AB8500_CORE_AudioDefInit()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION : This routine enables Audio with default & optimal config   */
/* PARAMETERS  :                                                            */
/*         IN  : None                                                       */
/*         OUT : None                                                       */
/*                                                                          */
/* RETURN      : ERROR CODE: If Error Occurs                                */
/*             : AB8500_CORE_OK: If no error is detected                   */
/*--------------------------------------------------------------------------*/
/* REENTRANCY       : Non Re-entrant                                        */
/* REENTRANCY ISSUES:                                                       */
/****************************************************************************/
PUBLIC t_ab8500_core_error AB8500_CORE_AudioDefInit(void)
{
    t_ab8500_core_error error;
    DBGENTER0();

    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_CTRL3_REG, 
		                                 AB8500_CORE_RESETn_AUDIO, AB8500_CORE_RESETn_AUDIO); /* Release AUDIO Registers From RESET */
    if(AB8500_CORE_OK != error)
    {
          DBGEXIT(error);
          return (error);     
    }
    
    error = AB8500_CORE_WriteBits(AB8500_CORE_SYSTEM_CTRL2_BANK, AB8500_CORE_SYSULPCLK_CTRL1_REG, 
		                                  AB8500_CORE_AUDIO_CLK_ENABLE, AB8500_CORE_AUDIO_CLK_ENABLE); /* Enable AUDIO Internal Clock */
    if(AB8500_CORE_OK != error)
    {
         DBGEXIT(error);
         return (error);     
    }
  
    /* VintVcore12 enabled */ 
    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_REGU_MISC1_REG, AB8500_CORE_VINT_VCORE12_ENA, AB8500_CORE_VINT_VCORE12_ENA);
    if(AB8500_CORE_OK != error)
    {
         DBGEXIT(error);
         return (error);     
    }

    /* Vaudio Enabled */ 
    error = AB8500_CORE_WriteBits(AB8500_CORE_REGULATOR_CTRL1_BANK, AB8500_CORE_VAUDIO_SUPPLY_REG, AB8500_CORE_VAUDIO_ENA, AB8500_CORE_VAUDIO_ENA);

    DBGEXIT0(error);
    return(error);

}


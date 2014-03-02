/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This provides services for STM initialization
* \author  ST-Ericsson
*/
/*****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "hcl_defs.h"
#include "memory_mapping.h"
#include "stm_services.h"

#ifndef __PEPS_8500
#include "gpio.h"
#endif


t_uint32 stm_message_counter=0;

/*--------------------------------------------------------------------------*
 * Public functions														    *
 *--------------------------------------------------------------------------*/



/**********************************************************************************************/
/* NAME:	t_stm_error SER_STM_initialize()	        		     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to initialize the STM GPIO pins			              */
/* PARAMETERS:																                  */
/* IN : 		t_stm_select_sxa  device: Select the SXA device,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
 
PUBLIC t_stm_error SER_STM_initialize(void)
{
    
    t_stm_error   error_stm = STM_OK;

    #ifndef __PEPS_8500
    //volatile t_uint32 *pTemp;
    t_gpio_error  error_gpio; 
	t_gpio_config config;
    
	config.mode = GPIO_MODE_ALT_FUNCTION_C;
	config.trig = GPIO_TRIG_BOTH_EDGES;
	config.direction = GPIO_DIR_OUTPUT;
	config.debounce = GPIO_DEBOUNCE_UNCHANGED;
	config.level = GPIO_LEVEL_LEAVE_UNCHANGED;
    #endif


#if 1
    #ifndef __PEPS_8500
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_70,config);
	if(error_gpio != GPIO_OK)	
		PRINT("GPIO 70 Enable Failed !!");
	
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_71,config);
    if(error_gpio != GPIO_OK)
		PRINT("GPIO 71 Enable Failed !!");
	
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_72,config);
	if(error_gpio != GPIO_OK)
		PRINT("GPIO 72 Enable Failed !!");
	

	error_gpio = GPIO_SetPinConfig(GPIO_PIN_73,config);
	if(error_gpio != GPIO_OK)
		PRINT("GPIO 73 Enable Failed !!");
	
	error_gpio = GPIO_SetPinConfig(GPIO_PIN_74,config);
	if(error_gpio != GPIO_OK)
		PRINT("GPIO 74 Enable Failed !!");
	#endif
    (*(volatile t_uint32 *) (PRCMU_REG_BASE_ADDR + 0x250)) |= 0x0F;
//	(*(volatile t_uint32 *) (PRCMU_REG_BASE_ADDR + 0x138)) |= (0x1 << 11);

#endif
    return(error_stm);
}


/**********************************************************************************************/
/* NAME:	t_stm_error SER_STM_SendString()		    		     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send string through STM 				              */
/* PARAMETERS:																                  */
/* IN : 		t_uint32  channel_no: The 0 - 255 number of the channel,                      */
/*              t_uint8  *data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/

PUBLIC t_stm_error SER_STM_SendString
(
IN t_uint32          channel_no,
IN t_uint8           *data
)
{
    t_stm_error stm_error = STM_OK;
    /* Load the status of STM transmit buffer */
    
    /* Send the command to enable string messages through STM */
    //STM_CMD_STRING(channel_no);
	stm_error = STM_SendMessage8(STM_ACTIVE_CHANNEL,0x20);
	if(stm_error != STM_OK)
	{
		stm_error = STM_INTERNAL_ERROR;
		return(stm_error);
	}

    while(*data)
    {
        stm_error = STM_SendMessage8(channel_no,*data);
		data++;
        if(stm_error != STM_OK)
        {
            stm_error = STM_INTERNAL_ERROR;
            return(stm_error);
        }
	
    }


    /* Send the final byte of the string */
    stm_error = STM_SendMessage8(channel_no,0x00);
    if(stm_error != STM_OK)
        {
            stm_error = STM_INTERNAL_ERROR;
            return(stm_error);
        }

    /* End the STM channel */
    //STM_CMD_END_OF_CHANNEL(channel_no);
//	STM_CMD_END_OF_CHANNEL(250);

   PRINT("Sending Passed");
    return(stm_error);
}

/****************************************************************************/
/* NAME:    SER_STM_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize stm		                            */
/* PARAMETERS: void                                                         */
/* RETURN: void                                                             */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_STM_Init(t_uint8 mask)
{
	t_stm_error   error_stm;
	t_stm_configuration stm_config;
	t_stm_ter  stm_ter;
    STM_SetBaseAddress(STM_CTRL_REG_BASE_ADDR);
    
    if(STM_Init(STM_CTRL_REG_BASE_ADDR) != STM_OK)
    {
    	PRINT("register map failed. ");
    }
#if 1
    error_stm = SER_STM_initialize();
    if(error_stm != STM_OK)
    {
        PRINT("SER_STM_initialise Failed");
    }
#endif   
	// Configure STM clock
	error_stm = STM_ConfigureFrequency(STM_CLKDIVBY06);
    if(error_stm != STM_OK)
		PRINT("STM Clock setting FAILED !!");

    STM_SetInitiatorBaseAddress((t_stm_initiator *)STM_CPU_REG_BASE_ADDR);

    /* Select the Master that has to be enabled */
	stm_ter.stm_te0 = TRUE;    /* Enable Master 0 */
    stm_ter.stm_te2 = TRUE;    /* Enable Master 2 */
	stm_ter.stm_te3 = TRUE;    /* Enable Master 3 */
	stm_ter.stm_te4 = TRUE;    /* Enable Master 4 */
	stm_ter.stm_te5 = TRUE;    /* Enable Master 5 */
	stm_ter.stm_te9 = TRUE;    /* Enable Master 9 */

	stm_config.stm_plp0 = STM_LOWEST_PRIORITY;
	stm_config.stm_plp1 = STM_LOWEST_PRIORITY_WITH_PLP0;
    stm_config.stm_dwng = STM_4_BIT_DATA_OUTPUT;
    error_stm = STM_Configuration(stm_config);
	if(error_stm != STM_OK)
		PRINT("STM priority and frequency setting FAILED !!");
    error_stm = STM_TraceEnable(stm_ter);
    if(error_stm != STM_OK)
		PRINT("STM trace enable FAILED !!");
        
}


/**********************************************************************************************/
/* NAME:	t_stm_error SER_STM_Send8()			        		     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send byte through STM 		    		              */
/* PARAMETERS:																                  */
/* IN : 		t_uint8  data: The data that is to be written ,                               */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/
PUBLIC t_stm_error SER_STM_Send8(t_uint8 data)
{
    t_stm_error   error_stm;
    error_stm = STM_OK;

    /* Ensure \n and \r are not passed */
     if ((data != 0x0a)&&(data != 0x0d))
         {
             PRINT("The device is Initialized");
             //STM_CMD_STRING(STM_ACTIVE_CHANNEL);
			 error_stm = STM_SendMessage8(STM_ACTIVE_CHANNEL,0x20);
			  if(error_stm != STM_OK)
			  {
				  error_stm = STM_INTERNAL_ERROR;
				  return(error_stm);
			  }

             
             /* Send the data through STM */
             PRINT("Send data through STM ");
             error_stm = STM_SendMessage8(STM_ACTIVE_CHANNEL,data);
             if (STM_OK != error_stm)
             {
	             PRINT ("STM error : %d",error_stm); 
                 return(error_stm);
             }
             else
             {
                 PRINT (" Message send ") ;
             }

             data = 0x00;/* End of data transmit */

             PRINT("Send data through STM ");
             error_stm = STM_SendMessage8(STM_ACTIVE_CHANNEL,data);
             if (STM_OK != error_stm)
             {
	             PRINT ("STM error : %d",error_stm); 
                 return(error_stm);
             }
             else
             {
                 PRINT (" Message send ") ;
             }
             PRINT("End of STM Channel");
             STM_CMD_END_OF_CHANNEL(STM_ACTIVE_CHANNEL);
         }
     return(error_stm);
}

/**********************************************************************************************/
/* NAME:	t_stm_error SER_STM_String()    	        		     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send string through STM 				              */
/* PARAMETERS:																                  */
/* IN : 		t_uint8  *data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/

PUBLIC t_stm_error SER_STM_SendStringMsg(t_uint8 *data)
{
    t_stm_error   error_stm;
    t_uint8 convert[MAX_DEBUG_STR];
	t_uint32 index=0;
    error_stm = STM_OK;

    while( (*data) && (index < MAX_DEBUG_STR-1))
	{
		convert[index] = (t_uint8)*data++;
		index++;
	}
	convert[index] = '\0'; /* Append the string with end of string character.*/

	//error_stm = SER_STM_SendString(STM_ACTIVE_CHANNEL, convert);
	error_stm = SER_STM_SendString(224, convert);
    if (STM_OK != error_stm)
             {
	             PRINT ("STM error : %d",error_stm); 
                 return(error_stm);
             }
             else
             {
                 PRINT (" Message send ") ;
             }

     return(error_stm);
}


 
/**********************************************************************************************/
/* NAME:	t_stm_error SER_STM_Close()      	        		     		     			  */
/*--------------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to send string through STM 				              */
/* PARAMETERS:																                  */
/* IN : 		t_uint8  *data: The data that is to be written ,                              */
/* OUT:         None                                                                          */
/* RETURN:		t_stm_error : STM_OK                          				            	  */
/*--------------------------------------------------------------------------------------------*/
/* REENTRANCY: Reentrant													                  */
/**********************************************************************************************/

void SER_STM_Close(void)
{
}


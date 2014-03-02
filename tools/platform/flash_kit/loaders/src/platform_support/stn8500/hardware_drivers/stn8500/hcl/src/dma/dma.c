/*
* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma.c
* This file contains the HCL functions
* --------------------------------------------------------------------------------------------*/
#include "dma.h"
#include "dma_base.h"
#include "dma_p.h"

/*------------------------------------------------------------------------
 * Global Variables                                   
 *------------------------------------------------------------------------*/
#ifdef __DEBUG
PRIVATE t_dbg_level             myDebugLevel_DMA = DEBUG_LEVEL0;
PRIVATE t_dbg_id                myDebugID_DMA = DMA_HCL_DBG_ID;
#endif
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_DMA
#define MY_DEBUG_ID             myDebugID_DMA

PRIVATE t_dma_relink_config     g_dma_relink_config;
PRIVATE t_dma_controller_desc   g_dma_controller_desc[DMA_NO_OF_EVENT_GROUPS];

/*-------------- Private APIs ------------*/
PRIVATE t_dma_error dma_UpdateSrcBufferPhyMode(IN t_dma_channel_config chan_desc , IN t_dma_pipe_config pipe_desc);
PRIVATE t_dma_error dma_UpdateSrcBufferLogMode(IN t_dma_channel_config chan_desc , IN t_dma_pipe_config pipe_desc);
PRIVATE t_dma_error dma_UpdateDestBufferPhyMode(IN t_dma_channel_config chan_desc ,  IN t_dma_pipe_config pipe_desc);
PRIVATE t_dma_error dma_UpdateDestBufferLogMode(IN t_dma_channel_config chan_desc ,  IN t_dma_pipe_config pipe_desc);
PRIVATE t_dma_error dma_SetPhyRelinkParameters(IN t_dma_pipe_config * , IN t_dma_half_chan);
PRIVATE t_dma_error dma_SetPhyRelinkParamSrcHalfChan(IN t_dma_pipe_config *p_pipe_desc);
PRIVATE t_dma_error dma_SetPhyRelinkParamDestHalfChan(IN t_dma_pipe_config *p_pipe_desc);
PRIVATE t_dma_error dma_SetLogChannelParameters(IN t_dma_pipe_config *, IN t_dma_half_chan); 
PRIVATE t_dma_error dma_SetLogRelinkParameters(IN t_dma_pipe_config * , IN t_dma_half_chan);
PRIVATE t_dma_error dma_SetLogRelinkParamSrcHalfChan(IN t_dma_pipe_config *p_pipe_desc);  
PRIVATE t_dma_error dma_SetLogRelinkParamDestHalfChan(IN t_dma_pipe_config *p_pipe_desc); 
PRIVATE t_dma_error dma_CheckPipeAvailability(IN t_dma_evt_grp_num , IN volatile t_dma_pipe_num * , IN volatile t_dma_pipe_num *);
PRIVATE t_dma_error dma_EventGroupRegisterPipe(IN t_dma_channel_config chan_desc, IN t_dma_evt_grp_num event_group, IN t_dma_pipe_num src_pipe_id, IN t_dma_pipe_num dest_pipe_id, 
                                               IN t_dma_src_device src_device, IN t_dma_dest_device dest_device, IN t_dma_event_group src_event_group, IN t_dma_event_group dest_event_group);
PRIVATE t_dma_error dma_GetPhyLLIState(IN t_dma_channel_config , IN t_dma_half_chan);
PRIVATE t_dma_error dma_GetLogLLIState(IN t_dma_pipe_config , t_dma_half_chan);
PRIVATE t_dma_error dma_ResetLogLLIState(IN t_dma_pipe_config);
PRIVATE t_dma_error dma_GetLogLLIStateSuspExch(IN t_dma_pipe_config pipe_desc , t_dma_half_chan half_chan);
PRIVATE t_dma_error dma_GetLogLinkStatus(IN t_dma_pipe_config , IN t_dma_half_chan , OUT t_uint8 *);

PRIVATE void dma_ProcessTCIt(IN t_dma_event_desc *);
PRIVATE void dma_ProcessErrorIt(IN t_dma_event_desc *p_event_desc);


/******************************************************************************************/
/* NAME:  DMA_Init                                                                        */
/*----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is called for Initialisation of DMA Hardware base address   */
/* PARAMETERS:                                                                            */
/* IN:    dma_base_address: Base address of DMA                                           */
/* INOUT: None                                                                            */
/* OUT:   None                                                                            */
/*                                                                                        */
/* RETURN: t_dma_error  which can take any of following values -                          */
/*              DMA_UNSUPPORTED_HW   :  If DMA peripheral ids don't match                 */
/*              DMA_INVALID_PARAMETER:    If DMA base address passed is zero              */
/*              DMA_OK               :    If no error                                     */
/*----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                   */
/* REENTRANCY ISSUE: NA                                                                   */

/******************************************************************************************/
PUBLIC t_dma_error DMA_Init(IN t_logical_address dma_base_address)
{
    DBGENTER1("DMA Base Address :%lx", dma_base_address);   /* Debug Macros defined in debug.h */
    if (NULL == dma_base_address)
    {
        DBGEXIT0(DMA_INVALID_PARAMETER);                    /* Debug Macros defined in debug.h */
        return(DMA_INVALID_PARAMETER);
    }

    /* Saving DMA base address to global dma system context variable */
    if (!dmaBase_Init(dma_base_address))
    {
        DBGEXIT0(DMA_UNSUPPORTED_HW);                       /* Debug Macros defined in debug.h */
        return(DMA_UNSUPPORTED_HW);
    }

    DBGEXIT0(DMA_OK);   /* Debug Macros defined in debug.h */
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_GetVersion                                                                   */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This function is called for getting the DMA HCL version used               */
/* PARAMETERS:                                                                             */
/* IN:    None                                                                             */
/* INOUT: None                                                                             */
/* OUT:   p_dma_hcl_version: DMA HCL version currently used                                */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*          DMA_INVALID_PARAMETER: If address passed for writing DMA HCL version is zero   */
/*          DMA_OK               : If no error                                             */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_GetVersion(OUT t_version *p_dma_hcl_version)
{
    DBGENTER1("Address of output variable(for DMA HCL version) :%lx", p_dma_hcl_version);
    if (NULL == p_dma_hcl_version)
    {
        DBGEXIT0(DMA_INVALID_PARAMETER);
        return(DMA_INVALID_PARAMETER);
    }

    p_dma_hcl_version->version = DMA_HCL_VERSION_ID;
    p_dma_hcl_version->major = DMA_HCL_MAJOR_ID;
    p_dma_hcl_version->minor = DMA_HCL_MINOR_ID;

    DBGEXIT3(DMA_OK, "DMA HCL version : %d.%d.%d", DMA_HCL_VERSION_ID, DMA_HCL_MAJOR_ID, DMA_HCL_MINOR_ID);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_SetDbgLevel                                                                  */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the debug level for DMA HCL                              */
/*              Debug levels are defined in debug.h file                                   */
/* PARAMETERS:                                                                             */
/* IN:    dbg_level: DMA debug level                                                       */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*            DMA_UNSUPPORTED_FEATURE: If not compiled for debug                           */
/*            DMA_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_SetDbgLevel(IN t_dbg_level dbg_level)
{
#if defined(__DEBUG)
    DBGENTER1("Debug Level :%d", dbg_level);
    myDebugLevel_DMA = dbg_level;
    DBGEXIT0(DMA_OK);
    return(DMA_OK);
#else
    /* dbg_level = dbg_level;   To remove warnings TBD */
    DBGEXIT0(DMA_UNSUPPORTED_FEATURE);
    return(DMA_UNSUPPORTED_FEATURE);
#endif
}

/*******************************************************************************************/
/* NAME:  DMA_InitRelinkMemory                                                             */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialiazes the relink base address of the  physical and the */
/*              logical channel. The 512K of memory is allocated by the DMA driver, 256K   */
/*                for the logical and 256 for the physical channel.                        */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    relink_base_addr  : Base Address of the relink memory                            */
/*          relink_memory_max : Maximum size allocated for the relink memory               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_dma_error                                                                     */
/*            DMA_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_InitRelinkMemory(IN t_system_address *relink_base_addr, IN t_uint32 relink_memory_size)
{
    g_dma_relink_config.physical_lli_base_addr = relink_base_addr->physical;
    g_dma_relink_config.logical_lli_base_addr = relink_base_addr->logical;

    g_dma_relink_config.phy_chan_param_phy_addr = g_dma_relink_config.physical_lli_base_addr;
    g_dma_relink_config.phy_chan_param_log_addr = g_dma_relink_config.logical_lli_base_addr;
    g_dma_relink_config.log_to_phy_offset = (t_sint32) (g_dma_relink_config.phy_chan_param_log_addr - g_dma_relink_config.phy_chan_param_phy_addr);

    g_dma_relink_config.log_chan_link_phy_addr = g_dma_relink_config.physical_lli_base_addr + DMA_PHYSICAL_RELINK_MEMORY_SIZE;
    g_dma_relink_config.log_chan_link_log_addr = g_dma_relink_config.logical_lli_base_addr + DMA_PHYSICAL_RELINK_MEMORY_SIZE;

    g_dma_relink_config.log_chan_param_phy_addr = g_dma_relink_config.log_chan_link_phy_addr + DMA_LOGICAL_RELINK_MEMORY_SIZE;
    g_dma_relink_config.log_chan_param_log_addr = g_dma_relink_config.log_chan_link_log_addr + DMA_LOGICAL_RELINK_MEMORY_SIZE;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/****************************************************************************/
/* NAME: t_dma_error DMA_OpenChannel()                                      */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*      This routine opens a channel for communication between memory and   */
/*        peripherals associated with the channel.                          */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN  :                                                                    */
/*      - dma_channel    :   channel id (NULL-7)                               */
/*                                                                          */
/* OUT : t_dma_channel_config                                               */
/*                                                                          */
/* RETURN:    t_dma_error  which can take any of following values -         */
/*            DMA_CHANNEL_UNAVAILABLE : If no physical resource is avbl     */
/*            DMA_OK                  : If no error                         */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_dma_error DMA_OpenChannel(IN t_dma_chan_id dma_channel, OUT t_dma_channel_config *p_chan_desc)
{
    t_dma_chan_num      chan_num = (t_dma_chan_num) dma_channel;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;

    DBGENTER1("Channel Id = %d", dma_channel);

    if (!g_dma_controller_desc[event_group].channel_config[chan_index].is_channel_open)
    {
        p_chan_desc->chan_num = chan_num;
        g_dma_controller_desc[event_group].channel_config[chan_index].is_channel_open = TRUE;
        g_dma_controller_desc[event_group].channel_config[chan_index].exchange_state = DMA_NOT_INIT_EXCHANGE;

        dmaBase_ResetChannel((t_dma_channel) chan_num);
    }
    else
    {
        DBGEXIT0(DMA_CHANNEL_UNAVAILABLE);
        return(DMA_CHANNEL_UNAVAILABLE);
    }

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/****************************************************************************/
/* NAME: t_dma_error DMA_OpenPipe()                                         */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*  This routine allows opens the pipe between a source and a destination   */
/*  on the valid channel.                                                   */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN  :                                                                    */
/*      - chan_desc     :   channel configuration                           */
/*      - src_device    :   identify the source DMA device(NULL - 63)       */
/*      - dest_device   :   identify the destination DMA device(0 - 63)     */
/*                                                                          */
/* OUT : t_dma_pipe_config : returns the pipe configuration                 */
/*                                                                          */
/* RETURN:    t_dma_error  which can take any of following values -         */
/*            DMA_UNSUPPORTED_TRANSFER    : if transfer not supported       */
/*            DMA_UNSUPPORTED_DEVICE      : if the device is not associated */
/*                                          with the inpt channel           */
/*            DMA_OK                      : If no error                     */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_dma_error DMA_OpenPipe
(
    IN t_dma_channel_config chan_desc,
    IN t_dma_src_device     src_device,
    IN t_dma_dest_device    dest_device,
    OUT t_dma_pipe_config   *p_pipe_desc
)
{
    t_dma_error         dma_error;
    volatile t_dma_pipe_num      src_pipe_id;
    volatile t_dma_pipe_num		dest_pipe_id;
    t_dma_event_group   src_event_group, dest_event_group;
    t_dma_chan_num      channel_id = chan_desc.chan_num;
    t_dma_evt_grp_num   event_group = channel_id / DMA_DIVISOR;

    DBGENTER2("Source Device Id = %d, Destination Device Id = %d", src_device, dest_device);

    /*------ Get source and destination pipe ids and associate channel to the event group ------- */
    if (src_device == DMA_MEMORY_SRC)
    {
        src_pipe_id = DMA_PIPE_DIVISOR;   /* starting index for the source memory device in the buffer*/
        src_event_group = DMA_NO_EVENT_GROUP;
    }
    else
    {
        src_pipe_id = ((t_dma_device) src_device % DMA_PIPE_DIVISOR);
        src_event_group = (t_dma_event_group) ((t_uint8) src_device / DMA_PIPE_DIVISOR);

        if (src_event_group != (t_dma_event_group) event_group)
        {
            DBGEXIT0(dma_error);
            return(DMA_UNSUPPORTED_DEVICE);
        }
    }

    if (dest_device == DMA_MEMORY_DEST)
    {
        dest_pipe_id = DMA_PIPE_DIVISOR;  /* starting index for the dest memory device  in the buffer*/
        dest_event_group = DMA_NO_EVENT_GROUP;
    }
    else
    {
        dest_pipe_id = ((t_dma_device) dest_device % DMA_PIPE_DIVISOR);
        dest_event_group = (t_dma_event_group) ((t_uint8) dest_device / DMA_PIPE_DIVISOR);

        if (dest_event_group != (t_dma_event_group) event_group)
        {
            DBGEXIT0(dma_error);
            return(DMA_UNSUPPORTED_DEVICE);
        }
    }

    /*--------------- Get available channel in the respective event groups --------------- */
    dma_error = dma_CheckPipeAvailability(event_group, &src_pipe_id, &dest_pipe_id);

    if (DMA_OK == dma_error)
    {
        dma_error = dma_EventGroupRegisterPipe
            (
                chan_desc,
                event_group,
                src_pipe_id,
                dest_pipe_id,
                src_device,
                dest_device,
                src_event_group,
                dest_event_group
            );
        if (src_pipe_id > DMA_MAX_PIPE_ID || dest_pipe_id > DMA_MAX_PIPE_ID)
        {
            return(DMA_PIPE_NOT_OPENED);
        }

        switch (chan_desc.mode)
        {
            case DMA_PHYSICAL:
                p_pipe_desc->irq_num = p_pipe_desc->chan_num;
                break;

            case DMA_LOGICAL:
                if (DMA_SRC_EVENT == g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_type)
                {
                    p_pipe_desc->irq_num = DMA_DIVISOR * g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_num;
                }
                else
                {
                    p_pipe_desc->irq_num = DMA_DIVISOR *
                        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_num +
                        MASK_BIT0;
                }
                break;
        }

        g_dma_controller_desc[event_group].src_device_config[src_pipe_id].log_lli_state = DMA_INIT_LLI_STATE;
        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].log_lli_state = DMA_INIT_LLI_STATE;
        g_dma_controller_desc[event_group].src_device_config[src_pipe_id].link_id = NULL;
        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].link_id = NULL;
        g_dma_controller_desc[event_group].src_device_config[src_pipe_id].no_of_links = NULL;
        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].no_of_links = NULL;

        g_dma_controller_desc[event_group].src_device_config[src_pipe_id].is_device_open = TRUE;
        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].is_device_open = TRUE;

        p_pipe_desc->chan_num = channel_id;
        p_pipe_desc->src_pipe_id = src_pipe_id;
        p_pipe_desc->dest_pipe_id = dest_pipe_id;
    }
    else
    {
        DBGEXIT0(dma_error);
        return(dma_error);
    }

    DBGEXIT0(dma_error);
    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  DMA_SetChannelType                                                               */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the channel type and security                            */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN    :      chan_type            :      STANDARD or EXTENDED                           */
/* IN    :      chan_security        :      SECURE or NON-SECURE                           */
/* INOUT :      t_dma_channel_config :      Returns Type and Security                      */
/* OUT   :      none                                                                       */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*          DMA_OK                                                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_SetChannelType
(
    IN t_dma_channel_type       chan_type,
    IN t_dma_channel_security   chan_security,
    INOUT t_dma_channel_config  *p_chan_desc
)
{
    t_dma_chan_num      chan_num = p_chan_desc->chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;

    DBGENTER0();

    /* Code to be put for the check if the channel is running. Cannot change the type and security when the 
	channel is running. */
    /* Set the Physical resource as Standard or Extended */
 
   #ifndef ST_HREFED
   
   if((chan_num != DMA_CHAN_6) && (chan_num != DMA_CHAN_7)) 

   {
   	
      dmaBase_SetPRType((t_dma_channel) chan_num, (t_dma_pr_type) chan_type);
   }
    
   #endif 
    g_dma_controller_desc[event_group].channel_config[chan_index].type = chan_type;

    /* Set the Physical resource in Secure or Non Secure Mode */
    
  #if defined(ST_8500ED) || defined(ST_8500V1) || defined(ST_8500V2) || defined(__PEPS_8500)|| defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2)
    

    dmaBase_SetPRSecurity((t_dma_channel) chan_num, (t_dma_pr_security) chan_security);
    
    #endif 


    
    g_dma_controller_desc[event_group].channel_config[chan_index].security = chan_security;

    p_chan_desc->type = chan_type;
    p_chan_desc->security = chan_security;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_SetChannelModeOption()                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the channel mode and option                              */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    chan_mode        :   PHYSICAL or LOGICAL or OPERATION                            */
/* IN:     chan_option     :   BASIC                 (Physical)                            */
/*                            MODULO_ADDRESSING                                            */
/*                            DOUBLE_DEST                                                  */
/*                                                                                         */
/*                            SRC_PHY_DEST_LOG    (logical)                                */
/*                            SRC_LOG_DEST_PHY                                             */
/*                            SRC_LOG_DEST_LOG                                             */
/* INOUT :      t_dma_channel_config :      Returns Type and Security                      */
/* OUT:         none                                                                       */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*          DMA_OK                                                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_SetChannelModeOption
(
    IN t_dma_channel_mode       chan_mode,
    IN t_dma_channel_option     chan_option,
    INOUT t_dma_channel_config  *p_chan_desc
)
{
    t_dma_base_addr     base_addr;
    t_dma_chan_num      chan_num = p_chan_desc->chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;

    DBGENTER0();

    /* Code to be put for the check if the channel is running. Cannot change the type and security when the 
	  channel is running. */
    /* Set the Physical resource in Physical , Logical or Operation Mode */
    dmaBase_SetPRModeSelect((t_dma_channel) chan_num, (t_dma_pr_mode) chan_mode);
    g_dma_controller_desc[event_group].channel_config[chan_index].mode = chan_mode;

    /* Set the Option for Physical resource in Physical , Logical or Operation Mode */
    dmaBase_SetPRModeOption((t_dma_channel) chan_num, (t_dma_pr_mode_option) chan_option);
    g_dma_controller_desc[event_group].channel_config[chan_index].option = chan_option;

    if (DMA_PHYSICAL == chan_mode)
    {
        g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num = NULL;
        g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num = NULL;
    }
    else if (DMA_LOGICAL == chan_mode)
    {
        base_addr.param_base_addr = (t_uint32) g_dma_relink_config.log_chan_param_phy_addr;
        base_addr.link_base_addr = (t_uint32) g_dma_relink_config.log_chan_link_phy_addr;

       
       #ifndef ST_HREFED
        /* Set the Parameter base address and Link base address */
        dmaBase_SetLogicalChanBaseAddr((t_dma_pr_security) p_chan_desc->security, &base_addr);
	 #endif 
        g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.base_phy_addr = g_dma_relink_config.
                log_chan_link_phy_addr +
            ONE_KB *
            chan_num;

        g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.base_log_addr = g_dma_relink_config.
                log_chan_link_log_addr +
            ONE_KB *
            chan_num;

        g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset = NULL;
    }

    p_chan_desc->mode = chan_mode;
    p_chan_desc->option = chan_option;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_SetTCInterruptLogic()                                                        */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the channel interrupt generation logic for the source    */
/*                and the destination half channels.                                       */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:      intr_gen        :   Relink or EOT                                              */
/*          src_intr        :   TRUE or FALSE                                              */
/*          dest_intr       :   TRUE or FALSE                                              */
/*                                                                                         */
/* INOUT: t_dma_channel_config                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*          DMA_OK                                                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_SetTCInterruptLogic
(
    IN t_dma_pipe_config    pipe_desc,
    IN t_dma_intr_gen       intr_gen,
    IN t_bool               src_intr,
    IN t_bool               dest_intr
)
{
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_pipe_num      src_pipe_id = pipe_desc.src_pipe_id;
    t_dma_pipe_num      dest_pipe_id = pipe_desc.dest_pipe_id;

    DBGENTER2("src_intr = %d, dest_intr = %d", src_intr, dest_intr);

    g_dma_controller_desc[event_group].src_device_config[src_pipe_id].intr_gen = intr_gen;
    g_dma_controller_desc[event_group].src_device_config[src_pipe_id].is_tc_intr = src_intr;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].intr_gen = intr_gen;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].is_tc_intr = dest_intr;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_SetMasterPort()                                                              */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets Master port for Source and Destination half channels.    */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:      t_dma_pipe_config :   pipe_desc                                                */
/*          t_dma_master      :   src_master_port                                          */
/*          t_dma_master      :   dest_master_port                                         */
/*                                                                                         */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*          DMA_OK                                                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_SetMasterPort
(
    IN t_dma_pipe_config    pipe_desc,
    IN t_dma_master         src_master_port,
    IN t_dma_master         dest_master_port
)
{
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_pipe_num      src_pipe_id = pipe_desc.src_pipe_id;
    t_dma_pipe_num      dest_pipe_id = pipe_desc.dest_pipe_id;

    DBGENTER0();

    g_dma_controller_desc[event_group].src_device_config[src_pipe_id].master_port = src_master_port;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].master_port = dest_master_port;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_SetTerminalCountPulse()                                                      */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine sets the channel terminal count pulse for the source          */
/*                and the destination half channels.                                       */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:      t_dma_pipe_config   :   pipe_desc                                              */
/*          t_bool              :   src_tcp                                                */
/*          t_bool              :   dest_tcp                                               */
/*                                                                                         */
/* INOUT: t_dma_channel_config                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*          DMA_OK                                                                         */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_SetTerminalCountPulse(IN t_dma_pipe_config pipe_desc, IN t_bool src_tcp, IN t_bool dest_tcp)
{
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_pipe_num      src_pipe_id = pipe_desc.src_pipe_id;
    t_dma_pipe_num      dest_pipe_id = pipe_desc.dest_pipe_id;

    DBGENTER0();

    g_dma_controller_desc[event_group].src_device_config[src_pipe_id].tcp_assert = src_tcp;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].tcp_assert = dest_tcp;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_ConfigSrcDevice()                                                            */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the source half channel parameters and stores in   */
/*                the global channel descriptor variable                                   */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:                                                                                     */
/*    p_chan_desc  :  channel configuration details                                        */
/*    src_addr     :  address of the source buffer.                                        */
/*    data_width   :  data_width of the transfer (MASK_BIT0,2,4,8 bytes)                           */
/*    burst_size   :  burst_size of the transfer (2,4,8,16) elements                       */
/*    burst_enable :  enable/disable the burst mode                                        */
/*    byte_index   :  byte distance between two consecutive elements                       */
/*    tx_size      :  Total size of the transfer                                           */
/*    block_size   :  Elements transferred in one link                                     */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*            DMA_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_ConfigSrcDevice
(
    IN t_dma_pipe_config    pipe_desc,
    IN t_uint32             src_addr,
    IN t_dma_data_width     data_width,
    IN t_dma_burst_size     burst_size,
    IN t_uint16             byte_index,
    IN t_uint32             tx_size,
    IN t_uint16             block_size
)
{
    t_dma_pipe_num      pipe_id = pipe_desc.src_pipe_id;
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_uint32            element_count;

    DBGENTER6
    (
        "src_addr @= 0x%08lX , width= %d , burst_size = %d , element_index = %d , tx_size = %ld , block_size = %d",
        src_addr,
        data_width,
        burst_size,
        byte_index,
        tx_size,
        block_size
    );

    /* Check for the transfer size is not divisible by the data width */
    if (NULL != (tx_size % (t_uint8) (MASK_BIT0 << (t_uint8) data_width)))
    {
        DBGEXIT0(DMA_INCOMPATIBLE_TRANSFER_SIZE);
        return(DMA_INCOMPATIBLE_TRANSFER_SIZE);
    }

    g_dma_controller_desc[event_group].src_device_config[pipe_id].address = src_addr;
    g_dma_controller_desc[event_group].src_device_config[pipe_id].curr_addr = src_addr;
    g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width = data_width;
    g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size = tx_size;
    g_dma_controller_desc[event_group].src_device_config[pipe_id].intr_gen = DMA_EOT;
    g_dma_controller_desc[event_group].src_device_config[pipe_id].is_tc_intr = FALSE;

    /*    if the device is memory    */
    if (DMA_NO_EVENT_GROUP == g_dma_controller_desc[event_group].src_device_config[pipe_id].event_group)
    {
        g_dma_controller_desc[event_group].src_device_config[pipe_id].master_port = DMA_MASTER_0;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].tx_mode = DMA_NO_SYNC;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].is_addr_incr = TRUE;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].event_line = NULL;

        /* Auto Configuration for the element index */
        if (byte_index != NULL)
        {
            g_dma_controller_desc[event_group].src_device_config[pipe_id].element_index = byte_index;
        }
        else
        {
            g_dma_controller_desc[event_group].src_device_config[pipe_id].element_index = (t_uint16) (MASK_BIT0 << (t_uint8) data_width);
        }
    }

    /*    if the device is peripheral    */
    else
    {
        g_dma_controller_desc[event_group].src_device_config[pipe_id].master_port = DMA_MASTER_1;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].tx_mode = DMA_PACKET_SYNC;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].is_addr_incr = FALSE;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].element_index = byte_index;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].event_line = pipe_id % DMA_PIPE_DIVISOR;
    }

    /* Check for burst enable. */
    if (DMA_NO_BURST == burst_size)
    {
        g_dma_controller_desc[event_group].src_device_config[pipe_id].is_pen = FALSE;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].burst_size = (t_dma_burst_size) NULL;
    }
    else
    {
        g_dma_controller_desc[event_group].src_device_config[pipe_id].is_pen = TRUE;
        g_dma_controller_desc[event_group].src_device_config[pipe_id].burst_size = burst_size;
    }

    g_dma_controller_desc[event_group].src_device_config[pipe_id].block_size = block_size;
    if ((block_size == NULL) || (tx_size <= block_size))
    {
        element_count = tx_size / (t_uint8) (MASK_BIT0 << (t_uint8) (data_width));
    }
    else
    {
        element_count = block_size / (t_uint8) (MASK_BIT0 << (t_uint8) (data_width));
    }

    if (DMA_ELEMENT_TRANSFER_MAX >= element_count)
    {
        g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count = (t_uint16) element_count;
    }
    else
    {
        g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count = DMA_ELEMENT_TRANSFER_MAX;
    }

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_UpdateSrcMemoryBuffer                                                        */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine updates the source half channel parameters into the DMA       */
/*                channel registers and also updates the link parameters.                  */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    t_dma_channel_config: channel configuration                                      */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_UpdateSrcMemoryBuffer(IN t_dma_channel_config chan_desc, IN t_dma_pipe_config pipe_desc)
{
    t_dma_error dma_error = DMA_OK;

    switch (chan_desc.mode)
    {
        case DMA_PHYSICAL:
            dma_error = dma_UpdateSrcBufferPhyMode(chan_desc, pipe_desc);
            break;

        case DMA_LOGICAL:
            dma_error = dma_UpdateSrcBufferLogMode(chan_desc, pipe_desc);
            break;
    }

    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  DMA_ConfigDestDevice()                                                           */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine configures the destination half channel parameters and stores */
/*                in the global channel descriptor variable                                */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:                                                                                     */
/*    chan_desc    :  channel configuration details                                        */
/*    dest_addr    :  address of the destination buffer.                                   */
/*    data_width   :  data_width of the transfer (1,2,4,8 bytes)                           */
/*    burst_size   :  burst_size of the transfer (2,4,8,16) elements                       */
/*    burst_enable :  enable/disable the burst mode                                        */
/*    byte_index   :  byte distance between two consecutive elements                       */
/*    tx_size      :  Total size of the transfer                                           */
/*    block_size   :  Elements transferred in one link                                     */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*            DMA_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_ConfigDestDevice
(
    IN t_dma_pipe_config    pipe_desc,
    IN t_uint32             dest_addr,
    IN t_dma_data_width     data_width,
    IN t_dma_burst_size     burst_size,
    IN t_uint16             byte_index,
    IN t_uint32             tx_size,
    IN t_uint16             block_size
)
{
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_pipe_num      pipe_id = pipe_desc.dest_pipe_id;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_uint32            element_count;

    DBGENTER6
    (
        "dest_addr @= 0x%08lX , width= %d , burst_size = %d , element_index = %d , tx_size = %ld , block_size = %d",
        dest_addr,
        data_width,
        burst_size,
        byte_index,
        tx_size,
        block_size
    );

    /* Check for the transfer size is not divisible by the data width */
    if (NULL != (tx_size % (t_uint8) (MASK_BIT0 << (t_uint8) data_width)))
    {
        DBGEXIT0(DMA_INCOMPATIBLE_TRANSFER_SIZE);
        return(DMA_INCOMPATIBLE_TRANSFER_SIZE);
    }

    g_dma_controller_desc[event_group].dest_device_config[pipe_id].address = dest_addr;
    g_dma_controller_desc[event_group].dest_device_config[pipe_id].curr_addr = dest_addr;
    g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width = data_width;
    g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size = tx_size;
    g_dma_controller_desc[event_group].dest_device_config[pipe_id].intr_gen = DMA_EOT;
    g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_tc_intr = TRUE;

    /*    if the device is memory    */
    if (DMA_NO_EVENT_GROUP == g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_group)
    {
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].master_port = DMA_MASTER_0;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].tx_mode = DMA_NO_SYNC;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_addr_incr = TRUE;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_line = NULL;

        /* Auto Configuration for the element index */
        if (byte_index != NULL)
        {
            g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_index = byte_index;
        }
        else
        {
            g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_index = (t_uint16) (MASK_BIT0 << (t_uint8) data_width);
        }
    }
    else    /*    if the device is peripheral    */
    {
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].master_port = DMA_MASTER_1;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].tx_mode = DMA_PACKET_SYNC;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_addr_incr = FALSE;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_index = byte_index;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_line = (pipe_id % DMA_PIPE_DIVISOR);
    }

    if (DMA_NO_BURST == burst_size)
    {
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_pen = FALSE;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].burst_size = (t_dma_burst_size) NULL;
    }
    else
    {
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_pen = TRUE;
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].burst_size = burst_size;
    }

    g_dma_controller_desc[event_group].dest_device_config[pipe_id].block_size = block_size;
    if ((block_size == NULL) || (tx_size <= block_size))
    {
        element_count = (tx_size / (t_uint8) (MASK_BIT0 << (t_uint8) (data_width)));
    }
    else
    {
        element_count = block_size / (t_uint8) (MASK_BIT0 << (t_uint8) (data_width));
    }

    if (DMA_ELEMENT_TRANSFER_MAX >= element_count)
    {
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count = element_count;
    }
    else
    {
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count = DMA_ELEMENT_TRANSFER_MAX;
    }

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  DMA_UpdateDestMemoryBuffer                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine updates the destination half channel parameters into the DMA  */
/*                channel registers and also updates the link parameters.                  */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_lli_desc : starting address of the relink memory.                             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_UpdateDestMemoryBuffer(IN t_dma_channel_config chan_desc, IN t_dma_pipe_config pipe_desc)
{
    t_dma_error dma_error = DMA_OK;

    switch (chan_desc.mode)
    {
        case DMA_PHYSICAL:
            dma_error = dma_UpdateDestBufferPhyMode(chan_desc, pipe_desc);
            break;

        case DMA_LOGICAL:
            dma_error = dma_UpdateDestBufferLogMode(chan_desc, pipe_desc);
            break;
    }

    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  DMA_TransferActivate()                                                           */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine activates the channel. The channel can be configured to the   */
/*                running, suspended or stop state.                                        */
/*                                                                                         */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:                                                                                     */
/*  chan_desc          :  channel configuration details                                    */
/*    active_status    :    RUN                                                          */
/*                        SUSPEND_REQ                                                      */
/*                        STOP                                                             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: t_dma_error  which can take any of following values -                           */
/*            DMA_OK               : If no error                                           */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PUBLIC t_dma_error DMA_TransferActivate(IN t_dma_channel_config chan_desc, IN t_dma_active_status active_status)
{
    t_uint8             pipe_index;
    t_dma_evt_grp_num   event_group = chan_desc.chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_desc.chan_num % DMA_DIVISOR;

    DBGENTER0();

    switch (active_status)
    {
        case DMA_RUN:
            dmaBase_SetPRActiveStatus((t_dma_channel) chan_desc.chan_num, DMA_ACTIVE_RUN);

            switch (chan_desc.mode)
            {
                case DMA_PHYSICAL:
                    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state = DMA_INIT_LLI_STATE;
                    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state = DMA_INIT_LLI_STATE;
                    break;

                case DMA_LOGICAL:
                    for (pipe_index = NULL; pipe_index < DMA_PIPES_PER_PHY_CHAN; pipe_index++)
                    {
                        g_dma_controller_desc[event_group].src_device_config[pipe_index].log_lli_state = DMA_INIT_LLI_STATE;
                        g_dma_controller_desc[event_group].dest_device_config[pipe_index].log_lli_state = DMA_INIT_LLI_STATE;
                    }
                    break;
            }

            g_dma_controller_desc[event_group].channel_config[chan_index].exchange_state = DMA_ONGOING_EXCHANGE;

            break;

        case DMA_SUSPEND:
            dmaBase_SetPRActiveStatus((t_dma_channel) chan_desc.chan_num, DMA_ACTIVE_SUSPEND_REQ);

            while
            (
                (dmaBase_GetPRActiveStatus((t_dma_channel) chan_desc.chan_num) != DMA_ACTIVE_SUSPENDED)
            &&  (dmaBase_GetPRActiveStatus((t_dma_channel) chan_desc.chan_num) != DMA_ACTIVE_STOP)
            )
                ;
            g_dma_controller_desc[event_group].channel_config[chan_index].exchange_state = DMA_SUSPENDED_EXCHANGE;

            if (dmaBase_GetPRActiveStatus((t_dma_channel) chan_desc.chan_num) == DMA_ACTIVE_STOP)
            {
                g_dma_controller_desc[event_group].channel_config[chan_index].exchange_state = DMA_HALTED_EXCHANGE;
                return(DMA_ERROR);
            }
            break;

        case DMA_STOP:
            dmaBase_SetPRActiveStatus((t_dma_channel) chan_desc.chan_num, DMA_ACTIVE_SUSPEND_REQ);

            while
            (
                (dmaBase_GetPRActiveStatus((t_dma_channel) chan_desc.chan_num) != DMA_ACTIVE_SUSPENDED)
            &&  (dmaBase_GetPRActiveStatus((t_dma_channel) chan_desc.chan_num) != DMA_ACTIVE_STOP)
            )
            {
                break;
            }

            dmaBase_SetPRActiveStatus((t_dma_channel) chan_desc.chan_num, DMA_ACTIVE_STOP);
            g_dma_controller_desc[event_group].channel_config[chan_index].exchange_state = DMA_HALTED_EXCHANGE;

            break;
    }

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/****************************************************************************/
/* NAME: t_dma_error DMA_ClosePipe()                                        */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    This routine closes the pipe between source and dest     */
//* PARAMETERS:                                                             */
/* IN  :                                                                    */
/*      - t_dma_channel_config    :    channel descriptor                   */
/*      - t_dma_pipe_config       :    pipe_descriptor                      */
/* OUT :                                                                    */
/*                                                                          */
/* RETURN:      t_dma_error                                                 */
//*-------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_dma_error DMA_ClosePipe(IN t_dma_channel_config chan_desc, IN t_dma_pipe_config pipe_desc)
{
    t_dma_chan_num      chan_num = chan_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_pipe_num      src_pipe_num, dest_pipe_num;
    t_uint8             event_line;
    t_dma_event_type    event_type;

    DBGENTER0();

    src_pipe_num = pipe_desc.src_pipe_id;
    dest_pipe_num = pipe_desc.dest_pipe_id;

    event_type = g_dma_controller_desc[event_group].src_device_config[src_pipe_num].event_type;
    event_line = g_dma_controller_desc[event_group].src_device_config[src_pipe_num].event_num;

    if ((DMA_LOGICAL == chan_desc.mode) && (DMA_NONSECURE == chan_desc.security))
    {
        dmaBase_ClearEventLineSecurity(DMA_PR_NONSECURE, (t_dma_evt_line_type) event_type, (t_dma_event) event_line);
    }

    g_dma_controller_desc[event_group].src_device_config[src_pipe_num].is_device_open = FALSE;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].is_device_open = FALSE;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/****************************************************************************/
/* NAME: t_dma_error DMA_CloseChannel()                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    This routine closes the channel.                         */
//* PARAMETERS:                                                             */
/* IN  :                                                                    */
/*      - t_dma_channel_config    :    channel descriptor                   */
/*                                                                          */
/* OUT :                                                                    */
/*                                                                          */
/* RETURN:      t_dma_error                                                 */
//*-------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_dma_error DMA_CloseChannel(IN t_dma_channel_config chan_desc)
{
    t_uint8             pipe_index;
    t_dma_chan_num      chan_num = chan_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;

    DBGENTER0();

    /* Close all source and destination pipes */
    for (pipe_index = NULL; pipe_index < DMA_PIPES_PER_PHY_CHAN; pipe_index++)
    {
        g_dma_controller_desc[event_group].src_device_config[pipe_index].is_device_open = FALSE;
        g_dma_controller_desc[event_group].dest_device_config[pipe_index].is_device_open = FALSE;
    }

    /* Stop the physical channel. 
	The physical channel cannot be stopped directly , a suspend request is send and waited for the 
	channel to get suspended, further, stop command is sent to the channel.*/
    if (dmaBase_GetPRActiveStatus((t_dma_channel) chan_num) != DMA_ACTIVE_STOP)
    {
        dmaBase_SetPRActiveStatus((t_dma_channel) chan_num, DMA_ACTIVE_SUSPEND_REQ);
        while (dmaBase_GetPRActiveStatus((t_dma_channel) chan_num) != DMA_ACTIVE_SUSPENDED)
            ;

        dmaBase_SetPRActiveStatus((t_dma_channel) chan_num, DMA_ACTIVE_STOP);
    }

    /* Set Channel flag to false */
    g_dma_controller_desc[event_group].channel_config[chan_index].is_channel_open = FALSE;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/****************************************************************************/
/* NAME: t_dma_error DMA_ResetChannel()                                     */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    This routine resets the physical channel parameters      */
//* PARAMETERS:                                                             */
/* IN  :                                                                    */
/*      - t_dma_channel_config    :    channel descriptor                   */
/*                                                                          */
/* OUT :                                                                    */
/*                                                                          */
/* RETURN:      t_dma_error                                                 */
//*-------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_dma_error DMA_ResetChannel(IN t_dma_channel_config chan_desc)
{
    DBGENTER0();

    dmaBase_ResetChannel((t_dma_channel) chan_desc.chan_num);

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/****************************************************************************/
/* NAME: t_dma_error DMA_FilterProcessIRQSrc                                */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*    This routine is called for any IRQs related to DMA IP.                */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN  :                                                                    */
/*    - p_irq_src_status: internal storage variable to save the HW context  */
/*    - filter_mode: always DMA_NO_FILTER_MODE                              */
/*    - p_event_desc: event descriptor                                      */
/* OUT :                                                                    */
/*                                                                          */
/* RETURN:                                                                  */
/*        t_dma_error                                                       */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PUBLIC t_dma_error DMA_FilterProcessIRQSrc
(
    IN t_dma_irq_status     *p_irq_src_status,
    IN t_dma_event_desc     *p_event_desc,
    IN t_dma_filter_mode    filter_mode
)
{
    if (p_irq_src_status->err_intr)
    {
        dma_ProcessErrorIt(p_event_desc);
    }
    else if (p_irq_src_status->tc_intr)
    {
        dma_ProcessTCIt(p_event_desc);
    }

    return(DMA_OK);
}

/*---------------------------------------------------------------------------*/
/*                          PRIVATE APIs                                     */
/*---------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME: t_dma_error dma_GetPhyLLIState()                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    This routine resets the physical channel.                */
//* PARAMETERS:                                                             */
/* IN  :                                                                    */
/*      - t_dma_channel_config    :    channel descriptor                   */
/*                                                                          */
/* OUT :                                                                    */
/*                                                                          */
/* RETURN:      t_dma_error                                                 */
/*-------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE t_dma_error dma_GetPhyLLIState(IN t_dma_channel_config chan_desc, IN t_dma_half_chan half_chan)
{
    t_dma_chan_num      chan_num = chan_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;
    t_uint32            phy_link_addr;

    DBGENTER0();

    switch (g_dma_controller_desc[event_group].channel_config[chan_index].exchange_state)
    {
        case DMA_SUSPENDED_EXCHANGE:
            switch (half_chan)
            {
                case DMA_SOURCE_HALF_CHANNEL:
                    if (g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num == MASK_BIT0)
                    {
                        g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state = DMA_APPEND_LLI_STATE;
                    }
                    else
                    {
                        dmaBase_GetPhyLinkStatus((t_dma_channel) chan_num, DMA_SRC_HALF_CHAN, &phy_link_addr);
                        if (phy_link_addr == DMA_NULL_ADDRESS)    /* this indicates that dma is executing last lli */
                        {
                            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num = MASK_BIT0;
                            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state = DMA_APPEND_LLI_STATE;
                        }
                        else
                        {
                            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state = DMA_QUEUE_LLI_STATE;
                        }
                    }
                    break;

                case DMA_DEST_HALF_CHANNEL:
                    if (g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num == MASK_BIT0)
                    {
                        g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state = DMA_APPEND_LLI_STATE;
                    }
                    else
                    {
                        dmaBase_GetPhyLinkStatus((t_dma_channel) chan_num, DMA_DEST_HALF_CHAN, &phy_link_addr);
                        if (phy_link_addr == DMA_NULL_ADDRESS)
                        {
                            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num = MASK_BIT0;
                            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state = DMA_APPEND_LLI_STATE;
                        }
                        else
                        {
                            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state = DMA_QUEUE_LLI_STATE;
                        }
                    }
                    break;
            }
            break;

        case DMA_HALTED_EXCHANGE:
            switch (half_chan)
            {
                case DMA_SOURCE_HALF_CHANNEL:
                    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state = DMA_INIT_LLI_STATE;
                    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num = NULL;
                    break;

                case DMA_DEST_HALF_CHANNEL:
                    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state = DMA_INIT_LLI_STATE;
                    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num = NULL;

                    break;
            }
            break;
    }

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/****************************************************************************/
/* NAME: t_dma_error dma_GetLogLLIState()                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    This routine gets the LLI state of the channel in        */
/*                  logical mode                                            */
/* PARAMETERS:                                                              */
/* IN  :                                                                    */
/*      - t_dma_channel_config    :    channel descriptor                   */
/*                                                                          */
/* OUT :                                                                    */
/*                                                                          */
/* RETURN:      t_dma_error                                                 */
//*-------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE t_dma_error dma_GetLogLLIState(IN t_dma_pipe_config pipe_desc, t_dma_half_chan half_chan)
{
    t_dma_error         dma_error = DMA_OK;
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;

    switch (g_dma_controller_desc[event_group].channel_config[chan_index].exchange_state)
    {
        case DMA_SUSPENDED_EXCHANGE:
            dma_error = dma_GetLogLLIStateSuspExch(pipe_desc, half_chan);

            break;

        case DMA_HALTED_EXCHANGE:
            dma_error = dma_ResetLogLLIState(pipe_desc);

            break;
    }

    return(dma_error);
}

/****************************************************************************/
/* NAME: t_dma_error dma_GetLogLLIStateSuspExch()                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    This routine gets the LLI status of the channel in       */
/*                  logical mode for the supended exchange state            */
/* PARAMETERS:                                                              */
/* IN  :                                                                    */
/*      - t_dma_channel_config    :    channel descriptor                   */
/*                                                                          */
/* OUT :                                                                    */
/*                                                                          */
/* RETURN:      t_dma_error                                                 */
//*-------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE t_dma_error dma_GetLogLLIStateSuspExch(IN t_dma_pipe_config pipe_desc, t_dma_half_chan half_chan)
{
    t_dma_error         dma_error = DMA_OK;
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_pipe_num      src_pipe_num, dest_pipe_num;
    t_uint8             log_link_addr;
    t_dma_event_num     event_num;
    t_dma_evt_ctrl      event_state = DMA_EVT_DISABLE;

    src_pipe_num = pipe_desc.src_pipe_id;
    dest_pipe_num = pipe_desc.dest_pipe_id;

    switch (g_dma_controller_desc[event_group].src_device_config[src_pipe_num].event_type)
    {
        case DMA_SRC_EVENT:
            event_num = g_dma_controller_desc[event_group].src_device_config[src_pipe_num].event_num;
            dmaBase_GetLogChannelEvent((t_dma_channel) chan_num, DMA_SRC_EVT_LINE, (t_dma_event) event_num, &event_state);

            break;

        case DMA_DEST_EVENT:
            event_num = g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].event_num;
            dmaBase_GetLogChannelEvent((t_dma_channel) chan_num, DMA_DEST_EVT_LINE, (t_dma_event) event_num, &event_state);

            break;
    }

    if (DMA_EVT_ENABLE == event_state || DMA_EVT_ROUND == event_state)
    {
        switch (half_chan)
        {
            case DMA_SOURCE_HALF_CHANNEL:
                if (g_dma_controller_desc[event_group].src_device_config[src_pipe_num].no_of_links == MASK_BIT0)
                {
                    g_dma_controller_desc[event_group].src_device_config[src_pipe_num].log_lli_state = DMA_APPEND_LLI_STATE;
                }
                else if ((g_dma_controller_desc[event_group].src_device_config[src_pipe_num].no_of_links > MASK_BIT0))
                {
                    dma_error = dma_GetLogLinkStatus(pipe_desc, DMA_SOURCE_HALF_CHANNEL, &log_link_addr);
                    if (log_link_addr == DMA_NULL_ADDRESS)
                    {
                        g_dma_controller_desc[event_group].src_device_config[src_pipe_num].no_of_links = MASK_BIT0;
                        g_dma_controller_desc[event_group].src_device_config[src_pipe_num].log_lli_state = DMA_APPEND_LLI_STATE;
                    }
                    else
                    {
                        g_dma_controller_desc[event_group].src_device_config[src_pipe_num].log_lli_state = DMA_QUEUE_LLI_STATE;
                    }
                }
                break;

            case DMA_DEST_HALF_CHANNEL:
                if (g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].no_of_links == MASK_BIT0)
                {
                    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].log_lli_state = DMA_APPEND_LLI_STATE;
                }
                else if ((g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].no_of_links > MASK_BIT0))
                {
                    dma_error = dma_GetLogLinkStatus(pipe_desc, DMA_DEST_HALF_CHANNEL, &log_link_addr);
                    if (log_link_addr == DMA_NULL_ADDRESS)
                    {
                        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].no_of_links = MASK_BIT0;
                        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].log_lli_state = DMA_APPEND_LLI_STATE;
                    }
                    else
                    {
                        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].log_lli_state = DMA_QUEUE_LLI_STATE;
                    }
                }
                break;
        }
    }
    else
    {
        dma_error = dma_ResetLogLLIState(pipe_desc);
    }

    return(dma_error);
}

/****************************************************************************/
/* NAME: t_dma_error DMA_ResetLogLLIState()                                 */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:    This routine resets the LLI state of the logical channel */
//* PARAMETERS:                                                             */
/* IN  :                                                                    */
/*      - t_dma_channel_config    :    channel descriptor                   */
/*                                                                          */
/* OUT :                                                                    */
/*                                                                          */
/* RETURN:      t_dma_error                                                 */
//*-------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE t_dma_error dma_ResetLogLLIState(IN t_dma_pipe_config pipe_desc)
{
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_pipe_num      src_pipe_num, dest_pipe_num;

    DBGENTER0();

    src_pipe_num = pipe_desc.src_pipe_id;
    dest_pipe_num = pipe_desc.dest_pipe_id;

    g_dma_controller_desc[event_group].src_device_config[src_pipe_num].log_lli_state = DMA_INIT_LLI_STATE;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].log_lli_state = DMA_INIT_LLI_STATE;

    g_dma_controller_desc[event_group].src_device_config[src_pipe_num].no_of_links = NULL;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].no_of_links = NULL;
    g_dma_controller_desc[event_group].src_device_config[src_pipe_num].link_id = NULL;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].link_id = NULL;

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  dma_UpdateSrcBufferPhyMode                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine updates the source half channel parameters into the DMA       */
/*                channel registers and also updates the link parameters in Phy mode       */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    t_dma_channel_config: channel configuration                                      */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_UpdateSrcBufferPhyMode(IN t_dma_channel_config chan_desc, IN t_dma_pipe_config pipe_desc)
{
    t_dma_error         dma_error = DMA_OK;
    t_uint8             event_num;
    t_uint32            max_transfer_in_one_link;
	/*coverity[var_decl]*/
    t_dma_sxcfg_config  sxcfg_config;
    /*coverity[var_decl]*/
    t_dma_sxelt_config  sxelt_config;
    /*coverity[var_decl]*/
    t_dma_sxlnk_config  sxlnk_config;

    t_dma_pipe_num      pipe_id = pipe_desc.src_pipe_id;
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;
    t_dma_relink_desc   *p_prev_lli_desc;

    dma_error = dma_GetPhyLLIState(chan_desc, DMA_SOURCE_HALF_CHANNEL);

    switch (g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state)
    {
        case DMA_INIT_LLI_STATE:
            max_transfer_in_one_link =
                (
                    MASK_BIT0 <<
                    (t_uint8) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width
                ) *
                g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count;

            /* Do Post Link Operation if transfer is completed without relink*/
            if (g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size <= max_transfer_in_one_link)
            {
                /* Set Channel Parameters  */
                sxcfg_config.master_port = (t_dma_master_port) g_dma_controller_desc[event_group].src_device_config[pipe_id].master_port;
                sxcfg_config.tc_intr_mask = (t_dma_interrupt_mask) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_tc_intr;
                sxcfg_config.err_intr_mask = (t_dma_interrupt_mask) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_err_intr;
                sxcfg_config.pen_incr = (t_dma_pen_incr) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_pen;
                sxcfg_config.packet_size = (t_dma_packet_size) g_dma_controller_desc[event_group].src_device_config[pipe_id].burst_size;
                sxcfg_config.element_size = (t_dma_element_size) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width;
                sxcfg_config.priority_level = (t_dma_pr_priority) g_dma_controller_desc[event_group].src_device_config[pipe_id].priority;
                sxcfg_config.switch_endian = (t_dma_endianess) g_dma_controller_desc[event_group].src_device_config[pipe_id].endian;
                sxcfg_config.transfer_mode = (t_dma_transfer_mode) g_dma_controller_desc[event_group].src_device_config[pipe_id].tx_mode;
                sxcfg_config.event_line = g_dma_controller_desc[event_group].src_device_config[pipe_id].event_line;

                /* Set Channel Configuration Parameters*/
				/*coverity[uninit_use_in_call]*/
                dmaBase_SetChannelConfig
                (
                    (t_dma_channel) chan_num,
                    (t_dma_pr_mode) chan_desc.mode,
                    DMA_SRC_HALF_CHAN,
                    &sxcfg_config
                );

                sxelt_config.element_counter = (t_uint16) (g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count);
                sxelt_config.element_index = g_dma_controller_desc[event_group].src_device_config[pipe_id].element_index;

                dmaBase_SetChannelElement
                (
                    (t_dma_channel) chan_num,
                    (t_dma_pr_mode) chan_desc.mode,
                    DMA_SRC_HALF_CHAN,
                    &sxelt_config
                );

                sxlnk_config.link_addr = NULL;
                sxlnk_config.pre_link = RELINK_AFTER_TRANSFER;
                sxlnk_config.link_master_port = DMA_MASTER_PORT_0;
                sxlnk_config.tcp_assert = (t_dma_tcp_assert) g_dma_controller_desc[event_group].src_device_config[pipe_id].tcp_assert;

                dmaBase_SetChannelLink((t_dma_channel) chan_num, DMA_SRC_HALF_CHAN, &sxlnk_config);

                dmaBase_SetChannelPointer
                (
                    (t_dma_channel) chan_num,
                    DMA_SRC_HALF_CHAN,
                    g_dma_controller_desc[event_group].src_device_config[pipe_id].address
                );

                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num = MASK_BIT0;
                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state = DMA_APPEND_LLI_STATE;
            }
            else    /* prelink operation for the transfer size > max transfer without relink*/
            {
                /* Copy the Relink Parameters in the external memory */
                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num = MASK_BIT0;
                dma_error = dma_SetPhyRelinkParameters(&pipe_desc, DMA_SOURCE_HALF_CHANNEL);
                if (DMA_OK != dma_error)
                {
                    return(dma_error);
                }

                /* configure the link register */
                sxlnk_config.link_addr = g_dma_controller_desc[event_group].src_device_config[pipe_id].link_addr.chan_link_phy_ptr;
                sxlnk_config.pre_link = RELINK_AFTER_TRANSFER;
                sxlnk_config.link_master_port = DMA_MASTER_PORT_0;
                sxlnk_config.tcp_assert = (t_dma_tcp_assert) g_dma_controller_desc[event_group].src_device_config[pipe_id].tcp_assert;;
                dmaBase_SetChannelLink((t_dma_channel) chan_num, DMA_SRC_HALF_CHAN, &sxlnk_config);

                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state = DMA_QUEUE_LLI_STATE;
            }

            /* Set Event Line security for Non Memory devices*/
            if (g_dma_controller_desc[event_group].src_device_config[pipe_id].event_group != DMA_NO_EVENT_GROUP)
            {
                event_num = g_dma_controller_desc[event_group].src_device_config[pipe_id].event_num;
               
           #if defined(ST_8500ED) || defined(ST_8500V1)|| defined(ST_8500V2) || defined(__PEPS_8500)|| defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2)
               
                dmaBase_SetEventLineSecurity
                (
                    (t_dma_pr_security) chan_desc.security,
                    DMA_SRC_EVT_LINE,
                    (t_dma_event) event_num
                );
                #endif 
            }
            break;

        case DMA_APPEND_LLI_STATE:
            dma_error = dma_SetPhyRelinkParameters(&pipe_desc, DMA_SOURCE_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }

            sxlnk_config.link_addr = g_dma_controller_desc[event_group].src_device_config[pipe_id].link_addr.chan_link_phy_ptr;
            sxlnk_config.pre_link = RELINK_AFTER_TRANSFER;
            sxlnk_config.link_master_port = DMA_MASTER_PORT_0;
            sxlnk_config.tcp_assert = (t_dma_tcp_assert) g_dma_controller_desc[event_group].src_device_config[pipe_id].tcp_assert;;
            dmaBase_SetChannelLink((t_dma_channel) chan_num, DMA_SRC_HALF_CHAN, &sxlnk_config);

            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.src_phy_lli_state = DMA_QUEUE_LLI_STATE;

            break;

        case DMA_QUEUE_LLI_STATE:
            dma_error = dma_SetPhyRelinkParameters(&pipe_desc, DMA_SOURCE_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }

            p_prev_lli_desc = (t_dma_relink_desc *) (g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_ptr) - MASK_BIT0;
            p_prev_lli_desc->ChanLink |= (((t_uint32) (p_prev_lli_desc + MASK_BIT0) - g_dma_relink_config.log_to_phy_offset) & DMA_SxLNK_LINK_MASK);

            break;

        case DMA_RELINK_LLI_STATE:
        case DMA_FREEZE_LLI_STATE:
            break;
    }
	/*coverity[self_assign]*/
	event_num = event_num; /*to remove the warning */
	
    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  dma_UpdateSrcBufferLogMode                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine updates the source half channel parameters into the DMA       */
/*                channel registers and also updates the link parameters in Log mode       */
/*                                                                                         */
/* PARAMETERS:                                                                             */
/* IN:    t_dma_channel_config: channel configuration                                      */
/*                                                                                         */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_UpdateSrcBufferLogMode(IN t_dma_channel_config chan_desc, IN t_dma_pipe_config pipe_desc)
{
    t_dma_error         dma_error = DMA_OK;
    t_uint8             event_num;
	/*coverity[var_decl]*/
    t_dma_sxcfg_config  sxcfg_config;
    /*coverity[var_decl]*/
    t_dma_sxelt_config  sxelt_config;

    t_dma_pipe_num      pipe_id = pipe_desc.src_pipe_id;
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_event_type    event_type;
   
    dma_error = dma_GetLogLLIState(pipe_desc, DMA_SOURCE_HALF_CHANNEL);

    switch (g_dma_controller_desc[event_group].src_device_config[pipe_id].log_lli_state)
    {
        case DMA_INIT_LLI_STATE:
            sxcfg_config.priority_level = (t_dma_pr_priority) g_dma_controller_desc[event_group].src_device_config[pipe_id].priority;
            sxcfg_config.switch_endian = (t_dma_endianess) g_dma_controller_desc[event_group].src_device_config[pipe_id].endian;
            sxcfg_config.global_intr_mask = INTR_ENABLE;
            sxcfg_config.master_par_fetch = DMA_MASTER_PORT_0;
            
            /*coverity[uninit_use_in_call]*/
            dmaBase_SetChannelConfig
            (
                (t_dma_channel) chan_num,
                (t_dma_pr_mode) chan_desc.mode,
                DMA_SRC_HALF_CHAN,
                &sxcfg_config
            );
           
            
            
            sxelt_config.link_index = chan_num;
            	
            /*coverity[uninit_use_in_call]*/	
            dmaBase_SetChannelElement
            (
                (t_dma_channel) chan_num,
                (t_dma_pr_mode) chan_desc.mode,
                DMA_SRC_HALF_CHAN,
                &sxelt_config
                
            );
           
            /* Set Event Line and security */
            event_num = g_dma_controller_desc[event_group].src_device_config[pipe_id].event_num;
            event_type = g_dma_controller_desc[event_group].src_device_config[pipe_id].event_type;
           
           #if defined(ST_8500ED) || defined(ST_8500V1)|| defined(ST_8500V2) || defined(__PEPS_8500) || defined(__PEPS_8500_V1) || defined(__PEPS_8500_V2)
            
            dmaBase_SetEventLineSecurity
            (
                (t_dma_pr_security) chan_desc.security,
                (t_dma_evt_line_type) event_type,
                (t_dma_event) event_num
            );
            #endif 
            
            dmaBase_SetLogChannelEvent
            (
                (t_dma_channel) chan_num,
                (t_dma_evt_line_type) event_type,
                (t_dma_event) event_num,
                DMA_EVT_ENABLE
            );

            /* Configure the Channel Parameters in the external memory */
            dma_error = dma_SetLogChannelParameters(&pipe_desc, DMA_SOURCE_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }
            break;

        case DMA_APPEND_LLI_STATE:
            /* Set the SLOS relink index */
            dma_error = dma_SetLogChannelParameters(&pipe_desc, DMA_SOURCE_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }

            dma_error = dma_SetLogRelinkParameters(&pipe_desc, DMA_SOURCE_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }
            break;

        case DMA_QUEUE_LLI_STATE:
            dma_error = dma_SetLogRelinkParameters(&pipe_desc, DMA_SOURCE_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }
            break;
    }

    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  dma_UpdateDestBufferPhyMode                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine updates the destination half channel parameters into the DMA  */
/*                channel registers and also updates the link parameters.                  */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_lli_desc : starting address of the relink memory.                             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_UpdateDestBufferPhyMode(IN t_dma_channel_config chan_desc, IN t_dma_pipe_config pipe_desc)
{
    t_dma_error         dma_error = DMA_OK;
    t_uint8             event_num;
    t_uint32            max_transfer_in_one_link;

    t_dma_sxcfg_config  sxcfg_config;
    t_dma_sxelt_config  sxelt_config;
    t_dma_sxlnk_config  sxlnk_config;

    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_pipe_num      pipe_id = pipe_desc.dest_pipe_id;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;
    t_dma_relink_desc   *p_prev_lli_desc;

    dma_error = dma_GetPhyLLIState(chan_desc, DMA_DEST_HALF_CHANNEL);

    switch (g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state)
    {
        case DMA_INIT_LLI_STATE:
            max_transfer_in_one_link =
                (
                    MASK_BIT0 <<
                    (t_uint8) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width
                ) *
                g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count;

            /* Do Post Link Operation if transfer is completed without relink*/
            if (g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size <= max_transfer_in_one_link)
            {
                /* Set Channel Parameters */
                sxcfg_config.master_port = (t_dma_master_port) g_dma_controller_desc[event_group].dest_device_config[pipe_id].master_port;
                sxcfg_config.tc_intr_mask = (t_dma_interrupt_mask) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_tc_intr;
                sxcfg_config.err_intr_mask = (t_dma_interrupt_mask) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_err_intr;
                sxcfg_config.pen_incr = (t_dma_pen_incr) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_pen;
                sxcfg_config.packet_size = (t_dma_packet_size) g_dma_controller_desc[event_group].dest_device_config[pipe_id].burst_size;
                sxcfg_config.element_size = (t_dma_element_size) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width;
                sxcfg_config.priority_level = (t_dma_pr_priority) g_dma_controller_desc[event_group].dest_device_config[pipe_id].priority;
                sxcfg_config.switch_endian = (t_dma_endianess) g_dma_controller_desc[event_group].dest_device_config[pipe_id].endian;
                sxcfg_config.transfer_mode = (t_dma_transfer_mode) g_dma_controller_desc[event_group].dest_device_config[pipe_id].tx_mode;
                sxcfg_config.event_line = g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_line;

                /* Set Channel Configuration Parameters */
                dmaBase_SetChannelConfig
                (
                    (t_dma_channel) chan_num,
                    (t_dma_pr_mode) chan_desc.mode,
                    DMA_DEST_HALF_CHAN,
                    &sxcfg_config
                );

                sxelt_config.element_counter = (t_uint16) (g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count);

                sxelt_config.element_index = g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_index;

                dmaBase_SetChannelElement
                (
                    (t_dma_channel) chan_num,
                    (t_dma_pr_mode) chan_desc.mode,
                    DMA_DEST_HALF_CHAN,
                    &sxelt_config
                );

                sxlnk_config.link_addr = NULL;
                sxlnk_config.pre_link = RELINK_AFTER_TRANSFER;
                sxlnk_config.link_master_port = DMA_MASTER_PORT_0;
                sxlnk_config.tcp_assert = (t_dma_tcp_assert) g_dma_controller_desc[event_group].dest_device_config[pipe_id].tcp_assert;;

                dmaBase_SetChannelLink((t_dma_channel) chan_num, DMA_DEST_HALF_CHAN, &sxlnk_config);

                dmaBase_SetChannelPointer
                (
                    (t_dma_channel) chan_num,
                    DMA_DEST_HALF_CHAN,
                    g_dma_controller_desc[event_group].dest_device_config[pipe_id].address
                );

                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num = MASK_BIT0;
                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state = DMA_APPEND_LLI_STATE;
            }
            else    /* prelink operation for the transfer size > max transfer without relink */
            {
                /* Copy the Relink Parameters in the external memory */
                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num = MASK_BIT0;
                dma_error = dma_SetPhyRelinkParameters(&pipe_desc, DMA_DEST_HALF_CHANNEL);
                if (DMA_OK != dma_error)
                {
                    DBGEXIT0(dma_error);
                    return(dma_error);
                }

                sxlnk_config.link_addr = g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_addr.chan_link_phy_ptr;
                sxlnk_config.pre_link = RELINK_AFTER_TRANSFER;
                sxlnk_config.link_master_port = DMA_MASTER_PORT_0;
                sxlnk_config.tcp_assert = (t_dma_tcp_assert) g_dma_controller_desc[event_group].dest_device_config[pipe_id].tcp_assert;;

                /* Load the Relink Address in Channel Register DMAC_SxPTR */
                dmaBase_SetChannelLink((t_dma_channel) chan_num, DMA_DEST_HALF_CHAN, &sxlnk_config);

                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state = DMA_QUEUE_LLI_STATE;
            }

            /* Set Event Line security for Non Memory devices*/
            if (g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_group != DMA_NO_EVENT_GROUP)
            {
                event_num = g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_num;
               
               #if defined(ST_8500ED) || defined(ST_8500V1) || defined(ST_8500V2)|| defined(__PEPS_8500) || defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2)
               
                dmaBase_SetEventLineSecurity
                (
                    (t_dma_pr_security) chan_desc.security,
                    DMA_DEST_EVT_LINE,
                    (t_dma_event) event_num
                );
                #endif 
            }
            break;

        case DMA_APPEND_LLI_STATE:
            /* Copy the Relink Parameters in the external memory */
            dma_error = dma_SetPhyRelinkParameters(&pipe_desc, DMA_DEST_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                DBGEXIT0(dma_error);
                return(dma_error);
            }

            sxlnk_config.link_addr = g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_addr.chan_link_phy_ptr;
            sxlnk_config.pre_link = RELINK_AFTER_TRANSFER;
            sxlnk_config.link_master_port = DMA_MASTER_PORT_0;
            sxlnk_config.tcp_assert = (t_dma_tcp_assert) g_dma_controller_desc[event_group].dest_device_config[pipe_id].tcp_assert;;

            dmaBase_SetChannelLink((t_dma_channel) chan_num, DMA_DEST_HALF_CHAN, &sxlnk_config);

            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.dest_phy_lli_state = DMA_QUEUE_LLI_STATE;

            break;

        case DMA_QUEUE_LLI_STATE:
            dma_error = dma_SetPhyRelinkParameters(&pipe_desc, DMA_DEST_HALF_CHANNEL);

            if (DMA_OK != dma_error)
            {
                DBGEXIT0(dma_error);
                return(dma_error);
            }

            p_prev_lli_desc = (t_dma_relink_desc *) (g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_ptr) - MASK_BIT0;
            p_prev_lli_desc->ChanLink |= (((t_uint32) (p_prev_lli_desc + MASK_BIT0) - g_dma_relink_config.log_to_phy_offset) & DMA_SxLNK_LINK_MASK);

            break;
    }
	/*coverity[self_assign]*/
	event_num = event_num; /*to remove the warning */
	
    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  dma_UpdateDestBufferLogMode                                                      */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine updates the destination half channel parameters into the DMA  */
/*                channel registers and also updates the link parameters in Log Mode       */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_lli_desc : starting address of the relink memory.                             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_UpdateDestBufferLogMode(IN t_dma_channel_config chan_desc, IN t_dma_pipe_config pipe_desc)
{
    t_dma_error         dma_error = DMA_OK;
    t_uint8             event_num;
    t_dma_sxcfg_config  sxcfg_config;
    t_dma_sxelt_config  sxelt_config;
    t_dma_chan_num      chan_num = pipe_desc.chan_num;
    t_dma_pipe_num      pipe_id = pipe_desc.dest_pipe_id;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_event_type    event_type;

    dma_error = dma_GetLogLLIState(pipe_desc, DMA_DEST_HALF_CHANNEL);

    switch (g_dma_controller_desc[event_group].dest_device_config[pipe_id].log_lli_state)
    {
        case DMA_INIT_LLI_STATE:
            sxcfg_config.priority_level = (t_dma_pr_priority) g_dma_controller_desc[event_group].dest_device_config[pipe_id].priority;
            sxcfg_config.switch_endian = (t_dma_endianess) g_dma_controller_desc[event_group].dest_device_config[pipe_id].endian;
            sxcfg_config.global_intr_mask = INTR_ENABLE;
            sxcfg_config.master_par_fetch = DMA_MASTER_PORT_0;
            dmaBase_SetChannelConfig
            (
                (t_dma_channel) chan_num,
                (t_dma_pr_mode) chan_desc.mode,
                DMA_DEST_HALF_CHAN,
                &sxcfg_config
            );

            sxelt_config.link_index = chan_num;

            dmaBase_SetChannelElement
            (
                (t_dma_channel) chan_num,
                (t_dma_pr_mode) chan_desc.mode,
                DMA_DEST_HALF_CHAN,
                &sxelt_config
            );

            /* Set Event Line and security */
            event_num = g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_num;
            event_type = g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_type;
            
            #if defined(ST_8500ED) || defined(ST_8500V1)|| defined(ST_8500V2) || defined(__PEPS_8500)|| defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2)
            
            dmaBase_SetEventLineSecurity
            (
                (t_dma_pr_security) chan_desc.security,
                (t_dma_evt_line_type) event_type,
                (t_dma_event) event_num
            );
            #endif 
            dmaBase_SetLogChannelEvent
            (
                (t_dma_channel) chan_num,
                (t_dma_evt_line_type) event_type,
                (t_dma_event) event_num,
                DMA_EVT_ENABLE
            );

            dma_error = dma_SetLogChannelParameters(&pipe_desc, DMA_DEST_HALF_CHANNEL);

            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }
            break;

        case DMA_APPEND_LLI_STATE:
            /* Set the DLOS relink index */
            dma_error = dma_SetLogChannelParameters(&pipe_desc, DMA_DEST_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }

            dma_error = dma_SetLogRelinkParameters(&pipe_desc, DMA_DEST_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }
            break;

        case DMA_QUEUE_LLI_STATE:
            dma_error = dma_SetLogRelinkParameters(&pipe_desc, DMA_DEST_HALF_CHANNEL);
            if (DMA_OK != dma_error)
            {
                return(dma_error);
            }
            break;
    }

    return(dma_error);
}

/*****************************************************************************/
/* NAME: t_dma_error dma_CheckPipeAvailability()                             */
/*-------------------------------------------------------------------------- */
/* DESCRIPTION: This routine returns the free channel available. The channel */
/*                is considered free when it is in  STOP state and not in    */
/*                suspended state.                                           */
/*                                                                           */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN  :                                                                     */
/*      - event_group:    Event group of the DMA device                      */
/*                                                                           */
/* OUT : none                                                                */
/*                                                                           */
/* RETURN:                                                                   */
/*        t_dma_error                                                        */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PRIVATE t_dma_error dma_CheckPipeAvailability
(
    IN t_dma_evt_grp_num    event_group,
    IN volatile t_dma_pipe_num       *p_src_pipe,
    IN volatile t_dma_pipe_num       *p_dest_pipe
)
{
    t_uint32    i;

    /* Check for the Source Pipe */
    if (*p_src_pipe == DMA_PIPE_DIVISOR)  /* Device is DMA_MEMORY_SRC */
    {
        for (i = NULL; i < DMA_PIPE_DIVISOR; i++)
        {
            if (!g_dma_controller_desc[event_group].src_device_config[*p_src_pipe].is_device_open)
            {
                break;
            }
            (*p_src_pipe)++;
        }

        if (i == DMA_PIPE_DIVISOR)
        {
            return(DMA_NO_MEMORY_PIPES_AVAILABLE);
        }
    }
    else
    {
        if (g_dma_controller_desc[event_group].src_device_config[*p_src_pipe].is_device_open)
        {
            return(DMA_DEVICE_ALREADY_USED);
        }
    }

    /* Check for the Destination Pipe */
    if (*p_dest_pipe == DMA_PIPE_DIVISOR) /* Device is DMA_MEMORY_DEST */
    {
        for (i = NULL; i < DMA_PIPE_DIVISOR; i++)
        {
            if (!g_dma_controller_desc[event_group].dest_device_config[*p_dest_pipe].is_device_open)
            {
                break;
            }
            (*p_dest_pipe)++;
        }

        if (i == DMA_PIPE_DIVISOR)
        {
            return(DMA_NO_MEMORY_PIPES_AVAILABLE);
        }
    }
    else
    {
        if (g_dma_controller_desc[event_group].dest_device_config[*p_dest_pipe].is_device_open)
        {
            return(DMA_DEVICE_ALREADY_USED);
        }
    }

    return(DMA_OK);
}

/*****************************************************************************/
/* NAME: t_dma_error dma_EventGroupRegisterPipe()                            */
/*-------------------------------------------------------------------------- */
/* DESCRIPTION: This routine registers the pipe based on the event group     */
/*                of both the source and the destination devices             */
/*                                                                           */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN  :                                                                     */
/*      - event_group:    Event group of the DMA device                      */
/*                                                                           */
/* OUT : none                                                                */
/*                                                                           */
/* RETURN:                                                                   */
/*        t_dma_error                                                        */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PRIVATE t_dma_error dma_EventGroupRegisterPipe
(
    IN t_dma_channel_config chan_desc,
    IN t_dma_evt_grp_num    event_group,
    IN t_dma_pipe_num       src_pipe_id,
    IN t_dma_pipe_num       dest_pipe_id,
    IN t_dma_src_device     src_device,
    IN t_dma_dest_device    dest_device,
    IN t_dma_event_group    src_event_group,
    IN t_dma_event_group    dest_event_group
)
{
    if ((src_event_group == DMA_NO_EVENT_GROUP) && (dest_event_group == DMA_NO_EVENT_GROUP))
    {
        if (DMA_PHYSICAL == chan_desc.mode)
        {
            g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_num = DMA_NO_EVENT_REQUEST;
            g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_num = DMA_NO_EVENT_REQUEST;
        }
        else
        {
            DBGEXIT0(DMA_UNSUPPORTED_TRANSFER);
            return(DMA_UNSUPPORTED_TRANSFER);
        }
    }
    else if ((src_event_group == DMA_NO_EVENT_GROUP) && (dest_event_group != DMA_NO_EVENT_GROUP))
    {
        g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_type = DMA_DEST_EVENT;
        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_type = DMA_DEST_EVENT;
        g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_num = (t_uint8) dest_device;
        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_num = (t_uint8) dest_device;
    }
    else if ((src_event_group != DMA_NO_EVENT_GROUP) && (dest_event_group == DMA_NO_EVENT_GROUP))
    {
        g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_type = DMA_SRC_EVENT;
        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_type = DMA_SRC_EVENT;
        g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_num = (t_uint8) src_device;
        g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_num = (t_uint8) src_device;
    }
    else if ((src_event_group != DMA_NO_EVENT_GROUP) && (dest_event_group != DMA_NO_EVENT_GROUP))
    {
        if ((src_event_group == dest_event_group))
        {
            if (DMA_PHYSICAL == chan_desc.mode)
            {
                g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_type = DMA_SRC_EVENT;
                g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_type = DMA_DEST_EVENT;
                g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_num = (t_uint8) src_device;
                g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_num = (t_uint8) dest_device;
            }
            else
            {
                DBGEXIT0(DMA_UNSUPPORTED_TRANSFER);
                return(DMA_UNSUPPORTED_TRANSFER);
            }
        }
        else
        {
            DBGEXIT0(DMA_UNSUPPORTED_TRANSFER);
            return(DMA_UNSUPPORTED_TRANSFER);
        }
    }

    g_dma_controller_desc[event_group].src_device_config[src_pipe_id].device = (t_dma_device) src_device;
    g_dma_controller_desc[event_group].src_device_config[src_pipe_id].event_group = src_event_group;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].device = (t_dma_device) dest_device;
    g_dma_controller_desc[event_group].dest_device_config[dest_pipe_id].event_group = dest_event_group;

    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  dma_SetPhyRelinkParameters                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine generates the relink parameters and stores in the external    */
/*                memory in the form of linked list.                                       */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_lli_desc : starting address of the relink memory.                             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_SetPhyRelinkParameters(IN t_dma_pipe_config *p_pipe_desc, IN t_dma_half_chan half_channel)
{
    t_dma_error dma_error = DMA_OK;

    switch (half_channel)
    {
        case DMA_SOURCE_HALF_CHANNEL:
            dma_error = dma_SetPhyRelinkParamSrcHalfChan(p_pipe_desc);
            break;

        case DMA_DEST_HALF_CHANNEL:
            dma_error = dma_SetPhyRelinkParamDestHalfChan(p_pipe_desc);
            break;
    }

    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  dma_SetPhyRelinkParamSrcHalfChan                                                 */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine generates the relink parameters and stores in the external    */
/*                memory in the form of linked list for the source half channel            */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_lli_desc : starting address of the relink memory.                             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_SetPhyRelinkParamSrcHalfChan(IN t_dma_pipe_config *p_pipe_desc)
{
    t_dma_pipe_num      pipe_id;
    t_uint32            unit_tx_size;
    t_uint32            i, num_of_links;
    t_uint32            tx_count;
    t_uint32            element_count, total_elem_count, max_elem_count, rem_elem_count;
    t_dma_relink_desc   *p_lli_desc;

    t_dma_chan_num      chan_num = p_pipe_desc->chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;

    pipe_id = p_pipe_desc->src_pipe_id;

    /* calculate for physical address */
    g_dma_controller_desc[event_group].src_device_config[pipe_id].link_addr.chan_link_phy_ptr = (t_physical_address)
        (
            (t_uint32) g_dma_relink_config.phy_chan_param_phy_addr +
            (DMA_ONE_EVT_GRP_MEM_SIZE * event_group) +
            DMA_ONE_PHY_CHAN_MEM_SIZE *
            chan_index
        );

    /* calculate for logical address */
    g_dma_controller_desc[event_group].src_device_config[pipe_id].link_addr.chan_link_log_ptr = (t_logical_address)
        (
            (t_uint32) g_dma_relink_config.phy_chan_param_log_addr +
            (DMA_ONE_EVT_GRP_MEM_SIZE * event_group) +
            DMA_ONE_PHY_CHAN_MEM_SIZE *
            chan_index
        );

    p_lli_desc = (t_dma_relink_desc *)
        (
            (g_dma_controller_desc[event_group].src_device_config[pipe_id].link_addr.chan_link_log_ptr) +
                (
                    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num -
                    MASK_BIT0
                ) *
                        DMA_PHY_CHAN_SIZE_OF_LINK
        );

    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_ptr = (t_uint32) p_lli_desc;

    /* data transfered in one block i.e without relink    */
    unit_tx_size = (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width) * g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count;

    /* Calculate the number of links required */
    num_of_links = g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size / unit_tx_size;
    if ((g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size % unit_tx_size) > NULL)
    {
        num_of_links = num_of_links + MASK_BIT0;
    }

    if
    (
        DMA_PHY_NO_OF_LINKS_MAX >=
            (
                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num +
                num_of_links
            )
    )
    {
        /* Calculate tx_count and element count */
        tx_count = g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size;
        total_elem_count = tx_count / (t_uint8) (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width);
        max_elem_count = g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count;
        rem_elem_count = total_elem_count;

        for (i = NULL; i < num_of_links; i++)
        {
            element_count = (rem_elem_count >= max_elem_count) ? max_elem_count : rem_elem_count;

            p_lli_desc[i].ChanConfig =
                (
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].master_port <<
                    DMA_SxCFG_MST_SHIFT
                );

            if (DMA_RELINK == g_dma_controller_desc[event_group].src_device_config[pipe_id].intr_gen)
            {
                p_lli_desc[i].ChanConfig |=
                    (
                        (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_tc_intr <<
                        DMA_SxCFG_TIM_SHIFT
                    );

                /*                p_lli_desc[i].ChanConfig    |=    ((t_uint32)g_dma_controller_desc[event_group].src_device_config[pipe_id].is_err_intr <<     DMA_SxCFG_EIM_SHIFT); */
            }

            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_pen <<
                    DMA_SxCFG_PEN_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].burst_size <<
                    DMA_SxCFG_PSIZE_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width <<
                    DMA_SxCFG_ESIZE_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].priority <<
                    DMA_SxCFG_PRI_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].endian <<
                    DMA_SxCFG_LBE_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].tx_mode <<
                    DMA_SxCFG_TM_SHIFT
                );
            p_lli_desc[i].ChanConfig |= ((t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].event_line & DMA_SxCFG_EVTL_MASK);

            p_lli_desc[i].ChanElement = (element_count << DMA_SxELT_ECNT_SHIFT);
            p_lli_desc[i].ChanElement |= (g_dma_controller_desc[event_group].src_device_config[pipe_id].element_index & DMA_SxELT_EIDX_MASK);

            /* If address_increment is FALSE do no increment the address ( for peripherals to write in the DR */
            if (g_dma_controller_desc[event_group].src_device_config[pipe_id].is_addr_incr)
            {   /* address_increment is 0*/
                p_lli_desc[i].ChanPointer =
                    (
                        (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].address +
                        i *
                        unit_tx_size
                    );
            }
            else
            {
                p_lli_desc[i].ChanPointer = ((t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].address);
            }

            p_lli_desc[i].ChanLink = ((((t_uint32) & p_lli_desc[i + MASK_BIT0].ChanConfig) - g_dma_relink_config.log_to_phy_offset) & DMA_SxLNK_LINK_MASK);
            p_lli_desc[i].ChanLink |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].relink_ctrl <<
                    DMA_SxLNK_PRE_SHIFT
                );

            //            p_lli_desc[i].ChanLink      |=    (p_sxlnk_config->link_master_port     <<         DMA_SxLNK_LMP_SHIFT);
            p_lli_desc[i].ChanLink |= (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].tcp_assert;

            rem_elem_count = rem_elem_count - element_count;

            g_dma_controller_desc[event_group].src_device_config[pipe_id].curr_addr += unit_tx_size;
            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_src_lli_num++;
        }

        /* Enable Interrupt for the src last link. */
        if (DMA_EOT == g_dma_controller_desc[event_group].src_device_config[pipe_id].intr_gen)
        {
            p_lli_desc[num_of_links - MASK_BIT0].ChanConfig |=
                    (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_tc_intr <<
                DMA_SxCFG_TIM_SHIFT;

            //            p_lli_desc[num_of_links-1].ChanConfig    |=    (t_uint32)g_dma_controller_desc[event_group].src_device_config[pipe_id].is_err_intr << DMA_SxCFG_EIM_SHIFT;
        }

        /* point last link to zero to stop relink */
        p_lli_desc[num_of_links - MASK_BIT0].ChanLink &= 0x00000007;    /* 0x7 is to Mask Last three bits*/
    }
    else
    {
        return(DMA_NO_MORE_LLIS_AVAILABLE);
    }

    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  dma_SetPhyRelinkParamDestHalfChan                                                 */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine generates the relink parameters and stores in the external    */
/*                memory in the form of linked list for the destination half channel       */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_lli_desc : starting address of the relink memory.                             */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_SetPhyRelinkParamDestHalfChan(IN t_dma_pipe_config *p_pipe_desc)
{
    t_dma_pipe_num      pipe_id;
    t_uint32            unit_tx_size;
    t_uint32            i, num_of_links;
    t_uint32            tx_count;
    t_uint32            element_count, total_elem_count, max_elem_count, rem_elem_count;
    t_dma_relink_desc   *p_lli_desc;

    t_dma_chan_num      chan_num = p_pipe_desc->chan_num;
    t_dma_evt_grp_num   event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index    chan_index = chan_num % DMA_DIVISOR;

    pipe_id = p_pipe_desc->dest_pipe_id;

    /* calculate for physical address */
    g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_addr.chan_link_phy_ptr = (t_physical_address)
        (
            (t_uint32) g_dma_relink_config.phy_chan_param_phy_addr + (DMA_ONE_EVT_GRP_MEM_SIZE * event_group) +
                (DMA_ONE_PHY_CHAN_MEM_SIZE * chan_index) + DMA_PHY_CHAN_DEST_MEM_OFFSET
        );

    /* calculate for logical address */
    g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_addr.chan_link_log_ptr = (t_logical_address)
        (
            (t_uint32) g_dma_relink_config.phy_chan_param_log_addr + (DMA_ONE_EVT_GRP_MEM_SIZE * event_group) +
                (DMA_ONE_PHY_CHAN_MEM_SIZE * chan_index) + DMA_PHY_CHAN_DEST_MEM_OFFSET
        );

    p_lli_desc = (t_dma_relink_desc *)
        (
            g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_addr.chan_link_log_ptr +
                (
                    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num -
                    MASK_BIT0
                ) *
                        DMA_PHY_CHAN_SIZE_OF_LINK
        );

    g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_ptr = (t_uint32) p_lli_desc;

    /* data transfered in one block i.e without relink    */
    unit_tx_size = (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width) * g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count;

    /* Calculate the number of links required */
    num_of_links = g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size / unit_tx_size;
    if ((g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size % unit_tx_size) > NULL)
    {
        num_of_links = num_of_links + MASK_BIT0;
    }

    if
    (
        DMA_PHY_NO_OF_LINKS_MAX >=
            (
                g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num +
                num_of_links
            )
    )
    {
        /* Calculate tx_count and element count */
        tx_count = g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size;
        total_elem_count = tx_count / (t_uint8) (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width);
        max_elem_count = g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count;
        rem_elem_count = total_elem_count;

        for (i = NULL; i < num_of_links; i++)
        {
            element_count = (rem_elem_count >= max_elem_count) ? max_elem_count : rem_elem_count;

            p_lli_desc[i].ChanConfig =
                (
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].master_port <<
                    DMA_SxCFG_MST_SHIFT
                );

            if (DMA_RELINK == g_dma_controller_desc[event_group].dest_device_config[pipe_id].intr_gen)
            {
                p_lli_desc[i].ChanConfig |=
                    (
                        (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_tc_intr <<
                        DMA_SxCFG_TIM_SHIFT
                    );
                p_lli_desc[i].ChanConfig |=
                    (
                        (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_err_intr <<
                        DMA_SxCFG_EIM_SHIFT
                    );
            }

            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_pen <<
                    DMA_SxCFG_PEN_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].burst_size <<
                    DMA_SxCFG_PSIZE_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width <<
                    DMA_SxCFG_ESIZE_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].priority <<
                    DMA_SxCFG_PRI_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].endian <<
                    DMA_SxCFG_LBE_SHIFT
                );
            p_lli_desc[i].ChanConfig |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].tx_mode <<
                    DMA_SxCFG_TM_SHIFT
                );
            p_lli_desc[i].ChanConfig |= ((t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_line & DMA_SxCFG_EVTL_MASK);

            p_lli_desc[i].ChanElement = (element_count << DMA_SxELT_ECNT_SHIFT);
            p_lli_desc[i].ChanElement |= (g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_index & DMA_SxELT_EIDX_MASK);

            /* If address_increment is FALSE do no increment the address ( for peripherals to write in the DR */
            if (g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_addr_incr)
            {
                p_lli_desc[i].ChanPointer =
                    (
                        (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].address +
                        unit_tx_size *
                        i
                    );
            }
            else
            {
                p_lli_desc[i].ChanPointer = ((t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].address);
            }

            p_lli_desc[i].ChanLink = ((((t_uint32) & p_lli_desc[i + MASK_BIT0].ChanConfig) - g_dma_relink_config.log_to_phy_offset) & DMA_SxLNK_LINK_MASK);
            p_lli_desc[i].ChanLink |=
                (
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].relink_ctrl <<
                    DMA_SxLNK_PRE_SHIFT
                );

            //              p_lli_desc[i].ChanLink  |=  (p_sxlnk_config->link_master_port     <<         DMA_SxLNK_LMP_SHIFT);
            p_lli_desc[i].ChanLink |= (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].tcp_assert;

            rem_elem_count = rem_elem_count - element_count;

            g_dma_controller_desc[event_group].dest_device_config[pipe_id].curr_addr += unit_tx_size;
            g_dma_controller_desc[event_group].channel_config[chan_index].phy_link_info.curr_dest_lli_num++;
        }

        /* Enable Interrupt for the last link. Interrupt will come only at the end of Transfer and not at relink */
        if (DMA_EOT == g_dma_controller_desc[event_group].dest_device_config[pipe_id].intr_gen)
        {
            p_lli_desc[num_of_links - MASK_BIT0].ChanConfig |=
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_tc_intr <<
                DMA_SxCFG_TIM_SHIFT;
            p_lli_desc[num_of_links - MASK_BIT0].ChanConfig |=
                    (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_tc_intr <<
                DMA_SxCFG_EIM_SHIFT;
        }

        /* point last link to zero    to stop relink */
        p_lli_desc[num_of_links - MASK_BIT0].ChanLink &= 0x00000007;    /* 0x7 is to Mask Last three bits*/
    }
    else
    {
        return(DMA_NO_MORE_LLIS_AVAILABLE);
    }

    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  dma_SetLogChannelParameters                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine generates the relink parameters and stores in the external    */
/*                memory in the form of linked list.                                       */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_pipe_desc : pipe configuration.                                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_SetLogChannelParameters(IN t_dma_pipe_config *p_pipe_desc, IN t_dma_half_chan half_channel)
{
    t_dma_error             dma_error = DMA_OK;
    t_dma_pipe_num          pipe_id;
    t_uint32                unit_tx_size, num_of_links;
    t_uint32                log_param_offset;
    t_uint32                src_config, dest_config;

    t_std_log_memory_param  *p_std_log_mem_param;

    t_dma_chan_num          chan_num = p_pipe_desc->chan_num;
    t_dma_evt_grp_num       event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index        chan_index = chan_num % DMA_DIVISOR;

    switch (half_channel)
    {
        case DMA_SOURCE_HALF_CHANNEL:
            pipe_id = p_pipe_desc->src_pipe_id;

            /* P2M transfer is a source event and M2P transfer is a Destination Event */
            if (DMA_SRC_EVENT == g_dma_controller_desc[event_group].src_device_config[pipe_id].event_type)
            {
                log_param_offset = (32 * g_dma_controller_desc[event_group].src_device_config[pipe_id].event_num);
            }
            else
            {
                log_param_offset = (32 * g_dma_controller_desc[event_group].src_device_config[pipe_id].event_num) + DMA_PIPE_DIVISOR;
            }

            p_std_log_mem_param = (t_std_log_memory_param *) (g_dma_relink_config.log_chan_param_log_addr + log_param_offset);
            g_dma_controller_desc[event_group].src_device_config[pipe_id].link_addr.chan_param_log_ptr = (t_logical_address) p_std_log_mem_param;

            switch (g_dma_controller_desc[event_group].src_device_config[pipe_id].log_lli_state)
            {
                case DMA_INIT_LLI_STATE:
                    /* data transfered in one block i.e without relink */
                    unit_tx_size = (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width) * g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count;

                    /* Calculate the number of links required */
                    num_of_links = g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size / unit_tx_size;
                    if ((g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size % unit_tx_size) > NULL)
                    {
                        num_of_links = num_of_links + MASK_BIT0;
                    }

                    src_config =
                        (
                            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].master_port <<
                            DMA_SxCFG_MST_SHIFT
                        );
                    src_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_tc_intr <<
                            DMA_SxCFG_TIM_SHIFT
                        );
                    src_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_err_intr <<
                            DMA_SxCFG_EIM_SHIFT
                        );
                    src_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_addr_incr <<
                            DMA_SxCFG_INCR_SHIFT
                        );
                    src_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].burst_size <<
                            DMA_SxCFG_PSIZE_SHIFT
                        );
                    src_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width <<
                            DMA_SxCFG_ESIZE_SHIFT
                        );

                    g_dma_controller_desc[event_group].src_device_config[pipe_id].link_addr.chan_param_log_ptr = (t_logical_address) p_std_log_mem_param;

                    p_std_log_mem_param->dmac_lcsp0 =
                        (g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count) <<
                        DMA_LCSP0_ECNT_SHIFT;
                    p_std_log_mem_param->dmac_lcsp0 |= g_dma_controller_desc[event_group].src_device_config[pipe_id].address & DMA_LCSP0_PTRLSB_MASK;

                    p_std_log_mem_param->dmac_lcsp1 = g_dma_controller_desc[event_group].src_device_config[pipe_id].address & DMA_LCSP1_PTRMSB_MASK;
                    p_std_log_mem_param->dmac_lcsp1 |= src_config;
                    p_std_log_mem_param->dmac_lcsp1 &= DMA_SLOS_ZERO_MASK;  /* Set the SLOS to zero */
                    p_std_log_mem_param->dmac_lcsp1 |= (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].tcp_assert;

                    g_dma_controller_desc[event_group].src_device_config[pipe_id].link_id = g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset;
                    g_dma_controller_desc[event_group].src_device_config[pipe_id].no_of_links++;
                    g_dma_controller_desc[event_group].src_device_config[pipe_id].log_lli_state = DMA_APPEND_LLI_STATE;

                    /*----------------------- Set Relink Parameters for Source Half Channel-----------------------------*/
                    if (num_of_links > MASK_BIT0)
                    {
                        /* Set terminal count interrupt to zero intr gen is at End Of Transfer*/
                        if (DMA_EOT == g_dma_controller_desc[event_group].src_device_config[pipe_id].intr_gen)
                        {
                            p_std_log_mem_param->dmac_lcsp1 &= DMA_SET_LOG_TERM_PARA;
                        }

                        /*Set SLOS to relink index offset */
                        g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset++;
                        p_std_log_mem_param->dmac_lcsp1 |= g_dma_controller_desc[event_group].channel_config[chan_index].
                                log_link_info.link_offset <<
                            DMA_LCSP1_LOS_SHIFT;
                        g_dma_controller_desc[event_group].src_device_config[pipe_id].log_lli_state = DMA_RELINK_LLI_STATE;
                        dma_error = dma_SetLogRelinkParameters(p_pipe_desc, half_channel);
                    }
                    break;

                case DMA_APPEND_LLI_STATE:
                    /*Set SLOS to relink index offset */
                    g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset++;
                    p_std_log_mem_param->dmac_lcsp1 |= g_dma_controller_desc[event_group].channel_config[chan_index].
                            log_link_info.link_offset <<
                        DMA_LCSP1_LOS_SHIFT;
                    break;
            }
            break;

        case DMA_DEST_HALF_CHANNEL:
            pipe_id = p_pipe_desc->dest_pipe_id;

            /* Periheral to Memory Transfer is a Source Event */
            if (DMA_SRC_EVENT == g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_type)
            {
                log_param_offset = (32 * g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_num);
            }
            else
            {
                log_param_offset = (32 * g_dma_controller_desc[event_group].dest_device_config[pipe_id].event_num) + DMA_PIPE_DIVISOR;
            }

            p_std_log_mem_param = (t_std_log_memory_param *) (g_dma_relink_config.log_chan_param_log_addr + log_param_offset);
            g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_addr.chan_param_log_ptr = (t_logical_address) p_std_log_mem_param;

            switch (g_dma_controller_desc[event_group].dest_device_config[pipe_id].log_lli_state)
            {
                case DMA_INIT_LLI_STATE:
                    /* data transfered in one block i.e without relink    */
                    unit_tx_size = (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width) * g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count;

                    /* Calculate the number of links required */
                    num_of_links = g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size / unit_tx_size;
                    if ((g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size % unit_tx_size) > NULL)
                    {
                        num_of_links = num_of_links + MASK_BIT0;
                    }

                    dest_config =
                        (
                            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].master_port <<
                            DMA_SxCFG_MST_SHIFT
                        );
                    dest_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_tc_intr <<
                            DMA_SxCFG_TIM_SHIFT
                        );
                    dest_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_err_intr <<
                            DMA_SxCFG_EIM_SHIFT
                        );
                    dest_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_addr_incr <<
                            DMA_SxCFG_PEN_SHIFT
                        );
                    dest_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].burst_size <<
                            DMA_SxCFG_PSIZE_SHIFT
                        );
                    dest_config |=
                        (
                            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width <<
                            DMA_SxCFG_ESIZE_SHIFT
                        );

                    g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_addr.chan_param_log_ptr = (t_logical_address) p_std_log_mem_param;

                    p_std_log_mem_param->dmac_lcsp2 = g_dma_controller_desc[event_group].dest_device_config[pipe_id].
                            element_count <<
                        DMA_LCSP2_ECNT_SHIFT;
                    p_std_log_mem_param->dmac_lcsp2 |= g_dma_controller_desc[event_group].dest_device_config[pipe_id].address & DMA_LCSP2_PTRLSB_MASK;

                    p_std_log_mem_param->dmac_lcsp3 = g_dma_controller_desc[event_group].dest_device_config[pipe_id].address & DMA_LCSP3_PTRMSB_MASK;
                    p_std_log_mem_param->dmac_lcsp3 |= dest_config;
                    p_std_log_mem_param->dmac_lcsp3 &= DMA_DLOS_ZERO_MASK;  /* Set the DLOS to zero */
                    p_std_log_mem_param->dmac_lcsp3 |= (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].tcp_assert;

                    g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_id = g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset;
                    g_dma_controller_desc[event_group].dest_device_config[pipe_id].no_of_links++;
                    g_dma_controller_desc[event_group].dest_device_config[pipe_id].log_lli_state = DMA_APPEND_LLI_STATE;

                    /*-------------------- Set Relink Parameters for Destination Half Channel ---------------------*/
                    if (num_of_links > MASK_BIT0)
                    {
                        /* Set terminal count interrupt to zero intr gen is at End Of Transfer*/
                        if (DMA_EOT == g_dma_controller_desc[event_group].dest_device_config[pipe_id].intr_gen)
                        {
                            p_std_log_mem_param->dmac_lcsp3 &= DMA_SET_LOG_TERM_PARA;
                        }

                        /*Set DLOS to relink index offset */
                        g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset++;
                        p_std_log_mem_param->dmac_lcsp3 |= g_dma_controller_desc[event_group].channel_config[chan_index].
                                log_link_info.link_offset <<
                            DMA_LCSP3_LOS_SHIFT;

                        g_dma_controller_desc[event_group].dest_device_config[pipe_id].log_lli_state = DMA_RELINK_LLI_STATE;
                        dma_error = dma_SetLogRelinkParameters(p_pipe_desc, half_channel);
                    }
                    break;

                case DMA_APPEND_LLI_STATE:
                    /*Set DLOS to relink index offset */
                    g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset++;
                    p_std_log_mem_param->dmac_lcsp3 |= g_dma_controller_desc[event_group].channel_config[chan_index].
                            log_link_info.link_offset <<
                        DMA_LCSP3_LOS_SHIFT;
                    break;
            }
    }

    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  dma_SetLogRelinkParameters                                                       */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine generates the relink parameters and stores in the external    */
/*                memory in the form of linked list.                                       */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_pipe_desc : pipe configuration.                                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_SetLogRelinkParameters(IN t_dma_pipe_config *p_pipe_desc, IN t_dma_half_chan half_channel)
{
    t_dma_error dma_error = DMA_OK;

    switch (half_channel)
    {
        case DMA_SOURCE_HALF_CHANNEL:
            dma_error = dma_SetLogRelinkParamSrcHalfChan(p_pipe_desc);
            break;

        case DMA_DEST_HALF_CHANNEL:
            dma_error = dma_SetLogRelinkParamDestHalfChan(p_pipe_desc);
            break;
    }

    return(dma_error);
}

/*******************************************************************************************/
/* NAME:  dma_SetLogRelinkParamSrcHalfChan                                                 */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine generates the relink parameters for the source half channel   */
/*                and stores in the external memory in the form of linked list.            */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_pipe_desc : pipe configuration.                                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_SetLogRelinkParamSrcHalfChan(IN t_dma_pipe_config *p_pipe_desc)
{
    t_dma_pipe_num              pipe_id;
    t_uint32                    i, unit_tx_size, num_of_links;
    t_uint32                    src_config;
    t_std_src_log_memory_param  *p_std_src_log_relink_param;
    t_uint32                    element_count, total_elem_count, max_elem_count, rem_elem_count;

    t_dma_chan_num              chan_num = p_pipe_desc->chan_num;
    t_dma_evt_grp_num           event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index            chan_index = chan_num % DMA_DIVISOR;

    pipe_id = p_pipe_desc->src_pipe_id;

    /* data transfered in one block i.e without relink    */
    unit_tx_size = (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width) * g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count;

    /* Calculate the number of links required */
    num_of_links = g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size / unit_tx_size;

    total_elem_count = g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size / (t_uint8) (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width);
    max_elem_count = g_dma_controller_desc[event_group].src_device_config[pipe_id].element_count;
    rem_elem_count = total_elem_count;

    if ((g_dma_controller_desc[event_group].src_device_config[pipe_id].transfer_size % unit_tx_size) > NULL)
    {
        num_of_links++;
    }

    switch (g_dma_controller_desc[event_group].src_device_config[pipe_id].log_lli_state)
    {
        case DMA_RELINK_LLI_STATE:
            num_of_links--; /* one link has already been configured in Parameter Base Address Space*/
            rem_elem_count -= max_elem_count;

            if (g_dma_controller_desc[event_group].src_device_config[pipe_id].is_addr_incr)
            {               /* if address_increment is enabled*/
                g_dma_controller_desc[event_group].src_device_config[pipe_id].curr_addr += unit_tx_size;
            }
            break;

        case DMA_QUEUE_LLI_STATE:
            p_std_src_log_relink_param = (t_std_src_log_memory_param *)
                (
                    g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.base_log_addr +
                    8 *
                    g_dma_controller_desc[event_group].src_device_config[pipe_id].link_id
                );

            g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset++;
            p_std_src_log_relink_param->dmac_lcsp1 |=
                (g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset) <<
                DMA_LCSP1_LOS_SHIFT;

            break;
    }

    src_config =
        (
            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].master_port <<
            DMA_SxCFG_MST_SHIFT
        );
    src_config |=
        (
            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].is_addr_incr <<
            DMA_SxCFG_PEN_SHIFT
        );
    src_config |=
        (
            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].burst_size <<
            DMA_SxCFG_PSIZE_SHIFT
        );
    src_config |=
        (
            (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].data_width <<
            DMA_SxCFG_ESIZE_SHIFT
        );

    /*----------------------- Set Relink Parameters for Source Half Channel-----------------------------*/
    if
    (
        DMA_LOG_NO_OF_LINKS_MAX >=
            (num_of_links + g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset)
    )
    {
        p_std_src_log_relink_param = (t_std_src_log_memory_param *)
            (
                g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.base_log_addr +
                8 *
                g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset
            );

        g_dma_controller_desc[event_group].src_device_config[pipe_id].link_addr.chan_link_log_ptr = (t_logical_address) p_std_src_log_relink_param;

        for (i = NULL; i < num_of_links; i++)
        {
            element_count = (rem_elem_count >= max_elem_count) ? max_elem_count : rem_elem_count;

            p_std_src_log_relink_param->dmac_lcsp0 = element_count << DMA_LCSP0_ECNT_SHIFT;
            p_std_src_log_relink_param->dmac_lcsp0 |=
                (t_uint32)
                    g_dma_controller_desc[event_group].src_device_config[pipe_id].curr_addr & DMA_LCSP0_PTRLSB_MASK;

            p_std_src_log_relink_param->dmac_lcsp1 =
                (t_uint32)
                    g_dma_controller_desc[event_group].src_device_config[pipe_id].curr_addr & DMA_LCSP1_PTRMSB_MASK;
            p_std_src_log_relink_param->dmac_lcsp1 |= src_config;
            p_std_src_log_relink_param->dmac_lcsp1 |=
                (
                    g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset +
                    MASK_BIT0
                ) <<
                DMA_LCSP1_LOS_SHIFT;
            p_std_src_log_relink_param->dmac_lcsp1 |= (t_uint32) g_dma_controller_desc[event_group].src_device_config[pipe_id].tcp_assert;

            if (g_dma_controller_desc[event_group].src_device_config[pipe_id].is_addr_incr)
            {               /* if address_increment is enabled*/
                g_dma_controller_desc[event_group].src_device_config[pipe_id].curr_addr += unit_tx_size;
            }

            if (DMA_RELINK == g_dma_controller_desc[event_group].src_device_config[pipe_id].intr_gen)
            {
                if (g_dma_controller_desc[event_group].src_device_config[pipe_id].is_tc_intr)
                {
                    p_std_src_log_relink_param->dmac_lcsp1 |= DMA_SET_LOGICAL_TERM_CNT;
                }
            }

            g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset++;
            g_dma_controller_desc[event_group].src_device_config[pipe_id].no_of_links++;
            p_std_src_log_relink_param++;

            rem_elem_count = rem_elem_count - element_count;
        }

        p_std_src_log_relink_param--;

        /* Set terminal count interrupt */
        if (g_dma_controller_desc[event_group].src_device_config[pipe_id].is_tc_intr)
        {
            p_std_src_log_relink_param->dmac_lcsp1 |= DMA_SET_LOGICAL_TERM_CNT;
        }

        /* Last Relink node SLOS set to zero */
        p_std_src_log_relink_param->dmac_lcsp1 &= DMA_LCSP;

        g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset--;

        /* Store the last index in the device pipe */
        g_dma_controller_desc[event_group].src_device_config[pipe_id].link_id = g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset;

        g_dma_controller_desc[event_group].src_device_config[pipe_id].log_lli_state = DMA_QUEUE_LLI_STATE;
    }
    else
    {
        return(DMA_NO_MORE_LLIS_AVAILABLE);
    }

    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  dma_SetLogRelinkParamDestHalfChan                                                 */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine generates the relink parameters for the source half channel   */
/*                and stores in the external memory in the form of linked list.            */
/* PARAMETERS:                                                                             */
/* IN:    half_channel: Source or destination half channel.                                */
/*        *p_pipe_desc : pipe configuration.                                               */
/*                                                                                         */
/* INOUT: None                                                                             */
/* OUT:   None                                                                             */
/*                                                                                         */
/* RETURN: NONE                                                                            */
/*-----------------------------------------------------------------------------------------*/
/* REENTRANCY:       NA                                                                    */
/* REENTRANCY ISSUE: NA                                                                    */

/*******************************************************************************************/
PRIVATE t_dma_error dma_SetLogRelinkParamDestHalfChan(IN t_dma_pipe_config *p_pipe_desc)
{
    t_dma_pipe_num              pipe_id;
    t_uint32                    i, unit_tx_size, num_of_links;
    t_uint32                    dest_config;
    t_std_dest_log_memory_param *p_std_dest_log_relink_param;
    t_uint32                    element_count, total_elem_count, max_elem_count, rem_elem_count;

    t_dma_chan_num              chan_num = p_pipe_desc->chan_num;
    t_dma_evt_grp_num           event_group = chan_num / DMA_DIVISOR;
    t_dma_chan_index            chan_index = chan_num % DMA_DIVISOR;

    pipe_id = p_pipe_desc->dest_pipe_id;

    /* data transfered in one block i.e without relink */
    unit_tx_size = (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width) * g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count;

    /* Calculate the number of links required */
    num_of_links = g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size / unit_tx_size;

    if ((g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size % unit_tx_size) > NULL)
    {
        num_of_links++;
    }

    total_elem_count = g_dma_controller_desc[event_group].dest_device_config[pipe_id].transfer_size / (t_uint8) (MASK_BIT0 << (t_uint8) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width);
    max_elem_count = g_dma_controller_desc[event_group].dest_device_config[pipe_id].element_count;
    rem_elem_count = total_elem_count;

    switch (g_dma_controller_desc[event_group].dest_device_config[pipe_id].log_lli_state)
    {
        case DMA_RELINK_LLI_STATE:
            num_of_links--; /* one link has already been configured in Parameter Base Address Space*/
            rem_elem_count -= max_elem_count;

            if (g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_addr_incr)
            {               /* address_increment is NULL*/
                g_dma_controller_desc[event_group].dest_device_config[pipe_id].curr_addr += unit_tx_size;
            }
            break;

        case DMA_QUEUE_LLI_STATE:
            g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset++;

            p_std_dest_log_relink_param = (t_std_dest_log_memory_param *)
                (
                    g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.base_log_addr +
                    8 *
                    g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_id
                );

            p_std_dest_log_relink_param->dmac_lcsp3 |=
                (g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset) <<
                DMA_LCSP3_LOS_SHIFT;
            break;
    }

    dest_config =
        (
            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].master_port <<
            DMA_SxCFG_MST_SHIFT
        );
    dest_config |=
        (
            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_addr_incr <<
            DMA_SxCFG_PEN_SHIFT
        );
    dest_config |=
        (
            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].burst_size <<
            DMA_SxCFG_PSIZE_SHIFT
        );
    dest_config |=
        (
            (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].data_width <<
            DMA_SxCFG_ESIZE_SHIFT
        );

    /*-------------------- Set Relink Parameters for Destination Half Channel ---------------------*/
    if
    (
        DMA_LOG_NO_OF_LINKS_MAX >=
            (num_of_links + g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset)
    )
    {
        p_std_dest_log_relink_param = (t_std_dest_log_memory_param *)
            (
                g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.base_log_addr +
                8 *
                g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset
            );

        g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_addr.chan_link_log_ptr = (t_logical_address) p_std_dest_log_relink_param;

        for (i = MASK_BIT0; i <= (num_of_links); i++)
        {
            element_count = (rem_elem_count >= max_elem_count) ? max_elem_count : rem_elem_count;

            p_std_dest_log_relink_param->dmac_lcsp2 = element_count << DMA_LCSP2_ECNT_SHIFT;
            p_std_dest_log_relink_param->dmac_lcsp2 |=
                (t_uint32)
                    g_dma_controller_desc[event_group].dest_device_config[pipe_id].curr_addr & DMA_LCSP2_PTRLSB_MASK;

            p_std_dest_log_relink_param->dmac_lcsp3 =
                (t_uint32)
                    g_dma_controller_desc[event_group].dest_device_config[pipe_id].curr_addr & DMA_LCSP3_PTRMSB_MASK;
            p_std_dest_log_relink_param->dmac_lcsp3 |= dest_config;
            p_std_dest_log_relink_param->dmac_lcsp3 |=
                (
                    g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset +
                    MASK_BIT0
                ) <<
                DMA_LCSP3_LOS_SHIFT;
            p_std_dest_log_relink_param->dmac_lcsp3 |= (t_uint32) g_dma_controller_desc[event_group].dest_device_config[pipe_id].tcp_assert;

            if (g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_addr_incr)
            {               /* address_increment is NULL*/
                g_dma_controller_desc[event_group].dest_device_config[pipe_id].curr_addr += unit_tx_size;
            }

            if (DMA_RELINK == g_dma_controller_desc[event_group].dest_device_config[pipe_id].intr_gen)
            {
                if (g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_tc_intr)
                {
                    p_std_dest_log_relink_param->dmac_lcsp3 |= DMA_SET_LOGICAL_TERM_CNT;
                }
            }

            g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset++;
            g_dma_controller_desc[event_group].dest_device_config[pipe_id].no_of_links++;
            p_std_dest_log_relink_param++;

            rem_elem_count = rem_elem_count - element_count;
        }

        p_std_dest_log_relink_param--;

        /* Set terminal count interrupt */
        if (g_dma_controller_desc[event_group].dest_device_config[pipe_id].is_tc_intr)
        {
            p_std_dest_log_relink_param->dmac_lcsp3 |= DMA_SET_LOGICAL_TERM_CNT;
        }

        /* Last Relink node SLOS & DLOS set to zero */
        p_std_dest_log_relink_param->dmac_lcsp3 &= DMA_LCSP;

        g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset--;

        /* Store the last index in the device pipe */
        g_dma_controller_desc[event_group].dest_device_config[pipe_id].link_id = g_dma_controller_desc[event_group].channel_config[chan_index].log_link_info.link_offset;

        g_dma_controller_desc[event_group].dest_device_config[pipe_id].log_lli_state = DMA_QUEUE_LLI_STATE;
    }
    else
    {
        return(DMA_NO_MORE_LLIS_AVAILABLE);
    }

    return(DMA_OK);
}

/*******************************************************************************************/
/* NAME:  dma_GetLogLinkStatus()                                                           */
/*-----------------------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the logical link status  of the channel.                 */
/*                                                                                         */
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
PRIVATE t_dma_error dma_GetLogLinkStatus
(
    IN t_dma_pipe_config    pipe_desc,
    IN t_dma_half_chan      half_channel,
    OUT t_uint8             *p_dma_link_status
)
{
    t_dma_chan_num          chan_num = pipe_desc.chan_num;
    t_dma_evt_grp_num       event_group = chan_num / DMA_DIVISOR;
    t_dma_pipe_num          src_pipe_num, dest_pipe_num;
    t_std_log_memory_param  *p_std_log_mem_param;

    DBGENTER0();

    if (DMA_SOURCE_HALF_CHANNEL == half_channel)
    {
        /* Get Source link status */
        src_pipe_num = pipe_desc.src_pipe_id;
        p_std_log_mem_param =
            (t_std_log_memory_param *)
                g_dma_controller_desc[event_group].src_device_config[src_pipe_num].link_addr.chan_param_log_ptr;
        *p_dma_link_status = (t_uint8) (p_std_log_mem_param->dmac_lcsp1 &~DMA_SLOS_ZERO_MASK);
    }
    else
    {
        /* Get Destination link status */
        dest_pipe_num = pipe_desc.dest_pipe_id;
        p_std_log_mem_param =
            (t_std_log_memory_param *)
                g_dma_controller_desc[event_group].dest_device_config[dest_pipe_num].link_addr.chan_param_log_ptr;
        *p_dma_link_status = (t_uint8) (p_std_log_mem_param->dmac_lcsp3 &~DMA_DLOS_ZERO_MASK);
    }

    DBGEXIT0(DMA_OK);
    return(DMA_OK);
}

/****************************************************************************/
/* NAME: void dma_ProcessTCIt (t_dma_event_desc *p_event_desc);             */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*      This routine processes the TC IRQ raised by a given channel         */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :                                                                     */
/*       - p_event_desc: descriptor of the event raised (provide here)      */
/*                                                                          */
/* OUT :- None                                                              */
/*                                                                          */
/* RETURN:                                                                  */
/*         void                                                             */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE void dma_ProcessTCIt(IN t_dma_event_desc *p_event_desc)
{
    switch (p_event_desc->channel_type)
    {
        case DMA_CHAN_PHYSICAL_NOT_SECURE:
        case DMA_CHAN_PHYSICAL_SECURE:
        case DMA_CHAN_LOGICAL_NOT_SECURE:
        case DMA_CHAN_LOGICAL_SECURE:
            break;
    }
}

/****************************************************************************/
/* NAME: void dma_ProcessErrorIt (t_dma_event_desc *p_event_desc);          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION:                                                             */
/*      This routine processes the Error IRQ raised by a given channel      */
/*                                                                          */
/* PARAMETERS:                                                              */
/* IN :                                                                     */
/*       - p_event_desc: descriptor of the event raised (provide here)      */
/*                                                                          */
/* OUT :- None                                                              */
/*                                                                          */
/* RETURN:                                                                  */
/*         void                                                             */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */

/****************************************************************************/
PRIVATE void dma_ProcessErrorIt(IN t_dma_event_desc *p_event_desc)
{
    switch (p_event_desc->channel_type)
    {
        case DMA_CHAN_PHYSICAL_NOT_SECURE:
        case DMA_CHAN_PHYSICAL_SECURE:
        case DMA_CHAN_LOGICAL_NOT_SECURE:
        case DMA_CHAN_LOGICAL_SECURE:
            break;
    }
}

/* end of dma.c*/



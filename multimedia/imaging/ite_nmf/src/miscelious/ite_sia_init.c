/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define ITE_SIA_INIT_C_

#include "ite_sia_init.h"
#include <stdio.h>

#include "ite_event.h"
#include "ite_main.h"

#include <los/api/error.h>  /* for CM_StringError() */
#include <los/api/time.h>

#include <cm/inc/cm.h>

#ifdef __ARM_SYMBIAN
#   include <los/api/los_api.h>

extern IMPORT_C void    ite_sia_init_redirection (void);
extern IMPORT_C void    ite_sia_deinit_redirection (void);
extern IMPORT_C void    registerStubsAndSkels (void);
extern IMPORT_C void    unregisterStubsAndSkels (void);

extern t_cm_domain_id   mpc2domainId (const char *mpcstring);
#endif

// grab NMF components related include
#include "host/grab/api/cmd.h"
#include "host/grab/api/alert.h"
#include "host/ispctl/api/cmd.h"
#include "host/ispctl/api/cfg.h"
#include "host/ispctl/api/alert.h"

#include <cm/inc/cm_macros.h>

//interface header files are included through following header that includes ite_sia_interface.h
#include "ite_sia_interface_data.h"
#include "ite_init.h"
#include "ite_sia_buffer.h"

extern void                 ITE_Refresh_FrameBuffer (void);
t_cm_domain_id              domainId;

/*--------------------------------------------------------------------------*
 * Private functions                                            					  *
 *--------------------------------------------------------------------------*/
static void                 ITE_Callback_grabHRInfo (
                            void            *ap_this,
                            enum e_grabInfo info_id,
                            t_uint16        buffer_id,
                            t_uint32        timestamp);
static void                 ITE_Callback_grabHRError (
                            void                *ap_this,
                            enum e_grabError    error_id,
                            t_uint16            data,
                            t_uint16            buffer_id,
                            t_uint32            timestamp);
static void                 ITE_Callback_grabHRDebug (
                            void                *ap_this,
                            enum e_grabDebug    debug_id,
                            t_uint16            buffer_id,
                            t_uint16            data1,
                            t_uint16            data2,
                            t_uint32            timestamp);
static void                 ITE_Callback_grabLRInfo (
                            void            *ap_this,
                            enum e_grabInfo info_id,
                            t_uint16        buffer_id,
                            t_uint32        timestamp);
static void                 ITE_Callback_grabLRError (
                            void                *ap_this,
                            enum e_grabError    error_id,
                            t_uint16            data,
                            t_uint16            buffer_id,
                            t_uint32            timestamp);
static void                 ITE_Callback_grabLRDebug (
                            void                *ap_this,
                            enum e_grabDebug    debug_id,
                            t_uint16            buffer_id,
                            t_uint16            data1,
                            t_uint16            data2,
                            t_uint32            timestamp);
static void                 ITE_Callback_grabBMSInfo (
                            void            *ap_this,
                            enum e_grabInfo info_id,
                            t_uint16        buffer_id,
                            t_uint32        timestamp);
static void                 ITE_Callback_grabBMSError (
                            void                *ap_this,
                            enum e_grabError    error_id,
                            t_uint16            data,
                            t_uint16            buffer_id,
                            t_uint32            timestamp);
static void                 ITE_Callback_grabBMSDebug (
                            void                *ap_this,
                            enum e_grabDebug    debug_id,
                            t_uint16            buffer_id,
                            t_uint16            data1,
                            t_uint16            data2,
                            t_uint32            timestamp);
static void                 ITE_Callback_grabBMLInfo (
                            void            *ap_this,
                            enum e_grabInfo info_id,
                            t_uint16        buffer_id,
                            t_uint32        timestamp);
static void                 ITE_Callback_grabBMLError (
                            void                *ap_this,
                            enum e_grabError    error_id,
                            t_uint16            data,
                            t_uint16            buffer_id,
                            t_uint32            timestamp);
static void                 ITE_Callback_grabBMLDebug (
                            void                *ap_this,
                            enum e_grabDebug    debug_id,
                            t_uint16            buffer_id,
                            t_uint16            data1,
                            t_uint16            data2,
                            t_uint32            timestamp);
static void                 ITE_Callback_ispctlInfo (
                            void                *ap_this,
                            enum e_ispctlInfo   info,
                            t_uint32            value,
                            t_uint32            timestamp);
static void                 ITE_Callback_ispctlInfoList (
                            void                *ap_this,
                            enum e_ispctlInfo   info,
                            ts_PageElement      Listvalue[],
                            t_uint16            number_of_pe,
                            t_uint32            timestamp);
static void                 ITE_Callback_ispctlError (
                            void                *ap_this,
                            enum e_ispctlError  error,
                            t_uint32            data,
                            t_uint32            timestamp);

//static void ITE_Callback_ispctlUnexpectedError( void *ap_this, t_eStateId State_id , t_eSignal data, t_uint32 timestamp);
static void                 ITE_Callback_ispctlDebug (
                            void                *ap_this,
                            enum e_ispctlDebug  debug_id,
                            t_uint16            data1,
                            t_uint16            data2,
                            t_uint32            timestamp);

//static void ITE_Callback_siaRMInfo( void *ap_this, enum e_siarmInfo info_id, t_uint16 resources);
//static void ITE_Callback_siaRMError( void *ap_this, enum e_siarmError error_id);
//static void ITE_Callback_siaRMDebug( void *ap_this, enum e_siarmDebug debug_id, t_uint16 data1, t_uint16 data2);
static t_bool               ITE_instantiate_sia_components (void);
static t_bool               ITE_bind_ispctl_fromto_host (void);
static t_bool               ITE_bind_grab_fromto_host (void);

//static t_bool ITE_bind_siarm_fromto_host( void );
//static t_bool ITE_bind_inter_components( void );
static t_bool               ITE_start_sia_components (void);

static t_bool               ITE_destroy_sia_components (void);
static t_bool               ITE_unbind_ispctl_fromto_host (void);
static t_bool               ITE_unbind_grab_fromto_host (void);

//static t_bool ITE_unbind_siarm_fromto_host( void );
//static t_bool ITE_unbind_inter_components( void );
static t_bool               ITE_stop_sia_components (void);

void                        ITE_DisplayGrabError (enum e_grabError error_id, t_uint16 buffer_id);
void                        ITE_DisplayIPPSDError (t_uint16 buffer_id);

//mle
extern ts_siapicturebuffer  GrabBufferLR[];

/*--------------------------------------------------------------------------*
 * Globals                                                  					  *
 *--------------------------------------------------------------------------*/
CBgrab_api_alert            grabAlert[4] =
{
    { ( void * ) 0,
            (void(*) ( void *, enum e_grabInfo, t_uint16, t_uint16, t_uint32 ))
                ITE_Callback_grabLRInfo, ITE_Callback_grabLRError, ITE_Callback_grabLRDebug },
    { ( void * ) 0,
            (void(*) ( void *, enum e_grabInfo, t_uint16, t_uint16, t_uint32 ))
                ITE_Callback_grabHRInfo, ITE_Callback_grabHRError, ITE_Callback_grabHRDebug },
    { ( void * ) 0,
            (void(*) ( void *, enum e_grabInfo, t_uint16, t_uint16, t_uint32 ))
                ITE_Callback_grabBMSInfo, ITE_Callback_grabBMSError, ITE_Callback_grabBMSDebug },
    { ( void * ) 0,
            (void(*) ( void *, enum e_grabInfo, t_uint16, t_uint16, t_uint32 ))
                ITE_Callback_grabBMLInfo, ITE_Callback_grabBMLError, ITE_Callback_grabBMLDebug }
};

CBispctl_api_alert          ispctlAlert =

//                {0, ITE_Callback_ispctlInfo, ITE_Callback_ispctlInfoList, ITE_Callback_ispctlError, ITE_Callback_ispctlUnexpectedError, ITE_Callback_ispctlDebug};
{ 0,
    ITE_Callback_ispctlInfo,
    ITE_Callback_ispctlInfoList,
    ITE_Callback_ispctlError,
    ITE_Callback_ispctlDebug };

//static CBsia_rm_api_alert siaRMAlert = {0, ITE_Callback_siaRMInfo, ITE_Callback_siaRMError, ITE_Callback_siaRMDebug};
volatile t_uint8            grabInfoReceived = 0;
volatile t_uint8            grabErrorReceived = 0;
volatile t_uint8            grabDebugReceived = 0;
volatile t_uint8            ispctlInfoReceived = 0;
volatile t_uint8            ispctlErrorReceived = 0;
volatile t_uint8            ispctlDebugReceived = 0;
volatile t_uint8            siaRMInfoReceived = 0;
volatile t_uint8            siaRMErrorReceived = 0;

t_uint8                     ispctl_clientId = 0;

static volatile t_bool      irpBootCompleted = (t_bool) FALSE;
static volatile t_bool      irpWriteCompleted = (t_bool) FALSE;
static volatile t_bool      irpReadCompleted = (t_bool) FALSE;
static volatile t_bool      irpAFStableCompleted = (t_bool) FALSE;
static volatile t_uint16    readPEValue = 0;

extern t_uint8              cm_debug_level;
extern t_nmf_channel        nmfCallBackFifoID;

// struct s_grabParams grabparamsLR = {GRBFMT_R5G6B5, 640, 480, 0, 0, 640, 480, 0, 0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 0, 0, {0, 0, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, 0, 0}};

// Only intialising members that are non-zero
struct s_grabParams         grabparamsLR =
{
    .output_format = GRBFMT_R5G6B5,
    .x_window_size = 640,
    .y_window_size = 480,
    .x_frame_size = 640,
    .y_frame_size = 480
};

struct s_grabParams         grabparamsHR =
{
    .output_format = GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED,
    .x_window_size = 640,
    .y_window_size = 480,
    .x_frame_size = 640,
    .y_frame_size = 480
};

struct s_grabParams         grabparamsBMS =
{
    .output_format = GRBFMT_RAW8,
    .x_window_size = 3280,
    .y_window_size = 2464,
    .x_frame_size = 3280,
    .y_frame_size = 2464
};

volatile t_uint32           g_grabHR_infinite = 0;
volatile t_uint32           g_grabLR_infinite = 0;
volatile t_uint32           g_grabBMS_infinite = 0;
volatile t_uint32           g_grabBML_infinite = 0;
volatile t_uint32           g_B2R2HR_infinite = 0;
volatile t_uint32           g_dplLR_infinite = 0;
volatile t_uint32           g_dplHR_infinite = 0;

// For debug
//to record pipe grabbed frame
volatile t_uint32           g_grabLR_count = 0;
volatile t_uint32           g_grabHR_count = 0;
volatile t_uint32           g_grabLR_countglobal = 0;
volatile t_uint32           g_grabHR_countglobal = 0;
volatile t_uint32           g_grabBMS_count = 0;
volatile t_uint32           g_statsGlaceHisto_count = 0;
volatile t_uint32           g_statsAF_count = 0;

InputImageSource_te         g_InputImageSource = InputImageSource_e_Sensor0;

t_bool                      TraceGrabLREvent = (t_bool) FALSE;
t_bool                      TraceBMLEvent = (t_bool) FALSE;

//#define TIMELOG 1 //Already defined in ite_main.h
#ifdef TIMELOG
extern ts_sia_eventtimelog  eventtimelog;
#endif

//extern void MMTE_MCDE_SwFrameSync();

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Register_GrabComponents
   PURPOSE  : Instanciate, Bind, ... all grab needed components
              and their respective interfaces.
   ------------------------------------------------------------------------ */
int
ITE_register_sia_components(void)
{
    /* Error with Component Manager */
    t_bool              result;
    volatile t_uint32   tempo = 0;

    //t_uint32    try_counter = 0xff;
    UNUSED(tempo);

    LOS_Log("\n===============================\n", NULL, NULL, NULL, NULL, NULL, NULL);
    LOS_Log("BUILD NMF COMPONENT NETWORK \n", NULL, NULL, NULL, NULL, NULL, NULL);
    LOS_Log("===============================\n\n", NULL, NULL, NULL, NULL, NULL, NULL);

    LOS_Log("Instantiate all NMF grab related components...\n", NULL, NULL, NULL, NULL, NULL, NULL);
    result = ITE_instantiate_sia_components();
    if (result == TRUE)
    {
        return (t_bool) TRUE;
    }


    LOS_Log("...Instantiate OK\n", NULL, NULL, NULL, NULL, NULL, NULL);

    LOS_Log("\nBind between ispctl component and Host(ARM)... \n", NULL, NULL, NULL, NULL, NULL, NULL);
    result = ITE_bind_ispctl_fromto_host();
    if (result == TRUE)
    {
        return (t_bool) TRUE;
    }


    LOS_Log("...Binding To/From Host OK\n", NULL, NULL, NULL, NULL, NULL, NULL);

    LOS_Log("\nBind between grab component and Host(ARM)... \n", NULL, NULL, NULL, NULL, NULL, NULL);
    result = ITE_bind_grab_fromto_host();
    if (result == TRUE)
    {
        return (t_bool) TRUE;
    }


    LOS_Log("...Binding To/From Host OK\n", NULL, NULL, NULL, NULL, NULL, NULL);

    /*MM
                   LOS_Log("\nBind between siarm component and Host(ARM)... \n", NULL, NULL, NULL, NULL, NULL, NULL);
                   result=ITE_bind_siarm_fromto_host();
                   if (result == TRUE) return (t_bool)TRUE;
                   LOS_Log("...Binding To/From Host OK\n", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

                   LOS_Log("\nBind between NMF components (synchronous intra DSP binding)... \n", NULL, NULL, NULL, NULL, NULL, NULL);
                   result=ITE_bind_inter_components();
                   if (result == TRUE) return (t_bool)TRUE;
                   LOS_Log("...Binding intra DSP OK\n", NULL, NULL, NULL, NULL, NULL, NULL);
                   */
    LOS_Log("\nStart Grab NMF components (enable interface calls)... \n", NULL, NULL, NULL, NULL, NULL, NULL);
    result = ITE_start_sia_components();
    if (result == TRUE)
    {
        return (t_bool) TRUE;
    }


    LOS_Log("...Components started OK\n", NULL, NULL, NULL, NULL, NULL, NULL);

    return (t_bool) FALSE;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_instantiate_grab_components
   PURPOSE  : instantiate all needed components for grab
   ------------------------------------------------------------------------ */
static t_bool
ITE_instantiate_sia_components(void)
{
    t_cm_error          cm_error;
    t_cm_domain_memory  mydomain;

#ifdef __ARM_SYMBIAN
    ite_sia_init_redirection();

    //registerStubsAndSkels();
#else
    CM_REGISTER_STUBS_SKELS(imaging);

    //CM_GetDefaultDomain(SIA_CORE_ID, &domainId) ;
#endif

    //Allocation for ESRAM
    mydomain.esramData.offset = 0;
    mydomain.esramData.size = 0x80000;
    mydomain.sdramData.offset = 0x3ff70;
    mydomain.sdramData.size = 0x7c0090;
    mydomain.esramCode.offset = 0;
    mydomain.esramCode.size = 0x80000;
    mydomain.sdramCode.offset = 0x3ff70;
    mydomain.sdramCode.size = 0x7c0090;
    mydomain.coreId = SIA_CORE_ID;
    CM_CreateMemoryDomain(&mydomain, &domainId);

    /* Instantiate Grab Component */
    cm_error = CM_InstantiateComponent(GRAB_COMPONENT_TYPE_NAME_STR, domainId, NMF_SCHED_NORMAL, "grab", &grab_handle);
    if (cm_error != CM_OK)
    {
        LOS_Log(
        "Error : Instantiate %s (error: %s)\n",
        GRAB_COMPONENT_TYPE_NAME_STR,
        CM_StringError(cm_error),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
        return (t_bool) TRUE;
    }


    /* Instantiate ispctl Component */
    cm_error = CM_InstantiateComponent(
        ISPCTL_COMPONENT_TYPE_NAME_STR,
        domainId,
        NMF_SCHED_NORMAL,
        "ispctl",
        &ispctl_handle);
    if (cm_error != CM_OK)
    {
        LOS_Log(
        "Error : Instantiate %s (error : %s)\n",
        ISPCTL_COMPONENT_TYPE_NAME_STR,
        CM_StringError(cm_error),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
        return (t_bool) TRUE;
    }


    /* Instantiate sia ressource manager Component */

    //FIXME: later, see to allocate SIA_RM in a specific function

    /*MM
                   cm_error = CM_InstantiateComponent( SIA_RESOURCE_MANAGER_COMPONENT_TYPE_NAME_STR ,
                                                       domainId           ,
                                                       NMF_SCHED_NORMAL     ,
                                                       "siarm"   ,
                                                       &sia_resource_manager_handle    );
                   if ( cm_error != CM_OK )
                   {
                      LOS_Log( "Error : Instantiate %s (error : %s)\n",
                              SIA_RESOURCE_MANAGER_COMPONENT_TYPE_NAME_STR, CM_StringError(cm_error)
                              , NULL, NULL, NULL, NULL, NULL);
                      return (t_bool)TRUE;
                   }
                   */
    return (t_bool) FALSE;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_start_grab_components
   PURPOSE  : start all needed components for grab
   ------------------------------------------------------------------------ */
static t_bool
ITE_start_sia_components(void)
{
    t_cm_error  cm_error;

    /*as components are instantiated and binded, we can start them*/

    /*MM   cm_error = CM_StartComponent(sia_resource_manager_handle);
                   if (cm_error != CM_OK)
                   {
                      LOS_Log("Error : Fail to start sia_resource_manager component (error = 0x%08X)\n", cm_error
                      		, NULL, NULL, NULL, NULL, NULL);
                      return (t_bool)TRUE;
                   }
                */
    cm_error = CM_StartComponent(ispctl_handle);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Fail to start ispctl component (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_StartComponent(grab_handle);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Fail to start grab component (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    return (t_bool) FALSE;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_bind_ispctl_fromto_host
   PURPOSE  : perform all binding form/to Host : related to ispctl component
   ------------------------------------------------------------------------ */
static t_bool
ITE_bind_ispctl_fromto_host(void)
{
    t_cm_error  cm_error;
    cm_error = CM_BindComponentFromUser(
        ispctl_handle,
        "iSetDebug",
        FIFO_COMS_SIZE,
        ( t_cm_interface_desc * ) &ispctlDebug);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Bind Host->ispctl (iSetDebug) (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_BindComponentFromUser(
        ispctl_handle,
        "iConfigure",
        FIFO_COMS_SIZE,
        ( t_cm_interface_desc * ) &ispctlConfigure);

    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Bind Host->ispctl (iConfigure) (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_BindComponentFromUser(
        ispctl_handle,
        "iCommand",
        FIFO_COMS_SIZE,
        ( t_cm_interface_desc * ) &ispctlCommand);

    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Bind Host->ispctl (iCommand) (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_BindComponentToUser(
        nmfCallBackFifoID,
        ispctl_handle,
        "iAlert[0]",    // [ispctl_clientId]
        ( t_cm_interface_desc * ) &ispctlAlert,
        FIFO_COMS_SIZE);

    if (cm_error != CM_OK)
    {
        /*  Fatal error : Return directly */
        LOS_Log("Error : Bind ispctl->Host (iAlert) (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    return (t_bool) FALSE;
}


/* ------------------------------------------------------------------------
   ------------------------------------------------------------------------ */
static t_bool
ITE_bind_grab_fromto_host(void)
{
    t_cm_error  cm_error;
    int         i;

    cm_error = CM_BindComponentFromUser(
        grab_handle,
        "iCommand",
        FIFO_COMS_SIZE,
        ( t_cm_interface_desc * ) &grabCommand);
    if (cm_error != CM_OK)
    {
        LOS_Log(
        "Error: Bind Host->grab (iCommand) error : %s\n",
        CM_StringError(cm_error),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_BindComponentFromUser(grab_handle, "iSetDebug", FIFO_COMS_SIZE, ( t_cm_interface_desc * ) &grabDebug);
    if (cm_error != CM_OK)
    {
        LOS_Log(
        "Error: Bind Host->grab (iSetDebug) error : %s\n",
        CM_StringError(cm_error),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
        return (t_bool) TRUE;
    }


    for (i = 0; i < 4; i++)
    {
        char    ifname[] = "iAlert[XX]";
        snprintf(ifname, 11, "iAlert[%i]", i);
        cm_error = CM_BindComponentToUser(
            nmfCallBackFifoID,
            grab_handle,
            ifname,
            ( t_cm_interface_desc * ) &grabAlert[i],
            FIFO_COMS_SIZE);
        if (cm_error != CM_OK)
        {
            LOS_Log(
            "Error: Bind Host->grab (%s) error : %s\n",
            ifname,
            CM_StringError(cm_error),
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
            return (t_bool) TRUE;
        }
    }


    /*    cm_error = CM_BindComponentToVoid(grab_handle, "iAlert[3]");
                    if (cm_error != CM_OK)
                    {
                        LOS_Log("Error: Bind Host->grab (%s) error : %s\n", "iAlert[2]", CM_StringError(cm_error), NULL, NULL, NULL, NULL, NULL);
                        return (t_bool)TRUE;
                    }
                */

    /*MM
                  for (i=2; i<16; i++)
                    {
                    char ifname[] = "iAlert[XX]";
                    snprintf(ifname, 11, "iAlert[%i]", i);
                    cm_error = CM_BindComponentToVoid( grab_handle,
                				       ifname);
                    if (cm_error != CM_OK)
                      {
                	LOS_Log("Error: Bind Host->grab (%s) error : %s\n", ifname, CM_StringError(cm_error)
                		, NULL, NULL, NULL, NULL, NULL);
                	return (t_bool)TRUE;
                      }
                    }*/
    return (t_bool) FALSE;
}


/******************************************************************************
 * bind SIA resource manager from/to host
 ******************************************************************************/

/*
static t_bool ITE_bind_siarm_fromto_host( void )
{
  t_cm_error cm_error;
  int i;

  cm_error = CM_BindComponentFromUser( sia_resource_manager_handle,
				       "iCommand",
				       FIFO_COMS_SIZE,
				       (t_cm_interface_desc *)&siaRMCommand);
  if (cm_error != CM_OK)
    {
      LOS_Log("Error: Bind Host->sia_rm (iCommand) error : %s\n", CM_StringError(cm_error)
      		, NULL, NULL, NULL, NULL, NULL);
      return (t_bool)TRUE;
    }

  for (i=0; i<16; i++) // note: though there are 17 interfaces, iAlert[16] is reserved to ispctl
    {
    char ifname[] = "iAlert[XX]";
    snprintf(ifname, 11, "iAlert[%i]", i);
    cm_error = CM_BindComponentToUser( nmfCallBackFifoID, sia_resource_manager_handle,
				       ifname,
				       (t_cm_interface_desc *)&siaRMAlert,
				       FIFO_COMS_SIZE);
    if (cm_error != CM_OK)
      {
	LOS_Log("Error: Bind sia_rm->Host (%s) error : %s\n", ifname, CM_StringError(cm_error)
		, NULL, NULL, NULL, NULL, NULL);
	return (t_bool)TRUE;
      }
    }

  return (t_bool)FALSE;
}
*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_bind_inter_components
   PURPOSE  : perform all binding inter components
   ------------------------------------------------------------------------ */

/*
static t_bool ITE_bind_inter_components( void )
{
  t_cm_error cm_error;

  // Bind between ispctrl and resource_manager component
  //management of CCP0/CCP1 ressource: index for interface collection binded to ispctrl is choosed here: LATEST by default

  cm_error = CM_BindComponent( sia_resource_manager_handle, "iAlert[16]" , ispctl_handle, "iSiaRMAlert" );
  if (cm_error != CM_OK)
    {
         //  Fatal error : Return directly
      LOS_Log("Error : sia_resource_manager->ispctl (iAlert) (error : %s)\n", CM_StringError(cm_error)
      		, NULL, NULL, NULL, NULL, NULL);
         return (t_bool)TRUE;
    }

    cm_error = CM_BindComponent( ispctl_handle, "iSiaRMCmd" , sia_resource_manager_handle, "iCommand" );
    if (cm_error != CM_OK)
    {
         //  Fatal error : Return directly
      LOS_Log("Error : Error : ispctl->sia_resource_manager (iCommand) (error : %s)\n", CM_StringError(cm_error)
      		, NULL, NULL, NULL, NULL, NULL);
         return (t_bool)TRUE;
    }

	return (t_bool)FALSE;
}
*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Unregister_GrabComponents
   PURPOSE  : Destroy, Unbind, ... all needed components
              and their respective interfaces.
   ------------------------------------------------------------------------ */
int
ITE_unregister_sia_components(void)
{
    /* Error with Component Manager */
    t_bool  result;

    LOS_Log("\n===============================\n", NULL, NULL, NULL, NULL, NULL, NULL);
    LOS_Log("DESTROY NMF COMPONENT NETWORK \n", NULL, NULL, NULL, NULL, NULL, NULL);
    LOS_Log("===============================\n", NULL, NULL, NULL, NULL, NULL, NULL);

    LOS_Log("\nStop all NMF SIA grab related components...\n", NULL, NULL, NULL, NULL, NULL, NULL);
    result = ITE_stop_sia_components();
    if (result == TRUE)
    {
        return (t_bool) TRUE;
    }


    LOS_Log("...Components STOP OK\n", NULL, NULL, NULL, NULL, NULL, NULL);

    /*MM
                   LOS_Log("\nUnbind intra dsp bindings...\n", NULL, NULL, NULL, NULL, NULL, NULL);
                   result = ITE_unbind_inter_components( );
                   if (result == TRUE) return (t_bool)TRUE;
                   LOS_Log("...Unbind intra dsp OK\n", NULL, NULL, NULL, NULL, NULL, NULL);
                   */
    LOS_Log("\nUnbind ispctl From/To Host bindings...\n", NULL, NULL, NULL, NULL, NULL, NULL);
    result = ITE_unbind_ispctl_fromto_host();
    if (result == TRUE)
    {
        return (t_bool) TRUE;
    }


    LOS_Log("...Unbind ispctl OK \n", NULL, NULL, NULL, NULL, NULL, NULL);

    LOS_Log("\nUnbind grab From/To Host bindings...\n", NULL, NULL, NULL, NULL, NULL, NULL);
    result = ITE_unbind_grab_fromto_host();
    if (result == TRUE)
    {
        return (t_bool) TRUE;
    }


    LOS_Log("...Unbind grab OK\n", NULL, NULL, NULL, NULL, NULL, NULL);

    /* DEPRECATED Ressource Manager
                   LOS_Log("\nUnbind Sia_rm From/To Host bindings...\n", NULL, NULL, NULL, NULL, NULL, NULL);
                   result = ITE_unbind_siarm_fromto_host( );
                   if (result == TRUE) return (t_bool)TRUE;
                   LOS_Log("...Unbind sia_rm OK\n", NULL, NULL, NULL, NULL, NULL, NULL);
                */
    LOS_Log("\nDestroy (desallocate) all NMF grab related components...\n", NULL, NULL, NULL, NULL, NULL, NULL);
    result = ITE_destroy_sia_components();
    if (result == TRUE)
    {
        return (t_bool) TRUE;
    }


    LOS_Log("...Destroy OK \n", NULL, NULL, NULL, NULL, NULL, NULL);

#ifdef __ARM_SYMBIAN
    //US 5:20 - Unregistering components also.
    ite_sia_deinit_redirection();
#else
    CM_UNREGISTER_STUBS_SKELS(imaging);
#endif
    return (t_bool) FALSE;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_destroy_grab_components
   PURPOSE  : destroy all needed components for grab
   ------------------------------------------------------------------------ */
static t_bool
ITE_destroy_sia_components(void)
{
    t_cm_error  cm_error;

    /* Destroy resource_manager component */

    /*MM   cm_error = CM_DestroyComponent( sia_resource_manager_handle );
                   if (cm_error != CM_OK)
                   {
                      LOS_Log("Error : Destroy sia_resource_manager (error = 0x%08X)\n", cm_error
                      		,NULL, NULL, NULL, NULL, NULL);
                      return (t_bool)TRUE;
                   }
                   */

    /* Destroy ispctl component */
    cm_error = CM_DestroyComponent(ispctl_handle);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Destroy ispctl (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    /* Destroy grab component */
    cm_error = CM_DestroyComponent(grab_handle);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Destroy grab (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    //freed memory domain
    CM_DestroyMemoryDomain(domainId);

    return (t_bool) FALSE;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_stop_grab_components
   PURPOSE  : start all needed components for grab
   ------------------------------------------------------------------------ */
static t_bool
ITE_stop_sia_components(void)
{
    t_cm_error  cm_error;

    /*First operation is to stop all components */

    /*MM   cm_error = CM_StopComponent(sia_resource_manager_handle);
                   if (cm_error != CM_OK)
                   {
                      LOS_Log("Error : Fail to stop sia_resource_manager component (error = 0x%08X)\n", cm_error
                      		, NULL, NULL, NULL, NULL, NULL);
                      return (t_bool)TRUE;
                   }
                */
    cm_error = CM_StopComponent(ispctl_handle);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Fail to stop ispctl component (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_StopComponent(grab_handle);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Fail to stop grab component (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    return (t_bool) FALSE;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_unbind_ispctl_fromto_host
   PURPOSE  : perform all unbinding form/to Host : related to ispctl component
   ------------------------------------------------------------------------ */
static t_bool
ITE_unbind_ispctl_fromto_host(void)
{
    t_cm_error  cm_error;

    cm_error = CM_UnbindComponentFromUser(( t_cm_interface_desc * ) &ispctlCommand);
    if (cm_error != CM_OK)
    {
        LOS_Log(
        "Error : Unbind Host->ispctl (ispctlCommand) (error = 0x%08X)\n",
        cm_error,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_UnbindComponentFromUser(( t_cm_interface_desc * ) &ispctlConfigure);
    if (cm_error != CM_OK)
    {
        LOS_Log(
        "Error : Unbind Host->ispctl (ispctlConfigure) (error = 0x%08X)\n",
        cm_error,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_UnbindComponentFromUser(( t_cm_interface_desc * ) &ispctlDebug);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Unbind Host->ispctl (ispctlDebug) (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_UnbindComponentToUser(nmfCallBackFifoID, ispctl_handle, "iAlert[0]", NULL);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Unbind ispctl->Host (iAlert) (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    return (t_bool) FALSE;
}


/*********************************************************************************
 * perform all unbinding form/to Host related to grab component
 *********************************************************************************/
static t_bool
ITE_unbind_grab_fromto_host(void)
{
    t_cm_error  cm_error;
    int         i;
    cm_error = CM_UnbindComponentFromUser(( t_cm_interface_desc * ) &grabCommand);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Unbind Host->grab (grabCommand) (error = 0x%08X)\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    cm_error = CM_UnbindComponentFromUser(( t_cm_interface_desc * ) &grabDebug);
    if (cm_error != CM_OK)
    {
        LOS_Log("Error : Unbind Host->grab (grabDebug) (error = 0x%08X))\n", cm_error, NULL, NULL, NULL, NULL, NULL);
        return (t_bool) TRUE;
    }


    for (i = 0; i < 4; i++)
    {
        char    if_name[] = "iAlert[XX]";
        snprintf(if_name, 11, "iAlert[%i]", i);

        cm_error = CM_UnbindComponentToUser(nmfCallBackFifoID, grab_handle, if_name, NULL);
        if (cm_error != CM_OK)
        {
            LOS_Log(
            "Error: Unbind grab->Host (%s) (error = 0x%08X)\n",
            if_name,
            cm_error,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
            return (t_bool) TRUE;
        }
    }


    /*    cm_error = CM_UnbindComponent(grab_handle, "iAlert[2]");
                    if (cm_error != CM_OK)
                    {
                        LOS_Log("Error: Unbind grab->Host (%s) (error = 0x%08X)\n", "iAlert[2]", cm_error, NULL, NULL, NULL, NULL, NULL);
                        return (t_bool)TRUE;
                    }
                */
    return (t_bool) FALSE;
}


/*********************************************************************************
 * perform all unbinding form/to Host related to grab component
 *********************************************************************************/

/*
static t_bool ITE_unbind_siarm_fromto_host( void )
{
  t_cm_error cm_error;
  int i;

  cm_error = CM_UnbindComponentFromUser( (t_cm_interface_desc *)&siaRMCommand );
  if (cm_error != CM_OK)
   {
      LOS_Log("Error : Unbind Host->sia_rm (iCommand) (error = 0x%08X)\n", cm_error
      		, NULL, NULL, NULL, NULL, NULL);
      return (t_bool)TRUE;
   }

  for (i=0; i<16; i++) // NOTE: last Alert (iAlert[16]) reserved for sia_rm->ispctl
    {
    char ifname[] = "iAlert[XX]";
    snprintf(ifname, 11, "iAlert[%i]", i);

    cm_error = CM_UnbindComponentToUser( sia_resource_manager_handle,
					 ifname);
    if (cm_error != CM_OK)
      {
	LOS_Log("Error: Unbind sia_rm->Host (%s) (error = 0x%08X)\n", ifname, cm_error
		, NULL, NULL, NULL, NULL, NULL);
	return (t_bool)TRUE;
      }
    }

  return (t_bool)FALSE;
}
*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_unbind_inter_components
   PURPOSE  : perform all unbinding inter components
   ------------------------------------------------------------------------ */

/*
static t_bool ITE_unbind_inter_components( void )
{
   t_cm_error cm_error;

   // Unbind ispctrl - resource_manager component
   cm_error = CM_UnbindComponent( sia_resource_manager_handle, "iAlert[16]" );
   if (cm_error != CM_OK)
   {
      LOS_Log("Error : Unbind sia_resource_manager->ispctl (iAlert) (error = 0x%08X)\n", cm_error
      		, NULL, NULL, NULL, NULL, NULL);
      return (t_bool)TRUE;
   }

   cm_error = CM_UnbindComponent( ispctl_handle, "iSiaRMCmd" );
   if (cm_error != CM_OK)
   {
      LOS_Log("Error : ispctl->sia_resource_manager (iCommand) (error = 0x%08X)\n", cm_error
      		, NULL, NULL, NULL, NULL, NULL);
      return (t_bool)TRUE;
   }

   return (t_bool)FALSE;
}
*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabLRInfo
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab processing end
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabLRInfo(void *ap_this, enum e_grabInfo info_id, t_uint16 buffer_id, t_uint32 timestamp)
{   // Two possible events:  GRAB_COMPLETED and GRAB_LAUNCHED
    union u_ITE_Event   event;
    UNUSED(ap_this);
    if ((info_id == GRBINF_GRAB_COMPLETED) || (info_id == GRBINF_GRAB_ABORTED))
    {
        event.type = ITE_EVT_GRABLR_INFO;
        event.grabLRInfo.info_id = info_id;
        event.grabLRInfo.buffer_id = buffer_id;
        event.grabLRInfo.timestamp = timestamp;

#if !(defined(__PEPS8500_SIA) || defined(_SVP_) || defined(__ARM_SYMBIAN))
        // display image
        if (g_dplLR_infinite == INFINITY)
        {
            ITE_Refresh_FrameBuffer();
        }


#endif
        if (info_id == GRBINF_GRAB_COMPLETED)
        {
            if (TraceGrabLREvent)
            {
                unsigned long long  time1 = 0;

                //LOS_Log("\n   -----------> grab LR complete at %9Ld us \n\n", LOS_getSystemTime(), NULL, NULL, NULL, NULL, NULL);
                time1 = LOS_getSystemTime();
                LOS_Log(
                "\n   -----------> grab LR complete at %.3Lfs ( %9Ld us)\n",
                (time1 / ( double ) 1000000),
                time1,
                NULL,
                NULL,
                NULL,
                NULL);
            }
        }


        g_grabLR_count++;
        g_grabLR_countglobal++;

        if ((g_grabLR_infinite != INFINITY) && (g_grabLR_infinite != 0))
        {
            g_grabLR_infinite--;
        }


        if (g_grabLR_infinite)
        {
            // store time info
#ifdef TIMELOG
            eventtimelog.grabLRinfo.time[eventtimelog.grabLRinfo.index] = LOS_getSystemTime();
            eventtimelog.grabLRinfo.timestamp[eventtimelog.grabLRinfo.index] = timestamp;
            eventtimelog.grabLRinfo.infoid[eventtimelog.grabLRinfo.index] = ( enum e_ispctlInfo ) info_id;  //__NO_WARNING__
            eventtimelog.grabLRinfo.index = (eventtimelog.grabLRinfo.index + 1) % 60;
#endif

            //mle ITE_CleanBuffer(GrabBufferLR);
            NMFCALL (grabCommand, execute) (GRBPID_PIPE_LR, grabparamsLR, GRBPID_PIPE_LR);
        }
        else
        {
            ITE_AddEvent(event);
        }
    }
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabLRError
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab error
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabLRError(
void                *ap_this,
enum e_grabError    error_id,
t_uint16            data,
t_uint16            buffer_id,
t_uint32            timestamp)
{
    //At this time, no error management is defined => we only monitor error on console

    /*
                   union u_ITE_Event event;
                   event.type = ITE_EVT_GRABLR_ERROR;
                   event.grabLRError.error_id = error_id;
                   event.grabLRError.data = data;
                   event.grabLRError.buffer_id = buffer_id;
                   event.grabLRError.timestamp = timestamp;

                   ITE_AddEvent(event);
                   */
    UNUSED(ap_this);
    UNUSED(buffer_id);

    //monitor on console incoming errors
    //LOS_Log("grab LR error occured ... error_id=%d ... buffer_id=%d\n", error_id, buffer_id, NULL, NULL, NULL, NULL);
#ifdef TIMELOG
    eventtimelog.grabLRerror.time[eventtimelog.grabLRerror.index] = LOS_getSystemTime();
    eventtimelog.grabLRerror.timestamp[eventtimelog.grabLRerror.index] = timestamp;
    eventtimelog.grabLRerror.infoid[eventtimelog.grabLRerror.index] = ( enum e_ispctlInfo ) error_id;   //__NO_WARNING__
    eventtimelog.grabLRerror.index = (eventtimelog.grabLRerror.index + 1) % 60;
#endif
    ITE_DisplayGrabError(error_id, data);

    // Add one line in case of False Pipeoverflow
    //	   grabparamsLR.y_frame_size = grabparamsLR.y_frame_size +1;
    //	   grabparamsLR.y_window_size = grabparamsLR.y_window_size +1;
    //	   NMFCALL (grabCommand, execute)(GRBPID_PIPE_LR, grabparamsLR,GRBPID_PIPE_LR);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabLRDebug
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab debug information
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabLRDebug(
void                *ap_this,
enum e_grabDebug    debug_id,
t_uint16            buffer_id,
t_uint16            data1,
t_uint16            data2,
t_uint32            timestamp)
{
    union u_ITE_Event   event;
    UNUSED(ap_this);
    event.type = ITE_EVT_GRABLR_DEBUG;
    event.grabLRDebug.debug_id = debug_id;
    event.grabLRDebug.buffer_id = buffer_id;
    event.grabLRDebug.data1 = data1;
    event.grabLRDebug.data2 = data2;
    event.grabLRDebug.timestamp = timestamp;

#ifdef TIMELOG
    eventtimelog.grabLRdebug.time[eventtimelog.grabLRdebug.index] = LOS_getSystemTime();
    eventtimelog.grabLRdebug.timestamp[eventtimelog.grabLRdebug.index] = timestamp;
    eventtimelog.grabLRdebug.infoid[eventtimelog.grabLRdebug.index] = ( enum e_ispctlInfo ) debug_id;   //__NO_WARNING__
    eventtimelog.grabLRdebug.index = (eventtimelog.grabLRdebug.index + 1) % 60;
#endif
    ITE_AddEvent(event);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabHRInfo
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab processing end
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabHRInfo(void *ap_this, enum e_grabInfo info_id, t_uint16 buffer_id, t_uint32 timestamp)
{   // Two possible events:  GRAB_COMPLETED and GRAB_LAUNCHED
    union u_ITE_Event   event;
    UNUSED(ap_this);
    if ((info_id == GRBINF_GRAB_COMPLETED) || (info_id == GRBINF_GRAB_ABORTED))
    {
        event.type = ITE_EVT_GRABHR_INFO;
        event.grabHRInfo.info_id = info_id;
        event.grabHRInfo.buffer_id = buffer_id;
        event.grabHRInfo.timestamp = timestamp;

        //LOS_Log("grab HR complete ... \n", NULL, NULL, NULL, NULL, NULL, NULL);
        // display image
        g_grabHR_count++;
        g_grabHR_countglobal++;

        /*
                                	if(g_dplHR_infinite==INFINITY) MMTE_MCDE_SwFrameSync();
                                */
        if ((g_grabHR_infinite != INFINITY) && (g_grabHR_infinite != 0))
        {
            g_grabHR_infinite--;
        }


        if (g_grabHR_infinite)
        {
#ifdef TIMELOG
            eventtimelog.grabHRinfo.time[eventtimelog.grabHRinfo.index] = LOS_getSystemTime();
            eventtimelog.grabHRinfo.timestamp[eventtimelog.grabHRinfo.index] = timestamp;
            eventtimelog.grabHRinfo.infoid[eventtimelog.grabHRinfo.index] = ( enum e_ispctlInfo ) info_id;  //__NO_WARNING__
            eventtimelog.grabHRinfo.index = (eventtimelog.grabHRinfo.index + 1) % 60;
#endif

            NMFCALL (grabCommand, execute) (GRBPID_PIPE_HR, grabparamsHR, GRBPID_PIPE_HR);
            if (g_B2R2HR_infinite)
            {
                /*
                                                                             MMTE_B2R2LaunchNodeList(B2R2_AQ3, &g_B2R2_node.B2R2_set_aq_reg);
                                                                */
            }
        }
        else
        {
            ITE_AddEvent(event);
        }
    }
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabHRError
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab error
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabHRError(
void                *ap_this,
enum e_grabError    error_id,
t_uint16            data,
t_uint16            buffer_id,
t_uint32            timestamp)
{
    //At this time, no error management is defined => we only monitor error on console

    /*
                   union u_ITE_Event event;
                   event.type = ITE_EVT_GRABHR_ERROR;
                   event.grabHRError.error_id = error_id;
                   event.grabHRError.data = data;
                   event.grabHRError.buffer_id = buffer_id;
                   event.grabHRError.timestamp = timestamp;

                   ITE_AddEvent(event);
                   */
    UNUSED(buffer_id);
    UNUSED(ap_this);

    //monitor on console incoming errors
    //LOS_Log("grab HR error occured ... error_id=%d ... buffer_id=%d\n", error_id, buffer_id, NULL, NULL, NULL, NULL);
#ifdef TIMELOG
    eventtimelog.grabHRerror.time[eventtimelog.grabHRerror.index] = LOS_getSystemTime();
    eventtimelog.grabHRerror.timestamp[eventtimelog.grabHRerror.index] = timestamp;
    eventtimelog.grabHRerror.infoid[eventtimelog.grabHRerror.index] = ( enum e_ispctlInfo ) error_id;   //__NO_WARNING__
    eventtimelog.grabHRerror.index = (eventtimelog.grabHRerror.index + 1) % 60;
#endif
    ITE_DisplayGrabError(error_id, data);

    //For ZOOM debug only
    if ((error_id == IPP_SD_ERROR_HR) && (data == 256))
    {
        NMFCALL (grabCommand, execute) (GRBPID_PIPE_HR, grabparamsHR, GRBPID_PIPE_HR);
    }
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabHRDebug
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab debug information
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabHRDebug(
void                *ap_this,
enum e_grabDebug    debug_id,
t_uint16            buffer_id,
t_uint16            data1,
t_uint16            data2,
t_uint32            timestamp)
{
    union u_ITE_Event   event;
    UNUSED(ap_this);
    event.type = ITE_EVT_GRABHR_DEBUG;
    event.grabHRDebug.debug_id = debug_id;
    event.grabHRDebug.buffer_id = buffer_id;
    event.grabHRDebug.data1 = data1;
    event.grabHRDebug.data2 = data2;
    event.grabHRDebug.timestamp = timestamp;

#ifdef TIMELOG
    eventtimelog.grabHRdebug.time[eventtimelog.grabHRdebug.index] = LOS_getSystemTime();
    eventtimelog.grabHRdebug.timestamp[eventtimelog.grabHRdebug.index] = timestamp;
    eventtimelog.grabHRdebug.infoid[eventtimelog.grabHRdebug.index] = ( enum e_ispctlInfo ) debug_id;   //__NO_WARNING__
    eventtimelog.grabHRdebug.index = (eventtimelog.grabHRdebug.index + 1) % 60;
#endif
    ITE_AddEvent(event);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabBMSInfo
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab processing end
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabBMSInfo(void *ap_this, enum e_grabInfo info_id, t_uint16 buffer_id, t_uint32 timestamp)
{   // Two possible events:  GRAB_COMPLETED and GRAB_LAUNCHED
    union u_ITE_Event   event;
    UNUSED(ap_this);
    if ((info_id == GRBINF_GRAB_LAUNCHED) || (info_id == GRBINF_GRAB_COMPLETED))
    {
        event.type = ITE_EVT_GRABBMS_INFO;
        event.grabBMSInfo.info_id = info_id;
        event.grabBMSInfo.buffer_id = buffer_id;
        event.grabBMSInfo.timestamp = timestamp;

        if (info_id == GRBINF_GRAB_COMPLETED)
        {
            // display image
            // if(g_dplBMS_infinite==INFINITY) MMTE_MCDE_SwFrameSync();
            g_grabBMS_count++;

            if ((g_grabBMS_infinite != INFINITY) && (g_grabBMS_infinite != 0))
            {
                g_grabBMS_infinite--;
            }


            if (g_grabBMS_infinite)
            {
#ifdef TIMELOG
                eventtimelog.grabBMSinfo.time[eventtimelog.grabBMSinfo.index] = LOS_getSystemTime();
                eventtimelog.grabBMSinfo.timestamp[eventtimelog.grabBMSinfo.index] = timestamp;
                eventtimelog.grabBMSinfo.infoid[eventtimelog.grabBMSinfo.index] = ( enum e_ispctlInfo ) info_id;    //__NO_WARNING__
                eventtimelog.grabBMSinfo.index = (eventtimelog.grabBMSinfo.index + 1) % 60;
#endif

                NMFCALL (grabCommand, execute) (GRBPID_PIPE_RAW_OUT, grabparamsBMS, GRBPID_PIPE_RAW_OUT);
                LOS_Log("[ITE_Callback_grabBMSInfo] BMS Grab Execute sent to NMF Grab Component. \n");              //[Sudeep]
            }
            else
            {
                ITE_AddEvent(event);
                LOS_Log("[ITE_Callback_grabBMSInfo] BMS Grab FillBufferDone Received! \n");                         //[Sudeep]
            }
        }
        else
        {
            ITE_AddEvent(event);
            LOS_Log("[ITE_Callback_grabBMSInfo] BMS Grab Launched Received! (DMA programmed)\n");                   //[Sudeep]
        }
    }
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabBMSError
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab error
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabBMSError(
void                *ap_this,
enum e_grabError    error_id,
t_uint16            data,
t_uint16            buffer_id,
t_uint32            timestamp)
{
    //At this time, no error management is defined => we only monitor error on console

    /*
                   union u_ITE_Event event;
                   event.type = ITE_EVT_GRABBMS_ERROR;
                   event.grabBMSError.error_id = error_id;
                   event.grabBMSError.data = data;
                   event.grabBMSError.buffer_id = buffer_id;
                   event.grabBMSError.timestamp = timestamp;


                   ITE_AddEvent(event);
                   */
    UNUSED(ap_this);
    UNUSED(data);
    UNUSED(buffer_id);
#ifdef TIMELOG
    eventtimelog.grabBMSerror.time[eventtimelog.grabBMSerror.index] = LOS_getSystemTime();
    eventtimelog.grabBMSerror.timestamp[eventtimelog.grabBMSerror.index] = timestamp;
    eventtimelog.grabBMSerror.infoid[eventtimelog.grabBMSerror.index] = ( enum e_ispctlInfo ) error_id; //__NO_WARNING__
    eventtimelog.grabBMSerror.index = (eventtimelog.grabBMSerror.index + 1) % 60;
#endif
    LOS_Log("\ngrab BMS error received !!! \n", NULL, NULL, NULL, NULL, NULL, NULL);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabBMSDebug
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab debug information
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabBMSDebug(
void                *ap_this,
enum e_grabDebug    debug_id,
t_uint16            buffer_id,
t_uint16            data1,
t_uint16            data2,
t_uint32            timestamp)
{
    UNUSED(ap_this);

    union u_ITE_Event   event;
    event.type = ITE_EVT_GRABBMS_DEBUG;
    event.grabBMSDebug.debug_id = debug_id;
    event.grabBMSDebug.buffer_id = buffer_id;
    event.grabBMSDebug.data1 = data1;
    event.grabBMSDebug.data2 = data2;
    event.grabBMSDebug.timestamp = timestamp;

#ifdef TIMELOG
    eventtimelog.grabBMSdebug.time[eventtimelog.grabBMSdebug.index] = LOS_getSystemTime();
    eventtimelog.grabBMSdebug.timestamp[eventtimelog.grabBMSdebug.index] = timestamp;
    eventtimelog.grabBMSdebug.infoid[eventtimelog.grabBMSdebug.index] = ( enum e_ispctlInfo ) debug_id; //__NO_WARNING__
    eventtimelog.grabBMSdebug.index = (eventtimelog.grabBMSdebug.index + 1) % 60;
#endif
    ITE_AddEvent(event);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabBMLInfo
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab processing end
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabBMLInfo(void *ap_this, enum e_grabInfo info_id, t_uint16 buffer_id, t_uint32 timestamp)
{   // Two possible events:  GRAB_COMPLETED and GRAB_LAUNCHED
    union u_ITE_Event   event;
    UNUSED(ap_this);

    if (info_id == GRBINF_GRAB_COMPLETED)
    {
        event.type = ITE_EVT_GRABBML_INFO;
        event.grabBMLInfo.info_id = info_id;
        event.grabBMLInfo.buffer_id = buffer_id;
        event.grabBMLInfo.timestamp = timestamp;

        // display image
        //if(g_dplBML_infinite==INFINITY) MMTE_MCDE_SwFrameSync();
        if ((g_grabBML_infinite != INFINITY) && (g_grabBML_infinite != 0))
        {
            g_grabBML_infinite--;
        }


        if (g_grabBML_infinite)
        {
            if (TraceBMLEvent)
            {
                unsigned long long  time1;

                //LOS_Log("\n   -----------> grab LR complete at %9Ld us \n\n", LOS_getSystemTime(), NULL, NULL, NULL, NULL, NULL);
                time1 = LOS_getSystemTime();
                LOS_Log(
                "\n   -----------> BML complete at %.3Lfs ( %9Ld us)\n",
                (time1 / ( double ) 1000000),
                time1,
                NULL,
                NULL,
                NULL,
                NULL);
            }


#ifdef TIMELOG
            eventtimelog.grabBMLinfo.time[eventtimelog.grabBMLinfo.index] = LOS_getSystemTime();
            eventtimelog.grabBMLinfo.timestamp[eventtimelog.grabBMLinfo.index] = timestamp;
            eventtimelog.grabBMLinfo.infoid[eventtimelog.grabBMLinfo.index] = ( enum e_ispctlInfo ) info_id;    //__NO_WARNING__
            eventtimelog.grabBMLinfo.index = (eventtimelog.grabBMLinfo.index + 1) % 60;
#endif

            NMFCALL (grabCommand, execute) (GRBPID_PIPE_RAW_IN, grabparamsBMS, GRBPID_PIPE_RAW_IN);
        }
        else
        {
            ITE_AddEvent(event);
        }
    }
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabBMLError
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab error
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabBMLError(
void                *ap_this,
enum e_grabError    error_id,
t_uint16            data,
t_uint16            buffer_id,
t_uint32            timestamp)
{
    //At this time, no error management is defined => we only monitor error on console

    /*
                   union u_ITE_Event event;
                   event.type = ITE_EVT_GRABBML_ERROR;
                   event.grabBMLError.error_id = error_id;
                   event.grabBMLError.data = data;
                   event.grabBMLError.buffer_id = buffer_id;
                   event.grabBMLError.timestamp = timestamp;

                   ITE_AddEvent(event);
                   */
    UNUSED(ap_this);
    UNUSED(data);
    UNUSED(buffer_id);

#ifdef TIMELOG
    eventtimelog.grabBMLerror.time[eventtimelog.grabBMLerror.index] = LOS_getSystemTime();
    eventtimelog.grabBMLerror.timestamp[eventtimelog.grabBMLerror.index] = timestamp;
    eventtimelog.grabBMLerror.infoid[eventtimelog.grabBMLerror.index] = ( enum e_ispctlInfo ) error_id; //__NO_WARNING__
    eventtimelog.grabBMLerror.index = (eventtimelog.grabBMLerror.index + 1) % 60;
#endif
    LOS_Log("\ngrab BML error received !!! \n", NULL, NULL, NULL, NULL, NULL, NULL);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_grabBMLDebug
   PURPOSE  : Callback routine for a grab alert. It will be automatically
              called at DSP grab debug information
   ------------------------------------------------------------------------ */
static void
ITE_Callback_grabBMLDebug(
void                *ap_this,
enum e_grabDebug    debug_id,
t_uint16            buffer_id,
t_uint16            data1,
t_uint16            data2,
t_uint32            timestamp)
{
    UNUSED(ap_this);

    union u_ITE_Event   event;
    event.type = ITE_EVT_GRABBML_DEBUG;
    event.grabBMLDebug.debug_id = debug_id;
    event.grabBMLDebug.buffer_id = buffer_id;
    event.grabBMLDebug.data1 = data1;
    event.grabBMLDebug.data2 = data2;
    event.grabBMLDebug.timestamp = timestamp;

#ifdef TIMELOG
    eventtimelog.grabBMLdebug.time[eventtimelog.grabBMLdebug.index] = LOS_getSystemTime();
    eventtimelog.grabBMLdebug.timestamp[eventtimelog.grabBMLdebug.index] = timestamp;
    eventtimelog.grabBMLdebug.infoid[eventtimelog.grabBMLdebug.index] = ( enum e_ispctlInfo ) debug_id; //__NO_WARNING__
    eventtimelog.grabBMLdebug.index = (eventtimelog.grabBMLdebug.index + 1) % 60;
#endif
    ITE_AddEvent(event);
}


/*NOTE:We have categorized all ISPCTL events into categories .
All events assosiated with a particular coin are placed in one category.
We wait for one event once event it is received received we delete that category of events*/

/*
  ISPCTL enum as defined in ispctl.types.idt.h in SIA hw components
  ISP_INF_NONE=0x0,                                                      <<NOT USED IN ITE_NMF>>
  ISP_READ_DONE=0x1,
  ISP_WRITE_DONE=0x2,
  ISP_INIT_DONE=0x3,
  ISP_READLIST_DONE=0x4,
  ISP_WRITELIST_DONE=0x5,
  ISP_POLLING_PE_VALUE_DONE=0x6,
  ISP_HOST_COMMS_READY=0x7,
  ISP_BOOT_COMPLETE=0x8,
  ISP_SLEEPING=0x9,
  ISP_WOKEN_UP=0xA,
  ISP_STREAMING=0xB,
  ISP_STOP_STREAMING=0xC,
  ISP_SENSOR_START_STREAMING=0xD,
  ISP_SENSOR_STOP_STREAMING=0xE,
  ISP_HOST_TO_SENSOR_ACCESS_COMPLETE=0xF,
  ISP_LOAD_READY=0x10,
  ISP_ZOOM_CONFIG_REQUEST_DENIED=0x11,
  ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED=0x12,
  ISP_ZOOM_STEP_COMPLETE=0x13,
  ISP_ZOOM_SET_OUT_OF_RANGE=0x14,
  ISP_RESET_COMPLETE=0x15,
  ISP_GLACE_STATS_READY=0x16,
  ISP_HISTOGRAM_STATS_READY=0x17,
  ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED=0x18,
  ISP_AUTOFOCUS_STATS_READY=0x19,
  ISP_FLADRIVER_LENS_STOP=0x20,
  ISP_ZOOM_OUTPUT_IMAGE_RESOLUTION_READY=0x21,
  ISP_COLOUR_MATRIX_PIPE0_UPDATE_COMPLETE=0x22,                   <<NOT USED>>
  ISP_COLOUR_MATRIX_PIPE1_UPDATE_COMPLETE=0x23,                   <<NOT USED>>
  ISP_SDL_UPDATE_READY=0x24,                                      <<NOT USED>>
  ISP_NVM_EXPORT_DONE=0x25,
  ISP_CDCC_AVAILABLE=0x26,                                        <<NOT USED>>
  ISP_POWER_NOTIFICATION=0x27,
  ISP_SMS_NOTIFICATION=0x28,
  ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION=0x29,
  ISP_VALID_BMS_FRAME_NOTIFICATION=0x30,
  ISP_SENSOR_COMMIT_NOTIFICATION=0x31,
  ISP_ISP_COMMIT_NOTIFICATION=0x32,
  ISP_DEBUG_MODE_ON=0x777,                                        <<NOT USED IN ITE_NMF>>
  ISP_DEBUG_MODE_OFF=0x0FF,                                       <<NOT USED IN ITE_NMF>>
  UNKNOWN_INFO=0x1000
 */

/* Events present in different event categories*/

/*=============================================================================================
      EVENT  CATEGORY                             |         EVENTS

=============================================================================================

 ISP_HOST_TO_SENSOR_ACCESS_COMPLETE_EVENTS        | ISP_HOST_TO_SENSOR_ACCESS_COMPLETE

=============================================================================================

 ITE_EVT_ISPCTL_ZOOM_EVENTS                       | ISP_ZOOM_CONFIG_REQUEST_DENIED

                                                  | ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED

                                                  | ISP_ZOOM_STEP_COMPLETE

                                                  | ISP_ZOOM_SET_OUT_OF_RANGE

==============================================================================================

 ITE_EVT_ISPCTL_STATS_EVENTS                      | ISP_GLACE_STATS_READY
                                                  | ISP_HISTOGRAM_STATS_READY
                                                  | ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED
                                                  | ISP_VALID_BMS_FRAME_NOTIFICATION

==============================================================================================

 ITE_EVT_ISPCTL_FOCUS_EVENTS                      |ISP_AUTOFOCUS_STATS_READY

                                                  |ISP_FLADRIVER_LENS_STOP

================================================================================

 ISP_NVM_EXPORT_DONE_EVENTS                       |ISP_NVM_EXPORT_DONE
================================================================================

 ISP_CDCC_AVAILABLE_EVENTS                        |ISP_CDCC_AVAILABLE

================================================================================

 ISP_POWER_NOTIFICATION_EVENTS                    | ISP_POWER_NOTIFICATION

================================================================================

 ISP_SMS_NOTIFICATION_EVENTS                      |ISP_SMS_NOTIFICATION

================================================================================

 ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION_EVENTS|ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION

================================================================================

 ISP_SENSOR_COMMIT_NOTIFICATION_EVENTS            | ISP_SENSOR_COMMIT_NOTIFICATION

================================================================================

 ISP_ISP_COMMIT_NOTIFICATION_EVENTS               | ISP_ISP_COMMIT_NOTIFICATION

================================================================================

 ITE_EVT_ISPCTL_RW_EVENTS                         | ISP_READ_DONE

                                                  | ISP_WRITE_DONE

================================================================================

 ITE_EVT_ISPCTL_INFO                              | ISP_INIT_DONE

                                                  | ISP_READLIST_DONE

                                                  | ISP_WRITELIST_DONE

                                                  | ISP_POLLING_PE_VALUE_DONE

                                                  | ISP_HOST_COMMS_READY

                                                  | ISP_BOOT_COMPLETE

                                                  | ISP_SLEEPING

                                                  | ISP_WOKEN_UP

                                                  | ISP_STREAMING

                                                  | ISP_STOP_STREAMING

                                                  | ISP_SENSOR_START_STREAMING

                                                  | ISP_SENSOR_STOP_STREAMING

                                                  | ISP_HOST_TO_SENSOR_ACCESS_COMPLETE

                                                  | ISP_LOAD_READY

=================================================================================*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_ispctlInfo
   PURPOSE  : Callback routine for a ispctl alert. It will be automatically
              called at DSP ispctl processing end
   ------------------------------------------------------------------------ */
static void
ITE_Callback_ispctlInfo(void *ap_this, enum e_ispctlInfo info_id, t_uint32 value, t_uint32 timestamp)
{
    UNUSED(ap_this);

    union u_ITE_Event   event;
    event.type = ITE_EVT_NONE;
    event.ispctlInfo.info_id = info_id;
    event.ispctlInfo.value = value;
    event.ispctlInfo.timestamp = timestamp;

    if (event.ispctlInfo.info_id == ISP_HOST_TO_SENSOR_ACCESS_COMPLETE)
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.isphosttosensacc.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.isphosttosensacc.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.isphosttosensacc.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.isphosttosensacc.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ISP_HOST_TO_SENSOR_ACCESS_COMPLETE_EVENTS;
    }
    else if
        (
            (event.ispctlInfo.info_id == ISP_ZOOM_CONFIG_REQUEST_DENIED)
        ||  (event.ispctlInfo.info_id == ISP_ZOOM_CONFIG_REPROGRAM_REQUIRED)
        ||  (event.ispctlInfo.info_id == ISP_ZOOM_STEP_COMPLETE)
        ||  (event.ispctlInfo.info_id == ISP_ZOOM_SET_OUT_OF_RANGE)
        )
    {
#ifdef TIMELOG
        eventtimelog.ispzoominfo.time[eventtimelog.ispzoominfo.index] = LOS_getSystemTime();
        eventtimelog.ispzoominfo.timestamp[eventtimelog.ispzoominfo.index] = timestamp;
        eventtimelog.ispzoominfo.infoid[eventtimelog.ispzoominfo.index] = info_id;
        eventtimelog.ispzoominfo.index = (eventtimelog.ispzoominfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ITE_EVT_ISPCTL_ZOOM_EVENTS;
    }
    else if
        (
            (event.ispctlInfo.info_id == ISP_GLACE_STATS_READY)
        ||  (event.ispctlInfo.info_id == ISP_HISTOGRAM_STATS_READY)
        ||  (event.ispctlInfo.info_id == ISP_EXPOSURE_AND_WB_PARAMETERS_UPDATED)
        ||  (event.ispctlInfo.info_id == ISP_VALID_BMS_FRAME_NOTIFICATION)
        )
    {
        g_statsGlaceHisto_count++;
#ifdef TIMELOG
        eventtimelog.ispstatinfo.time[eventtimelog.ispstatinfo.index] = LOS_getSystemTime();
        eventtimelog.ispstatinfo.timestamp[eventtimelog.ispstatinfo.index] = timestamp;
        eventtimelog.ispstatinfo.infoid[eventtimelog.ispstatinfo.index] = info_id;
        eventtimelog.ispstatinfo.index = (eventtimelog.ispstatinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ITE_EVT_ISPCTL_STATS_EVENTS;
    }
    else if
        (
            (event.ispctlInfo.info_id == ISP_AUTOFOCUS_STATS_READY)
        ||  (event.ispctlInfo.info_id == ISP_FLADRIVER_LENS_STOP)
        )
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.ispfocusinfo.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.ispfocusinfo.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.ispfocusinfo.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.ispfocusinfo.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ITE_EVT_ISPCTL_FOCUS_EVENTS;
    }
    else if (event.ispctlInfo.info_id == ISP_NVM_EXPORT_DONE)
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.ispnvminfo.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.ispnvminfo.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.ispnvminfo.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.ispnvminfo.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ISP_NVM_EXPORT_DONE_EVENTS;
    }
    else if (event.ispctlInfo.info_id == ISP_CDCC_AVAILABLE)
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.ispcdccinfo.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.ispcdccinfo.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.ispcdccinfo.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.ispcdccinfo.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ISP_CDCC_AVAILABLE_EVENTS;
    }
    else if (event.ispctlInfo.info_id == ISP_POWER_NOTIFICATION)
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.isppowerinfo.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.isppowerinfo.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.isppowerinfo.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.isppowerinfo.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ISP_POWER_NOTIFICATION_EVENTS;
    }
    else if (event.ispctlInfo.info_id == ISP_SMS_NOTIFICATION)
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.ispsmsinfo.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.ispsmsinfo.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.ispsmsinfo.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.ispsmsinfo.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ISP_SMS_NOTIFICATION_EVENTS;
    }
    else if (event.ispctlInfo.info_id == ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION)
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.ispsensopmodeinfo.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.ispsensopmodeinfo.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.ispsensopmodeinfo.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.ispsensopmodeinfo.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION_EVENTS;
    }
    else if (event.ispctlInfo.info_id == ISP_SENSOR_COMMIT_NOTIFICATION)
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.ispsenscommitinfo.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.ispsenscommitinfo.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.ispsenscommitinfo.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.ispsenscommitinfo.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ISP_SENSOR_COMMIT_NOTIFICATION_EVENTS;
    }
    else if (event.ispctlInfo.info_id == ISP_ISP_COMMIT_NOTIFICATION)
    {
        g_statsAF_count++;
#ifdef TIMELOG
        eventtimelog.ispispcommitinfo.time[eventtimelog.ispfocusinfo.index] = LOS_getSystemTime();
        eventtimelog.ispispcommitinfo.timestamp[eventtimelog.ispfocusinfo.index] = timestamp;
        eventtimelog.ispispcommitinfo.infoid[eventtimelog.ispfocusinfo.index] = info_id;
        eventtimelog.ispispcommitinfo.index = (eventtimelog.ispfocusinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ISP_ISP_COMMIT_NOTIFICATION_EVENTS;
    }
    else if ((event.ispctlInfo.info_id == ISP_READ_DONE) || (event.ispctlInfo.info_id == ISP_WRITE_DONE))
    {
#ifdef TIMELOG
        eventtimelog.ispfctlrw.time[eventtimelog.ispfctlrw.index] = LOS_getSystemTime();
        eventtimelog.ispfctlrw.timestamp[eventtimelog.ispfctlrw.index] = timestamp;
        eventtimelog.ispfctlrw.infoid[eventtimelog.ispfctlrw.index] = info_id;
        eventtimelog.ispfctlrw.index = (eventtimelog.ispfctlrw.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ITE_EVT_ISPCTL_RW_EVENTS;
    }

    //for gamma

    else if ((event.ispctlInfo.info_id == ISP_LR_GAMMA_UPDATE_COMPLETE) || (event.ispctlInfo.info_id == ISP_HR_GAMMA_UPDATE_COMPLETE))
        {
#ifdef TIMELOG
            eventtimelog.ispfctlrw.time[eventtimelog.ispfctlrw.index] = LOS_getSystemTime();
            eventtimelog.ispfctlrw.timestamp[eventtimelog.ispfctlrw.index] = timestamp;
            eventtimelog.ispfctlrw.infoid[eventtimelog.ispfctlrw.index] = info_id;
            eventtimelog.ispfctlrw.index = (eventtimelog.ispfctlrw.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
            event.type = ITE_EVT_ISPCTL_GAMMA_EVENTS;
        }


    /* This is for all event.type == ITE_EVT_ISPCTL_INFO */
    else
    {
#ifdef TIMELOG
        eventtimelog.ispctlinfo.time[eventtimelog.ispctlinfo.index] = LOS_getSystemTime();
        eventtimelog.ispctlinfo.timestamp[eventtimelog.ispctlinfo.index] = timestamp;
        eventtimelog.ispctlinfo.infoid[eventtimelog.ispctlinfo.index] = info_id;
        eventtimelog.ispctlinfo.index = (eventtimelog.ispctlinfo.index + 1) % SIA_EVENT_TIME_SIZE;
#endif
        event.type = ITE_EVT_ISPCTL_INFO;
    }


    /*Now add the event to the queue */
    ITE_AddEvent(event);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_ispctListlInfo
   PURPOSE  : Callback routine for a ispctl alert. It will be automatically
              called at DSP ispctl processing end
   ------------------------------------------------------------------------ */
static void
ITE_Callback_ispctlInfoList(
void                *ap_this,
enum e_ispctlInfo   info_id,
ts_PageElement      Listdata[],
t_uint16            number_of_pe,
t_uint32            timestamp)
{
    UNUSED(ap_this);

    union u_ITE_Event   event;
    int                 i = 0;
    event.type = ITE_EVT_ISPCTL_INFO;
    event.ispctlInfo.info_id = info_id;
    for (i = 0; i < number_of_pe; i++)
    {
        event.ispctlInfo.Listvalue[i] = Listdata[i];
    }


    event.ispctlInfo.number_of_pe = number_of_pe;
    event.ispctlInfo.timestamp = timestamp;

    ITE_AddEvent(event);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_ispctlError
   PURPOSE  : Callback routine for a ispctl alert. It will be automatically
              called at DSP grab error
   ------------------------------------------------------------------------ */
static void
ITE_Callback_ispctlError(void *ap_this, enum e_ispctlError error_id, t_uint32 value, t_uint32 timestamp)
{
    UNUSED(ap_this);

    union u_ITE_Event   event;
    event.type = ITE_EVT_ISPCTL_ERROR;
    event.ispctlError.error_id = error_id;
    event.ispctlError.last_PE_data_polled = value;  //depends on error_id,  last_PE_data_polled or number_of_pe is the value
    event.ispctlError.number_of_pe = value;
    event.ispctlError.timestamp = timestamp;

#ifdef TIMELOG
    eventtimelog.ispctlerror.time[eventtimelog.ispctlerror.index] = LOS_getSystemTime();
    eventtimelog.ispctlerror.timestamp[eventtimelog.ispctlerror.index] = timestamp;
    eventtimelog.ispctlerror.infoid[eventtimelog.ispctlerror.index] = ( enum e_ispctlInfo ) error_id;   //__NO_WARNING__
    eventtimelog.ispctlerror.index = (eventtimelog.ispctlerror.index + 1) % 60;
#endif
    ITE_AddEvent(event);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_ispctlUnexpectedError
   PURPOSE  : Callback routine for a ispctl alert. It will be automatically
              called at DSP ispctl error
   ------------------------------------------------------------------------ */

/*static void ITE_Callback_ispctlUnexpectedError( void *ap_this, t_eStateId State_id , t_eSignal data, t_uint32 timestamp)
{
   union u_ITE_Event event;
   event.type = ITE_EVT_ISPCTL_ERROR;
   event.ispctlError.state_id = State_id;
   event.ispctlError.signal_id = data;
   event.ispctlError.timestamp = timestamp;

   LOS_Log("\n !!!!!! unexpected ispctl error received !!! \n", NULL, NULL, NULL, NULL, NULL, NULL);
}*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_ispctlDebug
   PURPOSE  : Callback routine for a ispctl alert. It will be automatically
              called at DSP grab debug information
   ------------------------------------------------------------------------ */
static void
ITE_Callback_ispctlDebug(void *ap_this, enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp)
{
    UNUSED(ap_this);

    union u_ITE_Event   event;
    event.type = ITE_EVT_ISPCTL_DEBUG;
    event.ispctlDebug.debug_id = debug_id;
    event.ispctlDebug.data1 = data1;
    event.ispctlDebug.data2 = data2;
    event.ispctlDebug.timestamp = timestamp;
    ITE_AddEvent(event);
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_siaRMInfo
   PURPOSE  : Callback routine for a sia rm alert. It will be automatically
              called when DSP ressource allocation done
   ------------------------------------------------------------------------ */

/*
static void ITE_Callback_siaRMInfo( void *ap_this, enum e_siarmInfo info_id, t_uint16 resources)
{
   union u_ITE_Event event;
   event.type = ITE_EVT_SIARM_INFO;
   event.siarmInfo.info_id = info_id;
   event.siarmInfo.resources = resources;
   ITE_AddEvent(event);
}
*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_siaRMError
   PURPOSE  : Callback routine for a sia rm alert. It will be automatically
              called when DSP ressource allocation done
   ------------------------------------------------------------------------ */

/*
static void ITE_Callback_siaRMError( void *ap_this, enum e_siarmError error_id)
{
   union u_ITE_Event event;
   event.type = ITE_EVT_SIARM_ERROR;
   event.siarmError.error_id = error_id;
   ITE_AddEvent(event);
}
*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Callback_siaRMDebug
   PURPOSE  : Callback routine for a sia rm alert. It will be automatically
              called when DSP ressource allocation done
   ------------------------------------------------------------------------ */

/*
static void ITE_Callback_siaRMDebug( void *ap_this, enum e_siarmDebug debug_id, t_uint16 data1, t_uint16 data2)
{
   union u_ITE_Event event;
   event.type = ITE_EVT_SIARM_DEBUG;
   event.siarmDebug.debug_id = debug_id;
   event.siarmDebug.data1 = data1;
   event.siarmDebug.data2 = data2;
   ITE_AddEvent(event);
}
*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Register_SiaResourceManager
   PURPOSE  : Instanciate, Bind, ... all needed components
              and their respective interfaces.
   ------------------------------------------------------------------------ */

//Ressource Manager encapsulated in scenario function at first

/*
static t_bool ITE_Register_SiaResourceManager( void )
{

}
*/

/* ------------------------------------------------------------------------
   FUNCTION : ITE_Unregister_SiaResourceManager
   PURPOSE  : Destroy, Unbind, ... all needed components
              and their respective interfaces.
   ------------------------------------------------------------------------ */

//Ressource Manager encapsulated in scenario function at first

/*
static t_bool ITE_Unregister_SiaResourceManager( void )
{

}
*/

/* ------------------------------------------------------------------------
   FUNCTION : VAL_enableSiaClocks
   PURPOSE  : enable all SIA clocks (previously done by MMDSP scheduler)
   ------------------------------------------------------------------------ */
void
ITE_enableSiaClocks(void)
{
    //VAL_SiaSetReg(SIA_XBUS_CKG_BASE + SIA_XBUS_SIA_REG_CKEN , 0xFF);
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_SiaEnable
   PURPOSE  : Enable Sia block
   ------------------------------------------------------------------------ */

/*
void ITE_SiaEnable(void)
{

	*(t_uint32 *)(PMU_REG_BASE_ADDR + PMU_SWCR) |= 2;

}
*/

/* -----------------------------------------------------------------------
   FUNCTION : ITE_DisplayGrabError
   PURPOSE  : Display Grab error get from call back in a user friendly way
   ------------------------------------------------------------------------ */
void
ITE_DisplayGrabError(
enum e_grabError    error_id,
t_uint16            buffer_id)
{
    switch (error_id)
    {
        case GRBERR_FIFO_OVERFLOW:
            LOS_Log("\ngrab error_id = GRBERR_FIFO_OVERFLOW\n", NULL, NULL, NULL, NULL, NULL, NULL);
            break;

        case GRBERR_RAW_BUF_OVERFLOW:
            LOS_Log("\ngrab error_id = GRBERR_RAW_BUF_OVERFLOW\n", NULL, NULL, NULL, NULL, NULL, NULL);
            break;

        case GRBERR_FMT_UNSUPPORTED:
            LOS_Log("\ngrab error_id = GRBERR_FMT_UNSUPPORTED\n", NULL, NULL, NULL, NULL, NULL, NULL);
            break;

        case GRBERR_PIPE_UNSUPPORTED:
            LOS_Log("\ngrab error_id = GRBERR_PIPE_UNSUPPORTED\n", NULL, NULL, NULL, NULL, NULL, NULL);
            break;

        case GRBERR_GRB_IN_PROGRESS:
            LOS_Log("\ngrab error_id = GRBERR_GRB_IN_PROGRESS\n", NULL, NULL, NULL, NULL, NULL, NULL);
            break;

        case GRBERR_UNKNOWN:
            LOS_Log("\ngrab error_id = GRBERR_UNKNOWN\n", NULL, NULL, NULL, NULL, NULL, NULL);
            break;

        case GRBERR_STAGINGFULL:
            LOS_Log("\ngrab error_id = GRBERR_STAGINGFULL\n", NULL, NULL, NULL, NULL, NULL, NULL);
            break;

        case IPP_SD_ERROR_HR:
            LOS_Log("\ngrab error_id = IPP_SD_ERROR_HR\n", NULL, NULL, NULL, NULL, NULL, NULL);
            ITE_DisplayIPPSDError(buffer_id);
            break;

        case IPP_SD_ERROR_LR:
            LOS_Log("\ngrab error_id = IPP_SD_ERROR_LR\n", NULL, NULL, NULL, NULL, NULL, NULL);
            ITE_DisplayIPPSDError(buffer_id);
            break;

        case IPP_SD_ERROR_BMS:
            LOS_Log("\ngrab error_id = IPP_SD_ERROR_BMS\n", NULL, NULL, NULL, NULL, NULL, NULL);
            ITE_DisplayIPPSDError(buffer_id);
            break;

        case IPP_SD_ERROR_CCP:
            LOS_Log("\ngrab error_id = IPP_SD_ERROR_CCP\n", NULL, NULL, NULL, NULL, NULL, NULL);
            ITE_DisplayIPPSDError(buffer_id);
            break;

        case IPP_CSI2_DPHY_ERROR:
            LOS_Log("\ngrab error_id = IPP_CSI2_DPHY_ERROR\n", NULL, NULL, NULL, NULL, NULL, NULL);
            LOS_Log("grab buffer_id contains register value = %x\n", buffer_id, NULL, NULL, NULL, NULL, NULL);
            LOS_Log(
            "See enum e_IPP_CSI2_DPHY_Error (in grab_types.idt) to map to register content\n",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
            break;

        case IPP_CSI2_PACKET_ERROR:
            LOS_Log("\ngrab error_id = IPP_CSI2_PACKET_ERROR\n", NULL, NULL, NULL, NULL, NULL, NULL);
            LOS_Log("grab buffer_id contains register value = %x\n", buffer_id, NULL, NULL, NULL, NULL, NULL);
            LOS_Log(
            "See enum e_IPP_CSI2_PACKET_Error (in grab_types.idt) to map to register content\n",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL);
            break;

        default:
            LOS_Log_Err("NO Error Fell through to Default\n");
            break;
    }
}


/* -----------------------------------------------------------------------
   FUNCTION : ITE_DisplayGrabError
   PURPOSE  : Display Grab error get from call back in a user friendly way
   ------------------------------------------------------------------------ */
void
ITE_DisplayIPPSDError(
t_uint16    buffer_id)
{
    if (buffer_id & IPP_SD_ERROR_CCP_SHIFT_SYNC)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_CCP_SHIFT_SYNC\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_CCP_FALSE_SYNC)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_CCP_FALSE_SYNC\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_CCP_CRC_ERROR)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_CCP_CRC_ERROR\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_PIPE0_CH0_OVERFLOW)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_PIPE0_CH0_OVERFLOW\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_PIPE0_CH1_OVERFLOW)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_PIPE0_CH1_OVERFLOW\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_PIPE0_CH2_OVERFLOW)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_PIPE0_CH2_OVERFLOW\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_PIPE1_OVERFLOW)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_PIPE1_OVERFLOW\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_PIPE2_OVERFLOW)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_PIPE2_OVERFLOW\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_OPIPE0_MULT_ERR)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_PIPE0_CHX_MULT_ERR\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_ERROR_OPIPE1_MULT_ERR)
    {
        LOS_Log(
        "buffer_id (content of IPP_SD_ERROR) = IPP_SD_ERROR_PIPE1_MULT_ERR\n",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    }


    if (buffer_id & IPP_SD_CSI2_PACKET_ERROR)
    {
        LOS_Log("buffer_id (content of IPP_SD_ERROR) = IPP_SD_CSI2_PACKET_ERROR\n", NULL, NULL, NULL, NULL, NULL, NULL);
    }


    if (buffer_id & IPP_SD_CSI2_DPHY_ERROR)
    {
        LOS_Log("buffer_id (content of IPP_SD_ERROR) = IPP_SD_CSI2_DPHY_ERROR\n", NULL, NULL, NULL, NULL, NULL, NULL);
    }
}


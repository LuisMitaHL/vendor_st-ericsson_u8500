/*
* Copyright (C) ST-Ericsson 2009
*
* agpsfsm.c
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#include "agpsfsm.h"

static t_AgpsFsmContext* pg_AgpsFsmTable;

#define MAX_OSA_AVAILABLE_TIMERS    5

/* Timer Handle structure */
typedef struct
{
    uint64_t               v_TimerID;
    t_OsaTimerHandle       v_Handle;
    uint8_t                v_Used;
} t_AgpsFsmTimer;

/* Define 5 OS timers */
t_AgpsFsmTimer vg_OsaTimers[MAX_OSA_AVAILABLE_TIMERS];

t_OsaCriticSection  g_TimerCriticSection = OSA_DEFAULT_CRITIC_SECTION;

void AgpsFsmHandler( void* p_Param )
{
    bool            v_FsmHandlerExit = FALSE;
    t_MsgHeader     *pl_MsgHddr;
    t_AgpsFsmContext *p_Ctxt = (t_AgpsFsmContext *)p_Param;
    t_RtkObject     v_RtkObject;

    MC_FSM_TRACE_INF("FsmHandler Spawned thread %d, 0x%p" , p_Ctxt->v_ProcessId, p_Ctxt->p_Context );

    /*Thread Main Loop*/
    while ( v_FsmHandlerExit == FALSE )
    {
        const t_OperationDescriptor * p_CurrStateTableIdx;

        pl_MsgHddr = AgpsDeQueueFsmMsg( p_Ctxt );

        if(NULL!=pl_MsgHddr)
        {
            MC_FSM_TRACE_INF("FSM handler Event : 0x%x" , pl_MsgHddr->v_OperationType );

            p_CurrStateTableIdx = (t_OperationDescriptor *)p_Ctxt->p_Context;

            v_RtkObject.u_ReceivedObj.p_MessageAddress = pl_MsgHddr ;
            /* This is currently a hack. Modify interfaces so that we know who the message is from */
            /* Options are RTK_OBJ_MESSAGE / RTK_OBJ_TIMER */
            v_RtkObject.v_TypeOfObj = RTK_OBJ_MESSAGE;

            if (p_CurrStateTableIdx != NULL)
            {
                /*
                    p_CurrStateTableIdx->v_EventId can be one of three types of values :
                    1) user defined : In this case, the matching event handler is executed
                    2) END_OPERATION_TABLE : Search has hit the end of the operation table.
                    3) NEXT_OPERATION_TABLE : Search has to move on to another operation table. 
                                              There is no change in state because of this move
                */

                while( p_CurrStateTableIdx->v_EventId != END_OPERATION_TABLE)
                {
                    if( p_CurrStateTableIdx->v_EventId == NEXT_OPERATION_TABLE )
                    {
                        MC_FSM_TRACE_INF("Moving to Next operation table:");
                        p_CurrStateTableIdx = p_CurrStateTableIdx->p_NextStateCmd;
                    }
                    if(pl_MsgHddr->v_OperationType == (p_CurrStateTableIdx->v_EventId))
                    {
                        MC_FSM_TRACE_INF("FSM handler Found Event Handler in Table, event: 0x%x",p_CurrStateTableIdx->v_EventId);
                        break;
                    }
                    p_CurrStateTableIdx++;
                }

                if( p_CurrStateTableIdx->v_EventId == END_OPERATION_TABLE)
                {
                    MC_FSM_TRACE_INF("FSM handler Did not find Event Handler in Table");
                }


                /* If we have recvd message with event id FSM_SHUTDOWN_EVENT, we should go to shutdown state */

                if( p_CurrStateTableIdx->v_EventId != FSM_SHUTDOWN_EVENT )
                {
                    /*
                        The next state can be of three types :
                        1) SAME: This implies there is no state transtion after this event is handled. Event handler is void function
                        2) user defined state :The FSM transitions to this state after handler is executed. Event handler is void function
                        3) UNKNOWN : The next state is specified by the return of the event handler function. Can be SAME or user defined state
                            */
                    if ( UNKNOWN == p_CurrStateTableIdx->p_NextStateCmd )
                    {
                        const t_OperationDescriptor* pl_State=UNKNOWN;
                        MC_FSM_TRACE_INF("FSM found FSM table with nextcmdstate UNKOWN ");
                        
                        /* Execute the event handler and store the returned state */
                        pl_State = (p_CurrStateTableIdx->p_Function)(&v_RtkObject);
                        if(pl_State == UNKNOWN)
                        {
                            MC_FSM_TRACE_ERR("ERROR: Cannot transition to state UNKNOWN" );
                            v_FsmHandlerExit = TRUE;
                        }
                        else if (pl_State != SAME)
                        {
                            MC_FSM_TRACE_INF("State %p -> State %p" ,p_Ctxt->p_Context,pl_State );
                            p_Ctxt->p_Context = pl_State;
                        }
                    }
                    else
                    {
                        /* Execute the event handler */
                        (p_CurrStateTableIdx->p_Function)(&v_RtkObject);

                        if ( SAME != p_CurrStateTableIdx->p_NextStateCmd )
                        {
                            MC_FSM_TRACE_INF("State %p -> State %p" ,p_Ctxt->p_Context,p_CurrStateTableIdx->p_NextStateCmd );
                            p_Ctxt->p_Context = p_CurrStateTableIdx->p_NextStateCmd;
                        }
                    }
                }
                else
                {
                    v_FsmHandlerExit = TRUE;
                    MC_FSM_TRACE_INF("FSM handler shutdown ");
                }
            }
            else
            {
                MC_FSM_TRACE_INF("FSM event table not found");
            }
            /* Free the received message */
            OSA_Free( pl_MsgHddr );

            v_RtkObject.u_ReceivedObj.p_MessageAddress = NULL;
        }
        else
        {
            MC_FSM_TRACE_ERR("ERROR : AgpsDeQueueFsmMsg returned NULL FSM message!!!");
        }
    };

    return;
}

void AgpsInitFsm( t_AgpsFsmContext *p_Ctxt )
{
    MC_FSM_TRACE_INF("Intializing FSM with process ID : %u, %p" , p_Ctxt->v_ProcessId, p_Ctxt->p_Context );
 
    AgpsInitMsgQueue( p_Ctxt );

    if( (p_Ctxt->p_Stack == NULL ) && (p_Ctxt->v_StackSize == 0 ) )
    {
        MC_FSM_TRACE_INF("OSA_ThreadCreateEx");
        /* No default stack nor stack size provided */
        if ( OSA_ThreadCreateEx( p_Ctxt->v_FsmHandler , AgpsFsmHandler , p_Ctxt , p_Ctxt->v_FsmPriority ) )
        {
            MC_FSM_TRACE_ERR("ERROR :OSA_ThreadCreateEx");
        }
    }
    else if( p_Ctxt->p_Stack != NULL ) 
    {
        MC_FSM_TRACE_INF("OSA_ThreadCreateEx2");
        if ( OSA_ThreadCreateEx2( p_Ctxt->v_FsmHandler , AgpsFsmHandler , p_Ctxt , p_Ctxt->v_FsmPriority , p_Ctxt->p_Stack , p_Ctxt->v_StackSize ) )
        {
            MC_FSM_TRACE_ERR("ERROR :OSA_ThreadCreateEx2");
        }

    }
    else
    {
        MC_FSM_TRACE_INF("OSA_ThreadCreateEx3");
        if ( OSA_ThreadCreateEx3( p_Ctxt->v_FsmHandler , AgpsFsmHandler , p_Ctxt , p_Ctxt->v_FsmPriority , p_Ctxt->v_StackSize ) )
        {
            MC_FSM_TRACE_ERR("ERROR :OSA_ThreadCreateEx3");
        }

    }
}

void AgpsFsmExecInitCallbacks(t_AgpsFsmContext *p_Ctxt)
{
    if( p_Ctxt->p_InitCallback != NULL )
    {
        p_Ctxt->p_InitCallback();
    }
    else
    {
        MC_FSM_TRACE_INF("No InitFunction for process ID : %u" , p_Ctxt->v_ProcessId);
    }
}

void AgpsDeinitFsm( t_AgpsFsmContext *p_Ctxt )
{
    t_MsgHeader *p_MsgHeader=NULL;

    p_MsgHeader = (t_MsgHeader*)OSA_Malloc( sizeof(*p_MsgHeader ) );
    p_MsgHeader->v_OperationType = FSM_SHUTDOWN_EVENT;

    AgpsEnQueueFsmMsg( p_MsgHeader , p_Ctxt );

    if ( OSA_WaitForThreadExit( p_Ctxt->v_FsmHandler ) )
    {
        MC_FSM_TRACE_ERR("ERROR :AgpsDeinitFsm OSA_WaitForThreadExit");
    }


    AgpsDeinitMsgQueue( p_Ctxt );
   
}

void AgpsSpawnFsm( t_AgpsFsmContext a_FsmCtxts[] )
{
    uint8_t  i;

    /* Store the pointer to the table in global variable for future reference */
    pg_AgpsFsmTable = a_FsmCtxts;

    /* Initialising the OSA layer. Any timer , message queue , semaphore initialization is done here */
    AgpsOsaInit();

    /* Initialise the critical section for the FSM timers.
        These timers use the OSA timers to provide interrupts*/
    
    OSA_InitCriticalSection( &g_TimerCriticSection );

    /* We spawn the different FSM as defined in the table */
    for( i=0 ; pg_AgpsFsmTable[i].v_ProcessId != AGPS_INVALID_PROCESS_ID ; i++ )
    {
        AgpsInitFsm( pg_AgpsFsmTable + i );
    }

    /* After all the required FSMs are spawned, we execute the init functions for each of them */
    for( i=0 ; pg_AgpsFsmTable[i].v_ProcessId != AGPS_INVALID_PROCESS_ID ; i++ )
    {
        AgpsFsmExecInitCallbacks( pg_AgpsFsmTable + i );
    }
}

void AgpsShutdownAllFsm()
{
    uint8_t  i;

    for( i=0 ; pg_AgpsFsmTable[i].v_ProcessId != AGPS_INVALID_PROCESS_ID ; i++ )
    {
        AgpsDeinitFsm( pg_AgpsFsmTable + i );
    }

    OSA_DeinitCriticalSection( &g_TimerCriticSection );
    AgpsOsaDeinit();
}

void AgpsSendFsmMsg(   t_ProcessIdentity v_Src ,
                       t_ProcessIdentity v_Destn ,
                       t_OperationType v_OpCode ,
                       t_MsgHeader *p_Msg )
{
    uint8_t i;

    p_Msg->v_Destination = v_Destn;
    p_Msg->v_Source = v_Src;
    p_Msg->v_OperationType = v_OpCode;

    MC_FSM_TRACE_INF("MSG %u -> %u : %x" , v_Src , v_Destn , v_OpCode);

    

    for( i=0 ; (pg_AgpsFsmTable[i].v_ProcessId != AGPS_INVALID_PROCESS_ID) && (pg_AgpsFsmTable[i].v_ProcessId != v_Destn) ; i++ );

    if( pg_AgpsFsmTable[i].v_ProcessId != AGPS_INVALID_PROCESS_ID )
    {
        t_AgpsFsmContext *p_Ctxt = pg_AgpsFsmTable + i;
        /* Check Guard bits now */
        if( (p_Ctxt->v_Guard1 != AGPS_FSM_GUARD_BYTE1) || (p_Ctxt->v_Guard1 != AGPS_FSM_GUARD_BYTE1) )
        {
            MC_FSM_TRACE_ERR("ERROR : Guard bits have changed value!");
        }
        
        /* Dump the message to the appropriate queue */
        AgpsEnQueueFsmMsg( p_Msg , p_Ctxt );
    }
    else
    {
        MC_FSM_TRACE_ERR("ERROR : Invalid Process ID!");
        /* free this message */
        OSA_Free( p_Msg );
    }
}

void AgpsFsmSendTimerMsg( uint64_t v_TimerID )
{
    t_MsgHeader *p_MsgHeader=NULL;

    t_ProcessIdentity v_Process = (t_ProcessIdentity)(v_TimerID >>SHIFT_TIMER_PROC) & 0xFF;

    p_MsgHeader = OSA_Malloc( sizeof(*p_MsgHeader) );

    MC_FSM_TRACE_INF("Callback Timer for process %u", v_Process );
    AgpsSendFsmMsg( v_Process , v_Process , (t_OperationType) v_TimerID & 0x0000FFFF , p_MsgHeader );
}

void AgpsGetFsmState( t_ProcessIdentity v_Process , const t_OperationDescriptor** pp_State )
{
    uint8_t i;

    for( i=0 ; (pg_AgpsFsmTable[i].v_ProcessId != AGPS_INVALID_PROCESS_ID) && ( pg_AgpsFsmTable[i].v_ProcessId != v_Process ) ; i++ );

    if( pg_AgpsFsmTable[i].v_ProcessId != AGPS_INVALID_PROCESS_ID )
    {
        *pp_State = pg_AgpsFsmTable[i].p_Context;
    }
    else *pp_State = NULL;
}
/*+LMSqc42256*/        
void AgpsFsmTimerResetEntry( t_AgpsFsmTimer *p_Param  )
{       
     p_Param->v_Used          = FALSE;        
     p_Param->v_TimerID       = 0;
#if defined(WIN32)
    p_Param->v_Handle         = NULL;          
#else
    p_Param->v_Handle         = (uint32_t) NULL;
#endif
}
/*-LMSqc42256*/

void AgpsFsmSendTimeout( t_AgpsFsmTimer *p_Param )
{
    if( NULL != p_Param )
    {
        t_MsgHeader *p_MsgHeader=NULL;
/*+LMSqc42256*/        
        uint64_t vl_TimerId = p_Param->v_TimerID;
/*-LMSqc42256*/

        t_ProcessIdentity v_Process = (t_ProcessIdentity) (p_Param->v_TimerID >>SHIFT_TIMER_PROC) & 0xFF;
        
        MC_FSM_TRACE_INF("AgpsFsmSendTimeout: FSM Handle %d Timer Id %llx",p_Param->v_Handle, p_Param->v_TimerID );
    
        p_MsgHeader = OSA_Malloc( sizeof(*p_MsgHeader) );


        AgpsFsmTimerResetEntry(p_Param);
       
        AgpsSendFsmMsg( v_Process , v_Process ,(t_OperationType) (vl_TimerId & 0xFFFFFFFF) , p_MsgHeader );
/*-LMSqc42256*/    
    }
}

#ifdef WIN32


VOID CALLBACK AgpsFsmTimerRoutine(PVOID param, BOOLEAN TimerOrWaitFired)
{
    AgpsFsmSendTimeout( (t_AgpsFsmTimer*) param );
}


#elif defined( AGPS_LINUX_FTR )

void AgpsFsmTimerRoutine( void* param)
{
    AgpsFsmSendTimeout( (t_AgpsFsmTimer*) param );
}


#else
#error Platform not defined
#endif

void AgpsFsmStartTimer( uint32_t v_TimerID , uint32_t dueTimeMs )
{
    uint8_t i;
    BOOL done = FALSE;

    /* Search for first available timer */
    /* Requirements to find one are :
        Should have timer ID as passed to the func
        else Should be zero indicating not in use */

    OSA_EnterCriticalSection( &g_TimerCriticSection );

    /* We check if the same timer ID is used if it is, stop the old timer and start the new one! */
    /* Issue a stop request anyway! */
    AgpsFsmStopTimer( v_TimerID );

    for( i=0 ; (i < MAX_OSA_AVAILABLE_TIMERS) && ( !done ) ; i++ )
    {
        if( !vg_OsaTimers[i].v_Used )
        {
            vg_OsaTimers[i].v_TimerID = v_TimerID;
            
            if( OSA_StartTimer( vg_OsaTimers[i].v_Handle , (t_OsaTimerCallback)AgpsFsmTimerRoutine , vg_OsaTimers + i , dueTimeMs ) )
            {
                vg_OsaTimers[i].v_Used = TRUE;
                MC_FSM_TRACE_INF("FSM Handle %x  , Timer %x for process %u",vg_OsaTimers[i].v_Handle,v_TimerID , (v_TimerID >> SHIFT_TIMER_PROC) & 0xFF );
            }
            else
            { 
                MC_FSM_TRACE_INF("FSM No timer for process %u" , (v_TimerID >> SHIFT_TIMER_PROC) & 0xFF );
            }
            done = TRUE;
        }
    }

    OSA_ExitCriticalSection( &g_TimerCriticSection );

    if( i == MAX_OSA_AVAILABLE_TIMERS )
    { 
        MC_FSM_TRACE_INF("No available timer");
    }
}

void AgpsFsmStopTimer( uint32_t v_TimerID  )
{
    uint8_t i;
    
    MC_FSM_TRACE_INF("FSM Stopping Timer with timerID %x",v_TimerID);
    
    for( i=0 ; ( i < MAX_OSA_AVAILABLE_TIMERS ) ; i++ )
    {
        if ( vg_OsaTimers[i].v_TimerID == v_TimerID )
        {
            MC_FSM_TRACE_INF("FSM Stopped Timer %d with timerID %x for process %u", i ,v_TimerID , (v_TimerID >> SHIFT_TIMER_PROC) & 0xFF );
            if( vg_OsaTimers[i].v_Used == TRUE)
               OSA_StopTimer( vg_OsaTimers[i].v_Handle );


            AgpsFsmTimerResetEntry(vg_OsaTimers + i);
           
 
        }
        
     }
/*-LMSqc42256*/ 
}

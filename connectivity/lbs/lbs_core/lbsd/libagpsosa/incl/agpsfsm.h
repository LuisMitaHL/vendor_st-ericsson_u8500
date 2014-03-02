/*
* Copyright (C) ST-Ericsson 2009
*
* agpsfsm.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#ifndef AGPS_FSM_H
#define AGPS_FSM_H

#include "agpsosa.h"
#include "lbsosatrace.h"

/*! \addtogroup FSMAbstraction */
/*@{*/
/* We need to define these only for non-RTK no FSM oriented OSes */
/* In case we have to write the OSA for an FSM oriented OS, map the variables appropriately */


/*********************** MACROS related to definition of FSM Table ***********************/
/*! \addtogroup FSM_Macros FSM MACROS */
/*@{*/
/*!
    \def    AGPS_INVALID_PROCESS_ID
    \brief  This Process ID is not alloweds
*/
#define AGPS_INVALID_PROCESS_ID         0

/* \def     AGPS_FIRST_VALID_PROCESS_ID
   \brief   Defines the first valid process ID. All process IDs should be defined as increments on this ID 
*/
#define AGPS_FIRST_VALID_PROCESS_ID     0x80


#define AGPS_FSM_GUARD_BYTE1            0xAE
#define AGPS_FSM_GUARD_BYTE2            0x16


#define MC_FSM_TRACE_ERR(...)                LBSOSATRACE_PRINT(K_LBSOSATRACE_FSM, K_LBSOSATRACE_LOG_LEV_ERR, __VA_ARGS__)
#define MC_FSM_TRACE_INF(...)                LBSOSATRACE_PRINT(K_LBSOSATRACE_FSM, K_LBSOSATRACE_LOG_LEV_INF, __VA_ARGS__)
#define MC_FSM_TRACE_DBG(...)                LBSOSATRACE_PRINT(K_LBSOSATRACE_FSM, K_LBSOSATRACE_LOG_LEV_DBG, __VA_ARGS__)
#define MC_FSM_TRACE_FNC(...)                LBSOSATRACE_PRINT(K_LBSOSATRACE_FSM, K_LBSOSATRACE_LOG_LEV_FNC, __VA_ARGS__)
#define MC_FSM_TRACE_WRN(...)                LBSOSATRACE_PRINT(K_LBSOSATRACE_FSM, K_LBSOSATRACE_LOG_LEV_WRN, __VA_ARGS__)



/*!
    \def AGPS_NEW_FSM( process_name , process_init_state , init_func )
    \brief Macro used to define a new FSM
    Any new FSM that is to be spawned has to be defined in the FSM table.
    This macro sets up the static initialization of the FSM table
    <br>process_name cannot be \ref AGPS_INVALID_PROCESS_ID.
    <br>process_init_state  is initialization state of the FSM. This should
    be of type \ref t_OperationDescriptor[].
    <br>init_func is of type \ref t_AgpsInitCallback. Not mandatory. NULL when not present
*/

#ifdef AGPS_DONOT_USE_OS_QUEUE
#define AGPS_NEW_FSM( process_name , process_init_state , init_func , process_priority ) \
                                 {\
                                      AGPS_FSM_GUARD_BYTE1,\
                                      process_name,\
                                      {NULL,NULL,OSA_DEFAULT_CRITIC_SECTION,OSA_DEFAULT_SEM_HANDLE},\
                                      process_init_state,\
                                      0,\
                                      process_priority,\
                                      NULL,\
                                      0,\
                                      (t_AgpsInitCallback)init_func,\
                                      AGPS_FSM_GUARD_BYTE2\
                                 },   

#define AGPS_NEW_FSM_STACK_SPECIFIED( process_name , process_init_state , init_func , process_priority , stack_pointer , stack_size ) \
                                 {\
                                      AGPS_FSM_GUARD_BYTE1,\
                                      process_name,\
                                      {NULL,NULL,OSA_DEFAULT_CRITIC_SECTION,OSA_DEFAULT_SEM_HANDLE},\
                                      process_init_state,\
                                      NULL,\
                                      process_priority,\
                                      stack_pointer,\
                                      stack_size,\
                                      (t_AgpsInitCallback)init_func,\
                                      AGPS_FSM_GUARD_BYTE2\
                                 }, 

#define AGPS_NEW_FSM_STACK_SIZE_SPECIFIED( process_name , process_init_state , init_func , process_priority , stack_size ) \
                                 {\
                                      AGPS_FSM_GUARD_BYTE1,\
                                      process_name,\
                                      {NULL,NULL,OSA_DEFAULT_CRITIC_SECTION,OSA_DEFAULT_SEM_HANDLE},\
                                      process_init_state,\
                                      NULL,\
                                      process_priority,\
                                      NULL,\
                                      stack_size,\
                                      (t_AgpsInitCallback)init_func,\
                                      AGPS_FSM_GUARD_BYTE2\
                                 },                                 
#else
#define AGPS_NEW_FSM( process_name , process_init_state , init_func , process_priority ) \
                                 {\
                                      AGPS_FSM_GUARD_BYTE1,\
                                      process_name,\
                                      {0},\
                                      process_init_state,\
                                      NULL,\
                                      process_priority,\
                                      NULL,\
                                      0,\
                                      (t_AgpsInitCallback)init_func,\
                                      AGPS_FSM_GUARD_BYTE2\
                                 },   

#define AGPS_NEW_FSM_STACK_SPECIFIED( process_name , process_init_state , init_func , process_priority , stack_pointer , stack_size ) \
                                 {\
                                      AGPS_FSM_GUARD_BYTE1,\
                                      process_name,\
                                      {NULL,NULL,OSA_DEFAULT_CRITIC_SECTION,OSA_DEFAULT_SEM_HANDLE},\
                                      process_init_state,\
                                      NULL,\
                                      process_priority,\
                                      stack_pointer,\
                                      stack_size,\
                                      (t_AgpsInitCallback)init_func,\
                                      AGPS_FSM_GUARD_BYTE2\
                                 }, 

#define AGPS_NEW_FSM_STACK_SIZE_SPECIFIED( process_name , process_init_state , init_func , process_priority , stack_size ) \
                                 {\
                                      AGPS_FSM_GUARD_BYTE1,\
                                      process_name,\
                                      0,\
                                      process_init_state,\
                                      NULL,\
                                      process_priority,\
                                      NULL,\
                                      stack_size,\
                                      (t_AgpsInitCallback)init_func,\
                                      AGPS_FSM_GUARD_BYTE2\
                                 },
#endif
/*!
    \def AGPS_START_ALL_FSM( X )
    \brief Defines start of the FSM table
    The FSM table defines all FSM that are to be spawned.
*/
#define AGPS_START_ALL_FSM( X )   static t_AgpsFsmContext X [] = {

/*!
    \def AGPS_FINISH_ALL_FSM( X )
    \brief Defined end of the FSM table
*/
#define AGPS_FINISH_ALL_FSM( X )  AGPS_NEW_FSM( AGPS_INVALID_PROCESS_ID , NULL , NULL , 0 ) };

/*@}*/

/*********************** MACROS related to definition of FSM Events/Messages ***********************/

/*! \addtogroup FSM_Structures FSM Structures */
/*@{*/
/*!
    \def MC_PCC_BEGIN_STRUC(StructName)
    \brief Used to define structure where are sent as payload for events

    This macro defines a field of type t_MsgHeader which is used by the FSM
    handler to route the messages. Mandatory to be used with all messages sent
    to FSMs
*/
#define MC_PCC_BEGIN_STRUC(StructName)    \
             typedef struct {       \
             t_MsgHeader    s_MsgHeader; \

/*!
    \def MC_PCC_FIELD(x,y)
    \brief Defines a field in the structure of type x and name y
*/
#define MC_PCC_FIELD(x,y)         x     y;

/*!
    \def MC_PCC_ARRAY(x,y,z)
    \brief Defines a array of size z and of type x. Array name is x
*/
#define MC_PCC_ARRAY(x,y,z)       x     y[z];

/*!
    \def MC_PCC_END_STRUC(StructName)
    \brief Used to end the structure definition.
*/
#define MC_PCC_END_STRUC(StructName)        }StructName;

#define MC_PCC_FILL_STRUCT1     uint8_t  v_FillerU8;        /*!< \brief Padding in structure */

#define MC_PCC_FILL_STRUCT2     uint16_t v_FillerU16;    /*!< \brief Padding in structure*/

#define MC_PCC_FILL_STRUCT3     MC_PCC_FILL_STRUCT1 MC_PCC_FILL_STRUCT2    /*!< \brief Padding in structure*/

/*@}*/

/*********************** MACROS related to OS Timers ***********************/
#define SHIFT_TIMER_PROC    16

/*! \addtogroup FSM_Macros FSM MACROS */
/*@{*/
/*!
    \def MC_RTK_PROCESS_TIMER(ProcessName , Instance , Operation)
    \brief Used to define a OS Timer
*/
#define MC_RTK_PROCESS_TIMER( ProcessName , Instance , Operation )          Operation

/*!
    \def MC_RTK_PROCESS_START_TIMER(TIMERID , COUNT)
    \brief Used to start an OS Timer defined using \ref MC_RTK_PROCESS_TIMER
*/
#define MC_RTK_PROCESS_START_TIMER(TIMERID, COUNT)   \
                                    AgpsFsmStartTimer( TIMERID , COUNT )
/*!
    \def MC_RTK_STOP_TIMER(TIMERID)
    \brief Used to stop an OS Timer defined using \ref MC_RTK_PROCESS_TIMER
*/
#define MC_RTK_STOP_TIMER(TIMERID)     \
                                    AgpsFsmStopTimer(TIMERID)


/*********************** MACROS related to defining and posting of FSM Events ***********************/
/*!
    \def MC_PCC_FUNCTION_TYPE(x,y)
    \brief This is used to generate part of the operation type \ref t_OperationType
    
    <br>[in] x : This is generally an element from an enumeration of the possible message IDs
    <br>[in] y : This is one of the group \ref MessageType. 
                 Different operation types can be generated based on same enum value and differing
                 message type.
                 ex : CONNECT_REQ , CONNECT_IND etc
*/
#define MC_PCC_FUNCTION_TYPE(x,y) (0x0000 | ((x & 0xFF)<<8 ) | (y & 0xFF))

/*!
    \def MC_RTK_PROCESS_OPERATION(x,y)
    \brief This is used to generate the operation type \ref t_OperationType
    
    <br>[in] x : This is the process for which the message is defined
    <br>[in] y : This is return from \ref MC_PCC_FUNCTION_TYPE
*/
#define MC_RTK_PROCESS_OPERATION(x,y) (0x00000000 | ((x & 0xFF) << 16) | (y & 0xFFFF) )

/*!
    \def MC_PCC_HEADER(x,y)
    \brief This is used define the Operation Type value. Defines an enum X with this value.

    <br>[in] x : This is the enum which is defined as Operation Type
    <br>[in] y : This is value of enum. Generally \ref MC_RTK_PROCESS_OPERATION
*/
#define MC_PCC_HEADER(x,y,z,r) enum { z = r };

/*!
    \def   SAME
    \brief This is used in the FSM table definition to indicate 
    that the state of the FSM will not change based on the event handler
    */
#define SAME                    ((const t_OperationDescriptor *)NULL)

/*!
    \def   UNKNOWN
    \brief This is used in the FSM table definition to indicate 
    that the state of the FSM will be based on the return of the event handler.
    If state UNKNOWN is used and there is no return from the event handler, 
    behavior is unspecified
    */
#define UNKNOWN                 (SAME-1)

/*@}*/

/*! \addtogroup FSM_MessageType FSM MessageType*/
/*@{*/
#define PCC_T_REQ 0    /*!< \brief [Request]        Message sent from Owner Process */
#define PCC_T_CNF 1 /*!< \brief [Confirm]        Message sent to Owner Process after REQ from owner */
#define PCC_T_ERR 2 /*!< \brief [Error]            Message sent to Owner Process after REQ from owner */
#define PCC_T_IND 3 /*!< \brief [Indication]    Message sent to Owner Process [Unsolicited] */
#define PCC_T_RSP 4 /*!< \brief [Response]        Message sent from Owner Process on receipt of [Unsolicited] IND */
#define PCC_T_RJT 5 /*!< \brief [Reject]        Message sent from Owner Process on receipt of [Unsolicited] IND */
#define PCC_T_TIM 6 /*!< \brief [Timer]            Message sent to Owner Process from Timer module */
/*@}*/

/*! \addtogroup FSM_Internal FSM Internal */
/*@{*/
/*!
    \def   END_OPERATION_TABLE
    \brief This is internally used to indicate the end of the FSM Table. \ref MC_RTK_FFSM
    */
#define END_OPERATION_TABLE     ((uint32_t)-1 )

/*!
    \def   NEXT_OPERATION_TABLE
    \brief This is internally used to indicate that the current FSM should 
    concat with a FSM defined previously \ref MC_RTK_NTAB
    */
#define NEXT_OPERATION_TABLE    (END_OPERATION_TABLE-1)

/*!
    \def   FSM_SHUTDOWN_EVENT
    \brief This is internally used shutdown the FSM. Not used except through internal FSM calls
    */
#define FSM_SHUTDOWN_EVENT      (END_OPERATION_TABLE-2)
/*@}*/

/*! \addtogroup FSM_Macros FSM MACROS */
/*@{*/
/*!
    \def   MC_RTK_DFSM( NameOfState )
    \brief Defines a new FSM state. Each state is defined by a combination of one
    \ref MC_RTK_DFSM , one or more \ref MC_RTK_OFSM and one \ref MC_RTK_FFSM
    */
#define MC_RTK_DFSM( NameOfState ) \
                const t_OperationDescriptor NameOfState[] = {

/*!
    \def   MC_RTK_NTAB( NameOfState )
    \brief The current FSM state include the FSM table NameOfState. 
    The current state does not get changed to NameOfState
    */
#define MC_RTK_NTAB( NameOfState ) \
                {NEXT_OPERATION_TABLE,NULL,NameOfState}     };

/*!
    \def   MC_RTK_OFSM( OperationType, Transition, NextStateCmd )
    \brief Defines a mapping of an event(OperationType) to an event handler(Transition) 
    and possible next state (NextStateCmd). Next State is one of \ref SAME , \ref UNKNOWN or 
    a state defined by \ref MC_RTK_DFSM
    */
#define MC_RTK_OFSM( OperationType, Transition, NextStateCmd ) \
                {OperationType, (const t_OperationDescriptor *((*)(t_RtkObject*)))Transition, NextStateCmd},

/*!
    \def   MC_RTK_FFSM( FunctionPtr, StateCmd)
    \brief Defines a end of an FSM state. The FunctionPrt handler is called when an event arrives for which
    no event handlers are defined in FSM.
    */
#define MC_RTK_FFSM( FunctionPtr, StateCmd)\
                {FSM_SHUTDOWN_EVENT,NULL,NULL},\
                {END_OPERATION_TABLE,(const t_OperationDescriptor *((*)(t_RtkObject*)))FunctionPtr, StateCmd}   };

/*!
    \def   MC_RTK_GET_ACTIVE_PROCESS_STATE(Process, pp_State )
    \brief Returns the current FSM state of a particular process.
    */
#define MC_RTK_GET_ACTIVE_PROCESS_STATE(Process, pp_State )\
                                   AgpsGetFsmState( Process , pp_State )

/*!
    \def   MC_RTK_SEND_MSG_TO_PROCESS(SENDER , SEND_INST , RECIEVER, RECIEVER_INST , MESSAGE_ID, MESSAGE_DATAP)
    \brief Posts a message MESSAGE_DATAP to process RECIEVER.
    the structure of MESSAGE_DATAP MUST be defined using \ref MC_PCC_BEGIN_STRUC
    */
#define MC_RTK_SEND_MSG_TO_PROCESS(SENDER , SEND_INST , RECIEVER, RECIEVER_INST , MESSAGE_ID, MESSAGE_DATAP)     \
                                   AgpsSendFsmMsg(SENDER, RECIEVER, MESSAGE_ID, (t_MsgHeader*)MESSAGE_DATAP)

/*!
    \def   MC_RTK_GET_MSG_FROM_RTK_OBJECT(p_FsmObject)
    \brief Access to the Message sent to FSM. 
    */
#define MC_RTK_GET_MSG_FROM_RTK_OBJECT( p_FsmObject )    \
                                   p_FsmObject->u_ReceivedObj.p_MessageAddress    

/*@}*/

/*********************** APIs exposed by FSM Handler ***********************/

/*! \addtogroup FSM_Internal FSM Internal */
/*@{*/
//*****************************************************************************
/// \brief
///      Posts the message to the specified destination process
/// \details
///      Access through \ref MC_RTK_SEND_MSG_TO_PROCESS
/// \returns
///      Nothing
void            AgpsSendFsmMsg(    t_ProcessIdentity    v_Src , 
                                   t_ProcessIdentity    v_Destn , 
                                   t_OperationType      v_OpCode , 
                                   t_MsgHeader          *p_Msg 
                                   );
/*@}*/

//*****************************************************************************
/// \brief
///      This function will spawn the all the FSMs defined using Macro \ref AGPS_START_ALL_FSM
/// \details
///      
/// \returns
///      Nothing
void            AgpsSpawnFsm( t_AgpsFsmContext a_FsmCtxts[] );

//*****************************************************************************
/// \brief
///      This function shutdown down all the spawned FSMs
/// \details
///      
/// \returns
///      Nothing
void            AgpsShutdownAllFsm();

/*! \addtogroup FSM_Internal FSM Internal */
/*@{*/
//*****************************************************************************
/// \brief
///      This function is used to start a timer. Accessed via macro \ref MC_RTK_PROCESS_START_TIMER
/// \details
///      Access through \ref MC_RTK_PROCESS_START_TIMER
/// \returns
///      Nothing
void            AgpsFsmStartTimer( uint32_t v_TimerID , uint32_t dueTimeMs );

//*****************************************************************************
/// \brief
///      This function is used to stop a timer. Accessed via macro \ref MC_RTK_STOP_TIMER
/// \details
///      Access through \ref MC_RTK_STOP_TIMER
/// \returns
///      Nothing
void            AgpsFsmStopTimer( uint32_t v_TimerID  );

/// \brief
///      This function is request current FSM state of a process
/// \details
///      Access through \ref MC_RTK_GET_ACTIVE_PROCESS_STATE
/// \returns
///      Nothing
void            AgpsGetFsmState( t_ProcessIdentity v_Process , 
                                 const t_OperationDescriptor** pp_State 
                                 );
/*@}*/

/*@}*/

#endif /* AGPS_FSM_H */

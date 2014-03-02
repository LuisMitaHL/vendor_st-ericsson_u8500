/*
* Copyright (C) ST-Ericsson 2009
*
* agpsosa.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#ifndef AGPS_OSA_H
#define AGPS_OSA_H

/* 
    The OSA macros morph existing OS functions so that a common interface is
    exposed. All OS functions return TRUE on success and FALSE on failure
*/

/* 
    \defgroup   FSMAbstraction
    \brief      Defines the functions, definitions and 
                data structures associated with FSM
    \defgroup   OSAbstraction
    \brief      Provides a set of OS independent Functionalities

    \defgroup   CompileOptions
    \brief      Different Compile Options available in OSAL
*/

/*! \addtogroup FSMAbstraction */
/*@{*/
    /*!
    
    */
/*@}*/

/*! \addtogroup OSAbstraction */
    /*@{*/
    /*!
    \defgroup   BasicDataTypes
    \brief      Defines the basic data types available.
    \defgroup   CriticalSection
    \brief      Defines the functions, definitions and 
                data structures associated with critical sections
    \defgroup   BinarySemaphore
    \brief      Defines the functions, definitions and 
                data structures associated with Binary Semaphores
    \defgroup   CountingSemaphore
    \brief      Defines the functions, definitions and 
                data structures associated with Counting Semaphore
    \defgroup   Threads
    \brief      Defines the functions, definitions and 
                data structures associated with Threads
    \defgroup   Timers
    \brief      Defines the functions, definitions and 
                data structures associated with Timers
    \defgroup   Sockets
    \brief      Defines the functions, definitions and 
                data structures associated with sockets
    \defgroup   Files
    \brief      Defines the functions, definitions and 
                data structures associated with Files
    \defgroup   MemoryManagement
    \brief      Defines the functions associated with
                Memory Management
    \defgroup   FSM_Support
    \brief      Defines a set of services which are required for
                the FSM implementation
    \defgroup   Misc
    \brief      All other stuff which cannot be easily grouped.
    */
/*@}*/

/*! \addtogroup BasicDataTypes */
    /*@{*/
        /*!\defgroup Windows_BasicDataTypes Windows */
        /*!\defgroup Linux_BasicDataTypes Linux */
    /*@}*/
/*! \addtogroup CriticalSection */
    /*@{*/
        /*!\defgroup Windows_CriticalSection Windows */
        /*!\defgroup Linux_CriticalSection Linux */
    /*@}*/
/*! \addtogroup BinarySemaphore */
    /*@{*/
        /*!\defgroup Windows_BinarySemaphore Windows */
        /*!\defgroup Linux_BinarySemaphore Linux */
    /*@}*/
/*! \addtogroup CountingSemaphore */
    /*@{*/
        /*!\defgroup Windows_CountingSemaphore Windows */
        /*!\defgroup Linux_CountingSemaphore Linux */
    /*@}*/
/*! \addtogroup Threads */
    /*@{*/
        /*!\defgroup Windows_Threads Windows */
        /*!\defgroup Linux_Threads Linux */
    /*@}*/
/*! \addtogroup Timers */
    /*@{*/
        /*!\defgroup Windows_Timers Windows */
        /*!\defgroup Linux_Timers Linux */
    /*@}*/
/*! \addtogroup Sockets */
    /*@{*/
        /*!\defgroup Windows_Sockets Windows */
        /*!\defgroup Linux_Sockets Linux */
    /*@}*/
/*! \addtogroup Files */
    /*@{*/
        /*!\defgroup Windows_Files Windows */
        /*!\defgroup Linux_Files Linux */
    /*@}*/
/*! \addtogroup MemoryManagement */
    /*@{*/
        /*!\defgroup Windows_MemoryManagement Windows */
        /*!\defgroup Linux_MemoryManagement Linux */
    /*@}*/
/*! \addtogroup Misc */
    /*@{*/
        /*!\defgroup Windows_Misc Windows */
        /*!\defgroup Linux_Misc Linux */
    /*@}*/

#ifdef __RTK_E__

#define ACCESS_RIGHTS_ALL
/*RTK type definition*/
#include "master.h"
#undef ACCESS_RIGHTS_ALL

#elif defined( AGPS_LINUX_FTR )
#include "agpslinuxosa.h"
#elif defined( WIN32 )
#include "agpswinosa.h"
#else
/* Add new platforms here */
#error Unknown Platform
#endif /* #ifdef __RTK_E__ */

#ifndef __RTK_E__

/*! \addtogroup FSM_Support FSM Support*/
/*@{*/
/*!
    \brief This is used to uniquely identify each FSM instantiated
    All communication with a particular FSM happens through this ID.
    */
typedef uint8_t  t_ProcessIdentity;
/*!
    \brief The type of operation to be posted to an FSM
    This is a \ref uint32_t value. Also refer
    */
typedef uint32_t t_OperationType;
/*!
    \brief This is a dummy variable for compatibility with RTK environment.IGNORE
    */
typedef uint8_t  t_ProcessInstance;

/*!
    \struct t_MsgHeader
    \brief This structure is used to identify any message posted to an FSM
    */
typedef struct
{
  t_ProcessIdentity    v_Source; /*!< \brief FSM FROM which the message is posted */
  t_ProcessIdentity    v_Destination; /*!< \brief FSM TO which the message is posted */
  uint16_t                  v_MsgLength; /*!< \brief  Length of Message. Not implemented */
  t_OperationType      v_OperationType; /*!< \brief Message Identifier */
}t_MsgHeader;

#ifdef AGPS_DONOT_USE_OS_QUEUE
/*!
    \struct _t_MsgQueueElement
    \brief Defines an element of a queue. Enabled with flag AGPS_DONOT_USE_OS_QUEUE
*/
struct _t_MsgQueueElement
{
    t_MsgHeader *p_MsgHdr;    /*!< \brief A message posted to the Queue*/
    struct _t_MsgQueueElement *p_NextMsg; /*!< \brief Points to Next message in the Queue*/
} ;

/*!
    \brief Queue Element
    */
typedef struct _t_MsgQueueElement t_MsgQueueElement;

/*!
    \struct t_MsgQueue
    \brief Internal Queue Implementation. Enabled with flag AGPS_DONOT_USE_OS_QUEUE
*/
typedef struct
{    
    t_MsgQueueElement   *p_Head;        /*!< \brief Head of Queue*/
    t_MsgQueueElement   *p_Tail;        /*!< \brief Tail of Queue*/
#ifdef AGPS_LINUX_FTR    
    t_OsaSemHandle      v_Critic;       /*!< \brief Critical section which controls access to the queue */
#else
    t_OsaCriticSection  v_Critic;       /*!< \brief Critical section which controls access to the queue */
#endif    
    t_OsaSemHandle      v_CountingSem;  /*!< \brief Counting semaphore used to keep a count of queued up events */
} t_MsgQueue;
#else
typedef int t_MsgQueue;
#endif


/*!
    \typedef t_AgpsInitCallback
    \brief   This is the init callback defined for every FSM instance.

    The callback defined here is executed in the context of the
    spawner of the FSM and not in the FSM context itself.
    This callback is executed after the FSM is spawned
*/
typedef void ( *t_AgpsInitCallback ) (void);

#define RTK_OBJ_MESSAGE         0x00    /*!< \brief The RTK object in t_RtkObject is a message from another FSM */
#define RTK_OBJ_TIMER           0x01    /*!< \brief The RTK object in t_RtkObject is a message from TIMER */

/*!
    \struct t_RtkObject
    \brief Any FSM event is defined by the following RTK object
*/
typedef struct{
union {
      uint32_t           v_TimerID;          /*!< \brief This is currently dummy to emulate RTK */
      t_MsgHeader   *p_MessageAddress;    /*!< \brief The message for the FSM */
   }u_ReceivedObj;                        /*!< \brief Union. Choice depends on v_TypeOfObj */
   uint8_t   v_TypeOfObj;                    /*!< \brief Identifies the type of object \ref RTK_OBJ_MESSAGE or \ref RTK_OBJ_TIMER */
}t_RtkObject;

/*!
    \struct st_OperationDescriptor
    \brief Any FSM event handler is defined by this structure
*/
struct st_OperationDescriptor {
  t_OperationType                        v_EventId;    /*!< \brief The Event */
  const struct st_OperationDescriptor *(*p_Function )(t_RtkObject * ); /*!< \brief The event handler */
  const struct st_OperationDescriptor*    p_NextStateCmd; /*!< \brief The next state of FSM following the event. \ref SAME \ref UNKNOWN */
};

/*!
    \brief Event Handler
    */
typedef struct st_OperationDescriptor t_OperationDescriptor;

/*!
    \struct t_AgpsFsmContext
    \brief  This structure defines the context for an FSM.

    Every FSM is defined by a set of State Tables which in turn define
    the transition between the different states of the FSM.

*/
typedef struct
{
    uint32_t                        v_Guard1;
    t_ProcessIdentity               v_ProcessId;    /*< \brief Unique FSM identifier */
    t_MsgQueue                      v_Queue;        /*< \brief Event queue for the FSM */
    const t_OperationDescriptor*    p_Context;      /*< \brief Current state of the FSM */
    t_OsaThreadHandle               v_FsmHandler;   /*< \brief The thread on which the FSM is instantiated */
    uint32_t                        v_FsmPriority;  /*< \brief The priority to the FSM assigned by OS */
    uint8_t*                        p_Stack;        /*< \brief Stack to use for Thread */
    uint32_t                        v_StackSize;    /*< \brief Size of stack to be allocated from heap */
    t_AgpsInitCallback              p_InitCallback; /*< \brief The initialization function of each FSM. This callback is not executed from the FSM context */
    uint32_t                        v_Guard2;
} t_AgpsFsmContext;


#ifdef HEAP_MEMORY_DEBUG
#define SIGNATURE_SIZE 10
#define ARRAY_SIZE     5000
#endif

/* Platform Independent */

#define NO_BLOCKING_UNEXPECTED_FSM_MSG 0xFFFFFFFF
#define BLOCKING        0x01
#define NO_BLOCKING     0X02

/*@}*/

/* Start RTK Specific Mapping */

/* + LMSqc36093 libagpsosa - implementaion for fseek. */
#define    GFL_SEEK_SET     SEEK_SET
#define    GFL_SEEK_CUR     SEEK_CUR
#define    GFL_SEEK_END     SEEK_END

/* - LMSqc36093 libagpsosa - implementaion for fseek. */
#define ERROR_1

#ifdef HEAP_MEMORY_DEBUG

#define MC_RTK_GET_MEMORY(SIZE)                     OSA_Malloc_Debug(__FILE__,__FUNCTION__,__LINE__,SIZE)
#define MC_RTK_FREE_MEMORY(POINTER)                 OSA_Free_Debug(__FILE__,__FUNCTION__,__LINE__,POINTER)

#else

#define MC_RTK_GET_MEMORY(SIZE)                     OSA_Malloc(SIZE)
#define MC_RTK_FREE_MEMORY(POINTER)                 OSA_Free(POINTER)

#endif

#define MC_RTK_CREATE_PROCESS_INSTANCE(ProcessName, InstanceId) *InstanceId = 0; /* Dummy code */

#define MC_RTK_GET_MSG_OPTYPE(pMsgAddr)             ((t_MsgHeader*)pMsgAddr)->v_OperationType

#define MC_RTK_SEM_CONSUME( Semaphore)              OSA_WaitForSemSignalled( Semaphore ) // OSA_WaitOnBinarySemaphore( Semaphore )

#define MC_RTK_SEM_PRODUCE( Semaphore)              OSA_SemCount( Semaphore ) //OSA_SignalBinarySemaphore( Semaphore )

#define MC_ERR_RAISE(Type, pl_State, pFsmObject) /*Raghav TBD*/

#define MC_DIN_MILLISECONDS_TO_TICK( tickcount )    tickcount

#define MC_DIN_ERROR( BLOCKING, ERROR_1 ) /*Raghav TBD*/

typedef t_OsaFile t_gfl_FILE;

#define MC_GFL_FWRITE(DataPointer, NoOfStructs, Size, FileHandler) \
                                                    OSA_FileWrite( DataPointer, NoOfStructs, Size, FileHandler )
#if defined( AGPS_LINUX_FTR )
#define MC_GFL_WRITE(FileHandler, DataPointer, Size) \
                                                    OSA_Write(FileHandler, DataPointer, Size)
#endif

#define MC_GFL_FREAD(DataPointer, NoOfStructs, Size, FileHandler) \
                                                    OSA_FileRead( DataPointer, NoOfStructs, Size, FileHandler )

#if defined( AGPS_LINUX_FTR )
#define MC_GFL_MKFIFO(FileName, Mode)                OSA_MakeFIFO((const char*)FileName, (const char*)Mode)
#define MC_GFL_UNLINK(FileName)                OSA_Unlink((const char*)FileName)
#endif

#define MC_GFL_FOPEN(FileName, Mode)                OSA_FileOpen((const char*)FileName, (const char*)Mode)
#if defined( AGPS_LINUX_FTR )
#define MC_GFL_OPEN(FileName, Mode)                OSA_Open((const char*)FileName, (int)Mode)
#endif

#define MC_GFL_FCLOSE(FileHandler)                  OSA_FileClose(FileHandler)
#if defined( AGPS_LINUX_FTR )
#define MC_GFL_CLOSE(FileHandler)                  OSA_Close(FileHandler)
#endif

#define MC_GFL_FSEEK(FileHandler, Offset, Whence)   OSA_FileSeek(FileHandler, Offset, Whence)

#define MC_GFL_FTELL(FileHandler)                   OSA_FileTell(FileHandler)

#define MC_GFL_FSTAT(FileHandler, StatInstance)     OSA_FileFstat(FileHandler, StatInstance)

/* - LMSqc36093 libagpsosa - implementaion for fseek. */

#define MC_GFL_FFLUSH(FileHandler)                  OSA_FileFlush(FileHandler)
#define MC_GFL_REMOVE(FileName)                     OSA_FileDelete((const char*)FileName)

#define MC_GSEC_CLK_GET_SECURE_CLOCK()              OSA_GetRtcSecondCount()

#if defined( AGPS_LINUX_FTR )
#define MC_GFL_OPENDIR(DirPath)                OSA_opendir(DirPath)
#define MC_GFL_READDIR(DirPath)                OSA_readdir(DirPath)
#define MC_GFL_CLOSEDIR(DirPath)               OSA_closedir(DirPath)
#define MC_GFL_STAT(FilePath,statistics)       OSA_filestat(FilePath,statistics)
#define     GFL_EOF         EOF

#endif
/* End RTK Specific Mapping */

void            AgpsOsaInit();
void            AgpsOsaDeinit();
void            AgpsInitMsgQueue(t_AgpsFsmContext *p_Ctxt);
void            AgpsDeinitMsgQueue(t_AgpsFsmContext *p_Ctxt);
t_MsgHeader*    AgpsDeQueueFsmMsg( t_AgpsFsmContext *p_FsmCtxt );
void            AgpsEnQueueFsmMsg( t_MsgHeader  *p_Msg  , t_AgpsFsmContext *p_FsmCtxt );

#ifdef DEBUG_LOG_ALL
#define DEBUG_LOG_PRINT_ON
#define DEBUG_LOG_LEV2
#endif

#ifdef DEBUG_LOG
#define DEBUG_LOG_PRINT( X )                OSA_Print( X )
#else
#define DEBUG_LOG_PRINT( X )
#endif

#define DEBUG_LOG_PRINT_LEV1( X )           DEBUG_LOG_PRINT(X)

#ifdef DEBUG_LOG_LEV2
#define DEBUG_LOG_PRINT_LEV2( X )           DEBUG_LOG_PRINT( X )
#else  /* DEBUG_LOG_LEV2 */ 
#define DEBUG_LOG_PRINT_LEV2( X )
#endif /* DEBUG_LOG_LEV2 */


#define AGPS_MIN(a,b)   ( ( (a) < (b) ) ? (a) : (b) )
#define AGPS_MAX(a,b)   ( ( (a) < (b) ) ? (b) : (a) )

#endif /*ifndef __RTK_E__*/

#endif

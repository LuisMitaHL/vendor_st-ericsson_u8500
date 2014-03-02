/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _INCLUSION_GUARD_T_COMMUNICATION_SERVICE_H_
#define _INCLUSION_GUARD_T_COMMUNICATION_SERVICE_H_
/**
 * @addtogroup ldr_communication_serv
 * @{
 *
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "error_codes.h"
#include "t_security_algorithms.h"
#include "t_command_protocol.h"
#include "t_queue.h"
#include "t_time_utilities.h"


/*******************************************************************************
 * Types, constants and external variables
 ******************************************************************************/
/* Defined protocol families */
typedef enum {
    R15_FAMILY = 0,   /*< R15 protocol family. */
    A2_FAMILY = 1,    /*< A2 protocol family. */
#ifndef CFG_ENABLE_LOADER_TYPE
    TI_FAMILY = 2,    /*< TI protocol family. */
    PROTROM_FAMILY = 3, /*< PROTROM protocol family. */
    Z_FAMILY = 4      /*< Z protocol family. */
#endif
} Family_t;

typedef void (*CommunicationCallback_t)(const void *Data_p, const uint32 Length, void *Param_p);
typedef void (*HashCallback_t)(void *Data_p, const uint32 Length, uint8 *Hash_p, void *Param_p);
typedef ErrorCode_e(*DeviceRead_fn)(void *Data_p, uint32 Length, CommunicationCallback_t Callback_fn, void *Param_p);
typedef ErrorCode_e(*DeviceWrite_fn)(void *Data_p, uint32 Length, CommunicationCallback_t Callback_fn, void *Param_p);
typedef ErrorCode_e(*DeviceCancel_fn)(void *Param_p);
typedef ErrorCode_e(*DeviceSetTimeouts_fn)(void *Timeouts_p, void *Param_p);
typedef void (*HashDeviceCancel_fn)(void *Object_p, void **Param_p);
typedef void (*DeviceCalculate_fn)(void *Object_p, HashType_e Type, void *Data_p, const uint32 Length, uint8 *Hash_p, HashCallback_t Callback_fn, void *Param_p);

/** definition of Buffer interface functions */
typedef ErrorCode_e(*BuffersInit_t)(void *Object_p);
typedef void *(*BufferAllocate_t)(void *Object_p, int BufferSize);
typedef ErrorCode_e(*BufferRelease_t)(void *Object_p, void *Buffer_p, int BufferSize);
typedef uint32(*BuffersAvailable_t)(void *Object_p, int BufferSize);
typedef void (*BuffersDeinit_t)(void *Object_p);


/** definition of Timer interface functions */
typedef ErrorCode_e(*TimersInit_t)(void *Object_p, uint32 Timers);
typedef uint32(*TimerGet_t)(void *Object_p, Timer_t *Timer_p);
typedef ErrorCode_e(*TimerRelease_t)(void *Object_p, uint32 TimerKey);
typedef uint32(*ReadTime_t)(void *Object_p, uint32 TimerKey);
typedef uint32(*GetSystemTime_t)(void *Object_p);

/** definition of Queue interface functions */
typedef void (*FifoCreate_t)(void *Object_p, void **const Queue_pp, const uint32 MaxLength, void (*DestroyElement)(void *Element_p));
typedef void (*FifoDestroy_t)(void *Object_p, void **const Queue_pp);
typedef ErrorCode_e(*FifoEnqueue_t)(void *Object_p, void *const Queue_p, void *const Value_p);
typedef void *(*FifoDequeue_t)(void *Object_p, void *const Queue_p);
typedef QueueCallback_fn(*Fifo_SetCallback_t)(void *Object_p, void *const Queue_p,
        const QueueCallbackType_e Type,
        const QueueCallback_fn Callback,
        void *const Param_p);
typedef boolean(*Fifo_IsEmpty_t)(void *Object_p, const void *const Queue_p);
typedef boolean(*Fifo_IsMember_t)(void *Object_p, const void *const Queue_p, void *Value_p, boolean(*Match)(void *Value1_p, void *Value2_p));
typedef int (*Fifo_GetNrOfElements_t)(void *Object_p, const void *const Queue_p);

typedef void (*RFifoCreate_t)(void *Object_p, void **const Queue_pp, const uint32 MaxLength, void (*DestroyElement)(void *Element_p));
typedef void (*RFifoDestroy_t)(void *Object_p, void **const Queue_pp);
typedef ErrorCode_e(*RFifoEnqueue_t)(void *Object_p, void *const Queue_p, void *const Value_p);
typedef void *(*RFifoDequeue_t)(void *Object_p, void *const Queue_p);
typedef QueueCallback_fn(*RFifo_SetCallback_t)(void *Object_p, void *const Queue_p,
        const QueueCallbackType_e Type,
        const QueueCallback_fn Callback,
        void *const Param_p);
typedef boolean(*RFifo_IsEmpty_t)(void *Object_p, const void *const Queue_p);
typedef boolean(*RFifo_IsMember_t)(void *Object_p, const void *const Queue_p, void *Value_p, boolean(*Match)(void *Value1_p, void *Value2_p));
typedef int (*RFifo_GetNrOfElements_t)(void *Object_p, const void *const Queue_p);

typedef ErrorCode_e(*Do_CEH_Call_t)(void *Object_p, CommandData_t *CmdData_p);


/** Structure with pointers to buffer interface functions */
typedef struct {
    BuffersInit_t      BuffersInit_Fn;       /**< Pointer to function for Buffers
                                                initialization. */
    BufferAllocate_t   BufferAllocate_Fn;    /**< Pointer to function for Buffer
                                                allocation. */
    BufferRelease_t    BufferRelease_Fn;     /**< Pointer to function for Buffer
                                                releasing.*/
    BuffersAvailable_t BuffersAvailable_Fn;  /**< Pointer to function for check
                                                for available buffers with
                                                specified size of buffers. */
    BuffersDeinit_t    BuffersDeinit_Fn;     /**< Pointer to function for Buffers
                                                de-initialization. */
    void              *Object_p;             /**< Pointer for instancing. It is
                                                used in the PC application, but
                                                in the loaders is always NULL */
} BuffersInterface_t;


/** Structure with pointers to timer interface functions */
typedef struct {
    TimersInit_t    TimersInit_Fn;     /**< Pointer to function for Timers
                                          initialization. */
    TimerGet_t      TimerGet_Fn;       /**< Pointer to function for creating and
                                          starting timer. */
    TimerRelease_t  TimerRelease_Fn;   /**< Pointer to function for removing
                                          defined timer. */
    ReadTime_t      ReadTime_Fn;       /**< Pointer to function for read time from
                                          specified timer. */
    GetSystemTime_t GetSystemTime_Fn;  /**< Pointer to function for read current
                                          system time. */
    void           *Object_p;          /**< Pointer for instancing. It is used in
                                          the PC application, but in the loaders
                                          is always NULL */
} TimersInterface_t;

/** Structure with pointers to queue interface functions */
typedef struct {
    FifoCreate_t            FifoCreate_Fn;             /**< Pointer to function
                                                          for Fifo creating.*/
    FifoDestroy_t           FifoDestroy_Fn;            /**< Pointer to function
                                                          for Fifo destroying.*/
    FifoEnqueue_t           FifoEnqueue_Fn;            /**< Pointer to function
                                                          for enqueue packet in
                                                          the fifo. */
    FifoDequeue_t           FifoDequeue_Fn;            /**< Pointer to function
                                                          for dequeue packet
                                                          from the fifo. */
    Fifo_SetCallback_t      Fifo_SetCallback_Fn;       /**< Pointer to function
                                                          for registers an event
                                                          listener for the
                                                          specified queue.*/
    Fifo_IsEmpty_t          Fifo_IsEmpty_Fn;           /**< Pointer to function
                                                          for check, is Fifo
                                                          empty.*/
    Fifo_IsMember_t         Fifo_IsMember_Fn;          /**< Pointer to function
                                                          for Checks if the
                                                          provided element is
                                                          member of the fifo*/
    Fifo_GetNrOfElements_t  Fifo_GetNrOfElements_Fn;   /**< Pointer to function
                                                          for reading the number
                                                          of elements in the
                                                          Fifo.*/

    RFifoCreate_t           RFifoCreate_Fn;            /**< Pointer to function
                                                          for Fifo creating.
                                                          First all interrupts
                                                          are disabled,
                                                          function executed and
                                                          then all interrupt
                                                          enabled. */
    RFifoDestroy_t          RFifoDestroy_Fn;           /**< Pointer to function
                                                          for Fifo destroying.
                                                          First all interrupts
                                                          are disabled,
                                                          function executed and
                                                          then all interrupt
                                                          enabled. */
    RFifoEnqueue_t          RFifoEnqueue_Fn;           /**< Pointer to function
                                                          for enqueue packet in
                                                          the fifo. First all
                                                          interrupts are
                                                          disabled, function
                                                          executed and then all
                                                          interrupt enabled. */
    RFifoDequeue_t          RFifoDequeue_Fn;           /**< Pointer to function
                                                          for dequeue packet
                                                          from the fifo. First
                                                          all interrupts are
                                                          disabled, function
                                                          executed and then all
                                                          interrupt enabled. */
    RFifo_SetCallback_t     RFifo_SetCallback_Fn;      /**< Pointer to function
                                                          for setting two
                                                          callback functions
                                                          used by the FIFO.
                                                          First all interrupts
                                                          are disabled,
                                                          function executed and
                                                          then all interrupt
                                                          enabled. */
    RFifo_IsEmpty_t         RFifo_IsEmpty_Fn;          /**< Pointer to function
                                                          for for check, is Fifo
                                                          empty. First all
                                                          interrupts are
                                                          disabled, function
                                                          executed and then all
                                                          interrupt enabled. */
    RFifo_IsMember_t        RFifo_IsMember_Fn;         /**< Pointer to function
                                                          for checks if the
                                                          provided element is
                                                          member of the fifo.
                                                          First all interrupts
                                                          are disabled,
                                                          function executed and
                                                          then all interrupt
                                                          enabled. */
    RFifo_GetNrOfElements_t RFifo_GetNrOfElements_Fn;  /**< Pointer to function
                                                          for reading the number
                                                          of elements in the
                                                          Fifo. First all
                                                          interrupts are
                                                          disabled, function
                                                          executed and then all
                                                          interrupt enabled. */
    void                   *Object_p;                  /**<Pointer for instancing.
                                                          It is used in the PC
                                                          application, but in the
                                                          loaders is always NULL.*/
} QueueInterface_t;

/**
 * Interface structures for buffers, timers and queue.
 * Structure contain all interface functions for buffers, timers and
 * queue manipulating.
 */
typedef  struct {
    BuffersInterface_t *BufferFunctions_p;  /**< Pointer to buffer interface
                                               structure. */
    QueueInterface_t   *QueueFunctions_p;   /**< Pointer to queue interface
                                               structure. */
    TimersInterface_t  *TimerFunctions_p;   /**< Pointer to timers interface
                                               structure. */
} FunctionInterface_t;

/**
 *  Interface structure for communication device.
 *  Structure contain all functions for communication device manipulating.
 */
typedef struct {
    DeviceRead_fn        Read;           /**< Pointer to function for read data from
                                       communication device. */
    DeviceWrite_fn       Write;          /**< Pointer to function for write data thru the
                                       communication device. */
    DeviceCancel_fn      Cancel;         /**< Pointer to function for canceling current
                                       communication with communication device. */
    void                 *Context_p;     /**< Pointer to Device description data. */
    void                 *Object_p;      /**< Pointer to Object associated with the device. */
} CommunicationDevice_t;

/**
 * Interface structure for HASH device.
 * Structure contain all functions for HASH device manipulating.
 */
typedef struct {
    DeviceCalculate_fn  Calculate;  /**< Pointer to function for calculation. */
    HashDeviceCancel_fn Cancel;     /**< Pointer to function for cancel of
                                       calculation. */
    void               *Object_p;   /**< Pointer for instancing. It is used in the
                                       PC application, but in the loaders is
                                       always NULL.*/
} HashDevice_t;

/**
 * Holds information about a command that is placed (or will be placed) in
 * the command execution queue.
 */
typedef struct ExecutionContext_s {
    void           *Command_p;    /**< Pointer to command register structure
                                     containing information for initialization
                                     and execution of the command.*/
    void           *LocalState_p; /**< If needed holds information internal to the
                                     command, else it is NULL.*/
    boolean         Running;      /**< Indicates the state of the command that is
                                    registered in the Command Execution Service.*/
    CommandData_t   Received;     /**< Holds data for the received command. */
    Result_t        Result;       /**< Pointer to structure that holds command
                                     status and command data, including payload
                                     or output data.*/
    uint32          Progress;    /**< Command completed in percent. */
} ExecutionContext_t;


/** Communication context.*/
typedef struct {
    void                      *Inbound_p;            /**< Pointer to structure for
                                                        handling incoming
                                                        packets.*/
    void                      *Outbound_p;           /**< Pointer to structure for
                                                        handling outgoing
                                                        packets.*/
    HashDevice_t              *HashDevice_p;         /**< Pointer to Hash device
                                                        descriptor. */
    CommunicationDevice_t    *CommunicationDevice_p; /**< Pointer to Communication
                                                        device descriptor. */
    struct FamilyDescriptor_s *Family_p;             /**< Pointer to interface
                                                        structure for protocol
                                                        family. */
    void                      *FamilyContext_p;      /**< Pointer to current
                                                        protocol family context.*/
    uint8                     *BackupCommBuffer_p;   /**< Pointer to backup
                                                        buffer.*/
    uint32                    BackupCommBufferSize;  /**< Size of the backup
                                                        buffer.*/
    FunctionInterface_t       *Functions_p;          /**< Pointer to interface
                                                        functions for buffers,
                                                        timers and queue.*/
    Do_CEH_Call_t             Do_CEH_Call_Fn;        /**< Pointer to callback
                                                        function for handling
                                                        commands received thru
                                                        the LCM.*/
    void                      *Object_p;             /**< Pointer for instancing.
                                                        It is used in the PC
                                                        application, but in the
                                                        loaders is always NULL.*/
    HashType_e                CurrentFamilyHash;     /**< Hashing algorithm that
                                                        is used by the family
                                                        which is active at the
                                                        moment. */
} Communication_t;

/** Structure for initialization and manipulation of protocol family */
typedef struct FamilyDescriptor_s {
    /**< Pointer to Interface function for protocol family initialization. */
    ErrorCode_e(*FamilyInit_fn)(Communication_t *Communication_p);
    /**< Pointer to Interface function for protocol family de-initialization. */
    ErrorCode_e(*FamilyShutdown_fn)(Communication_t *Communication_p);
    /**< Pointer to Pooling function in current protocol family. */
    ErrorCode_e(*Process_fn)(Communication_t *Communication_p);
    /**< Pointer to function for sending packets.*/
    ErrorCode_e(*Send_fn)(Communication_t *Communication_p, void *InputData_p);
    /**< Pointer to function for setting communication timeouts for current protocol family. */
    ErrorCode_e(*SetProtocolTimeouts_fn)(Communication_t *Communication_p, void *TimeoutData_p);
    /**< Pointer to function for getting communication timeouts from current protocol family. */
    ErrorCode_e(*GetProtocolTimeouts_fn)(Communication_t *Communication_p, void *TimeoutData_p);
    /**< Pointer to function for stopping the transmission pool after sending the specified number of packets. */
    ErrorCode_e(*CancelReceiver_fn)(Communication_t *Communication_p, uint8 PacketsBeforeTransferStop);
} FamilyDescriptor_t;

/** @} */
#endif // _INCLUSION_GUARD_T_COMMUNICATION_SERVICE_H_


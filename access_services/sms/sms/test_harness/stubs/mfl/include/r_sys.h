#ifndef INCLUSION_GUARD_R_SYS_H
#define INCLUSION_GUARD_R_SYS_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
*
* DESCRIPTION:
*  
* Header file containing the prototypes for sys category.
*
*
*************************************************************************/

/*************************************************************************
* Includes
*************************************************************************/

#include "t_sys.h"

/** 
* Support for the old function names
*/
#define Do_ClientTag_Set(signal, client_tag) mfl_internal_client_tag_set((signal), (client_tag))

#define ClientTag_Get(a, b)         Do_ClientTag_Get(a, b)
#define ClientTag_Set(a, b)         mfl_internal_client_tag_set((a), (b))
#define ClientTag_Copy(a, b)        Do_ClientTag_Copy(a, b)
#define RequestControlBlock_Init(a) Do_RequestControlBlock_Init(a)

/*************************************************************************
* Declaration of functions 
**************************************************************************/

/**
 * This function returns the Client Tag information from the signal pointed to 
 * by SigStruct_p. It may be used to obtain the client tag information after 
 * having received an Event Channel signal or a response signal (response to a 
 * request function with client tag support).
 *
 * Example:
 *
 * \code
 * static const SIGSELECT AnySignal[] = {0};
 * union SIGNAL*          ResponseSignal_p;
 * ClientTag_t            Tag;
 * ResponseSignal_p = (union SIGNAL*)RECEIVE(AnySignal);
 *
 * Switch (ResponseSignal_p->Primitive)
 * {
 *   case RESPONSE_A_SERVICE:
 *    if (ClientTag_Get(ResponseSignal_p,&Tag)!=GS_CLIENT_TAG_OK )
 *    {
 *      // Handle the fault here
 *      SIGNAL_FREE(&ResponseSignal_p);
 *      return;
 *    }
 *    if( Tag == EXPECTED_TAG)
 *    {
 *      // Continue processing the response signal, possible having 
 *      // to unpack the rest of the response signal data 
 *     . . . 
 *    }
 *    break;
 *  case xxx:
 *    break;
 *  default:
 * }
 *
 * SIGNAL_FREE(&ResponseSignal_p);
 *\endcode
 *
 * @param [in] SigStruct_p Signal to be unpacked.
 * @param [out] Tag_p      Client tag information
 *
 * @retval GS_CLIENT_TAG_OK 
 *
 * @sigbased No
 * @signalid N/A
 * @waitmode N/A
 * @related None
 */

ClientTagStatus_t Do_ClientTag_Get(
    const void *const SigStruct_p,
    ClientTag_t * const Tag_p);


/**
 * This function sets the Client Tag information in the signal pointed to by 
 * SigStruct_p to the value specified in ThisTag. It assumes that the signal 
 * data structure has been defined using SigselectWithClientTag_t instead of 
 * SIGSELECT.
 *
 * Example:
 *
 * \code
 * RequestStatus_t Request_Object_Operation(
 *
 *   const RequestControlBlock_t * const pReqCtrl,
 *   const InParam_t InParameter,
 *   ...,
 *   OutParam_t* const OutParameter_p,
 *   ...)
 * {
 *   Object_OperationSignal_t* SignalToSend_p;
 *   ClientTagStatus_t         Status; 
 *   ....
 *   SignalToSend_p = SIGNAL_ALLOC(Object_OperationSignal_t,
 *                              REQUEST_OBJECT_OPERATION);
 *   Status = ClientTag_Set((void *)SignalToSend_p,
 *                          pReqCtrl->ClientTag);
 *   if (Status != GS_CLIENT_TAG_OK)
 *   {
 *     // handle the error
 *     SIGNAL_FREE(&SignalToSend_p);
 *   }
 *   // Continue processing the request operation
 *   .....
 * }
 * \endcode
 *
 * @param [in] SigStruct_p  Signal to be unpacked.
 * @param [in] ClientTagNew Client Tag value to copy into the signal.
 *
 * @retval GS_CLIENT_TAG_OK 
 *
 * @sigbased No
 * @signalid N/A
 * @waitmode N/A
 * @related None
 */
/*
ClientTagStatus_t 
Do_ClientTag_Set(void* const       SigStruct_p, 
                 const ClientTag_t ClientTagNew);
*/

/**
 * This function copies the Client Tag information from the signal pointed to 
 * by SourceSignal_p into the signal pointed to by Destination_p. It assumes 
 * that the signal data structure of both signals has been defined using 
 * SigselectWithClientTag_t instead of SIGSELECT.
 *
 * Example:
 *
 * \code
 * static const SIGSELECT AnySignal[] = {0};
 * union SIGNAL*          RequestSignal_p;
 * union SIGNAL*          ResponseSignal_p;
 * RequestSignal_p = (union SIGNAL*)RECEIVE(AnySignal);
 *
 * switch (RequestSignal_p->Primitive)
 * {
 *   case REQUEST_OBJECT_OPERATION:
 *     ResponseSignal_p = (Response_Object_Operation_t *)SIGNAL_ALLOC(
 *                           Response_Object_Operation_t,
 *                           RESPONSE_OBJECT_OPERATION);
 *     ClientTag_Copy((void *)ResponseSignal_p,
 *                    (void *)RequestSignal_p);
 *     // Continue processing the request operation
 *     break;
 *   case xxx:
 *     break;
 *   default:
 *   ...
 * }
 * \endcode
 *
 * @param [in, out] DestSignal_p Pointer to the signal where the Client Tag will
 *                               be copied to (input), or has been copied to (output).
 * @param [in] SourceSignal_p    Pointer to the signal from where the Client Tag 
 *                               will be copied. Destination_p Pointer to the 
 *                               signal where the Client Tag will be copied to.
 * @retval GS_CLIENT_TAG_OK 
 *
 * @sigbased No
 * @signalid N/A
 * @waitmode N/A
 * @related None
 */

ClientTagStatus_t Do_ClientTag_Copy(
    void *const DestSignal_p,
    const void *const SourceSignal_p);



/**
 * This function initialises all the elements in ReqCtrlBlk_p. Then, the caller 
 * is expected to set the elements that he uses. 
 *
 * The reason is that the control block may contain other (hidden) elements 
 * that must obtain a default value.
 *
 * Example:
 *
 * \code
 * RequestControlBlock_t RequestCtrlBlock;
 * EndpointId_t          ChannelId;
 * ...
 * RequestControlBlock_Init(&RequestCtrlBlock);
 * RequestCtrlBlock.WaitMode  = NO_WAIT_MODE;
 * RequestCtrlBlock.ClientTag = ChannelId;
 * ...
 * \endcode
 *
 * @param [in, out] ReqCtrlBlk_p  Input: Pointer to the request control block that will be 
 *                                       initialized.
 *                                Output: Pointer to the initialized block.
 * @return void
 *
 * @sigbased No
 * @signalid N/A
 * @waitmode N/A
 * @related None
 */
void Do_RequestControlBlock_Init(
    RequestControlBlock_t * const ReqCtrlBlk_p);


/**
 * Signal Info initialization function. Provides information about the signal.
 *
 * @param [in, out] SignalInfo_p Input:   SignalInfoBlock to intialize.
 *                               Output:  Intialized SignalInfo.
 * @param [in] Signal_p                   The signal.
 *
 * @return void
 */
void Do_SwBP_SignalInfo_Init(
    SwBP_SignalInfo_t * const SignalInfo_p,
    const void *const Signal_p);

/**
 * Returns the Client tag from the signal information.
 *
 * @param [in] SignalInfo_p The signal information to get ClientTag from.
 *
 * @return ClientTag_t The ClientTag value.
 *
 * @sigbased No
 * @signalid N/A
 * @waitmode N/A
 * @related None
 */
ClientTag_t Do_SwBP_SignalInfo_GetClientTag(
    const SwBP_SignalInfo_t * const SignalInfo_p);

/**
 * Returns the Process Id from the signal information
 *
 * @param [in] SignalInfo_p The SignalInfo to retrieve PROCESS id from.
 *
 * @return PROCESS The process id.
 *
 * @sigbased No
 * @signalid N/A
 * @waitmode N/A
 * @related None
 */
PROCESS Do_SwBP_SignalInfo_GetProcessId(
    const SwBP_SignalInfo_t * const SignalInfo_p);

/**
 * This function handles signals containing signal-handling-function-pointer.
 *
 * @param [in] Signal_pp A pointer to a pointer to the signal.
 *
 * @return void
 *
 * @sigbased No
 * @signalid N/A
 * @waitmode N/A
 * @related None
 */
void Do_SwBP_Signal_Handle(
    const union SIGNAL **const Signal_pp);

/*************************************************************************
* Macros for Provider Allocation
* Use when receiving signal buffers of varying and unknown length.
**************************************************************************/

/**
 * Used by service user before call to global service function.
 */
#define RESPONSE_BUFFER_CREATE(HeapArea_p)  \
    (HeapArea_p) = NULL

/**
 * Used by service user after received signal buffer contents has been used.
 */
#define RESPONSE_BUFFER_DESTROY(HeapArea_p) \
  if ((HeapArea_p) != NULL) \
    {                         \
        HEAP_FREE(&HeapArea_p); \
        (HeapArea_p) = NULL;    \
    }

/**
 * Condition used by service provider to detect that service user is prepared
 * to use and honour the provider allocation style in the global service.
 */
#define RESPONSE_BUFFER_IS_CREATED(HeapArea_p) \
    ((HeapArea_p) == NULL)

/**
 * Used by service provider when actual signal buffer size is known.
 */
#define RESPONSE_BUFFER_ALLOCATE(HeapArea_p, size) \
    (HeapArea_p) = HEAP_UNTYPED_ALLOC(size)

///////////////////////////////////////////

#define GS_SELECT_PRIMITIVE(WaitMode, PrimitiveName) ((WaitMode) ? PrimitiveName##_WAIT_MODE : PrimitiveName)

///////////////////////////////////////////

//The following compile time assert macro can be used among statements
#define COMPILE_TIME_ASSERT_STATEMENT(Condition) (void)(1/(int)!!(Condition))


//The following compile time assert macro can be used among declarations

/*lint -esym(752, CompileTimeAssertTestVariable) inhibit lint-message about not used symbol */
/*lint -esym(762, CompileTimeAssertTestVariable) inhibit lint-message about redundantly declared symbol */

#define COMPILE_TIME_ASSERT_DECL(Condition) extern int CompileTimeAssertTestVariable[1/(int)!!(Condition)]

///////////////////////////////////////////
// Version handling in Software backplane

#define GS_IS_COMPATIBLE_VERSION(ClientVersion, ServerVersion) \
  (((ServerVersion) / 100 == (ClientVersion) / 100) && \
   ((ServerVersion) % 100 >= (ClientVersion) % 100)   )

#define GS_VERSION(Interface, Major, Minor) \
  enum {IID_##Interface = (Major) * 100 + (Minor)}

#define GS_USER_ASSUMES_VERSION(Interface, Major, Minor) \
  COMPILE_TIME_ASSERT_DECL(GS_IS_COMPATIBLE_VERSION((Major) * 100 + (Minor), IID_##Interface))

#define GS_VERSION_OF(Interface) IID_##Interface


#endif                          //INCLUSION_GUARD_R_SYS_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>

#include "ENS_WrapperLog.h"
#include "ENS_Wrapper.h"

#include <OMX_debug.h>
#include <ENS_DBC.h>

void sprintCBMessage(char *buffer, CB_Message *message, size_t size)
{
    OMX_BUFFERHEADERTYPE* pBuffer;

    switch(message->type) {
    case OMX_EVENTHANDLER:
        strOMX_EVENTHANDLER(message->args.eventHandler.eEvent,
                            message->args.eventHandler.nData1,
                            message->args.eventHandler.nData2,
                            message->args.eventHandler.pEventData,
                            buffer, size);
        break;
    case OMX_EMPTYBUFFERDONE:
        pBuffer = message->args.bufferDone.pBuffer;
        sprintf(buffer, "EmptyBufferDone: pBuffer=0x%08x", (unsigned int) pBuffer);
        break;
    case OMX_FILLBUFFERDONE:
        pBuffer = message->args.bufferDone.pBuffer;
        sprintf(buffer, "FillBufferDone: pBuffer=0x%08x", (unsigned int) pBuffer);
        break;
    case CB_thread_exit:
        sprintf(buffer, "CB_thread_exit");
        break;
    }
}

/********************************************************************************
 * Call backs to clients
 ********************************************************************************/

void ENS_Wrapper::ENS_CallBackToClientThread(void *)
{
    // Let's enter the function which will exit only once CB_thread_exit message received
    processCBMessage();
    // Notify that we are exiting the ENS_CallBackToClientThread
    LOS_SemaphoreNotify(waitingForClientCallbackExit_sem);
}

void ENS_Wrapper::processCBMessage()
{
    // Make them static to not have them on the stack
    // only one thread is executing this function
    static char message_buffer[WRAPPER_MAX_DEBUG_BUFFER];
    static char header_buffer[WRAPPER_MAX_DEBUG_BUFFER];
    CB_Message *message;

    do {
        // We wait to be notified of a post
        LOS_SemaphoreWait(waitingForClientCallbackProcessing_sem);
        message = (CB_Message *) mClientCBmessageQueue.Pop();
        DBC_ASSERT(message != NULL);

        // Exit has been requested
        if(message->type == CB_thread_exit) {
            pushPreAllocatedCBMessage(message);
            return;
        }

        OMX_COMPONENTTYPE *hComponent = message->hComponent;

        int is_buffer_done_message =
            (message->type == OMX_EMPTYBUFFERDONE) || (message->type == OMX_FILLBUFFERDONE);

        if (OMX_WRAPPER_LOG_LEVEL || OMX_WRAPPER_DEBUG) {
            sprintCBMessage(message_buffer, message, WRAPPER_MAX_DEBUG_BUFFER);
        }
        WRAPPER_DEBUG_LOG(OMX_COMPONENT_DEF_STR " - processCBMessage executing message (0x%08x)",
                          OMX_COMPONENT_DEF_ARG(hComponent), (unsigned int) message);

        if (!is_buffer_done_message) {
            // Non buffer event
            WRAPPER_OMX_LOG(hComponent, OMX_COMPONENT_DEF_STR " sending %s",
                            OMX_COMPONENT_DEF_ARG(hComponent), message_buffer);
        } else {
            // Buffer event
            WRAPPER_OMX_LOG_BUFFER(hComponent, OMX_COMPONENT_DEF_STR " sending %s",
                                   OMX_COMPONENT_DEF_ARG(hComponent), message_buffer);
            if(message->type == OMX_FILLBUFFERDONE) {
                WRAPPER_OMX_LOG_BUFFER(hComponent, "%s",
                                       strOMX_BUFFERHEADERTYPE(message->args.bufferDone.pBuffer,
                                                               header_buffer,
                                                               WRAPPER_MAX_DEBUG_BUFFER));
                if (OMX_dump_enabled && OMX_dump_dir &&
                    OMX_HANDLETYPE_TO_WRAPPER(hComponent)->dumpBuffer) {
                    if (OMX_dump_hex) {
                        ALOGD("%s", strOMX_BUFFERDATA(message->args.bufferDone.pBuffer,
                                                 &OMX_dump_output_buffer,
                                                 &OMX_dump_output_buffer_size,
                                                 OMX_dump_size,
                                                 OMX_BUFFERDATA_NB_END_BYTES,
                                                 OMX_BUFFERDATA_NB_BYTES_PER_LINES));
                    } else if (!OMX_dump_hex && (OMX_dump_idx == message->args.bufferDone.pBuffer->nOutputPortIndex)) {
                        OMX_HANDLETYPE_TO_WRAPPER(hComponent)->dumpData(message->args.bufferDone.pBuffer,
                                                  "OUT",
                                                  OMX_dump_path,
                                                  OMX_dump_idx,
                                                  OMX_dump_size,
                                                  OMX_HANDLETYPE_TO_WRAPPER(hComponent)->name,
                                                  hComponent);
                    }
                }
            }
        }

        switch (message->type) {
        case OMX_EVENTHANDLER:
        {
#ifdef EXECUTE_COMMANDS_SEQUENTIALLY

            LOS_MutexLock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
            if((message->args.eventHandler.eEvent == OMX_EventCmdComplete) ||
               ((OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCmdBeingProcessed == OMX_TRUE) &&
                (message->args.eventHandler.eEvent == OMX_EventError))) {
                if (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCommandWaiting != 0) {
                    // We have a SendCommand on hold : release held lock and notify to unblock thread
                    LOS_MutexUnlock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
                    LOS_SemaphoreNotify(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mwaitingForCmdToComplete);
                } else {
                    // no waiting thread
                    OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mCmdBeingProcessed = OMX_FALSE;
                    LOS_MutexUnlock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
                }
            } else {
                LOS_MutexUnlock(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWrapperMutex_id);
            }

#endif // #ifdef EXECUTE_COMMANDS_SEQUENTIALLY

            if(message->args.eventHandler.eEvent == OMX_EventCmdReceived) {
                // STE OMX internal event - not sent to OMX client
                // No need to acquire lock as component is waiting on semaphore
                if(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait > 0) {
                    OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait--;
                    if(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mNbCmdReceivedToWait == 0) {
                        WRAPPER_OMX_LOG(hComponent, OMX_COMPONENT_DEF_STR
                                        " nbCmdReceivedtoWait == 0 notify for SendCommand release",
                                        OMX_COMPONENT_DEF_ARG(hComponent));
                        LOS_SemaphoreNotify(OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mWaitingForCmdReceived);
                    }
                }
            } else if (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB &&
                       OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB->EventHandler) {
                OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB->
                    EventHandler(hComponent,
                                 message->args.eventHandler.pAppData,
                                 message->args.eventHandler.eEvent,
                                 message->args.eventHandler.nData1,
                                 message->args.eventHandler.nData2,
                                 message->args.eventHandler.pEventData);
            }
        }
        break;
        case OMX_EMPTYBUFFERDONE:
            if (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB &&
                OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB->EmptyBufferDone) {
                OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB->
                    EmptyBufferDone(hComponent,
                                    message->args.bufferDone.pAppData,
                                    message->args.bufferDone.pBuffer);
            }
            break;
        case OMX_FILLBUFFERDONE:
            if (OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB &&
                OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB->FillBufferDone) {
                OMX_HANDLETYPE_TO_WRAPPER(hComponent)->mOMXClientCB->
                    FillBufferDone(hComponent,
                                   message->args.bufferDone.pAppData,
                                   message->args.bufferDone.pBuffer);
            }
            break;
        default:
            DBC_ASSERT(0);
        }
        // Put message back into list of preallocated
        pushPreAllocatedCBMessage(message);
    } while(1);
}

CB_Message * ENS_Wrapper::popPreAllocatedCBMessage()
{
    CB_Message *message = (CB_Message *) mClientCBfreeMessageQueue.Pop();
    DBC_ASSERT(message != NULL);
    return message;
}

void ENS_Wrapper::pushPreAllocatedCBMessage(CB_Message *message)
{
    DBC_ASSERT(mClientCBfreeMessageQueue.Push(message) == 0);
}

void ENS_Wrapper::pushCBMessage(CB_Message *message)
{
    // Lock is protecting this buffer
    static char buffer[WRAPPER_MAX_DEBUG_BUFFER];
    // Push CB message to wrapper's queue
    DBC_ASSERT(mClientCBmessageQueue.Push(message) == 0);
    if (OMX_WRAPPER_DEBUG) {
        sprintCBMessage(buffer, message, WRAPPER_MAX_DEBUG_BUFFER);
        WRAPPER_DEBUG_LOG(OMX_COMPONENT_DEF_STR " - pushCBMessage (0x%08x) %s",
                          OMX_COMPONENT_DEF_ARG(message->hComponent),
                          (unsigned int) message, buffer);
    }
    LOS_SemaphoreNotify(waitingForClientCallbackProcessing_sem);
}

/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   main.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <archi.h>
#include "encoders/framealigned/tester/main.nmf"
#include "dbc.h"
#include "string.h"
#include "fsm/generic/include/FSM.h"
#include <buffer.idt>

typedef enum {
    SET_STATE_EVT = FSM_USER_SIG,
    CMD_RECEIVED_EVT,
    TIMER_IT,
    EOS_EVT
};

typedef struct {
    FSM             fsm;
    OMX_STATETYPE   testerState;
    OMX_STATETYPE   wrapperState;
    bool            testerEosReceived;
    bool            wrapperEosReceived;
    bool            testerCmdReceived;
    bool            wrapperCmdReceived;
} Test;

Test mTest;

static void Test1_step0(Test *this, const FsmEvent *ev);
static void Test1_step1(Test *this, const FsmEvent *ev);
static void Test1_step2(Test *this, const FsmEvent *ev);
static void Test1_step3(Test *this, const FsmEvent *ev);

static void Test2_step0(Test *this, const FsmEvent *ev);
static void Test2_step1(Test *this, const FsmEvent *ev);
static void Test2_step2(Test *this, const FsmEvent *ev);
static void Test2_step3(Test *this, const FsmEvent *ev);

static void Test3_step0(Test *this, const FsmEvent *ev);
static void Test3_step1(Test *this, const FsmEvent *ev);
static void Test3_step2(Test *this, const FsmEvent *ev);

static void Test_exit(Test *this, const FsmEvent *ev);

int argc;
char **argv;
extern int *args_start;

static void
get_arguments(void) {
	argc = *args_start++;
	argv = args_start;
}


static void
startTimer(long cycles) {
    TIMER_mod1msb = (cycles >> 16) & 0xFFFF;
    TIMER_mod1lsb = cycles & 0xFFFF;
}

static void
stopTimer() {
    TIMER_mod1msb = 0;
    TIMER_mod1lsb = 0;
}

static void
configureGraph() {
    char *inputFile, *outputFile;
    int i;
    t_uint16 mNbChans;
    t_uint16  mSamplingFreq;
    t_uint16 fileSize, bufferSize, nbBufIn, nbBufOut;
    fsmInit_t initFsm;
    TraceInfo_t traceInfo;

    get_arguments();
    
    fileSize  = atoi(argv[1]);
    bufferSize = atoi(argv[2]);

    inputFile = malloc_ext(strlen(argv[3])+1);
    strcpy(inputFile, argv[3]);
    outputFile = malloc_ext(strlen(argv[4])+1);
    strcpy(outputFile, argv[4]);
		mSamplingFreq = atoi(argv[5]);
		mNbChans   = atoi(argv[6]);

    memorymgt.initHeap(-2048,0x0,-bufferSize-64536,0x0,0x0,0x0);

    tester_config.setParameter(fileSize, bufferSize, inputFile, outputFile, mSamplingFreq, mNbChans);

    // We shift the args to keep the same parser component between 
    // the streamed and framealigned version
    for (i=0;i<argc-7;i++){
        argv[i] = argv[i+7];
    }
    argc -= 7;

    printf("argc = %d\n",argc);
    
    algoConfParser.configureAlgo(argc,argv);

    initFsm.portsDisabled = 0;
    initFsm.portsTunneled = 0x3;
 	initFsm.traceInfoAddr = &traceInfo;
	initFsm.id1 = 0;
    fsminit.fsmInit(initFsm);
}

static void Test_configuration(Test *this, const FsmEvent *ev){
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            configureGraph();
            ITREMAP_interf_itmskl = 0;
            ITREMAP_interf_itmskh = 0;
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle 
                    && this->wrapperState == OMX_StateIdle) {
                FSM_TRANSITION(Test1_step0);
            }
            break;

        default:
            printf("PB\n");ASSERT(0);
            break;

    }
}

//--------------------------------------------------------------
// Test1:
// test Wrapper state machine transitions:
// Idle -> Executing -> Pause -> Executing -> Idle
//---------------------------------------------------------------


static void Test1_step0(Test *this, const FsmEvent *ev){
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Idle State Test1_step0\n");
            // send SetState(Exec) to buffer user first
            this->testerCmdReceived = false;
            this->wrapperCmdReceived = false;
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            if (this->wrapperCmdReceived) {
                this->wrapperCmdReceived = false;
                tester_command.sendCommand(
                        OMX_CommandStateSet, OMX_StateExecuting);
            }
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateExecuting 
                    && this->wrapperState == OMX_StateExecuting) {
                FSM_TRANSITION(Test1_step1);
            }
            break;

        default:
            ASSERT(0);
            break;

    }
}

// both components in executing
static void Test1_step1(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Executing State Test1_step1\n");
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            break;

        case SET_STATE_EVT:
            if (this->wrapperState == OMX_StatePause) {
                FSM_TRANSITION(Test1_step2);
            }
            break;

        case TIMER_IT:
            stopTimer();
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StatePause);
            break;

            
        default:
            ASSERT(0);
            break;

    }
}

static void Test1_step2(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Wrapper in Pause State Test1_step2\n");
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            break;

        case SET_STATE_EVT:
            if (this->wrapperState == OMX_StateExecuting) {
                FSM_TRANSITION(Test1_step3);
            }
            break;

            
        default:
            ASSERT(0);
            break;

    }
}

static void Test1_step3(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Wrapper back in Executing State Test1_step3\n");
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            if (this->testerCmdReceived){
                this->testerCmdReceived = false;
                wrapper_command.sendCommand(
                        OMX_CommandStateSet, OMX_StateIdle);
            }
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle 
                    && this->wrapperState == OMX_StateIdle) {
                FSM_TRANSITION(Test2_step0);
            }
            break;

        case TIMER_IT:
            stopTimer();
            this->testerCmdReceived = false;
            this->wrapperCmdReceived = false;
            // Send SetState(Idle) to buffer supplier first
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            break;
            
        default:
            ASSERT(0);
            break;

    }
}

//--------------------------------------------------------------
// Test2:
// test Wrapper state machine transitions:
// Idle -> Pause -> Executing -> Pause -> Idle
//---------------------------------------------------------------

static void Test2_step0(Test *this, const FsmEvent *ev) {

    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Idle State Test2_step0\n");
            this->testerCmdReceived = false;
            this->wrapperCmdReceived = false;
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StatePause);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            if (this->wrapperCmdReceived) {
                this->wrapperCmdReceived = false;
                tester_command.sendCommand(
                        OMX_CommandStateSet, OMX_StateExecuting);
            }
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateExecuting 
                    && this->wrapperState == OMX_StatePause) {
                FSM_TRANSITION(Test2_step1);
            }
            break;

        default:
            ASSERT(0);
            break;

    }
}

static void Test2_step1(Test *this, const FsmEvent *ev) {

    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Tester in Executing, Wrapper in Pause Test2_step1\n");
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            break;

        case SET_STATE_EVT:
            if (this->wrapperState == OMX_StateExecuting) {
                FSM_TRANSITION(Test2_step2);
            }
            break;

        default:
            ASSERT(0);
            break;

    }
}

// both components in executing
static void Test2_step2(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Executing State Test2_step2\n");
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            break;

        case SET_STATE_EVT:
            if (this->wrapperState == OMX_StatePause) {
                FSM_TRANSITION(Test2_step3);
            }
            break;

        case TIMER_IT:
            stopTimer();
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StatePause);
            break;

            
        default:
            ASSERT(0);
            break;

    }
}

static void Test2_step3(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Wrapper in Paused State, Tester in Executing Test2_step3\n");
            this->testerCmdReceived = false;
            this->wrapperCmdReceived = false;
            // Send SetState(Idle) to buffer supplier first
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            if (this->testerCmdReceived) {
                this->testerCmdReceived = false;
                wrapper_command.sendCommand(
                        OMX_CommandStateSet, OMX_StateIdle);
            }
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle
                    && this->wrapperState == OMX_StateIdle) {
                FSM_TRANSITION(Test3_step0);
            }
            break;

            
        default:
            ASSERT(0);
            break;

    }
}

//--------------------------------------------------------------
// Test3:
// test Wrapper EOS handling:
// Idle -> Executing -> EOS -> newStream -> Idle
//---------------------------------------------------------------

static void Test3_step0(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Idle State Test3_step0\n");
            this->testerCmdReceived = false;
            this->wrapperCmdReceived = false;
            // Send SetState(Executing) to buffer user first
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            if (this->wrapperCmdReceived) {
                this->wrapperCmdReceived = false;
                tester_command.sendCommand(
                        OMX_CommandStateSet, OMX_StateExecuting);
            }
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateExecuting 
                    && this->wrapperState == OMX_StateExecuting) {
                FSM_TRANSITION(Test3_step1);
            }
            break;

        default:
            ASSERT(0);
            break;

    }
}

// both components in executing
static void Test3_step1(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Executing State : wait EOS event Test3_step1\n");
            break;

        case FSM_EXIT_SIG:
            break;

        case EOS_EVT:
        if (this->wrapperEosReceived
                && this->testerEosReceived) {
            this->wrapperEosReceived = false;
            this->testerEosReceived = false;
            FSM_TRANSITION(Test3_step2);
        }
        break;

        default:
            ASSERT(0);
            break;

    }
}

// both components in executing
static void Test3_step2(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ EOS reached : start a new stream! Test3_step2\n");
            tester_config.restartDataFlow();
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case CMD_RECEIVED_EVT:
            if (this->testerCmdReceived) {
                this->testerCmdReceived = false;
                wrapper_command.sendCommand(
                        OMX_CommandStateSet, OMX_StateIdle);
            }
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle 
                    && this->wrapperState == OMX_StateIdle) {
                FSM_TRANSITION(Test_exit);
            }
            break;

        case TIMER_IT:
            stopTimer();
            this->testerCmdReceived = false;
            this->wrapperCmdReceived = false;
            // Send SetState(Idle) to buffer supplier first
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            break;

        default:
            ASSERT(0);
            break;

    }
}

//--------------------------------------------------------------

static void Test_exit(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Component in Idle state\n");
            printf("+ ALL TESTS PASSED\n");
            exit(0);
            break;

        case FSM_EXIT_SIG:
            break;

        default:
            ASSERT(0);
            break;

    }
}



EE_INTERRUPT void METH(handler)() {
    FsmEvent ev;
    stopTimer();
    ev.signal = TIMER_IT;
    FSM_dispatch(&mTest, &ev);
}


void METH(start)() {
    FSM_init(&mTest, (FSM_State) Test_configuration);

}


void METH(wrapper_eventHandler)(
        OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2) 
{
    FsmEvent ev;

    if (event == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        ev.signal  = SET_STATE_EVT;
        mTest.wrapperState = (OMX_STATETYPE) data2;
        FSM_dispatch(&mTest, &ev);
    }
    else if (event == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {
        ev.signal  = CMD_RECEIVED_EVT;
        mTest.wrapperCmdReceived = true;
        FSM_dispatch(&mTest, &ev);
    }
    else if (event == OMX_EventBufferFlag) {
        ev.signal  = EOS_EVT;
        // check port index
        ASSERT(data1 == 1);
        // check buffer flags
        ASSERT(data2 & BUFFERFLAG_EOS);
        mTest.wrapperEosReceived = true;
        FSM_dispatch(&mTest, &ev);
    }
    else {
        ASSERT(0);
    }
}

void METH(tester_eventHandler)(
        OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2) 
{
    FsmEvent ev;

    if (event == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        ev.signal  = SET_STATE_EVT;
        mTest.testerState = (OMX_STATETYPE) data2;
        FSM_dispatch(&mTest, &ev);
    }
    else if (event == OMX_EventCmdReceived && data1 == OMX_CommandStateSet) {
        ev.signal  = CMD_RECEIVED_EVT;
        mTest.testerCmdReceived = true;
        FSM_dispatch(&mTest, &ev);
    }
    else if (event == OMX_EventBufferFlag) {
        ev.signal  = EOS_EVT;
        mTest.testerEosReceived = true;
        FSM_dispatch(&mTest, &ev);
    }
    else {
        ASSERT(0);
    }
}

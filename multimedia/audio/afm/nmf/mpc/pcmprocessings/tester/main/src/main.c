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
#include "pcmprocessings/tester/main.nmf"
#include "dbc.h"
#include "string.h"
#include "fsm/generic/include/FSM.h"
#include <buffer.idt>
#include "audiotables.h"

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
    int             testerEosReceived;
    int             wrapperEosReceived;
    int *           input_buffer;
    int *           output_buffer;
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
static int mFull;

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

static t_sample_freq
convertSampleFreq(int freq) {
    int i, result;
    static int AUDIO_CONST_MEM * AudioTables_sampling_freqs_ptr1;

    //convert sampling frequency to enum value
    AudioTables_sampling_freqs_ptr1 = fn_AudioTables_sampling_freqs();

    for( i = ESAA_FREQ_LAST_IN_LIST; i>= 0; i--)
    {
        if(AudioTables_sampling_freqs_ptr1[i] == freq)
            break;
    }

    result = i ;

    return result;
}

static void
configureGraph() {
	char *inputFile, *outputFile;
    int i;
    t_uint16 fileSize, blockSizeIn, blockSizeOut, nbBufIn, nbBufOut;
    t_uint16 freq_in;
    t_uint16 maxchans, config;
    t_pcmprocessing_config pcmprocessing_config;

	fsmInit_t initFsm;
    TraceInfo_t traceInfo;

    get_arguments();
    
    fileSize      = atoi(argv[1]);
    blockSizeIn   = atoi(argv[2]);
    blockSizeOut  = atoi(argv[3]);

    inputFile     = malloc_ext(strlen(argv[4])+1);
    strcpy(inputFile, argv[4]);
    outputFile    = malloc_ext(strlen(argv[5])+1);
    strcpy(outputFile, argv[5]);
    config        = atoi(argv[6]);
    freq_in       = atoi(argv[7]);
    maxchans      = atoi(argv[8]);
    

    memorymgt.initHeap(-2048,0x0,-blockSizeIn-64536,0x0,0x0,0x0);

    tester_config.setParameter(fileSize, blockSizeIn, blockSizeOut, inputFile, outputFile, freq_in, maxchans, config);

    // We shift the args to keep the same parser component between 
    // the streamed and framealigned version
    for (i=0;i<argc-7;i++){
        argv[i] = argv[i+7];
    }
    argc -= 7;

    printf("argc = %d\n",argc);
    
    algoConfParser.configureAlgo(argc,argv);

    pcmprocessing_config.processingMode = config ? PCMPROCESSING_MODE_NOT_INPLACE : PCMPROCESSING_MODE_INPLACE_PUSH;
    pcmprocessing_config.effectConfig.block_size          = blockSizeIn;
    pcmprocessing_config.effectConfig.infmt.freq          = convertSampleFreq(freq_in);
    pcmprocessing_config.effectConfig.infmt.nof_channels  = maxchans;
    pcmprocessing_config.effectConfig.infmt.nof_bits_per_sample  = 24;
    pcmprocessing_config.effectConfig.outfmt.freq          = convertSampleFreq(freq_in);
    pcmprocessing_config.effectConfig.outfmt.nof_channels  = maxchans;
    pcmprocessing_config.effectConfig.outfmt.nof_bits_per_sample  = 24;

    wrapper_config.setParameter(pcmprocessing_config);

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
        printf("PB\n");
            ASSERT(0);
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
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
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

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle 
                    && this->wrapperState == OMX_StateIdle) {
                FSM_TRANSITION(Test2_step0);
            }
            break;

        case TIMER_IT:
            stopTimer();
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            wrapper_command.sendCommand(
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
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StatePause);
            break;

        case FSM_EXIT_SIG:
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
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            break;

        case FSM_EXIT_SIG:
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
// test Wrapper state machine transitions:
// Idle -> Executing -> EOS -> Idle
//---------------------------------------------------------------


static void Test3_step0(Test *this, const FsmEvent *ev) {

    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Idle State Test3_step0\n");
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
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

static void Test3_step2(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ EOS reached : start a new stream! Test3_step2\n");
            tester_config.restartDataFlow();
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle
                    && this->wrapperState == OMX_StateIdle) {
                FSM_TRANSITION(Test_exit);
            }
            break;

        case TIMER_IT:
            stopTimer();
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            wrapper_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            break;
            
        default:
            ASSERT(0);
            break;

    }
}

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
        // Do nothing
    }
    else if (event == OMX_EventBufferFlag) {
        ev.signal  = EOS_EVT;
        // check port index
        ASSERT(data1 == 1);
        // check buffer flags
        ASSERT(data2 & BUFFERFLAG_EOS);
        printf("Wrapper reported EOS\n");
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
        // Do nothing
    }
    else if (event == OMX_EventBufferFlag) {
        ev.signal  = EOS_EVT;
        printf("tester reported EOS\n");
        mTest.testerEosReceived = true;
        FSM_dispatch(&mTest, &ev);
    }
    else {
        ASSERT(0);
    }
}


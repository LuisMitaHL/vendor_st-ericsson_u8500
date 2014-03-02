/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   main.c
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <archi.h>
#include "mixer/nmfil/wrapper/test/main.nmf"
#include "dbc.h"
#include "assert.h"
#include "string.h"
#include "fsm/generic/include/FSM.h"
#include <buffer.idt>
#include <fillthisbuffer.h>
#include "libeffects/mpc/libresamplingapi/resample_api.h"
#include "libeffects/mpc/libvolctrlapi/volctrl_api.h"
#include "libeffects/include/effects_dsp_index.h"
#include "libeffects/include/effects_dsp_types.h"
#include "libeffects/mpc/libmalloc/include/dsp_mem_map.h"
#include <memorybank.idt>

// memory size is in number of words
#define YRAM_SIZE               (((unsigned int)(8 *1024))/sizeof (int))
#define XRAM_START              (U8500_TCM_BASE_ADDRESS + (U8500_TCM_SIZE/2))
#define XRAM_SIZE               (((U8500_TCM_SIZE/2) - YRAM_SIZE)/sizeof (int))
#define YRAM_START              (U8500_TCM_BASE_ADDRESS + U8500_TCM_SIZE - YRAM_SIZE)
#define DDR_24_START            (U8500_DDR_24_BASE_ADDRESS + (U8500_DDR_24_SIZE/2))
#define DDR_24_SIZE             ((unsigned int)(24 * 1024))
#define DDR_16_START            (U8500_DDR_16_BASE_ADDRESS + (U8500_DDR_16_SIZE/2))
#define DDR_16_SIZE             ((unsigned int)(24 * 1024))

#define NB_MAX_MIXER_INPUTS 8
#define ALL_PORTS_TUNNELED 0xFFFF

typedef enum {
    SET_STATE_EVT = FSM_USER_SIG,
    TIMER_IT,
    EOS_EVT
};

typedef struct {
    FSM             fsm;
    OMX_STATETYPE   testerState;
    OMX_STATETYPE   mixerState;
    bool            testerEosReceived;
    bool            mixerEosReceived;
} Test;

static BOOL mIsInputPortPaused[NB_MAX_MIXER_INPUTS];

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
static void Test3_step2_PausedPortsTests(Test *this, const FsmEvent *ev);
static void Test3_step3_PausedPortsTests(Test *this, const FsmEvent *ev);
static void Test3_step4_PausedPortsTests(Test *this, const FsmEvent *ev);

static void Test_exit(Test *this, const FsmEvent *ev);

int argc;
char **argv;
extern int *args_start;

static VolCtrlVolumeRampInfo_t mVolumeRampInfoStruct[NB_MAX_MIXER_INPUTS];
static MixerSamplesPlayed_t mMixerSamplesPlayed;

static t_uint16 mMixerInputPortIdx;
static t_uint16 mTesterOutputPortIdx;

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
    char *inputFile[8], *outputFile;
    t_uint16 nChannels;
    t_uint16 blockSize;
    int nbInputToMix;
    int nbSampleToMix;
    int i;
    MixerParam_t mixerParam;

    t_uint16 isPortSynchronized = ATTR(portsSynchronized);
    fsmInit_t initFsm;
    fsmInit_t initFsm2;
    TraceInfo_t initTrace;
    TraceInfo_t initTrace2;
    t_ExtendedAllocParams  allocParams;

    initTrace.parentHandle = 0;
    initTrace.traceEnable = 0;
    initTrace.dspAddr = 0;

    initTrace2.parentHandle = 0;
    initTrace2.traceEnable = 0;
    initTrace2.dspAddr = 0;

    initFsm.portsDisabled = ATTR(portsDisabled);
    initFsm.portsTunneled = ALL_PORTS_TUNNELED;
    initFsm.traceInfoAddr = &initTrace;

    get_arguments();

    blockSize = atoi(argv[1]);
    nChannels = atoi(argv[2]);

    nbInputToMix = atoi(argv[3]);
    nbSampleToMix = atoi(argv[4]);

    if(nbInputToMix > NB_MAX_MIXER_INPUTS) {
        printf("Bad parameter : max input to mix is 8\n");
    }

    printf("Initialize libmalloc singleton XRAM size=%ld, YRAM size=%ld, DDR 24 size=%d, DDR 16 size=%d\n", XRAM_SIZE, YRAM_SIZE, DDR_24_SIZE, DDR_16_SIZE);
    if (init_intmem(XRAM_START, XRAM_SIZE, YRAM_START, YRAM_SIZE) != MEM_SUCCESS)
    {
        printf("Error while initializing TCM\n");
        ASSERT(0);
    }
    if (init_extmem(DDR_24_START, DDR_24_SIZE, DDR_16_START, DDR_16_SIZE))
    {
        printf("Error while initializing DDR");
        ASSERT(0);
    }

    for(i=0 ; i<nbInputToMix; i++) {
        allocParams.bank = MEM_DDR24;
        allocParams.trace_p = NULL;
        inputFile[i] = vmalloc(strlen(argv[5+i])+1, &allocParams);
        strcpy(inputFile[i], argv[5+i]);
    }

    allocParams.bank = MEM_DDR24;
    allocParams.trace_p = NULL;
    outputFile = vmalloc(strlen(argv[5+i])+1, &allocParams);
    strcpy(outputFile, argv[5+i]);

    tester_config.setParameter(240, 240, nbInputToMix, nbSampleToMix, nChannels, inputFile, outputFile);
    initFsm2.portsDisabled = 0;
    initFsm2.portsTunneled = 0x3;
    initFsm2.traceInfoAddr = &initTrace2;
    tester_fsminit.fsmInit(initFsm2);

    for (i = 0; i < NB_MAX_MIXER_INPUTS; i++) {
        mixerParam.inputPortParam[i].nSamplingRate  = FREQ_44_1KHZ;
        mixerParam.inputPortParam[i].nChannels      = nChannels;
        mixerParam.inputPortParam[i].nBlockSize     = 240;
        mixerParam.inputPortParam[i].bPaused        = false;
        mixerParam.inputPortParam[i].nMemoryBank    = MEM_XTCM;

        if(initFsm.portsDisabled & (1 << i+1)) {
            mixerParam.inputPortParam[i].bDisabled = true;
            printf("input port idx %d is disabled\n", i+1);
        } else {
            mMixerInputPortIdx = i+1;
            mTesterOutputPortIdx = 1;
            mixerParam.inputPortParam[i].bDisabled = false;
            printf("input port idx %d is enabled\n", i+1);
        }
    }

    if(initFsm.portsDisabled & (1 << 0)) {
        mixerParam.outputPortParam.bDisabled = true;
        printf("output port idx 0 is disabled\n");
    } else {
        mixerParam.outputPortParam.bDisabled = false;
        printf("output port idx 0 is enabled\n");
    }
    mixerParam.outputPortParam.nSamplingRate  = FREQ_44_1KHZ;
    mixerParam.outputPortParam.nChannels      = nChannels;
    mixerParam.outputPortParam.nBlockSize     = 240;

    mixer_config.setParameter(mixerParam);

    if(nbInputToMix != 1) {
        mMixerInputPortIdx = nbInputToMix;
        mTesterOutputPortIdx = nbInputToMix;
    }

    mixer_samplesplayed.SetSamplesPlayedAddr(&mMixerSamplesPlayed);

    for (i = 1; i <= NB_MAX_MIXER_INPUTS; i++) {

        if(initFsm.portsDisabled & (1 << i)) continue;

        //Call setEffectGetInfoPtr on enabled input port for Volctrl to retrieve volume ramp informations
        mixer_config.setEffectGetInfoPtr(i, INPUT_VOLCTRL_POSITION, &mVolumeRampInfoStruct[i-1]);
    }

    mixer_fsminit.fsmInit(initFsm);
}


static void
printSrcHeapSizes() {
    t_sample_freq freqIn, freqOut;
    int heapsize;
    int heapsize2;
    int blocksize = 240;

    printf("int MixerNmfMpc::srcHeapSizes[%d][%d]=\n", FREQ_8KHZ-FREQ_48KHZ+1, FREQ_8KHZ-FREQ_48KHZ+1);
    printf("{\n");
    for (freqIn = FREQ_48KHZ ; freqIn <= FREQ_8KHZ; freqIn++) {
        printf("\t{");
        for (freqOut = FREQ_48KHZ ; freqOut <= FREQ_8KHZ; freqOut++) {
            resample_calc_max_heap_size_fixin_fixout(
                    freqIn, freqOut, 0, &heapsize, blocksize, 1, 0);
            printf("%d", heapsize);
            if (freqOut != FREQ_8KHZ) {
                printf(",");
            }
        }
        if (freqIn != FREQ_8KHZ) {
            printf("},\n");
        }
    }
    printf("}\n};\n");


    resample_calc_max_heap_size_fixin_fixout(0,0,6,&heapsize,0,1,0);
    printf("int MixerNmfMpc::srcHeapSizeLowLatency = %d;\n", heapsize);
}

static void Test_configuration(Test *this, const FsmEvent *ev){
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printSrcHeapSizes();
            configureGraph();
            ITREMAP_interf_itmskl = 0;
            ITREMAP_interf_itmskh = 0;
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle
                    && this->mixerState == OMX_StateIdle) {
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
// test mixer state machine transitions:
// Idle -> Executing -> Pause -> Executing -> Idle
//---------------------------------------------------------------


static void Test1_step0(Test *this, const FsmEvent *ev){
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Idle State Test1_step0\n");
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            mixer_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateExecuting 
                    && this->mixerState == OMX_StateExecuting) {
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
            if (this->mixerState == OMX_StatePause) {
                FSM_TRANSITION(Test1_step2);
            }
            break;

        case TIMER_IT:
            stopTimer();
            mixer_command.sendCommand(
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
            printf("+ mixer in Pause State Test1_step2\n");
            mixer_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->mixerState == OMX_StateExecuting) {
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
            printf("+ mixer back in Executing State Test1_step3\n");
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle 
                    && this->mixerState == OMX_StateIdle) {
                FSM_TRANSITION(Test2_step0);
            }
            break;

        case TIMER_IT:
            stopTimer();
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            mixer_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            break;
            
        default:
            ASSERT(0);
            break;

    }
}

//--------------------------------------------------------------
// Test2:
// test mixer state machine transitions:
// Idle -> Pause -> Executing -> Pause -> Idle
//---------------------------------------------------------------

static void Test2_step0(Test *this, const FsmEvent *ev) {

    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Idle State Test2_step0\n");
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            mixer_command.sendCommand(
                    OMX_CommandStateSet, OMX_StatePause);
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateExecuting 
                    && this->mixerState == OMX_StatePause) {
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
            printf("+ Tester in Executing, mixer in Pause Test2_step1\n");
            mixer_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->mixerState == OMX_StateExecuting) {
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
            if (this->mixerState == OMX_StatePause) {
                FSM_TRANSITION(Test2_step3);
            }
            break;

        case TIMER_IT:
            stopTimer();
            mixer_command.sendCommand(
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
            printf("+ mixer in Paused State, Tester in Executing Test2_step3\n");
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            mixer_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle
                    && this->mixerState == OMX_StateIdle) {
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
// test mixer EOS handling:
// Idle -> Executing -> EOS -> newStream -> Idle
//---------------------------------------------------------------

static void Test3_step0(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("+ Components in Idle State Test3_step0\n");
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            mixer_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateExecuting);
            break;

        case FSM_EXIT_SIG:
            break;

        case TIMER_IT:
            stopTimer();
            FSM_TRANSITION(Test3_step1);
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateExecuting 
                    && this->mixerState == OMX_StateExecuting) {
                printf("+ Components in Executing State\n");
                startTimer(0x123456);
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
            printf("Stop Data transmission on Tester Output Port : %d\n", mTesterOutputPortIdx);
            tester_config.stopDataTransmission(mTesterOutputPortIdx);
            //printf("Seb Mixer Input Port : %d as Paused\n", mMixerInputPortIdx);
            //mInputMixerPort[mMixerInputPortIdx - 1].bIsPaused = TRUE;
            //mixer_config.setPausedInputPort(mMixerInputPortIdx, TRUE);
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case EOS_EVT:
        if (this->mixerEosReceived
                && this->testerEosReceived) {
            this->mixerEosReceived = false;
            this->testerEosReceived = false;
            FSM_TRANSITION(Test3_step2);
        }
        break;

        case TIMER_IT:
            stopTimer();
            FSM_TRANSITION(Test3_step2_PausedPortsTests);
            break;

        default:
            ASSERT(0);
            break;

    }
}

// both components in executing
static void Test3_step2_PausedPortsTests(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            //printf("Set Mixer Input Port : %d as UnPaused\n", mMixerInputPortIdx);
            //mInputMixerPort[mMixerInputPortIdx - 1].bIsPaused = FALSE;
            //mixer_config.setPausedInputPort(mMixerInputPortIdx, FALSE);
            //printf("ReStart Data transmission on Tester output Port : %d\n", mTesterOutputPortIdx);
            tester_config.startDataTransmission(mTesterOutputPortIdx);
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case EOS_EVT:
        if (this->mixerEosReceived
                && this->testerEosReceived) {
            this->mixerEosReceived = false;
            this->testerEosReceived = false;
            stopTimer();
            FSM_TRANSITION(Test3_step2);
        }
        break;

        case TIMER_IT:
            stopTimer();
            FSM_TRANSITION(Test3_step3_PausedPortsTests);
            break;

        default:
            ASSERT(0);
            break;

    }
}

// both components in executing
static void Test3_step3_PausedPortsTests(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("Stop Data transmission on Tester output Port : %d\n", mTesterOutputPortIdx);
            tester_config.stopDataTransmission(mTesterOutputPortIdx);
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case EOS_EVT:
        if (this->mixerEosReceived
                && this->testerEosReceived) {
            this->mixerEosReceived = false;
            this->testerEosReceived = false;
            FSM_TRANSITION(Test3_step2);
        }
        break;

        case TIMER_IT:
            stopTimer();
            FSM_TRANSITION(Test3_step4_PausedPortsTests);
            break;

        default:
            ASSERT(0);
            break;

    }
}

// both components in executing
static void Test3_step4_PausedPortsTests(Test *this, const FsmEvent *ev) {
    switch(ev->signal) {
        case FSM_ENTRY_SIG:
            printf("ReStart Data transmission on Tester output Port : %d\n", mTesterOutputPortIdx);
            tester_config.startDataTransmission(mTesterOutputPortIdx);
            break;

        case FSM_EXIT_SIG:
            break;

        case EOS_EVT:
        if (this->mixerEosReceived
                && this->testerEosReceived) {
            this->mixerEosReceived = false;
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
            startTimer(0x123456);
            break;

        case FSM_EXIT_SIG:
            break;

        case SET_STATE_EVT:
            if (this->testerState == OMX_StateIdle
                    && this->mixerState == OMX_StateIdle) {
                FSM_TRANSITION(Test_exit);
            }
            break;

        case TIMER_IT:
            stopTimer();
            tester_command.sendCommand(
                    OMX_CommandStateSet, OMX_StateIdle);
            mixer_command.sendCommand(
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


void METH(start)(void) {

    FSM_init(&mTest, (FSM_State) Test_configuration);
}

void METH(mixer_eventHandler)(
        OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2)
{
    FsmEvent ev;

    if (event == OMX_EventCmdComplete && data1 == OMX_CommandStateSet) {
        ev.signal  = SET_STATE_EVT;
        mTest.mixerState = (OMX_STATETYPE) data2;
        FSM_dispatch(&mTest, &ev);
    } else if (event == OMX_EventBufferFlag) {
        ev.signal  = EOS_EVT;
        // check port index
        if(data1!=0) {
            return;
        }
        // check buffer flags
        ASSERT(data2 & BUFFERFLAG_EOS);
        mTest.mixerEosReceived = true;
        FSM_dispatch(&mTest, &ev);
    } else if (event == OMX_EventCmdReceived) {
        // Do nothing
    } else {
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
    else if (event == OMX_EventBufferFlag) {
        ev.signal  = EOS_EVT;
        mTest.testerEosReceived = true;
        FSM_dispatch(&mTest, &ev);
    } else if (event == OMX_EventCmdReceived) {
        // Do nothing
    } else {
        ASSERT(0);
    }
}

void METH(mixer_newConfigApplied)(t_uint32 configARMAddress) {
    printf("mixer newConfig has been applied\n");
}

void METH(mixer_pcmdump_complete)(PcmDumpCompleteInfo_t sCompleteInfo) {
    printf("mixer pcmdump complete (not used in simulation tests)\n");
}


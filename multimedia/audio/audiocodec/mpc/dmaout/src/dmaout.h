/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmaout.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _DMAOUT_H_
#define  _DMAOUT_H_

#include <audiocodec/mpc/dmaout.nmf>
#include "dma_common.h"


//-------------------------
//  TEST/DEBUG ONLY
//#define DEBUG_TRACE 1
//#define DEBUG_TRACE_BUFFERS
//#define NO_SLEEP
//#define DO_NOT_USE_PROVIDED_BUFFERS
//-------------------------


//--------------------------------
// NUMBER OF CHANNELS / SAMPLES
//--------------------------------
#define NB_SLOTS_IN_ONE_BURST    32
#define BLOCKSIZE_STEREO         (AB_BLOCKSIZE_MONO_1MS * NB_CHANNEL_STEREO)
#define BURST_SIZE_STEREO        (BLOCKSIZE_STEREO * NB_SLOTS_IN_ONE_BURST)
#define NB_SLOTS_IN_RING_BUFFER  (NB_SLOTS_IN_ONE_BURST * NB_CHANNEL_STEREO)

//Ab8500 internal frame counter step is 8 samples
#define FRAMECOUNT_INCREMENT     (AB_BLOCKSIZE_MONO_1MS / 8)

//-------------------------
// PORTS / INDEXES / SLOTS
//-------------------------
#define NB_INPUT_PORTS   6
#define NB_OUTPUT_PORTS  3
// Note that in table first elements are input ports, then output ports
#define TOTAL_NB_PORTS   (NB_INPUT_PORTS+NB_OUTPUT_PORTS)

#define AV_OUTPUT_PORT_IDX      (AVSINK_INPUT_PORT_IDX+NB_INPUT_PORTS)
#define AV_MSPSLOT              0

#define IHF_OUTPUT_PORT_IDX     (IHF_INPUT_PORT_IDX+NB_INPUT_PORTS)
#define IHF_MSPSLOT             2

#define EAR_OUTPUT_PORT_IDX     (EAR_INPUT_PORT_IDX+NB_INPUT_PORTS)
#define EAR_MSPSLOT             0

#define VIBRAL_MSPSLOT          4

#define VIBRAR_MSPSLOT          5

#define FMTX_MSPSLOT            6


//-------------------------
// Port information
//-------------------------
struct DmaOutPortInfo{
	unsigned int mspslot;        // associated msp slot
    unsigned int nb_channel;     // number of channel for this port
    //TODO: check if 2 buffer flag are needed => I think yes !!
    BufferFlag_e flags;          // memorize input port buffer flags to restore them in output port
};
typedef struct DmaOutPortInfo DmaOutPortInfo;

enum LowPowerState{
    NORMAL_MODE,
    BURST_MODE
};
typedef enum LowPowerState LowPowerState;

//--------------------
// Process function
//--------------------
void scheduleProcessFeedbackEvent(void);

void DmaOut_Process_NormalMode_NotYetStarted(void);
void DmaOut_Process_NormalMode_WithFeedback(void);
void DmaOut_Process_NormalMode_DisableItFifo(void);
void DmaOut_Process_PrepareBurstMode(void);
void DmaOut_Process_EnterBurstMode(void);
void DmaOut_Process_BurstMode(void);
void DmaOut_Process_PrepareSleep(void);
void DmaOut_Process_DoNothing(void);
void DmaOut_Process_AwaitSwitchToBurstMode (void);
//---------------------------------------
// Interrupt handler end of DMA transfer
//---------------------------------------
void IT_Handler_Normal_Mode_BackFromBurstMode(void);
void IT_Handler_Normal_Mode_FIFO_Emptied(void);
void IT_Handler_Normal_Mode_First_It(void);
void IT_Handler_Normal_Mode(void);
void IT_Handler_Normal_Last_It(void);
void IT_Handler_Burst_Mode_First_It(void);
void IT_Handler_Burst_Mode(void);
void IT_Handler_BurstMode_Leave(void);

//---------------------------------------
// Interrupt handler Fifo WaterMark
//---------------------------------------
void IT_AB8500_Fifo(void);

//---------------------------
// Buffer handling function
//---------------------------
bool processInputBuffers(bool force_output);
bool processInputBuffersBurst(t_uint16 port_idx);
void copySlotFromRingBufferToJitterBuffer(t_uint16 port_idx);
void fillBurstBufferWithZeros(void);
void processOutputBuffers(void);
void fillRingSlotWithZeros(void);
void zeroJitterBufferStripe( int channels, int mspSlot, int shift_in_bits);
void zeroJitterBufferSlot(t_uint16 idx);

void setup_mute_cb(t_uint16 port_idx, t_uint16 slots);
void done_fading(t_uint16 port_idx);
void start_fading(t_uint16 port_idx, bool fade_up);
//---------------------------
// Dma transfer
//---------------------------
void startDmaIfNeeded(void);
bool stopDmaIfNeeded(int portIdx);

//--------------------
// Usefull function
//--------------------
void fillBufferWithTriangle(int * buff, unsigned int nb_sample, unsigned int nb_channel);

// Implemented in dmaout_sleep.c
void DmaOut_okToSleep(void);
void DmaOut_nokToSleep(void);

//----------------------
// Global Variable
//----------------------
extern DmaOutPortInfo mAccessoryInfo      [NB_INPUT_PORTS];
extern portStatus_t   mMasterPortBuffers  [NB_INPUT_PORTS];
extern portStatus_t   mFeedbackPortBuffers[NB_OUTPUT_PORTS];
extern LowPowerState  actualLowPowerState;

extern t_uint16 mMemorizedPortIdxForIdle;

extern JitterBuffer_t JitterBuffer;

//--------------------------------------------------------------
// Buffer of 2 * BURST_SIZE_STEREO
// either used as a Ping Pong buffer each with size BURST_SIZE_STEREO in burst mode
// or as a ring buffer with NB_SLOTS_IN_RING_BUFFER slots each with size BLOCKSIZE_STEREO in all transition mode)
struct ringBuffer_t {
    int *               buffer;             // Dsp addr
    unsigned long       phy_addr[2];        // Physical addr of ping/pong buffer (in ARM addressable memory space)
    unsigned long       phy_addr_for_shift; // Physical addr of first (ping) burst used to work around AB8500v1 bug on "not first transition to burst"
    t_uint16            readSlotIndex;      // values goes from 0 to 63
    t_uint16            writeSlotIndex;     // values goes from 0 to 63
    volatile t_uint16   filledSlots;        // values goes from 0 to 64
    volatile t_uint16   filledBuffers;      // values goes from 0 to 2
    t_uint16            readIndex;          // values goes from 0 to 1
};
typedef struct ringBuffer_t ringBuffer_t;

extern ringBuffer_t   mRingBuffer;

//--------------------------------------------------------------
// HW related variable
extern t_uint16                mNbMspChannelEnabled;     // Note that this must be useless on DB8500v2. On v2 only 8 channels.
extern t_uint16                mDmaEndOfTranferIT;

extern t_uint48 * mSamplesPlayedPtr;
extern t_uint48 * mTimeStampPtr;
extern t_uint16 mRingbufferXruns;

#endif   // _DMAOUT_H_

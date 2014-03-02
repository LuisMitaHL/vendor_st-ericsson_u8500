/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   wrapper.hpp
* \brief  Pcm splitter NMF_ARM
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _pcm_splitter_nmfil_host_wrapper_hpp_
#define _pcm_splitter_nmfil_host_wrapper_hpp

#include "Component.h"
#include "common_interface.h"
#include "libeffects/libresampling/include/resample.h"

#define NB_OUTPUT 2
//#define NB_INPUT  1

//#define NMF_AUDIO_MAXCHANNELS 16

#define MS_GRANULARITY  5

#define OUT 1
#define IN  0

#define ALL_OUTPUTS         0x6     // bitmask with all output port idxs set
#define SET_BIT(a, i)       ((a) |= (1 << (i)))
#define CLEAR_BIT(a, i)     ((a) &= ~(1 << (i)))
#define IS_BIT_SET(a, i)    ((a) & (1 << (i)))

//#define TRACE_COUNT_BUFFER
//
class pcm_splitter_nmfil_host_wrapper : public Component, public pcm_splitter_nmfil_host_wrapperTemplate
{
    private:

    public:
        pcm_splitter_nmfil_host_wrapper();
        virtual ~pcm_splitter_nmfil_host_wrapper();

        //Component virtual functions
        virtual void process() ;
        virtual void reset() ;
        virtual void disablePortIndication(t_uint32 portIdx) ;
        virtual void enablePortIndication(t_uint32 portIdx) ;
        virtual void flushPortIndication(t_uint32 portIdx) ;

        virtual void fsmInit(fsmInit_t init);
        virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
        virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
        virtual void processEvent(void)  			       { Component::processEvent() ; }

        virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
        virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 idx);

        virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size);

        virtual void setParameter(HostSplitterParam_t splitterParam);
        virtual void setInputPortParameter(HostInputPortParam_t inputPortParam);
        virtual void setOutputPortParameter(t_uint16 idx, HostOutputPortParam_t outputPortParam);

        virtual void eventHandler(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2, t_uint8 idx);

        virtual void setChannelSwitchBoardOutputPort(t_uint16 portIdx, t_uint16 channelSwitchBoard[]);
        virtual void setPausedOutputPort(t_uint16 portIdx, BOOL bIsPaused);
        virtual void setMuteOutputPort(t_uint16 portIdx, BOOL bMute);
        virtual void setBalanceOutputPort(t_uint16 portIdx, t_sint16 nBalance);
        virtual void setVolumeOutputPort(t_uint16 portIdx, t_sint16 nVolume);
        virtual void setVolumeRampOutputPort(t_uint16 portIdx, t_sint16 nStartVolume, 
                t_sint16 nEndVolume, t_uint16 nChannels, 
                t_uint24 nDuration, BOOL bTerminate);

    private:

        //void setSynchronisedOutputPort(t_uint16 portIdx, BOOL bIsSynchronised);
        //void acknowledgeOutputUnderflow(t_uint16 portIdx);
        //void setOutputChannelConfig(t_uint16 portIdx, t_OutChannelConfig OutChannelConfig);
        //void setTrace(t_uint32 addr);

        void doChannelSwitch(int idx, OMX_BUFFERHEADERTYPE_p inbuf, OMX_BUFFERHEADERTYPE_p outbuf);
        void doVolume(int idx, OMX_BUFFERHEADERTYPE_p inbuf, OMX_BUFFERHEADERTYPE_p outbuf);
        void returnOutputBuffer(int idx, OMX_BUFFERHEADERTYPE_p outbuf);
        void releaseInputBuffer(int idx);
        void applyVolume(int idx);
        void applySrcAndVolume(int idx);
        void applyChannelSwitchAndVolume(int idx);
        void applyChannelSwitchAndSrcAndVolume(int idx);       
        void initOutputVolctrl(int idx);
        void *portMalloc(int idx, int size);
        bool isLowLatencySRC(int idx);
        void initOutputSrc(int idx);
        void deinitOutputSrc(int idx);
        void initOutputPortProcessFunction(int idx);
        void initEnabledOutputPort(int idx);
        void initAllEnabledOutputPorts();
        void resetPort(t_uint16 idx);
        void fillBufferWithZeroes(OMX_BUFFERHEADERTYPE_p buf);
        void resample_process_low_latency(ResampleContext *resampleContext, OMX_BUFFERHEADERTYPE_p inputBuf, OMX_BUFFERHEADERTYPE_p outputBuf,int channels, bool *needInputBuf, bool *filledOutputBuf);
        void resample_process_std(ResampleContext *resampleContext, OMX_BUFFERHEADERTYPE_p inputBuf, OMX_BUFFERHEADERTYPE_p outputBuf, int channels, bool *needInputBuf, bool *filledOutputBuf);
        void resample_process(ResampleContext *resampleContext, OMX_BUFFERHEADERTYPE_p inputBuf, OMX_BUFFERHEADERTYPE_p outputBuf, int channels, bool *needInputBuf, bool *filledOutputBuf);

        inline bool isPaused(int idx) { return IS_BIT_SET(mPausedOutputs, idx); }
        inline bool isDisabled(int idx) { return IS_BIT_SET(mDisabledPorts, idx); }

        inline bool 
            allOutputsPausedOrDisabled() { return (((mDisabledPorts | mPausedOutputs) & ALL_OUTPUTS) == ALL_OUTPUTS); }


        inline bool 
            isChannelSwitchingPort(int idx) {  
                unsigned int i;

                if (mOutputChannels[idx-OUT] != mInputChannels) {
                    return true;
                }

                for (i = 0; i < mOutputChannels[idx-OUT]; i++) {
                    if (mOutputChannelSwitchBoard[idx-OUT][i] != i) {
                        return true;
                    }
                }

                return false;
            } 

    private :
        OMX_BUFFERHEADERTYPE*       mFifoOut[NB_OUTPUT][1];
        OMX_BUFFERHEADERTYPE*       mFifoIn[1];
        Port         mPorts[NB_OUTPUT+1];
#ifdef TRACE_COUNT_BUFFER
        t_uint32 nbBufferReceived[3];
#endif

        OMX_BUFFERHEADERTYPE_p             mSplitterOutputBuffer[NB_OUTPUT];
        OMX_BUFFERHEADERTYPE             mSplitterOutputBufferHeader[NB_OUTPUT];
        OMX_BUFFERHEADERTYPE_p             mSrcInputBuffer[NB_OUTPUT];
        ResampleContext      mResampleContext[NB_OUTPUT];


        t_uint16             mInputChannels;
        t_sample_freq        mInputFreq;
        t_uint16             mInputBlockSize;        
        t_uint16             mInputBytesPerSample;

        t_uint16             mOutputChannels[NB_OUTPUT];
        t_sample_freq        mOutputFreq[NB_OUTPUT];
        t_uint16             mOutputBlockSize[NB_OUTPUT];
        t_channel_type       mChannelTypeOut[NB_OUTPUT];
        t_uint32             mOutputChannelSwitchBoard[NB_OUTPUT][NMF_AUDIO_MAXCHANNELS];
        void                 (pcm_splitter_nmfil_host_wrapper::*mProcessOutputPort[NB_OUTPUT])(int idx); 

        void *                mHeap[NB_OUTPUT];
        int                  mHeapSize[NB_OUTPUT];

        int                  mInputBufferRefCount;

        int                  mDisabledPorts;
        int                  mPausedOutputs;
        OMX_BOOL             mFsmInitDone;


};

#endif // _pcm_splitter_nmfil_host_wrapper_hpp_



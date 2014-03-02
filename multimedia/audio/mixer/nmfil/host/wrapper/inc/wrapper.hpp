/*****************************************************************************/
/**
 *  (c) ST-Ericsson, 2009 - All rights reserved
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson
 *
 * \file   wrapper_mixer.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _mixer_wrapper_hpp_
#define _mixer_wrapper_hpp_
#include "Component.h"
#include "common_interface.h"
#include "libeffects/libmixer/include/mixer.h"
#include "libeffects/libresampling/include/resample.h"

#define NB_INPUT  8
#define NB_OUTPUT 1

#define MS_GRANULARITY  5
#define LOW_LATENCY     6
#define FIRST_BUFFER    0
#define MIXER_MONO      1
#define MIXER_STEREO    (MIXER_MONO*2)

#define ALL_INPUTS          0xFF   // bitmask with all input port idxs set
#define SET_BIT(a, i)       ((a) |= (1 << (i)))
#define CLEAR_BIT(a, i)     ((a) &= ~(1 << (i)))
#define IS_BIT_SET(a, i)    ((a) & (1 << (i)))

class mixer_nmfil_host_wrapper : public Component, public mixer_nmfil_host_wrapperTemplate
{
public:
    mixer_nmfil_host_wrapper();
    virtual ~mixer_nmfil_host_wrapper();

    // Component virtual methods...
    virtual void reset();
    virtual void process();
    virtual void disablePortIndication(t_uint32 portIdx);
    virtual void enablePortIndication(t_uint32 portIdx);
    virtual void flushPortIndication(t_uint32 portIdx);

    virtual void fsmInit(fsmInit_t initFsm);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param);
    virtual void processEvent(void)  		               { Component::processEvent() ; }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer, t_uint8 idx);
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

    virtual void eventHandler(OMX_EVENTTYPE event, t_uint32 data1, t_uint32 data2, t_uint8 idx);
    
    // Methods provided by interface mixer.nmfil.host.wrapper.configure
    virtual void setParameter(HostMixerParam_t mixerParam);
    virtual void setInputPortParameter(t_uint16 idx, HostInputPortParam_t inputPortParam);
    virtual void setOutputPortParameter(HostOutputPortParam_t outputPortParam);
    virtual void setPausedInputPort(t_uint16 portIdx, BOOL bIsPaused);
    virtual void setMuteInputPort(t_uint16 portIdx, BOOL bIsPaused);
    virtual void setBalanceInputPort(t_uint16 portIdx, t_sint16 nBalance);
    virtual void setVolumeInputPort(t_uint16 portIdx, t_sint16 nVolume);
    virtual void setVolumeRampInputPort(t_uint16 portIdx, t_sint16 nStartVolume, t_sint16 nEndVolume, t_uint16 nChannels, t_uint24 nDuration, BOOL bTerminate);

    virtual void start(void);
    virtual void stop(void);
    // Methods provided by interface afm.nmf.host.common.pcmsettings:
    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size, t_uint8 idxs); 
    
    // Methods defined locally
    void fillBufferWithZeroes(OMX_BUFFERHEADERTYPE_p  buf);
    void sampleRateConversion(int idx, OMX_BUFFERHEADERTYPE_p inputBuf, OMX_BUFFERHEADERTYPE_p outputBuf, int blocksize, bool *needInputBuf, bool *filledOutputBuf ,int nbchannel);
    void applyVolumeAndSrc(int idx);
    void applySrcAndVolume(int idx);
    bool hasBufferToMix(int idx, int nbInputWithData);
    void do_mixing(t_uint16 nbInput);
    void ReleaseInput(t_uint16 idx);
    void releaseInputBuffer(t_uint16 idx);
    void releaseOutputBuffer();
    void initInputVolctrl(int idx);
    void *portMalloc(int idx, int size);
    bool isLowLatencySRC(int idx);
    void initInputSrc(int idx);
    void initEnabledInputPort(int idx);
    void initEnabledInputPorts();
    void resetPort(t_uint32 idx);
    bool checkConfig(HostMixerParam_t param);
    void mixerDefaultSettings();
    

    bool isPausedInput(int idx) { return IS_BIT_SET(mPausedInputs, idx-INPUT_PORT_IDX); }
    bool isEosInput(int idx) { return IS_BIT_SET(mEosInputs, idx-INPUT_PORT_IDX); }
    bool isDisabled(int idx) { return IS_BIT_SET(mDisabledPorts, idx); }

private:
    typedef enum { OUTPUT_PORT_IDX,INPUT_PORT_IDX} portname;
    typedef enum { NOMIX,UPMIX,DOWNMIX} mixing;
    

    Port                 mPorts[NB_INPUT+NB_OUTPUT];
   
    MIXER_LOCAL_STRUCT_T mAlgoStruct;

    
    MMshort              *mIn_list[NB_INPUT];
    OMX_BUFFERHEADERTYPE_p  mMixerInputBuffer[NB_INPUT];
    OMX_BUFFERHEADERTYPE_p  mSrcInputBuffer[NB_INPUT];
    OMX_BUFFERHEADERTYPE    mSrcOutputBufferHeader[NB_INPUT];
    OMX_BUFFERHEADERTYPE_p  mSrcOutputBuffer[NB_INPUT];
    
    OMX_BUFFERHEADERTYPE    mVolCtrlOutputBuf[NB_INPUT];
    OMX_BUFFERHEADERTYPE_p 	mOutputBufPtr;
    SRC_QUALITY_t           mSrcMode[NB_INPUT];
    
    ResampleContext      mResampleContext[NB_INPUT];


    t_sample_freq        mInputFreq[NB_INPUT];
    t_uint16             mInputChannels[NB_INPUT];
    t_channel_type       mChannelTypeIn[NB_INPUT][MAXCHANNELS_SUPPORT];
    void *               mHeap[NB_INPUT];
    t_uint16             mOutputBlockSize;        
    t_uint16		     mOutputChannels;
    t_uint16		     mBitsPerSample;
    t_uint16		     mBytePerSample;
    t_sample_freq        mOutputFreq;
    t_channel_type       mChannelTypeOut[MAXCHANNELS_SUPPORT];

    int                  mEosInputs; 
    int                  mDisabledPorts;
    int                  mPausedInputs;
    mixing               mUpOrDownmix[NB_INPUT];
    
};

#endif // _mixer_wrapper_hpp_

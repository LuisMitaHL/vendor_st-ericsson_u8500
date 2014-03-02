/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AudiocodecBaseNmfMpc.h
 * \brief  Bases classes that ease Nmf Mpc proxies implementation
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _AudiocodecBaseNmfMpc_H_
#define _AudiocodecBaseNmfMpc_H_

#include "AFMNmfMpc_ProcessingComp.h"
#include "NmfMpcVolCtrl.h"
#include "AudiocodecBase.h"
#include "alsactrl_audiocodec.h"

class NmfMpcBase;
class NmfMpcInAb8500;
class NmfMpcOutAb8500;

//extern "C" {

    //! Callbacks collection to interact with Audio HW  control chipset Apis type
//    typedef struct audioChipsetApiInterface audioChipsetApiInterface;
//}


//-----------------------------------------------------------------------------
//! \class AudiocodecBaseNmfMpc
//! \brief Base abstract class used by all OMX renderer and capturer (Ab8500 and BT)
//!
//! Proxy that manage all Nmf Mpc components.
//!
//! \note mNmfMain is binded to AFM controller
//-----------------------------------------------------------------------------
class AudiocodecBaseNmfMpc: public AFMNmfMpc_ProcessingComp {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        //! \param portIndex of Nmf Mpc components used for this source/sink
        AudiocodecBaseNmfMpc(AudiocodecBase &audiocodec, audio_hal_channel channel, OMX_U32 portIndex = 0);

        //! constructs this class, register stub/skel
        virtual OMX_ERRORTYPE construct(void);
        //! destroys this class, unregister stub/skel
        virtual OMX_ERRORTYPE destroy(void);		
        
        //! returns Nmf Mpc main port
        inline OMX_U32 getInOutNmfPortIdx(void){ return mDmaPortIndex; }

        //! creates unique instance of "dma*"
        virtual NmfMpcBase * getNmfMpcDmaHandle(const AudiocodecBase & audiocodec) = 0;
        //! frees unique instance of "dma*"
        virtual void         freeDmaHandle(void) = 0;

    protected:
        //! instantiates Nmf Mpc components 
        virtual OMX_ERRORTYPE instantiateMain(void);
        //! configures Nmf Mpc components 
        virtual OMX_ERRORTYPE configureMain(void);
        //! starts Nmf Mpc components 
        virtual OMX_ERRORTYPE startMain(void);
        //! stops Nmf Main Mpc components 
        virtual OMX_ERRORTYPE stopMain(void);
        //! destroys Nmf Mpc components 
        virtual OMX_ERRORTYPE deInstantiateMain(void);
      
        //! sets power, volume, mute... configuration
        virtual OMX_ERRORTYPE applyConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);

        //! retrieves power, volume, mute... configuration
        virtual OMX_ERRORTYPE retrieveConfig(
                OMX_INDEXTYPE nConfigIndex,  
                OMX_PTR pComponentConfigStructure);        
        
        //! Starts/stops dma transfer on Executing/Idle state change
        virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
       
        //! returns Nmf Mpc handle connected to a specified OMX port 
        virtual t_cm_instance_handle getNmfHandle(OMX_U32 portIdx) const;

        //! returns str concataned with a suffix "[nmfportnb]" if needed
        //! \note As BT have only one port str is directly returned
        // TODO remove virtual if BT aligned
        virtual ENS_String<32> addSuffixPerPortNb(const char * str);

        //! returns start time in us
        inline OMX_S64 getStartTimeInUs(void){ return mStartTimeinUs; }
      
        //! intatiates a volctrl
        OMX_ERRORTYPE instantiateVolCtrl(NmfMpcVolCtrl * & new_instance, OMX_DIRTYPE direction, OMX_U32 omxPortIndex);
        //! destroys a volctrl
        OMX_ERRORTYPE destroyVolCtrl(NmfMpcVolCtrl * & volctrl, OMX_DIRTYPE direction);
        //! configures a volctrl
        OMX_ERRORTYPE configureVolCtrl(NmfMpcVolCtrl * volctrl, OMX_U32 omxPortIdx);
        
        //! instantiates the controller
        OMX_ERRORTYPE instanciateController(void);
        //! destroys the controller
        OMX_ERRORTYPE destroyController(void);

        //! handles status returned by a call to audio_hw_ctrl layer
        void handleStatusFromAudiohwCtrl(AUDIO_HAL_STATUS status);

        //! notifiies that this component supports pcm probbing
        virtual bool supportsPcmProbe() const { return true; };

        //! pointer of volctrl connected to OMX port 0
        NmfMpcVolCtrl *         mVolCtrl;

        //! stores start transfer time in us
        //! \note must be always 0 for renderer as we count "buffer comsumption time" for renderer and "real time" for capturer
        OMX_S64                 mStartTimeinUs;

        //! stores OMX component reference
        AudiocodecBase &  mAudioCodecBase;
        //! stores Dma Nmf Mpc port osed for this OMX component
        OMX_U32                 mDmaPortIndex;
        //! Pointer on Dma Nmf Mpc singleton 
        NmfMpcBase       *      mDmaBase;

        //! static table that converts port number in string suffixe
        static const char* SuffixPerIndex[];

        //! handles errors from audio_chipset_api
        virtual void statusCB(AUDIO_HAL_STATUS status);

        //! The index that identifies what type of sink/source this is
        audio_hal_channel channel;

        //! actual samplerate used in port since it might differ from default value in audiocodec (BT NB/WB case)
        OMX_U32                 mSamplerate;

        //! actual blocksize to be used depending on port setting (as with mSamplerate)
        OMX_U32                 mBlockSize;

    private:
        //! returns sample frequency
        t_sample_freq getFreq(void);

        //! registers Stub / skel for Mpc host interface call
        void registerStubsAndSkels(void);
        //! unregisters Stub / skel for Mpc host interface call
        void unregisterStubsAndSkels(void);

        //! indicates if Dma tranfer has started
        bool                    mIsStarted;
};

//-----------------------------------------------------------------------------
//! \class AudioRendererNmfMpc
//! \brief Base abstract class used by all OMX renderer (Ab8500 and BT)
//!
//-----------------------------------------------------------------------------
class AudioRendererNmfMpc: public AudiocodecBaseNmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        //! \param portIndex of Nmf Mpc components used for this source/sink
        //! \param hasFeedbackPort indicates if this OMX component has a reference port
        AudioRendererNmfMpc(AudiocodecBase &audiocodec, audio_hal_channel channel, OMX_U32 portIndex = 0, OMX_BOOL hasFeedbackPort=OMX_FALSE) 
        : AudiocodecBaseNmfMpc(audiocodec, channel, portIndex), mHasFeedbackPort(hasFeedbackPort) { }
        
        //! computes OMX_IndexConfigAudioProcessedDataAmount
        virtual	OMX_ERRORTYPE retrieveConfig(
                    OMX_INDEXTYPE nConfigIndex,
                    OMX_PTR pComponentConfigStructure);
        
    protected:
        //! instantiates Nmf Mpc components 
        virtual OMX_ERRORTYPE instantiateMain(void);
        //! configures Nmf Mpc components 
        virtual OMX_ERRORTYPE configureMain(void);
        //! starts Nmf Mpc components 
        virtual OMX_ERRORTYPE startMain(void);
        //! stops Nmf Main Mpc components 
        virtual OMX_ERRORTYPE stopMain(void);
        //! destroys Nmf Mpc components 
        virtual OMX_ERRORTYPE deInstantiateMain(void);
        //! returns Nmf Mpc handle connected to a specified OMX port 
        virtual t_cm_instance_handle getNmfHandle(OMX_U32 portIdx) const;
        //! returns nmf internal component port id connected to OMX port
        virtual OMX_U32       getNmfPortIndex(OMX_U32 omxPortIdx) const { return omxPortIdx; };

    private:
        //! pointer on volume control connected to OMX reference port (1) 
        //! \note not all sinks have reference port
        NmfMpcVolCtrl *         mFeedbackVolCtrl;
        //! indicates if OMX compoent has a reference port 
        OMX_BOOL                mHasFeedbackPort; //TODO get information from AudioRenderer class, remove this info
};

//-----------------------------------------------------------------------------
//! \class AudioCapturerNmfMpc
//! \brief Base abstract class used by all OMX capturer (Ab8500 and BT)
//!
//-----------------------------------------------------------------------------
class AudioCapturerNmfMpc: public AudiocodecBaseNmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        //! \param portIndex of Nmf Mpc components used for this source/sink
        AudioCapturerNmfMpc(AudiocodecBase &audiocodec, audio_hal_channel channel, OMX_U32 portIndex = 0) 
            : AudiocodecBaseNmfMpc(audiocodec, channel, portIndex)  {}

        //! computes OMX_IndexConfigAudioProcessedDataAmount
        virtual	OMX_ERRORTYPE retrieveConfig(
                    OMX_INDEXTYPE nConfigIndex,
                    OMX_PTR pComponentConfigStructure);
        //! returns nmf internal component port id connected to OMX port
        virtual OMX_U32      getNmfPortIndex(OMX_U32 omxPortIdx) const;


};


//-----------------------------------------------------------------------------
//! \class AudioRendererAb8500NmfMpc
//! \brief Base class used by Ab8500 OMX renderer
//!
//-----------------------------------------------------------------------------
class AudioRendererAb8500NmfMpc: public AudioRendererNmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        //! \param portIndex of Nmf Mpc components used for this source/sink
        //! \param hasFeedbackPort indicates if this OMX component has a reference port
        AudioRendererAb8500NmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel, OMX_U32 portIndex = 0, OMX_BOOL hasFeedbackPort=OMX_FALSE) 
        : AudioRendererNmfMpc(audiocodec, channel, portIndex, hasFeedbackPort) {
            mPortIndex = portIndex;
            mDmaOut = static_cast<NmfMpcOutAb8500 *>(NULL);
        }
       
        //! provides AFM_IndexLowPowerRenderer that enables to switch from/to normal and burst mode
        virtual	OMX_ERRORTYPE applyConfig(
                    OMX_INDEXTYPE nConfigIndex,
                    OMX_PTR pComponentConfigStructure);

        virtual OMX_ERRORTYPE retrieveConfig(
                    OMX_INDEXTYPE nConfigIndex,
                    OMX_PTR pComponentConfigStructure);
        
        //! creates unique instance of "dmaout"
        virtual NmfMpcBase * getNmfMpcDmaHandle(const AudiocodecBase & audiocodec);
        //! frees unique instance of "dmaout"
        virtual void         freeDmaHandle(void);

        void invoke_mute(OMX_BOOL muted);

        NmfMpcOutAb8500 * getNmfMpcOutAb8500(void) { return mDmaOut; }

    private:
        //! Pointer on singleton used to handle Ab8500 Tx transfer
        NmfMpcOutAb8500 *              mDmaOut;
        OMX_U32                        mPortIndex;
};

//-----------------------------------------------------------------------------
//! \class AudioCapturerAb8500NmfMpc
//! \brief Base class used by Ab8500 OMX capturer
//!
//-----------------------------------------------------------------------------
class AudioCapturerAb8500NmfMpc: public AudioCapturerNmfMpc {
    public:
        //! \param audiocodec reference associated OMX component
        //! \param audioApi collection of callbacks for audio chipset apis
        //! \param portIndex of Nmf Mpc components used for this source/sink
        AudioCapturerAb8500NmfMpc(AudiocodecBase & audiocodec, audio_hal_channel channel, OMX_U32 portIndex = 0) 
            : AudioCapturerNmfMpc(audiocodec, channel, portIndex)  {}

        //! creates unique instance of "dmain"
        virtual NmfMpcBase * getNmfMpcDmaHandle(const AudiocodecBase & audiocodec);
        //! frees unique instance of "dmain"
        virtual void         freeDmaHandle(void);
    private:
        //! Pointer on singleton used to handle Ab8500 Rx transfer
        NmfMpcInAb8500 *               mDmaIn;
};

#endif // _AudiocodecBaseNmfMpc_H_

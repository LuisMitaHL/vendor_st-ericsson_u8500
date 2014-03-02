/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapper.hpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _pcmprocessings_wrapper_hpp_
#define _pcmprocessings_wrapper_hpp_

#include "Component.h"
#include "common_interface.h"

class hst_pcmprocessings_wrapper : public Component, public hst_pcmprocessings_wrapperTemplate
{
public:
    hst_pcmprocessings_wrapper();
    virtual ~hst_pcmprocessings_wrapper();

    // NMF life-cycle methods...
    virtual t_nmf_error construct(void);
    virtual void destroy(void);
    virtual void start(void);
    virtual void stop(void);

    // Component virtual methods...
    virtual void reset();
    virtual void process();
    virtual void disablePortIndication(t_uint32 portIdx);
    virtual void enablePortIndication(t_uint32 portIdx);
    virtual void flushPortIndication(t_uint32 portIdx);

    virtual void fsmInit(fsmInit_t initFsm);
    virtual void setTunnelStatus(t_sint16 portIdx, t_uint16 isTunneled);
    virtual void sendCommand(OMX_COMMANDTYPE cmd, t_uword param) { Component::sendCommand(cmd, param) ; }
    virtual void processEvent(void)  		               { Component::processEvent() ; }
    virtual void emptyThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);
    virtual void fillThisBuffer(OMX_BUFFERHEADERTYPE_p buffer);

    // Methods provided by interface afm.nmf.host.pcmprocessings.wrapper.configure:
    virtual void setParameter(t_host_pcmprocessing_config config);

    // Methods provided by interface afm.nmf.host.common.pcmsettings:
    virtual void newFormat(t_sample_freq sample_freq, t_uint16 chans_nb, t_uint16 sample_size); 

private:
    typedef enum {INPUT_PORT_IDX, OUTPUT_PORT_IDX} portname;

    void run_inplace();
    void run_not_inplace();
    OMX_S64 computeTimeStamp(t_uint16 nbNewSampleSize);
    void initTimeStampComputation(OMX_BUFFERHEADERTYPE * InputBuf, OMX_BUFFERHEADERTYPE * OutputBuf);

    Port                        mPorts[2];
    t_host_pcmprocessing_mode   mProcessingMode;
    t_host_effect_config        mEffectConfig;
    t_effect_caps               m_effect_caps;
    
    OMX_S64      mInitialTS;  
    OMX_S64      mNbSamplesForTSComputationOutput;
    bool         bPropagateTS;
};

#endif // _pcmprocessings_wrapper_hpp_

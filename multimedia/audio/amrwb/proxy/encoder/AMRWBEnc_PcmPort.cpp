/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AMRWBEnc_PcmPort.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include "AMRWBEnc_PcmPort.h"

OMX_ERRORTYPE   AMRWBEnc_PcmPort::checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const {
    if (pcmSettings.nChannels != 1) return OMX_ErrorUnsupportedSetting;
    if (pcmSettings.nSamplingRate != 16000) return OMX_ErrorUnsupportedSetting;
    return OMX_ErrorNone;
};


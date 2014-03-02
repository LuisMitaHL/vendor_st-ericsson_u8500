/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _STE_ENS_AUDIO_COMPONENTS_H_
#define _STE_ENS_AUDIO_COMPONENTS_H_

#include <ENS_Component.h>

OMX_ERRORTYPE mp3decFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE pcmhostdecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE pcmhostencFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE amrhostdecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE amrhostencFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE amrwbhostdecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE amrwbhostencFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE cscallFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE cscall_mpcFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE gsmhrdecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE gsmhrencFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE gsmfrdecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE gsmfrencFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE aacdecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE aachostencFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE mixerFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE mixerhostFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE nokiaAVSinkFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE nokiaAvSourceFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE multiMicFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE audiovirtualizerFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE transducer_equalizerFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE transducer_equalizer_host_FactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE earpieceFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE intHandsFreeFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE vibraFactoryMethod(ENS_Component_p * ppENSComponent,OMX_BOOL channel) __attribute__((weak));

OMX_ERRORTYPE rendererItFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE capturerItFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE FmTxFactoryMethod(ENS_Component_p *ppENSComponent)  __attribute__((weak));

OMX_ERRORTYPE FmRxFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE BtTxFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE BtRxFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE splitterFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE g711hostencFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE g711hostdecFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE mdrcFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE mdrc_host_FactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE pcmsplitterFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE drcPcmProcFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE channelsynthesisPcmProcFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE virtualsurroundPcmProcFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE audiovisualizationPcmProcFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE speechprocFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE wma_v9decFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE wmapro_v10decFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE geqFactoryMethod(ENS_Component_p *ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE flachostdecFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE ddplushostdecFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE audio3dmixerFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE tonegeneratorFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE alsasinkFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE alsasourceFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE reverbFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE mp3hostdecFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE splitterhostFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE spectrumAnalyzerPcmProcFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE timescalePcmProcFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE splFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));

OMX_ERRORTYPE g729hostencFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE g729hostdecFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE dtshostdecFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE aachostdecFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
//OMX_ERRORTYPE hdmiSinkFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));
OMX_ERRORTYPE chorusPcmProcFactoryMethod(ENS_Component_p * ppENSComponent) __attribute__((weak));


#endif /* _STE_ENS_AUDIO_COMPONENTS_H_ */



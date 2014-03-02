/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "ste_omx_ens_component_loader.h"
#include "avsink/AvSink.h"
#include "avsource/AvSource.h"
#include "multimic/MultiMic.h"
#include "earpiece/Earpiece.h"
#include "ihf/IntHandsFree.h"
#include "fmtx/FmTx.h"
#include "fmrx/FmRx.h"
#include "bttx/BtTx.h"
#include "btrx/BtRx.h"
#include "vibra/Vibra.h"

#define ENABLE_VIBRA 1

/** Definition of all the components this module will register to the STE ENS loader*/
static struct ste_omx_ens_component_def_array components_to_register = 
    { {
	    {{"OMX.ST.AFM.NOKIA_AV_SINK"},   {{"audio_renderer.pcm"}, {0}}, AvSinkFactoryMethod},
	    {{"OMX.ST.AFM.NOKIA_AV_SOURCE"}, {{"audio_capturer.pcm"}, {0}}, AvSourceFactoryMethod},
	    {{"OMX.ST.AFM.MULTI_MIC"},       {{"audio_capturer.pcm"}, {0}}, multiMicFactoryMethod},
	    {{"OMX.ST.AFM.EARPIECE"},        {{"audio_renderer.pcm"}, {0}}, earpieceFactoryMethod},
	    {{"OMX.ST.AFM.IHF"},             {{"audio_renderer.pcm"}, {0}}, intHandsFreeFactoryMethod},
	    {{"OMX.ST.AFM.FM_SINK"},         {{"audio_renderer.pcm"}, {0}}, FmTxFactoryMethod},
	    {{"OMX.ST.AFM.FM_SOURCE"},       {{"audio_capturer.pcm"}, {0}}, FmRxFactoryMethod},
	    {{"OMX.ST.AFM.BT_MONO_SINK"},    {{"audio_renderer.pcm"}, {0}}, BtTxFactoryMethod},
	    {{"OMX.ST.AFM.BT_MONO_SOURCE"},  {{"audio_capturer.pcm"}, {0}}, BtRxFactoryMethod},
#if ENABLE_VIBRA
	    {{"OMX.ST.HAPTICS_RENDERER.PCM.VIBL"}, {{"audio_renderer.pcm"}, {0}}, vibra1FactoryMethod},
	    {{"OMX.ST.HAPTICS_RENDERER.PCM.VIBR"}, {{"audio_renderer.pcm"}, {0}}, vibra2FactoryMethod},
#endif	// #if (defined ( ENABLE_VIBRA ))
	    {{0}, {{0}}, 0}
	} };

extern "C"
{
/** The function the OMX ENS loader is calling to register the components of this module*/
    void ste_omx_ens_component_register(struct ste_omx_ens_component_def_array **refs)
    {
	*refs = &components_to_register;
    }
}

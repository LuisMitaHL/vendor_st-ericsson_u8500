/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include "OMX_Audio.h"
#include "AFM_Types.h"
#include "AFM_Index.h"
#include "omx_ste_speech_proc.h"
#include "audio_render_chipset_api.h"
#include "audio_sourcesink_chipset_api.h"
#include "OMX_CoreExt.h"

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
    #include "audio_transducer_chipset_api.h"
    #include "OMX_STE_VirtualSurroundExt.h"
    #include "omx_ste_noise_reduction.h"
    #include "OMX_DRCExt.h"

    #ifdef STE_PLATFORM_U8500
        #include "OMX_CNGExt.h"
    #endif
#endif

static const char* cur_table;

// The reason sizeof( ((struct container*)0xF000)->name) is used instead
// of sizeof(container::name) is that gcc 4.3.3 has a problem with the later
// construct. It works fine in later versions of GCC.

#define ENTRY_INT(container,name) \
    printf("INSERT INTO StructDef VALUES('%s', '%s', 'int', %d, %d);\n", \
        cur_table, \
        #name, \
        8 * sizeof( ((struct container*)0xF000)->name), \
        8 * offsetof(container, name));

#define ENTRY_INT_ARRAY(container,name,index) \
    printf("INSERT INTO StructDef VALUES('%s', '%s', 'int', %d, %d);\n", \
        cur_table, \
        #name "_" #index, \
        8 * sizeof( ((struct container*) 0xF000)->name[0]), \
        8 * offsetof(container, name) + index*8*sizeof( ((struct container*)0xF000)->name[0]));

#define ENTRY_INT_STRUCT_ARRAY(container,Struct,member,index) \
    printf("INSERT INTO StructDef VALUES('%s', '%s', 'int', %d, %d);\n", \
        cur_table, \
        #Struct "_" #index "." #member, \
        8 * sizeof( ((struct container*) 0xF000)->Struct[0].member), \
        8 * offsetof(container, Struct[0].member) + index*8*sizeof( ((struct container*)0xF000)->Struct[0]));


#define ENTRY_INT_ARRAY_LOOP(container,name,size) \
    for (int index = 0; index < size; index++) \
        printf("INSERT INTO StructDef VALUES('%s', '%s_%d', 'int', %d, %d);\n", \
            cur_table, \
            #name, \
            index, \
            8 * sizeof( ((struct container*)0xF000)->name[0]), \
            8 * offsetof(container, name) + index*8*sizeof( ((struct container*)0xF000)->name[0]));

#define ENTRY_INT_STRUCT_ARRAY_LOOP(container,Struct,member,size) \
    for (int index = 0; index < size; index++) \
        printf("INSERT INTO StructDef VALUES('%s', '%s_%d.%s', 'int', %d, %d);\n", \
            cur_table, \
            #Struct, \
            index, \
            #member, \
            8 * sizeof( ((struct container*)0xF000)->Struct[0].member), \
            8 * offsetof(container, Struct[0].member) + index*8*sizeof(((struct container*) 0xF000)->Struct[0]));

#define ENTRY_INT_MATRIX(container,name,index) \
    printf("INSERT INTO StructDef VALUES('%s', '%s', 'int', %d, %d);\n", \
        cur_table, \
        #name "_" #index, \
        8 * sizeof(((struct container*)0xF000)->name[0][0]), \
        8 * offsetof(container, name) + index*8*sizeof(((struct container*)0xF000)->name[0][0]));

#define ENTRY_TEXT(container,name) \
    printf("INSERT INTO StructDef VALUES('%s', '%s', 'text', %d, %d);\n", \
        cur_table, \
        #name, \
        8 * sizeof(((struct container*)0xF000)->name), \
        8 * offsetof(container, name));

#define BEGIN_NEW_ENTRY(table) \
    cur_table = table; \
    printf("\n\n; %s\n", cur_table);


#include <stddef.h>
#include <stdio.h>


// INSERT INTO StructDef VALUES ('std_input_config', 'Strength', 'int', 32, 128);


void GenConfigEntries_PcmModeType()
{
    BEGIN_NEW_ENTRY("PCMMODETYPE");
        ENTRY_INT(OMX_AUDIO_PARAM_PCMMODETYPE, nPortIndex);
        ENTRY_INT(OMX_AUDIO_PARAM_PCMMODETYPE, nChannels);
        ENTRY_INT(OMX_AUDIO_PARAM_PCMMODETYPE, eNumData);
        ENTRY_INT(OMX_AUDIO_PARAM_PCMMODETYPE, eEndian);
        ENTRY_INT(OMX_AUDIO_PARAM_PCMMODETYPE, bInterleaved);
        ENTRY_INT(OMX_AUDIO_PARAM_PCMMODETYPE, nBitPerSample);
        ENTRY_INT(OMX_AUDIO_PARAM_PCMMODETYPE, nSamplingRate);
        ENTRY_INT(OMX_AUDIO_PARAM_PCMMODETYPE, ePCMMode);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 0);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 1);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 2);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 3);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 4);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 5);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 6);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 7);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 8);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 9);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 10);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 11);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 12);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 13);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 14);
        ENTRY_INT_ARRAY(OMX_AUDIO_PARAM_PCMMODETYPE, eChannelMapping, 15);
}

void GenConfigEntries_VOLUMETYPE()
{
    BEGIN_NEW_ENTRY("VOLUMETYPE");
        ENTRY_INT(OMX_AUDIO_CONFIG_VOLUMETYPE, nPortIndex);
        ENTRY_INT(OMX_AUDIO_CONFIG_VOLUMETYPE, bLinear);
        ENTRY_INT(OMX_AUDIO_CONFIG_VOLUMETYPE, sVolume.nValue);
        ENTRY_INT(OMX_AUDIO_CONFIG_VOLUMETYPE, sVolume.nMin);
        ENTRY_INT(OMX_AUDIO_CONFIG_VOLUMETYPE, sVolume.nMax);
}

void GenConfigEntries_MUTETYPE()
{
    BEGIN_NEW_ENTRY("MUTETYPE");
        ENTRY_INT(OMX_AUDIO_CONFIG_MUTETYPE, nPortIndex);
        ENTRY_INT(OMX_AUDIO_CONFIG_MUTETYPE, bMute);
}

void GenConfigEntries_CALLBACKREQUESTTYPE()
{
    BEGIN_NEW_ENTRY("CALLBACKREQUESTTYPE");
        ENTRY_INT(OMX_CONFIG_CALLBACKREQUESTTYPE, nPortIndex);
        ENTRY_INT(OMX_CONFIG_CALLBACKREQUESTTYPE, nIndex);
        ENTRY_INT(OMX_CONFIG_CALLBACKREQUESTTYPE, bEnable);
}

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
void GenConfigEntries_MDRCTYPE()
{
    BEGIN_NEW_ENTRY("MDRCTYPE");
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, nPortIndex);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, nChannel);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, sBandIndex.nValue);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, sBandIndex.nMin);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, sBandIndex.nMax);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, sBandRange.nValue);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, sBandLevel.nValue);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, sKneePoints.nValue);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,0);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,1);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,2);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,3);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,4);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,5);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,6);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,7);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,8);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,9);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,10);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,11);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,12);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,13);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,14);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,15);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,16);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,17);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,18);
        ENTRY_INT_MATRIX(OMX_AUDIO_CONFIG_MDRCTYPE, nDynamicResponse,19);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, sAttackTime.nValue);
        ENTRY_INT(OMX_AUDIO_CONFIG_MDRCTYPE, sReleaseTime.nValue);

    BEGIN_NEW_ENTRY("PARAM_MDRCTYPE");
        ENTRY_INT(AFM_AUDIO_PARAM_MDRC, nPortIndex);
        ENTRY_INT(AFM_AUDIO_PARAM_MDRC, nFilterType.nValue);
        ENTRY_INT(AFM_AUDIO_PARAM_MDRC, nFilterType.nMin);
        ENTRY_INT(AFM_AUDIO_PARAM_MDRC, nFilterType.nMax);

    BEGIN_NEW_ENTRY("CONFIG_MDRCTYPE");
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, nPortIndex);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, nMode);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, bLimiterRmsMeasure);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, sLimiterThresh);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, nBiquadOrder.nValue);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, nBiquadOrder.nMin);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, nBiquadOrder.nMax);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, nFIRSize.nValue);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, nFIRSize.nMin);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, nFIRSize.nMax);
        ENTRY_INT(AFM_AUDIO_GLOBAL_CONFIG_MDRC, bFIRGainOptimization);

}
#else
void GenConfigEntries_MDRCTYPE() {}
#endif

void GenConfigEntries_SPEECHTYPE()
{
    BEGIN_NEW_ENTRY("TXSPEECHENHANCEMENTTYPE");
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, nPortIndex);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, bEnable);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nEnabled);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nLinearEchoCancellation);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nEchoDetection);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nHighPassFilter);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nResidualEchoControl);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nComfortNoise);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nPureDelay);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFilterLength);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nShadowFilterLength);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxEchoPathGain);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nAdaptiveResidualLoss);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 0);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 1);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 2);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 3);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 4);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 5);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 6);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 7);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 8);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 9);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 10);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 11);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 12);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 13);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 14);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 15);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 16);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 17);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 18);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 19);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 20);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 21);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 22);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 23);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 24);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 25);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 26);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 27);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 28);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 29);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 30);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 31);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 32);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 33);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 34);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 35);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 36);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 37);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 38);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nRxResidualLossRegressor, 39);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nNonlinearSpeakerModelling);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nResidualEchoMargin);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nEchoDecayTime);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 0);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 1);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 2);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 3);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 4);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 5);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 6);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 7);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 8);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 9);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 10);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 11);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 12);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 13);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 14);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 15);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 16);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 17);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 18);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 19);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 20);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 21);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 22);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 23);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 24);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 25);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 26);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 27);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 28);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 29);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 30);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 31);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 32);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 33);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 34);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 35);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 36);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 37);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 38);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nMaxResidualLoss, 39);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nNonLinearEchoDecayTime);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nHarmonicGains, 0);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nHarmonicGains, 1);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nHarmonicGains, 2);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nHarmonicGains, 3);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nHarmonicGains, 4);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nHarmonicGains, 5);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 0);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 1);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 2);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 3);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 4);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 5);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 6);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 7);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 8);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 9);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 10);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 11);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 12);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 13);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 14);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 15);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 16);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 17);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 18);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 19);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 20);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 21);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 22);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 23);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 24);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 25);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 26);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 27);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 28);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 29);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 30);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nFundamentalGains, 31);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nHarmonicActivationLevel);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 0);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 1);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 2);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 3);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 4);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 5);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 6);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 7);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 8);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 9);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 10);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 11);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 12);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 13);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 14);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 15);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 16);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 17);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 18);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 19);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 20);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 21);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 22);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 23);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 24);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 25);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 26);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 27);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 28);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 29);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 30);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 31);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 32);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 33);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 34);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 35);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 36);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 37);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 38);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralLeakageGains, 39);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nLeakageActivationLevel);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nSpectralMaskingMargin);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nTemporalMaskingTime);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigEchoControl.nResidualEchoReleaserRate);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigMMic.nEnabled);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigMMic.nTxChannelMapping, 0);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigMMic.nTxChannelMapping, 1);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigMMic.nReduceFarField);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigMMic.nReduceWindNoise);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigMMic.nReferenceMicrophoneChannel);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nEnabled);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nAdaptiveHighPassFilter);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nDesiredNoiseReduction);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nNrCutOffFrequency);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nNoiseReductionDuringSpeech);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nNoiseReductionTradeoff);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nNoiseFloorPower);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nHighPassFilterSnrThreshold);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nHighPassCutOffMargin);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigTxLevelControl.nEnabled);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigTxLevelControl.nAdaptiveSignalGain);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigTxLevelControl.nNoiseAdaptiveGain);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigTxLevelControl.nNoiseAdaptiveGainSlope);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigTxLevelControl.nNoiseAdaptiveGainThreshold);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigTxLevelControl.nTargetLevel);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_TXSPEECHENHANCEMENTTYPE, sConfigTxLevelControl.nStaticGain);
    /* end "TXSPEECHENHANCEMENTTYPE" */
    BEGIN_NEW_ENTRY("RXSPEECHENHANCEMENTTYPE");
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, nPortIndex);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, bEnable);
        /* sConfigRxNoiseReduction member */
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nEnabled);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nAdaptiveHighPassFilter);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nDesiredNoiseReduction);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nNrCutOffFrequency);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nNoiseReductionDuringSpeech);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nNoiseReductionTradeoff);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nNoiseFloorPower);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nHighPassFilterSnrThreshold);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigNoiseReduction.nHighPassCutOffMargin);
        /* sConfigRxEqualizer member*/
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nEnabled);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nSpeechEnhance);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nCalibrationMode);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nComplexityLevel);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nMinNoiseSpeechEnhance);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nSaturationMargin);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nMinimumTargetSNR);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nMaxGainTargetSNR);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nThresholdSNREnergyRedist);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nMaxGainEnergyRedistHf);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nMaxLossEnergyRedistLf);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 0);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 1);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 2);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 3);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 4);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 5);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 6);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 7);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 8);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 9);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 10);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 11);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 12);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 13);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 14);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 15);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 16);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 17);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 18);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 19);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 20);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 21);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 22);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 23);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 24);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 25);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 26);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 27);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 28);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 29);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 30);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 31);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 32);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 33);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 34);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 35);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 36);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 37);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 38);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nTxAcomp, 39);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 0);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 1);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 2);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 3);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 4);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 5);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 6);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 7);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 8);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 9);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 10);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 11);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 12);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 13);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 14);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 15);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 16);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 17);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 18);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 19);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 20);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 21);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 22);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 23);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 24);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 25);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 26);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 27);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 28);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 29);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 30);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 31);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 32);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 33);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 34);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 35);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 36);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 37);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 38);
        ENTRY_INT_ARRAY(OMX_STE_AUDIO_CONFIG_RXSPEECHENHANCEMENTTYPE, sConfigRxEqualizer.nRxAcomp, 39);
    /* end "RXSPEECHENHANCEMENTTYPE" */
    BEGIN_NEW_ENTRY("AUDIOCTMTYPE");
        ENTRY_INT(OMX_STE_CONFIG_AUDIO_CTMTYPE, nPortIndex);
        ENTRY_INT(OMX_STE_CONFIG_AUDIO_CTMTYPE, bEnable);
    /* end "AUDIOCTMTYPE" */

}

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
void GenConfigEntries_AUDIODRCTYPE()
{
    BEGIN_NEW_ENTRY("AUDIODRCTYPE");
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, nPortIndex);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, bEnable);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, bUseSeparateChannelSettings);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sLookAheadTime.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, bUsePeakMeasure);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointX, nValue, 0);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointX, nValue, 1);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointX, nValue, 2);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointX, nValue, 3);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointX, nValue, 4);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointX, nValue, 5);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointX, nValue, 6);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointX, nValue, 7);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointY, nValue, 0);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointY, nValue, 1);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointY, nValue, 2);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointY, nValue, 3);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointY, nValue, 4);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointY, nValue, 5);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointY, nValue, 6);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointY, nValue, 7);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sInputGain.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sOffsetGain.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sLevelDetectorAttackTime.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sLevelDetectorReleaseTime.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sGainProcessorAttackTime.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sGainProcessorReleaseTime.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, eAcceptanceLevel);

       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, bUsePeakMeasureChannel2);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointXChannel2, nValue, 0);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointXChannel2, nValue, 1);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointXChannel2, nValue, 2);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointXChannel2, nValue, 3);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointXChannel2, nValue, 4);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointXChannel2, nValue, 5);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointXChannel2, nValue, 6);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointXChannel2, nValue, 7);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointYChannel2, nValue, 0);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointYChannel2, nValue, 1);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointYChannel2, nValue, 2);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointYChannel2, nValue, 3);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointYChannel2, nValue, 4);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointYChannel2, nValue, 5);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointYChannel2, nValue, 6);
       ENTRY_INT_STRUCT_ARRAY(OMX_AUDIO_CONFIG_DRCTYPE, sStaticCurvePointYChannel2, nValue, 7);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sInputGainChannel2.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sOffsetGainChannel2.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sLevelDetectorAttackTimeChannel2.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sLevelDetectorReleaseTimeChannel2.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sGainProcessorAttackTimeChannel2.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, sGainProcessorReleaseTimeChannel2.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_DRCTYPE, eAcceptanceLevelChannel2);
}
#else
void GenConfigEntries_AUDIODRCTYPE() {}
#endif


void GenConfigEntries_ALSADEVICETYPE()
{
    BEGIN_NEW_ENTRY("ALSADEVICETYPE");
        ENTRY_INT(OMX_AUDIO_CONFIG_ALSADEVICETYPE, nPortIndex);
        ENTRY_INT(OMX_AUDIO_CONFIG_ALSADEVICETYPE, bEnable);
        ENTRY_TEXT(OMX_AUDIO_CONFIG_ALSADEVICETYPE, cDeviceName);
}

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
void GenConfigEntries_TRANSDUCEREQUALIZERTYPE()
{
    BEGIN_NEW_ENTRY("PARAM_TRANSDUCEREQUALIZERTYPE");
        ENTRY_INT(AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE, MemoryPreset);
        ENTRY_INT(AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE, stereo);
        ENTRY_INT(AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE, nb_alloc_biquad_cells_per_channel);
        ENTRY_INT(AFM_AUDIO_PARAM_TRANSDUCEREQUALIZERTYPE, nb_alloc_FIR_coefs_per_channel);

    BEGIN_NEW_ENTRY("CONFIG_TRANSDUCEREQUALIZERTYPE");
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, same_biquad_l_r);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, same_FIR_l_r);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_first);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_gain_exp_l);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_gain_mant_l);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_gain_exp_r);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_gain_mant_r);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, FIR_gain_exp_l);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, FIR_gain_mant_l);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, FIR_gain_exp_r);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, FIR_gain_mant_r);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, nb_biquad_cells_per_channel);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, nb_FIR_coefs_per_channel);
        ENTRY_INT_STRUCT_ARRAY_LOOP(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_cell, b_exp, 40);
        ENTRY_INT_STRUCT_ARRAY_LOOP(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE ,biquad_cell, b0, 40);
        ENTRY_INT_STRUCT_ARRAY_LOOP(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_cell, b1, 40);
        ENTRY_INT_STRUCT_ARRAY_LOOP(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_cell, b2, 40);
        ENTRY_INT_STRUCT_ARRAY_LOOP(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_cell, a1, 40);
        ENTRY_INT_STRUCT_ARRAY_LOOP(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, biquad_cell, a2, 40);
        ENTRY_INT_ARRAY_LOOP(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, FIR_coef, 400);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, bEnable);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, bAutomaticTransitionGain);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, transition_gain_exp_l);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, transition_gain_mant_l);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, transition_gain_exp_r);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, transition_gain_mant_r);
        ENTRY_INT(AFM_AUDIO_CONFIG_TRANSDUCEREQUALIZERTYPE, smoothing_delay_ms);
}
#else
void GenConfigEntries_TRANSDUCEREQUALIZERTYPE() {}
#endif

#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
void GenConfigEntries_SPLTYPE()
{
    BEGIN_NEW_ENTRY("PARAM_SPLTYPE");
        ENTRY_INT(AFM_AUDIO_PARAM_SPLTYPE, MemoryPreset);
        ENTRY_INT(AFM_AUDIO_PARAM_SPLTYPE, nType);
        ENTRY_INT(AFM_AUDIO_PARAM_SPLTYPE, nAttackTime);
        ENTRY_INT(AFM_AUDIO_PARAM_SPLTYPE, nReleaseTime);

    BEGIN_NEW_ENTRY("CONFIG_SPLLIMITTYPE");
        ENTRY_INT(OMX_AUDIO_CONFIG_SPLLIMITTYPE, nPortIndex);
        ENTRY_INT(OMX_AUDIO_CONFIG_SPLLIMITTYPE, bEnable);
        ENTRY_INT(OMX_AUDIO_CONFIG_SPLLIMITTYPE, eMode);
        ENTRY_INT(OMX_AUDIO_CONFIG_SPLLIMITTYPE, nSplLimit);
        ENTRY_INT(OMX_AUDIO_CONFIG_SPLLIMITTYPE, nGain);
}
#else
void GenConfigEntries_SPLTYPE() {}
#endif


#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
void GenConfigEntries_NOISEREDUCTION()
{
    BEGIN_NEW_ENTRY("CONFIG_NOISEREDUCTIONTYPE");
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nPortIndex);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, bEnable);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, bUseSeparateSettings);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nAdaptiveHighPassFilter);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nDesiredNoiseReduction);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nNrCutOffFrequency);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nNoiseReductionDuringSpeech);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nNoiseReductionTradeoff);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nNoiseFloorPower);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nHighPassFilterSnrThreshold);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nHighPassCutOffMargin);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nAdaptiveHighPassFilterChannel2);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nDesiredNoiseReductionChannel2);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nNrCutOffFrequencyChannel2);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nNoiseReductionDuringSpeechChannel2);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nNoiseReductionTradeoffChannel2);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nNoiseFloorPowerChannel2);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nHighPassFilterSnrThresholdChannel2);
    ENTRY_INT(OMX_STE_AUDIO_CONFIG_NOISEREDUCTIONTYPE, nHighPassCutOffMarginChannel2);
}
#else
void GenConfigEntries_NOISEREDUCTION() {}
#endif


#ifndef ADM_NO_EFFECT_CONFIG_SUPPORT
void GenConfigEntries_VIRTUALSURROUNDTYPE()
{
    BEGIN_NEW_ENTRY("VIRTUALSURROUNDTYPE");
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nPortIndex);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, bEnable);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, eVirtualSurroundMode);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, bRoomSimulationFrontEnable);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, bRoomSimulationSurroundEnable);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, eRoomSimulationMode);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, eRoomSimulationRoomType);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nVirtualSurroundLeftFrontGain.nValue);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nVirtualSurroundRightFrontGain.nValue);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nVirtualSurroundCenterFrontGain.nValue);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nVirtualSurroundLeftSurroundGain.nValue);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nVirtualSurroundRightSurroundGain.nValue);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nVirtualSurroundLowFrequencyEffectsGain.nValue);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nVirtualSurroundFrontRoomSimulationGain.nValue);
        ENTRY_INT(OMX_STE_AUDIO_CONFIG_VIRTUALSURROUNDTYPE, nVirtualSurroundSurroundRoomSimulationGain.nValue);
}
#else
void GenConfigEntries_VIRTUALSURROUNDTYPE() {}
#endif


#if defined(STE_PLATFORM_U8500) && !defined(ADM_NO_EFFECT_CONFIG_SUPPORT)
void GenConfigEntries_AUDIOCNGTYPE()
{
    BEGIN_NEW_ENTRY("AUDIOCNGTYPE");
       ENTRY_INT(OMX_AUDIO_CONFIG_COMFORTNOISEGENERATIONTYPE, nPortIndex);
       ENTRY_INT(OMX_AUDIO_CONFIG_COMFORTNOISEGENERATIONTYPE, bEnable);
       ENTRY_INT(OMX_AUDIO_CONFIG_COMFORTNOISEGENERATIONTYPE, sLevel.nValue);
       ENTRY_INT(OMX_AUDIO_CONFIG_COMFORTNOISEGENERATIONTYPE, sLevel.nMin);
       ENTRY_INT(OMX_AUDIO_CONFIG_COMFORTNOISEGENERATIONTYPE, sLevel.nMax);
}
#else
void GenConfigEntries_AUDIOCNGTYPE() {}
#endif

int main()
{
    printf(";\n");
    printf("; SQL statements for common IL config structs\n");
    printf("; This file is autogenerated by gen_cfgdef.cc - do not edit\n");
    printf(";\n");

    GenConfigEntries_PcmModeType();
    GenConfigEntries_VOLUMETYPE();
    GenConfigEntries_MUTETYPE();
    GenConfigEntries_CALLBACKREQUESTTYPE();
    GenConfigEntries_MDRCTYPE();
    GenConfigEntries_SPEECHTYPE();
    GenConfigEntries_AUDIODRCTYPE();
    GenConfigEntries_AUDIOCNGTYPE();
    GenConfigEntries_ALSADEVICETYPE();
    GenConfigEntries_TRANSDUCEREQUALIZERTYPE();
    GenConfigEntries_SPLTYPE();
    GenConfigEntries_VIRTUALSURROUNDTYPE();
    GenConfigEntries_NOISEREDUCTION();
  
    return 0;
}



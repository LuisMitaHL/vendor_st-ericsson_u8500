/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "audiotables.h"

const AUDIO_CONST_MEM int AudioTables_sampling_freqs[ESAA_FREQ_LAST_IN_LIST] = {

	48000,            //ESAA_FREQ_UNKNOWNKHZ,
	192000,           //ESAA_FREQ_192KHZ,
	176400,           //ESAA_FREQ_176_4KHZ,
	128000,           //ESAA_FREQ_128KHZ,
	96000,            //ESAA_FREQ_96KHZ,
	88200,            //ESAA_FREQ_88_2KHZ,
	64000,            //ESAA_FREQ_64KHZ,
	48000,            //ESAA_FREQ_48KHZ,
	44100,            //ESAA_FREQ_44_1KHZ,
	32000,            //ESAA_FREQ_32KHZ,
	24000,            //ESAA_FREQ_24KHZ,
	22050,            //ESAA_FREQ_22_05KHZ,
	16000,            //ESAA_FREQ_16KHZ,
	12000,            //ESAA_FREQ_12KHZ,
    11025,            //ESAA_FREQ_11_025KHZ,
	8000,             //ESAA_FREQ_8KHZ,
	7200			  //ESAA_FREQ_7_2KHZ	
};

int  AUDIO_CONST_MEM *fn_AudioTables_sampling_freqs(void)
{
    return (int AUDIO_CONST_MEM *)AudioTables_sampling_freqs;
}

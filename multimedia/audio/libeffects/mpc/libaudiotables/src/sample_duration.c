/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "audiotables.h"

const AUDIO_CONST_MEM int AudioTables_sample_duration[ESAA_FREQ_LAST_IN_LIST] = {
	FORMAT_FLOAT((1.0/48000.0), MAXVAL),  //ESAA_FREQ_UNKNOWNKHZ,
	FORMAT_FLOAT((1.0/192000.0), MAXVAL), //ESAA_FREQ_192KHZ,
	FORMAT_FLOAT((1.0/176400.0), MAXVAL), //ESAA_FREQ_176_4KHZ,
	FORMAT_FLOAT((1.0/128000.0), MAXVAL), //ESAA_FREQ_128KHZ,
	FORMAT_FLOAT((1.0/96000.0), MAXVAL),  //ESAA_FREQ_96KHZ,
	FORMAT_FLOAT((1.0/88200.0), MAXVAL),  //ESAA_FREQ_88_2KHZ,
	FORMAT_FLOAT((1.0/64000.0), MAXVAL),  //ESAA_FREQ_64KHZ,
	FORMAT_FLOAT((1.0/48000.0), MAXVAL),  //ESAA_FREQ_48KHZ,
	FORMAT_FLOAT((1.0/44100.0), MAXVAL),  //ESAA_FREQ_44_1KHZ,
	FORMAT_FLOAT((1.0/32000.0), MAXVAL),  //ESAA_FREQ_32KHZ,
	FORMAT_FLOAT((1.0/24000.0), MAXVAL),  //ESAA_FREQ_24KHZ,
	FORMAT_FLOAT((1.0/22050.0), MAXVAL),  //ESAA_FREQ_22_05KHZ,
	FORMAT_FLOAT((1.0/16000.0), MAXVAL),  //ESAA_FREQ_16KHZ,
	FORMAT_FLOAT((1.0/12000.0), MAXVAL),  //ESAA_FREQ_12KHZ,
    FORMAT_FLOAT((1.0/11025.0), MAXVAL),  //ESAA_FREQ_11_025KHZ,
	FORMAT_FLOAT((1.0/8000.0), MAXVAL),   //ESAA_FREQ_8KHZ,
	FORMAT_FLOAT((1.0/7200.0), MAXVAL)    //ESAA_FREQ_7_2KHZ
};


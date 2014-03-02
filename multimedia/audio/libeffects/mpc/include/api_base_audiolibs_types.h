/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _api_base_audiolibs_types_h_
#define _api_base_audiolibs_types_h_

/* #ifdef _NMF_MPC_ */
#include "samplefreq.idt"
#include "memorypreset.idt"

/***************************************/
// Enum of supported sample frequencies.
/***************************************/
typedef enum
{
	ESAA_FREQ_UNKNOWNKHZ = FREQ_UNKNOWN,
	ESAA_FREQ_192KHZ     = FREQ_192KHZ,
	ESAA_FREQ_176_4KHZ   = FREQ_176_4KHZ,
	ESAA_FREQ_128KHZ     = FREQ_128KHZ,
	ESAA_FREQ_96KHZ      = FREQ_96KHZ,
	ESAA_FREQ_88_2KHZ    = FREQ_88_2KHZ,
	ESAA_FREQ_64KHZ      = FREQ_64KHZ,
	ESAA_FREQ_48KHZ      = FREQ_48KHZ,
	ESAA_FREQ_44_1KHZ    = FREQ_44_1KHZ,
	ESAA_FREQ_32KHZ      = FREQ_32KHZ,
	ESAA_FREQ_24KHZ      = FREQ_24KHZ,
	ESAA_FREQ_22_05KHZ   = FREQ_22_05KHZ,
	ESAA_FREQ_16KHZ      = FREQ_16KHZ,
	ESAA_FREQ_12KHZ      = FREQ_12KHZ,
	ESAA_FREQ_11_025KHZ  = FREQ_11_025KHZ,
	ESAA_FREQ_8KHZ       = FREQ_8KHZ,
	ESAA_FREQ_7_2KHZ     = FREQ_7_2KHZ,
	ESAA_FREQ_LAST_IN_LIST
} t_saa_sample_freq;

/*****************************************************/
// Enum of memory preset
/*****************************************************/
typedef enum {
    	ESAA_MEM_DEFAULT           = MEM_PRESET_DEFAULT,
  	ESAA_MEM_ALL_DDR           = MEM_PRESET_ALL_DDR,
        ESAA_MEM_ALL_TCM           = MEM_PRESET_ALL_TCM,
        ESAA_MEM_MIX_DDR_TCM_1     = MEM_PRESET_MIX_DDR_TCM_1,
        ESAA_MEM_MIX_DDR_TCM_2     = MEM_PRESET_MIX_DDR_TCM_2,
        ESAA_MEM_MIX_DDR_TCM_3     = MEM_PRESET_MIX_DDR_TCM_3,
        ESAA_MEM_MIX_DDR_TCM_4     = MEM_PRESET_MIX_DDR_TCM_4,
        ESAA_MEM_MIX_DDR_TCM_5     = MEM_PRESET_MIX_DDR_TCM_5,
        ESAA_MEM_ALL_ESRAM         = MEM_PRESET_ALL_ESRAM,
        ESAA_MEM_MIX_ESRAM_DDR     = MEM_PRESET_MIX_ESRAM_DDR,
        ESAA_MEM_MIX_ESRAM_TCM     = MEM_PRESET_MIX_ESRAM_TCM,
        ESAA_MEM_MIX_ESRAM_OTHER_1 = MEM_PRESET_MIX_ESRAM_OTHER_1,
        ESAA_MEM_MIX_ESRAM_OTHER_2 = MEM_PRESET_MIX_ESRAM_OTHER_2,
        ESAA_MEM_MIX_ESRAM_OTHER_3 = MEM_PRESET_MIX_ESRAM_OTHER_3,
        ESAA_MEM_MIX_ESRAM_OTHER_4 = MEM_PRESET_MIX_ESRAM_OTHER_4,
        ESAA_MEM_MIX_ESRAM_OTHER_5 = MEM_PRESET_MIX_ESRAM_OTHER_5
}t_saa_memory_preset;

/*
  #else _NMF_MPC_
*/  
/***************************************/
// Enum of supported sample frequencies.
/***************************************/
/*typedef enum
  {
  ESAA_FREQ_UNKNOWNKHZ,
  ESAA_FREQ_192KHZ,
  ESAA_FREQ_176_4KHZ,
  ESAA_FREQ_128KHZ,
  ESAA_FREQ_96KHZ,
  ESAA_FREQ_88_2KHZ,
  ESAA_FREQ_64KHZ,
  ESAA_FREQ_48KHZ,
  ESAA_FREQ_44_1KHZ,
  ESAA_FREQ_32KHZ,
  ESAA_FREQ_24KHZ,
  ESAA_FREQ_22_05KHZ,
  ESAA_FREQ_16KHZ,
  ESAA_FREQ_12KHZ,
  ESAA_FREQ_11_025KHZ,
  ESAA_FREQ_8KHZ,
  ESAA_FREQ_7_2KHZ,
  ESAA_FREQ_LAST_IN_LIST
  } t_saa_sample_freq;
*/
/*****************************************************/
// Enum of memory preset
/*****************************************************/
/*typedef enum {
  ESAA_MEM_DEFAULT,
  ESAA_MEM_ALL_DDR,
  ESAA_MEM_ALL_TCM,
  ESAA_MEM_MIX_DDR_TCM_1,
  ESAA_MEM_MIX_DDR_TCM_2,
  ESAA_MEM_MIX_DDR_TCM_3,
  ESAA_MEM_MIX_DDR_TCM_4,
  ESAA_MEM_MIX_DDR_TCM_5,
  ESAA_MEM_ALL_ESRAM,
  ESAA_MEM_MIX_ESRAM_DDR,
  ESAA_MEM_MIX_ESRAM_TCM,
  ESAA_MEM_MIX_ESRAM_OTHER_1,
  ESAA_MEM_MIX_ESRAM_OTHER_2,
  ESAA_MEM_MIX_ESRAM_OTHER_3,
  ESAA_MEM_MIX_ESRAM_OTHER_4,
  ESAA_MEM_MIX_ESRAM_OTHER_5
  }t_saa_memory_preset;
  
  #endif // _NMF_MPC_
*/
#endif //_api_base_audiolibs_types_h_ 

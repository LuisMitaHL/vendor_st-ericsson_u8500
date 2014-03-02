
 

#ifndef _audiotables_h_
#define _audiotables_h_

#include "audiolibs_common.h"
#include "api_base_audiolibs_types.h"

/* value of sampling frequencies in Hertz */
extern const AUDIO_CONST_MEM int AudioTables_sampling_freqs[ESAA_FREQ_LAST_IN_LIST];

/* length of one sample, inverse of previous table, fractional representation */
extern const AUDIO_CONST_MEM int AudioTables_sample_duration[ESAA_FREQ_LAST_IN_LIST];

/* table for fractional sqrt() */
extern Float const SQRT_FRACT AudioTables_fract_sqrtTab[512];

extern CPPCALL dFloat AudioTables_fract_sqrt(Float val);

int AUDIO_CONST_MEM *fn_AudioTables_sampling_freqs(void);
#endif /* Do not edit below this line */

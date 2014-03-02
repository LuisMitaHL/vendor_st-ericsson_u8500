#ifndef _aac_extern_ram_h_
#define _aac_extern_ram_h_

// Uncomment this if you plan to link this library against CRTL code

#include "audiolibs_common.h"
#include "defines.h"
#include "chandefs.h"

        // Float           State_bank[LN2 * (Chans - XChans)];
#ifdef COMPRESS_STATE_BUFFER
#define SIZE_STATE_BANK    (LN2/2*(Chans-XChans)*sizeof(Float))
#else
#define SIZE_STATE_BANK    (LN2*(Chans-XChans)*sizeof(Float))
#endif /* COMPRESS_STATE_BUFFER */

#define SIZE_FACTOR_BANK   (MAXBANDS*(Chans-XChans)*sizeof(SHORT))

#ifndef COMPRESS_CB_MAP_AND_SC_FACTORS
#define SIZE_CB_MAP_BANK   (MAXBANDS*(Chans-XChans)*sizeof(UCHAR))
#else
#define SIZE_CB_MAP_BANK   0
#endif /* COMPRESS_CB_MAP_AND_SC_FACTORS */

#define SIZE_GROUP_BANK    (NSHORT*Chans*sizeof(UCHAR))

#ifdef MAIN_PROFILE

#define SIZE_LP_FLAG_BANK  (MAXBANDS * Chans * sizeof(int))
#define SIZE_LEN_PRED_BANK ((LEN_PRED_RSTGRP + 1) * Chans * sizeof(int))
#define SIZE_PRED_BANK     (LN*Chans*sizeof(PRED_STATUS))

#else

#define SIZE_LP_FLAG_BANK  0
#define SIZE_LEN_PRED_BANK 0
#define SIZE_PRED_BANK     0

#endif /* MAIN_PROFILE */

#define SIZE_TNS_BANK      ((Chans - XChans) * sizeof(TNS_frame_info))

#define SIZE_MASK_BANK     (SIZE_MASK_ARRAY * (Winds-XChans) * sizeof(UCHAR))

#ifdef SSR

#define SIZE_SSR_BANK      ((Chans-XChans)*sizeof(GAIN_CONTROL_DATA))
#define SIZE_SSR_SAVE_BANK ((Chans-XChans)*sizeof(GAIN_CONTROL_DATA_SAVE))
#define SIZE_SSR_IPQF_FIFO ((Chans-XChans)*NPQFTAPS*sizeof(Float))    

#else

#define SIZE_SSR_BANK      0
#define SIZE_SSR_SAVE_BANK 0
#define SIZE_SSR_IPQF_FIFO 0

#endif /* SSR */
#ifdef MPEG4V1LTP
#define SIZE_LTP_BANK Chans*sizeof(NOK_LT_PRED_STATUS)
#else
#define SIZE_LTP_BANK 0
#endif
#ifndef __flexcc2__
/* account for alignement which results in memory losses */
#define SIZE_PADDING 2048
#else
#define SIZE_PADDING 0
#endif /* __flexcc2__ */

#define AAC_HEAP_SIZE  \
(SIZE_STATE_BANK     ) + \
(SIZE_FACTOR_BANK    ) + \
(SIZE_CB_MAP_BANK    ) + \
(SIZE_GROUP_BANK     ) + \
(SIZE_LP_FLAG_BANK   ) + \
(SIZE_LEN_PRED_BANK  ) + \
(SIZE_PRED_BANK      ) + \
(SIZE_TNS_BANK       ) + \
(SIZE_MASK_BANK      ) + \
(SIZE_SSR_BANK       ) + \
(SIZE_SSR_SAVE_BANK  ) + \
(SIZE_SSR_IPQF_FIFO  ) + \
(SIZE_LTP_BANK		 ) + \
(SIZE_PADDING)


#if !defined(CRTL)

extern int             aac_heap[AAC_HEAP_SIZE];

#endif

#endif /* Do not edit below this line */

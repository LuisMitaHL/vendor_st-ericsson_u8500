
#ifndef _local_fft_h_
#define _local_fft_h_

#include "audiolibs_common.h"
#include "fft.h"
#include "fft_table.h"

#define OPTIMIZE 
#define NDEBUG
#define CHECK_RANGE(a,b)  assert((a)<(b) && (a)>=-(b))

#ifdef PROJ_OP
/* growth detection is broken on op9 core */
#define SOFTWARE_GROWTH_DETECTION
#endif /* PROJ_OP */

/* defines for tests */
#ifdef __flexcc2__
#define TEST_MEM __EXTERN
#else // __flexcc2__
#define TEST_MEM 
#endif // __flexcc2__


#endif // _local_fft_h_

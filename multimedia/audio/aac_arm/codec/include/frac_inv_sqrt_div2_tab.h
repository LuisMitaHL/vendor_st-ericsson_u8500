#include "audiolibs_common.h"
#include "aac_local.h"


#if defined (PNS_IMP1)

#define INV_SQRT_DIV2_TAB_BITS 10
#define INV_SQRT_DIV2_TAB_SIZE (1 << INV_SQRT_DIV2_TAB_BITS)
extern Float const INV_SQRT_NRG aac_inv_sqrt_div2_IMP1[INV_SQRT_DIV2_TAB_SIZE];

#else

#if defined (PNS_IMP2)

#define INV_SQRT_DIV2_TAB_BITS 5
#define INV_SQRT_DIV2_TAB_SIZE (1 << INV_SQRT_DIV2_TAB_BITS)
extern Float const INV_SQRT_NRG aac_inv_sqrt_div2_IMP2[INV_SQRT_DIV2_TAB_SIZE];

#endif
#endif /* PNS_IMP1 elsif PNS_IMP2 */

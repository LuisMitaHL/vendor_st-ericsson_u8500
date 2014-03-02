#include "libeffects/libresampling/include/resample.h"
#include "api_base_audiolibs_types.h"
#include <assert.h>
#include <string.h>

int
resample_calc_heap_size_fixin_fixout(int inrate, int outrate,int lomips_to_out48, int *heapsiz, unsigned int blocksiz, unsigned int channel_nb)
{
    *heapsiz = 50000;
    return 0;
}

int
resample_calc_max_heap_size_fixin_fixout(int Inrate,int Outrate,int lomips_to_out48,int *heapsiz, unsigned int blocksiz,unsigned int channel_nb,unsigned int conversion_type)
{
    *heapsiz = 1000000;
    return 0;
}

int
resample_calc_scratch_size_fixin_fixout(int inrate,int outrate,int lomips_to_out48,int *scratchsiz, unsigned int blocksiz)
{
    *scratchsiz = 30000;
    return 0;
}

static int                   processing(struct _ResampleContext *ctx,
                                        int                     *in,
                                        int                     istride,
                                        int                     *nSamples,
                                        int                     *out,
                                        int                     *nbOutSamples,
                                        int                     *flush)
{
    assert( ((char*) ctx)[0] = 0x78);
    assert(in && nSamples && out && nbOutSamples && flush);
    *nbOutSamples = *nSamples;
    if (rand()%7 == 1) {
        *nbOutSamples = 0;
    } else if (rand()%7 == 1) {
        *nSamples = 0;
    }

    return 0;
}

int
resample_x_init_ctx_low_mips_fixin_fixout(char *heap,int size_heap,
                                          ResampleContext *ctx, int inrate,int outrate,int lomips_to_out48,
                                          unsigned int channel_nb, unsigned int blocksiz)
{
    memset(heap, 0x77, (size_t) size_heap);
    memset(ctx,  0x78, sizeof(*ctx));
    ctx->processing = processing;
    return 0;
}

// int resample_x_cpy_0_1_ctx_low_mips_fixin_fixout(ResampleContext *ctx, int Inrate,int Outrate,int lomips_to_out48,unsigned int blocksiz);

int resample_x_process_fixin_fixout(ResampleContext *ctx,
                                int *in,
                                int stride,
                                int *nbSamples,
                                int *output,
                                int *nbOut,
                                int *flush)
{
   return 0;
}

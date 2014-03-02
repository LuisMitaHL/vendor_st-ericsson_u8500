/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   main_CA9.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "transducer_equalizer_CA9.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef TEQ_CA9_NEON_PROFILING
#include "../../../tools/ARM/perf/include/tperf_api.h"
#endif /* TEQ_CA9_NEON_PROFILING */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __arm
extern void core_init(void);
#endif /* __arm */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif /* max */

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif /* min */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_TEST
#ifdef TEQ_CA9_BOARD_TEST
#include "pcm_tables_CA9.h"
#else /* TEQ_CA9_BOARD_TEST */
#define TEQ_STREAMS_PATH "../../../streams/transducer_equalizer/streams/"
#endif /* TEQ_CA9_BOARD_TEST */
#endif /* TEQ_CA9_TEST */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_VERIF_OPTIM_NEON
#define NB_SAMPLES_PER_CHANNEL 200
#endif /* TEQ_CA9_VERIF_OPTIM_NEON */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
#include <math.h>
extern void PerfEnableDisable(int);
extern void PerfSetTickRate(int);
extern void PerfReset(int);
extern int  PerfGetCount(int);

Tr_EQ_t_stat Tr_EQ_stat_biquad_mono          [NB_BIQUAD_MAX   - NB_BIQUAD_MIN   + 1], Tr_EQ_stat_biquad_stereo1          [NB_BIQUAD_MAX   - NB_BIQUAD_MIN   + 1], Tr_EQ_stat_biquad_stereo2          [NB_BIQUAD_MAX   - NB_BIQUAD_MIN   + 1];
Tr_EQ_t_stat Tr_EQ_stat_FIR_no_mem_shift_mono[NB_FIR_COEF_MAX - NB_FIR_COEF_MIN + 1], Tr_EQ_stat_FIR_no_mem_shift_stereo1[NB_FIR_COEF_MAX - NB_FIR_COEF_MIN + 1], Tr_EQ_stat_FIR_no_mem_shift_stereo2[NB_FIR_COEF_MAX - NB_FIR_COEF_MIN + 1];
Tr_EQ_t_stat Tr_EQ_stat_FIR_mem_shift_mono   [NB_FIR_COEF_MAX - NB_FIR_COEF_MIN + 1], Tr_EQ_stat_FIR_mem_shift_stereo1   [NB_FIR_COEF_MAX - NB_FIR_COEF_MIN + 1], Tr_EQ_stat_FIR_mem_shift_stereo2   [NB_FIR_COEF_MAX - NB_FIR_COEF_MIN + 1];
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TEQ_CA9_PRINT_NEON_CYCLES) || defined(TEQ_CA9_VERIF_OPTIM_NEON) || defined(TEQ_CA9_TEST)

#define BUFFER_PRINTF_SIZE 65536
#define MAX_PRINTF_SIZE    1024

static FILE *Tr_EQ_p_printf_file;
static char Tr_EQ_buffer_printf[BUFFER_PRINTF_SIZE];
char *Tr_EQ_p_buffer_printf;


static void Tr_EQ_buffer_printf_flush(void)
{
    int size = Tr_EQ_p_buffer_printf - Tr_EQ_buffer_printf;

    assert(size <= BUFFER_PRINTF_SIZE);
    if(size > 0)
    {
        if(Tr_EQ_p_printf_file != NULL)
        {
            fwrite(Tr_EQ_buffer_printf, 1, size, Tr_EQ_p_printf_file);
        }
        fwrite(Tr_EQ_buffer_printf, 1, size, stdout);
        Tr_EQ_p_buffer_printf = Tr_EQ_buffer_printf;
    }
}

void Tr_EQ_test_buffer_printf_flush(void)
{
#ifdef TEQ_CA9_BOARD_TEST
    if(Tr_EQ_p_buffer_printf - Tr_EQ_buffer_printf > BUFFER_PRINTF_SIZE - MAX_PRINTF_SIZE)
#endif /* TEQ_CA9_BOARD_TEST */
    {
        Tr_EQ_buffer_printf_flush();
    }
}

static void Tr_EQ_open_buffer_printf(void)
{
    //Tr_EQ_p_printf_file = fopen("transducer_equalizer_out.txt", "w");
    //assert(Tr_EQ_p_printf_file != NULL);
    Tr_EQ_p_printf_file   = NULL;
    Tr_EQ_p_buffer_printf = Tr_EQ_buffer_printf;
}
static void Tr_EQ_close_buffer_printf(void)
{
    Tr_EQ_buffer_printf_flush();
    if(Tr_EQ_p_printf_file != NULL)
    {
        fclose(Tr_EQ_p_printf_file);
        Tr_EQ_p_printf_file = NULL;
    }
}

#endif /* TEQ_CA9_PRINT_NEON_CYCLES || TEQ_CA9_VERIF_OPTIM_NEON || TEQ_CA9_TEST */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_PRINT_NEON_CYCLES

static void Tr_EQ_reset_stat(Tr_EQ_t_stat *p_stat)
{
    p_stat->cpt  = 0;
    p_stat->sum  = 0;
    p_stat->sum2 = 0;
    p_stat->min  = 0;
    p_stat->max  = 0;
}

void Tr_EQ_add_stat(Tr_EQ_t_stat *p_stat, int val)
{
    if(p_stat->cpt++ == 0)
    {
        p_stat->min = val;
        p_stat->max = val;
    }
    else if(val < p_stat->min)
    {
        p_stat->min = val;
    }
    else if(val > p_stat->max)
    {
        p_stat->max = val;
    }
    p_stat->sum  += (long long) val;
    p_stat->sum2 += ((long long) val) * ((long long) val);
}

static void Tr_EQ_display_stat(char *string, int nb_cells, Tr_EQ_t_stat *p_stat)
{
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "%s %d cells :\n", string, nb_cells);
    Tr_EQ_test_buffer_printf_flush();
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "cpt     = %d\n",  p_stat->cpt);
    Tr_EQ_test_buffer_printf_flush();
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "sum     = %lld\n",  p_stat->sum);
    Tr_EQ_test_buffer_printf_flush();
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "sum2    = %lld\n",  p_stat->sum2);
    Tr_EQ_test_buffer_printf_flush();
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "min     = %d\n",  p_stat->min);
    Tr_EQ_test_buffer_printf_flush();
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "max     = %d\n",  p_stat->max);
    Tr_EQ_test_buffer_printf_flush();
    if(p_stat->cpt > 0)
    {
        double avg     = ((double) p_stat->sum)  / ((double) p_stat->cpt);
        double var     = ((double) p_stat->sum2) / ((double) p_stat->cpt) - avg * avg;
        double std_dev = sqrt(var);

        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "avg     = %lf\n", avg);
        Tr_EQ_test_buffer_printf_flush();
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "var     = %lf\n", var);
        Tr_EQ_test_buffer_printf_flush();
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "std_dev = %lf\n", std_dev);
        Tr_EQ_test_buffer_printf_flush();
    }
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "------------------------------------------\n");
    Tr_EQ_test_buffer_printf_flush();
}

#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_TEST

#ifdef TEQ_CA9_BOARD_TEST
static void read_samples(const short *p_sample, int *buf, int n)
{
    int i;

    for(i = 0; i < n; i++)
    {
        buf[i] = ((int) *p_sample++) << 16;
    }
}
#else /* TEQ_CA9_BOARD_TEST */
static void read_samples(FILE *file, int *buf, int n)
{
    int msb, lsb, sample, i;

    for(i = 0; i < n; i++)
    {
        msb    = fgetc(file) & 0xFF;
        lsb    = fgetc(file) & 0xFF;
        sample = (msb << 24) + (lsb << 16);
        buf[i] = sample;
    }
}

static void write_samples(FILE *file, int *buf, int n)
{
    int sample, i;

    for(i = 0; i < n; i++)
    {
        if(buf[i] < 0x7FFF8000)
        {
            sample = ((buf[i] + (1 << 15)) >> 16);
        }
        else
        {
            sample = 0x7FFF;
        }

        fputc((sample >> 8) & 0xFF, file);
        fputc((sample >> 0) & 0xFF, file);
    }
}
#endif /* TEQ_CA9_BOARD_TEST */

#endif /* TEQ_CA9_TEST */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(TEQ_CA9_VERIF_OPTIM_NEON) || defined(TEQ_CA9_TEST)

typedef struct
{
    MMshort             gain_exp;
    MMshort             gain_mant;
    int                 nb_biquad_cells;
    const t_biquad_cell *p_biquad_cell;
} t_biquad_filter;

typedef struct
{
    MMshort       gain_exp;
    MMshort       gain_mant;
    int           nb_FIR_coefs;
    const MMshort *p_FIR_coef;
} t_FIR_filter;


// name            nb_biquads_cells gain_table                           frequency_table                                  sampling_frequency
// biquad_8k,      10,              [0 0 0.6 1 1 0.8 0.7 0.5 0.7],       [0 200 300 500 700 1000 2000 3000 4000],         8000
// biquad_low44k,  10,              [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1],   [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100
// biquad_high44k, 10,              [0.1 0.1 0.1 0.1 0.2 0.3 0.5 1 1 1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100
static const t_biquad_cell teq_biquad_8k[] =
{
    {1, 0x391DA5B7, 0x5C406F62, 0x24A142B1, 0x6910B878, 0x551E6271},
    {0, 0x7DE6ACC8, 0x5A6B5A76, 0x264589AF, 0x2426D9E1, 0x17754353},
    {1, 0x3ECD3481, 0x415E6C0E, 0x25B79105, 0x40B51A43, 0x4A5C2EA0},
    {1, 0x4041BC4F, 0xFD5299C5, 0x253033E5, 0xFD73F19C, 0x4AA130BA},
    {1, 0x2587A208, 0xB531A199, 0x257B2FF9, 0x941BDB08, 0x65495326},
    {1, 0x3E992809, 0xA19A97D5, 0x31B19F2A, 0x9E9AC86C, 0x675FA942},
    {1, 0x400BDB49, 0xFFEDF34F, 0x35279098, 0xFFF0BD52, 0x6A617D6A},
    {1, 0x3FC67295, 0x52306C47, 0x35BE404A, 0x52266B99, 0x6B1D4045},
    {2, 0x263FA605, 0xBD02A17E, 0x1FE4351E, 0x8DB027A4, 0x6BC2408B},
    {1, 0x3F259FE1, 0xAAD237E7, 0x35662512, 0xA98ADC51, 0x6BDC8C62}
};
static const t_biquad_cell teq_biquad_low44k[] =
{
    {0, 0x4CB54017, 0x97522D7D, 0x225EAE69, 0x9D20AF58, 0x4C24BD4D},
    {0, 0x4831086C, 0xB039472D, 0x1B372E76, 0xAA36B861, 0x3F340D4D},
    {0, 0x75B6570A, 0x8EF72F3D, 0x3B7E2F89, 0xBD9921ED, 0x44F971F6},
    {0, 0x7ACCB760, 0x972A7A32, 0x4F866B2D, 0xC7552C26, 0x52D34473},
    {0, 0x7E6AD176, 0x9B5DFE39, 0x6342A85B, 0xCC7053EB, 0x642AD035},
    {1, 0x3FE7DA0F, 0x9BC4A2BA, 0x36E9E906, 0x9BD485A5, 0x6D83C053},
    {0, 0x412BFF28, 0x891582F1, 0x37C04D3B, 0x89960E48, 0x6ED5B2C3},
    {0, 0x7FCF6BC7, 0x9BC6ED4B, 0x70617F2E, 0xCDF97DDF, 0x7004DC82},
    {1, 0x3A99A51C, 0x959F934B, 0x333D0BCC, 0x8ABAFC07, 0x71776291},
    {0, 0x7FFC7206, 0xA231A868, 0x7FFB2E4D, 0xD1176812, 0x7FFA78D4}
};
static const t_biquad_cell teq_biquad_high44k[] =
{
    {1, 0x34CDEFCA, 0xAD189DC8, 0x215A27E6, 0xB0DFBAC5, 0x33DE6942},
    {1, 0x3C23E812, 0xB7447E32, 0x1F3BCF8E, 0xB9E100A5, 0x3BF8742E},
    {0, 0x7E33BFF8, 0x84D32ADA, 0x479FAD35, 0xC3A1E23A, 0x484406D3},
    {0, 0x7FAD9B60, 0x90D9AC88, 0x589F556C, 0xC8EFF072, 0x5953252D},
    {1, 0x4003379E, 0xCB6AACEC, 0x35544F76, 0xCB91337F, 0x6AFC1B50},
    {1, 0x3E571B69, 0x8A8CEA97, 0x379F0473, 0x8AD6F976, 0x6C805D76},
    {1, 0x400399B8, 0x8B45ACC3, 0x3822ACA2, 0x8B434DDB, 0x706037E7},
    {1, 0x3FF79CDE, 0x9BB347C6, 0x383D49B7, 0x9BB5C3C7, 0x706177C8},
    {1, 0x4006A33F, 0xCEE377F6, 0x3A1967DD, 0xCEE254AD, 0x74367F75},
    {0, 0x7FFD2007, 0x9E1E1EC1, 0x7AF1F295, 0xCF0E7A97, 0x7AF4190E}
};

t_biquad_filter const biquad_filter_8k      = {1, 0x40FB1491, sizeof(teq_biquad_8k)      / sizeof(t_biquad_cell), teq_biquad_8k};
t_biquad_filter const biquad_filter_low44k  = {1, 0x400CDDA0, sizeof(teq_biquad_low44k)  / sizeof(t_biquad_cell), teq_biquad_low44k};
t_biquad_filter const biquad_filter_high44k = {1, 0x40012B4C, sizeof(teq_biquad_high44k) / sizeof(t_biquad_cell), teq_biquad_high44k};


// name         nb_FIRs_coefs gain_table                           frequency_table                                  sampling_frequency
// FIR_8k,      10,           [0 0 0.6 1 1 0.8 0.7 0.5 0.7],       [0 200 300 500 700 1000 2000 3000 4000],         8000
// FIR_low44k,  10,           [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1],   [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100
// FIR_high44k, 10,           [0.1 0.1 0.1 0.1 0.2 0.3 0.5 1 1 1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100
static const MMshort teq_FIR_8k[] =
{
    0x000A9E26, 0x000BC50C, 0x000F716A, 0x0014995A, 0x00193F1D, 0x0026467C, 0x002414E3, 0x002AD29D,
    0x00284D13, 0x00212127, 0x001F9D7E, 0x00164A23, 0xFFE93E5C, 0xFFE8B303, 0xFF2F8BC0, 0xFE9A1723,
    0xFD8698CA, 0xFC228AE1, 0xFAF57EFF, 0xF9E36809, 0xF833756E, 0xFB2CEFF0, 0xF6D38914, 0xFC0B55F0,
    0x0361008B, 0x53A9999A, 0x0361008B, 0xFC0B55F0, 0xF6D38914, 0xFB2CEFF0, 0xF833756E, 0xF9E36809,
    0xFAF57EFF, 0xFC228AE1, 0xFD8698CA, 0xFE9A1723, 0xFF2F8BC0, 0xFFE8B303, 0xFFE93E5C, 0x00164A23,
    0x001F9D7E, 0x00212127, 0x00284D13, 0x002AD29D, 0x002414E3, 0x0026467C, 0x00193F1D, 0x0014995A,
    0x000F716A, 0x000BC50C, 0x000A9E26
};
static const MMshort teq_FIR_low44k[] =
{
    0x0000B74D, 0x0002113F, 0x0003E060, 0x00070E4C, 0x000CC5C0, 0x0015DBC2, 0x00225C5B, 0x00322E16,
    0x00468349, 0x00620D0F, 0x00868768, 0x00B1D5D1, 0x00DF4721, 0x010DF3F6, 0x0145AA38, 0x0191E03B,
    0x01F3DBE0, 0x025ADC77, 0x02B191A6, 0x02FC4614, 0x036B15E4, 0x044338EA, 0x05A35E7C, 0x074FA1D5,
    0x08B88203, 0x16133333, 0x08B88203, 0x074FA1D5, 0x05A35E7C, 0x044338EA, 0x036B15E4, 0x02FC4614,
    0x02B191A6, 0x025ADC77, 0x01F3DBE0, 0x0191E03B, 0x0145AA38, 0x010DF3F6, 0x00DF4721, 0x00B1D5D1,
    0x00868768, 0x00620D0F, 0x00468349, 0x00322E16, 0x00225C5B, 0x0015DBC2, 0x000CC5C0, 0x00070E4C,
    0x0003E060, 0x0002113F, 0x0000B74D
};
static const MMshort teq_FIR_high44k[] =
{
    0xFFFF5116, 0xFFFF5856, 0x00005828, 0x0000AE4D, 0xFFFE376B, 0xFFF91F97, 0xFFF5B600, 0xFFF897CB,
    0xFFFED76A, 0xFFFCA7F9, 0xFFE9D66F, 0xFFD10DBA, 0xFFCAC1B2, 0xFFDE8CAC, 0xFFEB2A8F, 0xFFBF2388,
    0xFF5C07BC, 0xFF1A64AD, 0xFF62AF0A, 0x00132899, 0x002327CF, 0xFE157445, 0xF9260C16, 0xF2543F21,
    0xEC46EA15, 0x69D9999A, 0xEC46EA15, 0xF2543F21, 0xF9260C16, 0xFE157445, 0x002327CF, 0x00132899,
    0xFF62AF0A, 0xFF1A64AD, 0xFF5C07BC, 0xFFBF2388, 0xFFEB2A8F, 0xFFDE8CAC, 0xFFCAC1B2, 0xFFD10DBA,
    0xFFE9D66F, 0xFFFCA7F9, 0xFFFED76A, 0xFFF897CB, 0xFFF5B600, 0xFFF91F97, 0xFFFE376B, 0x0000AE4D,
    0x00005828, 0xFFFF5856, 0xFFFF5116
};

t_FIR_filter const FIR_filter_8k      = {0, 0x7FFFFFFF, sizeof(teq_FIR_8k)      / sizeof(MMshort), teq_FIR_8k};
t_FIR_filter const FIR_filter_low44k  = {0, 0x7FFFFFFF, sizeof(teq_FIR_low44k)  / sizeof(MMshort), teq_FIR_low44k};
t_FIR_filter const FIR_filter_high44k = {0, 0x7FFFFFFF, sizeof(teq_FIR_high44k) / sizeof(MMshort), teq_FIR_high44k};



static void close_teq(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_teq_struct)
{
    if(p_teq_struct->p_alloc_biquad_coef != NULL)
    {
        free(p_teq_struct->p_alloc_biquad_coef);
    }
    if(p_teq_struct->p_biquad_mem != NULL)
    {
        free(p_teq_struct->p_biquad_mem);
    }
    if(p_teq_struct->p_FIR_coef != NULL)
    {
        free(p_teq_struct->p_FIR_coef);
    }
    if(p_teq_struct->p_FIR_mem != NULL)
    {
        free(p_teq_struct->p_FIR_mem);
    }
}

static int init_teq(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_teq_struct,
                    t_biquad_filter                     *p_biquad_filter1,
                    t_biquad_filter                     *p_biquad_filter2,
                    t_FIR_filter                        *p_FIR_filter1,
                    t_FIR_filter                        *p_FIR_filter2)
{
    int nb_channels, n, i;


    assert(p_teq_struct->nb_biquad_cells_per_channel <= p_teq_struct->nb_alloc_biquad_cells_per_channel);
    assert(p_teq_struct->nb_FIR_coefs_per_channel    <= p_teq_struct->nb_alloc_FIR_coefs_per_channel);

    p_teq_struct->p_alloc_biquad_coef = NULL;
    p_teq_struct->p_biquad_coef       = NULL;
    p_teq_struct->p_biquad_coef_st    = NULL;
    p_teq_struct->p_biquad_mem        = NULL;
    p_teq_struct->p_FIR_coef          = NULL;
    p_teq_struct->p_FIR_mem           = NULL;
    nb_channels                       = (p_teq_struct->stereo_config ? 2 : 1);


    if(!p_teq_struct->stereo_config)
    {
        p_teq_struct->same_biquad_l_r = 1;
        p_teq_struct->same_FIR_l_r    = 1;
    }

    if(p_teq_struct->nb_alloc_biquad_cells_per_channel > 0)
    {
        p_teq_struct->p_alloc_biquad_coef = (void *) malloc(nb_channels * p_teq_struct->nb_alloc_biquad_cells_per_channel * sizeof(t_biquad_cell));
        if(p_teq_struct->p_alloc_biquad_coef == NULL)
        {
            fprintf(stderr, "can't allocate p_teq_struct->p_alloc_biquad_coef (size = %d)\n", nb_channels * p_teq_struct->nb_alloc_biquad_cells_per_channel * sizeof(t_biquad_cell));
            close_teq(p_teq_struct);
            return -1;
        }

        p_teq_struct->p_biquad_mem = (MMshort *) malloc(nb_channels * 2 * (p_teq_struct->nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort));
        if(p_teq_struct->p_biquad_mem == NULL)
        {
            fprintf(stderr, "can't allocate p_teq_struct->p_biquad_mem (size = %d)\n", nb_channels * 2 * (p_teq_struct->nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort));
            close_teq(p_teq_struct);
            return -1;
        }
        for(i = 0; i < nb_channels * 2 * (p_teq_struct->nb_alloc_biquad_cells_per_channel + 1); i++)
        {
            p_teq_struct->p_biquad_mem[i] = 0;
        }
    }

    if(p_teq_struct->same_biquad_l_r)
    {
        p_teq_struct->p_biquad_coef_st = NULL;
        p_teq_struct->p_biquad_coef    = (t_biquad_cell    *) p_teq_struct->p_alloc_biquad_coef;
    }
    else
    {
        p_teq_struct->p_biquad_coef_st = (t_biquad_cell_st *) p_teq_struct->p_alloc_biquad_coef;
        p_teq_struct->p_biquad_coef    = NULL;
    }

    if(p_teq_struct->nb_alloc_FIR_coefs_per_channel > 0)
    {
        MMshort *p_FIR_mem;

        p_teq_struct->p_FIR_coef = (MMshort *) malloc(nb_channels * p_teq_struct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));
        if(p_teq_struct->p_FIR_coef == NULL)
        {
            fprintf(stderr, "can't allocate p_teq_struct->p_FIR_coef (size = %d)\n", nb_channels * p_teq_struct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));
            close_teq(p_teq_struct);
            return -1;
        }

#ifdef __arm
        p_teq_struct->p_FIR_mem  = (MMshort *) malloc(nb_channels * (FIR_MEM_OVERHEAD + p_teq_struct->nb_alloc_FIR_coefs_per_channel) * sizeof(MMshort));
        if(p_teq_struct->p_FIR_mem == NULL)
        {
            fprintf(stderr, "can't allocate p_teq_struct->p_FIR_mem (size = %d)\n", nb_channels * (FIR_MEM_OVERHEAD + p_teq_struct->nb_alloc_FIR_coefs_per_channel) * sizeof(MMshort));
            close_teq(p_teq_struct);
            return -1;
        }
        p_teq_struct->FIR_index  = nb_channels * FIR_MEM_OVERHEAD;
        p_FIR_mem                = p_teq_struct->p_FIR_mem + p_teq_struct->FIR_index;
#else /* __arm */
        p_teq_struct->p_FIR_mem  = (MMshort *) malloc(nb_channels * p_teq_struct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));
        if(p_teq_struct->p_FIR_mem == NULL)
        {
            fprintf(stderr, "can't allocate p_teq_struct->p_FIR_mem (size = %d)\n", nb_channels * p_teq_struct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));
            close_teq(p_teq_struct);
            return -1;
        }
        p_FIR_mem                = p_teq_struct->p_FIR_mem;
#endif /* __arm */
        for(i = 0; i < nb_channels * p_teq_struct->nb_alloc_FIR_coefs_per_channel; i++)
        {
            p_FIR_mem[i] = 0;
        }
    }

    if(p_teq_struct->same_biquad_l_r)
    {
        for(i = 0; i < p_teq_struct->nb_biquad_cells_per_channel; i++)
        {
            p_teq_struct->p_biquad_coef[i].b_exp = 0;
            p_teq_struct->p_biquad_coef[i].b0    = 0x7FFFFFFF;
            p_teq_struct->p_biquad_coef[i].b1    = 0;
            p_teq_struct->p_biquad_coef[i].b2    = 0;
            p_teq_struct->p_biquad_coef[i].a1    = 0;
            p_teq_struct->p_biquad_coef[i].a2    = 0;
        }
        if(p_biquad_filter1 == NULL)
        {
            p_teq_struct->biquad_gain_exp_l  = 0;
            p_teq_struct->biquad_gain_mant_l = 0x7FFFFFFF;
        }
        else
        {
            p_teq_struct->biquad_gain_exp_l  = p_biquad_filter1->gain_exp;
            p_teq_struct->biquad_gain_mant_l = p_biquad_filter1->gain_mant;
            n = p_biquad_filter1->nb_biquad_cells;
            if(n > p_teq_struct->nb_biquad_cells_per_channel)
            {
                n = p_teq_struct->nb_biquad_cells_per_channel;
            }
            for(i = 0; i < n; i++)
            {
                p_teq_struct->p_biquad_coef[i].b_exp = p_biquad_filter1->p_biquad_cell[i].b_exp;
                p_teq_struct->p_biquad_coef[i].b0    = p_biquad_filter1->p_biquad_cell[i].b0;
                p_teq_struct->p_biquad_coef[i].b1    = p_biquad_filter1->p_biquad_cell[i].b1;
                p_teq_struct->p_biquad_coef[i].b2    = p_biquad_filter1->p_biquad_cell[i].b2;
                p_teq_struct->p_biquad_coef[i].a1    = p_biquad_filter1->p_biquad_cell[i].a1;
                p_teq_struct->p_biquad_coef[i].a2    = p_biquad_filter1->p_biquad_cell[i].a2;
            }
        }
    }
    else
    {
        for(i = 0; i < p_teq_struct->nb_biquad_cells_per_channel; i++)
        {
            p_teq_struct->p_biquad_coef_st[i].b_exp_l = 0;
            p_teq_struct->p_biquad_coef_st[i].b_exp_r = 0;
            p_teq_struct->p_biquad_coef_st[i].b0_l    = 0x7FFFFFFF;
            p_teq_struct->p_biquad_coef_st[i].b0_r    = 0x7FFFFFFF;
            p_teq_struct->p_biquad_coef_st[i].b1_l    = 0;
            p_teq_struct->p_biquad_coef_st[i].b1_r    = 0;
            p_teq_struct->p_biquad_coef_st[i].b2_l    = 0;
            p_teq_struct->p_biquad_coef_st[i].b2_r    = 0;
            p_teq_struct->p_biquad_coef_st[i].a1_l    = 0;
            p_teq_struct->p_biquad_coef_st[i].a1_r    = 0;
            p_teq_struct->p_biquad_coef_st[i].a2_l    = 0;
            p_teq_struct->p_biquad_coef_st[i].a2_r    = 0;
        }
        if(p_biquad_filter1 == NULL)
        {
            p_teq_struct->biquad_gain_exp_l  = 0;
            p_teq_struct->biquad_gain_mant_l = 0x7FFFFFFF;
        }
        else
        {
            p_teq_struct->biquad_gain_exp_l  = p_biquad_filter1->gain_exp;
            p_teq_struct->biquad_gain_mant_l = p_biquad_filter1->gain_mant;
            n = p_biquad_filter1->nb_biquad_cells;
            if(n > p_teq_struct->nb_biquad_cells_per_channel)
            {
                n = p_teq_struct->nb_biquad_cells_per_channel;
            }
            for(i = 0; i < n; i++)
            {
                p_teq_struct->p_biquad_coef_st[i].b_exp_l = p_biquad_filter1->p_biquad_cell[i].b_exp;
                p_teq_struct->p_biquad_coef_st[i].b0_l    = p_biquad_filter1->p_biquad_cell[i].b0;
                p_teq_struct->p_biquad_coef_st[i].b1_l    = p_biquad_filter1->p_biquad_cell[i].b1;
                p_teq_struct->p_biquad_coef_st[i].b2_l    = p_biquad_filter1->p_biquad_cell[i].b2;
                p_teq_struct->p_biquad_coef_st[i].a1_l    = p_biquad_filter1->p_biquad_cell[i].a1;
                p_teq_struct->p_biquad_coef_st[i].a2_l    = p_biquad_filter1->p_biquad_cell[i].a2;
            }
        }
        if(p_biquad_filter2 == NULL)
        {
            p_teq_struct->biquad_gain_exp_r  = 0;
            p_teq_struct->biquad_gain_mant_r = 0x7FFFFFFF;
        }
        else
        {
            p_teq_struct->biquad_gain_exp_r  = p_biquad_filter2->gain_exp;
            p_teq_struct->biquad_gain_mant_r = p_biquad_filter2->gain_mant;
            n = p_biquad_filter2->nb_biquad_cells;
            if(n > p_teq_struct->nb_biquad_cells_per_channel)
            {
                n = p_teq_struct->nb_biquad_cells_per_channel;
            }
            for(i = 0; i < n; i++)
            {
                p_teq_struct->p_biquad_coef_st[i].b_exp_r = p_biquad_filter2->p_biquad_cell[i].b_exp;
                p_teq_struct->p_biquad_coef_st[i].b0_r    = p_biquad_filter2->p_biquad_cell[i].b0;
                p_teq_struct->p_biquad_coef_st[i].b1_r    = p_biquad_filter2->p_biquad_cell[i].b1;
                p_teq_struct->p_biquad_coef_st[i].b2_r    = p_biquad_filter2->p_biquad_cell[i].b2;
                p_teq_struct->p_biquad_coef_st[i].a1_r    = p_biquad_filter2->p_biquad_cell[i].a1;
                p_teq_struct->p_biquad_coef_st[i].a2_r    = p_biquad_filter2->p_biquad_cell[i].a2;
            }
        }
    }

    if(p_teq_struct->same_FIR_l_r)
    {
        if(p_teq_struct->nb_FIR_coefs_per_channel > 0)
        {
            p_teq_struct->p_FIR_coef[0] = 0x7FFFFFFF;
            for(i = 1; i < p_teq_struct->nb_FIR_coefs_per_channel; i++)
            {
                p_teq_struct->p_FIR_coef[i] = 0;
            }
        }
        if(p_FIR_filter1 == NULL)
        {
            p_teq_struct->FIR_gain_exp_l  = 0;
            p_teq_struct->FIR_gain_mant_l = 0x7FFFFFFF;
        }
        else
        {
            p_teq_struct->FIR_gain_exp_l  = p_FIR_filter1->gain_exp;
            p_teq_struct->FIR_gain_mant_l = p_FIR_filter1->gain_mant;
            n = p_FIR_filter1->nb_FIR_coefs;
            if(n > p_teq_struct->nb_FIR_coefs_per_channel)
            {
                n = p_teq_struct->nb_FIR_coefs_per_channel;
            }
            for(i = 0; i < n; i++)
            {
                p_teq_struct->p_FIR_coef[i] = p_FIR_filter1->p_FIR_coef[i];
            }
        }
    }
    else
    {
        if(p_teq_struct->nb_FIR_coefs_per_channel > 0)
        {
            p_teq_struct->p_FIR_coef[0] = 0x7FFFFFFF;
            p_teq_struct->p_FIR_coef[1] = 0x7FFFFFFF;
            for(i = 2; i < 2 * p_teq_struct->nb_FIR_coefs_per_channel; i++)
            {
                p_teq_struct->p_FIR_coef[i] = 0;
            }
        }
        if(p_FIR_filter1 == NULL)
        {
            p_teq_struct->FIR_gain_exp_l  = 0;
            p_teq_struct->FIR_gain_mant_l = 0x7FFFFFFF;
        }
        else
        {
            p_teq_struct->FIR_gain_exp_l  = p_FIR_filter1->gain_exp;
            p_teq_struct->FIR_gain_mant_l = p_FIR_filter1->gain_mant;
            n = p_FIR_filter1->nb_FIR_coefs;
            if(n > p_teq_struct->nb_FIR_coefs_per_channel)
            {
                n = p_teq_struct->nb_FIR_coefs_per_channel;
            }
            for(i = 0; i < n; i++)
            {
                p_teq_struct->p_FIR_coef[2 * i + 0] = p_FIR_filter1->p_FIR_coef[i];
            }
        }
        if(p_FIR_filter2 == NULL)
        {
            p_teq_struct->FIR_gain_exp_r  = 0;
            p_teq_struct->FIR_gain_mant_r = 0x7FFFFFFF;
        }
        else
        {
            p_teq_struct->FIR_gain_exp_r  = p_FIR_filter2->gain_exp;
            p_teq_struct->FIR_gain_mant_r = p_FIR_filter2->gain_mant;
            n = p_FIR_filter2->nb_FIR_coefs;
            if(n > p_teq_struct->nb_FIR_coefs_per_channel)
            {
                n = p_teq_struct->nb_FIR_coefs_per_channel;
            }
            for(i = 0; i < n; i++)
            {
                p_teq_struct->p_FIR_coef[2 * i + 1] = p_FIR_filter2->p_FIR_coef[i];
            }
        }
    }

    transducer_equalizer_set_filters_pointer(p_teq_struct);

    return 0;
}

#endif /* TEQ_CA9_VERIF_OPTIM_NEON || TEQ_CA9_TEST */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_VERIF_OPTIM_NEON

static void verif(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_teq_struct, MMshort *biquad_mem1, MMshort *biquad_mem2, MMshort *FIR_mem1, MMshort *FIR_mem2, MMshort *samples_out1, MMshort *samples_out2, int nb_samples_per_channel)
{
    char verif_name[100];
    int  nb_channels, i;


    if(p_teq_struct->stereo_process)
    {
        nb_channels = 2;
        if(p_teq_struct->nb_biquad_cells_per_channel == 0)
        {
            strcpy(verif_name, "(no biquad,");
        }
        else if(p_teq_struct->same_biquad_l_r)
        {
            strcpy(verif_name, "(same biquad,");
        }
        else
        {
            strcpy(verif_name, "(different biquad,");
        }
        if(p_teq_struct->nb_FIR_coefs_per_channel == 0)
        {
            strcat(verif_name, " no FIR");
        }
        else if(p_teq_struct->same_FIR_l_r)
        {
            strcat(verif_name, " same FIR");
        }
        else
        {
            strcat(verif_name, " different FIR");
        }
        strcat(verif_name, ") stereo");
    }
    else
    {
        nb_channels = 1;
        if(p_teq_struct->nb_biquad_cells_per_channel == 0)
        {
            strcpy(verif_name, "(no biquad,");
        }
        else
        {
            strcpy(verif_name, "(biquad,");
        }
        if(p_teq_struct->nb_FIR_coefs_per_channel == 0)
        {
            strcat(verif_name, " no FIR");
        }
        else
        {
            strcat(verif_name, " FIR");
        }
        strcat(verif_name, ") mono");
    }

    if(p_teq_struct->nb_biquad_cells_per_channel > 0)
    {
        if(memcmp(biquad_mem1, biquad_mem2, nb_channels * 2 * (p_teq_struct->nb_biquad_cells_per_channel + 1) * sizeof(MMshort)) == 0)
        {
            Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\nmem biquad %s (%d biquads) OK\n", verif_name, p_teq_struct->nb_biquad_cells_per_channel);
            Tr_EQ_test_buffer_printf_flush();
        }
        else
        {
            Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\nmem biquad %s (%d biquads) NOK\n", verif_name, p_teq_struct->nb_biquad_cells_per_channel);
            Tr_EQ_test_buffer_printf_flush();
            for(i = 0; i < nb_channels * 2 * (p_teq_struct->nb_biquad_cells_per_channel + 1); i++)
            {
                Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "mem biquad[%d] = 0x%08X / 0x%08X => diff = 0x%08X\n", i, biquad_mem1[i], biquad_mem2[i], biquad_mem1[i] - biquad_mem2[i]);
                Tr_EQ_test_buffer_printf_flush();
            }
        }
    }

    if(p_teq_struct->nb_FIR_coefs_per_channel > 0)
    {
        if(memcmp(FIR_mem1 + nb_channels, FIR_mem2 + nb_channels, nb_channels * (p_teq_struct->nb_FIR_coefs_per_channel - 1) * sizeof(MMshort)) == 0)
        {
            Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\nmem FIR %s (%d coefficients) OK\n", verif_name, p_teq_struct->nb_FIR_coefs_per_channel);
            Tr_EQ_test_buffer_printf_flush();
        }
        else
        {
            Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\nmem FIR %s (%d coefficients) NOK\n", verif_name, p_teq_struct->nb_FIR_coefs_per_channel);
            Tr_EQ_test_buffer_printf_flush();
            for(i = nb_channels; i < nb_channels * p_teq_struct->nb_FIR_coefs_per_channel; i++)
            {
                Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "mem FIR[%d] = 0x%08X / 0x%08X => diff = 0x%08X\n", i, FIR_mem1[i], FIR_mem2[i], FIR_mem1[i] - FIR_mem2[i]);
                Tr_EQ_test_buffer_printf_flush();
            }
        }
    }

    if(memcmp(samples_out1, samples_out2, nb_channels * nb_samples_per_channel * sizeof(MMshort)) == 0)
    {
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\nResult %s OK\n", verif_name);
        Tr_EQ_test_buffer_printf_flush();
    }
    else
    {
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\nResult %s NOK\n", verif_name);
        Tr_EQ_test_buffer_printf_flush();
        for(i = 0; i < nb_channels * nb_samples_per_channel; i++)
        {
            Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "samples_out[%d] = 0x%08X / 0x%08X => diff = 0x%08X\n", i, samples_out1[i], samples_out2[i], samples_out1[i] - samples_out2[i]);
            Tr_EQ_test_buffer_printf_flush();
        }
    }

    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\n\n");
    Tr_EQ_test_buffer_printf_flush();
}

static void init_memcmp(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_teq_struct,
                        MMshort                             **biquad_mem1,
                        MMshort                             **biquad_mem2,
                        MMshort                             **FIR_mem1,
                        MMshort                             **FIR_mem2,
                        MMshort                             **samples_in,
                        MMshort                             **samples_out1,
                        MMshort                             **samples_out2,
                        int                                 nb_samples_per_channel)
{
    int nb_channels = (p_teq_struct->stereo_config ? 2 : 1);
    int i;


    if(p_teq_struct->nb_alloc_biquad_cells_per_channel > 0)
    {
        *biquad_mem1                  = (MMshort *) malloc(nb_channels * 2 * (p_teq_struct->nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort));
        *biquad_mem2                  = (MMshort *) malloc(nb_channels * 2 * (p_teq_struct->nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort));
    }
    if(p_teq_struct->nb_alloc_FIR_coefs_per_channel > 0)
    {
        *FIR_mem1                     = (MMshort *) malloc(nb_channels * p_teq_struct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));
        *FIR_mem2                     = (MMshort *) malloc(nb_channels * (FIR_MEM_OVERHEAD + p_teq_struct->nb_alloc_FIR_coefs_per_channel) * sizeof(MMshort));
    }
    *samples_in                       = (MMshort *) malloc(nb_channels * nb_samples_per_channel * sizeof(MMshort));
    *samples_out1                     = (MMshort *) malloc(nb_channels * nb_samples_per_channel * sizeof(MMshort));
    *samples_out2                     = (MMshort *) malloc(nb_channels * nb_samples_per_channel * sizeof(MMshort));


    for(i = 0; i < nb_channels * nb_samples_per_channel; i++)
    {
        (*samples_in)[i] = rand();
    }
    if(p_teq_struct->nb_biquad_cells_per_channel > 0)
    {
        for(i = 0; i < nb_channels * 2 * (p_teq_struct->nb_biquad_cells_per_channel + 1); i ++)
        {
            p_teq_struct->p_biquad_mem[i] = rand();
        }
    }
    for(i = nb_channels * FIR_MEM_OVERHEAD; i < nb_channels * (FIR_MEM_OVERHEAD + p_teq_struct->nb_FIR_coefs_per_channel); i ++)
    {
        p_teq_struct->p_FIR_mem[i] = rand();
    }

    if(p_teq_struct->nb_alloc_biquad_cells_per_channel > 0)
    {
        memcpy(*biquad_mem1, p_teq_struct->p_biquad_mem, nb_channels * 2 * (p_teq_struct->nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort));
        memcpy(*biquad_mem2, p_teq_struct->p_biquad_mem, nb_channels * 2 * (p_teq_struct->nb_alloc_biquad_cells_per_channel + 1) * sizeof(MMshort));
    }
    if(p_teq_struct->nb_alloc_FIR_coefs_per_channel > 0)
    {
        memcpy(*FIR_mem1,
               p_teq_struct->p_FIR_mem + nb_channels * FIR_MEM_OVERHEAD,
               nb_channels * p_teq_struct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));
        memcpy(*FIR_mem2 + nb_channels * FIR_MEM_OVERHEAD,
               p_teq_struct->p_FIR_mem + nb_channels * FIR_MEM_OVERHEAD,
               nb_channels * p_teq_struct->nb_alloc_FIR_coefs_per_channel * sizeof(MMshort));
        p_teq_struct->FIR_index = nb_channels * FIR_MEM_OVERHEAD;
    }
}

static void free_memcmp(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_teq_struct,
                        MMshort                             *biquad_mem1,
                        MMshort                             *biquad_mem2,
                        MMshort                             *FIR_mem1,
                        MMshort                             *FIR_mem2,
                        MMshort                             *samples_in,
                        MMshort                             *samples_out1,
                        MMshort                             *samples_out2)
{
    if(p_teq_struct->nb_biquad_cells_per_channel > 0)
    {
        free(biquad_mem1);
        free(biquad_mem2);
    }
    if(p_teq_struct->nb_FIR_coefs_per_channel > 0)
    {
        free(FIR_mem1);
        free(FIR_mem2);
    }
    free(samples_in);
    free(samples_out1);
    free(samples_out2);
}

#endif /* TEQ_CA9_VERIF_OPTIM_NEON */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_VERIF_OPTIM_NEON

static void test_mono_biquad(int nb_biquads)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T teq_struct;
    MMshort                             *samples_in, *samples_out1, *samples_out2, *biquad_mem1, *biquad_mem2, *FIR_mem1, *FIR_mem2;
    MMshort                             *p_biquad_mem0, *p_biquad_mem1, *p_biquad_mem2, *p_FIR_mem0, *p_FIR_mem1, *p_FIR_mem2;
    int                                 nb_channels;


#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_reset_stat(&Tr_EQ_stat_biquad_mono[nb_biquads - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

    teq_struct.stereo_config                     = 0;
    teq_struct.stereo_process                    = 0;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.biquad_first                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_8KHZ;
    teq_struct.chan_nb                           = 1;
    teq_struct.offset                            = 1;
    teq_struct.nb_alloc_biquad_cells_per_channel = nb_biquads;
    teq_struct.nb_biquad_cells_per_channel       = nb_biquads;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = 0;
    teq_struct.nb_FIR_coefs_per_channel          = 0;
    if(init_teq(&teq_struct,
                (t_biquad_filter *) &biquad_filter_8k,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL) == -1)
    {
        return;
    }

    init_memcmp(&teq_struct, &biquad_mem1, &biquad_mem2, &FIR_mem1, &FIR_mem2, &samples_in, &samples_out1, &samples_out2, NB_SAMPLES_PER_CHANNEL);
    p_biquad_mem0 = teq_struct.p_biquad_mem;
    p_biquad_mem1 = biquad_mem1;
    p_biquad_mem2 = biquad_mem2;
    p_FIR_mem0    = teq_struct.p_FIR_mem;
    p_FIR_mem1    = FIR_mem1;
    p_FIR_mem2    = FIR_mem2;

    nb_channels             = (teq_struct.stereo_config ? 2 : 1);
    teq_struct.p_biquad_mem = p_biquad_mem1;
    teq_struct.p_FIR_mem    = p_FIR_mem1;
    transducer_equalizer_filter_no_optim(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                         samples_in,
                                         samples_out1,
                                         &teq_struct);


    teq_struct.p_biquad_mem = p_biquad_mem2;
    teq_struct.p_FIR_mem    = p_FIR_mem2;
#ifdef TEQ_CA9_NEON_PROFILING
    startperf();
#endif /* TEQ_CA9_NEON_PROFILING */
    transducer_equalizer_filter(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                samples_in,
                                samples_out2,
                                &teq_struct);
#ifdef TEQ_CA9_NEON_PROFILING
    endperf();
#endif /* TEQ_CA9_NEON_PROFILING */

    verif(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2 + teq_struct.FIR_index, samples_out1, samples_out2, NB_SAMPLES_PER_CHANNEL);

    teq_struct.p_biquad_mem = p_biquad_mem0;
    teq_struct.p_FIR_mem    = p_FIR_mem0;
    close_teq(&teq_struct);
    free_memcmp(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2, samples_in, samples_out1, samples_out2);
}

static void test_stereo_same_biquad(int nb_biquads)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T teq_struct;
    MMshort                             *samples_in, *samples_out1, *samples_out2, *biquad_mem1, *biquad_mem2, *FIR_mem1, *FIR_mem2;
    MMshort                             *p_biquad_mem0, *p_biquad_mem1, *p_biquad_mem2, *p_FIR_mem0, *p_FIR_mem1, *p_FIR_mem2;
    int                                 nb_channels;


#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_reset_stat(&Tr_EQ_stat_biquad_stereo1[nb_biquads - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

    teq_struct.stereo_config                     = 1;
    teq_struct.stereo_process                    = 1;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_44_1KHZ;
    teq_struct.chan_nb                           = 2;
    teq_struct.offset                            = 2;
    teq_struct.nb_alloc_biquad_cells_per_channel = nb_biquads;
    teq_struct.nb_biquad_cells_per_channel       = nb_biquads;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = 0;
    teq_struct.nb_FIR_coefs_per_channel          = 0;
    if(init_teq(&teq_struct,
                (t_biquad_filter *) &biquad_filter_low44k,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL) == -1)
    {
        return;
    }

    init_memcmp(&teq_struct, &biquad_mem1, &biquad_mem2, &FIR_mem1, &FIR_mem2, &samples_in, &samples_out1, &samples_out2, NB_SAMPLES_PER_CHANNEL);
    p_biquad_mem0 = teq_struct.p_biquad_mem;
    p_biquad_mem1 = biquad_mem1;
    p_biquad_mem2 = biquad_mem2;
    p_FIR_mem0    = teq_struct.p_FIR_mem;
    p_FIR_mem1    = FIR_mem1;
    p_FIR_mem2    = FIR_mem2;

    nb_channels             = (teq_struct.stereo_config ? 2 : 1);
    teq_struct.p_biquad_mem = p_biquad_mem1;
    teq_struct.p_FIR_mem    = p_FIR_mem1;
    transducer_equalizer_filter_no_optim(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                         samples_in,
                                         samples_out1,
                                         &teq_struct);


    teq_struct.p_biquad_mem = p_biquad_mem2;
    teq_struct.p_FIR_mem    = p_FIR_mem2;
#ifdef TEQ_CA9_NEON_PROFILING
    startperf();
#endif /* TEQ_CA9_NEON_PROFILING */
    transducer_equalizer_filter(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                samples_in,
                                samples_out2,
                                &teq_struct);
#ifdef TEQ_CA9_NEON_PROFILING
    endperf();
#endif /* TEQ_CA9_NEON_PROFILING */

    verif(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2 + teq_struct.FIR_index, samples_out1, samples_out2, NB_SAMPLES_PER_CHANNEL);

    teq_struct.p_biquad_mem = p_biquad_mem0;
    teq_struct.p_FIR_mem    = p_FIR_mem0;
    close_teq(&teq_struct);
    free_memcmp(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2, samples_in, samples_out1, samples_out2);
}

static void test_stereo_different_biquad(int nb_biquads)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T teq_struct;
    MMshort                             *samples_in, *samples_out1, *samples_out2, *biquad_mem1, *biquad_mem2, *FIR_mem1, *FIR_mem2;
    MMshort                             *p_biquad_mem0, *p_biquad_mem1, *p_biquad_mem2, *p_FIR_mem0, *p_FIR_mem1, *p_FIR_mem2;
    int                                 nb_channels;


#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_reset_stat(&Tr_EQ_stat_biquad_stereo2[nb_biquads - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

    teq_struct.stereo_config                     = 1;
    teq_struct.stereo_process                    = 1;
    teq_struct.same_biquad_l_r                   = 0;
    teq_struct.same_FIR_l_r                      = 0;
    teq_struct.sample_freq                       = ESAA_FREQ_44_1KHZ;
    teq_struct.chan_nb                           = 2;
    teq_struct.offset                            = 2;
    teq_struct.nb_alloc_biquad_cells_per_channel = nb_biquads;
    teq_struct.nb_biquad_cells_per_channel       = nb_biquads;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = 0;
    teq_struct.nb_FIR_coefs_per_channel          = 0;
    if(init_teq(&teq_struct,
                (t_biquad_filter *) &biquad_filter_low44k,
                (t_biquad_filter *) &biquad_filter_high44k,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL) == -1)
    {
        return;
    }

    init_memcmp(&teq_struct, &biquad_mem1, &biquad_mem2, &FIR_mem1, &FIR_mem2, &samples_in, &samples_out1, &samples_out2, NB_SAMPLES_PER_CHANNEL);
    p_biquad_mem0 = teq_struct.p_biquad_mem;
    p_biquad_mem1 = biquad_mem1;
    p_biquad_mem2 = biquad_mem2;
    p_FIR_mem0    = teq_struct.p_FIR_mem;
    p_FIR_mem1    = FIR_mem1;
    p_FIR_mem2    = FIR_mem2;

    nb_channels             = (teq_struct.stereo_config ? 2 : 1);
    teq_struct.p_biquad_mem = p_biquad_mem1;
    teq_struct.p_FIR_mem    = p_FIR_mem1;
    transducer_equalizer_filter_no_optim(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                         samples_in,
                                         samples_out1,
                                         &teq_struct);


    teq_struct.p_biquad_mem = p_biquad_mem2;
    teq_struct.p_FIR_mem    = p_FIR_mem2;
#ifdef TEQ_CA9_NEON_PROFILING
    startperf();
#endif /* TEQ_CA9_NEON_PROFILING */
    transducer_equalizer_filter(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                samples_in,
                                samples_out2,
                                &teq_struct);
#ifdef TEQ_CA9_NEON_PROFILING
    endperf();
#endif /* TEQ_CA9_NEON_PROFILING */

    verif(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2 + teq_struct.FIR_index, samples_out1, samples_out2, NB_SAMPLES_PER_CHANNEL);

    teq_struct.p_biquad_mem = p_biquad_mem0;
    teq_struct.p_FIR_mem    = p_FIR_mem0;
    close_teq(&teq_struct);
    free_memcmp(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2, samples_in, samples_out1, samples_out2);
}

static void test_mono_FIR(int nb_FIR_coefs_per_channel)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T teq_struct;
    MMshort                             *samples_in, *samples_out1, *samples_out2, *biquad_mem1, *biquad_mem2, *FIR_mem1, *FIR_mem2;
    MMshort                             *p_biquad_mem0, *p_biquad_mem1, *p_biquad_mem2, *p_FIR_mem0, *p_FIR_mem1, *p_FIR_mem2;
    int                                 nb_channels;


#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_no_mem_shift_mono[nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_mem_shift_mono[nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

    teq_struct.stereo_config                     = 0;
    teq_struct.stereo_process                    = 0;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.biquad_first                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_8KHZ;
    teq_struct.chan_nb                           = 1;
    teq_struct.offset                            = 1;
    teq_struct.nb_alloc_biquad_cells_per_channel = 0;
    teq_struct.nb_biquad_cells_per_channel       = 0;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = nb_FIR_coefs_per_channel;
    teq_struct.nb_FIR_coefs_per_channel          = nb_FIR_coefs_per_channel;
    if(init_teq(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_8k,
                (t_FIR_filter    *) NULL) == -1)
    {
        return;
    }

    init_memcmp(&teq_struct, &biquad_mem1, &biquad_mem2, &FIR_mem1, &FIR_mem2, &samples_in, &samples_out1, &samples_out2, NB_SAMPLES_PER_CHANNEL);
    p_biquad_mem0 = teq_struct.p_biquad_mem;
    p_biquad_mem1 = biquad_mem1;
    p_biquad_mem2 = biquad_mem2;
    p_FIR_mem0    = teq_struct.p_FIR_mem;
    p_FIR_mem1    = FIR_mem1;
    p_FIR_mem2    = FIR_mem2;

    nb_channels             = (teq_struct.stereo_config ? 2 : 1);
    teq_struct.p_biquad_mem = p_biquad_mem1;
    teq_struct.p_FIR_mem    = p_FIR_mem1;
    transducer_equalizer_filter_no_optim(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                         samples_in,
                                         samples_out1,
                                         &teq_struct);


    teq_struct.p_biquad_mem = p_biquad_mem2;
    teq_struct.p_FIR_mem    = p_FIR_mem2;
#ifdef TEQ_CA9_NEON_PROFILING
    startperf();
#endif /* TEQ_CA9_NEON_PROFILING */
    transducer_equalizer_filter(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                samples_in,
                                samples_out2,
                                &teq_struct);
#ifdef TEQ_CA9_NEON_PROFILING
    endperf();
#endif /* TEQ_CA9_NEON_PROFILING */

    verif(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2 + teq_struct.FIR_index, samples_out1, samples_out2, NB_SAMPLES_PER_CHANNEL);

    teq_struct.p_biquad_mem = p_biquad_mem0;
    teq_struct.p_FIR_mem    = p_FIR_mem0;
    close_teq(&teq_struct);
    free_memcmp(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2, samples_in, samples_out1, samples_out2);
}

static void test_stereo_same_FIR(int nb_FIR_coefs_per_channel)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T teq_struct;
    MMshort                             *samples_in, *samples_out1, *samples_out2, *biquad_mem1, *biquad_mem2, *FIR_mem1, *FIR_mem2;
    MMshort                             *p_biquad_mem0, *p_biquad_mem1, *p_biquad_mem2, *p_FIR_mem0, *p_FIR_mem1, *p_FIR_mem2;
    int                                 nb_channels;


#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_no_mem_shift_stereo1[nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_mem_shift_stereo1[nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

    teq_struct.stereo_config                     = 1;
    teq_struct.stereo_process                    = 1;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_44_1KHZ;
    teq_struct.chan_nb                           = 2;
    teq_struct.offset                            = 2;
    teq_struct.nb_alloc_biquad_cells_per_channel = 0;
    teq_struct.nb_biquad_cells_per_channel       = 0;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = nb_FIR_coefs_per_channel;
    teq_struct.nb_FIR_coefs_per_channel          = nb_FIR_coefs_per_channel;
    if(init_teq(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_low44k,
                (t_FIR_filter    *) NULL) == -1)
    {
        return;
    }

    init_memcmp(&teq_struct, &biquad_mem1, &biquad_mem2, &FIR_mem1, &FIR_mem2, &samples_in, &samples_out1, &samples_out2, NB_SAMPLES_PER_CHANNEL);
    p_biquad_mem0 = teq_struct.p_biquad_mem;
    p_biquad_mem1 = biquad_mem1;
    p_biquad_mem2 = biquad_mem2;
    p_FIR_mem0    = teq_struct.p_FIR_mem;
    p_FIR_mem1    = FIR_mem1;
    p_FIR_mem2    = FIR_mem2;

    nb_channels             = (teq_struct.stereo_config ? 2 : 1);
    teq_struct.p_biquad_mem = p_biquad_mem1;
    teq_struct.p_FIR_mem    = p_FIR_mem1;
    transducer_equalizer_filter_no_optim(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                         samples_in,
                                         samples_out1,
                                         &teq_struct);


    teq_struct.p_biquad_mem = p_biquad_mem2;
    teq_struct.p_FIR_mem    = p_FIR_mem2;
#ifdef TEQ_CA9_NEON_PROFILING
    startperf();
#endif /* TEQ_CA9_NEON_PROFILING */
    transducer_equalizer_filter(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                samples_in,
                                samples_out2,
                                &teq_struct);
#ifdef TEQ_CA9_NEON_PROFILING
    endperf();
#endif /* TEQ_CA9_NEON_PROFILING */

    verif(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2 + teq_struct.FIR_index, samples_out1, samples_out2, NB_SAMPLES_PER_CHANNEL);

    teq_struct.p_biquad_mem = p_biquad_mem0;
    teq_struct.p_FIR_mem    = p_FIR_mem0;
    close_teq(&teq_struct);
    free_memcmp(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2, samples_in, samples_out1, samples_out2);
}

static void test_stereo_different_FIR(int nb_FIR_coefs_per_channel)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T teq_struct;
    MMshort                             *samples_in, *samples_out1, *samples_out2, *biquad_mem1, *biquad_mem2, *FIR_mem1, *FIR_mem2;
    MMshort                             *p_biquad_mem0, *p_biquad_mem1, *p_biquad_mem2, *p_FIR_mem0, *p_FIR_mem1, *p_FIR_mem2;
    int                                 nb_channels;


#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_no_mem_shift_stereo2[nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_mem_shift_stereo2[nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

    teq_struct.stereo_config                     = 1;
    teq_struct.stereo_process                    = 1;
    teq_struct.same_biquad_l_r                   = 0;
    teq_struct.same_FIR_l_r                      = 0;
    teq_struct.sample_freq                       = ESAA_FREQ_44_1KHZ;
    teq_struct.chan_nb                           = 2;
    teq_struct.offset                            = 2;
    teq_struct.nb_alloc_biquad_cells_per_channel = 0;
    teq_struct.nb_biquad_cells_per_channel       = 0;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = nb_FIR_coefs_per_channel;
    teq_struct.nb_FIR_coefs_per_channel          = nb_FIR_coefs_per_channel;
    if(init_teq(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_low44k,
                (t_FIR_filter    *) &FIR_filter_high44k) == -1)
    {
        return;
    }

    init_memcmp(&teq_struct, &biquad_mem1, &biquad_mem2, &FIR_mem1, &FIR_mem2, &samples_in, &samples_out1, &samples_out2, NB_SAMPLES_PER_CHANNEL);
    p_biquad_mem0 = teq_struct.p_biquad_mem;
    p_biquad_mem1 = biquad_mem1;
    p_biquad_mem2 = biquad_mem2;
    p_FIR_mem0    = teq_struct.p_FIR_mem;
    p_FIR_mem1    = FIR_mem1;
    p_FIR_mem2    = FIR_mem2;

    nb_channels             = (teq_struct.stereo_config ? 2 : 1);
    teq_struct.p_biquad_mem = p_biquad_mem1;
    teq_struct.p_FIR_mem    = p_FIR_mem1;
    transducer_equalizer_filter_no_optim(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                         samples_in,
                                         samples_out1,
                                         &teq_struct);


    teq_struct.p_biquad_mem = p_biquad_mem2;
    teq_struct.p_FIR_mem    = p_FIR_mem2;
#ifdef TEQ_CA9_NEON_PROFILING
    startperf();
#endif /* TEQ_CA9_NEON_PROFILING */
    transducer_equalizer_filter(nb_channels * NB_SAMPLES_PER_CHANNEL,
                                samples_in,
                                samples_out2,
                                &teq_struct);
#ifdef TEQ_CA9_NEON_PROFILING
    endperf();
#endif /* TEQ_CA9_NEON_PROFILING */

    verif(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2 + teq_struct.FIR_index, samples_out1, samples_out2, NB_SAMPLES_PER_CHANNEL);

    teq_struct.p_biquad_mem = p_biquad_mem0;
    teq_struct.p_FIR_mem    = p_FIR_mem0;
    close_teq(&teq_struct);
    free_memcmp(&teq_struct, biquad_mem1, biquad_mem2, FIR_mem1, FIR_mem2, samples_in, samples_out1, samples_out2);
}

#endif /* TEQ_CA9_VERIF_OPTIM_NEON */

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TEQ_CA9_TEST

#define BUFFER_SAMPLES_SIZE 65536
static MMshort buffer_samples1[BUFFER_SAMPLES_SIZE], buffer_samples2[BUFFER_SAMPLES_SIZE];


#ifdef TEQ_CA9_BOARD_TEST
static int compare_samples(char *p_test_name, int offset, int nb_samples, int nb_samples_ref, int nb_samples_out, const short *p_reference_samples, MMshort *p_samples_out)
#else /* TEQ_CA9_BOARD_TEST */
static int compare_samples(char *p_test_name, char *p_reference_file_name, char *p_output_file_name)
#endif /* TEQ_CA9_BOARD_TEST */
{
    static int nb_diff = 0, first_diff = -1;
    int        i, j, n;
#ifdef TEQ_CA9_BOARD_TEST
    if(offset == 0)
    {
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\nPCM sample compare (%s) :\n", p_test_name);
        Tr_EQ_test_buffer_printf_flush();
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "reference : %d samples\n", nb_samples_ref);
        Tr_EQ_test_buffer_printf_flush();
        Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "output : %d samples\n", nb_samples_out);
        Tr_EQ_test_buffer_printf_flush();
    }
    n = min(nb_samples_ref, nb_samples_out);
    nb_samples = min(nb_samples, n - offset);
#else /* TEQ_CA9_BOARD_TEST */
    int     nb_samples, nb_samples_ref, nb_samples_out, offset = 0;
    MMshort *p_samples_ref, *p_samples_out;
    FILE    *p_sample_ref_file, *p_sample_out_file;


    p_sample_ref_file = fopen(p_reference_file_name, "rb");
    if(p_sample_ref_file == NULL)
    {
        fprintf(stderr, "can't open \"%s\" reference file\n", p_reference_file_name);
        return -1;
    }
    fseek(p_sample_ref_file, 0, SEEK_END);
    nb_samples_ref = ftell(p_sample_ref_file) / 2;
    rewind(p_sample_ref_file);

    p_sample_out_file = fopen(p_output_file_name, "rb");
    if(p_sample_out_file == NULL)
    {
        fprintf(stderr, "can't open \"%s\" output file\n", p_output_file_name);
        fclose(p_sample_ref_file);
        return -1;
    }
    fseek(p_sample_out_file, 0, SEEK_END);
    nb_samples_out = ftell(p_sample_out_file) / 2;
    rewind(p_sample_out_file);

    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "\nPCM file compare (%s) :\n", p_test_name);
    Tr_EQ_test_buffer_printf_flush();
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "%s : %d samples\n", p_reference_file_name, nb_samples_ref);
    Tr_EQ_test_buffer_printf_flush();
    Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "%s : %d samples\n", p_output_file_name,    nb_samples_out);
    Tr_EQ_test_buffer_printf_flush();

    nb_samples = min(nb_samples_ref, nb_samples_out);
    if(nb_samples == 0)
    {
        fclose(p_sample_ref_file);
        fclose(p_sample_out_file);
        return -1;
    }
#endif /* TEQ_CA9_BOARD_TEST */

#ifndef TEQ_CA9_BOARD_TEST
    p_samples_ref = buffer_samples1;
    p_samples_out = buffer_samples2;
#endif /* !TEQ_CA9_BOARD_TEST */
    for(i = 0; i < nb_samples; i += n)
    {
        n = min(nb_samples - i, BUFFER_SAMPLES_SIZE);
#ifndef TEQ_CA9_BOARD_TEST
        read_samples(p_sample_ref_file, p_samples_ref, n);
        read_samples(p_sample_out_file, p_samples_out, n);
#endif /* !TEQ_CA9_BOARD_TEST */
        for(j = 0; j < n; j++)
        {
            int sample_ref, sample_out;

#ifdef TEQ_CA9_BOARD_TEST
            sample_ref = (int) p_reference_samples[offset + i + j];
            sample_out = p_samples_out[i + j];
            if(sample_out < 0x7FFF8000)
            {
                sample_out = ((sample_out + (1 << 15)) >> 16);
            }
            else
            {
                sample_out = 0x7FFF;
            }
#else /* TEQ_CA9_BOARD_TEST */
            sample_ref = (p_samples_ref[j] >> 16) & 0xFFFF;
            sample_out = (p_samples_out[j] >> 16) & 0xFFFF;
#endif /* TEQ_CA9_BOARD_TEST */

            if(sample_ref != sample_out)
            {
#ifdef TEQ_CA9_PRINT_ALL_DIFF
                Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf,
                                                 "sample %7d : ref = 0x%04X / out = 0x%04X / diff = 0x%04X\n",
                                                 i + j,
                                                 sample_ref,
                                                 sample_out,
                                                 (sample_ref - sample_out) & 0xFFFF);
                Tr_EQ_test_buffer_printf_flush();
#endif /* TEQ_CA9_PRINT_ALL_DIFF */
                nb_diff++;
                if(first_diff == -1)
                {
                    first_diff = offset + i + j;
                }
            }
        }
    }
#ifdef TEQ_CA9_BOARD_TEST
    nb_samples = min(nb_samples_ref, nb_samples_out);
    if(offset + i == nb_samples)
#endif /* TEQ_CA9_BOARD_TEST */
    {
        if(nb_diff == 0)
        {
            Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "%d samples identical\n", nb_samples);
        }
        else
        {
            Tr_EQ_p_buffer_printf += sprintf(Tr_EQ_p_buffer_printf, "first %d samples identical, then %d differences out of %d remaining samples\n", first_diff, nb_diff, nb_samples - first_diff);
        }
        Tr_EQ_test_buffer_printf_flush();
    }
#ifndef TEQ_CA9_BOARD_TEST
    fclose(p_sample_ref_file);
    fclose(p_sample_out_file);
#endif /* !TEQ_CA9_BOARD_TEST */

    return 0;
}

#ifdef TEQ_CA9_BOARD_TEST
static int test_filter(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_teq_struct,
                       t_biquad_filter                     *p_biquad_filter1,
                       t_biquad_filter                     *p_biquad_filter2,
                       t_FIR_filter                        *p_FIR_filter1,
                       t_FIR_filter                        *p_FIR_filter2,
                       char                                *p_test_name,
                       const int                           nb_samples_in,
                       const short                         *p_sample_table_in,
                       const int                           nb_samples_ref,
                       const short                         *p_sample_table_ref)
#else /* TEQ_CA9_BOARD_TEST */
static int test_filter(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *p_teq_struct,
                       t_biquad_filter                     *p_biquad_filter1,
                       t_biquad_filter                     *p_biquad_filter2,
                       t_FIR_filter                        *p_FIR_filter1,
                       t_FIR_filter                        *p_FIR_filter2,
                       char                                *p_test_name,
                       char                                *p_input_file_name,
                       char                                *p_output_file_name,
                       char                                *p_reference_file_name)
#endif /* TEQ_CA9_BOARD_TEST */
{
    int     i, n, nb_samples, ret = 0;
    MMshort *p_samples_in, *p_samples_out;
#ifdef TEQ_CA9_BOARD_TEST
    int     nb_samples_out;
#else /* TEQ_CA9_BOARD_TEST */
    FILE    *p_sample_in_file, *p_sample_out_file;
    char    *p_input_path_file_name, *p_reference_path_file_name;


    p_input_path_file_name = (char *) malloc(strlen(TEQ_STREAMS_PATH) + strlen(p_input_file_name) + 1);
    if(p_input_path_file_name == NULL)
    {
        fprintf(stderr, "can't allocate p_input_path_file_name\n");
        return -1;
    }
    strcpy(p_input_path_file_name, TEQ_STREAMS_PATH);
    strcat(p_input_path_file_name, p_input_file_name);
    p_input_file_name = p_input_path_file_name;

    p_reference_path_file_name = (char *) malloc(strlen(TEQ_STREAMS_PATH) + strlen(p_reference_file_name) + 1);
    if(p_reference_path_file_name == NULL)
    {
        fprintf(stderr, "can't allocate p_reference_path_file_name\n");
        free(p_input_path_file_name);
        return -1;
    }
    strcpy(p_reference_path_file_name, TEQ_STREAMS_PATH);
    strcat(p_reference_path_file_name, p_reference_file_name);
    p_reference_file_name = p_reference_path_file_name;
#endif /* TEQ_CA9_BOARD_TEST */

    if(init_teq(p_teq_struct,
                p_biquad_filter1,
                p_biquad_filter2,
                p_FIR_filter1,
                p_FIR_filter2) == -1)
    {
#ifndef TEQ_CA9_BOARD_TEST
        free(p_input_path_file_name);
        free(p_reference_path_file_name);
#endif /* !TEQ_CA9_BOARD_TEST */
        return -1;
    }

    p_samples_in  = buffer_samples1;
    p_samples_out = buffer_samples2;

#ifdef TEQ_CA9_BOARD_TEST
    nb_samples_out = nb_samples_in;
    nb_samples     = min(nb_samples_in, nb_samples_ref);

#ifdef TEQ_CA9_NEON_PROFILING
    startperf();
#endif /* TEQ_CA9_NEON_PROFILING */
    for(i = 0; i < nb_samples; i += n)
    {
        n = min(nb_samples - i, BUFFER_SAMPLES_SIZE);
        read_samples(p_sample_table_in + i, p_samples_in, n);

        transducer_equalizer_filter(n,
                                    p_samples_in,
                                    p_samples_out,
                                    p_teq_struct);

        ret = compare_samples(p_test_name, i, n, nb_samples_ref, nb_samples_out, p_sample_table_ref, p_samples_out);
        if(ret == -1)
        {
            break;
        }
    }
#ifdef TEQ_CA9_NEON_PROFILING
    endperf();
#endif /* TEQ_CA9_NEON_PROFILING */

#else /* TEQ_CA9_BOARD_TEST */
    p_sample_in_file = fopen(p_input_file_name, "rb");
    if(p_sample_in_file == NULL)
    {
        fprintf(stderr, "can't open \"%s\" input file\n", p_input_file_name);
        free(p_input_path_file_name);
        free(p_reference_path_file_name);
        close_teq(p_teq_struct);
        return -1;
    }
    free(p_input_path_file_name);

    p_sample_out_file = fopen(p_output_file_name, "wb");
    if(p_sample_out_file == NULL)
    {
        fprintf(stderr, "can't open \"%s\" output file\n", p_output_file_name);
        free(p_reference_path_file_name);
        close_teq(p_teq_struct);
        return -1;
    }

    fseek(p_sample_in_file, 0, SEEK_END);
    nb_samples = ftell(p_sample_in_file) / 2;
    rewind(p_sample_in_file);

#ifdef TEQ_CA9_NEON_PROFILING
    startperf();
#endif /* TEQ_CA9_NEON_PROFILING */
    for(i = 0; i < nb_samples; i += n)
    {
        n = min(nb_samples - i, BUFFER_SAMPLES_SIZE);
        read_samples(p_sample_in_file, p_samples_in, n);

        transducer_equalizer_filter(n,
                                    p_samples_in,
                                    p_samples_out,
                                    p_teq_struct);

        write_samples(p_sample_out_file, p_samples_out, n);
    }
#ifdef TEQ_CA9_NEON_PROFILING
    endperf();
#endif /* TEQ_CA9_NEON_PROFILING */

    fclose(p_sample_in_file);
    fclose(p_sample_out_file);

    ret = compare_samples(p_test_name, p_reference_file_name, p_output_file_name);
    free(p_reference_path_file_name);
#endif /* TEQ_CA9_BOARD_TEST */

    close_teq(p_teq_struct);

    return ret;
}

static void biquad_test(void)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T teq_struct;

    teq_struct.stereo_config                     = 0;
    teq_struct.stereo_process                    = 0;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.biquad_first                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_8KHZ;
    teq_struct.chan_nb                           = 1;
    teq_struct.offset                            = 1;
    teq_struct.nb_alloc_biquad_cells_per_channel = biquad_filter_8k.nb_biquad_cells;
    teq_struct.nb_biquad_cells_per_channel       = biquad_filter_8k.nb_biquad_cells;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = 0;
    teq_struct.nb_FIR_coefs_per_channel          = 0;

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    assert((teq_struct.nb_biquad_cells_per_channel >= NB_BIQUAD_MIN) && (teq_struct.nb_biquad_cells_per_channel <= NB_BIQUAD_MAX));
    Tr_EQ_reset_stat(&Tr_EQ_stat_biquad_mono[teq_struct.nb_biquad_cells_per_channel - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

#ifdef TEQ_CA9_BOARD_TEST
    test_filter(&teq_struct,
                (t_biquad_filter *) &biquad_filter_8k,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL,
                "biquad mono",
                pcm_in_bourm1_rx_size,
                pcm_in_bourm1_rx,
                pcm_ref_bourm1_rx_biquad_eq_CA9_size,
                pcm_ref_bourm1_rx_biquad_eq_CA9);
#else /* TEQ_CA9_BOARD_TEST */
    test_filter(&teq_struct,
                (t_biquad_filter *) &biquad_filter_8k,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL,
                "biquad mono",
                "signals/bourm1_rx.pcm",
                "bourm1_rx_biquad_eq_CA9.pcm",
                "reference/bourm1_rx_biquad_eq_CA9.pcm");
#endif /* TEQ_CA9_BOARD_TEST */

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_display_stat("mono biquad", teq_struct.nb_biquad_cells_per_channel, &Tr_EQ_stat_biquad_mono[teq_struct.nb_biquad_cells_per_channel - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    Tr_EQ_buffer_printf_flush();


    teq_struct.stereo_config                     = 1;
    teq_struct.stereo_process                    = 1;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.biquad_first                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_44_1KHZ;
    teq_struct.chan_nb                           = 2;
    teq_struct.offset                            = 2;
    teq_struct.nb_alloc_biquad_cells_per_channel = biquad_filter_low44k.nb_biquad_cells;
    teq_struct.nb_biquad_cells_per_channel       = biquad_filter_low44k.nb_biquad_cells;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = 0;
    teq_struct.nb_FIR_coefs_per_channel          = 0;

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    assert((teq_struct.nb_biquad_cells_per_channel >= NB_BIQUAD_MIN) && (teq_struct.nb_biquad_cells_per_channel <= NB_BIQUAD_MAX));
    Tr_EQ_reset_stat(&Tr_EQ_stat_biquad_stereo1[teq_struct.nb_biquad_cells_per_channel - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

#ifdef TEQ_CA9_BOARD_TEST
    test_filter(&teq_struct,
                (t_biquad_filter *) &biquad_filter_low44k,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL,
                "biquad stereo L=R",
                pcm_in_AirBatucada_44k_size,
                pcm_in_AirBatucada_44k,
                pcm_ref_AirBatucada_44k_biquad_eq1_CA9_size,
                pcm_ref_AirBatucada_44k_biquad_eq1_CA9);
#else /* TEQ_CA9_BOARD_TEST */
    test_filter(&teq_struct,
                (t_biquad_filter *) &biquad_filter_low44k,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL,
                "biquad stereo L=R",
                "signals/AirBatucada_44k.pcm",
                "AirBatucada_44k_biquad_eq1_CA9.pcm",
                "reference/AirBatucada_44k_biquad_eq1_CA9.pcm");
#endif /* TEQ_CA9_BOARD_TEST */

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_display_stat("stereo biquad (L=R)", teq_struct.nb_biquad_cells_per_channel, &Tr_EQ_stat_biquad_stereo1[teq_struct.nb_biquad_cells_per_channel - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    Tr_EQ_buffer_printf_flush();


    teq_struct.stereo_config                     = 1;
    teq_struct.stereo_process                    = 1;
    teq_struct.same_biquad_l_r                   = 0;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.biquad_first                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_44_1KHZ;
    teq_struct.chan_nb                           = 2;
    teq_struct.offset                            = 2;
    teq_struct.nb_alloc_biquad_cells_per_channel = max(biquad_filter_low44k.nb_biquad_cells, biquad_filter_high44k.nb_biquad_cells);
    teq_struct.nb_biquad_cells_per_channel       = teq_struct.nb_alloc_biquad_cells_per_channel;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = 0;
    teq_struct.nb_FIR_coefs_per_channel          = 0;

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    assert((teq_struct.nb_biquad_cells_per_channel >= NB_BIQUAD_MIN) && (teq_struct.nb_biquad_cells_per_channel <= NB_BIQUAD_MAX));
    Tr_EQ_reset_stat(&Tr_EQ_stat_biquad_stereo2[teq_struct.nb_biquad_cells_per_channel - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

#ifdef TEQ_CA9_BOARD_TEST
    test_filter(&teq_struct,
                (t_biquad_filter *) &biquad_filter_low44k,
                (t_biquad_filter *) &biquad_filter_high44k,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL,
                "biquad stereo L!=R",
                pcm_in_AirBatucada_44k_size,
                pcm_in_AirBatucada_44k,
                pcm_ref_AirBatucada_44k_biquad_eq2_CA9_size,
                pcm_ref_AirBatucada_44k_biquad_eq2_CA9);
#else /* TEQ_CA9_BOARD_TEST */
    test_filter(&teq_struct,
                (t_biquad_filter *) &biquad_filter_low44k,
                (t_biquad_filter *) &biquad_filter_high44k,
                (t_FIR_filter    *) NULL,
                (t_FIR_filter    *) NULL,
                "biquad stereo L!=R",
                "signals/AirBatucada_44k.pcm",
                "AirBatucada_44k_biquad_eq2_CA9.pcm",
                "reference/AirBatucada_44k_biquad_eq2_CA9.pcm");
#endif /* TEQ_CA9_BOARD_TEST */

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_display_stat("stereo biquad (L!=R)", teq_struct.nb_biquad_cells_per_channel, &Tr_EQ_stat_biquad_stereo2[teq_struct.nb_biquad_cells_per_channel - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    Tr_EQ_buffer_printf_flush();
}

static void FIR_test(void)
{
    TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T teq_struct;

    teq_struct.stereo_config                     = 0;
    teq_struct.stereo_process                    = 0;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.biquad_first                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_8KHZ;
    teq_struct.chan_nb                           = 1;
    teq_struct.offset                            = 1;
    teq_struct.nb_alloc_biquad_cells_per_channel = 0;
    teq_struct.nb_biquad_cells_per_channel       = 0;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = FIR_filter_8k.nb_FIR_coefs;
    teq_struct.nb_FIR_coefs_per_channel          = FIR_filter_8k.nb_FIR_coefs;

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    assert((teq_struct.nb_FIR_coefs_per_channel >= NB_FIR_COEF_MIN) && (teq_struct.nb_FIR_coefs_per_channel <= NB_FIR_COEF_MAX));
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_no_mem_shift_mono[teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_mem_shift_mono   [teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

#ifdef TEQ_CA9_BOARD_TEST
    test_filter(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_8k,
                (t_FIR_filter    *) NULL,
                "FIR mono",
                pcm_in_bourm1_rx_size,
                pcm_in_bourm1_rx,
                pcm_ref_bourm1_rx_FIR_eq_CA9_size,
                pcm_ref_bourm1_rx_FIR_eq_CA9);
#else /* TEQ_CA9_BOARD_TEST */
    test_filter(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_8k,
                (t_FIR_filter    *) NULL,
                "FIR mono",
                "signals/bourm1_rx.pcm",
                "bourm1_rx_FIR_eq_CA9.pcm",
                "reference/bourm1_rx_FIR_eq_CA9.pcm");
#endif /* TEQ_CA9_BOARD_TEST */

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    assert((teq_struct.nb_FIR_coefs_per_channel >= NB_FIR_COEF_MIN) && (teq_struct.nb_FIR_coefs_per_channel <= NB_FIR_COEF_MAX));
    Tr_EQ_display_stat("mono FIR without mem shift", teq_struct.nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_no_mem_shift_mono[teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_display_stat("mono FIR with mem shift",    teq_struct.nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_mem_shift_mono   [teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    Tr_EQ_buffer_printf_flush();


    teq_struct.stereo_config                     = 1;
    teq_struct.stereo_process                    = 1;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 1;
    teq_struct.biquad_first                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_44_1KHZ;
    teq_struct.chan_nb                           = 2;
    teq_struct.offset                            = 2;
    teq_struct.nb_alloc_biquad_cells_per_channel = 0;
    teq_struct.nb_biquad_cells_per_channel       = 0;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = FIR_filter_low44k.nb_FIR_coefs;
    teq_struct.nb_FIR_coefs_per_channel          = FIR_filter_low44k.nb_FIR_coefs;

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_no_mem_shift_stereo1[teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_mem_shift_stereo1   [teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

#ifdef TEQ_CA9_BOARD_TEST
    test_filter(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_low44k,
                (t_FIR_filter    *) NULL,
                "FIR stereo L=R",
                pcm_in_AirBatucada_44k_size,
                pcm_in_AirBatucada_44k,
                pcm_ref_AirBatucada_44k_FIR_eq1_CA9_size,
                pcm_ref_AirBatucada_44k_FIR_eq1_CA9);
#else /* TEQ_CA9_BOARD_TEST */
    test_filter(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_low44k,
                (t_FIR_filter    *) NULL,
                "FIR stereo L=R",
                "signals/AirBatucada_44k.pcm",
                "AirBatucada_44k_FIR_eq1_CA9.pcm",
                "reference/AirBatucada_44k_FIR_eq1_CA9.pcm");
#endif /* TEQ_CA9_BOARD_TEST */

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    assert((teq_struct.nb_FIR_coefs_per_channel >= NB_FIR_COEF_MIN) && (teq_struct.nb_FIR_coefs_per_channel <= NB_FIR_COEF_MAX));
    Tr_EQ_display_stat("stereo FIR (L=R) without mem shift", teq_struct.nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_no_mem_shift_stereo1[teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_display_stat("stereo FIR (L=R) with mem shift",    teq_struct.nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_mem_shift_stereo1   [teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    Tr_EQ_buffer_printf_flush();


    teq_struct.stereo_config                     = 1;
    teq_struct.stereo_process                    = 1;
    teq_struct.same_biquad_l_r                   = 1;
    teq_struct.same_FIR_l_r                      = 0;
    teq_struct.biquad_first                      = 1;
    teq_struct.sample_freq                       = ESAA_FREQ_44_1KHZ;
    teq_struct.chan_nb                           = 2;
    teq_struct.offset                            = 2;
    teq_struct.nb_alloc_biquad_cells_per_channel = 0;
    teq_struct.nb_biquad_cells_per_channel       = 0;
    teq_struct.nb_alloc_FIR_coefs_per_channel    = max(FIR_filter_low44k.nb_FIR_coefs, FIR_filter_high44k.nb_FIR_coefs);
    teq_struct.nb_FIR_coefs_per_channel          = teq_struct.nb_alloc_FIR_coefs_per_channel;

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_no_mem_shift_stereo2[teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_reset_stat(&Tr_EQ_stat_FIR_mem_shift_stereo2   [teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */

#ifdef TEQ_CA9_BOARD_TEST
    test_filter(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_low44k,
                (t_FIR_filter    *) &FIR_filter_high44k,
                "FIR stereo L!=R",
                pcm_in_AirBatucada_44k_size,
                pcm_in_AirBatucada_44k,
                pcm_ref_AirBatucada_44k_FIR_eq2_CA9_size,
                pcm_ref_AirBatucada_44k_FIR_eq2_CA9);
#else /* TEQ_CA9_BOARD_TEST */
    test_filter(&teq_struct,
                (t_biquad_filter *) NULL,
                (t_biquad_filter *) NULL,
                (t_FIR_filter    *) &FIR_filter_low44k,
                (t_FIR_filter    *) &FIR_filter_high44k,
                "FIR stereo L!=R",
                "signals/AirBatucada_44k.pcm",
                "AirBatucada_44k_FIR_eq2_CA9.pcm",
                "reference/AirBatucada_44k_FIR_eq2_CA9.pcm");
#endif /* TEQ_CA9_BOARD_TEST */

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    Tr_EQ_display_stat("stereo FIR (L!=R) without mem shift", teq_struct.nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_no_mem_shift_stereo2[teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
    Tr_EQ_display_stat("stereo FIR (L!=R with mem shift)",    teq_struct.nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_mem_shift_stereo2   [teq_struct.nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    Tr_EQ_buffer_printf_flush();
}
#endif /* TEQ_CA9_TEST */

////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
#ifdef TEQ_CA9_VERIF_OPTIM_NEON
    int nb_biquads_per_channel, nb_FIR_coefs_per_channel;
#endif /* TEQ_CA9_VERIF_OPTIM_NEON */


#ifdef __arm
    core_init();

#ifdef TEQ_CA9_PRINT_NEON_CYCLES
    PerfEnableDisable(1);
    PerfSetTickRate(0);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
#endif /* __arm */

    Tr_EQ_open_buffer_printf();

#ifdef TEQ_CA9_VERIF_OPTIM_NEON
    for(nb_biquads_per_channel = NB_BIQUAD_MIN; nb_biquads_per_channel <= NB_BIQUAD_MAX; nb_biquads_per_channel++)
    {
        test_mono_biquad            (nb_biquads_per_channel);
        test_stereo_same_biquad     (nb_biquads_per_channel);
        test_stereo_different_biquad(nb_biquads_per_channel);
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        Tr_EQ_display_stat("mono biquad",          nb_biquads_per_channel, &Tr_EQ_stat_biquad_mono   [nb_biquads_per_channel - NB_BIQUAD_MIN]);
        Tr_EQ_display_stat("stereo biquad (L=R)",  nb_biquads_per_channel, &Tr_EQ_stat_biquad_stereo1[nb_biquads_per_channel - NB_BIQUAD_MIN]);
        Tr_EQ_display_stat("stereo biquad (L!=R)", nb_biquads_per_channel, &Tr_EQ_stat_biquad_stereo2[nb_biquads_per_channel - NB_BIQUAD_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }

    for(nb_FIR_coefs_per_channel = NB_FIR_COEF_MIN; nb_FIR_coefs_per_channel <= NB_FIR_COEF_MAX; nb_FIR_coefs_per_channel++)
    {
        test_mono_FIR            (nb_FIR_coefs_per_channel);
        test_stereo_same_FIR     (nb_FIR_coefs_per_channel);
        test_stereo_different_FIR(nb_FIR_coefs_per_channel);
#ifdef TEQ_CA9_PRINT_NEON_CYCLES
        Tr_EQ_display_stat("mono FIR without mem shift",          nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_no_mem_shift_mono   [nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
        Tr_EQ_display_stat("stereo FIR (L=R) without mem shift",  nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_no_mem_shift_stereo1[nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
        Tr_EQ_display_stat("stereo FIR (L!=R) without mem shift", nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_no_mem_shift_stereo2[nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
        Tr_EQ_display_stat("mono FIR with mem shift",             nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_mem_shift_mono      [nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
        Tr_EQ_display_stat("stereo FIR (L=R) with mem shift",     nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_mem_shift_stereo1   [nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
        Tr_EQ_display_stat("stereo FIR (L!=R with mem shift)",    nb_FIR_coefs_per_channel, &Tr_EQ_stat_FIR_mem_shift_stereo2   [nb_FIR_coefs_per_channel - NB_FIR_COEF_MIN]);
#endif /* TEQ_CA9_PRINT_NEON_CYCLES */
    }
#endif /* TEQ_CA9_VERIF_OPTIM_NEON */

#ifdef TEQ_CA9_TEST
    biquad_test();
    FIR_test();
#endif /* TEQ_CA9_TEST */

    Tr_EQ_close_buffer_printf();

    return 0;
}

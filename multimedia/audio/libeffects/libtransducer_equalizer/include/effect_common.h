#ifndef _transducer_equalizer_common__utils_h_
#define _transducer_equalizer_common__utils_h_

#define NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS 40
#define NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS    400

typedef struct
{
    int                                 new_enable;
    int                                 new_biquad_first;
    int                                 new_same_biquad_l_r;
    MMshort                             new_biquad_gain_exp_l;
    MMshort                             new_biquad_gain_mant_l;
    MMshort                             new_biquad_gain_exp_r;
    MMshort                             new_biquad_gain_mant_r;
    MMshort                             new_nb_biquad_cells_per_channel;
    union
    {
        t_biquad_cell                   biquad_cell   [NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS];
        t_biquad_cell_st                biquad_cell_st[NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS / 2];
    } new_biquad_cells;
    int                                 old_same_FIR_l_r;
    MMshort                             old_FIR_gain_exp_l;
    MMshort                             old_FIR_gain_mant_l;
    MMshort                             old_FIR_gain_exp_r;
    MMshort                             old_FIR_gain_mant_r;
    MMshort                             old_nb_FIR_coefs_per_channel;
    MMshort                             old_FIR_coef[NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS];
    int                                 new_same_FIR_l_r;
    MMshort                             new_FIR_gain_exp_l;
    MMshort                             new_FIR_gain_mant_l;
    MMshort                             new_FIR_gain_exp_r;
    MMshort                             new_FIR_gain_mant_r;
    MMshort                             new_nb_FIR_coefs_per_channel;
    MMshort                             new_FIR_coef[NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS];
    MMshort                             save_new_FIR_coef[NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS];

    int                                 smooth_FIR_transition_duration;
    int                                 smooth_FIR_transition_counter;
    MMshort                             old_average_gain_exp_l;
    MMshort                             old_average_gain_mant_l;
    MMshort                             old_average_gain_exp_r;
    MMshort                             old_average_gain_mant_r;
    MMshort                             new_average_gain_exp_l;
    MMshort                             new_average_gain_mant_l;
    MMshort                             new_average_gain_exp_r;
    MMshort                             new_average_gain_mant_r;
    MMshort                             transition_gain_exp_l;
    MMshort                             transition_gain_mant_l;
    MMshort                             transition_gain_exp_r;
    MMshort                             transition_gain_mant_r;
    int                                 smooth_transition_duration1;
    int                                 smooth_transition_duration2;
    int                                 smooth_transition_counter1;
    int                                 smooth_transition_counter2;
    int                                 FIR_smoothing_delay_granularity;
    MMshort                             coef_div_transition1;
    int                                 shift_div_transition1;
    MMshort                             coef_div_transition2;
    int                                 shift_div_transition2;
} transducer_equalizer_smooth_config_t;

#endif // _transducer_equalizer_common__utils_h_




#ifdef TRANSDUCER_EQUALIZER_NMFIL_COMMON_SRC_CODE


#ifdef _NMF_MPC_

#define COMMON_ASSERT        ASSERT
#define COMMON_PRECONDITION  PRECONDITION
#define COMMON_POSTCONDITION POSTCONDITION
#define Q_ARITHMETIC         23
#define GAIN_EXP_INIT        1
#define GAIN_MANT_INIT       0x400000
#define MAX_RATIO            0x7FFFFF

#undef DEBUG_TEQ_SHAI_CONFIG_LEVEL      // impossible on MPC
#define DEBUG_TEQ_SHAI_CONFIG_LEVEL 0   //

#else // _NMF_MPC_

#define COMMON_ASSERT        ARMNMF_DBC_ASSERT
#define COMMON_PRECONDITION  ARMNMF_DBC_PRECONDITION
#define COMMON_POSTCONDITION ARMNMF_DBC_POSTCONDITION
#define Q_ARITHMETIC         31
#define GAIN_EXP_INIT        1
#define GAIN_MANT_INIT       0x40000000
#define MAX_RATIO            0x7FFFFFFF

static int wedge(int x)
{
    int shift = 0;

    if(x != 0)
    {
        while(1)
        {
            if(((x >> 30) ^ (x >> 31)) & 1)
            {
                break;
            }
            x <<= 1;
            shift++;
        }
    }

    return shift;
}

#endif // _NMF_MPC_

#ifdef  __cplusplus
namespace teq_common_utils {
    MMshort fract_ratio(MMshort num, MMshort den);
    void common_start(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, transducer_equalizer_smooth_config_t *pSmooth_config);
    void common_reset(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct);
    void common_setParameter(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, TransducerEqualizerParams_t *p_StaticParams);
    MMshort common_convert_lowhigh2MMshort(t_uint16 low, t_uint16 high);
    int common_get_mono_biquad_cell(t_transducer_equalizer_biquad_cell_params *p_transducer_equalizer_biquad_cell_params, t_biquad_cell *p_biquad_cell);
    void common_copy_new_config(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, transducer_equalizer_smooth_config_t *pSmooth_config);
    void common_setConfig(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, TransducerEqualizerConfig_t *pTrEqConfig, transducer_equalizer_smooth_config_t *pSmoothConfig);
}
#endif

#ifdef  __cplusplus
MMshort teq_common_utils::fract_ratio(MMshort num, MMshort den)
#else // __cplusplus
static MMshort fract_ratio(MMshort num, MMshort den)
#endif // __cplusplus
{
    if(num >= den)
    {
        return MAX_RATIO;
    }
    else
    {
        return (MMshort) ((((MMlong) num) << Q_ARITHMETIC) / den);
    }
}


#ifdef  __cplusplus
void teq_common_utils::common_start(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, transducer_equalizer_smooth_config_t *pSmooth_config)
#else // __cplusplus
static void common_start(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, transducer_equalizer_smooth_config_t *pSmooth_config)
#endif // __cplusplus
{
    pSmooth_config->smooth_FIR_transition_counter   = 0;
    pSmooth_config->smooth_transition_counter1      = 0;
    pSmooth_config->smooth_transition_counter2      = 0;
    pSmooth_config->old_average_gain_exp_l          = GAIN_EXP_INIT;
    pSmooth_config->old_average_gain_mant_l         = GAIN_MANT_INIT;
    pSmooth_config->old_average_gain_exp_r          = GAIN_EXP_INIT;
    pSmooth_config->old_average_gain_mant_r         = GAIN_MANT_INIT;
    pTrEqStruct->enable                             = FALSE;
    pTrEqStruct->smooth_config_change               = FALSE;
    pTrEqStruct->alloc_done                         = FALSE;
    pTrEqStruct->param_received                     = FALSE;
    pTrEqStruct->config_received                    = FALSE;
#ifdef _NMF_MPC_
    pTrEqStruct->memory_preset                      = MEM_DEFAULT;
#endif // _NMF_MPC_
    pTrEqStruct->stereo_config                     = FALSE;
    pTrEqStruct->stereo_process                    = FALSE;
    pTrEqStruct->same_biquad_l_r                   = TRUE;
    pTrEqStruct->same_FIR_l_r                      = TRUE;
    pTrEqStruct->biquad_first                      = TRUE;
    pTrEqStruct->biquad_gain_exp_l                 = GAIN_EXP_INIT;
    pTrEqStruct->biquad_gain_mant_l                = GAIN_MANT_INIT;
    pTrEqStruct->biquad_gain_exp_r                 = GAIN_EXP_INIT;
    pTrEqStruct->biquad_gain_mant_r                = GAIN_MANT_INIT;
    pTrEqStruct->FIR_gain_exp_l                    = GAIN_EXP_INIT;
    pTrEqStruct->FIR_gain_mant_l                   = GAIN_MANT_INIT;
    pTrEqStruct->FIR_gain_exp_r                    = GAIN_EXP_INIT;
    pTrEqStruct->FIR_gain_mant_r                   = GAIN_MANT_INIT;
    pTrEqStruct->nb_alloc_biquad_cells_per_channel = NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS;
    pTrEqStruct->nb_alloc_FIR_coefs_per_channel    = NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS;
    pTrEqStruct->nb_biquad_cells_per_channel       = 0;
    pTrEqStruct->nb_FIR_coefs_per_channel          = 0;
    pTrEqStruct->sample_freq                       = ESAA_FREQ_UNKNOWNKHZ;
    pTrEqStruct->alloc_chan_nb                     = -1;
    pTrEqStruct->new_chan_nb                       = -1;
    pTrEqStruct->chan_nb                           = -1;
    pTrEqStruct->offset                            = 1;
    pTrEqStruct->p_alloc_biquad_coef               = NULL;
    pTrEqStruct->p_biquad_coef                     = NULL;
    pTrEqStruct->p_biquad_coef_st                  = NULL;
    pTrEqStruct->p_FIR_coef                        = NULL;
    pTrEqStruct->p_biquad_mem                      = (MMshort YMEM *) NULL;
#ifdef __flexcc2__
    pTrEqStruct->p_FIR_mem                         = winit_circ_ymem_ptr((void YMEM *) NULL, (void YMEM *) NULL, (void YMEM *) NULL);
#else /* __flexcc2__ */
    pTrEqStruct->p_FIR_mem                         = NULL;
#endif /* __flexcc2__ */
    pTrEqStruct->first_filter                      = NULL;
    pTrEqStruct->second_filter                     = NULL;
}


#ifdef  __cplusplus
void teq_common_utils::common_reset(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct)
#else // __cplusplus
static void common_reset(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct)
#endif // __cplusplus
{
    int nb_channels = (pTrEqStruct->stereo_config ? 2 : 1);
    int i;


    COMMON_PRECONDITION(pTrEqStruct->alloc_done);

    if(pTrEqStruct->nb_alloc_biquad_cells_per_channel > 0)
    {
        for(i = 0; i < nb_channels * 2 * (pTrEqStruct->nb_alloc_biquad_cells_per_channel + 1); i ++)
        {
            pTrEqStruct->p_biquad_mem[i] = 0;
        }
    }
    if(pTrEqStruct->nb_alloc_FIR_coefs_per_channel > 0)
    {
#ifdef _NMF_MPC_
        for(i = 0; i < nb_channels * pTrEqStruct->nb_alloc_FIR_coefs_per_channel; i ++)
#else // _NMF_MPC_
        for(i = 0; i < nb_channels * (pTrEqStruct->nb_alloc_FIR_coefs_per_channel + FIR_MEM_OVERHEAD); i ++)
#endif // _NMF_MPC_
        {
            pTrEqStruct->p_FIR_mem[i] = 0;
        }
#ifndef _NMF_MPC_
        pTrEqStruct->FIR_index = nb_channels * FIR_MEM_OVERHEAD;
#endif // !_NMF_MPC_
    }
}


#ifdef  __cplusplus
void teq_common_utils::common_setParameter(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, TransducerEqualizerParams_t *p_StaticParams)
#else // __cplusplus
static void common_setParameter(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, TransducerEqualizerParams_t *p_StaticParams)
#endif // __cplusplus
{
    int nb_channels;

    COMMON_PRECONDITION(!pTrEqStruct->param_received);
    COMMON_PRECONDITION(!pTrEqStruct->alloc_done);

    pTrEqStruct->param_received                     = TRUE;
    pTrEqStruct->stereo_config                     = (p_StaticParams->istereo != 0);
    pTrEqStruct->stereo_process                    = pTrEqStruct->stereo_config;
    pTrEqStruct->nb_alloc_biquad_cells_per_channel = p_StaticParams->inb_alloc_biquad_cells_per_channel;
    pTrEqStruct->nb_alloc_FIR_coefs_per_channel    = p_StaticParams->inb_alloc_FIR_coefs_per_channel;

    switch(pTrEqStruct->stereo_config)
    {
        case 0:
            nb_channels = 1;
            break;
        case 1:
            nb_channels = 2;
            break;
        default:
            nb_channels = 0;
            COMMON_ASSERT(FALSE);
            break;
    }

    pTrEqStruct->alloc_chan_nb = pTrEqStruct->offset = nb_channels;
    COMMON_POSTCONDITION(nb_channels * pTrEqStruct->nb_alloc_biquad_cells_per_channel <= NB_MAX_TRANSDUCER_EQUALIZER_BIQUAD_CELLS);
    COMMON_POSTCONDITION(nb_channels * pTrEqStruct->nb_alloc_FIR_coefs_per_channel    <= NB_MAX_TRANSDUCER_EQUALIZER_FIR_COEFS);
}


#ifdef  __cplusplus
MMshort teq_common_utils::common_convert_lowhigh2MMshort(t_uint16 low, t_uint16 high)
#else // __cplusplus
static MMshort common_convert_lowhigh2MMshort(t_uint16 low, t_uint16 high)
#endif // __cplusplus
{
#ifdef __flexcc2__
    MMlong tmp;

    tmp  = ((MMlong) wX_deplsp(low  & 0xFFFF)) << 16;
    tmp |= ((MMlong) wX_deplsp(high & 0xFFFF)) << 32;

    return wround_L(tmp);
#else /* __flexcc2__ */
    return ((MMshort) (low  & 0xFFFF)) | (((MMshort) high) << 16);
#endif /* __flexcc2__ */
}


#ifdef  __cplusplus
int teq_common_utils::common_get_mono_biquad_cell(t_transducer_equalizer_biquad_cell_params *p_transducer_equalizer_biquad_cell_params, t_biquad_cell *p_biquad_cell)
#else // __cplusplus
static int common_get_mono_biquad_cell(t_transducer_equalizer_biquad_cell_params *p_transducer_equalizer_biquad_cell_params, t_biquad_cell *p_biquad_cell)
#endif // __cplusplus
{
    int tmp, idem = TRUE;

#ifdef __flexcc2__
    tmp = (((MMshort) p_transducer_equalizer_biquad_cell_params->b_exp) << 8) >> 8;
#else /* __flexcc2__ */
    tmp = (((MMshort) p_transducer_equalizer_biquad_cell_params->b_exp) << 16) >> 16;
#endif /* __flexcc2__ */
    if(tmp != p_biquad_cell->b_exp)
    {
        p_biquad_cell->b_exp = tmp;
        idem                 = FALSE;
    }
    tmp = common_convert_lowhigh2MMshort(p_transducer_equalizer_biquad_cell_params->b0_low, p_transducer_equalizer_biquad_cell_params->b0_high);
    if(tmp != p_biquad_cell->b0)
    {
        p_biquad_cell->b0    = tmp;
        idem                 = FALSE;
    }
    tmp = common_convert_lowhigh2MMshort(p_transducer_equalizer_biquad_cell_params->b1_low, p_transducer_equalizer_biquad_cell_params->b1_high);
    if(tmp != p_biquad_cell->b1)
    {
        p_biquad_cell->b1    = tmp;
        idem                 = FALSE;
    }
    tmp = common_convert_lowhigh2MMshort(p_transducer_equalizer_biquad_cell_params->b2_low, p_transducer_equalizer_biquad_cell_params->b2_high);
    if(tmp != p_biquad_cell->b2)
    {
        p_biquad_cell->b2    = tmp;
        idem                 = FALSE;
    }
    tmp = common_convert_lowhigh2MMshort(p_transducer_equalizer_biquad_cell_params->a1_low, p_transducer_equalizer_biquad_cell_params->a1_high);
    if(tmp != p_biquad_cell->a1)
    {
        p_biquad_cell->a1    = tmp;
        idem                 = FALSE;
    }
    tmp = common_convert_lowhigh2MMshort(p_transducer_equalizer_biquad_cell_params->a2_low, p_transducer_equalizer_biquad_cell_params->a2_high);
    if(tmp != p_biquad_cell->a2)
    {
        p_biquad_cell->a2    = tmp;
        idem                 = FALSE;
    }

    return idem;
}



#ifdef  __cplusplus
void teq_common_utils::common_copy_new_config(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, transducer_equalizer_smooth_config_t *pSmooth_config)
#else // __cplusplus
static void common_copy_new_config(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, transducer_equalizer_smooth_config_t *pSmooth_config)
#endif // __cplusplus
{
    MMshort *p1, *p2;
    int     nb_channels_FIR, i;


    pTrEqStruct->enable                      = pSmooth_config->new_enable;
    pTrEqStruct->biquad_first                = pSmooth_config->new_biquad_first;
    pTrEqStruct->same_biquad_l_r             = pSmooth_config->new_same_biquad_l_r;
    pTrEqStruct->biquad_gain_exp_l           = pSmooth_config->new_biquad_gain_exp_l;
    pTrEqStruct->biquad_gain_mant_l          = pSmooth_config->new_biquad_gain_mant_l;
    pTrEqStruct->biquad_gain_exp_r           = pSmooth_config->new_biquad_gain_exp_r;
    pTrEqStruct->biquad_gain_mant_r          = pSmooth_config->new_biquad_gain_mant_r;
    pTrEqStruct->same_FIR_l_r                = pSmooth_config->new_same_FIR_l_r;
    pTrEqStruct->FIR_gain_exp_l              = pSmooth_config->new_FIR_gain_exp_l;
    pTrEqStruct->FIR_gain_mant_l             = pSmooth_config->new_FIR_gain_mant_l;
    pTrEqStruct->FIR_gain_exp_r              = pSmooth_config->new_FIR_gain_exp_r;
    pTrEqStruct->FIR_gain_mant_r             = pSmooth_config->new_FIR_gain_mant_r;
    pTrEqStruct->nb_biquad_cells_per_channel = pSmooth_config->new_nb_biquad_cells_per_channel;
    pTrEqStruct->nb_FIR_coefs_per_channel    = pSmooth_config->new_nb_FIR_coefs_per_channel;

    if(pTrEqStruct->enable)
    {
        if(pTrEqStruct->same_biquad_l_r)
        {
            pTrEqStruct->p_biquad_coef_st = (t_biquad_cell_st *) NULL;
            pTrEqStruct->p_biquad_coef    = (t_biquad_cell    *) pTrEqStruct->p_alloc_biquad_coef;
            for(i = 0; i < pTrEqStruct->nb_biquad_cells_per_channel; i++)
            {
                pTrEqStruct->p_biquad_coef[i] = pSmooth_config->new_biquad_cells.biquad_cell[i];
            }
        }
        else
        {
            pTrEqStruct->p_biquad_coef_st = (t_biquad_cell_st *) pTrEqStruct->p_alloc_biquad_coef;
            pTrEqStruct->p_biquad_coef    = (t_biquad_cell    *) NULL;
            for(i = 0; i < pTrEqStruct->nb_biquad_cells_per_channel; i++)
            {
                pTrEqStruct->p_biquad_coef_st[i] = pSmooth_config->new_biquad_cells.biquad_cell_st[i];
            }
        }

        nb_channels_FIR = (pTrEqStruct->stereo_config ? (pTrEqStruct->same_FIR_l_r ? 1 : 2) : 1);
        p1              = pTrEqStruct->p_FIR_coef;
        p2              = pSmooth_config->save_new_FIR_coef;
        for(i = nb_channels_FIR * pTrEqStruct->nb_FIR_coefs_per_channel; i > 0; i--)
        {
            *p1++ = *p2++;
        }
    }
}


#ifdef  __cplusplus
void teq_common_utils::common_setConfig(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, TransducerEqualizerConfig_t *pTrEqConfig, transducer_equalizer_smooth_config_t *pSmoothConfig)
#else // __cplusplus
static void common_setConfig(TRANSDUCER_EQUALIZER_LOCAL_STRUCT_T *pTrEqStruct, TransducerEqualizerConfig_t *pTrEqConfig, transducer_equalizer_smooth_config_t *pSmoothConfig)
#endif // __cplusplus
{
    int i, nb_channels, nb_channels_biquad, nb_channels_FIR, idem_IIR, idem_FIR, shift;


#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
    write0_debug_TEQ_SHAI_file(instance, "setConfig :\n");
    write1_debug_TEQ_SHAI_file(instance, "alloc_done=%d\n", pTrEqStruct->alloc_done);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5

    pTrEqStruct->config_received = TRUE;

    if(pTrEqStruct->alloc_done)
    {
        idem_IIR = idem_FIR = TRUE;

#ifdef __flexcc2__
        pSmoothConfig->new_average_gain_exp_l       = (((MMshort) pTrEqConfig->iaverage_gain_exp_l) << 8) >> 8;
#else /* __flexcc2__ */
        pSmoothConfig->new_average_gain_exp_l       = (((MMshort) pTrEqConfig->iaverage_gain_exp_l) << 16) >> 16;
#endif /* __flexcc2__ */
        pSmoothConfig->new_average_gain_mant_l      = common_convert_lowhigh2MMshort(pTrEqConfig->iaverage_gain_mant_l_low, pTrEqConfig->iaverage_gain_mant_l_high);
        shift                                      = wedge(pSmoothConfig->new_average_gain_mant_l);
        pSmoothConfig->new_average_gain_exp_l      -= shift;
        pSmoothConfig->new_average_gain_mant_l    <<= shift;
#ifdef __flexcc2__
        pSmoothConfig->new_average_gain_exp_r       = (((MMshort) pTrEqConfig->iaverage_gain_exp_r) << 8) >> 8;
#else /* __flexcc2__ */
        pSmoothConfig->new_average_gain_exp_r       = (((MMshort) pTrEqConfig->iaverage_gain_exp_r) << 16) >> 16;
#endif /* __flexcc2__ */
        pSmoothConfig->new_average_gain_mant_r      = common_convert_lowhigh2MMshort(pTrEqConfig->iaverage_gain_mant_r_low, pTrEqConfig->iaverage_gain_mant_r_high);
        shift                                      = wedge(pSmoothConfig->new_average_gain_mant_r);
        pSmoothConfig->new_average_gain_exp_r      -= shift;
        pSmoothConfig->new_average_gain_mant_r    <<= shift;

        pSmoothConfig->old_nb_FIR_coefs_per_channel = pTrEqStruct->nb_FIR_coefs_per_channel;
        pSmoothConfig->old_same_FIR_l_r             = (pTrEqStruct->same_FIR_l_r != 0);
        pSmoothConfig->old_FIR_gain_exp_l           = pTrEqStruct->FIR_gain_exp_l;
        pSmoothConfig->old_FIR_gain_mant_l          = pTrEqStruct->FIR_gain_mant_l;
        shift                                      = wedge(pSmoothConfig->old_FIR_gain_mant_l);
        pSmoothConfig->old_FIR_gain_exp_l          -= shift;
        pSmoothConfig->old_FIR_gain_mant_l        <<= shift;
        if(pSmoothConfig->old_same_FIR_l_r)
        {
            pSmoothConfig->old_FIR_gain_exp_r       = pSmoothConfig->old_FIR_gain_exp_l;
            pSmoothConfig->old_FIR_gain_mant_r      = pSmoothConfig->old_FIR_gain_mant_l;
        }
        else
        {
            pSmoothConfig->old_FIR_gain_exp_r       = pTrEqStruct->FIR_gain_exp_r;
            pSmoothConfig->old_FIR_gain_mant_r      = pTrEqStruct->FIR_gain_mant_r;
            shift                                   = wedge(pSmoothConfig->old_FIR_gain_mant_r);
            pSmoothConfig->old_FIR_gain_exp_r      -= shift;
            pSmoothConfig->old_FIR_gain_mant_r    <<= shift;
        }
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
        write1_debug_TEQ_SHAI_file(instance, "old_same_FIR_l_r=%d\n", pSmoothConfig->old_same_FIR_l_r);
        write2_debug_TEQ_SHAI_file(instance, "old_FIR_gain_mant_l=0x%08X, old_FIR_gain_exp_l=%d\n", pSmoothConfig->old_FIR_gain_mant_l, pSmoothConfig->old_FIR_gain_exp_l);
        write2_debug_TEQ_SHAI_file(instance, "old_FIR_gain_mant_r=0x%08X, old_FIR_gain_exp_r=%d\n", pSmoothConfig->old_FIR_gain_mant_r, pSmoothConfig->old_FIR_gain_exp_r);
        write1_debug_TEQ_SHAI_file(instance, "old_nb_FIR_coefs_per_channel=%d\n", pSmoothConfig->old_nb_FIR_coefs_per_channel);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5

        pSmoothConfig->new_biquad_first = (pTrEqConfig->ibiquad_first != 0);
        if(pSmoothConfig->new_biquad_first != pTrEqStruct->biquad_first)
        {
            idem_IIR = idem_FIR = FALSE;
        }

        if(pTrEqStruct->stereo_config)
        {
            pSmoothConfig->new_same_biquad_l_r = (pTrEqConfig->isame_biquad_l_r != 0);
            if(pSmoothConfig->new_same_biquad_l_r != pTrEqStruct->same_biquad_l_r)
            {
                idem_IIR = FALSE;
            }

            pSmoothConfig->new_same_FIR_l_r = (pTrEqConfig->isame_FIR_l_r != 0);
            if(pSmoothConfig->new_same_FIR_l_r != pSmoothConfig->old_same_FIR_l_r)
            {
                idem_FIR = FALSE;
            }

            nb_channels_FIR = nb_channels_biquad = nb_channels = 2;
            if(pSmoothConfig->new_same_biquad_l_r)
            {
                nb_channels_biquad = 1;
            }
            if(pSmoothConfig->new_same_FIR_l_r)
            {
                nb_channels_FIR = 1;
            }
        }
        else
        {
            pSmoothConfig->new_same_biquad_l_r = pSmoothConfig->new_same_FIR_l_r = TRUE;
            nb_channels_FIR = nb_channels_biquad = nb_channels = 1;
        }

        pSmoothConfig->new_nb_biquad_cells_per_channel = (MMshort) pTrEqConfig->inb_biquad_cells_per_channel;
        if(pSmoothConfig->new_nb_biquad_cells_per_channel != (MMshort) pTrEqStruct->nb_biquad_cells_per_channel)
        {
            idem_IIR = FALSE;
        }
        COMMON_ASSERT(   (pSmoothConfig->new_nb_biquad_cells_per_channel >= 0)
                      && (pSmoothConfig->new_nb_biquad_cells_per_channel <= pTrEqStruct->nb_alloc_biquad_cells_per_channel));

        pSmoothConfig->new_nb_FIR_coefs_per_channel = (MMshort) pTrEqConfig->inb_FIR_coefs_per_channel;
        if(pSmoothConfig->new_nb_FIR_coefs_per_channel != pSmoothConfig->old_nb_FIR_coefs_per_channel)
        {
            idem_FIR = FALSE;
        }
        COMMON_ASSERT((pSmoothConfig->new_nb_FIR_coefs_per_channel >= 0) && (pSmoothConfig->new_nb_FIR_coefs_per_channel <= pTrEqStruct->nb_alloc_FIR_coefs_per_channel));

#ifdef __flexcc2__
        pSmoothConfig->new_biquad_gain_exp_l    = (((MMshort) pTrEqConfig->ibiquad_gain_exp_l) << 8) >> 8;
#else /* __flexcc2__ */
        pSmoothConfig->new_biquad_gain_exp_l    = (((MMshort) pTrEqConfig->ibiquad_gain_exp_l) << 16) >> 16;
#endif /* __flexcc2__ */
        pSmoothConfig->new_biquad_gain_mant_l   = common_convert_lowhigh2MMshort(pTrEqConfig->ibiquad_gain_mant_l_low, pTrEqConfig->ibiquad_gain_mant_l_high);
        shift                                  = wedge(pSmoothConfig->new_biquad_gain_mant_l);
        pSmoothConfig->new_biquad_gain_exp_l   -= shift;
        pSmoothConfig->new_biquad_gain_mant_l <<= shift;
        if(   (pSmoothConfig->new_biquad_gain_mant_l != pTrEqStruct->biquad_gain_mant_l)
           || (pSmoothConfig->new_biquad_gain_exp_l  != pTrEqStruct->biquad_gain_exp_l))
        {
            idem_IIR = FALSE;
        }

        if(pSmoothConfig->new_same_biquad_l_r)
        {
            pSmoothConfig->new_biquad_gain_exp_r  = pSmoothConfig->new_biquad_gain_exp_l;
            pSmoothConfig->new_biquad_gain_mant_r = pSmoothConfig->new_biquad_gain_mant_l;
        }
        else
        {
#ifdef __flexcc2__
            pSmoothConfig->new_biquad_gain_exp_r    = (((MMshort) pTrEqConfig->ibiquad_gain_exp_r) << 8) >> 8;
#else /* __flexcc2__ */
            pSmoothConfig->new_biquad_gain_exp_r    = (((MMshort) pTrEqConfig->ibiquad_gain_exp_r) << 16) >> 16;
#endif /* __flexcc2__ */
            pSmoothConfig->new_biquad_gain_mant_r   = common_convert_lowhigh2MMshort(pTrEqConfig->ibiquad_gain_mant_r_low, pTrEqConfig->ibiquad_gain_mant_r_high);
            shift                                  = wedge(pSmoothConfig->new_biquad_gain_mant_r);
            pSmoothConfig->new_biquad_gain_exp_r   -= shift;
            pSmoothConfig->new_biquad_gain_mant_r <<= shift;
            if(   (pSmoothConfig->new_biquad_gain_mant_r != pTrEqStruct->biquad_gain_mant_r)
               || (pSmoothConfig->new_biquad_gain_exp_r  != pTrEqStruct->biquad_gain_exp_r))
            {
                idem_IIR = FALSE;
            }
        }

#ifdef __flexcc2__
        pSmoothConfig->new_FIR_gain_exp_l    = (((MMshort) pTrEqConfig->iFIR_gain_exp_l) << 8) >> 8;
#else /* __flexcc2__ */
        pSmoothConfig->new_FIR_gain_exp_l    = (((MMshort) pTrEqConfig->iFIR_gain_exp_l) << 16) >> 16;
#endif /* __flexcc2__ */
        pSmoothConfig->new_FIR_gain_mant_l   = common_convert_lowhigh2MMshort(pTrEqConfig->iFIR_gain_mant_l_low, pTrEqConfig->iFIR_gain_mant_l_high);
        shift                               = wedge(pSmoothConfig->new_FIR_gain_mant_l);
        pSmoothConfig->new_FIR_gain_exp_l   -= shift;
        pSmoothConfig->new_FIR_gain_mant_l <<= shift;
        if(   (pSmoothConfig->new_FIR_gain_mant_l != pSmoothConfig->old_FIR_gain_mant_l)
           || (pSmoothConfig->new_FIR_gain_exp_l  != pSmoothConfig->old_FIR_gain_exp_l))
        {
            idem_FIR = FALSE;
        }

        if(pSmoothConfig->new_same_FIR_l_r)
        {
            pSmoothConfig->new_FIR_gain_exp_r  = pSmoothConfig->new_FIR_gain_exp_l;
            pSmoothConfig->new_FIR_gain_mant_r = pSmoothConfig->new_FIR_gain_mant_l;
        }
        else
        {
#ifdef __flexcc2__
            pSmoothConfig->new_FIR_gain_exp_r    = (((MMshort) pTrEqConfig->iFIR_gain_exp_r) << 8) >> 8;
#else /* __flexcc2__ */
            pSmoothConfig->new_FIR_gain_exp_r    = (((MMshort) pTrEqConfig->iFIR_gain_exp_r) << 16) >> 16;
#endif /* __flexcc2__ */
            pSmoothConfig->new_FIR_gain_mant_r   = common_convert_lowhigh2MMshort(pTrEqConfig->iFIR_gain_mant_r_low, pTrEqConfig->iFIR_gain_mant_r_high);
            shift                               = wedge(pSmoothConfig->new_FIR_gain_mant_r);
            pSmoothConfig->new_FIR_gain_exp_r   -= shift;
            pSmoothConfig->new_FIR_gain_mant_r <<= shift;
            if(   (pSmoothConfig->new_FIR_gain_mant_r != pSmoothConfig->old_FIR_gain_mant_r)
               || (pSmoothConfig->new_FIR_gain_exp_r  != pSmoothConfig->old_FIR_gain_exp_r))
            {
                idem_FIR = FALSE;
            }
        }
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
        write1_debug_TEQ_SHAI_file(instance, "new_same_FIR_l_r=%d\n", pSmoothConfig->new_same_FIR_l_r);
        write2_debug_TEQ_SHAI_file(instance, "new_FIR_gain_mant_l=0x%08X, new_FIR_gain_exp_l=%d\n", pSmoothConfig->new_FIR_gain_mant_l, pSmoothConfig->new_FIR_gain_exp_l);
        write2_debug_TEQ_SHAI_file(instance, "new_FIR_gain_mant_r=0x%08X, new_FIR_gain_exp_r=%d\n", pSmoothConfig->new_FIR_gain_mant_r, pSmoothConfig->new_FIR_gain_exp_r);
        write1_debug_TEQ_SHAI_file(instance, "new_nb_FIR_coefs_per_channel=%d\n", pSmoothConfig->new_nb_FIR_coefs_per_channel);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5

        // we read the biquad parameters and compare with previous values
        if(pSmoothConfig->new_nb_biquad_cells_per_channel > 0)
        {
            if(pSmoothConfig->new_same_biquad_l_r)
            {
                if(pTrEqStruct->p_biquad_coef != NULL)
                {
                    for(i = 0; i < pSmoothConfig->new_nb_biquad_cells_per_channel; i++)
                    {
                        pSmoothConfig->new_biquad_cells.biquad_cell[i] = pTrEqStruct->p_biquad_coef[i];
                        idem_IIR                                     &= common_get_mono_biquad_cell(&pTrEqConfig->ibiquad_cells[i], &pSmoothConfig->new_biquad_cells.biquad_cell[i]);
                    }
                }
                else
                {
                    idem_IIR = FALSE;
                    for(i = 0; i < pSmoothConfig->new_nb_biquad_cells_per_channel; i++)
                    {
                        common_get_mono_biquad_cell(&pTrEqConfig->ibiquad_cells[i], &pSmoothConfig->new_biquad_cells.biquad_cell[i]);
                    }
                }
            }
            else
            {
                t_biquad_cell biquad_cell;

                if(pTrEqStruct->p_biquad_coef_st != NULL)
                {
                    for(i = 0; i < pSmoothConfig->new_nb_biquad_cells_per_channel; i++)
                    {
                        biquad_cell.b_exp                                        = pTrEqStruct->p_biquad_coef_st[i].b_exp_l;
                        biquad_cell.b0                                           = pTrEqStruct->p_biquad_coef_st[i].b0_l;
                        biquad_cell.b1                                           = pTrEqStruct->p_biquad_coef_st[i].b1_l;
                        biquad_cell.b2                                           = pTrEqStruct->p_biquad_coef_st[i].b2_l;
                        biquad_cell.a1                                           = pTrEqStruct->p_biquad_coef_st[i].a1_l;
                        biquad_cell.a2                                           = pTrEqStruct->p_biquad_coef_st[i].a2_l;
                        idem_IIR                                                &= common_get_mono_biquad_cell(&pTrEqConfig->ibiquad_cells[2 * i],     &biquad_cell);
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b_exp_l = biquad_cell.b_exp;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b0_l    = biquad_cell.b0;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b1_l    = biquad_cell.b1;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b2_l    = biquad_cell.b2;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].a1_l    = biquad_cell.a1;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].a2_l    = biquad_cell.a2;

                        biquad_cell.b_exp                                        = pTrEqStruct->p_biquad_coef_st[i].b_exp_r;
                        biquad_cell.b0                                           = pTrEqStruct->p_biquad_coef_st[i].b0_r;
                        biquad_cell.b1                                           = pTrEqStruct->p_biquad_coef_st[i].b1_r;
                        biquad_cell.b2                                           = pTrEqStruct->p_biquad_coef_st[i].b2_r;
                        biquad_cell.a1                                           = pTrEqStruct->p_biquad_coef_st[i].a1_r;
                        biquad_cell.a2                                           = pTrEqStruct->p_biquad_coef_st[i].a2_r;
                        idem_IIR                                                &= common_get_mono_biquad_cell(&pTrEqConfig->ibiquad_cells[2 * i + 1], &biquad_cell);
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b_exp_r = biquad_cell.b_exp;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b0_r    = biquad_cell.b0;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b1_r    = biquad_cell.b1;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b2_r    = biquad_cell.b2;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].a1_r    = biquad_cell.a1;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].a2_r    = biquad_cell.a2;
                    }
                }
                else
                {
                    idem_IIR = FALSE;
                    for(i = 0; i < pSmoothConfig->new_nb_biquad_cells_per_channel; i++)
                    {
                        common_get_mono_biquad_cell(&pTrEqConfig->ibiquad_cells[2 * i],     &biquad_cell);
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b_exp_l = biquad_cell.b_exp;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b0_l    = biquad_cell.b0;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b1_l    = biquad_cell.b1;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b2_l    = biquad_cell.b2;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].a1_l    = biquad_cell.a1;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].a2_l    = biquad_cell.a2;

                        common_get_mono_biquad_cell(&pTrEqConfig->ibiquad_cells[2 * i + 1], &biquad_cell);
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b_exp_r = biquad_cell.b_exp;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b0_r    = biquad_cell.b0;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b1_r    = biquad_cell.b1;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].b2_r    = biquad_cell.b2;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].a1_r    = biquad_cell.a1;
                        pSmoothConfig->new_biquad_cells.biquad_cell_st[i].a2_r    = biquad_cell.a2;
                    }
                }
            }
        }

        // we read the FIR parameters and compare with previous values
        for(i = 0; i < nb_channels_FIR * pSmoothConfig->new_nb_FIR_coefs_per_channel; i++)
        {
            pSmoothConfig->save_new_FIR_coef[i] = pSmoothConfig->new_FIR_coef[i] = common_convert_lowhigh2MMshort(pTrEqConfig->iFIR_coefs[i].coef_low, pTrEqConfig->iFIR_coefs[i].coef_high);
            if(pSmoothConfig->new_FIR_coef[i] != pTrEqStruct->p_FIR_coef[i])
            {
                idem_FIR = FALSE;
            }
        }

        pSmoothConfig->new_enable = (pTrEqConfig->iEnable != 0);
        if(pSmoothConfig->new_enable)
        {
            pSmoothConfig->new_enable = (pSmoothConfig->new_nb_biquad_cells_per_channel > 0)
                                    || (pSmoothConfig->new_nb_FIR_coefs_per_channel    > 0)
                                    || (pSmoothConfig->new_biquad_gain_exp_l           != GAIN_EXP_INIT)
                                    || (pSmoothConfig->new_biquad_gain_mant_l          != GAIN_MANT_INIT)
                                    || (pSmoothConfig->new_FIR_gain_exp_l              != GAIN_EXP_INIT)
                                    || (pSmoothConfig->new_FIR_gain_mant_l             != GAIN_MANT_INIT);
            if(pTrEqStruct->stereo_config)
            {
                if(!pSmoothConfig->new_same_biquad_l_r && !pSmoothConfig->new_enable)
                {
                    pSmoothConfig->new_enable = (pSmoothConfig->new_biquad_gain_exp_r   != GAIN_EXP_INIT)
                                            || (pSmoothConfig->new_biquad_gain_mant_r  != GAIN_MANT_INIT);
                }
                if(!pSmoothConfig->new_same_FIR_l_r && !pSmoothConfig->new_enable)
                {
                    pSmoothConfig->new_enable = (pSmoothConfig->new_FIR_gain_exp_r      != GAIN_EXP_INIT)
                                            || (pSmoothConfig->new_FIR_gain_mant_r     != GAIN_MANT_INIT);
                }
            }
        }
        if(pSmoothConfig->new_enable != pTrEqStruct->enable)
        {
            int smoothing_delay = (((int) pTrEqConfig->ismoothing_delay_low) & 0xFFFF) + ((((int) pTrEqConfig->ismoothing_delay_high) & 0xFFFF) << 16);

            pSmoothConfig->transition_gain_exp_l  = GAIN_EXP_INIT;
            pSmoothConfig->transition_gain_mant_l = GAIN_MANT_INIT;
            pSmoothConfig->transition_gain_exp_r  = GAIN_EXP_INIT;
            pSmoothConfig->transition_gain_mant_r = GAIN_MANT_INIT;
            if(pSmoothConfig->new_enable)
            {
                // transition from disable to enable
                common_copy_new_config(pTrEqStruct, pSmoothConfig);
                common_reset(pTrEqStruct);

                pSmoothConfig->smooth_FIR_transition_duration = pSmoothConfig->smooth_FIR_transition_counter = pSmoothConfig->smooth_transition_duration1 = pSmoothConfig->smooth_transition_counter1 = 0;
                pSmoothConfig->smooth_transition_duration2 = pSmoothConfig->smooth_transition_counter2 = smoothing_delay;
                pTrEqStruct->smooth_config_change = (smoothing_delay > 0);
                if(pTrEqStruct->smooth_config_change)
                {
                    pSmoothConfig->shift_div_transition2 = 2 * Q_ARITHMETIC - 1 - wedge(smoothing_delay);
                    pSmoothConfig->coef_div_transition2  = (MMshort) (((((MMlong) 1) << pSmoothConfig->shift_div_transition2) + (smoothing_delay >> 1)) / smoothing_delay);    // + (smoothing_delay >> 1) for rounding
                    pSmoothConfig->shift_div_transition2 = Q_ARITHMETIC - pSmoothConfig->shift_div_transition2;
                }
                else
                {
                    pSmoothConfig->shift_div_transition2 = 0;
                    pSmoothConfig->coef_div_transition2  = 0;
                }
                pSmoothConfig->shift_div_transition1 = 0;
                pSmoothConfig->coef_div_transition1  = 0;
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                write0_debug_TEQ_SHAI_file(instance, "transition from disable to enable :\n");
                write1_debug_TEQ_SHAI_file(instance, "smooth_transition_duration1=%d\n", pSmoothConfig->smooth_transition_duration1);
                write1_debug_TEQ_SHAI_file(instance, "smooth_transition_duration2=%d\n", pSmoothConfig->smooth_transition_duration2);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
            }
            else
            {
                // transition from enable to disable
                pSmoothConfig->smooth_FIR_transition_duration = pSmoothConfig->smooth_FIR_transition_counter = pSmoothConfig->smooth_transition_duration2 = pSmoothConfig->smooth_transition_counter2 = 0;
                pSmoothConfig->smooth_transition_duration1 = pSmoothConfig->smooth_transition_counter1 = smoothing_delay;
                pTrEqStruct->smooth_config_change = (smoothing_delay > 0);
                if(pTrEqStruct->smooth_config_change)
                {
                    pSmoothConfig->shift_div_transition1 = 2 * Q_ARITHMETIC - 1 - wedge(smoothing_delay);
                    pSmoothConfig->coef_div_transition1  = (MMshort) (((((MMlong) 1) << pSmoothConfig->shift_div_transition1) + (smoothing_delay >> 1)) / smoothing_delay);    // + (smoothing_delay >> 1) for rounding
                    pSmoothConfig->shift_div_transition1 = Q_ARITHMETIC - pSmoothConfig->shift_div_transition1;
                }
                else
                {
                    pSmoothConfig->shift_div_transition1 = 0;
                    pSmoothConfig->coef_div_transition1  = 0;
                }
                pSmoothConfig->shift_div_transition2 = 0;
                pSmoothConfig->coef_div_transition2  = 0;
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                write0_debug_TEQ_SHAI_file(instance, "transition from enable to disable :\n");
                write1_debug_TEQ_SHAI_file(instance, "smooth_transition_duration1=%d\n", pSmoothConfig->smooth_transition_duration1);
                write1_debug_TEQ_SHAI_file(instance, "smooth_transition_duration2=%d\n", pSmoothConfig->smooth_transition_duration2);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
            }
        }
        else if(idem_IIR && idem_FIR)
        {
            pTrEqStruct->smooth_config_change = FALSE;
        }
        else
        {
            MMshort *p1, *p2, FIR_coef, gain_coef;
            int     smoothing_delay = (((int) pTrEqConfig->ismoothing_delay_low) & 0xFFFF) + ((((int) pTrEqConfig->ismoothing_delay_high) & 0xFFFF) << 16);

            if(pTrEqConfig->iautomatic_transition_gain != 0)
            {
                // compute transition_gain as (old_average_gain + new_average_gain) / 2
                if(pSmoothConfig->old_average_gain_exp_l < pSmoothConfig->new_average_gain_exp_l)
                {
                    pSmoothConfig->transition_gain_mant_l = (pSmoothConfig->old_average_gain_mant_l >> (pSmoothConfig->new_average_gain_exp_l - pSmoothConfig->old_average_gain_exp_l + 1))
                                                         + (pSmoothConfig->new_average_gain_mant_l >> 1);
                    pSmoothConfig->transition_gain_exp_l  = pSmoothConfig->new_average_gain_exp_l;
                }
                else
                {
                    pSmoothConfig->transition_gain_mant_l = (pSmoothConfig->old_average_gain_mant_l >> 1)
                                                         + (pSmoothConfig->new_average_gain_mant_l >> (pSmoothConfig->old_average_gain_exp_l - pSmoothConfig->new_average_gain_exp_l + 1));
                    pSmoothConfig->transition_gain_exp_l  = pSmoothConfig->old_average_gain_exp_l;
                }
                if(pSmoothConfig->old_average_gain_exp_r < pSmoothConfig->new_average_gain_exp_r)
                {
                    pSmoothConfig->transition_gain_mant_r = (pSmoothConfig->old_average_gain_mant_r >> (pSmoothConfig->new_average_gain_exp_r - pSmoothConfig->old_average_gain_exp_r + 1))
                                                         + (pSmoothConfig->new_average_gain_mant_r >> 1);
                    pSmoothConfig->transition_gain_exp_r  = pSmoothConfig->new_average_gain_exp_r;
                }
                else
                {
                    pSmoothConfig->transition_gain_mant_r = (pSmoothConfig->old_average_gain_mant_r >> 1)
                                                         + (pSmoothConfig->new_average_gain_mant_r >> (pSmoothConfig->old_average_gain_exp_r - pSmoothConfig->new_average_gain_exp_r + 1));
                    pSmoothConfig->transition_gain_exp_r  = pSmoothConfig->old_average_gain_exp_r;
                }
                shift                                    = wedge(pSmoothConfig->transition_gain_mant_l);
                pSmoothConfig->transition_gain_exp_l     -= shift;
                pSmoothConfig->transition_gain_mant_l   <<= shift;
                shift                                    = wedge(pSmoothConfig->transition_gain_mant_r);
                pSmoothConfig->transition_gain_exp_r     -= shift;
                pSmoothConfig->transition_gain_mant_r   <<= shift;
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                write0_debug_TEQ_SHAI_file(instance, "use_average_gain :\n");
                write2_debug_TEQ_SHAI_file(instance, "old_average_gain_mant_l=0x%08X, old_average_gain_exp_l=%d\n", pSmoothConfig->old_average_gain_mant_l, pSmoothConfig->old_average_gain_exp_l);
                write2_debug_TEQ_SHAI_file(instance, "old_average_gain_mant_r=0x%08X, old_average_gain_exp_r=%d\n", pSmoothConfig->old_average_gain_mant_r, pSmoothConfig->old_average_gain_exp_r);
                write2_debug_TEQ_SHAI_file(instance, "new_average_gain_mant_l=0x%08X, new_average_gain_exp_l=%d\n", pSmoothConfig->new_average_gain_mant_l, pSmoothConfig->new_average_gain_exp_l);
                write2_debug_TEQ_SHAI_file(instance, "new_average_gain_mant_r=0x%08X, new_average_gain_exp_r=%d\n", pSmoothConfig->new_average_gain_mant_r, pSmoothConfig->new_average_gain_exp_r);
                write2_debug_TEQ_SHAI_file(instance, "transition_gain_mant_l =0x%08X, transition_gain_exp_l =%d\n", pSmoothConfig->transition_gain_mant_l,  pSmoothConfig->transition_gain_exp_l);
                write2_debug_TEQ_SHAI_file(instance, "transition_gain_mant_r =0x%08X, transition_gain_exp_r =%d\n", pSmoothConfig->transition_gain_mant_r,  pSmoothConfig->transition_gain_exp_r);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
            }
            else
            {
#ifdef __flexcc2__
                pSmoothConfig->transition_gain_exp_l      = (((MMshort) pTrEqConfig->itransition_gain_exp_l) << 8) >> 8;
                pSmoothConfig->transition_gain_exp_r      = (((MMshort) pTrEqConfig->itransition_gain_exp_r) << 8) >> 8;
#else /* __flexcc2__ */
                pSmoothConfig->transition_gain_exp_l      = (((MMshort) pTrEqConfig->itransition_gain_exp_l) << 16) >> 16;
                pSmoothConfig->transition_gain_exp_r      = (((MMshort) pTrEqConfig->itransition_gain_exp_r) << 16) >> 16;
#endif /* __flexcc2__ */
                pSmoothConfig->transition_gain_mant_l     = common_convert_lowhigh2MMshort(pTrEqConfig->itransition_gain_mant_l_low, pTrEqConfig->itransition_gain_mant_l_high);
                pSmoothConfig->transition_gain_mant_r     = common_convert_lowhigh2MMshort(pTrEqConfig->itransition_gain_mant_r_low, pTrEqConfig->itransition_gain_mant_r_high);
                shift                                    = wedge(pSmoothConfig->transition_gain_mant_l);
                pSmoothConfig->transition_gain_exp_l     -= shift;
                pSmoothConfig->transition_gain_mant_l   <<= shift;
                shift                                    = wedge(pSmoothConfig->transition_gain_mant_r);
                pSmoothConfig->transition_gain_exp_r     -= shift;
                pSmoothConfig->transition_gain_mant_r   <<= shift;
            }

            if(!idem_IIR)
            {
                // samples have already been processed, at least IIR filter has changed
                smoothing_delay >>= 1;
                pSmoothConfig->smooth_transition_duration1 = pSmoothConfig->smooth_transition_duration2 = pSmoothConfig->smooth_transition_counter1 = pSmoothConfig->smooth_transition_counter2 = smoothing_delay;
                pSmoothConfig->smooth_FIR_transition_duration = pSmoothConfig->smooth_FIR_transition_counter = 0;
                pTrEqStruct->smooth_config_change = (smoothing_delay > 0);
                if(pTrEqStruct->smooth_config_change)
                {
                    pSmoothConfig->shift_div_transition1 = 2 * Q_ARITHMETIC - 1 - wedge(smoothing_delay);
                    pSmoothConfig->coef_div_transition1  = (MMshort) (((((MMlong) 1) << pSmoothConfig->shift_div_transition1) + (smoothing_delay >> 1)) / smoothing_delay);    // + (smoothing_delay >> 1) for rounding
                    pSmoothConfig->shift_div_transition1 = Q_ARITHMETIC - pSmoothConfig->shift_div_transition1;
                }
                else
                {
                    pSmoothConfig->shift_div_transition1 = 0;
                    pSmoothConfig->coef_div_transition1  = 0;
                }
                pSmoothConfig->shift_div_transition2 = pSmoothConfig->shift_div_transition1;
                pSmoothConfig->coef_div_transition2  = pSmoothConfig->coef_div_transition1;
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                write0_debug_TEQ_SHAI_file(instance, "IIR+FIR smoothing :\n");
                write1_debug_TEQ_SHAI_file(instance, "smooth_transition_duration1=%d\n", pSmoothConfig->smooth_transition_duration1);
                write1_debug_TEQ_SHAI_file(instance, "smooth_transition_duration2=%d\n", pSmoothConfig->smooth_transition_duration2);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
            }
            else
            {
                // FIR filter has changed but IIR filter has not changed
                // thus we can smooth FIR filter
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                write0_debug_TEQ_SHAI_file(instance, "FIR smoothing :\n");
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                pSmoothConfig->smooth_transition_duration1 = pSmoothConfig->smooth_transition_duration2 = pSmoothConfig->smooth_transition_counter1 = pSmoothConfig->smooth_transition_counter2 = 0;
                pSmoothConfig->smooth_FIR_transition_duration = pSmoothConfig->smooth_FIR_transition_counter = smoothing_delay;
                pTrEqStruct->smooth_config_change = (smoothing_delay > 0);
                if(pTrEqStruct->smooth_config_change)
                {
                    pSmoothConfig->FIR_smoothing_delay_granularity = (((int) pTrEqConfig->iFIR_smoothing_delay_granularity) & 0xFFFF);
                    if((pSmoothConfig->new_nb_FIR_coefs_per_channel > 0) || (pSmoothConfig->old_nb_FIR_coefs_per_channel > 0))
                    {
                        if(pSmoothConfig->new_same_FIR_l_r != pSmoothConfig->old_same_FIR_l_r)
                        {
                            pTrEqStruct->same_FIR_l_r = FALSE;
                            nb_channels_FIR          = 2;
                            p1                       = pSmoothConfig->old_FIR_coef;
                            p2                       = pTrEqStruct->p_FIR_coef;
                            if(pSmoothConfig->old_same_FIR_l_r)
                            {
                                for(i = pSmoothConfig->old_nb_FIR_coefs_per_channel; i > 0; i--)
                                {
                                    FIR_coef = *p2++;
                                    *p1++    = FIR_coef;
                                    *p1++    = FIR_coef;
                                }
                            }
                            else
                            {
                                for(i = 2 * pSmoothConfig->old_nb_FIR_coefs_per_channel; i > 0; i--)
                                {
                                    *p1++ = *p2++;
                                }
                                p1 = pSmoothConfig->new_FIR_coef;
                                p2 = pSmoothConfig->save_new_FIR_coef;
                                for(i = pSmoothConfig->new_nb_FIR_coefs_per_channel; i > 0; i--)
                                {
                                    FIR_coef = *p2++;
                                    *p1++    = FIR_coef;
                                    *p1++    = FIR_coef;
                                }
                            }
                        }
                        else
                        {
                            p1 = pSmoothConfig->old_FIR_coef;
                            p2 = pTrEqStruct->p_FIR_coef;
                            for(i = nb_channels_FIR * pSmoothConfig->old_nb_FIR_coefs_per_channel; i > 0; i--)
                            {
                                *p1++ = *p2++;
                            }
                        }
                        if(pSmoothConfig->new_nb_FIR_coefs_per_channel > pSmoothConfig->old_nb_FIR_coefs_per_channel)
                        {
                            p1 = &pSmoothConfig->old_FIR_coef[nb_channels_FIR * pSmoothConfig->old_nb_FIR_coefs_per_channel];
                            for(i = nb_channels_FIR * (pSmoothConfig->new_nb_FIR_coefs_per_channel - pSmoothConfig->old_nb_FIR_coefs_per_channel); i > 0; i--)
                            {
                                *p1++ = 0;
                            }
                            pTrEqStruct->nb_FIR_coefs_per_channel = pSmoothConfig->new_nb_FIR_coefs_per_channel;
                        }
                        else
                        {
                            p1 = &pSmoothConfig->new_FIR_coef[nb_channels_FIR * pSmoothConfig->new_nb_FIR_coefs_per_channel];
                            for(i = nb_channels_FIR * (pSmoothConfig->old_nb_FIR_coefs_per_channel - pSmoothConfig->new_nb_FIR_coefs_per_channel); i > 0; i--)
                            {
                                *p1++ = 0;
                            }
                            pTrEqStruct->nb_FIR_coefs_per_channel = pSmoothConfig->old_nb_FIR_coefs_per_channel;
                        }
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                        write0_debug_TEQ_SHAI_file(instance, "old_FIR_coef\n");
                        for(i = 0; i < nb_channels_FIR * pTrEqStruct->nb_FIR_coefs_per_channel; i++)
                        {
                            switch(i % 8)
                            {
                                case 0:
                                    write1_debug_TEQ_SHAI_file(instance, "0x%08X", pSmoothConfig->old_FIR_coef[i]);
                                    break;
                                case 7:
                                    write1_debug_TEQ_SHAI_file(instance, " 0x%08X\n", pSmoothConfig->old_FIR_coef[i]);
                                    break;
                                default:
                                    write1_debug_TEQ_SHAI_file(instance, " 0x%08X", pSmoothConfig->old_FIR_coef[i]);
                                    break;
                            }
                        }
                        if((i % 8) != 0)
                        {
                            write0_debug_TEQ_SHAI_file(instance, "\n");
                        }
                        write0_debug_TEQ_SHAI_file(instance, "new_FIR_coef\n");
                        for(i = 0; i < nb_channels_FIR * pTrEqStruct->nb_FIR_coefs_per_channel; i++)
                        {
                            switch(i % 8)
                            {
                                case 0:
                                    write1_debug_TEQ_SHAI_file(instance, "0x%08X", pSmoothConfig->new_FIR_coef[i]);
                                    break;
                                case 7:
                                    write1_debug_TEQ_SHAI_file(instance, " 0x%08X\n", pSmoothConfig->new_FIR_coef[i]);
                                    break;
                                default:
                                    write1_debug_TEQ_SHAI_file(instance, " 0x%08X", pSmoothConfig->new_FIR_coef[i]);
                                    break;
                            }
                        }
                        if((i % 8) != 0)
                        {
                            write0_debug_TEQ_SHAI_file(instance, "\n");
                        }
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                    }

                    if((pSmoothConfig->old_FIR_gain_exp_l == pSmoothConfig->new_FIR_gain_exp_l) && (pSmoothConfig->old_FIR_gain_mant_l == pSmoothConfig->new_FIR_gain_mant_l))
                    {
                        // do nothing
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                        write0_debug_TEQ_SHAI_file(instance, "old_FIR_gain_l == new_FIR_gain_l\n");
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                    }
                    else if(   (pSmoothConfig->old_FIR_gain_exp_l > pSmoothConfig->new_FIR_gain_exp_l)
                            || ((pSmoothConfig->old_FIR_gain_exp_l == pSmoothConfig->new_FIR_gain_exp_l) && (pSmoothConfig->old_FIR_gain_mant_l > pSmoothConfig->new_FIR_gain_mant_l)))
                    {
                        // old_FIR_gain_l > new_FIR_gain_l
                        // multiply new_FIR_coef_l by new_FIR_gain_l/old_FIR_gain_l and set new_FIR_gain_l to old_FIR_gain_l
                        gain_coef = (MMshort) ((((MMlong) pSmoothConfig->new_FIR_gain_mant_l) << (Q_ARITHMETIC + pSmoothConfig->new_FIR_gain_exp_l - pSmoothConfig->old_FIR_gain_exp_l)) / pSmoothConfig->old_FIR_gain_mant_l);
                        p1        = pSmoothConfig->new_FIR_coef;
                        for(i = pSmoothConfig->new_nb_FIR_coefs_per_channel; i > 0; i--)
                        {
#ifdef _NMF_MPC_
                            *p1 = wfmulr(*p1, gain_coef);
#else // _NMF_MPC_
                            *p1 = (MMshort) ((((MMlong) *p1) * ((MMlong) gain_coef)) >> 31);
#endif // _NMF_MPC_
                            p1 += nb_channels_FIR;
                        }
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                        write1_debug_TEQ_SHAI_file(instance, "old_FIR_gain_l > new_FIR_gain_l : multiply new_FIR_coef_l by 0x%08X\n", gain_coef);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                    }
                    else
                    {
                        // old_FIR_gain_l < new_FIR_gain_l
                        // multiply old_FIR_coef_l by old_FIR_gain_l/new_FIR_gain_l and set old_FIR_gain_l to new_FIR_gain_l
                        gain_coef = (MMshort) ((((MMlong) pSmoothConfig->old_FIR_gain_mant_l) << (Q_ARITHMETIC + pSmoothConfig->old_FIR_gain_exp_l - pSmoothConfig->new_FIR_gain_exp_l)) / pSmoothConfig->new_FIR_gain_mant_l);
                        p1        = pSmoothConfig->old_FIR_coef;
                        for(i = pSmoothConfig->old_nb_FIR_coefs_per_channel; i > 0; i--)
                        {
#ifdef _NMF_MPC_
                            *p1 = wfmulr(*p1, gain_coef);
#else // _NMF_MPC_
                            *p1 = (MMshort) ((((MMlong) *p1) * ((MMlong) gain_coef)) >> 31);
#endif // _NMF_MPC_
                            p1 += nb_channels_FIR;
                        }
                        pTrEqStruct->FIR_gain_exp_l  = pSmoothConfig->new_FIR_gain_exp_l;
                        pTrEqStruct->FIR_gain_mant_l = pSmoothConfig->new_FIR_gain_mant_l;
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                        write1_debug_TEQ_SHAI_file(instance, "old_FIR_gain_l < new_FIR_gain_l : multiply old_FIR_gain_coef_l by 0x%08X\n", gain_coef);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                    }
                    if(pTrEqStruct->same_FIR_l_r)
                    {
                        pTrEqStruct->FIR_gain_exp_r  = pTrEqStruct->FIR_gain_exp_l;
                        pTrEqStruct->FIR_gain_mant_r = pTrEqStruct->FIR_gain_mant_l;
                    }
                    else
                    {
                        if((pSmoothConfig->old_FIR_gain_exp_r == pSmoothConfig->new_FIR_gain_exp_r) && (pSmoothConfig->old_FIR_gain_mant_r == pSmoothConfig->new_FIR_gain_mant_r))
                        {
                            // do nothing
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                            write0_debug_TEQ_SHAI_file(instance, "old_FIR_gain_r == new_FIR_gain_r\n");
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                        }
                        else if(   (pSmoothConfig->old_FIR_gain_exp_r > pSmoothConfig->new_FIR_gain_exp_r)
                                || ((pSmoothConfig->old_FIR_gain_exp_r == pSmoothConfig->new_FIR_gain_exp_r) && (pSmoothConfig->old_FIR_gain_mant_r > pSmoothConfig->new_FIR_gain_mant_r)))
                        {
                            // old_FIR_gain_r > new_FIR_gain_r
                            // multiply new_FIR_coef_r by new_FIR_gain_r/old_FIR_gain_r and set new_FIR_gain_r to old_FIR_gain_r
                            gain_coef = (MMshort) ((((MMlong) pSmoothConfig->new_FIR_gain_mant_r) << (Q_ARITHMETIC + pSmoothConfig->new_FIR_gain_exp_r - pSmoothConfig->old_FIR_gain_exp_r)) / pSmoothConfig->old_FIR_gain_mant_r);
                            p1        = &pSmoothConfig->new_FIR_coef[1];
                            for(i = pSmoothConfig->new_nb_FIR_coefs_per_channel; i > 0; i--)
                            {
#ifdef _NMF_MPC_
                                *p1 = wfmulr(*p1, gain_coef);
#else // _NMF_MPC_
                                *p1 = (MMshort) ((((MMlong) *p1) * ((MMlong) gain_coef)) >> 31);
#endif // _NMF_MPC_
                                p1 += 2;
                            }
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                            write1_debug_TEQ_SHAI_file(instance, "old_FIR_gain_r > new_FIR_gain_r : multiply new_FIR_gain_coef_r by 0x%08X\n", gain_coef);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                        }
                        else
                        {
                            // old_FIR_gain_r < new_FIR_gain_r
                            // multiply old_FIR_coef_r by old_FIR_gain_r/new_FIR_gain_r and set old_FIR_gain_r to new_FIR_gain_r
                            gain_coef = (MMshort) ((((MMlong) pSmoothConfig->old_FIR_gain_mant_r) << (Q_ARITHMETIC + pSmoothConfig->old_FIR_gain_exp_r - pSmoothConfig->new_FIR_gain_exp_r)) / pSmoothConfig->new_FIR_gain_mant_r);
                            p1        = &pSmoothConfig->old_FIR_coef[1];
                            for(i = pSmoothConfig->old_nb_FIR_coefs_per_channel; i > 0; i--)
                            {
#ifdef _NMF_MPC_
                                *p1 = wfmulr(*p1, gain_coef);
#else // _NMF_MPC_
                                *p1 = (MMshort) ((((MMlong) *p1) * ((MMlong) gain_coef)) >> 31);
#endif // _NMF_MPC_
                                p1 += 2;
                            }
                            pTrEqStruct->FIR_gain_exp_r  = pSmoothConfig->new_FIR_gain_exp_r;
                            pTrEqStruct->FIR_gain_mant_r = pSmoothConfig->new_FIR_gain_mant_r;
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                            write1_debug_TEQ_SHAI_file(instance, "old_FIR_gain_r < new_FIR_gain_r : multiply old_FIR_gain_coef_r by 0x%08X\n", gain_coef);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                        }
                    }
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                    write1_debug_TEQ_SHAI_file(instance, "same_FIR_l_r=%d\n", pTrEqStruct->same_FIR_l_r);
                    write2_debug_TEQ_SHAI_file(instance, "FIR_gain_mant_l=0x%08X, FIR_gain_exp_l=%d\n", pTrEqStruct->FIR_gain_mant_l, pTrEqStruct->FIR_gain_exp_l);
                    write2_debug_TEQ_SHAI_file(instance, "FIR_gain_mant_r=0x%08X, FIR_gain_exp_r=%d\n", pTrEqStruct->FIR_gain_mant_r, pTrEqStruct->FIR_gain_exp_r);
                    write1_debug_TEQ_SHAI_file(instance, "nb_FIR_coefs_per_channel=%d\n", pTrEqStruct->nb_FIR_coefs_per_channel);
                    write1_debug_TEQ_SHAI_file(instance, "smooth_FIR_transition_duration=%d\n", pSmoothConfig->smooth_FIR_transition_duration);
                    write0_debug_TEQ_SHAI_file(instance, "old_FIR_coef recomputed\n");
                    for(i = 0; i < nb_channels_FIR * pTrEqStruct->nb_FIR_coefs_per_channel; i++)
                    {
                        switch(i % 8)
                        {
                            case 0:
                                write1_debug_TEQ_SHAI_file(instance, "0x%08X", pSmoothConfig->old_FIR_coef[i]);
                                break;
                            case 7:
                                write1_debug_TEQ_SHAI_file(instance, " 0x%08X\n", pSmoothConfig->old_FIR_coef[i]);
                                break;
                            default:
                                write1_debug_TEQ_SHAI_file(instance, " 0x%08X", pSmoothConfig->old_FIR_coef[i]);
                                break;
                        }
                    }
                    if((i % 8) != 0)
                    {
                        write0_debug_TEQ_SHAI_file(instance, "\n");
                    }
                    write0_debug_TEQ_SHAI_file(instance, "new_FIR_coef recomputed\n");
                    for(i = 0; i < nb_channels_FIR * pTrEqStruct->nb_FIR_coefs_per_channel; i++)
                    {
                        switch(i % 8)
                        {
                            case 0:
                                write1_debug_TEQ_SHAI_file(instance, "0x%08X", pSmoothConfig->new_FIR_coef[i]);
                                break;
                            case 7:
                                write1_debug_TEQ_SHAI_file(instance, " 0x%08X\n", pSmoothConfig->new_FIR_coef[i]);
                                break;
                            default:
                                write1_debug_TEQ_SHAI_file(instance, " 0x%08X", pSmoothConfig->new_FIR_coef[i]);
                                break;
                        }
                    }
                    if((i % 8) != 0)
                    {
                        write0_debug_TEQ_SHAI_file(instance, "\n");
                    }
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
                }
            }

            if(!pTrEqStruct->smooth_config_change)
            {
                common_copy_new_config(pTrEqStruct, pSmoothConfig);
                common_reset(pTrEqStruct);
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
                write0_debug_TEQ_SHAI_file(instance, "no smoothing :\n");
                write1_debug_TEQ_SHAI_file(instance, "same_FIR_l_r=%d\n", pTrEqStruct->same_FIR_l_r);
                write2_debug_TEQ_SHAI_file(instance, "FIR_gain_mant_l=0x%08X, FIR_gain_exp_l=%d\n", pTrEqStruct->FIR_gain_mant_l, pTrEqStruct->FIR_gain_exp_l);
                write2_debug_TEQ_SHAI_file(instance, "FIR_gain_mant_r=0x%08X, FIR_gain_exp_r=%d\n", pTrEqStruct->FIR_gain_mant_r, pTrEqStruct->FIR_gain_exp_r);
                write1_debug_TEQ_SHAI_file(instance, "nb_FIR_coefs_per_channel=%d\n", pTrEqStruct->nb_FIR_coefs_per_channel);
                write1_debug_TEQ_SHAI_file(instance, "same_biquad_l_r=%d\n", pTrEqStruct->same_biquad_l_r);
                write2_debug_TEQ_SHAI_file(instance, "biquad_gain_mant_l=0x%08X, biquad_gain_exp_l=%d\n", pTrEqStruct->biquad_gain_mant_l, pTrEqStruct->biquad_gain_exp_l);
                write2_debug_TEQ_SHAI_file(instance, "biquad_gain_mant_r=0x%08X, biquad_gain_exp_r=%d\n", pTrEqStruct->biquad_gain_mant_r, pTrEqStruct->biquad_gain_exp_r);
                write1_debug_TEQ_SHAI_file(instance, "nb_biquad_cells_per_channel=%d\n", pTrEqStruct->nb_biquad_cells_per_channel);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
            }
        }

        transducer_equalizer_set_filters_pointer(pTrEqStruct);
    }
#if (DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5)
    write0_debug_TEQ_SHAI_file(instance, "--------------------------------------------------\n");
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 5
}


#endif // TRANSDUCER_EQUALIZER_NMFIL_COMMON_SRC_CODE

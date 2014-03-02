#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "TEQ_SHAI.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef FALSE
#define FALSE (1 == 0)
#endif
#ifndef TRUE
#define TRUE (1 == 1)
#endif


typedef struct
{
    char *p_start;
    char *p_end;
} t_ptr_tag;


static int search_tag(const char *tag_name, char *p_buf, t_ptr_tag *p_ptr_tag)
{
    char string[100];

    sprintf(string, "<%s", tag_name);

    do
    {
        p_buf = strstr(p_buf, string);
        if(p_buf == NULL)
        {
            return -1;
        }
        p_buf += strlen(string);
    } while((*p_buf != ' ') && (*p_buf != '\t') && (*p_buf != '>'));
    while((*p_buf == ' ') || (*p_buf == '\t'))
    {
        p_buf++;
    }
    p_ptr_tag->p_start = p_buf;

    p_buf = strchr(p_buf, '>');
    if(p_buf == NULL)
    {
        return -1;
    }
    p_ptr_tag->p_end = p_buf;

    return 0;
}


static int search_field(char *field_name, t_ptr_tag *p_ptr_tag, char **p_field_value)
{
    char *p_buf = p_ptr_tag->p_start;

    do
    {
        p_buf = strstr(p_buf, field_name);
        if((p_buf == NULL) || (p_buf >= p_ptr_tag->p_end))
        {
            return -1;
        }
        p_buf += strlen(field_name);
    } while((*p_buf != ' ') && (*p_buf != '\t') && (*p_buf != '='));
    if(*p_buf == '=')
    {
        p_buf++;
    }
    else
    {
        while((*p_buf == ' ') || (*p_buf == '\t'))
        {
            p_buf++;
        }
        if((p_buf >= p_ptr_tag->p_end) || (*p_buf != '='))
        {
            return -1;
        }
        p_buf++;
    }
    while((*p_buf == ' ') || (*p_buf == '\t'))
    {
        p_buf++;
    }
    if((p_buf >= p_ptr_tag->p_end) || (*p_buf != '"'))
    {
        return -1;
    }
    p_buf++;
    *p_field_value = p_buf;

    return 0;
}


static int get_string(char *p_start, char *p_end, char *p_string, int string_size_max)
{
    int i;

    if(p_end <= p_start)
    {
        return -1;
    }

    for(i = 0; i < string_size_max; i++)
    {
        p_string[i] = *p_start++;
        if(p_string[i] == '"')
        {
            p_string[i] = 0;
            return 0;
        }
        if(p_start == p_end)
        {
            return -1;
        }
    }
    return -1;
}


#define NB_MAX_BIQUAD 40

int main(int argc, char *argv[])
{
    FILE      *p_file_in, *p_file_ref;
    char      *p_buf_in, *p_buf_ref;
    int       i, size, instance = 0, nb_bits_quantif = 32;
    t_ptr_tag ptr_start_ctrlIIR, ptr_end_ctrlIIR;


#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
    open_debug_TEQ_SHAI_file(instance);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0

    if((argc < 3) || (argc > 4))
    {
        fprintf(stderr, "usage : %s <xml input file> <xml reference file> [<number of quantization bits>]\n", argv[0]);
        return -1;
    }
    p_file_in = fopen(argv[1], "r");
    if(p_file_in == NULL)
    {
        fprintf(stderr, "can't open \"%s\" input file\n", argv[1]);
        return -1;
    }
    fseek(p_file_in, 0, SEEK_END);
    size = ftell(p_file_in);
    rewind(p_file_in);
    p_buf_in = (char *) malloc(size + 1);
    if(p_buf_in == NULL)
    {
        fprintf(stderr, "can't allocate input buffer\n");
        return -1;
    }
    if(fread(p_buf_in, 1, size, p_file_in) != size)
    {
        fprintf(stderr, "fread error\n");
        return -1;
    }
    *(p_buf_in + size) = 0;
    fclose(p_file_in);
    for(i = 0; i < size; i++)
    {
        switch(p_buf_in[i])
        {
            case '\r':
            case '\n':
                p_buf_in[i] = ' ';
                break;
            default:
                break;
        }
    }

    p_file_ref = fopen(argv[2], "r");
    if(p_file_ref == NULL)
    {
        fprintf(stderr, "can't open \"%s\" reference file\n", argv[2]);
        return -1;
    }
    fseek(p_file_ref, 0, SEEK_END);
    size = ftell(p_file_ref);
    rewind(p_file_ref);
    p_buf_ref = (char *) malloc(size + 1);
    if(p_buf_ref == NULL)
    {
        fprintf(stderr, "can't allocate reference buffer\n");
        return -1;
    }
    if(fread(p_buf_ref, 1, size, p_file_ref) != size)
    {
        fprintf(stderr, "fread error\n");
        return -1;
    }
    *(p_buf_ref + size) = 0;
    fclose(p_file_ref);
    for(i = 0; i < size; i++)
    {
        switch(p_buf_ref[i])
        {
            case '\r':
            case '\n':
                p_buf_ref[i] = ' ';
                break;
            default:
                break;
        }
    }

    if(argc == 4)
    {
        nb_bits_quantif = atoi(argv[3]);
    }

    ptr_end_ctrlIIR.p_end = p_buf_in;
    while(search_tag("ctrlIIR", ptr_end_ctrlIIR.p_end, &ptr_start_ctrlIIR) == 0)
    {
        OMX_AUDIO_CONFIG_IIR_EQUALIZER mOMX_ConfigAudioTransEqualizer_iireq[NB_MAX_BIQUAD];
        t_ptr_tag                      ptr_set, ptr_commit;
        char                           *p_id;
        char                           id[100];
        int                            nb_max_biquads = 0;

        for(i = 0; i < NB_MAX_BIQUAD; i++)
        {
            mOMX_ConfigAudioTransEqualizer_iireq[i].sBandIndex.nValue = NB_MAX_BIQUAD;
        }

        if(search_tag("/ctrlIIR", ptr_start_ctrlIIR.p_end, &ptr_end_ctrlIIR) != 0)
        {
            fprintf(stderr, "can't find /ctrlIIR tag\n");
            return -1;
        }
        if(search_field("id", &ptr_start_ctrlIIR, &p_id) != 0)
        {
            fprintf(stderr, "can't find id field into ctrlIIR tag\n");
            return -1;
        }
        if(get_string(p_id, ptr_start_ctrlIIR.p_end, id, sizeof(id)) != 0)
        {
            fprintf(stderr, "can't read id field value into ctrlIIR tag\n");
            return -1;
        }
        printf("\n\n******** IIR %s ********\n", id);

        ptr_set.p_end = ptr_start_ctrlIIR.p_end;
        while(   (search_tag("set", ptr_set.p_end, &ptr_set) == 0)
              && (ptr_set.p_end < ptr_end_ctrlIIR.p_start))
        {
            char *p_sBandIndex, *p_eFilterType, *p_nFilterGain_mB, *p_nFilterBandwidth_Hz, *p_nFilterCenterCutoffFrequency_Hz;
            int  sBandIndex, nFilterGain_mB, nFilterBandwidth_Hz, nFilterCenterCutoffFrequency_Hz;
            char eFilterType[100];

            if(search_field("sBandIndex", &ptr_set, &p_sBandIndex) != 0)
            {
                fprintf(stderr, "can't find sBandIndex field into set tag\n");
                return -1;
            }
            sBandIndex = atoi(p_sBandIndex);
            if(search_field("eFilterType", &ptr_set, &p_eFilterType) != 0)
            {
                fprintf(stderr, "can't find eFilterType field into set tag\n");
                return -1;
            }
            if(get_string(p_eFilterType, ptr_set.p_end, eFilterType, sizeof(eFilterType)) != 0)
            {
                fprintf(stderr, "can't read eFilterType field value into set tag\n");
                return -1;
            }
            if(search_field("nFilterGain_mB", &ptr_set, &p_nFilterGain_mB) != 0)
            {
                fprintf(stderr, "can't find nFilterGain_mB field into set tag\n");
                return -1;
            }
            nFilterGain_mB = atoi(p_nFilterGain_mB);
            if(search_field("nFilterBandwidth_Hz", &ptr_set, &p_nFilterBandwidth_Hz) != 0)
            {
                fprintf(stderr, "can't find nFilterBandwidth_Hz field into set tag\n");
                return -1;
            }
            nFilterBandwidth_Hz = atoi(p_nFilterBandwidth_Hz);
            if(search_field("nFilterCenterCutoffFrequency_Hz", &ptr_set, &p_nFilterCenterCutoffFrequency_Hz) != 0)
            {
                fprintf(stderr, "can't find nFilterCenterCutoffFrequency_Hz field into set tag\n");
                return -1;
            }
            nFilterCenterCutoffFrequency_Hz = atoi(p_nFilterCenterCutoffFrequency_Hz);
            nb_max_biquads = max(sBandIndex, nb_max_biquads);
            sBandIndex--;
            if(sBandIndex >= NB_MAX_BIQUAD)
            {
                fprintf(stderr, "sBandIndex must be <= %d\n", NB_MAX_BIQUAD);
                return -1;
            }
            mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].sBandIndex.nValue            = sBandIndex;
            mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].nFilterGain                  = nFilterGain_mB;
            mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].nFilterCenterCutoffFrequency = nFilterCenterCutoffFrequency_Hz;
            mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].nFilterBandwidth             = nFilterBandwidth_Hz;
            if(strcmp(eFilterType, "OMX_AUDIO_Lowpass") == 0)
            {
                mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].eFilterType              = OMX_AUDIO_Lowpass;
            }
            else if(strcmp(eFilterType, "OMX_AUDIO_Highpass") == 0)
            {
                mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].eFilterType              = OMX_AUDIO_Highpass;
            }
            else if(strcmp(eFilterType, "OMX_AUDIO_Peaking") == 0)
            {
                mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].eFilterType              = OMX_AUDIO_Peaking;
            }
            else if(strcmp(eFilterType, "OMX_AUDIO_Lowshelf") == 0)
            {
                mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].eFilterType              = OMX_AUDIO_Lowshelf;
            }
            else if(strcmp(eFilterType, "OMX_AUDIO_Highshelf") == 0)
            {
                mOMX_ConfigAudioTransEqualizer_iireq[sBandIndex].eFilterType              = OMX_AUDIO_Highshelf;
            }
            else
            {
                fprintf(stderr, "unknown %s eFilterType\n", eFilterType);
                return -1;
            }
        }

        ptr_commit = ptr_start_ctrlIIR;
        while(   (search_tag("commit", ptr_commit.p_end, &ptr_commit) == 0)
              && (ptr_commit.p_end < ptr_end_ctrlIIR.p_start))
        {
            t_IIR_filter_float      IIR_filter;
            t_IIR_filter_quantified IIR_filter_quantified;
            OMX_ERRORTYPE           error;
            char                    *p_nSamplingRate_Hz, *p_sName, *p_bSort;
            int                     nSamplingRate_Hz, id_found, nb_biquads;
            char                    sName[100], bSort[100];
            t_ptr_tag               ptr_start_FW_API_IIR, ptr_end_FW_API_IIR;
            t_biquad_sort           biquad_sort;
            t_float                 gain;

            if(search_field("nSamplingRate_Hz", &ptr_commit, &p_nSamplingRate_Hz) != 0)
            {
                fprintf(stderr, "can't find nSamplingRate_Hz field into commit tag\n");
                return -1;
            }
            nSamplingRate_Hz = atoi(p_nSamplingRate_Hz);
            if(search_field("sName", &ptr_commit, &p_sName) != 0)
            {
                fprintf(stderr, "can't find sName field into commit tag\n");
                return -1;
            }
            if(get_string(p_sName, ptr_commit.p_end, sName, sizeof(sName)) != 0)
            {
                fprintf(stderr, "can't read sName field value into commit tag\n");
                return -1;
            }
            if(search_field("bSort", &ptr_commit, &p_bSort) != 0)
            {
                fprintf(stderr, "can't find bSort field into commit tag\n");
                return -1;
            }
            if(get_string(p_bSort, ptr_commit.p_end, bSort, sizeof(bSort)) != 0)
            {
                fprintf(stderr, "can't read bSort field value into commit tag\n");
                return -1;
            }
            if(strcmp(bSort, "no_sort") == 0)
            {
                biquad_sort = BIQUAD_NO_SORT;
            }
            else if(strcmp(bSort, "up_sort") == 0)
            {
                // revert biquad_sort type to be compliant to matlab function zp2sos
                //biquad_sort = BIQUAD_UP_SORT;
                biquad_sort = BIQUAD_DOWN_SORT;
            }
            else if(strcmp(bSort, "down_sort") == 0)
            {
                // revert biquad_sort type to be compliant to matlab function zp2sos
                //biquad_sort = BIQUAD_DOWN_SORT;
                biquad_sort = BIQUAD_UP_SORT;
            }
            else
            {
                fprintf(stderr, "unknown %s bSort\n", bSort);
                return -1;
            }

            IIR_filter.p_biquad_cell_list = NULL;
            for(i = 0, nb_biquads = 0; i < nb_max_biquads; i++)
            {
                if(mOMX_ConfigAudioTransEqualizer_iireq[i].sBandIndex.nValue < NB_MAX_BIQUAD)
                {
                    printf("\nbiquad %d at %d Hz :\n", i + 1, nSamplingRate_Hz);
                    switch(mOMX_ConfigAudioTransEqualizer_iireq[i].eFilterType)
                    {
                        case OMX_AUDIO_Lowpass:
                            printf("\teFilterType                  = OMX_AUDIO_Lowpass\n");
                            break;
                        case OMX_AUDIO_Highpass:
                            printf("\teFilterType                  = OMX_AUDIO_Highpass\n");
                            break;
                        case OMX_AUDIO_Peaking:
                            printf("\teFilterType                  = OMX_AUDIO_Peaking\n");
                            break;
                        case OMX_AUDIO_Lowshelf:
                            printf("\teFilterType                  = OMX_AUDIO_Lowshelf\n");
                            break;
                        case OMX_AUDIO_Highshelf:
                            printf("\teFilterType                  = OMX_AUDIO_Highshelf\n");
                            break;
                        default:
                            break;
                    }
                    printf("\tnFilterGain                  = %d mB\n", (int) mOMX_ConfigAudioTransEqualizer_iireq[i].nFilterGain);
                    printf("\tnFilterCenterCutoffFrequency = %d Hz\n", (int) mOMX_ConfigAudioTransEqualizer_iireq[i].nFilterCenterCutoffFrequency);
                    printf("\tnFilterBandwidth             = %d Hz\n", (int) mOMX_ConfigAudioTransEqualizer_iireq[i].nFilterBandwidth);
                    mOMX_ConfigAudioTransEqualizer_iireq[i].sBandIndex.nMin = 0;
                    mOMX_ConfigAudioTransEqualizer_iireq[i].sBandIndex.nMax = nb_max_biquads - 1;
                    error = TEQ_SHAI_compute_biquad(&mOMX_ConfigAudioTransEqualizer_iireq[i], nSamplingRate_Hz, &IIR_filter, instance);
                    if(error != OMX_ErrorNone)
                    {
                        fprintf(stderr, "error = 0x%08X in TEQ_SHAI_compute_biquad() for biquad %d\n", error, i + 1);
                        return -1;
                    }
                    nb_biquads++;
                    printf("(%+12.9f %+12.9f.z^-1 %+12.9f.z^-2) / (1 %+12.9f.z^-1 %+12.9f.z^-2)\n",
                           IIR_filter.p_last_biquad_cell->num.q0 / IIR_filter.p_last_biquad_cell->den.q0,
                           IIR_filter.p_last_biquad_cell->num.q1 / IIR_filter.p_last_biquad_cell->den.q0,
                           IIR_filter.p_last_biquad_cell->num.q2 / IIR_filter.p_last_biquad_cell->den.q0,
                           IIR_filter.p_last_biquad_cell->den.q1 / IIR_filter.p_last_biquad_cell->den.q0,
                           IIR_filter.p_last_biquad_cell->den.q2 / IIR_filter.p_last_biquad_cell->den.q0);
                }
            }
            printf("\n\ncommit %s at %d Hz with biquad %s\n", sName, nSamplingRate_Hz, bSort);
            error = TEQ_SHAI_compute_IIR(nb_biquads, &IIR_filter, &IIR_filter_quantified, biquad_sort, nb_bits_quantif, instance);
            if(error != OMX_ErrorNone)
            {
                fprintf(stderr, "error = 0x%08X in TEQ_SHAI_compute_IIR()\n", error);
                return -1;
            }
            printf("\nEquivalent floating point IIR after %d bits quantization :\n", nb_bits_quantif);
            for(i = 0; i < nb_biquads; i++)
            {
                t_float coef_b = Pow2((t_float) (IIR_filter_quantified.biquad_cell[i].b_exp - 31));
                t_float b0     = ((t_float) IIR_filter_quantified.biquad_cell[i].b0) * coef_b;
                t_float b1     = ((t_float) IIR_filter_quantified.biquad_cell[i].b1) * coef_b;
                t_float b2     = ((t_float) IIR_filter_quantified.biquad_cell[i].b2) * coef_b;
                t_float a1     = ((t_float) IIR_filter_quantified.biquad_cell[i].a1) * Pow2(-30.0);
                t_float a2     = ((t_float) IIR_filter_quantified.biquad_cell[i].a2) * Pow2(-31.0);

                printf("(%+12.9f %+12.9f.z^-1 %+12.9f.z^-2) / (1 %+12.9f.z^-1 %+12.9f.z^-2)\n",
                       b0, b1, b2, a1, a2);
            }
            gain = ((t_float) IIR_filter_quantified.gain_mant) * Pow2((t_float) (IIR_filter_quantified.gain_exp - 31));
            printf("biquad_gain = %1.8f = %1.3f dB\n", gain, 20.0 * Log10(gain));

            i = 0;
            id_found = FALSE;
            ptr_end_FW_API_IIR.p_end = p_buf_ref;
            while(search_tag("FW_API_IIR", ptr_end_FW_API_IIR.p_end, &ptr_start_FW_API_IIR) == 0)
            {
                char *p_id_ref;
                char id_ref[100];

                if(search_tag("/FW_API_IIR", ptr_start_FW_API_IIR.p_end, &ptr_end_FW_API_IIR) != 0)
                {
                    fprintf(stderr, "can't find /FW_API_IIR tag\n");
                    return -1;
                }
                if(search_field("id", &ptr_start_FW_API_IIR, &p_id_ref) != 0)
                {
                    fprintf(stderr, "can't find id field into FW_API_IIR tag\n");
                    return -1;
                }
                if(get_string(p_id_ref, ptr_start_FW_API_IIR.p_end, id_ref, sizeof(id_ref)) != 0)
                {
                    fprintf(stderr, "can't read id field value into FW_API_IIR tag\n");
                    return -1;
                }
                if(strcmp(id_ref, id) == 0)
                {
                    id_found = TRUE;
                    break;
                }
            }
            if(id_found)
            {
                int       sName_ref_found;
                char      sName_ref[100];
                t_ptr_tag ptr_start_call, ptr_end_call;

                sName_ref_found = FALSE;
                ptr_end_call.p_end = ptr_start_FW_API_IIR.p_end;
                while(   (search_tag("call", ptr_end_call.p_end, &ptr_start_call) == 0)
                      && (ptr_start_call.p_end < ptr_end_FW_API_IIR.p_start))
                {
                    if(search_tag("/call", ptr_start_call.p_end, &ptr_end_call) != 0)
                    {
                        fprintf(stderr, "can't find /call tag\n");
                        return -1;
                    }
                    if(search_field("sName", &ptr_start_call, &p_sName) != 0)
                    {
                        fprintf(stderr, "can't find sName field into call tag\n");
                        return -1;
                    }
                    if(get_string(p_sName, ptr_start_call.p_end, sName_ref, sizeof(sName_ref)) != 0)
                    {
                        fprintf(stderr, "can't read sName field value into call tag\n");
                        return -1;
                    }
                    if(strcmp(sName_ref, sName) == 0)
                    {
                        printf("\nReference %s :\n", sName_ref);
                        sName_ref_found = TRUE;
                        break;
                    }
                }
                if(sName_ref_found)
                {
                    t_ptr_tag ptr_start_set_biquads, ptr_end_set_biquads, ptr_biquad, ptr_set_gain;
                    char      *p_gain;
                    t_float   gain_ref, global_gain_ref, global_gain, error, error_maxi;

                    if(   (search_tag("set_biquads", ptr_start_call.p_end, &ptr_start_set_biquads) != 0)
                       || (ptr_start_set_biquads.p_end >= ptr_end_call.p_start))
                    {
                        fprintf(stderr, "can't find set_biquads tag\n");
                        return -1;
                    }
                    if(   (search_tag("/set_biquads", ptr_start_set_biquads.p_end, &ptr_end_set_biquads) != 0)
                       || (ptr_end_set_biquads.p_end >= ptr_end_call.p_start))
                    {
                        fprintf(stderr, "can't find /set_biquads tag\n");
                        return -1;
                    }
                    if(   (search_tag("set_gain", ptr_start_call.p_end, &ptr_set_gain) != 0)
                       || (ptr_set_gain.p_end >= ptr_end_call.p_start))
                    {
                        fprintf(stderr, "can't find set_gain tag\n");
                        return -1;
                    }
                    if(search_field("value", &ptr_set_gain, &p_gain) != 0)
                    {
                        fprintf(stderr, "can't find value field into ptr_set_gain tag\n");
                        return -1;
                    }
                    global_gain_ref = gain_ref = atof(p_gain);
                    global_gain = gain;
                    error_maxi = Pow2((t_float) (4 - nb_bits_quantif));
                    ptr_biquad.p_end = ptr_start_call.p_end;
                    while(   (search_tag("biquad", ptr_biquad.p_end, &ptr_biquad) == 0)
                          && (ptr_biquad.p_end < ptr_end_set_biquads.p_start))
                    {
                        t_float coef_b = Pow2((t_float) (IIR_filter_quantified.biquad_cell[i].b_exp - 31));
                        t_float b0     = ((t_float) IIR_filter_quantified.biquad_cell[i].b0) * coef_b;
                        t_float b1     = ((t_float) IIR_filter_quantified.biquad_cell[i].b1) * coef_b;
                        t_float b2     = ((t_float) IIR_filter_quantified.biquad_cell[i].b2) * coef_b;
                        t_float a1     = ((t_float) IIR_filter_quantified.biquad_cell[i].a1) * Pow2(-30.0);
                        t_float a2     = ((t_float) IIR_filter_quantified.biquad_cell[i].a2) * Pow2(-31.0);
                        char    *p_b0, *p_b1, *p_b2, *p_a0, *p_a1, *p_a2;
                        t_float b0_ref, b1_ref, b2_ref, a0_ref, a1_ref, a2_ref;
                        t_float ratio_b0, ratio_b1, ratio_b2, ratio_a1, ratio_a2;

                        i++;

                        if(search_field("b0", &ptr_biquad, &p_b0) != 0)
                        {
                            fprintf(stderr, "can't find b0 field into biquad tag\n");
                            return -1;
                        }
                        b0_ref = atof(p_b0);
                        if(search_field("b1", &ptr_biquad, &p_b1) != 0)
                        {
                            fprintf(stderr, "can't find b1 field into biquad tag\n");
                            return -1;
                        }
                        b1_ref = atof(p_b1);
                        if(search_field("b2", &ptr_biquad, &p_b2) != 0)
                        {
                            fprintf(stderr, "can't find b2 field into biquad tag\n");
                            return -1;
                        }
                        b2_ref = atof(p_b2);
                        if(search_field("a0", &ptr_biquad, &p_a0) != 0)
                        {
                            fprintf(stderr, "can't find a0 field into biquad tag\n");
                            return -1;
                        }
                        a0_ref = atof(p_a0);
                        if(search_field("a1", &ptr_biquad, &p_a1) != 0)
                        {
                            fprintf(stderr, "can't find a1 field into biquad tag\n");
                            return -1;
                        }
                        a1_ref = atof(p_a1);
                        if(search_field("a2", &ptr_biquad, &p_a2) != 0)
                        {
                            fprintf(stderr, "can't find a2 field into biquad tag\n");
                            return -1;
                        }
                        a2_ref = atof(p_a2);

                        if((Fabs(b0_ref) < 0.0000000001) || (Fabs(b0) < 0.0000000001))
                        {
                            fprintf(stderr, "error : b0 = %1.12f, b0_ref = %1.12f\n", b0, b0_ref);
                            return -1;
                        }
                        else
                        {
                            ratio_b0 = b0_ref / b0;
                        }
                        if(fabs(b1 * ratio_b0) < 0.0000000001)
                        {
                            if(fabs(b1_ref) < 0.000000001)
                            {
                                ratio_b1 = 1.0;
                            }
                            else
                            {
                                fprintf(stderr, "error : b1 = %1.12f, b1_ref = %1.12f\n", b1, b1_ref);
                                return -1;
                            }
                        }
                        else
                        {
                            ratio_b1 = (b1_ref / b1) / ratio_b0;
                        }
                        if(fabs(b2 * ratio_b0) < 0.0000000001)
                        {
                            if(fabs(b2_ref) < 0.000000001)
                            {
                                ratio_b2 = 1.0;
                            }
                            else
                            {
                                fprintf(stderr, "error : b2 = %1.12f, b2_ref = %1.12f\n", b2, b2_ref);
                                return -1;
                            }
                        }
                        else
                        {
                            ratio_b2 = (b2_ref / b2) / ratio_b0;
                        }
                        if(fabs(a1) < 0.0000000001)
                        {
                            if(fabs(a1_ref) < 0.000000001)
                            {
                                ratio_a1 = 1.0;
                            }
                            else
                            {
                                fprintf(stderr, "error : a1 = %1.12f, a1_ref = %1.12f\n", a1, a1_ref);
                                return -1;
                            }
                        }
                        else
                        {
                            ratio_a1 = a1_ref / a1;
                        }
                        if(fabs(a2) < 0.0000000001)
                        {
                            if(fabs(a2_ref) < 0.000000001)
                            {
                                ratio_a2 = 1.0;
                            }
                            else
                            {
                                fprintf(stderr, "error : a2 = %1.12f, a2_ref = %1.12f\n", a2, a2_ref);
                                return -1;
                            }
                        }
                        else
                        {
                            ratio_a2 = a2_ref / a2;
                        }
                        global_gain_ref *= b0_ref;
                        global_gain     *= b0;
                        error            = Fabs(ratio_b1 - 1.0) + Fabs(ratio_b2 - 1.0) + Fabs(ratio_a1 - 1.0) + Fabs(ratio_a2 - 1.0);
                        printf("(%+12.9f %+12.9f.z^-1 %+12.9f.z^-2) / (1 %+12.9f.z^-1 %+12.9f.z^-2) = quantified biquad * %11.9f [1 %12.10f %12.10f] / [1 %12.10f %12.10f] : %s\n",
                               b0_ref, b1_ref, b2_ref, a1_ref, a2_ref,
                               ratio_b0, ratio_b1, ratio_b2, ratio_a1, ratio_a2,
                               (error > error_maxi ? "NOK" : "OK"));
                    }
                    printf("biquad_gain = %1.8f = %1.3f dB\n", gain_ref, 20.0 * Log10(gain_ref));
                    error = Fabs(global_gain_ref / global_gain - 1.0);
                    printf("\nbiquad_gain * prod[b0(i)] = %1.10f, biquad_gain_ref * prod[b0_ref(i)] = %1.10f : %s\n\n", global_gain, global_gain_ref, (error > ((t_float) nb_biquads) * error_maxi ? "NOK" : "OK"));
                }
                else
                {
                    printf("\nsName %s not found in reference\n", sName_ref);
                }
            }
            else
            {
                printf("\nid %s not found in reference\n", id);
            }
        }
    }

    free(p_buf_in);
    free(p_buf_ref);

#if DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0
    close_debug_TEQ_SHAI_file(instance);
#endif // DEBUG_TEQ_SHAI_CONFIG_LEVEL > 0

    return 0;
}

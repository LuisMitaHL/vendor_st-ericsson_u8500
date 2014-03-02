/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   make_pcm_tables.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TEQ_STREAMS_PATH "../../../streams/transducer_equalizer/streams/"



static void read_samples(FILE *file, short *buf, int n)
{
    int msb, lsb, sample, i;

    for(i = 0; i < n; i++)
    {
        msb    = fgetc(file) & 0xFF;
        lsb    = fgetc(file) & 0xFF;
        sample = (msb << 8) + (lsb << 0);
        buf[i] = (short) sample;
    }
}



static void make_table(FILE *p_file_table_c, FILE *p_file_table_h, char *p_table_name, char *p_file_name)
{
    char  *p_path_file_name;
    FILE  *p_file_sample;
    int   i, nb_samples;
    short *p_buf_samples;


    p_path_file_name = (char *) malloc(strlen(TEQ_STREAMS_PATH) + strlen(p_file_name) + 1);
    if(p_path_file_name == NULL)
    {
        fprintf(stderr, "can't allocate p_path_file_name\n");
        return;
    }
    strcpy(p_path_file_name, TEQ_STREAMS_PATH);
    strcat(p_path_file_name, p_file_name);

    p_file_sample = fopen(p_path_file_name, "rb");
    if(p_file_sample == NULL)
    {
        fprintf(stderr, "can't open \"%s\" input file\n", p_path_file_name);
        free(p_path_file_name);
        return;
    }
    free(p_path_file_name);

    fseek(p_file_sample, 0, SEEK_END);
    nb_samples = ftell(p_file_sample) / sizeof(short);
    rewind(p_file_sample);

    p_buf_samples = (short *) malloc(nb_samples * sizeof(short));
    if(p_buf_samples == NULL)
    {
        fprintf(stderr, "can't allocate p_buf_samples\n");
        fclose(p_file_sample);
        return;
    }

    read_samples(p_file_sample, p_buf_samples, nb_samples);
    fclose(p_file_sample);


    fprintf(p_file_table_h, "extern const int %s_size;\n", p_table_name, nb_samples);
    fprintf(p_file_table_h, "extern const short %s[%d];\n\n", p_table_name, nb_samples);

    fprintf(p_file_table_c, "const int %s_size = %d;\n", p_table_name, nb_samples);
    fprintf(p_file_table_c, "const short %s[%d] =\n{\n", p_table_name, nb_samples);
    for(i = 0; i < nb_samples; i++)
    {
        if((i % 16) == 0)
        {
            fprintf(p_file_table_c, "\t");
        }
        else
        {
            fprintf(p_file_table_c, " ");
        }
        fprintf(p_file_table_c, "0x%04X", ((int) p_buf_samples[i]) & 0xFFFF);
        if(i < nb_samples - 1)
        {
            fprintf(p_file_table_c, ",");
        }
        if((i % 16) == 15)
        {
            fprintf(p_file_table_c, "\n");
        }
    }
    if((nb_samples % 16) != 0)
    {
        fprintf(p_file_table_c, "\n");
    }
    fprintf(p_file_table_c, "};\n\n");

    free(p_buf_samples);
}



int main(void)
{
    FILE *p_file_c, *p_file_h;


    p_file_c = fopen("pcm_tables_CA9.c", "wb");
    p_file_h = fopen("pcm_tables_CA9.h", "wb");

    make_table(p_file_c, p_file_h, "pcm_in_bourm1_rx",                       "signals/bourm1_rx.pcm");
    make_table(p_file_c, p_file_h, "pcm_in_AirBatucada_44k",                 "signals/AirBatucada_44k.pcm");
    make_table(p_file_c, p_file_h, "pcm_ref_bourm1_rx_biquad_eq_CA9",        "reference/bourm1_rx_biquad_eq_CA9.pcm");
    make_table(p_file_c, p_file_h, "pcm_ref_AirBatucada_44k_biquad_eq1_CA9", "reference/AirBatucada_44k_biquad_eq1_CA9.pcm");
    make_table(p_file_c, p_file_h, "pcm_ref_AirBatucada_44k_biquad_eq2_CA9", "reference/AirBatucada_44k_biquad_eq2_CA9.pcm");
    make_table(p_file_c, p_file_h, "pcm_ref_bourm1_rx_FIR_eq_CA9",           "reference/bourm1_rx_FIR_eq_CA9.pcm");
    make_table(p_file_c, p_file_h, "pcm_ref_AirBatucada_44k_FIR_eq1_CA9",    "reference/AirBatucada_44k_FIR_eq1_CA9.pcm");
    make_table(p_file_c, p_file_h, "pcm_ref_AirBatucada_44k_FIR_eq2_CA9",    "reference/AirBatucada_44k_FIR_eq2_CA9.pcm");

    fclose(p_file_c);
    fclose(p_file_h);

    return 0;
}

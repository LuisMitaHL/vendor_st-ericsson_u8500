#include <stdio.h>
#include <string.h>

#include "mdrc5b.h"



#if defined(DEBUG_FILTERS_OUTPUT) || defined(DEBUG_COMPRESSOR_OUTPUT) || defined(DEBUG_LIMITER_OUTPUT)
int debug_cpt_samples;

static FILE    EXTMEM *p_debug_file;
#ifdef DEBUG_FILTERS_OUTPUT
static FILE    EXTMEM *p_debug_filter_output_file[MDRC5B_MAIN_CH_MAX][MDRC5B_SUBBAND_MAX];
#endif // DEBUG_FILTERS_OUTPUT
#ifdef DEBUG_COMPRESSOR_OUTPUT
static FILE    EXTMEM *p_debug_compressor_output_file[MDRC5B_MAIN_CH_MAX];
#endif // DEBUG_COMPRESSOR_OUTPUT
#ifdef DEBUG_LIMITER_OUTPUT
static FILE    EXTMEM *p_debug_limiter_output_file[MDRC5B_MAIN_CH_MAX];
#endif // DEBUG_LIMITER_OUTPUT

#ifdef __flexcc2__
#define DEBUG_BUFFER_SIZE 65536
static char    EXTMEM debug_buffer[DEBUG_BUFFER_SIZE];
static char    EXTMEM *p_debug_buffer;
#ifdef DEBUG_FILTERS_OUTPUT
static MMshort EXTMEM debug_filter_output_buffer[MDRC5B_MAIN_CH_MAX][MDRC5B_SUBBAND_MAX][DEBUG_BUFFER_SIZE];
static MMshort EXTMEM *p_debug_filter_output_buffer[MDRC5B_MAIN_CH_MAX][MDRC5B_SUBBAND_MAX];
#endif // DEBUG_FILTERS_OUTPUT
#ifdef DEBUG_COMPRESSOR_OUTPUT
static MMshort EXTMEM debug_compressor_output_buffer[MDRC5B_MAIN_CH_MAX][DEBUG_BUFFER_SIZE];
static MMshort EXTMEM *p_debug_compressor_output_buffer[MDRC5B_MAIN_CH_MAX];
#endif // DEBUG_LIMITER_OUTPUT
#ifdef DEBUG_LIMITER_OUTPUT
static MMshort EXTMEM debug_limiter_output_buffer[MDRC5B_MAIN_CH_MAX][DEBUG_BUFFER_SIZE];
static MMshort EXTMEM *p_debug_limiter_output_buffer[MDRC5B_MAIN_CH_MAX];
#endif // DEBUG_LIMITER_OUTPUT

static void debug_flush(void)
{
    char *p_char;

    for(p_char = debug_buffer; p_char < p_debug_buffer; p_char++)
    {
        fputc(*p_char, p_debug_file);
    }
    p_debug_buffer = debug_buffer;
    fflush(p_debug_file);
}

#ifdef DEBUG_FILTERS_OUTPUT
static void debug_filter_output_flush(int ch, int bidx)
{
    MMshort sample, *p_sample;

    for(p_sample = debug_filter_output_buffer[ch][bidx]; p_sample < p_debug_filter_output_buffer[ch][bidx]; p_sample++)
    {
        sample = *p_sample;
        fputc((sample >> 8) & 0xFF, p_debug_filter_output_file[ch][bidx]);
        fputc( sample       & 0xFF, p_debug_filter_output_file[ch][bidx]);
    }
    p_debug_filter_output_buffer[ch][bidx] = debug_filter_output_buffer[ch][bidx];
    fflush(p_debug_filter_output_file[ch][bidx]);
}
#endif // DEBUG_FILTERS_OUTPUT

#ifdef DEBUG_COMPRESSOR_OUTPUT
static void debug_compressor_output_flush(int ch)
{
    MMshort sample, *p_sample;

    for(p_sample = debug_compressor_output_buffer[ch]; p_sample < p_debug_compressor_output_buffer[ch]; p_sample++)
    {
        sample = *p_sample;
        fputc((sample >> 8) & 0xFF, p_debug_compressor_output_file[ch]);
        fputc( sample       & 0xFF, p_debug_compressor_output_file[ch]);
    }
    p_debug_compressor_output_buffer[ch] = debug_compressor_output_buffer[ch];
    fflush(p_debug_compressor_output_file[ch]);
}
#endif // DEBUG_COMPRESSOR_OUTPUT

#ifdef DEBUG_LIMITER_OUTPUT
void debug_limiter_output_flush(int ch)
{
    MMshort sample, *p_sample;

    for(p_sample = debug_limiter_output_buffer[ch]; p_sample < p_debug_limiter_output_buffer[ch]; p_sample++)
    {
        sample = *p_sample;
        fputc((sample >> 8) & 0xFF, p_debug_limiter_output_file[ch]);
        fputc( sample       & 0xFF, p_debug_limiter_output_file[ch]);
    }
    p_debug_limiter_output_buffer[ch] = debug_limiter_output_buffer[ch];
    fflush(p_debug_limiter_output_file[ch]);
}
#endif // DEBUG_LIMITER_OUTPUT
#endif // __flexcc2__

void debug_write_string(char *string)
{
#ifdef __flexcc2__
    if(p_debug_buffer + strlen(string) >= debug_buffer + DEBUG_BUFFER_SIZE)
    {
        debug_flush();
    }
    p_debug_buffer += sprintf(p_debug_buffer, "%s", string);
#else // __flexcc2__
    fprintf(p_debug_file, "%s", string);
#endif // __flexcc2__
}

#ifdef DEBUG_FILTERS_OUTPUT
void debug_write_filter_output(int ch, int bidx, MMshort sample)
{
    char string[10];

#ifndef __flexcc2__
#ifdef SAMPLES_24_BITS
    sample = (sample << 8) >> 8;
#else // SAMPLES_24_BITS
    sample = (sample + (sample < 0x7FFFFF80 ? (1 << 7) : 0)) >> 8;
#endif // SAMPLES_24_BITS
#endif // !__flexcc2__
    sprintf(string, "0x%06X ", sample & 0xFFFFFF);
    debug_write_string(string);

    sample = (sample + (sample < 0x7FFF80 ? (1 << 7) : 0)) >> 8;
#ifdef __flexcc2__
    if(p_debug_filter_output_buffer[ch][bidx] + 1 >= debug_filter_output_buffer[ch][bidx] + DEBUG_BUFFER_SIZE)
    {
        debug_filter_output_flush(ch, bidx);
    }
    *p_debug_filter_output_buffer[ch][bidx]++ = sample;
#else // __flexcc2__
    fputc((sample >> 8) & 0xFF, p_debug_filter_output_file[ch][bidx]);
    fputc( sample       & 0xFF, p_debug_filter_output_file[ch][bidx]);
#endif // __flexcc2__
}
#endif // DEBUG_FILTERS_OUTPUT

#ifdef DEBUG_COMPRESSOR_OUTPUT
void debug_write_compressor_output(int ch, MMshort sample)
{
    char string[10];

#ifndef __flexcc2__
#ifdef SAMPLES_24_BITS
    sample = (sample << 8) >> 8;
#else // SAMPLES_24_BITS
    sample = (sample + (sample < 0x7FFFFF80 ? (1 << 7) : 0)) >> 8;
#endif // SAMPLES_24_BITS
#endif // !__flexcc2__
    sprintf(string, "0x%06X ", sample & 0xFFFFFF);
    debug_write_string(string);

    sample = (sample + (sample < 0x7FFF80 ? (1 << 7) : 0)) >> 8;
#ifdef __flexcc2__
    if(p_debug_compressor_output_buffer[ch] + 1 >= debug_compressor_output_buffer[ch] + DEBUG_BUFFER_SIZE)
    {
        debug_compressor_output_flush(ch);
    }
    *p_debug_compressor_output_buffer[ch]++ = sample;
#else // __flexcc2__
    fputc((sample >> 8) & 0xFF, p_debug_compressor_output_file[ch]);
    fputc( sample       & 0xFF, p_debug_compressor_output_file[ch]);
#endif // __flexcc2__
}
#endif // DEBUG_COMPRESSOR_OUTPUT

#ifdef DEBUG_LIMITER_OUTPUT
void debug_write_limiter_output(int ch, MMshort sample)
{
    char string[10];

#ifndef __flexcc2__
#ifdef SAMPLES_24_BITS
    sample = (sample << 8) >> 8;
#else // SAMPLES_24_BITS
    sample = (sample + (sample < 0x7FFFFF80 ? (1 << 7) : 0)) >> 8;
#endif // SAMPLES_24_BITS
#endif // !__flexcc2__
    sprintf(string, "0x%06X ", sample & 0xFFFFFF);
    debug_write_string(string);

    sample = (sample + (sample < 0x7FFF80 ? (1 << 7) : 0)) >> 8;
#ifdef __flexcc2__
    if(p_debug_limiter_output_buffer[ch] + 1 >= debug_limiter_output_buffer[ch] + DEBUG_BUFFER_SIZE)
    {
        debug_limiter_output_flush(ch);
    }
    *p_debug_limiter_output_buffer[ch]++ = sample;
#else // __flexcc2__
    fputc((sample >> 8) & 0xFF, p_debug_limiter_output_file[ch]);
    fputc( sample       & 0xFF, p_debug_limiter_output_file[ch]);
#endif // __flexcc2__
}
#endif // DEBUG_LIMITER_OUTPUT


char *debug_samples_open(int nb_chan, int nb_bands, char *filename_mainout)
{
    static char string_error[200];
    char debug_filename[200], *p_char;
    int  ch;
#ifdef DEBUG_FILTERS_OUTPUT
    int  bidx;
#endif // DEBUG_FILTERS_OUTPUT

    debug_cpt_samples = 0;
    p_char = strrchr(filename_mainout, '.');
    if(p_char)
    {
        *p_char = 0;
    }
    sprintf(debug_filename, "%s.txt", filename_mainout);
    p_debug_file = fopen(debug_filename, "wb");
    if(p_debug_file == NULL)
    {
        sprintf(string_error, "can't open %s output file !\n", debug_filename);
        return string_error;
    }
#ifdef __flexcc2__
    p_debug_buffer = debug_buffer;
#endif // __flexcc2__
    for(ch = 0; ch < nb_chan; ch++)
    {
#ifdef DEBUG_FILTERS_OUTPUT
        for(bidx = 0; bidx < nb_bands; bidx++)
        {
            sprintf(debug_filename, "%s_ch%d_bnd%d.pcm", filename_mainout, ch, bidx);
            p_debug_filter_output_file[ch][bidx] = fopen(debug_filename, "wb");
            if(p_debug_filter_output_file[ch][bidx] == NULL)
            {
                sprintf(string_error, "can't open %s output file !\n", debug_filename);
                return string_error;
            }
#ifdef __flexcc2__
            p_debug_filter_output_buffer[ch][bidx] = debug_filter_output_buffer[ch][bidx];
#endif // __flexcc2__
        }
#endif // DEBUG_FILTERS_OUTPUT
#ifdef DEBUG_COMPRESSOR_OUTPUT
        sprintf(debug_filename, "%s_comp_ch%d.pcm", filename_mainout, ch);
        p_debug_compressor_output_file[ch] = fopen(debug_filename, "wb");
        if(p_debug_compressor_output_file[ch] == NULL)
        {
            sprintf(string_error, "can't open %s output file !\n", debug_filename);
            return string_error;
        }
#ifdef __flexcc2__
        p_debug_compressor_output_buffer[ch] = debug_compressor_output_buffer[ch];
#endif // __flexcc2__
#endif // DEBUG_COMPRESSOR_OUTPUT

#ifdef DEBUG_LIMITER_OUTPUT
        sprintf(debug_filename, "%s_limit_ch%d.pcm", filename_mainout, ch);
        p_debug_limiter_output_file[ch] = fopen(debug_filename, "wb");

        if(p_debug_limiter_output_file[ch] == NULL)
        {
            sprintf(string_error, "can't open %s output file !\n", debug_filename);
            return string_error;
        }
#ifdef __flexcc2__
        p_debug_limiter_output_buffer[ch] = debug_limiter_output_buffer[ch];
#endif // __flexcc2__
#endif // DEBUG_LIMITER_OUTPUT
    }

    return NULL;
}


void debug_samples_close(int nb_chan, int nb_bands)
{
    int ch;
#ifdef DEBUG_FILTERS_OUTPUT
    int bidx;
#endif // DEBUG_FILTERS_OUTPUT

#ifdef __flexcc2__
    debug_flush();
#endif // __flexcc2__
    fclose(p_debug_file);
    for(ch = 0; ch < nb_chan; ch++)
    {
#ifdef DEBUG_FILTERS_OUTPUT
        for(bidx = 0; bidx < nb_bands; bidx++)
        {
#ifdef __flexcc2__
            debug_filter_output_flush(ch, bidx);
#endif // __flexcc2__
            fclose(p_debug_filter_output_file[ch][bidx]);
        }
#endif // DEBUG_FILTERS_OUTPUT
#ifdef DEBUG_COMPRESSOR_OUTPUT
#ifdef __flexcc2__
        debug_compressor_output_flush(ch);
#endif // __flexcc2__
        fclose(p_debug_compressor_output_file[ch]);
#endif // DEBUG_COMPRESSOR_OUTPUT

#ifdef DEBUG_LIMITER_OUTPUT
#ifdef __flexcc2__
        debug_limiter_output_flush(ch);
#endif // __flexcc2__
        fclose(p_debug_limiter_output_file[ch]);
#endif // DEBUG_LIMITER_OUTPUT
    }
}

#endif // DEBUG_FILTERS_OUTPUT || DEBUG_COMPRESSOR_OUTPUT || DEBUG_LIMITER_OUTPUT

#ifndef _debug_samples_h_
#define _debug_samples_h_

//#define DEBUG_FILTERS_OUTPUT
//#define DEBUG_COMPRESSOR_OUTPUT
//#define DEBUG_LIMITER_OUTPUT

#if defined(DEBUG_FILTERS_OUTPUT) || defined(DEBUG_COMPRESSOR_OUTPUT) || defined(DEBUG_LIMITER_OUTPUT)

#define DEBUG_CPT_MIN 0
#define DEBUG_CPT_MAX 1000000

/*
//IIR sink mono
#define DEBUG_CPT_MIN 223296
#define DEBUG_CPT_MAX 223535
*/

extern int debug_cpt_samples;

extern void debug_write_string(char *string);

#ifdef DEBUG_FILTERS_OUTPUT
extern void debug_write_filter_output(int ch, int bidx, MMshort sample);
#endif // DEBUG_FILTERS_OUTPUT

#ifdef DEBUG_COMPRESSOR_OUTPUT
extern void debug_write_compressor_output(int ch, MMshort sample);
#endif // DEBUG_COMPRESSOR_OUTPUT

#ifdef DEBUG_LIMITER_OUTPUT
extern void debug_write_limiter_output(int ch, MMshort sample);
#endif // DEBUG_LIMITER_OUTPUT

extern char *debug_samples_open (int nb_chan, int nb_bands, char *filename_mainout);
extern void  debug_samples_close(int nb_chan, int nb_bands);

#endif // DEBUG_FILTERS_OUTPUT || DEBUG_COMPRESSOR_OUTPUT || DEBUG_LIMITER_OUTPUT

#endif // _debug_samples_h_

#ifndef _drc_h_
#define _drc_h_

#ifdef DRC

#include "drc_defs.txt"

#define DRC_REF_LEVEL	(27*4)	/* -27 dB below FS (typical for movies) */
#define MAX_DRC_THREADS	1	/* could be more! */
#define MAX_CHAN	(FChans + SChans + BChans + LChans + ICChans)
#define MAX_DRC_BANDS	(1<<LEN_DRC_BAND_INCR)

#ifndef BIT_FIELDS

typedef struct {
    unsigned int             drc_sgn;
    unsigned int             drc_mag;
} DRC_Gain;

typedef struct {
    int             excl_chn_present;
    int             prog_ref_level_present;
    int             prog_ref_level;
    int             num_bands;
    int             excl_chn_mask[MAX_CHAN];
    int             band_top[MAX_DRC_BANDS];
    DRC_Gain        drc_gain[MAX_DRC_BANDS];
} DRC_Bitstream;

#else

typedef struct { 
    unsigned int             drc_sgn:LEN_DRC_SGN;
    unsigned int             drc_mag:LEN_DRC_MAG;
} DRC_Gain;

typedef struct {
    unsigned int             excl_chn_present:1;
    unsigned int             prog_ref_level_present:1;
    unsigned int             prog_ref_level:LEN_DRC_PL;
    int             num_bands;
    int             excl_chn_mask[MAX_CHAN];
    int             band_top[MAX_DRC_BANDS];
    DRC_Gain        drc_gain[MAX_DRC_BANDS];
} DRC_Bitstream;

#endif /* BIT_FIELDS */


typedef struct {
    int             num_bands;
    int             band_top[MAX_DRC_BANDS];
    DRC_Gain        drc_gain[MAX_DRC_BANDS];
} DRC_Info;


extern void            aac_apply_drc(MC_Info * mip, Float * coef,
				     int *exponent, int ch);
extern void            aac_set_drc(int enable, int hi, int lo);
extern void            aac_reset_drc(void);
extern void            aac_map_drc_channels(MC_Info * mip);
extern void            aac_init_drc(void);
#ifdef BUFFER_ALLOCATION_IN_SCRATCH_BUFFER
extern int			   aac_dynamic_range_info(Float *aac_scratch_buffer);
#else
extern int             aac_dynamic_range_info(void);
#endif

#endif /* DRC */

#endif /* Do not edit below this point */

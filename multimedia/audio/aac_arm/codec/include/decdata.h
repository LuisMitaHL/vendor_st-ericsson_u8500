#ifndef _decdata_h_
#define _decdata_h_

/* all variables are now fields of AAC_DECODER_STRUCT_T */

extern SR_Info  const SR_MEM       aac_samp_rate_info[(1 << LEN_SAMP_IDX)]; 
extern int      const SR_MEM       aac_fe_samp_rate_lut[(1 << LEN_SAMP_IDX)];

extern int const SR_MEM
aac_tns_max_bands_tbl[(1 << LEN_SAMP_IDX)][4];

extern int const SR_MEM
aac_pred_max_bands_tbl[(1 << LEN_SAMP_IDX)];

SR_Info SR_MEM *fn_aac_samp_rate_info(void); 
int SR_MEM *fn_aac_fe_samp_rate_lut(void);
int SR_MEM *fn_aac_pred_max_bands_tbl(void);

#endif /* Do not edit below this point */

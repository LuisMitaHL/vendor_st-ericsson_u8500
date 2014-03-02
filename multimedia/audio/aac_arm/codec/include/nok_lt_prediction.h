/*****************************************************************************
 *                                                                           *
SC 29 Software Copyright Licencing Disclaimer:

This software module was originally developed by
Nokia in the course of development of the MPEG-2 AAC/MPEG-4 
Audio standard ISO/IEC13818-7, 14496-1, 2 and 3.
This software module is an implementation of a part
of one or more MPEG-2 AAC/MPEG-4 Audio tools as specified by the
MPEG-2 aac/MPEG-4 Audio standard. ISO/IEC  gives users of the
MPEG-2aac/MPEG-4 Audio standards free license to this software module
or modifications thereof for use in hardware or software products
claiming conformance to the MPEG-2 aac/MPEG-4 Audio  standards. Those
intending to use this software module in hardware or software products
are advised that this use may infringe existing patents. The original
developer of this software module, the subsequent
editors and their companies, and ISO/IEC have no liability for use of
this software module or modifications thereof in an
implementation. Copyright is not released for non MPEG-2 aac/MPEG-4
Audio conforming products. The original developer retains full right to
use the code for the developer's own purpose, assign or donate the code to a
third party and to inhibit third party from using the code for non
MPEG-2 aac/MPEG-4 Audio conforming products. This copyright notice
must be included in all copies or derivative works.
Copyright (c)1997.  

***************************************************************************/

#ifndef NOK_LT_PREDICTION_H_
#define NOK_LT_PREDICTION_H_

extern void 
nok_init_lt_pred (NOK_LT_PRED_STATUS * lt_status);

#ifdef ARM

extern void
nok_lt_predict(Info *info, AAC_DECODER_STRUCT_T *p_global,
			   int *current_frame, Word16 wn,
			   Word16 ch, MC_Info aac_mc_info);

#else
extern void
nok_lt_predict(Info *info, AAC_DECODER_STRUCT_T *p_global,
			   Float *current_frame,int *exponent, Word16 wn,
			   Word16 ch, MC_Info aac_mc_info);
#endif
extern void
nok_lt_update(NOK_LT_PRED_STATUS *lt_status, Float *time_signal, 
	      Float *overlap_signal);

extern void 
nok_lt_decode(void *hBs,int max_sfb, int *sbk_prediction_used, 
	      int *sfb_prediction_used, Float *weight, int *delay);

#endif /* NOK_LT_PREDICTION_H_ */

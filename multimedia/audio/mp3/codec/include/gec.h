

#ifndef _gec_h_
#define _gec_h_

/***************************************************************************/
/* Author       : Audio Firmware Team                                      */
/* Function     : gec_init                                                 */
/* Input        : GEC Struct                                               */
/*              : generic_err_conc : flag set by higher layer              */
/*              : max_nb_chan       				                       */
/*				: max_samp_freq enum									   */
/*              : max_nb_samples   : size max for a block                  */
/* Output       :                                                          */
/* Return       : error status                                             */
/* Objective    : error concealment structure allocation                   */
/*                and initialization (constant values)                     */
/***************************************************************************/
CODEC_INIT_T GEC_init(GEC_CONTROL_STRUCT_T *p_gen_err_conc, 
                      int generic_err_conc,
					  Word16 max_nb_chan, 
					  Word16 max_samp_freq,
                      int max_nb_samples_in_word,
					  int sample_size_16);


/***************************************************************************/
/* Author       : Audio Firmware Team                                      */
/* Function     : gec_free                                                 */
/* Input        : GEC Struct                                               */
/* Output       :                                                          */
/* Return       : error status                                             */
/* Objective    : error concealment structure allocation                   */
/*                and initialization (constant values)                     */
/***************************************************************************/
void GEC_free(GEC_CONTROL_STRUCT_T *p_gen_err_conc);

/***************************************************************************/
/* Author       : Audio Firmware Team                                      */
/* Function     : gec_main_process                                         */
/* Input        : GEC Struct                                               */
/*              : p_input          : pointer to input buffer               */
/*              : p_output         : pointer to output buffer              */
/*              : generic_err_conc : flag set by higher layer              */
/*              : error            : frame is erronous                     */
/*              : block_size       : size of the block to process          */
/*              : chans_nb         : nb of the channels to process         */
/* Output       :                                                          */
/* Return       : error status                                             */
/* Objective    : error concealment algorithm main                         */
/***************************************************************************/
int GEC_main_process(GEC_CONTROL_STRUCT_T *p_gen_err_conc,
                     int *p_input,
                     int *p_output,
                     int generic_err_conc,
                     int error,
                     int block_size,
					 int chans_nb);


#endif /* _gec_h_ */



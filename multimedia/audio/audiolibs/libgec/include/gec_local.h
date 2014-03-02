/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _gec_local_h_
#define _gec_local_h_

#define PREVENT_SATURATION_INTO_OVERLAP

#define MODE      24
#define MIN_SHORT (0x800000L)
#define MAX_SHORT (0x7FFFFFL)
#define ROUNDVAL (0x400000L)

/* error concealment state */
typedef enum
{
    STATE_NO_ERROR,
    STATE_LESS_THAN_10MS_ERROR,
    STATE_BETWEEN_10MS_AND_20MS_ERROR,
    STATE_BETWEEN_20MS_AND_30MS_ERROR,
    STATE_BETWEEN_30MS_AND_60MS_ERROR,
    STATE_MORE_THAN_60MS_ERROR,
    STATE_END_OF_ERROR
} t_state;


/* circular buffer description structure */
typedef struct
{
    int     read_index;
    int     write_index;
    int     size;
    int *p_buf;
} t_circ_buf;


/* error concealment algo-structure  for one channel */
typedef struct
{
    t_state    state;

    int        block_size;
    int        delay;
    int        duration_10ms;
    int        duration_20ms;
    int        duration_30ms;
    int        duration_60ms;
    int        duration_4ms;
	int        step_corr;
	int		   sample_size_16;
    int    	   cpt_sample;
    int    	   error_duration;

    int    gain;
    int    gain_step;

    int    ola_coef;
    int    ola_coef_step;

    int        final_ola_duration;
    int    final_ola_coef;
    int    final_ola_coef_step;

    int        min_corr_offset;
    int        nb_corr;
    int        pitch;
    int        quarter_pitch;
    int        pitch_offset;
    int        current_pitch_copy_size;
    int        next_pitch_copy_size;
    int    	   *p_alloc_pitch_buf;
    int        *p_pitch_buf;
    int        *p_current_pitch_begin;
    int        *p_next_pitch_begin;
    int        *p_current_pitch_end;

    t_circ_buf circ_buf;
} t_error_conceal_struct;

// Enum that defines cuurent status of generic error concealment for a decoder.
typedef enum
{
  GEC_STATUS_UNKNOWN,
  GEC_STATUS_UNINITIALIZED,
  GEC_STATUS_ACTIVATED,
  GEC_STATUS_DISACTIVATED
} GEC_STATUS_T;


// Structure to control the generic error concealment in case the codec is a decoder.
typedef struct
{
  int                       nb_chan_alloc;        // Number of channel to process max
  GEC_STATUS_T              gen_err_conc_status;  // indicates current status
  t_error_conceal_struct    *p_err_conc_chan1;    // Pointer on struct used for generic error concealment channel 1
  t_error_conceal_struct    *p_err_conc_chan2;    // Pointer on struct used for generic error concealment channel 2
} GEC_CONTROL_STRUCT_T;


/***************************************************************************/
/* Author       : Michel MENU                                              */
/* Function     : init_error_conceal                                       */
/* Input        : block_size                                               */
/*              : sampling_frequency                                       */
/* Output       :                                                          */
/* Return       : pointer to t_error_conceal_struct allocated structure    */
/*                if OK or NULL if NOK                                     */
/* Objective    : error concealment structure allocation                   */
/*                and initialization (constant values)                     */
/***************************************************************************/
extern t_error_conceal_struct *init_error_conceal(int block_size, int coded_frequency,int sample_size_16);


/***************************************************************************/
/* Author       : Michel MENU                                              */
/* Function     : reset_error_conceal                                      */
/* Input        : p_error_conceal_struct : pointer to allocated error      */
/*                                         error concealment structure     */
/* Output       :                                                          */
/* Return       :                                                          */
/* Objective    : error concealment structure initialization               */
/*                (variables values)                                       */
/***************************************************************************/
extern void reset_error_conceal(t_error_conceal_struct *p_error_conceal_struct);


/***************************************************************************/
/* Author       : Michel MENU                                              */
/* Function     : free_error_conceal                                       */
/* Input        : p_error_conceal_struct : pointer to allocated error      */
/*                                         error concealment structure     */
/* Output       :                                                          */
/* Return       :                                                          */
/* Objective    : error concealment structure deallocation                 */
/***************************************************************************/
extern void free_error_conceal(t_error_conceal_struct *p_error_conceal_struct);

/***************************************************************************/
/* Author       : Audio Firmware team                                      */
/* Function     : main_error_conceal_stereo                                */
/* Input        : p_err_conc_chan1 : pointer to allocated error            */
/*                                   error concealment structure for chan1 */
/*              : p_err_conc_chan2 : pointer to allocated error            */
/*                                   error concealment structure for chan2 */
/*              : p_input                : pointer to input sample buffer  */
/*              : p_output               : pointer to output sample buffer */
/*              : error                  : good or bad input buffer        */
/*              : block_size             : if 0, use block size given into */
/*                                               init_error_conceal        */
/*                                               routine                   */
/*                                         else, use this block size only  */
/*                                               for this block            */
/* Output       : error concealed buffers                                  */
/* Return       : 1 if OK, 0 if NOK                                        */
/* Objective    : error concealment algorithm corresponding to ITU G711    */
/*                appendix 1 (09/99) generalized to any sampling frequency */
/*                and any block size :                                     */
/*                    processes block_size input samples and produces      */
/*                    block_size output samples                            */
/***************************************************************************/
int main_error_conceal_stereo(	t_error_conceal_struct	*p_err_conc_chan1, 
								t_error_conceal_struct	*p_err_conc_chan2,
                       			int                 	*p_input,
                      			int                 	*p_output,
                       			int                    	error,
                       			int                   	block_size);

/***************************************************************************/
/* Author       : Michel MENU                                              */
/* Function     : main_error_conceal                                       */
/* Input        : p_error_conceal_struct : pointer to allocated error      */
/*                                         error concealment structure     */
/*              : p_input                : pointer to input sample buffer  */
/*              : p_output               : pointer to output sample buffer */
/*              : error                  : good or bad input buffer        */
/*              : block_size             : if 0, use block size given into */
/*                                               init_error_conceal        */
/*                                               routine                   */
/*                                         else, use this block size only  */
/*                                               for this block            */
/*              : step_size is used to differentiate mono and stereo input */
/*                                                                         */
/* Output       : error concealed buffer                                   */
/* Return       : 1 if OK, 0 if NOK                                        */
/* Objective    : error concealment algorithm corresponding to ITU G711    */
/*                appendix 1 (09/99) generalized to any sampling frequency */
/*                and any block size :                                     */
/*                    processes block_size input samples and produces      */
/*                    block_size output samples                            */
/***************************************************************************/
extern int main_error_conceal(	t_error_conceal_struct	*p_error_conceal_struct,
                                int  					*p_input,
                                int	    				*p_output,
                                int						error,
                                int						block_size,
					            int						step_size);
#endif /* _gec_local_h_ */



/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author
*/
/*****************************************************************************/
#ifndef INCLUSION_GUARD_NOISE_REDUCTION_H
#define INCLUSION_GUARD_NOISE_REDUCTION_H

//#include "t_spl.h"
#include "speech_enhancement.h"

#if 0
/** Type definition for a signed 16 bit data entity. */
typedef signed short sint16;
/** Type definition for an unsigned 16 bit data entity. */
typedef unsigned short uint16;
/** Type definition for a signed 32 bit data entity. */
typedef signed long sint32;
/** Type definition for an unsigned 32 bit data entity. */
typedef unsigned long uint32;
#endif


/** Type definition for a boolean/logical value */
typedef int boolean;

#ifndef TRUE
/** Value representing the boolean/logical value false. */
#define FALSE 0
/** Value representing the boolean/logical value true. */
#define TRUE (!FALSE)
#endif


typedef struct noise_reduction_handle_t noise_reduction_handle_t;

noise_reduction_handle_t* noise_reduction_create(void);
boolean                   noise_reduction_open(noise_reduction_handle_t *handle_p, int channels,
                                               int samplerate);
void                      noise_reduction_set_config(noise_reduction_handle_t *handle_p, SE_NrParameters_t *params_p);
void                      noise_reduction_process(noise_reduction_handle_t *handle_p, short **in_buf_p, short **out_buf_p);
void                      noise_reduction_reset(noise_reduction_handle_t *handle_p);
void                      noise_reduction_destroy(noise_reduction_handle_t **handle_pp);

#endif //INCLUSION_GUARD_NOISE_REDUCTION_H

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _aep_h_
#define _aep_h_

#ifdef __flexcc2__
#define AEP_EXTERN EXTMEM
#else
#define AEP_EXTERN
#endif /* end __flexcc2__ */


#include "audiolibs_common.h"
#include "fwmalloc.h"
#include "vector.h"
#include <stdlib.h>
#ifdef __flexcc2__
#include "alloc.h"
#endif


/****************************************************************/
/****************************************************************/
#define INVSQROOT2 FORMAT_FLOAT(7.07106781e-01, MAXVAL)

/****************************************************************/
/* DATA PACKET                                                  */
/****************************************************************/
/******************************************/
/**
 * \brief Alert type
 *
 ******************************************/
typedef enum
{
  AEP_ALERT_TYPE_AUDIO_VISU,
  AEP_ALERT_TYPE_DTMF,
  AEP_ALERT_TYPE_TRANSDUCER_EQ,
  AEP_ALERT_TYPE_WBNB_NAEC,
  AEP_ALERT_TYPE_ERROR
} AEP_ALERT_TYPE_T;

/******************************************/
/**
 * \brief Status of a data packet
 *
 ******************************************/
typedef enum
{
  AEP_PACKET_STATUS_FREE,
  AEP_PACKET_STATUS_USED,
  AEP_PACKET_STATUS_TO_BE_DELETED = 2
} AEP_PACKET_STATUS_T;

/****************************************************************/
/**
 * \brief Format of a packet
 ****************************************************************/
typedef enum
 {
   AEP_SAMPLE_CHANNEL_UNKNOWN,
   AEP_SAMPLE_CHANNEL_MONO,
   AEP_SAMPLE_CHANNEL_STEREO,
   AEP_SAMPLE_CHANNEL_DUAL_MONO
 } AEP_TYPE_CHANNEL_MODE;

/****************************************************************/
/**
 * \brief Mix or Mute
 ****************************************************************/
typedef enum
 {
   AEP_DATA_MIXED,
   AEP_DATA_MUTED
 } AEP_MIXMUTE_T;



/****************************************************************/
/****************************************************************/
/**
 * \brief Structure of a data packet
 ****************************************************************/
typedef struct AEP_PACKET_T
{
  AEP_PACKET_STATUS_T status;     //!<Status of the packet
  int *buf_ptr;                   //!<Poiner of buffer
  t_saa_sample_freq sample_freq;  //!<Sample freq
  AEP_TYPE_CHANNEL_MODE chan_nb;  //!<Number of channel
  int nb_valid_sample;            //!<Number of valid sample in the buffer
  int packet_id;                  //!<Id of the position who owns the packet
  AEP_MIXMUTE_T MixMute;          //!<
  int PortId;                     //!<Id of the iniput Port which (help to) generated this packet
} AEP_PACKET_T;


/****************************************************************/
/****************************************************************/
/**
 * \brief Type of effect.
 *
 * - NO_BL_WORKER : The effect can work at sample rate, and resuse input
 *                  packet as output
 * - BL_WORKER    : The effect works per block of fixed size and/or cannot
 *                  reuse input as output
 ****************************************************************/
typedef enum
{
    AEP_NO_BL_WORKER,
    AEP_BL_WORKER
} AEP_WORKER_T;

/************************************************************************/
/************************************************************************/
/**
 * \brief Command send to effect
 *
 * Each command may be composed of 3 basic command:
 * - LOAD_PAK : indicates that the table of input buffer is valid,
 *              and that effect should read it
 * - GIVE_PAK : indicates that the table of output buffer is valid,
 *              and that effect should writte it
 * - FLUSH    : indicates that internal buffer should be flushed. This
 *              command is ALWAYS associated with LOAD_PAK, and it means
 *              that this is the last input packet(s)
 *
 * Other fields describe possible command (combination of basic command)
 *
 ***********************************************************************/
typedef enum
{
  /* base of the command */
  AEP_COMPO_NO_CMD        = 0,
  AEP_COMPO_LOAD_PAK_CMD  = 1,
  AEP_COMPO_GIVE_PAK_CMD  = (1<<1),
  AEP_COMPO_FLUSH_EOF     = (1<<2),
  /* the following enum are used for debug */
  AEP_COMPO_LOAD_AND_GIVE_CMD  = AEP_COMPO_LOAD_PAK_CMD | AEP_COMPO_GIVE_PAK_CMD,
  AEP_COMPO_FLUSH_AND_GIVE_CMD = AEP_COMPO_LOAD_PAK_CMD | AEP_COMPO_GIVE_PAK_CMD | AEP_COMPO_FLUSH_EOF,
  AEP_COMPO_FLUSH_CMD          = AEP_COMPO_LOAD_PAK_CMD | AEP_COMPO_FLUSH_EOF
}AEP_COMMAND_T;


/***********************************************************************/
/***********************************************************************/
/**
 * \brief Effect answer
 *
 * Each answer may be composed of 7 basic answers
 * - PAK_LOADED     : indicates that input buffer have been read
 *                    It should only be set for answer to effect_main function
 * - PAK_GIVEN      : indicates that output buffer have been written
 *                    It should only be set for answer to effect_main function
 * - CONFIG_APPLIED : indicates that config has been taken into account.
 *                    It should only be set for answer to effect_configure function
 * - FLUSHED        : Indicates that internal buffer have been flushed.
 *                    It should only be set for answer to effect_main function, and MUST
 *                    always be assocaited with PAK_GIVEN
 * - ERROR          : Indicates that a error have been detected, during runtime
 *
 * - CONFIG_ERROR   : Indicates that a error have been detected in parameters
 *
 * - NO_MEMORY      : Indicates that not enough memory is available
 *
 * Other fields describe possible answer (combination of basic answer)
 *
 ***************************************************************/
typedef enum
{
  /* base of the answer */
  AEP_RESET_PAK_STATUS   = 0,
  AEP_PAK_LOADED         = 1,
  AEP_PAK_GIVEN          = (1<<1),
  AEP_CONFIG_APPLIED     = (1<<2),
  AEP_COMPO_FLUSHED      = (1<<3),
  AEP_COMPO_ERROR        = (1<<4),
  AEP_COMPO_CONFIG_ERROR = (1<<5),
  AEP_COMPO_NO_MEMORY    = (1<<6),
  /* the following enum are used for debug */
  AEP_PAK_LOADED_AND_GIVEN            = AEP_PAK_LOADED | AEP_PAK_GIVEN,
  AEP_PAK_LOADED_AND_CONFIGURED       = AEP_PAK_LOADED | AEP_CONFIG_APPLIED,
  AEP_PAK_GIVEN_AND_CONFIGURED        = AEP_PAK_GIVEN  | AEP_CONFIG_APPLIED,
  AEP_PAK_LOADED_GIVEN_AND_CONFIGURED = AEP_PAK_LOADED | AEP_PAK_GIVEN | AEP_CONFIG_APPLIED,
  AEP_PAK_LOADED_AND_FLUSHED          = AEP_PAK_LOADED | AEP_PAK_GIVEN | AEP_COMPO_FLUSHED,
  AEP_PAK_FLUSHED                     = AEP_PAK_GIVEN  | AEP_COMPO_FLUSHED
}AEP_RETURN_STATUS_T;


/****************************************************************/
/****************************************************************/
/**
 * \brief Effect description
 *
 * Each effect must define one instance of such structure
 ****************************************************************/
typedef struct AEP_EFFECT_DESCRIPTION_T
{
  AEP_WORKER_T bl_worker_type;     //!< Type of effect (inplace or not)
  int scratch_size;                //!< Size needed for scratch buffer
  int nb_static_param;             //!< Number of static parameters
  int nb_dyn_param;                //!< Number of dynamic parameters
  AEP_RETURN_STATUS_T (*effect_main)(AEP_PACKET_T **in_packet_tab,       //!< Pointer on main function
                                     AEP_PACKET_T **out_packet,
                                     AEP_COMMAND_T compo_cmd,
                                     void* local_struct_ptr,
                                     int *scratch_buffer);
  AEP_RETURN_STATUS_T (*effect_init)(t_uint16 *parameter_list_ptr,       //!< Pointer on init function
                                     int data_block_size,
                                     int aep_id,
                                     int position_id,
                                     AEP_TYPE_CHANNEL_MODE aep_type,
                                     void** local_struct_ptr);
  void (*effect_reset)(void* local_struct_ptr);                          //!< Pointer on reset function
  void (*effect_free)(void* local_struct_ptr);
  AEP_RETURN_STATUS_T (*effect_configure)(void* local_struct_ptr,        //!< Pointer on configuration function
                                          t_uint16 *parameter_list_ptr);
  int (*effect_info)(void* local_struct_ptr,                             //!< Pointer on information function
                     t_uint16 *parameter_list_ptr);
  int NbOutput;                    //!< Minimum number of running output needed
  int NbInput;                     //!< Minimum number of running input needed
  int MandatoryInputs;             //!< Bit Map of mandatory input. 
                                   /**< If bit x is set, it means that input x (towards connection order) is mandatory.
										Set it to 0 if no particular input is mandatory or all NbInput are mandatory */
} AEP_EFFECT_DESCRIPTION_T;

/****************************************************************/
/****************************************************************/
/* functions required for dynamic memory allocation */
extern void *aep_malloc(size_t size);
extern void  aep_free(void *addr);
extern void  aep_free_y(void YMEM *addr);
extern void YMEM *aep_malloc_y(size_t size);
extern void *aep_malloc_ext24(size_t size);
extern void *aep_malloc_ext16(size_t size);
extern void *aep_malloc_esram(size_t size);

extern void *(* const EXTERN aep_malloc_ptr[5])(size_t);
/****************************************************************/
/****************************************************************/
/* functions required to send alerts to host */
extern void aep_send_hcl_alert(AEP_ALERT_TYPE_T alert_type, int param_num, int *params,
                       int aep_id, int pos_id);
extern int aep_pgcd(int a, int b);


#endif //_aep_h_


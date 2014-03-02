/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file for SBAG
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SBAGP_H_
#define _SBAGP_H_

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#include "sbag.h"

/*--------------------------------------------------------------------------*
 * Defines																	*
 *--------------------------------------------------------------------------*/
#define SBAG_HCL_VERSION_ID 1
#define SBAG_HCL_MAJOR_ID   0
#define SBAG_HCL_MINOR_ID   0

typedef struct
{
    t_sbag_register *p_sbag_register;           /*The SBAG Register Structure*/
    t_sbag_mode     sbag_mode;                  /*The SBAG Operating Mode Link, Trace or Interrupt Mode*/
    t_bool          sbag_fpf_message_enable;    /*FPF message Generation is Enabled or Disabled*/
    t_uint32        time_absolute_lsb;          /*The Value of the time counter since SYNC*/
    t_uint16        time_absolute_msb;          /*The Value of the time counter since SYNC*/
    t_sbag_device   sbag_device;                /*To take care of interrupts*/

    /*WPSAT Data Read*/
    t_uint16        wpsat_src;
    t_uint8         wpsat_opc;
    t_uint8         wpsat_be;
    t_uint8         wpsat_tid;
    t_uint32        wpsat_lsb_address;
    t_uint32        wpsat_data_lsb;
    t_uint32        wpsat_data_msb;
    t_uint8         wpsat_req_resp;

    /*TMSAT Data Read*/
    t_uint32        tmsat_metric1;
    t_uint32        tmsat_metric2;
    t_uint32        tmsat_metric3;

    /*FPF Message*/
    t_uint32        fpf_message;
} t_sbag_system_context;

/*------------------------------------------------------------------------------*
 * Private Functions															*
 *------------------------------------------------------------------------------*/
PRIVATE t_sbag_error    sbag_WPSATSetDataAvailability(IN t_sbag_wpsat_id sbag_wpsat_id);
PRIVATE t_sbag_error    sbag_TMSATSetDataAvailability(IN t_sbag_tmsat_id sbag_tmsat_id);
PRIVATE t_sbag_error    sbag_WPSATClearDataAvailability(IN t_sbag_wpsat_id sbag_wpsat_id);
PRIVATE t_sbag_error    sbag_TMSATClearDataAvailability(IN t_sbag_tmsat_id sbag_tmsat_id);
PRIVATE t_sbag_error    sbag_GetTMSATErrorStatus(IN t_sbag_tmsat_id sbag_tmsat_id);
PRIVATE t_sbag_error    sbag_ProcessIt(IN t_sbag_irq_src sbag_irq_src);
PRIVATE t_sbag_error    sbag_WPSATProcessIt(t_uint32 interrupt_src);
PRIVATE t_sbag_error    sbag_TMSATProcessIt(t_uint32 interrupt_src);
PRIVATE t_sbag_error    sbag_PIProcessIt(t_uint32 interrupt_src);
#endif

/*end of sbag_p.h */

/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

 /** \file    t1xhv_irp.h */

#ifndef _T1XHV_IRP_H_
#define _T1XHV_IRP_H_

#include "crm_mmdsp.h" /* register definition */
#include "dma_mmdsp.h" /* register definition */
#include "pictor_mmdsp.h" /* register definition */
#include "proj_mmdsp.h" /* register definition */
#include "sia_mmdsp.h" /* register definition */
#include "stbp_mmdsp.h" /* register definition */
#include "irp_api.h"
#include "ispctl_types.idt.h"

#ifdef IRP_TRACE_DEBUG
#include "itc_api.h"
#endif

#include <FSM.h>

#ifdef IRP_GLOBAL 
#define TRCS_EXTERN 
#else
#define TRCS_EXTERN extern
#endif
/*
  Since ispctl is not derived from ENS FSM, 
  we need to declare here locally the structures
  needed by OST macros. 
*/
  
// typedef struct {
//	t_uint32 parentHandle;  
/*              this is the OMX handle of the parent component, it appears in the header of traces
 *              that are sent to the host and can be used by the host to filter them at its end */
//	t_uint16 traceEnable;
/*              this is a mask on the desired trace level. It acts as a filter on the emitter side,
 *              in the sense that if this mask is not enabled the traces are not generated at all.
 *              This mask ccrresponds to the definitions of TRACE_DEBUG, TRACE_WARNING etc. */ 
//	t_uint32 dspAddr;
/*              this is unused in our case (it is used in ENS to keep track of shared trace info area 
 *              between ARM and DSP) */
//} TraceInfo_t;

TRCS_EXTERN TraceInfo_t traceInfo
#ifdef IRP_GLOBAL
   = {0xDEADF00DUL,0xFFFFU,0}  
#endif
;

//typedef  struct Trace {
//	t_uint16      mId1;
/*              this appears in the header of traces that are sent to the host and can be used by 
 *              the host to filter them at its end */
//	TraceInfo_t * mTraceInfoPtr;
//} TRACE_t;

TRCS_EXTERN TRACE_t     traceObject
#ifdef IRP_GLOBAL
 = {0,&traceInfo}
#endif
;

TRCS_EXTERN TRACE_t *   this
#ifdef IRP_GLOBAL
 = (TRACE_t *)&traceObject;
#endif
;

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

#define NB_TEMP_VALUE_STORED 16

#define MTF_READ_TINY_CMD_DATA(a, b) mtf_read_table(a, b, MTF_CMD_PACKET_SIZE, DMA_MTF_INCR_1, MTF_NO_SWAP_BYTE_NO_SWAP_SHORT)

#define DEFAULT_POLLING_TICKS    0x00000A8C            //30ms at 90KHz frequency: 0.030*90000=2700=0xA8C

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/* Commands mapped on grab ctl register */
typedef enum
{
    HOST_COMMAND_READ = 1,
    HOST_COMMAND_WRITE,
    HOST_COMMAND_BOOT
} t_host_command;


typedef enum {
    IRP_PE_NOT_FILTERED,     /**< the PE blocked */
    IRP_PE_FILTERED              /**< The PE was virtual, stop now.*/
} t_filtered; /**< possible results of filter_page_element */


typedef struct
{
    t_uint16 addr; /**< address of the Page/Element */
    t_uint32 data; /**< Data payload for the Page/Element */
} ts_page_element, *tps_page_element; /**< a Page Element structure */

typedef struct {
    t_uint16 n_host_pe; /* number of page-elements we recieved from the host */
    t_uint16 n_times_irp_clk_it; /* number of times we received and IRP_SENSOR_CLOCK_IT */
} ts_irp_debug;

typedef enum
{
    HOST_WRITE_SINGLE=0x0,
    HOST_WRITE_LIST=0x1,
    HOST_READ_SINGLE=0x2,
    HOST_READ_LIST=0x3,
    HOST_READ_POLLING=0x4,
    HOST_READ_CHECK=0x5,
    HOST_WRITING_SIG,
    HOST_READING_SIG,
    HOST_POLLING_SIG
}t_host_request;


/*------------------------------------------------------------------------
 * Variables (exported)
 *----------------------------------------------------------------------*/

extern volatile ts_irp_debug G_irp_debug;

extern volatile t_uint16 mtf_cmd_packet[MTF_CMD_PACKET_SIZE];
//extern volatile t_uint32 G_debug_array[2048];
extern volatile t_uint32 G_debug_count;


extern volatile t_uint16 G_irp_booted;

extern volatile t_uint16 G_Current_Pe_Addr;
extern volatile t_uint32 G_Current_Pe_Value;

extern volatile ts_PageElement G_ts_tab_pe[ISPCTL_SIZE_TAB_PE];
extern volatile t_uint16  G_nb_of_pe;
extern volatile t_host_request  G_host_request;

extern volatile t_uint32  G_polling_ticks;
extern volatile t_uint32  G_polling_counter;
extern volatile t_uint16  G_temp_value_polled[NB_TEMP_VALUE_STORED];


//  Call_Active_State is defined in file ispctl_core.c
extern volatile t_uint32 G_Ispctl_Processing_OnGoing;

//      t_eStateId is an enum type defined in file ispctl_types.idt
extern volatile t_eStateId G_State_Id;
//      e_ispctlInfo is an enum defined in file ispctl_types.idt
extern volatile enum e_ispctlInfo G_Ispctl_Info_Log;

/* macros */
/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/
/* in t1xhv_irp_interrupts.c */
void irp_itc_comp0(void);
void irp_itc_comp1_panic(void);

void Isp_ITM0_handler(void);
void Isp_ITM1_handler(void);
void Isp_ITM2_handler(void);

void irp_alert_info(enum e_ispctlInfo info_id);
void irp_alert_error(enum e_ispctlError error_id);
void irp_alert_debug(enum e_ispctlError error_id);

void Start_Panic_timer(void);
void Stop_Panic_timer(void);

void Start_Polling_timer(void);
void Stop_Polling_timer(void);

void Booting_State(t_eSignal signal);
void Reading_State(t_eSignal signal);
void Writing_State(t_eSignal signal);
void Polling_State(t_eSignal signal);

/*------------------------------------------------------------------------
 * Functions (inlined)
 *----------------------------------------------------------------------*/

#endif /* _T1XHV_IRP_H_ */

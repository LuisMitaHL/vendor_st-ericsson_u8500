/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef ITE_EVENT_QUEUE_H_
#define ITE_EVENT_QUEUE_H_

#include "cm/inc/cm_type.h" /* provided by nmf. Used to t_uint* types */
#include "ispctl_types.idt.h"
#include "VhcElementDefs.h"
#include "ite_nmf_standard_functions.h"

#define ITE_EVENT_QUEUE_SIZE 256

/* notes on e_ITE_EventType choice: this is an enum rather than mask values.
 * Mask values would have been useful to easily select different types
 * of events to WaitEvent() for, but enum values are damn useful for debugging.
 * Note that the order of the enums is the same as the order of types in the
 * union, this is useful to quickly find the right type in the debugger (value
 * 4 -> see 4th type).
 * Since I spend so much time debugging, I prefer the enum values
 * extra code to select groups of values in WaitEvent() */

enum e_ITE_EventType {
   ITE_EVT_NONE = 0,
   ITE_EVT_SIARM_INFO,
   ITE_EVT_SIARM_ERROR,
   ITE_EVT_SIARM_DEBUG,
   ITE_EVT_ISPCTL_INFO,
   ITE_EVT_ISPCTL_ERROR,
   ITE_EVT_ISPCTL_DEBUG,
   ITE_EVT_GRABLR_INFO,
   ITE_EVT_GRABLR_ERROR,
   ITE_EVT_GRABLR_DEBUG,
   ITE_EVT_GRABHR_INFO,
   ITE_EVT_GRABHR_ERROR,
   ITE_EVT_GRABHR_DEBUG,
   ITE_EVT_GRABBMS_INFO,
   ITE_EVT_GRABBMS_ERROR,
   ITE_EVT_GRABBMS_DEBUG,
   ITE_EVT_GRABBML_INFO,
   ITE_EVT_GRABBML_ERROR,
   ITE_EVT_GRABBML_DEBUG,

   ITE_EVT_MAX_EVENTS,
   
   /* the following aren't real event types, but are used
       * for groups of event types in GetEvent() 
       */
   ITE_EVT_SIARM_EVENTS=0x60,
   ITE_EVT_ISPCTL_EVENTS,
   ITE_EVT_GRABLR_EVENTS,
   ITE_EVT_GRABHR_EVENTS,
   ITE_EVT_GRABBMS_EVENTS,
   ITE_EVT_GRABBML_EVENTS,
   ISP_HOST_TO_SENSOR_ACCESS_COMPLETE_EVENTS,
   ITE_EVT_ISPCTL_ZOOM_EVENTS,
   ITE_EVT_ISPCTL_STATS_EVENTS,
   ITE_EVT_ISPCTL_FOCUS_EVENTS,
   ISP_NVM_EXPORT_DONE_EVENTS,
   ISP_CDCC_AVAILABLE_EVENTS,
   ISP_POWER_NOTIFICATION_EVENTS,
   ISP_SMS_NOTIFICATION_EVENTS,
   ISP_SENSOR_OUTPUT_MODE_EXPORT_NOTIFICATION_EVENTS,
   ISP_SENSOR_COMMIT_NOTIFICATION_EVENTS,
   ISP_ISP_COMMIT_NOTIFICATION_EVENTS,
   ITE_EVT_ISPCTL_RW_EVENTS,
   ITE_EVT_ISPCTL_GAMMA_EVENTS,

   ITE_EVT_ALL_EVENTS=0xFFFF
};

/* all specific event structures MUST start with "t_uint8 type;" */
struct s_ITE_NMF_siarmInfoEvent {
   t_uint8 type;
   t_uint16 info_id;
   t_uint16 resources;
};

struct s_ITE_NMF_siarmErrorEvent {
   t_uint8 type;
   t_uint16 error_id;
};

struct s_ITE_NMF_siarmDebugEvent {
   t_uint8 type;
   t_uint16 debug_id;
   t_uint16 data1;
   t_uint16 data2;
};

struct s_ITE_NMF_ispctlInfoEvent {
    t_uint8 type;
    t_uint16 info_id;
    t_uint32 value;
    ts_PageElement Listvalue[ISPCTL_SIZE_TAB_PE];
    t_uint16 number_of_pe;
    t_uint32 timestamp;
};

struct s_ITE_NMF_ispctlErrorEvent {
   t_uint8 type;
   t_uint16 error_id;
   t_uint16 last_PE_data_polled;
   t_uint16 number_of_pe;
   t_uint32 timestamp;
};

struct s_ITE_NMF_ispctlDebugEvent {
   t_uint8 type;
   t_uint16 debug_id;
   t_uint16 data1;
   t_uint16 data2;
   t_uint32 timestamp;
};

struct s_ITE_NMF_grabInfoEvent {
   t_uint8 type;
   t_uint16 info_id;
   t_uint16 buffer_id;
   t_uint32 timestamp;
};

struct s_ITE_NMF_grabErrorEvent {
   t_uint8 type;
   t_uint16 error_id;
   t_uint16 buffer_id;
   t_uint16 data;
   t_uint32 timestamp;
};

struct s_ITE_NMF_grabDebugEvent {
   t_uint8 type;
   t_uint16 debug_id;
   t_uint16 buffer_id;
   t_uint16 data1;
   t_uint16 data2;
   t_uint32 timestamp;
};

union u_ITE_Event {
   t_uint8 type;
   struct s_ITE_NMF_siarmInfoEvent   siarmInfo;
   struct s_ITE_NMF_siarmErrorEvent  siarmError;
   struct s_ITE_NMF_siarmDebugEvent  siarmDebug;
   struct s_ITE_NMF_ispctlInfoEvent  ispctlInfo;
   struct s_ITE_NMF_ispctlErrorEvent ispctlError;
   struct s_ITE_NMF_ispctlDebugEvent ispctlDebug;
   struct s_ITE_NMF_grabInfoEvent    grabLRInfo;
   struct s_ITE_NMF_grabErrorEvent   grabLRError;
   struct s_ITE_NMF_grabDebugEvent   grabLRDebug;
   struct s_ITE_NMF_grabInfoEvent    grabHRInfo;
   struct s_ITE_NMF_grabErrorEvent   grabHRError;
   struct s_ITE_NMF_grabDebugEvent   grabHRDebug;
   struct s_ITE_NMF_grabInfoEvent    grabBMSInfo;
   struct s_ITE_NMF_grabErrorEvent   grabBMSError;
   struct s_ITE_NMF_grabDebugEvent   grabBMSDebug;
   struct s_ITE_NMF_grabInfoEvent    grabBMLInfo;
   struct s_ITE_NMF_grabErrorEvent   grabBMLError;
   struct s_ITE_NMF_grabDebugEvent   grabBMLDebug;
};


#define EVENT_COUNT_0_BASE    Event0_Count_u16_EVENT0_0_HOST_COMMS_READY_Byte0
#define EVENT_COUNT_1_BASE    Event1_Count_u16_EVENT1_0_HOST_TO_MASTER_I2C_ACCESS_Byte0
#define EVENT_COUNT_2_BASE    Event2_Count_u16_Reserved_Byte0
#define EVENT_COUNT_3_BASE    Event3_Count_u16_EVENT3_0_DMA_GRAB_NOK_Byte0
#define EVENT_PAGE_SIZE       0x40


#ifdef __cplusplus

extern "C"

{

#endif


void ITE_Log_Event_enable(void);
void ITE_Log_Event_disable(void);
union u_ITE_Event ITE_GetEvent(t_uint16 event_type);
union u_ITE_Event ITE_GetZoomEvent(void);
int ITE_AddEvent(union u_ITE_Event event);

int ITE_DisplayEvent(void);

union u_ITE_Event ITE_WaitEvent(t_uint16 event_type);

t_uint8 ITE_WaitEventTimeout(t_uint16 event_type, int timeout);

void ITE_traceISPCTLevent(union u_ITE_Event);

void ITE_CleanEvent(t_uint16 event_type);

int ITE_GetEventCount(t_uint16 addr);
void ITE_RefreshEventCount(t_uint16 addr);
int ITE_Check_Event_Count(t_uint16 no_of_event_recv);
void ITE_initEvent(void);
void ITE_ExitEvent(void);
int ITE_NMF_WaitEvent(enum e_ispctlInfo   ispctlInfo);
int ITE_FindEvent(t_uint16 event_type);
union u_ITE_Event ITE_GetStatsEvent();

#ifdef __cplusplus

}

#endif

#endif /* ITE_EVENT_QUEUE_H_*/

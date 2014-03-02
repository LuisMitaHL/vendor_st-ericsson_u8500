#ifndef ENS_REDEFINE_CLASS_H
#define ENS_REDEFINE_CLASS_H

#include "ENS_ThisVersion.h"

#if 0       // Old version of ENS. Preparing for multi-versionning

#ifndef ENS_VERSION_SHARED
#define ENS_VERSION_SHARED 1
#endif

#ifndef ENS_VERSION_VIDEO
#define ENS_VERSION_VIDEO 2
#endif

#ifndef ENS_VERSION_AUDIO
#define ENS_VERSION_AUDIO 3
#endif

#ifndef ENS_VERSION_IMAGING
#define ENS_VERSION_IMAGING 4
#endif

#ifndef ENS_VERSION
#define ENS_VERSION ENS_VERSION_SHARED
#endif

#else

#ifndef ENS_VERSION_SHARED
#error ENS_VERSION_SHARED must be defined
#endif

#ifndef ENS_VERSION_VIDEO
#error ENS_VERSION_VIDEO must be defined
#endif

#ifndef ENS_VERSION_AUDIO
#error ENS_VERSION_AUDIO must be defined
#endif

#ifndef ENS_VERSION_IMAGING
#error ENS_VERSION_IMAGING must be defined
#endif

#ifndef ENS_VERSION
#error ENS_VERSION must be defined
#endif

#endif

#if ENS_VERSION != THIS_ENS_VERSION
#error ENS_VERSION and THIS_ENS_VERSION do not match
#endif

#if ENS_VERSION == ENS_VERSION_VIDEO

#define ENS ENS_Video
#define TraceBuilder TraceBuilder_Video
#define TraceBuilderHost TraceBuilderHost_Video
#define TraceBuilderMpc TraceBuilderMpc_Video
#define MemoryTrace MemoryTrace_Video
#define enableMemoryTrace enableMemoryTrace_Video
#define trace_buffer trace_buffer_Video
#define ENS_List ENS_List_Video
#define ENS_Component ENS_Component_Video
#define ENS_Port ENS_Port_Video
#define ENS_ResourcesDB ENS_ResourcesDB_Video
#define ENS_Fsm ENS_Fsm_Video
#define ENS_Component_Fsm ENS_Component_Fsm_Video
#define ENS_Port_Fsm ENS_Port_Fsm_Video
#define SharedBuffer SharedBuffer_Video
#define NmfHost_ProcessingComponent NmfHost_ProcessingComponent_Video
#define NmfMpc_ProcessingComponent NmfMpc_ProcessingComponent_Video
#define NmfHostMpc_ProcessingComponent NmfHostMpc_ProcessingComponent_Video
#define NmfHost_ProcessingComponentCB NmfHost_ProcessingComponentCB_Video
#define NmfMpc_ProcessingComponentCB NmfMpc_ProcessingComponentCB_Video
#define NmfHostMpc_ProcessingComponentCB NmfHostMpc_ProcessingComponentCB_Video
#define EnsCommonPortData EnsCommonPortData_Video
#define Port Port_Video
#define Component Component_Video
#define PortQueue PortQueue_Video
#define Controller Controller_Video
#define FSM FSM_Video
#define OMX_COMMANDTYPE_NAME OMX_COMMANDTYPE_NAME_Video
#define OMX_EVENTTYPE_NAME OMX_EVENTTYPE_NAME_Video
#define OMX_STATETYPE_NAME OMX_STATETYPE_NAME_Video
#define _memory_status _memory_status_Video
#define _memory_status_after _memory_status_after_Video
#define _memory_status_before _memory_status_before_Video
#define _memory_usage _memory_usage_Video
#define TraceObject TraceObject_Video
#define isMemoryTraceEnabled isMemoryTraceEnabled_Video
#define getMemoryUsage getMemoryUsage_Video
#define traceMemoryUsage traceMemoryUsage_Video
#define getMemoryStatus getMemoryStatus_Video
#define traceMemoryStatus traceMemoryStatus_Video

#elif ENS_VERSION == ENS_VERSION_AUDIO

#define ENS ENS_Audio
#define TraceBuilder TraceBuilder_Audio
#define TraceBuilderHost TraceBuilderHost_Audio
#define TraceBuilderMpc TraceBuilderMpc_Audio
#define MemoryTrace MemoryTrace_Audio
#define enableMemoryTrace enableMemoryTrace_Audio
#define trace_buffer trace_buffer_Audio
#define ENS_List ENS_List_Audio
#define ENS_Component ENS_Component_Audio
#define ENS_Port ENS_Port_Audio
#define ENS_ResourcesDB ENS_ResourcesDB_Audio
#define ENS_Fsm ENS_Fsm_Audio
#define ENS_Component_Fsm ENS_Component_Fsm_Audio
#define ENS_Port_Fsm ENS_Port_Fsm_Audio
#define SharedBuffer SharedBuffer_Audio
#define NmfHost_ProcessingComponent NmfHost_ProcessingComponent_Audio
#define NmfMpc_ProcessingComponent NmfMpc_ProcessingComponent_Audio
#define NmfHostMpc_ProcessingComponent NmfHostMpc_ProcessingComponent_Audio
#define NmfHost_ProcessingComponentCB NmfHost_ProcessingComponentCB_Audio
#define NmfMpc_ProcessingComponentCB NmfMpc_ProcessingComponentCB_Audio
#define NmfHostMpc_ProcessingComponentCB NmfHostMpc_ProcessingComponentCB_Audio
#define EnsCommonPortData EnsCommonPortData_Audio
#define Port Port_Audio
#define Component Component_Audio
#define PortQueue PortQueue_Audio
#define Controller Controller_Audio
#define FSM FSM_Audio
#define OMX_COMMANDTYPE_NAME OMX_COMMANDTYPE_NAME_Audio
#define OMX_EVENTTYPE_NAME OMX_EVENTTYPE_NAME_Audio
#define OMX_STATETYPE_NAME OMX_STATETYPE_NAME_Audio
#define memory_status memory_status_Audio
#define memory_status_after memory_status_after_Audio
#define memory_status_before memory_status_before_Audio
#define memory_usage memory_usage_Audio
#define TraceObject TraceObject_Audio
#define isMemoryTraceEnabled isMemoryTraceEnabled_Audio
#define getMemoryUsage getMemoryUsage_Audio
#define traceMemoryUsage traceMemoryUsage_Audio
#define getMemoryStatus getMemoryStatus_Audio
#define traceMemoryStatus traceMemoryStatus_Audio

#elif ENS_VERSION == ENS_VERSION_IMAGING

#define ENS ENS_Imaging

#define TraceBuilder TraceBuilder_Imaging
#define TraceBuilderHost TraceBuilderHost_Imaging
#define TraceBuilderMpc TraceBuilderMpc_Imaging
#define MemoryTrace MemoryTrace_Imaging
#define enableMemoryTrace enableMemoryTrace_Imaging
#define trace_buffer trace_buffer_Imaging
#define ENS_List ENS_List_Imaging
#define ENS_Component ENS_Component_Imaging
#define ENS_Port ENS_Port_Imaging
#define ENS_ResourcesDB ENS_ResourcesDB_Imaging
#define ENS_Fsm ENS_Fsm_Imaging
#define ENS_Component_Fsm ENS_Component_Fsm_Imaging
#define ENS_Port_Fsm ENS_Port_Fsm_Imaging
#define SharedBuffer SharedBuffer_Imaging
#define NmfHost_ProcessingComponent NmfHost_ProcessingComponent_Imaging
#define NmfMpc_ProcessingComponent NmfMpc_ProcessingComponent_Imaging
#define NmfHostMpc_ProcessingComponent NmfHostMpc_ProcessingComponent_Imaging
#define NmfHost_ProcessingComponentCB NmfHost_ProcessingComponentCB_Imaging
#define NmfMpc_ProcessingComponentCB NmfMpc_ProcessingComponentCB_Imaging
#define NmfHostMpc_ProcessingComponentCB NmfHostMpc_ProcessingComponentCB_Imaging
#define EnsCommonPortData EnsCommonPortData_Imaging
#define Port Port_Imaging
#define Component Component_Imaging
#define PortQueue PortQueue_Imaging
#define Controller Controller_Imaging
#define FSM FSM_Imaging
#define OMX_COMMANDTYPE_NAME OMX_COMMANDTYPE_NAME_Imaging
#define OMX_EVENTTYPE_NAME OMX_EVENTTYPE_NAME_Imaging
#define OMX_STATETYPE_NAME OMX_STATETYPE_NAME_Imaging
#define memory_status memory_status_Imaging
#define memory_status_after memory_status_after_Imaging
#define memory_status_before memory_status_before_Imaging
#define memory_usage memory_usage_Imaging
#define TraceObject TraceObject_Imaging
#define isMemoryTraceEnabled isMemoryTraceEnabled_Imaging
#define getMemoryUsage getMemoryUsage_Imaging
#define traceMemoryUsage traceMemoryUsage_Imaging
#define getMemoryStatus getMemoryStatus_Imaging
#define traceMemoryStatus traceMemoryStatus_Imaging

#elif ENS_VERSION == ENS_VERSION_SHARED

// No redefine yet

#else
#error Wrong definition of ENS_VERSION
#endif

#endif      // ENS_REDEFINE_CLASS_H

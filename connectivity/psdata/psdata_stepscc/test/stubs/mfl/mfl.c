/*
 * mfl.c
 *
 *  Created on: Mar 23, 2011
 *      Author: echnils
 */
#include "mfl.h"
#include "t_sys.h"
#include "g_ns_pss.h"

modem_session_t* mfl_session_begin_named_socket(const char* const name_ptr)
{
  return 0;
}
void mfl_session_get_file_descriptor(modem_session_t* modem_session_ptr,
                   int*       fd_ptr)
{
  return;
}
void mfl_session_end(modem_session_t* modem_session_ptr)
{
  return;
}
RequestControlBlock_t* mfl_request_control_block_alloc(modem_session_t* const modem_session_ptr, const ClientTag_t  client_tag)
{
  return 0;
}
void mfl_request_control_block_free(RequestControlBlock_t* request_control_block_ptr)
{
  return;
}

void* mfl_session_get_signal(modem_session_t* const modem_session_ptr)
{
  return 0;
}

ClientTag_t mfl_get_client_tag(void* signal_ptr)
{
  return 0;
}
void mfl_signal_free(void** signal_ptr_ptr)
{
  return;
}
RequestStatus_t Request_PS_PDP_Context_Activate_V3(
  const RequestControlBlock_t *const RequestCtrl_p,
  const uint8 *const AccessPointName_p,
  const PS_HeaderCompression_t HeaderCompression,
  const PS_DataCompression_t DataCompression,
  const PS_QOS_Type_t QOS_Type,
  const boolean QOS_Reverted,
  const PS_QOS_Profile_t *const MinAcceptableQOS_p,
  const PS_QOS_Profile_t *const RequestedQOS_p,
  const PS_PDP_Type_t PDP_Type,
  const PS_PDP_Address_t *const PDP_Address_p,
  const uint8 *const ProtocolOptions_p,
  const uint8 ProtocolOptionsLength,
  const PS_Priority_t PS_Priority,
  const PS_UserPlaneSettings_t *const UserPlaneSettings_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_PDP_ContextActivateResult_t *const Result_p)
{
  return 0;
}

RequestStatus_t Response_PS_PDP_Context_Activate_V2(
  const void *const SigStruct_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_PDP_ContextActivateResult_t *const Result_p)
{
  return 0;
}

RequestStatus_t Response_PS_PDP_Context_Deactivate_V2(
    const void*         const SigStruct_p,
    PS_GeneralResult_t* const Result_p)
{
  return 0;
}

RequestStatus_t Request_PS_PDP_Context_Modify_V3(
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_NSAPI_t NSAPI,
  const PS_QOS_Type_t QOS_Type,
  const PS_QOS_Profile_t *const MinAcceptableQOS_p,
  const PS_QOS_Profile_t *const NewQOS_p,
  const PS_TFT_OperationCode_t TFT_OperationCode,
  const uint8 PacketFiltersToDelete,
  const PS_TFT_PacketFilter_t *const TFT_PacketFilter_p,
  const PS_TFT_ParameterList_t *const TFT_ParameterList_p,
  const uint8 *const ProtocolOptions_p,
  uint8 *const ProtocolOptionsLength_p,
  uint8 **const ProtocolOptions_pp,
  PS_CauseCode_t *const Cause_p,
  PS_GeneralResult_t *const Result_p)
{
  return 0;
}

RequestStatus_t Response_PS_PDP_Context_Modify_V3(
    const void*         const SigStruct_p,
    uint8*              const ProtocolOptionsLength_p,
    uint8**             const ProtocolOptions_pp,
    PS_CauseCode_t*     const Cause_p,
    PS_GeneralResult_t* const Result_p)
{
  return 0;
}

RequestStatus_t Request_PS_QOS_Profile_Read(
    const RequestControlBlock_t* const RequestCtrl_p,
    const PS_NSAPI_t                   NSAPI,
    PS_QOS_Profile_t*            const QOS_Profile_p,
    PS_GeneralResult_t*          const Result_p)
{
  return 0;
}

RequestStatus_t Response_PS_QOS_Profile_Read(
  const void *const SigStruct_p,
  PS_QOS_Profile_t *const QOS_Profile_p,
  PS_GeneralResult_t *const Result_p)
{
  return 0;
}

EventStatus_t Event_PS_PDP_ContextStatus(
  const void *const SigStruct_p,
  PS_PDP_Type_t *const PDP_Type_p,
  PS_PDP_Address_t *const PDP_Address_p,
  uint8 **const ProtocolOptions_pp,
  uint8 *const ProtocolOptionsLength_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_CauseCode_t *const Cause_p,
  PS_PDP_ContextEvent_t *const ContextEvent_p)
{
  return 0;
}

EventStatus_t Event_PS_PDP_ContextDeactivate_V2(
  const void *const SigStruct_p,
  PS_NSAPI_t *const NSAPI_p,
  PS_TimeAndDataVolume_t *const TimeAndDataVolume_p,
  uint8 **const ProtocolOptions_pp,
  uint8 *const ProtocolOptionsLength_p,
  PS_CauseCode_t *const Cause_p)
{
  return 0;
}

RequestStatus_t Request_PS_PDP_Context_Deactivate_V2(
  const RequestControlBlock_t *const RequestCtrl_p,
  const PS_NSAPI_t NSAPI,
  const PS_TearDownIndicator_t TearDownIndicator,
  const uint8 *const ProtocolOptions_p,
  const uint8 *const ProtocolOptionsLength_p,
  PS_GeneralResult_t *const Result_p)
{
  return 0;
}

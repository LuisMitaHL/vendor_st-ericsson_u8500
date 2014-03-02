/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <cops_api_internal.h>
#include <cops_shared_util.h>
#include <cops_log.h>
#include <r_dispatch.h>
#include <r_sim_si.h>
#include <g_cops_cops.h>
#include <g_cops_simlock.h>

extern uint8_t run_mode;

static void cops_api_proxy_ota_unlock_status_updated_cb(void *event_aux,
                         const cops_simlock_ota_unlock_status_t *unlock_status);

static void cops_api_proxy_simlock_status_changed_cb(void *event_aux,
                                               const cops_sim_status_t *status);

static void cops_api_proxy_authentication_changed_cb(void *event_aux);

void cops_api_proxy_startup(cops_context_id_t **context)
{
    cops_event_callbacks_t cbs = {cops_api_proxy_ota_unlock_status_updated_cb,
                                  cops_api_proxy_simlock_status_changed_cb,
                                  cops_api_proxy_authentication_changed_cb};
    cops_event_callbacks_t *cbs_p = NULL;

    if (COPS_RUN_MODE_MODEM == run_mode) {
        cbs_p = &cbs;
#ifdef COPS_EXPLICIT_VERIFY_IMSI
        RequestStatus_t status;

        status = Request_EventChannel_Subscribe(WAIT_RESPONSE, EVENT_SUBSCRIBE,
                                                EVENT_ID_SIM_STATE_CHANGED);

        if (REQUEST_OK != status) {
            COPS_LOG(LOG_WARNING,
                    "Failed to subscribe to SIM event: 0x%x\n", status);
        }
#endif /* COPS_EXPLICIT_VERIFY_IMSI */
    }

    (void)cops_context_create(context, cbs_p, NULL);
}

int cops_api_proxy_handle_signal(union SIGNAL *signal,
                                 cops_context_id_t *context)
{
    int signal_handled = 0;

#ifdef COPS_IM_STUB_LEVEL_API_PROXY
    (void)context;
#endif

    switch (signal->sig_no) {
#ifdef COPS_EXPLICIT_VERIFY_IMSI
        case EVENT_SIM_STATE_CHANGED: {
            UICC_ServiceHandle_t service_handle;
            SIM_PinRefDataId_t id;
            SIM_State_t state;
            EventStatus_t status = Event_SIM_State_Changed(signal,
                                                           &service_handle,
                                                           &id, &state);

            if (GS_EVENT_OK == status) {
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
                cops_sim_status_t sim;
                sim.card_status = COPS_SIMLOCK_CARD_STATUS_APPROVED;
                cops_api_proxy_simlock_status_changed_cb(NULL, &sim);
#else
                if (SIM_STATE_ACTIVE == state) {
                    SIMLock_IMSI_t IMSI;
                    SIM_ISO_ErrorCause_t error_cause;

                    RequestStatus_t request_status =
                        Request_SIM_IMSI_Read(WAIT_RESPONSE, service_handle,
                                              IMSI.Data, &error_cause);

                    if (REQUEST_OK == request_status) {
                        (void) cops_simlock_verify_imsi(context,
                                            (struct cops_simlock_imsi *)&IMSI);
                    }
                } else if (SIM_STATE_POWER_OFF == state) {
                    SIMLock_IMSI_t IMSI;
                    /* This means no SIM(or IMSI) and will clear cached IMSI */
                    memset(IMSI.Data, 0xFF, sizeof(IMSI.Data));
                    (void) cops_simlock_verify_imsi(context,
                                            (struct cops_simlock_imsi *)&IMSI);
                }
#endif
            }

            signal_handled = 1;
            break;
        }
#endif /* COPS_EXPLICIT_VERIFY_IMSI */
        case REQUEST_COPS_READIMEI: {
            Request_COPS_ReadIMEI_t *request = NIL;
            Response_COPS_ReadIMEI_t *response = NIL;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            uint8 imei[] = {3, 5, 0, 0, 5, 0, 5, 0, 0, 0, 0, 9, 6, 9, 2};
#endif
            request = (Request_COPS_ReadIMEI_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_ReadIMEI_t,
                                    RESPONSE_COPS_READIMEI);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            memcpy((void *)response->IMEI.Digits, imei,
                   sizeof(response->IMEI.Digits));
            response->ReturnCode = COPS_RC_OK;
#else
            response->ReturnCode =
                cops_read_imei(context, (cops_imei_t *)&response->IMEI);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_AUTHENTICATE: {
            Request_COPS_Authenticate_t *request = NIL;
            Response_COPS_Authenticate_t *response = NIL;
#ifndef COPS_IM_STUB_LEVEL_API_PROXY
            cops_auth_data_t auth_data;
#endif
            request = (Request_COPS_Authenticate_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_Authenticate_t,
                                    RESPONSE_COPS_AUTHENTICATE);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            auth_data.auth_type = (cops_auth_type_t)request->AuthData.AuthType;
            auth_data.length = request->AuthData.Length;
            auth_data.data = &request->AuthDataBuffer[0];

            response->ReturnCode = cops_authenticate(context,
                                                     request->Permanently,
                                                     &auth_data);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_DEAUTHENTICATE: {
            Request_COPS_Deauthenticate_t *request = NIL;
            Response_COPS_Deauthenticate_t *response = NIL;

            request = (Request_COPS_Deauthenticate_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_Deauthenticate_t,
                                    RESPONSE_COPS_DEAUTHENTICATE);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode = cops_deauthenticate(context,
                                                       request->Permanently);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_GETCHALLENGE: {
            Request_COPS_GetChallenge_t *request = NIL;
            Response_COPS_GetChallenge_t *response = NIL;

            request = (Request_COPS_GetChallenge_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_GetChallenge_t,
                                    RESPONSE_COPS_GETCHALLENGE);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_get_challenge(context, (cops_auth_type_t)request->AuthType,
                                   response->Buffer, &response->BufferLength);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_GETNBROFOTPROWS: {
            Request_COPS_GetNbrOfOTP_Rows_t *request = NIL;
            Response_COPS_GetNbrOfOTP_Rows_t *response = NIL;

            request = (Request_COPS_GetNbrOfOTP_Rows_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_GetNbrOfOTP_Rows_t,
                                    RESPONSE_COPS_GETNBROFOTPROWS);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
        #ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
        #else
            response->ReturnCode = cops_get_nbr_of_otp_rows(context,
                                             (uint32_t *)&response->NbrOfRows);
        #endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_READOTP: {
            Request_COPS_ReadOTP_t *request = NIL;
            Response_COPS_ReadOTP_t *response = NIL;

            request = (Request_COPS_ReadOTP_t *)signal;

            response = SIGNAL_UNTYPED_ALLOC(sizeof(Response_COPS_ReadOTP_t) -
                                            sizeof(uint32) + request->Size,
                                            RESPONSE_COPS_READOTP);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode = cops_read_otp(context, &request->OTP_Buf[0],
                                                 request->Size);

            if (COPS_RETURN_CODE_OK == response->ReturnCode) {
                response->Size = request->Size;
                memcpy(&response->OTP_Buf[0], &request->OTP_Buf[0],
                       request->Size);
            }
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_WRITEOTP: {
            Request_COPS_WriteOTP_t *request = NIL;
            Response_COPS_WriteOTP_t *response = NIL;

            request = (Request_COPS_WriteOTP_t *)signal;

            response = SIGNAL_UNTYPED_ALLOC(sizeof(Response_COPS_WriteOTP_t) -
                                            sizeof(uint32) + request->Size,
                                            RESPONSE_COPS_WRITEOTP);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode = cops_write_otp(context,
                                                  &request->OTP_Data[0],
                                                  request->Size);

            if (COPS_RETURN_CODE_OK == response->ReturnCode) {
                response->Size = request->Size;
                memcpy(&response->OTP_Data[0], &request->OTP_Data[0],
                       request->Size);
            }
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_BINDDATA: {
            Request_COPS_BindData_t *request = NIL;
            Response_COPS_BindData_t *response = NIL;

            request = (Request_COPS_BindData_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_BindData_t,
                                    RESPONSE_COPS_BINDDATA);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_bind_data(context, (uint8_t *)request->Data,
                               request->DataLength,
                               (struct cops_digest *)&response->MAC);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_BINDPROPERTIES: {
            Request_COPS_BindProperties_t *request = NIL;
            Response_COPS_BindProperties_t *response = NIL;
#ifndef COPS_IM_STUB_LEVEL_API_PROXY
            cops_bind_properties_arg_t arg;
            size_t offset;
            size_t n;
            cops_auth_data_t *auth_data_p;
#endif
            request = (Request_COPS_BindProperties_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_BindProperties_t,
                                    RESPONSE_COPS_BINDPROPERTIES);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            /* Check if IMEI shall be updated */
            if (request->IMEI.Digits[0] == 0xFF)
                arg.imei = NULL;
            else
                arg.imei = (cops_imei_t *)&request->IMEI;

            arg.num_new_auth_data = request->NumNewAuthData;
            arg.cops_data_length = request->COPS_DataLength;
            arg.merge_cops_data = request->Merge_COPS_Data;
            arg.cops_data = &request->Data[0];
            offset = arg.cops_data_length;

            arg.auth_data = (cops_auth_data_t *)(&request->Data[offset]);
            offset += (arg.num_new_auth_data * sizeof(cops_auth_data_t));

            for (n = 0; n < arg.num_new_auth_data; n++) {
                auth_data_p = (cops_auth_data_t *)&arg.auth_data[n];
                auth_data_p->data = &request->Data[offset];
                offset += arg.auth_data[n].length;
            }

            /* Validate offset */
            if (offset != request->DataLength) {
                /* if this fails, then the structs in t_cops.h and cops.h are
                 * probably not identical
                 * TODO: Add a warning printout that looks something like this:
                COPS_LOG(LOG_ERROR,
                         "BindProperties: offset(%d) != DataLength(%d)\n",
                         offset, request->DataLength);
                */
            }

            response->ReturnCode = cops_bind_properties(context, &arg);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_READDATA: {
            Request_COPS_ReadData_t *request = NIL;
            Response_COPS_ReadData_t *response = NIL;

            request = (Request_COPS_ReadData_t *)signal;

            response = SIGNAL_UNTYPED_ALLOC(sizeof(Response_COPS_ReadData_t) -
                                            1 + request->ReadLength,
                                            RESPONSE_COPS_READDATA);

            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
            response->DataLength = request->ReadLength;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode = cops_read_data(context,
                            request->ReadLength == 0 ? NULL : response->Data,
                            &response->DataLength);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_VERIFYDATABINDING: {
            Request_COPS_VerifyDataBinding_t *request = NIL;
            Response_COPS_VerifyDataBinding_t *response = NIL;

            request = (Request_COPS_VerifyDataBinding_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_VerifyDataBinding_t,
                                    RESPONSE_COPS_VERIFYDATABINDING);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_verify_data_binding(context, (uint8_t *)request->Data,
                                         request->DataLength,
                                         (struct cops_digest *)&request->MAC);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_VERIFYSIGNEDHEADER: {
            Request_COPS_VerifySignedHeader_t *request = NIL;
            Response_COPS_VerifySignedHeader_t *response = NIL;

            request = (Request_COPS_VerifySignedHeader_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_VerifySignedHeader_t,
                                    RESPONSE_COPS_VERIFYSIGNEDHEADER);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_verify_signed_header(context, request->Header,
                    (enum cops_payload_type)request->PayloadType,
                    (struct cops_vsh *)&response->HeaderInfo);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_CALCDIGEST: {
            Request_COPS_CalcDigest_t *request = NIL;
            Response_COPS_CalcDigest_t *response = NIL;

            request = (Request_COPS_CalcDigest_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_CalcDigest_t,
                                    RESPONSE_COPS_CALCDIGEST);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_calcdigest(context, (enum cops_hash_type)request->HashType,
                                (uint8 *)request->Data, request->DataLength,
                                (struct cops_digest *)&response->Hash);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_GETDEVICESTATE: {
            Request_COPS_GetDeviceState_t *request = NIL;
            Response_COPS_GetDeviceState_t *response = NIL;

            request = (Request_COPS_GetDeviceState_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_GetDeviceState_t,
                                    RESPONSE_COPS_GETDEVICESTATE);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_get_device_state(context,
                                 (cops_device_state_t *)&response->DeviceState);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_FINDPARAMETER: {
            Request_COPS_FindParameter_t *request = NIL;
            Response_COPS_FindParameter_t *response = NIL;
#ifndef COPS_IM_STUB_LEVEL_API_PROXY
            uint8_t *param_data = NULL;
#endif
            request = (Request_COPS_FindParameter_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_FindParameter_t,
                                    RESPONSE_COPS_FINDPARAMETER);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_util_find_parameter(&request->Data[0], request->DataLength,
                                         request->ParamId, &param_data,
                                         &response->ParamLen);

            if (COPS_RETURN_CODE_OK == response->ReturnCode) {
                response->Offset = param_data - &request->Data[0];
            }
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_GETPRODDBGSETTINGS: {
            Request_COPS_GetProdDbgSettings_t *request = NIL;
            Response_COPS_GetProdDbgSettings_t *response = NIL;

            request = (Request_COPS_GetProdDbgSettings_t *)signal;

            response = SIGNAL_ALLOC(Response_COPS_GetProdDbgSettings_t,
                                    RESPONSE_COPS_GETPRODDBGSETTINGS);
            response->SigSelectWithClientTag.ClientTag =
                                     request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                     cops_get_product_debug_settings(context,
                                                     &response->DebugSettings);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_GETSTATUS: {
            Request_SIMLock_GetStatus_t *request = NIL;
            Response_SIMLock_GetStatus_t *response = NIL;

            request = (Request_SIMLock_GetStatus_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_GetStatus_t,
                                    RESPONSE_SIMLOCK_GETSTATUS);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            memset((void *)&(response->Status), 0,
                   sizeof(cops_simlock_status_t));
            response->Status.SIM_CardStatus = COPS_SIMLOCK_CARD_STATUS_APPROVED;
            response->Status.NL_Status.LockType =
                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
            response->Status.NL_Status.AttemptsLeft = 10;
            response->Status.NSL_Status.LockType =
                    COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK;
            response->Status.NSL_Status.AttemptsLeft = 10;
            response->Status.SPL_Status.LockType =
                    COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK;
            response->Status.SPL_Status.AttemptsLeft = 10;
            response->Status.CL_Status.LockType =
                    COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
            response->Status.CL_Status.AttemptsLeft = 10;
            response->Status.SIML_Status.LockType =
                    COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK;
            response->Status.SIML_Status.AttemptsLeft = 10;
            response->Status.ESLL_Status.LockType =
                    COPS_SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK;
            response->Status.ESLL_Status.AttemptsLeft = 10;
            response->Status.FailedLock = -1; /* 0xFF - SIGNED_ENUM8 */

            response->ReturnCode = COPS_RC_OK;
#else
            response->ReturnCode = cops_simlock_get_status(context,
                                    (cops_simlock_status_t *)&response->Status);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_VERIFYIMSI: {
            Request_SIMLock_VerifyIMSI_t *request = NIL;
            Response_SIMLock_VerifyIMSI_t *response = NIL;

            request = (Request_SIMLock_VerifyIMSI_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_VerifyIMSI_t,
                                    RESPONSE_SIMLOCK_VERIFYIMSI);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_OK;
#else
            response->ReturnCode =
                cops_simlock_verify_imsi(context,
                                    (struct cops_simlock_imsi *)&request->IMSI);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_UNLOCK: {
            Request_SIMLock_Unlock_t *request = NIL;
            Response_SIMLock_Unlock_t *response = NIL;

            request = (Request_SIMLock_Unlock_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_Unlock_t,
                                    RESPONSE_SIMLOCK_UNLOCK);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_simlock_unlock(context,
                            (cops_simlock_lock_type_t)request->LockType,
                            (cops_simlock_control_key_t *)&request->ControlKey);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_LOCK: {
            Request_SIMLock_Lock_t *request = NIL;
            Response_SIMLock_Lock_t *response = NIL;
#ifndef COPS_IM_STUB_LEVEL_API_PROXY
            cops_simlock_lock_arg_t lock_arg;
            struct cops_simlock_explicitlockdata explicitlockdata;
#endif
            request = (Request_SIMLock_Lock_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_Lock_t,
                                    RESPONSE_SIMLOCK_LOCK);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            memset(&lock_arg, 0, sizeof(cops_simlock_lock_arg_t));

            memset(&explicitlockdata, 0,
                   sizeof(struct cops_simlock_explicitlockdata));

            lock_arg.lockmode = request->LockArg.LockMode;

            lock_arg.lockdata.locktype = request->LockArg.LockData.LockType;
            strcpy(lock_arg.lockdata.controlkey.value,
                   request->LockArg.LockData.ControlKey.Value);
            lock_arg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKDATA |
                                       COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA |
                                       COPS_SIMLOCK_LOCKOP_SETLOCKSETTING;
            lock_arg.lockdata.update_lockdef =
                request->LockArg.LockData.UpdateLockDefinition;
            lock_arg.lockdata.lockdef =
                request->LockArg.LockData.LockDefinition;

            if (request->LockArg.ExplicitLockData_p != NULL) {
                (void)cops_util_unpack_imsi(
                       (void *)(request->LockArg.ExplicitLockData_p->IMSI.Data),
                       explicitlockdata.imsi.data);

                explicitlockdata.gid1 =
                    request->LockArg.ExplicitLockData_p->GID1;

                explicitlockdata.gid2 =
                    request->LockArg.ExplicitLockData_p->GID2;

                explicitlockdata.length =
                    request->LockArg.ExplicitLockData_p->FlexibleESL_Length;

                if (0 < explicitlockdata.length) {
                    explicitlockdata.data = malloc(explicitlockdata.length);

                    if (NULL != explicitlockdata.data) {
                        memcpy(explicitlockdata.data,
                               request->LockArg.ExplicitLockData_p->
                                   FlexibleESL_Data_p,
                               request->LockArg.ExplicitLockData_p->
                                   FlexibleESL_Length);
                    }
                }

                lock_arg.explicitlockdata = &explicitlockdata;
            }

            response->ReturnCode = cops_simlock_lock(context, &lock_arg);

            if (NULL != explicitlockdata.data) {
                free(explicitlockdata.data);
            }
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_OTA_UNLOCK: {
            Request_SIMLock_OTA_Unlock_t *request = NIL;
            Response_SIMLock_OTA_Unlock_t *response = NIL;

            request = (Request_SIMLock_OTA_Unlock_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_OTA_Unlock_t,
                                    RESPONSE_SIMLOCK_OTA_UNLOCK);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_simlock_ota_unlock(context, &request->Buf[0],
                   request->BufLen,
                   (cops_simlock_ota_reply_message_t *)&response->ReplyMessage);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_VERIFYCONTROLKEYS: {
            Request_SIMLock_VerifyControlKeys_t *request = NIL;
            Response_SIMLock_VerifyControlKeys_t *response = NIL;

            request = (Request_SIMLock_VerifyControlKeys_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_VerifyControlKeys_t,
                                    RESPONSE_SIMLOCK_VERIFYCONTROLKEYS);
            response->SigSelectWithClientTag.ClientTag =
                request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode =
                cops_simlock_verify_control_keys(context,
                                 (cops_simlock_control_keys_t *)&request->Keys);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_SIM_CHANGELOCKCONTROLKEY: {
            Request_SIMLock_SIM_ChangeLockControlKey_t *request = NIL;
            Response_SIMLock_SIM_ChangeLockControlKey_t *response = NIL;

            request = (Request_SIMLock_SIM_ChangeLockControlKey_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_SIM_ChangeLockControlKey_t,
                                    RESPONSE_SIMLOCK_SIM_CHANGELOCKCONTROLKEY);
            response->SigSelectWithClientTag.ClientTag =
                    request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode = cops_simlock_change_sim_control_key(context,
                         (cops_simlock_control_key_t *)&request->OldControlKey,
                         (cops_simlock_control_key_t *)&request->NewControlKey);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        default: {
            break;
        }
    }

    return signal_handled;
}

static void cops_api_proxy_ota_unlock_status_updated_cb(void *event_aux,
                          const cops_simlock_ota_unlock_status_t *unlock_status)
{
    Event_SIMLock_OTA_UnlockStatusUpdated_t *signal = NIL;
    COPS_LOG(LOG_INFO, "Callback received\n");

    signal = SIGNAL_ALLOC(Event_SIMLock_OTA_UnlockStatusUpdated_t,
                          EVENT_SIMLOCK_OTA_UNLOCKSTATUSUPDATED);

    signal->UnlockStatus.Network = unlock_status->network;
    signal->UnlockStatus.NetworkSubset = unlock_status->network_subset;
    signal->UnlockStatus.ServiceProvider = unlock_status->service_provider;
    signal->UnlockStatus.Corporate = unlock_status->corporate;

    (void)Do_Dispatch(EVENT_ID_SIMLOCK_OTA_UNLOCKSTATUSUPDATED, FALSE,
                      sizeof(Event_SIMLock_OTA_UnlockStatusUpdated_t),
                      (union SIGNAL *)signal);
}

static void cops_api_proxy_simlock_status_changed_cb(void *event_aux,
                                                const cops_sim_status_t *status)
{
    Event_SIMLock_StatusChanged_t *signal = NIL;
    COPS_LOG(LOG_INFO, "Callback received\n");

    signal = SIGNAL_ALLOC(Event_SIMLock_StatusChanged_t,
                          EVENT_SIMLOCK_STATUSCHANGED);

    signal->SIMLock_SIM_CardStatus = status->card_status;
    signal->SIMLock_SIM_CardStatusChanged = TRUE;
    signal->SIMLock_FailedLock = -1; /* 0xFF - declared as SIGNED_ENUM8 */

    (void)Do_Dispatch(EVENT_ID_SIMLOCK_STATUSCHANGED, FALSE,
                      sizeof(Event_SIMLock_StatusChanged_t),
                      (union SIGNAL *)signal);
}

static void cops_api_proxy_authentication_changed_cb(void *event_aux)
{
    Event_Authentication_Changed_t *signal = NIL;
    COPS_LOG(LOG_INFO, "Callback received\n");

    signal = SIGNAL_ALLOC(Event_Authentication_Changed_t,
                          EVENT_COPS_AUTHENTICATION_CHANGED);

    (void)Do_Dispatch(EVENT_ID_COPS_AUTHENTICATION_CHANGED, FALSE,
                      sizeof(Event_Authentication_Changed_t),
                      (union SIGNAL *)signal);
}


/*************************************************************************
 * Copyright ST-Ericsson 2011
 ************************************************************************/
#include <cops_api_proxy.h>
#include <r_sim_si.h>
#include <r_dispatch.h>
#include <g_cops_cops.h>
#include <g_cops_simlock.h>

extern uint8_t run_mode;

/* Definitions used to set meta data of IMEI in COPS_IMEI_t struct */
#define IMEI_IDENTITY           2 /* 1=IMSI, 2=IMEI */
#define IMEI_COMPRESSED_LENGTH  8 /* Compressed length (4 bits/digit).
                                     This value is kept only for backward
                                     compatibility. */

static void convert_simlock_status(cops_simlock_lock_status_t *in,
                                   SIMLock_LockStatus_t *out);

static void cops_api_proxy_ousu_cb(void *event_aux,
                                   const cops_simlock_ota_unlock_status_t
                                   *unlock_status);
static void cops_api_proxy_ssc_cb(void *event_aux,
                                  const cops_sim_status_t *status);

void cops_api_proxy_startup(cops_context_id_t **context)
{
    cops_event_callbacks_t cbs = {cops_api_proxy_ousu_cb,
                                  cops_api_proxy_ssc_cb};

    if (COPS_RUN_MODE_MODEM == run_mode) {
        RequestStatus_t status;

        status = Request_EventChannel_Subscribe(WAIT_RESPONSE, EVENT_SUBSCRIBE,
                                                EVENT_ID_SIM_STATE_CHANGED);

        if (REQUEST_OK != status) {
            COPS_LOG(LOG_WARNING,
                     "Failed to subscribe to SIM event: 0x%x\n", status);
        }

        status = Request_SIM_PowerOn(DONT_WAIT);

        if (REQUEST_PENDING != status) {
            COPS_LOG(LOG_WARNING,
                     "Request_SIM_PowerOn() failed: 0x%x\n", status);
        }
    }

    (void)cops_context_create(context, &cbs, NULL);
}


int cops_api_proxy_handle_signal(union SIGNAL *signal,
                                 cops_context_id_t *context)
{
    int signal_handled = 0;

#ifdef COPS_IM_STUB_LEVEL_API_PROXY
    (void)context;
#endif

    switch (signal->sig_no) {
        case REQUEST_COPS_READIMEI: {
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            uint8_t imei[] = {3, 5, 0, 0, 5, 0, 5, 0, 0, 0, 0, 9, 6, 9, 2};
#else
            cops_imei_t imei;
#endif
            Request_COPS_ReadIMEI_t *request = NIL;
            Response_COPS_ReadIMEI_t *response = NIL;

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
            response->ReturnCode = cops_read_imei(context, &imei);

            if (COPS_RC_OK == response->ReturnCode) {
                response->IMEI.Length = IMEI_COMPRESSED_LENGTH;
                response->IMEI.TypeOfIdentity = IMEI_IDENTITY;
                response->IMEI.OddEvenIndication = 1;
                memcpy(response->IMEI.Digits, imei.digits,
                        COPS_UNPACKED_IMEI_LENGTH);
            }
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_GETSTATUS: {
#ifndef COPS_IM_STUB_LEVEL_API_PROXY
            cops_simlock_status_t status;
#endif
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
            response->Status.Locks.Network.LockType =
                    SIMLOCK_LOCK_TYPE_NETWORK_LOCK;
            response->Status.Locks.Network.AttemptsLeft = 10;
            response->Status.Locks.NetworkSubset.LockType =
                    SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK;
            response->Status.Locks.NetworkSubset.AttemptsLeft = 10;
            response->Status.Locks.ServiceProvider.LockType =
                    SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK;
            response->Status.Locks.ServiceProvider.AttemptsLeft = 10;
            response->Status.Locks.Corporate.LockType =
                    SIMLOCK_LOCK_TYPE_CORPORATE_LOCK;
            response->Status.Locks.Corporate.AttemptsLeft = 10;
            response->Status.Locks.LockToSIM.LockType =
                    SIMLOCK_LOCK_TYPE_LOCK_TO_SIM_LOCK;
            response->Status.Locks.LockToSIM.AttemptsLeft = 10;
            response->Status.Locks.FlexibleESL.LockType =
                    SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK;
            response->Status.Locks.FlexibleESL.AttemptsLeft = 10;

            response->ReturnCode = COPS_RC_OK;
#else
            response->ReturnCode = cops_simlock_get_status(context, &status);

            if (COPS_RC_OK == response->ReturnCode) {
                response->Status.SIM_CardStatus = status.sim_card_status;
                convert_simlock_status(&status.nl_status,
                                       &response->Status.Locks.Network);
                convert_simlock_status(&status.nsl_status,
                                       &response->Status.Locks.NetworkSubset);
                convert_simlock_status(&status.spl_status,
                                       &response->Status.Locks.ServiceProvider);
                convert_simlock_status(&status.cl_status,
                                       &response->Status.Locks.Corporate);
                convert_simlock_status(&status.siml_status,
                                       &response->Status.Locks.LockToSIM);
                convert_simlock_status(&status.esll_status,
                                       &response->Status.Locks.FlexibleESL);
            }
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_VERIFYIMSI: {
#ifndef COPS_IM_STUB_LEVEL_API_PROXY
            int i;
            struct cops_simlock_imsi imsi;
#endif
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
            for (i = 0; i < COPS_PACKED_IMSI_LENGTH; i++) {
                imsi.data[i] = request->IMSI.Data[i];
            }

            response->ReturnCode = cops_simlock_verify_imsi(context, &imsi);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case RESPONSE_SIM_POWERON: {
            UICC_ServiceHandle_t service_handle;
            SIM_PinRefDataId_t id;
            SIM_State_t state;
            RequestStatus_t status = Response_SIM_PowerOn(signal);

            if (REQUEST_OK != status) {
                COPS_LOG(LOG_WARNING, "Response_SIM_PowerOn() failed. "
                         "RequestStatus=0x%x\n", status);
            }

            status = Request_SIM_State(WAIT_RESPONSE, &service_handle,
                                       &id, &state);

            if (REQUEST_OK == status && SIM_STATE_ACTIVE == state) {
                /* COPS_Core_Init_SimState */
            }

            signal_handled = 1;
            break;
        }
        case EVENT_SIM_STATE_CHANGED: {
            UICC_ServiceHandle_t service_handle;
            SIM_PinRefDataId_t id;
            SIM_State_t state;
            EventStatus_t status = Event_SIM_State_Changed(signal,
                                                           &service_handle,
                                                           &id, &state);

            if (GS_EVENT_OK == status && SIM_STATE_ACTIVE == state) {
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
                cops_sim_status_t sim;

                sim.card_status = COPS_SIMLOCK_CARD_STATUS_APPROVED;
                cops_api_proxy_ssc_cb(NULL, &sim);
#else
                struct cops_simlock_imsi imsi;
                SIM_ISO_ErrorCause_t error_cause;

                RequestStatus_t request_status =
                        Request_SIM_IMSI_Read(WAIT_RESPONSE,
                                              service_handle,
                                              imsi.data,
                                              &error_cause);

                if (REQUEST_OK == request_status) {
                    cops_return_code_t ret_code;
                    cops_sim_status_t status;

                    ret_code = cops_simlock_verify_imsi(context, &imsi);

                    if (COPS_RC_OK == ret_code) {
                        status.card_status = COPS_SIMLOCK_CARD_STATUS_APPROVED;
                    } else {
                        status.card_status =
                                COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED;
                    }
                    cops_api_proxy_ssc_cb(NULL, &status);
                }
#endif
            } else {
                /*COPS_Core_Lock_SetCardStatus(&NotCheckedStatus,
                                               &NotCheckedStatus) */
            }

            signal_handled = 1;
            break;
        }
        case REQUEST_MODEMLOCK_GETSTATUS: {
            Request_ModemLock_GetStatus_t *request = NIL;
            Response_ModemLock_GetStatus_t *response = NIL;

            request = (Request_ModemLock_GetStatus_t *)signal;

            response = SIGNAL_ALLOC(Response_ModemLock_GetStatus_t,
                                    RESPONSE_MODEMLOCK_GETSTATUS);
            response->SigSelectWithClientTag.ClientTag =
                    request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
            response->ReturnCode = COPS_RC_OK;

            if (COPS_RC_OK == response->ReturnCode) {
                response->Status.SIM_CardStatus =
                        SIMLOCK_SIM_CARD_STATUS_APPROVED;
                response->Status.Lock.LockType = SIMLOCK_LOCK_TYPE_MODEM_LOCK;
                response->Status.Lock.LockDefinition = 0;
                response->Status.Lock.Setting = SIMLOCK_LOCK_SETTING_UNLOCKED;
                response->Status.Lock.AttemptsLeft = 0;
                response->Status.Lock.TimerIsRunning = FALSE;
                response->Status.Lock.TimeLeftOnRunningTimer.Hours = 0;
                response->Status.Lock.TimeLeftOnRunningTimer.Minutes = 0;
                response->Status.Lock.TimeLeftOnRunningTimer.Seconds = 0;
                response->Status.Lock.TimerAttemptsLeft = 0;
            }

            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_COPS_BINDDATA: {
            struct cops_digest mac;
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
                    cops_bind_data(context,
                                   (uint8_t *)request->Data,
                                   request->DataLength,
                                   (struct cops_digest *)&response->MAC);
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
                    cops_verify_data_binding(context,
                                           (uint8_t *)request->Data,
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
            response->ReturnCode = cops_verify_signed_header(context,
                                   request->Header,
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
                    cops_calcdigest(context,
                                    (enum cops_hash_type)request->HashType,
                                    (uint8 *)request->Data,
                                    request->DataLength,
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
            response->ReturnCode = cops_get_device_state(context,
                                 (cops_device_state_t *)&response->DeviceState);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_CHANGESIMCONTROLKEY: {
            Request_SIMLock_ChangeSimControlKey_t *request = NIL;
            Response_SIMLock_ChangeSimControlKey_t *response = NIL;

            request = (Request_SIMLock_ChangeSimControlKey_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_ChangeSimControlKey_t,
                                    RESPONSE_SIMLOCK_CHANGESIMCONTROLKEY);
            response->SigSelectWithClientTag.ClientTag =
                    request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            response->ReturnCode = cops_simlock_change_sim_control_key(context,
                              (cops_simlock_control_key_t *)&request->Old_Key,
                              (cops_simlock_control_key_t *)&request->New_Key);
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_UNLOCK: {
            Request_SIMLock_Unlock_t *request = NIL;
            Response_SIMLock_Unlock_t *response = NIL;
#ifndef COPS_IM_STUB_LEVEL_API_PROXY
            uint8_t i;
#endif
            request = (Request_SIMLock_Unlock_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_Unlock_t,
                                    RESPONSE_SIMLOCK_UNLOCK);
            response->SigSelectWithClientTag.ClientTag =
                    request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            for (i = 0; i < request->Unlock.NoOfUnlockData; i++) {

                response->FailingLock =
                        request->Unlock.UnlockData_p[i].LockType;
                response->ReturnCode = cops_simlock_unlock(context,
                                    request->Unlock.UnlockData_p[i].LockType,
     (cops_simlock_control_key_t *)&request->Unlock.UnlockData_p[i].ControlKey);

                if (COPS_RC_OK != response->ReturnCode) {
                    break;
                }
            }
#endif
            SEND(&response, SENDER(&request));
            signal_handled = 1;
            break;
        }
        case REQUEST_SIMLOCK_LOCK: {
            Request_SIMLock_Lock_t *request = NIL;
            Response_SIMLock_Lock_t *response = NIL;
            uint8_t i;
            cops_simlock_lock_arg_t lock_arg;
            struct cops_simlock_explicitlockdata explicitlockdata;

            request = (Request_SIMLock_Lock_t *)signal;

            response = SIGNAL_ALLOC(Response_SIMLock_Lock_t,
                                    RESPONSE_SIMLOCK_LOCK);
            response->SigSelectWithClientTag.ClientTag =
                    request->SigSelectWithClientTag.ClientTag;
            response->RequestStatus = REQUEST_OK;
#ifdef COPS_IM_STUB_LEVEL_API_PROXY
            (void)i;
            (void)lock_arg;
            (void)explicitlockdata;
            response->ReturnCode = COPS_RC_SERVICE_NOT_AVAILABLE_ERROR;
#else
            memset(&lock_arg, 0, sizeof(cops_simlock_lock_arg_t));

            memset(&explicitlockdata, 0,
                              sizeof(struct cops_simlock_explicitlockdata));

            lock_arg.lockmode = request->Lock.LockMode;

            if (NULL != request->Lock.ExplicitLockData_p) {
                memcpy(explicitlockdata.imsi.data,
                       request->Lock.ExplicitLockData_p->IMSI.Data,
                       SIM_IMSI_LEN);
                explicitlockdata.gid1 = request->Lock.ExplicitLockData_p->GID1;
                explicitlockdata.gid2 = request->Lock.ExplicitLockData_p->GID2;
                explicitlockdata.length =
                        request->Lock.ExplicitLockData_p->FlexibleESL_Length;

                if (explicitlockdata.length > 0) {
                    explicitlockdata.data = malloc(explicitlockdata.length);
                    memcpy(explicitlockdata.data,
                           request->Lock.ExplicitLockData_p->FlexibleESL_Data_p,
                           explicitlockdata.length);
                }

                lock_arg.explicitlockdata = &explicitlockdata;
            }

            for (i = 0; i < request->Lock.NoOfLockData; i++) {

                lock_arg.lockdata.locktype =
                        request->Lock.LockData_p[i].LockType;
                memcpy(lock_arg.lockdata.controlkey.value,
                       request->Lock.LockData_p[i].ControlKey.ControlKey,
                       COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH + 1);
                lock_arg.lockdata.lockop = COPS_SIMLOCK_LOCKOP_SETLOCKDATA |
                                           COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA |
                                           COPS_SIMLOCK_LOCKOP_SETLOCKSETTING;
                lock_arg.lockdata.update_lockdef =
                        request->Lock.LockData_p[i].UpdateLockDefinition;
                lock_arg.lockdata.lockdef =
                        request->Lock.LockData_p[i].LockDefinition;

                response->FailingLock = request->Lock.LockData_p[i].LockType;
                response->ReturnCode = cops_simlock_lock(context, &lock_arg);

                if (COPS_RC_OK != response->ReturnCode) {
                    break;
                }
            }

            if (NULL != explicitlockdata.data) {
                free(explicitlockdata.data);
            }
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

static void convert_simlock_status(cops_simlock_lock_status_t *in,
                                   SIMLock_LockStatus_t *out)
{
    out->LockType = in->lock_type;
    out->LockDefinition = in->lock_definition;
    out->Setting = in->lock_setting;
    out->AttemptsLeft = in->attempts_left;
    out->TimerIsRunning = in->timer_is_running;
    out->TimeLeftOnRunningTimer.Hours = in->time_left_on_running_timer / 3600;
    out->TimeLeftOnRunningTimer.Minutes =
            (in->time_left_on_running_timer % 3600) / 60;
    out->TimeLeftOnRunningTimer.Seconds = in->time_left_on_running_timer % 60;
    out->TimerAttemptsLeft = in->timer_attempts_left;
}

void cops_api_proxy_ousu_cb(void *event_aux,
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

void cops_api_proxy_ssc_cb(void *event_aux,
                           const cops_sim_status_t *status)
{
    Event_SIMLock_StatusChanged_t *signal = NIL;
    COPS_LOG(LOG_INFO, "Callback received\n");
    signal = SIGNAL_ALLOC(Event_SIMLock_StatusChanged_t,
                          EVENT_SIMLOCK_STATUSCHANGED);

    signal->ModemLock_SIM_CardStatus = status->card_status;
    signal->ModemLock_SIM_CardStatusChanged = TRUE;
    signal->SIMLock_SIM_CardStatus = status->card_status;
    signal->SIMLock_SIM_CardStatusChanged = TRUE;

    (void)Do_Dispatch(EVENT_ID_SIMLOCK_STATUSCHANGED, FALSE,
                      sizeof(Event_SIMLock_StatusChanged_t),
                      (union SIGNAL *)signal);
}

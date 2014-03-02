/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#define ADM_LOG_FILENAME "api_modem"



#include "ste_adm_api_modem.h"

#ifndef ADM_ENABLE_FEATURE_FAT_MODEM

  ste_adm_res_t modem_init(){ return STE_ADM_RES_OK;}
  ste_adm_res_t modem_deinit(){ return STE_ADM_RES_OK;}

  modem_state_t modem_get_state(){ return ADM_MODEM_STATE_ON;}
  ste_adm_res_t modem_set_samplerate(int samplerate){ (void)samplerate; return STE_ADM_RES_OK;}
  ste_adm_res_t modem_configure_speech_proc(int samplerate, const char* input_dev, const char* output_dev){ (void)samplerate; (void)input_dev; (void)output_dev; return STE_ADM_RES_OK;}
  ste_adm_res_t modem_set_volume(int volume){ (void)volume ; return STE_ADM_RES_OK;}
  ste_adm_res_t modem_set_tx_path(modem_path_t path_tx){ (void)path_tx ; return STE_ADM_RES_OK;}
  ste_adm_res_t modem_set_rx_path(modem_path_t path_rx){ (void)path_rx ; return STE_ADM_RES_OK;}
  ste_adm_res_t modem_reset_all_paths(){ return STE_ADM_RES_OK;}
  ste_adm_res_t modem_set_rec_rx_path(modem_path_t path_rec){ (void)path_rec ; return STE_ADM_RES_OK;}
  ste_adm_res_t modem_set_rec_tx_path(modem_path_t path_rec){ (void)path_rec ; return STE_ADM_RES_OK;}
  ste_adm_res_t modem_wait_end_of_configure_speechproc_comp(void){ return ADM_MODEM_STATE_ON;}
  ste_adm_res_t modem_set_modem_loop(int loopback_type, int loop_enable, int codec_type){ (void)loopback_type; (void)loop_enable; (void)codec_type;return STE_ADM_RES_OK;}
#else

#include <string.h>
#include <linux/caif/caif_socket.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <dbus/dbus.h>
#include <atchannel.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>


#include "ste_adm_dbg.h"
#include "ste_adm_srv.h"
#include "ste_adm_db.h"
#include "ste_adm_cscall_omx.h"
#include "ste_adm_client.h"


// Number of times to try to connect to D-Bus before giving up
#define ADM_MODEM_DBUS_CONNECT_ATTEMPTS        5
// Maximum time to sleep between D-Bus reconnect attempts in seconds
#define ADM_MODEM_DBUS_MAX_SLEEP_TIME          10

#define MIN(A,B) (((A)<(B))?(A):(B))

#define ADM_MODEM_AT_MAX_STRING_SIZE 256 /* TBC (might be reduced) */


#define ADM_MODEM_DBUS_CONNECTION_NAME  "com.stericsson.mid"
#define ADM_MODEM_DBUS_OBJECT_PATH      "/com/stericsson/mid/Modem"
#define ADM_MODEM_DBUS_OBJECT_INTERFACE "com.stericsson.mid.Modem"
#define ADM_MODEM_DBUS_GET_MODEM_ID     "GetModemId"
#define ADM_MODEM_DBUS_GET_MODEM_STATE  "GetState"
#define ADM_MODEM_DBUS_STATE_CHANGE     "StateChange"

int modem_dbus_init(void);
int modem_dbus_deinit(void);
DBusHandlerResult modem_dbus_state_change_handler(DBusConnection* connection __attribute__((unused)), DBusMessage* message, void* user_data);
int modem_dbus_send_command(char *Command, char**Result);
int modem_dbus_get_modem_id(char **modemId);
int modem_dbus_get_modem_state_char(char **modemState);
modem_state_t modem_dbus_get_modem_state(void);
modem_state_t modem_dbus_convert_modem_state_in_state(char * modemstate);
char * modem_dbus_convert_modem_state_in_char( modem_state_t modemstate);

int modem_at_init();
int modem_at_deinit();
void modem_at_on_unsolicited(const char *s, const char *pdu, void *userData );
int modem_at_send_command(const char *fmt, ...) ;



inline int modem_dbus_is_modem_on(void);
void srv_send_modem_event_msg( modem_state_t state );
int modem_copy_already_done(void);
void modem_copy_apf_file(void);

static modem_state_t g_adm_modem_state;


ste_adm_res_t modem_init()
{
    g_adm_modem_state = ADM_MODEM_STATE_UNKNOWN;

    if (modem_dbus_init()) {
        ALOG_ERR("Undable to initialiaze Dbus services ");
        return STE_ADM_RES_UNRECOVERABLE_ERROR;
    }
    int type = adm_get_modem_type();
    if((type==ADM_FAT_MODEM) || (type==ADM_FULL_FAT_MODEM)){
        if(modem_copy_already_done()==0)
            modem_copy_apf_file();
    }
    return STE_ADM_RES_OK;
}



ste_adm_res_t modem_deinit()
{
    if(modem_at_deinit()){
        ALOG_ERR("Undable to Stop AT Cmd");
    }

    if(modem_dbus_deinit()){
        ALOG_ERR("Undable to Stop Dbus");
    }
    g_adm_modem_state = ADM_MODEM_STATE_UNKNOWN;

    return STE_ADM_RES_OK;
}



modem_state_t modem_get_state()
{
    return g_adm_modem_state;
}

///////////// AT Command Interface ///////////
static int fd_socket_caif = -1;

int modem_at_init()
{
    int ret = -1;

    struct sockaddr_caif addr;
    addr.family = AF_CAIF;
    addr.u.at.type = CAIF_ATTYPE_PLAIN;

    ac_alloc();

    fd_socket_caif = socket(AF_CAIF, SOCK_SEQPACKET, CAIFPROTO_AT);

    ALOG_INFO("%s: SOCKET errno=%d", __func__, errno);
    ALOG_INFO("%s: SOCKET fd=%d", __func__, fd_socket_caif);

    if (unlikely(fd_socket_caif < 0)) {
        /* Open will fail if CAIF is not ready yet
        then the device will be opened in setModemRouting
        However, Constructor should have been called only after CAIF is up and running
        */
        ALOG_ERR("%s: socket(%x, %x, %x) failed", __func__, AF_CAIF, SOCK_SEQPACKET, CAIFPROTO_AT);
        return fd_socket_caif;
    }
    ret = connect(fd_socket_caif, (struct sockaddr *) &addr, sizeof(addr));
        if (ret) {
        ALOG_ERR("Unsuccessful(ret:%d) connect to AT socket. "
        "Errno:%d, %s\n", ret, errno, strerror(errno));
        return ret;
    }

    int rc = at_open(fd_socket_caif, modem_at_on_unsolicited,NULL);

    if (unlikely(rc < 0)) {
        ALOG_ERR("%s: at_open() failed: %d", __func__, rc);
        at_close();
        close(fd_socket_caif);
    }
    else {
        at_make_default_channel();
    }

    return 0;
}
int modem_at_deinit()
{
    at_close();
    close(fd_socket_caif);
    fd_socket_caif = -1;
    return 0;
}

void modem_at_on_unsolicited(const char *s, const char *pdu, void *userData)
{
     (void)pdu;
     (void)userData;
     ALOG_INFO("modem_AT_on_unsolicited: %s ", s );
}

static unsigned int tv_ms_diff(struct timeval a, struct timeval b)
{
    return (unsigned int) (1000 * (a.tv_sec - b.tv_sec) + (a.tv_usec - b.tv_usec) / 1000);
}

int modem_at_send_command(const char *fmt, ...) {
    char at_string[ADM_MODEM_AT_MAX_STRING_SIZE];
    ATResponse *atresponse = NULL;
    int err;
    struct timeval tv_start,tv_end;

    va_list args;

    va_start(args, fmt);
    vsprintf(at_string, fmt, args);
    va_end(args);
    ALOG_INFO(">%s(%s)",__func__,at_string);

    if(g_adm_modem_state!=ADM_MODEM_STATE_ON){
        ALOG_INFO("%s: Trying to send (%s) At command while modem is in %s state ",__func__, at_string,modem_dbus_convert_modem_state_in_char(g_adm_modem_state));
        return -1;
    }

    gettimeofday(&tv_start, NULL);
    err = at_send_command(at_string, &atresponse);
    gettimeofday(&tv_end, NULL);

    unsigned int time_used = tv_ms_diff(tv_end, tv_start);
    ALOG_INFO("AT Command \"%s\" sent in  %u ms\n",at_string, time_used);

    if ((err < 0) || (atresponse != NULL && atresponse->success != 1)) {
        char finalresponse[100];

        /*
        *copy string to avoid a crash in case of invalid
        *response: force last indexes to 0
        */
        if (atresponse != NULL) {
            strncpy(finalresponse,atresponse->finalResponse,100);
            finalresponse[99] ='\0';

            ALOG_ERR("%s: Unable to send (%s) returned : %s ",  __func__, at_string, finalresponse);
            at_response_free(atresponse);
        } else {
            ALOG_ERR("%s: Unable to send (%s) returned : %s ",  __func__, at_string);
        }
        return -1;
    }
    ALOG_INFO("<%s received %s ",__func__, (atresponse != NULL ? atresponse->finalResponse : "atresponse = NULL" ));

    return 0;
}

///////////////DBUS interface////

static DBusError dbusError;
static DBusConnection *dbusConnection = NULL;
static int g_modem_thread_alive;


#define DBUS_ERROR_MSG(dbusError) \
if (dbus_error_is_set(&dbusError)) { \
    ALOG_ERR("DBus error in function '%s' line %d: name: %s message: %s", \
            __func__, __LINE__, dbusError.name, dbusError.message); \
    dbus_error_free(&dbusError); \
}


void* modem_dbus_thread(void* param){
    char        *modemId;
    (void)param;
    ALOG_INFO("Dbus modem thread created ...");

    if(modem_dbus_is_modem_on()){
        g_adm_modem_state = ADM_MODEM_STATE_ON;
        if (modem_dbus_get_modem_id(&modemId)) {
            ALOG_ERR("Unable to get the Modem Id ");
            g_adm_modem_state = ADM_MODEM_STATE_UNKNOWN;
            g_modem_thread_alive= 0;
            return NULL;
        }
        ALOG_STATUS("Modem ID: %s", modemId);

        int type = adm_get_modem_type();
        // At command are supported only if we are FAT Modem
        if((type==ADM_FAT_MODEM) || (type==ADM_FULL_FAT_MODEM)){
            //Modem is On, we can activate AT command channel
            if(modem_at_init()){
                ALOG_ERR("Undable to start AT Cmd");
                g_adm_modem_state = ADM_MODEM_STATE_UNKNOWN;
                g_modem_thread_alive= 0;
                return NULL;
            }
            modem_set_samplerate(16000);
            // Workarround : disable default config before enable new ones. To remove after ER444592 on modem side
            modem_reset_all_paths();
            // Enable Rx and Tx paths on modem side
            modem_set_tx_path(ADM_MODEM_PATH_OPEN);
            modem_set_rx_path(ADM_MODEM_PATH_OPEN);
        }

        if(adm_get_modem_type()==ADM_HALF_SLIM_MODEM){
            // In case of Half Slim the cscall module has to be started when the modem is On
            adm_cscall_create_cscall_cmp_slim(&g_cscall_omx_state);
        }

        ALOG_STATUS("MODEM is ON");
    }
    else {
        ALOG_STATUS("Modem not yet powered On -> Waiting for Call Back");
    }

    // Start to listen even from the modem

    while(g_modem_thread_alive){
        dbus_connection_read_write_dispatch(dbusConnection, -1);
    }
    ALOG_INFO("Dbus modem thread destroyed ...");
    return NULL;
}

int modem_dbus_init(void)
{
    int status = 0;
    DBusError err;
    int num_attempts = 0;
    int sleep_time = 1;

    dbus_error_init(&err);
    do {
        dbusConnection = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
        if (dbus_error_is_set(&err)) {
            ALOG_ERR("%s(): DBus Connection Error (%s)\n", __func__, err.message);
            dbus_error_free(&err);
        }
        num_attempts++;
        if (dbusConnection == NULL) {
            ALOG_WARN("Attempt : %d - DBUS not yet iniatilzed, wait for %d sec ",num_attempts, sleep_time);
            sleep(sleep_time);
            sleep_time = MIN(sleep_time * 2, ADM_MODEM_DBUS_MAX_SLEEP_TIME);
        }
    }while (dbusConnection == NULL && num_attempts < ADM_MODEM_DBUS_CONNECT_ATTEMPTS);
    if (dbusConnection == NULL) {
        status = -1;
        ALOG_ERR("D-Bus connection still failed after %d attempts",num_attempts);
        goto exit;
    }

    // listen to MID state messages
    dbus_bus_add_match(dbusConnection, "type='signal',"
        "interface='"ADM_MODEM_DBUS_OBJECT_INTERFACE"'", &err);

    if (dbus_error_is_set(&err)) {
        ALOG_ERR("DBUS match error %s: %s", err.name, err.message);
        dbus_error_free(&err);
        status = -1;
        goto exit;
    }

    if(!dbus_connection_add_filter(dbusConnection, modem_dbus_state_change_handler, NULL, NULL)){
        ALOG_ERR("DBUS filter error");
        goto exit;
    }


    pthread_t thread;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    g_modem_thread_alive = 1;

    if (pthread_create(&thread, &tattr, modem_dbus_thread, NULL)) {
        ALOG_ERR("pthread_create() failed\n");
    }

exit:
    return status;
}
int modem_dbus_deinit(void)
{
    dbus_connection_remove_filter(dbusConnection,modem_dbus_state_change_handler,NULL );
    dbus_connection_unref(dbusConnection);
    dbusConnection = NULL;
    g_modem_thread_alive = 0;
    return 0;
}

DBusHandlerResult modem_dbus_state_change_handler(DBusConnection* connection __attribute__((unused)), DBusMessage* message, void* user_data)
{
    char *state = NULL;
    char *reason = NULL;
    DBusError err;
    DBusHandlerResult result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    (void)user_data;
    modem_state_t modem_state;

    dbus_error_init(&err);

    if (!dbus_message_is_signal(message,
            ADM_MODEM_DBUS_OBJECT_INTERFACE, ADM_MODEM_DBUS_STATE_CHANGE)) {
        ALOG_WARN("%s(): got unknown message \"%s.%s\" from D-Bus. Ignoring",
            __func__,
            dbus_message_get_interface(message),
            dbus_message_get_member(message));
        goto exit;
    }

    result = DBUS_HANDLER_RESULT_HANDLED;
    if (!dbus_message_get_args(message, &err, DBUS_TYPE_STRING, &state, DBUS_TYPE_STRING,&reason, DBUS_TYPE_INVALID)) {
        ALOG_WARN("%s(): got a malformed " ADM_MODEM_DBUS_STATE_CHANGE " message: %s!", __func__, err.message);
        dbus_error_free(&err);
        goto exit;
    }

    ALOG_INFO("Modem state changed : %s", state);

    modem_state = modem_dbus_convert_modem_state_in_state(state);

    srv_send_modem_event_msg(modem_state);

exit:
    return result;
}
int modem_dbus_send_command(char *Command, char**Result)
{
    int error = 0;
    DBusMessage* message;
    DBusMessageIter args;
    DBusPendingCall* pending;


    message = dbus_message_new_method_call( ADM_MODEM_DBUS_CONNECTION_NAME,
                                            ADM_MODEM_DBUS_OBJECT_PATH,
                                            ADM_MODEM_DBUS_OBJECT_INTERFACE,
                                            Command);
    DBUS_ERROR_MSG(dbusError);
    if (message == NULL) {
        ALOG_ERR("%s: Error DBus message NULL", __func__);
        return -ENOSYS;
    }

    if (!dbus_connection_send_with_reply(dbusConnection, message, &pending, -1)) {
        DBUS_ERROR_MSG(dbusError);
        ALOG_ERR("%s: DBus Out Of Memory", __func__);
        return -ENOSYS;
    }
    if (pending == NULL) {
        DBUS_ERROR_MSG(dbusError);
        ALOG_ERR("%s: DBus Pending Call Null", __func__);
        return -ENOSYS;
    }

    dbus_connection_flush(dbusConnection);
    dbus_message_unref(message);

    dbus_pending_call_block(pending);

    message = dbus_pending_call_steal_reply(pending);
    if (message == NULL) {
        DBUS_ERROR_MSG(dbusError);
        ALOG_ERR("%s: No method reply", __func__);
        return -ENOSYS;
    }

    dbus_pending_call_unref(pending);

    if (!dbus_message_iter_init(message, &args)) {
        DBUS_ERROR_MSG(dbusError);
        ALOG_ERR("%s: No reply argument", __func__);
        return -ENOSYS;
    }

    if (dbus_message_iter_get_arg_type(&args) == DBUS_TYPE_STRING) {
        dbus_message_iter_get_basic(&args, Result);
    } else {
        DBUS_ERROR_MSG(dbusError);
        ALOG_ERR("%s: wrong reply argument type", __func__);
        return -ENOSYS;
    }

    dbus_message_unref(message);
    ALOG_INFO("modem_dbusSendCommand sent : %s, received : %s ",Command,*Result);

    return error;
}

int modem_dbus_get_modem_id(char **modemId)
{
    int error = 0;

    error = modem_dbus_send_command(ADM_MODEM_DBUS_GET_MODEM_ID, modemId);

    return error;
}


int modem_dbus_get_modem_state_char(char **modemState)
{
    int error = 0;

    error = modem_dbus_send_command(ADM_MODEM_DBUS_GET_MODEM_STATE, modemState);

    return error;
}
modem_state_t modem_dbus_convert_modem_state_in_state(char * modemstate)
{
    if (!strcmp(modemstate, "unknown"))
        return  ADM_MODEM_STATE_UNKNOWN;

    if (!strcmp(modemstate, "booting"))
        return  ADM_MODEM_STATE_BOOTING;

    if (!strcmp(modemstate, "upgrading"))
        return  ADM_MODEM_STATE_UPGRADING;

    if (!strcmp(modemstate, "on"))
        return  ADM_MODEM_STATE_ON;

    if (!strcmp(modemstate, "dumping"))
        return  ADM_MODEM_STATE_DUMPING;

    if (!strcmp(modemstate, "prepare_off"))
        return  ADM_MODEM_STATE_PREPARE_OFF;

    if (!strcmp(modemstate, "off")) {
        return  ADM_MODEM_STATE_OFF;
    } else {
        ALOG_ERR("Wrong Modem state");
        return  ADM_MODEM_STATE_UNKNOWN;
    }
}

char * modem_dbus_convert_modem_state_in_char( modem_state_t modemstate)
{
    switch(modemstate){
        case ADM_MODEM_STATE_UNKNOWN : return "unknown";
        case ADM_MODEM_STATE_BOOTING : return "booting";
        case ADM_MODEM_STATE_UPGRADING : return "upgrading";
        case ADM_MODEM_STATE_ON : return "on";
        case ADM_MODEM_STATE_DUMPING : return "dumping";
        case ADM_MODEM_STATE_OFF : return "off";
        case ADM_MODEM_STATE_PREPARE_OFF : return "prepare_off";
        default : return "unknown";
    }
}

modem_state_t modem_dbus_get_modem_state(void)
{
    char *modemState;

    if (modem_dbus_get_modem_state_char(&modemState))
        return  ADM_MODEM_STATE_UNKNOWN;

    return (modem_dbus_convert_modem_state_in_state(modemState));

}

inline int modem_dbus_is_modem_on(void)
{
    return (modem_dbus_get_modem_state() == ADM_MODEM_STATE_ON);
}

void modem_event_adm_ctx(void*  pGenEventData, int  DataSize, void* pGenEventUserData)
{
    modem_state_t next_state, state;
    modem_state_t * p_state;
    p_state = (modem_state_t*)pGenEventData;
    (void)DataSize;
    (void)pGenEventUserData;
    state = *p_state;

    ALOG_INFO("Previous Modem state was %s . New Modem state is %s",
                            modem_dbus_convert_modem_state_in_char(g_adm_modem_state),
                            modem_dbus_convert_modem_state_in_char(state));

    next_state = state;

    if ( state== ADM_MODEM_STATE_ON){
        if(g_adm_modem_state != ADM_MODEM_STATE_ON) {
            ALOG_INFO("modem_dbus_get_modem_id");
            char *modemId ;

            if (modem_dbus_get_modem_id(&modemId)) {
                ALOG_ERR("Unable to get the Modem Id ");
                next_state = ADM_MODEM_STATE_UNKNOWN;
                g_modem_thread_alive= 0;
            }
            ALOG_STATUS("Modem ID: %s", modemId);

            int type = adm_get_modem_type();
            // At command are supported only if we are FAT Modem
            if((type==ADM_FAT_MODEM) || (type==ADM_FULL_FAT_MODEM)){
                //Modem is On, we can activate AT command channel
                if(modem_at_init()){
                    ALOG_ERR("Undable to start AT Cmd");
                    next_state = ADM_MODEM_STATE_UNKNOWN;
                    g_modem_thread_alive= 0;
                }
                g_adm_modem_state = next_state;
                modem_set_samplerate(16000);
                // Workarround : disable default config before enable new ones. To remove after ER444592 on modem side
                modem_reset_all_paths();
                // Enable Rx and Tx paths on modem side
                modem_set_rx_path(ADM_MODEM_PATH_OPEN);
                modem_set_tx_path(ADM_MODEM_PATH_OPEN);
            }

            if(adm_get_modem_type()==ADM_HALF_SLIM_MODEM){
                // In case of Half Slim the cscall module has to be started when the modem is On
                adm_cscall_create_cscall_cmp_slim(&g_cscall_omx_state);
            }

            ALOG_STATUS("MODEM is ON");

        }
    }
    else if (( state== ADM_MODEM_STATE_DUMPING) || (state == ADM_MODEM_STATE_PREPARE_OFF)){
        if(g_adm_modem_state == ADM_MODEM_STATE_ON) {
            ALOG_ERR("!! Modem has crashed !!");
            // cscall must de released to be able to reinitialise modem Commection
            int fd_adm;
            fd_adm = ste_adm_client_connect();
            ALOG_WARN("!! force cscall deactivation !!");
            ste_adm_set_cscall_devices(fd_adm, NULL, NULL);
            ste_adm_client_disconnect(fd_adm);
            int type = adm_get_modem_type();
            if((type==ADM_FAT_MODEM) || (type==ADM_FULL_FAT_MODEM)){
                ALOG_WARN("!! close AT CAIF socket !!");
                modem_at_deinit();
            }
            if(adm_get_modem_type() == ADM_HALF_SLIM_MODEM){
               ALOG_WARN("!! destroy cscall component !!");
               adm_cscall_omx_destroy_cscall();
            }
        }
    }
    g_adm_modem_state = next_state;
}

// This fonction is used to send a message to the ADM normal Thread.
void srv_send_modem_event_msg( modem_state_t state )
{
    modem_event_adm_ctx((void*)(&state), 0, 0);
}


ste_adm_res_t modem_set_samplerate(int samplerate)
{
    int at_samplerate = -1;
    ALOG_INFO(">%s(%d)",__func__, samplerate);
    switch(samplerate){
        case 8000 :
            at_samplerate = 1;
            break;
        case 16000 :
            at_samplerate = 2;
            break;
        default :
            return STE_ADM_RES_INVALID_PARAMETER;
    }
    return modem_at_send_command("AT*EACSR=%d",at_samplerate);
}

#define ADM_MODEM_APF_PROFILE_PATH      "/afs/settings/acoustic"
#define ADM_MODEM_APF_PROFILE_EXT       "apf"
#define ADM_MODEM_COMPLETE_PATH         "/modemfs/C2C/Phone_FS"ADM_MODEM_APF_PROFILE_PATH
#define ADM_SYSTEM_APF_DIRECTORY        "/system/etc/apf/"
#define ADM_EXEC_APF_DIRECTORY          ADM_MODEM_COMPLETE_PATH"/"

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int thread_apf_running = 0;

void * modem_configure_speech_proc_thread(void* param );

ste_adm_res_t modem_configure_speech_proc(int samplerate, const char* input_dev, const char* output_dev)
{
    ste_adm_res_t res = STE_ADM_RES_OK;
    const char *apf_profile = NULL;
    char *apf_path = NULL;
    thread_apf_running = 0;

    ALOG_INFO(">%s(%d,%s,%s)",__func__, samplerate,input_dev,output_dev);
    // 1- GetBack the Acoustic Profile strored in databse according to samplerate, and devices
    res = adm_db_speech_apf_get(input_dev, output_dev, samplerate,&apf_profile);
    if(res!=STE_ADM_RES_OK){
        ALOG_ERR("%s : Apf Profile not found for DevIn %s DevOut %s samplerate%d", __func__ , input_dev,output_dev,samplerate);
        goto cleanup;
    }
    // 2- Verify if profile is available
    if(strcmp("None",apf_profile)==0){
        ALOG_WARN("Apf Profile not set for deviceIn %s DevOut %s samplerate %d - No Config send to modem", input_dev,output_dev,samplerate);
        res = STE_ADM_RES_OK;
        goto cleanup;
    }
    // 3 - Construct the complete path
    apf_path = malloc(256 * sizeof(char));
        if (!apf_path) {
            ALOG_ERR("%s : Out of memory when trying to allocate apf_path",__func__);
            res = STE_ADM_RES_UNRECOVERABLE_ERROR;
            goto cleanup;
    }


    // 4- Verify the validity of the File
    memset(apf_path, 0, 256 * sizeof(char));
    sprintf(apf_path, "%s/%s.%s",ADM_MODEM_COMPLETE_PATH,apf_profile,ADM_MODEM_APF_PROFILE_EXT);
    ALOG_INFO("%s : Verification of the file '%s' ", __func__ , apf_path);

    FILE *apf_file;
    apf_file = fopen(apf_path, "rb");
    if (apf_file == NULL) {
        ALOG_ERR("%s : Failed to open file '%s' ", __func__, apf_path);
        res = STE_ADM_RES_OK;
        goto cleanup;
    }
    fclose(apf_file);

    // 5- Send the At Command
    memset(apf_path, 0, 256 * sizeof(char));
    sprintf(apf_path, "%s/%s.%s",ADM_MODEM_APF_PROFILE_PATH,apf_profile,ADM_MODEM_APF_PROFILE_EXT);
    ALOG_INFO("%s : Send '%s' to modem", __func__ , apf_path);


    pthread_t thread;
    pthread_attr_t tattr;
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    thread_apf_running=1;
    if (pthread_create(&thread, &tattr, modem_configure_speech_proc_thread, (void*)apf_path)) {
        ALOG_ERR("pthread_create() failed\n");
        thread_apf_running=0;
    }

cleanup :
    return STE_ADM_RES_OK;

}
void * modem_configure_speech_proc_thread(void* param )
{
    int ret=0;

    char *apf_path = (char*)param;

    ret= modem_at_send_command("AT*EAPF=0,0,\"%s\"",apf_path);
    if(ret!=0)
        ALOG_ERR("Unable to send AT Command AT*EAPF");

    if(apf_path)
        free(apf_path);

    pthread_mutex_lock(&mut);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mut);
    thread_apf_running=0;
    return 0;

}

#define ADM_TIMEOUT_APF_CMD 1 // in seconds

ste_adm_res_t modem_wait_end_of_configure_speechproc_comp(void)
{
    struct timeval now;
    struct timespec timeout;
    int retcode;
    if(thread_apf_running)
    {
        pthread_mutex_lock(&mut);
        gettimeofday(&now,NULL);
        timeout.tv_sec = now.tv_sec + ADM_TIMEOUT_APF_CMD ;
        timeout.tv_nsec = now.tv_usec * 1000;
        retcode = 0;
        retcode = pthread_cond_timedwait(&cond, &mut, &timeout);
        if (retcode == ETIMEDOUT) {
            ALOG_ERR(" Timeout on Speech proc scan updated");
        } else {
            ALOG_INFO("Speech proc scan updated");
        }
        pthread_mutex_unlock(&mut);
    }
    else
        ALOG_INFO("No wait needed -> no thread created");
    thread_apf_running=0;

    return STE_ADM_RES_OK;
}

ste_adm_res_t modem_set_volume(int volume)
{
    ALOG_INFO(">%s(%d)",__func__, volume);
    if (volume < 0  || volume>8){
        ALOG_ERR("%s: Volume out of range: %d ",__func__,volume);
        return STE_ADM_RES_INVALID_PARAMETER;
    }
    return modem_at_send_command("AT+CLVL=%d",volume);
}
char * modem_dbus_convert_modem_path_in_char( modem_path_t modempath)
{
    switch(modempath){
        case ADM_MODEM_PATH_CLOSE : return "path_close";
        case ADM_MODEM_PATH_OPEN : return "path_open";
        default : return "path_unknown";
    }
}

ste_adm_res_t modem_set_tx_path(modem_path_t path_tx)
{
    ALOG_INFO(">%s(%s)",__func__, modem_dbus_convert_modem_path_in_char(path_tx));

    if(path_tx!=ADM_MODEM_PATH_OPEN && path_tx!=ADM_MODEM_PATH_CLOSE){
        ALOG_ERR("%s: path_tx out of range: %d ",__func__,path_tx);
        return STE_ADM_RES_INVALID_PARAMETER;
    }
    return modem_at_send_command("AT*EAUR=5,%d,9",path_tx);
}

ste_adm_res_t modem_set_rx_path(modem_path_t path_rx)
{
    ALOG_INFO(">%s(%s)",__func__, modem_dbus_convert_modem_path_in_char(path_rx));

    if(path_rx!=ADM_MODEM_PATH_OPEN && path_rx!=ADM_MODEM_PATH_CLOSE){
        ALOG_ERR("%s: path_tx out of range: %d ",__func__,path_rx);
        return STE_ADM_RES_INVALID_PARAMETER;
    }
    return modem_at_send_command("AT*EAUR=9,%d,5",path_rx);
}

ste_adm_res_t modem_set_rec_rx_path(modem_path_t path_rec)
{
    if(adm_get_modem_type()==ADM_FAT_MODEM) {
        ALOG_INFO(">%s(%s)",__func__, modem_dbus_convert_modem_path_in_char(path_rec));

        if(path_rec!=ADM_MODEM_PATH_OPEN && path_rec!=ADM_MODEM_PATH_CLOSE){
            ALOG_ERR("%s: path_tx out of range: %d ",__func__,path_rec);
            return STE_ADM_RES_INVALID_PARAMETER;
        }
        return modem_at_send_command("AT*EAUR=0,%d,2",path_rec);
    }else
        return STE_ADM_RES_OK;
}

ste_adm_res_t modem_set_rec_tx_path(modem_path_t path_rec)
{
    if(adm_get_modem_type()==ADM_FAT_MODEM) {
        ALOG_INFO(">%s(%s)",__func__, modem_dbus_convert_modem_path_in_char(path_rec));

        if(path_rec!=ADM_MODEM_PATH_OPEN && path_rec!=ADM_MODEM_PATH_CLOSE){
            ALOG_ERR("%s: path_tx out of range: %d ",__func__,path_rec);
            return STE_ADM_RES_INVALID_PARAMETER;
        }
        return modem_at_send_command("AT*EAUR=1,%d,2",path_rec);
    }else
        return STE_ADM_RES_OK;
}

ste_adm_res_t modem_reset_all_paths()
{
    ALOG_INFO(">%s(%s)",__func__, modem_reset_all_paths());

    // Reset all paths by closing all existing paths (I2S AND TDM)
    modem_at_send_command("AT*EAUR=9,0,5");
    modem_at_send_command("AT*EAUR=5,0,9");
    modem_at_send_command("AT*EAUR=9,0,3");
    modem_at_send_command("AT*EAUR=3,0,9");

    return STE_ADM_RES_OK;
}


ste_adm_res_t modem_set_modem_loop(int loopback_type, int loop_enable, int codec_type)
{
    ALOG_INFO(">%s(%d)",__func__, loop_enable);
    (void)codec_type;
    int ret=0;
    ste_adm_res_t res = STE_ADM_RES_OK;

    if(loopback_type == 3) {
        /*modem loop*/
        ret = modem_at_send_command("AT*EAUR=5,%d,5",loop_enable);
        if(ret!=0){
            res = STE_ADM_RES_UNRECOVERABLE_ERROR;
	    ALOG_ERR("Unable to send AT Command AT*EAUR");
	}
    }
    else if( loopback_type == 4) {
        /*modem speech proc loop*/
        ALOG_WARN("Modem Speech proc loop not yet supported");
    }
    else if( loopback_type == 5) {
        /*modem codec loop*/
        ALOG_WARN("Modem Speech proc loop not yet supported");
    }
    else {
        ALOG_ERR("Unsupported loop number");
        res= STE_ADM_RES_UNRECOVERABLE_ERROR;
    }
    return res;
}



int modem_copy_file(char const * const src, char const * const dest)
{
    FILE* fSrc;
    FILE* fDest;
    char buffer[512];
    int NbRead;

    ALOG_INFO_VERBOSE("Copy %s -> %s \n", src,dest );
    if ((fSrc = fopen(src, "rb")) == NULL){
        return -1;
    }

    if ((fDest = fopen(dest, "wb")) == NULL){
        fclose(fSrc);
        return -2;
    }

    while ((NbRead = fread(buffer, 1, 512, fSrc)) != 0)
        fwrite(buffer, 1, NbRead, fDest);

    fclose(fDest);
    fclose(fSrc);

    return 0;
}



int modem_copy_already_done(void)
{
    struct dirent *dir_read;
    struct stat buf;
    char apf_dest[256];
    DIR *rep;

    rep = opendir(ADM_SYSTEM_APF_DIRECTORY);
    if (rep!=NULL){
        while ((dir_read = readdir(rep))) {
            if(strcmp(".",dir_read->d_name)==0) continue;
            if(strcmp("..",dir_read->d_name)==0) continue ;
            memset(apf_dest, 0, 256 * sizeof(char));
            sprintf(apf_dest,"%s\%s",ADM_EXEC_APF_DIRECTORY,dir_read->d_name);
            if(stat(apf_dest,&buf)){
                ALOG_INFO("file doesn't exist %s", apf_dest);
                closedir(rep);
                return 0;
            }
        }
    }
    else{
        ALOG_ERR("Unable to read %s , %s",ADM_SYSTEM_APF_DIRECTORY,strerror(errno));
    }
    if (rep!=NULL){
        closedir(rep);
    }
    return 1;
}
void modem_copy_apf_file(void)
{
    struct dirent *dir_read;
    char apf_src[256];
    char apf_dest[256];
    DIR *rep;
    ALOG_INFO("Copy of apf file from %s to %s directory", ADM_SYSTEM_APF_DIRECTORY,ADM_EXEC_APF_DIRECTORY);

    rep = opendir(ADM_SYSTEM_APF_DIRECTORY);
    if (rep!=NULL){
        while ((dir_read = readdir(rep))) {
            if(strcmp(".",dir_read->d_name)==0) continue;
            if(strcmp("..",dir_read->d_name)==0) continue ;
            memset(apf_src, 0, 256 * sizeof(char));
            memset(apf_dest, 0, 256 * sizeof(char));
            sprintf(apf_src,"%s\%s",ADM_SYSTEM_APF_DIRECTORY,dir_read->d_name);
            sprintf(apf_dest,"%s\%s",ADM_EXEC_APF_DIRECTORY,dir_read->d_name);
            if(modem_copy_file(apf_src,apf_dest))
                ALOG_ERR("Unable to copy %s",apf_src);
        }
    }
    else{
        ALOG_ERR("Unable to read %s , %s",ADM_SYSTEM_APF_DIRECTORY,strerror(errno));
    }
    if (rep!=NULL){
        closedir(rep);
    }
}
#endif

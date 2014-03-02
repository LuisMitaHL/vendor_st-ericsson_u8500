/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/route.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <stdarg.h>
#include <assert.h>
#include <asm/poll.h>
#include <pthread.h>
#include <sys/wait.h>
#include <linux/caif/caif_socket.h>
#include <linux/caif/if_caif.h>
#include "stercd_caif.h"
#include "sterc_netif.h"
#include "sterc_misc.h"
#include "atchannel.h"
#include "at_tok.h"
#include <stdbool.h>
#ifndef EXTERNAL_MODEM_CONTROL_MODULE_DISABLED
#include "dbus/dbus.h"
#include <poll.h>
#endif
#define MAX_IFNAME_LEN 16
#define MAX_AT_RESPONSE (8 * 1024)
#define DEFAULT_AT_TIMEOUT_MSEC (3 * 60 * 1000)
#define NUM_ELEMS(x) (sizeof(x) / sizeof(x[0]))

#define PS_CONNECT 1
#define PS_DISCONNECT 0
#ifndef EXTERNAL_MODEM_CONTROL_MODULE_DISABLED
#define DBUS_CONNECT_DELAY 1        /* Retry timeout in seconds waiting for Dbus and MID */
#define DBUS_CONNECTION_NAME        "com.stericsson.mid"
#define DBUS_OBJECT_PATH            "/com/stericsson/mid"
#define DBUS_OBJECT_INTERFACE       "com.stericsson.mid.Modem"
#define BUF_MID_RESPONSE_SIZE 32
#endif

enum eolresult {
    EOL_SMS = 0,
    EOL_FOUND = 1,
    EOL_NOTFOUND = 2
};

enum config_params {
    lan_netmask_id,
    lan_dhcp_range_first_id,
    lan_dhcp_range_last_id,
    lan_dhcp_lease_id,
    lan_own_ip_id,
    lan_device_id,
    config_params_last_id
};

//rt table parameters
const char lan_netmask[] = "lan_netmask=*:255.255.255.0";
const char lan_dhcp_range_first[] = "lan_dhcp_range_first=*:192.168.42.100";
const char lan_dhcp_range_last[] = "lan_dhcp_range_last=*:192.168.42.200";
const char lan_dhcp_lease[] = "lan_dhcp_lease=*:12h";
const char lan_own_ip[] = "lan_own_ip=*:192.168.42.129";
const char lan_device[] = "lan_device=usb0";

struct atcontext {
    pthread_t atreader;
    int fd;                     /* fd of the AT channel. */
    int readerCmdFds[2];
    int isInitialized;
    ATUnsolHandler unsolHandler;

    /* For input buffering. */
    char ATBuffer[MAX_AT_RESPONSE + 1];
    char *ATBufferCur;

    /*
     * For current pending command, these are protected by commandmutex.
     *
     * The mutex and cond struct is memset in the getAtChannel() function,
     * so no initializer should be needed.
     */

    pthread_mutex_t requestmutex;
    pthread_mutex_t commandmutex;
    pthread_cond_t requestcond;
    pthread_cond_t commandcond;

    ATCommandType type;
    const char *responsePrefix;
    ATResponse *response;

    void (*onTimeout)(void);
    void (*onReaderClosed)(void);
    int readerClosed;

    int timeoutMsec;
};

static bool g_sterc_queueRunnertRelease = true;
pthread_cond_t wait_for_emrdy_cond;
pthread_mutex_t wait_for_emrdy_mutex;

#ifndef EXTERNAL_MODEM_CONTROL_MODULE_DISABLED
pthread_mutex_t sterc_queue_runner_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t s_sterc_tid_dbusRunner;
#define STERC_DBUS_MAX_WATCHERS 2         /* 1 for reading, 1 for writing. */

static DBusWatch *sterc_used_watches[STERC_DBUS_MAX_WATCHERS];
static DBusWatch *sterc_unused_watches[STERC_DBUS_MAX_WATCHERS];

static int sterc_dbus_used_fds = 0;
static int sterc_dbus_not_used_fds = 0;

static struct pollfd sterc_dbus_used_pollfds_tab[STERC_DBUS_MAX_WATCHERS];
static struct pollfd sterc_dbus_not_used_pollfds_tab[STERC_DBUS_MAX_WATCHERS];

static pthread_mutex_t s_sterc_dbus_watch_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static NAPRequestQueue napRequestQueue = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, NULL};
static NAPRequest nap_request;

static int nap_cid;
static int nap_connect_action;
/**
 * Returns 1 if line is a final response indicating success.
 * See 27.007 annex B.
 * WARNING: NO CARRIER and others are sometimes unsolicited.
 */
static const char *s_finalResponsesSuccess[] = {
    "OK",
};
static const char *s_finalResponsesError[] = {
    "ERROR",
    "+CMS ERROR:",
    "+CME ERROR:"
};

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/
static int daemon_mode = 1;
int debug_mode = 0;
//for config file and all parameters read from that
char *config_path = "sterc.conf";
char *sterc_runscript_path_connect = NULL;
char *sterc_runscript_path_disconnect = NULL;

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/
static void print_usage(char *prog);
static sterc_transport_type_t get_transport_type(char *str_p);
static void handle_terminate(int signo);
static void *create_env_dnsmasq_config_file(void);
static bool createNetworkInterface(const char *ifname, int connection_id);
static int configureNetworkInterfaceIPv4(char *ifname, char *ipAddrStr,
        char *subnetMaskStr,
        char **defaultGatewayStr, int *prefix);
static void *get_param_string(int id);
static void *readerLoop(void *arg);
static int sterc_subscribe_enap();
static void onUnsolicited(const char *s, const char *data);
void *queueRunner(void *param);
static int activatePDPContextWithEPPSD(int currCid, int caifConnId);
static int deactivatePDPContextWithEPPSD(int currCid, int caifConnId);

struct sockaddr_caif at_caif = {
    .family = AF_CAIF,
    .u.at.type = CAIF_ATTYPE_PLAIN,
};

/**
 * main()
 **/
int main(int argc, char *argv[])
{

    int result;
    int ret, err;
    int nfds;
    fd_set rfds;
    int caif_fd;
    char **env = NULL;
    int env_idx = 0;
    pthread_attr_t attr;
    pthread_t tid_queuerunner;

    struct sockaddr_caif at_caif;
    struct sockaddr_un ctrl_addr_un;
    struct sockaddr_un event_addr_un;
    struct sockaddr_in ctrl_addr_in;
    struct sockaddr_in event_addr_in;
    pid_t pid;
    pid_t sid;
    sterc_transport_type_t transport_type = tt_unknown;
    int pscc_reconnect_tries = 5;

    /* optarg stuff */
    int c;
    extern char *optarg;
    extern int optind, optopt, opterr;
    int option_index = 0;
    static struct option long_options[] = {
        {"pt", 1, 0, 0},
        {"pc", 1, 0, 0},
        {"pe", 1, 0, 0},
        {0, 0, 0, 0}
    };
    /* optarg end */


    while ((c = getopt(argc, argv, "fd:t:r:c:s:")) != -1) {
        switch (c) {
        case 'f':
            daemon_mode = 0;
            break;
        case 'd':
            debug_mode = strtol(optarg, NULL, 10);
            break;
        case 'c':
            config_path = optarg;
            break;
        case 'r':
            sterc_runscript_path_connect = optarg;
            break;
        case 's':
            sterc_runscript_path_disconnect = optarg;
            break;
        case 't':
            transport_type = get_transport_type(optarg);
            break;
        case ':':
        case '?':
            print_usage(argv[0]);
            exit(1);
        }
    }

    if (daemon_mode) {
        fprintf(stderr, "\nenetring daemon mode\n");
        /* Daemonize */
        int fd;
        /* Become child of init */
        pid = fork();

        if (pid < 0) {
            exit(1);
        }

        if (pid > 0) {
            exit(0);
        }

        /* Override the umask */
        umask(0);

        /* Start a new session */
        sid = setsid();

        if (sid < 0) {
            exit(1);
        }

        /* Change working directory to something that surely exists */
        if (chdir("/") < 0) {
            exit(1);
        }

        /* re-direct standard file descriptors to /dev/null */
        fd = open("/dev/null", O_RDWR);

        if (fd < 0) {

            fprintf(stderr, "\nclosing iosP\n");
            close(0);
            close(1);
            close(2);
        } else {
            fprintf(stderr, "\nclosing iosP\n");

            if (dup2(fd, STDIN_FILENO) < 0) {
                close(0);
            }

            if (dup2(fd, STDOUT_FILENO) < 0) {
                close(1);
            }

            if (dup2(fd, STDERR_FILENO) < 0) {
                close(2);
            }

            close(fd);
        }

        /* We are now daemonized */
    }

#ifndef STERC_SW_VARIANT_ANDROID
    /* Open any logs here */
    fprintf("\n#ifndef STERC_SW_VARIANT_ANDROID\n");
    openlog(argv[0], LOG_CONS | LOG_PID, LOG_DAEMON);
#endif //STERC_SW_VARIANT_ANDROID

    /* Handle some signals */
    signal(SIGTERM, handle_terminate);

    if (!daemon_mode) {
        signal(SIGINT, handle_terminate);
    }

    if (sterc_runscript_path_connect == NULL) {
        STERC_DBG_TRACE(LOG_WARNING, "sterc_runscript_path_connect is NULL!\n");
    }

    if (sterc_runscript_path_disconnect == NULL) {
        STERC_DBG_TRACE(LOG_WARNING, "sterc_runscript_path_disconnect is NULL!\n");
    }

    if (transport_type == tt_caif) {
        if ((err = pthread_attr_init(&attr)) != 0)
            STERC_DBG_TRACE(LOG_INFO, "%s() failed to initialize pthread attribute: %s",
                            __func__, strerror(err));

        if ((err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)) != 0)
            STERC_DBG_TRACE(LOG_INFO, "%s() failed to set the "
                            "PTHREAD_CREATE_DETACHED attribute: %s", __func__, strerror(err));

        if ((err = pthread_create(&tid_queuerunner, &attr, queueRunner, NULL)) != 0) {
            STERC_DBG_TRACE(LOG_INFO, "%s() failed to create queue runner thread: %s",
                            __func__, strerror(err));

            assert(0);
        }

        ret = pthread_join(tid_queuerunner, NULL);
        if (ret != 0) {
            STERC_DBG_TRACE(LOG_INFO,"%s() failed joining with queueRunner to main. Errno: %s"
            "Asserting", __func__, strerror(errno));
            assert(0);
        }
    } else {
        err = -1;
    }

    return(err);
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/
/**
 * handle_terminate()
 **/
static void handle_terminate(int signo)
{
    STERC_DBG_TRACE(LOG_INFO, "Terminated by signal %d.\n", signo);
    exit(0);
}

/**
 * print_usage()
 **/
static void print_usage(char *prog)
{
#ifdef STERC_SW_VARIANT_ANDROID
    STERC_DBG_TRACE(LOG_INFO,
#else
    fprintf(stderr,
#endif
                    "Usage: %s [-f] [-d level] [-t unix/ip] [-c <config file>]  [-r <run-script>] [--pt uinx/ip] [--pc ctrl] [--pe event]\n"
                    "       -f   : foreground mode (no daemon)\n"
                    "       -d   : debug\n"
                    "       -t   : transport type (CAIF)\n"
                    "       -c   : specify the config file\n"
                    "       -r   : specify the connect run-script file\n"
                    "       -s   : specify the disconnect run-script file\n", prog);
}

static sterc_transport_type_t get_transport_type(char *str_p)
{
    assert(str_p != NULL);

    if (!strncmp(str_p, "CAIF", 4)) {
        return (tt_caif);
    }

    return (tt_unknown);
}

/**
 * Very simple function that extract and returns whats between ElementBeginTag
 * and ElementEndTag.
 *
 * Optional ppszRemainingDocument that can return a pointer to the remaining
 * of the document to be "scanned". This can be used if subsequent
 * scanning/searching is desired.
 *
 * This function is used to extract the parameters from the XML result
 * returned by U3xx during a PDP Context setup, and used to parse the
 * tuples of operators returned from AT+COPS.
 *
 *  const char* document        - Document to be scanned
 *  const char* elementBeginTag - Begin tag to scan for, return whats
 *                                between begin/end tags
 *  const char* elementEndTag   - End tag to scan for, return whats
 *                                between begin/end tags
 *  char** remainingDocumen t   - Can return the a pointer to the remaining
 *                                of pszDocument, this parameter is optional
 *
 *  return char* containing whats between begin/end tags, allocated on the
 *               heap, need to free this.
 *               return NULL if nothing is found.
 */
char *getFirstElementValue(const char *document,
                           const char *elementBeginTag,
                           const char *elementEndTag,
                           char **remainingDocument)
{
    char *value = NULL;
    char *start = NULL;
    char *end = NULL;

    if (document != NULL && elementBeginTag != NULL
            && elementEndTag != NULL) {
        start = strstr(document, elementBeginTag);

        if (start != NULL) {
            end = strstr(start, elementEndTag);

            if (end != NULL) {
                int n = strlen(elementBeginTag);
                int m = end - (start + n);
                value = (char *) malloc((m + 1) * sizeof(char));
                strncpy(value, (start + n), m);
                value[m] = (char) 0;

                /* Optional, return a pointer to the remaining document,
                   to be used when document contains many tags with same name. */
                if (remainingDocument != NULL) {
                    *remainingDocument = end + strlen(elementEndTag);
                }
            }
        }
    }

    return value;
}

static int nap_connect(int currCid, int caifConnId)
{
    int ret;

    ret  = activatePDPContextWithEPPSD(currCid, caifConnId);

    if (ret < 0) {
        STERC_DBG_TRACE(LOG_ERR, "\nactivatePDPContextWithEPPSD FAILED\n");
        return ret;
    }

    return ret;
}

static int nap_disconnect(int currCid, int caifConnId)
{
    int ret;

    STERC_DBG_TRACE(LOG_INFO, "\nnap_connect\n");

    ret  = deactivatePDPContextWithEPPSD(currCid, caifConnId);

    if (ret < 0) {
        STERC_DBG_TRACE(LOG_ERR, "\ndeactivatePDPContextWithEPPSD FAILED\n");
        return ret;
    }

    return ret;
}

static int parseEnapEvent(const char *s, struct NAPRequest *nap_request)
{
    char *cp = NULL, *token;
    int connect, cid, orig_channel;
    int err;

    if (nap_request == NULL) {
        goto error;
    }

    cp = strdup(s);

    /*Get forwarded channel*/
    token = strchr(cp, ':');

    if (token == NULL) {
        goto error;
    }

    token++;
    token++;
    orig_channel = atoi(token);

    /*Start parsing*/
    token = strchr(cp, '=');

    if (token == NULL) {
        goto error;
    }

    token++;

    connect = atoi(token);
    token++;
    token++;

    if (token == NULL) {
        goto error;
    }

    cid = atoi(token);

    STERC_DBG_TRACE(LOG_INFO, "ENAP :CID : %d: Action: %d: %s", connect, cid, s);

    nap_request->request = connect;
    nap_request->cid = cid;
    nap_request->orig_channel = orig_channel;

    free(cp);
    return 0;

error:
    free(cp);
    STERC_DBG_TRACE(LOG_INFO, "\nSome error in parsing\n");
    return -1;

}

static int sterc_subscribe_enap()
{
    int ret = 0, err = 0;
    ATResponse *atresponse = NULL;
    const char *cmd = "AT*EFWD=1,\"*ENAP\",255";

    ret = at_send_command(cmd, &atresponse);

    if (ret < 0 || atresponse->success == 0) {
        STERC_DBG_TRACE(LOG_INFO, "Could not succed to subscribe ENAP");
        err = -1;
    }

    return err;
}

static void *create_env_dnsmasq_config_file(void)
{
    size_t elem_size;
    char *res = NULL;
    char *lan_device;

    elem_size = snprintf(NULL, 0,
                         "dnsmasq_config_file=/data/misc/psdata/dnsmasq.conf");
    elem_size++;

    res = malloc(elem_size);

    if (NULL == res) {
        STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
        return NULL;
    }

    elem_size = snprintf(res, elem_size, "dnsmasq_config_file=/data/misc/psdata/dnsmasq.conf");
    res[elem_size] = '\0';

    return res;
}


static void *get_param_string(int id)
{
    size_t elem_size;
    char *res = NULL;
    const char *param = NULL;

    switch (id) {
    case lan_netmask_id:
        param = lan_netmask;
        break;
    case lan_own_ip_id:
        param = lan_own_ip;
        break;
    case lan_device_id:
        param = lan_device;
        break;
    case lan_dhcp_range_first_id:
        param = lan_dhcp_range_first;
        break;
    case lan_dhcp_range_last_id:
        param = lan_dhcp_range_last;
        break;
    case lan_dhcp_lease_id:
        param = lan_dhcp_lease;
        break;
    default:
        break;
    };

    if (param != NULL) {
        elem_size = snprintf(NULL, 0, "%s", param);
        elem_size++;

        res = malloc(elem_size);

        if (NULL == res)  {
            STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
            return NULL;
        }

        elem_size = snprintf(res, elem_size, "%s", param);
        res[elem_size] = '\0';

        param = res;
    }

    return (void *)param;
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/

static void onUnsolicited(const char *s, const char *data)
{
    int cid, err;
    struct NAPRequestQueue *q = &napRequestQueue;

    STERC_DBG_TRACE(LOG_INFO, "onUnsolicited: %s", s);

    if (strStartsWith(s, "*EMRDY:")) {
        pthread_mutex_lock(&wait_for_emrdy_mutex);
        pthread_cond_signal(&wait_for_emrdy_cond);
        pthread_mutex_unlock(&wait_for_emrdy_mutex);
    }

    if (strStartsWith(s, "*EFWD:")) {
        err = parseEnapEvent(s, &nap_request);

        if (err < 0) {
            return;
        }

        pthread_mutex_lock(&q->lock);
        pthread_cond_signal(&q->cond);
        pthread_mutex_unlock(&q->lock);

    }
}

#ifndef EXTERNAL_MODEM_CONTROL_MODULE_DISABLED


static void releaseModemConnect(void)
{
    STERC_DBG_TRACE(LOG_INFO, "Releasing queuerunner.");
    pthread_mutex_lock(&sterc_queue_runner_mutex);
    g_sterc_queueRunnertRelease = true;
    pthread_mutex_unlock(&sterc_queue_runner_mutex);
}

static void releasingMain(void)
{
    struct NAPRequestQueue *q = &napRequestQueue;
    STERC_DBG_TRACE(LOG_INFO, "Off recieved on DBUS.Will wait for ON");
    pthread_mutex_lock(&sterc_queue_runner_mutex);
    g_sterc_queueRunnertRelease = false;
    pthread_mutex_unlock(&sterc_queue_runner_mutex);
    pthread_mutex_lock(&q->lock);
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
}

static dbus_bool_t addWatch(DBusWatch *watch, void *data)
{
    STERC_IDENTIFIER_NOT_USED(data);
    short cond = POLLHUP | POLLERR;
    int fd;
    dbus_bool_t ret = TRUE;
    unsigned int flags;
    dbus_bool_t res;
    int mutexRes;

#ifdef ANDROID
    fd = dbus_watch_get_fd(watch);
#else
    fd = dbus_watch_get_unix_fd(watch);
#endif

    flags = dbus_watch_get_flags(watch);

    if (flags & DBUS_WATCH_READABLE) {
        cond |= POLLIN;
        STERC_DBG_TRACE(LOG_INFO, "STERC get new dbus watch for READABLE condition");
    }

    if (flags & DBUS_WATCH_WRITABLE) {
        cond |= POLLOUT;
        STERC_DBG_TRACE(LOG_INFO, "STERC get new dbus watch for WRITABLE condition");
    }

    mutexRes = pthread_mutex_lock(&s_sterc_dbus_watch_mutex);

    if (mutexRes != 0)
        STERC_DBG_TRACE(LOG_INFO, "Unable to take dbus watch mutex");

    res = dbus_watch_get_enabled(watch);

    if (res) {
        if (sterc_dbus_used_fds < STERC_DBUS_MAX_WATCHERS) {
            STERC_DBG_TRACE(LOG_INFO, "new dbus watch id: %d is marked USED", sterc_dbus_used_fds);
            sterc_used_watches[sterc_dbus_used_fds] = watch;
            sterc_dbus_used_pollfds_tab[sterc_dbus_used_fds].fd = fd;
            sterc_dbus_used_pollfds_tab[sterc_dbus_used_fds].events = cond;
            sterc_dbus_used_fds++;
        } else {
            STERC_DBG_TRACE(LOG_INFO, "new dbus watch id: %d is marked USED. BUT can not be "
                            "added", sterc_dbus_used_fds);
            goto error;
        }
    } else {
        if (sterc_dbus_not_used_fds < STERC_DBUS_MAX_WATCHERS) {
            STERC_DBG_TRACE(LOG_INFO, "new dbus watch id: %d is marked UNUSED", sterc_dbus_not_used_fds);
            sterc_unused_watches[sterc_dbus_not_used_fds] = watch;
            sterc_dbus_not_used_pollfds_tab[sterc_dbus_not_used_fds].fd = fd;
            sterc_dbus_not_used_pollfds_tab[sterc_dbus_not_used_fds].events = cond;
            sterc_dbus_not_used_fds++;
        } else {
            STERC_DBG_TRACE(LOG_INFO, "new dbus watch id: %d is marked UNUSED. BUT can not be "
                            "added", sterc_dbus_not_used_fds);
            goto error;
        }
    }

    goto exit;
error:
    ret = FALSE;
exit:
    mutexRes = pthread_mutex_unlock(&s_sterc_dbus_watch_mutex);

    if (mutexRes != 0)
        STERC_DBG_TRACE(LOG_INFO, "Unable to release dbus watch mutex.");

    return ret;
}

static void removeWatch(DBusWatch *watch, void *data)
{
    STERC_IDENTIFIER_NOT_USED(data);
    int i, index;
    bool found = false;
    int mutexRes;

    STERC_DBG_TRACE(LOG_INFO, "Removing dbus watch");

    mutexRes = pthread_mutex_lock(&s_sterc_dbus_watch_mutex);

    if (mutexRes != 0)
        STERC_DBG_TRACE(LOG_INFO, "Unable to take dbus watch mutex");

    for (i = 0; i < sterc_dbus_not_used_fds; i++) {
        if (sterc_unused_watches[i] == watch) {
            found = true;
            index = i;
            break;
        }
    }

    if (!found) {
        STERC_DBG_TRACE(LOG_INFO, "watch %p not found in unused pool, try used pool...", (void *)watch);

        for (i = 0; i < sterc_dbus_used_fds; i++) {
            if (sterc_used_watches[i] == watch) {
                found = true;
                index = i;
                break;
            }
        }

        if (!found) {
            STERC_DBG_TRACE(LOG_INFO, "watch %p not found in any pool...", (void *)watch);
            goto exit;
        } else {
            STERC_DBG_TRACE(LOG_INFO, "STERC watch %p found in used pool. Removed", (void *)watch);

            for (i = index; i < (sterc_dbus_used_fds - 1); i++) {
                sterc_used_watches[i] = sterc_used_watches[i + 1];
                memcpy(&sterc_dbus_used_pollfds_tab[i], &sterc_dbus_used_pollfds_tab[i + 1],
                       sizeof(sterc_dbus_used_pollfds_tab[i + 1]));
            }

            sterc_used_watches[i] = NULL;
            memset(&sterc_dbus_used_pollfds_tab[i], 0,
                   sizeof(sterc_dbus_used_pollfds_tab[i]));
            sterc_dbus_used_fds--;
        }
    } else {
        STERC_DBG_TRACE(LOG_INFO, "STERC watch %p found in unused pool. Removed", (void *)watch);

        for (i = index; i < (sterc_dbus_not_used_fds - 1); i++) {
            sterc_unused_watches[i] = sterc_unused_watches[i + 1];
            memcpy(&sterc_dbus_not_used_pollfds_tab[i],
                   &sterc_dbus_not_used_pollfds_tab[i + 1],
                   sizeof(sterc_dbus_not_used_pollfds_tab[i + 1]));
        }

        sterc_unused_watches[i] = NULL;
        memset(&sterc_dbus_not_used_pollfds_tab[i], 0,
               sizeof(sterc_dbus_not_used_pollfds_tab[i]));
        sterc_dbus_not_used_fds--;
    }

exit:
    mutexRes = pthread_mutex_unlock(&s_sterc_dbus_watch_mutex);

    if (mutexRes != 0)
        STERC_DBG_TRACE(LOG_INFO, "Unable to release dbus watch mutex.");

}
static void notifyForEvent(int index, short event)
{
    unsigned int flags = 0;

    if (event & POLLIN)
        flags |= DBUS_WATCH_READABLE;

    if (event & POLLOUT)
        flags |= DBUS_WATCH_WRITABLE;

    if (event & POLLHUP)
        flags |= DBUS_WATCH_HANGUP;

    if (event & POLLERR)
        flags |= DBUS_WATCH_ERROR;

    while (!dbus_watch_handle(sterc_used_watches[index], flags)) {
        STERC_DBG_TRACE(LOG_INFO, "dbus_watch_handle needs more memory. Spinning");
        sleep(1);
    }

    STERC_DBG_TRACE(LOG_INFO, "used id: %d dbus_watch_handle selected for DBUS operation"
                    , index);
}
static void processDbusEvent(DBusConnection *dbcon)
{
    int res = 0;

    for (;;) {
        res = dbus_connection_dispatch(dbcon);

        switch (res) {
        case DBUS_DISPATCH_COMPLETE:
            return;
        case DBUS_DISPATCH_NEED_MEMORY:
            STERC_DBG_TRACE(LOG_INFO, "dbus_connection_dispatch needs more memory."
                            "spinning");
            sleep(1);
            break;
        case DBUS_DISPATCH_DATA_REMAINS:
            STERC_DBG_TRACE(LOG_INFO, "dispatch: remaining data for DBUS operation."
                            "Spinning");
            break;
        default:
            /* This should not happen */
            break;
        }
    }
}

static void *dbusAndThreadRunner(void *param)
{
    DBusConnection *dbcon = (DBusConnection *)param;
    int ret;

    int i;

    for (;;) {
        ret = poll(sterc_dbus_used_pollfds_tab, STERC_DBUS_MAX_WATCHERS, -1);

        if (ret > 0) {
            for (i = 0; i < STERC_DBUS_MAX_WATCHERS; i++) {
                if (sterc_dbus_used_pollfds_tab[i].revents) {
                    notifyForEvent(i, sterc_dbus_used_pollfds_tab[i].revents);
                    processDbusEvent(dbcon);
                }
            }
        }
    }

    return 0;

}
static void flushDbusEvents(DBusConnection *dbcon)
{
    DBusMessageIter args;
    DBusMessage *dbmsg;
    char *signame = NULL;
    int res = 0;

    for (;;) {
        res = dbus_connection_get_dispatch_status(dbcon);

        switch (res) {
        case DBUS_DISPATCH_COMPLETE:
            return;
        case DBUS_DISPATCH_NEED_MEMORY:
            STERC_DBG_TRACE(LOG_INFO, "needs more memory. spinning.");
            sleep(1);
            break;
        case DBUS_DISPATCH_DATA_REMAINS:
            dbmsg = dbus_connection_pop_message(dbcon);

            if (dbus_message_is_signal(dbmsg, "com.stericsson.mid.Modem",
                                       "StateChange")) {
                if (!dbus_message_iter_init(dbmsg, &args)) {
                    STERC_DBG_TRACE(LOG_INFO, "Message has no arguments!");
                } else if (DBUS_TYPE_STRING !=
                           dbus_message_iter_get_arg_type(&args)) {
                    STERC_DBG_TRACE(LOG_INFO, "Argument is not string!");
                } else {
                    dbus_message_iter_get_basic(&args, &signame);
                    STERC_DBG_TRACE(LOG_INFO, "Flushing Signal with value \"%s\"", signame);
                }
            }

            break;
        default:
            /* This should not happen */
            break;
        }
    }
}

/* MID signal message handler */
static DBusHandlerResult midSignalHandler(DBusConnection *dbcon, DBusMessage
        *msg, void *data)
{
    DBusMessageIter args;
    STERC_IDENTIFIER_NOT_USED(data);
    char *signame = NULL;

    if (dbcon == NULL) {
        STERC_DBG_TRACE(LOG_INFO, "dbcon NULL recieved \n");
    }

    if (dbus_message_is_signal(msg, "com.stericsson.mid.Modem",
                               "StateChange")) {
        if (!dbus_message_iter_init(msg, &args)) {
            STERC_DBG_TRACE(LOG_INFO, "Message has no arguments!");
        } else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
            STERC_DBG_TRACE(LOG_INFO, "Argument is not string!");
        } else {
            dbus_message_iter_get_basic(&args, &signame);
            STERC_DBG_TRACE(LOG_INFO, " Got Signal with value \"%s\"", signame);

            if (strncmp(signame, "on", 2) == 0) {
                if (g_sterc_queueRunnertRelease) {
                    STERC_DBG_TRACE(LOG_INFO, "Already running state. Signal ignored...");
                } else {
                    releaseModemConnect();
                }
            } else if (strncmp(signame, "off", 3) == 0) {

                releasingMain();
            }

            else
                STERC_DBG_TRACE(LOG_INFO, "  message \"%s\" ignored.",  signame);

            return DBUS_HANDLER_RESULT_HANDLED;
        }
    }

    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

static int requestMIDWithResponse(DBusConnection *dbcon,
                                  char *requestMethod,
                                  char *response)
{
    char *return_val = NULL;
    int ret = 0;

    DBusPendingCall *pending = NULL;
    DBusMessage *dbmsg = NULL;
    DBusMessageIter args;

    dbmsg = dbus_message_new_method_call(DBUS_CONNECTION_NAME,
                                         DBUS_OBJECT_PATH,
                                         DBUS_OBJECT_INTERFACE,
                                         requestMethod);

    if (dbmsg == NULL) {
        STERC_DBG_TRACE(LOG_INFO, "%s(): Failed to create a method call", __func__);
        goto error;
    }

    if (!dbus_connection_send_with_reply(dbcon, dbmsg, &pending, -1)) {
        STERC_DBG_TRACE(LOG_INFO, "%s(): Failed to send method call", __func__);
        goto error;
    }

    if (pending == NULL) {
        STERC_DBG_TRACE(LOG_INFO, "%s(): Failed to send method call, connection closed", __func__);
        goto error;
    }

    dbus_connection_flush(dbcon);
    dbus_message_unref(dbmsg);
    dbus_pending_call_block(pending);

    dbmsg = dbus_pending_call_steal_reply(pending);
    dbus_pending_call_unref(pending);

    if (dbmsg == NULL) {
        STERC_DBG_TRACE(LOG_INFO, "%s(): Error on the received message.", __func__);
        goto error;
    }

    if (!dbus_message_iter_init(dbmsg, &args)) {
        STERC_DBG_TRACE(LOG_INFO, " %s(): Received message has no arguments!", __func__);
        goto error;
    } else if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&args)) {
        STERC_DBG_TRACE(LOG_INFO, "%s(): Argument is not a string!", __func__);
        goto error;
    } else {
        dbus_message_iter_get_basic(&args, &return_val);
        strncpy(response, return_val, BUF_MID_RESPONSE_SIZE - 1);
        STERC_DBG_TRACE(LOG_INFO, "%s(): Got message: \"%s\", response: \"%s\"",
                        __func__, return_val, response);
    }

    goto exit;
error:
    ret = -1;
exit:
    dbus_message_unref(dbmsg);
    return ret;
}

static bool queryModemOn(DBusConnection *dbcon)
{
    bool res = false;
    char responseArray[BUF_MID_RESPONSE_SIZE];
    char *pResponse = responseArray;

    memset(pResponse, 0x0, BUF_MID_RESPONSE_SIZE);

    if (requestMIDWithResponse(dbcon, "GetState", pResponse) != 0) {
        STERC_DBG_TRACE(LOG_INFO, "%s(): Failed to query state of MID.", __func__);
    } else {
        if (strncmp(pResponse, "on", 2) == 0) {
            if (g_sterc_queueRunnertRelease) {
                STERC_DBG_TRACE(LOG_INFO, "Already running state. Signal ignored...");
            } else {
                releaseModemConnect();
            }

            res = true;
        } else
            STERC_DBG_TRACE(LOG_INFO, "%s(): %s returned and ignored.", __func__, pResponse);
    }

    return res;
}

#endif

void *queueRunner(void *param)
{
    int fd;
    int ret = -1, err;
    char *cmd;
    ATResponse *atresponse = NULL;
    struct NAPRequestQueue *q = &napRequestQueue;
#ifndef EXTERNAL_MODEM_CONTROL_MODULE_DISABLED
    DBusConnection *query_db_con = NULL;
    uint8_t dbus_is_here = 0;
    DBusError dbus_err;
    pthread_attr_t attr;
    g_sterc_queueRunnertRelease = false;
#endif

    pthread_cond_init(&wait_for_emrdy_cond, NULL);
    pthread_mutex_init(&wait_for_emrdy_mutex, NULL);

#ifndef EXTERNAL_MODEM_CONTROL_MODULE_DISABLED
    dbus_is_here = 0;

    /*****************************************************
      * Dbus-connection for listening:
      * Loop until we have connection to Dbus daemon      *
      *****************************************************/
    for (;;) {
        /* Connect to system dbus */
        do {
            dbus_error_init(&dbus_err);
            query_db_con = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_err);

            if (!query_db_con || dbus_error_is_set(&dbus_err)) {
                STERC_DBG_TRACE(LOG_INFO, "[DBUS]: connect to DBUS(listenDbcon) daemon returned error (%s)."
                                "Will try again in %d second(s)",
                                dbus_err.name, DBUS_CONNECT_DELAY);
                dbus_error_free(&dbus_err);
                sleep(DBUS_CONNECT_DELAY);
            } else
                dbus_is_here = true;
        } while (!dbus_is_here);

        STERC_DBG_TRACE(LOG_INFO, "[DBUS]: listenDbcon Connected to system dbus.");
        ret = pthread_attr_init(&attr);

        if (ret != 0)
            STERC_DBG_TRACE(LOG_INFO, "Failed to initialize dbus pthread attribute: %s",
                            strerror(ret));

        ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (ret != 0)
            STERC_DBG_TRACE(LOG_INFO, "Failed to set the dbus PTHREAD_CREATE_DETACHED "
                            "attribute: %s", strerror(ret));

        /*************************************************
         * Dbus-connection for listening:
         * Configure Dbus Connection.                    *
         *************************************************/
        if (!dbus_connection_set_watch_functions(query_db_con, addWatch,
                removeWatch, NULL, NULL, NULL)) {
            STERC_DBG_TRACE(LOG_INFO, "dbus_connection_set_watch_functions failed.");
        }

        STERC_DBG_TRACE(LOG_INFO, " Add Watches done\n");

        dbus_error_init(&dbus_err);
        /*
         * Adds a match rule to match messages going through the message bus
         * Listen only signal from com.stericsson.mid.Modem interface (MID
         * state changes)
         */
        dbus_bus_add_match(query_db_con,
                           "type='signal', interface='com.stericsson.mid.Modem'", &dbus_err);

        if (dbus_error_is_set(&dbus_err)) {
            STERC_DBG_TRACE(LOG_INFO, "  DBUS match error %s: %s.",
                            dbus_err.name, dbus_err.message);
            dbus_error_free(&dbus_err);
        }

        /* Add a message filter to process incoming messages */
        if (!dbus_connection_add_filter(query_db_con,
                                        (DBusHandleMessageFunction)midSignalHandler, NULL, NULL)) {
            STERC_DBG_TRACE(LOG_INFO, " DBUS filter error.");
        }

        /*
        * Need to be sure CAIF interfaces are up. Wait for
        * "on" message.
        */

        while (!queryModemOn(query_db_con))
            sleep(DBUS_CONNECT_DELAY);

        /**
              * Need to empty the listen-buffer initially.
              * This must be done before we release command threads.
              */

        flushDbusEvents(query_db_con);

        ret = pthread_create(&s_sterc_tid_dbusRunner, &attr,
                             dbusAndThreadRunner, (void *)query_db_con);

        if (ret != 0) {
            STERC_DBG_TRACE(LOG_INFO, " Failed to create dbus runner thread: %s. Asserting."
                            ,  strerror(ret));
            assert(0);
        }

        if (ret != 0)
            STERC_DBG_TRACE(LOG_INFO, "Failed to unlock mutex. err: %s",
                            strerror(-ret));

        STERC_DBG_TRACE(LOG_INFO, "On recieved on DBUS. Creating socket");


#endif

        fd = -1;

        while (fd < 0) {
            int cf_prio = CAIF_PRIO_HIGH;

            struct sockaddr_caif addr = {
                .family = AF_CAIF,
                .u.at.type = CAIF_ATTYPE_PLAIN
            };

            fd = socket(AF_CAIF, SOCK_SEQPACKET, CAIFPROTO_AT);

            if (fd < 0) {
                STERC_DBG_TRACE(LOG_ERR, "%s(): failed to create socket. errno: %d(%s).",
                                __func__, errno, strerror(-errno));
                return NULL;
            }

            if (setsockopt(fd, SOL_SOCKET, SO_PRIORITY, &cf_prio,
                           sizeof(cf_prio)) != 0)
                STERC_DBG_TRACE(LOG_ERR, "%s(): Not able to set socket priority. Errno:%d(%s).",
                                __func__, errno, strerror(-errno));

            ret = connect(fd, (struct sockaddr *) &addr, sizeof(addr));

            if (ret != 0) {
                STERC_DBG_TRACE(LOG_ERR, "%s(): Failed to connect. errno: %d(%s).", __func__,
                                errno, strerror(-errno));
                STERC_DBG_TRACE(LOG_ERR, "%s(): Retry after 10 sec");
                close(fd);
                fd = -1;
                sleep(10);
            }
        }

        ret = at_open(fd, onUnsolicited);

        if (ret < 0) {
            STERC_DBG_TRACE(LOG_INFO, "%s(): AT error %d on at_open!", __func__, ret);
        }

        STERC_DBG_TRACE(LOG_INFO, "%s(): AT waiting for EMRDY!", __func__, ret);

        pthread_mutex_lock(&wait_for_emrdy_mutex);
        err = pthread_cond_wait(&wait_for_emrdy_cond, &wait_for_emrdy_mutex);
        pthread_mutex_unlock(&wait_for_emrdy_mutex);

        sterc_subscribe_enap();

        while (g_sterc_queueRunnertRelease) {
            NAPRequest *r = NULL;

            pthread_mutex_lock(&q->lock);
            pthread_cond_wait(&q->cond, &q->lock);
            pthread_mutex_unlock(&q->lock);

            if (g_sterc_queueRunnertRelease) {

                r = &nap_request;

                if (r != NULL) {

                    switch (r->request) {
                    case 1:
                        ret = nap_connect(r->cid, r->cid);
                        break;
                    case 0: //disconnect
                        ret = nap_disconnect(r->cid, r->cid);
                        break;
                    default:
                        break;
                    };

                    /*send response*/
                    asprintf(&cmd, "AT*EFWDR=%d,%d", r->orig_channel, (ret < 0) ? 1 : 0);

                    at_send_command(cmd, &atresponse);

                    at_response_free(atresponse);
                }
            }
        }

        at_close();
        STERC_DBG_TRACE(LOG_INFO, "Re-opening after close");
    }

    return NULL;
}

static char *getFakedDefaultGatewayStr(in_addr_t addr)
{
    in_addr_t gw;
    struct in_addr gwaddr;

    gw = ntohl(addr) & 0xFFFFFF00;
    gw |= (ntohl(addr) & 0x000000FF) ^ 1;

    gwaddr.s_addr = htonl(gw);

    return inet_ntoa(gwaddr);
}

static int deactivatePDPContextWithEPPSD(int currCid, int caifConnId)
{
    ATResponse *atresponse = NULL;
    int err;
    char *cmd;
    char *argv[2] = {NULL, NULL};
    char *env[2] = {NULL, NULL};
    int pid;

    asprintf(&cmd, "AT*EPPSD=0,%d,%d", caifConnId, currCid);
    err = at_send_command(cmd, &atresponse);
    free(cmd);

    if (err < 0) {
        STERC_DBG_TRACE(LOG_ERR, "%s() failed to send deactivating ccommand to cid %d!", __func__, currCid);
    }

    if (atresponse->success == 0) {
        STERC_DBG_TRACE(LOG_ERR, "%s() failed deactivating cid %d!", __func__, currCid);
        err = -1;
    }

    at_response_free(atresponse);

    env[0] = get_param_string(lan_device_id);
    argv[0] = sterc_runscript_path_disconnect;
    env[1] = NULL;

    pid = exec_script(argv, env);
    STERC_DBG_TRACE(LOG_DEBUG, "Pid was %d\n", pid);

    if (pid == -1) {
        err = -1;
    } else if (pid != 0) {
        /* Wait for the script to finish */
        while (waitpid(pid, &err, 0) == -1) {
            if (errno != EINTR) {
                STERC_DBG_TRACE(LOG_ERR, "waitpid: %s", strerror(errno));
                err = -1;
                break;
            }
        }
    }


    free(env[0]);
    return err;
}

static int activatePDPContextWithEPPSD(int currCid, int caifConnId)
{
    PdpContextParam pdpContext = {NULL, NULL, NULL, NULL, NULL};
    ATResponse *atresponse = NULL;
    ATLine *currentLine;
    char *env[10] = {NULL};
    int env_idx = 0;
    int err;
    char *cmd = NULL;
    char ifaceName[16];
    char *defaultGatewayStr = NULL;
    int prefix;
    int elem_size;
    char *argv[2] = {NULL, NULL};
    pid_t pid;
    int number_of_envs = 9;

    while (env_idx < number_of_envs)
        env[env_idx++] = NULL;

    env_idx = 0;
    /* AT*EPPSD=<state>,<channel_id>,<cid> */
    asprintf(&cmd, "AT*EPPSD=1,%x,%d", caifConnId, currCid);
    err = at_send_command_multiline(cmd, "   <", &atresponse);
    free(cmd);

    if (err < 0) {
        goto exit;
    }

    if (atresponse->success == 0) {
        err = -1;
        goto exit;
    }

    argv[0] = sterc_runscript_path_connect;

    /* Parse response from EPPSD */
    int buflen = 0;
    int pos = 0;
    char *doc = NULL;
    char *docTail = NULL;
    char *line = NULL;
    char *value = NULL;

    /* Loop once to calculate buffer length. */
    for (currentLine = atresponse->p_intermediates;
            currentLine != NULL; currentLine = currentLine->p_next) {
        char *line = currentLine->line;

        if (line != NULL) {
            buflen += strlen(line);
        }
    }

    if (buflen > 0) {
        doc = malloc(buflen + 1);
        assert(doc != NULL);

        /* Build the buffer containing all lines. */
        for (currentLine = atresponse->p_intermediates;
                currentLine != NULL; currentLine = currentLine->p_next) {
            line = currentLine->line;

            if (line != NULL) {
                strcpy(doc + pos, line);
                pos += strlen(line);
            }
        }

        /* Get IP address */
        value = getFirstElementValue(doc, "<ip_address>", "</ip_address>", NULL);

        if (value != NULL) {
            STERC_DBG_TRACE(LOG_INFO, "IP Address: %s", value);
            pdpContext.ipAddrStr = value;
            value = NULL;
        }

        /* Get Subnet */
        value = getFirstElementValue(doc, "<subnet_mask>", "</subnet_mask>",
                                     NULL);

        if (value != NULL) {
            STERC_DBG_TRACE(LOG_INFO, "Subnet Mask: %s", value);
            pdpContext.subnetMaskStr = value;
            value = NULL;
        }

        /* Get mtu */
        value = getFirstElementValue(doc, "<mtu>", "</mtu>", NULL);

        if (value != NULL) {
            STERC_DBG_TRACE(LOG_INFO, "MTU: %s", value);
            pdpContext.mtuStr = value;
            value = NULL;
        }

        /* We support two DNS servers */
        docTail = NULL;
        value = getFirstElementValue(doc, "<dns_server>", "</dns_server>",
                                     &docTail);

        if (value != NULL) {
            STERC_DBG_TRACE(LOG_INFO, "dns1: %s", value);
            pdpContext.primDnsStr = value;
            value = NULL;
        }

        if (docTail != NULL) {
            /* One more DNS server found */
            value = getFirstElementValue(docTail, "<dns_server>",
                                         "</dns_server>", NULL);

            if (value != NULL) {
                STERC_DBG_TRACE(LOG_INFO, "dns2: %s", value);
                pdpContext.secDnsStr = value;
                value = NULL;
            }
        }

        /*
         * Note GW is not fetched. Default GW is calculated by function
         * getFakedDefaultGatewayStr().
         */
        free(doc);
    }

    /*generate ifacename, normally rmnet0/1/2*/
    snprintf(ifaceName, MAX_IFNAME_LEN, "%s%d", "rmnet", currCid - 1);

    if (!createNetworkInterface(ifaceName, currCid)) {
        STERC_DBG_TRACE(LOG_ERR, "%s(): Failed to create Caif interface. Asserting.", __func__);
        err = -1;
    }


    if (configureNetworkInterfaceIPv4(ifaceName, pdpContext.ipAddrStr, pdpContext.subnetMaskStr,
                                      &defaultGatewayStr, &prefix) < 0) {
        STERC_DBG_TRACE(LOG_ERR, "Failed to configure Caif interface");
        err = -1;
    }

    STERC_DBG_TRACE(LOG_INFO, "Activation and Configuration Successful");

    /*creating environment*/
    env[env_idx] = create_env_dnsmasq_config_file();

    if (NULL != env[env_idx]) {
        env_idx++;
    }

    /*Get static script parameters*/
    for (number_of_envs = 0; number_of_envs < config_params_last_id; number_of_envs++) {
        env[env_idx] = get_param_string(number_of_envs);

        if (NULL != env[env_idx]) {
            env_idx++;
        }
    }

    /*Get dns_address*/
    {
        int my_idx = 0;
        elem_size = strlen("dns_addresses=");
        elem_size += snprintf(NULL, 0, "%s ", pdpContext.primDnsStr);
        elem_size++;
        env[env_idx] = malloc(elem_size);

        if (NULL == env[env_idx]) {
            STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
        }

        strcpy(&env[env_idx][my_idx], "dns_addresses=");
        my_idx += strlen("dns_addresses=");
        my_idx += snprintf(&env[env_idx][my_idx], elem_size - my_idx, "%s ", pdpContext.primDnsStr);
        env[env_idx][my_idx++] = '\0';
        env_idx++;
    }

    /*Get wan_device*/
    {
        int my_idx = 0;
        elem_size = strlen("wan_device=");
        elem_size += strlen(ifaceName);
        elem_size++;
        env[env_idx] = malloc(elem_size);

        if (NULL == env[env_idx]) {
            STERC_DBG_TRACE(LOG_ERR, "malloc failed\n");
        }

        strcpy(&env[env_idx][my_idx], "wan_device=");
        my_idx += strlen("wan_device=");
        my_idx += snprintf(&env[env_idx][my_idx], elem_size - my_idx, "%s ", ifaceName);
        env[env_idx][elem_size -1] = '\0';
        env_idx++;
    }

    env[env_idx] = NULL;

    pid = exec_script(argv, env);
    STERC_DBG_TRACE(LOG_DEBUG, "Pid was %d", pid);

    if (pid == -1) {
        err = -1;
    } else if (pid != 0) {
        /* Wait for the script to finish */
        while (waitpid(pid, &err, 0) == -1) {
            if (errno != EINTR) {
                STERC_DBG_TRACE(LOG_ERR, "waitpid: %s", strerror(errno));
                err = -1;
                break;
            }
        }
    }

exit:
    env_idx = 0;

    while (env[env_idx] != NULL)
        free(env[env_idx++]);

    at_response_free(atresponse);
    return err;
}

static bool createNetworkInterface(const char *ifname, int connection_id)
{
    int ret;
    int ifindex = -1;
    char loop = 0;
    char ifnamecpy[MAX_IFNAME_LEN];
    bool success = true;

    strncpy(ifnamecpy, ifname, MAX_IFNAME_LEN);
    ifnamecpy[MAX_IFNAME_LEN - 1] = '\0';

    ret = rtnl_create_caif_interface(IFLA_CAIF_IPV4_CONNID, connection_id,
                                     ifnamecpy, &ifindex, loop);

    if (!ret)
        STERC_DBG_TRACE(LOG_ERR, "%s() created CAIF net-interface: Name = %s, connection ID = %d, "
                        "Index = %d", __func__, ifnamecpy, connection_id, ifindex);
    else if (ret == -EEXIST) /* Use the existing interface, NOT an error. */
        STERC_DBG_TRACE(LOG_ERR, "%s() found existing CAIF net-interface with same name, reusing: "
                        "Name = %s, connection ID = %d, Index = %d",
                        __func__, ifnamecpy, connection_id, ifindex);
    else {
        STERC_DBG_TRACE(LOG_ERR, "%s() failed creating CAIF net-interface. errno: %d (%s)!",
                        __func__, errno, strerror(errno));
        success = false;
    }

    if (strncmp(ifnamecpy, ifname, MAX_IFNAME_LEN) != 0) {
        STERC_DBG_TRACE(LOG_ERR, "%s() did not get required interface name. Suggested %s but got "
                        "%s. This is considered an error.", __func__, ifname, ifnamecpy);
        success = false;
    }

    return success;
}

static int configureNetworkInterfaceIPv4(char *ifname, char *ipAddrStr,
        char *subnetMaskStr,
        char **defaultGatewayStr, int *prefix)
{
    in_addr_t addr, subaddr;
    in_addr_t tmpsubaddr;
    char *defaultGatewayStrLocal = NULL;
    char *cmd = NULL;
    int fd = -1;
    int af = AF_INET; /* address family */
    struct ifreq ifr;
    struct sockaddr_in *sockaddr_p;
    struct sockaddr_in *saddr_p;
    struct rtentry route;

    if (ifname == NULL || ipAddrStr == NULL || subnetMaskStr == NULL)
        return -1;

    fd = socket(af, SOCK_DGRAM, 0);

    if (fd < 0) {
        STERC_DBG_TRACE(LOG_ERR, "cannot open ioctl socket");
        return -1;
    }

    /*take iface down*/
    memset((char *) &ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
        STERC_DBG_TRACE(LOG_ERR, "failed to get flags");
        goto error;
    }

    ifr.ifr_flags = ifr.ifr_flags & (~IFF_UP);

    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
        STERC_DBG_TRACE(LOG_ERR, "failed to set flags");
        goto error;
    }

    /*Assign IP address*/
    if (inet_pton(AF_INET, ipAddrStr, &addr) <= 0) {
        STERC_DBG_TRACE(LOG_ERR, "not a valid address..failed to convert");
        goto error;
    }

    memset((char *) &ifr, 0, sizeof(struct ifreq));
    sockaddr_p = (struct sockaddr_in *) & (ifr.ifr_addr);
    strncpy((char *) ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    sockaddr_p->sin_family = AF_INET;
    sockaddr_p->sin_port = 0;
    sockaddr_p->sin_addr.s_addr = addr;

    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0) {
        if (EEXIST != errno) {
            STERC_DBG_TRACE(LOG_ERR, "Failed to assign ipaddress %s to interface %s, error: %s", ipAddrStr, ifname, strerror(errno));
        }
    }

    if (inet_pton(AF_INET, subnetMaskStr, &subaddr) <= 0) {
        STERC_DBG_TRACE(LOG_ERR, "%s() failed when calling inet_pton() for %s!", __func__,
                        subnetMaskStr);
        goto error;
    }

    /*
     * This will fake a /31 CIDR network as defined in RFC 3021 to enable us to
     * have 'normal' routes in the routing table.
     */
    if (defaultGatewayStrLocal == NULL && subaddr == htonl(0xFFFFFFFF)) {
        in_addr_t gw;
        struct in_addr gwaddr;
        subaddr = htonl(0xFFFFFFFE);    /* 255.255.255.254, CIDR /31. */
        gw = ntohl(addr) & 0xFFFFFF00;
        gw |= (ntohl(addr) & 0x000000FF);
        gwaddr.s_addr = htonl(gw);
        defaultGatewayStrLocal = strdup(inet_ntoa(gwaddr));
        assert(defaultGatewayStrLocal != NULL);
        STERC_DBG_TRACE(LOG_INFO, "%s generated new fake /31 subnet with gw: %s", __func__,
                        defaultGatewayStrLocal);
    }

    /*Assign netmask*/
    memset((char *) &ifr, 0, sizeof(struct ifreq));
    sockaddr_p = (struct sockaddr_in *) & (ifr.ifr_addr);
    strncpy((char *) ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;
    sockaddr_p->sin_family = AF_INET;
    sockaddr_p->sin_port = 0;
    sockaddr_p->sin_addr.s_addr = subaddr;

    if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0) {
        if (EEXIST != errno) {
            STERC_DBG_TRACE(LOG_ERR, "Failed to assign netmask %s to interface %s, error: %s", subnetMaskStr, ifname, strerror(errno));
        }
    }

    /*take iface up*/
    memset((char *) &ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
        STERC_DBG_TRACE(LOG_ERR, "failed to get flags");
        goto error;
    }

    ifr.ifr_flags = ifr.ifr_flags | IFF_UP;

    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
        STERC_DBG_TRACE(LOG_ERR, "failed to set flags");
        goto error;
    }

    /* Compute prefix length from subnet mask */
    tmpsubaddr = ntohl(subaddr);
    *prefix = 0;

    while (((tmpsubaddr & 0x80000000) != 0) && (*prefix <= 32)) {
        (*prefix)++;
        tmpsubaddr <<= 1;
    }

    if (defaultGatewayStrLocal != NULL) {
        in_addr_t gw;

        if (inet_pton(AF_INET, defaultGatewayStrLocal, &gw) <= 0) {
            STERC_DBG_TRACE(LOG_ERR, "%s() failed calling inet_pton for gw %s!", __func__,
                            defaultGatewayStrLocal);

            goto error;
        }

        memset((char *) &route, 0, sizeof(route));
        route.rt_dst.sa_family = AF_INET;
        route.rt_flags = RTF_UP;
        route.rt_dev = (void *) ifname;
        saddr_p = (struct sockaddr_in *) &route.rt_genmask;
        saddr_p->sin_family = AF_INET;

        if (ioctl(fd, SIOCADDRT, &route) < 0) {
            STERC_DBG_TRACE(LOG_ERR, "Failed to set default route");
            goto error;
        }

        free(defaultGatewayStrLocal);
    }

    close(fd);
    return 0;

error:
    close(fd);
    free(defaultGatewayStrLocal);
    return -1;
}



/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
*
* DESCRIPTION:
*
* Timer porting for Linux.
*
*************************************************************************/

#include <sys/time.h>
#include <pthread.h>

#include "r_smslinuxporting.h"  //for SMS_B_ASSERT, etc.
#include "util_security.h"


//========================================================================
// Local types and definitions
//========================================================================
//========================================================================
//========================================================================

#define IMS_TIMER_IDLE_PERIOD_S   (1000)


enum timer_server_action_t {
    TIMER_SERVER_ACTION_NULL = 0,
    TIMER_SERVER_ACTION_REGISTER,
    TIMER_SERVER_ACTION_DEREGISTER,
    TIMER_SERVER_ACTION_SET,
    TIMER_SERVER_ACTION_SET_RECURRENT,
    TIMER_SERVER_ACTION_RESET,
    TIMER_SERVER_ACTION_KILL_THREAD
};


struct timer_server_interface_data_t {
    enum timer_server_action_t action;
    pthread_t thread;
    int fd;
    uint32_t sig_no;
    uint32_t client_tag;
    uint32_t period_ms;
};


struct timer_server_client_entry_t {
    pthread_t thread;
    int fd;
    struct timer_server_client_entry_t *next_p;
};


struct timer_server_expiry_entry_t {
    int fd;
    uint32_t sig_no;
    uint32_t client_tag;
    uint8_t recurrent;
    struct timeval expiry_time;
    struct timeval period;      // only relevant for recurrent timers
    struct timer_server_expiry_entry_t *next_p;
};


struct timer_server_signal_raw_t {
// PrivateSignalHeader_t     private_signal_header;
    uint32_t Size;
    SMS_SigselectWithClientTag_t sig_wct;
};



//========================================================================
// Variables
//========================================================================
//========================================================================
//========================================================================

static pthread_mutex_t timer_server_interface_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t timer_server_interface_cond_server = PTHREAD_COND_INITIALIZER;
static pthread_cond_t timer_server_interface_cond_client = PTHREAD_COND_INITIALIZER;

static struct timer_server_interface_data_t
 timer_server_interface_data = { TIMER_SERVER_ACTION_NULL, 0, -1, 0, 0, 0 };

static uint8_t timer_server_started = FALSE;

//========================================================================
// Local function prototypes
//========================================================================
//========================================================================
//========================================================================

static void internal_timer_action(
    const struct timer_server_interface_data_t *const timer_if_p);

static void get_next_timeout(
    struct timeval *now_p,
    struct timer_server_expiry_entry_t *expiry_list,
    struct timespec *timeout_time_p);

static void create_and_add_client(
    struct timer_server_client_entry_t **client_list_p,
    struct timer_server_interface_data_t *if_data_p);

static void remove_and_free_client(
    struct timer_server_client_entry_t **client_list_p,
    struct timer_server_expiry_entry_t **expiry_list_p,
    struct timer_server_interface_data_t *if_data_p);

static int get_fd_from_thread(
    struct timer_server_client_entry_t *client_list,
    pthread_t thread);

static void create_and_add_entry_to_expiry_list(
    struct timeval *now_p,
    struct timer_server_client_entry_t *client_list,
    struct timer_server_expiry_entry_t **expiry_list_p,
    struct timer_server_interface_data_t *if_data_p);

static void add_entry_to_expiry_list(
    struct timer_server_expiry_entry_t **expiry_list_p,
    struct timer_server_expiry_entry_t *new_entry_p,
    uint8_t order);

static void remove_and_free_entry_from_expiry_list(
    struct timer_server_client_entry_t *client_list,
    struct timer_server_expiry_entry_t **expiry_list_p,
    struct timer_server_interface_data_t *if_data_p);

static void remove_and_free_entries_from_expiry_list_with_fd(
    struct timer_server_expiry_entry_t **expiry_list_p,
    const int fd);

static void check_and_handle_expirations(
    struct timeval *now_p,
    struct timer_server_expiry_entry_t **expiry_list_p);

static void notify_client(
    int fd,
    uint32_t sig_no,
    uint32_t client_tag);

static void client_list_destroy(
    struct timer_server_client_entry_t **client_list_p);

static void expiry_list_destroy(
    struct timer_server_expiry_entry_t **expiry_list_p);


//========================================================================
// Function bodies - public functions
//========================================================================
//========================================================================
//========================================================================



//========================================================================
// timer_server_ms_since_reset()
//========================================================================
uint32_t timer_server_ms_since_reset(
    void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


//========================================================================
// timer_server_user_register()
//========================================================================
void timer_server_user_register(
    const int expiry_fd)
{
    struct timer_server_interface_data_t timer_if;

    timer_if.action = TIMER_SERVER_ACTION_REGISTER;
    timer_if.thread = pthread_self();
    timer_if.fd = expiry_fd;
    timer_if.sig_no = 0;
    timer_if.client_tag = 0;
    timer_if.period_ms = 0;

    if (timer_server_started) {
        internal_timer_action(&timer_if);
    } else {
        SMS_A_(SMS_LOG_E("(timer_server_user_register)  ERROR: TIMER THREAD NOT STARTED"));
    }
}


//========================================================================
// timer_server_user_deregister()
//
// removes a thread<-->fd pair
//========================================================================
void timer_server_user_deregister(
    void)
{
    struct timer_server_interface_data_t timer_if;

    timer_if.action = TIMER_SERVER_ACTION_DEREGISTER;
    timer_if.thread = pthread_self();
    timer_if.fd = -1;
    timer_if.sig_no = 0;
    timer_if.client_tag = 0;
    timer_if.period_ms = 0;

    internal_timer_action(&timer_if);
}


//========================================================================
// timer_server_set_ms()
//========================================================================
void timer_server_set_ms(
    const uint32_t sig_no,
    const uint32_t client_tag,
    const uint32_t period_ms)
{
    struct timer_server_interface_data_t timer_if;

    timer_if.action = TIMER_SERVER_ACTION_SET;
    timer_if.thread = pthread_self();
    timer_if.fd = -1;
    timer_if.sig_no = sig_no;
    timer_if.client_tag = client_tag;
    timer_if.period_ms = period_ms;

    internal_timer_action(&timer_if);
}


//========================================================================
// timer_server_recurrent_set_ms()
//========================================================================
void timer_server_recurrent_set_ms(
    const uint32_t sig_no,
    const uint32_t client_tag,
    const uint32_t timer_period)
{
    struct timer_server_interface_data_t timer_if;

    timer_if.action = TIMER_SERVER_ACTION_SET_RECURRENT;
    timer_if.thread = pthread_self();
    timer_if.fd = -1;
    timer_if.sig_no = sig_no;
    timer_if.client_tag = client_tag;
    timer_if.period_ms = timer_period;

    internal_timer_action(&timer_if);
}


//========================================================================
// timer_server_reset_ms()
//========================================================================
void timer_server_reset_ms(
    const uint32_t sig_no,
    const uint32_t client_tag)
{
    struct timer_server_interface_data_t timer_if;

    timer_if.action = TIMER_SERVER_ACTION_RESET;
    timer_if.thread = pthread_self();
    timer_if.fd = -1;
    timer_if.sig_no = sig_no;
    timer_if.client_tag = client_tag;
    timer_if.period_ms = 0;

    internal_timer_action(&timer_if);
}


//========================================================================
// timer_server_kill_thread()
//========================================================================
void timer_server_kill_thread(
    void)
{
    struct timer_server_interface_data_t timer_if;

    timer_if.action = TIMER_SERVER_ACTION_KILL_THREAD;
    timer_if.thread = pthread_self();
    timer_if.fd = -1;
    timer_if.sig_no = 0;
    timer_if.client_tag = 0;
    timer_if.period_ms = 0;

    internal_timer_action(&timer_if);
}


//========================================================================
// timer_server_thread()
//========================================================================
void *timer_server_thread(
    void *arg)
{
    static struct timer_server_client_entry_t *client_list = NULL;
    static struct timer_server_expiry_entry_t *expiry_list = NULL;

    struct timespec timeout_time;
    struct timeval now;
    int ret = 0;
    uint8_t alive = TRUE;

    SMS_IDENTIFIER_NOT_USED(arg);

    pthread_detach(pthread_self());

    // Throw away privilege and become a normal user (applies to this thread).
    util_continue_as_non_privileged_user();

    timer_server_interface_data.action = TIMER_SERVER_ACTION_NULL;

    SMS_B_(SMS_LOG_D("(timer_server_thread)  Thread started"));
    timer_server_started = TRUE;

    if (0 != pthread_mutex_lock(&timer_server_interface_mutex)) {
        SMS_B_(SMS_LOG_E("(timer_server_thread) pthread_mutex_lock() failed"));
    }

    // Signal that startup sequence is complete
    start_up_sequence_complete();

    gettimeofday(&now, NULL);

    get_next_timeout(&now, expiry_list, &timeout_time);

    while (alive) {
        do {
            ret = pthread_cond_timedwait(&timer_server_interface_cond_server, &timer_server_interface_mutex, &timeout_time);
        } while (ret == 0 && timer_server_interface_data.action == TIMER_SERVER_ACTION_NULL);


        SMS_B_(SMS_LOG_D("(timer_server_thread)  ret = %d action = %d thread %ld", ret, timer_server_interface_data.action, timer_server_interface_data.thread));

        gettimeofday(&now, NULL);

        if (ret == ETIMEDOUT || ret == 0) {
            if (ret == ETIMEDOUT && expiry_list) {
                // send 'expiry signal' to client(s),
                // remove corresponding entry/entries from expiry list,
                // if recurrent timer(s), calculate next expiry time(s) and re-add entry/entries to expiry list
                check_and_handle_expirations(&now, &expiry_list);
            }                   // else: nothing to do, just restart 'idle timer' to wait for new actions

            switch (timer_server_interface_data.action) {
            case TIMER_SERVER_ACTION_REGISTER:
                create_and_add_client(&client_list, &timer_server_interface_data);
                break;
            case TIMER_SERVER_ACTION_DEREGISTER:
                remove_and_free_client(&client_list, &expiry_list, &timer_server_interface_data);
                break;
            case TIMER_SERVER_ACTION_SET:
            case TIMER_SERVER_ACTION_SET_RECURRENT:
                create_and_add_entry_to_expiry_list(&now, client_list, &expiry_list, &timer_server_interface_data);
                if (timer_server_interface_data.period_ms == 0) {
                    if (timer_server_interface_data.action == TIMER_SERVER_ACTION_SET_RECURRENT) {
                        SMS_B_(SMS_LOG_D("(timer_server_thread) TIMER_ACTION_SET_RECURRENT can not have period 0"));
                    }
                    check_and_handle_expirations(&now, &expiry_list);
                }
                break;
            case TIMER_SERVER_ACTION_RESET:
                remove_and_free_entry_from_expiry_list(client_list, &expiry_list, &timer_server_interface_data);
                break;
            case TIMER_SERVER_ACTION_KILL_THREAD:
                alive = FALSE;
                SMS_A_(SMS_LOG_I("(timer_server_thread)  killed by thread %u", (unsigned int) (timer_server_interface_data.thread)));
                break;
                //case TIMER_ACTION_NULL:
            default:
                if (ret == 0) {
                    SMS_A_(SMS_LOG_E("(timer_server_thread)  timer_server_interface_data.action = %u (bad action) - shall never happen here", timer_server_interface_data.action));
                }
                break;
            }                   // switch
        } else {
            SMS_A_(SMS_LOG_E("(timer_server_thread)  ERROR: pthread_cond_timedwait() returned %d", ret));
        }

        timer_server_interface_data.action = TIMER_SERVER_ACTION_NULL;  // 'done'
        get_next_timeout(&now, expiry_list, &timeout_time);

        if (0 != pthread_cond_broadcast(&timer_server_interface_cond_client)) {
            SMS_B_(SMS_LOG_D("(timer_server_thread)  pthread_cond_broadcast() failed"));
        }
    }

    client_list_destroy(&client_list);
    expiry_list_destroy(&expiry_list);

    pthread_mutex_unlock(&timer_server_interface_mutex);
    pthread_mutex_destroy(&timer_server_interface_mutex);
    pthread_cond_destroy(&timer_server_interface_cond_server);
    pthread_cond_destroy(&timer_server_interface_cond_client);

    timer_server_started = FALSE;

    pthread_exit(NULL);

    return NULL;                // just to avoid compiler warning
}



//========================================================================
// Function bodies - local (static) functions
//========================================================================
//========================================================================
//========================================================================


//========================================================================
// internal_timer_action()
//
// helper for timer_server_user_register(), timer_server_user_deregister(),
// timer_server_set_ms() etc - does the 'cond_signal'-ing
//========================================================================
static void internal_timer_action(
    const struct timer_server_interface_data_t *const timer_if_p)
{

    if (0 != pthread_mutex_lock(&timer_server_interface_mutex)) {
        SMS_B_(SMS_LOG_D("(internal_timer_action) pthread_mutex_lock() failed"));
    }


    SMS_B_(SMS_LOG_D("(internal_timer_action) new action = %d from thread %ld", timer_if_p->action, timer_if_p->thread));

    while (timer_server_interface_data.action != TIMER_SERVER_ACTION_NULL) {
        SMS_B_(SMS_LOG_D
               ("(internal_timer_action)  timer_server_interface_data.action not cleared (= %d, new action = %d), waiting", timer_server_interface_data.action, timer_if_p->action));
        (void) pthread_cond_wait(&timer_server_interface_cond_client, &timer_server_interface_mutex);

    }
    // write data to 'interface struct' - including a non-TIMER_ACTION_NULL value at .action
    timer_server_interface_data = *timer_if_p;

    if (0 != pthread_cond_signal(&timer_server_interface_cond_server)) {
        SMS_B_(SMS_LOG_D("(internal_timer_action) pthread_cond_signal() failed"));
    }

    if (0 != pthread_mutex_unlock(&timer_server_interface_mutex)) {
        SMS_B_(SMS_LOG_D("(internal_timer_action) pthread_mutex_unlock() failed"));
    }
}


//========================================================================
// get_next_timeout()
//========================================================================
static void get_next_timeout(
    struct timeval *now_p,
    struct timer_server_expiry_entry_t *expiry_list,
    struct timespec *timeout_time_p)
{
    struct timeval next_expiry_time;

    if (expiry_list) {
        // first element in list is next to expire
        next_expiry_time = expiry_list->expiry_time;
    } else {
        // nothing to do yet, just (re)start to wait for new actions
        static const struct timeval idle_period = { IMS_TIMER_IDLE_PERIOD_S, 0 };
        timeradd(now_p, (struct timeval *) &idle_period, &next_expiry_time);
    }

    // translate to timespec
    timeout_time_p->tv_sec = next_expiry_time.tv_sec;
    timeout_time_p->tv_nsec = next_expiry_time.tv_usec * 1000;
}


//========================================================================
// create_and_add_client()
//========================================================================
static void create_and_add_client(
    struct timer_server_client_entry_t **client_list_p,
    struct timer_server_interface_data_t *if_data_p)
{
    struct timer_server_client_entry_t *new_entry_p = (struct timer_server_client_entry_t *) malloc(sizeof(struct timer_server_client_entry_t));

    if (new_entry_p) {
        struct timer_server_client_entry_t *current_p = *client_list_p;
        struct timer_server_client_entry_t *previous_p = NULL;

        // make new entry
        new_entry_p->thread = if_data_p->thread;
        new_entry_p->fd = if_data_p->fd;
        new_entry_p->next_p = NULL;

        // add to list


        while (current_p) {
            if (current_p->thread == new_entry_p->thread) {
                if (current_p->fd == new_entry_p->fd) {
                    SMS_A_(SMS_LOG_W("(timer_server_thread - add_client)  WARNING: trying to register client which is already registered (same fd), ignoring"));
                } else {
                    SMS_A_(SMS_LOG_W("(timer_server_thread - add_client)  ERROR: trying to register client which is already registered (different fd), ignoring"));
                }
                free(new_entry_p);
                return;
            }
            previous_p = current_p;
            current_p = current_p->next_p;
        }
        if (previous_p) {       // non-empty list
            previous_p->next_p = new_entry_p;
        } else {
            // empty list
            *client_list_p = new_entry_p;
        }
    }
}


//========================================================================
// remove_and_free_client()
//========================================================================
static void remove_and_free_client(
    struct timer_server_client_entry_t **client_list_p,
    struct timer_server_expiry_entry_t **expiry_list_p,
    struct timer_server_interface_data_t *if_data_p)
{
    if (*client_list_p) {
        // non-empty list
        struct timer_server_client_entry_t *current_p = *client_list_p;
        struct timer_server_client_entry_t *previous_p = NULL;

        while (current_p) {
            if (current_p->thread == if_data_p->thread) {
                // first remove possible timer_server_expiry_entries with fd == current_p->fd
                remove_and_free_entries_from_expiry_list_with_fd(expiry_list_p, current_p->fd);
                // remove user from client list
                if (previous_p) {
                    // non-first element in list
                    previous_p->next_p = current_p->next_p;
                } else {
                    // first element in list
                    *client_list_p = current_p->next_p;
                }
                free(current_p);
                SMS_B_(SMS_LOG_D("(timer_server_thread - remove_client)  found and removed client (thread)"));
                return;
            }
            previous_p = current_p;
            current_p = current_p->next_p;
        }
        SMS_A_(SMS_LOG_E("(timer_server_thread - remove_client)  ERROR: client (thread) not found"));
    } else {
        // empty list, ERROR
        SMS_A_(SMS_LOG_E("(timer_server_thread - remove_client)  ERROR: trying to de-register client, NO clients registered, ignoring"));
    }
}


//========================================================================
// get_fd_from_thread()
//========================================================================
static int get_fd_from_thread(
    struct timer_server_client_entry_t *client_list,
    pthread_t thread)
{
    struct timer_server_client_entry_t *current_p = client_list;

    while (current_p) {
        if (current_p->thread == thread) {
            return current_p->fd;
        }
        current_p = current_p->next_p;
    }
    return -1;
}


//========================================================================
// create_and_add_entry_to_expiry_list()
//
// (set a timer)
//========================================================================
static void create_and_add_entry_to_expiry_list(
    struct timeval *now_p,
    struct timer_server_client_entry_t *client_list,
    struct timer_server_expiry_entry_t **expiry_list_p,
    struct timer_server_interface_data_t *if_data_p)
{
    struct timer_server_expiry_entry_t *new_entry_p = NULL;
    int fd = get_fd_from_thread(client_list, if_data_p->thread);
    if (fd < 0) {
        SMS_A_(SMS_LOG_W("(timer_server_thread - create_and_add_entry_to_expiry_list)  ERROR: client (thread) not registered, ignoring"));
        return;
    }

    new_entry_p = (struct timer_server_expiry_entry_t *) malloc(sizeof(struct timer_server_expiry_entry_t));

    if (new_entry_p) {
        // make new entry
        new_entry_p->fd = fd;
        new_entry_p->sig_no = if_data_p->sig_no;
        new_entry_p->client_tag = if_data_p->client_tag;
        new_entry_p->recurrent = (if_data_p->action == TIMER_SERVER_ACTION_SET_RECURRENT);
        new_entry_p->period.tv_sec = if_data_p->period_ms / 1000;
        new_entry_p->period.tv_usec = (if_data_p->period_ms % 1000) * 1000;
        timeradd(now_p, &new_entry_p->period, &new_entry_p->expiry_time);
        new_entry_p->next_p = NULL;

        // add to list
        add_entry_to_expiry_list(expiry_list_p, new_entry_p, TRUE);
    }
}


//========================================================================
// add_entry_to_expiry_list()
//
// If 'order' is TRUE, the new entry is inserted in expiry time order.
// If 'order' is FALSE, the new entry is inserted last.
//========================================================================
static void add_entry_to_expiry_list(
    struct timer_server_expiry_entry_t **expiry_list_p,
    struct timer_server_expiry_entry_t *new_entry_p,
    uint8_t order)
{
    if (*expiry_list_p) {
        // non-empty list - first to expire should be first
        struct timer_server_expiry_entry_t *current_p = *expiry_list_p;
        struct timer_server_expiry_entry_t *previous_p = NULL;

        while (current_p) {
            if (order && timercmp(&current_p->expiry_time, &new_entry_p->expiry_time, >)) {
                break;
            }
            previous_p = current_p;
            current_p = current_p->next_p;
        }
        if (previous_p) {
            // add non-first in non-empty list
            new_entry_p->next_p = current_p;    // NULL if adding last
            previous_p->next_p = new_entry_p;
        } else {
            // add first in non-empty list
            new_entry_p->next_p = *expiry_list_p;
            *expiry_list_p = new_entry_p;
        }
    } else {
        // empty list
        new_entry_p->next_p = NULL;     // just to be shure..
        *expiry_list_p = new_entry_p;
    }
}


//========================================================================
// remove_and_free_entry_from_expiry_list()
//
// (reset a timer)
//========================================================================
static void remove_and_free_entry_from_expiry_list(
    struct timer_server_client_entry_t *client_list,
    struct timer_server_expiry_entry_t **expiry_list_p,
    struct timer_server_interface_data_t *if_data_p)
{
    int fd = get_fd_from_thread(client_list, if_data_p->thread);
    if (fd < 0) {
        SMS_A_(SMS_LOG_E("(timer_server_thread - remove_and_free_entry_from_expiry_list)  ERROR: client (thread) not registered, ignoring"));
        return;
    }
    // remove from list
    if (*expiry_list_p) {
        // non-empty list - first to expire should be first
        struct timer_server_expiry_entry_t *current_p = *expiry_list_p;
        struct timer_server_expiry_entry_t *previous_p = NULL;

        while (current_p) {
            if (current_p->fd == fd && current_p->sig_no == if_data_p->sig_no && current_p->client_tag == if_data_p->client_tag) {
                break;
            }
            previous_p = current_p;
            current_p = current_p->next_p;
        }
        if (current_p) {
            if (previous_p) {
                // element to remove is not first in list
                previous_p->next_p = current_p->next_p; // NULL if removing last
            } else {
                // element to remove is first in list
                *expiry_list_p = current_p->next_p;     // NULL if removing last;
            }
            free(current_p);
            SMS_B_(SMS_LOG_D("(timer_server_thread - remove_and_free_entry_from_expiry_list)  found and removed entry"));
        } else {
            SMS_B_(SMS_LOG_D("(timer_server_thread - remove_and_free_entry_from_expiry_list) warning: entry with sig_no = 0x%08X, client_tag 0x%08X, not found",
                          if_data_p->sig_no, if_data_p->client_tag));
        }
    } else {
        SMS_B_(SMS_LOG_D("(timer_server_thread - remove_and_free_entry_from_expiry_list)  warning: expiry list is empty, ignoring"));
    }
}




//========================================================================
// remove_and_free_entries_from_expiry_list_with_fd()
//
// (reset all timers of a certain client)
//========================================================================
static void remove_and_free_entries_from_expiry_list_with_fd(
    struct timer_server_expiry_entry_t **expiry_list_p,
    const int fd)
{
    if (fd < 0) {
        SMS_LOG_D("(timer_server_thread - remove_and_free_entries_from_expiry_list_with_fd)  ERROR: client (thread) not registered, ignoring");
        return;
    }
    // remove from list
    if (*expiry_list_p) {
        // non-empty list - first to expire should be first
        struct timer_server_expiry_entry_t *current_p = *expiry_list_p;
        struct timer_server_expiry_entry_t *next_p = NULL;
        struct timer_server_expiry_entry_t *previous_p = NULL;

        while (current_p) {
            next_p = current_p->next_p;

            if (current_p->fd == fd) {
                if (previous_p) {
                    // element to remove is not first in list
                    previous_p->next_p = current_p->next_p;     // NULL if removing last
                } else {
                    // element to remove is first in list
                    *expiry_list_p = current_p->next_p; // NULL if removing last;
                }
                free(current_p);
                //SMS_LOG_W("(timer_server_thread - remove_and_free_entries_from_expiry_list_with_fd)  found and removed one entry");

                // keep previous_p as is
                current_p = next_p;
            } else {
                previous_p = current_p;
            }
            current_p = next_p;
        }
    } else {
        SMS_LOG_W("(timer_server_thread - remove_and_free_entries_from_expiry_list_with_fd)  warning: expiry list is empty, ignoring");
    }
}



//========================================================================
// check_and_handle_expirations()
//========================================================================
static void check_and_handle_expirations(
    struct timeval *now_p,
    struct timer_server_expiry_entry_t **expiry_list_p)
{
    if (*expiry_list_p) {
        struct timer_server_expiry_entry_t *temp_list = NULL;
        struct timer_server_expiry_entry_t *entry_p = NULL;

        while (*expiry_list_p && !timercmp(&((*expiry_list_p)->expiry_time), now_p, >)) {
            entry_p = *expiry_list_p;
            notify_client(entry_p->fd, entry_p->sig_no, entry_p->client_tag);
            *expiry_list_p = entry_p->next_p;   // remove first from list
            if (entry_p->recurrent) {
                // put in temp list for now
                entry_p->next_p = NULL;
                add_entry_to_expiry_list(&temp_list, entry_p, FALSE);
            } else {
                free(entry_p);
            }
        }

        // update expired recurrent entries, if any, and insert to 'normal list'
        if (temp_list) {
            struct timer_server_expiry_entry_t *next_entry_p = NULL;

            entry_p = temp_list;
            while (entry_p) {
                next_entry_p = entry_p->next_p;
                timeradd(&entry_p->expiry_time, &entry_p->period, &entry_p->expiry_time);
                entry_p->next_p = NULL;
                add_entry_to_expiry_list(expiry_list_p, entry_p, TRUE);
                entry_p = next_entry_p;
            }
        }

    }                           // if( *expiry_list_p  )
}


//========================================================================
// notify_client()
//
// send a 'timer signal' to the user
//========================================================================
static void notify_client(
    int fd,
    uint32_t sig_no,
    uint32_t client_tag)
{
    ssize_t ret;
    ssize_t raw_size;
    struct timer_server_signal_raw_t sig_raw;

    sig_raw.Size = sizeof(SMS_SigselectWithClientTag_t);
    sig_raw.sig_wct.Primitive = sig_no;
    //SMS_LOG_D("mmprot_timers_linux.c: sending time out signal with sig_no %lu", sig_raw.sig_wct.Primitive);
    sig_raw.sig_wct.ClientTag = client_tag;

    raw_size = sizeof(struct timer_server_signal_raw_t);

    ret = write(fd, &sig_raw, raw_size);
    if (ret != raw_size) {
        if (ret < 0) {
            SMS_A_(SMS_LOG_E("(timer_server_thread - notify_client)  ERROR: write failed, errno = %d", errno));
        } else {
            SMS_A_(SMS_LOG_E("(timer_server_thread - notify_client)  ERROR: write failed, ret = %d (!=%d)", (int) ret, (int) raw_size));
        }
    }
}


//========================================================================
// client_list_destroy()
//========================================================================
static void client_list_destroy(
    struct timer_server_client_entry_t **client_list_p)
{
    struct timer_server_client_entry_t *next_entry_p = NULL;

    while (*client_list_p) {
        next_entry_p = (*client_list_p)->next_p;
        free(*client_list_p);
        *client_list_p = next_entry_p;
    }
}


//========================================================================
// expiry_list_destroy()
//========================================================================
static void expiry_list_destroy(
    struct timer_server_expiry_entry_t **expiry_list_p)
{
    struct timer_server_expiry_entry_t *next_entry_p = NULL;

    while (*expiry_list_p) {
        next_entry_p = (*expiry_list_p)->next_p;
        free(*expiry_list_p);
        *expiry_list_p = next_entry_p;
    }
}


//EOF

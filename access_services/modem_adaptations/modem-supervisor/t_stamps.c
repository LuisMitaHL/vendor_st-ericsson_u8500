/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

#include "t_stamps.h"
#include "modem-sup.h"


#define BINNAME "TIME-STAMP"

#ifdef HAVE_ANDROID_OS
#define LOG_TAG BINNAME
#include <cutils/log.h>
#define OPENLOG(facility) ((void)0)
#else
#include <syslog.h>
#define OPENLOG(facility) openlog(BINNAME, LOG_PID | LOG_CONS, facility)
#define LOG(priority, format, ...) syslog(priority, format, ##__VA_ARGS__)
#define ALOGV(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGD(format, ...)   LOG(LOG_DEBUG, format, ##__VA_ARGS__)
#define ALOGI(format, ...)   LOG(LOG_INFO, format, ##__VA_ARGS__)
#define ALOGW(format, ...)   LOG(LOG_WARNING, format, ##__VA_ARGS__)
#define ALOGE(format, ...)   LOG(LOG_ERR, format, ##__VA_ARGS__)
#endif


/*
   Get Target current time and return time in s
 */
int get_timestamp(long long int * ll_time)
{
    struct timespec now;
    int status;

    status = clock_gettime(CLOCK_REALTIME, &now);

    /* Calculate time in s  */
    /*     (*ll_time)=(long long int)((long long int)(now.tv_sec*1000)+(long long int)(now.tv_nsec/1000000)); */
    (*ll_time) = (long long int)((long long int)(now.tv_sec) + (long long int)(now.tv_nsec / 1000000000));

    /*    ALOGI ("Time %lld sec %lld nsec\n", (long long int)now.tv_sec, (long long int)now.tv_nsec); */
    ALOGI("TimeStamp  %lld sec \n", (long long int)(*ll_time));

    return status;
}


/*

*/
void list_selfirst(info_reset_s *p_info_reset)
{
    if (p_info_reset) {
        p_info_reset->list = p_info_reset->p_start->next;
    }
}


void list_selnext(info_reset_s * p_info_reset)
{
    if (p_info_reset && p_info_reset->list) {
        p_info_reset->list = p_info_reset->list->next;
    }
}

void list_sellast(info_reset_s * p_info_reset)
{
    if (p_info_reset) {
        while (p_info_reset->list->next != NULL) {
            list_selnext(p_info_reset);
        }
    }
}

/*

*/
void list_removeNext(info_reset_s *p_info_reset)
{

    if (p_info_reset && p_info_reset->list) {
        reset_ind_s *p_l = p_info_reset->list;
        reset_ind_s *p_n = NULL;

        p_n = p_l->next;
        p_l->next = p_n->next;
        free(p_n);
        p_n = NULL;
        p_info_reset->total_reset_number--;
    }
}

/*

*/
void list_removeFirst(info_reset_s *p_info_reset)
{
    if (p_info_reset) {
        p_info_reset->list = p_info_reset->p_start;
        list_removeNext(p_info_reset);
    }
}


/*

*/
void list_removeLast(info_reset_s *p_info_reset)
{

    if (p_info_reset) {
        list_selfirst(p_info_reset);

        while (p_info_reset->list->next->next != NULL) {
            p_info_reset->list = (reset_ind_s *) p_info_reset->list->next;
        }

        list_removeNext(p_info_reset);

        /* Back to the top list */
        list_selfirst(p_info_reset);
    }
}



/*

*/
void *list_display_data(info_reset_s * p_info_reset)
{
    return ((p_info_reset && p_info_reset->list) ? &(p_info_reset->list->rtime) : NULL);
}


size_t list_sizeof_reset(info_reset_s * p_info_reset)
{
    size_t n = 0;

    if (p_info_reset) {
        list_selfirst(p_info_reset);

        while (p_info_reset->list != NULL) {
            n++;
            list_selnext(p_info_reset);
        }
    }

    return n;
}


/*
   Creation Reset List
*/
info_reset_s * list_create_reset(void)
{
    reset_ind_s * p_reset_ind;
    info_reset_s * p_info_reset = NULL;

    p_info_reset = malloc(sizeof(info_reset_s));

    if (p_info_reset) {
        p_reset_ind = malloc(sizeof(reset_ind_s));

        if (p_reset_ind) {
            p_reset_ind->rtime = 0;
            p_reset_ind->next = NULL;

            p_info_reset->p_start = p_reset_ind;
            p_info_reset->list = NULL;
            p_info_reset->total_reset_number = 0;
        } else {
            ALOGE("Memory allocation failure \n");
            return NULL;
        }
    } else {
        ALOGE("Memory allocation failure \n");
        return NULL;

    }

    ALOGI("Reset modem list created \n");
    return p_info_reset;
}


/*

*/
void list_remove_reset(info_reset_s ** pp_info_reset)
{
    if (pp_info_reset && *pp_info_reset) {
        list_selfirst(*pp_info_reset);

        while ((*pp_info_reset)->list->next != NULL) {
            list_removeNext(*pp_info_reset);
        }

        list_removeFirst(*pp_info_reset);
        free((*pp_info_reset)->list);
        (*pp_info_reset)->list = NULL;
        free(*pp_info_reset);
        *pp_info_reset = NULL;
    }
}

/*

*/
/*int list_add_reset (info_reset_s *p_info_reset)*/
info_reset_s * list_add_reset(info_reset_s *p_info_reset)
{
    long long int  rtime;
    reset_ind_s *p_n = NULL;
    info_reset_s * p_info_reset_1 = p_info_reset;

    get_timestamp(&rtime);

    if (p_info_reset_1) {
        reset_ind_s *p_l = p_info_reset_1->list;
        p_n = malloc(sizeof(*p_n));

        if (p_n) {
            p_n->rtime = rtime;

            if (p_l == NULL) {
                p_info_reset_1->p_start->next = p_n;
                p_n->next = NULL;
            } else {
                p_info_reset_1->p_start->next = p_n;
                p_n->next = p_l;
            }

            p_info_reset_1->list = p_n;
            p_info_reset_1->total_reset_number++;
        } else {
            ALOGI("Memory allocation failure \n");
            return (info_reset_s *) - 1;
        }
    }

    return (info_reset_s *) p_info_reset_1;
}



/*

*/
long int list_diff_time(info_reset_s *p_info_reset)
{

    int i;
    long int * p_tmp = NULL, *p_last_reset = NULL, *p_first_reset = NULL;

    list_selfirst(p_info_reset);

    for (i = 0; i < (p_info_reset->total_reset_number); i++) {
        if (list_display_data(p_info_reset) != NULL) {
            p_tmp = list_display_data(p_info_reset);
            ALOGI(" Elt %d/%d Time:%ld sec \n", i , p_info_reset->total_reset_number, (long int)*p_tmp);

            if (i == 0)
                p_first_reset = p_tmp;
        }

        list_selnext(p_info_reset);
    }

    /* Back to the top list */
    list_selfirst(p_info_reset);

    p_last_reset = p_tmp;

    if ((p_last_reset != NULL) && (p_first_reset != NULL)) {
        ALOGI("First Reset:%ld  Last reset:%ld TimeDiff:%ld s\n",
             (long int)*p_first_reset, (long int)*p_last_reset,
             (long int)((*p_first_reset) - (*p_last_reset)));

        return ((long int)((*p_first_reset) - (*p_last_reset)));
    } else
        return 0;
}


/*

*/
int list_check_reset_number(info_reset_s *p_info_reset, int max_reset_number)
{

    if ((p_info_reset->total_reset_number) >= max_reset_number) {
        ALOGI(" Reset number =%d Max =%d \n  Remove first reset entry  \n",
             p_info_reset->total_reset_number, max_reset_number);
        list_removeLast(p_info_reset);
    }

    return SUCCESS;
}

/* Will clear the diff time list */
void list_clear_reset(info_reset_s ** pp_info_reset)
{
    if (pp_info_reset && *pp_info_reset) {
        list_selfirst(*pp_info_reset);

        while ((*pp_info_reset)->list->next != NULL) {
            list_removeNext(*pp_info_reset);
        }

        list_removeFirst(*pp_info_reset);
    }
}

/*

*/
int store_reset_ind(info_reset_s *p_info_reset, int max_reset_number, int min_reset_delay)
{
    long int diff_time;

    list_add_reset(p_info_reset);

    ALOGI("#store_reset_ind# reset nbr        : %d \n", p_info_reset->total_reset_number);
    diff_time = list_diff_time(p_info_reset);

	/*
	 * Might get a negative time due to user entering
	 * and leaving flight mode. If so clear time list
	 * since data is of no use anymore.
	 */
    if(diff_time < 0) {
        ALOGI("Negative value in modem reset history so clear list\n");
        list_clear_reset(&p_info_reset);
    } else {
        if (diff_time) {
            /* Condition that define if complete target restarted needed */
            if ((diff_time < min_reset_delay) &&
                (p_info_reset->total_reset_number >= max_reset_number)) {
                ALOGI("max_reset_nbr   : %d current reset nbr : %d \n", max_reset_number , p_info_reset->total_reset_number);
                ALOGI("min_reset_delay : %ld difftime \n", diff_time);
                return (int) - 1;
            } else {
                ALOGI("max_reset_nbr   : %d current reset nbr : %d \n", max_reset_number , p_info_reset->total_reset_number);
                ALOGI("min_reset_delay : %ld difftime \n", diff_time);
            }
        }

        list_check_reset_number(p_info_reset, max_reset_number);

        ALOGI("Reset List   : 0x%x \n", (int)p_info_reset->list);
    }
    return SUCCESS;
}

/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   Profile.h
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _PROFILE_H_
#define _PROFILE_H_

#include "sys/time.h"
#include "stdio.h"

typedef struct cycles_struct{
	int nb_calls;
	int test;
	struct timeval start,total;
} cycles_struct;

typedef struct prof_struct{
	int size;
	cycles_struct *cycles;
} prof_struct;

extern prof_struct prf;

void init_perf(int init_nb);
void prof_concl();

long long get_total_times ();
int get_mean_times ();

static int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y)
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}
static void timeval_add (struct timeval *result, struct timeval *x, struct timeval *y)
{
    result->tv_usec = x->tv_usec + y->tv_usec;
    result->tv_sec = x->tv_sec + y->tv_sec;
    while (result->tv_usec >= 1000000) {
        result->tv_sec ++;
        result->tv_usec -= 1000000;
    }
}

static void start_perf(int index)
{
	struct timezone tz;
	prf.cycles[index].test = gettimeofday(&(prf.cycles[index].start), &tz);
}

static void end_perf(int index)
{
    struct timeval end;
    struct timezone tz;
    if (!(prf.cycles[index].test || gettimeofday(&end, &tz))) {
		struct timeval diff;
        timeval_subtract(&diff,&end,&(prf.cycles[index].start));
        timeval_add(&(prf.cycles[index].total),&(prf.cycles[index].total),&diff);
		prf.cycles[index].nb_calls++;
	}
}


#endif


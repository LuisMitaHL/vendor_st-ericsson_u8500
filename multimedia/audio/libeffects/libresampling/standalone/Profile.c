/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   Profile.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
#include "Profile.h"

prof_struct prf;

void init_perf(int init_nb)
{
//    printf("Cycle cont used\n");
	prf.cycles = calloc(init_nb,sizeof(cycles_struct));
	prf.size=init_nb;
}

void get_total_times ()
{
	int index;
    printf("total times: \n");
	for(index=0;index<prf.size;index++) {
		if (prf.cycles[index].nb_calls > 0)
		{
			long long time= ((long long)(prf.cycles[index].total.tv_usec)) + ((long long)(prf.cycles[index].total.tv_sec))*((long long)1000000);
			printf("\tIndex %d: total time is %lld us for %d calls\n", index,time,prf.cycles[index].nb_calls);
		}
		else
		{
			printf("\tIndex %d has not been used or has problems\n", index);
		}
	}
}

int get_mean_times ()
{
	int timeval,index;
//    printf("mean times:\n");
	for(index=0;index<prf.size;index++) {
		if (prf.cycles[index].nb_calls > 0)
		{
			long long time= ((long long)(prf.cycles[index].total.tv_usec)) + ((long long)(prf.cycles[index].total.tv_sec))*((long long)1000000);
//			printf("\tIndex %d: mean time by call is %d us\n", index, (int)(time/((long long)(prf.cycles[index].nb_calls))));
			timeval=(int)(time/((long long)(prf.cycles[index].nb_calls)));
		}
		else
		{
			printf("\tIndex %d has not been used or has problems\n", index);
		}
	}
	return timeval; 
}

void prof_concl()
{
	int index;
    printf("conclusion:\n");
	for(index=0;index<prf.size;index++) {
		if (prf.cycles[index].nb_calls > 0)
		{
			long long time=((long long)(prf.cycles[index].total.tv_usec)) + ((long long)(prf.cycles[index].total.tv_sec))*((long long)1000000);
			printf("\tIndex %d: total time is %lld us for %d calls, ", index, time, prf.cycles[index].nb_calls);
            printf("mean time by call is %d us\n", (int)(time/((long long)(prf.cycles[index].nb_calls))));
		}
		else
		{
			printf("\tIndex %d has not been used or has problems\n", index);
		}
	}

}


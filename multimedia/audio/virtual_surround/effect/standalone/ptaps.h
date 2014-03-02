#ifndef __ptaps_h__
#define __ptaps_h__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "pmu_api.h"
#ifdef ARM_PMU
    int init_platform(int);
    int ptap(int); /* do not change int (int) */;
    int ptapd(int,int); /* do not change int (int) */;
    int ptapdall(int flag);
    #define ARMTAPPER(x) x    
#else
    #define init_platform(x)
    #define ptap(x)
    #define ptapd(x,y)
    #define ptapdall(x)
    #define ARMTAPPER(x) 
#endif
#define MAX_TAPD 255
#define MAX_TAP_ID_LENGTH 100
#define EVENT_MAX 6
typedef struct {
 unsigned int cycle;
 unsigned int pm[EVENT_MAX];
} t_pmu_data;

typedef struct {
 unsigned long long cycle;
 unsigned long long pm[EVENT_MAX];
} t_pmu_data_long;  /* for total case */


 typedef struct {
 unsigned int id;
 unsigned int tap_s;
 unsigned int tap_e;
 unsigned int count;
 t_pmu_data pmu;
 t_pmu_data max_pmu;
 unsigned int max_pmu_index;
 t_pmu_data_long total_pmu; 
} t_pmutapd;




typedef struct {
 unsigned int no_of_entries;
 t_pmutapd *tap_table;
 char (*tapd_hash)[MAX_TAP_ID_LENGTH];
 unsigned int max_entries;
} t_pmutapdSD;


#define PMCR_ENABLE 0x1
#define PMCR_CCNT_RESET 0x4
#define PMCR_PMUS_RESET 0x2


#define COUNTERS_P0 0x1
#define COUNTERS_P1 0x2
#define COUNTERS_P2 0x4
#define COUNTERS_P3 0x8
#define COUNTERS_P4 0x10
#define COUNTERS_P5 0x20    
#define COUNTERS_CCNT 0x80000000

void PMU_Init(unsigned int p0, unsigned int p1,unsigned int p2,unsigned int p3,unsigned int p4,unsigned int p5 );
int addto_tap_table(int end,int start,int line,char* filename);
void PMU_verbose_actual(int end, int start,int line,char* filename);
void update_tap_table(int end,int start,int index);
int PMU_tap(int tapid); /* do not change int (int) */

void PerfEnableDisable(int);
void PerfSetTickRate(int);
void PerfSelectEvent(int,int);  
void PerfSelectEvent(int,int); 
void PerfSelectEvent(int,int); 
void PerfSelectEvent(int,int); 
void PerfReset(int);  /* reset cycle counter*/
void PerfReset(int);
int PerfGetCount(int);

int init_platform(int );
int ptap(int) /* do not change int (int) */;
int ptapd(int,int) /* do not change int (int) */;
int ptapdall(int flag);


void overhead(void);
int getPf0(int);
int getPf1(int);
int tapID2Index(int start, int end, int line , char *filename );
void PMU_print_display(int,int,char*);
void PMU_print_verbose(int,int,char*);
void PMU_verbose_actual(int,int,int,char*);
void PMU_display_actual(int,int,int,char*);
int  PMU_quiet_actual(int,int,int,char*);

#endif



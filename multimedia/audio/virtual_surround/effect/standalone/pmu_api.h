#ifndef __PTAPS_API_H
#define __PTAPS_API_H

#define PMU_INSTRUCTIONS 0x68
#define PMU_DATA_MISS    0x03
#define PMU_DATA_PENALTY 0x61 
#define PMU_INST_MISS    0x01
#define PMU_INST_PENALTY 0x60
#define PMU_BRANCH_MISS  0x10
/* add here more */

#ifdef ARM_CYCLES
void PMU_init(unsigned int,unsigned int,unsigned int,unsigned int,unsigned int,unsigned int); /* init the PMU */
int  PMU_tap(int);  /* Mark a tap point */
int  PMU_tapgetevent(int end,int start,int event);  /* get event measurement between 2 tappoints */
int  PMU_tapgetcycles(int end,int start);/* get cycle measurement between 2 tappoints end and start */
#define PMU_display(e,s) PMU_display_actual(e,s,__LINE__,__FILE__)
/* Display output captured between two tap points*/
#define PMU_verbose(e,s) PMU_verbose_actual(e,s,__LINE__,__FILE__)
/* Display output captured between two tap points. This api displays max/avg data also*/
#define PMU_quiet(e,s)   PMU_quiet_actual(e,s,__LINE__,__FILE__) 
/* Mark point for verbose output, but do not display. Display when unquiet is called. Pass the return value from quiet to unquiet*/
#define PMU_unquiet(a)   PMU_unquiet_actual(a,__LINE__,__FILE__) 
/* Mark point for verbose output, but do not display. Display when unquiet is called. Pass the return value from quiet to unquiet*/
int  PMU_getevent(int); /* get event value at any instant */
int  PMU_getcycles(void); /* get cycle register value at any instant */
int PMU_unquiet_actual(int,int, char*); /* verbose output of quiet tapped point */
void PMU_verbose_actual(int end,int start,int line,char* filename);
void PMU_display_actual(int end,int start,int line,char* filename);
int  PMU_quiet_actual(int end,int start,int line,char* filename);
void PerfEnableDisable(int);
void PerfSetTickRate(int);
void PerfSelectEvent(int,int);  
void PerfReset(int);  /* reset cycle counter*/
void PerfReset(int);
int  PerfGetCount(int);
void PMU_pause(void);
void PMU_unpause(void);
#else
#define PMU_init(a,b,c,d,e,f)
#define PMU_tap(x)
#define PMU_tapgetevent(a,b,c)
#define PMU_tapgetcycles(a,b)
#define PMU_getevent(a)
#define PMU_getcycles()
#define PMU_display(a,b)
#define PMU_verbose(a,b)
#define PMU_quiet(a,b)
#define PMU_unquiet(a)
#define PMU_pause()
#define PMU_unpause()
#endif
#ifdef L1_CACHE_PURGE
#else
#endif

#endif



#ifndef _debug_h_
#define _debug_h_

#ifdef __cplusplus
#define CPPCALL "C"
#else
#define CPPCALL
#endif

extern CPPCALL volatile int  DebugOn;
extern CPPCALL void debuglog_short(char *name, short *t, int n);
extern CPPCALL void debuglog_ushort(char *name, unsigned short *t, int n);
extern CPPCALL void debuglog(char *name, int *t, int n);
extern CPPCALL void debuglog_long(char *name, long *t, int n);
extern CPPCALL void debuglog_float(char *name, float *t, int n);

#endif /* Do not edit below this line */
     

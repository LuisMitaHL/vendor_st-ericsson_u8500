//**********************************************************************
//	ST Microelectronics.
//	TPA, DMD, MMTC, Audio, Soft, ZeOS.
//	Grenoble design center.
//
//	MMDSP+ Zeos Project (Zede's Embedded Operating System).
//	Jean-Marc Zins (zede).
//
// Code	   :    ANSI C for MMDSP+ processor.
//
//**********************************************************************
#ifndef __mem_h__
#define __mem_h__

#include "inc/type.h"
#include <stddef.h>

#define MEM_SUCCESS              0
#define MEM_INVALID_ADDRESS     -1
#define MEM_DISABLED_ALLOC_TYPE -2

/* #ifndef UNIX */
/* extern unsigned int Z_mem_Size_Left[NB_MEMORY_SPACE]; */
/* #endif */

#ifndef _NMF_MPC_

extern int  init_intmem(void * p_intmemX_start, size_t sz_intmemX, void * p_intmemY_start, size_t sz_intmemY);

extern void *malloc_X(size_t size);
extern int   mfree(void *mem);
extern YMEM void *malloc_Y(size_t size);
//extern void Z_Mem_Free_Y(void *ptr);


#ifdef EXTMEM_ALLOC
extern void *malloc_ext(size_t size);
extern void *malloc_ext16(size_t size);
#endif //EXTMEM_ALLOC

#ifdef __esram__
extern void *malloc_esram(size_t size);
extern void *malloc_esram16(size_t size);
#endif //__esram__

#ifdef __ext_mem__
/*
 * Set available external memories area for malloc use : 
 * @param P_extmem24_start :the start address (MMDSP memory addressing) of available 24 bits external memory
 * @param sz_extmem24 : the size (in MMDSP MAU) of the available 24 bits external memory
 * @param p_extmem16_start : the start address (MMDSP memory addressing) of available 16 bits external memory
 * @param sz_extmem16 : the size (in MMDSP MAU) of the available 16 bits external memory
 */
extern int init_extmem(void * p_extmem24_start, size_t sz_extmem24, void * p_extmem16_start, size_t sz_extmem16);
#endif //__ext_mem__

#ifdef __esram__
/*
 * Set available embeded SDRAM area for malloc use :
 * @param p_esram24_start : the start address (MMDSP memory addressing) of available 24 bits esram
 * @param sz_esram24 : the size (in MMDSP MAU) of the available 24 bits esram
 * @param p_esram16_start : the start address (MMDSP memory addressing) of available 16 bits esram
 * @param sz_esram16 : the size (in MMDSP MAU) of the available 16 bits esram
 */
extern int init_esram(void * p_esram24_start, size_t sz_esram24, void * p_esram16_start, size_t sz_esram16);
#endif // __esram__

extern void Z_Get_Thread_All_Mem(int pid, unsigned int *xmem, unsigned int *ymem, unsigned int *esram24, unsigned int *esram16, unsigned int *ext24, unsigned int *ext16);
extern void Z_Get_Thread_Max_Block(int pid, unsigned int *xmax, unsigned int *ymax, unsigned int *esram24max, unsigned int *esram16max, unsigned int *ext24max, unsigned int *ext16max);
#endif // _NMF_MPC_ 

#endif // __mem_h__

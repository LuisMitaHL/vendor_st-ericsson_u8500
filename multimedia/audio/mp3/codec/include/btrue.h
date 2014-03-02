
#ifndef _btrue_h_
#define _btrue_h_

#include "audiolibs_common.h"

#ifndef MMDSP

#include <stdio.h>

#define bit_field(pos, size)	((pos & 0x1f) | ((size & 0x1f) << 8))
#define cmove(cond, a, b)	    (a = (cond) ? (b) : (a))


/* definitions for loop counter */
#define MAX_NESTED_LOOP 500 // no limit of nested loop in non mmdsp mode
#if 0
#define loop(a)   _lct++; \
if (_lct>MAX_NESTED_LOOP) \
{ fprintf(stderr,"Too much nested loops\n");exit(1);}\
for(_ct[_lct]=a;_ct[_lct]>0 || (_lct--==-1);_ct[_lct]--)
#else
#define loop(a)   _lct++; \
for(_ct[_lct]=a;_ct[_lct]>0 || (_lct--==-1);_ct[_lct]--)
#endif
extern int             _lct;
extern int	       _ct[1 + MAX_NESTED_LOOP];	
extern int             _lstack[100];
extern int             _lsp;
extern int             _Fcarry;

extern int             wextract(int a, int b);
extern int             wextractu(int a, int b);
extern int             wmerge(int a, int b);
extern int             winsert(int a,int b,int  r2l);
extern void            *mod_add(void *reg_value, int index,
                              void *Max_value, void *Min_value);
//extern Float          *br_add( Float *ad, int index, Float *base);
extern void 		   *br_add(void *ad, int index, void *base);

extern int             wasl(int a, int shift);
extern int             wasr(int a, int shift);
extern int             wlsr(int a, int shift);
extern CPPCALL AUDIO_API_IMPORT int             winterval(int a, int maxval);
extern int             wxor(int a, int b);
extern int             wmaxm(int a, int b);
extern int             wmin(int a, int b);
extern int             wmax(int a, int b);
extern int             wtstcarry(void);
extern WORD56          wX_asl(WORD56 val, int shift);
extern WORD56          winsert_l(WORD56 a, int b);
extern WORD56          winsert_h(WORD56 a, int b);
extern int             wextract_l(WORD56 a);
extern int             wextract_h(WORD56 a);


extern CPPCALL AUDIO_API_IMPORT int             waddsat(int a, int b);
extern unsigned int		winsertu(int a,int b,int  r2l);


#define cmoveifcarry(a, b)	(a = _Fcarry ? (b) : (a))

#ifdef ARM
/* some bit-true functionalities are ported on 32-bit ARM
 * for ease of portability and efficient than x86 implementation.
 * some simple functions are replaceed with Macros and defined 
 * in cortexa9_common.h */
int wextract_arm(int a,int b);

int wextractu_arm(int a,int b);

unsigned int winsertu_arm(int a,int b,int  r2l);

int winsert_arm(int a,int b,int  r2l);

int	 wlsr_arm(int a,int b);

int  wextract_l_arm(WORD56 a);

#endif // ARM

#endif /* MMDSP */

#endif /* _btrue_h_ */

/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   tstiir.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
//#define DEBUGIIR

#define DOUBLE_PRECISION_FLOAT
#define FORMAT_FLOAT_WITH_DOUBLES

#include "audiolibs_common.h"
#include "resample_local.h"
#ifdef INTERP6
#include "resample_iircoef_6.c"
#include "testval_iircoef_I_6.c"
#define FS 48.0
#endif
#ifdef INTERP3
#include "resample_iircoef_3.c"
#include "testval_iircoef_I_3.c"
#define FS 24.0
#endif
#ifdef INTERP2
#include "resample_iircoef_2.c"
#include "testval_iircoef_I_2.c"
#define FS 16.0
#endif
#ifdef DECIM6
#include "resample_iircoef_6.c"
#include "testval_iircoef_D_6.c"
#define FS 48.0
#endif
#ifdef DECIM3
#include "resample_iircoef_3.c"
#include "testval_iircoef_D_3.c"
#define FS 48.0
#endif
#ifdef DECIM2
#include "resample_iircoef_2.c"
#include "testval_iircoef_D_2.c"
#define FS 48.0
#endif
#include <stdio.h>
#include <stdlib.h>
#define numcells 5
void runbiq(void);
Float out[N_OUT];
int iter,numiter,phase,interp,cell,ratio,shiftout;
Float kout,val,D[4*numcells];
Float YMEM coef_tab[numcells*5+2];
Float const YMEM *coefptr,*outptr;
Float *inptr,*resptr,coefatten;
char *msg;
#ifdef MMDSP
	Float acc0;
	Word56 acc;
#else
	FILE *f1;
	Float acc;
#endif
#ifdef __flexcc2__
	FILE *f2;
	float test_time;
#endif
unsigned long curr_count,new_count,cycle_count;

int main(void)
{
/*----------------- INIT ---------------------*/
#ifdef INTERP6
	interp=1;
	ratio=6;
	coefptr=resample_iircoef_6;
	inptr=in_iir_I_6;
	outptr=out_iir_I_6;
	coefatten=FORMAT_FLOAT(1.000000000000000,MAXVAL);
	msg="interp by 6";
#endif
#ifdef INTERP3
	interp=1;
	ratio=3;
	coefptr=resample_iircoef_3;
	inptr=in_iir_I_3;
	outptr=out_iir_I_3;
	coefatten=FORMAT_FLOAT(1.000000000000000,MAXVAL);
	msg="interp by 3";
#endif
#ifdef INTERP2
	interp=1;
	ratio=2;
	coefptr=resample_iircoef_2;
	inptr=in_iir_I_2;
	outptr=out_iir_I_2;
	coefatten=FORMAT_FLOAT(1.000000000000000,MAXVAL);
	msg="interp by 2";
#endif
#ifdef DECIM6
	interp=0;
	ratio=6;
	coefptr=resample_iircoef_6;
	inptr=in_iir_D_6;
	outptr=out_iir_D_6;
	coefatten=FORMAT_FLOAT(0.166666666666666,MAXVAL);
	msg="decim by 6";
#endif
#ifdef DECIM3
	interp=0;
	ratio=3;
	coefptr=resample_iircoef_3;
	inptr=in_iir_D_3;
	outptr=out_iir_D_3;
	coefatten=FORMAT_FLOAT(0.333333333333333,MAXVAL);
	msg="decim by 3";
#endif
#ifdef DECIM2
	interp=0;
	ratio=2;
	coefptr=resample_iircoef_2;
	inptr=in_iir_D_2;
	outptr=out_iir_D_2;
	coefatten=FORMAT_FLOAT(0.500000000000000,MAXVAL);
	msg="decim by 2";
#endif
	shiftout=coefptr[5*numcells];
	kout=coefptr[5*numcells+1];
	phase=0;
#ifdef MMDSP
	acc0=0; 
#endif
	for (iter=0;iter<4*numcells;iter++)
	{
		D[iter]=0;
	}
	if (interp==1)
	{
		numiter=N_OUT;
	}
	else
	{
		numiter=N_IN;
	}
	for (iter=0;iter<3;iter++)
	{
#ifdef MMDSP
		acc = wX_fmul(*coefptr,coefatten);
		coef_tab[iter]=waddr(acc,acc0);
#else
		coef_tab[iter]=(*coefptr)*coefatten;
#endif
		coefptr++;
	}
	for (iter=3;iter<5*numcells+2;iter++)
	{
		coef_tab[iter]=*coefptr++;
	}
	new_count=0L;
	curr_count=0L;
/*-------------- TEST -------------------------*/
	resptr=out;
	for (iter=0;iter<numiter;iter++)
	{
		runbiq();
		cycle_count=new_count-curr_count;
#ifdef DEBUGIIR
		printf("%s iter: %d cycles: %ld\n",msg,iter,cycle_count);
#endif
	}
/*------------------ VERIFY RESULTS -------------------------------------*/
#ifndef MMDSP
#define ONE_24_BIT_ERROR .000000119
#define MAXERROR ONE_24_BIT_ERROR

	f1=fopen("float_sig.txt","wt");
    for(iter=0;iter<N_OUT;iter++) 
	{
		fprintf(f1,"%e\n",out[iter]);
      	if( fabs(out[iter]-outptr[iter]) > MAXERROR ) 
		{
         	printf("error at # %d, diff %f \n",iter, out[iter]-outptr[iter]);
			return 1;
       	}
  	}
	fclose(f1);
#else
#define ONE_24_BIT_ERROR 1
#define MAXERROR 256*ONE_24_BIT_ERROR
   	for(iter=0;iter<N_OUT;iter++) 
	{
     	if( abs(out[iter]-outptr[iter]) > MAXERROR) 
		{
         	printf("error at # %d, diff %d \n",iter, out[iter]-outptr[iter]);
			return 1;
        }
   	}

#endif
#ifdef __flexcc2__
	test_time=numiter*1000./FS;
	f2=fopen("test_time","w t");
	printf("Test passed for %d samples.Total time in microseconds(denominator for total cost in cycles to mips conversion): %f.\n",numiter,test_time);
	fprintf(f2,"%f\n",test_time);
	fclose(f2);
#endif
	return 0;
} /* end main() */

void runbiq(void)
{
#ifdef DEBUGIIR
#ifdef MMDSP
		curr_count = get_cycle_count();
#endif
#endif
		if (interp!=0)
		{
			if (phase==0)
			{
				val=*inptr++;
				phase=ratio-1;
			}
			else
			{
				val=0;
				phase--;
			}
		}
		else
		{
			val=*inptr++;
		}
		for (cell=0;cell<(int)numcells;cell++)
		{
			/*------------- numerator mac -----------*/
#ifdef MMDSP
			acc=wL_imul(val,coef_tab[5*cell]);
			acc += wX_fmul(D[4*cell],coef_tab[5*cell+1]); /* use fractional multiplication to get 2*(1st order coef) */
			acc += wL_imul(D[4*cell+1],coef_tab[5*cell+2]);
#else
			acc=val*coef_tab[5*cell];
			acc+=D[4*cell]*coef_tab[5*cell+1]*2;
			acc+=D[4*cell+1]*coef_tab[5*cell+2];
#endif
			/*------------- denominator mac ---------*/
#ifdef MMDSP
			acc-= wX_fmul(D[4*cell+2],coef_tab[5*cell+3]); /* use fractional multiplication to get 2*(1st order coef) */
			acc-= wL_imul(D[4*cell+3],coef_tab[5*cell+4]);
#else
			acc-=D[4*cell+2]*coef_tab[5*cell+3]*2;
			acc-=D[4*cell+3]*coef_tab[5*cell+4];
#endif
			/*-------------- update delay line ------*/
			D[4*cell+1]=D[4*cell];
			D[4*cell]=val;
			D[4*cell+3]=D[4*cell+2];
#ifdef MMDSP
			acc=wX_msl(acc,1); /* double result to compensate for integer multiplication*/
			val=waddr(acc,acc0);
#else
			val=acc;
#endif
			D[4*cell+2]=val;
		}
		/*------------- process output sample --------*/
		if (interp!=0)
		{
#ifdef MMDSP
			acc=wX_fmul(val,kout);
			acc= wX_msl(acc, shiftout);
			*resptr++=waddr(acc,acc0);
#else
			acc=val*kout;
			acc = acc * (1<<shiftout);
			*resptr++=acc;
#endif
		}
		else
		{
			if (phase==0)
			{
				*resptr++=val;
				phase=ratio-1;
			}
			else
			{
				phase--;
			}
		}
		/*---------------------------------------------*/
#ifdef DEBUGIIR
#ifdef MMDSP
		new_count = get_cycle_count();
#endif
#endif
}


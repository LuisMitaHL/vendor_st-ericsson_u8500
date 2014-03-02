/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   tstiir1.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
//#define DEBUGIIR

#define DOUBLE_PRECISION_FLOAT
#define FORMAT_FLOAT_WITH_DOUBLES
#include "audiolibs_common.h"
#include "resample_local.h"
#ifdef INTERP10
#include "resample_iircoef_10.c"
#include "testval_iircoef_I_10.c"
#define FOUT 8.0
#endif
#ifdef DECIM10
#include "resample_iircoef_10.c"
#include "testval_iircoef_D_10.c"
#define FOUT 7.2
#endif
#include <stdio.h>
#include <stdlib.h>
#define numcells 5



int runbiq(void);
Float out[N_OUT];
int iter,numiter,phasein,phaseout,interp,cell,ratio,ratio1,shiftout,ret_val;
Float kout,kout_dec,val,D[4*numcells];
Float const YMEM *coefptr,*outptr;
Float *inptr,*resptr;
char *msg;
#ifdef MMDSP
	Float acc0;
	Word56 acc;
#else
	FILE *f1;
	Float acc;
#endif
#ifdef __flexcc2__
	float test_time;
	FILE *f2;
#endif
unsigned long curr_count,new_count,cycle_count;

int main(void)
{
/*----------------- INIT ---------------------*/
	ratio=10;
	ratio1=9;
#ifdef INTERP10
	interp=1;
	coefptr=resample_iircoef_10;
	inptr=in_iir_I_10;
	outptr=out_iir_I_10;
	msg="interp by 10";
#endif
#ifdef DECIM10
	interp=0;
	coefptr=resample_iircoef_10;
	inptr=in_iir_D_10;
	outptr=out_iir_D_10;
	msg="decim by 10";
#endif
	shiftout=coefptr[5*numcells];
	kout=coefptr[5*numcells+1];
	kout_dec=coefptr[5*numcells+2];
	phasein=0;
	phaseout=0;
#ifdef MMDSP
	acc0=0; 
#endif
	for (iter=0;iter<4*numcells;iter++)
	{
		D[iter]=0;
	}
	numiter=N_OUT;
	new_count=0L;
	curr_count=0L;
/*-------------- TEST -------------------------*/
	resptr=out;
	iter=0;
	while (iter<numiter)
	{
		if (runbiq()) iter++;
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
         	printf("error at # %d, diff: %f \n",iter, out[iter]-outptr[iter]);
			return 1;
       	}
  	}
	fclose(f1);
#else
#define ONE_24_BIT_ERROR 1

// fixed point result  on 16 msb's 

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
	test_time=N_OUT*1000./FOUT;
	f2=fopen("test_time","w t");
	printf("Test passed for %d output samples.Total time in microseconds(denominator for total cost in cycles to mips conversion): %f.\n",N_OUT,test_time);
	fprintf(f2,"%f\n",test_time);
	fclose(f2);
#endif
	return 0;
} /* end main() */

int runbiq(void)
{
#ifdef DEBUGIIR
#ifdef MMDSP
		curr_count = get_cycle_count();
#endif
#endif
		if (interp!=0)
		{
			if (phasein==0)
			{
				val=*inptr++;
				phasein=ratio-1;
			}
			else
			{
				val=0;
				phasein--;
			}
		}
		else
		{
			if (phasein==0)
			{
				val=*inptr++;
				phasein=ratio1-1;
			}
			else
			{
				val=0;
				phasein--;
			}
		}
		for (cell=0;cell<(int)numcells;cell++)
		{
			/*------------- numerator mac -----------*/
#ifdef MMDSP
			acc=wL_imul(val,coefptr[5*cell]);
			acc += wX_fmul(D[4*cell],coefptr[5*cell+1]); /* use fractional multiplication to get 2*(1st order coef) */
			acc += wL_imul(D[4*cell+1],coefptr[5*cell+2]);
#else
			acc=val*coefptr[5*cell];
			acc+=D[4*cell]*coefptr[5*cell+1]*2;
			acc+=D[4*cell+1]*coefptr[5*cell+2];
#endif
			/*------------- denominator mac ---------*/
#ifdef MMDSP
			acc-= wX_fmul(D[4*cell+2],coefptr[5*cell+3]); /* use fractional multiplication to get 2*(1st order coef) */
			acc-= wL_imul(D[4*cell+3],coefptr[5*cell+4]);
#else
			acc-=D[4*cell+2]*coefptr[5*cell+3]*2;
			acc-=D[4*cell+3]*coefptr[5*cell+4];
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
			if (phaseout==0)
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
				phaseout=ratio1-1;
				ret_val=1;
			}
			else
			{
				phaseout--;
				ret_val=0;
			}
		}
		else
		{
			if (phaseout==0)
			{
#ifdef MMDSP
				acc=wX_fmul(val,kout_dec);
				acc= wX_msl(acc, shiftout);
				*resptr++=waddr(acc,acc0);
#else
				acc=val*kout_dec;
				acc = acc * (1<<shiftout);
				*resptr++=acc;
#endif
				phaseout=ratio-1;
				ret_val=1;
			}
			else
			{
				phaseout--;
				ret_val=0;
			}
		}
		/*---------------------------------------------*/
#ifdef DEBUGIIR
#ifdef MMDSP
		new_count = get_cycle_count();
#endif
#endif
		return(ret_val);
}


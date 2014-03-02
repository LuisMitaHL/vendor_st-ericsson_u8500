/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   diagnostic.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
#include "resample_local.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef MMDSP
#if !defined(ARM) && !defined(NEON)
#define USE_MMDSP_INTRINSICS
#endif
#endif
#ifdef USE_MMDSP_INTRINSICS
#define SHIFT1 16
#define SHIFT2 8
#define SHIFT3 0
#else
#define SHIFT1 24
#define SHIFT2 16
#define SHIFT3 8
#endif
#include "vsrc1_table1.c"
#include "vsrc1_table2.c"
#define MAXVALUE 0x7fffffff
#define NCOEF1 80
#define NCOEF2 350
#define INTERPRATIO1 2
#define INTERPRATIO2 25
#define NTAPS1 NCOEF1/INTERPRATIO1
#define NTAPS2 NCOEF2/INTERPRATIO2
#define USEORD3
#ifdef USEORD3
#define ORD 3
#else
#define ORD 2
#endif

#define ONE_HALF 4194304
#define ONE 8388607
#define BLOCKSIZE 64
typedef struct 
{
	int convcnt;
	int M1;
	int L1;
	int offset1;
	int ntaps1;
	int D1index;
	int D1[NTAPS1];
	int M2;
	int L2;
	int offset2;
	int ntaps2;
	int D2index;
	int D2[NTAPS2];
	Float y[ORD+1];
	int alpha;
} Vsrc1Context;

void init_vsrc1(Vsrc1Context *ctx);
void vsrc1(int dalpha, int *indata, int *consumed, int *outdata ,Vsrc1Context *ctx);
int poly3(Float *ypts,Float x);
int poly2a(Float *ypts,Float x);

int main(int argc, char *argv[])
{
	//------------------ standalone resources --------
	FILE *f1,*f2;
	int i,itemp;
	float ppm,fsin,fsout;
	unsigned int tmp;
	int indata[BLOCKSIZE+1],outdata[BLOCKSIZE];
	int strtsmpl=0;
	//------------- arguments ----------------------------
	int dalpha;
	int consumed;
	//------------ structure ------------------------------
	Vsrc1Context ctx;
	//------------ parse ---------------------------------
	if (argc<4)
	{
  		printf("\n\nuse: vsrc1 INFILE OUTFILE fsout \n");
		printf("-------------------------------------------------------------------------------------------------\n");
		printf("Run the variable sample rate converter\n\n");
		printf("where:\n");
		printf("        INFILE= 24-bit input binary 48000Hz sample file name\n");
		printf("        OUTFILE= 24-bit output binary fsout Hz sample file name\n");
  		printf("        fsout=output frequency in Hz\n");
		printf("        for reference, 48048Hz=+1000 ppm and 47952=-1000 ppm\n");
  		exit(1);
	}
	fsin=48000;
	fsout=atoi(argv[3]);
	ppm=1e6*(fsout-fsin)/fsin;
	printf("ppm; %f\n",ppm);
	if ((f1=fopen(argv[1],"rb"))==0)
	{
  		printf("\n\n********** BAD INPUT FILE NAME  **********\n\n");
  		exit(1);
	}
	f2=fopen(argv[2],"wb");

	//-------------- init ------------------------
	consumed=BLOCKSIZE+1;
	dalpha=(int)((-(float)INTERPRATIO1*(float)INTERPRATIO2*ppm/1e6)*ONE);
	init_vsrc1(&ctx);
	//------------------- loop -----------------------------
	while (1) // main loop 
	{
		//--------------------- read sample file ----------
		if (consumed==BLOCKSIZE+1) 
		{
			strtsmpl=0;
		}
		if (consumed==BLOCKSIZE)
		{
			indata[0]=indata[BLOCKSIZE];
			strtsmpl=1;
		}
		if (consumed<BLOCKSIZE)
		{
			indata[0]=indata[BLOCKSIZE-1];
			indata[1]=indata[BLOCKSIZE];
			strtsmpl=2;
		}
		for (i=strtsmpl;i<BLOCKSIZE+1;i++)
		{
			if (feof(f1)) break;
			tmp=fgetc(f1);
			itemp = tmp<<SHIFT1;
			tmp=fgetc(f1);
			itemp += (tmp<<SHIFT2);
			tmp=fgetc(f1);
			itemp += (tmp<<SHIFT3);
			indata[i]=itemp;
		}
		if (feof(f1)) break;
		//-------------------- generate vsrc1 --------------
		vsrc1(dalpha, indata, &consumed, outdata, &ctx);
		//------------------- write sample file ------------
		for (i=0;i<BLOCKSIZE;i++)
		{
			putc(outdata[i]>>SHIFT1,f2);
			putc(outdata[i]>>SHIFT2,f2);
			putc(outdata[i]>>SHIFT3,f2);
		}
	}
	fclose(f1);
	fclose(f2);
	return 0;
}

void init_vsrc1(Vsrc1Context *ctx)
{
	int i;
	ctx->M1=INTERPRATIO1;
	ctx->L1=1;
	ctx->M2=INTERPRATIO2;
	ctx->L2=INTERPRATIO1*INTERPRATIO2;
	ctx->ntaps1=NTAPS1;
	ctx->ntaps2=NTAPS2;
	ctx->alpha=0;
	for (i=0;i<(ORD+1);i++)
		ctx->y[i]=0;
	for (i=0;i<NTAPS1;i++)
		ctx->D1[i]=0;
	for (i=0;i<NTAPS2;i++)
		ctx->D2[i]=0;
	ctx->convcnt=0;
	ctx->offset1=0;
	ctx->offset2=0;
	ctx->D1index=0;
	ctx->D2index=0;
}
#ifdef MMDSP
void vsrc1(int dalpha, int *indata, int *consumed, int *outdata, Vsrc1Context *ctx)
{
	int done,i,nbconv,enbl_interp,iter/*,pos,neg,incr*/;
	int nxtsamp=0,generated=0;
	const YMEM Float *Cfptr,*Cbptr;
	WORD56 accu;
	int   *baseAddress1 = &(ctx->D1[0]);
	int   *baseAddress2 = &(ctx->D2[0]);
#ifndef __flexcc2__
	int *p_val1f;
	int *p_val2f;
	int *p_valb;
#else
	int __circ *p_val1f;
	int __circ *p_val2f;
	int __circ *p_valb;
#endif
	int *max_adr1 = &baseAddress1[ctx->ntaps1];
	int *min_adr1 = baseAddress1;
	int *max_adr2 = &baseAddress2[ctx->ntaps2];
	int *min_adr2 = baseAddress2;
	//--------------- restore circular pointers ---
#ifndef __flexcc2__
	p_val1f = baseAddress1;
	p_val1f += ctx->D1index;
	p_val2f = baseAddress2;
	p_val2f += ctx->D2index;
#else
	p_val1f = winit_circ_ptr(
			&baseAddress1[ctx->D1index],        /* dummy value, this will be initialized later */
			min_adr1,    /* minimum value           */
			max_adr1     /* maximum value +1        */
			);
	p_val2f = winit_circ_ptr(
			&baseAddress2[ctx->D2index],        /* dummy value, this will be initialized later */
			min_adr2,    /* minimum value           */
			max_adr2     /* maximum value +1        */
			);
#endif
	//--------------- calculate 1 output sample -------
	*consumed=0;
	while (generated<BLOCKSIZE) //------- 2nd stage call -------
	{
		if (ctx->offset2<ctx->M2)
		{
			nbconv=wmin((ctx->M2-ctx->offset2),ORD+1)-ctx->convcnt;
			enbl_interp=wmax((nbconv+ctx->convcnt-ORD),0);
			for (iter=0;iter<nbconv;iter++)
			{
				//----- convolution for ORD+1 x50 samples using the symmetrical coefs---
				accu=0;
				Cfptr=&vsrc1_table2[ctx->offset2];
				Cbptr=&vsrc1_table2[ctx->M2-ctx->offset2-1];
#ifdef __flexcc2__
				p_valb = winit_circ_ptr(
						&baseAddress2[ctx->D2index],        /* dummy value, this will be initialized later */
						min_adr2,    /* minimum value           */
						max_adr2     /* maximum value +1        */
						);
				p_valb=wset_circ_addr(p_valb, wget_circ_addr(p_val2f));
#else
				p_valb=p_val2f;
#endif
				p_valb=mod_add(p_valb,ctx->ntaps2-1,max_adr2,min_adr2);

				for (i=0;i<ctx->ntaps2/2;i++)
				{
					accu=wX_add(accu,wX_fmul(*p_val2f,*Cfptr));
					accu=wX_add(accu,wX_fmul(*p_valb,*Cbptr));
					Cfptr+=ctx->M2;
					Cbptr+=ctx->M2;
					p_val2f = mod_add(p_val2f, 1, max_adr2, min_adr2);
					p_valb = mod_add(p_valb, -1, max_adr2, min_adr2);
				}
				p_val2f = mod_add(p_val2f,(-(ctx->ntaps2/2)), max_adr2, min_adr2);
				ctx->y[ctx->convcnt]=wround_X(accu);
				ctx->offset2++;
				ctx->convcnt++;
			}
			for (iter=0;iter<enbl_interp;iter++)
			{
				ctx->convcnt=0;
				ctx->offset2+=ctx->L2-(ORD+1);
				//----------------- interpolate ORD+1 x50 samples ----------
#ifdef USEORD3
				outdata[generated]=poly3(ctx->y,ctx->alpha+ONE_HALF);
#else
				outdata[generated]=poly2a(ctx->y,ctx->alpha+ONE_HALF);
#endif

				generated++;
#if 0
 				!!!!!!!! no MIPS saved here !!!!!!!!!
				ctx->alpha+=dalpha;
				pos=wmsr(waddsat(0x400000,wneg(ctx->alpha)),23);
				neg=wmsr(waddsat(0x400000,ctx->alpha),23);
				ctx->alpha=wadd(ctx->alpha,wand(neg,0x7fffff));
				ctx->alpha=wsub(ctx->alpha,wand(pos,0x7fffff));
				incr=wsub(wand(pos,1),wand(neg,1));
				ctx->offset2+=incr;	
#else
				//--------------- update sampling phase -----------
				ctx->alpha+=dalpha;
				if (ctx->alpha>ONE_HALF)
				{
					ctx->alpha-=ONE;
					ctx->offset2+=1;
				}
				else if (ctx->alpha<-ONE_HALF)
				{
					ctx->alpha+=ONE;
					ctx->offset2-=1;
				}
#endif			
			}
		}
		else
		{
			//--------- update 2nd stage delay line with one 1st stage output sample ---
			done=0;
			while (done==0)
			{
				if (ctx->offset1<ctx->M1)
				{
					//--------- convolution for 1 x2 samples 
					accu=0;
					Cfptr=&vsrc1_table1[ctx->offset1];
					Cbptr=&vsrc1_table1[ctx->M1-ctx->offset1-1];
#ifdef __flexcc2__
					p_valb = winit_circ_ptr(
						&baseAddress1[ctx->D1index],        /* dummy value, this will be initialized later */
						min_adr1,    /* minimum value           */
						max_adr1     /* maximum value +1        */
					);
					p_valb=wset_circ_addr(p_valb, wget_circ_addr(p_val1f));
#else
					p_valb=p_val1f;
#endif
					p_valb=mod_add(p_valb,ctx->ntaps1-1,max_adr1,min_adr1);
					for (i=0;i<ctx->ntaps1/2;i++)
					{
						accu=wX_add(accu,wX_fmul(*p_val1f,*Cfptr));
						accu=wX_add(accu,wX_fmul(*p_valb,*Cbptr));
						Cfptr+=ctx->M1;
						Cbptr+=ctx->M1;
						p_val1f = mod_add(p_val1f, 1, max_adr1, min_adr1);
						p_valb = mod_add(p_valb, -1, max_adr1, min_adr1);
					}
					p_val1f = mod_add(p_val1f,(-(ctx->ntaps1/2)), max_adr1, min_adr1);
					nxtsamp=wround_X(accu);
					ctx->offset1+=ctx->L1;
					done=1;
				}
				else
				{
					//-------- update 1st stage delay line with one input sample
					nxtsamp=indata[*consumed];
					(*consumed)++;
					p_val1f = mod_add(p_val1f, -1, max_adr1, min_adr1);
					*p_val1f = nxtsamp;
					ctx->offset1-=ctx->M1;
				}
			}// while (done==0)
			p_val2f = mod_add(p_val2f, -1, max_adr2, min_adr2);
			*p_val2f = nxtsamp;
			ctx->offset2-=ctx->M2;
		} //if (offset2<M2)
	} //while (generated<BLOCKSIZE)
	//--------------- store circular pointers ---
	ctx->D1index = p_val1f - baseAddress1;
	ctx->D2index = p_val2f - baseAddress2;
}
int poly3(Float *ypts,Float x)
{
	//function y=poly3(ypts,x);
	//3rd order polynomial calculation approximation for y=f(x) with x input in range 0..3
	//ypts is the vector containing 4 known values of y at abcisseses 0:3 respectively 
	int y,A,B,C,D,L,M,l0,l1,l2,l3;
	WORD56 accu;

	A=wsub(wfmul(0x2aaaaa,x),0x555555);
	B=wsub(x,0x7fffff);
	C=wsub(wfmul(0x400000,x),0x7fffff);
	D=wsub(wfmul(0x2aaaaa,x),0x7fffff);
	L=wround_X(wX_msl(wX_fmul(B,x),2));
	M=wfmul(C,D);
	l0=wneg(wfmul(B,M));
	l1=wround_X(wX_msl(wX_fmul(x,M),1));
	l2=wfmul(D,L);
	l3=wfmul(A,L);
	accu=wX_fmul(ypts[0],l0);
	accu=wX_add(accu,wX_msr(wX_fmul(ypts[1],l1),1));
	accu=wX_add(accu,wX_msr(wX_fmul(ypts[1],l1),1));
	accu=wX_add(accu,wX_msr(wX_fmul(ypts[1],l1),1));
	accu=wX_sub(accu,wX_msr(wX_fmul(ypts[2],l2),3));
	accu=wX_sub(accu,wX_msr(wX_fmul(ypts[2],l2),3));
	accu=wX_sub(accu,wX_msr(wX_fmul(ypts[2],l2),3));
	accu=wX_add(accu,wX_msr(wX_fmul(ypts[3],l3),3));
	y=wround_X(accu);
	return y;
}

int poly2a(Float *ypts,Float x)
{
	Float B,C,l0,l1,l2,y;
	WORD56 accu;
	//------------- Lagrange ------------
	B=     	wsub(x,0x7fffff);
	C=  	wsub(wfmul(0x400000,x),0x7fffff);
	l0=     wfmul(B,C);
	l1=		wround_X(wX_msl(wX_fmul(x,C),1));
	l2=		wround_X(wX_msl(wX_fmul(x,B),2));
	accu=wX_sub(wX_fmul(ypts[0],l0),wX_sub(wX_fmul(ypts[1],l1),wX_msr(wX_fmul(ypts[2],l2),3)));
	y=wround_X(accu);
	return (int)y;
}

#else //#ifdef MMDSP
void vsrc1(int dalpha, int *indata, int *consumed, int *outdata, Vsrc1Context *ctx)
{
	int done,i,D2fptr,Dbptr,D1fptr,Cfptr,Cbptr;
	int minadr1,maxadr1,minadr2,maxadr2,nxtsamp=0,generated=0;
	double accu;
	minadr1=0;
	maxadr1=ctx->ntaps1-1;
	minadr2=0;
	maxadr2=ctx->ntaps2-1;
	//--------------- restore circular pointers ---
	D1fptr=ctx->D1index;
	D2fptr=ctx->D2index;
	//--------------- calculate 1 output sample -------
	*consumed=0;
	while (generated<BLOCKSIZE) //------- 2nd stage call -------
	{
		if (ctx->offset2<ctx->M2)
		{
			//----- convolution for 1 x50 samples using the symmetrical coefs---
			accu=0;
			Cfptr=ctx->offset2;
			Cbptr=ctx->M2-ctx->offset2-1;
			Dbptr=D2fptr+ctx->ntaps2-1; if (Dbptr>maxadr2) Dbptr=minadr2+(Dbptr-maxadr2)-1; //circular arithmetic
			for (i=0;i<ctx->ntaps2/2;i++)
			{
				accu+=ctx->D2[D2fptr]*vsrc1_table2[Cfptr];
				accu+=ctx->D2[Dbptr]*vsrc1_table2[Cbptr];
				Cfptr+=ctx->M2;
				Cbptr+=ctx->M2;
				D2fptr++; if (D2fptr>maxadr2) D2fptr=minadr2; //circular arithmetic
				Dbptr--; if (Dbptr<minadr2) Dbptr=maxadr2; //circular arithmetic
			}
			D2fptr-=ctx->ntaps2/2; if (D2fptr<minadr2) D2fptr=maxadr2-(minadr2-D2fptr)+1; //circular arithmetic
			ctx->y[ctx->convcnt]=accu;
			ctx->offset2++;
			ctx->convcnt++;
			if (ctx->convcnt==(ORD+1))
			{
				ctx->convcnt=0;
				ctx->offset2+=ctx->L2-(ORD+1);
				//----------------- interpolate ORD+1 x50 samples ----------
#ifdef USEORD3
				outdata[generated]=poly3(ctx->y,((float)(ctx->alpha+ONE_HALF)/(float)ONE));
#else
				outdata[generated]=poly2a(ctx->y,((float)(ctx->alpha+ONE_HALF)/(float)ONE));
#endif
				generated++;
				//--------------- update sampling phase -----------
				ctx->alpha+=dalpha;
				if (ctx->alpha>ONE_HALF)
				{
					ctx->alpha-=ONE;
					ctx->offset2+=1;
				}
				else if (ctx->alpha<-ONE_HALF)
				{
					ctx->alpha+=ONE;
					ctx->offset2-=1;
				}
			}
		}
		else
		{
			//--------- update 2nd stage delay line with one 1st stage output sample ---
			done=0;
			while (done==0)
			{
				if (ctx->offset1<ctx->M1)
				{
					//--------- convolution for 1 x2 samples 
					accu=0;
					Cfptr=ctx->offset1;
					Cbptr=ctx->M1-ctx->offset1-1;
					Dbptr=D1fptr+ctx->ntaps1-1; if (Dbptr>maxadr1) Dbptr=minadr1+(Dbptr-maxadr1)-1; //circular arithmetic
					for (i=0;i<ctx->ntaps1/2;i++)
					{
						accu+=ctx->D1[D1fptr]*vsrc1_table1[Cfptr];
						accu+=ctx->D1[Dbptr]*vsrc1_table1[Cbptr];
						Cfptr+=ctx->M1;
						Cbptr+=ctx->M1;
						D1fptr++; if (D1fptr>maxadr1) D1fptr=minadr1; //circular arithmetic
						Dbptr--; if (Dbptr<minadr1) Dbptr=maxadr1; //circular arithmetic
					}
					D1fptr-=ctx->ntaps1/2; if (D1fptr<minadr1) D1fptr=maxadr1-(minadr1-D1fptr)+1; //circular arithmetic
					nxtsamp=(int)accu;
					ctx->offset1+=ctx->L1;
					done=1;
				}
				else
				{
					//-------- update 1st stage delay line with one input sample
					nxtsamp=indata[*consumed];
					(*consumed)++;
					D1fptr--; if (D1fptr<minadr1) D1fptr=maxadr1; //circular arithmetic
					ctx->D1[D1fptr]=nxtsamp;
					ctx->offset1-=ctx->M1;
				}
			}// while (done==0)
			D2fptr--; if (D2fptr<minadr2) D2fptr=maxadr2; //circular arithmetic
			ctx->D2[D2fptr]=nxtsamp;
			ctx->offset2-=ctx->M2;
		} //if (offset2<M2)
	} //while (generated<BLOCKSIZE)
	//--------------- store circular pointers ---
	ctx->D1index=D1fptr;
	ctx->D2index=D2fptr;
}
int poly3(Float *ypts,Float x)
{
	//function y=poly3(ypts,x);
	//3rd order polynomial calculation approximation for y=f(x) with x input in range 0..3
	//ypts is the vector containing 4 known values of y at abcisseses 0:3 respectively 

	Float A,B,C,D,L,M,l0,l1,l2,l3,y,y0,y1,y2,y3;
	y3=ypts[3];
	y2=ypts[2];
	y1=ypts[1];
	y0=ypts[0];
	//---------- Lagrange -----------------
	A=   .333333333*x  -  .6666666667;
	B=              x  -  1;
	C=           .5*x  -  1;
	D=     .3333333*x  -  1;
	L=          4*B*x;
	M=            C*D;

	l0=          -B*M;
	l1=         2*x*M;
	l2=           D*L;
	l3=           A*L;
	y = y0*l0 + y1*l1/2 + y1*l1/2 + y1*l1/2 -  y2*l2/8 - y2*l2/8 - y2*l2/8 +  y3*l3/8;
	return (int)y;
}

int poly2a(Float *ypts,Float x)
{
	Float B,C,l0,l1,l2,y,y0,y1,y2;
	y2=ypts[2];
	y1=ypts[1];
	y0=ypts[0];
	//------------- Lagrange ------------
	B=     x  -  1;
	C=  .5*x  -  1;

	l0=        B*C;
	l1=      2*x*C;
	l2=      4*x*B;
	y = y0*l0 - y1*l1 + y2*l2/8;
	return (int)y;
}
#endif //#ifdef MMDSP











































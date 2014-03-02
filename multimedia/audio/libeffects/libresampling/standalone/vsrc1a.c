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
#include "vsrc1_table.c"
#define MAXVALUE 0x7fffffff
#define NCOEF 1050
#define INTERPRATIO 25
#define NTAPS NCOEF/INTERPRATIO
#define ORD 3

#define ONE_HALF 4194304
#define ONE 8388607
#define BLOCKSIZE 64
typedef struct 
{
	int convcnt;
	int M;
	int L;
	int offset;
	int ntaps;
	int Dindex;
	int D[NTAPS];
	Float y[ORD+1];
	int alpha;
} Vsrc1Context;

void init_vsrc1(Vsrc1Context *ctx);
void vsrc1(int dalpha, int *indata, int *consumed, int *outdata ,Vsrc1Context *ctx);
int poly3(Float *ypts,Float x);


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
	dalpha=(int)((-(float)INTERPRATIO*ppm/1e6)*ONE);
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
	ctx->M=INTERPRATIO;
	ctx->L=INTERPRATIO;
	ctx->ntaps=NTAPS;
	ctx->alpha=0;
	for (i=0;i<(ORD+1);i++)
		ctx->y[i]=0;
	for (i=0;i<NTAPS;i++)
		ctx->D[i]=0;
	ctx->convcnt=0;
	ctx->offset=0;
	ctx->Dindex=0;
}
#ifdef MMDSP
void vsrc1(int dalpha, int *indata, int *consumed, int *outdata, Vsrc1Context *ctx)
{
	int i,nbconv,enbl_interp,iter,pos,neg,incr;
	int nxtsamp=0,generated=0;
	const YMEM Float *Cfptr,*Cbptr;
	WORD56 accu;
	int   *baseAddress = &(ctx->D[0]);
#ifndef __flexcc2__
	int *p_valf;
	int *p_valb;
#else
	int __circ *p_valf;
	int __circ *p_valb;
#endif
	int *max_adr = &baseAddress[ctx->ntaps];
	int *min_adr = baseAddress;
	//--------------- restore circular pointers ---
#ifndef __flexcc2__
	p_valf = baseAddress;
	p_valf += ctx->Dindex;
#else
	p_valf = winit_circ_ptr(
			&baseAddress[ctx->Dindex],        
			min_adr,    /* minimum value           */
			max_adr     /* maximum value +1        */
			);
			p_valb = winit_circ_ptr(
				&baseAddress[ctx->Dindex],       
				min_adr,    /* minimum value           */
				max_adr     /* maximum value +1        */
			);
#endif
	//--------------- calculate 1 output sample -------
	*consumed=0;
	while (generated<BLOCKSIZE) 
	{
		if (ctx->offset<ctx->M)
		{

			nbconv=wmin((ctx->M-ctx->offset),ORD+1)-ctx->convcnt;
			enbl_interp=wmax((nbconv+ctx->convcnt-ORD),0);	
			for (iter=0;iter<nbconv;iter++)
			{
				//----- convolution for 4 x25 samples using the symmetrical coefs---
				accu=0;
				Cfptr=&vsrc1_table[ctx->offset];
				Cbptr=&vsrc1_table[ctx->M-ctx->offset-1];
#ifdef __flexcc2__
				p_valb=wset_circ_addr(p_valb, wget_circ_addr(p_valf));
#else
				p_valb=p_valf;
#endif
				p_valb=mod_add(p_valb,ctx->ntaps-1,max_adr,min_adr);
				for (i=0;i<ctx->ntaps/2;i++)
				{
					accu=wX_add(accu,wX_fmul(*p_valf,*Cfptr));
					accu=wX_add(accu,wX_fmul(*p_valb,*Cbptr));
					Cfptr+=ctx->M;
					Cbptr+=ctx->M;
					p_valf = mod_add(p_valf, 1, max_adr, min_adr);
					p_valb = mod_add(p_valb, -1, max_adr, min_adr);
				}
				p_valf = mod_add(p_valf,(-(ctx->ntaps/2)), max_adr, min_adr);
				ctx->y[ctx->convcnt]=wround_X(accu);
				ctx->offset++;
				ctx->convcnt++;
			}
			for (iter=0;iter<enbl_interp;iter++)
			{
				ctx->convcnt=0;
				ctx->offset+=ctx->L-(ORD+1);
				outdata[generated]=poly3(ctx->y,ctx->alpha+ONE_HALF);
				generated++;
				//--------------- update sampling phase -----------

				ctx->alpha+=dalpha;
				pos=wmsr(waddsat(0x400000,wneg(ctx->alpha)),23);
				neg=wmsr(waddsat(0x400000,ctx->alpha),23);
				ctx->alpha=wadd(ctx->alpha,wand(neg,0x7fffff));
				ctx->alpha=wsub(ctx->alpha,wand(pos,0x7fffff));
				incr=wsub(wand(pos,1),wand(neg,1));

/*				
				if (ctx->alpha>ONE_HALF)
				{
					ctx->alpha-=ONE;
					ctx->offset+=1;
				}
				else if (ctx->alpha<-ONE_HALF)
				{
					ctx->alpha+=ONE;
					ctx->offset-=1;
				}
*/
				ctx->offset+=incr;
			}
		}
		else
		{
			//-------- update delay line with one input sample
			nxtsamp=indata[*consumed];
			(*consumed)++;
			p_valf = mod_add(p_valf, -1, max_adr, min_adr);
			*p_valf = nxtsamp;
			ctx->offset-=ctx->M;
		} //if (offset<M)
	} //while (done==0)
	//--------------- store circular pointers ---
	ctx->Dindex = p_valf - baseAddress;
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
#else //#ifdef MMDSP
void vsrc1(int dalpha, int *indata, int *consumed, int *outdata, Vsrc1Context *ctx)
{
	int i,Dfptr,Dbptr,Cfptr,Cbptr;
	int minadr,maxadr,nxtsamp=0,generated=0;
	double accu;
	minadr=0;
	maxadr=ctx->ntaps-1;
	//--------------- restore circular pointers ---
	Dfptr=ctx->Dindex;
	*consumed=0;

	while (generated<BLOCKSIZE) 
	{
		if (ctx->offset<ctx->M)
		{
			//----- convolution for 1 x25 samples using the symmetrical coefs---
			accu=0;
			Cfptr=ctx->offset;
			Cbptr=ctx->M-ctx->offset-1;
			Dbptr=Dfptr+ctx->ntaps-1; if (Dbptr>maxadr) Dbptr=minadr+(Dbptr-maxadr)-1; //circular arithmetic
			for (i=0;i<ctx->ntaps/2;i++)
			{
				accu+=ctx->D[Dfptr]*vsrc1_table[Cfptr];
				accu+=ctx->D[Dbptr]*vsrc1_table[Cbptr];
				Cfptr+=ctx->M;
				Cbptr+=ctx->M;
				Dfptr++; if (Dfptr>maxadr) Dfptr=minadr; //circular arithmetic
				Dbptr--; if (Dbptr<minadr) Dbptr=maxadr; //circular arithmetic
			}
			Dfptr-=ctx->ntaps/2; if (Dfptr<minadr) Dfptr=maxadr-(minadr-Dfptr)+1; //circular arithmetic
			ctx->y[ctx->convcnt]=accu;
			ctx->offset++;
			ctx->convcnt++;
			if (ctx->convcnt==(ORD+1))
			{
				ctx->convcnt=0;
				ctx->offset+=ctx->L-(ORD+1);
				//----------------- interpolate ORD+1 x50 samples ----------
				outdata[generated]=poly3(ctx->y,((float)(ctx->alpha+ONE_HALF)/(float)ONE));
				generated++;
				//--------------- update sampling phase -----------
				ctx->alpha+=dalpha;
				if (ctx->alpha>ONE_HALF)
				{
					ctx->alpha-=ONE;
					ctx->offset+=1;
				}
				else if (ctx->alpha<-ONE_HALF)
				{
					ctx->alpha+=ONE;
					ctx->offset-=1;
				}
			}
		}
		else
		{
			//--------- update  stage delay line with one input ---
			nxtsamp=indata[*consumed];
			(*consumed)++;
			Dfptr--; if (Dfptr<minadr) Dfptr=maxadr; //circular arithmetic
			ctx->D[Dfptr]=nxtsamp;
			ctx->offset-=ctx->M;
		} //if (offset<M)
	} //while (generated<BLOCKSIZE)
	//--------------- store circular pointers ---
	ctx->Dindex=Dfptr;
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
#endif //#ifdef MMDSP











































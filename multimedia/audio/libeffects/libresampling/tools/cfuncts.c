/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   cfuncts.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cfuncts.h"
/**************************************************************************************/
int	fftcalc(float *in,float	*out,int ord)
{
	int	NPASS,inptr,outptr,tempin,i,nset,nbut,pass,setno,butter,indxa,indxb;
	float theta,Wr,Wi,*inr,*ini,*outr,*outi,tempra,tempia,temprb,tempib;
	inr=in;ini=in+ord;outr=out;outi=out+ord;
	/*---------------- Bit Reverse -----------------------*/
	NPASS=(int)(log10((float)ord)/log10(2.));
	for	(inptr=0;inptr<ord;inptr++)
	{
		tempin=inptr;
		outptr=0;
		for	(i=0;i<NPASS;i++)
		{
			outptr=(2*outptr)+tempin%2;
			tempin/=2;
		}
		inr[outptr+ord]=inr[inptr];
	}
	for	(i=0;i<ord;i++)
	{
		inr[i]=inr[i+ord];
		ini[i]=0.0;
	}
	/*-------------- Radix 2 FFT -----------------------*/
	nset=ord/2;
	nbut=1;
	for	(pass=1;pass<=NPASS;pass++)
	{
		for	(setno=1;setno<=nset;setno++)
		{
			for	(butter=0;butter<nbut;butter++)
			{
				theta=((float)(nset*butter))*pi/((float)(ord/2));
				Wr=cos(theta);Wi=-sin(theta);
				indxa=butter+((setno-1)*2*nbut);indxb=indxa+nbut;
				tempra=inr[indxa]+Wr*inr[indxb]-Wi*ini[indxb];
				tempia=ini[indxa]+Wr*ini[indxb]+Wi*inr[indxb];
				temprb=inr[indxa]-Wr*inr[indxb]+Wi*ini[indxb];
				tempib=ini[indxa]-Wr*ini[indxb]-Wi*inr[indxb];
				outr[indxa]=tempra;
				outi[indxa]=tempia;
				outr[indxb]=temprb;
				outi[indxb]=tempib;
			}
		}
		nbut*=2;nset/=2;
		for	(i=0;i<ord;i++)
		{
			inr[i]=outr[i];
			ini[i]=outi[i];
		}
	}
	return(0);
}
int	chebwin(int	npts,double	mindB,double *coefs)
{
	double p[npts];
	double *cc,x,xi,f;
	double normval,dp,c1,c2,df,x0,alpha,beta;
	int	i,j,n,xn,nf,inptr,outptr,iter;
	cc=coefs;
	nf=npts;
	dp=pow(10.,(-mindB/20.));
	if ((nf%2)==0) nf++;
	n=((nf+1)/2);
	xn=nf-1;
	c1=acosh((1.+dp)/dp);
	c2=cosh(c1/((double)xn));
	df=acos(1./c2)/pi;
	x0=(3.-cos(2.*pi*df))/(1.+cos(2.*pi*df));
	alpha=(1.+x0)/2.;
	beta=(x0-1.)/2.;
	c2=xn/2.;
	for	(i=0;i<nf;i++)
	{
		xi=(double)i;
		f=xi/nf;
		x=beta+alpha*cos(2.*pi*f);
		if (fabs(x)>1.0)
		{
			p[i]=dp*cosh(c2*acosh(x));
		}
		else
		{
			p[i]=dp*cos(c2*acos(x));
		}
	}
	for	(i=0;i<nf;i++)
	{
		cc[i]=0.0;
		for	(j=0;j<nf;j++)
		{
			cc[i]=cc[i]+p[j]*cos(-2.0*pi*i*j/nf);
		}
	}
	normval=cc[0];
	inptr=n-1;
	outptr=nf-1;
	for	(iter=0;iter<n;iter++)
	{
		cc[outptr]=cc[inptr]/normval;
		inptr--;
		outptr--;
	}
	inptr=nf-1;
	outptr=0;
	for	(iter=0;iter<n-1;iter++)
	{
		cc[outptr]=cc[inptr];
		inptr--;
		outptr++;
	}
	if (cc[0]>cc[1]) cc[0]=cc[1];
	if (cc[npts-1]>cc[npts-2]) cc[npts-1]=cc[npts-2];
	return(0);
}
int weight(float fs,int type,int numcoef,float *coefs)
{
#define arraysiz 7
#define MINFREQ 10.0
	int numpts,i,j,degree,fftsiz;
	float *ifftin,*ifftout,*tempcoef,freqindex,freq,tmp,xferfunct[arraysiz][2],indx[arraysiz];
	double logval[arraysiz],logindx[arraysiz],p[arraysiz];
	fftsiz=numcoef+1;
	if (type==0)
	{
    	//ANSI A Weighting
		xferfunct[0][0]=12.0;
		xferfunct[0][1]=-40.0;
		xferfunct[1][0]=100.0;
		xferfunct[1][1]=-20.0;
		xferfunct[2][0]=1000.0;
		xferfunct[2][1]=0.0;
		xferfunct[3][0]=10000.0;
		xferfunct[3][1]=-2.0;
		xferfunct[4][0]=20000.0;
		xferfunct[4][1]=-9.0;
		xferfunct[5][0]=100000.0;
		xferfunct[5][1]=-35.0;
		numpts=6;
	}
	else if (type==1)
	{
    	//ANSI C Weighting
		xferfunct[0][0]=12.0;
		xferfunct[0][1]=-3.5;
		xferfunct[1][0]=100.0;
		xferfunct[1][1]=-1.0;
		xferfunct[2][0]=1000.0;
		xferfunct[2][1]=0.0;
		xferfunct[3][0]=10000.0;
		xferfunct[3][1]=-5.0;
		xferfunct[4][0]=20000.0;
		xferfunct[4][1]=-12.0;
		xferfunct[5][0]=100000.0;
		xferfunct[5][1]=-37.0;
		numpts=6;
	}
	else if (type==2)
	{
    	//CCIR468-4 Weighting
		xferfunct[0][0]=20.0;
		xferfunct[0][1]=-35.0;
		xferfunct[1][0]=50.0;
		xferfunct[1][1]=-27.0;
		xferfunct[2][0]=100.0;
		xferfunct[2][1]=-20.0;
		xferfunct[3][0]=1000.0;
		xferfunct[3][1]=0.0;
		xferfunct[4][0]=7000.0;
		xferfunct[4][1]=10.0;
		xferfunct[5][0]=10000.0;
		xferfunct[5][1]=7.0;
		xferfunct[6][0]=20000.0;
		xferfunct[6][1]=-20.0;
		numpts=7;
	}
	for (i=0;i<numpts;i++)
	{
    	tmp=xferfunct[i][0];
    	indx[i]=tmp;
    	logval[i]=xferfunct[i][1]/20.0;
		logindx[i]=log10(indx[i]);
	}
	degree=polyfit(numpts,logindx,logval,numpts-1,p);
	ifftin=coefs;
	ifftout=ifftin;
	for (i=0;i<fftsiz/2;i++)
	{
		freq=fs*((float)i)/(float)(fftsiz);
		if (freq<MINFREQ) freq=MINFREQ;
		freqindex=log10(freq);
		ifftin[i]=pow(10.0,(float)(polyval(p,(double)freqindex,degree)));
	}
	j=(fftsiz/2)-1;
	ifftin[fftsiz/2]=0.0;
	for (i=(fftsiz/2)+1;i<fftsiz;i++)
	{
		ifftin[i]=ifftin[j];
		j--;
	}
	fftcalc(ifftin,ifftout,fftsiz);
	tempcoef=&coefs[fftsiz];
	for (i=0;i<(fftsiz/2)-1;i++)
	{
		*tempcoef=ifftout[i+(fftsiz/2)+1]/(float)fftsiz;
		tempcoef++;
	}
	for (i=0;i<fftsiz/2;i++)
	{
		*tempcoef=ifftout[i]/(float)fftsiz;
		tempcoef++;
	}
	tempcoef=&coefs[fftsiz];
	for (i=0;i<fftsiz-1;i++)
	{
		coefs[i]=tempcoef[i];
	}
	return(0);
}

int	polyfit(int	npts,double	*x,	double *y, int order, double *p)
{
	int	odegre=0;
	double contrl=100.;
	double sumsq[23],aa[23],bb[23];
	if (npts>5000)
	{
		printf(" number	of points must be <	5000\n");
	}
	else
	{
		if (order >	20)
		{
			printf(" order must	not	exceed 20\n");
		}
		else
		{
			if (order>=npts)
			{
				printf(" order must	be less	than number	of points\n");
			}
			else
			{
				odegre=fit1(x,y,npts,contrl,order,p,sumsq,aa,bb);
				if (odegre>0) fit2(p,odegre,aa,bb);
			}
		}
	}
	return(odegre);
}
int	fit1(double	*xx,double *yy,int num,double contrl,int idegre,double *coefs,double *sumsqs,double	*aa,double *bb)
{
	int	error=0;
	int	odegre;
	int	i,ideg1,ii,i1,i2,jj,kk,kl,ll,nn;
	double rr=0.0,ss,tt,uu,vv,ww,zz;
	double x1,x2,x3,x4,y1,y2;
	double work[2*WORKSIZ];
	double xxx[NUM1];
	for	(i=0;i<num;i++)
		xxx[i]=xx[i];
	if ((idegre<num)&&(idegre>=1))
	{
		if (contrl>0.0)
		{
			if (contrl>100.0)
			{
				error=4;
				contrl=100.0;
			}
			if (idegre>20) error=5;
			x4=xxx[1-1];
			x2=xxx[1-1];
			nn=1;
			for	(jj=2;jj<=num;jj++)
			{
				if (yy[jj-1-1]==yy[jj-1]) nn++;
				if (xxx[jj-1]>x4) x4=xxx[jj-1];
				if (xxx[jj-1]<x2) x2=xxx[jj-1];
			}
			x1=x2;
			x3=x4;
			if ((x1!=x3)&&(nn!=num))
			{
				y1=(x1+x3)*.5;
				y2=4.0/(x3-x1);
				i1=1;
				i2=2;
				for	(jj=1;jj<=num;jj++)
				{
					work[jj-1+((i1-1)*WORKSIZ)]=1.0;
					ss=y2*(xxx[jj-1]-y1);
					rr+=ss;
					xxx[jj-1]=ss;
				}
				rr/=num;
				aa[1-1]=rr;
				bb[1-1]=0.0;
				tt=0.0;
				for	(jj=1;jj<=num;jj++)
				{
					tt+=yy[jj-1];
					work[jj-1+((i2-1)*WORKSIZ)]=xxx[jj-1]-rr;
				}
				x3=0.0;
				for	(jj=1;jj<=num;jj++)
				{
					x3+=work[jj-1+((i2-1)*WORKSIZ)]*work[jj-1+((i2-1)*WORKSIZ)];
				}
				uu=0.0;
				for	(jj=1;jj<=num;jj++)
				{
					uu+=yy[jj-1]*yy[jj-1];
				}
				sumsqs[1-1]=uu;
				ww=contrl*.01;
				zz=0.0;
				sumsqs[2-1]=tt*tt/num;
				uu-=sumsqs[2-1];
				rr=(double)num;
				coefs[1-1]=tt/num;
				ideg1=idegre+1;
				odegre=1;
				tt=0.0;
				for	(jj=1;jj<=num;jj++)
				{
					tt+=work[jj-1+((i2-1)*WORKSIZ)]*yy[jj-1];					
				}
				coefs[2-1]=tt/x3;
				if (ideg1>=2)
				{
					for	(jj=2;jj<=ideg1;jj++)
					{
						kk=jj+1;
						sumsqs[kk-1]=tt*tt/x3;
						zz+=sumsqs[kk-1];
						uu-=sumsqs[kk-1];
						if (uu<=0.0) uu=0.0;
						if ((zz/(sumsqs[1-1]-sumsqs[2-1]))>ww) break;
						if (jj==ideg1) break;
						odegre=jj;
						vv=0.0;
						for	(ll=1;ll<=num;ll++)
						{
							vv+=xxx[ll-1]*work[ll-1+((i2-1)*WORKSIZ)]*work[ll-1+((i2-1)*WORKSIZ)];
						}
						vv/=x3;
						x1=x3/rr;
						for	(ll=1;ll<=num;ll++)
						{
							work[ll-1+((i1-1)*WORKSIZ)]=
								(xxx[ll-1]-vv)*work[ll-1+((i2-1)*WORKSIZ)]-
									(x1*work[ll-1+((i1-1)*WORKSIZ)]);
						}
						aa[jj-1]=vv;
						bb[jj-1]=x1;
						rr=x3;
						tt=0.0;
						for	(ll=1;ll<=num;ll++)
						{
							tt+=work[ll-1+((i1-1)*WORKSIZ)]*yy[ll-1];
						}
						x3=0.0;
						for	(kl=1;kl<=num;kl++)
						{
							x3+=work[kl-1+((i1-1)*WORKSIZ)]*work[kl-1+((i1-1)*WORKSIZ)];
						}
						coefs[kk-1]=tt/x3;
						ii=i1;
						i1=i2;
						i2=ii;
/*--75--*/
					}
				}
/*--80--*/
				sumsqs[idegre+3-1]=uu;
				coefs[odegre+2-1]=y2;
				coefs[odegre+3-1]=-y2*y1;
			}
			else
			{
				error=2;
			}
		}
		else
		{
			error=3;
		}
	}
	else
	{
		error=1;
	}
/*--90--*/	
	if (error!=0)
	{
		printf("****** Error Message %d	******\n",error);
		switch(error)
		{
			case 1:
				printf(" not enough	data points\n");
				break;
			case 2:
				printf(" the x or y	vector was a constant\n");
				break;
			case 3:
				printf(" the contrl	variable was < 0\n");
				break;
			case 4:
				printf(" the contrl	variable was > 100.0\n");
				break;
			case 5:
				printf(" the desired degree	of fit is >	20\n");
				break;
		}
		return(0);
	}
	else
	{
		return(odegre);
	}
}
int	fit2(double	*coefs,int odegre,double *aa,double	*bb)
{
	int	odeg1,ii,jj,kk,ll,mm;
	double alpha,beta,cc,ee,ff,gg,pp,qq;
	double dd[4][21];
	odeg1=odegre+1;
	for	(jj=1;jj<=odeg1;jj++)
	{
		dd[4-1][jj-1]=coefs[jj-1];
		for	(ll=1;ll<=3;ll++)
		{
			dd[ll-1][jj-1]=0.0;
		}
/*--5--*/
	}
	for	(jj=2;jj<=odeg1;jj++)
	{
		dd[2-1][jj-1]=1.0;
		ll=jj-1;
		alpha=aa[ll-1];
		beta=bb[ll-1];
		for	(ii=2;ii<=jj;ii++)
		{
			mm=ii-1;
			dd[3-1][ii-1]=dd[2-1][mm-1]-(dd[2-1][ii-1]*alpha)-(beta*dd[1-1][ii-1]);
			dd[4-1][mm-1]=dd[4-1][mm-1]+(coefs[jj-1]*dd[3-1][ii-1]);
		}
		if (jj==odeg1) break;
		for	(ii=1;ii<=jj;ii++)
		{
			dd[1-1][ii-1]=dd[2-1][ii-1];
			dd[2-1][ii-1]=dd[3-1][ii-1];
		}
/*--15--*/
	}
/*--20--*/
	beta=coefs[odegre+2-1];
	ff=1.0;
	gg=coefs[odegre+3-1];
	cc=1.0;
	qq=1.0;
	pp=1.0;
	for	(kk=1;kk<=odeg1;kk++)
	{
		ee=ff*dd[4-1][kk-1];
		ll=kk+1;
		if (ll<=odeg1)
		{
			if (kk!=1)
			{
				pp=kk;
				qq=1.0;
			}
			for	(jj=ll;jj<=odeg1;jj++)
			{
				cc=gg*cc*pp/qq;
				ee=ee+dd[4-1][jj-1]*cc;
				if (kk!=1)
				{
					pp+=1.0;
					qq+=1.0;
				}
/*--30--*/
			}
		}
/*--35--*/
		coefs[kk-1]=ee;
		ff*=beta;
		cc=ff;
/*--40--*/
	}
	return(0);
}
double polyval(double *p, double x,	int	order)
{

	double value=0.0;
	double temp=1.0;
	int	i;
	for	(i=0;i<=order;i++)
	{
		value+=p[i]*temp;
		temp*=x;
	}
	return(value);
}
int	pfit(double	ratio,int ntaps,int	order ,double mindB, double	*pval)
{
	double tnorm;
	double *padr;
	double DALPHA=1./((double)NUM1-1);
	double alpha[NUM1],wval[NUM1],h[NUM1];
	double k[ntaps],p[order+5];
	int	i,j,L,foundorder;
	int	LTAPS=(ntaps-1)/2;
	padr=pval;
	for	(i=0;i<NUM1;i++)
		alpha[i]=-.5+DALPHA*i;
	chebwin(ntaps,mindB,k);
	for	(L=-LTAPS;L<=0;L++)
	{
		evalwin(k,L+LTAPS,alpha,wval,ntaps,NUM1);
		for	(j=0;j<NUM1;j++)
		{
			tnorm=L+alpha[j];
			if (tnorm==0.0)	tnorm=Epsilon;
			h[j]=wval[j]*((2.*ratio)*sin(2.*pi*ratio*(tnorm)))/(2.*pi*ratio*(tnorm));
		}
		foundorder=polyfit(NUM1,alpha,h,order,p);
		for	(i=0;i<=order;i++) 
		{
			if (i>foundorder) p[i]=0.0;
			*padr=p[i];
			padr++;
		}
	}
	return(0);
}
int	evalwin(double *wintab,int coefindx,double *alphatab,double	*interpolatedwinvalues,int ntaps,int nalpha)
{
	int	delta=(NCOFS-1)/2;
	int	pord,xmin,xmax,i,j,foundorder;
	double *xval,*kval,xtemp;
	double p[NCOFS+4];
	xval=interpolatedwinvalues;
	kval=interpolatedwinvalues+NCOFS;
	pord=NCOFS-1;
	if (coefindx<=((ntaps+1)/2))
	{
		xmin=1;
		if ((coefindx-delta)>1)	xmin=coefindx-delta;
		xmax=xmin+NCOFS-1;
	}
	else
	{
		xmax=ntaps;
		if (ntaps<(coefindx+delta))	xmax=coefindx+delta;
		xmin=xmax-NCOFS+1;
	}
	i=0;
	for	(j=xmin;j<=xmax;j++)
	{
		kval[i]=wintab[j];
		i++;
	}
	i=0;
	for	(j=-delta;j<=delta;j++)
	{
		xval[i]=j;
		i++;
	}
	foundorder=polyfit(NCOFS,xval,kval,pord,p);
	for	(i=0;i<nalpha;i++)
	{
		xtemp=coefindx-xmin-delta+alphatab[i];
		interpolatedwinvalues[i]=polyval(p,xtemp,foundorder);
		if (interpolatedwinvalues[i]<0.0) interpolatedwinvalues[i]=0.0;
	}
	return(0);
}
int	sinxcalc(double	*pval, double alpha, int order,	int	ntaps, double *coefs)
{
	double val;
	double value[order+1];
	double evensum;
	double oddsum;
	int	i,j,indx;
	val=1.0;
	for (i=0;i<=order;i++)
	{
		value[i]=val;
		val*=alpha;
	}
	for	(j=1;j<=(ntaps-1)/2;j++)
	{
		evensum=0.0;
		indx=((j-1)*(order+1));
		for	(i=0;i<=order;i+=2)
		{
			evensum+=pval[indx+i]*value[i];
		}
		oddsum=0.0;
		for	(i=1;i<=order;i+=2)
		{
			oddsum+=pval[indx+i]*value[i];
		}
		coefs[j-1]=evensum+oddsum;
		coefs[ntaps-j]=evensum-oddsum;
	}
	coefs[((ntaps+1)/2)-1]=0.0;
	for	(i=0;i<=order;i++)
	{
		coefs[((ntaps+1)/2)-1]+=pval[((((ntaps+1)/2)-1)*(order+1))+i]*value[i];
	}
	return(0);
}
int	coefcalc(double	ratio,int ntaps,double mindB, double *coefs)
{
	double tnorm;
	double *k;
	int	L;
	int	LTAPS=(ntaps-1)/2;
	k=coefs;
	chebwin(ntaps,mindB,k);
	for	(L=-LTAPS;L<=LTAPS;L++)
	{
		tnorm=L;
		if (tnorm==0.0)	tnorm=Epsilon;
		coefs[L+LTAPS]=k[L+LTAPS]*((2.*ratio)*sin(2.*pi*ratio*(tnorm)))/(2.*pi*ratio*(tnorm));
	}
	return(0);
}
int	srcinit(float xMINDB,float xpassratiopoly,float	fs,float fso,int INTAPS,
	int	IMAXPHASE,int iNTAPSsinx,int idegree,double	*srcbulk,srcvar	*SRC)
{
	double passratiof,passratiosinx;
	int	i,NTAPSfinal,INTAPSfinaltot,num;
	INTAPSfinaltot=INTAPS*(IMAXPHASE+1);
	num=0;
	SRC->KF=&srcbulk[num];
	num+=INTAPSfinaltot;
	SRC->polydlyline=&srcbulk[num];
	num+=INTAPSfinaltot;
	SRC->outdlyline=&srcbulk[num];
	num+=iNTAPSsinx;
	SRC->p=&srcbulk[num];
	SRC->fs=(double)fs;
	SRC->fso=(double)fso;
	SRC->iNTAPSsinx=iNTAPSsinx;
	SRC->idegree=idegree;
	if (SRC->fs>SRC->fso)
	{
		SRC->decimratio= SRC->fs/SRC->fso;
		SRC->interpratio=1;
		NTAPSfinal=INTAPS*SRC->fs/SRC->fso;
		passratiof=xpassratiopoly*SRC->fso/SRC->fs;
		passratiosinx=xpassratiopoly;
		SRC->filtergain=1.0;
	}
	else
	{
		SRC->decimratio=1;
		SRC->interpratio=1+(SRC->fso/SRC->fs);
		NTAPSfinal=INTAPS*SRC->interpratio;
		passratiof=xpassratiopoly/SRC->interpratio;
		passratiosinx=.3;
		SRC->filtergain=(float)SRC->interpratio;
	}
	pfit(passratiosinx,iNTAPSsinx,idegree,xMINDB,SRC->p);
	NTAPSfinal+=(NTAPSfinal%2)+1;
	coefcalc(passratiof,NTAPSfinal,xMINDB,SRC->KF);
	SRC->NTAPSperphase=(NTAPSfinal/SRC->interpratio)+1;
	for	(i=NTAPSfinal;i<INTAPSfinaltot;i++)
	{
		SRC->KF[i]=0.0;
	}
	SRC->modulo_incntr=0;
	SRC->modulo_outcntr=0;
	SRC->polyptr=0;
	for	(i=0;i<SRC->NTAPSperphase;i++)
		SRC->polydlyline[i]=0.0;
	SRC->smplrdy=0;
	SRC->tin=0.0;
	SRC->tout=0.0;
	SRC->outptr=0;
	for	(i=0;i<iNTAPSsinx;i++)
		SRC->outdlyline[i]=0.0;
	return(0);
}
int	src(double *insmpl,int inblksiz,double *outsmpl,int outblksiz,int	*nbgenerated,srcvar	*SRC)
{
	int	i,j,phase,nbconsumed,breakit;
	double alpha,sigout;
	double XK[SRC->iNTAPSsinx];
	nbconsumed=0;*nbgenerated=0;breakit=0;
	while (1)
	{
		while (SRC->smplrdy!=0)
		{
			if (SRC->tin>=SRC->tout)
			{
				if (*nbgenerated==outblksiz)
				{ 
					breakit=1;
					break;
				}
				alpha=(SRC->fs*(SRC->tout-SRC->tin)/SRC->decimratio)+.5;
				sinxcalc(SRC->p	,alpha,SRC->idegree,SRC->iNTAPSsinx,XK);
				sigout=0.0;
				j=0;
				for	(i=SRC->outptr;i<SRC->iNTAPSsinx;i++)
				{
					sigout+=SRC->outdlyline[i]*XK[j];
					j++;
				}
				for	(i=0;i<SRC->outptr;i++)
				{
					sigout+=SRC->outdlyline[i]*XK[j];
					j++;
				}
				SRC->tout+=	(SRC->interpratio/SRC->fso);
				outsmpl[*nbgenerated]=(float)sigout;
				(*nbgenerated)++;
			}
			else
			{
				SRC->smplrdy=0;
				SRC->outptr--;
				if (SRC->outptr<0) SRC->outptr=SRC->iNTAPSsinx-1;
				SRC->outdlyline[SRC->outptr]=SRC->polyphaseout;
				SRC->tin+=(SRC->decimratio/SRC->fs);
			}
			if (breakit==1)	break;
		}
		while (SRC->smplrdy==0)
		{
			if ((SRC->modulo_incntr*SRC->interpratio)>=(SRC->modulo_outcntr*SRC->decimratio))
			{
				phase=SRC->interpratio+(SRC->decimratio*SRC->modulo_outcntr) -
					(SRC->interpratio*SRC->modulo_incntr)-1;
				SRC->polyphaseout=0.0;
				j=0;
				for	(i=SRC->polyptr;i<SRC->NTAPSperphase;i++)
				{
					SRC->polyphaseout+=SRC->polydlyline[i]*SRC->KF[phase+(j*SRC->interpratio)];
					j++;
				}
				for	(i=0;i<SRC->polyptr;i++)
				{
					SRC->polyphaseout+=SRC->polydlyline[i]*SRC->KF[phase+(j*SRC->interpratio)];
					j++;
				}
				SRC->polyphaseout*=SRC->filtergain;
				SRC->modulo_outcntr++;
				SRC->smplrdy=1;
			}
			else
			{
				if (nbconsumed==inblksiz)
				{ 
					breakit=1;
					break;
				}
				SRC->polyptr--;
				if (SRC->polyptr<0) SRC->polyptr=SRC->NTAPSperphase-1;
				SRC->polydlyline[SRC->polyptr]=(double)(insmpl[nbconsumed]);
				SRC->modulo_incntr++;
				nbconsumed++;
			}
			if ((SRC->modulo_incntr>=SRC->decimratio)&&(SRC->modulo_outcntr>=SRC->interpratio))
			{
				SRC->modulo_incntr=0;
				SRC->modulo_outcntr=0;
			}
			if (breakit==1)	break;
		}
		if (breakit==1)	break;
	}
	return(nbconsumed);
}



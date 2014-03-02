/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   evaldist.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evaldist.h"
#define NUM_FREQ 2
#define MIN_THD 75.
#define FREQ_MIN 985.
#define FREQ_MAX 1015.
#define MIN_LVL .20
/*----------------------------------------------------*/
int	main(int argc, char	*argv[])
{
	FILE *fin;
	char *finspec,*filename;
	int	ibuf,retval=0;
	int	datavalid=1,i,j,indx,indx1,indx2,startindx;
	int	wgttype,endindx,curptr,numsmpls,fintype;
	int	existval,foundindx,numfreq;
	int	sigindx[NUMMAX];
	long iter,totsmpls=0L;
	float *coef,*dlyline,*dlyline1,*val,*fftout,*wgtcoef;
	float intemp,xtemp,xtemp1,flo,fhi,df,fs,f1,f2,signrg,noisnrg,thd,maxres,ampl,thresh,xmult;
	float fftin[2*fftsiz],fsig[NUMMAX];
	double winval[fftsiz-1];

	int OptionIndex=1;
	float xMIN_THD=MIN_THD,xFREQ_MIN=FREQ_MIN,xFREQ_MAX=FREQ_MAX,xMIN_LVL=MIN_LVL;
	int iNUM_FREQ=NUM_FREQ;
	xmult=pow(2.,31.)-1.0;
	wgttype=-1;
	if (argc<3)
	{
		printf("\nuse:evaldist SIGFILE FS [THRESHOLD[WEIGHT_TYPE[OPTIONS]]]\n");
		printf("      SIGFILE  =  32-bit binary (or floating text if name\n");
		printf("                  starts with float_)\n");
		printf("           FS  =  signal sampling frequency\n");
		printf("    THRESHOLD  =  auto detect signal threshold (def=.0001)\n");
		printf("                  this is the level of each freq found relative\n");
		printf("                  to the max found over the band\n");
		printf("  WEIGHT_TYPE  =  weighting if any (-1: NONE(def) 0:ANSI A 1:ANSI C 2:CCIR468-4)\n\n");
		printf("                         OPTIONS:\n\n");
		printf("------- The following are expected test parameter limits for evaldist to return 0, else return 1 --------\n");
		printf("--num_freq     =  value     max number of expected frequency components detected (def %d)\n",NUM_FREQ);
		printf("--min_thd      =  value     minimum thd value (def %f) \n",MIN_THD);
		printf("--freq_min     =  value     minimum 1st frequency component (def %f) \n",FREQ_MIN);
		printf("--freq_max     =  value     maximum 1st frequency component (def %f) \n",FREQ_MAX);
		printf("--min_lvl      =  value     minimum numerical value of all detected frequency components (def %f)\n",MIN_LVL);

		exit(1);
	}
	if ((argv[OptionIndex][0]=='f')&&(argv[OptionIndex][1]=='l')&&(argv[OptionIndex][2]=='o')&&(argv[OptionIndex][3]=='a')&&(argv[OptionIndex][4]=='t')&&(argv[OptionIndex][5]=='_'))
	{
		fintype=1;
		finspec="rt";
	}
	else	
	{
		fintype=0;
		finspec="rb";
	}
	filename=argv[OptionIndex];
	OptionIndex++;
	if ((fin=fopen(filename,finspec))==0)
	{
		printf("\n\n********** BAD INPUT FILE NAME **********\n\n");
		exit(1);
	}
	fs=atof(argv[OptionIndex]);
	OptionIndex++;
	if (argc>OptionIndex)
		thresh=atof(argv[OptionIndex]);
	else
		thresh=sigthresh;
	OptionIndex++;
	if (argc>OptionIndex)
	{
		wgttype=atoi(argv[OptionIndex]);
		if (wgttype>2) wgttype=2;
	}
	OptionIndex++;

/*------------------------------ Check Optional Arguments For Test Parameters ------------*/
	while (OptionIndex < argc) 
	{
		if (strncmp(argv[OptionIndex], "--num_freq=", 11) == 0) 
		{
			iNUM_FREQ = atoi(&argv[OptionIndex][11]);
		}
		if (strncmp(argv[OptionIndex], "--min_thd=", 10) == 0) 
		{
			xMIN_THD = atof(&argv[OptionIndex][10]);
		}
		if (strncmp(argv[OptionIndex], "--freq_min=", 11) == 0) 
		{
			xFREQ_MIN = atof(&argv[OptionIndex][11]);
		}
		if (strncmp(argv[OptionIndex], "--freq_max=", 11) == 0) 
		{
			xFREQ_MAX = atof(&argv[OptionIndex][11]);
		}
		if (strncmp(argv[OptionIndex], "--min_lvl=", 10) == 0) 
		{
			xMIN_LVL = atof(&argv[OptionIndex][10]);
		}
		OptionIndex++;
	}
/*------------------------------------------------------------------------------------------*/


	

	
	if (wgttype>=0)
	{
		numsmpls=fftsiz;
	}
	else
	{
		numsmpls=fftsiz/8;
	}
	df=8*fs/fftsiz;
	fftout=fftin;
	/*-------- find	fsigs -----------------------------*/
	dlyline=fftin+fftsiz;
	for	(i=0;i<fftsiz-1;i++)
		dlyline[i]=0;
	datavalid=1;
	curptr=0;
	ibuf=0;
	xtemp=ibuf/32767.;
	while (datavalid==1)
	{
		dlyline[curptr]=xtemp;
		curptr--;
		if (curptr<0)
			curptr=fftsiz-1;
		if (fintype==0)
		{
			datavalid=fread(&ibuf,4,1,fin);
			xtemp=ibuf/xmult;
		}
		else
		{
			datavalid=fscanf(fin,"%e\n",&xtemp);
		}
		totsmpls++;
	}
	totsmpls--;
	i=0;
	/*-------- Copy	tail of	input signal to	fft	input ---*/
	for	(j=curptr;j>=0;j--)
	{
		fftin[i]=dlyline[j];
		i++;
	}
	for	(j=fftsiz-1;j>curptr;j--)
	{
		fftin[i]=dlyline[j];
		i++;
	}
	rewind(fin);
	/*------------ Cheby Window	the	fft	input ---------------------*/
	chebwin(fftsiz-1,MINDBtest,winval);
	for	(i=0;i<fftsiz-1;i++)
		fftin[i]=fftin[i]*winval[i];
	fftin[fftsiz-1]=0.0;
	/*------------ Calculate the fft --------------------------------*/
	fftcalc(fftin,fftout,fftsiz);
	/*------------ Take	the	magnitude of fftout	up to fs/2 ----------*/
	for	(i=0;i<fftsiz/2;i++)
		fftout[i]=(4./fftsiz)*sqrt((fftout[i]*fftout[i])+(fftout[i+fftsiz]*fftout[i+fftsiz]));
	/*---------	Look for spectral lines	greater	than threshold -------*/
	maxres=0.0;
	for	(i=0;i<fftsiz/2;i++)
	{
		if (fftout[i]>maxres)
			maxres=fftout[i];
	}
	maxres*=thresh;
	existval=0;
	for	(i=0;i<fftsiz/2;i++)
	{
		if (fftout[i]>maxres)
			existval=1;
	}
	if (existval==0)
	{
		printf("\n\n********** NO SIGNAL FOUND **********\n\n\n");
		exit(1);
	}
	iter=0;
	/*---------	find signal	frequencies	-----------------*/
	while (existval>0)
	{
		numfreq=iter+1;
		if (numfreq>NUMMAX)
		{
			printf("\n\n********* SIGNAL TOO NOISY FOR FURTHER ANALYSIS *********\n\n\n");
			exit(1);
		}
		foundindx=0;
		startindx=0;
		i=0;
		/*------ look for rising edge of first spectral	line ----*/
		while ((foundindx==0)&&(i<fftsiz/2))
		{
			if (fftout[i]>maxres)
			{
				foundindx=1;
				startindx=i;
			}
			i++;
		}
		indx=startindx;
		/*------ look for falling edge of first	spectral line ---*/
		while ((fftout[indx]>maxres)&&(indx<fftsiz/2))
		{
			endindx=indx;
			indx++;
		}
		/*------ find center location of 1st set of	spectral lines -----------*/
		sigindx[iter]=(int)(.5+((float)(indx+startindx+1)/2.0));
		fsig[iter]=((float)((float)(sigindx[iter]-1)/fftsiz))*fs;
		ampl=0;
		/*-----	remove this	set	of spectral	lines from the total ----*/
		for	(i=startindx;i<=endindx;i++)
		{
			if (fftout[i]>ampl)
				ampl=fftout[i];
			fftout[i]=0;
		}
		printf("signal no: %d freq: %f Hz magnitude: %f freq/dfreq: %f \n",
				(int)iter+1,fsig[iter],ampl,(float)sigindx[iter]/(float)(endindx-startindx));
		if (ampl<xMIN_LVL) retval=1;
		if (iter>=iNUM_FREQ) retval=1;
		/*-----	continue if	there are any remaining	spectral lines ------*/
		iter++;
		existval=0;
		for	(i=0;i<fftsiz/2;i++)
		{
			if (fftout[i]>maxres)
				existval=1;
		}
	}
	/*-------- calculate the notch coefs --------------*/
	val=fftin+fftsiz;
	for	(i=0;i<fftsiz/2;i++)
		val[i]=1.0;
	/*-------- zero	out	the	xfer function around fsigs of each set of spectral lines ----*/
	for	(iter=0;iter<numfreq;iter++)
	{
		f1=fsig[iter]-df;
		f2=fsig[iter]+df;
		indx1=(int)(((f1/(fs/2.0))*(fftsiz/2))+0.5);
		indx2=(int)(((f2/(fs/2.0))*(fftsiz/2))+0.5);
		for	(i=indx1;i<=indx2;i++)
			val[i]=0.0;
	}
	/*-------- zero	out	the	xfer function from DC to flo --*/
	flo=20.0;
	indx1=0;
	indx2=(int)(((flo/(fs/2.0))*(fftsiz/2))+0.5);
	for	(i=indx1;i<=indx2;i++)
		val[i]=0.0;
	/*-------- zero	out	the	xfer function above	fhi	-------*/
	fhi=22000.0;
	indx1=(int)(((fhi/(fs/2.0))*(fftsiz/2))+0.5);
	indx2=(fftsiz/2)-1;
	for	(i=indx1;i<=indx2;i++)
		val[i]=0.0;
	/*-------- calculate the total fft input -------------*/
	for	(i=0;i<fftsiz/2;i++)
		fftin[i]=val[i];
	for	(i=fftsiz;i>(fftsiz/2);i--)
		fftin[i]=val[fftsiz-i];
	fftin[fftsiz/2]=0.0;
	/*---------	call the fft -----------------------------*/
	fftcalc(fftin,fftout,fftsiz);
	/*---------	scale the fft output ---------------------*/
	for	(i=0;i<fftsiz;i++)
		fftout[i]/=fftsiz;
	/*---------	re-arrange the coefficients	---------------*/
	j=1+(fftsiz/2);
	coef=fftin+fftsiz;
	for	(i=0;i<(fftsiz/2)-1;i++)
	{
		coef[i]=fftout[j];
		j++;
	}
	j=0;
	for	(i=(fftsiz/2)-1;i<fftsiz-1;i++)
	{
		coef[i]=fftout[j];
		j++;
	}
	/*-------- Chebwin of the coefficients ---------------*/ 
	for	(i=0;i<fftsiz-1;i++)
		coef[i]*=winval[i];
	/*---------- Calculate the weighting coefficients ----*/
	if (wgttype>=0)
	{
		wgtcoef=malloc(2*fftsiz*sizeof(float));
		weight(fs,wgttype,fftsiz-1,wgtcoef);
		dlyline1=&wgtcoef[fftsiz];
		for	(i=0;i<fftsiz-1;i++)
		{
			dlyline1[i]=0;
		}
	}
	/*------filter the input signal	thru the notch and weighting filter -------*/
	dlyline=fftin;
	for	(i=0;i<fftsiz-1;i++)
	{
		dlyline[i]=0;
	}
	curptr=0;
	datavalid=1;
	iter=0L;
	signrg=0.0;
	noisnrg=0.0;
	while (datavalid==1)
	{
		/*-------------- notch convolution ----------------*/
		if (((iter>=(totsmpls-numsmpls))&&(iter<totsmpls))||(wgttype>=0))
		{
			xtemp=0.0;
			j=0;
			for	(i=curptr;i<fftsiz-1;i++)
			{
				xtemp+=dlyline[i]*coef[j];
				j++;
			}
			for	(i=0;i<curptr;i++)
			{
				xtemp+=dlyline[i]*coef[j];
				j++;
			}
			if (wgttype<0)
			{
				noisnrg+=xtemp*xtemp;
				signrg+=dlyline[curptr]*dlyline[curptr];
			}
		}
		/*-------------- weight convolution ----------------*/
		if (wgttype>=0)
		{
			if ((iter>=(totsmpls-numsmpls))&&(iter<totsmpls))
			{
				xtemp1=0.0;
				j=0;
				for	(i=curptr;i<fftsiz-1;i++)
				{
					xtemp1+=dlyline1[i]*wgtcoef[j];
					j++;
				}
				for	(i=0;i<curptr;i++)
				{
					xtemp1+=dlyline1[i]*wgtcoef[j];
					j++;
				}
				noisnrg+=xtemp1*xtemp1;
				signrg+=dlyline[curptr]*dlyline[curptr];
			}
		}
		/*---------	update notch dly line 	----------------*/
		curptr--;
		if (curptr<0)
			curptr=fftsiz-2;
		if (fintype==0)
		{
			datavalid=fread(&ibuf,4,1,fin);
			intemp=ibuf/xmult;
		}
		else
			datavalid=fscanf(fin,"%e\n",&intemp);
		dlyline[curptr]=intemp;
		iter++;
		/*--------- update weigting filter dly line -------*/
		if (wgttype>=0)
		{
			dlyline1[curptr]=xtemp;
		}
	}
	/*-----	compare	the	signal and the filtered	signal ---*/
	thd=10.*log10((signrg/noisnrg)+1.0e-10);
	printf("THD: %f dB on %ld samples\n",thd,totsmpls);
	fclose(fin);
	if (wgttype>=0)
		free(wgtcoef);
	if (thd<xMIN_THD) retval=1;
	if (fsig[0] < xFREQ_MIN) retval=1;
	if (fsig[0] > xFREQ_MAX) retval=1;
	return(retval);
}



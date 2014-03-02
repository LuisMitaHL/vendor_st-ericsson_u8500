/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   evdist.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evaldist.h"
#define NUMFREQ 8
#define MAXCH 8
/*----------------------------------------------------*/
int	main(int argc, char	*argv[])
{
	FILE *fin,*ftemp;
	char *finspec,*foutspec;
	int	ibuf,nbytes=2;
	int	datavalid,i,j,indx,indx1,indx2,startindx;
	int	wgttype,endindx,curptr,numsmpls;
	int	existval,foundindx,numfreq;
	int	sigindx[NUMFREQ];
	long iter,totsmpls;
	float *coef,*dlyline,*dlyline1,*val,*fftout,*wgtcoef;
	float intemp,xtemp,xtemp1,flo,fhi,df,fs=44100.,f1,f2,signrg,noisnrg,thd,maxres,ampl,thresh,xmult;
	float fftin[2*fftsiz],fsig[NUMFREQ];
	double winval[fftsiz-1];
	long smplstart=0,smplstop=0x7fffffff;
	int use_all_of_file=1;
	int OptionIndex=1;
	unsigned int itemp;
	int nch=1,channelno,chno;


	char endieness[50],fintype[50];
	unsigned char cbuf;
	strcpy(endieness,"b");
	strcpy(fintype,"b");

	thresh=sigthresh;
	xmult=pow(2.,31.)-1.0;
	wgttype=-1;
	if (argc<2)
	{
		printf("\nuse: evdist [specifications] sigfile\n\n");
		printf("  sigfile = binary file containing samples to be evaluated\n\n");
		printf("SPECIFICATIONS:\n");
		printf("-------------------------------------------------------------------------------------\n");
		printf("--fs=value          value=sampling rate in Hz (def 44100)\n");
		printf("--thdtarget=value   value=signal det threshold in +dB(def 80) or fract(def.0001)\n");
		printf("--weight=value      value= (-1:NONE(def) 0:ANSI-A 1:ANSI_C 2:CCIR468-4\n");
		printf("--smplstart=value   value=signal evaluation start window in nsamples (def 0)\n");
		printf("--smplstop=value    value=signal evaluation stop window in nsamples (def all)\n");
		printf("--nbytes=value      value=number of bytes per sample (def 2)\n");
		printf("--endieness=value   value=endieness little(l) big(b) (def b)\n");
		printf("--itype=value       value=filetype floating text(f) binary(b) (def b)\n");
		printf("--nch=value         value=number of channels (def 1)\n");
		exit(1);
	}
	/*--------------- Parse command line ---------------------*/
	while (OptionIndex < argc) 
	{
		if (strncmp(argv[OptionIndex], "--fs=", 5) == 0) 
		{
			fs = atof(&argv[OptionIndex][5]);
		}
		else if (strncmp(argv[OptionIndex], "--thdtarget=", 12) == 0)
		{
			thresh=atof(&argv[OptionIndex][12]);
			if (thresh>1.0) thresh=pow(10.0,(-(thresh)/20.0));
		}
		else if (strncmp(argv[OptionIndex], "--weight=", 9) == 0)
		{
			wgttype=atoi(&argv[OptionIndex][9]);
			if ((wgttype>2)||(wgttype<-1))
			{
				printf("bad value for weight\n");
				exit(1);
			}
		}
		else if (strncmp(argv[OptionIndex], "--smplstart=", 12) == 0)
		{
			smplstart=atoi(&argv[OptionIndex][12]);
			use_all_of_file=0;
		}
		else if (strncmp(argv[OptionIndex], "--smplstop=", 11) == 0)
		{
			smplstop=atoi(&argv[OptionIndex][11]);
			use_all_of_file=0;
		}
		else if (strncmp(argv[OptionIndex], "--nbytes=", 9) == 0)
		{
			nbytes=atoi(&argv[OptionIndex][9]);
			if (nbytes>4)
			{
				printf("bad value for nbytes\n");
				exit(1);
			}
		}
		else if (strncmp(argv[OptionIndex], "--endieness=", 12) == 0) 
		{
			strcpy(endieness,argv[OptionIndex]+12);
		}
		else if (strncmp(argv[OptionIndex], "--itype=", 8) == 0) 
		{
			strcpy(fintype,argv[OptionIndex]+8);
		}
		else if (strncmp(argv[OptionIndex], "--nch=", 6) == 0) 
		{
			nch=atoi(argv[OptionIndex]+6);
		}
		OptionIndex++;
	}
	if (fintype[0]=='b')
	{
		finspec="rb";
		foutspec="wb";
	}
	else
	{
		finspec="rt";
		foutspec="wt";
	}
	printf("\n----------------------- evdist ------------------------------------------\n");
	for (chno=0;chno<nch;chno++)
	{
		datavalid=1;
		totsmpls=0L;
		/*---------------- Remove only channel to be analyzed ------------------------------------------------------*/
		if ((fin=fopen(argv[argc-1],finspec))==0)
		{
			printf("\n\n********** BAD INPUT FILE NAME **********\n\n");
			exit(1);
		}
		ftemp=fopen("tmpfile",foutspec);
		datavalid=1;
		while (datavalid==1)
		{
			for (channelno=0;channelno<nch;channelno++)
			{
				if (fintype[0]=='b')
				{
					for (i=0;i<nbytes;i++)
					{
						cbuf=fgetc(fin);
						if (feof(fin))
						{
							datavalid=0;
							break;
						}
						if (channelno==chno) fputc(cbuf,ftemp);
					}
				}
				else
				{
					datavalid=fscanf(fin,"%e\n",&xtemp);
					if (datavalid!=1) break;
					if (channelno==chno) fprintf(ftemp,"%e\n",xtemp);
				}
				if (datavalid!=1) break;
			}
		}
		fclose(fin);
		fclose(ftemp);
		fin=fopen("tmpfile",finspec);
		datavalid=1;
		/*----------------------------------------------------------------------------------------------------------*/
		if (use_all_of_file==0)
		{
			/*------------ Copy portion of input file to temporary file with zone to be analyzed for disto ---------*/
			ftemp=fopen("tmpfile1",foutspec);
			datavalid=1;
			totsmpls=0L;
			while (datavalid==1)
			{
				if (fintype[0]=='b')
				{
					for (i=0;i<nbytes;i++)
					{
						cbuf=fgetc(fin);
						if (feof(fin))
						{
							datavalid=0;
							break;
						}
						if ((totsmpls>=smplstart)&&(totsmpls<smplstop)) fputc(cbuf,ftemp);
					}
				}
				else
				{
					datavalid=fscanf(fin,"%e\n",&xtemp);
					if (datavalid!=1) break;
					if ((totsmpls>=smplstart)&&(totsmpls<smplstop)) fprintf(ftemp,"%e\n",xtemp);
				}
				if (datavalid!=1) break;
				totsmpls++;
			}
			fclose(fin);
			fclose(ftemp);
			fin=fopen("tmpfile1",finspec);
			datavalid=1;
			totsmpls=0L;
		}
		/*--------------------------------------------------------------------------------------------------*/
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
		xtemp=0.0;
		while (datavalid==1)
		{
			dlyline[curptr]=xtemp;
			curptr--;
			if (curptr<0)
				curptr=fftsiz-1;
			if (fintype[0]=='b')
			{
				itemp=0;
				for (i=0;i<nbytes;i++)
				{
					cbuf=fgetc(fin);
					if (endieness[0]=='l')
					{
						itemp+=cbuf<<((i+4-nbytes)*8);
					}
					else
					{
						itemp+=cbuf<<((3-i)*8);
					}
				}
				ibuf=(int)itemp;
				if (feof(fin)) datavalid=0;
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
			if (numfreq>NUMFREQ)
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
			if (fintype[0]=='b')
			{
				itemp=0;
				for (i=0;i<nbytes;i++)
				{
					cbuf=fgetc(fin);
					if (endieness[0]=='l')
					{
						itemp+=cbuf<<((i+4-nbytes)*8);
					}
					else
					{
						itemp+=cbuf<<((3-i)*8);
					}
				}
				ibuf=(int)itemp;
				if (feof(fin)) datavalid=0;
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
		printf("         CHANNEL %d THD is: %f dB on %ld samples\n",chno,thd,totsmpls);
		fclose(fin);
		if (wgttype>=0)
			free(wgtcoef);
//#ifdef UNIX
		system("rm -f tmpfile");
		if (use_all_of_file==0) system("rm -f tmpfile1");
//#else
//		system("del	tmpfile");
//		if (use_all_of_file==0) system("del	tmpfile1");
//#endif
	} //for (chno=0;chno<nch;chno++)
	printf("-------------------------------------------------------------------------\n\n");
	return(0);
}



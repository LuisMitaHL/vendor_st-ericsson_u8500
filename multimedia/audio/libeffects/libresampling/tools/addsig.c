/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   addsig.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define numfilesmax 10

int main(int argc, char *argv[] )
{
FILE *fin,*ftempr,*ftempw,*fout;
char *finspec,*foutspec;
int datavalid,numfiles,iter=0,toggle=0,fintype,fouttype;
int ibuf;
long npoints;
double sinavg,soutavg;
float xtemp,xtemp1,sigamp,xmult;
numfiles=0;
if (argc>1)
	numfiles=atoi(argv[1]);
if (argc!=numfiles+4)
 {
  printf("\n\n********* PARAMETER ERROR *********");
  printf("\n\nuse: addsig NUMFILES INFILE(1) INFILE(2) .. INFILE(NUMFILES) OUTFILE AMP \n\n");
  printf("        NUMFILES  = number of files containing signal samples to be added\n");
  printf("       INFILE(x)  = 32-bit binary (or floating if starts with float_) file \n");
  printf("         OUTFILE  = 32-bit binary (or floating if starts with float_) file\n");
  printf("             AMP  = desired output amplitude multiplier from 0 to 1.0\n");
  exit(1);
 }
if (numfiles>=numfilesmax)
{
	printf("\n****** TOO MANY INPUT FILES! ******\n");
	exit(1);
}
xmult=pow(2.,31.)-1.0;
sigamp=atof(argv[numfiles+3]);
while (iter<numfiles)
{
	if ((argv[2+iter][0]=='f')&&(argv[2+iter][1]=='l')&&(argv[2+iter][2]=='o')&&
		(argv[2+iter][3]=='a')&&(argv[2+iter][4]=='t')&&(argv[2+iter][5]=='_'))
	{
		fintype=1;
		finspec="rt";
	}
	else
	{
		fintype=0;
		finspec="rb";
	}
	if ((fin=fopen(argv[2+iter],finspec))==0)
 	{
  		printf("\n\n********** BAD INPUT FILE NAME  **********\n\n");
  		exit(1);
 	}
	if ((argv[numfiles+2][0]=='f')&&(argv[numfiles+2][1]=='l')&&(argv[numfiles+2][2]=='o')&&
		(argv[numfiles+2][3]=='a')&&(argv[numfiles+2][4]=='t')&&(argv[numfiles+2][5]=='_'))
	{
		fouttype=1;
		foutspec="wt";
	}
	else
	{
		fouttype=0;
		foutspec="wb";
	}
	fout=fopen(argv[numfiles+2],foutspec);
	if (toggle==0)
	{
		ftempw=fopen("temp0.txt","wt");
		if (iter>0) ftempr=fopen("temp1.txt","rt");
		toggle=1;
	}
	else
	{
		ftempw=fopen("temp1.txt","wt");
		if (iter>0) ftempr=fopen("temp0.txt","rt");
		toggle=0;
	}
	sinavg=0.0;
	soutavg=0.0;
	npoints=0L;
	datavalid=1;
	while (datavalid==1)
	{
		if (fintype==0)
		{
			datavalid=fread(&ibuf,4,1,fin);
			xtemp=ibuf/xmult;
		}
		else
			datavalid=fscanf(fin,"%e\n",&xtemp);
	 	if (iter>0) datavalid=fscanf(ftempr,"%e\n",&xtemp1);
		if (datavalid==1)
		{
			sinavg+=(xtemp*xtemp);
			if (iter>0) xtemp+=xtemp1;
			npoints++;
			soutavg+=(xtemp*xtemp*sigamp*sigamp);
			fprintf(ftempw,"%e\n",xtemp);
  			if (xtemp>=0)
  				ibuf=((sigamp*xtemp*xmult)+.5);
  			else
	  			ibuf=((sigamp*xtemp*xmult)-.5);
			if (fouttype==0)
  				fwrite(&ibuf,4,1,fout);
			else
				fprintf(fout,"%e\n",xtemp);
		}
	}
	sinavg/=npoints;
	sinavg=sqrt(sinavg);
	soutavg/=npoints;
	soutavg=sqrt(soutavg);
	printf(" Input signal %d level: %f\n",iter+1,sinavg);
	fclose(ftempw);
	if (iter>0) fclose(ftempr);
	fclose(fin);
	fclose(fout);
	iter++;
}
printf("-------------------------------\n");
printf("   Final output level: %f\n",soutavg);
system("rm temp0.txt");
if (iter>1) system("rm temp1.txt");
return 0;
}


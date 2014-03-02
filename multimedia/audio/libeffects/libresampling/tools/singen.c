/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   singen.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
//#include <dos.h>
#include <math.h>
//#include <process.h>
#include <stdlib.h>
#include <ctype.h>
//#include <io.h>
#include <string.h>

int main(int argc, char *argv[] )
{
FILE *fout;
char *foutspec,*cbuf;
int fspecerr,out_type,straight;
int ibuf;
long npoints,npoints_sil1,npoints_sil2,npoints_sig,iter;
float xtemp,fc,fs,AMP,pi,tsil1,tsil2,tsig,PHASE,xmult;
long double theta;
if (argc==9)
        straight=1;
else
        straight=0;
pi=3.1415927;
xmult=pow(2.,31.)-1.0;
theta=0.0;
cbuf=(char *) malloc(5);
fspecerr=0;
if ((argc!=9)&&(argc!=8))
 {
  printf("\n\n********* PARAMETER ERROR *********");
  printf("\n\n singen OUTFILE  fc fs AMP [PHASE] tsil1 tsig tsil2 \n\n");
  printf("      OUTFILE   = 32-bit binary (or floating text if name starts with float_) file containing output samples\n");
  printf("           fc   = desired frequency\n");
  printf("           fs   = desired sampling frequency\n");
  printf("          AMP   = desired signal amplitude when present in dBm\n");
  printf("        PHASE   = desired signal starting phase when present in dBm\n");
  printf("        tsil1   = desired number of seconds of initial silence\n");
  printf("         tsig   = desired number of seconds of signal presence\n");
  printf("        tsil2   = desired number of seconds of ending silence\n");
  exit(1);
 }
/*------------  INITIALIZATION -------------------------*/
if ((argv[1][0]=='f')&&(argv[1][1]=='l')&&(argv[1][2]=='o')&&(argv[1][3]=='a')&&(argv[1][4]=='t')&&(argv[1][5]=='_'))
{
	out_type=1;
	foutspec="wt";
}
else	
{
	out_type=0;
	foutspec="wb";
}

fout=fopen(argv[1],foutspec);
fc=atof(argv[2]);
fs=atof(argv[3]);
AMP=atof(argv[4]);
if (AMP>100.)
 {
  AMP=100.;
 }
else if
 (AMP<(-200.))
 {
  AMP=-200.;
 }
AMP=(1./.70710678)*(.77459667)*pow(10.,(AMP/20.));
if (straight==1)
        PHASE=atof(argv[5]);
else
        PHASE=0.0;
tsil1=atof(argv[5+straight]);
npoints_sil1=(long)(fs*tsil1);
tsig=atof(argv[6+straight]);
npoints_sig=(long)(fs*tsig);
tsil2=atof(argv[7+straight]);
npoints_sil2=(long)(fs*tsil2);
npoints=npoints_sil1+npoints_sig+npoints_sil2;
for (iter=0;iter<npoints;iter++)
 {
  if ((iter>=npoints_sil1)&&(iter<(npoints_sil1+npoints_sig)))
  {
   xtemp=AMP*sin(theta+PHASE);
   theta+=2.*pi*fc/fs;
  }
  else
  {
   xtemp=0.0;
  }
  if (xtemp>=0)
  		ibuf=(int)((xtemp*xmult)+.5);
  else
	  	ibuf=(int)((xtemp*xmult)-.5);
  if (out_type==0)
	  	fwrite(&ibuf,4,1,fout);
  else
		fprintf(fout,"%e\n",xtemp);
 
 }
fclose(fout);
return(0);
}


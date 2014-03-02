/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   functgen.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <math.h>
#include "functgen.h"
#define pi 3.1415927
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
enum Waveforms
{
	sine,
	pulse,
	sawtooth,
	triangle,
	sweepf,
	sweepa,
	dc,
	noise_uniform,
	ramp
};
functgen::functgen(double Ampval,double Thetaval,double Phaseval,double Offsetval,double Dutycycleval,
		double Fcval,double Fstopval,double Fstartval,double Astopval,double Astartval)
{
	int i;
	Amp=new double;
	Theta=new double[MAXCH];
	Phase=new double;
	Offset=new double;
	Dutycycle=new double;
	Fc=new double;
	Fstop=new double;
	Fstart=new double;
	Astop=new double;
	Astart=new double;
	*Amp=Ampval;
	for (i=0;i<MAXCH;i++)
		Theta[i]=Thetaval;
	*Phase=Phaseval;
	*Offset=Offsetval;
	*Dutycycle=Dutycycleval;
	*Fc=Fcval;
	*Fstop=Fstopval;
	*Fstart=Fstartval;
	*Astop=Astopval;
	*Astart=Astartval;
	srand(1);
}
functgen::~functgen()
{
	delete Amp;
	delete Theta;
	delete Phase;
	delete Offset;
	delete Dutycycle;
	delete Fc;
	delete Fstop;
	delete Fstart;
}

double functgen::singen(int numfreq)
{
	double xtemp,amp;
	int i;
	amp=*Amp/(numfreq);
	xtemp=0;
	for (i=0;i<numfreq;i++)
		xtemp+=amp*sin(Theta[i]+*Phase);
	xtemp+=(*Offset);
	return xtemp;
}
double functgen::pulsegen()
{
	double pos,xtemp;
	pos=(*Phase+Theta[0])/(2.*pi);
	pos=pos-(double)((int)pos);
	if (pos>(*Dutycycle/100.))
		xtemp=-.5;
	else
		xtemp=.5;
	xtemp=2*xtemp*(*Amp)+*Offset;
	return xtemp;
}
double functgen::sawtoothgen()
{
	double pos,xtemp;
	pos=(*Phase+Theta[0])/(2.*pi);
	pos=pos-(double)((int)pos);
	xtemp=2*pos*(*Amp)-*Amp+*Offset;
	return xtemp;
}
double functgen::trianglegen()
{
	double xtemp,pos;
	pos=(*Phase+Theta[0])/(2.*pi);
	pos=pos-(double)((int)pos);
	if (pos>.5)
		xtemp=4*(.75-pos);
	else
		xtemp=4*(pos-.25);
	xtemp=xtemp*(*Amp)+*Offset;
	return xtemp;
}
double functgen::sweepfgen(int npoints,int iter)
{
	double slope,dx,xtemp;
	slope=2.*(*Fstop-*Fstart)/(double)npoints;
	xtemp=*Amp*sin(Theta[0]+*Phase)+*Offset;
	if (iter<npoints/2)
		dx=(double)iter;
	else
		dx=(double)npoints-(double)iter;
	*Fc=*Fstart+slope*dx;
	return xtemp;
}
double functgen::sweepagen(int npoints,int iter)
{
	double slope,dx,xtemp;
	slope=2.*(*Astop-*Astart)/(double)npoints;
	xtemp=*Amp*sin(Theta[0]+*Phase);
	if (iter<npoints/2)
		dx=(double)iter;
	else
		dx=(double)npoints-(double)iter;
	*Amp=*Astart+slope*dx;
	return xtemp;
}
double functgen::dcgen()
{
	double xtemp;
	xtemp=*Offset;
	return xtemp;
}
double functgen::noisegen()
{
	double xtemp;
	xtemp=*Offset+2*(*Amp)*(((double)rand())/(pow(2.,31.)-1.0)-.5);
	return xtemp;
}
double functgen::ramp(int npoints,int iter)
{
	double xtemp;
	xtemp=*Offset+2.*(*Amp)*(double)iter/(double)npoints;
	return xtemp;
}
void functgen::thetaup(double fs,int numfreq)
{
	int i;
	double df=0,f;
	f=*Fc;
	if (numfreq>1) df=(*Fstop-*Fc)/(numfreq-1);
	for (i=0;i<numfreq;i++)
	{
		Theta[i]+=2.*pi*f/fs;
		f+=df;
	}
}

int main(int argc, char *argv[] )
{
	FILE *fout;
	char cbuf,endieness[50];
	int i,ibuf,waveform=sine,OptionIndex=1,nbytes=2,gotfilename=0,numfreq=1,nch=1,chno=0;
	int npoints,iter,nskip;
	double xtemp,fs=44100.,fc=1000.,amp=.5,offset=0.0,tsig=1.0,phase=0,xmult,dutycycle=50.0,fstop=0.0,astop=0.0;
	strcpy(endieness,"b");
	if (argc<2)
	{
		cout<<endl<<"use: functgen [specifications] outfile"<<endl<<endl;
		cout<<"generate binary samples with following specifications:"<<endl<<endl;
		cout<<"specifications:"<<endl;
		cout<<"--waveform=value   value=sine,pulse,sawtooth,triangle,sweepf,sweepa,dc,noise_uniform,ramp (def sine)"<<endl;
		cout<<"--fc=value         value=signal frequency in Hz (def 1000.0)"<<endl;
		cout<<"--fstop=value      value=sweep or multifrequency signal end frequency in Hz (def fs/2)"<<endl;
		cout<<"--numfreq=value    value=number of frequencies in sine waveform (def 1)"<<endl;	
		cout<<"--fs=value         value=sample frequency in Hz (def 44100.0)"<<endl;
		cout<<"--amp=value        value=signal p-p amplitude (def 1.0)"<<endl;
		cout<<"--astop=value      value=sweep signal end p-p amplitude (def 1.8)"<<endl;
		cout<<"--offset=value     value=signal offset (def 0.0)"<<endl;
		cout<<"--phase=value      value=signal starting phase in radians (def 0.0)"<<endl;
		cout<<"--dutycycle=value  value=dutycycle for pulse wave in percent (def 50.0)"<<endl;
		cout<<"--tsig=value       value=signal length in seconds (def 1.0)"<<endl;
		cout<<"--nbytes=value     value=number of bytes per sample (def 2)"<<endl;
		cout<<"--endieness=value  value=endieness little(l) big(b) (def b)"<<endl;
		cout<<"--nch=value        value=number of channels(def 1)"<<endl;
		cout<<"--chno=value       value=channel number to write to(def 0)"<<endl;
		exit(1);
	}
	/*--------------- Parse command line ---------------------*/
	while (OptionIndex < argc) 
	{
		if (strncmp(argv[OptionIndex], "--waveform=", 11) == 0) 
		{
			if (strncmp(argv[OptionIndex]+11,"sine",3)==0)
				waveform=sine;
			else if (strncmp(argv[OptionIndex]+11,"pulse",3)==0)
				waveform=pulse;
			else if (strncmp(argv[OptionIndex]+11,"sawtooth",3)==0)
				waveform=sawtooth;
			else if (strncmp(argv[OptionIndex]+11,"triangle",3)==0)
				waveform=triangle;
			else if (strncmp(argv[OptionIndex]+11,"sweepf",6)==0)
				waveform=sweepf;
			else if (strncmp(argv[OptionIndex]+11,"sweepa",6)==0)
				waveform=sweepa;
			else if (strncmp(argv[OptionIndex]+11,"dc",3)==0)
				waveform=dc;
			else if (strncmp(argv[OptionIndex]+11,"noise_uniform",3)==0)
				waveform=noise_uniform;
			else if (strncmp(argv[OptionIndex]+11,"ramp",2)==0)
				waveform=ramp;

			else
			{
				cout<<"bad waveform "<<endl; 
				exit(1);
			}
		}
		else if (strncmp(argv[OptionIndex], "--fc=", 5) == 0) 
		{
			fc = atof(&argv[OptionIndex][5]);
		}
		else if (strncmp(argv[OptionIndex], "--fstop=", 8) == 0) 
		{
			fstop = atof(&argv[OptionIndex][8]);
		}
		else if (strncmp(argv[OptionIndex], "--numfreq=", 10) == 0) 
		{
			numfreq = atoi(&argv[OptionIndex][10]);
			if (numfreq>MAXCH)
			{
				cout<<"bad value for numfreq "<<endl;
				exit(1);
			}

		}
		else if (strncmp(argv[OptionIndex], "--fs=", 5) == 0) 
		{
			fs = atof(&argv[OptionIndex][5]);
		}
		else if (strncmp(argv[OptionIndex], "--amp=", 6) == 0) 
		{
			amp = atof(&argv[OptionIndex][6])/2;
		}
		else if (strncmp(argv[OptionIndex], "--astop=", 8) == 0) 
		{
			astop = atof(&argv[OptionIndex][8])/2;
		}
		else if (strncmp(argv[OptionIndex], "--offset=", 9) == 0) 
		{
			offset = atof(&argv[OptionIndex][9]);
		}
		else if (strncmp(argv[OptionIndex], "--phase=", 8) == 0) 
		{
			phase = atof(&argv[OptionIndex][8]);
		}
		else if (strncmp(argv[OptionIndex], "--dutycycle=", 12) == 0) 
		{
			dutycycle = atof(&argv[OptionIndex][12]);
			if ((dutycycle<0.0)||(dutycycle>100.0))
			{
				cout<<"bad value for dutycycle "<<endl;
				exit(1);
			}
		}
		else if (strncmp(argv[OptionIndex], "--tsig=", 7) == 0) 
		{
			tsig = atof(&argv[OptionIndex][7]);
		}
		else if (strncmp(argv[OptionIndex], "--nbytes=", 9) == 0) 
		{
			nbytes = atoi(&argv[OptionIndex][9]);
			if ((nbytes>4)||(nbytes<1))
			{
				cout<<"bad number of bytes "<<endl;
				exit(1);
			}
		}
		else if (strncmp(argv[OptionIndex], "--endieness=", 12) == 0) 
		{
			strcpy(endieness,argv[OptionIndex]+12);
			if ((endieness[0]!='b')&&(endieness[0]!='l'))
			{
				cout<<"bad endieness value "<<endl;
				exit(1);
			}
		}
		else if (strncmp(argv[OptionIndex], "--nch=", 6) == 0) 
		{
			nch = atoi(&argv[OptionIndex][6]);
		}
		else if (strncmp(argv[OptionIndex], "--chno=", 7) == 0) 
		{
			chno = atoi(&argv[OptionIndex][7]);
		}
		else
		{
			if (argv[OptionIndex][0]=='-')
			{
				cout<<"bad parameter "<<endl;
				exit(1);
			}
			else if (OptionIndex==argc-1) 
				gotfilename=1;
		}
		OptionIndex++;
	}
	if (fstop==0) 
		fstop=fs/2;
	if (astop==0) 
		astop=.9;
	if (waveform==sweepa)
	{
		if (astop<amp)
		{
			cout<<"bad parameter for sweepa stop value"<<endl;
			exit(1);
		}
	}
	if (waveform==sweepf)
	{
		if (fstop<fc)
		{
			cout<<"bad parameter for sweepf stop value"<<endl;
			exit(1);
		}
	}
	if (gotfilename==0)
	{
		cout<<"need filename "<<endl;
		exit(1);
	}
	/*--------------- init the generator ---------------------*/
	if (chno==0)
		fout=fopen(argv[argc-1],"wb");
	else
		fout=fopen(argv[argc-1],"r+b");
	npoints=(int)(fs*tsig);
	xmult=pow(2.,(double)(8*nbytes-1))-1.0;
	functgen Functgen(amp,0,phase,offset,dutycycle,fc,fstop,fc,astop,amp);//init values for Amp,Theta,Phase,Offset,Dutycycle,Fc,Fstop,Fstart,Astop,Astart
	/*--------------- run the generator ----------------------*/
	nskip=nbytes*chno;
	for (i=0;i<nskip;i++)
		fgetc(fout);
	for (iter=0;iter<npoints;iter++)
	{
		switch(waveform)
		{
			case sine:
				xtemp=Functgen.singen(numfreq);
				break;
			case pulse:
				xtemp=Functgen.pulsegen();
				break;
			case sawtooth:
				xtemp=Functgen.sawtoothgen();
				break;
			case triangle:
				xtemp=Functgen.trianglegen();
				break;
			case sweepf:
				xtemp=Functgen.sweepfgen(npoints,iter);
				break;
			case sweepa:
				xtemp=Functgen.sweepagen(npoints,iter);
				break;
			case dc:
				xtemp=Functgen.dcgen();;
				break;
			case noise_uniform:
				xtemp=Functgen.noisegen();
				break;
			case ramp:
				xtemp=Functgen.ramp(npoints,iter);
				break;
		}
		Functgen.thetaup(fs,numfreq);
		if (xtemp>=0)
			ibuf=(int)((xtemp*xmult)+.5);
		else
			ibuf=(int)((xtemp*xmult)-.5);
		for (i=0;i<nbytes;i++)
		{
			if (endieness[0]=='l')
				cbuf=(char)(ibuf>>(i*8));
			else
				cbuf=(char)(ibuf>>((nbytes-i-1)*8));
			fputc(cbuf,fout);
		}
		nskip=nbytes*(nch-1);
		for (i=0;i<nskip;i++)
		{
			if (chno==0) fputc(0,fout);
				else fgetc(fout);
		}
	}
	/*----------------------- finish up ---------------------*/
	fclose(fout);
	return(0);
}


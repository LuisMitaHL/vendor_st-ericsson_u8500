/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   diffmaxb.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
int	main(int argc, char	*argv[])
{
	FILE *fin[2];
	char charbuf[4];
	char rootname[200],fname[200];
	int	fileno,chan=0,chantotest=0,i,iter=0,maxdiff=0,err=0,diff,numskip,nskip[2],ntest=0x7fffffff,nch=1,swap[2],display=0,nbytes[2],testallchannels=0;
	int datasample[2],OptionIndex=0,stop=0,bufstart[2],bufstop[2],incval[2],len,rightshiftval[2],roundval[2];
	float tmpres,tmpref,maxdiffdB=100.0,diffdB;
	nskip[0]=nskip[1]=0;
	nbytes[0]=nbytes[1]=2;
	if (argc<3)
	{
		printf("\nuse: diffmaxb [OPTIONS] infile1 infile2\n\n");
		printf("Compares 2 binary nch interleaved sample files.\n\n");
		printf("OPTIONS:\n");
		printf("--nskip1=value     Value is no. of samples(def 0), each file, on channel chantotest\n");
		printf("--nskip2=value     to be skipped before compare starts. \n"); 
		printf("--ntest=value      Value is no. of samples on channel chantotest to be compared. (def all)\n");
		printf("--maxdiff=value    Value is max integer difference(def 0).\n");
		printf("--maxdiffdB=value  Value is max integer difference in dB (def 100).\n");
		printf("--nch=value        1=mono(def), 2=interleaved stereo, 3=interleaved 3-channel (etc..).\n");
		printf("--chantotest=value 0(def)=mono or L, 1=R, (2,3,4,..etc. or all) channel to be tested \n");
		printf("--endieness=value  Value bb(def) for big file1 big file2,\n"); 
		printf("                   bl for big file1 little file2, ..etc.\n");
		printf("--nbytes=value     Either both Infiles have data coded on value=1, 2(def), 3, or 4 bytes.\n");
		printf("--nbytes1=value    Or infile1 data is coded on value=1, 2(def), 3, or 4 bytes\n");
		printf("--nbytes2=value    And infile2 data is coded on value=1, 2(def), 3, or 4 bytes.\n");
		printf("                   The comparison is done based on the minimum of nbytes1,nbytes2.\n");
		printf("                   The larger word  will be converted after rounding to the shorter wordlength.\n"); 
		printf("--display          Display every difference. (def inactive)\n");
		exit(1);
	}
	/*--------------- Parse command line ---------------------*/
	swap[0]=0;swap[1]=0;
	while (OptionIndex < argc) 
	{
		if (strncmp(argv[OptionIndex], "--nskip1=", 9) == 0) 
		{
			nskip[0] = atoi(&argv[OptionIndex][9]);
		}
		if (strncmp(argv[OptionIndex], "--nskip2=", 9) == 0) 
		{
			nskip[1] = atoi(&argv[OptionIndex][9]);
		}
		if (strncmp(argv[OptionIndex], "--ntest=", 8) == 0) 
		{
			ntest = atoi(&argv[OptionIndex][8]);
		}
		if (strncmp(argv[OptionIndex], "--maxdiff=", 10) == 0) 
		{
			maxdiff = atoi(&argv[OptionIndex][10]);
		}
		if (strncmp(argv[OptionIndex], "--maxdiffdB=", 12) == 0) 
		{
			maxdiffdB = atof(&argv[OptionIndex][12]);
		}
		if (strncmp(argv[OptionIndex], "--nch=", 6) == 0) 
		{
			nch = atoi(&argv[OptionIndex][6]);
		}
		if (strncmp(argv[OptionIndex], "--chantotest=", 13) == 0) 
		{
		 	if (strncmp(&argv[OptionIndex][13],"all",3) == 0)
				testallchannels=1;
			else
				chantotest = atoi(&argv[OptionIndex][13]);
		}
		if (strncmp(argv[OptionIndex], "--endieness=b", 13) == 0) 
		{
			swap[0]=0;swap[1]=0;
			if (((strlen(argv[OptionIndex]))==14)&&(argv[OptionIndex][13]=='l')) swap[1]=1;
		}
		if (strncmp(argv[OptionIndex], "--endieness=l", 13) == 0) 
		{
			swap[0]=1;swap[1]=1;
			if (((strlen(argv[OptionIndex]))==14)&&(argv[OptionIndex][13]=='b')) swap[1]=0;
		}
		if (strncmp(argv[OptionIndex], "--nbytes1=", 10) == 0) 
		{
			nbytes[0]=atoi(&argv[OptionIndex][10]);
			if (nbytes[0]>4)
			{
				printf("nbytes1 BAD VALUE\n");
				exit(1);
			}
		}
		if (strncmp(argv[OptionIndex], "--nbytes2=", 10) == 0) 
		{
			nbytes[1]=atoi(&argv[OptionIndex][10]);
			if (nbytes[1]>4)
			{
				printf("nbytes2 BAD VALUE\n");
				exit(1);
			}
		}
		if (strncmp(argv[OptionIndex], "--nbytes=", 9) == 0) 
		{
			nbytes[0]=atoi(&argv[OptionIndex][9]);
			nbytes[1]=atoi(&argv[OptionIndex][9]);
			if (nbytes[0]>4)
			{
				printf("nbytes BAD VALUE\n");
				exit(1);
			}

		}
		if (strncmp(argv[OptionIndex], "--display", 9) == 0) 
		{
			display=1;
		}
		OptionIndex++;
	}
	if (chantotest>(nch-1)) chantotest=nch-1;
	/*--------------- Open the 2 files to be compared -----------------------------------*/
	for (fileno=0;fileno<2;fileno++)
	{
		strcpy(fname,argv[argc-2+fileno]);
		len=strlen(fname);
		if (fileno==0)
		{
			/*--------- get rootname of 1st file without path ------------*/
			while (len>0)
			{
				if (fname[len-1]=='/')
				{
					break;
				}
				strcpy(rootname,&fname[len-1]);
				len--;
			}
			/*------------------------------------------------------------*/
		}
		else
		{
			/*--------- check for implied 2nd filename -------*/
			if (fname[len-1]=='.')
			{
				fname[len-1]='\0';
				strcat(fname,rootname);
			}
			/*-----------------------------------------------*/
		}
		if ((fin[fileno]=fopen(fname,"rb"))==0)
		{
			/*----- check if files are successfully opened ---*/
			printf("ERROR-FILE NOT FOUND!\n");
			exit(1);
			/*------------------------------------------------*/
		}
	}
	/*-------------- Skip to starting point of each file ------------*/
	for (fileno=0;fileno<2;fileno++)
	{
		numskip=nch*nskip[fileno];
		while (numskip>0)
		{
			for (i=0;i<nbytes[fileno];i++)
			{
				fgetc(fin[fileno]);
				if (feof(fin[fileno]))
				{
					stop=1;
					break;
				}
			}
			if (stop!=0)
				break;
			numskip--;
		}
		if (stop!=0)
			break;
	}
	/*------------- Prepare for reading each sample of each file with correct endieness -----------*/
	for (fileno=0;fileno<2;fileno++)
	{
		if (swap[fileno]==0)
		{
			incval[fileno]=1;
			bufstart[fileno]=0;
			bufstop[fileno]=nbytes[fileno]+incval[fileno]-1;
		}
		else
		{
			incval[fileno]=-1;
			bufstart[fileno]=nbytes[fileno]-1;
			bufstop[fileno]=0+incval[fileno];
		}
	}
	/*-------------------------- Prepare for adjusting for different coded word lengths ---------------------------*/
	/*----------- The sample from the file with the longest byte length is rounded to the shorter byte length -----*/
	if (nbytes[0]>nbytes[1])
	{
		rightshiftval[0]=8*(nbytes[0]-nbytes[1]);
		roundval[0]=1<<(rightshiftval[0]-1);

		rightshiftval[1]=0;
		roundval[1]=0;
	}
	else
	{
		if (nbytes[1]>nbytes[0])
		{
			rightshiftval[1]=8*(nbytes[1]-nbytes[0]);
			roundval[1]=1<<(rightshiftval[1]-1);

			rightshiftval[0]=0;
			roundval[0]=0;
		}
		else
		{
			rightshiftval[0]=rightshiftval[1]=0;
			roundval[0]=roundval[1]=0;
		}
	}
	/*------------ Main Loop for comparison ------------------------*/
	while ((ntest>0)&&(stop==0))
	{
		/*--------------- Read test and reference samples ----------*/
		for (fileno=0;fileno<2;fileno++)
		{
			for (i=bufstart[fileno];i!=bufstop[fileno];i+=incval[fileno])
			{
				charbuf[i]=fgetc(fin[fileno]);
				if (feof(fin[fileno]))
				{
					stop=1;
					break;
				}
			}
			if (stop!=0)
				break;
			datasample[fileno]=(int)charbuf[0];
			for (i=1;i<nbytes[fileno];i++)
			{
				datasample[fileno]=(datasample[fileno]<<8)|(((int)charbuf[i])&0xff);
			}
		}
		if (stop!=0)    
			break;
		/*------- Compare test and reference samples -------*/
		if ((chan==chantotest)||(testallchannels!=0)) //compare only 1 of the samples if nch>1 unless testallchannels inactive
		{
			diff=abs(((datasample[1]+roundval[1])>>rightshiftval[1])-((datasample[0]+roundval[0])>>rightshiftval[0]));
			tmpref=(float)(abs((datasample[1]+roundval[1])>>rightshiftval[1]));
			tmpres=(float)(abs((datasample[0]+roundval[0])>>rightshiftval[0]));
			if ((tmpref==0.0)||(tmpres==0.0))
				diffdB=0.0;
			else
				diffdB=20*log10(tmpres/tmpref);
			if (diffdB<0.0)
				diffdB=-diffdB;


			if (diff>maxdiff)
			{
				if (display!=0)
				{
					printf("error on chan: %d sample no. : %d , input1: %x input2: %x maxdiff: %x\n",chan,iter,datasample[0],datasample[1],maxdiff);
				}
				err=1;
			}
			else if (diffdB>maxdiffdB)
			{
				if (display!=0)
				{
					printf("dB error on  chan: %d sample no.: %d , input1: %x input2: %x maxdiffdB: %f\n",chan,iter,datasample[0],datasample[1],maxdiffdB);
				}
				err=1;
			}


			if (ntest!=0x7fffffff)
			{
				ntest--;
			}
			iter++;
		}
		chan++;
		if (chan==nch) chan=0;
	}
	if (err==1)	
		printf("########################## Error!! The 2 files differ!! ########################\n");
	fclose(fin[0]);
	fclose(fin[1]);
	return(err);
}



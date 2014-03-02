/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   findmax.c
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
	FILE *fin;
	char *in_name;
	int	maxval=0,datavalid=1;
	short databyte;
	float fs;
	long maxpos=0L,nbtest=0L;
	if (argc<2)
	{
		printf("\nuse: findmax infile [fs]\n\n");
		printf("------------------- description -------------------------------------\n");
		printf("finds max abs value and its location in a raw 16-bit binary file infile\n");
		printf("if sampling frequency in Hz is specified the location is in milliseconds\n");
		printf("----------------------------------------------------------------------\n");
		exit(1);
	} 
	in_name=argv[1];
	if((fin=fopen(in_name,"rb"))==0)
	{
		printf("ERROR-FILE NOT FOUND!\n");
		exit(1);
	}
	if (argc==3)
		fs=atof(argv[2]);
	while (datavalid!=0)
	{
		datavalid=fread(&databyte,2,1,fin);
		if (datavalid!=0)
		{	
			if (abs(databyte)>maxval)
			{
				maxval=abs(databyte);
				maxpos=nbtest;
			}
			nbtest++;
		}
	}
	if (argc==3)
		printf("max abs value: %d at delay of %f milliseconds\n",maxval,((float)maxpos/fs)*1000.0);
	else
		printf("max abs value: %d location: %ld\n",maxval,maxpos);
		
	fclose(fin);
	return 0;
}



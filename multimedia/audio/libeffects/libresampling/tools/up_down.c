/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   up_down.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char *argv[] )
{
	FILE *fin,*fout;
	int datavalid=1;
	short ibuf;
	int rate=1,up=1,i;


	if (argc!=5)
	{
		printf("\n\n********* PARAMETER ERROR *********");
		printf("\n\n up_down INFILE  OUTFILE up rate \n\n");
		printf("  INFILE   = 16-bit raw binary file with input samples\n");
		printf("  OUTFILE  = 16-bit raw binary file with output samples\n");
		printf("  up       = 1: rate is upsample rate(add zeros)  2: rate is downsample rate\n");   
		exit(1);
	}


	if ((fin=fopen(argv[1],"rb"))==0)
	{
		printf("\n\n********** BAD INPUT FILE NAME  **********\n\n");
		exit(1);
	}

	fout=fopen(argv[2],"wb");

	up=atoi(argv[3]);
	rate=atoi(argv[4]);

	if (up!=0)
	{
		//---------- upsample -----
		while (datavalid==1)
		{
			datavalid=fread(&ibuf,2,1,fin);
			if (datavalid!=1) break;
			fwrite(&ibuf,2,1,fout);
			ibuf=0;
			for (i=0;i<rate-1;i++) fwrite(&ibuf,2,1,fout);
		}
	}
	else
	{
		//---------- downsample ------
		while (datavalid==1)
		{
			for (i=0;i<rate;i++)
			{
				datavalid=fread(&ibuf,2,1,fin);
				if (datavalid!=1) break;
			}
			if (datavalid!=1) break;
			fwrite(&ibuf,2,1,fout);
		}
	}
	fclose(fin);
	fclose(fout);
	return 0;
}


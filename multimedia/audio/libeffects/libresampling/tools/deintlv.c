/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   deintlv.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAXCH 8
void use(void);
int main(int argc, char *argv[])
{
 	FILE *fin,*fout[MAXCH];
	int i,nch,databyte,datavalid=1;
	if (argc<4)
	{
		use();
		exit(1);
	}
	nch=atoi(argv[2]);
	if (nch>MAXCH)
	{
		use();
		printf("specify nch<=6\n");
		exit(1);
	}
	if (argc!=(nch+3))
	{
		use();
		exit(1);
	}
	if((fin=fopen(argv[1],"rb"))==0)
	{
		printf("ERROR-FILE NOT FOUND!!\n");
		exit(1);
	}
	for (i=0;i<nch;i++)
	{
		fout[i]=fopen(argv[i+3],"wb");
	}
	while (datavalid==1)
	{
		for (i=0;i<nch;i++)
		{
			datavalid=fread(&databyte,2,1,fin);
			if (datavalid!=1) break;
			fwrite(&databyte,2,1,fout[i]);
		}
		if (datavalid!=1) break;
	}
	fclose(fin);
	for (i=0;i<nch;i++)
 		fclose(fout[i]);
	return(0);
}
void use(void)
{
		printf("use: deintlv infile nch outfile_0 [outfile_1] .. [outfile_nch-1]\n");
		printf("----------------------------------------------------------------------------\n");
		printf("deinterleave 16-bit raw nch binary file to nch 16-bit raw mono binary files\n");
		printf("----------------------------------------------------------------------------\n");
}



/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   diffbmax.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
//#define PRINTF
enum byte_order { order_unknown, order_bigEndian, order_littleEndian };
enum byte_order NativeByteOrder = order_unknown;
enum byte_order DetermineByteOrder(void);


enum byte_order DetermineByteOrder(void)
{
	enum byte_order ByteOrder = order_unknown;

	char s[ sizeof(long) + 1 ];
	union
	{
		long longval;
		char charval[ sizeof(long) ];
	} probe;
	probe.longval = 0x41424344L;  /* ABCD in ASCII */
	strncpy( s, probe.charval, sizeof(long) );
	s[ sizeof(long) ] = '\0';
	if ( strcmp(s, "ABCD") == 0 ) {
		ByteOrder = order_bigEndian;
	}
	else if ( strcmp(s, "DCBA") == 0 ) {
		ByteOrder = order_littleEndian;
	}

	if ( ByteOrder == order_unknown ) {
		#ifdef PRINTF
		printf("unknownEndian, byte order not determined\n" );
		#endif
		exit( 1 );
	} else if (ByteOrder == order_bigEndian) {
		#ifdef PRINTF
		printf("bigEndian, no byte swap required\n");
		#endif
	} else {
		#ifdef PRINTF
		printf("littleEndian, byte swap required\n");
		#endif
	}
	return ByteOrder;
}

void swapbytes(short *data)
{
	short outdata;
	outdata=(*data>>8)&0x00ff;
	outdata|=((*data<<8)&0xff00);
	*data=outdata;
}
int	main(int argc, char	*argv[])
{
	FILE *fin1,*fin2;
	char *in_name1,*in_name2;
	int	iter=0,datavalid=1,maxdiff,err=0,diff,nskip1=0,nskip2=0,ntest=0x7fffffff,stride=1,i,swap=0,display=0;;
	short databyte1,databyte2;
	if ((argc!=4)&&(argc!=8)&&(argc!=9)&&(argc!=10))
	{
		printf("\nuse: diffbmax infile1 [nskip1]* infile2 [nskip2]* maxdiff [ntest]* [stride]* [endienness]** [display]** \n\n");
		printf("------------------- description -------------------------------------\n");
		printf("compares 2 raw 16-bit binary files infile1,infile2 allowing max\n");
		printf("           integer difference of maxdiff(|0-32767|)\n");
		printf("    nskip: optional no. samples,each file to be skipped before comparing \n");
		printf("    ntest: optional no. samples to compare (else continues to EOF)	\n");
		printf("   stride: optional only check every stride samples\n");
		printf("   endienness: optional b(ig) or l(ittle) (def=b)\n");
		printf("   display: d for display results on each error\n");
		printf("        *: optional arguments either all present or all absent! \n");
		printf("       **: optional arguments, tested only if all other * optional arguments  present \n");
		printf("----------------------------------------------------------------------\n");
		exit(1);
	} 
	in_name1=argv[1];
	if (argc>=8) 
	{
		nskip1=atoi(argv[2]);
		in_name2=argv[3];
		nskip2=atoi(argv[4]);
		maxdiff=abs(atoi(argv[5]));
		ntest=atoi(argv[6]);
		stride=atoi(argv[7]);
	}
	else
	{
		in_name2=argv[2];
		maxdiff=abs(atoi(argv[3]));
	}
	i=stride;
	if((fin1=fopen(in_name1,"rb"))==0)
	{
		printf("ERROR-FILE NOT FOUND!\n");
		exit(1);
	}
	if((fin2=fopen(in_name2,"rb"))==0)
	{
		printf("ERROR-FILE NOT FOUND!\n");
		exit(1);
	}
	while ((datavalid!=0)&&(nskip1>0))
	{
		datavalid=fread(&databyte1,2,1,fin1);
		nskip1--;
	}
	while ((datavalid!=0)&&(nskip2>0))
	{
		datavalid=fread(&databyte1,2,1,fin2);
		nskip2--;
	}
	if ((argc>=9)&&(argv[8][0]=='l'))
		swap=1;
	if ((argc==10)&&(argv[9][0]=='d'))
		display=1;
	NativeByteOrder = DetermineByteOrder();
	if (NativeByteOrder == order_littleEndian)
	{
		if (swap==1) swap=0;
		else swap=1;
	}
	while ((datavalid!=0)&&(ntest>0))
	{
		i--;
		datavalid=fread(&databyte1,2,1,fin1);
		if (swap!=0)
			swapbytes(&databyte1);
		if (datavalid!=0) 
			datavalid=fread(&databyte2,2,1,fin2);
		if (swap!=0)
			swapbytes(&databyte2);
		if (datavalid!=0)
		{
			if (i==0)
			{
				diff=abs(databyte2-databyte1);
				if (diff>maxdiff)
				{
					if (display!=0)
					{
						printf("error on sample: %d , input1: %x input2: %x diff: %x\n",iter,databyte1,databyte2,diff);
					}
					err=1;
				}
				i=stride;
				if (argc>=8)
				{
					ntest--;
				}
				iter++;
			}
		}
	}
	if (err==1)	
		printf(" Error! The 2 files differ\n");
	fclose(fin1);
	fclose(fin2);
	return(err);
}



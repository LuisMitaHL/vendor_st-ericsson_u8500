/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   byterev.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  FILE *in,*out;
  int niter;
  char msb,lsb;
  
  if (argc < 3)
  {
	printf("usage: %s InputFile OutputFile",argv[0]);
	return 1;
  }
  
  if((in  = fopen(argv[1], "rb")) == NULL )
  {
	fprintf(stderr,"%s :",argv[1]);
	perror("");
	exit(1);
  }
	
  if((out = fopen(argv[2], "wb"))== NULL )
  {
	fprintf(stderr,"%s :",argv[2]);
	perror("");
	exit(1);
  }
	 
  fseek(in, 0, SEEK_END);
  niter = ftell(in) / 2;
  rewind(in);
  
  while(niter)
  {
	msb = getc(in);
	lsb = getc(in);
	putc(lsb,out);
	putc(msb,out);
	
	niter--;
  }
  
  fclose(in);
  fclose(out);
  exit(0);
}

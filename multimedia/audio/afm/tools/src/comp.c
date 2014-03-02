/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   comp.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
/*
 * Basic program for bitexactness 
 *
 * return : - 0 if the files are identical (even if one file is shorter)
 *          - 1 if the files differs or one file is empty 
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  FILE *file1,*file2;
  int size,size1,size2,i;
  int size_asked = 0;
  char char1,char2;
  
  /* check if enough param */
  if(argc < 3)
  {
	fprintf(stderr,"Usage : \n %s <file1> <file2> [number_of_byte_to_compare]\n",argv[0]);
	exit(1);
  }

  if ((file1 = fopen(argv[1], "rb")) == NULL)
	{
	  fprintf(stderr,"%s :",argv[1]);
	  perror("");
	  exit(1);
	}

  if ((file2 = fopen(argv[2], "rb")) == NULL)
	{
	  fprintf(stderr,"%s :",argv[2]);
	  perror("");
	  exit(1);
	}

  if(argc == 4)
  {
	size_asked = atoi(argv[3]);
  }
  
  /* check file size */
  fseek(file1, 0, 2);
  size1 = ftell(file1);
  rewind(file1);
  if(size1 == 0)
  {
	fprintf(stderr,"%s is empty\n",argv[1]);
	exit(1);
  }

  fseek(file2, 0, 2);
  size2 = ftell(file2);
  rewind(file2);
  if(size2 == 0)
  {
	fprintf(stderr,"%s is empty\n",argv[2]);
	exit(1);
  }
  
  if(size1<size2)
	size=size1;
  else
	size=size2;

  if((size_asked > 0) &&
	 (size_asked < size))
	size = size_asked;


  /* compare */
  for(i=0;i<size;i++)
  {
	char1=getc(file1);
	char2=getc(file2);

	if(char1 != char2)
	{
	  fclose(file1);
	  fclose(file2);
	  fprintf(stderr,"Files %s and %s differ at byte %d\n",argv[1],argv[2],i+1);
	  exit(1);
	}
  }

  printf("%d bytes compared \n",size);

  fclose(file1);
  fclose(file2);

  exit(0);
}

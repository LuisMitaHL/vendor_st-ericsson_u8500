/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   afmtestmalloc.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <afmtestmalloc.nmf>
#include <memorybank.idt>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


#pragma nopid
extern int _end1_;
#pragma nopid
extern int _min_ram1_;
#pragma nopid
extern int _max_ram1_;

#pragma nopid
extern int _end2_;
#pragma nopid
extern int _min_ram2_;
#pragma nopid
extern int _max_ram2_;

#pragma nopid
extern __EXTERN int _end5_;
#pragma nopid
extern __EXTERN int _min_ram5_;
#pragma nopid
extern __EXTERN int _max_ram5_;

#pragma nopid
extern __EXTERN16 int _end6_;
#pragma nopid
extern __EXTERN16 int _min_ram6_;
#pragma nopid
extern __EXTERN16 int _max_ram6_;

#pragma nopid
extern __ESRAM int _end8_;
#pragma nopid
extern __ESRAM int _min_ram8_;
#pragma nopid
extern __ESRAM int _max_ram8_;

#pragma nopid
extern __ESRAM16 int _end9_;
#pragma nopid
extern __ESRAM16 int _min_ram9_;
#pragma nopid
extern __ESRAM16 int _max_ram9_;


void METH(initHeap)(t_sint24 sizeX, t_sint24 sizeY, t_sint24 sizeDDR, t_sint24 sizeDDR16,
					t_sint24 sizeESR, t_sint24 sizeESR16)
{
  int maxsize;
  int start;
  
  
  /****************** XTCM ******************/
  maxsize = (((int)(&_max_ram1_) - (int)(&_min_ram1_)) -
			 ((int)(&_end1_) - (int)(&_min_ram1_)));
  maxsize -= 10;
  
  if(sizeX < 0)
  {
	maxsize += sizeX;
	if(maxsize < 0){
	  fprintf(stderr,"Cannot reserve %d words in X memory\n",sizeX);
	  exit(55);
	}
  }
  else if(sizeX > 0)
  {
	maxsize = wmin(maxsize,sizeX);
  }
  
  start = (int)malloc(maxsize);
  if(start)
  {
	printf("Allocated %5d words in     X memory for malloc component\n",maxsize);
  }
  else
  {
	fprintf(stderr,"Error while allocating X memory Heap (%d)\n",maxsize);
	exit(55);
  }
  
  if(start & 0x1)
  {
	start++;
	maxsize--;
  }
  setheap.setHeap(MEM_XTCM,(void *)start,maxsize);

  /****************** YTCM ******************/
  maxsize = (((int)(&_max_ram2_) - (int)(&_min_ram2_)) -
		  ((int)(&_end2_) - (int)(&_min_ram2_)));
  maxsize -= 10;
  
  if(sizeY < 0)
  {
	maxsize += sizeY;
	if(maxsize < 0){
	  fprintf(stderr,"Cannot reserve %d words in Y memory\n",sizeY);
	  exit(55);
	}
  }
  else if(sizeY > 0)
  {
	maxsize = wmin(maxsize,sizeY);
  }
  
  start = (int)malloc_y(maxsize);
  if(start)
  {
	printf("Allocated %5d words in     Y memory for malloc component\n",maxsize);
  }
  else
  {
	fprintf(stderr,"Error while allocating Y memory Heap (%d)\n",maxsize);
	exit(55);
  }
  
  if(start & 0x1)
  {
	start++;
	maxsize--;
  }
  setheap.setHeap(MEM_YTCM,(void *)start,maxsize);

  
  
  /****************** DDR24 ******************/
  maxsize = (((int)(&_max_ram5_) - (int)(&_min_ram5_)) -
			 ((int)(&_end5_) - (int)(&_min_ram5_)));
  maxsize -= 10;
  
  if(sizeDDR < 0)
  {
	maxsize += sizeDDR;
	if(maxsize < 0){
	  fprintf(stderr,"Cannot reserve %d words in DDR memory\n",sizeDDR);
	  exit(55);
	}
  }
  else if(sizeDDR > 0)
  {
	maxsize = wmin(maxsize,sizeDDR);
  }
  
  start = (int)malloc_ext(maxsize);
  if(start)
  {
	printf("Allocated %5d words in DDR24 memory for malloc component\n",maxsize);
  }
  else
  {
	fprintf(stderr,"Error while allocating DDR24 memory Heap (%d)\n",maxsize);
	exit(55);
  }
  
  if(start & 0x1)
  {
	start++;
	maxsize--;
  }
  setheap.setHeap(MEM_DDR24,(void *)start,maxsize);
  
  /****************** DDR16 ******************/
  maxsize = (((int)(&_max_ram6_) - (int)(&_min_ram6_)) -
			 ((int)(&_end6_) - (int)(&_min_ram6_)));
  maxsize -= 10;
  
  if(sizeDDR16 < 0)
  {
	maxsize += sizeDDR16;
	if(maxsize < 0){
	  fprintf(stderr,"Cannot reserve %d words in DDR memory\n",sizeDDR16);
	  exit(55);
	}
  }
  else if(sizeDDR16 > 0)
  {
	maxsize = wmin(maxsize,sizeDDR16);
  }
  
  start = (int)malloc_ext16(maxsize);
  if(start)
  {
	printf("Allocated %5d words in DDR16 memory for malloc component\n",maxsize);
  }
  else
  {
	fprintf(stderr,"Error while allocating DDR16 memory Heap (%d)\n",maxsize);
	exit(55);
  }
  
  if(start & 0x1)
  {
	start++;
	maxsize--;
  }
  setheap.setHeap(MEM_DDR16,(void *)start,maxsize);
 
  /****************** ESR24 *****************/
  maxsize = (((int)(&_max_ram8_) - (int)(&_min_ram8_)) -
			 ((int)(&_end8_) - (int)(&_min_ram8_)));
   maxsize -= 10;
 
  if(sizeESR < 0)
  {
	maxsize += sizeESR;
	if(maxsize < 0){
	  fprintf(stderr,"Cannot reserve %d words in ESR memory\n",sizeESR);
	  exit(55);
	}
  }
  else if(sizeESR > 0)
  {
	maxsize = wmin(maxsize,sizeESR);
  }
  
  start = (int)malloc_esram(maxsize);
  if(start)
  {
	printf("Allocated %5d words in ESR24 memory for malloc component\n",maxsize);
  }
  else
  {
	fprintf(stderr,"Error while allocating ESR24 memory Heap (%d)\n",maxsize);
	exit(55);
  }
  
  if(start & 0x1)
  {
	start++;
	maxsize--;
  }
  setheap.setHeap(MEM_ESR24,(void *)start,maxsize);
  
  /*********************** ESR16 **********************/
  maxsize = (((int)(&_max_ram9_) - (int)(&_min_ram9_)) -
			 ((int)(&_end9_) - (int)(&_min_ram9_)));
  maxsize -= 10;
  
  if(sizeESR16 < 0)
  {
	maxsize += sizeESR16;
	if(maxsize < 0){
	  fprintf(stderr,"Cannot reserve %d words in ESR memory\n",sizeESR16);
	  exit(55);
	}
  }
  else if(sizeESR16 > 0)
  {
	maxsize = wmin(maxsize,sizeESR16);
  }
  
  start = (int)malloc_esram16(maxsize);
  if(start)
  {
	printf("Allocated %5d words in ESR16 memory for malloc component\n",maxsize);
  }
  else
  {
	fprintf(stderr,"Error while allocating ESR16 memory Heap (%d)\n",maxsize);
	exit(55);
  }
  
  if(start & 0x1)
  {
	start++;
	maxsize--;
  }
  setheap.setHeap(MEM_ESR16,(void *)start,maxsize);
}

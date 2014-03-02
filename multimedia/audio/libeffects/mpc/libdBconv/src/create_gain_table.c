/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "dBconv_include.h"


int main(void)
{
  FILE *file_h;
  double step;
  int i;
  
  file_h = fopen("./include/dB_tab_gain.h","wb");
  if(file_h == NULL)
  {
	printf("Error can't open ./include/dB_tab_gain.h\n");
	exit(1);
  }

  fprintf(file_h,"/* File created by create_tab_gain */ \n");
  fprintf(file_h,"#ifndef _dB_tab_gain_h_\n#define _dB_tab_gain_h_\n\n\n");

  step = (6.02/(double)(NB_STEP -1));

  fprintf(file_h,"MMshort DBCONV_MEM tab_gain[%d] = {\n",NB_STEP);
  for(i=0;i<NB_STEP;i++)
  {
	fprintf(file_h,"\tFORMAT_FLOAT(%f,MAXVAL),\n",
		   (float)(pow(10.0,
					   (double)((double)(step*i)/(double)20.0)))/2);
  }
  fprintf(file_h,"};\n\n\n");

  fprintf(file_h,"#endif /* _dB_tab_gain_h_ */ \n");
  exit(0);
}

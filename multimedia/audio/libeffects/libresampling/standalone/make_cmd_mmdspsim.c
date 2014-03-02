/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   make_cmd_mmdspsim.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	FILE *f1;
	char testname[50];
	float test_time;
	if ((f1=fopen("test_time","r t"))==0)
		return 0;
	fscanf(f1,"%f\n",&test_time);
	fclose(f1);
	if ((f1=fopen("cmd_mmdspsim","r t"))==0)
		return 0;
	fscanf(f1,"%s %s\n",testname,testname);
	fclose(f1);
	f1=fopen("cmd_mmdspsim","w t");
	fprintf(f1,"file %s\n",testname);
	fprintf(f1,"enable oldprofile\n");
	fprintf(f1,"oldprofile -fx lib*/*\n");
	fprintf(f1,"break exit\n");
	fprintf(f1,"run\n");
	if (test_time<.01)
	{
		fprintf(f1,"oldprofile -c -a -u cycles -o %s.prof\n",testname);
	}
	else
	{
		fprintf(f1,"oldprofile -c -a -d %f -o %s.prof\n",test_time/1000.,testname);
	}
	fprintf(f1,"cont\n");
	fclose(f1);
	return 0;
} /* end main() */


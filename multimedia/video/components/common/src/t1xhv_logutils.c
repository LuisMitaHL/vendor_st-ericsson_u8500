/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*******************************************************************************
 * Utilities for trace
 *------------------------------------------------------------------------
 * Created by Philippe Rochette on Fri Sep 21 15:45:58 2001
 * $Log$
 * (10/10/2002) Revision /main/dev_rochettp/5 by rochettp :
 * 	Updated structure and types
 * 
 * (2/7/2002) Revision /main/dev_rochettp/6 by rochettp :
 * 	Validation made on mp4 mode. Added HW I/O for PXP/MECC. 
 *  Error resilience implemented, but not validated. Still optimization to do.
 * 
 * (1/8/2002) Revision /main/dev_rochettp/5 by rochettp :
 * 	Modif: Now, 2 define are needed, M4D_TRACE and M4D_TRACE_LEVEL=value
 * 
 * (12/6/2001) Revision /main/dev_rochettp/4 by rochettp :
 * 	On ne s'en lasse pas
 * 
 * (12/6/2001) Revision /main/dev_rochettp/3 by rochettp :
 * 	Plus de test.
 * 
 * (12/6/2001) Revision /main/dev_rochettp/2 by rochettp :
 * 	Another checkin trigger test.
 * 
 ******************************************************************************/
#define _LOGUTILS_C_

#ifdef T1XHV_TRACE
/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include <stdio.h>
#include <t1xhv_types.h>
#include <t1xhv_logutils.h>

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/
#ifdef BIT_TRUE
#define LOG_FILE_NAME "log_bitstream.sun"
#define RESULT_FILE_NAME "log_result.sun"
#else 
#define LOG_FILE_NAME "log_bitstream.dsp"
#define RESULT_FILE_NAME "log_result.dsp"
#endif
/*------------------------------------------------------------------------
 * Global Variables							       
 *----------------------------------------------------------------------*/
FILE *FP_log_bitstream, *FP_log_result;

/*****************************************************************************/
void loginit(void)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
    /* Open file for bitstream */
    FP_log_bitstream=fopen(LOG_FILE_NAME,"w");
    if(FP_log_bitstream==NULL) error("logfile for bitstream could not be created...\n");
    fprintf(FP_log_bitstream,"File created---> Log of bitstream\n");
    fprintf(FP_log_bitstream,"%35s\t%9s\t%9s\t%32s\n\n","Variable","Bits","Hex val.","Bin Val.");

    /* Open file for result */
    FP_log_result=fopen(RESULT_FILE_NAME,"w");
    if(FP_log_result==NULL) error("logfile for bitstream could not be created...\n");
    fprintf(FP_log_result,"File created---> Result of bitstream\n");
    fprintf(FP_log_result,"%35s\t%8s\n\n","Variable","Value");

}

/*****************************************************************************/
void logclose(void)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
    if(FP_log_bitstream!=NULL)
		{ 
			fprintf(FP_log_bitstream,"\n****\nEnd of file (log of bitstream)\n");
			fclose(FP_log_bitstream);
		}
    if(FP_log_result!=NULL) 	
		{ 
			fprintf(FP_log_result,"\n****\nEnd of file (log of result)\n");
			fclose(FP_log_result);
		}
}

/*****************************************************************************/
void logvalue(int level, char *name, t_uint32 value, int nb_bits)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_bitstream,"%35s\t",name);
		fprintf(FP_log_bitstream,"(%2d bits)\t",nb_bits);
		fprinthex(FP_log_bitstream,value,nb_bits);
		fprintbits(FP_log_bitstream,value,nb_bits);
		fprintf(FP_log_bitstream,"\n");
	}
}

/*****************************************************************************/
void logresult(int level, char *name, int value, int nb_bits, int result)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_bitstream,"%35s\t",name);
		fprintbits(FP_log_bitstream,value,nb_bits);
		fprintf(FP_log_bitstream,"(%2d bits)\t",nb_bits);
		fprintf(FP_log_bitstream,"%8d\n",result);
	}
}

/*****************************************************************************/
void logresult2(int level, char *name, int value, int nb_bits, int result1, int result2)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_bitstream,"%35s\t",name);
		fprintbits(FP_log_bitstream,value,nb_bits);
		fprintf(FP_log_bitstream,"(%2d bits)\t",nb_bits);
		fprintf(FP_log_bitstream,"%8d - %8d\n",result1,result2);
	}
}
/*****************************************************************************/
void logonlyresult(int level, char *name, int result)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_result,"%35s\t",name);
		fprintf(FP_log_result,"%8d\n",result);
	}
}

/*****************************************************************************/
void logonlyresult2(int level, char *name, int result1, int result2)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_result,"%35s\t",name);
		fprintf(FP_log_result,"%8d, %8d\n",result1,result2);
	}
}

/*****************************************************************************/
void logonlyresult3(int level, char *name, int result1, int result2, int result3)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_result,"%35s\t",name);
		fprintf(FP_log_result,"%8d, %8d, %8d\n",result1,result2,result3);
	}
}

/*****************************************************************************/
void logonlyresult4( int level, char *name, int escape, int result1, int result2, int result3)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_result,"%35s\t",name);
		fprintf(FP_log_result,"%8d - %8d, %8d, %8d\n",escape, result1,result2,result3);
	}
}


/*****************************************************************************/
void logonlyresulthex(int level, char *name, int result)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_result,"%35s\t",name);
		fprintf(FP_log_result,"  0x%.4X\n",result);
	}
}

/*****************************************************************************/
void logonlyresultbin(int level, char *name, int result, int length)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
    int i,j;
    char string[256];

	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_result,"%35s\t0xb",name);
		i=0;
		while(result>0) {string[i++]=(result&1)+'0';result>>=1;}
		for(j=0;j<length-i;j++)
			{
				fprintf(FP_log_result,"0");
			}
		for(i--;i>=0;i--)
			{
				fputc(string[i],FP_log_result);
			}
	    fprintf(FP_log_result,"\n");
	}
}
/*****************************************************************************/
void logonlyresultstring(int level, char *name, char *result)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {

		fprintf(FP_log_result,"%35s\t",name);
		fprintf(FP_log_result,"%10s\n",result);
	}
}

/*****************************************************************************/
void logline(int level, char *name, int value)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_bitstream,"%-4s %2d - %s %40s\n","####",value,name,"########################################");
		fprintf(FP_log_result,"%-4s %2d - %s %40s\n","####",value,name,"########################################");
	}
}

/*****************************************************************************/
void logseparator(int level, char *name)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
	if(level<=T1XHV_TRACE_LEVEL) {
		fprintf(FP_log_bitstream,"%16s %4s %s %4s\n"," ","----",name,"----");
		fprintf(FP_log_result,"%16s %4s %s %4s\n"," ","----",name,"----");
	}
}
/*****************************************************************************/
void fprinthex(FILE *fp, t_uint32 value, int nb_bits)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
    t_uint32 i;
	t_sint32 space;
	t_sint32 number;

    if(nb_bits<0) error("fprinthex: Nb of bits < 0");
    if(nb_bits>32) error("fprinthex: Nb of bits > 32");

	space = 8-((nb_bits>>2)-!(nb_bits&0x3));
    for(i=space;i>0;i--)
		fprintf(fp," ");

	number=((1<<(((nb_bits>>2)-!(nb_bits&0x3))*4))-1);

    for(i=number;i>0;(i>>=4))
		if(i>value) fprintf(fp,"0");

    fprintf(fp,"%lXh\t",value);

}

/*****************************************************************************/
void fprintbits(FILE *fp, t_uint32 value, int nb_bits)
/*-----------------------------------------------------------------------------
 * IN  : 
 * OUT :
 * aim of the function
 *-----------------------------------------------------------------------------
 * particularities
 ******************************************************************************/
{
    t_sint32 i;
    if(nb_bits<0) error("fprintbits: Nb of bits < 0");
    if(nb_bits>32) error("fprintbits: Nb of bits > 32");
    for(i=32-nb_bits-nb_bits/4+!(nb_bits%4);i>0;i--)
		fprintf(fp," ");
    i=1;
    for(i=(i<<(nb_bits-1));i>0;(i>>=1))	
		fprintf(fp,"%1d%s",(value&i)==0?0:1,((i&((t_uint32)0x11111110))!=0)?" ":"");
	
	fprintf(fp,"b\t");

}
#endif
/* END of logutils.c */

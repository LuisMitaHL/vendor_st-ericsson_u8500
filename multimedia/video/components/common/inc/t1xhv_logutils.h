/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*******************************************************************************
 * Created by Philippe Rochette on Fri Sep 21 15:52:33 2001
 * $Log$
 * 
 * (10/10/2002) Revision /main/dev_rochettp/3 by rochettp :
 * 	Updated structure and types
 * 
 * (7/18/2002) Revision /main/1 by rochettp :
 * 	
 * 
 * (7/15/2002) Revision /main/dev_rochettp/2 by rochettp :
 * 	
 * 
 *
 * (1/8/2002) Revision /main/dev_rochettp/1 by rochettp :
 * 	First release
 * 
 ******************************************************************************/


#ifndef _LOGUTILS_H_
#define _LOGUTILS_H_


/* Private Part */
#ifdef _LOGUTILS_C_



/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Private functions
 *----------------------------------------------------------------------*/
void fprintbits(FILE *fp, t_uint32 value, int nb_bits);
void fprinthex(FILE *fp, t_uint32 value, int nb_bits);

#endif

#ifdef T1XHV_TRACE
/*------------------------------------------------------------------------
 * Public functions
 *----------------------------------------------------------------------*/
void logvalue(int level, char *name, t_uint32 value, int nb_bits);
void loginit(void);
void logline(int level, char *name, int value);
void logseparator(int level, char *name);
void logclose(void);
void logresult(int level, char *name, int value, int nb_bits, int result);
void logresult2(int level, char *name, int value, int nb_bits, int result1, int result2);
void logonlyresult(int level, char *name, int result);
void logonlyresult3(int level, char *name, int result1, int result2, int result3);
void logonlyresult4(int level, char *name, int escape, int result1, int result2, int result3);
void logonlyresult2(int level, char *name, int result1, int result2);
void logonlyresulthex(int level, char *name, int result);
void logonlyresultbin(int level, char *name, int result, int length);
void logonlyresultstring(int level, char *name, char *result);
#else

#define logvalue( level, name,  value,  nb_bits)
#define loginit()
#define logline( level, name,  value)
#define logseparator( level, name)
#define logclose()
#define logresult( level, name,  value,  nb_bits,  result)
#define logresult2( level, name,  value,  nb_bits,  result1,  result2)
#define logonlyresult( level, name,  result)
#define logonlyresult3( level, name,  result1,  result2,  result3)
#define logonlyresult4( level, name,  escape,  result1,  result2,  result3)
#define logonlyresult2( level, name,  result1,  result2)
#define logonlyresulthex( level, name,  result)
#define logonlyresultbin( level, name,  result,  length)
#define logonlyresultstring( level, name, result)
#endif

#endif /* _LOGUTILS_H_ */



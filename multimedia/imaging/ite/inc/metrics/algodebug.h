/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */


#ifndef __INCLUDE_ALGO_DEBUG_H__
#define __INCLUDE_ALGO_DEBUG_H__

/*
 * Includes 
 */
#ifndef WIN32
#include <los/api/los_api.h>
#else
#include <stdio.h>
#endif


#ifndef WIN32
#define ALGO_Log_0(a) LOS_Log(a, NULL, NULL, NULL, NULL, NULL, NULL)
#define ALGO_Log_1(a,b) LOS_Log(a,b, NULL, NULL, NULL, NULL, NULL)
#define ALGO_Log_2(a,b,c) LOS_Log(a,b,c, NULL, NULL, NULL, NULL)
#define ALGO_Log_3(a,b,c,d) LOS_Log(a,b,c,d, NULL, NULL, NULL)
#define ALGO_Log_4(a,b,c,d,e) LOS_Log(a,b,c,d,e, NULL, NULL)
#define ALGO_Log_5(a,b,c,d,e,f) LOS_Log(a,b,c,d,e,f, NULL)
#define ALGO_Log_6(a,b,c,d,e,f,g) LOS_Log(a,b,c,d,e,f,g)
#define ALGO_Log_end() 

#else

//	#define ALGO_LOG_IN_FILE 1

	#if ALGO_LOG_IN_FILE != 1
		#define ALGO_Log_0(a) printf(a)
		#define ALGO_Log_1(a,b) printf(a,b)
		#define ALGO_Log_2(a,b,c) printf(a,b,c)
		#define ALGO_Log_3(a,b,c,d) printf(a,b,c,d)
		#define ALGO_Log_4(a,b,c,d,e) printf(a,b,c,d,e)
		#define ALGO_Log_5(a,b,c,d,e,f) printf(a,b,c,d,e,f)
		#define ALGO_Log_6(a,b,c,d,e,f,g) printf(a,b,c,d,e,f,g)
		#define ALGO_Log_end() 
	#else
		#ifdef _ALGOINTERFACE_C_
		#define Alog_Extern
		#else
		#define Alog_Extern extern
		#endif

		Alog_Extern FILE* global_File_Log
		#ifdef _ALGOINTERFACE_C_
		= (FILE*)NULL;
		#else
		;
		#endif

		#define FILE_NAME_LOG_FILE "algo_log.txt"

		#define ALGO_Log_end {\
			if(global_File_Log != (FILE*)NULL) \
			{\
				printf("closing global_File_Log"\n); \
				fclose(global_File_Log);global_File_Log=(FILE*)NULL;\
			}\
			}\

		#define ALGO_Log_0(a) {\
			if(global_File_Log == (FILE*)NULL) \
			{\
				global_File_Log = fopen(FILE_NAME_LOG_FILE, "wb"); \
			}\
			fprintf(global_File_Log, a); \
			printf(a); \
			}\

		#define ALGO_Log_1(a,b) {\
			if(global_File_Log == (FILE*)NULL) \
			{\
				global_File_Log = fopen(FILE_NAME_LOG_FILE, "wb"); \
			}\
			fprintf(global_File_Log, a, b); \
			printf(a, b); \
			}\

		#define ALGO_Log_2(a,b,c) {\
			if(global_File_Log == (FILE*)NULL) \
			{\
				global_File_Log = fopen(FILE_NAME_LOG_FILE, "wb"); \
			}\
			fprintf(global_File_Log, a,b,c); \
			printf(a,b,c); \
			}\

		#define ALGO_Log_3(a,b,c,d) {\
			if(global_File_Log == (FILE*)NULL) \
			{\
				global_File_Log = fopen(FILE_NAME_LOG_FILE, "wb"); \
			}\
			fprintf(global_File_Log, a,b,c,d); \
			printf(a,b,c,d); \
			}\

		#define ALGO_Log_4(a,b,c,d,e) {\
			if(global_File_Log == (FILE*)NULL) \
			{\
				global_File_Log = fopen(FILE_NAME_LOG_FILE, "wb"); \
			}\
			fprintf(global_File_Log, a,b,c,d,e); \
			printf(a,b,c,d,e); \
			}\

		#define ALGO_Log_5(a,b,c,d,e,f) {\
			if(global_File_Log == (FILE*)NULL) \
			{\
				global_File_Log = fopen(FILE_NAME_LOG_FILE, "wb"); \
			}\
			fprintf(global_File_Log, a,b,c,d,e,f); \
			printf(a,b,c,d,e,f); \
			}\

		#define ALGO_Log_6(a,b,c,d,e,f,g) {\
			if(global_File_Log == (FILE*)NULL) \
			{\
				global_File_Log = fopen(FILE_NAME_LOG_FILE, "wb"); \
			}\
			fprintf(global_File_Log, a,b,c,d,e,f,g); \
			printf(a,b,c,d,e,f,g); \
			}\

	#endif

#endif


#endif	//__INCLUDE_ALGO_DEBUG_H__



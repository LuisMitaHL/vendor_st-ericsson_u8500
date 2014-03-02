/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __ITE_TESTENV_UTILS_H
#define __ITE_TESTENV_UTILS_H

//For Linux
#include <inc/type.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "grab_types.idt.h"
#include "ite_sia_buffer.h"
#include "algotypedefs.h"

#include "ite_recordsystem_time.h"

typedef enum error_code{EOK=0,EFMT =-1}error_code;

/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
#define MAX_STRING_LENGTH 512
#define MAX_ARGS 10
#define MAX_CHAR                256
#define FILENAME_LENGTH         128

#ifdef ABS
#undef ABS
#define ABS(x)  (((x) < 0) ? ((x) * (-1)) : (x))
#endif

#ifdef MAX
#undef MAX
#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#endif

#ifdef MIN
#undef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

/*------------------------------------------------------------------------- ------------------------*
* Enums                                                                                                                                                                  *
*---------------------------------------------------------------------------------------------------*
* Usage of MMTE_TEST macros                                                                                                                                  *
* 1. MMTE_TEST_START(name, path, description)   should be the first  macro to be called                                               *
* 2. It should be followed by  MMTE_TEST_NEXT(comment)                                                                                           *
* 3. MMTE_TEST_PASSED() OR MMTE_TEST_FAILED() should be called if test passes or fails                                            *
* 4. For tests having intermediate results please call MMTE_RESULT_PASSED() or MMTE_RESULT_FAILED()                       *
* 5. If you are using MMTE_RESULT_PASSED/FAILED then make sure to use MMTE_TEST_END() at the end of the test       *
* 6  After using MMTE_TEST_END() the only macro that can be used is MMTE_TEST_START(name, path, description)          *
* 7. If you use any of the MACROS in a random order you can see an invalid call being printed on the screen                    *
* 8. Do not use the macros in combination with mmte_test functions,either use mmte_test functions or MMTE_TEST macros*
*----------------------------------------------------------------------------------------------------*/
typedef enum
{
 MMTE_TEST_IDLE = 0,
 MMTE_TEST_STARTED = 1,
 MMTE_TEST_RESULT_SET = 2,
 MMTE_TEST_ERROR = 99
}e_MMTE_TEST_STATE;

/*--------------------------------------------------------------------------*
 * Externs/Globals                                                                  *
 *--------------------------------------------------------------------------*/
extern char g_filename[FILENAME_LENGTH];
extern e_MMTE_TEST_STATE mmte_test_state;

/*--------------------------------------------------------------------------*
 * Generic Utilities                                                                                                           *
 *--------------------------------------------------------------------------*/

#define  MMTE_TEST_FOLDER_CREATE(path)                        \
                                                               if ((mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))== -1)\
                                                               {if (EEXIST != errno){CLI_disp_msg("\nERROR FOLDER NOT CREATED, ERRORNO:%d\n",errno);}}\
                                                               else\
                                                               {CLI_disp_msg("\nFOLDER %s CREATED !!\n",path);}

#define  MMTE_TEST_START(name, path, description)              \
                                                                if ((mmte_test_state != MMTE_TEST_IDLE)&&(mmte_test_state == MMTE_TEST_ERROR)&&(mmte_test_state != MMTE_TEST_RESULT_SET)) {LOS_Log("MMTE_TEST_START invalid call"); mmte_test_state = MMTE_TEST_ERROR;} \
                                                                if (usecase.sensor==0) {snprintf(g_filename,sizeof(g_filename),"Cam0_");} \
                                                                else {snprintf(g_filename,sizeof(g_filename),"Cam1_");}                 \
                                                                if ((strlen(name) + strlen(g_filename)) >= FILENAME_LENGTH) {LOS_Log("MMTE_TEST_START error: too long filename");mmte_test_state = MMTE_TEST_ERROR;}\
                                                                strcat(g_filename, name);                                  \
                                                                if (g_out_path[0] != '\0') {LOS_Log("MMTE_TEST_START error: g_out_path not empty\n"); mmte_test_state = MMTE_TEST_ERROR;} \
                                                                snprintf(g_out_path,sizeof(g_out_path),IMAGING_PATH); \
                                                                strcat(g_out_path, path);                                  \
                                                                MMTE_TEST_FOLDER_CREATE(g_out_path);\
                                                                mmte_testStart(g_filename, description, g_out_path) ;\
                                                                mmte_test_state = MMTE_TEST_STARTED


#define  MMTE_TEST_END()                                        \
                                                                if ((mmte_test_state != MMTE_TEST_STARTED)&&(mmte_test_state != MMTE_TEST_RESULT_SET)) \
                                                                {LOS_Log("MMTE_TEST_END invalid call"); mmte_test_state = MMTE_TEST_ERROR;} \
                                                                mmte_testEnd();                                              \
                                                                memset ( g_out_path, 0, KlogDirectoryLentgh*sizeof (char) ); \
                                                                mmte_test_state = MMTE_TEST_IDLE


#define  MMTE_RESULT_FAILED_PERFORMANCE()                                   \
                                                                 if ((mmte_test_state != MMTE_TEST_STARTED)&&(mmte_test_state != MMTE_TEST_RESULT_SET)&& (mmte_test_state != MMTE_TEST_IDLE)) \
                                                                {LOS_Log("MMTE_RESULT_FAILED invalid call"); mmte_test_state = MMTE_TEST_ERROR;}  \
                                                                PERFORMANCE_RESULT_FAILED();                                \
                                                                mmte_test_state = MMTE_TEST_RESULT_SET


#define  MMTE_RESULT_PASSED_PERFORMANCE()                                   \
                                                                 if ((mmte_test_state != MMTE_TEST_STARTED)&&(mmte_test_state != MMTE_TEST_RESULT_SET)&& (mmte_test_state != MMTE_TEST_IDLE)) \
                                                                {LOS_Log("MMTE_RESULT_PASSED invalid call"); mmte_test_state = MMTE_TEST_ERROR;} \
                                                                PERFORMANCE_RESULT_PASSED();\
                                                                mmte_test_state = MMTE_TEST_RESULT_SET


#define  MMTE_RESULT_SKIPPED()                                   \
                                                                LOS_Log("\nTEST SKIPPED\n");  \
                                                                mmte_testComment("FEATURE NOT SUPPORTED\n");\
                                                                mmte_testResult(TEST_SKIPPED);\
                                                                mmte_test_state = MMTE_TEST_RESULT_SET


#define  MMTE_RESULT_FAILED()                                   \
                                                                 if ((mmte_test_state != MMTE_TEST_STARTED)&&(mmte_test_state != MMTE_TEST_RESULT_SET)&& (mmte_test_state != MMTE_TEST_IDLE)) \
                                                                {LOS_Log("MMTE_RESULT_FAILED invalid call"); mmte_test_state = MMTE_TEST_ERROR;}  \
                                                                mmte_testResult(TEST_FAILED);                                \
                                                                mmte_test_state = MMTE_TEST_RESULT_SET


#define  MMTE_RESULT_PASSED()                                   \
                                                                if ((mmte_test_state != MMTE_TEST_STARTED)&&(mmte_test_state != MMTE_TEST_RESULT_SET)&& (mmte_test_state != MMTE_TEST_IDLE)) \
                                                                {LOS_Log("MMTE_RESULT_PASSED invalid call"); mmte_test_state = MMTE_TEST_ERROR;} \
                                                                mmte_testResult(TEST_PASSED);\
                                                                mmte_test_state = MMTE_TEST_RESULT_SET

#define  MMTE_TEST_FAILED_PERFORMACE()                                     \
                                                                MMTE_RESULT_FAILED_PERFORMANCE();                                        \
                                                                MMTE_TEST_END()

#define  MMTE_TEST_PASSED_PERFORMANCE()                                     \
                                                                MMTE_RESULT_PASSED_PERFORMANCE();                                        \
                                                                MMTE_TEST_END()

#define  MMTE_TEST_FAILED()                                     \
                                                                MMTE_RESULT_FAILED();                                        \
                                                                MMTE_TEST_END()

#define  MMTE_TEST_PASSED()                                     \
                                                                MMTE_RESULT_PASSED();                                        \
                                                                MMTE_TEST_END()


#define  MMTE_TEST_SKIPPED()                                     \
                                                                MMTE_RESULT_SKIPPED();                                        \
                                                                MMTE_TEST_END()

#define  MMTE_TEST_COMMENT(comment)                             \
                                                                if ((mmte_test_state != MMTE_TEST_STARTED)&&(mmte_test_state != MMTE_TEST_RESULT_SET)&& (mmte_test_state != MMTE_TEST_IDLE))\
                                                                {LOS_Log("MMTE_TEST_COMMENT invalid call"); mmte_test_state = MMTE_TEST_ERROR;}   \
                                                                LOS_Log(comment);                                            \
                                                                mmte_testComment(comment)

#define  MMTE_TEST_NEXT(comment)                                \
                                                                if ((mmte_test_state != MMTE_TEST_STARTED)&&(mmte_test_state != MMTE_TEST_RESULT_SET)&& (mmte_test_state != MMTE_TEST_IDLE)) \
                                                                {LOS_Log("MMTE_TEST_NEXT invalid call"); mmte_test_state = MMTE_TEST_ERROR;} \
                                                                LOS_Log(comment);                                            \
                                                                mmte_testNext(comment)



typedef void (* t_command_ptr)( int, char ** );

typedef struct
{
 char name[MAX_CHAR];    /*< Command name */
 t_command_ptr command;  /*< Pointer to the command function */
 char* help;			    /*< Help message  */
} t_command ;


#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
   SUBQCIF=0,
   SUBQCIFPLUS,
   QCIF,
   QCIFPLUS,
   CIF,
   CIFPLUS,
   QQVGA,
   QVGA,
   QVGAPLUS,
   VGA,
   NHD,
   VGAPLUS,
   NTSC,
   PAL,
   SVGA,
   WVGA,
   QHD,
   XGA,
   HD,
   SXGA,
   FULLHD,
   UXGA,
   TWOMEGA,
   THREEMEGA,
   FOURMEGA,
   FIVEMEGA,
   FIVEMEGA_W,
   EIGHTMEGA,
   EIGHTMEGA_W,
   LASTRESOLUTION
} e_resolution;

typedef struct
{
    char* pcSize;
    unsigned int uiSizeX;
    unsigned int uiSizeY;
} stStandardSizesType;

typedef enum
{
    PIPE_HR,
    PIPE_LR,
    PIPE_RAW
} ePipeType;

typedef enum
{
    WIDTH,
    HEIGHT
} eSizeType;

typedef struct
{
    char* pcFormat;
    enum e_grabFormat eFormatVal;
    unsigned int iXConstraint;
    unsigned int iYConstraint;
} stSupportedFormatType;

/*--------------------------------------------------------------------------*
 * Functions                                                                *
 *--------------------------------------------------------------------------*/


void ITE_GetArgs(char *ap_line, int *ap_nb_args, char *pArgs[]);

t_uint32 ITE_GetOptionValue(char * ap_option_line, const char * ap_option_pattern, t_uint32 a_default_value);
t_uint32 ITE_GetHexOptionValue(char * ap_option_line, const char * ap_option_pattern, t_uint32 a_default_value);
t_bool ITE_IsOptionValue(char *ap_option_line, const char * ap_option_pattern, t_uint32 a_default_value, t_uint32 * ap_value);
t_sint32 ITE_GetSignedOptionValue(char * ap_option_line, const char * ap_option_pattern, t_sint32 a_default_value);
t_bool ITE_GetOptionString(char *ap_result_string, char *ap_option_line, const char * ap_option_pattern, char * a_default_value);
t_bool ITE_GetOption(char * ap_option_line, const char * ap_option_pattern);
t_bool ITE_IsOption(char * ap_option_line, const char * ap_option_pattern);


t_uint8  ITE_ConvToInt8(char STR[6]);
t_uint16 ITE_ConvToInt16(char STR[6]);
t_uint32 ITE_ConvToInt32(char STR[6]);
float    ITE_ConvToFloat(char STR[6]);
float ITE_hex_to_float_integer_plus_fraction(t_uint32 input_num);


int ITE_GiveXSize(ePipeType ePipe, char* resolution, char* format);
int ITE_GiveYSize(ePipeType ePipe, char* resolution, char* format);
int ITE_GiveFormat(ePipeType ePipe,
                   char* pcReqFormat,
                   enum e_grabFormat* peFormatVal);
void ITE_GiveFormatStringFromEnum(enum e_grabFormat grbformat, char* name);
e_resolution ITE_GiveResolution(char* resolution);
int ITE_GiveXSizeFromEnum(ePipeType ePipe,
                          e_resolution resolution,
                          enum e_grabFormat grabformat);
int ITE_GiveYSizeFromEnum(ePipeType ePipe,
                          e_resolution resolution,
                          enum e_grabFormat grabformat);
void ITE_GiveStringNameFromEnum(e_resolution resolution, char *name);
void ITE_GiveStringResolution(char *ap_pipe,char *name);
void ITE_GiveStringBMLinput(char *name);
void ITE_GiveStringBMSoutput(char *name);
void ITE_GiveStringFormat(char *ap_pipe, char *name);
float ITE_GiveAspectRatioFromEnum(ePipeType ePipe,
                                  e_resolution resolution,
                                  enum e_grabFormat grabformat);

TAlgoImageFormat ITE_NMF_GiveMetricFormat(tps_siapicturebuffer p_buffer);
TUint32 ITE_NMF_ComputeStride(tps_siapicturebuffer p_buffer);

char ITE_GetHexDigit(int digit);
void ITE_itoa(int num, char *string, int radix);
void ITE_GetStringNameForBMSConfiguration(char *name);
void ITE_GetStringNameForBMSResolution(char *name);
char ITE_ConvToUppercase(char * STR);
char ITE_stricmp(char* s8Str1, char* s8Str2);
char ITE_strincmp(char* s8Str1, char* s8Str2, t_uint32 u32Size);


#ifdef __cplusplus
}
#endif

#endif /* __ITE_TESTENV_UTILS_H */


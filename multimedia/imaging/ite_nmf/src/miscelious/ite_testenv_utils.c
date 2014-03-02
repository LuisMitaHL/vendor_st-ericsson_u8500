/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "cli.h"
#include "ite_testenv_utils.h"
#include <inc/type.h>
#include "grab_types.idt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <los/api/los_api.h>

#include "ite_main.h"
#include "algotypedefs.h"
#include <assert.h>
#include <ctype.h>

extern ts_sia_usecase usecase;
extern char g_sensor_select[20];

static stStandardSizesType stStandardSizesArray[] =
{
    /* Size string  XSize   YSize */
    {"SUBQCIF",     128,    96},
    {"SUBQCIFPLUS", 144,    112},
    {"QQVGA",       160,    120},
    {"QCIF",        176,    144},
    {"QCIFPLUS",    192,    160},
    {"QVGA",        320,    240},
    {"QVGAPLUS",    352,    256},
    {"CIF",         352,    288},
    {"CIFPLUS",     384,    304},
    {"VGA",         640,    480},
    {"NHD",         640,    368},
    {"VGAPLUS",     672,    512},
    {"NTSC",        720,    480},
    {"PAL",         720,    576},
    {"SVGA",        800,    608},
    {"WVGA",        864,    480},
    {"QHD",         960,    540},
    {"QHD_1",       960,    544},
    {"XGA",         1024,   768},
    {"HD",          1280,   720},
    {"SXGA",        1280,   960},
    {"UXGA",        1600,   1200},
    {"2MEGA",       1608,   1200},
    {"TWOMEGA",     1608,   1200},
    {"FULLHD",      1920,   1088},
    {"3MEGA",       2064,   1536},
    {"THREEMEGA",   2064,   1536},
    {"3MEGAW",      2048,   1152},
    {"THREEMEGA_W", 2048,   1152},
    {"4MEGA",       2272,   1696},
    {"FOURMEGA",    2272,   1696},
    {"5MEGA",       2592,   1944},
    {"FIVEMEGA",    2592,   1944},
    {"5MEGAW",      2592,   1456},
    {"FIVEMEGA_W",  2592,   1456},
    {"8MEGA",       3264,   2448},
    {"EIGHTMEGA",   3264,   2448},
    {"8MEGAW",      3264,   1832},
    {"EIGHTMEGA_W", 3264,   1832},
    {"NULL",        0,      0}
};

#define GRBFMT_INVALID (-1)

/* Allignment for RGB/YUV formats is not taken cared off in grab nmf component
   on MMDSP hence we do the allignment here as per format restrictions.
*/
static stSupportedFormatType stSupportedFormatArrayLR[] =
{
    /* Format string    FormatVal                            XConstraint  YConstraint */
    {"YUV422RI",        GRBFMT_YUV422_RASTER_INTERLEAVED,      8,           1},
    {"ARGB4444",        GRBFMT_A4R4G4B4,                       8,           1},
    {"ARGB1555",        GRBFMT_A1R5G5B5,                       8,           1},
    {"RGB565",          GRBFMT_R5G6B5,                         8,           1},
    {"RGB888",          GRBFMT_R8G8B8,                         8,           1},
    {"ARGB8888",        GRBFMT_A8R8G8B8,                       8,           1},
    {"NULL",            GRBFMT_INVALID,                        1,           1}
};

/* Allignment for RGB/YUV formats is not taken cared off in grab nmf component
   on MMDSP hence we do the allignment here as per format restrictions.
*/
static stSupportedFormatType stSupportedFormatArrayHR[] =
{
    /* Format string    FormatVal                            XConstraint  YConstraint */
    {"YUV422RI",        GRBFMT_YUV422_RASTER_INTERLEAVED,      8,           1},
    {"YUV420P",         GRBFMT_YUV420_RASTER_PLANAR_I420,      16,          2},
    {"YUV420RP_I420",   GRBFMT_YUV420_RASTER_PLANAR_I420,      16,          2},
    {"YUV420RP_YU12",   GRBFMT_YUV420_RASTER_PLANAR_YV12,      16,          2},
    {"YUV422MB",        GRBFMT_YUV422_MB_SEMIPLANAR,           16,          16},
    {"YUV420MB_F",      GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED,  16,          16},
    {"YUV420MB_D",      GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED, 16,          16},
    {"YUV422RI_NV21",   GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21,  16,          16},
    {"YUV422RI_NV12",   GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12,  16,          16},
    {"RGB30",           GRBFMT_RGB30,                          1,           1},
    {"RGB101010",       GRBFMT_RGB30,                          1,           1},
    {"NULL",            GRBFMT_INVALID,                        1,           1}
};

/* Allignment for RAW formts (i.e. for BMS/BML) is already taken cared off in
   grab nmf component on MMDSP hence we don't do any allignment here.
   So XConstraint and YConstraint values are 1.
*/
static stSupportedFormatType stSupportedFormatArrayRAW[] =
{
    /* Format string    FormatVal                            XConstraint  YConstraint */
    {"RAW8",            GRBFMT_RAW8,                           1,           1},
    {"RAW12",           GRBFMT_RAW12,                          1,           1},
    {"FASTRAW8",        GRBFMT_FAST_RAW8,                      1,           1},
    {"FASTRAW12",       GRBFMT_FAST_RAW12,                     1,           1},
    {"BML2RAW8",        GRBFMT_BML2_RAW8,                      1,           1},
    {"BML2RAW12",       GRBFMT_BML2_RAW12,                     1,           1},
    {"NULL",            GRBFMT_INVALID,                        1,           1}
};

/************************************************************************************************/
/*       GLOBAL VARIABLES USED FOR RECORDING SYSTEM TIME AND FOR INITIALIZING TEST STATES                  */
/************************************************************************************************/
t_los_file                      *g_trace_file;
volatile ts_time_keeper         g_time_keeper[MAX_RECORDINGS]={{0,"xxx"}};
t_uint32                        g_time_keeper_index = 0 ;
t_uint32                        g_current_record_index = 0;
t_uint32                        g_test_perf_status=PERFORMANCE_SKIPPED;
char                            g_record_msg[MAX_CHAR];
volatile unsigned long long     g_time_diff = 0,g_time = 0;
char                            g_filename[FILENAME_LENGTH];
e_MMTE_TEST_STATE               mmte_test_state = MMTE_TEST_IDLE;

/*************************************************************************************************/

/** ITE_ApplyAlignmentConstraint: Apply any hw alignment constraints due to the
    format in use.

    @param [in] ePipe
        Pipe to be used
    @param [in] sizeType
        Width or Height
    @param [in] size
        Current size of Width or Height
    @param [in] format
        Current format
    @return Image size after adjustment
 */
int ITE_ApplyAlignmentConstraint(ePipeType ePipe,
                                 eSizeType eTypeOfSize,
                                 unsigned int uiSize,
                                 char* pcFormat)
{
    int iConstraint = 1;
    int index = 0;
    stSupportedFormatType* pstSupportedFormats = NULL;

    if (NULL != pcFormat)
    {
        switch (ePipe)
        {
            case PIPE_HR:
                pstSupportedFormats = stSupportedFormatArrayHR;
                break;

            case PIPE_LR:
                pstSupportedFormats = stSupportedFormatArrayLR;
                break;

            case PIPE_RAW:
                pstSupportedFormats = stSupportedFormatArrayRAW;
                break;

            default:
                break;
        }

        if (NULL != pstSupportedFormats)
        {
            while (0 != ITE_stricmp(pstSupportedFormats[index].pcFormat, "NULL"))
            {
                if (0 == ITE_stricmp(pstSupportedFormats[index].pcFormat, pcFormat))
                {
                    if (WIDTH == eTypeOfSize)
                    {
                        iConstraint = pstSupportedFormats[index].iXConstraint;
                    }
                    else
                    {
                        iConstraint = pstSupportedFormats[index].iYConstraint;
                    }
                    break;
                }
                else
                {
                    index++;
                }
            }
        }
        else
        {
            LOS_Log("ITE_ApplyAlignmentConstraint: pstSupportedFormats should "
                    "never be NULL. Asserting!!!\n");
            ITE_NMF_ASSERT(pstSupportedFormats);
        }
    }
    else
    {
        LOS_Log("ITE_ApplyAlignmentConstraint: pcFormat should never be NULL. "
                "Asserting!!!\n");
        ITE_NMF_ASSERT(pcFormat);
    }

    if (0 != uiSize % iConstraint)
    {
        uiSize += iConstraint;
        uiSize = (uiSize / iConstraint) * iConstraint;

        LOS_Log("ITE_ApplyAlignmentConstraint: %s doesn't meet allignment "
                "constraints for %s format hence changed to %d\n",
                ((WIDTH == eTypeOfSize) ? "width" : "height"),
                pcFormat,
                uiSize);
    }

   return (uiSize);
}

/* ------------------------------------------------------------------------
   FUNCTION : ITE_GetArgs
   PURPOSE  : Parse the input line
   ------------------------------------------------------------------------ */
void ITE_GetArgs(char *ap_line, int *ap_nb_args, char *pArgs[])
{
   t_uint32 nbArgs = 0;
   char *pArg;
   char line[MAX_STRING_LENGTH];
   t_uint32 i, j;

   strcpy(line, ap_line);

   // First stage: count the number of args
   pArg =  strtok (line, " ");

   while (pArg != NULL)
   {
      nbArgs++;
      pArg = strtok (NULL, " ");
   }

   for (i = 0; i < nbArgs; i++)
   {
      if (i == 0)
      {
         pArg = strtok (ap_line, " ");
      }
      else
      {
         pArg = strtok (NULL, " \0\n");
      }
      // Remove EOL
      for (j = 0; j < strlen(pArg); j++)
      {
         if (pArg[j] == '\n')
         {
            pArg[j] ='\0';
            break;
         }
      }
      strcpy(pArgs[i], pArg);
   }
   *ap_nb_args = nbArgs;

}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_IsOptionValue
   PURPOSE  : Get an option value boolean
   ------------------------------------------------------------------------ */
t_bool ITE_IsOptionValue( char       *ap_option_line    ,
                          const char *ap_option_pattern ,
                          t_uint32    a_default_value   ,
                          t_uint32   *ap_value          )
{
   t_bool isFound;
   if (ITE_GetOption(ap_option_line, ap_option_pattern))
   {
      isFound = (t_bool)TRUE;
      *ap_value = ITE_GetOptionValue(ap_option_line, ap_option_pattern, a_default_value);
   }
   else
   {
      *ap_value = a_default_value;
      isFound = (t_bool)FALSE;
   }
   return isFound;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_GetOptionValue
   PURPOSE  : Get an option value
   ------------------------------------------------------------------------ */
t_uint32 ITE_GetOptionValue( char       *ap_option_line    ,
                             const char *ap_option_pattern ,
                             t_uint32    a_default_value   )
{
   char OptionLine[MAX_STRING_LENGTH];
   char OptionPattern[MAX_STRING_LENGTH];
   char Rest[MAX_STRING_LENGTH];

   t_uint32 Value = a_default_value;
   char * pPatternStart;
   char * pPatternEnd;
/*   char * pStart;*/

   strcpy(OptionPattern, "_");
   strcat(OptionPattern, ap_option_pattern);
   strcpy(OptionLine, ap_option_line);
   pPatternStart = strstr(OptionLine, OptionPattern);

   if (pPatternStart != NULL)
   {
      // retrieve option value
      pPatternEnd = strchr(pPatternStart+strlen(OptionPattern), '_');
      if (pPatternEnd != NULL)
      {
         strcpy(Rest, pPatternEnd);
         // the following line removes the pattern from the original line
      }
      sscanf(strtok(pPatternStart+strlen(OptionPattern), "_"), "%d", (int*)&Value); //__NO_WARNING__
   }
   else
   {
      // Option not found, assign default value
   }
   return Value;
}



/* ------------------------------------------------------------------------
   FUNCTION : ITE_HexGetOptionValue
   PURPOSE  : Get an hexadecimal option value
   ------------------------------------------------------------------------ */
t_uint32 ITE_GetHexOptionValue( char       *ap_option_line    ,
                             const char *ap_option_pattern ,
                             t_uint32    a_default_value   )
{
   char OptionLine[MAX_STRING_LENGTH];
   char OptionPattern[MAX_STRING_LENGTH];
   char Rest[MAX_STRING_LENGTH];

   t_uint32 Value = a_default_value;
   char * pPatternStart;
   char * pPatternEnd;
/*   char * pStart;*/

   strcpy(OptionPattern, "_");
   strcat(OptionPattern, ap_option_pattern);
   strcat(OptionPattern, "0x");
   strcpy(OptionLine, ap_option_line);
   pPatternStart = strstr(OptionLine, OptionPattern);

   if (pPatternStart != NULL)
   {
      // retrieve option value
      pPatternEnd = strchr(pPatternStart+strlen(OptionPattern), '_');
      if (pPatternEnd != NULL)
      {
         strcpy(Rest, pPatternEnd);
         // the following line removes the pattern from the original line
      }
      sscanf(strtok(pPatternStart+strlen(OptionPattern), "_"), "%x", (unsigned int*)&Value); //__NO_WARNING__
   }
   else
   {
      // Option not found, assign default value
   }
   return Value;
}



/* ------------------------------------------------------------------------
   FUNCTION : ITE_GetSignedOptionValue
   PURPOSE  : Get a signed option value
   ------------------------------------------------------------------------ */
t_sint32 ITE_GetSignedOptionValue( char       *ap_option_line    ,
                                   const char *ap_option_pattern ,
                                   t_sint32    a_default_value   )
{
   char OptionLine[MAX_STRING_LENGTH];
   char OptionPattern[MAX_STRING_LENGTH];
   char Rest[MAX_STRING_LENGTH];

   t_sint32 Value = a_default_value;
   char * pPatternStart;
   char * pPatternEnd;
   char * pStart;

   strcpy(OptionPattern, "_");
   strcat(OptionPattern, ap_option_pattern);
   strcpy(OptionLine, ap_option_line);
   pPatternStart = strstr(OptionLine, OptionPattern);

   if (pPatternStart != NULL)
   {
      // retrieve option value
      pPatternEnd = strchr(pPatternStart+strlen(OptionPattern), '_');
      if (pPatternEnd != NULL)
      {
         strcpy(Rest, pPatternEnd);
         // the following line removes the pattern from the original line
      }
      else
      {
         pStart = strstr(ap_option_line, OptionPattern);
         pStart[0] = '\0';
      }
      sscanf(strtok(pPatternStart+strlen(OptionPattern), "_"), "%d", (int*)&Value); //__NO_WARNING
   }
   else
   {
      // Option not found, assign default value
   }
   return Value;
}



/* ------------------------------------------------------------------------
   FUNCTION : ITE_GetOption
   PURPOSE  : Get an option
   ------------------------------------------------------------------------ */
t_bool ITE_GetOption(char *ap_option_line, const char *ap_option_pattern)
{
   char OptionPattern[MAX_STRING_LENGTH];
/*   char Rest[MAX_STRING_LENGTH];*/
   char * pPatternStart;
   t_bool Result;

   strcpy(OptionPattern, "_");
   strcat(OptionPattern, ap_option_pattern);

   pPatternStart = strstr(ap_option_line, OptionPattern);
   if ( pPatternStart != NULL)
   {
      Result = (t_bool)TRUE;
   }
   else
   {
      Result = (t_bool)FALSE;
   }
   return Result;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_GetOptionString
   PURPOSE  : Get an option string
   ------------------------------------------------------------------------ */
t_bool ITE_GetOptionString(char *ap_result_string, char *ap_option_line,
                          const char * ap_option_pattern, char * a_default_value){
    char OptionLine[MAX_STRING_LENGTH];
    char OptionPattern[MAX_STRING_LENGTH];
    char Rest[MAX_STRING_LENGTH];

    char String[MAX_STRING_LENGTH];

    char * pPatternStart;
    char * pPatternEnd;
    char * pStart;
    t_bool Result=(t_bool)FALSE;

    strcpy(String, a_default_value);
    strcpy(ap_result_string, String);

    strcpy(OptionPattern, "_");
    strcat(OptionPattern, ap_option_pattern);
    strcpy(OptionLine, ap_option_line);
    pPatternStart = strstr(OptionLine, OptionPattern);

    if (pPatternStart != NULL)
        {
        // retrieve option value
        pPatternEnd = strchr(pPatternStart+strlen(OptionPattern), '_');
        if (pPatternEnd != NULL)
            {
            strcpy(Rest, pPatternEnd);
            // the following line removes the pattern from the original line
            }
        else
            {
            pStart = strstr(ap_option_line, OptionPattern);
            pStart[0] = '\0';
            }
        sscanf(strtok(pPatternStart+strlen(OptionPattern), "_"), "%s", String);
        strcpy(ap_result_string, String);
        Result = (t_bool)TRUE;
        }
    return Result;
}


/* ------------------------------------------------------------------------
   FUNCTION : ITE_IsOption
   PURPOSE  : Get an option boolean
   ------------------------------------------------------------------------ */
t_bool ITE_IsOption(char *ap_option_line, const char *ap_option_pattern)
{
   char OptionPattern[MAX_STRING_LENGTH];
   char * pPatternStart;
   t_bool Result;

   strcpy(OptionPattern, "_");
   strcat(OptionPattern, ap_option_pattern);

   pPatternStart = strstr(ap_option_line, OptionPattern);
   if ( pPatternStart != NULL)
   {
      Result = (t_bool)TRUE;
   }
   else
   {
      Result = (t_bool)FALSE;
   }
   return Result;
}

/** ITE_GiveSize: Get the image width/height corresponding to the resolution
    passed. This function will also apply any hw alignment constraints
    due to the format in use.

    @param [in] ePipe
        Pipe to be used
    @param [in] resolution
        Current resolution
    @param [in] format
        Current format
    @return Image Width/Height
 */
int ITE_GiveSize(ePipeType ePipe, eSizeType eTypeOfSize, char* resolution, char* format)
{
    unsigned int uiSize = 0;
    int index = 0;

    while (0 != ITE_stricmp(stStandardSizesArray[index].pcSize, "NULL"))
    {
        if (0 == ITE_stricmp(stStandardSizesArray[index].pcSize, resolution))
        {
            if (WIDTH == eTypeOfSize)
            {
                uiSize = stStandardSizesArray[index].uiSizeX;
            }
            else
            {
                uiSize = stStandardSizesArray[index].uiSizeY;
            }
            break;
        }
        else
        {
            index++;
        }
    }

    if ((0 == ITE_stricmp(stStandardSizesArray[index].pcSize, "NULL")) &&
        (0 == stStandardSizesArray[index].uiSizeX) &&
        (0 == stStandardSizesArray[index].uiSizeY))
    {
        /* Processing Width X Height format */
        if (WIDTH == eTypeOfSize)
        {
            sscanf(resolution, "%d%*[xX]%*d", &uiSize);
        }
        else
        {
            sscanf(resolution, "%*d%*[xX]%d", &uiSize);
        }
    }

    if (0 == uiSize)
    {
        LOS_Log("ITE_GiveSize: failed to compute Size. Asserting!!!\n");
        ITE_NMF_ASSERT(uiSize);
    }

    uiSize = ITE_ApplyAlignmentConstraint(ePipe, eTypeOfSize, uiSize, format);

    return(uiSize);
}

/** ITE_GiveXSize: Get the image width corresponding to the resolution
    passed. This function will also apply any hw alignment constraints
    due to the format in use.

    @param [in] ePipe
        Pipe to be used
    @param [in] sizeType
        Width or Height
    @param [in] resolution
        Current resolution
    @param [in] format
        Current format
    @return Image Width
 */
int ITE_GiveXSize(ePipeType ePipe, char* resolution, char* format)
{
    return ITE_GiveSize(ePipe, WIDTH, resolution, format);
}

/* ITE_GiveXSizeFromEnum: Return horizontal size corresponding to enum
   resolution passed.

    @param [in] ePipe
        Pipe to be used
    @param [in] resolution
        Current resolution
    @param [in] format
        Current format
    @return Image Width
*/
int ITE_GiveXSizeFromEnum(ePipeType ePipe,
                          e_resolution resolution,
                          enum e_grabFormat grabformat)
{
    char resolutionName[16];
    char formatName[16];

    ITE_GiveStringNameFromEnum(resolution, resolutionName);
    ITE_GiveFormatStringFromEnum(grabformat, formatName);

    return(ITE_GiveXSize(ePipe, resolutionName, formatName));
}

/** ITE_GiveYSize: Get the image height corresponding to the resolution
    passed. This function will also apply any hw alignment constraints
    due to the format in use.

    @param [in] ePipe
        Pipe to be used
    @param [in] resolution
        Current resolution
    @param [in] format
        Current format
    @return Image Height
*/
int ITE_GiveYSize(ePipeType ePipe, char* resolution, char* format)
{
    return ITE_GiveSize(ePipe, HEIGHT, resolution, format);
}

/* ITE_GiveYSizeFromEnum: Return vertical size corresponding to enum
   resolution passed.

    @param [in] ePipe
        Pipe to be used
    @param [in] resolution
        Current resolution
    @param [in] format
        Current format
    @return Image Width
*/
int ITE_GiveYSizeFromEnum(ePipeType ePipe,
                          e_resolution resolution,
                          enum e_grabFormat grabformat)
{
    char resolutionName[16];
    char formatName[16];

    ITE_GiveStringNameFromEnum(resolution, resolutionName);
    ITE_GiveFormatStringFromEnum(grabformat, formatName);

    return(ITE_GiveYSize(ePipe, resolutionName, formatName));
}

/** ITE_GiveAspectRatioFromEnum: Get the aspect ratio for given format.

    @param [in] ePipe
        Pipe to be used
    @param [in] resolution
        Current resolution
    @param [in] grabformat
        Current format
    @return aspect ratio
 */
float ITE_GiveAspectRatioFromEnum(ePipeType ePipe,
                                  e_resolution resolution,
                                  enum e_grabFormat grabformat)
{
    return((float) ITE_GiveXSizeFromEnum(ePipe, resolution, grabformat) /
           (float) ITE_GiveYSizeFromEnum(ePipe, resolution, grabformat));
}

/** ITE_GiveFormat: Get the grab format value corresponding to user passed string.
    This function checks supported formats for given pipe.

    @param [in] ePipe
        Pipe to be used
    @param [in] pcReqFormat
        Requested format string
    @param [out] peFormatVal
        Corresponding format enum value if format is supported
    @return Status
 */
int ITE_GiveFormat(ePipeType ePipe, char* pcReqFormat, enum e_grabFormat* peFormatVal)
{
    int ret = EFMT;
    int index = 0;
    stSupportedFormatType* pstSupportedFormats = NULL;

    if (NULL != peFormatVal)
    {
        switch (ePipe)
        {
            case PIPE_HR:
                pstSupportedFormats = stSupportedFormatArrayHR;
                break;

            case PIPE_LR:
                pstSupportedFormats = stSupportedFormatArrayLR;
                break;

            case PIPE_RAW:
                pstSupportedFormats = stSupportedFormatArrayRAW;
                break;

            default:
                break;
        }

        if (NULL != pstSupportedFormats)
        {
            while (0 != ITE_stricmp(pstSupportedFormats[index].pcFormat, "NULL"))
            {
                if (0 == ITE_stricmp(pstSupportedFormats[index].pcFormat, pcReqFormat))
                {
                    *peFormatVal = pstSupportedFormats[index].eFormatVal;
                    ret = EOK;

                    LOS_Log("ITE_GiveFormat: %s is valid format for pipe %d.\n",
                            pcReqFormat,
                            ePipe);
                    break;
                }
                else
                {
                    index++;
                }
            }

            if (EOK != ret)
            {
                LOS_Log("ITE_GiveFormat: %s is invalid format for pipe %d. Asserting!!!\n",
                        pcReqFormat,
                        ePipe);
                ITE_NMF_ASSERT(NULL);
            }
        }
        else
        {
            LOS_Log("ITE_GiveFormat: %s format requested for invalid pipe %d. "
                    "pstSupportedFormats should never be NULL. Asserting!!!\n",
                    pcReqFormat,
                    ePipe);
            ITE_NMF_ASSERT(pstSupportedFormats);
        }
    }
    else
    {
        LOS_Log("ITE_GiveFormat: invalid param passed. peFormatVal should never "
                "be NULL. Asserting!!!\n");
        ITE_NMF_ASSERT(peFormatVal);
    }

    return ret;
}

/***********************************************************************************/
/* return string GrbFormat corresponding to enum GrbFormat             */
/***********************************************************************************/
void ITE_GiveFormatStringFromEnum(enum e_grabFormat grbformat, char* name)
{
    switch (grbformat)
    {
        case (GRBFMT_YUV422_RASTER_INTERLEAVED): sprintf(name,"YUV422RI"); break;
        case (GRBFMT_YUV420_RASTER_PLANAR_I420): sprintf(name,"YUV420RP_I420"); break;
        case (GRBFMT_YUV420_RASTER_PLANAR_YV12): sprintf(name,"YUV420RP_YU12"); break;
        case (GRBFMT_YUV422_MB_SEMIPLANAR): sprintf(name,"YUV422MB"); break;
        case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED): sprintf(name,"YUV420MB_F"); break;
        case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED): sprintf(name,"YUV420MB_D"); break;
        case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21): sprintf(name,"YUV422RI_NV21"); break;
        case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12): sprintf(name,"YUV422RI_NV12"); break;
        case (GRBFMT_RGB30): sprintf(name,"RGB30"); break;
        case (GRBFMT_RAW8): sprintf(name,"RAW8"); break;
        case (GRBFMT_RAW12): sprintf(name,"RAW12"); break;
        case (GRBFMT_FAST_RAW8): sprintf(name,"FASTRAW8"); break;
        case (GRBFMT_FAST_RAW12): sprintf(name,"FASTRAW12"); break;
        case (GRBFMT_BML2_RAW8): sprintf(name,"BML2RAW8"); break;
        case (GRBFMT_BML2_RAW12): sprintf(name,"BML2RAW12"); break;
        case (GRBFMT_A4R4G4B4): sprintf(name,"ARGB4444"); break;
        case (GRBFMT_A1R5G5B5): sprintf(name,"ARGB1555"); break;
        case (GRBFMT_R5G6B5): sprintf(name,"RGB565"); break;
        case (GRBFMT_R8G8B8): sprintf(name,"RGB888"); break;
        case (GRBFMT_A8R8G8B8): sprintf(name,"ARGB8888"); break;
        default: sprintf(name,"ERROR"); break;
  }
}


/*************************************************************************/
/*    for Metrics                            */
/*  return metric enum format                        */
/*************************************************************************/
TAlgoImageFormat ITE_NMF_GiveMetricFormat(tps_siapicturebuffer p_buffer) {

 TAlgoImageFormat format = EImageFormatUnknown;  //__NO_WARNING__ Not set before being used

switch (p_buffer->Grb_colorformat) {
  case (GRBFMT_YUV422_RASTER_INTERLEAVED):
    format = EImageFormatYUV422Interleaved;
    break;
  case (GRBFMT_YUV422_MB_SEMIPLANAR):
    format = EImageFormatUnknown;
    break;
  case (GRBFMT_YUV420_RASTER_PLANAR_I420):
    format = EImageFormatYUV420PackedPlanarI420;
    //format = EImageFormatYUV420PackedPlanar;
    // = EImageFormatUnknown;
    break;
  case (GRBFMT_YUV420_RASTER_PLANAR_YV12):
    format = EImageFormatYUV420PackedPlanarI420;
    //format = EImageFormatYUV420PackedPlanar;
    break;
  case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
    format = EImageFormatYUV420MBSemiPlanarFiltered;
    break;
  case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
    format = EImageFormatYUV420MBSemiPlanarFiltered;
    //format = EImageFormatYUV420Planar;
    break;
  case (GRBFMT_RGB30):
    format = EImageFormatUnknown;
    break;
  case (GRBFMT_RAW8):
    format = EImageFormatRAW8;
    break;
  case (GRBFMT_FAST_RAW8):
    format = EImageFormatRAW8;
    break;
  case (GRBFMT_RAW12):
    format = EImageFormatRAW12;
    break;
  case (GRBFMT_FAST_RAW12):
    format = EImageFormatRAW12;
    break;
  case (GRBFMT_A4R4G4B4):
    //format = EImageFormatARGB4444;
    break;
  case (GRBFMT_A1R5G5B5):
    //format = EImageFormatARGB1555;
    break;
  case (GRBFMT_R5G6B5):
    //format = EImageFormatRGB16;
    break;
  case (GRBFMT_R8G8B8):
    //format = EImageFormatRGB24;
    break;
  case (GRBFMT_A8R8G8B8):
    //format = EImageFormatARGB8888;
    break;
  default :     // Just to avoid compilation Warning
    break;
    }
 return(format);
}

/*************************************************************************/
/*    for Metrics                            */
/*  return computed Stride                       */
/*************************************************************************/
TUint32 ITE_NMF_ComputeStride(tps_siapicturebuffer p_buffer) {

TUint32 Stride = 0; //__NO_WARNING__ Not set before being used
switch (p_buffer->Grb_colorformat) {
  case (GRBFMT_YUV422_RASTER_INTERLEAVED):
    Stride = p_buffer->framesizeX * 2;
    break;
  case (GRBFMT_YUV422_MB_SEMIPLANAR):
    Stride = p_buffer->framesizeX * 2;
    break;
  case (GRBFMT_YUV420_RASTER_PLANAR_I420):
    Stride = p_buffer->framesizeX * 3;
    Stride = Stride / 2;
    break;
  case (GRBFMT_YUV420_RASTER_PLANAR_YV12):
    Stride = p_buffer->framesizeX * 3;
    Stride = Stride / 2;
    break;
  case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):
    Stride = p_buffer->framesizeX * 3;
    Stride = Stride / 2;
    break;
  case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):
    Stride = p_buffer->framesizeX * 3;
    Stride = Stride / 2;
    break;
  case (GRBFMT_RGB30):
    Stride = p_buffer->framesizeX * 4;   // ???????
    break;
  case (GRBFMT_RAW8):
    Stride = p_buffer->framesizeX * 1;
    break;
  case (GRBFMT_FAST_RAW8):
    Stride = p_buffer->framesizeX * 1;
    break;
  case (GRBFMT_RAW12):
    Stride = p_buffer->framesizeX * 3;
    Stride = Stride / 2;
    break;
  case (GRBFMT_FAST_RAW12):
    Stride = p_buffer->framesizeX * 3;
    Stride = Stride / 2;
    break;
  case (GRBFMT_A4R4G4B4):
    Stride = p_buffer->framesizeX * 2;
    break;
  case (GRBFMT_A1R5G5B5):
    Stride = p_buffer->framesizeX * 2;
    break;
  case (GRBFMT_R5G6B5):
    Stride = p_buffer->framesizeX * 2;
    break;
  case (GRBFMT_R8G8B8):
    Stride = p_buffer->framesizeX * 3;
    break;
  case (GRBFMT_A8R8G8B8):
    Stride = p_buffer->framesizeX * 4;
    break;
  default :     // Just to avoid compilation Warning
    break;
    }
 return(Stride);

}




/* ---------------------------------------
   FUNCTION :
   PURPOSE  :
   --------------------------------------- */
float ITE_ConvToFloat(char STR[6]) {
volatile float tmp;

sscanf(STR,"%f",(float*)&tmp);

return(tmp);
}

/* ---------------------------------------
   FUNCTION :
   PURPOSE  :
   --------------------------------------- */
t_uint8 ITE_ConvToInt8(char STR[6]) {
volatile t_uint8 tmp;

//if (STR[1] == 'x') {
//   sscanf(STR,"%x",(t_uint8*)&tmp);}
//else {
   tmp = CLI_atoi(STR);//}
return(tmp);
}


/* ---------------------------------------
   FUNCTION :
   PURPOSE  :
   --------------------------------------- */
t_uint16 ITE_ConvToInt16(char STR[6]) {
volatile t_uint16 tmp;

//if (STR[1] == 'x') {
//   sscanf(STR,"%x",(int*)&tmp);}
//else {
   tmp = CLI_atoi(STR);//}
return(tmp);
}


/* ---------------------------------------
   FUNCTION :
   PURPOSE  :
   --------------------------------------- */
t_uint32 ITE_ConvToInt32(char STR[6]) {
volatile t_uint32 tmp;

//if (STR[1] == 'x') {
//   sscanf(STR,"%x",(t_uint32 *)&tmp);}
//else {
   tmp = CLI_atoi(STR);//}
return(tmp);
}

/* ---------------------------------------
   FUNCTION :
   PURPOSE  :
   --------------------------------------- */
char ITE_ConvToUppercase(char * STR) {
volatile  int index=0;
char letter;
//LOS_Log("Before Conversion :%s \n",STR);
  while (STR[index])
  {
    letter=STR[index];
    STR[index] = (toupper(letter));
    index++;
  }
//LOS_Log("After Conversion :%s \n",STR);

return 0;

}

char ITE_stricmp(char* s8Str1, char* s8Str2)
{
    t_uint32  u32Index = 0;

    if ((s8Str1 == NULL) || (s8Str2 == NULL))
    {
        ITE_NMF_ASSERT(NULL);
    }

    while (('\0' != s8Str1[u32Index]) && ('\0' != s8Str2[u32Index]))
    {
        if (toupper(s8Str1[u32Index]) != (toupper(s8Str2[u32Index])) )
        {
            return -1;
        }
        u32Index++;
    }

    if (('\0' != s8Str1[u32Index]) || ('\0' != s8Str2[u32Index]))
    {
        return -1;
    }

    return 0;
}

char ITE_strincmp(char* s8Str1, char* s8Str2, t_uint32 u32Size)
{
    t_uint32 u32Index = 0;

    if ((s8Str1 == NULL) || (s8Str2 == NULL) || (u32Size == 0))
    {
        ITE_NMF_ASSERT(NULL);
    }

    while (('\0' != s8Str1[u32Index]) && ('\0' != s8Str2[u32Index]) && (u32Index < u32Size))
    {
        if (toupper(s8Str1[u32Index]) != (toupper(s8Str2[u32Index])))
        {
            return -1;
        }
        u32Index++;
    }

    if (u32Index != u32Size)
    {
        return -1;
    }

    return 0;
}

/* ---------------------------------------
   FUNCTION :
   PURPOSE  :to convert hex values to float
   --------------------------------------- */
float ITE_hex_to_float_integer_plus_fraction(t_uint32 input_num)
{
 t_uint32  integer_part, fractional_part, power_1, n, multiply_factor;
 float     temp, fvalue  = 0.0;

 //t_uint32  input_num = (t_uint32)num & 0x0000FFFF;
 t_uint32  sign      = (input_num & 0x80000000) >> 31;
 t_uint32  exponent  = (input_num & 0x7F800000) >> 23;
 t_uint32  mantissa  = (input_num & 0x7FFFFF);

 exponent = exponent - 127;
 n = exponent; //__NO_WARNING__ not being set before being used

        if (exponent <= 23)
         {
              integer_part = (mantissa | 0x800000) >> (23 - exponent);
              fractional_part = mantissa & ( (0x1 << ( 23 - exponent )) - 1 );

              if (integer_part == 0 )
              {
                  fractional_part = (fractional_part | 0x800000);
                  n = 23 - exponent;
              }

              while(fractional_part > 1)
              {

                  temp = (fractional_part & 0x1) + 0.0 ;
                  power_1 = 1 << n ;
                  temp = temp/power_1 ;
                  fvalue = fvalue + temp;
                  n = n - 1;
                  fractional_part = fractional_part >> 1;
              }
         }
       else
         {
            integer_part = (mantissa |0x800000) << (exponent - 23);
         }

       multiply_factor = (sign == 1) ? (-1) : (1);

       fvalue = fvalue + integer_part  ;

       return (fvalue * multiply_factor);

}

/***********************************************************************************/
/* return in name the corresponding string of enum resolution              */
/***********************************************************************************/
void ITE_GiveStringNameFromEnum(e_resolution resolution, char *name)
{

switch (resolution)
{

 #ifdef samsung8M

    case (SUBQCIF)      :sprintf(name,"SUBQCIF")        ;break;
    case (SUBQCIFPLUS)  :sprintf(name,"SUBQCIFPLUS")    ;break;
    case (QCIF)         :sprintf(name,"QCIF")       ;break;
    case (QCIFPLUS)     :sprintf(name,"QCIFPLUS")       ;break;
    case (CIF)          :sprintf(name,"CIF")        ;break;
    case (CIFPLUS)      :sprintf(name,"CIFPLUS")        ;break;
    case (QQVGA)        :sprintf(name,"QQVGA")          ;break;
    case (QVGA)         :sprintf(name,"QVGA")       ;break;
    case (QVGAPLUS)     :sprintf(name,"QVGAPLUS")       ;break;
    case (VGA)          :sprintf(name,"VGA")        ;break;
        case (NHD)          :sprintf(name,"NHD")        ;break;
    case (VGAPLUS)      :sprintf(name,"VGAPLUS")        ;break;
    case (NTSC)         :sprintf(name,"NTSC")       ;break;
    case (PAL)          :sprintf(name,"PAL")        ;break;
    case (SVGA)         :sprintf(name,"SVGA")       ;break;
    case (WVGA)         :sprintf(name,"WVGA")       ;break;
    case (QHD)          :sprintf(name,"QHD")        ;break;
    case (HD)           :sprintf(name,"HD")         ;break;
    case (FULLHD)       :sprintf(name,"FULLHD")         ;break;
    case (XGA)          :sprintf(name,"XGA")        ;break;
    case (SXGA)         :sprintf(name,"SXGA")       ;break;
    case (UXGA)         :sprintf(name,"UXGA")       ;break;
     case (X512x384)      :sprintf(name,"X512x384")     ;break;
       case (X272x272)      :sprintf(name,"X272x272")   ;break;
       case (X400x400)      :sprintf(name,"X400x400")   ;break;
       case (QHD_1)         :sprintf(name,"QHD_1")     ;break;
    case (TWOMEGA)      :sprintf(name,"TWOMEGA")        ;break;
       case (THREEMEGA_W)   :sprintf(name,"THREEMEGA_W")    ;break;
    case (THREEMEGA)    :sprintf(name,"THREEMEGA")      ;break;
    case (FOURMEGA)     :sprintf(name,"FOURMEGA")       ;break;
    case (FIVEMEGA)     :sprintf(name,"FIVEMEGA")       ;break;
    case (FIVEMEGA_W)   :sprintf(name,"FIVEMEGA_W")     ;break;
    case (EIGHTMEGA)    :sprintf(name,"EIGHTMEGA")      ;break;
    case (EIGHTMEGA_W)  :sprintf(name,"EIGHTMEGA_W")    ;break;
    case (LASTRESOLUTION):sprintf(name,"LASTRESOLUTION")    ;break;

    default         :sprintf(name,"ERROR")          ;break;
 #else

    case (SUBQCIF)      :sprintf(name,"SUBQCIF")        ;break;
    case (SUBQCIFPLUS)  :sprintf(name,"SUBQCIFPLUS")    ;break;
    case (QCIF)         :sprintf(name,"QCIF")       ;break;
    case (QCIFPLUS)     :sprintf(name,"QCIFPLUS")       ;break;
    case (CIF)          :sprintf(name,"CIF")        ;break;
    case (CIFPLUS)      :sprintf(name,"CIFPLUS")        ;break;
    case (QQVGA)        :sprintf(name,"QQVGA")          ;break;
    case (QVGA)         :sprintf(name,"QVGA")       ;break;
    case (QVGAPLUS)     :sprintf(name,"QVGAPLUS")       ;break;
    case (VGA)          :sprintf(name,"VGA")        ;break;
        case (NHD)          :sprintf(name,"NHD")        ;break;
    case (VGAPLUS)      :sprintf(name,"VGAPLUS")        ;break;
    case (NTSC)         :sprintf(name,"NTSC")       ;break;
    case (PAL)          :sprintf(name,"PAL")        ;break;
    case (SVGA)         :sprintf(name,"SVGA")       ;break;
    case (WVGA)         :sprintf(name,"WVGA")       ;break;
    case (QHD)          :sprintf(name,"QHD")        ;break;
    case (HD)           :sprintf(name,"HD")         ;break;
    case (FULLHD)       :sprintf(name,"FULLHD")         ;break;
    case (XGA)          :sprintf(name,"XGA")        ;break;
    case (SXGA)         :sprintf(name,"SXGA")       ;break;
    case (UXGA)         :sprintf(name,"UXGA")       ;break;
    case (TWOMEGA)      :sprintf(name,"TWOMEGA")        ;break;
    case (THREEMEGA)    :sprintf(name,"THREEMEGA")      ;break;
    case (FOURMEGA)     :sprintf(name,"FOURMEGA")       ;break;
    case (FIVEMEGA)     :sprintf(name,"FIVEMEGA")       ;break;
    case (FIVEMEGA_W)   :sprintf(name,"FIVEMEGA_W")     ;break;
    case (EIGHTMEGA)    :sprintf(name,"EIGHTMEGA")      ;break;
    case (EIGHTMEGA_W)  :sprintf(name,"EIGHTMEGA_W")    ;break;
    case (LASTRESOLUTION):sprintf(name,"LASTRESOLUTION")    ;break;

    default         :sprintf(name,"ERROR")          ;break;

#endif


    }

}

char ITE_GetHexDigit(int digit)
{
    assert( (digit>9) && (digit<15));
    switch(digit)
    {
        case 10 : return 'A';
        case 11 : return 'B';
        case 12 : return 'C';
        case 13 : return 'D';
        case 14 : return 'E';
        case 15 : return 'F';
    }

    return 'X';
}

void ITE_itoa(int num, char *string, int radix)
{
    char reverse[10];
    char c = 0;
    int count = 0;
    int len = 0;
    while(0 != num)
    {
        c = num%radix;
        if(c<=9)
            reverse[count++] = c + '0';
        else
            reverse[count++] = ITE_GetHexDigit(c);
        num = num/radix;
    }

    reverse[count] = 0;
    len = strlen(reverse);

    for(num=0; num<len; num++)
    {
        string[num] = reverse[len-1-num];
    }

    string[num] = 0;
}


void ITE_GetStringNameForBMSConfiguration(char *name)
{
  char width[10];
  char height[10];
  char data_format[10];
  char framerate[10];
  char woi_x[10];
  char woi_y[10];
  ITE_itoa(usecase.BMS_XSize, width, 10);
  ITE_itoa(usecase.BMS_YSize, height, 10);
  ITE_itoa(usecase.BMS_woi_X, woi_x, 10);
  ITE_itoa(usecase.BMS_woi_Y, woi_y, 10);
  ITE_itoa(usecase.framerate_x100/100, framerate, 10);
  ITE_itoa(usecase.data_format, data_format, 16);
  sprintf(name, "mode_%s_%s_%s_%s_FR_%s_Fmt_%s", width, height, woi_x, woi_y, framerate, data_format);
}

void ITE_GetStringNameForBMSResolution(char *name)
{
  char width[10];
  char height[10];
  char data_format[10];

  ITE_itoa(usecase.BMS_XSize, width, 10);
  ITE_itoa(usecase.BMS_YSize, height, 10);
  ITE_itoa(usecase.data_format, data_format, 16);
  sprintf(name, "%s_%s_%s", width, height, data_format);
}


/***********************************************************************************/
/* convert string resolution to resolution enum                    */
/***********************************************************************************/
e_resolution ITE_GiveResolution(char* resolution)
{
    e_resolution reso = SUBQCIF; // __NO_WARNING__ 0;

#ifdef samsung8M

    if( 0 == strcmp(resolution, "SUBQCIF")){    reso=SUBQCIF;       goto resolution_end;}
    if( 0 == strcmp(resolution, "SUBQCIFPLUS")){    reso=SUBQCIFPLUS;   goto resolution_end;}
    if( 0 == strcmp(resolution, "QCIF")){       reso=QCIF;      goto resolution_end;}
    if( 0 == strcmp(resolution, "QCIFPLUS")){   reso=QCIFPLUS;      goto resolution_end;}
    if( 0 == strcmp(resolution, "CIF")){        reso=CIF;       goto resolution_end;}
    if( 0 == strcmp(resolution, "CIFPLUS")){    reso=CIFPLUS;       goto resolution_end;}
    if( 0 == strcmp(resolution, "QQVGA")){      reso=QQVGA;         goto resolution_end;}
    if( 0 == strcmp(resolution, "QVGA")){       reso=QVGA;      goto resolution_end;}
    if( 0 == strcmp(resolution, "QVGAPLUS")){   reso=QVGAPLUS;      goto resolution_end;}
    if( 0 == strcmp(resolution, "VGA")){        reso=VGA;       goto resolution_end;}
    if( 0 == strcmp(resolution, "WVGA")){       reso=WVGA;      goto resolution_end;}
    if( 0 == strcmp(resolution, "VGAPLUS")){    reso=VGAPLUS;       goto resolution_end;}
    if( 0 == strcmp(resolution, "NTSC")){       reso=NTSC;      goto resolution_end;}
    if( 0 == strcmp(resolution, "QHD")){        reso=QHD;       goto resolution_end;}
    if( 0 == strcmp(resolution, "PAL")){        reso=PAL;       goto resolution_end;}
    if( 0 == strcmp(resolution, "SVGA")){       reso=SVGA;      goto resolution_end;} // 600 not a multiple of 16
    if( 0 == strcmp(resolution, "HD")){     reso=HD;        goto resolution_end;}
    if( 0 == strcmp(resolution, "XGA")){        reso=XGA;       goto resolution_end;}
    if( 0 == strcmp(resolution, "SXGA")){       reso=SXGA;      goto resolution_end;}  // 1024
    if( 0 == strcmp(resolution, "FULLHD")){     reso=FULLHD;        goto resolution_end;}  // 1080 not a multiple of 16
    if( 0 == strcmp(resolution, "UXGA")){       reso=UXGA;      goto resolution_end;}
    if( 0 == strcmp(resolution, "NHD")){    reso=NHD;       goto resolution_end;}
    if( 0 == strcmp(resolution, "X512x384")){   reso=X512x384;      goto resolution_end;}
    if( 0 == strcmp(resolution, "X272x272")){   reso=X272x272;  goto resolution_end;}
    if( 0 == strcmp(resolution, "X400x400")){       reso=X400x400;      goto resolution_end;}
    if( 0 == strcmp(resolution, "QHD_1")){  reso=QHD_1;         goto resolution_end;}
    if(( 0 == strcmp(resolution, "3MEGAW"))||( 0 == strcmp(resolution, "THREEMEGA_W"))){
                            reso=THREEMEGA_W;   goto resolution_end;}
    if(( 0 == strcmp(resolution, "2MEGA"))||( 0 == strcmp(resolution, "TWOMEGA"))){
                            reso=TWOMEGA;       goto resolution_end;}
    if(( 0 == strcmp(resolution, "3MEGA"))||( 0 == strcmp(resolution, "THREEMEGA"))){
                            reso=THREEMEGA;     goto resolution_end;}
    if(( 0 == strcmp(resolution, "4MEGA"))||( 0 == strcmp(resolution, "FOURMEGA"))){
                            reso=FOURMEGA;      goto resolution_end;}
    if(( 0 == strcmp(resolution, "5MEGA"))||( 0 == strcmp(resolution, "FIVEMEGA"))){
                            reso=FIVEMEGA;      goto resolution_end;}
    if(( 0 == strcmp(resolution, "5MEGAW"))||( 0 == strcmp(resolution, "FIVEMEGA_W"))){
                            reso=FIVEMEGA_W;    goto resolution_end;}
    if(( 0 == strcmp(resolution, "8MEGA"))||( 0 == strcmp(resolution, "EIGHTMEGA"))){
                            reso=EIGHTMEGA;     goto resolution_end;}
    if(( 0 == strcmp(resolution, "8MEGAW"))||( 0 == strcmp(resolution, "EIGHTMEGA_W"))){
                            reso=EIGHTMEGA_W;   goto resolution_end;}

#else

    if( 0 == strcmp(resolution, "SUBQCIF")){    reso=SUBQCIF;       goto resolution_end;}
    if( 0 == strcmp(resolution, "SUBQCIFPLUS")){    reso=SUBQCIFPLUS;   goto resolution_end;}
    if( 0 == strcmp(resolution, "QCIF")){       reso=QCIF;      goto resolution_end;}
    if( 0 == strcmp(resolution, "QCIFPLUS")){   reso=QCIFPLUS;      goto resolution_end;}
    if( 0 == strcmp(resolution, "CIF")){        reso=CIF;       goto resolution_end;}
    if( 0 == strcmp(resolution, "CIFPLUS")){    reso=CIFPLUS;       goto resolution_end;}
    if( 0 == strcmp(resolution, "QQVGA")){      reso=QQVGA;         goto resolution_end;}
    if( 0 == strcmp(resolution, "QVGA")){       reso=QVGA;      goto resolution_end;}
    if( 0 == strcmp(resolution, "QVGAPLUS")){   reso=QVGAPLUS;      goto resolution_end;}
    if( 0 == strcmp(resolution, "VGA")){        reso=VGA;       goto resolution_end;}
    if( 0 == strcmp(resolution, "WVGA")){       reso=WVGA;      goto resolution_end;}
    if( 0 == strcmp(resolution, "VGAPLUS")){    reso=VGAPLUS;       goto resolution_end;}
    if( 0 == strcmp(resolution, "NTSC")){       reso=NTSC;      goto resolution_end;}
    if( 0 == strcmp(resolution, "QHD")){        reso=QHD;       goto resolution_end;}
    if( 0 == strcmp(resolution, "PAL")){        reso=PAL;       goto resolution_end;}
    if( 0 == strcmp(resolution, "SVGA")){       reso=SVGA;      goto resolution_end;} // 600 not a multiple of 16
    if( 0 == strcmp(resolution, "HD")){     reso=HD;        goto resolution_end;}
    if( 0 == strcmp(resolution, "XGA")){        reso=XGA;       goto resolution_end;}
    if( 0 == strcmp(resolution, "SXGA")){       reso=SXGA;      goto resolution_end;}  // 1024
    if( 0 == strcmp(resolution, "FULLHD")){     reso=FULLHD;        goto resolution_end;}  // 1080 not a multiple of 16
    if( 0 == strcmp(resolution, "UXGA")){       reso=UXGA;      goto resolution_end;}
    if( 0 == strcmp(resolution, "NHD")){    reso=NHD;       goto resolution_end;}
    if(( 0 == strcmp(resolution, "2MEGA"))||( 0 == strcmp(resolution, "TWOMEGA"))){
                            reso=TWOMEGA;       goto resolution_end;}
    if(( 0 == strcmp(resolution, "3MEGA"))||( 0 == strcmp(resolution, "THREEMEGA"))){
                            reso=THREEMEGA;     goto resolution_end;}
    if(( 0 == strcmp(resolution, "4MEGA"))||( 0 == strcmp(resolution, "FOURMEGA"))){
                            reso=FOURMEGA;      goto resolution_end;}
    if(( 0 == strcmp(resolution, "5MEGA"))||( 0 == strcmp(resolution, "FIVEMEGA"))){
                            reso=FIVEMEGA;      goto resolution_end;}
    if(( 0 == strcmp(resolution, "5MEGAW"))||( 0 == strcmp(resolution, "FIVEMEGA_W"))){
                            reso=FIVEMEGA_W;    goto resolution_end;}
    if(( 0 == strcmp(resolution, "8MEGA"))||( 0 == strcmp(resolution, "EIGHTMEGA"))){
                            reso=EIGHTMEGA;     goto resolution_end;}
    if(( 0 == strcmp(resolution, "8MEGAW"))||( 0 == strcmp(resolution, "EIGHTMEGA_W"))){
                            reso=EIGHTMEGA_W;   goto resolution_end;}
#endif

resolution_end :
    return(reso);
}


/***********************************************************************************/
/* return in "name" the string corresponding to selected pipe GrbFormat        */
/***********************************************************************************/
void ITE_GiveStringFormat(char *ap_pipe, char *name)
{
  enum e_grabFormat grbformat = GRBFMT_YUV422_RASTER_INTERLEAVED; //__NO_WARNING__ Not being set before potential use

  if (0== strcmp(ap_pipe,"LR")) grbformat = usecase.LR_GrbFormat;
  if (0== strcmp(ap_pipe,"HR")) grbformat = usecase.HR_GrbFormat;
  if (0== strcmp(ap_pipe,"BMS")) grbformat = usecase.BMS_GrbFormat;
  if (0== strcmp(ap_pipe,"BML")) grbformat = usecase.BML_GrbFormat;

  switch (grbformat) {
      case (GRBFMT_YUV422_RASTER_INTERLEAVED)   :sprintf(name,"YUV422RI")  ;break;
      case (GRBFMT_YUV420_RASTER_PLANAR_I420)   :sprintf(name,"YUV420RP_I420")  ;break;
      case (GRBFMT_YUV420_RASTER_PLANAR_YV12)   :sprintf(name,"YUV420RP_YU12")  ;break;
      case (GRBFMT_YUV422_MB_SEMIPLANAR)        :sprintf(name,"YUV422MB")  ;break;
      case (GRBFMT_YUV420_MB_SEMIPLANAR_FILTERED):sprintf(name,"YUV420MB_F")  ;break;
      case (GRBFMT_YUV420_MB_SEMIPLANAR_DECIMATED):sprintf(name,"YUV420MB_D")  ;break;
      case (GRBFMT_YUV420_MB_SWAP_SEMIPLANAR_DECIMATED)  :sprintf(name,"YUV420MB_SD")  ;break;
      case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV21):sprintf(name,"YUV422RI_NV21");break;
      case (GRBFMT_YUV420_RASTER_SEMIPLANAR_NV12):sprintf(name,"YUV422RI_NV12");break;
      case (GRBFMT_RGB30)               :sprintf(name,"RGB30")  ;break;
      case (GRBFMT_RAW8)                :sprintf(name,"RAW8")  ;break;
      case (GRBFMT_FAST_RAW8)           :sprintf(name,"RAW8")  ;break;
      case (GRBFMT_BML2_RAW8)           :sprintf(name,"RAW8")  ;break;
      case (GRBFMT_RAW12)               :sprintf(name,"RAW12")  ;break;
      case (GRBFMT_FAST_RAW12)              :sprintf(name,"RAW12")  ;break;
      case (GRBFMT_BML2_RAW12)              :sprintf(name,"RAW12")  ;break;
      case (GRBFMT_A4R4G4B4)            :sprintf(name,"ARGB4444")  ;break;
      case (GRBFMT_A1R5G5B5)            :sprintf(name,"ARGB1555")  ;break;
      case (GRBFMT_R5G6B5)              :sprintf(name,"RGB565")  ;break;
      case (GRBFMT_R8G8B8)              :sprintf(name,"RGB888")  ;break;
      case (GRBFMT_A8R8G8B8)            :sprintf(name,"ARGB8888")  ;break;

  }

}

/***********************************************************************************/
/* return in "name" the string corresponding to selected BML input         */
/***********************************************************************************/
void ITE_GiveStringBMLinput(char *name)
{
  switch (usecase.BML_input) {
      case (BAYERLOAD1) :sprintf(name,"BML1")  ;break;
      case (BAYERLOAD2) :sprintf(name,"BML2")  ;break;
      case (RGBLOAD)    :sprintf(name,"RGBLOAD")  ;break;
      default: sprintf(name,"NONE")  ;break; //__NO_WARNING__
  }

}

/***********************************************************************************/
/* return in "name" the string corresponding to selected BMS output            */
/***********************************************************************************/
void ITE_GiveStringBMSoutput(char *name)
{
  switch (usecase.BML_input) {
      case (BAYERSTORE0) :sprintf(name,"BAYERSTORE0")  ;break;
      case (BAYERSTORE1) :sprintf(name,"BAYERSTORE1")  ;break;
      case (BAYERSTORE2) :sprintf(name,"BAYERSTORE2")  ;break;
      default: sprintf(name,"NONE")  ;break; //__NO_WARNING__
  }

}

/***********************************************************************************/
/* return in "name" the string corresponding to selected pipe resolution       */
/***********************************************************************************/
void ITE_GiveStringResolution(char *ap_pipe, char *name)
{
e_resolution resolution = SUBQCIF; //__NO_WARNING__ Not being set before use

  if (0== strcmp(ap_pipe,"LR")) {
      resolution = usecase.LR_resolution;
  }
  if (0== strcmp(ap_pipe,"HR")) {
      resolution = usecase.HR_resolution;
  }
  if (0== strcmp(ap_pipe,"BMS")) {
      resolution = usecase.BMS_resolution;
  }
  if (0== strcmp(ap_pipe,"BML")) {
      resolution = usecase.BMS_resolution;
  }

ITE_GiveStringNameFromEnum(resolution, name);

}


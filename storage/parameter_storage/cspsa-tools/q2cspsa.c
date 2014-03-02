/*******************************************************************
 *
 * Copyright (C) 2012 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Writes parameters from a q-file to CSPSA
 *
 * Author: 2012, Apparao Urla. <apparao.urla@stericsson.com>
 * Author: 2011, Hans Holmberg. <hans.xh.holmbeg@stericsson.com>
 * Based upon the tool CSPSA2QSTORE created by
 * Mikael GULLBERG <mikael.xx.gullberg@stericsson.com>
 *
 * This functionality is used in U9540,
 * to back up modem parameters to CSPSA.
 *******************************************************************/

#include "cspsa.h"
#include "caliblist.h"
#include "qtools_src/getarg.h"
#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#ifdef LINUX
#define O_BINARY 0
#endif

#ifdef LINUX
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#else
#include <io.h>
#endif

/* define for logs. */
#define PROGRAM "Q2CSPSA:"
#define VERSION 0.0.1
#define LOGINFO_KEY 0xFFFFE000
#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x
#define ERRORSTR      "ERROR: "
#define LOGINFO_SIZE  1000
static const char ERRORSTR_INTERNAL[] = PROGRAM ERRORSTR "Internal error at %d.\n";
static const char ERRORSTR_OUT_OF_MEMORY[] = PROGRAM ERRORSTR "Out of memory at %d.\n";

struct Config_t
{
  CSPSA_Handle_t   CSPSA_Handle;
  char             CSPSA_Name[PATH_MAX];
  char             QfileName[PATH_MAX];
  char*            ItpUpdateFlag_p;
  char*            CSPSA_Key_p;
  CSPSA_Key_t      ItpUpdateFlagKey;
  uint32_t         ItpUpdateFlagBitIndex;
};

/** Status of CSPSA to file operations */
typedef enum
{
  CONVERT_STATUS_OK = 0,
  /* ITP update item not found */
  CONVERT_STATUS_UPDATE_ITEM_NOT_FOUND,
} ConvertStatus_t;

static void print_help(void)
{
  printf(
    "\n"
    "Usage: q2cspsa -c cspsa_name -i q-file -k 0x0001 -u 0xFFFFFFFD:1 [-h]\n"
    " -h              Prints this help.\n"
    " -c cspsa_name   The name of the parameter storage area to read.\n"
    " -i q-file       The input q file\n"
    " -k cspsa_key    The cspsa key where q file must be backup.\n"
    "                 (see value in cspsa_image/pff/0x00000000-0x0000FFFF.modem).\n"
    " -u <m:n>        Conversion is only done if bit n in CSPSA item\n"
    "                 m is set. Value m must be a 32-bit hexadecimal value, while n must\n"
    "                 be a decimal value from 0 to 31. This bit is reset after conversion.\n"
  );
}

/**
 * Parse the command line arguments
 *
 */
static int parseCmdline(int argc, char *argv[], struct Config_t* config_p, uint32_t *Qfile_P)
{
  bool set_CSPSA = false;
  bool set_QFILE = false;
  bool set_UPDATE = false;
  int  r = 0;
  int  result = 0;
  char *end_p = NULL;
  char *Keystr_p = config_p->ItpUpdateFlag_p;
  char *BitIndexStr_p = NULL;
  bool LegalFormat = false;

  while ((r = getopt (argc, argv, "hc:i:k:u:")) != -1)
  {
    switch (r) {
    case 'h':
      print_help();
      result = -1;
      break;

    case 'c':
      strncpy(config_p->CSPSA_Name, optarg, PATH_MAX);
      set_CSPSA = true;
      break;

    case 'i':
      strncpy(config_p->QfileName, optarg, PATH_MAX);
      set_QFILE = true;
      break;

    case 'u':
      config_p->ItpUpdateFlag_p = strdup(optarg);
      set_UPDATE = true;
      break;

    case 'k':
      config_p->CSPSA_Key_p = strdup(optarg);
      break;

    default:
      result = -1;
    }
  }

  if (result < 0)
    goto exit;

  if ((set_CSPSA == false) || (set_QFILE == false) || (config_p->CSPSA_Key_p == NULL))
  {
    fprintf(stderr, PROGRAM "Incorrect parameter for q2cspsa.\n"
           "Example use q2cspsa -c CSPSA0 -i acc_calib.q -k 0x0001 -u (optional) 0xfffffffd:1 or use -h option\n");
    result = -1;
    goto exit;
  }

  /* Parse and check the CSPSA File Key value (-k option) */
  *Qfile_P = strtoul(config_p->CSPSA_Key_p, &end_p, 0);

  if ( set_UPDATE == false )
    goto exit; // Skip Parsing the -u option.

  if (config_p->ItpUpdateFlag_p == NULL)
  {
    fprintf(stderr, PROGRAM "Incorrect parameter for q2cspsa. you are using -u option \n"
           "Example use q2cspsa -c CSPSA0 -i acc_calib.q -k 0x0001 -u  0xfffffffd:1\n");
    result = -1;
    goto exit;
  }

  /* Parse and check -u m:n parameter. */
  config_p->ItpUpdateFlagKey = strtoul(config_p->ItpUpdateFlag_p, &end_p, 0);
  LegalFormat = (*end_p == ':');
  if (((config_p->ItpUpdateFlagKey != 0) || (config_p->ItpUpdateFlag_p != end_p)) && LegalFormat)
  {
    BitIndexStr_p = end_p + 1; // Skip the ':'.
    config_p->ItpUpdateFlagBitIndex = strtoul(BitIndexStr_p, &end_p, 0);
    LegalFormat = (*end_p == '\0');
  }

  if
   (
    (config_p->ItpUpdateFlagKey == 0)
    ||
    ((config_p->ItpUpdateFlagKey == 0) && (Keystr_p == end_p))
    ||
    ((config_p->ItpUpdateFlagBitIndex == 0) && (BitIndexStr_p == end_p))
    ||
    (config_p->ItpUpdateFlagBitIndex > 31)
    ||
    !LegalFormat
   )
  {
    fprintf(stdout,
            PROGRAM "Error the parameter following -u must be of the form <id>:<bit index>,\n"
            "where id is in {0 - 0xFFFFFFFF} and bit index in {0 - 31}.\n");
    result = -1;
  }
  else
    fprintf(stdout, PROGRAM "write parameters from  Q-file[%s] to CSPSA[%s] \n",
            config_p->QfileName, config_p->CSPSA_Name);

exit:
  return result;
}

/**
 * Checks ITP update flag
 * and returns flag_p >0 if flag is set and 0 if not.
 */
static ConvertStatus_t setItpUpdateFlag(struct Config_t* const config_p, bool flagActive)
{
  ConvertStatus_t Result = CONVERT_STATUS_UPDATE_ITEM_NOT_FOUND;
  CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_OK;
  CSPSA_Key_t Key = config_p->ItpUpdateFlagKey;
  CSPSA_Size_t Size;
  uint32_t BitIndex = config_p->ItpUpdateFlagBitIndex;
  uint32_t Value;

  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->CSPSA_Handle, Key, &Size);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    goto ErrorExit;

  if (Size * 8 < BitIndex)
  {
    fprintf(stdout, PROGRAM "Error size of item 0x%08X is smaller than the bit index (%d).\n", Key, BitIndex);
    goto ErrorExit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    goto ErrorExit;

  if (flagActive)
    Value = Value | (1 << BitIndex);
  else
    Value = Value & ~(1 << BitIndex);

  CSPSA_Result = CSPSA_WriteValue(config_p->CSPSA_Handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    goto ErrorExit;

  Result = CONVERT_STATUS_OK;
  fprintf(stdout, PROGRAM "ITP Update flag (0x%08X:%d) is now set to %i.\n", Key, BitIndex,Value);

ErrorExit:

  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    fprintf(stdout, PROGRAM "Error cannot read CSPSA item 0x%08X.\n", Key);

  return Result;
}

/**
 * Opens the CSPSA area for writing
 *
 */
static int InitCSPSA(struct Config_t *config_p)
{
  CSPSA_Result_t     CSPSA_Result;
  int result = 0;

  CSPSA_Result = CSPSA_Open(config_p->CSPSA_Name, &config_p->CSPSA_Handle);
  if (CSPSA_Result > T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, PROGRAM "Fail to open CSPSA (%d).\n", CSPSA_Result);
    result = -1;
  }
  return result;
}


/**
 * Close the CSPSA area
 *
 */
static void UnInit(struct Config_t *config_p)
{
  CSPSA_Result_t     CSPSA_Result;

  if (config_p->CSPSA_Handle)
  {
    CSPSA_Result = CSPSA_Close(&config_p->CSPSA_Handle);
    if (CSPSA_Result > T_CSPSA_RESULT_ERRORS)
      fprintf(stderr, PROGRAM "Fail to close CSPSA %d. \n",CSPSA_Result);
  }
  return;
}

/**
 * Writes a q-file parameters to CSPSA
 *
 */
static int WriteParametersToCSPSA(uint8_t *Data_ptr, uint32_t Qfile_Id, uint32_t QfileSize, struct Config_t* config_p)
{
  CSPSA_Result_t         CSPSA_Result;
  int                    result = 0;

  if (Data_ptr != NULL)
  {
    CSPSA_Result = CSPSA_WriteValue(config_p->CSPSA_Handle, Qfile_Id, QfileSize, Data_ptr);

    if(CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    {
      fprintf(stderr, PROGRAM "Failed to write %d Q-file\n",Qfile_Id);
      result = -1;
      goto error;
    }
    fprintf(stdout, PROGRAM "%i bytes of file %s have been backup in CSPSA\n",QfileSize, config_p->QfileName);
  }
  else
  {
    result = -1;
    fprintf(stderr, PROGRAM "Fail to write %s file in CSPSA\n",config_p->QfileName);
  }
error:
  return result;
}

/**
 *  Reads a q-file and returns true if successful.
 */

static int QFile_Read( char *QFileName_p, uint8_t **DataArea_pp,  uint32_t *FileSize_p)
{
  int Handle;
  int FileSize = 0;
  int DataRead = 0;
  int count = 0;
  int result = 0;
  uint8_t *Data_p = NULL;
  struct stat FileStat;

  Handle = open(QFileName_p, O_RDONLY | O_BINARY, 0);
  if (Handle == -1)
  {
    fprintf(stderr, PROGRAM "Cannot open file %s \n", QFileName_p);
    return -1;
  }

  if (fstat(Handle, &FileStat) < 0)
  {
    fprintf(stderr, PROGRAM "Failed to get the %s File info \n" ,QFileName_p);
    result = -1;
    goto error;
  }

  if (FileStat.st_size)
  {
    Data_p = (uint8_t *)malloc(FileStat.st_size);

    if (Data_p == NULL)
    {
      fprintf(stderr, PROGRAM "Failed to allocate %lli bytes \n" ,FileStat.st_size);
      result = -1;
      goto error;
    }
  }

  /* Read no. of requested bytes(till end of file) */
  FileSize = FileStat.st_size;

  while (FileSize > 0)
  {
    count = read(Handle, Data_p + DataRead, FileSize);
    if (count < 0)
    {
      fprintf(stderr, PROGRAM "Could not read as expected \n");
      free(Data_p);
      result = -1;
      goto error;
    }
    DataRead = DataRead + count;
    FileSize = FileSize - count;
  }
  /* Fill output parameters */
  *DataArea_pp = Data_p;
  *FileSize_p = DataRead;

error:

  if (Handle)
    close(Handle);

  return result;
}

static int WriteLogToCSPSA(struct Config_t* config_p, CSPSA_Key_t creation_time_stamp_id)
{
  struct tm *time_p;
  time_t tm;
  char *strtime;
  char *creation_message = NULL;
  uint8_t *Data_p = NULL;
  uint32_t LogfileSize=0;
  int result = 0;
  CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_OK;

  creation_message = (char *) malloc(LOGINFO_SIZE);
  if (creation_message == NULL)
  {
    fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
    result = -1;
    goto error_exit;
  }

  tm = time(NULL);
  if (tm == (time_t)-1)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    result = -1;
    goto error_exit;
  }

  time_p = localtime(&tm);
  if (time_p == NULL)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    result = -1;
    goto error_exit;
  }

  strtime = asctime(time_p);
  if (strtime == NULL)
  {
    fprintf(stderr, ERRORSTR_INTERNAL, __LINE__);
    result = -1;
    goto error_exit;
  }

  sprintf(creation_message, "%s %s %s: %s backup in CSPSA.\n", PROGRAM, STRINGIFY(VERSION), strtime, config_p->QfileName);

  /*read original log in CSPSA*/
  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->CSPSA_Handle, creation_time_stamp_id, &LogfileSize);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, PROGRAM "error code(%d)", CSPSA_Result);
    result = -1;
    goto error_exit;
  }

  Data_p = (uint8_t *) malloc(LogfileSize);
  if (Data_p == NULL)
  {
    fprintf(stderr, ERRORSTR_OUT_OF_MEMORY, __LINE__);
    result = -1;
    goto error_exit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, creation_time_stamp_id, LogfileSize, Data_p);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, PROGRAM "error code(%d)", CSPSA_Result);
    result = -1;
    goto error_exit;
  }

  /* put at the beginning our message until size max defined */
  strncat(creation_message, (char *)Data_p, LOGINFO_SIZE-strlen(creation_message) );
  creation_message[strlen(creation_message)-1] = '\0';

  if (CSPSA_WriteValue(config_p->CSPSA_Handle, creation_time_stamp_id, strlen(creation_message) + 1, (uint8_t *)creation_message) >= T_CSPSA_RESULT_ERRORS)
  {
    fprintf(stderr, ERRORSTR "Writing creation details failed.\n");
    result = -1;
    goto error_exit;
  }

error_exit:

  if (creation_message != NULL)
    free(creation_message);

  if (Data_p != NULL)
    free(Data_p);

  if (result < 0)
    fprintf(stderr, PROGRAM "Update log info in CSPSA failed.\n");

  return result;
}

int main(int argc, char *argv[])
{
  struct Config_t config;
  int    result = 0;
  uint8_t *Data_g = NULL;
  uint32_t QfileSize;
  uint32_t Qfile_Id = 0;

  memset(&config, 0x00, sizeof(struct Config_t));

  /* Parse the input argument and if not correct print info and exit */
  if (parseCmdline(argc, argv, &config, &Qfile_Id) == -1)
  {
    result = -1;
    goto exit;
  }

  /* First we open the CSPSA area */
  if (InitCSPSA(&config) != 0)
  {
    result = -1;
    goto exit;
  }

  /* Open and Read Q-store file */
  if (0 != QFile_Read(config.QfileName, &Data_g, &QfileSize))
  {
    result = -1;
    goto exit;
  }

  /* Write parameters to CSPSA area */
  if (0 != WriteParametersToCSPSA(Data_g, Qfile_Id, QfileSize, &config ))
  {
    fprintf(stderr, PROGRAM "Failed to write parameters to CSPSA.\n");
    result = -1;
    goto exit;
  }

  /**
   * Update ITP update flag
   * 0:ITP flag is not set
   * 1:ITP flag is set, ie that CSPSA contents have been updated
   **/

  if (config.ItpUpdateFlagKey)
    setItpUpdateFlag(&config, false);

  /* write log in CSPSA */
  if (0 != WriteLogToCSPSA(&config, LOGINFO_KEY))
  {
    fprintf(stderr, PROGRAM "Failed to write log info to CSPSA.\n");
    result = -1;
  }

exit:

  if (Data_g != NULL)
    free(Data_g);

  /* Clean up and close any open files etc.. */
  UnInit(&config);

  return result;
}

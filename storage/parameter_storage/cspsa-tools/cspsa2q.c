/*******************************************************************
 *
 * Copyright (C) 2012 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Creates a q-store file from CSPSA parameters.
 *
 * Author: 2012, Apparao Urla. <apparao.urla@stericsson.com>
 * Author: 2011, Hans Holmberg. <hans.xh.holmbeg@stericsson.com>
 * Based upon the tool CSPSA2QSTORE created by
 * Mikael GULLBERG <mikael.xx.gullberg@stericsson.com>
 *
 * This functionality is used in U9540,
 * to restore calibration parameters from the backup in cpssa.
 *
 *******************************************************************/

#include "cspsa.h"
#include "caliblist.h"

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

#define PROGRAM         "CSPSA2Q: "

/* CSPSA key for Log file . */
#define LOGFILE_KEY     0xFFFFE000

struct Config_t
{
  CSPSA_Handle_t   CSPSA_Handle;
  char             CSPSA_Name[PATH_MAX];
  char             Q_FileName[PATH_MAX];
  char             Log_FileName[PATH_MAX];
  char*            cspsaUpdateItemInfo_p;
  char*            CalibParamUpdateItemInfo_p;
  char*            CSPSA_Key_p;
  bool             forceFileCreation;
  bool             Enable_Log;
  CSPSA_Key_t      CalibParamUpdateFlagKey;
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
    "Usage: cspsa2q -c cspsa_name -o q-file -k 0x0001 -u <m:n> -m 0xFFFFFFFE [-h]\n"
    " -h              Prints this help.\n"
    " -c cspsa_name   The name of the parameter storage area to read.\n"
    " -u <m:n>        Conversion is only done if bit n in CSPSA item\n"
    "                 m is set. Value m must be a 32-bit hexadecimal value, while n must\n"
    "                 be a decimal value from 0 to 31. This bit is reset after conversion.\n"
    " -f              Force conversion of CSPSA parameters to files, even if the update bit\n"
    "                 is not set\n"
    " -o q-file       The out q-file file with data read from parameter storage.\n"
    " -m <cspsa_key>  Flag to check modem calibration parameters validity.\n"
    "                 cspsa_key_Value must be a 32-bit hexadecimal value\n"
    " -k <cspsa_key>  The cspsa key where q file must be restored.\n"
    "                 (see value in cspsa_image/pff/0x00000000-0x0000FFFF.modem).\n"
    " -d <debug>      Enable to store debug logs\n"
  );
}

/**
 * Parse the command line arguments
 *
 */
static int parseCmdline(int argc, char *argv[], struct Config_t* config_p , uint32_t *Qfile_P)
{
  bool set_CSPSA = false;
  bool set_QFILE = false;
  int  r = 0;
  int  result = 0;
  char *end_p = NULL;
  char *Keystr_p = config_p->cspsaUpdateItemInfo_p;
  char *BitIndexStr_p = NULL;
  bool LegalFormat = false;


  while ((r = getopt (argc, argv, "hc:fo:k:u:m:d:")) != -1)
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

    case 'f':
      config_p->forceFileCreation = true;
      break;

    case 'k':
      config_p->CSPSA_Key_p = strdup(optarg);
      break;

    case 'o':
      strncpy(config_p->Q_FileName, optarg, PATH_MAX);
      set_QFILE = true;
      break;

    case 'u':
      config_p->cspsaUpdateItemInfo_p = strdup(optarg);
      break;

    case 'm':
      config_p->CalibParamUpdateItemInfo_p = strdup(optarg);
      break;

    case 'd':
      strncpy(config_p->Log_FileName, optarg, PATH_MAX);
      config_p->Enable_Log = true;
      break;

    default:
       result = -1;
    }
  }

  if (result < 0)
    goto exit;

  if ((set_CSPSA == false) || (set_QFILE == false) ||
      (config_p->cspsaUpdateItemInfo_p == NULL) ||
      (config_p->CalibParamUpdateItemInfo_p == NULL)||
      (config_p->CSPSA_Key_p == NULL))
  {
    fprintf(stderr, PROGRAM "Incorrect parameter for cspsa2q.\n"
           "Example use cspsa2q -c CSPSA0 -o acc_calib.q -k 0x0001 -u 0xfffffffd:0 -m 0xfffffffe\n" );
    result = -1;
    goto exit;
  }

  /* Parse and check the CSPSA File Key value: -k option */
  *Qfile_P = strtoul(config_p->CSPSA_Key_p, &end_p, 0);

  /* Parse and check itp update key value: -u <m:n> option  */
  config_p->ItpUpdateFlagKey = strtoul(config_p->cspsaUpdateItemInfo_p, &end_p, 0);
  LegalFormat = (*end_p == ':');
  if (((config_p->ItpUpdateFlagKey != 0) || (config_p->cspsaUpdateItemInfo_p != end_p)) && LegalFormat)
  {
    BitIndexStr_p = end_p + 1; // Skip the ':'.
    config_p->ItpUpdateFlagBitIndex = strtoul(BitIndexStr_p, &end_p, 0);
    LegalFormat = (*end_p == '\0');
  }

  /* Parse and check calib. parameters update key value: -m option */
  config_p->CalibParamUpdateFlagKey = strtoul(config_p->CalibParamUpdateItemInfo_p, &end_p, 0);
  if
  (
    ((config_p->ItpUpdateFlagKey == 0) && (Keystr_p == end_p))
    ||
    ((config_p->ItpUpdateFlagBitIndex == 0) && (BitIndexStr_p == end_p))
    ||
    (config_p->ItpUpdateFlagBitIndex > 31)
    ||
    !LegalFormat
    ||
    (config_p->CalibParamUpdateFlagKey == 0)
  )
  {
    fprintf(stderr, PROGRAM
            "Error the parameter following -u must be of the form <id>:<bit index>,\n"
            "where id is in {0 - 0xFFFFFFFF} and bit index in {0 - 31}.\n");
    result = -1;
  }
  else
    fprintf(stdout, PROGRAM "read from CSPSA[%s] to q-fle[%s]\n", config_p->CSPSA_Name, config_p->Q_FileName);

exit:
  return result;
}

/**
 * Opens the CSPSA area for reading
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
      fprintf(stderr, PROGRAM "Fail to close CSPSA (%d). \n",CSPSA_Result);
  }
  return;
}

/**
 * Reads parameters from the CSPSA area and stores them in Linux filesystem
 * This function is generic for all files (calib data, log data, etc,.).
 */
static int LoadingFromCSPSA(struct Config_t* config_p, uint32_t File_Id, uint32_t setFileName)
{
  CSPSA_Result_t        CSPSA_Result;
  uint8_t               *Data_p = NULL;
  uint32_t              File_Size;
  uint32_t              count;
  FILE                  *File_Ptr = NULL;
  int                   result=0;
  char                  *Open_FileName;

  /* Open output file */
  if (setFileName)
    Open_FileName = config_p->Log_FileName;
  else
    Open_FileName = config_p->Q_FileName;

  File_Ptr = fopen(Open_FileName, "wt");
  if (File_Ptr == NULL)
  {
    fprintf(stderr, PROGRAM "Could not open file %s. Error code = %d\n", Open_FileName, errno);
    return -1;
  }

  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->CSPSA_Handle, File_Id, &File_Size);

  Data_p = (uint8_t *) malloc(File_Size);
  if (Data_p == NULL)
  {
    fprintf(stderr, PROGRAM "Failed to allocate memory. Error code = %d\n", errno);
    result = -1;
    goto ErrorExit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, File_Id, File_Size, Data_p);

  if (CSPSA_Result <= T_CSPSA_RESULT_ERRORS)
  {
    count = fwrite(Data_p, 1, File_Size, File_Ptr);
    fprintf(stdout, PROGRAM "Write successfully %i in file %s.\n", count, Open_FileName);
    if (count != File_Size)
    {
      fprintf(stderr, PROGRAM "Incomplete write : only %d bytes written in file %s.\n", count, Open_FileName);
      result = -1;
      goto ErrorExit;
    }
  }
  else
  {
    result = -1;
    goto ErrorExit;
  }

  fprintf(stdout, PROGRAM "%d CSPSA Parameters have been copied\n", File_Size);

ErrorExit:

  if (result == -1)
    fprintf(stderr, PROGRAM "Failed to copy CSPSA Parameters.\n");

  if (File_Ptr != NULL)
    fclose(File_Ptr);

  if(Data_p != NULL)
    free(Data_p);

  return result;
}

/**
 * Checks ITP update flag and returns flag_p >0 if flag is set and 0 if not.
 */
static ConvertStatus_t checkItpUpdateFlag(struct Config_t* config_p, int *flag_p)
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
    fprintf(stderr, PROGRAM "Error size of item 0x%08X is smaller than the bit index (%d).\n", Key, BitIndex);
    goto ErrorExit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    goto ErrorExit;

  *flag_p = ((1 << BitIndex) & Value) >> BitIndex;
  Result = CONVERT_STATUS_OK;
  fprintf(stdout, PROGRAM "ITP Update flag (0x%08X) is set to %i. Return %x for bit index %d  main.\n",
          Key, Value, *flag_p, BitIndex);

ErrorExit:

  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    fprintf(stderr, PROGRAM "cannot read CSPSA item 0x%08X.\n", Key);

  return Result;
}

/**
 * Checks Modem calibration parameters validity update flag
 * and returns flag_p >0 if flag is set and 0 if not.
 */
static ConvertStatus_t checkCalibParamUpdateFlag(struct Config_t* config_p, int *flag_p)
{
  ConvertStatus_t Result = CONVERT_STATUS_UPDATE_ITEM_NOT_FOUND;
  CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_OK;
  CSPSA_Key_t Key = config_p->CalibParamUpdateFlagKey;
  CSPSA_Size_t Size;
  uint32_t Value;

  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->CSPSA_Handle, Key, &Size);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    goto ErrorExit;

  if (Size != sizeof(Value))
  {
    fprintf(stderr, PROGRAM "Error size of CalibParamUpdateFlagKey \n");
    goto ErrorExit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    goto ErrorExit;

  *flag_p =  Value;
  Result = CONVERT_STATUS_OK;
  fprintf(stdout, PROGRAM "Modem Calib data Update: 0x%08X is set to 0x%x\n", Key, Value);

ErrorExit:

  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    fprintf(stderr,PROGRAM "Cannot read CSPSA item 0x%08X.\n", Key);

  return Result;
}

/**
 * Set and clearing ITP update flag based on the data updation.
 */
static ConvertStatus_t setItpUpdateFlag(struct Config_t* const config_p, bool flagActive)
{
  ConvertStatus_t Result = CONVERT_STATUS_UPDATE_ITEM_NOT_FOUND;
  CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_OK;
  CSPSA_Key_t Key = config_p->ItpUpdateFlagKey;
  CSPSA_Size_t Size;
  uint32_t BitIndex = config_p->ItpUpdateFlagBitIndex;
  uint32_t Value;

  fprintf(stdout,PROGRAM "Clearing ITP Update: 0x%08X:%d\n", Key, BitIndex);

  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->CSPSA_Handle, Key, &Size);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    goto ErrorExit;

  if (Size * 8 < BitIndex)
  {
    fprintf(stderr, PROGRAM "Error size of item 0x%08X is smaller than the bit index (%d).\n", Key, BitIndex);
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
  fprintf(stdout, PROGRAM "ITP Update flag (0x%08X:%d) is now set to %i.\n", Key, BitIndex, Value);

ErrorExit:

  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
    fprintf(stderr, PROGRAM "Error cannot read CSPSA item 0x%08X.\n", Key);

  return Result;
}

int main(int argc, char *argv[])
{
  struct Config_t config;
  int    result = 0;
  int    ItpUpdateFlag = 0;
  int    CalibParamUpdateFlag = 1;
  uint32_t Qfile_Key = 0;
  uint32_t setFileName = 0;

  memset(&config, 0x00, sizeof(struct Config_t));

  /* Parse the input argument and if not correct print info and exit */
  if (parseCmdline(argc, argv, &config, &Qfile_Key) == -1)
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

  /* Check if the qstore file should be updated based upon entires in CSPSA */
  result = checkItpUpdateFlag(&config, &ItpUpdateFlag);
  if (result != CONVERT_STATUS_OK)
    goto exit ;

  /* Check if the qstore file should be updated based upon entires in CSPSA */
  result = checkCalibParamUpdateFlag(&config, &CalibParamUpdateFlag);
  if (result != CONVERT_STATUS_OK)
    goto exit ;

  if ((config.forceFileCreation) || (!ItpUpdateFlag && CalibParamUpdateFlag))
  {
    /* reset flag before extracing CSPSA, otherwise, the ITP would reset the flag at shutdown */
    setItpUpdateFlag(&config, true);

    /* Parse CSPSA and save in qstore file */
    if (LoadingFromCSPSA(&config, Qfile_Key, setFileName) == -1)
    {
      /* loading failed, restore update flag so conversion will be retried */
      if (ItpUpdateFlag)
        setItpUpdateFlag(&config, false);
      result = -1;
      goto exit;
    }
  }
  else
  {
    fprintf(stderr, PROGRAM
            "No restore of q store file %s becasue ITP update flag not set(%i) or parameters not valid(%i).\n",
            config.Q_FileName, ItpUpdateFlag, CalibParamUpdateFlag);
    result = -1;
  }

  /* Parse CSPSA and save in log file */
  if (config.Enable_Log)
  {
    setFileName = 1;
    if (LoadingFromCSPSA(&config, LOGFILE_KEY, setFileName) == -1)
    {
      result = -1;
      goto exit;
    }
    else
      fprintf(stdout,PROGRAM "CSPSA Log file created.\n");
  }

exit:

  /* Clean up and close any open files etc.. */
  UnInit(&config);
  return result;
}

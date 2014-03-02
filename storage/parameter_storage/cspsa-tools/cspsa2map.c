/*******************************************************************
 *
 * Copyright (C) 2011 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Creates a q-store file from CSPSA parameters.
 *
 * Author: 2011, Hans Holmberg. <hans.xh.holmbeg@stericsson.com>
 * Based upon the tool CSPSA2QSTORE created by
 * Mikael GULLBERG <mikael.xx.gullberg@stericsson.com>
 *
 * This functionality is used in U5500, in combination with map2q
 * to restore calibration parameters from the backup in cpssa.
 * At first "normal" startup, this program should be run to convert
 * the calibration data in the trim area to corresponding files.
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

#define FALSE 0
#define TRUE  1

/* TODO: is this a limitation we really want?"*/
#define QSTORE_GDFS_MAX_PARAM_SIZE  2048

#define MAP_PARAMETERS_PER_ROW      16

struct Config_t
{
  CSPSA_Handle_t   CSPSA_Handle;
  char             CSPSA_Name[PATH_MAX];
  char             MAP_FileName[PATH_MAX];
  char*            cspsaUpdateItemInfo_p;
  bool             forceFileCreation;
  bool             calibrationParametersOnly;
  CSPSA_Key_t      ItpUpdateFlagKey;
  uint32_t         ItpUpdateFlagBitIndex;
};

/** Status of CSPSA to file operations */
typedef enum
{
  CONVERT_STATUS_OK = 0,
  /** Could not open the CSPSA */
  CONVERT_STATUS_OPEN_FAILED,
  /** Could not read from the CSPSA, contents are corrupt or ITP has not written any data */
  CONVERT_STATUS_CSPSA_CORRUPT,
  CONVERT_STATUS_OUT_OF_MEMORY,
  /** Error occurred while writing ITP parameter contents to file */
  CONVERT_STATUS_WRITE_FAILED,
  /** Unspecified error occurred */
  CONVERT_STATUS_ERROR,
  /** Invalid usage */
  CONVERT_STATUS_INVALID_USAGE,
  /** ITP update item not found */
  CONVERT_STATUS_UPDATE_ITEM_NOT_FOUND,
} ConvertStatus_t;

static void print_help(void)
{
  printf(
    "\n"
    "Usage: cspsa2map -c cspsa_name -o mapfile -u m:n [-h]\n"
    " -h              Prints this help.\n"
    " -c cspsa_name   The name of the parameter storage area to read.\n"
    " -u <m:n>        Conversion is only done if bit n in CSPSA item\n"
    "                 m is set. Value m must be a 32-bit hexadecimal value, while n must\n"
    "                 be a decimal value from 0 to 31. This bit is reset after conversion.\n"
    " -f              Force conversion of CSPSA parameters to files, even if the update bit\n"
    "                 is not set\n"
    " -x              Only read calibration parameters from CSPSA."
    " -o mapfile      The out MAP/GDF file with data read from parameter storage.\n"
  );
}

/**
 * Parse the command line arguments
 *
 */
static int parseCmdline(int argc, char *argv[], struct Config_t* config_p)
{
  bool set_CSPSA = false;
  bool set_MAP = false;
  
  int r = 0;
  int result = 0;


  while ((r = getopt (argc, argv, "hc:xo:fu:")) != -1)
  {
    switch (r) {
    case 'h':
      print_help();
      result = -1;
      break;

    case 'c':
      strncpy(config_p->CSPSA_Name, optarg, PATH_MAX);
      config_p->CSPSA_Name[PATH_MAX - 1] = '\0';
      set_CSPSA = TRUE;
      break;

    case 'f':
      config_p->forceFileCreation = true;
      break;
      
    case 'x':
      config_p->calibrationParametersOnly = true;
      break;
      
    case 'u':
      config_p->cspsaUpdateItemInfo_p = strdup(optarg);
      break;

    case 'o':
      strncpy(config_p->MAP_FileName, optarg, PATH_MAX);
      config_p->MAP_FileName[PATH_MAX - 1] = '\0';
      set_MAP = TRUE;

      break;

    default:
       result = -1;
    }
  }

  if (result < 0)
    goto exit;

  if ((set_CSPSA == false) || (set_MAP == false) || (config_p->cspsaUpdateItemInfo_p == NULL))
  {
    fprintf(stderr, "CSPSA2MAP: Incorrect parameter for cspsa2map. Example use CSPSA2MAP -c CSPSA0 -o gdfs -u 0xfffffffd:0\n\n");
    result = -1;
  }
  else
  {
    // Parse and check -u m:n parameter.
    {
      char* Keystr_p = config_p->cspsaUpdateItemInfo_p;
      char* BitIndexStr_p = NULL;
      char* end_p = NULL;
      bool LegalFormat = false;

      config_p->ItpUpdateFlagKey = strtoul(config_p->cspsaUpdateItemInfo_p, &end_p, 0);
      LegalFormat = (*end_p == ':');
      if (((config_p->ItpUpdateFlagKey != 0) || (config_p->cspsaUpdateItemInfo_p != end_p)) && LegalFormat)
      {
        BitIndexStr_p = end_p + 1; // Skip the ':'.
        config_p->ItpUpdateFlagBitIndex = strtoul(BitIndexStr_p, &end_p, 0);
        LegalFormat = (*end_p == '\0');
      }

      if
      (
        ((config_p->ItpUpdateFlagKey == 0) && (Keystr_p == end_p))
        ||
        ((config_p->ItpUpdateFlagBitIndex == 0) && (BitIndexStr_p == end_p))
        ||
        (config_p->ItpUpdateFlagBitIndex > 31)
        ||
        !LegalFormat
      )
      {
        printf(
          "CSPSA2MAP: Error the parameter following -u must be of the form <id>:<bit index>,\n"
          "where id is in {0 - 0xFFFFFFFF} and bit index in {0 - 31}.\n");
        result = -1;
      }
      else
      {
        fprintf(stdout, "CSPSA2MAP: read from CSPSA2MAP[%s] to MAP[%s]\n", config_p->CSPSA_Name, config_p->MAP_FileName);
      }
    }

  }

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
    fprintf(stderr, "CSPSA2MAP: Fail to open CSPSA (%d).\n", CSPSA_Result);
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
    {
      fprintf(stderr, "CSPSA2MAP: Fail to close CSPSA (%d). \n",CSPSA_Result);
    }
  }
  return;
}

static void write_MAP_header(FILE *f)
{
  /* Generate a header in the file. */
  fprintf( f, "//****************************************************************************\n");
  fprintf( f, "//\n");
  fprintf( f, "//   GDFS Map file created by cspsa2map\n");
  fprintf( f, "//\n");
  fprintf( f, "//****************************************************************************\n");
}

static void write_MAP_parameter(FILE *f,uint32_t id, uint32_t size, uint8_t *data_p)
{
  uint32_t k=0;
  
  if ( size == 0 )
   return;
   
  /* write the parameter header */
  fprintf( f, "\n%04x %d 0000", id, size);

  /* wite the data*/ 
  for ( k = 0 ; k < size ; k++ )
  {
    if ( (k % MAP_PARAMETERS_PER_ROW) == 0 )
    {
      fprintf( f, "\n\t");
    }

    fprintf( f, " %02x", data_p[k]);
  }
}

/**
 * Reads parameters from the CSPSA area and stores them in .MAP format in Linux filesystem
 *
 */
static int LoadingFromCSPSA(struct Config_t* config_p)
{
  CSPSA_Result_t     	CSPSA_Result;
  CSPSA_Key_t		Id = 0;
  CSPSA_Size_t		Size = 0;
  uint8_t		*Data_p = NULL;
  int			NbrParamsCopied = 0;
  int			result = 0;
  FILE			*MAP_file = NULL;
  uint32_t		i = 0;
  int			NbrCalibrationParams = 0;
 
  /* Open the output file*/
  MAP_file= fopen(config_p->MAP_FileName, "w");
  if ( NULL == MAP_file)
  {
     fprintf(stderr, "Could not open file %s. Error code = %d\n", config_p->MAP_FileName, errno);
     return -1;
  }
  write_MAP_header(MAP_file);
  
  Data_p = malloc(QSTORE_GDFS_MAX_PARAM_SIZE);
  if(config_p->calibrationParametersOnly)
  {
	for(i=0;i<GDFS_NoofCalibRecords;i++) {
		for(Id=GDFS_CalibList[i].StartId;
			Id<(GDFS_CalibList[i].NoofElements+GDFS_CalibList[i].StartId);Id++){
			CSPSA_Result = CSPSA_GetSizeOfValue(config_p->CSPSA_Handle,Id,&Size);
			if (Size > QSTORE_GDFS_MAX_PARAM_SIZE)
			{
				fprintf(stderr, "CSPSA2MAP: Size of CSPSA parameter %u larger than allocated size:%u\n", Size, QSTORE_GDFS_MAX_PARAM_SIZE );
				result = -1;
				goto done;
			}
			CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, Id, Size, Data_p);
			/* If some parameter is missing, we'll generate a warning
			   in the end
			*/
			if (CSPSA_Result <= T_CSPSA_RESULT_ERRORS){
				write_MAP_parameter(MAP_file,Id, Size,Data_p);
				NbrParamsCopied++;
			}
			NbrCalibrationParams++;
	 	}
	 }
	 if(NbrParamsCopied != NbrCalibrationParams)
	 	printf("CSPSA2MAP: WARNING: not all %d calibration parameters were available in CSPSA.\n", 
	 		NbrCalibrationParams);
  } else {
    // Read the first CSPSA value
    CSPSA_Result = CSPSA_GetSizeOfFirstValue(config_p->CSPSA_Handle, &Id, &Size);
    if (CSPSA_Result > T_CSPSA_RESULT_ERRORS)
    {
      fprintf(stderr, "CSPSA2MAP: Fail to get size of first value. (%d)\n", CSPSA_Result);
      result = -1;
    } else {	
      do
      {
              
        if (Size > QSTORE_GDFS_MAX_PARAM_SIZE)
        {
          fprintf(stderr, "CSPSA2MAP: Size of CSPSA parameter %u larger than allocated size:%u\n", Size, QSTORE_GDFS_MAX_PARAM_SIZE );
          result = -1;
          break;
        }
  
        CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, Id, Size, Data_p);
  
        if (CSPSA_Result > T_CSPSA_RESULT_ERRORS)
        {
          fprintf(stderr, "CSPSA2MAP: Fail to read the value. (%d)\n", CSPSA_Result);
          result = -1;
          break;
        }
  
        write_MAP_parameter(MAP_file,Id, Size,Data_p);
        NbrParamsCopied ++;
  
        CSPSA_Result = CSPSA_GetSizeOfNextValue(config_p->CSPSA_Handle, &Id, &Size);
        if (CSPSA_Result == T_CSPSA_RESULT_E_END_OF_DATA)
        {
          break;
        }
        else if (CSPSA_Result > T_CSPSA_RESULT_ERRORS)
        {
          fprintf(stderr, "CSPSA2MAP: Fail to get size of next value. (%d)\n", CSPSA_Result);
          result = -1;
          break;
        }
  
      }while (CSPSA_Result < T_CSPSA_RESULT_ERRORS);
    }
  }

done:
  printf("CSPSA2MAP: %d CSPSA Parameters have been copied to the .MAP file\n", NbrParamsCopied);
  
  if(NULL != MAP_file)
   fclose(MAP_file);
   
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

  printf("CSPSA2MAP: ITP Update: 0x%08X:%d\n", Key, BitIndex);

  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->CSPSA_Handle, Key, &Size);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  if (Size * 8 < BitIndex)
  {
    printf("CSPSA2MAP: Error size of item 0x%08X is smaller than the bit index (%d).\n", Key, BitIndex);
    goto ErrorExit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  *flag_p = (1 << BitIndex) & Value;

  Result = CONVERT_STATUS_OK;

ErrorExit:

  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
  {
    printf("CSPSA2MAP: cannot read CSPSA item 0x%08X.\n", Key);
  }
  return Result;
}

/**
 * Checks ITP update flag and returns flag_p >0 if flag is set and 0 if not.
 */
static ConvertStatus_t setItpUpdateFlag(struct Config_t* const config_p, bool flagActive)
{
  ConvertStatus_t Result = CONVERT_STATUS_UPDATE_ITEM_NOT_FOUND;
  CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_OK;
  CSPSA_Key_t Key = config_p->ItpUpdateFlagKey;
  CSPSA_Size_t Size;
  uint32_t BitIndex = config_p->ItpUpdateFlagBitIndex;
  uint32_t Value;

  printf("CSPSA2MAP: Clearing ITP Update: 0x%08X:%d\n", Key, BitIndex);

  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->CSPSA_Handle, Key, &Size);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  if (Size * 8 < BitIndex)
  {
    printf("CSPSA2MAP: Error size of item 0x%08X is smaller than the bit index (%d).\n", Key, BitIndex);
    goto ErrorExit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->CSPSA_Handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  if(flagActive)
  {
    Value = Value | (1 << BitIndex);
  }
  else
  {
    Value = Value & ~(1 << BitIndex);
  }

  CSPSA_Result = CSPSA_WriteValue(config_p->CSPSA_Handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  Result = CONVERT_STATUS_OK;

ErrorExit:

  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
  {
    printf("CSPSA2MAP: Error cannot read CSPSA item 0x%08X.\n", Key);
  }
  return Result;
}

int main(int argc, char *argv[])
{

  struct Config_t config;
  bool CommitAllowed = false;
  int result = 0;
  int UpdateFlag = 0;

  memset(&config, 0x00, sizeof(struct Config_t));

  /* Parse the input argument and if not correct print info and exit */
  if (parseCmdline(argc, argv, &config) == -1)
  {
    goto exit;
  }

  /* First we open the CSPSA area */
  if( InitCSPSA(&config) != 0)
  {
    result = -1;
    goto exit;
  }

  /* Check if the qstore file should be updated based upon entires in CSPSA */
  result = checkItpUpdateFlag(&config, &UpdateFlag);
  if (result != CONVERT_STATUS_OK)
  {
    goto exit ;
  }

  if (config.forceFileCreation || UpdateFlag)
  {
    /* reset flag before extracing CSPSA, otherwise, the ITP would reset the flag at shutdown */
    setItpUpdateFlag(&config, false);

    /* Parse CSPSA and save in qstore file */
    if ( LoadingFromCSPSA(&config) == -1)
    {
      if(UpdateFlag)
      {
        /* loading failed, restore update flag so conversion will be retried */
        setItpUpdateFlag(&config, true);
      }
      result = -1;
      goto exit;
    }
  }
  CommitAllowed = true;

exit:

  /* Clean up and close any open files etc.. */
  if ( config.CSPSA_Handle != 0x00)
  {
    UnInit(&config);
  }
  return result;
}



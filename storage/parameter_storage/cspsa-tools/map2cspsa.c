/*******************************************************************
 *
 * Copyright (C) 2011 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Writes parameters from a .map/.gdf file to CSPSA
 *
 * Author: 2011, Hans Holmberg. <hans.xh.holmbeg@stericsson.com>
 * Based upon the tool CSPSA2QSTORE created by
 * Mikael GULLBERG <mikael.xx.gullberg@stericsson.com>
 *
 * This functionality is used in U5500, in combination with map2q
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


/* The following declarations originates from map2q.c */
#define FALSE 0
#define TRUE  1

#define MAX_LINE                128
#define MAX_MAP_ARG             16
#define PARAMETERS_PER_ROW      16

#define WAITING_FOR_PARAMETER   0
#define READING_PARAMETER       1

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef unsigned char boolean;

static bool fDebug = FALSE;

typedef struct GDFS_MapData_
{
  struct GDFS_MapData_ *Next_p;
  struct GDFS_MapData_ *Prev_p;
  uint32 Id;
  uint32 Size;
  uint32 Data;
} GDFS_MAPDATA;

typedef struct GDFS_ListData_
{
  struct GDFS_ListData_ *Next_p;
  uint32 StartId;
  uint32 EndId;
} GDFS_LISTDATA;

struct Config_t
{
  CSPSA_Handle_t   CSPSA_Handle;
  char             CSPSA_Name[PATH_MAX];
  char             MAP_FileName[PATH_MAX];
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
    "Usage: map2cpsa -c cspsa_name -i mapfile [-h]\n"
    " -h              Prints this help.\n"
    " -c cspsa_name   The name of the parameter storage area to read.\n"
    " -i mapfile      The input MAP/GDF file\n"
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


  while ((r = getopt (argc, argv, "hc:i:")) != -1)
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

    case 'i':
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

  if ((set_CSPSA == false) || (set_MAP == false))
  {
    fprintf(stderr, "MAP2CSPSA: Incorrect parameter for map2cspsa. Example use MAP2CSPSA -c CSPSA0 -i calibdata.map\n");
    result = -1;
  }
  else
  {
     fprintf(stdout, "MAP2CSPSA: write parameters from  MAP[%s] to CSPSA[%s] \n", config_p->MAP_FileName, config_p->CSPSA_Name);
  }

exit:
  return result;
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
    fprintf(stderr, "MAP2CSPSA: Fail to open CSPSA (%d).\n", CSPSA_Result);
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
      fprintf(stderr, "MAP2CSPSA: Fail to close CSPSA %d. \n",CSPSA_Result);
  }
  return;
}

/* The following function (with dependant type declerations) originates from map2q.c */
static bool ReadMapFile( char *psFile, uint32 *ParCount_p, GDFS_MAPDATA **Head_pp, GDFS_MAPDATA **Tail_pp)
{
  FILE *pFile;
  char psLineBuffer[MAX_LINE];
  short wLine;
  bool rc, Error, fEndFound;
  short ArgCount, Decode, k;
  char *ArgVector[MAX_MAP_ARG + 1];
  short State = WAITING_FOR_PARAMETER;
  GDFS_MAPDATA *New_p=NULL;
  uint32 Id = 0, Size = 0, WrPos = 0;
  char *Dummy_p = NULL;
  uint8 *Data_p = NULL;
  uint32 ParCount = 0;
  GDFS_MAPDATA *Head_p = NULL;
  GDFS_MAPDATA *Tail_p = NULL;

  pFile = fopen(psFile, "r");
  if ( pFile == NULL )
  {
    fprintf(stderr, "Could not open file %s\n", psFile);
    return(FALSE);
  }

  wLine = 1;
  Error = FALSE;
  fEndFound = FALSE;
  ParCount = 0;

  while ( fgets( psLineBuffer, sizeof( psLineBuffer), pFile) != NULL && !Error && !fEndFound )
  {
    if ( psLineBuffer[0] != '/' )  /* Check that line not is a comment */
    {
      GetArg( psLineBuffer, MAX_MAP_ARG, TRUE, &ArgCount, ArgVector);

      if ( ArgCount > 0 )
      {
        switch( State)
        {
        case WAITING_FOR_PARAMETER:
          if ( ArgCount == 3 )   /* Typical first line for a parameter */
          {
            Id = strtoul( ArgVector[0], &Dummy_p, 16);
            Size = strtoul( ArgVector[1], &Dummy_p, 10);  /* This parameter is in decimal format. */

            New_p = (GDFS_MAPDATA *)malloc( sizeof(GDFS_MAPDATA) + Size - sizeof( uint32));
            New_p->Next_p = NULL;
            New_p->Id = Id;
            New_p->Size = Size;

            /* Configuration variables for parameter reading. */
            Data_p = (uint8 *)&New_p->Data;
            WrPos = 0;
            State = READING_PARAMETER;
            ParCount++;

            /* Link into single linked list. */
            if ( Head_p == NULL )
            {
              Head_p = New_p;
              New_p->Prev_p = NULL;
            }
            else
            {
              Tail_p->Next_p = New_p;
              New_p->Prev_p = Tail_p;
            }

            Tail_p = New_p;
          }
          else
          {
            fprintf(stderr, "Erroneus data at line %d beginning %s... \n", wLine, ArgVector[0]);
            Error = TRUE;
          }
          break;

        case READING_PARAMETER:

          if ( Size - WrPos >= PARAMETERS_PER_ROW )
            Decode = PARAMETERS_PER_ROW; else
            Decode = (short)(Size - WrPos);

          if ( Decode != ArgCount )
          {
            fprintf(stderr, "Erroneus number of paramters at line %d. Found %d, expected %d\n", wLine, Decode, ArgCount);
            Error = TRUE;
            break;
          }

          /* Store data */
          for ( k = 0 ; k < Decode ; k++ )
          {
            Data_p[WrPos] = (uint8)strtoul( ArgVector[k], &Dummy_p, 16);
            WrPos++;
          }

          /* All data for the parameter is decoded */
          if ( WrPos == Size )
            State = WAITING_FOR_PARAMETER;

          break;
        }
      }
    }

    wLine++;
  }

  if ( fDebug )
  {
    fprintf(stderr, "Found %d parameters in map file\n", ParCount);
  }

  fclose( pFile);

  *ParCount_p = ParCount;
  *Head_pp = Head_p;
  *Tail_pp = Tail_p;

  if ( !Error )
  {
    rc = TRUE;
  }
  else
  {
    rc = FALSE;
  }

  return( rc);
}
/**
 * Writes a GDFS parameter list to CSPSA
 *
 */
static int write_parameters_to_cspsa(GDFS_MAPDATA *Head_p, struct Config_t* config_p){

  CSPSA_Result_t     	CSPSA_Result;
  CSPSA_Key_t		Id = 0;
  CSPSA_Size_t		Size = 0;
  uint8_t		*Data_p = NULL;
  int			NbrParamsCopied = 0;
  int			result = 0;
  FILE			*MAP_file = NULL;
  GDFS_MAPDATA		*This_p = NULL;

  /* Open the input file*/
  MAP_file= fopen(config_p->MAP_FileName, "r");
  if ( NULL == MAP_file){
    fprintf(stderr, "Could not open file %s. Error code = %d\n", config_p->MAP_FileName, errno);
    return -1;
  }

  for( This_p = Head_p ; This_p != NULL ; This_p = This_p->Next_p ){
    Id = This_p->Id;
    Size = This_p->Size;
    Data_p = (uint8_t *)(&This_p->Data);
    CSPSA_Result = CSPSA_WriteValue(config_p->CSPSA_Handle, Id, Size, Data_p);

    if(CSPSA_Result >= T_CSPSA_RESULT_ERRORS){
      fprintf(stderr, "CSPSA2MAP: Failed to write parameter 0x%08X\n",Id);
      result = -1;
      goto error;
    }

    if(fDebug)
      printf("Wrote parameter 0x%08X\n",Id);

    NbrParamsCopied ++;
  }
  printf("MAP2CSPSA: %d Parameters have been written to CSPSA\n", NbrParamsCopied);
error:
  if(NULL != MAP_file)
    fclose(MAP_file);

  return result;
}

int main(int argc, char *argv[])
{
  struct Config_t config;
  GDFS_MAPDATA *Source_MapHead_p, *Source_MapTail_p;
  uint32 Noof_Source_Parameters = 0;
  int result = 0;

  memset(&config, 0x00, sizeof(struct Config_t));

  /* Parse the input argument and if not correct print info and exit */
  if (parseCmdline(argc, argv, &config) == -1)
  {
    result = -1;
    goto exit;
  }

  /* First we open the CSPSA area */
  if( InitCSPSA(&config) != 0)
  {
    result = -1;
    goto exit;
  }

  if(!ReadMapFile( config.MAP_FileName, &Noof_Source_Parameters, &Source_MapHead_p, &Source_MapTail_p)){
    fprintf(stderr, "CSPSA2MAP: Failed to open input map file.\n");
    result = -1;
    goto exit;
  }

  if(0 != write_parameters_to_cspsa(Source_MapHead_p, &config)){
    fprintf(stderr, "CSPSA2MAP: Failed to write parameters to CSPSA.\n");
    result = -1;
    goto exit;
  }

exit:

  /* Clean up and close any open files etc.. */
  if(config.CSPSA_Handle != 0x00){
    UnInit(&config);
  }

  return result;
}


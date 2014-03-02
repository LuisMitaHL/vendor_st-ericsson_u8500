/************************************************************************
*      $Copyright ST-Ericsson 2011 $                                    *
* Name:     map2q                                                       *
*                                                                       *
* Program for reading GDFS parameter map file and generate q-file       *
************************************************************************/
#ifdef LINUX
#define O_BINARY 0
#define S_IREAD S_IRUSR
#define S_IWRITE S_IWUSR
#endif

#include <stdio.h>
#if defined(MSDEV) || defined(LINUX)
  #define FALSE 0
  #define TRUE  1
#else
  #include <stddefs.h>
  #include <mem.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#ifdef LINUX 
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#else
#include <io.h>
#endif

#include <sys/stat.h>
#include "getarg.h"
#include <time.h>



/**************
*   Defines   *
**************/

#define QFORMAT_REVISION        2

#define MAX_LINE                128
#define MAX_MAP_ARG             16
#define PARAMETERS_PER_ROW      16

#define WAITING_FOR_PARAMETER   0
#define READING_PARAMETER       1

#define ALIGN_MASK              0x03
#define ALIGN_SIZE              4

#define MAX_CALIB_ARG           3

/**************
*   Typedefs  *
**************/

#ifdef LINUX
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef unsigned char boolean;
#else
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
#endif

typedef char bool;

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

typedef struct
{
  uint32 Revision;
  uint32 OldestRevision;
  uint32 NoofParameters;
  uint32 CheckSum;
  uint32 DataPosition;
  uint32 DataSize;
  uint32 StorageLevel;
  uint32 StorageCount;
} QHEADER;

/**********************
*   Global variables  *
**********************/

static bool fDebug;
static bool fVerbose;

/**********************
*   Prototypes        *
**********************/
static void ShowHelp( void);
static bool ReadMapFile( char *psFile, uint32 *ParCount_p, GDFS_MAPDATA **Head_pp, GDFS_MAPDATA **Tail_pp);
static bool ReadCalibListFile( char *psFile, uint32 *ParCount_p, GDFS_LISTDATA **Head_pp, GDFS_LISTDATA **Tail_pp);
static bool ExtractCalibParameters( GDFS_MAPDATA **Map_Head_pp, GDFS_MAPDATA **Map_Tail_pp, GDFS_LISTDATA *Calib_Head_p);
static bool MergeMapFiles( uint32 *NoofParameters_p, GDFS_MAPDATA **Map_Head_pp, GDFS_MAPDATA **Map_Tail_pp,
                           GDFS_MAPDATA **Calib_Head_pp, GDFS_MAPDATA **Calib_Tail_pp);
static bool MakeQFile( char *psFile, uint32 NoofParameters, GDFS_MAPDATA *Head_p, bool fSourceFile, char *psVariable);

/********************************************************
*       main                                            *
********************************************************/

int main(int argc, char *argv[])
{
  char *psSourceFile = NULL, *psDestFile = NULL, *psCalibFile = NULL;
  char *psMergeFile = NULL, *psVariableName = NULL;
  short wArg;
  uint32 Noof_Source_Parameters, Noof_Merge_Parameters, Noof_Calib_Records;
  GDFS_MAPDATA *Source_MapHead_p, *Source_MapTail_p;
  GDFS_MAPDATA *Merge_MapHead_p, *Merge_MapTail_p;
  GDFS_LISTDATA *Calib_Head_p, *Calib_Tail_p;
  bool fMerge, fCalib, fSource;

  psSourceFile = NULL;
  psVariableName = NULL;
  fMerge = FALSE;
  fCalib = FALSE;
  fSource = FALSE;

  if ( argc >= 3 && argc <= 7 )
  {
    /* Input file. */
    psSourceFile = argv[1];

    for ( wArg = 2 ; wArg < argc - 1 ; wArg++ )
    {
      if ( argv[wArg][0] == '-' || argv[wArg][0] == '/' )
      {
        /* The parameter is an option. */
        switch( argv[wArg][1] )
        {
        case 'c':
        case 'C':
          fCalib = TRUE;
          psCalibFile = &argv[wArg][2];
          break;

        case 'd':
        case 'D':
          fDebug = TRUE;
          break;

        case 'm':
        case 'M':
          fMerge = TRUE;
          psMergeFile = &argv[wArg][2];
          break;

        case 's':
        case 'S':
          /* Generate output data as C source code */
          fSource = TRUE;
          psVariableName = &argv[wArg][2];
          break;

        case 'v':
        case 'V':
          fVerbose = TRUE;
          break;

        case '?':
          ShowHelp();
          exit( 0);

        default:
          printf( "Unknown option '%c' specified.\n", argv[wArg][1]);
          ShowHelp();
          printf( "Exit!\n");
          exit( -1);
        }
      }
    }
  }
  else
  {
    printf( "Wrong number of parameters\n");
    ShowHelp();
    printf( "Exit!\n");
    exit( -1);
  }

  psDestFile = argv[argc - 1];

  /* Check parameters */
  if ( fMerge && !fCalib )
  {
    printf( "No calibration list file specified for merge operation.\n");
    ShowHelp();
    printf( "Exit!\n");
    exit( -1);
  }

  /* Read and parse map file */
  if ( fVerbose )
  {
    printf( "Reading input file %s ...\n", psSourceFile);
  }

  if ( !ReadMapFile( psSourceFile, &Noof_Source_Parameters, &Source_MapHead_p, &Source_MapTail_p) )
  {
    exit( -1);
  }

  if ( fVerbose )
  {
    printf( "Read %d parameters in input file.\n", Noof_Source_Parameters);
  }

  if ( fMerge )
  {
    if ( fVerbose )
    {
      printf( "Reading merge file %s ...\n", psMergeFile);
    }

    if ( !ReadMapFile( psMergeFile, &Noof_Merge_Parameters, &Merge_MapHead_p, &Merge_MapTail_p) )
    {
      exit( -1);
    }

    if ( fVerbose )
    {
      printf( "Read %d parameters in merge file.\n", Noof_Merge_Parameters);
    }

    if ( fVerbose )
    {
      printf( "Reading calibration list file file %s ...\n", psMergeFile);
    }

    if ( !ReadCalibListFile( psCalibFile, &Noof_Calib_Records, &Calib_Head_p, &Calib_Tail_p) )
    {
      exit( -1);
    }

    if ( fVerbose )
    {
      printf( "Extract calibration parameters from merge data ...\n");
    }

    if ( !ExtractCalibParameters( &Merge_MapHead_p, &Merge_MapTail_p, Calib_Head_p) )
    {
      exit( -1);
    }

    if ( fVerbose )
    {
      printf( "Merge calibration data to destination data ...\n");
    }

    if ( !MergeMapFiles( &Noof_Source_Parameters, &Source_MapHead_p, &Source_MapTail_p,
          &Merge_MapHead_p, &Merge_MapTail_p) )
    {
      exit( -1);
    }
  }

  /* Data is now available in linked list MapHead_p */
  if ( fVerbose )
  {
    printf( "Generating output file %s ...\n", psDestFile);
  }

  if ( !MakeQFile( psDestFile, Noof_Source_Parameters, Source_MapHead_p, fSource, psVariableName) )
  {
    exit( -1);
  }

  if ( fVerbose )
  {
    printf( "Output file %s successfully generated\n", psDestFile);
  }

  return( 0);
}

/********************************************************
*       ShowHelp                                        *
********************************************************/

static void ShowHelp( void)
{
  printf( "Usage:\n\n");
  printf( "map2q <input_file> [-d] [-v] [-s<variable name>] [-m<merge_file> -c<calib_list_file>] <output_file>\n\n");
  printf( "Parameters (all optional):\n");
  printf( "-d\tGives some extra detailed information during execution.\n");
  printf( "-v\tGives progress information during executon.\n");
  printf( "-s\tData in output file will be C source code with variable name <variable name> for the array.\n");
  printf( "\tData format is the same as q-file\n");
  printf( "-m\tSpecifies merge_file of /map/gdf/ type containing calibration data.\n");
  printf( "-c\tSpecifies a file containing a list of parameters being calibration\n\tparameters.\n\n");
  printf( "<input_file> is type /map/gdf/ file. <output_file> is a binary file of q-file.\n");
  printf( "format. When option '-m' is used, the parameter '-c' must also be specifed.\n");
}

/********************************************************
*       ReadMapFile                                     *
*       Reads map file for GDFS.                        *
*       Returns TRUE if successful.                     *
********************************************************/

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

  pFile = fopen( psFile, "r");
  if (pFile == NULL )
  {
    printf( "Could not open file %s\n", psFile);
    return( FALSE);
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
            printf( "Erroneus data at line %d beginning %s... \n", wLine, ArgVector[0]);
            Error = TRUE;
          }
          break;

        case READING_PARAMETER:
          if ( Size - WrPos >= PARAMETERS_PER_ROW )
          {
            Decode = PARAMETERS_PER_ROW;
          }
          else
          {
            Decode = (short)(Size - WrPos);
          }

          if ( Decode != ArgCount )
          {
            printf( "Erroneus number of paramters at line %d. Found %d, expected %d\n", wLine, Decode, ArgCount);
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
          {
            State = WAITING_FOR_PARAMETER;
          }
          break;
        }
      }
    }

    wLine++;
  }

  if ( fDebug )
  {
    printf( "Found %d parameters in map file\n", ParCount);
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

/********************************************************
*       ReadCalibListFile                               *
*       Reads list file for calibration parameters.     *
*       Returns TRUE if successful.                     *
********************************************************/

static bool ReadCalibListFile( char *psFile, uint32 *ParCount_p, GDFS_LISTDATA **Head_pp, GDFS_LISTDATA **Tail_pp)
{
  FILE *pFile;
  char psLineBuffer[MAX_LINE];
  short wLine;
  bool rc;
  short ArgCount, ParCount;
  char *ArgVector[MAX_CALIB_ARG + 1];
  GDFS_LISTDATA *New_p;
  uint32 StartId, EndId;
  char *Dummy_p;
  GDFS_LISTDATA *Head_p = NULL;
  GDFS_LISTDATA *Tail_p = NULL;

  pFile = fopen( psFile, "r");
  if (pFile == NULL )
  {
    printf( "Could not open file %s\n", psFile);
    return( FALSE);
  }

  wLine = 1;
  ParCount = 0;

  while ( fgets( psLineBuffer, sizeof( psLineBuffer), pFile) != NULL )
  {
    if ( psLineBuffer[0] != '#' )  /* Check that line not is a comment */
    {
      GetArg( psLineBuffer, MAX_CALIB_ARG, TRUE, &ArgCount, ArgVector);

      if ( ArgCount > 0 )
      {
        StartId = strtoul( ArgVector[0], &Dummy_p, 10);

        if ( ArgCount > 1 )
        {
          EndId = strtoul( ArgVector[1], &Dummy_p, 10);
        }
        else
        {
          EndId = 0;
        }

        New_p = (GDFS_LISTDATA *)malloc( sizeof(GDFS_LISTDATA));
        New_p->Next_p = NULL;
        New_p->StartId = StartId;
        New_p->EndId = EndId;

        /* Configuration variables for parameter reading. */
        ParCount++;

        /* Link into single linked list. */
        if ( Head_p == NULL )
        {
          Head_p = New_p;
        }
        else
        {
          Tail_p->Next_p = New_p;
        }

        Tail_p = New_p;
      }
    }

    wLine++;
  }
  fclose(pFile);

  if ( fDebug )
  {
    printf( "Found %d records and %d lines in %s \n", ParCount, wLine, psFile);
  }

  if ( ParCount > 0 )
  {
    *ParCount_p = ParCount;
    *Head_pp = Head_p;
    *Tail_pp = Tail_p;
    rc = TRUE;
  }
  else
  {
    rc = FALSE;
  }

  return( rc);
}

/********************************************************
*       ExtractCalibParameters                          *
*       Removes all non calibration parameters.         *
*       Returns TRUE if successful.                     *
********************************************************/

static bool ExtractCalibParameters( GDFS_MAPDATA **Map_Head_pp, GDFS_MAPDATA **Map_Tail_pp, GDFS_LISTDATA *Calib_Head_p)
{
  GDFS_MAPDATA *ThisParameter_p, *Map_Head_p, *Map_Tail_p, *Next_p;
  GDFS_LISTDATA *ThisRecord_p;
  uint32 NextCalibId=0, CalibCount=0;
  bool rc = FALSE;

  /* Initial parameters. */
  Map_Head_p = *Map_Head_pp;
  Map_Tail_p = *Map_Tail_pp;

  /* Initialize calibration list parameters. */
  ThisRecord_p = Calib_Head_p;
  if(ThisRecord_p)
    NextCalibId = ThisRecord_p->StartId;

  if ( Map_Head_p && (Map_Head_p->Prev_p != NULL) )
  {
    printf( "Head not properly initialized\n");
    return( FALSE);
  }
  if ( Map_Tail_p && (Map_Tail_p->Next_p != NULL) )
  {
    printf( "Tail not properly initialized\n");
    return( FALSE);
  }

  ThisParameter_p = Map_Head_p;
  while( ThisParameter_p != NULL )
  {
    if ( ThisParameter_p->Id < NextCalibId )
    {
      Next_p = ThisParameter_p->Next_p;

      /* Remove this record from linked list. */
      if ( ThisParameter_p->Prev_p != NULL )
      {
        ThisParameter_p->Prev_p->Next_p = ThisParameter_p->Next_p;
      }
      else
      {
        Map_Head_p = ThisParameter_p->Next_p;
      }

      if ( ThisParameter_p->Next_p != NULL )
      {
        ThisParameter_p->Next_p->Prev_p = ThisParameter_p->Prev_p;
      }
      else
      {
        Map_Tail_p = ThisParameter_p->Prev_p;
      }

      free( (void *)ThisParameter_p);

      ThisParameter_p = Next_p;
    }
    else
    {
      if ( ThisParameter_p->Id == NextCalibId )
      {
        /* Keep this parameter in list. */
        ThisParameter_p = ThisParameter_p->Next_p;
        CalibCount++;
        rc = TRUE;  /* At least one parameter has been found. */
      }
      else if ( ThisParameter_p->Id > NextCalibId )
      {
        /* The calibration parameter is missing in the map file. */
        if ( fVerbose )
        {
          printf( "WARNING! Calibration data for parameter %d (0x%x) missing in merge file\n", NextCalibId, NextCalibId);
        }
      }

      /* Get next calibration parameter */
      if ( ThisRecord_p != NULL )
      {
        if ( ThisRecord_p->EndId == 0 || ThisRecord_p->EndId == NextCalibId )
        {
          /* Time to get next record */
          ThisRecord_p = ThisRecord_p->Next_p;
          if ( ThisRecord_p != NULL )
          {
            NextCalibId = ThisRecord_p->StartId;
          }
          else
          {
            NextCalibId = 0xFFFFFFFFL; /* This number will not be found in the list. */
          }
        }
        else
        {
          /* There are multiple calibration parameters for this record. */
          NextCalibId++;
        }
      }
    }
  }

  if ( fDebug )
  {
    printf( "Found %d calibration parameters in merge file\n", CalibCount);
  }

  /* Set out parameters. */
  *Map_Head_pp = Map_Head_p;
  *Map_Tail_pp = Map_Tail_p;

  return( rc);
}

/********************************************************
*       MergeMapFiles                                   *
*       Merges calibration data to map data.             *
*       Returns TRUE if successful.                     *
********************************************************/

static bool MergeMapFiles( uint32 *NoofParameters_p, GDFS_MAPDATA **Map_Head_pp, GDFS_MAPDATA **Map_Tail_pp,
                           GDFS_MAPDATA **Calib_Head_pp, GDFS_MAPDATA **Calib_Tail_pp)
{
  GDFS_MAPDATA *ThisParameter_p, *Map_Head_p, *Map_Tail_p, *ThisCalib_p;
  GDFS_MAPDATA *Calib_Head_p, *Calib_Tail_p;
  GDFS_MAPDATA *InsertAfter_p;
  uint32 NoofParameters;
  bool rc;

  /* Initial parameters. */
  Map_Head_p = *Map_Head_pp;
  Map_Tail_p = *Map_Tail_pp;
  NoofParameters = *NoofParameters_p;
  Calib_Head_p = *Calib_Head_pp;
  Calib_Tail_p = *Calib_Tail_pp;

  ThisParameter_p = Map_Head_p;
  ThisCalib_p = Calib_Head_p;

  while( ThisParameter_p != NULL && ThisCalib_p != NULL )
  {
    if ( ThisParameter_p->Id < ThisCalib_p->Id )
    {
      /* Move to next parameter */
      ThisParameter_p = ThisParameter_p->Next_p;
    }
    else if ( ThisParameter_p->Id == ThisCalib_p->Id )
    {
      /* Remove this parameter from the calibration data inked list. */
      if ( ThisCalib_p->Prev_p != NULL )
      {
        ThisCalib_p->Prev_p->Next_p = ThisCalib_p->Next_p;
      }
      else
      {
        Calib_Head_p = ThisCalib_p->Next_p;
      }

      if ( ThisCalib_p->Next_p != NULL )
      {
        ThisCalib_p->Next_p->Prev_p = ThisCalib_p->Prev_p;
      }
      else
      {
        Calib_Tail_p = ThisCalib_p->Prev_p;
      }

      ThisCalib_p->Prev_p = NULL;
      ThisCalib_p->Next_p = NULL;

      /* Save where to insert element */
      InsertAfter_p = ThisParameter_p->Prev_p;

      /* Remove this record from linked list. */
      if ( ThisParameter_p->Prev_p != NULL )
      {
        ThisParameter_p->Prev_p->Next_p = ThisParameter_p->Next_p;
      }
      else
      {
        Map_Head_p = ThisParameter_p->Next_p;
      }

      if ( ThisParameter_p->Next_p != NULL )
      {
        ThisParameter_p->Next_p->Prev_p = ThisParameter_p->Prev_p;
      }
      else
      {
        Map_Tail_p = ThisParameter_p->Prev_p;
      }

      free( (void *)ThisParameter_p);

      /* Insert ThisCalib_p in Map list. */

      if ( InsertAfter_p == NULL )
      {
        /* Insert as first element in list. */
        Map_Head_p->Prev_p = ThisCalib_p;
        ThisCalib_p->Next_p = Map_Head_p;
        Map_Head_p = ThisCalib_p;
      }
      else
      {
        if ( InsertAfter_p->Next_p != NULL )
        {
          InsertAfter_p->Next_p->Prev_p = ThisCalib_p;
        }
        else
        {
          Map_Tail_p = ThisCalib_p;
        }
        ThisCalib_p->Next_p = InsertAfter_p->Next_p;

        InsertAfter_p->Next_p = ThisCalib_p;
        ThisCalib_p->Prev_p = InsertAfter_p;
      }

      /* Update parameters */
      ThisParameter_p = ThisCalib_p->Next_p;
      ThisCalib_p = Calib_Head_p;
    }
    else if ( ThisParameter_p->Id > ThisCalib_p->Id )
    {
      /* Remove this parameter from the calibration data inked list. */
      if ( ThisCalib_p->Prev_p != NULL )
      {
        ThisCalib_p->Prev_p->Next_p = ThisCalib_p->Next_p;
      }
      else
      {
        Calib_Head_p = ThisCalib_p->Next_p;
      }

      if ( ThisCalib_p->Next_p != NULL )
      {
        ThisCalib_p->Next_p->Prev_p = ThisCalib_p->Prev_p;
      }
      else
      {
        Calib_Tail_p = ThisCalib_p->Prev_p;
      }

      ThisCalib_p->Prev_p = NULL;
      ThisCalib_p->Next_p = NULL;

      /* Insert ThisCalib_p before ThisParameter_p */
      if ( ThisParameter_p->Prev_p != NULL )
      {
        ThisParameter_p->Prev_p->Next_p = ThisCalib_p;
      }
      else
      {
        Map_Head_p = ThisCalib_p;
      }
      ThisCalib_p->Prev_p = ThisParameter_p->Prev_p;

      ThisParameter_p->Prev_p = ThisCalib_p;
      ThisCalib_p->Next_p = ThisParameter_p;

      NoofParameters++;

      /* Update parameters */
      ThisCalib_p = Calib_Head_p;
    }
  }

  /* The merge list should be empty. */
  if ( Calib_Head_p == NULL && Calib_Tail_p == NULL )
  {
    rc = TRUE;
  }
  else
  {
    rc = FALSE;
  }

  if ( fDebug )
  {
    printf( "After merge there are %d parameters in file\n", NoofParameters);
  }

  /* Set out parameters. */
  *Map_Head_pp = Map_Head_p;
  *Map_Tail_pp = Map_Tail_p;
  *NoofParameters_p = NoofParameters;
  *Calib_Head_pp = Calib_Head_p;
  *Calib_Tail_pp = Calib_Tail_p;

  return( rc);
}

/********************************************************
*       MakeQFile                                       *
*       Generates binary Q-file.                        *
*       Returns TRUE if successful.                     *
********************************************************/

static bool MakeQFile( char *psFile, uint32 NoofParameters, GDFS_MAPDATA *Head_p, bool fSourceFile, char *psVariable)
{
  int Handle = -1, Len;
  GDFS_MAPDATA *This_p;
  uint32 DataSize, ThisSize, FillCount;
  uint32 *DataArea_p;
  uint32 *CurrPos_p;
  uint8  *FillPos_p, *WritePos_p;
  uint32 CRC, k;
  QHEADER QHeader;
  FILE *pFile = NULL;
  time_t TimeValue;
  struct tm *CurrTime_p;

  if ( fSourceFile )
  {
    pFile = fopen( psFile, "wt");
    if (pFile == NULL )
    {
      printf( "Could not open file %s\n", psFile);
      return( FALSE);
    }
  }
  else
  {
    Handle = open( psFile, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, S_IREAD | S_IWRITE);
    if ( Handle == -1 )
    {
      printf( "Could not open file %s. Error code = %d\n", psFile, errno);
      return( FALSE);
    }
  }

  /* Calculate size for Data area */
  DataSize = 0;
  for ( This_p = Head_p ; This_p != NULL ; This_p = This_p->Next_p )
  {
    /* Calculate size for this record. */
    ThisSize = This_p->Size;
    if ( ThisSize & ALIGN_MASK )
    {
      ThisSize += ALIGN_SIZE;
      ThisSize &= ~ALIGN_MASK;
    }

     ThisSize += 2 * sizeof( uint32); /* Id and Size */

    /* Store accumulated result */
    DataSize += ThisSize;
  }

  if ( fDebug )
  {
    printf( "Q-file data range size is %d bytes\n", DataSize);
  }

  DataArea_p = (uint32 *)malloc( DataSize);
  CurrPos_p = DataArea_p;

  /* Copy data to the data range */
  for ( This_p = Head_p ; This_p != NULL ; This_p = This_p->Next_p )
  {
    CurrPos_p[0] = This_p->Id;
    CurrPos_p[1] = This_p->Size;
    CurrPos_p += 2;

    memcpy( (uint8 *)CurrPos_p, (uint8 *)&This_p->Data, This_p->Size);

    /* Is there need to fill data for alignment */
    if ( This_p->Size & ALIGN_MASK )
    {
      FillPos_p = (uint8 *)CurrPos_p + This_p->Size;
      FillCount = ALIGN_SIZE - (This_p->Size & ALIGN_MASK);
      memset( FillPos_p, 0x00, FillCount);
      CurrPos_p += 1 + This_p->Size / sizeof( uint32);
    }
    else
    {
      CurrPos_p += This_p->Size / sizeof( uint32);
    }
  }

  /* Check that CurrPos_p not is beyond limit. */
  if ( CurrPos_p != DataArea_p + DataSize / sizeof( uint32) )
  {
    uint32 Used;

    Used = (uint8 *)CurrPos_p - (uint8 *)DataArea_p;

    printf( "Data size ended up wrong. Used = %d, Expected = %d\n", Used, DataSize);
  }

  /* Calculate checksum for data range. */
  CRC = 0;
  CurrPos_p = DataArea_p;
  for ( k = 0 ; k < DataSize / sizeof( uint32) ; k++ )
  {
    CRC ^= *CurrPos_p;
    CurrPos_p++;
  }

  /* Fill Header information. */
  QHeader.Revision = QFORMAT_REVISION;
  QHeader.OldestRevision = 1;
  QHeader.NoofParameters = NoofParameters;
  QHeader.CheckSum = CRC;
  QHeader.DataPosition = sizeof( QHEADER);
  QHeader.DataSize = DataSize;
  QHeader.StorageLevel = 0;
  QHeader.StorageCount = 0;

  if ( fSourceFile )
  {
    TimeValue = time( &TimeValue);
    CurrTime_p = gmtime( &TimeValue);

    fprintf( pFile, "/*************************************************************************\n");
    fprintf( pFile, " * $Copyright ST-Ericsson %d $\n", CurrTime_p->tm_year + 1900);
    fprintf( pFile, " *\n");
    fprintf( pFile, " * This file is autogenerated fom map2q using option -s. Example\n");
    fprintf( pFile, " *    map2q <input_file> -s<variable_name> <output_file>\n");
    fprintf( pFile, "*************************************************************************/\n\n");
    fprintf( pFile, "#include \"r_basicdefinitions.h\"\n\n");
    fprintf( pFile, "uint8 %s[] = {\n", psVariable);
    fprintf( pFile, "/* Header */\n");

    WritePos_p = (uint8 *)&QHeader;
    for ( k = 0 ; k < sizeof( QHEADER) ; k++ )
    {
      fprintf( pFile, "0x%02x,", WritePos_p[k]);
      if ( ((k + 1) & 0x0F) == 0 )
      {
        fprintf( pFile, "\n");
      }
      else
      {
        fprintf( pFile, " ");
      }
    }

    if ( (k & 0x0F) != 0 )
    {
      /* Loop did not end with newline */
      fprintf( pFile, "\n");
    }
  }
  else
  {
    Len = write( Handle, (uint8 *)&QHeader, sizeof( QHeader));
    if ( Len != sizeof( QHeader) )
    {
      printf( "Failed to write data to output file. Error code = %d\n", errno);
      free( DataArea_p);
      close( Handle);
      return( FALSE);
    }
  }

  if ( fSourceFile )
  {
    fprintf( pFile, "/* Data */\n");
    WritePos_p = (uint8 *)DataArea_p;
    for ( k = 0 ; k < DataSize ; k++ )
    {
      fprintf( pFile, "0x%02x", WritePos_p[k]);
      if ( k < DataSize - 1 )
      {
        fprintf( pFile, ",");
      }

      if ( ((k + 1) & 0x0F) == 0 )
      {
        fprintf( pFile, "\n");
      }
      else
      {
        fprintf( pFile, " ");
      }
    }

    if ( (k & 0x0F) != 0 )
    {
      /* Loop did not end with newline */
      fprintf( pFile, "\n");
    }

    fprintf( pFile, "};\n\n");
  }
  else
  {
    Len = write( Handle, (uint8 *)DataArea_p, DataSize);
    if ( (uint32)Len != DataSize )
    {
      printf( "Failed to write data to output file. Error code = %d\n", errno);
      free( DataArea_p);
      close( Handle);
      return( FALSE);
    }
  }

  free( DataArea_p);

  if ( fSourceFile )
  {
    fclose( pFile);
  }
  else
  {
    close( Handle);
  }

  return( TRUE);
}


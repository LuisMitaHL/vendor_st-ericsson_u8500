/************************************************************************
*      $Copyright Ericsson AB 2009 $                                    *
* Name:     q2map                                                       *
*                                                                       *
* Program for reading q-file and generate GDFS map file                 *
************************************************************************/

#ifdef LINUX
#define O_BINARY 0
#endif

#include <stdio.h>
#if defined(MSDEV) || defined(LINUX)
#define FALSE 0
#define TRUE  1
#else
#include "stddefs.h"
#include "mem.h"
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

/**************
*   Defines   *
**************/

#define QFORMAT_REVISION        2

#define MAX_LINE                128
#define MAX_ARG                 16
#define PARAMETERS_PER_ROW      16

#define WAITING_FOR_PARAMETER   0
#define READING_PARAMETER       1

#define ALIGN_MASK              0x03
#define ALIGN_SIZE              4

/**************
*   Typedefs  *
**************/

#ifdef LINUX
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef unsigned char bool;
#else
typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
#endif

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
static bool fForce;

/**********************
*   Prototypes        *
**********************/
static void ShowHelp( void);
static bool QFORMAT_ReadFile( char *QFileName_p, uint32 *NoofParameters_p, uint8 **DataArea_pp, uint32 *DataSize_p);
static bool MakeMapFile( char *psFile, uint32 NoofParameters, uint8 *DataArea_p);

/********************************************************
*       main                                            *
********************************************************/

int main(int argc, char *argv[])
{
  char *psSourceFile, *psDestFile;
  short wArg;
  uint32 Parameters;
  uint8 *DataArea_p;
  uint32 DataSize;

  psSourceFile = NULL;
  fDebug = FALSE;
  fVerbose = FALSE;

  if ( argc >= 3 && argc <= 6 )
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
        case 'd':
        case 'D':
          fDebug = TRUE;
          break;

        case 'f':
        case 'F':
          fForce = TRUE;
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

  /* Read and parse map file */
  if ( fVerbose )
  {
    printf( "Reading input file %s ...\n", psSourceFile);
  }

  if ( !QFORMAT_ReadFile( psSourceFile, &Parameters, &DataArea_p, &DataSize) )
  {
    exit( -1);
  }

  /* Data is now available in linked list MapHead_p */
  if ( fVerbose )
  {
    printf( "Generating output file %s ...\n", psDestFile);
  }

  if ( !MakeMapFile( psDestFile,Parameters, DataArea_p) )
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
  printf( "q2map <input_file> [-d] [-v] [-f] <output_file>\n\n");
  printf( "Parameters (all optional):\n");
  printf( "-d\tGives some extra detailed information during execution.\n");
  printf( "-v\tGives progress information during execution.\n");
  printf( "-f\tDiscards checksum error and tries to read file anyway.\n\n");
  printf( "<input_file> is q-file format data file. <output_file> is a GDFS map file.\n");
}

/********************************************************
*       QFORMAT_ReadFile                                *
*       Reads file of Q-format.                         *
*       Returns TRUE if successful.                     *
********************************************************/

static bool QFORMAT_ReadFile( char *QFileName_p, uint32 *NoofParameters_p, uint8 **DataArea_pp, uint32 *DataSize_p)
{
  int Handle, Size;
  uint8 *Data_p;
  uint32 *CurrPos_p;
  QHEADER QHeader;
  uint32 CRC, k;

  Handle = open( QFileName_p, O_RDONLY | O_BINARY, 0);
  if ( Handle == -1 )
  {
    printf( "\nQFORMAT: Cannot open file %s", QFileName_p);
    return( FALSE);
  }

  Size = read( Handle, (uint8 *)&QHeader, sizeof( QHEADER));
  if ( Size != sizeof( QHEADER) )
  {
    printf( "\nQFORMAT: Could not read header data from file %s", QFileName_p);
    close( Handle);
    return( FALSE);
  }

  if ( QHeader.OldestRevision > QFORMAT_REVISION )
  {
    printf( "\nQFORMAT: Revision in file is not backward compatible. Current revision: %d. Oldest supported revision in file: %d", QFORMAT_REVISION, QHeader.OldestRevision);
    close( Handle);
    return( FALSE);
  }

  if ( QHeader.Revision != QFORMAT_REVISION )
  {
    long Position;

    printf( "\nQFORMAT: waring! Revision in GDFS file is newer than current SW but still supported");

    /* There might be things in file before data is stored. */
    if ( QHeader.DataPosition != sizeof( QHEADER) )
    {
      /* Must bypass data in file. */
      Position = lseek( Handle, QHeader.DataPosition, SEEK_SET);

      if ( (uint32)Position != QHeader.DataPosition )
      {
        /* Failed to move file read position. */
        printf( "\nQFORMAT: Failed to move file read position");
        close( Handle);
        return( FALSE);
      }
    }
  }

  /* The position is now at data - read it */
  if ( QHeader.NoofParameters != 0 )
  {
    Data_p = (uint8 *)malloc( QHeader.DataSize);
    Size = read( Handle, Data_p, QHeader.DataSize);
    if ( (uint32)Size != QHeader.DataSize )
    {
      /* Could not read data as expected */
      free( Data_p);
      close( Handle);
      return( FALSE);
    }

    close( Handle);

    /* Check data checksum. */
    CRC = 0;
    CurrPos_p = (uint32 *)Data_p;
    for ( k = 0 ; k < QHeader.DataSize / sizeof( uint32) ; k++ )
    {
      CRC ^= *CurrPos_p;
      CurrPos_p++;
    }

    if ( QHeader.CheckSum != CRC )
    {
      printf( "\nQFORMAT: CRC error in GDFS file");

      if ( fForce )
      {
        printf( "Error discarded. Checksum in file 0x%08x, calculated checksum 0x%08x\n", QHeader.CheckSum, CRC);
      }
      else
      {
        free( Data_p);
        return( FALSE);
      }
    }
  }
  else
  {
    /* No data in file. */
    close( Handle);
    Data_p = NULL;
  }

  /* Fill output parameters */
  *NoofParameters_p = QHeader.NoofParameters;
  *DataArea_pp = Data_p;
  *DataSize_p = QHeader.DataSize;

  return( TRUE);
}

/********************************************************
*       MakeMapFile                                     *
*       Generates binary Q-file.                        *
*       Returns TRUE if successful.                     *
********************************************************/

static bool MakeMapFile( char *psFile, uint32 NoofParameters, uint8 *DataArea_p)
{
  FILE *pFile;
  uint32 *CurrPos_p;
  uint32 Param, Id, Size, AlignedSize, k;
  uint8 *DataPos_p;

  pFile = fopen( psFile, "w");
  if ( pFile == NULL )
  {
    printf( "Could not open file %s. Error code = %d\n", psFile, errno);
    return( FALSE);
  }

  /* Make a header in the file. */
  fprintf( pFile, "//****************************************************************************\n");
  fprintf( pFile, "//\n");
  fprintf( pFile, "//   GDFS Map file created by q2map\n");
  fprintf( pFile, "//\n");
  fprintf( pFile, "//****************************************************************************\n");

  CurrPos_p = (uint32 *)DataArea_p;

  for ( Param = 0 ; Param < NoofParameters ; Param++ )
  {
    Id = CurrPos_p[0];
    Size = CurrPos_p[1];
    CurrPos_p += 2;

    if ( Size > 0 )
    {
      DataPos_p = (uint8 *)(CurrPos_p);

      /* Calculate location for next parameter */
      if ( Size & ALIGN_MASK )
      {
        AlignedSize = (Size + ALIGN_SIZE) & ~ALIGN_MASK;
      }
      else
      {
        AlignedSize = Size;
      }

      AlignedSize /= ALIGN_SIZE;

      /* Make the parameter header */
      fprintf( pFile, "\n%04x %d 0000", Id, Size);

      for ( k = 0 ; k < Size ; k++ )
      {
        if ( (k % PARAMETERS_PER_ROW) == 0 )
        {
          fprintf( pFile, "\n\t");
        }

        fprintf( pFile, " %02x", DataPos_p[k]);
      }

      CurrPos_p += AlignedSize;
    }
    else if ( fDebug )
    {
      printf( "Record for deleted parameter 0x%08x detected\n", Id);
    }
  }

  fclose( pFile);

  return( TRUE);
}



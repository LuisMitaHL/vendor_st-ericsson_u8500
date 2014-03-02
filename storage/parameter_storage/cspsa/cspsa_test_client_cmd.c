/* **************************************************************************
 *
 * cspsa_test_client_cmd.c
 *
 * Copyright (C) 2009 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Martin Lundholm <martin.xa.lundholm@stericsson.com>
 *
 * DESCRIPTION:
 *
 * Simple command interface to CSPSA.
 *
 **************************************************************************** */

// =============================================================================
//  Configurations
// =============================================================================

// =============================================================================
//  Include Header Files
// =============================================================================

#include "cspsa.h"
#include "cspsa_core.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>


// =============================================================================
//  Defines
// =============================================================================

// - D_CSPSA_TEST_CLIENT_READER_TEST_DATA_SIZE: Size of test data buffer. Must not be smaller than 4kB.
#define D_CSPSA_TEST_CLIENT_CMD_STRING_SIZE                 	  (1*1024)

#define D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER                     "[CSPSA]: "

// - D_CSPSA_TEST_CLIENT_CMD_MAX_ARGS Max number of arguments in a command
#define D_CSPSA_TEST_CLIENT_CMD_MAX_ARGS                 	  (100)

// =============================================================================
//  Macros
// =============================================================================

// - M_CSPSA_TEST_CLIENT_READER_FREE: Wrapper to free a pointer.
#define M_CSPSA_TEST_CLIENT_READER_FREE(Pointer) \
  do                          \
  {                           \
    free(Pointer);            \
    Pointer = NULL;           \
  } while (0)

// =============================================================================
//  Local Types
// =============================================================================

/** Executes a command. Returns false if incorrect usage */
typedef bool CSPSA_TEST_CLIENT_CMD_CommandFunction_t(int Argc, const char* Argv_p[]);

/**
 * Represents an available command.
 */
typedef struct
{
  const char* Name_p;
  const char* Usage_p;
  const char* Help_p;
  CSPSA_TEST_CLIENT_CMD_CommandFunction_t* Exec_p;
} CSPSA_TEST_CLIENT_CMD_Command_t;


// =============================================================================
//  Local Constants
// =============================================================================

// =============================================================================
//  Local Function Declarations
// =============================================================================

static char* F_CSPSA_TEST_CLIENT_CMD_PrintResult(const CSPSA_Result_t Result);

static bool F_CSPSA_TEST_CLIENT_CMD_HelpCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_OpenCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_CreateCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_CloseCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_FlushCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_ReadToFileCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_WriteFromFileCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_WriteCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_RemoveCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_CopyCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_WriteToGDF_Cmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_ImportFromGDF_Cmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_ListCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_DumpCmd(int Argc, const char* Argv_p[]);
static bool F_CSPSA_TEST_CLIENT_CMD_QuitCmd(int Argc, const char* Argv_p[]);

// =============================================================================
//  Local Variables
// =============================================================================

// - V_CSPSA_TEST_Handle Handle to the currently open CSPSA
static CSPSA_Handle_t V_CSPSA_TEST_Handle = (CSPSA_Handle_t) (0);

// - V_CSPSA_TEST_SupportedCommands All available commands
static CSPSA_TEST_CLIENT_CMD_Command_t V_CSPSA_TEST_SupportedCommands[] =
{
  {
    "help",
    NULL,
    NULL,
    F_CSPSA_TEST_CLIENT_CMD_HelpCmd
  },
  {
    "open",
    "open <cspsa>",
    NULL,
    F_CSPSA_TEST_CLIENT_CMD_OpenCmd
  },
  {
    "create",
    "create <cspsa>",
    "Creates an empty CSPSA",
    F_CSPSA_TEST_CLIENT_CMD_CreateCmd
  },
  {
    "flush",
    NULL,
    NULL,
    F_CSPSA_TEST_CLIENT_CMD_FlushCmd
  },
  {
    "close",
    NULL,
    NULL,
    F_CSPSA_TEST_CLIENT_CMD_CloseCmd
  },
  {
    "read_to_file",
    "read_to_file <key> <filename>",
    "Saves the value of a parameter to file",
    F_CSPSA_TEST_CLIENT_CMD_ReadToFileCmd
  },
  {
    "write_from_file",
    "write_from_file <key> <filename>",
    "Changes a parameter; contents are read from file",
    F_CSPSA_TEST_CLIENT_CMD_WriteFromFileCmd
  },
  {
    "write",
    "write <key> <hex contents>",
    "Creates/writes a parameter with hex contents, e.g. write 5 bd 00 cc dd",
    F_CSPSA_TEST_CLIENT_CMD_WriteCmd
  },
  {
    "rm",
    "rm <key1> <key2> ... // or rm * to delete everything",
    NULL,
    F_CSPSA_TEST_CLIENT_CMD_RemoveCmd
  },
  {
    "copy",
    "copy <dest cspsa>",
    "Copies all parameters from the current CSPSA to the dest CSPSA",
    F_CSPSA_TEST_CLIENT_CMD_CopyCmd
  },
  {
    "export",
    "export <.gdf file name>",
    NULL,
    F_CSPSA_TEST_CLIENT_CMD_WriteToGDF_Cmd
  },
  {
    "import",
    "import <.gdf file name>",
    NULL,
    F_CSPSA_TEST_CLIENT_CMD_ImportFromGDF_Cmd
  },
  {
    "ls",
    NULL,
    "Lists all keys of the currently open CSPSA.",
    F_CSPSA_TEST_CLIENT_CMD_ListCmd
  },
  {
    "dump",
    "dump [key]",
    "Dumps parameters in the currently open CSPSA. [key] is optional",
    F_CSPSA_TEST_CLIENT_CMD_DumpCmd
  },
  {
    "quit",
    NULL,
    NULL,
    F_CSPSA_TEST_CLIENT_CMD_QuitCmd
  },
  // last entry
  { NULL, NULL, NULL, NULL }
};

// =============================================================================
//  Global External Variables
// =============================================================================

// =============================================================================
//  External Declarations
// =============================================================================

// =============================================================================
//  Local Function Definitions
// =============================================================================

// - F_CSPSA_TEST_CLIENT_CMD_IsOpen: returns true if we have opened a CSPSA
static bool F_CSPSA_TEST_CLIENT_CMD_IsOpen(void)
{
  return V_CSPSA_TEST_Handle != (CSPSA_Handle_t) (0);
}

// - F_CSPSA_TEST_CLIENT_CMD_CheckOpen: Checks if we have opened a CSPSA, issues error if not
static bool F_CSPSA_TEST_CLIENT_CMD_CheckOpen(void)
{
  bool Result = F_CSPSA_TEST_CLIENT_CMD_IsOpen();
  if (!Result)
  {
    printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "No CSPSA is open; use 'open' command to open a CSPSA\n");
  }
  return Result;
}

static char* F_CSPSA_TEST_CLIENT_CMD_PrintResult(const CSPSA_Result_t Result)
{
  char* Result_p = "Unknown";

  #define M_MACRO_TO_STRING(__n) case __n: Result_p = #__n;break;
  switch (Result)
  {
    M_MACRO_TO_STRING(T_CSPSA_RESULT_OK)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_WARNINGS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_SIZE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_NEW_KEY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_W_INVALID_HEADER_FOUND)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_ERRORS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_BAD_PARAMETER)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INVALID_KEY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INVALID_SIZE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OUT_OF_SPACE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_NO_VALID_IMAGE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_MEDIA_INACCESSIBLE)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_READ_ONLY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_READ_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_WRITE_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_END_OF_DATA)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OPEN_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_ALREADY_EXISTS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_OUT_OF_MEMORY)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_INTERNAL_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_E_SOCKET_ERROR)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_NUMBER_OF_ITEMS)
    M_MACRO_TO_STRING(T_CSPSA_RESULT_UNDEFINED)
  }
  #undef M_MACRO_TO_STRING
  return Result_p;
}

/** hex dumps a memory buffer */
static void F_CSPSA_TEST_CLIENT_CMD_HexDump_V2(FILE* fp, char* Data_p, size_t Size, bool GDF_Format)
{
  size_t PerLine = 16;
  size_t i, j;

  for (i = 0; i < Size; i+=PerLine)
  {
    if (GDF_Format)
    {
      fprintf(fp, "\t");
    }
    else
    {
      fprintf(fp, "0x%04x ", (unsigned int) i);
    }
    for (j = 0; j < PerLine; ++j)
    {
      if (i + j >= Size)
      {
        if (!GDF_Format)
        {
          fprintf(fp, "   ");
        }
      }
      else
      {
        fprintf(fp, " %02x", Data_p[i+j] & 0xff);
      }
    }
    if (!GDF_Format)
    {
      fprintf(fp, " ");
      for (j = 0; j < PerLine; ++j)
      {
        if (i + j >= Size)
        {
          fprintf(fp, " ");
        }
        else
        {
          char c = Data_p[i+j];
          if (c < ' ' || c > 'z')
          {
            fprintf(fp, ".");
          }
          else
          {
            fprintf(fp, "%c", c);
          }
        }
      }
    }
    fprintf(fp, "\n");
  }
}

/** hex dumps a memory buffer */
static void F_CSPSA_TEST_CLIENT_CMD_HexDump(char* Data_p, size_t Size)
{
  F_CSPSA_TEST_CLIENT_CMD_HexDump_V2(stdout, Data_p, Size, false);
}

/**
 * Shows keys/sizes of all parameters.
 */
void F_CSPSA_TEST_CLIENT_CMD_List(CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint32_t NrOfKeys = 0;
  uint32_t TotalSize = 0;

  printf("%10s %10s\n", "Key", "Size");
  Result = CSPSA_GetSizeOfFirstValue(Handle, &Key, &Size);
  while (Result == T_CSPSA_RESULT_OK)
  {
    printf("%10d %10d\n", Key, Size);
    ++NrOfKeys;
    TotalSize += Size;
    Result = CSPSA_GetSizeOfNextValue(Handle, &Key, &Size);
  }
  printf("\n");
  printf("Number of keys in CSPSA : %u\n", NrOfKeys);
  printf("Total size of all values: %u\n", TotalSize);
  printf("\n");
}

void F_CSPSA_TEST_CLIENT_CMD_Dump(CSPSA_Handle_t Handle, CSPSA_Key_t Key)
{
  CSPSA_Result_t Result;
  CSPSA_Size_t Size;

  Result = CSPSA_GetSizeOfValue(Handle, Key, &Size);
  if (Result != T_CSPSA_RESULT_OK)
  {
    printf("<CSPSA_GetSizeOfValue(key %08x) fails, result: %s>\n",
             Key, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
  }
  else
  {
    char* Data_p = malloc(Size);

    if (Data_p == NULL)
    {
      printf("<out of memory>\n");
      return;
    }
    Result = CSPSA_ReadValue(Handle, Key, Size, (uint8_t*)Data_p);
    if (Result != T_CSPSA_RESULT_OK)
    {
      printf("<CSPSA_ReadValue(key %08x, size %u) fails, result: %s>\n",
             Key, Size, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
      return;
    }
    printf("Dump of key %08x, size %u:\n", Key, Size);
    F_CSPSA_TEST_CLIENT_CMD_HexDump(Data_p, Size);
    free(Data_p);
  }
}

static void F_CSPSA_TEST_CLIENT_CMD_DumpAsGDF(FILE* fp, CSPSA_Handle_t Handle, CSPSA_Key_t Key)
{
  CSPSA_Result_t Result;
  CSPSA_Size_t Size;

  Result = CSPSA_GetSizeOfValue(Handle, Key, &Size);
  if (Result != T_CSPSA_RESULT_OK)
  {
    printf("<CSPSA_GetSizeOfValue(key %08x) fails, result: %s>\n",
             Key, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
  }
  else
  {
    char* Data_p = malloc(Size);

    Result = CSPSA_ReadValue(Handle, Key, Size, (uint8_t*)Data_p);
    if (Result != T_CSPSA_RESULT_OK)
    {
      printf("<CSPSA_ReadValue(key %08x, size %u) fails, result: %s>\n",
             Key, Size, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
      return;
    }
    if (Key < UINT16_MAX)
    {
      fprintf(fp, "%04x %u 0000\n", Key, Size);
    }
    else
    {
      fprintf(fp, "%08x %u 0000\n", Key, Size);
    }
    F_CSPSA_TEST_CLIENT_CMD_HexDump_V2(fp, Data_p, Size, true);
    free(Data_p);
  }
}

/**
 * Logs contents of all trim area parameters
 */
static void F_CSPSA_TEST_CLIENT_CMD_DumpAll(CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint32_t NrOfKeys = 0;
  uint32_t TotalSize = 0;

  Result = CSPSA_GetSizeOfFirstValue(Handle, &Key, &Size);
  while (Result == T_CSPSA_RESULT_OK)
  {
    F_CSPSA_TEST_CLIENT_CMD_Dump(Handle, Key);
    ++NrOfKeys;
    TotalSize += Size;
    Result = CSPSA_GetSizeOfNextValue(Handle, &Key, &Size);
  }
  printf("\n");
  printf("Number of keys in CSPSA : %u\n", NrOfKeys);
  printf("Total size of all values: %u\n", TotalSize);
  printf("\n");
}

/**
 * Copies contents of all trim area parameters to the CSPSA with the given name.
 */
void F_CSPSA_TEST_CLIENT_CMD_Copy(const char* DestCSPSA_p, CSPSA_Handle_t SourceHandle)
{
  CSPSA_Result_t Result;
  CSPSA_Handle_t DestHandle;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;

  Result = CSPSA_Create(DestCSPSA_p, &DestHandle);
  if (Result != T_CSPSA_RESULT_OK)
  {
    printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "Could not create CSPSA %s, error '%s'>\n",
           DestCSPSA_p, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
    return;
  }
  Result = CSPSA_GetSizeOfFirstValue(SourceHandle, &Key, &Size);
  while (Result < T_CSPSA_RESULT_ERRORS)
  {
    uint8_t* Data_p = malloc(Size);

    if (Data_p == NULL)
    {
      printf("out of memory\n");
      return;
    }
    Result = CSPSA_ReadValue(SourceHandle, Key, Size, Data_p);
    if (Result >= T_CSPSA_RESULT_ERRORS)
    {
      printf("    copy failed; CSPSA_ReadValue(key %08x, size %u) fails, result: %s\n",
             Key, Size, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
      return;
    }
    Result = CSPSA_WriteValue(DestHandle, Key, Size, Data_p);
    if (Result >= T_CSPSA_RESULT_ERRORS)
    {
      printf("    copy failed; CSPSA_WriteValue(key %08x, size %u) fails, result: %s\n",
             Key, Size, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
      return;
    }
    Result = CSPSA_GetSizeOfNextValue(SourceHandle, &Key, &Size);
    free(Data_p);
  }
  Result = CSPSA_Close(&DestHandle);
  if (Result == T_CSPSA_RESULT_OK)
  {
    printf("<Successfully copied contents of currently open CSPSA to %s>\n", DestCSPSA_p);
  }
  else
  {
      printf("    copy failed; CSPSA_Close fails, result: %s\n", F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
  }
}

/**
 * Removes all trim area parameters from the given CSPSA.
 */
void F_CSPSA_TEST_CLIENT_CMD_RemoveAll(CSPSA_Handle_t Handle)
{
  CSPSA_Result_t Result;
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;

  do {
    Result = CSPSA_GetSizeOfFirstValue(Handle, &Key, &Size);
    if (Key >= D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY)
    {
      break;
    }
    if (Result == T_CSPSA_RESULT_OK)
    {
      Result = CSPSA_DeleteValue(Handle, Key);
      if (Result != T_CSPSA_RESULT_OK)
      {
        printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Could not delete key %08x, error %s>\n",
               Key, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
      }
    }
    for (Key = D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY; Key < D_CSPSA_CORE_FIRST_FAST_PARAMETER_KEY + D_CSPSA_CORE_NBR_OF_FAST_PARAMETERS - 1; Key++)
    {
      uint32_t empty = 0;
      CSPSA_WriteValue(Handle, Key, sizeof(empty), (CSPSA_Data_t*)&empty);
    }
  } while (Result == T_CSPSA_RESULT_OK);
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Removed all entries>\n");
}

// =============================================================================
//  Command Functions
// =============================================================================

// - F_CSPSA_TEST_CLIENT_CMD_OpenCmd:
//            Opens a CSPSA. If there was already an open CSPSA, it is closed first.
bool F_CSPSA_TEST_CLIENT_CMD_OpenCmd(int Argc, const char* Argv_p[])
{
  CSPSA_Result_t CSPSA_Result;

  if (Argc != 2)
  {
    return false;
  }
  // if CSPSA is already open: close it
  if (F_CSPSA_TEST_CLIENT_CMD_IsOpen())
  {
    F_CSPSA_TEST_CLIENT_CMD_CloseCmd(0, NULL);
  }
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_Open('%s').>\n", Argv_p[1]);
  CSPSA_Result = CSPSA_Open(Argv_p[1], &V_CSPSA_TEST_Handle);
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Result '%s'>\n", F_CSPSA_TEST_CLIENT_CMD_PrintResult(CSPSA_Result));
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_CreateCmd:
//           Creates an empty CSPSA.
bool F_CSPSA_TEST_CLIENT_CMD_CreateCmd(int Argc, const char* Argv_p[])
{
  CSPSA_Result_t CSPSA_Result;

  if (Argc != 2)
  {
    return false;
  }
  // if CSPSA is already open: close it
  if (F_CSPSA_TEST_CLIENT_CMD_IsOpen())
  {
    F_CSPSA_TEST_CLIENT_CMD_CloseCmd(0, NULL);
  }
  // first try to open CSPSA; if that succeeds, just remove all elements from it
  CSPSA_Result = CSPSA_Open(Argv_p[1], &V_CSPSA_TEST_Handle);
  if (CSPSA_Result == T_CSPSA_RESULT_OK)
  {
    F_CSPSA_TEST_CLIENT_CMD_RemoveAll(V_CSPSA_TEST_Handle);
  }
  else
  {
    CSPSA_Close(&V_CSPSA_TEST_Handle);
    CSPSA_Result = CSPSA_Create(Argv_p[1], &V_CSPSA_TEST_Handle);
  }
  // close to create the CSPSA on the media
  F_CSPSA_TEST_CLIENT_CMD_CloseCmd(0, NULL);
  // and open the CSPSA again
  F_CSPSA_TEST_CLIENT_CMD_OpenCmd(Argc, Argv_p);
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_FlushCmd:
//            Flushes the currently open CSPSA.
bool F_CSPSA_TEST_CLIENT_CMD_FlushCmd(int Argc, const char* Argv_p[])
{
  (void)Argc;
  (void)Argv_p;
  if (F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    CSPSA_Result_t Result = CSPSA_Flush(V_CSPSA_TEST_Handle);
    printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_Flush(): %s>\n", F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
  }
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_CloseCmd:
//           Closes the currently open CSPSA.
bool F_CSPSA_TEST_CLIENT_CMD_CloseCmd(int Argc, const char* Argv_p[])
{
  CSPSA_Result_t CSPSA_Result;

  (void)Argc;
  (void)Argv_p;
  if (F_CSPSA_TEST_CLIENT_CMD_IsOpen())
  {
    CSPSA_Result = CSPSA_Close(&V_CSPSA_TEST_Handle);
    if (CSPSA_Result != T_CSPSA_RESULT_OK)
    {
      printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Error closing CSPSA, result '%s'>\n",
             F_CSPSA_TEST_CLIENT_CMD_PrintResult(CSPSA_Result));
    }
    else
    {
      printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Successfully closed the currently open CSPSA>\n");
    }
  }
  else
  {
    printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<No CSPSA is currently open>\n");
  }
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_ReadToFileCmd:
//             Exports a CSPSA parameter to a file
bool F_CSPSA_TEST_CLIENT_CMD_ReadToFileCmd(int Argc, const char* Argv_p[])
{
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  const char* FileName_p;
  uint8_t* DataBuffer_p = NULL;
  CSPSA_Result_t CSPSA_Result;
  int r;

  if (Argc != 3)
  {
    return false;
  }
  if (!F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    goto ErrorExit;
  }
  r = sscanf(Argv_p[1], "%x", &Key);
  if (r < 1)
  {
    printf("<Illegal key %s>\n", Argv_p[1]);
    goto ErrorExit;
  }
  FileName_p = Argv_p[2];
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Read (%08x) to file '%s'>\n", Key, FileName_p);

  CSPSA_Result = CSPSA_GetSizeOfValue(V_CSPSA_TEST_Handle, Key, &Size);
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "CSPSA_GetSizeOfValue(%08x): %s\n", Key, F_CSPSA_TEST_CLIENT_CMD_PrintResult(CSPSA_Result));
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  DataBuffer_p = (uint8_t*) malloc(Size);
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_Read(%08x).>\n", Key);
  CSPSA_Result = CSPSA_ReadValue(V_CSPSA_TEST_Handle, Key, Size, DataBuffer_p);
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_ReadValue(%08x, %u): %s>\n", Key, Size, F_CSPSA_TEST_CLIENT_CMD_PrintResult(CSPSA_Result));
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  {
    int FileHandle = open(FileName_p, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    int IntResult;

    if (FileHandle == -1)
    {
      printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Could not open file '%s' for writing.>\n", FileName_p);
      goto ErrorExit;
    }
    IntResult = lseek(FileHandle, 0, SEEK_SET);
    if (IntResult == -1)
    {
      close(FileHandle);
      goto ErrorExit;
    }
    IntResult = write(FileHandle, DataBuffer_p, Size);
    close(FileHandle);
    if (IntResult == -1) goto ErrorExit;
    printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<%i bytes written to file '%s'.>\n", IntResult, FileName_p);
  }
ErrorExit:
  M_CSPSA_TEST_CLIENT_READER_FREE(DataBuffer_p);
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_WriteFromFileCmd:
//          Imports a CSPSA parameter from a file
bool F_CSPSA_TEST_CLIENT_CMD_WriteFromFileCmd(int Argc, const char* Argv_p[])
{
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  const char* FileName_p;
  uint8_t* DataBuffer_p = NULL;
  CSPSA_Result_t CSPSA_Result;
  int r;

  if (Argc != 3)
  {
    return false;
  }
  if (!F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    goto ErrorExit;
  }
  r = sscanf(Argv_p[1], "%x", &Key);
  if (r < 1)
  {
    printf("<Illegal key %s>\n", Argv_p[1]);
    goto ErrorExit;
  }
  FileName_p = Argv_p[2];

  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Write (%08x) from file '%s'>\n", Key, FileName_p);
  {
    struct stat StatData;
    int IntResult;

    IntResult = stat(FileName_p, &StatData);
    if (IntResult == -1)
    {
      printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Could not find file '%s'.>\n", FileName_p);
      goto ErrorExit;
    }
    Size = StatData.st_size;
  }

  DataBuffer_p = (uint8_t*) malloc(Size);
  {
    int IntResult;
    int FileHandle = open(FileName_p, O_RDONLY, 0);

    if (FileHandle == -1)
    {
      printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<Could not open file '%s' for reading.>\n", FileName_p);
      goto ErrorExit;
    }
    IntResult = lseek(FileHandle, 0, SEEK_SET);
    if (IntResult == -1)
    {
      close(FileHandle);
      goto ErrorExit;
    }
    IntResult = read(FileHandle, DataBuffer_p, Size);
    close(FileHandle);
    if (IntResult == -1) goto ErrorExit;
    printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<%i bytes read from file '%s'.>\n", IntResult, FileName_p);
  }

  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_WriteValue(%08x, %u).>\n", Key, Size);
  CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, DataBuffer_p);
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_WriteValue(%08x, %u): %s>\n", Key, Size, F_CSPSA_TEST_CLIENT_CMD_PrintResult(CSPSA_Result));

ErrorExit:
  M_CSPSA_TEST_CLIENT_READER_FREE(DataBuffer_p);
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_WriteCmd:
//          Writes a CSPSA parameter. First arg is key, rest of the arguments are
//          hex-formated bytes.
bool F_CSPSA_TEST_CLIENT_CMD_WriteCmd(int Argc, const char* Argv_p[])
{
  CSPSA_Key_t Key;
  CSPSA_Size_t Size;
  uint8_t* DataBuffer_p = NULL;
  CSPSA_Result_t CSPSA_Result;
  uint32_t i;
  int r;

  if (Argc < 3)
  {
    return false;
  }
  if (!F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    goto ErrorExit;
  }
  r = sscanf(Argv_p[1], "%x", &Key);
  if (r < 1)
  {
    printf("<Illegal key %s>\n", Argv_p[1]);
    goto ErrorExit;
  }
  Size = Argc - 2;
  DataBuffer_p = (uint8_t*)malloc(Size);
  for (i = 0; i < Size; ++i)
  {
    uint32_t Value;

    if (!isxdigit(Argv_p[i+2][0]))
    {
      printf("<Illegal value %s>\n", Argv_p[i+2]);
      goto ErrorExit;
    }
    sscanf(Argv_p[i+2], "%x", &Value);
    if (Value > 0xff)
    {
      printf("<Illegal value %s>\n", Argv_p[i+2]);
      goto ErrorExit;
    }
    DataBuffer_p[i] = (uint8_t)Value;
  }
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_WriteValue(%08x, %u)>\n", Key, Size);
  CSPSA_Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, DataBuffer_p);
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_WriteValue(%08x, %u): %s>\n", Key, Size, F_CSPSA_TEST_CLIENT_CMD_PrintResult(CSPSA_Result));

ErrorExit:
  M_CSPSA_TEST_CLIENT_READER_FREE(DataBuffer_p);
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_RemoveCmd:
//         Removes one or more keys (every argument contains a key)
bool F_CSPSA_TEST_CLIENT_CMD_RemoveCmd(int Argc, const char* Argv_p[])
{
  bool result = false;
  CSPSA_Key_t Key;
  CSPSA_Result_t CSPSA_Result;

  if (Argc < 2)
  {
    return false;
  }
  if (F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    int i;

    for (i = 1; i < Argc; ++i)
    {
      if (Argv_p[i][0] == '*')
      {
        F_CSPSA_TEST_CLIENT_CMD_RemoveAll(V_CSPSA_TEST_Handle);
      }
      else
      {
        int r = sscanf(Argv_p[i], "%x", &Key);
        if (r < 1)
        {
          printf("<Illegal key %s>\n", Argv_p[1]);
          goto ErrorExit;
        }
        CSPSA_Result = CSPSA_DeleteValue(V_CSPSA_TEST_Handle, Key);
        printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_DeleteValue(%08x): %s>\n", Key, F_CSPSA_TEST_CLIENT_CMD_PrintResult(CSPSA_Result));
      }
    }
  }
  result = true;

ErrorExit:
  return result;
}

// - F_CSPSA_TEST_CLIENT_CMD_CopyCmd:
//       Copies the contents of the currently open CSPSA to another CSPSA.
bool F_CSPSA_TEST_CLIENT_CMD_CopyCmd(int Argc, const char* Argv_p[])
{
  if (Argc != 2)
  {
    return false;
  }
  if (F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    F_CSPSA_TEST_CLIENT_CMD_Copy(Argv_p[1], V_CSPSA_TEST_Handle);
  }
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_WriteToGDF_Cmd:
//             Exports the contents of the currently open CSPSA to a .gdf file
static bool F_CSPSA_TEST_CLIENT_CMD_WriteToGDF_Cmd(int Argc, const char* Argv_p[])
{
  if (Argc != 2)
  {
    return false;
  }
  if (F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    CSPSA_Result_t Result;
    CSPSA_Key_t Key;
    CSPSA_Size_t Size;
    FILE* fp = fopen(Argv_p[1], "w");

    if (fp == NULL)
    {
      printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "Cannot open %s\n", Argv_p[1]);
      goto ErrorExit;
    }
    fprintf(fp, "// Data format:\n");
    fprintf(fp, "// <UNIT>(hex) <DATASIZE>(dec) <BLOCK>(hex) [<DATA>(hex)]\n");

    Result = CSPSA_GetSizeOfFirstValue(V_CSPSA_TEST_Handle, &Key, &Size);
    while (Result == T_CSPSA_RESULT_OK)
    {
      F_CSPSA_TEST_CLIENT_CMD_DumpAsGDF(fp, V_CSPSA_TEST_Handle, Key);
      Result = CSPSA_GetSizeOfNextValue(V_CSPSA_TEST_Handle, &Key, &Size);
    }
    fclose(fp);
  }
ErrorExit:
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_ImportFromGDF_Cmd: reads a .gdf file (as used in GDFS)
//      and imports the values in the file into the currently open CSPSA.
static bool F_CSPSA_TEST_CLIENT_CMD_ImportFromGDF_Cmd(int Argc, const char* Argv_p[])
{
  FILE* fp = NULL;
  char Line[100];
  CSPSA_Key_t Key = 0;
  CSPSA_Size_t Size;
  uint8_t* Data_p = NULL;
  uint32_t CurrIndex = 0;
  CSPSA_Result_t Result;
  uint32_t ItemsImported = 0;

  if (Argc != 2)
  {
    return false;
  }
  if (!F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    goto ErrorExit;
  }
  fp = fopen(Argv_p[1], "r");
  if (fp == NULL)
  {
    printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "Cannot open %s\n", Argv_p[1]);
    goto ErrorExit;
  }

  while (fgets(Line, sizeof (Line), fp) != NULL)
  {
    if (strlen(Line) == 0 || Line[0] == '/')
    {
      continue;
    }
    if (isxdigit(Line[0]))
    {
      free(Data_p);
      sscanf(Line, "%x %u", &Key, &Size);
      Data_p = malloc(Size);
      CurrIndex = 0;
    }
    else if (Line[0] == '\t')
    {
      char* Value_p;

      for (Value_p = strtok(Line, " \t\n\r"); Value_p != NULL; Value_p = strtok(NULL, " \t\n\r"))
      {
        uint32_t Value;

        sscanf(Value_p, "%x", &Value);
        Data_p[CurrIndex] = Value;
        ++CurrIndex;
      }
      if (CurrIndex >= Size)
      {
        Result = CSPSA_WriteValue(V_CSPSA_TEST_Handle, Key, Size, Data_p);
        if (Result >= T_CSPSA_RESULT_ERRORS)
        {
          printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "<CSPSA_WriteValue(%08x, %u) failed, result: %s>\n",
                 Key, Size, F_CSPSA_TEST_CLIENT_CMD_PrintResult(Result));
          goto ErrorExit;
        }
        ++ItemsImported;
      }
    }
    else
    /* By calling strtok here we allow trailing blanks in the file */
    if (strtok(Line, " \t\n\r"))
    {
      fprintf(stderr, D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "ERROR: Input file is malformatted for key(%08x).\n", Key);
      fprintf(stderr, D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "ERROR: Failed parsing line starting with '%s'.\n", Line);
      goto ErrorExit;
    }
  }
ErrorExit:
  if (fp != NULL)
  {
    fclose(fp);
  }
  free(Data_p);
  printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "Imported %u items\n", ItemsImported);
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_ListCmd: lists all keys + their sizes
bool F_CSPSA_TEST_CLIENT_CMD_ListCmd(int Argc, const char* Argv_p[])
{
  (void)Argc;
  (void)Argv_p;

  if (F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    F_CSPSA_TEST_CLIENT_CMD_List(V_CSPSA_TEST_Handle);
  }
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_DumpCmd: Dumps CSPSA contents,
//                if no arguments, the entire CSPSA is dumped,
//                otherwise every argument is a key that will be dumped
bool F_CSPSA_TEST_CLIENT_CMD_DumpCmd(int Argc, const char* Argv_p[])
{
  bool result = false;

  if (F_CSPSA_TEST_CLIENT_CMD_CheckOpen())
  {
    if (Argc <= 1)
    {
      F_CSPSA_TEST_CLIENT_CMD_DumpAll(V_CSPSA_TEST_Handle);
    }
    else
    {
      CSPSA_Key_t Key;
      int i;

      for (i = 1; i < Argc; ++i)
      {
        int r = sscanf(Argv_p[i], "%x", &Key);
        if (r < 1)
        {
          printf("<Illegal key %s>\n", Argv_p[1]);
          goto ErrorExit;
        }

        F_CSPSA_TEST_CLIENT_CMD_Dump(V_CSPSA_TEST_Handle, Key);
      }
    }
  }

  result = true;

ErrorExit:
  return result;
}

// - F_CSPSA_TEST_CLIENT_CMD_QuitCmd: exits
bool F_CSPSA_TEST_CLIENT_CMD_QuitCmd(int Argc, const char* Argv_p[])
{
  (void)Argc;
  (void)Argv_p;

  exit(0);
  return true;
}

// - F_CSPSA_TEST_CLIENT_CMD_HelpCmd: shows available commands
bool F_CSPSA_TEST_CLIENT_CMD_HelpCmd(int Argc, const char* Argv_p[])
{
  int i;

  (void)Argc;
  (void)Argv_p;
  printf("Available commands (case sensitive):\n");
  for (i = 0; V_CSPSA_TEST_SupportedCommands[i].Name_p != NULL; ++i)
  {
    const char* Cmd_p = V_CSPSA_TEST_SupportedCommands[i].Usage_p;
    const char* Extra_p = V_CSPSA_TEST_SupportedCommands[i].Help_p;

    if (Cmd_p == NULL)
    {
      Cmd_p = V_CSPSA_TEST_SupportedCommands[i].Name_p;
    }
    printf("  %s\n", Cmd_p);
    if (Extra_p != NULL)
    {
      printf("     - %s\n", Extra_p);
    }
  }
  printf("\n");
  return true;
}


// - F_CSPSA_TEST_CLIENT_CMD_Exec: tokenizes the command string and executes the corresponding command
void F_CSPSA_TEST_CLIENT_CMD_Exec(char* CommandString_p)
{
  int Argc = 0;
  const char* Argv_p[D_CSPSA_TEST_CLIENT_CMD_MAX_ARGS];
  char* CommandName_p = strtok(CommandString_p, " \t");
  if (CommandName_p != NULL)
  {
    int i;
    const CSPSA_TEST_CLIENT_CMD_Command_t* Cmd_p = NULL;

    // find command
    for (i = 0; Cmd_p == NULL && V_CSPSA_TEST_SupportedCommands[i].Name_p != NULL; ++i)
    {
      if (strcmp(V_CSPSA_TEST_SupportedCommands[i].Name_p, CommandName_p) == 0)
      {
        Cmd_p = &V_CSPSA_TEST_SupportedCommands[i];
      }
    }
    if (Cmd_p == NULL)
    {
      printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER
             "Unknown command '%s'. Type 'help' to see valid commands.\n\n", CommandName_p);
    }
    else
    {
      // tokenize command string
      Argv_p[0] = CommandName_p;
      Argc = 1;
      while (Argc < D_CSPSA_TEST_CLIENT_CMD_MAX_ARGS
            && (Argv_p[Argc] = strtok(NULL, " \t")) != NULL)
      {
        ++Argc;
      }
      // execute the command
      if (!Cmd_p->Exec_p(Argc, Argv_p) && Cmd_p->Usage_p != NULL)
      {
        // incorrect usage of the command; show usage
        printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER "Usage: %s\n\n", Cmd_p->Usage_p);
        if (Cmd_p->Help_p != NULL)
        {
          printf("     - %s\n", Cmd_p->Help_p);
        }
      }
    }
  }
}

// =============================================================================
//   Global Function Definitions
// =============================================================================

// =============================================================================
//   Processes()
// =============================================================================

// =============================================================================
//   main()
// =============================================================================

// - main(): Open CSPSA area and read for.

int main(int Argc, char *Argv_p[])
{

  (void) Argc;
  (void) Argv_p;

  while (true)
  {
    char V_CSPSA_TEST_CommandString_p[D_CSPSA_TEST_CLIENT_CMD_STRING_SIZE];
    char* CommandString_p;

    CommandString_p = V_CSPSA_TEST_CommandString_p;
    printf(D_CSPSA_TEST_CLIENT_CMD_PRINT_HEADER);

    *CommandString_p = getchar();
    while (*CommandString_p != '\n')
    {
      CommandString_p++;
      *CommandString_p = getchar();
    }
    *CommandString_p = '\0';
    F_CSPSA_TEST_CLIENT_CMD_Exec(V_CSPSA_TEST_CommandString_p);
  }
  return 0;
}

// =============================================================================
//   End of file
// =============================================================================

/*******************************************************************
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: Martin Lundholm <martin.xa.lundholm@stericsson.com>
 *
 * Creates files from CSPSA parameters.
 *
 * This functionality is used in U8500:
 * ITP in U8500 has no file access, so ITP writes calibration data to trim area (= CSPSA).
 * The modem side expects this data to be present in files in the file system.
 *
 * At first "normal" startup, this program should be run to convert
 * the calibration data in the trim area to corresponding files.
 *
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>
#include <cspsa.h>
#include "nwmcommon.h"

#define CSPSA2NWM_TAG "[cspsa2nwm] "

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

/**
 * Configuration options for CSPSA to file conversion.
 */
typedef struct
{
  CSPSA_Handle_t handle;
  int logLevel;
  bool logToStdout;
  char* cspsaName_p;
  char* rootDir_p;
  char* guardFileName_p;
  bool forceFileCreation;
  char* cspsaUpdateItemInfo_p;
  CSPSA_Key_t ItpUpdateFlagKey;
  uint32_t ItpUpdateFlagBitIndex;
  char** filterPaths_pp;
  uint32_t nbrOfFilterPaths;
} Config_t;

//------------------------------------------------------------------------------
//  LOGGING
//------------------------------------------------------------------------------

static bool logToStdout = false;

/**
 * Opens syslog.
 * @param level Only events >= level will really be logged.
 * @param toStdout Logs also to stdout (in addition to syslog)
 */
void cspsaOpenLog(int level, bool toStdout)
{
  openlog(CSPSA2NWM_TAG, LOG_PID | LOG_NDELAY, LOG_USER);
  setlogmask(LOG_UPTO(level));
  logToStdout = toStdout;
}

/**
 * printf-like function that writes to the log.
 * @param level As used in syslog.
 */
void cspsaLog(int level, const char *fmt, ...)
{
  va_list vl;

  va_start(vl, fmt);
  vsyslog(level | LOG_USER, fmt, vl);
  if (level == LOG_ERR)
  {
    // log errors to stderr
    vfprintf(stderr, fmt, vl);
  }
  else if (logToStdout)
  {
    vfprintf(stdout, fmt, vl);
  }
  va_end(vl);
}

void cspsaCloseLog(void)
{
  closelog();
}

//------------------------------------------------------------------------------
//  FILE CONVERSION
//------------------------------------------------------------------------------

/**
 * Reads 4 bytes from the data, converts it to an uint32.
 */
static uint32_t getUint32(const uint8_t* data_p)
{
  // Data is stored in little endian format
  return data_p[0]
         + (data_p[1] << 8)
         + (data_p[2] << 16)
         + (data_p[3] << 24);
}

/**
 * Reads a parameter from the CSPSA with the given key and which must be of the given size.
 */
static ConvertStatus_t readParameter(CSPSA_Handle_t handle, CSPSA_Key_t key, uint32_t size, uint8_t* data_p)
{
  uint32_t actualSize;
  CSPSA_Result_t result;

  cspsaLog(LOG_DEBUG, "readParameter(key %u, size %u)\n", key, size);
  // Read size of the key
  result = CSPSA_GetSizeOfValue(handle, key, &actualSize);
  if (result != T_CSPSA_RESULT_OK)
  {
    cspsaLog(LOG_ERR, "Error: cannot read CSPSA parameter %u, CSPSA_GetSizeOfValue returns %d\n",
             key, result);
    goto ErrorExit;
  }
  if (size != actualSize)
  {
    cspsaLog(LOG_ERR, "Error reading CSPSA parameter %u, size is %u, expected size is %u\n", key, actualSize, size);
    result = T_CSPSA_RESULT_UNDEFINED;
    goto ErrorExit;
  }
  // Read the value
  result = CSPSA_ReadValue(handle, key, size, data_p);
  if (result != T_CSPSA_RESULT_OK)
  {
    cspsaLog(LOG_ERR, "Error reading CSPSA parameter %u, CSPSA_ReadValue returns %d\n", key, result);
    goto ErrorExit;
  }
ErrorExit:
  return result == T_CSPSA_RESULT_OK ? CONVERT_STATUS_OK : CONVERT_STATUS_CSPSA_CORRUPT;
}

/**
 * Creates a file in the file system. Is used to create both regular files and directories.
 *
 * @param [in] rootDir_p  Root directory in which files will be created, e.g. "/modemfs"
 * @param [in] fileInfo_p Contains info about the file.
 * @param [in] size       The size of the file to be created (only used for regular files)
 * @param [in] fileContents_p The contents of the file (use NULL when creating a directory)
 */
static ConvertStatus_t createFile(const Config_t* const config_p, const char* rootDir_p, uint8_t* fileInfo_p, uint32_t size, uint8_t* fileContents_p)
{
  ConvertStatus_t status = CONVERT_STATUS_OK;
  const char* path_p = (const char*)&fileInfo_p[PATH_INDEX];
  const char* fileName_p = (const char*)&fileInfo_p[NAME_INDEX];
  char fullName[PATH_INDEX + SIZE_OF_FILE_INFO + 1];
  mode_t mode;
  int len;
  bool createTheFile;

  strcpy(fullName, rootDir_p);
  len = strlen(fullName);
  strncpy(&fullName[len], path_p, FS_PATH_MAX_LENGTH);
  fullName[len + FS_PATH_MAX_LENGTH] = '\0';
  // The directory name stored in CSPSA already ends with /, so we can directly strcat filename
  len = strlen(fullName);
  strncpy(&fullName[len], fileName_p, FS_NAME_MAX_LENGTH);
  fullName[len + FS_NAME_MAX_LENGTH] = '\0';

  // create the file if its path is in the path filter list, or if there
  // is no filter list.
  createTheFile = (config_p->nbrOfFilterPaths == 0);
  if (!createTheFile)
  {
    uint32_t nbrOfFilterPaths = config_p->nbrOfFilterPaths;
    uint32_t rootDirStrLen = strlen(rootDir_p);
    while (nbrOfFilterPaths--)
    {
      int shortestNameLen = MIN(strlen(&fullName[rootDirStrLen]), strlen(config_p->filterPaths_pp[nbrOfFilterPaths]));
      createTheFile = (strncmp(&fullName[rootDirStrLen], config_p->filterPaths_pp[nbrOfFilterPaths], shortestNameLen) == 0);
      if (createTheFile)
      {
        break;
      }
    }
  }
  if (!createTheFile)
  {
    goto ErrorExit;;
  }

  mode = getUint32(&fileInfo_p[MODE_INDEX]);
  cspsaLog(LOG_DEBUG, "createFile, file %s, mode 0%o\n", fullName, mode);
  if (getUint32(&fileInfo_p[TYPE_INDEX]) == FS_TYPE_REGULAR)
  {
    // entry is a regular file
    int fd = open(fullName, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd < 0)
    {
      cspsaLog(LOG_ERR, "Error: could not open %s for writing: %s\n", fullName, strerror(errno));
      status = CONVERT_STATUS_WRITE_FAILED;
    }
    else
    {
      // write contents to file
      uint32_t totalWritten = 0;

      while (totalWritten < size)
      {
        ssize_t written = write(fd, &fileContents_p[totalWritten], size - totalWritten);
        if (written < 0)
        {
          cspsaLog(LOG_ERR, "Error: write to %s failed: %s\n", fullName, strerror(errno));
          status = CONVERT_STATUS_WRITE_FAILED;
          break;
        }
        totalWritten -= written;
      }
      if (close(fd) < 0)
      {
        cspsaLog(LOG_ERR, "Error: could not close %s: %s\n", fileName_p, strerror(errno));
        status = CONVERT_STATUS_WRITE_FAILED;
      }
    }
  } else {
    // entry is a directory
    if (mkdir(fullName, mode) < 0)
    {
      if (errno == EEXIST)
      {
        // Directory already exists.
        // We could check if its mode is as it should, and if not, try to change the mode.
        // However, ITP will add all leading directories (like "/") to the list and we
        // don't really want to change the mode of these directories, so we just leave
        // the mode as it is.
        /*struct stat statBuf;

        if (stat(fullName, &statBuf) < 0
           || (statBuf.st_mode != mode && chmod(fullName, mode) < 0))
        {
          // Impossible to stat the mode or to set it to the correct mode.
          // However, ITP will add directories like / to the list
          cspsaLog(LOG_ERR, "Error: cannot set mode of %s to 0x%x\n", fullName, mode);
          status = CONVERT_STATUS_WRITE_FAILED;
        }*/
      }
      else
      {
        // some other problem occurred
        cspsaLog(LOG_ERR, "Error: mkdir %s failed: %s\n", fullName, strerror(errno));
        status = CONVERT_STATUS_WRITE_FAILED;
      }
    }
  }

ErrorExit:
  return status;
}

/**
 * Creates one file (or directory) based on the file info found in the CSPSA.
 *
 * @param [in] rootDir_p  Root directory in which files will be created, e.g. "/modemfs"
 * @param [in] handle      CSPSA handle
 * @param [in] fileInfo_p  Info about the file to be created
 */
static ConvertStatus_t convertParameterToFile(const Config_t* const config_p, const char* rootDir_p, CSPSA_Handle_t handle, uint8_t* fileInfo_p)
{
  ConvertStatus_t status = CONVERT_STATUS_OK;

  if (getUint32(&fileInfo_p[TYPE_INDEX]) == FS_TYPE_REGULAR)
  {
    // parameter contains info about a regular file
    uint32_t fileSize = getUint32(&fileInfo_p[FILE_SIZE_INDEX]);
    uint32_t fileContentsKey = getUint32(&fileInfo_p[DUNIT_INDEX]);
    uint8_t* fileContents_p = (uint8_t*)malloc(fileSize);

    if (fileContents_p == NULL)
    {
      cspsaLog(LOG_ERR, "Out of memory\n");
      status = CONVERT_STATUS_OUT_OF_MEMORY;
      goto ErrorExit;
    }
    // read file contents from CSPSA
    status = readParameter(handle, fileContentsKey, fileSize, fileContents_p);
    if (status == CONVERT_STATUS_OK)
    {
      // create the file
      status = createFile(config_p, rootDir_p, fileInfo_p, fileSize, fileContents_p);
    }
    free(fileContents_p);
  }
  else
  {
    // create a directory
    status = createFile(config_p, rootDir_p, fileInfo_p, 0, NULL);
  }
ErrorExit:
  return status;
}

/**
 * Reads the CSPSA, finds the ITP parameters and converts them to files.
 */
ConvertStatus_t createFilesFromParameters(const Config_t* const config_p, CSPSA_Key_t firstKey)
{
  ConvertStatus_t status;
  uint8_t* fileInfo_p = NULL;
  uint32_t nrOfFiles = 0;
  uint32_t i;

  // Read first key (4 bytes, contains the number of files)
  {
    uint8_t data[4];

    status = readParameter(config_p->handle, firstKey, sizeof(data), data);
    if (status != CONVERT_STATUS_OK)
    {
      goto ErrorExit;
    }
    nrOfFiles = getUint32(data);
  }
  fileInfo_p = malloc(SIZE_OF_FILE_INFO);
  if (fileInfo_p == NULL)
  {
    cspsaLog(LOG_ERR, "Out of memory\n");
    status = CONVERT_STATUS_OUT_OF_MEMORY;
    goto ErrorExit;
  }
  for (i = 0; status == CONVERT_STATUS_OK && i < nrOfFiles; ++i)
  {
    CSPSA_Key_t key = firstKey + 1 + i;

    status = readParameter(config_p->handle, key, SIZE_OF_FILE_INFO, fileInfo_p);
    if (status == CONVERT_STATUS_OK)
    {
      status = convertParameterToFile(config_p, config_p->rootDir_p, config_p->handle, fileInfo_p);
    }
  }
ErrorExit:
  free(fileInfo_p);
  return status;
}

/**
 * Checks ITP update flag and returns flag_p >0 if flag is set and 0 if not.
 */
static ConvertStatus_t checkItpUpdateFlag(Config_t* config_p, int* flag_p)
{
  ConvertStatus_t Result = CONVERT_STATUS_UPDATE_ITEM_NOT_FOUND;
  CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_OK;
  CSPSA_Key_t Key = config_p->ItpUpdateFlagKey;
  CSPSA_Size_t Size;
  uint32_t BitIndex = config_p->ItpUpdateFlagBitIndex;
  uint32_t Value;

  cspsaLog(LOG_INFO, "Check ITP update flag: 0x%08X:%d\n", Key, BitIndex);

  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->handle, Key, &Size);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  if (Size * 8 < BitIndex)
  {
    cspsaLog(LOG_ERR, "Error: bit index (%d) out of range in item 0x%08X.\n", BitIndex, Key);
    goto ErrorExit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  *flag_p = Value & (1 << BitIndex);

  Result = CONVERT_STATUS_OK;

ErrorExit:

  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
  {
    cspsaLog(LOG_ERR, "Error: cannot read CSPSA item 0x%08X.\n", Key);
  }
  return Result;
}

/**
 * Checks ITP update flag and returns flag_p >0 if flag is set and 0 if not.
 */
static ConvertStatus_t clearItpUpdateFlag(Config_t* config_p)
{
  ConvertStatus_t Result = CONVERT_STATUS_UPDATE_ITEM_NOT_FOUND;
  CSPSA_Result_t CSPSA_Result = T_CSPSA_RESULT_OK;
  CSPSA_Key_t Key = config_p->ItpUpdateFlagKey;
  CSPSA_Size_t Size;
  uint32_t BitIndex = config_p->ItpUpdateFlagBitIndex;
  uint32_t Value;

  cspsaLog(LOG_INFO, "Clearing ITP update flag: 0x%08X:%d\n", Key, BitIndex);

  CSPSA_Result = CSPSA_GetSizeOfValue(config_p->handle, Key, &Size);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  if (Size * 8 < BitIndex)
  {
    cspsaLog(LOG_ERR, "Error: bit index (%d) out of range in item 0x%08X.\n", BitIndex, Key);
    goto ErrorExit;
  }

  CSPSA_Result = CSPSA_ReadValue(config_p->handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  Value = Value & ~(1 << BitIndex);

  CSPSA_Result = CSPSA_WriteValue(config_p->handle, Key, Size, (CSPSA_Data_t *) &Value);
  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS) goto ErrorExit;

  Result = CONVERT_STATUS_OK;

ErrorExit:

  if (CSPSA_Result >= T_CSPSA_RESULT_ERRORS)
  {
    cspsaLog(LOG_ERR, "Error: cannot read CSPSA item 0x%08X.\n", Key);
  }
  return Result;
}

//------------------------------------------------------------------------------
//  HELP FUNCTIONS
//------------------------------------------------------------------------------

static void createEmptyFile(const char* fileName_p)
{
  if (fileName_p != NULL)
  {
    FILE *fp = fopen(fileName_p, "w");

    if (fp == NULL)
    {
      cspsaLog(LOG_ERR, "Error: cannot create %s: %s\n", fileName_p, strerror(errno));
    }
    else
    {
      (void)fclose(fp);
    }
  }
}

/** Converts status to human readable string representation. */
static const char* statusToString(int status)
{
  switch (status)
  {
    case CONVERT_STATUS_OK: return "ok";
    case CONVERT_STATUS_OPEN_FAILED: return "open failed";
    case CONVERT_STATUS_CSPSA_CORRUPT: return "CSPSA corrupt";
    case CONVERT_STATUS_OUT_OF_MEMORY: return "out of memory";
    case CONVERT_STATUS_WRITE_FAILED: return "write failed";
    case CONVERT_STATUS_ERROR: return "internal error";
    case CONVERT_STATUS_INVALID_USAGE: return "invalid usage";
    default: return "?";
  }

}

static void removeDoubleSlashes(char* fileName)
{
  char* tempStr = fileName;
  while ((tempStr = strstr(tempStr, "//")) != NULL)
  {
    strcpy(tempStr, tempStr + 1);
  }
}

//------------------------------------------------------------------------------
//  COMMAND LINE PARSING
//------------------------------------------------------------------------------

static void usage(void)
{
  printf(
    "\n"
    "Usage: cspsa2nwm [-r rootdir] [-f] [-d] [-g guardFile] [-p path] [-p ...] -u <m:n> cspsaName\n"
    " -h           Prints this help.\n"
    " -g guardFile Name of the guard file. If this file exists, nothing will be\n"
    "              done unless -f is specified. After copying, the guard file\n"
    "              is created.\n"
    " -u <m:n>     NWM configuration structure is updated if bit n in CSPSA item\n"
    "              m is set. Value m must be a 32-bit hexadecimal value, while n must\n"
    "              must be a decimal value from 0 to 31.\n"
    " -f           Force copying of CSPSA parameters to files, even if the update bit\n"
    "              is not set or the guard file exists\n"
    " -r rootdir   Create all files relative to the root dir. Default is /modemfs\n"
    " -d           Enable debug logging (to syslog)\n\n"
    " -dd          Enable debug logging (to syslog + stdout)\n\n"
    " -p path      Only files on the specified path with be copied from CSPA to file system. There may be\n"
    "              several -p options specified, each allowing a specific path to be copied.\n"
    "Creates NWM calibration files from calibration data stored in a trim area.\n\n"
    "\n");
}

static ConvertStatus_t parseCmdline(int argc, char *argv[], Config_t *config_p)
{
  int c;
  int result = CONVERT_STATUS_OK;

  //Defaults
  config_p->logLevel = LOG_INFO;
  config_p->logToStdout = false;
  config_p->cspsaName_p = NULL;
  config_p->rootDir_p = strdup("/modemfs/");
  if (config_p->rootDir_p == NULL)
  {
    cspsaLog(LOG_ERR, "Out of memory\n");
    result = CONVERT_STATUS_OUT_OF_MEMORY;
    goto ErrorExit;
  }
  config_p->guardFileName_p = NULL;
  config_p->forceFileCreation = false;

  while ((c = getopt (argc, argv, "fg:u:r:p:dh?")) != -1)
  {
    switch (c)
    {
    case 'f':
      config_p->forceFileCreation = true;
      break;

    case 'g':
      config_p->guardFileName_p = strdup(optarg);
      if (config_p->guardFileName_p == NULL)
      {
        cspsaLog(LOG_ERR, "Out of memory\n");
        result = CONVERT_STATUS_OUT_OF_MEMORY;
        goto ErrorExit;
      }
      break;

    case 'u':
      config_p->cspsaUpdateItemInfo_p = strdup(optarg);
      if (config_p->cspsaUpdateItemInfo_p == NULL)
      {
        cspsaLog(LOG_ERR, "Out of memory\n");
        result = CONVERT_STATUS_OUT_OF_MEMORY;
        goto ErrorExit;
      }
      break;

    case 'r':
      free(config_p->rootDir_p);
      config_p->rootDir_p = strdup(optarg);
      if (config_p->rootDir_p == NULL)
      {
        cspsaLog(LOG_ERR, "Out of memory\n");
        result = CONVERT_STATUS_OUT_OF_MEMORY;
        goto ErrorExit;
      }
      break;

    case 'p':
        config_p->filterPaths_pp =
          realloc(
            config_p->filterPaths_pp,
            (config_p->nbrOfFilterPaths + 1) * sizeof(*config_p->filterPaths_pp)
          );
        if (config_p->filterPaths_pp == NULL)
        {
          cspsaLog(LOG_ERR, "Out of memory\n");
          result = CONVERT_STATUS_OUT_OF_MEMORY;
          goto ErrorExit;
        }
        config_p->filterPaths_pp[config_p->nbrOfFilterPaths] = strdup(optarg);
        if (config_p->filterPaths_pp[config_p->nbrOfFilterPaths] == NULL)
        {
          cspsaLog(LOG_ERR, "Out of memory\n");
          result = CONVERT_STATUS_OUT_OF_MEMORY;
          goto ErrorExit;
        }
        config_p->nbrOfFilterPaths++;
        break;

    case 'd':
      if (config_p->logLevel == LOG_DEBUG)
      {
        config_p->logToStdout = true;
      }
      config_p->logLevel = LOG_DEBUG;
      break;

    case 'h':
    case '?':
      return CONVERT_STATUS_INVALID_USAGE;

    default:
      return CONVERT_STATUS_INVALID_USAGE; //error
    }
  }
  if (config_p->cspsaUpdateItemInfo_p == NULL)
  {
    fprintf(stderr, "%s: -u option is not set.\n", (strrchr(argv[0], '/') != 0) ? strrchr(argv[0], '/') + 1 : argv[0]);
    result = CONVERT_STATUS_INVALID_USAGE;
  }
  else
  if (optind < argc)
  {
    // CSPSA name is found at index optind
    config_p->cspsaName_p = argv[optind];
    if (optind < argc - 1)
    {
      // more than 1 non-optional arguments were on the command line
      fprintf(stderr, "%s: Invalid usage\n", argv[0]);
      result = CONVERT_STATUS_INVALID_USAGE;
    }
  }
  else
  {
    fprintf(stderr, "%s: CSPSA name is missing in command line\n", argv[0]);
    result = CONVERT_STATUS_INVALID_USAGE;
  }
  if (config_p->guardFileName_p != NULL)
  {
    // guard file is relative to root directory; calculate full filename
    char* fullGuardFileName_p = (char*)malloc(strlen(config_p->rootDir_p) + strlen(config_p->guardFileName_p) + 1);
    if (fullGuardFileName_p == NULL)
    {
      cspsaLog(LOG_ERR, "Out of memory\n");
      result = CONVERT_STATUS_OUT_OF_MEMORY;
      goto ErrorExit;
    }
    strcpy(fullGuardFileName_p, config_p->rootDir_p);
    strcat(fullGuardFileName_p, config_p->guardFileName_p);
    free(config_p->guardFileName_p);
    removeDoubleSlashes(fullGuardFileName_p);
    config_p->guardFileName_p = fullGuardFileName_p;
  }

  // Parse and check -u m:n parameter.
  if (config_p->cspsaUpdateItemInfo_p != NULL)
  {
    char* Keystr_p = config_p->cspsaUpdateItemInfo_p;
    char* BitIndexStr_p = NULL;
    char* end_p;
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
      cspsaLog(
        LOG_ERR,
        "Error: the parameter following -u must be of the form <id>:<bit index>,\n"
        "where id is in {0 - 0xFFFFFFFF} and bit index in {0 - 31}.\n");
      result = CONVERT_STATUS_INVALID_USAGE;
    }
  }

ErrorExit:
  return result;
}

//------------------------------------------------------------------------------
//  MAIN
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  Config_t config;
  CSPSA_Result_t CSPSA_Result;
  int result;
  int UpdateFlag = 0;

  memset(&config, 0, sizeof(config));
  result = parseCmdline(argc, argv, &config);

  if (result != CONVERT_STATUS_OK)
  {
    usage();
  }
  else
  {
    cspsaOpenLog(config.logLevel, config.logToStdout);

    CSPSA_Result = CSPSA_Open(config.cspsaName_p, &config.handle);
    if (CSPSA_Result != T_CSPSA_RESULT_OK)
    {
      cspsaLog(LOG_ERR, "Error: cannot open CSPSA %s, error code %d\n", config.cspsaName_p, CSPSA_Result);
      result = CONVERT_STATUS_OPEN_FAILED;
      goto ErrorExit;
    }
    cspsaLog(LOG_INFO, "Create calibration files from parameters in trim area\n");
    result = checkItpUpdateFlag(&config, &UpdateFlag);
    if (result != CONVERT_STATUS_OK) goto ErrorExit;
    if (config.forceFileCreation || UpdateFlag || ((config.guardFileName_p != NULL) && (access(config.guardFileName_p, F_OK) != 0)))
    {
      // We need to create files.
      result = createFilesFromParameters(&config, 0);
      cspsaLog(LOG_INFO, "Finished creation of files from CSPSA %s, result: %s\n",
             config.cspsaName_p, statusToString(result));
      if (result != CONVERT_STATUS_OK) goto ErrorExit;
      result = clearItpUpdateFlag(&config);
      if (result != CONVERT_STATUS_OK)
      {
        goto ErrorExit;
      }
      if (config.guardFileName_p != NULL)
      {
        createEmptyFile(config.guardFileName_p);
      }
    }
    else
    {
      cspsaLog(LOG_INFO, "ITP update flag was not set, so no need to create calibration files\n");
    }
    cspsaCloseLog();
  }

ErrorExit:

  free(config.cspsaUpdateItemInfo_p);
  free(config.guardFileName_p);
  free(config.rootDir_p);
  while (config.nbrOfFilterPaths--)
  {
    free(config.filterPaths_pp[config.nbrOfFilterPaths]);
  }
  free(config.filterPaths_pp);

  if (config.handle) {
    CSPSA_Result = CSPSA_Close(&config.handle);
    if (CSPSA_Result != T_CSPSA_RESULT_OK)
    {
      cspsaLog(LOG_ERR, "Error: could not close CSPSA %s, error code %d\n", config.cspsaName_p, CSPSA_Result);
      result = CONVERT_STATUS_OPEN_FAILED;
    }
  }

  return -result;
}

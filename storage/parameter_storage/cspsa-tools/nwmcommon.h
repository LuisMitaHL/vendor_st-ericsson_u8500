/* **************************************************************************
 *
 * nwmcommon.h
 *
 * Copyright (C) 2012 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2012, johan.rudholm@stericsson.com
 *
 * DESCRIPTION:
 * Header file common to the nwm*-commands
 *
 **************************************************************************** */

#ifndef NWMCOMMON_H
#define NWMCOMMON_H

/** Size of a parameter with info on a file that is to be created from a CSPSA parameter */
#define SIZE_OF_FILE_INFO 96
/** Max length of a path name (excluding trailing 0)  */
#define FS_PATH_MAX_LENGTH    52
/** Max length of a file name (excluding trailing 0) */
#define FS_NAME_MAX_LENGTH    12

/** Index in file info that contains the type of the file */
#define TYPE_INDEX           0
/** Index in file info that contains creation time of the file, seconds since 1970 */
#define C_TIME_SEC_INDEX     4
/** Index in file info that contains creation time of the file, micro seconds */
#define C_TIME_USEC_INDEX    8
/** Index in file info that contains the mode of the file */
#define MODE_INDEX          12
/** Index in file info that contains the size of the file */
#define FILE_SIZE_INDEX     16
/** Index in file info that contains the path to the file */
#define PATH_INDEX          24
/** Index in file info that contains the name of the file */
#define NAME_INDEX      ((PATH_INDEX) + FS_PATH_MAX_LENGTH)
/** Index in file info that contains CSPSA key that contains the contents of the file */
#define DUNIT_INDEX     ((NAME_INDEX) + FS_NAME_MAX_LENGTH)
/** Index in file info that unused part of file info */
#define UNUSED_INDEX    ((DUNIT_INDEX) + 4)

/** The item is a directory */
#define FS_TYPE_DIR  0
/** The item is a regular file */
#define FS_TYPE_REGULAR 1

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* This is an arbitrarily chosen limit for this conversion program, in order to
make it easy to separate file info units from file data units. */
#define MAX_FILES 999

#define ERRORSTR "ERROR: "

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

/* ========================================================================== */

static const char ERRORSTR_INTERNAL[] = ERRORSTR "Internal error at %d.\n";
static const char ERRORSTR_OUT_OF_MEMORY[] = ERRORSTR "Out of memory at %d.\n";

/* ========================================================================== */

struct param
{
  struct param *next;
  struct param *prev;
  int key;
  unsigned int size;
  unsigned char *data;
};

typedef struct param param_t;

struct state
{
  int entries;
  param_t *par;
};

struct cmd_line_options
{
  char *outfile;
  char *inpath;
  bool debug;
  char *program;
  char **subpath;
  int subpaths;
  char *area;
};

/* ========================================================================== */

int create_id(char *program, char *version, char *paramstr, unsigned int size);
int process_dirtree(struct cmd_line_options* cmd_line_options, char *realpath,
				char *path, char *subpath, struct state *state);
void free_param(param_t *par);
param_t *next_param(param_t *par);

/* ========================================================================== */

#endif /* NWMCOMMON_H */

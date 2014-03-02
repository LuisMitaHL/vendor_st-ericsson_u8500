/*******************************************************************
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Mikael GULLBERG <mikael.xx.gullberg@stericsson.com>
 *
 * DESCRIPTION: Implement functions for QSTORE.
 *
 *******************************************************************/
#include "r_os.h"

#include "errno.h"
#include "sys/stat.h"
#include "wchar.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/types.h"
#include "stdio.h"

char* wchar_to_char(const wchar_t *path_p)
{
    char* src_p = (char *)path_p;
    char* dest_p = HEAP_UNTYPED_ALLOC(wcslen(path_p)+1);
    unsigned int i;

    for(i = 0; i< wcslen(path_p); i++)
    {
      dest_p[i]=src_p[i * sizeof(wchar_t)];
    }
    dest_p[i]='\0';

    return dest_p;
}

int w_open(const wchar_t *PathName_p, int Omode, mode_t Amode)
{
  int ret = 0, err = 0;
  char *newpath_p;

  newpath_p = wchar_to_char(PathName_p);
  ret = open(newpath_p, Omode, Amode);
  err = errno;

  HEAP_FREE(&newpath_p);
  errno = err;
  return ret;
}

int w_rename(const wchar_t *Old_p, const wchar_t *New_p)
{
  int ret = 0, err = 0;
  char *pathA_p, *pathB_p;

  pathA_p = wchar_to_char(Old_p);
  pathB_p = wchar_to_char(New_p);

  ret = rename(pathA_p, pathB_p);
  err = errno;

  HEAP_FREE(&pathA_p);
  HEAP_FREE(&pathB_p);

  errno = err;
  return ret;
}

int w_remove(const wchar_t *FileName_p)
{
  int ret = 0, err = 0;
  char *newpath_p;

  newpath_p = wchar_to_char(FileName_p);
  ret = remove(newpath_p);
  err = errno;

  HEAP_FREE(&newpath_p);
  errno = err;
  return ret;
}

int w_stat(const wchar_t *Path_p, struct stat *Buf_p)
{
  int ret = 0, err = 0;
  char *newpath_p;

  newpath_p = wchar_to_char(Path_p);
  ret = stat(newpath_p, Buf_p);
  err = errno;

  HEAP_FREE(&newpath_p);
  errno = err;
  return ret;
}

FILE *w_fopen(const wchar_t *FileName_p, const char *Mode_p)
{
  FILE* ret;
  int err = 0;
  char *newpath_p;

  newpath_p = wchar_to_char(FileName_p);
  ret = fopen(newpath_p, Mode_p);
  err = errno;

  HEAP_FREE(&newpath_p);
  errno = err;
  return ret;
}

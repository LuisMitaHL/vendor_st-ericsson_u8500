/* **************************************************************************
 *
 * r_fsu.h
 *
 * Copyright (C) 2010 ST-Ericsson SA
 *
 * This software is released either under the terms of the BSD-style
 * license accompanying CSPSA or a proprietary license obtained from
 * ST-Ericsson SA.
 *
 * Author: 2010, Mikael GULLBERG <mikael.xx.gullberg@stericsson.com>
 *
 **************************************************************************** */

#ifndef R_FSU_H
#define R_FSU_H

#include <wchar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

char* wchar_to_char(wchar_t *path_p);
int w_open(const wchar_t *PathName_p, int Omode, mode_t Amode);
int w_rename(const wchar_t *Old_p, const wchar_t *New_p);
int w_remove(const wchar_t *FileName_p);
int w_stat(const wchar_t *Path_p, struct stat *Buf_p);
FILE *w_fopen(const wchar_t *FileName_p, const char *Mode_p);

#endif //#ifndef R_FSU_H

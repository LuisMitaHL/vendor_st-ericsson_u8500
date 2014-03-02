/*
* Copyright 2008, The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "Img_EnsWrapper_Shared.h"
#ifndef _MSC_VER
	#include <sys/types.h>
	#include <unistd.h>
#else
	#include <process.h>
	#define getpid _getpid
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Img_mapinfo.h"

img_mapinfo *parse_maps_line(char *line)
{
	img_mapinfo *mi;
	int len = strlen(line);

	if(len < 1)
		return 0;
	line[--len] = 0;

	if(len < 50)
		return 0;
	if(line[20] != 'x')
		return 0;

	mi = (img_mapinfo *)malloc(sizeof(img_mapinfo) + (len - 47));
	if(mi == 0)
		return 0;

	mi->start = strtoul(line, 0, 16);
	mi->end   = strtoul(line + 9, 0, 16);
	/* To be filled in parse_exidx_info if the mapped section starts with
	* elf_header
	*/
	mi->exidx_start = mi->exidx_end = 0;
	mi->next = 0;
	strcpy(mi->name, line + 49);

	return mi;
}

/* Map a pc address to the name of the containing ELF file */
const char *map_to_name(img_mapinfo *mi, unsigned pc, const char* def, unsigned *offset)
{
	while(mi)
	{
		if((pc >= mi->start) && (pc < mi->end))
		{
			*offset = pc - mi->start;
			return mi->name;
		}
		mi = mi->next;
	}
	return def;
}

void free_mapinfo(img_mapinfo *mi)
{
	while(mi)
	{
		img_mapinfo *tmp = mi;
		mi = mi->next;
		free(tmp);
	}
}

img_mapinfo *get_process_mapinfo()
{
	char data[1024];
	img_mapinfo *milist = 0;
	FILE *fp;

	sprintf(data, "/proc/%d/maps", getpid());
	fp = fopen(data, "r");
	if(fp)
	{
		while(fgets(data, 1024, fp))
		{
			img_mapinfo *mi = parse_maps_line(data);
			if(mi)
			{
				mi->next = milist;
				milist = mi;
			}
		}
		fclose(fp);
	}
	return milist;
}

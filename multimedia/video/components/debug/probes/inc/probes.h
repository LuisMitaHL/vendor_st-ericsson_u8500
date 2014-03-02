/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _INST_DUMP_H_
#define _INST_DUMP_H_

#include "simple_types.h"

#include "gen_inst_dump.h"

extern FILE *fp_h264_param_in;
extern FILE *fp_h264_param_out;

void open_dump_file (void);
void close_dump_file (void);

void dump_array(char* filename, int index, void* addr, unsigned long size);

t_uint32 crc(t_uint8 *data, t_uint32 len);
t_uint16 dbg_make_crc(t_uint16 channel, t_uint8* addr, t_uint32 len, t_uint16 conf);
t_uint16 dbg_make_crc_(t_uint16 channel, t_desc_struct* desc, t_uint8* addr, t_uint32 len, t_uint16 conf);
void write_struct( t_uint8* struct_address, FILE * fp, char* header, int size );
void marshall(t_desc_struct* desc, t_uint8* addr, t_uint32 len, t_uint8** buf, t_uint32* sz);

#define dbg_make_crc_type(channel, type, addr, len, conf) \
  dbg_make_crc_(channel, &desc_##type, (t_uint8*)addr, len, conf)

void dump_struct_to_file (t_uint32* struct_address, t_uint32 struct_size, FILE * file_ptr, char* header);
void dump_struct_from_file ( t_uint32* struct_address, t_uint32 struct_size, FILE* file_ptr);

#define dump_struct_to_file_header(p, s, f, i) \
{ \
  char tmp[100]; \
  sprintf(tmp, "# Pic %ld, Struct %s, Size %d\n", i, #s, sizeof(s)); \
  { \
	t_uint8* buf; \
	t_uint32 size; \
	t_uint32 i; \
    marshall(&desc_##s, (t_uint8*)p, 1, &buf, &size); \
	write_struct((t_uint8*)buf, f, tmp, size); \
  } \
}

#define dump_struct_to_file_header_raw(p, s, f, i) \
{ \
  char tmp[100]; \
  sprintf(tmp, "# Pic %ld, Struct %s, Size %d\n", i, #s, sizeof(s)); \
  dump_struct_to_file((t_uint32*)p, sizeof(s), f, tmp); \
}

#endif


/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "probes.h"


/* #define DEBUG */

/* define all txt files */
FILE *fp_h264_param_in = NULL;
FILE *fp_h264_param_out = NULL;

#if ( defined(_DEBUG_DUMP_) || defined(_DEBUG_COMP_) )
FILE *fp_h264_struct_mv_in = NULL;
FILE *fp_h264_struct_mv_out = NULL;
FILE *fp_h264_array_mv_in = NULL;
FILE *fp_h264_array_mv_out = NULL;
#endif /* ( _DEBUG_DUMP_ || _DEBUG_COMP_ ) */

void open_dump_file (void)
{
	if ( (fp_h264_param_in = fopen ("h264_param_in.txt", "wr")) == NULL )
	{
		printf("Cannot open h264_param_in.txt file");
	}

	if ( (fp_h264_param_out = fopen ("h264_param_out.txt", "wr")) == NULL )
	{
		printf("Cannot open h264_param_out.txt file");
	}
}

void close_dump_file (void)
{
	fclose (fp_h264_param_in);
	fclose (fp_h264_param_out);
}

#define QUOTIENT 0x04c11db7

t_uint32 crc(t_uint8 *data, t_uint32 len)
{
	t_uint32 result;
	t_uint32 i,j;
	t_uint8  octet;
	t_uint32 incr = 0;

	if (len & 0x80000000)
	{
		data++;
		incr = 1;
		len &= ~0x80000000;
	}

	result = 0xFFFFFFFF;

	for (i=0; i<len; i++)
	{
		octet = *(data++);
		data += incr;
		for (j=0; j<8; j++)
		{
			if ((octet >> 7) ^ (result >> 31))
			{
				result = (result << 1) ^ QUOTIENT;
			}
			else
			{
				result = (result << 1);
			}
			octet <<= 1;
		}
	}

	return ~result;
}

#define MAX_CHANNELS 20

typedef struct
{
	FILE*    fp;
	t_uint32 nb;
	t_uint16 init;
} entry;

entry ftab[MAX_CHANNELS];

static char tmp[256];

int channel_init = 0;

unsigned int check_point_channel = 0;
unsigned int check_point_entry   = 0;
unsigned int enable_channels     = 0;

int toto;

#define SEP " \t"

#include "../inc/gen_inst_dump.c"

void dbg_break(void);

void dbg_break(void)
{
	toto = 1;
}

t_uint16 dbg_make_crc(t_uint16 channel, t_uint8* addr, t_uint32 len, t_uint16 conf)
{
	t_uint32 ref_sum;
	char filename[50];

	if (!channel_init)
	{
		FILE* h = fopen("config.txt", "rt");
		if (h)
		{
			while(fgets(tmp, sizeof(tmp), h))
			{
				char* str;
				str = strtok(tmp, SEP);
				if (str[0] == '#')
					break;
				else if (!strcmp(str, "check_point_channel"))
				{
					str = strtok(NULL, SEP);
					sscanf(str, "%d", &check_point_channel);
				}
				else if (!strcmp(str, "check_point_entry"))
				{
					str = strtok(NULL, SEP);
					sscanf(str, "%d", &check_point_entry);
				}
				else if (!strcmp(str, "enable_channels"))
				{
					str = strtok(NULL, SEP);
					sscanf(str, "%d", &enable_channels);
				}
			}
		}
		channel_init = 1;
	}

	if (channel >= MAX_CHANNELS)
		return 1;

	if (!ftab[channel].init)
	{
		if (enable_channels)
		{
			sprintf(filename, "crc%d.txt", channel);
			ftab[channel].fp = fopen(filename, "wt");
			if (!ftab[channel].fp)
			{
				printf("Cannot open file %s\n", filename); 
				return 0;
			}
		}

		ftab[channel].nb = 0;
		ftab[channel].init = 1;
	}

	ftab[channel].nb++;

	if (enable_channels)
	{
		ref_sum = crc(addr, len + (conf ? 0x80000000 : 0));
		fprintf(ftab[channel].fp, "%08lx\n", ref_sum);
		/*printf(">>> %d %d\n", channel, ftab[channel].nb);*/
	}

	if (check_point_entry && (channel == check_point_channel) && (ftab[channel].nb == check_point_entry))
	{
		printf("Reached last successfull checkpoint\n");
		dbg_break();
	}
	return 0;
}

int is_BE(void);
int is_BE(void)
{
	int val = 1;
	return *((char*)&val) ? 0 : 1;
}

/* desc		: ptr to structure descriptor
 * addr2	: array of ptr to structure
 * len		: nb of elements in array
 * buf2		: output buffer
 * sz		: initial size of output buffer
 * */
void marshall_struct(t_desc_struct* desc, t_uint8** addr2, t_uint32 len, t_uint8** buf2, t_uint32* sz);
void marshall_struct(t_desc_struct* desc, t_uint8** addr2, t_uint32 len, t_uint8** buf2, t_uint32* sz)
{
	t_uint32 i;
	int j,l;
	int nb_types = desc->nb_types;
	int do_align = 1;
	t_uint32 size = *sz;
	t_uint8* addr = *addr2;
	t_uint8* buf = *buf2;

	if (!nb_types)
	{
		nb_types = 1;
		do_align = 0;
	}

	for (i = 0; i < len; i++)
	{
		for (j = 0; j < desc->nb_types; j++)
		{
			int num = desc->types[j].num;
			if (!num) num = 1;

			if ((desc->types[j].size == 32) || (desc->types[j].size == 33))
			{
				/* Alignement 32 */
				if (do_align && (size % 4))
				{
					t_uint32 k;
					t_uint32 pad = size % 4;
					for (k = 0; k < pad; k++)
					{
						*buf++ = 0;
						size++;
					}
				}
				for(l = 0; l < num; l++)
				{
					/* handle swapping */
					if (is_BE())
					{
						/* no swap */
						if (desc->types[j].size == 32)
						{
							/* t_uint32 and t_sint32 */
							*buf++ = *addr++;
							*buf++ = *addr++;
							*buf++ = *addr++;
							*buf++ = *addr++;
							size += 4;
						}
						else
						{
							/* t_ulong_value and t_slong_value */
							*buf++ = addr[2];
							*buf++ = addr[3];
							*buf++ = addr[0];
							*buf++ = addr[1];
							addr += 4;
							size += 4;
						}
					}
					else
					{
						/* swap */
						if (desc->types[j].size == 32)
						{
							/* t_uint32 and t_sint32 */
							*buf++ = addr[3];
							*buf++ = addr[2];
							*buf++ = addr[1];
							*buf++ = addr[0];
							addr += 4;
							size += 4;
						}
						else
						{
							/* t_ulong_value and t_slong_value */
							*buf++ = addr[1];
							*buf++ = addr[0];
							*buf++ = addr[3];
							*buf++ = addr[2];
							addr += 4;
							size += 4;
						}
					}
				}
			}
			else if (desc->types[j].size == 16)
			{
				/* Alignement 16 ( This should never happend because t_uint8 is 16-bits */
				/*if (size % 2)
				  {
				  int k;
				  int pad = size % 2;
				  for (k = 0; k < pad; k++)
				  {
				 *buf++ = 0;
				 size++;
				 }
				 }*/
				/* handle swapping */
				for(l = 0; l < num; l++)
				{
					if (is_BE())
					{
						/* no swap */
						*buf++ = *addr++;
						*buf++ = *addr++;
						size += 2;
					}
					else
					{
						/* swap */
						*buf++ = addr[1];
						*buf++ = addr[0];
						addr += 2;
						size += 2;
					}
				}
			}
			else if (desc->types[j].size == 8)
			{
				/* t_uint8 is 16-bits */
				for(l = 0; l < num; l++)
				{
					if (is_BE())
					{
						*buf++ = 0;
						*buf++ = *addr++;
						size += 2;
					}
					else
					{
						*buf++ = *addr++;
						*buf++ = 0;
						size += 2;
					}
				}
			}
			else if (desc->types[j].pstruct)
			{
				marshall_struct((t_desc_struct*)desc->types[j].pstruct, &addr, num, &buf, &size);
			}
			else
			{
				printf("error\n");
				exit(0);
			}
		}
		/* Struct padding (32-bits) */
		if (0 & do_align && (size % 4))
		{
			int k;
			int pad = size % 4;
			for (k = 0; k < pad; k++)
			{
				*buf++ = 0;
				size++;
			}
		}
	}
	*addr2 = addr;
	*buf2 = buf;
	*sz = size;
}

void write_struct( t_uint8* struct_address, FILE * fp, char* header, int size )
{
	t_uint8 *ptr = (t_uint8*) struct_address;
	int i;
	fprintf(fp, header);
	/*fprintf(fp, "size = %d\n", size);*/
	for(i = 0; i < (size/4); i++)
	{
		fprintf(fp, "%02x\t%02x\t%02x\t%02x\n",ptr[0],ptr[1],ptr[2], ptr[3]);
		ptr += 4;
	}
	fprintf(fp, "\n");
}

void marshall(t_desc_struct* desc, t_uint8* addr, t_uint32 len, t_uint8** buf, t_uint32* sz)
{
	t_uint8* buffer = (t_uint8*)malloc(65536*5);
	t_uint8* buffer2 = buffer;
	t_uint8* addr2 = addr;
	*sz = 0;
	marshall_struct(desc, &addr2, len, &buffer2, sz);
	*buf = buffer;
}

t_uint16 dbg_make_crc_(t_uint16 channel, t_desc_struct* desc, t_uint8* addr, t_uint32 len, t_uint16 conf)
{
	t_uint32 ref_sum;
	char filename[50];

	if (!channel_init)
	{
		FILE* h = fopen("config.txt", "rt");
		if (h)
		{
			while(fgets(tmp, sizeof(tmp), h))
			{
				char* str;
				str = strtok(tmp, SEP);
				if (str[0] == '#')
					break;
				else if (!strcmp(str, "check_point_channel"))
				{
					str = strtok(NULL, SEP);
					sscanf(str, "%d", &check_point_channel);
				}
				else if (!strcmp(str, "check_point_entry"))
				{
					str = strtok(NULL, SEP);
					sscanf(str, "%d", &check_point_entry);
				}
				else if (!strcmp(str, "enable_channels"))
				{
					str = strtok(NULL, SEP);
					sscanf(str, "%d", &enable_channels);
				}
			}
		}
		channel_init = 1;
	}

	if (channel >= MAX_CHANNELS)
		return 1;

	if (!ftab[channel].init)
	{
		if (enable_channels)
		{
			sprintf(filename, "crc%d.txt", channel);
			ftab[channel].fp = fopen(filename, "wt");
			if (!ftab[channel].fp)
			{
				printf("Cannot open file %s\n", filename); 
				return 0;
			}
		}

		ftab[channel].nb = 0;
		ftab[channel].init = 1;
	}

	ftab[channel].nb++;

	if (enable_channels)
	{
		t_uint8* buf;
		t_uint32 size;
		marshall(desc, addr, len, &buf, &size);
		ref_sum = crc(buf, size + (conf ? 0x80000000 : 0));
		free(buf);
		fprintf(ftab[channel].fp, "%08lx\n", ref_sum);
		/*printf(">>> %d %d\n", channel, ftab[channel].nb);*/
	}

	if (check_point_entry && (channel == check_point_channel) && (ftab[channel].nb == check_point_entry))
	{
		printf("Reached last successfull checkpoint\n");
		dbg_break();
	}
	return 0;
}

void dump_struct_to_file( t_uint32* struct_address, t_uint32 struct_size, FILE * file_ptr, char* header )
{
	t_uint32 size_in_byte_number=0; 
	t_uint8 *struct_ptr;
	t_uint32 cpt=0;

	size_in_byte_number = struct_size/sizeof(t_uint8); 

	struct_ptr = (t_uint8*) struct_address;

	fprintf(file_ptr, header);

	for( cpt=1; cpt<=size_in_byte_number; cpt++)
	{
		if ( cpt % 4 == 0)
			fprintf(file_ptr,"%02x\n",*struct_ptr);
		else
			fprintf(file_ptr,"%02x\t",*struct_ptr);

		struct_ptr ++;
	}

	fprintf(file_ptr,"\n");
}           

void dump_struct_from_file ( t_uint32* struct_address, t_uint32 struct_size, FILE* file_ptr)
{     
	t_uint8 *struct_ptr;
	t_uint32 size_in_byte_number=0; 
	t_uint32 cpt=0;
	char line[100];
	char test_read_1[50];
	char test_read_2[50];
	char test_read_3[50];
	char test_read_4[50]; 

	struct_ptr = (t_uint8*) struct_address;

	size_in_byte_number = struct_size/sizeof(t_uint8);


	for( cpt=0; cpt<size_in_byte_number; cpt++)
	{
		fgets(line, 100, file_ptr);
#ifdef DEBUG 
		printf("\nline : %s", line);
#endif /*DEBUG */
		sscanf(line, "%s\t%s\t%s\t%s\n", test_read_1, test_read_2, test_read_3, test_read_4);
#ifdef DEBUG 
		printf("\ntest_read : %s\t%s\t%s\t%s", test_read_1, test_read_2, test_read_3, test_read_4); 
#endif /*DEBUG */

		*struct_ptr = strtol(test_read_1,NULL,16);
#ifdef DEBUG 
		printf("\ntest_ptr :%02x",*struct_ptr);
#endif /*DEBUG */
		struct_ptr ++;

		*struct_ptr = strtol(test_read_2,NULL,16);
#ifdef DEBUG 
		printf("\t%02x",*struct_ptr);
#endif /*DEBUG */
		struct_ptr ++;

		*struct_ptr = strtol(test_read_3,NULL,16);
#ifdef DEBUG 
		printf("\t%02x",*struct_ptr);
#endif /*DEBUG */
		struct_ptr ++;

		*struct_ptr = strtol(test_read_4,NULL,16);
#ifdef DEBUG 
		printf("\t%02x",*struct_ptr);
#endif /*DEBUG */
		struct_ptr ++;

	}

}

#define MAX_CHANNELS_DUMP 20

typedef struct
{
	FILE* fp_data;
	FILE* fp_index;
	int   cur_index;
} dump_entry;

dump_entry dump_tab[MAX_CHANNELS_DUMP];

void dump_array(char* filename, int index, void* addr, unsigned long size)
{
}


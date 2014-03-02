/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   DTH Filter
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdlib.h>
#include <unistd.h> /* for getopt */
#include <stdio.h> /* for printf */
#include <string.h> /* for strcmp */
#include <stdlib.h>
#include <stdint.h>
#include <string.h> /*for strcmp */

void
usage()
{
	fprintf(stderr, "dthfilter: -r type or -w type [-a cell index]\n"
		"-w\twrites simple or array of the specified type.\n"
		"-r\treads a single or array of the specified type. "
		"this option supersedes -w.\n"
		"-a\toptional: index of the cell array to read (first is 0). "
		"If the index exceeds the array bounds then "
		"no value is returned.\t-a is processed only if -r is set. "
		"By default the whole array is returned\n");
	exit(-1);
}

int get_data_size(char *type)
{
	int size = 0 ;

	/* Check data size according to type string. */
	if (strcmp(type, "u8") == 0) {
		size = sizeof(uint8_t) ;
	} else if (strcmp(type, "s8") == 0) {
		size = sizeof(int8_t) ;
	} else if (strcmp(type, "u16") == 0) {
		size = sizeof(uint16_t) ;
	} else if (strcmp(type, "s16") == 0) {
		size = sizeof(int16_t) ;
	} else if (strcmp(type, "u32") == 0) {
		size = sizeof(uint32_t) ;
	} else if (strcmp(type, "s32") == 0) {
		size = sizeof(int32_t) ;
	} else if (strcmp(type, "u64") == 0) {
		size = sizeof(uint64_t) ;
	} else if (strcmp(type, "s64") == 0) {
		size = sizeof(int64_t) ;
	} else if (strcmp(type, "float") == 0) {
		size = sizeof(float);
	} else if (strcmp(type, "b8") == 0) {
		size = sizeof(uint8_t) ;
	} else if (strcmp(type, "b16") == 0) {
		size = sizeof(uint16_t) ;
	} else if (strcmp(type, "b32") == 0) {
		size = sizeof(uint32_t) ;
	} else if (strcmp(type, "b64") == 0) {
		size = sizeof(uint64_t) ;
	} else if (strcmp(type, "string") == 0) {
		size = sizeof(char) ;

	} else {
		fprintf(stderr, "invalid type.\n"
		"Corrects values are among u8, s8, u16,"
		"s16, u32, s32, u64, s64 and float\n");
		exit(-1);
	}

	return size ;
}

int64_t cast_value(int64_t data, char *type)
{
	int64_t value = 0;

	/*Check data size according to type string. */
	if (strcmp(type, "u8") == 0) {
		value = (uint8_t) data ;
	} else if (strcmp(type, "s8") == 0) {
		value = (int8_t) data ;
	} else if (strcmp(type, "u16") == 0) {
		value = (uint16_t) data ;
	} else if (strcmp(type, "s16") == 0) {
		value = (int16_t) data ;
	} else if (strcmp(type, "u32") == 0) {
		value = (uint32_t) data ;
	} else if (strcmp(type, "s32") == 0) {
		value = (int32_t) data ;
	} else if (strcmp(type, "u64") == 0) {
		value = (uint64_t) data ;
	} else if (strcmp(type, "s64") == 0) {
		value = (int64_t) data ;
	} else if (strcmp(type, "float") == 0) {
		memcpy((float *)&value, (float*)&data, sizeof(float));
	} else if (strcmp(type, "b8") == 0) {
			value = (uint8_t) data ;
	} else if (strcmp(type, "b16") == 0) {
			value = (uint16_t) data ;
	} else if (strcmp(type, "b32") == 0) {
			value = (uint32_t) data ;
	} else if (strcmp(type, "b64") == 0) {
			value = (uint64_t) data ;
		} else {
		fprintf(stderr, "invalid type.\n\
				Corrects values are among u8, s8, u16,"
		"s16, u32, s32, u64, s64, float, b8, b16, b32, b64, string and float\n");
		exit(-1);
	}

	return value ;
}

int do_read(int fd, unsigned char *buffer, size_t *size)
{
    ssize_t readchr;
    size_t buf_size = *size;

	if (SIZE_MAX < buf_size) {
		fprintf(stderr, "buffer to large to be read\n");
		return -1;
	}

	*size = 0;
	do {
		readchr = read(fd, buffer + *size, buf_size - *size);

		if (readchr < 0) /* error on read */
			return -1;

		if (0 == readchr) /* end of file */
			return 0;

		*size += readchr;

	} while (buf_size > *size);

	return 0;
}

int do_write(int fd, unsigned char *buffer, size_t size)
{
	unsigned char *ptr = (unsigned char *)buffer;
	ssize_t wsize;
	do {
		wsize = write(fd, ptr, size);
		if (wsize < 0)
			return wsize;

		size -= wsize;
		ptr += wsize;

	} while (size > 0);

	return 0;
}


void writeData(int size, char *type)
{
	int retval ;
	int64_t value ;
	float fvalue;
	char in[32765];
	char *str, *tok;
	unsigned char buffer[4096];
	size_t buf_len = 0;

	str = in;
	fgets(in, 32765, stdin);
	do {
		tok = strtok(str, ";");
		str = NULL;
		if (tok) {
			/* debug */
			/*fprintf(stderr, "write input %s\n", tok);*/

			if (strcmp(type, "float") == 0) {
				retval = sscanf(tok, "%f", &fvalue);
				if (retval == EOF) {
					perror("dthfilter write float");
					abort();
				}

				/* debug */
				/*fprintf(stderr, "writing a float %f, \
				 * size=%d\n", fvalue, size);*/
				value = 0;
				memcpy((float *)(buffer + buf_len), \
						&fvalue, sizeof(float));
				buf_len += size;
			} else {
				retval = sscanf(tok, "%lld", &value);
				if (retval == EOF) {
					perror("dthfilter write integer");
					abort();
				}

				memcpy(buffer + buf_len, &value, size);
				buf_len += size;
			}
		}
	} while (tok);

	retval = do_write(1, buffer, buf_len);

	if (retval)
		perror("dthfilter write");
}

int
main(int argc, char **argv)
{
	int c, size;
	char *type = NULL;
	int op = 0;
	int get_at = -1;

	while ((c = getopt(argc, argv, "r:w:a:")) != -1) {
		switch (c) {
		case 'w':
			op = 1;
			type = optarg;
			break;

		case 'r':
			op = 2;
			type = optarg;
			break;

		case 'a':
			get_at = atoi(optarg);
			break;

		default:
			usage();
		}
	}

	switch (op) {
	case 1:
	{
		if (*type == '\0') {
			usage();
		} else {
			if (strcmp(type, "string") == 0) {
				fprintf(stderr, "String not managed in write mode.\n"
				"Use \"echo -n\" instead.\n");
				break;
			}
			size = get_data_size(type);
			writeData(size, type);
		}
	}
	break;

	case 2:
	{
		if (*type == '\0') {
			usage();
			break;
		}

		size_t buf_size = 4096; /* out: number of bytes read */
		unsigned char buffer[buf_size];
		unsigned int i=0;
		size = get_data_size(type);

		int retval = do_read(0, buffer, &buf_size);

		if (retval) {
			perror("error reading input file\n");
			break;
		}

		if (strcmp(type, "string") == 0) {
			if (get_at == -1 || (unsigned int)get_at == i) {
				buffer[buf_size]='\0';
				printf("%s", buffer);
			}
		}else{
			for (i = 0; i < buf_size/size; i++) {
				if ((i > 0) && (get_at == -1))
					printf(";");

				if (strcmp(type, "float") == 0) {
					if (get_at == -1 || (unsigned int)get_at == i)
						printf("%f", \
							*((float *)(buffer + (i * size))));

				}else {
					if (get_at == -1 || (unsigned int)get_at == i) {
						int64_t value = 0;
						memcpy(&value, \
							buffer + (i * size), size);
						printf("%lld", cast_value(value, type));
					}
				}
			}
		}
	}
		break;

	default:
		usage();
	}

	return 0 ;
}


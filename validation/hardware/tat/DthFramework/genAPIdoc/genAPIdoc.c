/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   API Documentation Generator
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <dthfs/dth_filesystem.h>
#include <dthsrvhelper/dthsrvhelper.h>

inline void usage()
{
	fprintf(stderr, "genAPIdoc: -v -h filename."
		"\n-v verbose level\n-h print this help\n");
	exit(-1);
}

int main(int argc, char **argv)
{
	int c, err = 0;
	char *s;
	char *api_doc_filename;
	int debuglevel = 0;

	while ((c = getopt(argc, argv, "v:h:")) != -1) {
		switch (c) {
		case 'v':
			debuglevel = strtol(optarg, &s, 10);
			if (*s != '\0')
				usage();
			break;
		case 'h':
		default:
			usage();
			exit(-1);
		}
	}

	if (optind >= argc) {
		printf("error no output file specified\n");
		exit(-1);
    }

	api_doc_filename = (char *)calloc(strlen(argv[optind]) + 1, sizeof(char));
	strcpy(api_doc_filename, argv[optind]);

	printf("loaddthsrvherlper write in :%s\n", api_doc_filename);

	err = genAPIdoc(api_doc_filename, debuglevel);

	exit(err);
}

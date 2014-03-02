/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This is the main Modem Audio Stub
* \author  ST-Ericsson
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE				"/dev/audio"
#define MAXDATA 			4096

/**
 * main function
 */
int main (void)
{
	int32_t fd, count;
	uint8_t data[MAXDATA];

	// initialisation
	fd = open(DEVICE, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Wrong device node %s:[%s]\n", DEVICE, strerror(errno));
		return(-1);
	}
	// main loop
	for(;;)
	{
		// read data
		bzero(data, sizeof(data));
		count = read(fd, data, MAXDATA-1);
/*
		if (count < 0) {
			fprintf(stderr, "Read failed:[%s]\n", strerror(errno));
			continue;
		}
		if (count == 0) {
			fprintf(stderr, "No data available\n");
		  continue;
		}
		fprintf(stderr, "Rx:[%s]\n", data);
*/
	}
	// normaly not reached !!
	close(fd);
	return(0);
}


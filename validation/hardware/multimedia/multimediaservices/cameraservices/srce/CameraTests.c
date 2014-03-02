/*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose Video function for tests (Display (Main, Sub, TV-OUT), Camera)
* \author  ST-Ericsson
*/
/* Not compiled*/
/*****************************************************************************/

#include <stdint.h>		/* uint32_t ... */
#include <stdio.h>		/* printf ... */
#include <string.h>		/* strlen ... */
#include <stdlib.h>		/* exit ... */

#include "CameraServices.h"

/*
	Service allowing to compare beginning of argument arg with a string s1
	return=0 if equal
*/
int32_t compareArgString(char *s1, char *arg)
{
	return strncmp(s1, arg, strlen(s1));
}

/*
	Print help
*/
void printHelp()
{
	printf("-h, --help:		help\n");
	printf("-m=#:  mode of test\n\
	(#=1): Camera Mode\n");
	printf("-s=#:  sensor Id\n\
	(#=%d): Primary sensor\n\
	(#=%d): Secondary sensor\n", SENSOR_PRIMARY, SENSOR_SECONDARY);

	exit(EXIT_SUCCESS);
}

int32_t main(int32_t argc, char *argv[])
{
	uint32_t vl_mode = 0xFF;
	uint32_t vl_SensorId = SENSOR_PRIMARY;	/* default value is primary sensor */

	/* A - parse parameters */
	if (argc > 0) {
		int32_t i;

		for (i = 1; i < argc; i++) {
			if ((compareArgString("-h", argv[i]) == 0)
			    || (compareArgString("--help", argv[i]) == 0)) {
				printHelp();
				continue;
			}
			if (compareArgString("-m=", argv[i]) == 0) {
				if (sscanf
				    (argv[i], "-m=%d",
				     (int32_t *)&vl_mode) == 1) {
					continue;
				}
			}
			if (compareArgString("-s=", argv[i]) == 0) {
				if (sscanf
				    (argv[i], "-s=%d",
				     (int32_t *)&(vl_SensorId)) == 1) {
					continue;
				}
			}
			/* parameter error */
			printf("parameter error: %s", argv[i]);
			exit(EXIT_FAILURE);
		}
	}

	/* if(argc > 0) */
	/* B - Call related Camera services functions */
	/* Camera Mode */
#ifdef CAMERA_ED_TEST
	if (vl_mode == 1) {
		e_CameraServ_SensorId_t SensorId = SENSOR_PRIMARY;
		t_CameraServ_SensorProperties SensorProperties;

		if (vl_SensorId == SENSOR_PRIMARY)
			SensorId = SENSOR_PRIMARY;
		else if (vl_SensorId == SENSOR_SECONDARY)
			SensorId = SENSOR_SECONDARY;
		else {
			printf("Error: incorrect sensor Id\n");
			exit(EXIT_FAILURE);
		}

		if (CameraServ_CameraSensorInit(SensorId) != 0)
			printf("CameraServ_CameraSensorInit KO\n");
		if (CameraServ_CameraSetSensorProp(&SensorId, &SensorProperties)
		    != 0)
			printf("CameraServ_CameraSetSensorProp KO\n");
		if (CameraServ_CameraPreviewStart() != 0)
			printf("CameraServ_CameraPreviewStart KO\n");
		/* Mount userfs2  for image storage */
		system("mount /mnt/UserFS2");
		system("rm /mnt/UserFS2/result1.bmp");
		if (CameraServ_CameraTakeAShot
		    ((int8_t *) "/mnt/UserFS2/result1.bmp", SINGLE_SHOT,
		     NULL) != 0)
			printf("CameraServ_CameraTakeAShot KO\n");
/* unmount SD card */
		system("umount /mnt/UserFS2");
	} else {
		printf("Error: please specify mode\n");
		exit(EXIT_FAILURE);
	}
#else
	printf("Error: please specify mode\n");
	exit(EXIT_FAILURE);
#endif

	exit(EXIT_SUCCESS);
}

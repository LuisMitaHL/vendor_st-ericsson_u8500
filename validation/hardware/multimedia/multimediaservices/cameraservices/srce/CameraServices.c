 /*****************************************************************************/
/**
*  Copyright (C) ST-Ericsson 2009
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Propose Video function for tests (Camera)
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdint.h>		/* uint32_t ... */
#include <stdio.h>		/* printf ... */
#include <linux/fb.h>		/* fb struct & var */
#include <fcntl.h>		/* O_RDWR */
#include <sys/mman.h>		/* mmap() ... */
#include <sys/stat.h>		/* mknod .. */
#include <unistd.h>		/* fork ... */
#include <string.h>		/* memcpy .. */
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>		/* malloc(), free() */

#include <signal.h>		/*kill ... */

#include <sys/ioctl.h>

#include "CameraServices.h"
#include <errno.h>

uint32_t fd_output_camera_pipe = 0;
uint32_t fd_input_camera_pipe = 0;
uint32_t fd_camera_output_MMTE_task_pipe = 0;

uint32_t CameraServ_InitPipe(char *input_pipe, char *output_pipe,
			     char *output_MMTE_task_pipe, int *fd_ip,
			     int *fd_op, int *fd_op_mmte_task, int pts)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	uint32_t fd_output;
	uint32_t fd_input;
	uint32_t fd_output_MMTE_task_pipe;;
	char command[100];
	FILE *fd_popen;

	errno = 0;
	if (mkfifo(input_pipe, 0666) == 0) {	/* creation tube avec droit lecture/ecriture */
		printf("ok");
	} else {
		if (errno == EEXIST)
			printf("this file (%s) exists already \n", input_pipe);
		else
			printf("Issue for the creation of %s", input_pipe);
	}
	fd_input = open(input_pipe, O_RDWR | O_NDELAY);
	if (fd_input < 0) {
		printf("error for the opening: %s", input_pipe);
		return CAMERASERV_FAILED;
	} else {
		fd_input_camera_pipe = *fd_ip = fd_input;
	}
	errno = 0;
	if (mkfifo(output_pipe, 0666) == 0) {	/* creation tube avec droit lecture/ecriture */
		printf("ok");
	} else {
		if (errno == EEXIST)
			printf("this file (%s) exists already \n", output_pipe);
		else
			printf("Issue for the creation of %s", output_pipe);
	}
	errno = 0;
	fd_output = open(output_pipe, O_RDWR | O_NDELAY);
	if (fd_output < 0) {
		printf("error for the opening: %s", output_pipe);
		return CAMERASERV_FAILED;
	} else {
		fd_output_camera_pipe = *fd_op = fd_output;
	}
	if (mkfifo(output_MMTE_task_pipe, 0666) == 0) {	/* creation tube avec droit lecture/ecriture */
		printf("ok");
	} else {
		if (errno == EEXIST)
			printf("this file (%s) exists already \n",
			       output_MMTE_task_pipe);
		else
			printf("Issue for the creation of %s",
			       output_MMTE_task_pipe);
	}
	errno = 0;
	fd_output_MMTE_task_pipe =
	    open(output_MMTE_task_pipe, O_RDWR | O_NDELAY);
	if (fd_output_MMTE_task_pipe < 0) {
		printf("error for the opening: %s", output_MMTE_task_pipe);
		return CAMERASERV_FAILED;
	} else {
		fd_camera_output_MMTE_task_pipe = *fd_op_mmte_task =
		    fd_output_MMTE_task_pipe;
	}
	printf("Initialization for the MMTE pipe finished\n");

	/*sprintf(command,"strace -f -c MmteOutputManagement %d",fd_output);*/
	sprintf(command, "MmteOutputManagement %d %d %s %d 1>%s 2>&1",
		fd_output, fd_output_MMTE_task_pipe, output_pipe, pts,
		output_MMTE_task_pipe);
	fd_popen = popen(command, "w");

	printf("MmteOutputManagement started\n");

	return vl_error;
}

void CameraServ_WaitAcknowledge(int fd, char *trigger, int timeout)
{
	char string[1024];
	ssize_t size = 0;

	fd_set rfds;
	struct timeval tv;
	int retval;
	int vl_timeout;

	usleep(30000);

	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);

	/* Wait up to timeout/1000 seconds. */
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = 0;
	memset(string, 0, sizeof(string));
	retval = select((fd + 1), &rfds, NULL, NULL, &tv);

	vl_timeout = ((int)(tv.tv_sec) * 1000) + ((int)(tv.tv_usec) / 1000);
	printf("timeout = %d/%d\n", vl_timeout, timeout);

	if (retval == -1)
		perror("select()");
	else if (retval) {
		size = read(fd, string, 34);
		if (strncmp(string, trigger, strlen(trigger)) == 0) {
			printf("mmte command/script executed: %s\n", trigger);
		} else
		    if (strncmp
			(string, "Error MMTE script detected ",
			 strlen("Error MMTE script detected ")) == 0) {
			printf("Error MMTE script detected\n");

			CameraServ_WaitAcknowledge(fd, trigger, vl_timeout);
		} else
		    if (strncmp
			(string, "Error MMTE command detected",
			 strlen("Error MMTE command detected")) == 0) {
			printf("Error MMTE command detected\n");
			CameraServ_WaitAcknowledge(fd, trigger, vl_timeout);
		} else
		    if (strncmp
			(string, "Timeout event detected     ",
			 strlen("Timeout event detected     ")) == 0) {
			printf("Timeout event detected\n");
			CameraServ_WaitAcknowledge(fd, trigger, vl_timeout);
		} else {
			printf("Unexpected Acknowledge: %s\n", string);
			CameraServ_WaitAcknowledge(fd, trigger, vl_timeout);
		}
	} else {
		printf("Mmte command timeout\n");
	}
	usleep(30000);
}

uint32_t CameraServ_SetMmteAlias(int fd_in, int fd_out, char *mmte_command,
				 int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine, mmte_command);
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, "Command executed", timeout);
	return vl_error;
}

uint32_t CameraServ_InitMmte(int fd_in, int fd_out, char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine, "do /usr/share/mm-valid/imaging/init_kanna.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_Init2Mmte(int fd_in, int fd_out, char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine, "do /usr/share/mm-valid/imaging/init_aptina.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StartPreview(int fd_in, int fd_out, char *trigger,
				 int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/start_preview.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StartIdle(int fd_in, int fd_out, char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/start_idle.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StartPause(int fd_in, int fd_out, char *trigger,
			       int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/start_pause.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StartResume(int fd_in, int fd_out, char *trigger,
				int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/start_running.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StartStillPictureCapture(int fd_in, int fd_out,
					     char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/still_picture_capture.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StartRecord(int fd_in, int fd_out, char *trigger,
				int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/start_record.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StopRecord(int fd_in, int fd_out, char *trigger,
			       int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/stop_record.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StartStillBurstPictureCapture(int fd_in, int fd_out,
						  char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/still_burst_picture_capture.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StopStillBurstPictureCapture(int fd_in, int fd_out,
						 char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/stop_still_burst_picture_capture.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StopIdle(int fd_in, int fd_out, char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/start_idle_to_init.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StopPreview(int fd_in, int fd_out, char *trigger,
				int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine,
	       "do /usr/share/mm-valid/imaging/ite_omx_use_cases/imaging_scripts/intern_usecases/start_executing_to_idle.ite\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StartMmte(char *pipe_input, char *pipe_output, int fd_out,
			      char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char command[200];

	printf("Starting MMTE\n");

	system("rm -f /tmp/mmte_in_background.sh");
	system
	    ("echo \"export OMX_BELLAGIO_REGISTRY=/system/omxcomponents\" > /tmp/mmte_in_background.sh");
	sprintf(command,
		"echo \"mmte_bellagio 0<%s 1>%s 2>&1\" >> /tmp/mmte_in_background.sh",
		pipe_input, pipe_output);
	system(command);
	system("chmod 777 /tmp/mmte_in_background.sh &");
	system("/tmp/mmte_in_background.sh &");

	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_StopMmte(int fd_in, int fd_out, char *trigger, int timeout)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	char chaine[200];

	strcpy(chaine, "quit\n");
	printf("Write a buffer of %d bytes with text %s\n", strlen(chaine),
	       chaine);
	write(fd_in, chaine, strlen(chaine));
	CameraServ_WaitAcknowledge(fd_out, trigger, timeout);

	return vl_error;
}

uint32_t CameraServ_CloseFileDescriptors(void)
{
	uint32_t vl_error = CAMERASERV_NO_ERROR;

	close(fd_output_camera_pipe);
	close(fd_input_camera_pipe);
	close(fd_camera_output_MMTE_task_pipe);

	return vl_error;
}

/*
 * CameraServ_CameraSensorInit - Service allowing to initialize a sensor
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraSensorInit(e_CameraServ_SensorId_t SensorId
				     __attribute__ ((unused)))
{
#ifdef CAMERA_ED_TEST
#define CAMERA_ED_MINOR 0
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	uint8_t camera_major = 0;
	char camera_major_string[50];

	/* find major for camera */
	unsigned char command[200];
	FILE *file_ptr;

	/* If the node is already created, don't try to create it again */
	file_ptr = fopen(CAMERA_DEV, "r");
	if (file_ptr != NULL) {
		/* node is already created */
		fclose(file_ptr);
		return CAMERASERV_NO_ERROR;
	}

/* cat /proc/devices | grep camera >/tmp/CamDeviceMajor.txt */
	snprintf((char *)command, 200,
		 "cat /proc/devices | grep cameraTests >/tmp/CamDeviceMajor.txt");
	printf("\n%s\n", command);
	system((char *)command);
	file_ptr = fopen("/tmp/CamDeviceMajor.txt", "r");
	if (file_ptr == NULL) {
		/* no file means no error */
		printf("\nFILE not found : ");
		return 1;
	} else {
		/*fscanf(file_ptr,"%d\n",&camera_major); */
		fgets(camera_major_string, 50, file_ptr);
		camera_major = (uint8_t) strtol(camera_major_string, NULL, 10);
		printf("\ncamera_major : %d", camera_major);
		fclose(file_ptr);
	}
	/* clear the status error file */
	system("rm -f /tmp/CamDeviceMajor.txt");

	/* create node => mknod /dev/fbX c Major Minor */
	if (mknod(CAMERA_DEV, S_IFCHR, (camera_major << 8) + CAMERA_ED_MINOR) <
	    0) {
		printf
		    ("CameraServ_CameraSensorInit: Error during mknod operation\n");
		vl_error = CAMERASERV_FAILED;
		goto error;
	}

	return CAMERASERV_NO_ERROR;

error:
	return vl_error;
#else
	return CAMERASERV_NOT_AVAILABLE;
#endif /*CAMERA_ED_TEST */

}

/*
 * CameraServ_CameraSetSensorProp - Service allowing to set the properties of the sensor for camera use cases
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraSetSensorProp(e_CameraServ_SensorId_t *SensorId
					__attribute__ ((unused)),
					t_CameraServ_SensorProperties *
					SensorProperties
					__attribute__ ((unused)))
{
#ifdef CAMERA_ED_TEST
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	FILE *pl_tmp_fd;

	/* Save the sensor Id */
	char al_TmpFile[30] = "/tmp/tmp.Camera";

	pl_tmp_fd = fopen(al_TmpFile, "w");
	if (pl_tmp_fd != NULL) {
		fwrite(SensorId, 1, sizeof(e_CameraServ_SensorId_t), pl_tmp_fd);
		fclose(pl_tmp_fd);
	} else {
		vl_error = CAMERASERV_FAILED;
		goto error;
	}

	return CAMERASERV_NO_ERROR;

error:
	return vl_error;
#else
	return CAMERASERV_NOT_AVAILABLE;
#endif /*CAMERA_ED_TEST */
}

/*
 * CameraServ_CameraPreviewStart - Service allowing to start the preview
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraPreviewStart(void)
{
#ifdef CAMERA_ED_TEST
	int32_t fb_filedesc;
	uint32_t vl_result;
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	FILE *pl_tmp_fd;

	/* Get the sensor Id */
	char al_TmpFile[30] = "/tmp/tmp.Camera";

	pl_tmp_fd = fopen(al_TmpFile, "r");
	if (pl_tmp_fd != NULL) {
		fread(&vl_result, 1, sizeof(e_CameraServ_SensorId_t),
		      pl_tmp_fd);
		fclose(pl_tmp_fd);
	} else {
		vl_error = CAMERASERV_FAILED;
		printf("CameraServ_CameraPreviewStart open al_TmpFile KO\n");
		goto error;
	}

	/* start the preview */
	fb_filedesc = open(CAMERA_DEV, O_RDWR);
	if (fb_filedesc <= 0) {
		vl_error = CAMERASERV_FAILED;
		printf("CameraServ_CameraPreviewStart open device KO\n");
		goto error;
	}

	if (ioctl(fb_filedesc, CAMERA_LAUNCH_PREVIEW, &vl_result) < 0) {
		vl_error = CAMERASERV_FAILED;
		printf
		    ("CameraServ_CameraPreviewStart ioctl CAMERA_LAUNCH_PREVIEW KO\n");
		goto out_free;
	}

	if (vl_result != 0) {
		vl_error = CAMERASERV_FAILED;
		goto out_free;
	}

	close(fb_filedesc);

	return CAMERASERV_NO_ERROR;

out_free:
	/* close frame buffer  Access */
	close(fb_filedesc);
error:
	return vl_error;

#else
	return CAMERASERV_NOT_AVAILABLE;
#endif /*CAMERA_ED_TEST */
}

/*
 * CameraServ_CameraTakeAShot - Service allowing to take a picture
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraTakeAShot(int8_t *outputFilePath
				    __attribute__ ((unused)),
				    e_CameraServ_ShotMode_t ShotMode
				    __attribute__ ((unused)),
				    t_CameraServ_BurstProperties *
				    BurstProperties __attribute__ ((unused)))
{
#ifdef CAMERA_ED_TEST
	uint32_t vl_result;
	uint32_t vl_sensorId;
	uint32_t vl_error = CAMERASERV_NO_ERROR;
	int32_t fb_filedesc;
	uint32_t *p_mmap_buffer = NULL;
	uint32_t sizex, sizey, nb_of_embeddeddatalane;
	FILE *pl_tmp_fd;

	/* Get the sensor Id */
	char al_TmpFile[30] = "/tmp/tmp.Camera";

	pl_tmp_fd = fopen(al_TmpFile, "r");
	if (pl_tmp_fd != NULL) {
		fread(&vl_sensorId, 1, sizeof(e_CameraServ_SensorId_t),
		      pl_tmp_fd);
		fclose(pl_tmp_fd);
	} else {
		vl_error = CAMERASERV_FAILED;
		printf("CameraServ_CameraPreviewStart open al_TmpFile KO\n");
		goto error;
	}

	if (vl_sensorId == SENSOR_PRIMARY) {
		sizex = 3264 /*3280 */ ;	/* 3280 size should be aligned to 24 => 3264 */
		sizey = 2448 /*2464 */ ;	/* 2464 size should be aligned to 24 => 2448 */
		nb_of_embeddeddatalane = 2;
	} else {
		sizex = 624 /*640 */ ;	/* 640 size should be aligned to 24 => 624 */
		sizey = 480;
		nb_of_embeddeddatalane = 3;
	}

	/* Take the picture */
	fb_filedesc = open(CAMERA_DEV, O_RDWR);
	if (fb_filedesc < 0) {
		vl_error = CAMERASERV_FAILED;
		goto error;
	}

	if (ioctl(fb_filedesc, CAMERA_TAKE_PICTURE, &vl_result) < 0) {
		vl_error = CAMERASERV_FAILED;
		goto out_free;
	}

	p_mmap_buffer =
	     mmap(NULL,
		  sizex * (sizey + nb_of_embeddeddatalane +
			   1) /* Size to remap */ , PROT_READ | PROT_WRITE,
		  MAP_SHARED,
		  fb_filedesc, 0);
	if (p_mmap_buffer == NULL) {
		vl_error = CAMERASERV_FAILED;
		goto out_free;
	}

/* Write image to SD card */
	ITE_StoreinBMPFile((char *)outputFilePath, (uint32_t) p_mmap_buffer,
			   sizex, sizey, nb_of_embeddeddatalane);

	if (vl_result != 0) {
		vl_error = CAMERASERV_FAILED;
		goto out_free;
	}

	close(fb_filedesc);

	return CAMERASERV_NO_ERROR;

out_free:
	/* close frame buffer  Access */
	close(fb_filedesc);
error:
	return vl_error;

#else
	return CAMERASERV_NOT_AVAILABLE;
#endif /*CAMERA_ED_TEST */
}

/*
 * CameraServ_CameraStopShot - Service allowing to stop the infinite burst shot
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraStopShot(void)
{
	/* Stub only */
	return CAMERASERV_NOT_AVAILABLE;
}

/*
 * CameraServ_CameraSetPreviewProp - Service allowing to set the properties of the preview for camera use cases
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraSetPreviewProp(uint32_t *DisplayId
					 __attribute__ ((unused)),
					 t_CameraServ_PreviewResolution *
					 PreviewResolution
					 __attribute__ ((unused)))
{
	return CAMERASERV_NOT_AVAILABLE;
}

/*
 * CameraServ_CameraGetPreviewProp - Service allowing to get the properties of the preview for camera use cases
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraGetPreviewProp(uint32_t *DisplayId
					 __attribute__ ((unused)),
					 t_CameraServ_PreviewResolution *
					 PreviewResolution
					 __attribute__ ((unused)))
{
	return CAMERASERV_NOT_AVAILABLE;
}

/*
 * CameraServ_CameraSetPictureProp - Service allowing to set the properties of the picture to take for camera use cases
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraSetPictureProp(uint32_t *PictureResolution
					 __attribute__ ((unused)),
					 uint32_t *PictureFormat
					 __attribute__ ((unused)))
{
	return CAMERASERV_NOT_AVAILABLE;
}

/*
 * CameraServ_CameraGetPictureProp - Service allowing to get the properties of the picture for camera use cases
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraGetPictureProp(uint32_t *PictureResolution
					 __attribute__ ((unused)),
					 uint32_t *PictureFormat
					 __attribute__ ((unused)))
{
	return CAMERASERV_NOT_AVAILABLE;
}

/*
 * CameraServ_CameraGetSensorProp - Service allowing to get the properties of the sensor for camera use cases
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraGetSensorProp(e_CameraServ_SensorId_t *SensorId
					__attribute__ ((unused)),
					t_CameraServ_SensorProperties *
					SensorProperties
					__attribute__ ((unused)),
					uint32_t *MaxSensorResolution
					__attribute__ ((unused)))
{
	return CAMERASERV_NOT_AVAILABLE;
}

/*
 * CameraServ_CameraSetSensorZoom - Service allowing to set the zoom of the sensor for camera use cases
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraSetSensorZoom(t_CameraServ_ZoomProperties *
					ZoomPropertiesi
					__attribute__ ((unused)))
{
	return CAMERASERV_NOT_AVAILABLE;
}

/*
 * CameraServ_CameraGetSensorZoom - Service allowing to get the zoom of the sensor for camera use cases
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraGetSensorZoom(t_CameraServ_ZoomProperties *
					ZoomProperties __attribute__ ((unused)))
{
	return CAMERASERV_NOT_AVAILABLE;
}

/*
 * CameraServ_CameraPreviewStop - Service allowing to stop the preview
 *
 * refer to VideoServices.h file for parameters, return, notes and warnings
 *
 * */
uint32_t CameraServ_CameraPreviewStop(void)
{
	return CAMERASERV_NOT_AVAILABLE;
}

#ifdef CAMERA_ED_TEST
uint8_t ITE_StoreinBMPFile(char *filename, uint32_t addr_buffer, uint16_t sizex,
			   uint16_t sizey, uint16_t nb_of_embedded_line)
{
	uint16_t header[1];
	uint32_t file_header[3];
	uint32_t pict_header[10];
	uint8_t pel[3];
	uint32_t RedAddr, BlueAddr, GreenBAddr, GreenRAddr;
	uint32_t i, j;
	uint8_t *pl_TemporaryImagePointer;
	uint8_t *pl_TakenBMPImage;
	FILE *fp;

	pl_TakenBMPImage =
	    (uint8_t *) malloc(sizex * sizey * 3 + 40 + 14/*,GFP_KERNEL */);
	pl_TemporaryImagePointer = pl_TakenBMPImage;
	/*write bmp file header*/
	header[0] = 0x4d42;	/* Bitmap Windows */
	memcpy(pl_TemporaryImagePointer, (uint8_t *) (header), 2);

	pl_TemporaryImagePointer = pl_TemporaryImagePointer + 2;

	file_header[0] = (sizex * sizey * 3) + 40 + 14;	/* total size of file */
	file_header[1] = 0;	/* reserved */
	file_header[2] = 40 + 14;	/* picture offset 40+14 ou 14???? */
	memcpy(pl_TemporaryImagePointer, (uint8_t *) (file_header), 3 * 4);

	pl_TemporaryImagePointer = pl_TemporaryImagePointer + 12;

	pict_header[0] = 40;	/* Picture header size */
	pict_header[1] = sizex;
	pict_header[2] = sizey;
	pict_header[3] = 0x00180001;	/* color coding 24bit/ plan */
	pict_header[4] = 0;	/* compression methode 0: no compression */
	pict_header[5] = sizex * sizey * 3;	/* Picture total size */
	pict_header[6] = 0x0ec4;	/* horiz resolution */
	pict_header[7] = 0x0ec4;	/* vertical resolution */
	pict_header[8] = 0;	/* palette nb of color */
	pict_header[9] = 0;	/* palette nb of important color */
	memcpy(pl_TemporaryImagePointer, (uint8_t *) (pict_header), 10 * 4);

	pl_TemporaryImagePointer = pl_TemporaryImagePointer + 40;

	for (j = 0; j < sizey; j++) {
		for (i = 0; i < sizex; i++) {
			/* assuming B-Gb / Gr-R */
			/*
			   BlueAddr = addr_buffer + (2*sizex) + ((j&0xfffe)*sizex) + (i & 0xFFFE);
			   RedAddr = addr_buffer + (2*sizex) + ((j|0x1)*sizex) + (i|0x1);
			   GreenBAddr = addr_buffer + (2*sizex) + ((j&0xfffe)*sizex) + (i|0x1);
			   GreenRAddr = addr_buffer + (2*sizex) + ((j|0x1)*sizex) + (i&0xfffe);
			 */
			/* assuming Gr-R / B-Gb */

			GreenRAddr =
			    addr_buffer + (nb_of_embedded_line * sizex) +
			    ((j & 0xfffe) * sizex) + (i & 0xFFFE);
			GreenBAddr =
			    addr_buffer + (nb_of_embedded_line * sizex) +
			    ((j | 0x1) * sizex) + (i | 0x1);
			RedAddr =
			    addr_buffer + (nb_of_embedded_line * sizex) +
			    ((j & 0xfffe) * sizex) + (i | 0x1);
			BlueAddr =
			    addr_buffer + (nb_of_embedded_line * sizex) +
			    ((j | 0x1) * sizex) + (i & 0xfffe);

			pel[2] = (*((volatile unsigned char *)(RedAddr)));	/*color.RedValue*/
			pel[1] = ((*((volatile unsigned char *)(GreenRAddr))) + (*((volatile unsigned char *)(GreenBAddr)))) / 2;	/*color.GreenValue;*/
			pel[0] = (*((volatile unsigned char *)(BlueAddr)));	/*color.BlueValue;*/
			memcpy(pl_TemporaryImagePointer, (uint8_t *) (pel), 3);
			pl_TemporaryImagePointer = pl_TemporaryImagePointer + 3;

		}
	}

	fp = fopen(filename, "wb");
	if (fp != NULL) {
		fwrite(pl_TakenBMPImage, 1, sizex * sizey * 3 + 40 + 14, fp);
		fclose(fp);
	}

	free(pl_TakenBMPImage);

	return 0;
}
#endif /*CAMERA_ED_TEST */

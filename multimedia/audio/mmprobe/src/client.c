/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>

#define LOG_TAG "MMPROBE_CLIENT"
#include <cutils/log.h>

#include "../include/utils.h"
#include "../include/common.h"
#include "../include/client.h"
#include "../api/r_mm_probe.h"
#include "../api/t_mm_probe.h"

#include <sys/mman.h>

extern void MyInitFunc(void) __attribute__ ((constructor));
extern void MyTermFunc(void) __attribute__ ((destructor));

static char* probe_list;
static void* file_memory;

static unsigned int seqNo = 0;
static int probe_list_opened = 0;
static int initialized = 0;
//Fill in default data since DataFormat_t does not exist in V1
static DataFormat_t defaultFormat = {MM_PROBE_SAMPLE_RATE_UNKNOWN, 1, MM_PROBE_INTERLEAVED_NONE, MM_PROBE_FORMAT_S16_LE};

int open_probe_list(void);

/*********************************************************************************/
void MyInitFunc(void)
{
  if (!initialized) {

    open_probe_list();

    initialized = 1;
  }
}

/*********************************************************************************/
void MyTermFunc(void)
{
  munmap (file_memory, MM_PROBE_MAX_NUMBER_PROBES);
}

/*********************************************************************************/
int open_probe_list(void) {

  int file, retval = 0;
  struct stat fileinfo;

  // Check if the list exists
  if ((retval = stat(LIST_NAME_PATH, &fileinfo)) < 0) {
      return retval;
  }

  /* Open the file. */
  file = open (LIST_NAME_PATH, O_RDWR, S_IRUSR | S_IWUSR);

  /* Create the memory mapping. */
  file_memory = mmap (0, MM_PROBE_MAX_NUMBER_PROBES, PROT_READ | PROT_WRITE, MAP_SHARED, file, 0);

  probe_list = (char*)file_memory;

  close (file);

  probe_list_opened = 1;

  return retval;
}

/*********************************************************************************/
probe_t* create_probe(int id){
  struct timeval timestamp;
  probe_t* p = (probe_t*)malloc(sizeof(probe_t));
  p->Header.Size = 0;
  p->Header.ProbeId   = id;
  p->Data_p  = malloc(1);
  p->Header.SeqNo = 0; //dummy value here needs to be set as close to write as possible

  gettimeofday(&timestamp, NULL);

  int milli_seconds = (int)(timestamp.tv_usec/1000);
  int seconds = (int)timestamp.tv_sec;

  p->Header.TimeStamp = (seconds << 16) | (0x0000FFFF & milli_seconds);

  return p;
}

/*********************************************************************************/
void delete_probe(probe_t* p){
  free(p->Data_p);
  free(p);
}

/*********************************************************************************/
int add_data_to_probe(probe_t* p, void* data, int length, DataFormat_t* format_p){
  int size;
  char* tmp;


  size = length * sizeof(char);

  tmp = malloc(p->Header.Size + size);
  memcpy(tmp, p->Data_p, p->Header.Size);

  memcpy(&tmp[p->Header.Size], data, size);
  p->Header.Size += size;

  free(p->Data_p);

  p->Data_p = tmp;

  p->Header.DataFormat = *format_p;

  return size;
}

/*********************************************************************************/
int send_probe(probe_t* p){
  int retval    = 0;
  int fd        = -1;
  char *buffer;
  int probeSize = p->Header.Size + sizeof(probeHeader_t);

  struct stat fileinfo;

  if (probeSize > MM_PROBE_MAX_DATA_SEND_SIZE) {
    errno = EOVERFLOW;
    return retval;
  }

  // Change to server working directory.
  if ((retval = chdir(RUNNING_DIR)) < 0) {
    return retval;
  }

  // Check if the pipe exists
  if ((retval = stat(FIFO_NAME, &fileinfo)) < 0) {
    return retval;
  }

  // Open for writing
  if ((fd = open(FIFO_NAME, O_WRONLY|O_NONBLOCK)) < 0) {
    return fd;
  }

  buffer = malloc(probeSize);
  p->Header.SeqNo = seqNo++; //value here needs to be set as close to write as possible

  // copy header
  memcpy(buffer, p, sizeof(probeHeader_t));
  // copy payload
  memcpy(buffer + sizeof(probeHeader_t), p->Data_p, p->Header.Size);

  retval = write(fd, buffer, probeSize);

  if (retval < 0) {
    ALOGE("Write to pipe returned %d %s: probe %d content cannot be trusted", retval, strerror(errno), p->Header.ProbeId);
  }

  close(fd);
  free(buffer);

  return retval < 0 ? retval : 0;
}

/* public api */
/*********************************************************************************/
int mmprobe_status(int id) {
  int retval = 0;

  if (id > MM_PROBE_NUMBER_OF_SPEECH_PROBES)
    return 0;

  if (!probe_list_opened)
    retval = open_probe_list();

  return retval < 0 ? 0 : probe_list[id-1];
}

/*********************************************************************************/
int mmprobe_probe(int id, void* data, int length){
  return mmprobe_probe_V2(id, data, length, &defaultFormat);
}

/*********************************************************************************/
int mmprobe_probe_V2(int id, void* data, int length, DataFormat_t* format_p){
  int retval = 0;

  if (!probe_list_opened) {
    if (open_probe_list() < 0)
      return -1;
  }

  if ((id > MM_PROBE_NUMBER_OF_SPEECH_PROBES) || (probe_list[id-1] == 1)) {
    probe_t* p = create_probe(id);

    add_data_to_probe(p, data, length, format_p);

    retval = send_probe(p);

    delete_probe(p);
  }

  return retval;
}

/* end public api */

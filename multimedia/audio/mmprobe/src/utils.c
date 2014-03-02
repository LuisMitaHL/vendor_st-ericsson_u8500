/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "../include/utils.h"
#include "../include/logger.h"

typedef struct {
  unsigned int   ChunkId;
  unsigned int   ChunkSize;
  unsigned int   Format;
  unsigned int   SubChunk1Id;
  unsigned int   SubChunk1Size;
  unsigned short AudioFormat;
  unsigned short NumChannels;
  unsigned int   SampleRate;
  unsigned int   AvgBytesPerSec;
  unsigned short BlockAlign;
  unsigned short BitsPerSample;
  unsigned int   SubChunk2Id;
  unsigned int   SubChunk2Size;
} wav_header_t;

/*********************************************************************************/
char* itoa(int val, int base) {

  static char buf[32] = {0};
  int i = 30;

  for(; val && i ; --i, val /= base) {
    buf[i] = "0123456789abcdef"[val % base];
  }

  return &buf[i+1];
}

/*********************************************************************************/
int trim(char * trimstring, const char * what) {
  int i = 0;
  int j = 0;

  char newstring[strlen(trimstring)];
  if (strstr(trimstring, what) != NULL) {
    i = strlen(what);
  }
  for (; trimstring[i]; i++, j++) {
    newstring[j] = trimstring[i];
  }
  newstring[j] = '\0';

  strcpy(trimstring, newstring);

  return 0;
}

/*********************************************************************************/
pid_t pidof(const char *p_processname, pid_t excludepid) {
  DIR *dir_p;
  struct dirent *dir_entry_p;
  char dir_name[40];
  char target_name[252];
  int target_result;
  char exe_link[252];
  pid_t result = -1;

  dir_p = opendir("/proc/");
  while (NULL != (dir_entry_p = readdir(dir_p))) {
    if (strspn(dir_entry_p->d_name, "0123456789") == strlen(dir_entry_p->d_name)) {
      strcpy(dir_name, "/proc/");
      strcat(dir_name, dir_entry_p->d_name);
      strcat(dir_name, "/");
      exe_link[0] = 0;
      strcat(exe_link, dir_name);
      strcat(exe_link, "exe");
      target_result = readlink(exe_link, target_name, sizeof(target_name)-1);

      if (target_result > 0) {
        target_name[target_result] = 0;
        if (strstr(target_name, p_processname) != NULL) {
          result = (pid_t) atoi(dir_entry_p->d_name);

          // We want to exclude ourselves when searching for the pid.
          if (result != excludepid) {
            closedir(dir_p);
            return result;
          }
          else {
            result = -1;
          }
        }
      }
    }
  }
  closedir(dir_p);
  return result;
}

/* returns the sample width (in bytes) */
unsigned int getSampleResolution(MM_ProbeResolution_t resolution)
{
  switch(resolution) {
  case MM_PROBE_FORMAT_S16_LE:
  case MM_PROBE_FORMAT_S16_BE:
  case MM_PROBE_FORMAT_U16_LE:
  case MM_PROBE_FORMAT_U16_BE:
    return 2;
  case MM_PROBE_PCM_FORMAT_S24_LE:
  case MM_PROBE_PCM_FORMAT_S24_BE:
  case MM_PROBE_FORMAT_S32_LE:
  case MM_PROBE_FORMAT_S32_BE:
  case MM_PROBE_FORMAT_U32_LE:
  case MM_PROBE_FORMAT_U32_BE:
    return 4;
  case MM_PROBE_PCM_FORMAT_UNKNOWN:
  default:
    return 0;
  }
}

unsigned int getSampleValidResolution(MM_ProbeResolution_t resolution)
{
  if ((resolution == MM_PROBE_PCM_FORMAT_S24_LE) ||
      (resolution == MM_PROBE_PCM_FORMAT_S24_BE)) {
    return 3;
  } else {
    return getSampleResolution(resolution);
  }
}

unsigned int getSampleRate(MM_ProbeSampleRate_t samplerate)
{
    switch (samplerate) {
    case MM_PROBE_SAMPLE_RATE_192KHZ:
      return 192000;
    case MM_PROBE_SAMPLE_RATE_176_4KHZ:
      return 176400;
    case MM_PROBE_SAMPLE_RATE_128KHZ:
      return 128000;
    case MM_PROBE_SAMPLE_RATE_96KHZ:
      return 96000;
    case MM_PROBE_SAMPLE_RATE_88_2KHZ:
      return 88200;
    case MM_PROBE_SAMPLE_RATE_64KHZ:
      return 64000;
    case MM_PROBE_SAMPLE_RATE_48KHZ:
      return 48000;
    case MM_PROBE_SAMPLE_RATE_44_1KHZ:
      return 44100;
    case MM_PROBE_SAMPLE_RATE_32KHZ:
      return 32000;
    case MM_PROBE_SAMPLE_RATE_24KHZ:
      return 24000;
    case MM_PROBE_SAMPLE_RATE_22_05KHZ:
      return 22050;
    case MM_PROBE_SAMPLE_RATE_16KHZ:
      return 16000;
    case MM_PROBE_SAMPLE_RATE_12KHZ:
      return 12000;
    case MM_PROBE_SAMPLE_RATE_11_025KHZ:
      return 11025;
    case MM_PROBE_SAMPLE_RATE_8KHZ:
      return 8000;
    case MM_PROBE_SAMPLE_RATE_7_2KHZ:
      return 7200;
    case MM_PROBE_SAMPLE_RATE_UNKNOWN:
    default:
      return 0;
    }
}

void writeWavHeader(DataFormat_t* format, FILE* fp)
{
    wav_header_t waveHeader;
    unsigned int sampleRes = getSampleResolution(format->Resolution); // this is the size of a sample (valid bits can be less)
    memset(&waveHeader, 0, sizeof(wav_header_t));
    waveHeader.ChunkId = 0x46464952; /* "RIFF" */
    waveHeader.ChunkSize = 36; // need to add the number of samples when closing the file
    waveHeader.Format = 0x45564157; // "WAVE"
    waveHeader.SubChunk1Id = 0x20746d66; // "fmt"
    waveHeader.SubChunk1Size = 16;
    waveHeader.AudioFormat = 1; // PCM
    waveHeader.NumChannels = format->NoChannels;
    waveHeader.SampleRate = getSampleRate(format->SampleRate);
    waveHeader.AvgBytesPerSec = waveHeader.SampleRate * format->NoChannels * sampleRes;
    waveHeader.BlockAlign = sampleRes * format->NoChannels;
    waveHeader.BitsPerSample = 8 * getSampleValidResolution(format->Resolution);
    waveHeader.SubChunk2Id = 0x61746164; //"data"
    waveHeader.SubChunk2Size = sampleRes * format->NoChannels; // needs to be updated when closing the file
    // write wav header
    fwrite(&waveHeader, 1, sizeof(wav_header_t), fp);
}

void updateWavHeader(FILE* fp)
{
    wav_header_t waveHeader;
    fpos_t pos;
    // got to the file start
    fseek(fp, 0, SEEK_SET);
    if (fread(&waveHeader, sizeof(wav_header_t), 1, fp) != 1)
    {
        log_message(L_ERR, "Error when reading wave header %s", strerror(errno));
    }

    fseek(fp, 0, SEEK_END);

    fgetpos(fp, &pos);

    waveHeader.ChunkSize = pos - 8;
    waveHeader.SubChunk2Size = waveHeader.ChunkSize - 36;

    fseek(fp, 0, SEEK_SET);

    // write wav header
    if (fwrite(&waveHeader, sizeof(wav_header_t), 1, fp) != 1)
    {
        log_message(L_ERR, "Error when writing wave header %s", strerror(errno));
    }
}
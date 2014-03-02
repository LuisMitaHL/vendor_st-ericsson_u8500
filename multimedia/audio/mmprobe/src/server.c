/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include "../include/server.h"
#include "../include/logger.h"
#include "../include/utils.h"
#include "../include/common.h"
#include "../include/client.h"
#include "../api/t_mm_probe.h"
#ifndef  X86
#include "ste_adm_client.h"
#endif

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <string.h>

#include <unistd.h>

// Set to true when a client is connected.
static boolean connected = false;
static boolean client_terminated = true;

static int enabled_speech_probes;
static int enabled_device_probes;

static int file;
static char* probe_list;
static void* file_memory;
static int nbr_of_subscribers;

static int subscriber_list[MM_PROBE_MAX_NUMBER_SUBSCRIBERS];
static int server_client_suspended = 0;
#define SOCK_PATH "mmprobed_socket"

//#define MODULE_TEST 0
#define MMPROBE_VERSION_STING_MAX_LEN 128
#define MMPROBE_PLATFORM_PROP_PATH    "/system/build.prop"
#define MMPROBE_SHORT_MESSAGE_SIZE 6

typedef struct{
        int logDecoderFormat;
        int clientfd;
        char* enabledProbes;
}threadInputData_t;

//Used for device probes (ADM/OMX)
typedef struct {
        int pid;
        FILE* file;
        char formatWritten;
}deviceProbeFiles_t;
static deviceProbeFiles_t deviceProbeList[MM_PROBE_MAX_NUMBER_ADM_PROBES];

void enable_probe(int pid);
void disable_probe(int pid);
void disable_all_probes();


/*********************************************************************************/
static void MMPROBE_GetVersionString(char *const VersionString_p)
{
  char ReadLine[1024];
  char CheckStr_User[20]      = {"ro.build.user=\0"};   /* Size 14 */
  char CheckStr_Model[20]     = {"ro.product.name=\0"}; /* Size 16 */
  char CheckStr_BuildTime[20] = {"ro.build.date=\0"};   /* Size 14 */
  char User[1024]      = {"\0"};
  char Model[1024]     = {"\0"};
  char BuildTime[1024] = {"\0"};

  int  UserStringLen = 0;
  int  ModelStringLen = 0;
  int  BuildTimeStringLen = 0;

  FILE *PropFile_p;

  /* Open file */
  PropFile_p = fopen(MMPROBE_PLATFORM_PROP_PATH, "r");

  /* Check if file exists */
  if(PropFile_p == NULL)
  {
    log_message(L_INFO, "Error Opening Property file: %s", MMPROBE_PLATFORM_PROP_PATH);
    return;
  }

  /* Parse through properties file, TODO: Use Regedit */
  while(!feof(PropFile_p) )
  {
    (void) fgets(ReadLine, 1024, PropFile_p);

    /* Check for User */
    if (strncmp(ReadLine, CheckStr_User, 14) == 0)
    {
      strcpy(User, &ReadLine[14]);
      UserStringLen  = strlen(User) - 1; //Remove Newline
    }

    /* Check for Modelname */
    if (strncmp(ReadLine, CheckStr_Model, 16) == 0)
    {
      strcpy(Model, &ReadLine[16]);
      ModelStringLen  = strlen(Model) - 1; //Remove Newline
    }

    /* Check for Buildtime, */
    if (strncmp(ReadLine, CheckStr_BuildTime, 14) == 0)
    {
      strcpy(BuildTime, &ReadLine[14]);
      BuildTimeStringLen  = strlen(BuildTime) - 1; //Remove Newline

    }
  }
  /* Close file */
  fclose(PropFile_p);

  /* Create Version string */
  if( (UserStringLen > 0) && (UserStringLen < MMPROBE_VERSION_STING_MAX_LEN) )
  {
    strncpy(VersionString_p, User, UserStringLen);
    strcat(VersionString_p, ", ");
  }

  if( (ModelStringLen > 0) && (ModelStringLen < (MMPROBE_VERSION_STING_MAX_LEN - UserStringLen) ) )
  {
    strncat(VersionString_p, Model, ModelStringLen);
    strcat(VersionString_p, ", ");
  }

  if( (BuildTimeStringLen > 0) && (BuildTimeStringLen < (MMPROBE_VERSION_STING_MAX_LEN - UserStringLen - ModelStringLen) ) )
    strcat(VersionString_p, BuildTime);
}

/*********************************************************************************/
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*********************************************************************************/
void sigchld_handler(int sig) {
  IDENTIFIER_NOT_USED(sig);
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

/*********************************************************************************/
int create_files(char *probeIDs, int logDecoderFormat, FILE **binFileToOpen, FILE *wavFilePathsToOpen[]) {
  char filename[50] = {0};
  char filepath[MM_PROBE_MAX_PATH_SIZE+50] = {0};
  int length = 0;
  struct tm *tmp;
  time_t t = time(NULL);
  FILE *pathFile = NULL;

  //Find out file path
  pathFile = fopen(MMPROBETXT, "r");
  if (pathFile != NULL) {
    fread(&filepath, 1, MM_PROBE_MAX_PATH_SIZE, pathFile);
  } else {
    log_message(L_ERR, "client: Path could not open file:%s for reading", MMPROBETXT);
    return 1;
  }
  fclose(pathFile);
  length = strlen(filepath);
  if (filepath[length-1] != '/') {
    filepath[length] = '/';
    filepath[length + 1] = '\0';
  }
  //Create directory if not already exists
  if (mkdir(filepath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    if (errno != EEXIST) {
      log_message(L_ERR, "client: could not create dir:%s, error=%s", filepath , strerror(errno));
      return 1;
    }
  }

  //Create filename
  tmp = localtime(&t);
  if (tmp  != NULL) {
    if (logDecoderFormat != 0) {
      strftime(filename, sizeof(filename), "ProbeData_%Y-%m-%d_%H%M%S.bin", tmp);
    } else {
      strftime(filename, sizeof(filename), "ProbeData_%Y-%m-%d_%H%M%S", tmp);
    }
  } else {
    log_message(L_ERR, "client: could not set filename");
    return 1;
  }
  strncat(filepath, filename, 50);
  if (logDecoderFormat != 0) {
    //Running together with LogDecoder i.e only one file
    //Check if a file already is open i.e. someone tried to change the Path
    if (*binFileToOpen != NULL) {
      fclose(*binFileToOpen);
      *binFileToOpen = NULL;
    }

    *binFileToOpen = fopen(filepath, "wb");
    if (*binFileToOpen == NULL) {
      log_message(L_ERR, "client: could not open file:%s for writing", filepath);
      return 1;
    } else {
      char str[15]; //for writing mmprobe version
      sprintf(str, "#!VER %s\n", MM_PROBE_PROBING_VERSION);
      //write mmprobe version to file in first line
      fwrite(str, 1, strlen(str), *binFileToOpen);
      log_message(L_INFO, "client: filepath=%s", filepath);
    }
  } else {
    int probeId = 0;
    char* token = NULL;

    //parse all probes activated
    token = strtok(probeIDs, " ,");
    while (token != NULL) {
      char str[15];
      char filepathID[MM_PROBE_MAX_PATH_SIZE+50] = {0};

      probeId = atoi(token);
      sprintf(str, "_pid%d.wav", probeId);
      strncat(filepathID, filepath, MM_PROBE_MAX_PATH_SIZE);
      strncat(filepathID, str, 15);
      //Speech probes excluding
      if ( (probeId < MM_PROBE_NUMBER_OF_SPEECH_PROBES) &&
           (probeId != MM_PROBE_TX_SPE_OUTPUT) &&
           (probeId != MM_PROBE_RX_SPD_INPUT) ) {
        if (wavFilePathsToOpen[probeId] != NULL) {
          fclose(wavFilePathsToOpen[probeId]);
          wavFilePathsToOpen[probeId] = NULL;
        }
        wavFilePathsToOpen[probeId] = fopen(filepathID, "w+b");
        if (wavFilePathsToOpen[probeId] == NULL) {
          log_message(L_ERR, "client: could not open wavFilePathsToOpen[%d]:%s for writing", probeId, filepathID);
          return 1;
        } else {
          log_message(L_INFO, "client: filepathID=%s", filepathID);
        }
      } else if (probeId > MM_PROBE_NUMBER_OF_SPEECH_PROBES) {
        //ADM/OMX probes
        int i;

        enable_probe(probeId); //Make sure the probe is enabled if not already done

        for (i=0; i<MM_PROBE_MAX_NUMBER_ADM_PROBES; i++) {
          if (deviceProbeList[i].pid == probeId) {
            if (deviceProbeList[i].file != NULL) {
              fclose(deviceProbeList[i].file);
              deviceProbeList[i].file = NULL;
            }
            deviceProbeList[i].file = fopen(filepathID, "w+b");
            if (deviceProbeList[i].file == NULL) {
               log_message(L_ERR, "client: could not open deviceProbeList[%d]:%s for pid=%d for writing", i, filepathID, probeId);
               return 1;
            } else {
              log_message(L_INFO, "client: filepathID=%s", filepathID);
            }
          }
        }
      }
      token = strtok(NULL, " ,");
    }
  }
  return 0;
}

/*********************************************************************************/
static void *handle_clients(void* args) {
  char s[MM_PROBE_MAX_DATA_SEND_SIZE + sizeof(probeHeader_t)] = {0};
  int num, fd = -1;
  boolean run = true;
  client_terminated = false;
  FILE *binFile = NULL; //Used for combined bin-file
  int server_store_location = 0;  //0=>PC 1=>sdcard
  FILE *wavSpeechFilePaths[MM_PROBE_NUMBER_OF_SPEECH_PROBES] = {NULL}; //Used for Speech probes wav-files
  char wavSpeechFormatWritten[MM_PROBE_NUMBER_OF_SPEECH_PROBES] = {0}; //to store enabled probes for filepath creation
  int probeDataCnt = 0;     //How much data from a read has been used
  int moreProbeData = 1;    //to see if one read contain more than one probe
  char tmpProbe[MM_PROBE_MAX_DATA_SEND_SIZE + sizeof(probeHeader_t)] = {0}; //Temp store for next probe to be written to file
  int tmpProbeHeaderSize = 0;
  int tmpProbePayloadSize = 0;
  threadInputData_t *input = (threadInputData_t*) args;
  char probeIDs[MM_PROBE_NUMBER_OF_SPEECH_PROBES * 3 + 1] = {0}; //to store enabled probes for filepath creation

  char *tmpBinFileBuffer = malloc(MM_PROBE_MAX_DATA_SEND_SIZE);
  int tmpBinFileBufferCounter = 0;

  log_message(L_INFO, "client: waiting for data...");

  fd = open(FIFO_NAME, O_RDONLY);
  if (input->enabledProbes != NULL) {
    strncat(probeIDs, input->enabledProbes, MM_PROBE_NUMBER_OF_SPEECH_PROBES * 3);
  }
  // To make read blocking we must always have a writer connected to the pipe
  int dummy_client = open(FIFO_NAME, O_WRONLY);

  while (run) {
    if ((num = read(fd, s, MM_PROBE_MAX_DATA_SEND_SIZE)) == -1) {
      log_message(L_INFO, "client: failed to read from pipe");
      run = false;
    // Silly temporary solution to kill the client.
    // Works only for 1 client. The rest will still be running since
    // there is a race of which client gets the read.
    // I need to use select with multiple fds instead (I think)...
    } else if (strcmp(s, "die") == 0) {
      log_message(L_INFO, "client: die message received");
      run = false;
    } else if (strcmp(s, "PC") == 0) {
      server_store_location = 0;
      log_message(L_INFO, "client: server_store_location=PC");
    } else if (strcmp(s, "Phone") == 0) {
      server_store_location = 1;
      log_message(L_INFO, "client: server_store_location=Phone");
    } else if (strcmp(s, "Path") == 0) {
      //Check if LogDecoder or commandline interface is used
      //If LogDecoder is used set logDecoderFormat to combined file
      if (probeIDs[0] == '\0') {
        input->logDecoderFormat = 1;
      }
      //Create necessary files for probing to locally on phone
      if (create_files(probeIDs, input->logDecoderFormat, &binFile, wavSpeechFilePaths) != 0) {
        run = false;
      }
    } else if(num > 0) {
      if (server_store_location == 1) {
        if (input->logDecoderFormat != 0) {
          //write all probe data to a single combined file in LogDecoder file format
          if (binFile != NULL) {
            //if the tmp-buffer is full write data to filesystem. Buffer used to reduce no of fwrite which takes long time
            if (num >= (MM_PROBE_MAX_DATA_SEND_SIZE - tmpBinFileBufferCounter)) {
              fwrite(tmpBinFileBuffer, 1, tmpBinFileBufferCounter, binFile);
              tmpBinFileBufferCounter = 0;
            }
            //Copy probe-data to tmp-buffer
            memcpy(tmpBinFileBuffer + tmpBinFileBufferCounter, s, num);
            tmpBinFileBufferCounter += num;
          }
        } else {
          //write probe data to multiple files, one per pid, pcm .wav format
          char* probe_p = s;
          probeHeader_t * header_p = (probeHeader_t *)tmpProbe;
          probeDataCnt = 0;
          moreProbeData = 1;

          while (moreProbeData) {

            //Copy header
            if (tmpProbeHeaderSize < (int)sizeof(probeHeader_t)) {
              int toBeCopied = ( (sizeof(probeHeader_t)-tmpProbeHeaderSize) <= (num -probeDataCnt)  ? (sizeof(probeHeader_t)-tmpProbeHeaderSize) : (num -probeDataCnt) );

              memcpy(tmpProbe, probe_p, toBeCopied);
              probe_p += toBeCopied;
              probeDataCnt += toBeCopied;
              tmpProbeHeaderSize += toBeCopied;
            }
            //Copy payload
            if (tmpProbeHeaderSize == sizeof(probeHeader_t)) {
              if ( tmpProbePayloadSize < header_p->Size ) {
                int toBeCopied = ( ( header_p->Size - tmpProbePayloadSize) <= (num -probeDataCnt) ? header_p->Size - tmpProbePayloadSize : (num -probeDataCnt) );

                memcpy( (tmpProbe + tmpProbeHeaderSize + tmpProbePayloadSize), probe_p, toBeCopied);
                probe_p += toBeCopied;
                probeDataCnt += toBeCopied;
                tmpProbePayloadSize += toBeCopied;
              }
            }

            //If a complete full probe exist write to file
            if ( (tmpProbeHeaderSize == sizeof(probeHeader_t)) && (header_p->Size == tmpProbePayloadSize) ) {
              int probeId = header_p->ProbeId;
              //Check if a file has been open for writing
              //Speech probes
              if ( (probeId < MM_PROBE_NUMBER_OF_SPEECH_PROBES) && (wavSpeechFilePaths[probeId] != NULL) ) {

                if (wavSpeechFormatWritten[probeId] == false) {
                  //Speech probes that uses v1 have to override samplerate
                  if (header_p->DataFormat.SampleRate == MM_PROBE_SAMPLE_RATE_UNKNOWN) {
                    //20 ms probes in CScall
                    if (  (probeId == MM_PROBE_RX_SPD_OUTPUT) || (probeId == MM_PROBE_TX_SPE_INPUT) ||
                          (probeId == MM_PROBE_TX_CS_CALL_INPUT) || (probeId == MM_PROBE_RX_CS_CALL_OUTPUT) ) {
                      if (header_p->Size == 320)
                        header_p->DataFormat.SampleRate = MM_PROBE_SAMPLE_RATE_8KHZ;
                      else
                        header_p->DataFormat.SampleRate = MM_PROBE_SAMPLE_RATE_16KHZ;
                    } else {
                    //10 ms probes in Speech_proc
                      if (header_p->Size == 160)
                        header_p->DataFormat.SampleRate = MM_PROBE_SAMPLE_RATE_8KHZ;
                      else
                        header_p->DataFormat.SampleRate = MM_PROBE_SAMPLE_RATE_16KHZ;
                    }
                  }
                  writeWavHeader((DataFormat_t*) &header_p->DataFormat, wavSpeechFilePaths[probeId]);
                  wavSpeechFormatWritten[probeId] = true;
                }
                fwrite(tmpProbe + sizeof(probeHeader_t), 1, tmpProbePayloadSize, wavSpeechFilePaths[probeId]);
              } else if (probeId > MM_PROBE_NUMBER_OF_SPEECH_PROBES) {
                //ADM/OMX probes
                int i;

                for (i=0; i<MM_PROBE_MAX_NUMBER_ADM_PROBES; i++) {
                  if ( (deviceProbeList[i].pid  == probeId) && (deviceProbeList[i].file  != NULL) ) {

                    if (deviceProbeList[i].formatWritten == false) {
                      writeWavHeader((DataFormat_t*) &header_p->DataFormat, deviceProbeList[i].file);
                      deviceProbeList[i].formatWritten = true;
                    }
                    fwrite(tmpProbe + sizeof(probeHeader_t), 1, tmpProbePayloadSize, deviceProbeList[i].file);
                    break;
                  }
                }
              }
              tmpProbeHeaderSize = 0;
              tmpProbePayloadSize = 0;
            }

            //all read data used
            if ( probeDataCnt == num ) {
              moreProbeData = 0;
            }
          }
        }
      } else {
        send(input->clientfd, s, num, 0);
      }
    } else {
      log_message(L_INFO, "client: this should never happen!");
      run = false;
    }
  }

  if ((server_store_location == 1) && (binFile != NULL)) {
    fclose(binFile);
  } else if ((server_store_location == 1) && (input->logDecoderFormat == 0)) {
    int i;
    //Running without LogDecoder i.e. each active probe file should be closed
    //Speech probes
    for (i=1; i < MM_PROBE_NUMBER_OF_SPEECH_PROBES; i++) {
      if (probe_list[i-1] == 1) {
        if (wavSpeechFilePaths[i] != NULL) {
          if (wavSpeechFormatWritten[i] == true) {
            updateWavHeader(wavSpeechFilePaths[i]);
          }
          fclose(wavSpeechFilePaths[i]);
          wavSpeechFilePaths[i] = NULL;
        }
      }
    }
    //ADM/OMX probes
    for (i=0;  i <MM_PROBE_MAX_NUMBER_ADM_PROBES; i++) {
      if (deviceProbeList[i].file != NULL) {
        if (deviceProbeList[i].formatWritten == true) {
          updateWavHeader(deviceProbeList[i].file);
        }
        fclose(deviceProbeList[i].file);
        deviceProbeList[i].file = NULL;
      }
    }
  }
  close(fd);
  close(dummy_client);
  client_terminated = true;
  disable_all_probes();

  // Client process can now go and die.
  log_message(L_INFO, "client: shutting down.");
  close(input->clientfd);

  return NULL;
}

/*********************************************************************************/
void stop_client() {
  int fd = -1;
  char buf[MMPROBE_SHORT_MESSAGE_SIZE] = {0};

  if ((fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
      log_message(L_WARN, "server: could not open FIFO: (%s).",
          strerror(errno));
  } else {
    log_message(L_INFO, "server: terminating client.");
    // Write something to the FIFO to exit the loop
    sprintf(buf, "die");
    if (!write(fd, buf, sizeof(buf)) > 0) {
      log_message(L_WARN, "server: could not send to FIFO (%s).",
          strerror(errno));
    }
  }

  close(fd);
}

/*********************************************************************************/
void enable_probe(int pid) {
  //Speech probes
  if (pid > 0 && pid < MM_PROBE_NUMBER_OF_SPEECH_PROBES) {
    if (probe_list[pid-1] == 0) {
      probe_list[pid-1] = 1;
      enabled_speech_probes++;
    }
    log_message(L_INFO, "server: enabled probe %d, number of SPEECH probes enabled = %d", pid, enabled_speech_probes);
  } else {

    //ADM/OMX probes
#ifndef X86
    ste_adm_res_t adm_result = STE_ADM_RES_OK;
    adm_result = ste_adm_client_set_pcm_probe(pid, 1); //Inform ADM about enabled device probe
    if (adm_result != STE_ADM_RES_OK) {
      log_message(L_ERR, "server: failed to enabled probe %d, number of DEVICE probes enabled = %d, adm_result=%d", pid, enabled_device_probes, adm_result);
    }
      else
#endif
    {
      int i;

      for (i=0; ( (i<MM_PROBE_MAX_NUMBER_ADM_PROBES) && (deviceProbeList[i].pid != pid) ); i++)
        ; //do nothing just to check if pid already in list

      if (i == MM_PROBE_MAX_NUMBER_ADM_PROBES) {
        for (i=0; i<MM_PROBE_MAX_NUMBER_ADM_PROBES; i++) {
          if (deviceProbeList[i].pid  == 0) {
            deviceProbeList[i].pid = pid;
            enabled_device_probes++;
            log_message(L_INFO, "server: enabled probe %d, number of DEVICE probes enabled = %d", pid, enabled_device_probes);
            break;
          }
        }
        if (i == MM_PROBE_MAX_NUMBER_ADM_PROBES) {
          log_message(L_ERR, "server: failed to enabled probe %d, number of DEVICE probes enabled = %d. MM_PROBE_MAX_NUMBER_ADM_PROBES = %d"
                              , pid, enabled_device_probes, MM_PROBE_MAX_NUMBER_ADM_PROBES);
        }
      }
    }
  }
}

/*********************************************************************************/
void disable_probe(int pid) {
  //Speech probes
  if (pid > 0 && pid < MM_PROBE_NUMBER_OF_SPEECH_PROBES) {
    if (probe_list[pid-1] == 1) {
      probe_list[pid-1] = 0;
      enabled_speech_probes--;
    }
    log_message(L_INFO, "server: disabled probe %d, number of SPEECH probes enabled = %d", pid, enabled_speech_probes);
  } else {

    //ADM/OMX probes
#ifndef X86
    ste_adm_res_t adm_result = STE_ADM_RES_OK;
    adm_result = ste_adm_client_set_pcm_probe(pid, 0); //Inform ADM about disabled device probe
    if (adm_result != STE_ADM_RES_OK) {
      log_message(L_ERR, "server: failed to disable probe %d, number of DEVICE probes enabled = %d, adm_result=%d", pid, enabled_device_probes, adm_result);
    }
      else
#endif
    {
      int i;

      for (i=0; i<MM_PROBE_MAX_NUMBER_ADM_PROBES; i++) {
        if (deviceProbeList[i].pid  == pid) {
          deviceProbeList[i].pid = 0;
          if (deviceProbeList[i].file != NULL) {
            fclose(deviceProbeList[i].file);
            deviceProbeList[i].file = NULL;
          }
          deviceProbeList[i].formatWritten = false;
          enabled_device_probes--;
          log_message(L_INFO, "server: disabled probe %d, number of DEVICE probes enabled = %d", pid, enabled_device_probes);
        }
      }
    }
  }
}

/*********************************************************************************/
void disable_all_probes() {
  int i;

  //All Speech probes
  memset(probe_list, 0 , MM_PROBE_MAX_NUMBER_PROBES);
  enabled_speech_probes = 0;

  //All ADM/OMX probes
  for (i=0; i < MM_PROBE_MAX_NUMBER_ADM_PROBES; i++) {
#ifndef X86
  if (deviceProbeList[i].pid != 0) {
    (void) ste_adm_client_set_pcm_probe(deviceProbeList[i].pid, 0); //Inform ADM about disabled device probe
}
#endif

    deviceProbeList[i].pid  = 0;
    deviceProbeList[i].file = NULL;
    deviceProbeList[i].formatWritten = false;
  }
  enabled_device_probes = 0;

  log_message(L_INFO, "server: disabling all probes");
}

/*********************************************************************************/
void *handle_subscribers() {
    int s, s2, len;
    unsigned int t;
    struct sockaddr_un local, remote;

    if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(s, (struct sockaddr *)&local, len) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(s, 5) == -1) {
        perror("listen");
        exit(1);
    }

    for(;;) {
        log_message(L_INFO, "server: waiting for subscribers...");
        t = sizeof(remote);
        if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
            perror("accept");
        }

        log_message(L_INFO, "server: subscriber connected, fd=%d", s2);

        MM_ProbeSubscriberID_t subscriber_id;

        recv(s2, &subscriber_id, sizeof(MM_ProbeSubscriberID_t), 0);

        subscriber_list[subscriber_id] = s2;
        nbr_of_subscribers++;

        log_message(L_INFO, "server: proxy with fd=%d registered subscriber id=%d", s2, subscriber_id);
    }
}

/*********************************************************************************/
int init_server(const int port, int *sockfd) {
  struct addrinfo hints, *servinfo, *p;
  const int yes = 1;
  int rv = -1;
  char * service = itoa(port, 10);
  char buf[MM_PROBE_MAX_PATH_SIZE] = {0};

  pthread_t thread;
  int rc = -1;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  log_message(L_INFO, "server: starting listening server on port %d", port);

  if ((rv = getaddrinfo(NULL, service, &hints, &servinfo)) != 0) {
    log_message(L_ERR,"getaddrinfo: %s", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {

    // setup the stream to receive incoming connections from.
    if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    // allow socket descriptor to be reusable.
    if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
        (socklen_t) sizeof(int)) == -1) {
      perror("setsockopt");
      return 1;
    }

    // set non blocking socket.
    int opts;
    if ((opts = fcntl(*sockfd, F_GETFL)) < 0) {
      perror("fnctl(F_GETFL");
      return 1;
    }
    if (fcntl(*sockfd, F_SETFL, opts | O_NONBLOCK) < 0) {
      perror("fnctl(F_SETFL");
      return 1;
    }

    // bind the socket
    if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(*sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  if (p == NULL)  {
    log_message(L_ERR,"server: failed to bind.");
    return 2;
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (listen(*sockfd, BACKLOG) == -1) {
    perror("listen");
    return 3;
  }

  remove(FIFO_NAME_PATH);

  // Create the FIFO pipe
  mode_t mode = umask(0);
  if (mknod(FIFO_NAME, S_IFIFO | 0666 , 0) != 0) {
    log_message(L_ERR, "server: could not create FIFO pipe: %s",
        strerror(errno));
    return 4;
  }
  else {
    getcwd(buf, MM_PROBE_MAX_PATH_SIZE);
    log_message(L_INFO, "server: FIFO pipe listening at: %s/%s",
        buf, FIFO_NAME);
  }

  // Used mapped memory to acces the probe list in the filesystem
  file = open(LIST_NAME_PATH, O_RDWR | O_CREAT, 0666);
        lseek(file, MM_PROBE_MAX_NUMBER_PROBES+1, SEEK_SET);
        write(file, "", 1);
        lseek(file, 0, SEEK_SET);
        file_memory = mmap(0, MM_PROBE_MAX_NUMBER_PROBES, PROT_WRITE, MAP_SHARED, file, 0);
        close(file);
        probe_list = (char*)file_memory;

        log_message(L_INFO, "server: mapped memory for probe list");

  nbr_of_subscribers = 0;
  memset(subscriber_list, -1, MM_PROBE_MAX_NUMBER_SUBSCRIBERS);

  rc = pthread_create(&thread, NULL, handle_subscribers, NULL);
        if (rc){
          log_message(L_ERR, "ERROR; return code from pthread_create() is %d", rc);
        }

  umask(mode);

  return 0;
}

/*********************************************************************************/
int start_server(const int sockfd, char* enabledProbes, int logDecoderFormat) {
  int i = 0;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size = 0;
  struct sigaction sa;
  char s[INET6_ADDRSTRLEN];

  char VersionString[MMPROBE_VERSION_STING_MAX_LEN] = "\0"; // Version string

  // Reception buffer.
  unsigned char buf[MM_PROBE_MAX_DATA_RECEIVE_SIZE] = {0};
  int nbytes = 0;
  int retval;
  threadInputData_t input;

  // Maximum file descriptor number
  int highsock = sockfd;
  // Newly accepted socket descriptor
  int new_fd = -1;
  // Master file descriptor
  fd_set socks;
  // Temporary file descriptor list for select()
  fd_set read_fds;

  struct timeval tv;

  // Get Version from ME
  MMPROBE_GetVersionString(VersionString);

  //are we running mmprobe standalone without LogDecoder?
  if (enabledProbes != NULL) {
    char* token = NULL;
    int probeId = 0;

    disable_all_probes();

    //need a sleep for handle_clients have time process
    usleep(10000);

    //Start the handle client thread
    pthread_t thread;
    input.logDecoderFormat = logDecoderFormat;
    input.clientfd = -1;
    input.enabledProbes = enabledProbes;
    pthread_create(&thread, NULL, handle_clients, (void*) &input);

    //need a sleep for handle_clients have time process
    usleep(10000);
    int fd = -1;

    if ((fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
      log_message(L_WARN, "server: could not open FIFO: (%s).",
      strerror(errno));
    } else {
      //Set probes will be stored locally on phone
      char location_buf[MMPROBE_SHORT_MESSAGE_SIZE] = {0};

      sprintf(location_buf, "Phone");
      if (!write(fd, location_buf, sizeof(location_buf)) > 0) {
        log_message(L_WARN, "server: Phone could not send to FIFO (%s).",
        strerror(errno));
      }
    }
    close(fd);
    //need a sleep for handle_clients have time process
    usleep(10000);

    if ((fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
      log_message(L_WARN, "server: could not open FIFO: (%s).",
      strerror(errno));
    } else {
      //Set path
      char location_buf[MMPROBE_SHORT_MESSAGE_SIZE] = {0};

      sprintf(location_buf, "Path");
      if (!write(fd, location_buf, sizeof(location_buf)) > 0) {
        log_message(L_WARN, "server: Path could not send to FIFO (%s).",
        strerror(errno));
      }
    }
    close(fd);
    //need a sleep for handle_clients have time to process and create files
    sleep(1);

    //parse all probes activated
    token = strtok(enabledProbes, " ,");
    while (token != NULL) {
      probeId = atoi(token);
      enable_probe(probeId);
      token = strtok(NULL, " ,");
      usleep(2000);
    }

    while (true) {
      sleep(1);
    }
  }

  // Reap all dead processes
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    log_message(L_ERR, "server: sigaction failed: %s.", strerror(errno));
    return 1;
  }

  // Build the select list.
  FD_ZERO(&socks);
  FD_ZERO(&read_fds);

  // Add the listener to the master set
  FD_SET(sockfd, &socks);

  log_message(L_INFO, "server: waiting for new connections...");

  while (true) { // main accept() loop;
    read_fds = socks;

    // if (select(highsock + 1, &read_fds, NULL, NULL, &tv) == -1) {
      // log_message(L_ERR, "server: select failed: %s.", strerror(errno));
      // continue;
    // }
    // Wait up to five seconds.
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    retval = select(highsock + 1, &read_fds, NULL, NULL, &tv);

    if (retval == -1) {
      log_message(L_ERR, "server: select failed: %s.", strerror(errno));
      continue;
    } else if (retval) {
      for (i = 0; i <= highsock; i++) {
        if (FD_ISSET(i, &read_fds)) {
          if (i == sockfd) {
            // A new connection is detected.
            sin_size = sizeof(their_addr);

            new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
                    &sin_size);

            if (new_fd == -1) {
              log_message(L_ERR, "server: accept failed: %s",
                  strerror(errno));
              continue;
            } else {

              // Check if a client already is connected
              if (connected == true) {
                log_message(L_ERR,
                    "server: a client is already connected.");
                close(new_fd);
                new_fd = -1;
                continue;
              }

              // Add the new connection to the master set.
              FD_SET(new_fd, &socks);

              // Adds the new fd as top most.
              if (new_fd > highsock) {
                highsock = new_fd;
              }

              inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s,
                  sizeof(s));

              log_message(L_INFO,"server: got connection from %s.", s);

              disable_all_probes();
              //Start the handle client thread
              pthread_t thread;
              input.logDecoderFormat = logDecoderFormat;
              input.clientfd = new_fd;
              input.enabledProbes = NULL;
              pthread_create(&thread, NULL, handle_clients, (void*) &input);

              connected = true;
            }
          } else {
            if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
              log_message(L_INFO,
                  "server: client disconnect detected.");

              if (server_client_suspended == 0) {
                stop_client();
              }

              close(i);
              FD_CLR(i, &socks);

              connected = false;
            } else {
              log_message(L_INFO, "server: client sent %d bytes", nbytes);

              int j = 0;

              while (j < nbytes) {
                switch (buf[j]) {
                  case MM_PROBE_REQUEST_ME_VERSION:
                  {
                    log_message(L_INFO, "server: client sent MM_PROBE_REQUEST_ME_VERSION, Returning: %s",VersionString);
                    j += sizeof(MM_ProbeGetME_VersionCmd_t);

                    send(new_fd, VersionString, MMPROBE_VERSION_STING_MAX_LEN, 0);
                    break;
                  }

                  case MM_PROBE_SET_PROBE_STATUS:
                  {
                    MM_ProbeSetStatusCmd_t *cmd = (MM_ProbeSetStatusCmd_t*)&buf[j];

                    if (cmd->ProbeStatus == MM_PROBE_STATUS_ENABLED)
                      enable_probe(cmd->ProbeId);
                    else
                      disable_probe(cmd->ProbeId);

                    j += sizeof(MM_ProbeSetStatusCmd_t);
                    break;
                  }

                  case MM_PROBE_SET_PROBE_STATUS_V2:
                  {
                    MM_ProbeSetStatusCmd_V2_t *cmd = (MM_ProbeSetStatusCmd_V2_t*)&buf[j];

                    if (cmd->ProbeStatus == MM_PROBE_STATUS_ENABLED)
                      enable_probe(cmd->ProbeId);
                    else
                      disable_probe(cmd->ProbeId);

                    j += sizeof(MM_ProbeSetStatusCmd_V2_t);
                    break;
                  }

                  case MM_PROBE_SEND_DATA:
                    log_message(L_INFO, "server: client sent MM_PROBE_SEND_DATA");

                    if (nbytes < sizeof(MM_ProbeSendDataCmd_t)) {
                      log_message(L_INFO, "server: received %d bytes but expected %d bytes", nbytes, sizeof(MM_ProbeSendDataCmd_t));
                      while(nbytes < sizeof(MM_ProbeSendDataCmd_t)) {
                        char* tmp = (char*) buf + nbytes;
                        nbytes += recv(i, tmp, sizeof(buf), 0);
                        log_message(L_INFO, "server: received extra bytes");
                      }
                    }

                    MM_ProbeSendDataCmd_t *cmd = (MM_ProbeSendDataCmd_t*)&buf[j];

                    MM_ProbeSubscriberID_t subscriber_id = cmd->SubscriberId; //Read from message
                    int fd = -1;

                    j += sizeof(MM_ProbeSendDataCmd_t);

                    if (subscriber_id < MM_PROBE_MAX_NUMBER_SUBSCRIBERS) {
                      log_message(L_INFO, "server: got subscriber_id=%d", subscriber_id);
                      fd = subscriber_list[subscriber_id];

                    } else {
                      log_message(L_ERR, "server: invalid subscriber id=%d", subscriber_id);
                      break;
                    }

                    if (fd > 0) {
                      int t = 0;
                      log_message(L_INFO, "server: proxy subscriber_id=%d", subscriber_id);

                      if ((t = send(fd, cmd->Data, cmd->DataSize, 0)) < 0)
                        log_message(L_ERR, "server: failed writing to fd=%d", fd);
                      else
                        log_message(L_INFO, "server: sent %d bytes to fd=%d", t, fd);
                    } else {
                      log_message(L_ERR, "server: invalid subscriber fd=%d", fd);
                      break;
                    }

                    break;

                  case MM_PROBE_SET_LOGGING_LOCATION:
                  {
                    int fd = -1;
                    char location_buf[MMPROBE_SHORT_MESSAGE_SIZE] = {0};
                    MM_ProbeSetLoggingLocation_t *cmd = (MM_ProbeSetLoggingLocation_t*)&buf[j];
                    log_message(L_INFO, "server: MM_PROBE_SET_LOGGING_LOCATION received...Location=%d", cmd->Location);

                    if ((fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
                      log_message(L_WARN, "server: could not open FIFO: (%s).",
                      strerror(errno));
                    } else {
                      // Write store location to the FIFO
                      if (cmd->Location == 1) {
                        sprintf(location_buf, "Phone");
                      } else {
                        sprintf(location_buf, "PC");
                      }
                      if (!write(fd, location_buf, sizeof(location_buf)) > 0) {
                        log_message(L_WARN, "server: could not send to FIFO (%s).",
                        strerror(errno));
                      }
                    }
                    close(fd);

                    j += sizeof(MM_ProbeSetLoggingLocation_t);
                    break;
                  }

                  case MM_PROBE_SUSPEND:
                  {
                    log_message(L_INFO, "server: MM_PROBE_SUSPEND received...");
                    server_client_suspended = 1;
                    connected = false;
                    j += sizeof(MM_ProbeSuspendCmd_t);
                    break;
                  }

                  case MM_PROBE_SET_PATH:
                  {
                    int fd = -1;
                    char location_buf[MMPROBE_SHORT_MESSAGE_SIZE] = {0};
                    FILE *file;
                    MM_ProbeSetPath_t *cmd = (MM_ProbeSetPath_t*)&buf[j];
                    log_message(L_INFO, "server: MM_PROBE_SET_PATH received... Path=%s", cmd->Path);
                    file = fopen(MMPROBETXT, "w");
                    if (file != NULL) {
                      fwrite(cmd->Path, 1, strlen(cmd->Path), file);
                      fclose(file);
                    } else {
                      log_message(L_ERR, "server: MM_PROBE_SET_PATH could not open file:%s for writing", MMPROBETXT);
                    }

                    if ((fd = open(FIFO_NAME, O_WRONLY | O_NONBLOCK)) == -1) {
                      log_message(L_WARN, "server: could not open FIFO: (%s).",
                      strerror(errno));
                    } else {
                      // Write store Path to the FIFO
                      sprintf(location_buf, "Path");
                      if (!write(fd, location_buf, sizeof(location_buf)) > 0) {
                        log_message(L_WARN, "server: could not send to FIFO (%s).",
                        strerror(errno));
                      }
                    }
                    close(fd);
                    j+=sizeof(MM_ProbeSetPath_t);
                    break;
                  }

                  case MM_PROBE_REQUEST_MMPROBE_VERSION:
                  {
                    log_message(L_INFO, "server: client sent MM_PROBE_REQUEST_MMPROBE_VERSION, Returning: %s", MM_PROBE_PROBING_VERSION);
                    j += sizeof(MM_ProbeGetMM_ProbeVersionCmd_t);

                    send(new_fd, MM_PROBE_PROBING_VERSION, MMPROBE_VERSION_STING_MAX_LEN, 0);
                    break;
                  }
                  default:
                    buf[nbytes] = '\0';
                    log_message(L_INFO, "server: client sent bogus data: %s.", buf);
                    j = nbytes;
                    break;
                }

              }
            }
          }
        }
      }
    }
  }

  log_message(L_INFO, "server : shutdown");
  return 0;
}

/*********************************************************************************/
int stop_server(const int sockfd) {

  stop_client();

  log_message(L_INFO, "server: cleaning...");

  while (client_terminated == false) {
    sleep(3);
  }

  if (remove(FIFO_NAME) != 0) {
    log_message(L_WARN, "server: could not remove the FIFO (%s).",
        strerror(errno));
  }

  close(sockfd);

  munmap(file_memory, MM_PROBE_MAX_NUMBER_PROBES);

  log_message(L_INFO, "server: stopped...");
  return 0;
}

/*********************************************************************************/
// Internal test method.
#ifdef MODULE_TEST
int main(int argc, char **argv) {
  int sockfd = -1;
  int port = 20000;

  if (init_server(port, &sockfd) != 0) {
    fprintf(stderr, "Failed to create the server\n");
    return 1;
  }

  if (start_server(sockfd) != 0) {
    fprintf(stderr, "Failed to start the server\n");
    return 1;
  }

  sleep(30);

  stop_server(sockfd);

  exit(EXIT_SUCCESS);
}
#endif

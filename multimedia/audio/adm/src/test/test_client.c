/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file test_client.c
*   \brief Simple client for testing.

    Connects to the server and sends/receives messages.
*/
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include "cmdline_parser.h"
#include <ste_adm_client.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#ifdef ADM_DBG_X86
  #include <signal.h>
  typedef unsigned int uint_t;
#endif

#ifndef ADM_DBG_X86
  #include <linux/videodev2.h>
#endif

#include "ste_adm_client.h"
#include "OMX_Audio.h"
#include "AFM_Index.h"
#include "AFM_Types.h"

#define IL_INIT_CONFIG_STRUCT(__il_config_struct) \
    do { \
        memset(&__il_config_struct, 0, sizeof(__il_config_struct)); \
        __il_config_struct.nSize = sizeof(__il_config_struct); \
        __il_config_struct.nVersion.nVersion = OMX_VERSION; \
    } while(0)



typedef struct {
    OMX_HANDLETYPE omx_handle;
    int some_data;
    char *omx_name;
} ste_adm_omx_t;


/* Default actual out devices */
#define DEVICE_EARP                     "REF_Earpiece"
#define DEVICE_SPEAKER                  "REF_Speaker"
#define DEVICE_HSOUT                    "REF_HSetOut"
#define DEVICE_HSIN                     "REF_HSetIn"
#define DEVICE_MIC                      "REF_Mic"
#define DEVICE_USBOUT                   "REF_USBOut"
#define DEVICE_USBIN                    "REF_USBIn"
#define DEVICE_LINEIN                   "REF_LineIn"
/*************************************************************************
* @brief        Parse arguments to determine test configuration.
*
* @param        argc     [in]   Number of arguments to parse
* @param        argv     [in]   Arguments to parse
*
* @return       nothing
*
*************************************************************************/
static void exitf(const char *fmt, ...) // TODO duplicated code
{
    va_list a;
    va_start(a, fmt);
    vfprintf(stderr, fmt, a);
    exit(EXIT_FAILURE);
}





#define FM_FACTOR 62.5


#ifndef RADIO_DEV_NAME
 #define RADIO_DEV_NAME "/dev/radio0"
#endif

#ifdef ADM_DBG_X86
static int radio_control(int transmit, int freq)
{
    fprintf(stderr, "radio_control - stub\n");
    (void) transmit;
    (void) freq;
    return 0;
}
#else

#include <fcntl.h>
#include "linux/videodev2.h"
#include <sys/ioctl.h>

static int radio_control(int transmit, int freq_in_hz)
{
    struct v4l2_capability caps;
    struct v4l2_frequency freq;
    struct v4l2_tuner tuner;
    struct v4l2_modulator modulator;
    struct v4l2_control ctrl;


    fprintf(stderr, "Frequency = %d\n", freq_in_hz);

    int fd = open(RADIO_DEV_NAME, O_RDONLY);
    if(fd < 0)
    {
        fprintf(stderr, "Error!! Unable to open radio device %s\n", RADIO_DEV_NAME);
        return -1;
    }
    fprintf(stderr, "FM Radio device opened with fd = %d\n", fd);

    memset(&caps, 0, sizeof(struct v4l2_capability));
    if(ioctl(fd, VIDIOC_QUERYCAP, &caps) != 0) {
        fprintf(stderr, "Error getting VIDIOC_QUERYCAP from device \n");
        close(fd);
        return -1;
    }
    if(((caps.capabilities & V4L2_CAP_TUNER) && (caps.capabilities & V4L2_CAP_MODULATOR)) == 0) {
        fprintf(stderr, "The FM radio doesn't support tuner and modulator\n");
    } else {
        fprintf(stderr, "The FM radio does support tuner and modulator\n");
    }
    if(!transmit)
    {
        fprintf(stderr, "Mode is set to FMRx\n");
        memset(&tuner, 0, sizeof(struct v4l2_tuner));
        tuner.index = 0;
        tuner.rxsubchans |= V4L2_TUNER_SUB_STEREO;
        if(ioctl(fd, VIDIOC_S_TUNER, &tuner) != 0)
        {
            fprintf(stderr, "Error setting VIDIOC_S_TUNER to RX mode in device\n");
            close(fd);
            return -1;
        }
    } else {
        fprintf(stderr, "Mode is set to FMTx\n");
        memset(&modulator, 0, sizeof(struct v4l2_modulator));
        modulator.index = 0;
        modulator.txsubchans |= V4L2_TUNER_SUB_STEREO;
        if(ioctl(fd, VIDIOC_S_MODULATOR, &modulator) != 0)
        {
            fprintf(stderr, "Error setting VIDIOC_S_MODULATOR to Tx mode in device\n");
            close(fd);
            return -1;
        }
    }

    memset(&tuner, 0, sizeof(struct v4l2_tuner));
    if(ioctl(fd, VIDIOC_G_TUNER, &tuner) != 0) {
        fprintf(stderr, "Error getting VIDIOC_G_TUNER from device\n");
        close(fd);
        return -1;
    }

    memset(&ctrl, 0, sizeof(struct v4l2_control));
    ctrl.id = V4L2_CID_CG2900_RADIO_SELECT_ANTENNA;
    ctrl.value = V4L2_CG2900_RADIO_WIRED_ANTENNA;
    if(ioctl(fd, VIDIOC_S_CTRL, &ctrl)  < 0) {
        fprintf(stderr, "Error when switching to wired antenna\n");
        close(fd);
        return -1;
    }
    fprintf(stderr, "Changed to wired antenn\n");

    memset(&freq, 0, sizeof(struct v4l2_frequency));
    freq.frequency = freq_in_hz / FM_FACTOR;
    if(ioctl(fd, VIDIOC_S_FREQUENCY, &freq) != 0) {
        fprintf(stderr, "Error setting frequency\n");
        close(fd);
        return -1;
    }

    memset(&freq, 0, sizeof(struct v4l2_frequency));
    if(ioctl(fd, VIDIOC_G_FREQUENCY, &freq) != 0)
    {
        fprintf(stderr, "Error getting frequemcy\n");
        close(fd);
        return -1;
    }
    fprintf(stderr, "Current Frequency is = %5.2f MHz \n", (freq.frequency * FM_FACTOR) / 1000000);

    return fd;
}
#endif



static void reinit_topdevice_to_default_device(void)
{
    ste_adm_set_toplevel_map_live(STE_ADM_DEVICE_STRING_HSOUT,DEVICE_HSOUT, NULL, NULL);
    ste_adm_set_toplevel_map_live(STE_ADM_DEVICE_STRING_HSIN,DEVICE_HSIN, NULL, NULL);
    ste_adm_set_toplevel_map_live(STE_ADM_DEVICE_STRING_EARP,DEVICE_EARP, NULL, NULL);
    ste_adm_set_toplevel_map_live(STE_ADM_DEVICE_STRING_SPEAKER,DEVICE_SPEAKER, NULL, NULL);
    ste_adm_set_toplevel_map_live(STE_ADM_DEVICE_STRING_MIC,DEVICE_MIC, NULL, NULL);
    ste_adm_set_toplevel_map_live(STE_ADM_DEVICE_STRING_USBOUT,DEVICE_USBOUT, NULL, NULL);
    ste_adm_set_toplevel_map_live(STE_ADM_DEVICE_STRING_USBIN,DEVICE_USBIN, NULL, NULL);


}

static unsigned int tv_ms_diff(struct timeval a, struct timeval b)
{
  return (unsigned int) (1000 * (a.tv_sec - b.tv_sec) + (a.tv_usec - b.tv_usec) / 1000);
}


static int readint32(char* buf)
{
    return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
}

static int readint16(char* buf)
{
    return (buf[1] << 8) | buf[0];
}

static void reorder_wav_to_hdmi(void* buf, size_t bytes)
{
//        fprintf(stderr, "  Order expected by ADM (HDMI): LF  RF  LFE CF  LS  RS\n");
    short* sbuf = (short*) buf;
    size_t samples = bytes / 12;
    size_t i;
    for (i=0 ; i < samples ; i++) {
        short cf  = sbuf[i*6 + 2];
        short lfe = sbuf[i*6 + 3];
        sbuf[i*6 + 2] = lfe;
        sbuf[i*6 + 3] = cf;
    }
}



int parse_wav(const char* filename, int* sr, int* chans)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    char buf[44];
    fread(buf,44,1,f);
    fclose(f);

    if (memcmp(buf, "RIFF", 4) != 0 || memcmp(buf+8, "WAVE", 4) != 0 ||
        memcmp(buf+12, "fmt ", 4) != 0 || memcmp(buf+36, "data", 4) != 0)
    {
        return 0;
    }

    *sr    = readint32(buf+24);
    *chans = readint16(buf+22);

    return 1;
}

ste_adm_format_t get_format_from_string(const char* s)
{
    static const struct {
        const char* str;
        ste_adm_format_t fmt;
    } fmt[] = {
        { "ac3",     STE_ADM_FORMAT_AC3     },
        { "mpeg1",   STE_ADM_FORMAT_MPEG1   },
        { "mp3",     STE_ADM_FORMAT_MP3     },
        { "mpeg2",   STE_ADM_FORMAT_MPEG2   },
        { "aac",     STE_ADM_FORMAT_AAC     },
        { "dts",     STE_ADM_FORMAT_DTS     },
        { "atrac",   STE_ADM_FORMAT_ATRAC   },
        { "oba",     STE_ADM_FORMAT_OBA     },
        { "ddplus",  STE_ADM_FORMAT_DDPLUS  },
        { "dts_hd",  STE_ADM_FORMAT_DTS_HD  },
        { "mat",     STE_ADM_FORMAT_MAT     },
        { "dst",     STE_ADM_FORMAT_DST     },
        { "wma_pro", STE_ADM_FORMAT_WMA_PRO },
        { "mat",     STE_ADM_FORMAT_MAT     },
        { NULL,      STE_ADM_FORMAT_INVALID }
    }, *cur_fmt = fmt;

    for ( ; cur_fmt->str ; cur_fmt++) {
        if (strcmp(cur_fmt->str, s) == 0) {
            return cur_fmt->fmt;
        }
    }

    return STE_ADM_FORMAT_INVALID;
}


// Make it possible to control 'ste-adm-test play' while it is running,
// with commands on stdin such as:
//   open Speaker
//   close HSetOut
//

static int play_record_stdinctl(int adm_fd, char** bufpp, int samplerate, int channels, int bufsz, int num_bufs)
{
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    char buf[120];
    int cnt=read(0, buf, 120);
    int opened_device = 0;
    if (cnt > 0) {
        const char* cmd = strtok(buf, " \n");
        const char* dev = strtok(0, " \n");
        if (cmd && dev) {
            if (strcmp(cmd, "open") == 0) {
                int actual_samplerate;
                ste_adm_res_t res = ste_adm_client_open_device(adm_fd, dev, samplerate,
                                        channels, &actual_samplerate, bufpp, bufsz, num_bufs);
                if (res != STE_ADM_RES_OK) {
                    fprintf(stderr, "ste_adm_client_open_device failed: '%s', dev '%s' %X %X %X %X\n", ste_adm_res_to_str(res), dev, dev[5], dev[6], dev[7], dev[8]);
                } else {
                    fprintf(stderr, "ste_adm_client_open_device %s OK\n", dev);
                }
                opened_device = 1;
            } else if (strcmp(cmd, "close") == 0) {
                ste_adm_res_t res = ste_adm_close_device(adm_fd, dev);
                if (res != STE_ADM_RES_OK) {
                    fprintf(stderr, "ste_adm_client_open_device failed: '%s'\n", ste_adm_res_to_str(res));
                } else {
                    fprintf(stderr, "ste_adm_close_device %s OK\n", dev);
                }
            }
        }
    }

    fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_NONBLOCK);
    return opened_device;
}

int play(int argc, char** argv)
{
    const char* play_input_filename  = NULL;
    const char* play_device   = "HSetOut";
    const char* play_device_2 = NULL;
    unsigned play_max_time_ms = 0xFFFFFFFF;
    unsigned play_min_time_ms = 0;
    int wait_key = 0;
    int samplerate = 48000;
    int channels = 2;
    const char* fmtstr = NULL;
    int wait2blocks = 0;
    int radio_freq=0;
    int random_drains=0;
    int random_behavior=0;
    int duration_limit=0;
    int is_wav = 0;
    int pause_after_open=0;
    int delay=0;
    uint_t play_device_2_delay=0;
    uint_t play_device_2_close_delay=0;
    uint_t play_device_2_reopendelay=0;
    int ramp_down = 0;
    int volumeQ15 = 32768;
    srand((unsigned int) time(NULL));

    {
        static const char *HelpText =
            "usage: play <infile> <arguments>\n";

        CmdLineArgs_t Args[] = {
            {CMD_LINE_ARG_TYPE_STRING, &play_input_filename, "<infile>",
            "Name of input file, raw pcm directly sent to ADM. Mandatory\n"},
            {CMD_LINE_ARG_TYPE_STRING, &play_device, "<device name>",
            "Device to play on. Default is HSetOut. Options:\n"
            "  HSetOut/Speaker/EarPiece/USBOut/BtOut/A2DPOut/FMTx/HDMI/x-ste-adm-dict-play\n"},
            {CMD_LINE_ARG_TYPE_INT, &play_max_time_ms, "-maxtime",
            "Maximum allowed time for play to finish. Used for module test.\n"},
            {CMD_LINE_ARG_TYPE_INT, &play_min_time_ms, "-mintime",
            "Minimum allowed time for play to finish. Used for module test.\n"},
            {CMD_LINE_ARG_TYPE_BOOL, &wait_key, "-waitkey",
            "Wait for keypress before exiting.\n"},
            {CMD_LINE_ARG_TYPE_BOOL, &wait2blocks, "-wait2blocks",
            "Wait for 5 sec after sending 2 blocks.\n"},
            {CMD_LINE_ARG_TYPE_INT, &samplerate, "-sr",
            "Sample rate. Default is 48000 Hz.\n"},
            {CMD_LINE_ARG_TYPE_INT, &channels, "-ch",
            "Number of channels. Default is 2.\n"},
            {CMD_LINE_ARG_TYPE_STRING, &fmtstr, "-fmt",
            "Format (coded data, e.g. AAC). Overrides -ch. Options:\n"
            "  ac3 mpeg1 mp3 mpeg2 aac dts atrac oba\n"
            "  ddplus dts_hd mat dst wma_pro mat\n"},
            {CMD_LINE_ARG_TYPE_INT, &duration_limit, "-duration",
            "Limit playback duration. Unit is milliseconds. Resolution is not tip top.\n"},
            {CMD_LINE_ARG_TYPE_BOOL, &random_drains, "-drains",
            "Do random drain calls during transfer.\n"},
            {CMD_LINE_ARG_TYPE_BOOL, &random_behavior, "-random_behavior",
            "Random allowed behaviors\n"},
            {CMD_LINE_ARG_TYPE_BOOL, &pause_after_open, "-pause_after_open",
            "Wait for input on STDIN after open before send data\n"},
            {CMD_LINE_ARG_TYPE_INT, &radio_freq, "-rf",
            "Start radio with this freq (in Hz) in transmit mode. Default is off.\n"},
            {CMD_LINE_ARG_TYPE_INT, &delay, "-delay",
            "Delay start of playback (in seconds). Used for module test.\n"},
            {CMD_LINE_ARG_TYPE_STRING, &play_device_2, "-dev2",
            "Second device to play on.\n"},
            {CMD_LINE_ARG_TYPE_INT, &play_device_2_delay, "-dev2delay",
            "Delay in ms (consumed) until second device is opened\n" },
            {CMD_LINE_ARG_TYPE_INT, &play_device_2_close_delay, "-dev2closedelay",
            "Delay in ms (consumed) until second device is closed (from start of play of dev 1)\n" },
            {CMD_LINE_ARG_TYPE_INT, &play_device_2_reopendelay, "-dev2reopendelay",
            "Delay in ms (consumed) until second device is opened again...\n" },


        };
        const int NumArgs = sizeof(Args) / sizeof(CmdLineArgs_t);
        CmdLineArgs_t ArgsCopy[NumArgs];
        memcpy(ArgsCopy, Args, sizeof(ArgsCopy));

        if (!ParseConfigHandler(argc, argv, Args, NumArgs, HelpText, NULL, 0)) {
            exitf("Error: could not parse input parameters\n");
        }

        if (parse_wav(play_input_filename, &samplerate, &channels)) {
            fprintf(stderr, "WAV format detected. %d Hz, %d channels in file\n", samplerate, channels);
            is_wav = 1;

            // Re-run ParseConfigHandler to see if sample rate / channels should be overridden
            if (!ParseConfigHandler(argc, argv, ArgsCopy, NumArgs, HelpText, NULL, 0)) {
                exitf("Error: could not parse input parameters\n");
            }
        }

        if (fmtstr) {
            channels = (int) get_format_from_string(fmtstr);
        }
    }

    const size_t byte_limit = (size_t) duration_limit * ((size_t) samplerate * (size_t) channels * 2) / 1000;

    if (radio_freq !=0) {
        if (radio_control(1, radio_freq) < 0) {
            exitf("Failed to enable radio\n");
        }
    }

    if (!play_input_filename || !play_device) exitf("filename and device must be specified\n");

    fprintf(stderr, "Filename '%s'\n", play_input_filename);
    fprintf(stderr, "Device '%s'\n", play_device);
    fprintf(stderr, "play_max_time_ms: %u play_min_time_ms: %u\n", play_max_time_ms, play_min_time_ms);

    if (channels == 6) {
        fprintf(stderr, "5.1 audio (ste-adm-test will swap)\n");
        fprintf(stderr, "  Order expected in file:       LF  RF  CF  LFE LS  RS\n");
        fprintf(stderr, "  Order expected by ADM (HDMI): LF  RF  LFE CF  LS  RS\n");
    }

    struct stat fd_stat;

    if (stat(play_input_filename, &fd_stat) < 0) exitf("stat failed error %d\n", errno);
    int fd = open(play_input_filename, O_RDONLY);
    if (fd < 0) exitf("open '%s' O_RDONLY failed, errno=%d\n", errno);
    void *buffer = mmap(0, (size_t) fd_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buffer == 0) exitf("mmap failed, errno = %d\n", errno);
    (void) close(fd);

    // Touch data so we don't get MMC reads during operation, to be able
    // to compare with LPA
    int i;
    int dummy = 0;
    for (i=0 ; i < fd_stat.st_size ; i+=4096) {
        dummy += ((char*) buffer)[i];
    }


    fprintf(stderr, "Connecting to ADM\n");

    struct timeval tv_start_open_device;
    gettimeofday(&tv_start_open_device, NULL);


    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");


    int actual_samplerate;
    ste_adm_res_t res;

    char* bufp;
    int bufsz = 8192;
    int num_bufs = 3;

    // 2 channels, 44.1 kHz: 5644 bytes / buffer
    // 2 channels, 48 kHz: 6144 bytes / buffer
    if (channels < STE_ADM_FORMAT_FIRST_CODED) {
        bufsz = samplerate * 32 / 1000 * channels * 2; // 6144
    }

    res = ste_adm_client_open_device(adm_id, play_device, samplerate, channels, &actual_samplerate, &bufp, bufsz, num_bufs);
    if (res != STE_ADM_RES_OK)
        exitf("ste_adm_client_open_device failed: '%s'\n", ste_adm_res_to_str(res));

    if (play_device_2 != NULL && play_device_2_delay == 0){
        res = ste_adm_client_open_device(adm_id, play_device_2, samplerate, channels, &actual_samplerate, &bufp, bufsz, num_bufs);
        if (res != STE_ADM_RES_OK)
            exitf("ste_adm_client_open_device failed: '%s'\n", ste_adm_res_to_str(res));
    }

    struct timeval tv_end_open_device;
    gettimeofday(&tv_end_open_device, NULL);
    unsigned open_device_time_used = tv_ms_diff(tv_end_open_device, tv_start_open_device);

    fprintf(stderr, "Using process shared memory; %d buffers of %d bytes at %X\n", num_bufs, bufsz, (int) bufp);
    fprintf(stderr, "Open ok in %d ms, actual_samplerate=%d Hz  ", open_device_time_used, actual_samplerate);
    if (channels < STE_ADM_FORMAT_FIRST_CODED) {
        fprintf(stderr, "channels %d\n", channels);
    }
    fprintf(stderr, "\n");
    if (delay != 0) {
        fprintf(stderr, "Delay start of playback %d seconds\n", delay);
        sleep((unsigned int) delay);
    }

    size_t bytes = (size_t) fd_stat.st_size;

    size_t consumed_bytes = 0;
    char *cbuf = (char *) buffer;
    int blocks=0;

    fprintf(stderr, "Playing file '%s' (%d bytes) on device '%s'\n", play_input_filename, (int) fd_stat.st_size, play_device);
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    int curbuf=0;

    if (is_wav) {
        consumed_bytes += 44; // Skip .WAV header
    }

    if (pause_after_open) {
        fprintf(stderr, "Open OK, send something to STDIN to begin playback\n");
        char buf[80];
        read(0, buf, sizeof(buf));
    }

    while (consumed_bytes < bytes) {
        size_t transfer_bytes = (size_t) bufsz;

        /* TODO: Written bytes should be returned */
        if (transfer_bytes > (bytes - consumed_bytes)) {
            transfer_bytes = bytes - consumed_bytes;
            fprintf(stderr, "\nSize of last write %d\n", transfer_bytes);
        }

        memcpy(bufp + curbuf*bufsz, &cbuf[consumed_bytes], transfer_bytes);
        if (channels == 6) {
            reorder_wav_to_hdmi(bufp + curbuf*bufsz, transfer_bytes);
        }

        if (ramp_down == 1) {
            short* sbuf = (short*) (bufp + curbuf*bufsz);
            size_t samples = transfer_bytes / ((size_t)channels * 2);
            for (i=0 ; i < (int) samples ; i++) {
                volumeQ15 -= 8;
                if (volumeQ15 < 0) volumeQ15=0;
                int c;
                for (c=0 ; c < channels ; c++) {
                    sbuf[i*channels + c] = (short) ((volumeQ15 * sbuf[i*channels + c]) / 32768);
                }
            }

            if (volumeQ15 == 0) {
                ramp_down = 2;
            }
        }

        //fprintf(stderr, "Writing buffer C (%d) T(%d) P (0x%08X)\n", consumed_bytes, transfer_bytes, (unsigned int)&cbuf[consumed_bytes]);
        int dummy_lpa_mode;
        res = ste_adm_client_send(adm_id, curbuf, (int) transfer_bytes, &dummy_lpa_mode);
        if (res == STE_ADM_RES_INCORRECT_STATE) {
            fprintf(stderr, "ste_adm_client_send --> INCORRECT_STATE, last dev closed? You can open new device.\n");
            while (play_record_stdinctl(adm_id, &bufp, samplerate, channels, bufsz, num_bufs) == 0) usleep(10000);
        } else if (res != STE_ADM_RES_OK) {
            exitf("\nste_adm_client_send failed: '%s'\n", ste_adm_res_to_str(res));
        } else {
            curbuf = (curbuf+1) % num_bufs;
        }

        if (random_drains && rand() % 17 == 2) {
            fprintf(stderr, "\nRandom drain...");
            fflush(stderr);

            res = ste_adm_client_drain(adm_id);
            if (res != STE_ADM_RES_OK)
                exitf("ste_adm_client_drain failed: '%s'\n", ste_adm_res_to_str(res));

            fprintf(stderr, "\nRandom drain DONE\n");
            fflush(stderr);
        }

        fprintf(stderr, ".");
        fflush(stderr);

        consumed_bytes += transfer_bytes;
        blocks++;

        if (wait2blocks && blocks == 2) usleep(5 * 1000 * 1000);

        uint_t consumed_time = (uint_t) ((long long) 1000 * consumed_bytes / (samplerate * channels * 2));
        if (play_device_2 && play_device_2_delay > 0 && consumed_time > play_device_2_delay) {
            fprintf(stderr, "Consumed %u ms, opening second device\n", consumed_time);
            res = ste_adm_client_open_device(adm_id, play_device_2, samplerate, channels, &actual_samplerate, &bufp, bufsz, num_bufs);
            if (res != STE_ADM_RES_OK) {
               exitf("ste_adm_client_open_device failed: '%s'\n", ste_adm_res_to_str(res));
            }
            play_device_2_delay = 0;
        }

        if (play_device_2 && play_device_2_close_delay > 0 && consumed_time > play_device_2_close_delay) {
            fprintf(stderr, "Consumed %u ms, closing second device\n", consumed_time);
            ste_adm_close_device(adm_id, play_device_2);
            play_device_2_close_delay = 0;
        }

        if (play_device_2 && play_device_2_reopendelay > 0 && consumed_time > play_device_2_reopendelay) {
            fprintf(stderr, "Consumed %u ms, re-opening second device\n", consumed_time);
            res = ste_adm_client_open_device(adm_id, play_device_2, samplerate, channels, &actual_samplerate, &bufp, bufsz, num_bufs);
            if (res != STE_ADM_RES_OK) {
               exitf("ste_adm_client_open_device failed: '%s'\n", ste_adm_res_to_str(res));
            }
            play_device_2_reopendelay=0;
        }

        if (!ramp_down && byte_limit && consumed_bytes >= byte_limit) {
            fprintf(stderr, "Duration limit reached (%d bytes, %d ms), ramping down\n", consumed_bytes, consumed_bytes * 1000 / (size_t) (samplerate * channels * 2));
            ramp_down = 1;
        }

        if (ramp_down == 2) {
            fprintf(stderr, "Ramping down complete\n");
            break;
        }

        play_record_stdinctl(adm_id, &bufp, samplerate, channels, bufsz, num_bufs);
    }

    fprintf(stderr, "\n");
    fflush(stderr);

    fprintf(stderr, "Draining...\n");
    fflush(stderr);

    if (!random_behavior || rand() % 17 > 8) {
        res = ste_adm_client_drain(adm_id);
        if (res != STE_ADM_RES_OK)
            exitf("ste_adm_client_drain failed: '%s'\n", ste_adm_res_to_str(res));

        fprintf(stderr, "Draining DONE\n");
        fflush(stderr);
    }

    if (random_behavior && rand() % 3 == 1) {
        usleep(400 * 1000);
    }

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    unsigned time_used = tv_ms_diff(tv_end, tv_start);

    if (!random_behavior || rand() % 17 > 8) {
        res = ste_adm_close_device(adm_id, play_device);

        if (res != STE_ADM_RES_OK)
            exitf("ste_adm_close_device failed: '%s'\n", ste_adm_res_to_str(res));

        fprintf(stderr, "CloseDevice DONE\n");
        fflush(stderr);
    }


    if (wait_key) {
        fprintf(stderr, "Send a char to STDIN to continue (which will close the device)\n");
        char c;
        read(0, &c, 1);
    }
    if (time_used == 0) exitf("timer error likely");

    if (is_wav) {
        consumed_bytes -= 44; // Skip .WAV header in data transfered calculation
    }

    fprintf(stderr, "Sending %u bytes completed in %u ms (%u bytes / sec)\n", (unsigned int)consumed_bytes , time_used, (unsigned int)((unsigned long long)consumed_bytes * 1000 / (time_used+1)) );

    if (channels < STE_ADM_FORMAT_FIRST_CODED) {
        fprintf(stderr, "Expected: %u bytes / sec\n", samplerate * channels * 2);
    }

    if (time_used < play_min_time_ms) exitf("Data transferred too fast\n");
    if (time_used > play_max_time_ms) exitf("Data transferred too slow\n");


    munmap(buffer, (size_t) fd_stat.st_size);
    munmap(bufp, (size_t) (bufsz * num_bufs));

    return 0;
}

typedef struct play_info
{
  int admfd;
  char* inbuf;
  int num_bufs;
  int bufsz;
  int idx;
  char* admbuf;
  pthread_t thread;
  pthread_mutex_t mutex;
} play_info_t;

void* play_thread(void*p)
{
    int curbuf = 0;
    play_info_t* this = (play_info_t*) p;
    while(1) {
        usleep(1);
        pthread_mutex_lock(&this->mutex);
        int dummy_lpa_mode;
        ste_adm_res_t res = ste_adm_client_send(this->admfd, curbuf, this->bufsz, &dummy_lpa_mode);
        curbuf = (curbuf+1) % this->num_bufs;
        pthread_mutex_unlock(&this->mutex);
        if (res != STE_ADM_RES_OK) {
            fprintf(stderr, "ste_adm_client_send to fd %d failed %s, exiting play_thread\n", this->admfd, ste_adm_res_to_str(res));
            return 0;
        }
    }
}

int crossplay(int argc, char** argv)
{
    play_info_t p1, p2;
    pthread_mutex_init(&p1.mutex, 0);
    pthread_mutex_init(&p2.mutex, 0);

    p1.admfd = ste_adm_client_connect();
    if (p1.admfd < 0) exitf("ste_adm_client_connect failed\n");

    p2.admfd = ste_adm_client_connect();
    if (p2.admfd < 0) exitf("ste_adm_client_connect failed\n");



    // 2 channels, 44.1 kHz: 5644 bytes / buffer
    // 2 channels, 48 kHz: 6144 bytes / buffer
    int samplerate = 48000;
    int channels   = 2;

    ste_adm_res_t res;
    p1.num_bufs = p2.num_bufs = 3;
    p1.bufsz    = p2.bufsz    = samplerate * 32 / 1000 * channels * 2; // 6144

    int actual_samplerate;
    res = ste_adm_client_open_device(p1.admfd, "HSetOut", samplerate, channels, &actual_samplerate, &p1.admbuf, p1.bufsz, p1.num_bufs);
    if (res != STE_ADM_RES_OK) exitf("ste_adm_client_open_device failed: '%s'\n", ste_adm_res_to_str(res));

    res = ste_adm_client_open_device(p2.admfd, "Speaker", samplerate, channels, &actual_samplerate, &p2.admbuf, p2.bufsz, p2.num_bufs);
    if (res != STE_ADM_RES_OK) exitf("ste_adm_client_open_device failed: '%s'\n", ste_adm_res_to_str(res));

    pthread_create(&p1.thread, 0, play_thread, (void*) &p1);
    pthread_create(&p2.thread, 0, play_thread, (void*) &p2);

    int iterations = 10000;
    while(iterations--) {

        usleep(1000 * 1000);

        int method = 2;

        fprintf(stderr, "Opening cross devices while playing, %d to go\n", iterations);
        char* dummy;

        if (method == 1) {
            pthread_mutex_lock(&p1.mutex);
            fprintf(stderr, "Opening additional route to speaker..\n");
            ste_adm_client_open_device(p1.admfd, "Speaker", samplerate, channels, &actual_samplerate, &dummy, p1.bufsz, p1.num_bufs);
            fprintf(stderr, "DONE\n");
            pthread_mutex_unlock(&p1.mutex);

            pthread_mutex_lock(&p2.mutex);
            fprintf(stderr, "Opening additional route to HSetOut..\n");
            ste_adm_client_open_device(p2.admfd, "HSetOut", samplerate, channels, &actual_samplerate, &dummy, p2.bufsz, p2.num_bufs);
            fprintf(stderr, "DONE\n");
            pthread_mutex_unlock(&p2.mutex);
        } else {
            pthread_mutex_lock(&p1.mutex);
            pthread_mutex_lock(&p2.mutex);
            fprintf(stderr, "Opening additional route to speaker..\n");
            ste_adm_client_open_device(p1.admfd, "Speaker", samplerate, channels, &actual_samplerate, &dummy, p1.bufsz, p1.num_bufs);
            fprintf(stderr, "Opening additional route to HSetOut..\n");
            ste_adm_client_open_device(p2.admfd, "HSetOut", samplerate, channels, &actual_samplerate, &dummy, p2.bufsz, p2.num_bufs);
            fprintf(stderr, "DONE\n");
            pthread_mutex_unlock(&p2.mutex);
            pthread_mutex_unlock(&p1.mutex);
        }

        fprintf(stderr, "Done, now playing for a while\n");
        usleep(5 * 1000 * 1000);

        fprintf(stderr, "Now closing devices...\n");
        pthread_mutex_lock(&p1.mutex);
        pthread_mutex_lock(&p2.mutex);
        ste_adm_close_device(p1.admfd, "Speaker");
        ste_adm_close_device(p2.admfd, "HSetOut");
        pthread_mutex_unlock(&p2.mutex);
        pthread_mutex_unlock(&p1.mutex);
    }

    close(p1.admfd);
    close(p2.admfd);

    void* dummy_result;
    pthread_join(p1.thread, &dummy_result);
    pthread_join(p2.thread, &dummy_result);

    return 0;
}


int quickcheck_nosilence_mono(short* inbuf, int samples)
{
  if (samples == 0) return 0;
  int sample = 1;
  for ( ; sample < samples ; sample++)
    if (inbuf[0] != inbuf[sample])
        return 1;

  return 0;
}

int quickcheck_nosilence_stereo(short* inbuf, unsigned int samples)
{
  if (samples == 0) return 0;
  unsigned int sample = 1;
  for ( ; sample < samples ; sample++)
    if (inbuf[0] != inbuf[2*sample] && inbuf[1] != inbuf[2*sample+1])
        return 1;

  return 0;
}

int set_dictaphone_mode(int argc, char** argv)
{
    if (argc != 1) {
        fprintf(stderr, "args: <path>\n");
        fprintf(stderr, " path : %d=uplink %d=downlink %d=both.\n", STE_ADM_DICTAPHONE_UPLINK, STE_ADM_DICTAPHONE_DOWNLINK, STE_ADM_DICTAPHONE_UPLINK_DOWNLINK);
        exit(0);
    }

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    if (ste_adm_client_set_cscall_dictaphone_mode(atoi(argv[0])) < 0)
        exitf("ste_adm_client_set_cscall_dictaphone_mode('%s') failed\n", argv[0]);

    return 0;
}

int set_dictaphone_mute(int argc, char** argv)
{
    if (argc != 1) {
        fprintf(stderr, "args: <mute state>\n");
        fprintf(stderr, " mute state : %d=unmute %d=mute\n", 0, 1);
        fprintf(stderr, "Note : only fat modem supported, yet\n");
        exit(0);
    }

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    if (ste_adm_client_set_cscall_dictaphone_mute(atoi(argv[0])) < 0)
        exitf("ste_adm_client_set_cscall_dictaphone_mute('%s') failed\n", argv[0]);

    return 0;
}

int get_dictaphone_mute(int argc, char** argv)
{
    int mute;
    if (argc > 0) {
        fprintf(stderr, "get_dictaphone_mute : no parameters supported\n");
        exit(0);
    }

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    if (ste_adm_client_get_cscall_dictaphone_mute(&mute) < 0)
        exitf("ste_adm_client_get_cscall_dictaphone_mute() failed\n");

    fprintf(stderr, "Dictaphone mute state %d\n", mute);

    return 0;
}

int record(int argc, char** argv)
{
    static const char *HelpText =
        "usage: record <outfile> <arguments>\n";

    const char* record_filename = NULL;
    const char* record_device = "HSetIn";
    unsigned int record_max_time_ms = 0xFFFFFFFF;
    unsigned int record_min_time_ms = 0;
    unsigned int bytes_to_record = 32000;
    int no_stereo_silence = 0;
    int samplerate = 8000;
    int channels = 1;
    int wait_key = 0;
    int radio_freq = 0;
    int random_behavior = 0;
    int delay=0;

    CmdLineArgs_t Args[] = {
        {CMD_LINE_ARG_TYPE_STRING, &record_filename, "<output>",
        "Name of output file, raw pcm directly sent from ADM. Mandatory\n"},
        {CMD_LINE_ARG_TYPE_STRING, &record_device, "<device name>",
        "Device to record from. Mic USBIn HSetIn BtIn FMRx x-ste-adm-dict-rec\n"},
        {CMD_LINE_ARG_TYPE_INT, &record_max_time_ms, "-maxtime",
        "Maximum allowed time for play to finish. Used for module test.\n"},
        {CMD_LINE_ARG_TYPE_INT, &record_min_time_ms, "-mintime",
        "Minimum allowed time for play to finish. Used for module test.\n"},
        {CMD_LINE_ARG_TYPE_INT, &bytes_to_record, "-bytes",
        "Minimum allowed time for play to finish. Used for module test.\n"},
        {CMD_LINE_ARG_TYPE_INT, &samplerate, "-sr",
        "Sample rate. Default is 8000 Hz.\n"},
        {CMD_LINE_ARG_TYPE_INT, &channels, "-ch",
        "Number of channels. Default is 1.\n"},
        {CMD_LINE_ARG_TYPE_INT, &radio_freq, "-rf",
        "Start radio with this freq (in Hz) in recv mode. Default is off.\n"},
        {CMD_LINE_ARG_TYPE_BOOL, &wait_key, "-waitkey",
        "Wait for keypress before exiting.\n"},
        {CMD_LINE_ARG_TYPE_BOOL, &random_behavior, "-random_behavior",
        "Random allowed behaviors\n"},
        {CMD_LINE_ARG_TYPE_BOOL, &no_stereo_silence, "-no_stereo_silence",
        "If set, requires that recorded data contains non-silence to succeed. \n"},
        {CMD_LINE_ARG_TYPE_INT, &delay, "-delay",
        "Delay start of recording (in seconds). Used for module test.\n"},

    };
    const int NumArgs = sizeof(Args) / sizeof(CmdLineArgs_t);

    if (!ParseConfigHandler(argc, argv, Args, NumArgs, HelpText, NULL, 0)) {
        fprintf(stderr, "Error: could not parse input parameters\n");
        return 0;
    }

    if (!record_filename || !record_device) exitf("filename and device must be specified\n");


    if (radio_freq !=0) {
        if (radio_control(0, radio_freq) < 0) {
            exitf("Failed to enable radio\n");
        }
    }

    fprintf(stderr, "Connecting to ADM\n");

    struct timeval tv_start_open_device;
    gettimeofday(&tv_start_open_device, NULL);

    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    char* bufp;
    int bufsz = 4096;
    int num_bufs = 4;
    ste_adm_res_t res;
    res = ste_adm_client_open_device(adm_id, record_device, samplerate, channels, NULL, &bufp, bufsz, num_bufs);
    if (res != STE_ADM_RES_OK)
        exitf("ste_adm_client_open_device failed: '%s'\n", ste_adm_res_to_str(res));

    fprintf(stderr, "Using process shared memory; %d buffers of %d bytes at %X\n", num_bufs, bufsz, (int) bufp);

    struct timeval tv_end_open_device;
    gettimeofday(&tv_end_open_device, NULL);
    unsigned open_device_time_used = tv_ms_diff(tv_end_open_device, tv_start_open_device);


    if (delay != 0) {
        fprintf(stderr, "Delay start of recording %d seconds\n", delay);
        sleep((unsigned int) delay);
    }

    fprintf(stderr, "Opened device in %d ms. Will record %u bytes..\n", open_device_time_used, bytes_to_record);

    FILE* out = fopen(record_filename, "wb");
    if (!out) exitf("Failed to open output file '%s'\n", record_filename);


    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    unsigned int bytes_recorded = 0;
    int curbuf = 0;
    while (bytes_recorded < bytes_to_record) {
        res = ste_adm_client_receive(adm_id, &curbuf);
        if (res == STE_ADM_RES_INCORRECT_STATE) {
            fprintf(stderr, "ste_adm_client_receive --> INCORRECT_STATE, last dev closed? You can open new device.\n");
            while (play_record_stdinctl(adm_id, &bufp, samplerate, channels, bufsz, num_bufs) == 0) usleep(10000);
        } else if (res != STE_ADM_RES_OK)
            exitf("\nste_adm_client_receive failed: '%s'\n", ste_adm_res_to_str(res));


        if (fwrite(bufp + bufsz*curbuf, (size_t) bufsz, 1, out) != 1)
            exitf("\nfwrite failed\n");

        if (no_stereo_silence)
            if (quickcheck_nosilence_stereo((short*) (bufp + bufsz*curbuf), (size_t) bufsz/4))
                no_stereo_silence = 0;

        fprintf(stderr, ".");
        fflush(stderr);

        bytes_recorded += (unsigned int) bufsz;

        play_record_stdinctl(adm_id, &bufp, samplerate, channels, bufsz, num_bufs);
    }

    fprintf(stderr, "\n");
    fflush(stderr);

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    unsigned time_used = tv_ms_diff(tv_end, tv_start);


    if (!random_behavior || rand() % 17 > 8) {
        res = ste_adm_close_device(adm_id, record_device);

        if (res != STE_ADM_RES_OK)
            exitf("ste_adm_close_device failed: '%s'\n", ste_adm_res_to_str(res));

        fprintf(stderr, "CloseDevice DONE\n");
        fflush(stderr);
    }


    if (no_stereo_silence) exitf("Failed, nonsilence not detected\n");

    fprintf(stderr, "Send of %u bytes completed in %u ms (%u bytes / sec)\n", bytes_recorded , time_used, bytes_recorded * 1000 / (time_used+1) );
    if (wait_key) {
        fprintf(stderr, "Send a char to STDIN to continue (which will close the device)\n");
        char c;
        read(0, &c, 1);
    }
    if (time_used == 0) exitf("timer error likely");

    if (time_used < record_min_time_ms) exitf("Data transferred too fast\n");
    if (time_used > record_max_time_ms) exitf("Data transferred too slow\n");


    if (fclose(out) != 0) exitf("fclose() failed\n");

    munmap(bufp, (size_t) (bufsz * num_bufs));


    return EXIT_SUCCESS;
}

int mute_upstream(int argc, char** argv)
{
    if (argc == 0 || strcmp(argv[0], "?") == 0) {
        fprintf(stderr, "0=mute off  1=mute on\n");
        exit(0);
    }

    int result = ste_adm_client_set_cscall_upstream_mute(atoi(argv[0]));
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}


int mute_downstream(int argc, char** argv)
{
    if (argc == 0 || strcmp(argv[0], "?") == 0) {
        fprintf(stderr, "0=mute off  1=mute on\n");
        exit(0);
    }

    int result = ste_adm_client_set_cscall_downstream_mute(atoi(argv[0]));
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

int set_csloop(int argc, char** argv)
{
    int codectype = 0;

    if (argc == 0 || strcmp(argv[0], "?") == 0) {
#ifndef ADM_ENABLE_FEATURE_FAT_MODEM
        fprintf(stderr, "csloop [0=disable 1=enable] [codec type]\n");

#else
        fprintf(stderr, "Slim Modem : csloop [0=disable 1=enable] [codec type]\n");
        fprintf(stderr, "Fat Modem  : csloop [loop type] [codec type]\n");
        fprintf(stderr, " loop type \n");
        fprintf(stderr, " 0           disabled - (codec type not used)\n");
        fprintf(stderr, " 1           loopback in APE cscall component - (codec type not used)\n");
        fprintf(stderr, " 2           loopback in APE MSP4 - (codec type not used)\n");
        fprintf(stderr, " 3           loopback in modem (IIS in to IIS out) - (codec type not used)\n");
        fprintf(stderr, " 4           loopback in modem via speech proc - (codec type not used)\n");
        fprintf(stderr, " 5           loopback in modem via speech codec\n\n");
#endif
        fprintf(stderr, " CodecType   CodingType  BandMode\n");
        fprintf(stderr, " 0           AMR         AMRBandModeNB0\n");
        fprintf(stderr, " 1           AMR         AMRBandModeNB1\n");
        fprintf(stderr, " 2           AMR         AMRBandModeNB2\n");
        fprintf(stderr, " 3           AMR         AMRBandModeNB3\n");
        fprintf(stderr, " 4           AMR         AMRBandModeNB4\n");
        fprintf(stderr, " 5           AMR         AMRBandModeNB5\n");
        fprintf(stderr, " 6           AMR         AMRBandModeNB6\n");
        fprintf(stderr, " 7           AMR         AMRBandModeNB7\n");
        fprintf(stderr, " 8           GSMEFR\n");
        fprintf(stderr, " 9           GSMHR\n");
        fprintf(stderr, " 10          GSMFR\n");

        exit(0);
    }
    if(argc<2)
        codectype = 0;
    else
        codectype = atoi(argv[1]);
    int result = ste_adm_set_cscall_loopback_mode(atoi(argv[0]),codectype );
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}


int vc_status(int argc, char** argv)
{
    int upstream_mute;
    int downstream_mute;
    int upstream_volume;
    int downstream_volume;
    ste_adm_tty_mode_t tty;

    int result = ste_adm_client_get_cscall_upstream_mute(&upstream_mute);

    (void) argc;
    (void) argv;

    if (result != 0) {
        fprintf(stderr, "ste_adm_client_get_cscall_upstream_mute failed\n");
        return EXIT_FAILURE;
    }

    result = ste_adm_client_get_cscall_downstream_mute(&downstream_mute);
    if (result != 0) {
        fprintf(stderr, "ste_adm_client_get_cscall_downstream_mute failed\n");
        return EXIT_FAILURE;
    }

    result = ste_adm_client_get_cscall_upstream_volume(&upstream_volume);
    if (result != 0) {
        fprintf(stderr, "ste_adm_client_get_cscall_upstream_volume failed\n");
        return EXIT_FAILURE;
    }

    result = ste_adm_client_get_cscall_downstream_volume(&downstream_volume);
    if (result != 0) {
        fprintf(stderr, "ste_adm_client_get_cscall_downstream_volume failed\n");
        return EXIT_FAILURE;
    }

    result = ste_adm_client_get_cscall_tty_mode(&tty);
    if (result != 0) {
        fprintf(stderr, "ste_adm_client_get_cscall_tty_mode failed\n");
        // return EXIT_FAILURE; the call is not implemented yet; ignore
    }

    fprintf(stderr, "VC Mute status: upstream=%s downstream=%s\n",
        upstream_mute   ? "enabled" : "disabled",
        downstream_mute ? "enabled" : "disabled");

    // Nice prints
    const unsigned int dbg_vol_text_max_len = 1 + sizeof(int)*3 + 4;
    char dbg_up_vol_text[dbg_vol_text_max_len];
    if (upstream_volume == INT_MIN) {
        strcpy(dbg_up_vol_text, "MUTE");
    } else {
        snprintf(dbg_up_vol_text, dbg_vol_text_max_len, "%d mB", upstream_volume);
    }

    char dbg_down_vol_text[dbg_vol_text_max_len];
    if (downstream_volume == INT_MIN) {
        strcpy(dbg_down_vol_text, "MUTE");
    } else {
        snprintf(dbg_down_vol_text, dbg_vol_text_max_len, "%d mB", downstream_volume);
    }

    fprintf(stderr, "VC Volume: upstream=%s  downstream=%s\n", dbg_up_vol_text, dbg_down_vol_text);
    fprintf(stderr, "VC TTY: %d\n", tty);

    return EXIT_SUCCESS;
}


void remap_devices(const char* in_dev, const char* out_dev )
{

    if(strcmp(in_dev,"Mic")==0){
        if(strcmp(out_dev,"Speaker")==0){
            ste_adm_set_toplevel_map_live("Speaker","REF_Speaker", NULL, NULL);
            ste_adm_set_toplevel_map_live("Mic","REF_Mic_Speaker", NULL, NULL);
        }else if(strcmp(out_dev,"EarPiece")==0){
            ste_adm_set_toplevel_map_live("EarPiece","REF_Earpiece", NULL, NULL);
            ste_adm_set_toplevel_map_live("Mic","REF_Mic_Earpiece", NULL, NULL);
        } else if(strcmp(out_dev,"HSetOut")==0){
            ste_adm_set_toplevel_map_live("HSetOut","REF_HSetOut", NULL, NULL);
            ste_adm_set_toplevel_map_live("Mic","REF_Mic_HSetOut", NULL, NULL);
        }
    }
}

int voicecall(int argc, char** argv)
{
    if (argc == 1) {
        fprintf(stderr, "<indev: HSetIn, Mic, USBIn , BTIn> <outdev: Speaker, HSetOut, EarPiece, BTOut> <voip_mode: 0=disabled (default), 1=enabled> <voip_samplerate: 8000, 16000>\n");
        fprintf(stderr, "If given no arguents, VC will be opened for HSetIn and HSetOut\n");
        exit(0);
    }

    const char* in_dev  = (argc >= 2) ? argv[0] : "HSetIn";
    const char* out_dev = (argc >= 2) ? argv[1] : "HSetOut";
    ste_adm_voip_mode_t mode;

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("Failed to connect to the ADM\n");

    int voip_enabled = 0;
    int voip_disable = 0;
    int voip_samplerate = 16000;
    if (argc >= 3) {
        voip_enabled = atoi(argv[2]);
        fprintf(stderr, "VoIP mode = %d\n", voip_enabled);

    }
    if (argc >= 4) {
        voip_samplerate = atoi(argv[3]);
        fprintf(stderr, "VoIP samplerate = %d\n", voip_samplerate);
    }
    mode.in_samplerate = voip_samplerate;
    mode.out_samplerate = voip_samplerate;
    mode.in_channels =1;
    mode.out_channels = 2;

    /* first remap device to expected device */
    reinit_topdevice_to_default_device();

    /* Perform map relive according to Input / output selection if needed */
    remap_devices(in_dev,out_dev);
    ste_adm_res_t res;
    if (voip_enabled) {
        res = ste_adm_client_set_cscall_voip_mode(voip_enabled, &mode);
        if (res != STE_ADM_RES_OK)
            exitf("ste_adm_client_set_cscall_voip_mode failed: '%s'\n", ste_adm_res_to_str(res));
    }
    res = ste_adm_set_cscall_devices(adm_id, in_dev, out_dev);
    if (res != STE_ADM_RES_OK)
        exitf("ste_adm_set_cscall_devices failed: '%s'\n", ste_adm_res_to_str(res));

    fprintf(stderr, "CScall set to [%s, %s]\n",in_dev, out_dev);
    fprintf(stderr, "CSCall will remain requested until this session is closed\n");
    fprintf(stderr, "To re-route, input a pair of devices on stdin\n");
    fprintf(stderr, "To exit, input the single word 'exit' on stdin\n");
    fprintf(stderr, "To close voicecall, input another single word on stdin\n");
    fprintf(stderr, "Common pairs: \n");
    fprintf(stderr, "  HSetIn HSetOut\n");
    fprintf(stderr, "  Mic    Speaker\n");
    fprintf(stderr, "  Mic    EarPiece\n");
    fprintf(stderr, "  BTIn   BTOut\n");
    fprintf(stderr, "  TTYIn  TTYOut\n");


    while(1) {
        char line[80];

        int pos=0;
        while(pos < 79) {
            char ch;
            read(0, &ch, 1);
            line[pos++] = ch;
            if (ch == '\n') break;
        }
        line[pos] = 0;


        char* strtok_tmp;
        in_dev  = strtok_r(line, " \n", &strtok_tmp);
        out_dev = strtok_r(NULL, " \n", &strtok_tmp);

        if (in_dev && strcmp("exit",in_dev) == 0 && !out_dev) {
            fprintf(stderr, "Ending voicecall and exiting ste-adm-test\n");
            return 0;
        }
        if (in_dev && !out_dev) {
            fprintf(stderr, "Ending voicecall\n");
            res = ste_adm_set_cscall_devices(adm_id, "", NULL);
            if (res != STE_ADM_RES_OK)
                fprintf(stderr, "ste_adm_set_cscall_devices failed: '%s'\n", ste_adm_res_to_str(res));
            voip_disable = 1;
        }
        if (in_dev && out_dev) {
            fprintf(stderr, "New routing requested: [%s, %s]\n", in_dev, out_dev);
            if (voip_enabled && voip_disable) {
                res = ste_adm_client_set_cscall_voip_mode(voip_enabled, &mode);
                if (res != STE_ADM_RES_OK)
                    exitf("ste_adm_client_set_cscall_voip_mode failed: '%s'\n", ste_adm_res_to_str(res));
                voip_disable = 0;
            }
            remap_devices(in_dev,out_dev);
            res = ste_adm_set_cscall_devices(adm_id, in_dev, out_dev);
            if (res != STE_ADM_RES_OK)
                fprintf(stderr, "ste_adm_set_cscall_devices failed: '%s'\n", ste_adm_res_to_str(res));
        }
    }



    return 0;
}

int vc_set_tty_mode(int argc, char** argv)
{
    (void) argv;
    if (argc == 0 || strcmp(argv[0], "?") == 0) {
        fprintf(stderr, "vc_set_tty_mode: 0 off, 1 full, 2 HCO, 3 VCO\n");
        exit(0);
    }

    int mode = atoi(argv[0]);

    if (ste_adm_client_set_cscall_tty_mode(mode))
        exitf("ste_adm_client_set_cscall_tty_mode(%d) failed\n", mode);

    return 0;
}


int vctx_volume(int argc, char** argv)
{
    if (argc == 0 || strcmp(argv[0], "?") == 0) {
        fprintf(stderr, "Set VCTX (upstream) volume. Unit is mB (or use word 'mute')\n");
        exit(0);
    }

    int volume;
    if (strcasecmp(argv[0], "mute") == 0)
        volume = INT_MIN;
    else
        volume = atoi(argv[0]);

    if (ste_adm_client_set_cscall_upstream_volume(volume))
        exitf("ste_adm_client_set_cscall_upstream_volume failed\n");

    return 0;
}

int vcrx_volume(int argc, char** argv)
{
    if (argc == 0 || strcmp(argv[0], "?") == 0) {
        fprintf(stderr, "Set VCRX (downstream) volume. Unit is mB (or use word 'mute')\n");
        exit(0);
    }

    int volume;
    if (strcasecmp(argv[0], "mute") == 0)
        volume = INT_MIN;
    else
        volume = atoi(argv[0]);

    if (ste_adm_client_set_cscall_downstream_volume(volume))
        exitf("ste_adm_client_set_cscall_downstream_volume failed\n");

    return 0;
}

int setappvol(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "args: <device> <volume in mB, [-10000,0] or the word 'mute'>\n");
        fprintf(stderr, "Devices: OUT Speaker/HSetOut/EarPiece/BtOut/A2DPOut/FMTx/HDMI\n");
        fprintf(stderr, "         IN   HSetIn, Mic, BtIn, FMRx\n");
        exit(0);
    }

    int volume;
    if (strcasecmp(argv[1], "mute") == 0)
        volume = INT_MIN;
    else
        volume = atoi(argv[1]);

    if (ste_adm_client_set_app_volume(argv[0], volume))
        exitf("ste_adm_client_set_app_volume failed\n");

    return EXIT_SUCCESS;
}

int getappvol(int argc, char** argv)
{
    if (argc != 1) {
        fprintf(stderr, "args: <device>\n");
        fprintf(stderr, "Devices: OUT Speaker/USBOut/HSetOut/EarPiece/BtOut/A2DPOut/FMTx/HDMI\n");
        fprintf(stderr, "         IN  HSetIn, Mic, BtIn, FMRx\n");
        exit(0);
    }

    int volume;
    if (ste_adm_client_get_app_volume(argv[0], &volume))
        exitf("ste_adm_client_set_app_volume failed\n");

    if (volume == INT_MIN)
        fprintf(stderr, "Volume: MUTE\n");
    else
        fprintf(stderr, "Volume: %d mB\n", volume);
    return EXIT_SUCCESS;
}

int map(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "<toplevel device> <actual device>\n");
        fprintf(stderr, " toplevel names: Speaker, USBOut , EarPiece, HSetOut, Mic, USBIn , HSetIn, FMTX, FMRX\n");
        fprintf(stderr, " standard devices:\n");
        fprintf(stderr, "   REF_Speaker REF_HSetOut REF_Earpiece REF_Mic    REF_HSetIn  REF_FMOut\n");
        fprintf(stderr, "   REF_FMIn    REF_BTOut   REF_BTIn     REF_A2DP   REF_VibraL  REF_VibraR\n");
        fprintf(stderr, "   REF_HDMI    REF_HDMI_51 REF_TTYOut   REF_TTYIn  REF_USBOut  REF_USBIn\n");
        fprintf(stderr, " test devices: REF_StereoMic\n");

        exit(0);
    }

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    if (ste_adm_client_set_toplevel_map(argv[0], argv[1]) < 0)
        exitf("ste_adm_client_set_toplevel_map('%s', '%s') failed\n", argv[0], argv[1]);

    return 0;
}


int livemap(int argc, char** argv)
{
    if (argc != 2 && argc != 4) {
        fprintf(stderr, "<toplevel device> <actual device> <toplevel device 2 (opt)> <actual device 2 (opt)>\n");
        fprintf(stderr, " toplevel names: Speaker, EarPiece, HSetOut, Mic, USBIn , HSetIn, FMTX, FMRX\n");
        fprintf(stderr, " standard devices:\n");
        fprintf(stderr, "   REF_Speaker REF_HSetOut REF_Earpiece REF_Mic    REF_HSetIn  REF_FMOut\n");
        fprintf(stderr, "   REF_FMIn    REF_BTOut   REF_BTIn     REF_A2DP   REF_VibraL  REF_VibraR\n");
        fprintf(stderr, "   REF_HDMI    REF_HDMI_51 REF_TTYOut   REF_TTYIn  REF_USBOut  REF_USBIn\n\n");

        fprintf(stderr,
            " * The graph of the previous device and the new device must be compatible\n"
            " * to allow this operation. This mean that, for both devices (old device\n"
            " * and new device), the following must be true:\n"
            " *  - the settings for the sink/source must be the same in both devices\n"
            " *  - both devices must contain the same effects, in the same order, in the\n"
            " *    voicecall-, app- and common chains.\n"
            " *  - for each effect:\n"
            " *    - the set of parameters of OMX_SetParameter kind must be the same\n"
            " *      for both devices\n"
            " *    - the contents of the parameters of OMX_SetParameter kind must be\n"
            " *      the same for both devices\n"
            " *    - the set of parameters of OMX_SetConfig kind must be the same for\n"
            " *      both devices\n");

        exit(0);
    }

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    const char* toplevel2 = "";
    const char* actual2   = "";
    if (argc == 4) {
        toplevel2 = argv[2];
        actual2   = argv[3];
    }

    ste_adm_res_t res = ste_adm_set_toplevel_map_live(argv[0], argv[1], toplevel2, actual2);
    if (res != STE_ADM_RES_OK)
        exitf("ste_adm_client_set_toplevel_map_live('%s', '%s', '%s', '%s') failed, err %s\n", argv[0], argv[1], toplevel2, actual2, ste_adm_res_to_str(res));

    return 0;
}

int map_list() {

  char *toplevel_device[] = {"Speaker", "EarPiece", "USBOut" , "USBIn" , "HSetOut", "Mic", "HSetIn", "FMTX", "FMRX"};
  char actual_device[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];
  const int iter = 9;
  int i;

  for(i = 0; i < iter; i++) {
    if (ste_adm_client_get_toplevel_map(toplevel_device[i], actual_device) < 0) {
      fprintf(stderr, "ste_adm_client_get_toplevel_map for '%s' failed\n", toplevel_device[i]);
    }
    else {
      fprintf(stderr, "Top level device '%s' is mapped to actual device '%s'.\n", toplevel_device[i], actual_device);
    }
  }
  return 0;
}

int get_toplevel_device(int argc, char** argv) {

    if (argc != 1) {
        fprintf(stderr, "<actual device>\n");
        exit(0);
    }

    char toplevel_device[STE_ADM_MAX_DEVICE_NAME_LENGTH+1];

    if (ste_adm_client_get_toplevel_device(argv[0], toplevel_device) < 0) {
        fprintf(stderr, "ste_adm_client_get_toplevel_device for '%s' failed\n", argv[0]);
    } else {
        fprintf(stderr, "Top level device associated with '%s' is: '%s'\n", argv[0], toplevel_device);
    }

    return 0;
}

int d2d(int argc, char** argv)
{
    /* first remap device to expected device */
    reinit_topdevice_to_default_device();

    ste_adm_dev2dev_flags_t flags;
    if (argc != 3) {
        fprintf(stderr, "<src> <dst> <loopType 0=>logical 1=>hw>\n");
        exit(0);
    }

    switch (atoi(argv[2])){
        case 0:
            flags = STE_ADM_D2D_FLAG_LOGICAL;
            break;
        case 1:
            flags = STE_ADM_D2D_FLAG_HW;
            break;
        default:
            fprintf(stderr, "loopType %i not supported, possible valules 0=>logical or 1=>hw>\n",atoi(argv[2]));
            exit(0);
    }

    int adm_id = ste_adm_dev2dev_connect(argv[0], argv[1], &flags);
    if (adm_id < 0) exitf("ste_adm_dev2dev_connect failed with (%d)\n", adm_id);

    fprintf(stderr, "d2d active. Send a char to stdin to close\n");
    char c;
    read(0, &c, 1);

    if (ste_adm_client_disconnect(adm_id))
        fprintf(stderr, "Unclean disconnect\n");

    return 0;
}

int play_tones(int argc, char** argv)
{
    (void) argc;
    (void) argv;
    struct {
        int id;
        const char* name;
    } tones[] = {
        {STE_ADM_COMFORT_TONE_DIAL, "STE_ADM_COMFORT_TONE_DIAL"},
        {STE_ADM_COMFORT_TONE_SUBSCRIBER_BUSY, "STE_ADM_COMFORT_TONE_SUBSCRIBER_BUSY"},
        {STE_ADM_COMFORT_TONE_CONGESTION, "STE_ADM_COMFORT_TONE_CONGESTION"},
        {STE_ADM_COMFORT_TONE_RADIO_PATH_ACKNOWLEDGE, "STE_ADM_COMFORT_TONE_RADIO_PATH_ACKNOWLEDGE"},
        {STE_ADM_COMFORT_TONE_RADIO_PATH_NOT_AVAILABLE, "STE_ADM_COMFORT_TONE_RADIO_PATH_NOT_AVAILABLE"},
        {STE_ADM_COMFORT_TONE_ERROR_SPECIAL_INFORMATION, "STE_ADM_COMFORT_TONE_ERROR_SPECIAL_INFORMATION"},
        {STE_ADM_COMFORT_TONE_CALL_WAITING, "STE_ADM_COMFORT_TONE_CALL_WAITING"},
        {STE_ADM_COMFORT_TONE_RINGING, "STE_ADM_COMFORT_TONE_RINGING"}
    };

    size_t i;
    for (i=0 ; i < sizeof(tones) / sizeof(tones[0]) ; i++) {
        fprintf(stderr, "Playing tone: %s\n", tones[i].name);
        ste_adm_start_comfort_tone(STE_ADM_COMFORT_TONE_STANDARD_CEPT, tones[i].id);
        usleep(3500 * 1000);

        fprintf(stderr, "No tone playing\n");
        ste_adm_stop_comfort_tone();
        usleep(3000 * 1000);
    }

    for (i=0 ; i < sizeof(tones) / sizeof(tones[0]) ; i++) {
        fprintf(stderr, "Playing tone: %s\n", tones[i].name);
        ste_adm_start_comfort_tone(STE_ADM_COMFORT_TONE_STANDARD_CEPT, tones[i].id);
        usleep(3500 * 1000);
    }

    ste_adm_stop_comfort_tone();
    return 0;
}


int set_ext_delay(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "args: <device> <external_delay>\n");
        fprintf(stderr, " device: corresponds to Name in table Device.\n");
        exit(0);
    }

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    if (ste_adm_client_set_external_delay(argv[0], atoi(argv[1])) < 0)
        exitf("ste_adm_client_set_external_delay('%s', %s) failed\n",
            argv[0], argv[1]);

    return 0;
}

int get_max_out_latency(int argc, char** argv)
{
    int latency = -1;

    if (argc != 1) {
        fprintf(stderr, "args: <device>\n");
        fprintf(stderr, " device: corresponds to Name in table Device.\n");
        exit(0);
    }

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    if (ste_adm_client_max_out_latency(argv[0], &latency) < 0)
        exitf("ste_adm_client_max_out_latency('%s') failed\n", argv[0]);

    fprintf(stderr, "Latency: %d\n", latency);

    return 0;
}

int set_pcm_probe(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "args: <probe_id state>\n");
        fprintf(stderr, " state: 1=enabled, 0=disabled\n");
        exit(0);
    }

    if (ste_adm_client_set_pcm_probe(atoi(argv[0]), atoi(argv[1])) < 0)
        exitf("ste_adm_client_set_pcm_probe(%d, %d) failed\n", atoi(argv[0]), atoi(argv[1]));

    return 0;
}

int rescan(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "args: <device> <chain app/common/voice> <IL component name>\n");
        exit(EXIT_FAILURE);
    }

    ste_adm_effect_chain_type_t chain_id = 0;
    if (strcasecmp(argv[1], "app") == 0) {
        chain_id = STE_ADM_APPLICATION_CHAIN;
    } else if (strcasecmp(argv[1], "voice") == 0) {
        chain_id = STE_ADM_VOICE_CHAIN;
    }  else if (strcasecmp(argv[1], "common") == 0) {
        chain_id = STE_ADM_COMMON_CHAIN;
    } else {
        exitf("Invalid chain name\n");
    }

    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    ste_adm_res_t res = ste_adm_rescan_config(argv[0], chain_id, argv[2]);

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    unsigned time_used = tv_ms_diff(tv_end, tv_start);


    if (res == STE_ADM_RES_OK) {
        fprintf(stderr, "Rescan finished in %u ms\n", time_used);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Rescan failed. Return code: %s\n", ste_adm_res_to_str(res));
    return EXIT_FAILURE;
}

int rescan_spechproc()
{
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    ste_adm_res_t res = ste_adm_rescan_speechproc();

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    unsigned time_used = tv_ms_diff(tv_end, tv_start);


    if (res == STE_ADM_RES_OK) {
        fprintf(stderr, "Rescan speechproc finished in %u ms\n", time_used);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Rescan speechproc failed. Return code: %s\n", ste_adm_res_to_str(res));
    return EXIT_FAILURE;
}


int reload_device_settings()
{
    struct timeval tv_start;
    gettimeofday(&tv_start, NULL);

    ste_adm_res_t res = ste_adm_reload_device_settings();

    struct timeval tv_end;
    gettimeofday(&tv_end, NULL);
    unsigned time_used = tv_ms_diff(tv_end, tv_start);


    if (res == STE_ADM_RES_OK) {
        fprintf(stderr, "Reload device settings finished in %u ms\n", time_used);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "Reload device settings failed. Return code: %s\n", ste_adm_res_to_str(res));
    return EXIT_FAILURE;
}

int execute_sql(int argc, char** argv)
{
    if (argc != 1) {
        fprintf(stderr, "<sql statement>\n");
        exit(0);
    }

    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("ste_adm_client_connect failed\n");

    if (ste_adm_client_execute_sql(argv[0]) < 0)
        exitf("ste_adm_client_execute_sql(\"%s\") failed\n", argv[0]);

    return 0;
}

int get_active_devices(int argc, char** argv)
{
    (void) argc;
    (void) argv;
    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("Failed to connect to the ADM\n");

    ste_adm_res_t res;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(adm_id, &rfds);
    FD_SET(0, &rfds);

    fprintf(stderr, "Press enter to exit\n");

    while(1)
    {
            FD_ZERO(&rfds);
            FD_SET(adm_id, &rfds);
            FD_SET(0, &rfds);

            fprintf(stderr, "Requesting active device list\n");
            res = ste_adm_client_request_active_device_list(adm_id);
            if (res != STE_ADM_RES_OK)
            {
                fprintf(stderr, "ste_adm_client_request_active_device_list failed: '%s'\n", ste_adm_res_to_str(res));
            }

            int retval = select(adm_id + 1, &rfds, NULL, NULL, NULL);

            if (retval == -1)
            {
                fprintf(stderr, "error from select %d\n", retval);
                break;
            }
            else if (FD_ISSET(0, &rfds))
            {
                fprintf(stderr, "Key pressed, exiting\n");
                break;
            }
            else if (FD_ISSET(adm_id, &rfds))
            {
                fprintf(stderr, "Recieved response: requesting list\n");
                active_device_t *devices_p = NULL;
                int size = 0;
                res = ste_adm_client_read_active_device_list(adm_id, &devices_p, &size);
                if (res != STE_ADM_RES_OK)
                {
                    fprintf(stderr, "error from ste_adm_client_read_active_device_list: %s\n", ste_adm_res_to_str(res));
                    break;
                }
                int i;
                if (size == 0)
                {
                    fprintf(stderr, "No active devices\n");
                }
                for (i = 0; i < size; i++)
                {
                    fprintf(stderr, "Device: %s, app = %d, voice = %d, samplerate = %d\n",
                            devices_p[i].dev_name, devices_p[i].app_active, devices_p[i].voice_active, devices_p[i].voicecall_samplerate);
                }
                free (devices_p);
                fprintf(stderr, "----------------------------------------------------------\n");
            }

    }

    fprintf(stderr, "Disconnecting\n");
    ste_adm_client_disconnect(adm_id);



    return 0;
}

int get_modem_vc_state(int argc, char** argv)
{
    (void) argc;
    (void) argv;
    fprintf(stderr, "Connecting to ADM\n");
    int adm_id = ste_adm_client_connect();
    if (adm_id < 0) exitf("Failed to connect to the ADM\n");

    ste_adm_res_t res;
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(adm_id, &rfds);
    FD_SET(0, &rfds);

    fprintf(stderr, "Press enter to exit\n");

    while(1) {
        FD_ZERO(&rfds);
        FD_SET(adm_id, &rfds);
        FD_SET(0, &rfds);

        fprintf(stderr, "Requesting modem vc state\n");
        res = ste_adm_client_request_modem_vc_state(adm_id);
        if (res != STE_ADM_RES_OK) {
            fprintf(stderr, "ste_adm_client_request_modem_vc_state failed: '%s'\n", ste_adm_res_to_str(res));
        }

        int retval = select(adm_id + 1, &rfds, NULL, NULL, NULL);

        if (retval == -1) {
            fprintf(stderr, "error from select %d\n", retval);
            break;
        } else if (FD_ISSET(0, &rfds)) {
            fprintf(stderr, "Key pressed, exiting\n");
            break;
        } else if (FD_ISSET(adm_id, &rfds)) {
            ste_adm_vc_modem_status_t vc_modem_status;
            res = ste_adm_client_read_modem_vc_state(adm_id, &vc_modem_status);
            if (res != STE_ADM_RES_OK) {
                fprintf(stderr, "error from ste_adm_client_read_modem_vc_state: %s\n", ste_adm_res_to_str(res));
                break;
            }
            fprintf(stderr, "Current vc modem state is %s\n", vc_modem_status == STE_ADM_VC_MODEM_STATUS_CONNECTED ? "CONNECTED" : "DISCONNECTED");
        } else {
            fprintf(stderr, ".\n");
        }
    }
    fprintf(stderr, "Disconnecting\n");
    ste_adm_client_disconnect(adm_id);
    return 0;
}

static const char* prop(const char* target_key)
{
    static char buf[800];
#ifndef ADM_DBG_X86
    sprintf(buf, "setprop %s ", target_key);
#else

    // use getenv on host. Dots not allowed in name.
    char* keycopy = strdup(target_key);
    char* a = keycopy;
    for ( ; *a ; a++) {
        if(*a == '.') {
            *a = '_';
        }
    }
    sprintf(buf, "export %s=", keycopy);
    free(keycopy);
#endif
    return buf;
}

//
//
//
//
//
//

void restore_stdio(int a, siginfo_t *b, void *c)
{
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_NONBLOCK);
    exit(20);
}

int main(int argc, char** argv)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = restore_stdio;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);


    if (argc < 2) {
        fprintf(stderr, "Usage: test <CMD> ... (to get help: ste-adm-test <CMD> ?)\n");
        fprintf(stderr, "   play - open a device and send a file to it\n");
        fprintf(stderr, "   record - open a device and record data from it to a file\n");
        fprintf(stderr, "   set_dict_mode - set dictaphone record path\n");
        fprintf(stderr, "   set_dict_mute - mute/unmute dictaphone\n");
        fprintf(stderr, "   get_dict_mute - mute/unmute dictaphone\n");
        fprintf(stderr, "   mute_upstream - set upstream mute status\n");
        fprintf(stderr, "   mute_downstream - set downstream mute status\n");
        fprintf(stderr, "   vc_status - get vocie call mute/volume/tty status\n");
        fprintf(stderr, "   voicecall - start a voicecall (only ADM communication; no dialing etc)\n");
        fprintf(stderr, "   vc_set_tty_mode\n");
        fprintf(stderr, "   vctx_volume / vcrx_volume\n");
        fprintf(stderr, "   setappvol / getappvol - set/get volume of application chain\n");
        fprintf(stderr, "   map - associate a toplevel device with an actual device\n");
        fprintf(stderr, "   livemap - map, and rescan settings from DB. Settings must be compatible.\n");
        fprintf(stderr, "   map_list - print a list of all toplevel device mappings\n");
        fprintf(stderr, "   get_toplevel_device - get toplevel device associated with specified actual device\n");
        fprintf(stderr, "   d2d - setup device to device connection\n");
        fprintf(stderr, "   set_ext_delay - set external delay in db for a device\n");
        fprintf(stderr, "   get_max_out_latency - get external delay in db for a device\n");
        fprintf(stderr, "   set_pcm_probe - set state for pcm probe \n");
        fprintf(stderr, "   play_tones - plays CEPT comfort tones, one by one. No arguments.\n");
        fprintf(stderr, "   csloop - set cscall loopback status\n");
        fprintf(stderr, "   rescan - re-read db settings for an active chain\n");
        fprintf(stderr, "   rescan_speechproc - re-read db settings for speechproc\n");
        fprintf(stderr, "   reload_device_settings - Cloases and restarts the current voicecall devices.\n");
        fprintf(stderr, "   sql - execute sql statement\n");
        fprintf(stderr, "   get_active_devices - Shows active devicelist and wait for notifications.\n");
        fprintf(stderr, "   shutdown - tell ADM server to exit. Used for Valgrind. \n");
        fprintf(stderr, "   LPA test: setprop ste.debug.adm.minilpa 1\n");
        fprintf(stderr, "   Core dumps: (you usually can't dump to memory card with FAT)\n");
        fprintf(stderr, "       setprop ste.debug.adm.coredir /data/adm_core_dump\n");
        fprintf(stderr, "       setprop ste.debug.adm.coresize 200 (total size before removing old dumps, in Mb)\n");
        fprintf(stderr, "OST: Refreshed each time a component is created, except for cscall, \n");
        fprintf(stderr, "   which is refreshed each time a new call is started.\n");
        fprintf(stderr, "   %s\"<component name>[,<usage> (optional)] ..\"\n", prop("ste.debug.adm.ost"));
        fprintf(stderr, "   %s\"OMX.ST.AFM.NOKIA_AV_SINK\"\n", prop("ste.debug.adm.ost"));
        fprintf(stderr, "   %s\"OMX.ST.AFM.NOKIA_AV_SOURCE\"\n", prop("ste.debug.adm.ost"));
        fprintf(stderr, "   %s\"OMX.ST.AFM.NOKIA_AV_SOURCE OMX.ST.AFM.NOKIA_AV_SINK\"\n", prop("ste.debug.adm.ost"));
        fprintf(stderr, "   %s\"OMX.ST.AFM.NOKIA_AV_SINK OMX.ST.AFM.mixer,common\"\n", prop("ste.debug.adm.ost"));
        fprintf(stderr, "   %s\"OMX.ST.AFM.speech_proc\"\n", prop("ste.debug.adm.ost"));
        fprintf(stderr, "   %s\"OMX.ST.AFM.cscall\"\n", prop("ste.debug.adm.ost"));
        fprintf(stderr, "   %s\",\" (enables all traces)\n", prop("ste.debug.adm.ost"));
        fprintf(stderr, "DISABLE ADM automatic reboot for unrecoverable errors\n");
        fprintf(stderr, "   setprop ste.debug.adm.reboot.disabled 1\n");
        fprintf(stderr, "SELECT if ADM-internal mixer/splitter should be used\n");
        fprintf(stderr, "   setprop ste.debug.adm.intmixsplit 1\n");
        fprintf(stderr, "SWITCH to Earpiece & Mic, will be removed when proper accessory detection is implemented\n");
        fprintf(stderr, "   setprop ste.debug.anm.enable.earp_mic 1\n");
        fprintf(stderr, "LOG: Log settings for Alsasink, Alsasource & Alsactrl\n");
        fprintf(stderr, "   %s\"INFO WARN\"\n", prop("ste.debug.alsactrl.log"));
        fprintf(stderr, "   %s\"INFO WARN\"\n", prop("ste.debug.alsasource.log"));
        fprintf(stderr, "   %s\"INFO WARN\"\n", prop("ste.debug.alsasink.log"));
        fprintf(stderr, "DUMP: Dump data entering/leaving ADIL to /sdcard/adil/<file>.\n");
        fprintf(stderr, "   %s\"ADIL_DUMP\" adb shell rm /sdcard/adil/*   adb pull /sdcard/adil\n", prop("ste.debug.adm.log"));
        fprintf(stderr, "LOG: Log settings are refreshed each time ADM accept()s a new connection\n");
        fprintf(stderr, "   %s\"INFO\"\n", prop("ste.debug.anm.log"));
        fprintf(stderr, "   %s\"ADIL ADIL_FL ADIL_INT\"\n", prop("ste.debug.adm.log"));
        fprintf(stderr, "   %s\"OMXIL\"\n", prop("ste.debug.adm.log"));
        fprintf(stderr, "   %s\"OMXIL INFO\"\n", prop("ste.debug.adm.log"));
        fprintf(stderr, "   %s\"OMXIL INFO OMXIL_FL OMXIL_V INFO_V\"\n", prop("ste.debug.adm.log"));

        exit(1);
    }


    const char* cmd=argv[1];
    if (strcmp(cmd, "play") == 0) {
        return play(argc-2, argv+2); // always skips one argv itself (usually the filename)
    } else if (strcmp(cmd, "record") == 0) {
        return record(argc-2, argv+2); // always skips one argv itself (usually the filename)
    } else if (strcmp(cmd, "set_dict_mode") == 0) {
        return set_dictaphone_mode(argc-2, argv+2);
    } else if (strcmp(cmd, "set_dict_mute") == 0) {
        return set_dictaphone_mute(argc-2, argv+2);
    } else if (strcmp(cmd, "get_dict_mute") == 0) {
        return get_dictaphone_mute(argc-2, argv+2);
    } else if (strcmp(cmd, "mute_upstream") == 0) {
        return mute_upstream(argc-2, argv+2);
    } else if (strcmp(cmd, "mute_downstream") == 0) {
        return mute_downstream(argc-2, argv+2);
    } else if (strcmp(cmd, "vc_status") == 0) {
        return vc_status(argc-2, argv+2);
    } else if (strcmp(cmd, "vc_set_tty_mode") == 0) {
        return vc_set_tty_mode(argc-2, argv+2);
    } else if (strcmp(cmd, "vcrx_volume") == 0) {
        return vcrx_volume(argc-2, argv+2);
    } else if (strcmp(cmd, "vctx_volume") == 0) {
        return vctx_volume(argc-2, argv+2);
    } else if (strcmp(cmd, "voicecall") == 0) {
        return voicecall(argc-2, argv+2);
    } else if (strcmp(cmd, "setappvol") == 0) {
        return setappvol(argc-2, argv+2);
    } else if (strcmp(cmd, "getappvol") == 0) {
        return getappvol(argc-2, argv+2);
    } else if (strcmp(cmd, "map") == 0) {
        return map(argc-2, argv+2);
    } else if (strcmp(cmd, "livemap") == 0) {
        return livemap(argc-2, argv+2);
    } else if(strcmp(cmd, "map_list") == 0) {
          return map_list();
    } else if(strcmp(cmd, "get_toplevel_device") == 0) {
          return get_toplevel_device(argc-2, argv+2);
    } else if (strcmp(cmd, "d2d") == 0) {
        return d2d(argc-2, argv+2);
    } else if (strcmp(cmd, "set_ext_delay") == 0) {
        return set_ext_delay(argc-2, argv+2);
    } else if (strcmp(cmd, "get_max_out_latency") == 0) {
        return get_max_out_latency(argc-2, argv+2);
    } else if (strcmp(cmd, "set_pcm_probe") == 0) {
        return set_pcm_probe(argc-2, argv+2);        
    } else if (strcmp(cmd, "play_tones") == 0) {
        return play_tones(argc-2, argv+2);
    } else if (strcmp(cmd, "shutdown") == 0) {
        ste_adm_client_debug_shutdown();
        return 0;
    } else if (strcmp(cmd, "dump") == 0) {
        ste_adm_client_debug_dump_state();
        return 0;
    } else if (strcmp(cmd, "csloop") == 0) {
        return set_csloop(argc-2, argv+2);
    } else if (strcmp(cmd, "rescan") == 0) {
        return rescan(argc-2, argv+2);
    } else if (strcmp(cmd, "rescan_speechproc") == 0) {
        return rescan_spechproc();
    } else if (strcmp(cmd, "reload_device_settings") == 0) {
        return reload_device_settings();
    } else if (strcmp(cmd, "sql") == 0) {
        return execute_sql(argc-2, argv+2);
    } else if (strcmp(cmd, "get_active_devices") == 0) {
        return get_active_devices(argc-2, argv+2);
    } else if (strcmp(cmd, "crossplay") == 0) {
        return crossplay(argc-2, argv+2);
    } else if (strcmp(cmd, "get_modem_vc_state") == 0) {
        return get_modem_vc_state(argc-2, argv+2);
    }


    fprintf(stderr, "No recognized command\n");
    exit(1);
}


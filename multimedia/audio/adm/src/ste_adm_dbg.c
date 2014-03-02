/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_dbg.c
    \brief Debugging utilities.

    Implementation of debugging utilities.
*/

#define LOG_TAG "ADM"
#define ADM_LOG_FILENAME "adm_dbg"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stddef.h>
#include <signal.h>
#include <unwind.h>
#include <stdlib.h>

#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/statfs.h>

#ifndef ADM_DBG_X86
  #include "cutils/properties.h"
#else
  #define PROPERTY_VALUE_MAX 96
  #include <stdlib.h> // getenv
#endif

#define LOG_TAG "ADM"

/**
* Path to log file used by STE ADM Daemon process
* TODO: should be in some better place (var/log requires root..)
*/
#ifdef ANDROID
#include <cutils/log.h>
#else
#include <stdio.h>
#define ALOGI printf
#define ALOGE printf
#define ALOGW printf
#define ALOGV printf
#define ALOGD printf
#endif

#include "ste_adm_dbg.h"
#include "ste_adm_omx_core.h"
#include "ste_adm_omx_log.h"

#ifndef ADM_DBG_X86
    #include <hardware_legacy/power.h>
    #define STE_ADM_WAKE_LOCK "STE_ADM"
#endif

#ifndef ADM_CORE_DUMP_MIN_FREE_SPACE
  #define ADM_CORE_DUMP_MIN_FREE_SPACE (1024 * 1024 * 80)
#endif

// free space required to write core dump, in %
#ifndef ADM_CORE_DUMP_MIN_FREE_PERCENT
  #define ADM_CORE_DUMP_MIN_FREE_PERCENT 10
#endif

/**
* File stream pointer used for logging to file.
*/
unsigned int g_ste_adm_debug_mask;

char *log_prefix[STE_ADM_LOG_LAST] = {
    "INFO_V",
    "INFO",
    "D2D_V",
    "D2D",
    "OMXIL_V",
    "OMXIL",
    "OMXIL_FL",
    "ADIL",
    "ADIL_FL",
    "ADIL_INT",
    "ADIL_DUMP",
    "DB_V",
    "DB",
    "STATUS",
    "WARN",
    "ERR",
};

/*! \def LOG_TO_FILE_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_TO_FILE
#define LOG_TO_FILE_DEFINED 0
#else
#define LOG_TO_FILE_DEFINED 1
#endif


/*! \def LOG_INFO_VERBOSE_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_INFO_VERBOSE
#define LOG_INFO_VERBOSE_DEFINED 0
#else
#define LOG_INFO_VERBOSE_DEFINED 1
#endif

/*! \def LOG_INFO_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_INFO
#define LOG_INFO_DEFINED 0
#else
#define LOG_INFO_DEFINED 1
#endif

/*! \def LOG_D2D_VERBOSE_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_D2D_VERBOSE
#define LOG_D2D_VERBOSE_DEFINED 0
#else
#define LOG_D2D_VERBOSE_DEFINED 1
#endif

/*! \def LOG_D2D_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_D2D
#define LOG_D2D_DEFINED 0
#else
#define LOG_D2D_DEFINED 1
#endif

/*! \def LOG_OMXIL_VERBOSE_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_OMXIL_VERBOSE
#define LOG_OMXIL_VERBOSE_DEFINED 0
#else
#define LOG_OMXIL_VERBOSE_DEFINED 1
#endif

/*! \def LOG_OMXIL_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_OMXIL
#define LOG_OMXIL_DEFINED 0
#else
#define LOG_OMXIL_DEFINED 1
#endif

/*! \def LOG_OMXIL_FLOW_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_OMXIL_FLOW
#define LOG_OMXIL_FLOW_DEFINED 0
#else
#define LOG_OMXIL_FLOW_DEFINED 1
#endif

/*! \def LOG_ADIL_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_ADIL
#define LOG_ADIL_DEFINED 0
#else
#define LOG_ADIL_DEFINED 1
#endif

/*! \def LOG_ADIL_FLOW_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_ADIL_FLOW
#define LOG_ADIL_FLOW_DEFINED 0
#else
#define LOG_ADIL_FLOW_DEFINED 1
#endif

/*! \def LOG_ADIL_INT_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_ADIL_INT
#define LOG_ADIL_INT_DEFINED 0
#else
#define LOG_ADIL_INT_DEFINED 1
#endif

/*! \def LOG_DB_VERBOSE_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_DB_VERBOSE
#define LOG_DB_VERBOSE_DEFINED 0
#else
#define LOG_DB_VERBOSE_DEFINED 1
#endif

/*! \def LOG_DB_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_DB
#define LOG_DB_DEFINED 0
#else
#define LOG_DB_DEFINED 1
#endif

/*! \def LOG_STATUS_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_STATUS
#define LOG_STATUS_DEFINED 0
#else
#define LOG_STATUS_DEFINED 1
#endif

/*! \def LOG_WARN_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_WARNINGS
#define LOG_WARN_DEFINED 0
#else
#define LOG_WARN_DEFINED 1
#endif

/*! \def LOG_ERR_DEFINED

    Helper macro to avoid \#ifdef usage for conditional compilation
*/
#ifndef ADM_LOG_ERRORS
#define LOG_ERR_DEFINED 0
#else
#define LOG_ERR_DEFINED 1
#endif

void ste_adm_debug_coredump_and_die_if_enabled(void);


static const char* get_prop(const char* key)
{
    static char prop_static_buf[PROPERTY_VALUE_MAX];

#ifndef ADM_DBG_X86
    property_get(key, prop_static_buf, "");
#else
    // use getenv on host. Dots not allowed in name.
    char* keycopy = strdup(key);
    char* a = keycopy;
    for ( ; *a ; a++) {
        if(*a == '.') {
            *a = '_';
        }
    }

    char* str = getenv(keycopy);
    free(keycopy);
    if (str) {
        strncpy(prop_static_buf, str, PROPERTY_VALUE_MAX);
    } else {
        prop_static_buf[0]=0;
    }
#endif
    prop_static_buf[PROPERTY_VALUE_MAX-1]=0; // not sure if needed for property_get
    return prop_static_buf;
}

static void set_logging_defaults(void)
{
    g_ste_adm_debug_mask =
        (LOG_INFO_VERBOSE_DEFINED  << STE_ADM_LOG_INFO_VERBOSE) |
        (LOG_INFO_DEFINED          << STE_ADM_LOG_INFO) |
        (LOG_D2D_VERBOSE_DEFINED   << STE_ADM_LOG_D2D_VERBOSE) |
        (LOG_D2D_DEFINED           << STE_ADM_LOG_D2D) |
        (LOG_OMXIL_VERBOSE_DEFINED << STE_ADM_LOG_OMXIL_VERBOSE) |
        (LOG_OMXIL_DEFINED         << STE_ADM_LOG_OMXIL) |
        (LOG_OMXIL_FLOW_DEFINED    << STE_ADM_LOG_OMXIL_FLOW) |
        (LOG_ADIL_DEFINED          << STE_ADM_LOG_ADIL) |
        (LOG_ADIL_FLOW_DEFINED     << STE_ADM_LOG_ADIL_FLOW) |
        (LOG_ADIL_INT_DEFINED      << STE_ADM_LOG_ADIL_INT) |
        (LOG_DB_VERBOSE_DEFINED    << STE_ADM_LOG_DB_VERBOSE) |
        (LOG_DB_DEFINED            << STE_ADM_LOG_DB) |
        (LOG_STATUS_DEFINED        << STE_ADM_LOG_STATUS) |
        (LOG_WARN_DEFINED          << STE_ADM_LOG_WARN) |
        (LOG_ERR_DEFINED           << STE_ADM_LOG_ERR);
}

void dbg_set_logs_from_properties()
{
    set_logging_defaults();
    const char* name_start= get_prop("ste.debug.adm.log");
    while(*name_start) {
        const char* name_end  = name_start;
        while (*name_end && *name_end != ' ') name_end++;

        int log_type;
        for (log_type=0 ; log_type < STE_ADM_LOG_LAST; log_type++) {
            if ((size_t)(name_end-name_start) == strlen(log_prefix[log_type]) &&
                strncasecmp(log_prefix[log_type], name_start, (size_t)(name_end-name_start)) == 0)
            {
                g_ste_adm_debug_mask |= 1U << log_type;
            }
        }

        name_start = name_end;
        while (*name_start && *name_start == ' ') name_start++;
    }
}

int ste_adm_debug_is_log_enabled(enum STE_ADM_LOGLEVEL level)
{
    return (g_ste_adm_debug_mask & (1U << level)) ? 1 : 0;
}


#define BUFFER_LENGTH 2048
#define PRINT_FORMAT "%9.9s:%-4.d %8.8s %s%s"
int ste_adm_debug_print(enum STE_ADM_LOGLEVEL level, const char *file, int line, const char *function, const char *fmt, ...)
{
  va_list ap;
  char buffer[BUFFER_LENGTH];
  char insert_newline[2] = {0, 0};
  (void) function;

  va_start(ap, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, ap);
  va_end(ap);

  if (buffer[strlen(buffer)-1] != '\n') {
    insert_newline[0] = '\n';
  }

  switch (level) {
    case STE_ADM_LOG_ERR:
      ALOGE(PRINT_FORMAT, file, line, log_prefix[level], buffer, insert_newline);
      break;
    case STE_ADM_LOG_WARN:
      ALOGW(PRINT_FORMAT, file, line, log_prefix[level], buffer, insert_newline);
      break;
    case STE_ADM_LOG_INFO:
    case STE_ADM_LOG_D2D:
    case STE_ADM_LOG_DB:
    case STE_ADM_LOG_STATUS:
    case STE_ADM_LOG_OMXIL_VERBOSE:
    case STE_ADM_LOG_OMXIL:
    case STE_ADM_LOG_OMXIL_FLOW:
    case STE_ADM_LOG_INFO_VERBOSE:
    case STE_ADM_LOG_D2D_VERBOSE:
    case STE_ADM_LOG_DB_VERBOSE:
    default:
      ALOGD(PRINT_FORMAT, file, line, log_prefix[level], buffer, insert_newline);
      break;
  }

#ifndef ANDROID
  fflush(stdout);
#endif
  return 0;
}

int ste_adm_debug_omx_trace_enabled(const char* name, const char* type)
{
    char prop[PROPERTY_VALUE_MAX];

#ifndef ADM_DBG_X86
    property_get("ste.debug.adm.ost", prop, "");
#else
    char* str = getenv("ste_debug_adm_ost");
    if (str)
      strncpy(prop, str, PROPERTY_VALUE_MAX);
    else
      prop[0]=0;
#endif
    prop[PROPERTY_VALUE_MAX-1]=0; // not sure if needed for property_get

    const char* name_start=prop;

    while(*name_start) {
        const char* name_end  = name_start;
        while (*name_end && *name_end != ' ' && *name_end != ',') name_end++;

        const char* type_start = NULL;
        const char* type_end;
        if (*name_end == ',') {
            type_start = name_end + 1;
            type_end   = type_start;
            while (*type_end && *type_end != ' ') type_end++;
        }

        if (    (size_t)(name_end-name_start) == strlen(name) &&
                strncasecmp(name, name_start, (size_t)(name_end-name_start)) == 0 &&
                (type_start == NULL || (strlen(type) == (size_t)(type_end-type_start) &&
                strncasecmp(type, type_start, (size_t)(type_end-type_start)) == 0)))
        {
            return 1;
        }

        while (*name_start && *name_start != ' ') name_start++;
        while (*name_start && *name_start == ' ') name_start++;
    }
   return 0;
}

static int ste_adm_debug_adm_reboot_disabled(void)
{
    char prop[PROPERTY_VALUE_MAX];

#ifdef ADM_DBG_X86
    return 0;
#else
    property_get("ste.debug.adm.reboot.disabled", prop, "");
    prop[PROPERTY_VALUE_MAX-1]=0;

    if (strcmp(prop, "1") == 0)
        return 1;
    else
        return 0;
#endif
}

void ste_adm_debug_reboot(void)
{
    pid_t pid;
    pid = getpid();
    dbg_print_debug_information();

    if (!ste_adm_debug_adm_reboot_disabled()) {
#ifndef ADM_DBG_X86
        ALOG_ERR("Releasing STE_ADM wakelock before killing ADM");
        release_wake_lock(STE_ADM_WAKE_LOCK);
#endif
        ALOG_ERR("Unrecoverable error detected, killing ADM pid %d\n", pid);
        ste_adm_debug_coredump_and_die_if_enabled();

        // If core dump didn't work or was disabled, we try to
        // die another way
        kill(pid, SIGTERM);
        sleep(1);
        /* If SIGTERM failed, then try SIGKILL */
        kill(pid, SIGKILL);
        sleep(1);
    }
    else {
        while (1) {
            ALOG_ERR("Unrecoverable error detected, reboot disabled. Blocking ADM!");
            sleep(60);
        }
    }
}


/*
static void catf(char* buf, const char* fmt, ...)
{
    va_list a;
    va_start(a, fmt);
    vsprintf(buf+strlen(buf), fmt, a);
    va_end(a);
}

static void hexdump(const char* buf, unsigned int bytes)
{
    unsigned int cur=0;
    char linebuf[100];
    linebuf[0]=0;
    while (cur < bytes)
    {
        if (cur%16 == 0) catf(linebuf, "   %08X: ",cur);
        cur++;
        catf(linebuf,"%02X", 0xFF & *buf++);
        if (cur%16 == 0) { catf(linebuf,"\n");  ALOG_WARN(linebuf); linebuf[0]=0; }
        else if (cur%4  == 0) catf(linebuf," ");
    }
    if (cur%16 != 0) { catf(linebuf,"\n");  ALOG_WARN(linebuf); }
} */


#define MSP0_BASE_ADDR (0x80123000)
#define MSP1_BASE_ADDR (0x80124000)
#define MSP3_BASE_ADDR (0x80125000)
#define DMAC_BASE_ADDR (0x801C0000)

static void dump_hw_registers(void)
{
    static const struct
    {
        const char*  name;
        unsigned int addr;
    } registers_to_dump[] = {
//        { "MSP0_DR",      MSP0_BASE_ADDR + 0x00 },
        { "MSP0_GCR",     MSP0_BASE_ADDR + 0x04 },
        { "MSP0_TCF",     MSP0_BASE_ADDR + 0x08 },
        { "MSP0_RCF",     MSP0_BASE_ADDR + 0x0C },
        { "MSP0_SRG",     MSP0_BASE_ADDR + 0x10 },
        { "MSP0_FLR",     MSP0_BASE_ADDR + 0x14 },
        { "MSP0_DMACR",   MSP0_BASE_ADDR + 0x18 },

        { "MSP0_IMSC",    MSP0_BASE_ADDR + 0x20 },
        { "MSP0_RIS",     MSP0_BASE_ADDR + 0x24 },
        { "MSP0_MIS",     MSP0_BASE_ADDR + 0x28 },
        { "MSP0_ICR",     MSP0_BASE_ADDR + 0x2C },
        { "MSP0_MCR",     MSP0_BASE_ADDR + 0x30 },
        { "MSP0_RCV",     MSP0_BASE_ADDR + 0x34 },
        { "MSP0_RCM",     MSP0_BASE_ADDR + 0x38 },

        { "MSP0_TCE0",    MSP0_BASE_ADDR + 0x40 },
        { "MSP0_TCE1",    MSP0_BASE_ADDR + 0x44 },
        { "MSP0_TCE2",    MSP0_BASE_ADDR + 0x48 },
        { "MSP0_TCE3",    MSP0_BASE_ADDR + 0x4C },

        { "MSP0_RCE0",    MSP0_BASE_ADDR + 0x60 },
        { "MSP0_RCE1",    MSP0_BASE_ADDR + 0x64 },
        { "MSP0_RCE2",    MSP0_BASE_ADDR + 0x68 },
        { "MSP0_RCE3",    MSP0_BASE_ADDR + 0x6C },
        { "MSP0_IODLY",   MSP0_BASE_ADDR + 0x70 },

        { "MSP0_ITCR",    MSP0_BASE_ADDR + 0x80 },
        { "MSP0_ITIP",    MSP0_BASE_ADDR + 0x84 },
        { "MSP0_ITOP",    MSP0_BASE_ADDR + 0x88 },
        { "MSP0_TSTDR",   MSP0_BASE_ADDR + 0x8C },


//        { "MSP1_DR",      MSP1_BASE_ADDR + 0x00 },
        { "MSP1_GCR",     MSP1_BASE_ADDR + 0x04 },
        { "MSP1_TCF",     MSP1_BASE_ADDR + 0x08 },
        { "MSP1_RCF",     MSP1_BASE_ADDR + 0x0C },
        { "MSP1_SRG",     MSP1_BASE_ADDR + 0x10 },
        { "MSP1_FLR",     MSP1_BASE_ADDR + 0x14 },
        { "MSP1_DMACR",   MSP1_BASE_ADDR + 0x18 },

        { "MSP1_IMSC",    MSP1_BASE_ADDR + 0x20 },
        { "MSP1_RIS",     MSP1_BASE_ADDR + 0x24 },
        { "MSP1_MIS",     MSP1_BASE_ADDR + 0x28 },
        { "MSP1_ICR",     MSP1_BASE_ADDR + 0x2C },
        { "MSP1_MCR",     MSP1_BASE_ADDR + 0x30 },
        { "MSP1_RCV",     MSP1_BASE_ADDR + 0x34 },
        { "MSP1_RCM",     MSP1_BASE_ADDR + 0x38 },

        { "MSP1_TCE0",    MSP1_BASE_ADDR + 0x40 },
        { "MSP1_TCE1",    MSP1_BASE_ADDR + 0x44 },
        { "MSP1_TCE2",    MSP1_BASE_ADDR + 0x48 },
        { "MSP1_TCE3",    MSP1_BASE_ADDR + 0x4C },

        { "MSP1_RCE0",    MSP1_BASE_ADDR + 0x60 },
        { "MSP1_RCE1",    MSP1_BASE_ADDR + 0x64 },
        { "MSP1_RCE2",    MSP1_BASE_ADDR + 0x68 },
        { "MSP1_RCE3",    MSP1_BASE_ADDR + 0x6C },
        { "MSP1_IODLY",   MSP1_BASE_ADDR + 0x70 },

        { "MSP1_ITCR",    MSP1_BASE_ADDR + 0x80 },
        { "MSP1_ITIP",    MSP1_BASE_ADDR + 0x84 },
        { "MSP1_ITOP",    MSP1_BASE_ADDR + 0x88 },
        { "MSP1_TSTDR",   MSP1_BASE_ADDR + 0x8C },



//        { "MSP3_DR",      MSP3_BASE_ADDR + 0x00 },
        { "MSP3_GCR",     MSP3_BASE_ADDR + 0x04 },
        { "MSP3_TCF",     MSP3_BASE_ADDR + 0x08 },
        { "MSP3_RCF",     MSP3_BASE_ADDR + 0x0C },
        { "MSP3_SRG",     MSP3_BASE_ADDR + 0x10 },
        { "MSP3_FLR",     MSP3_BASE_ADDR + 0x14 },
        { "MSP3_DMACR",   MSP3_BASE_ADDR + 0x18 },

        { "MSP3_IMSC",    MSP3_BASE_ADDR + 0x20 },
        { "MSP3_RIS",     MSP3_BASE_ADDR + 0x24 },
        { "MSP3_MIS",     MSP3_BASE_ADDR + 0x28 },
        { "MSP3_ICR",     MSP3_BASE_ADDR + 0x2C },
        { "MSP3_MCR",     MSP3_BASE_ADDR + 0x30 },
        { "MSP3_RCV",     MSP3_BASE_ADDR + 0x34 },
        { "MSP3_RCM",     MSP3_BASE_ADDR + 0x38 },

        { "MSP3_TCE0",    MSP3_BASE_ADDR + 0x40 },
        { "MSP3_TCE1",    MSP3_BASE_ADDR + 0x44 },
        { "MSP3_TCE2",    MSP3_BASE_ADDR + 0x48 },
        { "MSP3_TCE3",    MSP3_BASE_ADDR + 0x4C },

        { "MSP3_RCE0",    MSP3_BASE_ADDR + 0x60 },
        { "MSP3_RCE1",    MSP3_BASE_ADDR + 0x64 },
        { "MSP3_RCE2",    MSP3_BASE_ADDR + 0x68 },
        { "MSP3_RCE3",    MSP3_BASE_ADDR + 0x6C },
        { "MSP3_IODLY",   MSP3_BASE_ADDR + 0x70 },

        { "MSP3_ITCR",    MSP3_BASE_ADDR + 0x80 },
        { "MSP3_ITIP",    MSP3_BASE_ADDR + 0x84 },
        { "MSP3_ITOP",    MSP3_BASE_ADDR + 0x88 },
        { "MSP3_TSTDR",   MSP3_BASE_ADDR + 0x8C },


        { "DMAC_GCC",     DMAC_BASE_ADDR + 0x0   },
        { "DMAC_PRTYP",   DMAC_BASE_ADDR + 0x004 },
        { "DMAC_PRSME",   DMAC_BASE_ADDR + 0x008 },
        { "DMAC_PRSMO",   DMAC_BASE_ADDR + 0x00C },
        { "DMAC_PRMSE",   DMAC_BASE_ADDR + 0x010 },
        { "DMAC_PRMSO",   DMAC_BASE_ADDR + 0x014 },
        { "DMAC_PRMOE",   DMAC_BASE_ADDR + 0x018 },
        { "DMAC_PRMOO",   DMAC_BASE_ADDR + 0x01C },
        { "DMAC_LCPA",    DMAC_BASE_ADDR + 0x020 },
        { "DMAC_LCLA",    DMAC_BASE_ADDR + 0x024 },
        { "DMAC_ACTIVE",  DMAC_BASE_ADDR + 0x050 },
        { "DMAC_ACTIVO",  DMAC_BASE_ADDR + 0x054 },
        { "DMAC_FSEB1",   DMAC_BASE_ADDR + 0x058 },
        { "DMAC_FSEB2",   DMAC_BASE_ADDR + 0x05C },
        { "DMAC_PCMIS",   DMAC_BASE_ADDR + 0x060 },
        { "DMAC_PCICR",   DMAC_BASE_ADDR + 0x064 },
        { "DMAC_PCTIS",   DMAC_BASE_ADDR + 0x068 },
        { "DMAC_PCEIS",   DMAC_BASE_ADDR + 0x06C },

        // Physical channel 2
        { "DMC2_SSCFG",   DMAC_BASE_ADDR + 0x400 + 2 * 32 + 0x00 },
        { "DMC2_SSELT",   DMAC_BASE_ADDR + 0x400 + 2 * 32 + 0x04 },
        { "DMC2_SSPTR",   DMAC_BASE_ADDR + 0x400 + 2 * 32 + 0x08 },
        { "DMC2_SSLNK",   DMAC_BASE_ADDR + 0x400 + 2 * 32 + 0x0C },
        { "DMC2_SDCFG",   DMAC_BASE_ADDR + 0x400 + 2 * 32 + 0x10 },
        { "DMC2_SDELT",   DMAC_BASE_ADDR + 0x400 + 2 * 32 + 0x14 },
        { "DMC2_SDPTR",   DMAC_BASE_ADDR + 0x400 + 2 * 32 + 0x18 },
        { "DMC2_SDLNK",   DMAC_BASE_ADDR + 0x400 + 2 * 32 + 0x1C },

    };


    const int num_regs = sizeof(registers_to_dump) / sizeof(registers_to_dump[0]);
    int i;
    unsigned int cur_base_addr=0;
    unsigned int failed_base_addr=0;
    int* base=0;
    int fd = open("/dev/mem", O_RDONLY);
    if (fd < 0) {
        ALOG_ERR("Could not open /dev/mem, errno=%d\n", errno);
        return;
    }
    char buf[120];
    int bufoffs = 0;
    for (i=0 ; i < num_regs ; i++) {
        unsigned int addr = registers_to_dump[i].addr;
        if ((addr & 0xFFFFF000) != cur_base_addr) {
            if (base) {
                munmap(base, 4096);
                base=0;
            }
            cur_base_addr = addr & 0xFFFFF000;
            base = (int*) mmap(0, 4096, PROT_READ, MAP_PRIVATE, fd, cur_base_addr);
            if (!base && cur_base_addr != failed_base_addr) {
                failed_base_addr = cur_base_addr;
                ALOG_ERR("Could not mmap /dev/mem to %X, errno %d\n", cur_base_addr, errno);
            }
        }
        if (base) {
            int idx = (addr & 0xFFF) / 4;
            int val = base[idx];
            bufoffs += snprintf(buf+bufoffs, sizeof(buf)-bufoffs, "%10.10s %08X  ", registers_to_dump[i].name, val);
            if (bufoffs > (int)sizeof(buf) - 22) {
                ALOG_WARN("%s\n", buf);
                bufoffs=0;
            }
        }
    }

    if (bufoffs) {
        ALOG_WARN("%s\n", buf);
    }
    if (base) {
        munmap(base, 4096);
    }
    if (fd >= 0) {
        close(fd);
    }
}


static void print_file(char *file, int collect_on_line, int (*match_function)(const char*))
{
    char buf[200];
    FILE *f = fopen (file,"rb");
    if (f == NULL) {
        ALOG_ERR("dumpAB8500: Failed to open %s", file);
        return;
    }

    char outbuf[140];
    int outbufoffs=0;
    while (fgets(buf, 200, f))
    {
        // chop trailing '\n'
        if (buf[0] && buf[strlen(buf)-1] == '\n') {
            buf[strlen(buf)-1]=0;
        }
        if (match_function && !match_function(buf)) {
            continue;
        }
        if (collect_on_line) {
            outbufoffs += snprintf(outbuf+outbufoffs, sizeof(outbuf)-outbufoffs, "{%s} ", buf);
            if (outbufoffs > 90) {
                ALOG_WARN("%s", outbuf);
                outbufoffs = 0;
            }
        } else {
            ALOG_WARN("%s\n", buf);
        }
        usleep(1000); // Printing too fast may cause losong prints
    }
    if (collect_on_line && outbufoffs > 0) {
        ALOG_WARN("%s\n", outbuf);
    }

    fclose(f);
}

int regulator_match_function(const char* line)
{
    static const char* include_if_contains[] = {
        "+",
        "VaudioSupply",
        "VapeRegu",
        "SysUlpClkCtrl1",
        "SysClkCtrl",
        "VusbCtrl",
        "ReguCtrl1VAmic" };

    int num_print_lines = sizeof(include_if_contains) / sizeof(include_if_contains[0]);
    int i;
    for (i=0 ; i < num_print_lines ; i++) {
        if (strstr(line, include_if_contains[i]) != 0) {
            return 1;
        }
    }

    return 0;
}

static void get_debugfs_mountpoint(char **debugfs_path)
{
    FILE *file;
    char fstype[40], mountpt[128];
    int found = 0;

    file = fopen("/proc/mounts", "r" );
    if (file == NULL) {
        ALOG_ERR("Can not open file /proc/mounts: %s\n", strerror(errno));
        return;
    }

    while (fscanf(file, "%*s %s %s %*s %*d %*d\n", mountpt, fstype) != EOF) {
        if (strcmp(fstype, "debugfs") == 0) {
            found = 1;
            *debugfs_path = strdup(mountpt);
            break;
        }
    }

    if ((!found) && (!feof(file)) && (ferror(file))) {
        ALOG_ERR("Error parsing file /proc/mounts: %s\n", strerror(errno));
    }
    fclose(file);
}

static void print_ab8500_registers()
{
    char *debugfs_path = NULL;
    char filename[128];

    get_debugfs_mountpoint(&debugfs_path);
    if(debugfs_path == NULL) {
        ALOG_ERR("Cannot find path to debugfs for AB8500 register dump");
        return;
    }

    sprintf(filename, "%s/ab8500-regulator/dump", debugfs_path);
    print_file(filename, 0, regulator_match_function);

    sprintf(filename, "%s/ab8500/register-bank", debugfs_path);
    FILE *rb = fopen(filename, "wb");
    if (rb == NULL) {
        ALOG_ERR("dumpAB8500: Failed to open %s", filename);
        goto clean_up;
    }

    fprintf(rb,"0x0D");
    fclose(rb);

    sprintf(filename, "%s/ab8500/all-bank-registers", debugfs_path);
    print_file(filename, 1, 0);

    sprintf(filename, "%s/clk/msp1clk/requests", debugfs_path);
    ALOG_WARN("Dumping %s", filename);
    print_file(filename, 0, 0);

clean_up:
    if (debugfs_path != NULL) free(debugfs_path);
}

void dbg_print_debug_information(void)
{
    // call stedump to get mpc and arm circular buffer dumps
    ALOG_WARN("Requesting sia and arm trace buffer dumps");
    //system("/system/bin/stedump");
    //print_ab8500_registers();
    //dump_hw_registers();
    //ste_adm_omx_core_dump();
    //ste_adm_debug_logbuf_show();
}

static void print_callstack_line(unsigned int addr)
{
    FILE* f=fopen("/proc/self/maps", "rb");
    if (f) {
        unsigned int start, end;
        char line[120];
        while (fgets(line, 120, f)) {
            sscanf(line, "%X-%X", &start, &end);
            if (addr >= start && addr <= end) {
                if (*line) {
                    line[strlen(line)-1] = 0; // chop \n
                    char* libname = line + strlen(line);
                    while (libname > line && *libname != ' ') libname--;
                    // Almost same format as debuggerd to re-use scripts..
                    ALOG_WARN("   (debug callstack) #00  pc %x   %s\n", addr - start, libname);
                    fclose(f);
                    return;
                }
            }
        }
        fclose(f);
    }

    ALOG_WARN("   (debug callstack) unresolved %X\n", addr);
}

#ifndef ADM_DBG_X86
static _Unwind_Reason_Code trace_function(_Unwind_Context *context, void *arg)
{
   (void) arg;
   intptr_t ip = (intptr_t)_Unwind_GetIP(context);
   if (ip) {
            print_callstack_line((unsigned int) ip);
        }
   return _URC_OK;
}


void ste_adm_debug_print_callstack(void)
{
    ALOG_WARN("Start of callstack dump for debug\n");
    _Unwind_Backtrace(trace_function, 0);
    ALOG_WARN("End of callstack dump for debug\n");
}

#endif


///////////////////////////////////////////////////////////////////////////////
//
// Generate a core dump
// Must limit this to admsrv process, and only limited number on disc
// to not fill disc
// We can't modify e.g. init.rc
//
// Dumping core on sdcard does not work. You end up with a 0 bytes 'core' file.
// The reason is that Linux requires the UID of the inode to match the fsuid of the
// process that dumps core. On MMC in MBL, the MMC is mounted uid=1000, so the uid
// will always be 1000 != fsuid (which is typically 0)
//

static void print_file_content_if_diff(const char* filename, const char* expected)
{
    int fd = open(filename, O_RDONLY);
    if (fd >= 0) {
        char buf[80];
        int result = read(fd, buf, 79);
        if (result <= 0) {
            ALOG_WARN("read on %s failed, errno %d\n", filename, errno);
        } else {
            buf[result] = 0;
            if (strcmp(expected, buf) != 0) {
                if (buf[0] && buf[strlen(buf)-1] == '\n') {
                    buf[strlen(buf)-1]=0;
                }
                ALOG_WARN("Content of %s is '%s'\n", filename, buf);
            }
        }
        close(fd);
    } else {
        ALOG_WARN("print_file_content_if_diff couldn't open %s\n", filename);
    }
}

static int core_pattern_is_complex_path(void)
{
    int complex_path = 0;
    int fd = open("/proc/sys/kernel/core_pattern", O_RDONLY);
    if (fd >= 0) {
        char buf[1];
        int result = read(fd, buf, 1);
        if (result != 1) {
            ALOG_WARN("read on /proc/sys/kernel/core_pattern failed, errno %d\n", errno);
        } else if (buf[0] == '/' || buf[0] == '|') {
            complex_path = 1;
        }
        close(fd);
    } else {
        ALOG_WARN("core_pattern_is_complex_path: couldn't open /proc/sys/kernel/core_pattern, errno=%d\n", errno);
    }
    return complex_path;
}


static void remove_old_dumps(const char* adm_core_dir, long long max_bytes)
{
    int max_tries = 10;
    while(max_tries--) {
        DIR* dir = opendir(adm_core_dir);
        if (!dir) {
            ALOG_ERR("remove_old_dumps couldn't open %s\n", adm_core_dir);
            return;
        }

        struct dirent ent;
        struct dirent* ent_p = 0;
        time_t oldest = 0;
        long long total_bytes = 0;
        while(!readdir_r(dir, &ent, &ent_p) && ent_p) {
            struct tm tm;
            memset(&tm, 0, sizeof(tm));
            sscanf(ent.d_name, "%02d_%02d-%02d_%02d_%02d", &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
            tm.tm_mon--;
            tm.tm_isdst = -1;
            tm.tm_year = 100; // 2000; anything leap year will work
            if (tm.tm_mday >= 1) {
                time_t cur = mktime(&tm);
                if (oldest == 0 || oldest > cur) {
                    oldest = cur;
                }

                struct stat st;
                char fname[PATH_MAX];
                snprintf(fname, sizeof(fname), "%s/%s/core", adm_core_dir, ent.d_name);
                stat(fname, &st);
                total_bytes += st.st_size;

                ALOG_INFO("%s is %d bytes, time %d\n", fname, st.st_size, cur);
            }
        }
        closedir(dir);

        if (oldest != 0 && total_bytes > max_bytes) {
            ALOG_STATUS("remove_old_dumps: %d Mb > %d Mb, removing oldest dump\n", (int) (total_bytes/1024/1024), (int) (max_bytes/1024/1024));

            char fname[PATH_MAX];
            struct tm tm;
            localtime_r(&oldest, &tm);
            snprintf(fname, sizeof(fname), "%s/%02d_%02d-%02d_%02d_%02d/core", adm_core_dir, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            ALOG_STATUS(" --> Removing file %s\n", fname);
            if (unlink(fname)) {
                ALOG_ERR("Couldn't unlink file %s, errno=%d\n", fname, errno);
            }
            snprintf(fname, sizeof(fname), "%s/%02d_%02d-%02d_%02d_%02d", adm_core_dir, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            ALOG_STATUS(" --> Removing dir %s\n", fname);
            if (rmdir(fname)) {
                ALOG_ERR("Couldn't rmdir %s, errno=%d\n", fname, errno);
            }
        } else {
           ALOG_INFO("total bytes in dumps: %lld, max %lld\n", total_bytes, max_bytes);
           return;
        }
    }
}

static void ste_adm_debug_die_with_core(const char* adm_core_dir, long long max_dump_bytes)
{
    struct rlimit rlim;
    if (getrlimit(RLIMIT_CORE, &rlim)) {
        ALOG_ERR("getrlimit failed, errno=%d\n", errno);
        return;
    }

    rlim.rlim_cur = rlim.rlim_max;
    if (setrlimit(RLIMIT_CORE, &rlim)) {
        ALOG_ERR("setrlimit %d %d failed, errno %d\n", rlim.rlim_cur, rlim.rlim_max, errno);
        return;
    }

    if (mkdir(adm_core_dir, 0700)) {
        if (errno != EEXIST) {
            ALOG_ERR("mkdir %s failed, errno %d\n", adm_core_dir, errno);
            return;
        }
    }

    if (core_pattern_is_complex_path()) {
        ALOG_INFO("/proc/sys/kernel/core_pattern is complex path (starts with / or |), let's not try to figure\n");
        ALOG_INFO("out what it does. Just dump core.\n");
    } else {
        // core_pattern is probably just a file name. We can do tricks like place ourself in a
        // suitable folder with date, remove old dumps, etc.
        remove_old_dumps(adm_core_dir, max_dump_bytes);

        // Check if there is reasonabe amount of free space on the file system,
        // to avoid being the one who fills it all up..
        struct statfs fs_info;
        if (statfs(adm_core_dir, &fs_info)) {
            ALOG_ERR("statfs on %s failed, errno %d\n", adm_core_dir, errno);
            return;
        }

        long long bytes_free = fs_info.f_bsize * fs_info.f_bavail;
        if (bytes_free < ADM_CORE_DUMP_MIN_FREE_SPACE) {
            ALOG_ERR("Only %lld bytes free on %s; skipping core dump\n", bytes_free, adm_core_dir);
            return;
        }

        long long root_bytes_free = fs_info.f_bsize * fs_info.f_bfree;
        long long total_size = fs_info.f_bsize * fs_info.f_blocks;
        int fraction_free = (int) (100 * root_bytes_free / total_size);
        if (fraction_free < ADM_CORE_DUMP_MIN_FREE_PERCENT) {
            ALOG_ERR("Only %d%% free on %s; skipping core dump\n", fraction_free, adm_core_dir);
            return;
        }

        // for FAT, without inodes, f_files will be 0, so ffree is also invalid
        if (fs_info.f_files > 0 && fs_info.f_ffree < 100) {
            ALOG_ERR("Only %d free inodes on %s; skipping core dump\n", fs_info.f_ffree, adm_core_dir);
           return;
        }

        ALOG_INFO("%lld bytes (%d%%) free on %s\n", bytes_free, fraction_free, adm_core_dir);


        time_t tt = time(0);
        struct tm tm;
        localtime_r(&tt, &tm);

        char dir_with_date[PATH_MAX] = {0};
        snprintf(dir_with_date, sizeof(dir_with_date), "%s/%02d_%02d-%02d_%02d_%02d", adm_core_dir, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

        if (mkdir(dir_with_date, 0700)) {
            ALOG_ERR("mkdir %s failed, errno %d\n", adm_core_dir, errno);
            return;
        }

        if (chdir(dir_with_date)) {
            ALOG_ERR("chdir failed, errno %d\n", errno);
            return;
        }

        ALOG_STATUS("ADM changed current dir to %s to attempt to get core dump placed in this dir\n", dir_with_date);
    }


    // to debug what's going on if we don't get any core dumps
    print_file_content_if_diff("/proc/sys/kernel/core_pattern", "core\n");
    print_file_content_if_diff("/proc/sys/kernel/core_uses_pid", "0\n");
    print_file_content_if_diff("/proc/self/coredump_filter", "00000023\n");

    ALOG_STATUS("admsrv will try to generate core dump by sending SIGQUIT to itself\n");

    pid_t pid = getpid();
    if (kill(pid, SIGQUIT)) {
        ALOG_ERR("kill SIGQUIT failed, errno %d\n", errno);
        return;
    }

    usleep(400 * 1000);
    int wait_death = 10;
    while (wait_death--) {
        ALOG_INFO("QUIT signal did not yet crash process\n");
        usleep(200 * 1000);
    }

    ALOG_ERR("QUIT signal did not crash process\n");
}



void ste_adm_debug_coredump_and_die_if_enabled(void)
{
    char prop_coredir[PROPERTY_VALUE_MAX];
    char prop_coresize[PROPERTY_VALUE_MAX];

#ifdef ADM_DBG_X86
    return;
#else
    property_get("ste.debug.adm.coredir", prop_coredir, "");
    property_get("ste.debug.adm.coresize", prop_coresize, "");

    prop_coredir[PROPERTY_VALUE_MAX-1]=0;
    prop_coresize[PROPERTY_VALUE_MAX-1]=0;

    if (prop_coredir[0]) {
        ste_adm_debug_die_with_core(prop_coredir, (long long) atoi(prop_coresize) * 1024 * 1024);
    }
#endif
}


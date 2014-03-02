/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef COREDUMP_H
#define COREDUMP_H

#include "utils.h"

#define PREFIX_REPORT               "rpt_"
#define PREFIX_COREDUMP_INFO        "dumpinfo_"
#define PREFIX_COREDUMP_USB         "USB"

#define CONFIG_FILE                 "/system/etc/coredump.conf"

#define TRACE_MODEM_DRV             "/dev/db8500-modem-trace"
#define MLOADER_FW_DRV              "/dev/dbx500_mloader_fw"

#define AB8500_DUMP_REG             "/sys/kernel/debug/ab8500/all-banks"
#define PRCMU_DUMP_REG              "/sys/kernel/debug/prcmu/regs"
#define PRCMU_DUMP_MEM              "/sys/kernel/debug/prcmu/data_mem"
#define SHRM_FTRACE                 "/sys/kernel/debug/tracing/trace"

#define TRIGGER_PATH                "/sdcard/triggers"
#define DEFAULT_MODEM_DUMP          "/sdcard/core_dumps/modem"
#define MODEM_SHM_DATA              "modem_shared"
#define AB8500_DUMP_NAME            "ab8500_register_dump"
#define PRCMU_MEMDUMP_NAME          "prcmu_mem_dump"
#define PRCMU_REGDUMP_NAME          "prcmu_reg_dump"
#define SHRM_FTRACE_NAME            "shrm-ftrace"
#define XFILE_DIRECTORY             "/data/xfiles"

#define MODEMDUMP_MSG               "Modem coredump available "

/* Average size of an modem core dump */
#define CORE_DUMP_DATA_LENGTH      20*1024*1024

#ifdef SPLIT_TRACE_FILES
#define MLR_MAX_EVAC_FILE 4
static const char *filename_suffix[] = {
    "_L1_L",
    "_L23_L",
    "_L1_M",
    "_L23_M"
};

enum {
    MLR_MEDIUM_BUF_TYPE,
    MLR_LONG_BUF_TYPE,
    MLR_MAX_TYPE_BUF_TYPE
};

static const int mlr_buffer_type[] = { MLR_LONG_BUF_TYPE, \
                                       MLR_LONG_BUF_TYPE, MLR_MEDIUM_BUF_TYPE, MLR_MEDIUM_BUF_TYPE
                                     };

#else
#define MLR_MAX_EVAC_FILE 1
#endif

char log_complete_status[NAME_LENGTH];
char trigger_directory[NAME_LENGTH];
int use_debug;
int dump_fd;

/* Max number of core dumps to keep on SDcard (0 = Unlimited) */
int dump_files;

/* Max number of trigger report to keep on SDcard (0 = Unlimited) */
int trigger_files;

int coredump_generation(int fd_dump, char *coredump_path, char *trace_filename);
int report_generation(int fd_dump, char *report_path, char *trace_filename,
                      char *report_name, char *modem_version);
int coredump_write_trace_data(char *, char *);

struct secure_modem_mem_dump {
    uint8_t mem_id;
    char name[NAME_LENGTH];
    uint32_t size;
};

#endif /* ifndef COREDUMP_H */


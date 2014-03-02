/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "xfiledecoder.h"
#include "decodeinfo.h"
#include "utils.h"
#include "memfile.h"
#include "rilhandler.h"

#include "l1_os_trace_data.h"
#include "l23_os_trace_data.h"
#include "mm_l1_identifiers.h"
#include "mm_l23_identifiers.h"


static uint8_t creator_big_endian = 0;
static uint8_t reader_big_endian = 0;

static FILE *out_file;
static FILE *src_file;

static uint8_t cpuID = 0;
static char x_error_reason[LINE_LENGTH];

static ril_reset_info_t ril_reset_info;


/** @brief Hex representations of MM L1 rnd identifiers. */
static const char *mm_identifiers[] = {
    NULL,
    mm_l23_identifiers_txt,
    mm_l1_identifiers_txt
};

/** @brief Length of MM L1 rnd identifiers. */
static unsigned int mm_identifiers_len[] = {
    0,
    MM_L23_IDENTIFIERS_TXT_LEN,
    MM_L1_IDENTIFIERS_TXT_LEN
};

/** @brief Hex representations of OS trace data. */
static const char *os_trace_data[] = {
    NULL,
    l23_os_trace_data_txt,
    l1_os_trace_data_txt
};

/** @brief Length of OS trace data. */
static unsigned int os_trace_data_len[] = {
    0,
    L23_OS_TRACE_DATA_TXT_LEN,
    L1_OS_TRACE_DATA_TXT_LEN
};

/*
 * is_creator_bigendian
 *
 * Checks if file was created on CPU using big or little endian
 *
 * input  : NA
 * output : 1 Big Endian
 *          0 Little Endian
 */
static int is_creator_bigendian(uint8_t byte)
{
    if (byte & 0x80) {
        return 1;
    }

    return 0;
}

/*
 * is_reader_bigendian
 *
 * Checks if platform is reading big or little endian
 *
 * input  : NA
 * output : 1 Big Endian
 *          0 Little Endian
 */
static int is_reader_bigendian()
{
    return (get_endian() == BIGENDIAN) ? 1 : 0;
}

/*
 * open_modem_shared
 *
 * Opens modem_shared part of core dump files.
 * modem_shared inlcudes X-file information.
 *
 * input  : Path to modem_shared file
 * output : File*
 *          NULL if NOT OK
 */
static FILE *open_modem_shared(char *path)
{
    FILE *file = fopen(path, "rb");
    return file;
}

/*
 * get_file_size
 *
 * Returns file size of specified file
 * input  : FILE*
 * output : >0 OK
 *          other value on errors
 */
static unsigned long int get_file_size(FILE *file)
{
    unsigned long int size = 0;

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);
    }

    return size;
}

/*
 * create_out_file
 *
 * Creates decoded output file
 * input  : Path to output file
 * output : FILE*
 *          NULL if NOT OK
 */
static FILE *create_out_file(char *path)
{
    FILE *file = fopen(path, "w");
    return file;
}

/*
 * close_files
 *
 * Closes both input and output file
 * input  : NA
 * output : NA
 */
static void close_files()
{
    if (src_file != NULL) {
        fclose(src_file);
        src_file = NULL;
    }

    if (out_file != NULL) {
        fclose(out_file);
        out_file = NULL;
    }
}

static void parse_identifier_file(uint32_t value)
{
    char line[LINE_LENGTH];
    char error_reason[LINE_LENGTH];
    unsigned int read_value;
    int ret;
    int reason_found = 0;

    if (cpuID > 0 && value > 0) {
        MFILE *mf = memfopen(mm_identifiers[cpuID], mm_identifiers_len[cpuID]);

        if (NULL != mf) {
            // Read file line by line until match found.
            while (memfreadline(mf, line, LINE_LENGTH) != EOF) {
                ret = sscanf(line, "%s %d", error_reason, &read_value);

                if ((ret == 2) && (read_value == value)) {
                    // Copy all content of error_reason except last char.
                    snprintf(x_error_reason, strlen(error_reason), "%s",
                             error_reason);
                    reason_found = 1;
                    break;
                }
            }

            memfclose(mf);
        } else {
            ALOGE("Cannot open identifier file\n");
        }
    }

    if (!reason_found) {
        snprintf(x_error_reason, LINE_LENGTH, "0x%08X", value);
    }
}

/**
 * strip_ansi_comment
 *
 * Removes the sourrounding comment markers from the string.
 *
 * @brief Strip ANSI comment.
 *
 * @param [in] str String with comment markers.
 *
 * @return Returns the string without comment markers.
 */
static char *strip_ansi_comment(char *str)
{
    char *s1 = strchr(str, '*');
    char *s2 = strrchr(str, '*');

    if ((NULL != s1) && (NULL != s2)) {
        if (s2 > s1) {
            *s2 = '\0';
            return s1 + 1;
        }
    }

    return NULL;
}

/**
 * parse_os_trace_data
 *
 * @brief Parse l(1|23).os_trace_data file.
 *
 * @param [in]  section    Section in an os_trace_data file.
 * @param [in]  value      Value to parse in the specified section.
 * @param [out] trace      Output buffer for value parsing.
 * @param [in]  trace_size Size of output buffer.
 */
static void parse_os_trace_data(const char *section, int32_t value,
                                char *trace, size_t trace_size)
{
    char line[LINE_LENGTH];
    char tmp[LINE_LENGTH];
    MFILE *mf;
    size_t cpuID_max;
    int found_section = 0;
    int found_trace = 0;

    trace[0] = '\0';

    cpuID_max = (sizeof(os_trace_data) / sizeof(os_trace_data[0])) - 1;

    // Verify CPU id.
    if (cpuID == 0 || (cpuID > cpuID_max)) {
        ALOGE("CPU id out of range: %d\n", cpuID);
        return;
    }

    // Open the os_trace_data file.
    mf = memfopen(os_trace_data[cpuID], os_trace_data_len[cpuID]);

    if (NULL == mf) {
        ALOGE("Cannot open OS trace data\n");
        return;
    }

    // Read data line by line until the section is found.
    while (memfreadline(mf, line, LINE_LENGTH) != EOF) {
        if (NULL != strstr(line, section)) {
            found_section = 1;
            break;
        }
    }

    // Read the current section to find the trace value.
    if (found_section) {
        while (memfreadline(mf, line, LINE_LENGTH) != EOF) {
            if (strlen(line) == 0) {
                break;
            }

            // Check current line if it contains the value.
            char *s = strip_ansi_comment(line);

            if (NULL != s) {
                int32_t read_value;
                int32_t ret = sscanf(s, "%s %d", tmp, &read_value);

                if ((EOF != ret) && (ret == 2) && (read_value == value)) {
                    found_trace = 1;
                    break;
                }
            }
        }
    }

    memfclose(mf);

    if (found_trace) {
        snprintf(trace, trace_size - 1, "%s ", tmp);
    }
}

/*
 * calc_time
 *
 * Converts int to hours, minutes, seconds and milliseconds
 * input  : time as integer
 *          h hours
 *          m minutes
 *          s seconds
 *          ms milliseconds
 * output : NA
 */
static void calc_time(uint32_t time, int *h, int *m, int *s, int *ms)
{
    *h = time / 3600000;
    *m = (time % 3600000) / 60000;
    *s = ((time % 3600000) % 60000) / 1000;
    *ms = ((time % 3600000) % 60000) % 1000;
}

/*
 * swap_endian_if_needed
 *
 * Swaps endian if reader and creator endian differs
 * input  : 32 bits int data to swap
 * output : 32 bits int data
 */
static uint32_t swap_endian_if_needed(uint32_t data)
{
    uint32_t read_data = data;

    if (creator_big_endian != reader_big_endian) {
        read_data = swap_endian_32(data);
    }

    return read_data;
}

/*
 * align_length
 *
 * Adjust length to read to adapt to 32 bits data alignment
 * input  : Length
 * output : 32 bit aligned length
 */
static uint16_t align_length(uint16_t length)
{
    uint16_t tmp_length = length;

    if ((length % 4) != 0) {
        int aligned_32bit = (length + 4 - 1) / 4;
        tmp_length = aligned_32bit * 4;
    }

    return tmp_length;
}

/*
 * read_16bit
 *
 * Reads 16 bit (2 bytes) from source file
 * input  : fp        Input file descriptor
 *          data      output buffer to store size
 * output : 2 byte if OK
 *          -1 if NOT OK
 */
static int read_16bit(FILE *fp, uint16_t *data)
{
    int result = -1;

    if (fp != NULL) {
        result = fread(data, 2, 1, fp);
    }

    return result;
}

/*
 * read_header_info
 *
 * Read information from header part of xfile
 * input  : fp     Input file descriptor
 * output : Header information as uint16
 */
static uint16_t read_header_info(FILE *fp)
{
    uint16_t header_info;
    int result = read_16bit(fp, &header_info);

    if (result != -1 && !is_reader_bigendian()) {
        header_info = (header_info >> 8) | (header_info << 8);
    }

    return header_info;
}

/*
 * get_xfile_size
 *
 * Returns size of xfile part of the modem_shared file
 * input  : fp     Input file descriptor
 * output : Xfile  size
 */
static uint16_t get_xfile_size(FILE *fp)
{

    uint16_t size = read_header_info(fp);
    //Deduct 16 bytes from size since we are not interested in xfile name
    size -= 16;

    return size;
}

/*
 * print_wgm_data
 *
 * Prints WGM data to file
 * input  : struct of type hw_version
 * output : NA
 */
static void print_wgm_data(hw_version hw_ver)
{

    if (hw_ver.host == 3) {
        fprintf(out_file, "\tWGM HOST: WGM_HOST_8500\n");
    } else {
        fprintf(out_file, "\tWGM HOST: WGM_HOST_UNKNOWN\n");
    }

    if (hw_ver.host_rev < sizeof(wgm_host_rev)) {
        fprintf(out_file, "\tWGM HOST REVISION: %s\n", wgm_host_rev[hw_ver.host_rev]);
    } else {
        fprintf(out_file, "\tWGM HOST REVISION: NA\n");
    }

    fprintf(out_file, "\tWGM BCD VERSION: WGM%01x.%01x\n", (hw_ver.wgm_b << 4)>>4, hw_ver.wgm_a);

}

/*
 * print_cpu_info
 *
 * Prints CPU data to file
 * input  : cpu_id
 * output : NA
 */
static void print_cpu_info(uint8_t cpu_id)
{
    if (cpu_id == CORTEX_L2) {
        fprintf(out_file, "\tCPU identifier: Cortex L2\n");
    } else if (cpu_id == CORTEX_L1) {
        fprintf(out_file, "\tCPU identifier: Cortex L1\n");
    } else {
        fprintf(out_file, "\tCPU identifier: UNKNOWN\n");
    }
}

/*
 * print_sw_version
 *
 * Prints SW version data to file
 * input  : character array containing sw version data
 * output : NA
 */
static void print_sw_version(char sw_version_data[64])
{
    int i;

    for (i = 0; i < 64; ++i) {
        if (sw_version_data[i] == '\n') {
            sw_version_data[i] = ' ';
        }
    }

    fprintf(out_file, "\tSW Version: %s\n", sw_version_data);
}

/*
 * decode_modeminfo
 *
 * Decode Modem info part of the X-file
 * input  : NA
 * output : size of data
 */
static int decode_modeminfo()
{
    modem_info data;
    fread(&data, sizeof(data), 1, src_file);
    creator_big_endian = is_creator_bigendian(data.cpu_id);
    int h, m, s, ms;
    uint32_t time = swap_endian_if_needed(data.system_time);
    calc_time(time, &h, &m, &s, &ms);

    fprintf(out_file, "\tASIC endianess: %s\n",
            endianess[creator_big_endian]);

    print_cpu_info(data.cpu_id);
    cpuID = data.cpu_id;

    fprintf(out_file, "\tReset reason: %s\n", reset_reason[data.reset_reason]);
    fprintf(out_file, "\tSystem time: %d (runtime: %d:%d:%d.%d)\n",
            time, h, m, s, ms);

    print_sw_version(data.sw_version_data);

    print_wgm_data(data.hw_vers);
    fprintf(out_file, "\t%s\n\t%s\n\n",
            data.build_id, data.company_id);

    // Save info needed by the RIL.
    ril_reset_info.reason = data.reset_reason;

    return sizeof(data);
}

/*
 * decode_assertinfo
 *
 * Decode Assert info part of the X-file
 * input  : NA
 * output : size of data
 */
static int decode_assertinfo()
{
    assert_info data;
    fread(&data, sizeof(data), 1, src_file);
    fprintf(out_file, "\tSource filename: %s\n\tSource line number: %d\n\n",
            data.source_name, swap_endian_if_needed(data.line_number));

    // Save info needed by the RIL.
    strncpy(ril_reset_info.assert.source_name, data.source_name,
        ASSERT_INFO_SRC_NAME_SIZE);
    ril_reset_info.assert.source_name[ASSERT_INFO_SRC_NAME_SIZE - 1] = '\0';
    ril_reset_info.assert.line_number = data.line_number;

    return sizeof(data);
}

/*
 * decode_task_swap_history
 *
 * Decode Task swap history info part of the X-file
 * input  : NA
 * output : size of data
 */
static int decode_task_swap_history()
{
    task_swap_history data;
    fread(&data, sizeof(data), 1, src_file);
    fprintf(out_file, "\tIndex to oldest entry : %d\n\n", swap_endian_if_needed(data.index));
    fprintf(out_file, "     Time          Task-State  Priority  Task   Event\n");
    unsigned int i = 0;
    int h, m, s, ms;

    for (i = 0; i < 128; i++) {
        uint32_t time = swap_endian_if_needed(data.info[i].time);
        calc_time(time, &h, &m, &s, &ms);
        fprintf(out_file, "   : %d:%02d:%02d.%03d    ", h, m, s, ms);

        if (data.info[i].event == 1) {
            uint curr_time = data.info[i].id << 16 |
                             data.info[i].priority << 8 |
                             data.info[i].state;
            fprintf(out_file, "Event 1 - Curr OS Time %u\n", curr_time);
        } else {
            fprintf(out_file, " %03d      %03d      %03d      %d\n",
                    data.info[i].state,  data.info[i].priority,
                    data.info[i].id, data.info[i].event);
        }
    }

    return sizeof(data);
}

/*
 * decode_error_info
 *
 * Decode error info part of the X-file
 * input  : NA
 * output : size of data
 */
static int decode_error_info()
{
    error_info data;
    fread(&data, sizeof(data), 1, src_file);

    parse_identifier_file(swap_endian_if_needed(data.x_error_code));
    fprintf(out_file, "\tError code\t\t\t: %s (0x%08X)\n",
            x_error_reason, swap_endian_if_needed(data.x_error_code));
    fprintf(out_file, "\tExtra error info\t: 0x%08X\n\n",
            swap_endian_if_needed(data.x_error_info));
    return sizeof(data);
}

/*
 * decode_os_info
 *
 * Decode last os info part of the X-file
 * input  : NA
 * output : size of data
 */
static int decode_os_info()
{
    os_info data;
    char desc[LINE_LENGTH];
    int32_t value;

    fread(&data, sizeof(data), 1, src_file);

    /*** Error desc ***/

    fprintf(out_file, "\tERROR INFORMATION\n");
    fprintf(out_file, "\tParameter\t\t\t\tValue\n");
    fprintf(out_file, "\t==============================================\n");

    // Task ID.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.error_info_task);
    parse_os_trace_data(OS_INFO_SECTION__TASK, value, desc, LINE_LENGTH);
    fprintf(out_file, "\tTask ID\t\t\t\t\t%s(%d)\n", desc, value);

    // Function ID.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.error_info_function_id);
    parse_os_trace_data(OS_INFO_SECTION__FUNCTION_ID, value, desc, LINE_LENGTH);
    fprintf(out_file, "\tFunction ID\t\t\t\t%s(%d)\n", desc, value);

    // Status.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.error_info_status_code);
    parse_os_trace_data(OS_INFO_SECTION__STATUS, (int8_t)value, desc, LINE_LENGTH);
    fprintf(out_file, "\tStatus\t\t\t\t\t%s(0x%02x)\n", desc, value);

    // Params.
    fprintf(out_file, "\tError info: Param 1\t\t0x%04x\n",
            swap_endian_if_needed(data.error_info_param1));
    fprintf(out_file, "\tError info: Param 2\t\t0x%04x\n",
            swap_endian_if_needed(data.error_info_param2));
    fprintf(out_file, "\tError info: Param 3\t\t0x%04x\n\n",
            swap_endian_if_needed(data.error_info_param3));

    /*** Warning desc ***/

    fprintf(out_file, "\tWARNING INFORMATION\n");
    fprintf(out_file, "\tParameter\t\t\t\tValue\n");
    fprintf(out_file, "\t==============================================\n");

    // Task ID.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.warning_info_task);
    parse_os_trace_data(OS_INFO_SECTION__TASK, value, desc, LINE_LENGTH);
    fprintf(out_file, "\tTask ID\t\t\t\t\t%s(%d)\n", desc, value);

    // Function ID.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.warning_info_function_id);
    parse_os_trace_data(OS_INFO_SECTION__FUNCTION_ID, value, desc, LINE_LENGTH);
    fprintf(out_file, "\tFunction ID\t\t\t\t%s(%d)\n", desc, value);

    // Status.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.warning_info_status_code);
    parse_os_trace_data(OS_INFO_SECTION__STATUS, (int8_t)value, desc, LINE_LENGTH);
    fprintf(out_file, "\tStatus\t\t\t\t\t%s(0x%02x)\n", desc, value);

    // Params.
    fprintf(out_file, "\tError info: Param 1\t\t0x%04x\n",
            swap_endian_if_needed(data.warning_info_param1));
    fprintf(out_file, "\tError info: Param 2\t\t0x%04x\n",
            swap_endian_if_needed(data.warning_info_param2));
    fprintf(out_file, "\tError info: Param 3\t\t0x%04x\n\n",
            swap_endian_if_needed(data.warning_info_param3));

    /*** Information desc ***/

    fprintf(out_file, "\tINFO INFORMATION\n");
    fprintf(out_file, "\tParameter\t\t\t\tValue\n");
    fprintf(out_file, "\t==============================================\n");

    // Task ID.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.info_info_task);
    parse_os_trace_data(OS_INFO_SECTION__TASK, value, desc, LINE_LENGTH);
    fprintf(out_file, "\tTask ID\t\t\t\t\t%s(%d)\n", desc, value);

    // Function ID.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.info_info_function_id);
    parse_os_trace_data(OS_INFO_SECTION__FUNCTION_ID, value, desc, LINE_LENGTH);
    fprintf(out_file, "\tFunction ID\t\t\t\t%s(%d)\n", desc, value);

    // Status.
    value = (int32_t)swap_endian_if_needed((uint32_t)data.info_info_status_code);
    parse_os_trace_data(OS_INFO_SECTION__STATUS, (int8_t)value, desc, LINE_LENGTH);
    fprintf(out_file, "\tStatus\t\t\t\t\t%s(0x%02x)\n", desc, value);

    // Params.
    fprintf(out_file, "\tError info: Param 1\t\t0x%04x\n",
            swap_endian_if_needed(data.info_info_param1));
    fprintf(out_file, "\tError info: Param 2\t\t0x%04x\n",
            swap_endian_if_needed(data.info_info_param2));
    fprintf(out_file, "\tError info: Param 3\t\t0x%04x\n\n",
            swap_endian_if_needed(data.info_info_param3));

    return sizeof(data);
}

/*
 * decode_abort_data
 *
 * Decode last os info part of the X-file
 * input  : NA
 * output : size of data
 */
static int decode_abort_data()
{
    abort_data data;
    fread(&data, sizeof(data), 1, src_file);
    fprintf(out_file, "\tR8_FIQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r8_fiq_mode));
    fprintf(out_file, "\tR9_FIQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r9_fiq_mode));
    fprintf(out_file, "\tR10_FIQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r10_fiq_mode));
    fprintf(out_file, "\tR11_FIQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r11_fiq_mode));
    fprintf(out_file, "\tR12_FIQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r12_fiq_mode));
    fprintf(out_file, "\tSPSR_FIQ\t\t: 0x%08X\n",
            swap_endian_if_needed(data.spsr_fiq_mode));
    fprintf(out_file, "\tSP_FIQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.sp_fiq_mode));
    fprintf(out_file, "\tLR_FIQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.lr_fiq_mode));
    fprintf(out_file, "\tSPSR_IRQ\t\t: 0x%08X\n",
            swap_endian_if_needed(data.spsr_irq_mode));
    fprintf(out_file, "\tSP_IRQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.sp_irq_mode));
    fprintf(out_file, "\tSP_IRQ\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.lr_irq_mode));
    fprintf(out_file, "\tSPSR_Undef\t\t: 0x%08X\n",
            swap_endian_if_needed(data.spsr_undefined_mode));
    fprintf(out_file, "\tSP_Undef\t\t: 0x%08X\n",
            swap_endian_if_needed(data.sp_undefined_mode));
    fprintf(out_file, "\tLR_Undef\t\t: 0x%08X\n",
            swap_endian_if_needed(data.lr_undefined_mode));
    fprintf(out_file, "\tSPSR_Abort\t\t: 0x%08X\n",
            swap_endian_if_needed(data.spsr_abort_mode));
    fprintf(out_file, "\tSP_Abort\t\t: 0x%08X\n",
            swap_endian_if_needed(data.sp_abort_mode));
    fprintf(out_file, "\tLR_Abort\t\t: 0x%08X\n",
            swap_endian_if_needed(data.lr_abort_mode));
    fprintf(out_file, "\tSPSR_Superv\t\t: 0x%08X\n",
            swap_endian_if_needed(data.spsr_supervisor_mode));
    fprintf(out_file, "\tSP_Superv\t\t: 0x%08X\n",
            swap_endian_if_needed(data.sp_supervisor_mode));
    fprintf(out_file, "\tLR_Superv\t\t: 0x%08X\n",
            swap_endian_if_needed(data.lr_supervisor_mode));
    fprintf(out_file, "\tFiller\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.filler));
    fprintf(out_file, "\tCPSR\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.cpsr));
    fprintf(out_file, "\tSP_System\t\t: 0x%08X\n",
            swap_endian_if_needed(data.sp_system_and_user_mode));
    fprintf(out_file, "\tLR_System\t\t: 0x%08X\n",
            swap_endian_if_needed(data.lr_system_and_user_mode));
    fprintf(out_file, "\tR0\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r0));
    fprintf(out_file, "\tR1\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r1));
    fprintf(out_file, "\tR2\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r2));
    fprintf(out_file, "\tR3\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r3));
    fprintf(out_file, "\tR4\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r4));
    fprintf(out_file, "\tR5\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r5));
    fprintf(out_file, "\tR6\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r6));
    fprintf(out_file, "\tR7\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r7));
    fprintf(out_file, "\tR8\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r8));
    fprintf(out_file, "\tR9\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r9));
    fprintf(out_file, "\tR10\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r10));
    fprintf(out_file, "\tR11\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r11));
    fprintf(out_file, "\tR12\t\t\t\t: 0x%08X\n",
            swap_endian_if_needed(data.r12));
    fprintf(out_file, "\tLR_Cur\t\t\t: 0x%08X\n\n",
            swap_endian_if_needed(data.lr_current));
    return sizeof(data);
}

/*
 * decode_data
 *
 * Checks header ID to call correct decode function
 * input  : Header ID
 *          Data length
 * output : NA
 */
static void decode_data(enum ID id, uint16_t length)
{
    int n = length;

    switch (id) {
    case MODEM_INFO:
        ALOGI("Decoding Modem Info with id: %d and length %d\n", id, length);
        fprintf(out_file, "==================================================\n");
        fprintf(out_file, "Modem Info (ID:%d, Length:%d)\n", id, length);
        fprintf(out_file, "==================================================\n\n");
        n = decode_modeminfo();
        break;
    case ERROR_INFO:
        ALOGI("Decoding Error Info with id: %d and length %d\n", id, length);
        fprintf(out_file, "==================================================\n");
        fprintf(out_file, "Error Info (ID:%d, Length:%d)\n", id, length);
        fprintf(out_file, "==================================================\n\n");
        n = decode_error_info();
        break;
    case OS_INFO:
        ALOGI("Decoding Last OS Info with id: %d and length %d\n", id, length);
        fprintf(out_file, "==================================================\n");
        fprintf(out_file, "Last OS Info (ID:%d, Length:%d)\n", id, length);
        fprintf(out_file, "==================================================\n\n");
        n = decode_os_info();
        break;
    case ABORT_DATA:
        ALOGI("Decoding Abort Data with id: %d and length %d\n", id, length);
        fprintf(out_file, "==================================================\n");
        fprintf(out_file, "Abort Data (ID:%d, Length:%d)\n", id, length);
        fprintf(out_file, "==================================================\n\n");
        n = decode_abort_data();
        break;
    case ASSERT_INFO:
        ALOGI("Decoding Assert Info with id: %d and length %d\n", id, length);
        fprintf(out_file, "==================================================\n");
        fprintf(out_file, "Assert Info (ID:%d, Length:%d)\n", id, length);
        fprintf(out_file, "==================================================\n\n");
        n = decode_assertinfo();
        break;
    case TASK_SWAP_HISTORY:
        ALOGI("Decoding Task Swap History with id: %d and length %d\n", id, length);
        fprintf(out_file, "==================================================\n");
        fprintf(out_file, "Task Swap History (ID:%d, Length:%d)\n", id, length);
        fprintf(out_file, "==================================================\n\n");
        n = decode_task_swap_history();
        break;
    default:
        fseek(src_file, length, SEEK_CUR);
        break;
    }

    if (n < length) {
        fseek(src_file, length - n, SEEK_CUR);
    }
}

/**
 * @brief Sends modem reset information to the RIL.
 */
static void send_ril_info(void)
{
    #define RIL_RESET_DESC "Modem reset: ("
    #define RIL_RESET_REASON "Reason: "
    #define RIL_RESET_SFILE ", Source filename: "
    #define RIL_RESET_SLINE ", Source line number: "
    #define RIL_MSG_OVERHEAD 16U // E.g. length of an integer, plus more.

    char *msg;
    char *reason = "";
    uint32_t size;

    ALOGI("Sending modem reset reason to RIL");

    // Look-up reset reason string.
    if (ril_reset_info.reason < ARRAY_SIZE(reset_reason)) {
        reason = reset_reason[ril_reset_info.reason];
    }

    // Calculate the size of the RIL message.
    size = strlen(RIL_RESET_DESC) + strlen(RIL_RESET_REASON) +
        strlen(RIL_RESET_SFILE) + strlen(RIL_RESET_SLINE) +
        strlen(reason) + strlen(ril_reset_info.assert.source_name) +
        RIL_MSG_OVERHEAD;

    // Build and send the RIL message.
    msg = malloc(size);
    if (msg) {
        snprintf(msg, size, "%s%s%s%s%s%s%d)", RIL_RESET_DESC,
            RIL_RESET_REASON, reason, RIL_RESET_SFILE,
            ril_reset_info.assert.source_name, RIL_RESET_SLINE,
            ril_reset_info.assert.line_number);

        send_to_ril(msg);
        free(msg);
    }
    else {
        ALOGE("Failed to allocated memory for the RIL message");
    }
}

int generate_dump_info(char *infile, char *outfile)
{
    uint16_t length = 0;
    long int size = 0;
    long int pos = 0;
    reader_big_endian = is_reader_bigendian();

    // Reset RIL reset info.
    ril_reset_info.reason = 1; // HW_RESET_UNKNOWN
    strcpy(ril_reset_info.assert.source_name, "-");
    ril_reset_info.assert.line_number = 0;

    src_file = open_modem_shared(infile);
    out_file = create_out_file(outfile);

    if (src_file == NULL || out_file == NULL) {
        ALOGE("Failed to open src file %s or out file %s", infile, outfile);
        close_files();
        return 1;
    }

    fseek(src_file, X_FILE_START_ADDR, SEEK_SET);
    fseek(src_file, FILE_INFO_SIZE, SEEK_CUR);

    pos = ftell(src_file);

    if (pos == -1) {
        ALOGE("generate_dump_info() : Error in ftell");
        close_files();
        remove(outfile);
        return 1;
    }

    size = pos + get_xfile_size(src_file);
    ALOGI("Looking for matching index in modem_shared");

    while (pos < size) {
        enum ID id = read_header_info(src_file);
        length = align_length(read_header_info(src_file));
        fseek(src_file, DLL_INFO, SEEK_CUR);
        decode_data(id, length);
        pos = ftell(src_file);

        if (pos == -1) {
            ALOGE("generate_dump_info() : Error in ftell while decoding items");
            close_files();
            remove(outfile);
            return 1;
        }

    }

    // Send modem reset info to RIL.
    send_ril_info();

    // Checks if out file is empty. If so delete it.
    if (get_file_size(out_file) == 0) {
        ALOGW("No dump information available");
        close_files();

        if (remove(outfile) != 0) {
            EXTRADEBUG("can't delete file %s", outfile);
        }
    } else {
        close_files();
    }

    return 0;
}

/*
 * Extract x-file from the modem shared file.
 *
 * Parameter  :
 *              modem_shared_path       Path to modem shared file
 *              xfile_path              Path to output file
 *
 *
 * Returns    : 0 on success, otherwise -1
 *
 */
int generate_xfile(char *modem_shared_path, char *xfile_path)
{
    FILE *fd_modem_shared, *fd_xfile;
    char *buffer;
    int size, ret;

    ALOGI("generate_xfile() : start generating xfile");

    // Open modem share file
    fd_modem_shared = fopen(modem_shared_path, "rb");

    if (fd_modem_shared == NULL) {
        ALOGE("generate_xfile() : Unable to open modem shared file %s", modem_shared_path);
        return -1;
    }

    // Open the x-file path
    fd_xfile = fopen(xfile_path, "wb");

    if (fd_xfile == NULL) {
        ALOGE("generate_xfile() : Unable to create output xfile %s", xfile_path);
        fclose(fd_modem_shared);
        return -1;
    }

    // Go to start of x-file
    fseek(fd_modem_shared, X_FILE_START_ADDR, SEEK_SET);

    // Skip file information
    fseek(fd_modem_shared, FILE_INFO_SIZE, SEEK_CUR);

    // Get the x-file size
    size = get_xfile_size(fd_modem_shared);
    EXTRADEBUG("generate_xfile() : xfile size %d", size);

    // Allocate buffer to read
    buffer = malloc(size);

    if (buffer == NULL) {
        ALOGE("generate_xfile() : malloc failed");
        ret = -1;
        goto cleanup;
    }

    // Read x-file part from modem share to buffer
    ret = fread(buffer, 1, size, fd_modem_shared);

    if (ret != size) {
        ALOGW("generate_xfile() : read error in modem shared file");
        ret = -1;
        goto cleanup;
    }

    // Write buffer to x-file
    ret = fwrite(buffer, 1, size, fd_xfile);

    if (ret != size) {
        ALOGW("generate_xfile() : write error in xfile");
        ret = -1;
        goto cleanup;
    }

    ALOGI("generate_xfile() : Successfully created xfile");
    ret = 0; // Success

    // Clean and leave
cleanup:

    if (buffer != NULL) {
        free(buffer);
    }

    fclose(fd_modem_shared);
    fclose(fd_xfile);

    return ret;
}

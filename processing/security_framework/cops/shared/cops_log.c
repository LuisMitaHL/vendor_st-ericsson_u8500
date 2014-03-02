/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/
#include <cops_common.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdint.h>

#ifndef COPS_ENABLE_ENV_MODE_ITP
#include <unistd.h>
#else
#include <xstdio.h>
#endif

#ifdef COPS_OSE_ENVIRONMENT
#include <r_os.h>
#if defined(COPS_ENABLE_LOG_TO_FILE) && !defined(COPS_IN_LOADERS)
static SEMAPHORE *cops_log_semaphore = NULL;
#endif
#endif

extern uint8_t run_mode;

#if defined(COPS_ENABLE_LOG_TO_FILE) && !defined(COPS_IN_LOADERS)
#define LOG_FILE_MAX_SIZE 0x16000
static void cops_log_to_file(char *buf);
#endif

#ifndef COPS_ENABLE_ENV_MODE_ITP
void cops_log(enum cops_log_level level, const char *file,
              const char *func, int line, const char *format, ...)
{
    char *buf = NULL;
    int buf_size = 1024;
    int pid = 0;
    va_list args;

    buf = malloc(buf_size);

    if (NULL == buf) {
        fprintf(stderr, "Malloc failed\n");
        return;
    }

#ifdef COPS_OSE_ENVIRONMENT
#ifndef COPS_IN_LOADERS
    pid = CURRENT_PROC();
#endif
#else
    pid = getpid();
#endif

    /* consider using level for something meaningful */
    (void) level;

    if (file != NULL) {
        const char *bname = strrchr(file, '/');

        if (bname != NULL) {
            bname++;
        } else {
            bname = file;
        }

        (void)snprintf(buf, buf_size, "COPS[%d]: %s:%d %s(): ",
                       pid, bname, line, func);
    } else if (func != NULL) {
        (void)snprintf(buf, buf_size, "COPS[%d]: %s():%d: ",
                       pid, func, line);
    } else {
        (void)snprintf(buf, buf_size, "COPS[%d]: ", pid);
    }

    /* Append variadic arguments */
    va_start(args, format);
    (void)vsprintf(&buf[strlen(buf)], format, args);
    va_end(args);

    /* Print to out stream TODO: Use T&V here */
#ifndef COPS_IN_LOADERS
    fprintf(stderr, "%s", buf);
#else
    printf("%s", buf);
#endif

    /* Log to file */
#if defined(COPS_ENABLE_LOG_TO_FILE) && !defined(COPS_IN_LOADERS)
    cops_log_to_file(buf);
#endif

    free(buf);
}
#endif /* COPS_ENABLE_ENV_MODE_ITP */

#if defined(COPS_ENABLE_LOG_TO_FILE) && !defined(COPS_IN_LOADERS)
static void cops_log_to_file(char *buf)
{
    FILE *logfp = NULL;
    uint32_t file_len;
#ifdef COPS_OSE_ENVIRONMENT
    char file_name[64];
    char backup_file_name[64];
    (void)snprintf(file_name, 64, "%scops_%d.log",
                   STR(COPS_STORAGE_DIR), run_mode);
    (void)snprintf(backup_file_name, 64, "%sbcops_%d.log",
                   STR(COPS_STORAGE_DIR), run_mode);

    /* Create(if applicable) and take semaphore */
    if (NULL == cops_log_semaphore) {
        cops_log_semaphore = CREATE_SEM((OSSEMVAL)1);
    }
    WAIT_SEM(cops_log_semaphore);
#else
    char file_name[] = STR(COPS_LOG_FILE);
    char backup_file_name[] = STR(BACKUP_COPS_LOG_FILE);
#endif

    logfp = fopen(file_name, "a");

    if (logfp == NULL) {
        fprintf(stderr, "Failed to open log file(%s). %s\n",
                file_name, strerror(errno));
        goto function_exit;
    }

    /* Check if the file is too big and should be backup-ed and erased */
    if (fseek(logfp, 0, SEEK_END) != 0) {
        fprintf(stderr, "Failed to seek end of log file(%s). %s\n",
                file_name, strerror(errno));
        goto function_exit;
    }
    file_len = ftell(logfp);

    if (fseek(logfp, 0, SEEK_SET) != 0) {
        fprintf(stderr, "Failed to seek beginning of log file(%s). %s\n",
                file_name, strerror(errno));
        goto function_exit;
    }

    if (file_len > LOG_FILE_MAX_SIZE) {
        (void)remove(backup_file_name);

        if (rename(file_name, backup_file_name) != 0) {
            fprintf(stderr, "Failed to backup the log file(%s). %s\n",
                    file_name, strerror(errno));
            goto function_exit;
        }
        logfp = fopen(file_name, "w");

        if (logfp == NULL) {
            fprintf(stderr, "Failed to open log file(%s). %s\n",
                    file_name, strerror(errno));
            goto function_exit;
        }
    }

#ifdef COPS_OSE_ENVIRONMENT
    fwrite(buf, 1, strlen(buf), logfp);
#else
    fprintf(logfp, "%s", buf);
#endif

function_exit:
    if (logfp != NULL) {
        (void)fclose(logfp);
    }
#ifdef COPS_OSE_ENVIRONMENT
    /* Release semaphore */
    SIGNAL_SEM(cops_log_semaphore);
#endif
}
#endif /* COPS_ENABLE_LOG_TO_FILE */

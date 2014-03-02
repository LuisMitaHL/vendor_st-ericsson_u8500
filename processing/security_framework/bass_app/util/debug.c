#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <debug.h>

#define MIN(x, y) ((x) < (y)) ? (x) : (y)

/*
 * The length of the prefix is 37, for example it looks like this:
 * P = Prefix
 * M = Message
 * F = Function name
 * L = Line number
 * PPPP: MMMMM [FFFFFFFFFFFFFFF : LLLLL]
 */
#define PREFIX_LENGTH 37
#define MAX_PRINT_SIZE 256
#define MAX_FUNC_PRINT_SIZE 16

#ifdef BASS_APP_LOG_FILE
static void log_to_file(const char *buffer)
{
    FILE *log_file;
    log_file = fopen(BASS_APP_LOG_FILE, "a");

    if (log_file != NULL) {
        fprintf(log_file, "%s", buffer);
        fclose(log_file);
        log_file = NULL;
    }
}
#else
#define log_to_file(buffer)
#endif

#ifdef OS_FREE
int _dprintf(const char *function, int flen, int line, int level,
             const char *prefix, const char *fmt, ...)
{
    char buf[MAX_PRINT_SIZE];
    int err;

    va_list ap;
    va_start(ap, fmt);
    err = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    printf("%s", buf);
}
#else
int _dprintf(const char *function, int flen, int line, int level,
             const char *prefix, const char *fmt, ...)
{
    char buf[MAX_PRINT_SIZE - PREFIX_LENGTH];
    char print_buffer[MAX_PRINT_SIZE];
    char func_buf[MAX_FUNC_PRINT_SIZE];
    int err;
    va_list ap;

    memset(buf, 0, sizeof(buf));
    memset(print_buffer, 0, sizeof(buf));

    va_start(ap, fmt);
    err = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    /* Limit the function name to MAX_FUNC_PRINT_SIZE characters. */
    strncpy(func_buf, function, flen > MAX_FUNC_PRINT_SIZE ?
            (MAX_FUNC_PRINT_SIZE - 1) : flen);
    if (flen < (MAX_FUNC_PRINT_SIZE - 1)) {
        memset(func_buf + flen, 0x20, (MAX_FUNC_PRINT_SIZE - flen));
    }
    func_buf[MAX_FUNC_PRINT_SIZE - 1] = '\0';

    switch (level) {
    case CRITICAL:
        snprintf(print_buffer, MAX_PRINT_SIZE,
                 "%s: CRITI [%s : %05d] %s", prefix, func_buf, line, buf);
        break;

    case ERROR:
        snprintf(print_buffer, MAX_PRINT_SIZE,
                 "%s: ERROR [%s : %05d] %s", prefix, func_buf, line, buf);
        break;

    case ALWAYS:
        snprintf(print_buffer, MAX_PRINT_SIZE,
                "%s: ALWAY [%s : %05d] %s", prefix, func_buf, line, buf);
        break;

    case INFO:
        snprintf(print_buffer, MAX_PRINT_SIZE,
                "%s: INFO  [%s : %05d] %s", prefix, func_buf, line, buf);
        break;

    case FLOOD:
        snprintf(print_buffer, MAX_PRINT_SIZE,
                "%s: FLOOD [%s : %05d] %s", prefix, func_buf, line, buf);
        break;

    default:
        break;
    }

    fprintf(stdout, "%s", print_buffer);
    log_to_file(print_buffer);

    return err;
}
#endif

#if !defined(OS_FREE) && (defined(DEBUGLEVEL_3) || defined(DEBUGLEVEL_true) || \
    defined(DEBUGLEVEL_4))
void dump_buffer(const char *bname, const uint8_t *buffer, size_t blen)
{
    fprintf(stderr, "#### %s\n", bname);

    while (blen > 0) {
        size_t    n;

        for (n = 0; n < 16; n++) {
            if (n < blen) {
                fprintf(stderr, "%02x ", (int)buffer[n]);
            } else {
                fprintf(stderr, "   ");
            }

            if (n == 7) {
                fprintf(stderr, " ");
            }
        }

        fprintf(stderr, " |");

        for (n = 0; n < 16; n++) {
            if (n < blen) {
                if (isprint(buffer[n]))
                    fprintf(stderr, "%c", (int)buffer[n]);
                else {
                    fprintf(stderr, ".");
                }
            }
        }

        fprintf(stderr, "|\n");

        blen -= MIN(blen, 16);
        buffer += 16;
    }
}
#else
void dump_buffer(const char *bname, const uint8_t *buffer, size_t blen)
{
    (void)bname;
    (void)buffer;
    (void)blen;
}
#endif

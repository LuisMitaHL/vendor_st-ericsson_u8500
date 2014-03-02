#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include "util_log.h"
#include "r_smslinuxporting.h"

static util_log_type_t sms_log_level = (UTIL_LOG_TYPE_INFO | UTIL_LOG_TYPE_ERROR);
int sms_log_fd = -1;
int sms_log_file_wd = -1;
int sms_log_dir_wd = -1;

void sms_set_log_level(util_log_type_t log_level)
{
    if (log_level > 0) {
        /* It should not be possible to turn of ERROR logs. */
        sms_log_level = (log_level | UTIL_LOG_TYPE_ERROR);
    }
}

util_log_type_t sms_get_log_level()
{
    return sms_log_level;
}

int sms_log_init()
{
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;

    log_level = util_read_log_file(UTIL_LOG_MODULE_SMS);
    sms_set_log_level(log_level);
    sms_log_fd = inotify_init();

    if (sms_log_fd < 0) {
        SMS_LOG_E("Error %d (%s) when init inotify",errno, strerror(errno), UTIL_LOG_FILE);
        goto exit;
    }

    sms_log_file_wd = inotify_add_watch(sms_log_fd, UTIL_LOG_FILE, IN_MODIFY | IN_MOVE_SELF);

    if (sms_log_file_wd < 0) {
        SMS_LOG_E("Error %d (%s) when adding watch on file %s",errno, strerror(errno), UTIL_LOG_FILE);
    }

    sms_log_dir_wd = inotify_add_watch(sms_log_fd, UTIL_LOG_DIR, IN_CREATE);

    if (sms_log_dir_wd < 0) {
        SMS_LOG_E("Error %d (%s) when adding watch on dir %s",errno, strerror(errno), UTIL_LOG_DIR);
    }

exit:
    return sms_log_fd;
}

void sms_log_close()
{
    (void)inotify_rm_watch(sms_log_fd, sms_log_file_wd);
    (void)inotify_rm_watch(sms_log_fd, sms_log_dir_wd);
    close(sms_log_fd);
}

int sms_log_select_callback(const int fd, const void *data_p)
{
    struct inotify_event *inotify_data_p = NULL;
    char tmp_data[100];
    int res = -1;
    (void)data_p;
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;
    SMS_LOG_I("sms_log_select_callback");

    res = read(fd, &tmp_data, sizeof(tmp_data));

    if (res < 0) {
        SMS_LOG_E("Error %d (%s) when reading inotify ",
                  errno, strerror(errno));
        goto exit;
    }

    inotify_data_p = (struct inotify_event *)tmp_data;

    if ((inotify_data_p->mask & IN_IGNORED) > 0) {
        SMS_LOG_I("log configuration file updated, waiting for file to settle");
    }

    if ((inotify_data_p->mask & IN_CREATE) > 0 && strncmp(inotify_data_p->name, UTIL_LOG_FILE_NAME, inotify_data_p->len) == 0) {

        SMS_LOG_D("log file created add new watch");
        (void)inotify_rm_watch(sms_log_fd, sms_log_file_wd);
        sms_log_file_wd = inotify_add_watch(sms_log_fd, UTIL_LOG_FILE, IN_MODIFY);

        if (sms_log_file_wd < 0) {
            SMS_LOG_E("Error %d (%s) when adding watch on file ",
                      errno, strerror(errno), UTIL_LOG_FILE);
        }
    }

    if ((inotify_data_p->mask & IN_IGNORED) == 0) {
        log_level = util_read_log_file(UTIL_LOG_MODULE_SMS);
        sms_set_log_level(log_level);
    }

exit:
    return TRUE;
}
#ifndef PLATFORM_ANDROID
util_log_type_t util_read_log_file(util_log_module_t module)
{
    return UTIL_LOG_TYPE_ERROR;
}
#endif /* PLATFORM_ANDROID */

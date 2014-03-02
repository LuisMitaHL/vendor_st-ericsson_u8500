#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include "atc_log.h"
#include "atc_selector.h"

static int atc_log_fd = -1;
static int atc_log_file_wd = -1;
static int atc_log_dir_wd = -1;
static util_log_type_t atc_log_level = (UTIL_LOG_TYPE_INFO | UTIL_LOG_TYPE_ERROR);

bool atc_log_select_callback(int fd, void *data_p)
{
    struct inotify_event *inotify_data_p = NULL;
    char tmp_data[100];
    int res = -1;
    (void)data_p;
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;

    res = read(fd, &tmp_data, sizeof(tmp_data));

    if (res < 0) {
        ATC_LOG_E("Error %d (%s) when reading inotify ",
                 errno, strerror(errno));
        goto exit;
    }

    inotify_data_p = (struct inotify_event *)tmp_data;

    if ((inotify_data_p->mask & IN_IGNORED) > 0) {
        ATC_LOG_I("log configuration file updated, waiting for file to settle");
    }

    if ((inotify_data_p->mask & IN_CREATE) > 0 && strncmp(inotify_data_p->name, UTIL_LOG_FILE_NAME, inotify_data_p->len) == 0) {

        ATC_LOG_D("log file created add new watch");
        (void)inotify_rm_watch(atc_log_fd, atc_log_file_wd);
        atc_log_file_wd = inotify_add_watch(atc_log_fd, UTIL_LOG_FILE, IN_MODIFY);

        if (atc_log_file_wd < 0) {
            ATC_LOG_E("Error %d (%s) when adding watch on file ",
                     errno, strerror(errno), UTIL_LOG_FILE);
        }
    }

    if ((inotify_data_p->mask & IN_IGNORED) == 0) {
        log_level = util_read_log_file(UTIL_LOG_MODULE_AT);
        atc_set_log_level(log_level);
    }

exit:
    return TRUE;
}

void atc_log_init()
{
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;
    log_level = util_read_log_file(UTIL_LOG_MODULE_AT);
    atc_set_log_level(log_level);
    atc_log_fd = inotify_init();

    if (atc_log_fd < 0) {
        ATC_LOG_E("Error %d (%s) when init inotify",
                 errno, strerror(errno), UTIL_LOG_FILE);
        goto exit;

    }

    atc_log_file_wd = inotify_add_watch(atc_log_fd, UTIL_LOG_FILE, IN_MODIFY | IN_MOVE_SELF);

    if (atc_log_file_wd < 0) {
        ATC_LOG_E("Error %d (%s) when adding watch on file %s",
                 errno, strerror(errno), UTIL_LOG_FILE);
    }

    atc_log_dir_wd = inotify_add_watch(atc_log_fd, UTIL_LOG_DIR, IN_CREATE);

    if (atc_log_dir_wd < 0) {
        ATC_LOG_E("Error %d (%s) when adding watch on dir %s",
                 errno, strerror(errno), UTIL_LOG_DIR);
    }


    selector_register_callback_for_fd(atc_log_fd, atc_log_select_callback, NULL);
exit:
 return;
}

void atc_log_close()
{
    selector_deregister_callback_for_fd(atc_log_fd);
    (void)inotify_rm_watch(atc_log_fd, atc_log_file_wd);
    (void)inotify_rm_watch(atc_log_fd, atc_log_dir_wd);
    close(atc_log_fd);
}

void atc_set_log_level(util_log_type_t log_level)
{
    if (log_level > 0) {
        /* It should not be possible to turn of ERROR logs. */
        atc_log_level = (log_level | UTIL_LOG_TYPE_ERROR);
    }
}

util_log_type_t atc_get_log_level(void)
{
    return atc_log_level;
}

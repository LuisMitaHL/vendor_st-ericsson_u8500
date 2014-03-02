#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include "cn_log.h"
#include "fdmon.h"
#include "cn_macros.h"

static int cn_log_fd = -1;
static int cn_log_file_wd = -1;
static int cn_log_dir_wd = -1;

int cn_log_select_callback(const int fd, const void *data_p)
{
    struct inotify_event *inotify_data_p = NULL;
    char tmp_data[100];
    int res = -1;
    (void)data_p;
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;

    res = read(fd, &tmp_data, sizeof(tmp_data));

    if (res < 0) {
        CN_LOG_E("Error %d (%s) when reading inotify ",
                 errno, strerror(errno));
        goto exit;
    }

    inotify_data_p = (struct inotify_event *)tmp_data;

    if ((inotify_data_p->mask & IN_IGNORED) > 0) {
        CN_LOG_I("log configuration file updated, waiting for file to settle");
    }

    if ((inotify_data_p->mask & IN_CREATE) > 0 && strncmp(inotify_data_p->name, UTIL_LOG_FILE_NAME, inotify_data_p->len) == 0) {

        CN_LOG_D("log file created add new watch");
        (void)inotify_rm_watch(cn_log_fd, cn_log_file_wd);
        cn_log_file_wd = inotify_add_watch(cn_log_fd, UTIL_LOG_FILE, IN_MODIFY);

        if (cn_log_file_wd < 0) {
            CN_LOG_E("Error %d (%s) when adding watch on file ",
                     errno, strerror(errno), UTIL_LOG_FILE);
        }
    }

    if ((inotify_data_p->mask & IN_IGNORED) == 0) {
        log_level = util_read_log_file(UTIL_LOG_MODULE_CN);
        cn_set_log_level(log_level);
    }

exit:
    return TRUE;
}

void cn_log_init()
{
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;
    log_level = util_read_log_file(UTIL_LOG_MODULE_CN);
    cn_set_log_level(log_level);
    cn_log_fd = inotify_init();

    if (cn_log_fd < 0) {
        CN_LOG_E("Error %d (%s) when init inotify",
                 errno, strerror(errno), UTIL_LOG_FILE);
        goto exit;

    }

    cn_log_file_wd = inotify_add_watch(cn_log_fd, UTIL_LOG_FILE, IN_MODIFY | IN_MOVE_SELF);

    if (cn_log_file_wd < 0) {
        CN_LOG_E("Error %d (%s) when adding watch on file %s",
                 errno, strerror(errno), UTIL_LOG_FILE);
    }

    cn_log_dir_wd = inotify_add_watch(cn_log_fd, UTIL_LOG_DIR, IN_CREATE);

    if (cn_log_dir_wd < 0) {
        CN_LOG_E("Error %d (%s) when adding watch on dir %s",
                 errno, strerror(errno), UTIL_LOG_DIR);
    }

    fdmon_add(cn_log_fd, (void *)NULL, cn_log_select_callback, NULL);

exit:
    return;
}

void cn_log_close()
{
    fdmon_del(cn_log_fd);
    (void)inotify_rm_watch(cn_log_fd, cn_log_file_wd);
    (void)inotify_rm_watch(cn_log_fd, cn_log_dir_wd);
    close(cn_log_fd);
}

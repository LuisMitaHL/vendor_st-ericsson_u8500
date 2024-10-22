#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include "psccd.h"

static int pscc_log_fd = -1;
static int pscc_log_file_wd = -1;
static int pscc_log_dir_wd = -1;

int pscc_log_select_callback(const int fd, const void *data_p)
{
  struct inotify_event *inotify_data_p = NULL;
  char tmp_data[100];
  int res = -1;
  (void)data_p;
  util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;

  res = read(fd, &tmp_data, sizeof(tmp_data));

  if (res < 0) {
    PSCC_DBG_TRACE(LOG_ERR,"Error %d (%s) when reading inotify ",
                 errno, strerror(errno));
        goto exit;
    }

  inotify_data_p = (struct inotify_event *)tmp_data;

  if ((inotify_data_p->mask & IN_IGNORED) > 0) {
        PSCC_DBG_TRACE(LOG_INFO,"log configuration file updated, waiting for file to settle");
  }

  if ((inotify_data_p->mask & IN_CREATE) > 0 && strncmp(inotify_data_p->name, UTIL_LOG_FILE_NAME, inotify_data_p->len) == 0) {

    PSCC_DBG_TRACE(LOG_DEBUG,"log file created add new watch");
    (void)inotify_rm_watch(pscc_log_fd, pscc_log_file_wd);
    pscc_log_file_wd = inotify_add_watch(pscc_log_fd, UTIL_LOG_FILE, IN_MODIFY);

    if (pscc_log_file_wd < 0) {
      PSCC_DBG_TRACE(LOG_ERR,"Error %d (%s) when adding watch on file ",
                     errno, strerror(errno), UTIL_LOG_FILE);
    }
  }

  if ((inotify_data_p->mask & IN_IGNORED) == 0) {
    log_level = util_read_log_file(UTIL_LOG_MODULE_PSCC);
    pscc_set_log_level(log_level);
  }

exit:
  return TRUE;
}

int pscc_log_init()
{
  util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;
  log_level = util_read_log_file(UTIL_LOG_MODULE_PSCC);
  pscc_set_log_level(log_level);
  pscc_log_fd = inotify_init();

  if (pscc_log_fd < 0) {
    PSCC_DBG_TRACE(LOG_ERR,"Error %d (%s) when init inotify",
                 errno, strerror(errno), UTIL_LOG_FILE);
    goto exit;
  }

  pscc_log_file_wd = inotify_add_watch(pscc_log_fd, UTIL_LOG_FILE, IN_MODIFY | IN_MOVE_SELF);

  if (pscc_log_file_wd < 0) {
    PSCC_DBG_TRACE(LOG_ERR,"Error %d (%s) when adding watch on file %s",
                 errno, strerror(errno), UTIL_LOG_FILE);
  }

  pscc_log_dir_wd = inotify_add_watch(pscc_log_fd, UTIL_LOG_DIR, IN_CREATE);

  if (pscc_log_dir_wd < 0) {
    PSCC_DBG_TRACE(LOG_ERR,"Error %d (%s) when adding watch on dir %s",
                 errno, strerror(errno), UTIL_LOG_DIR);
  }

exit:
  return pscc_log_fd;
}

void pscc_log_close()
{
  (void)inotify_rm_watch(pscc_log_fd, pscc_log_file_wd);
  (void)inotify_rm_watch(pscc_log_fd, pscc_log_dir_wd);
  close(pscc_log_fd);
}

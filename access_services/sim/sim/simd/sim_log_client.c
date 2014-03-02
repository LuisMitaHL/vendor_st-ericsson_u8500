#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>
#include "simd.h"
#include "catd_reader.h"
#include "sim_log.h"


static util_log_type_t sim_log_level =  SIM_LOGGING_E;
static int sim_log_fd = -1;
static int sim_log_file_wd = -1;
static int sim_log_dir_wd = -1;
#define INPUT_BUFFER_SIZE       (4096)


int sim_log_get_log_fd()
{
    return sim_log_fd;
}

static void sim_set_log_level(util_log_type_t log_level)
{
    if (log_level > 0) {
        /* It should not be possible to turn of ERROR logs. */
        sim_log_level = (log_level | UTIL_LOG_TYPE_ERROR);
    }
}

static ssize_t sim_log_select_callback(char *buf, char *buf_max, void *ud)
{
    struct inotify_event *inotify_data_p = NULL;
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;
    SIM_LOG_E("sim_log_select_callback called");

    if(buf==NULL || buf_max==NULL) {
        SIM_LOG_E(" buf or buf_max received as NULL! Do cleanup ");
        /*Event stream is being deleted.Don't close FD sim_log_fd.
        This will be closed from function ste_es_array_read_and_parse*/
        (void)inotify_rm_watch(sim_log_fd, sim_log_file_wd);
        (void)inotify_rm_watch(sim_log_fd, sim_log_dir_wd);
        goto exit;
    }

    inotify_data_p = (struct inotify_event *)buf;

    if ((inotify_data_p->mask & IN_IGNORED) > 0) {
        SIM_LOG_I("log configuration file updated, waiting for file to settle");
    }

    if ((inotify_data_p->mask & IN_CREATE) > 0 && strncmp(inotify_data_p->name, UTIL_LOG_FILE_NAME, inotify_data_p->len) == 0) {

        SIM_LOG_D("log file created add new watch");
        (void)inotify_rm_watch(sim_log_fd, sim_log_file_wd);
        sim_log_file_wd = inotify_add_watch(sim_log_fd, UTIL_LOG_FILE, IN_MODIFY);

        if (sim_log_file_wd < 0) {
            SIM_LOG_E("Error %d (%s) when adding watch on file ",
                     errno, strerror(errno), UTIL_LOG_FILE);
        }
    }

    if ((inotify_data_p->mask & IN_IGNORED) == 0) {
        log_level = util_read_log_file(UTIL_LOG_MODULE_SIM);
        sim_set_log_level(log_level);
    }

    return (sizeof(struct inotify_event) + inotify_data_p->len);
exit:
    return 0;
}

static void sim_add_es_stream(ste_catd_reader_t      *catd_rdr)
{

    ste_es_parser_closure_t pc;
    pc.func = sim_log_select_callback;
    pc.user_data = NULL;
    ste_es_t *es = ste_es_new_normal(sim_log_get_log_fd(), INPUT_BUFFER_SIZE, &pc);

    if (!es)
    {
        SIM_LOG_E("Log Error: Invalid event stream created");
        return;
    }

    if( ste_catd_reader_add_es(catd_rdr, es))
    {
        SIM_LOG_E("Log Error: occured while adding Log event stream");

    }
}

void sim_log_init(ste_catd_reader_t      *catd_rdr)
{
    util_log_type_t log_level = UTIL_LOG_TYPE_UNKNOWN;
    log_level = util_read_log_file(UTIL_LOG_MODULE_SIM);
    sim_set_log_level(log_level);
    sim_log_fd = inotify_init();

    if (catd_rdr==NULL) {
        SIM_LOG_E("Log Error: catd_rdr not yet initialized");
        goto exit;
    }


    if (sim_log_fd < 0) {
        SIM_LOG_E("Error %d (%s) when init inotify",
                 errno, strerror(errno), UTIL_LOG_FILE);
        goto exit;
    }

    sim_log_file_wd = inotify_add_watch(sim_log_fd, UTIL_LOG_FILE, IN_MODIFY | IN_MOVE_SELF);

    if (sim_log_file_wd < 0) {
        SIM_LOG_E("Error %d (%s) when adding watch on file %s",
                 errno, strerror(errno), UTIL_LOG_FILE);
        goto exit;
    }

    sim_log_dir_wd = inotify_add_watch(sim_log_fd, UTIL_LOG_DIR, IN_CREATE);

    if (sim_log_dir_wd < 0) {
        SIM_LOG_E("Error %d (%s) when adding watch on dir %s",
                 errno, strerror(errno), UTIL_LOG_DIR);
    }
    sim_add_es_stream(catd_rdr);
exit:
    return;
}

void sim_log_close(ste_catd_reader_t      *catd_rdr)
{
    if (catd_rdr==NULL) {
        SIM_LOG_E("Log Error: catd_rdr invalid");
        return;
    }

    ste_catd_reader_rem_es(catd_rdr, sim_log_fd);
    (void)inotify_rm_watch(sim_log_fd, sim_log_file_wd);
    (void)inotify_rm_watch(sim_log_fd, sim_log_dir_wd);
    close(sim_log_fd);
}

util_log_type_t sim_get_log_level()
{
    return sim_log_level;
}

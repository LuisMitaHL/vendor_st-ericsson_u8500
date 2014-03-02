#define _GNU_SOURCE

/*
 * wd_kicker is a userspace watchdog kicker.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <sys/syscall.h>


#ifdef ANDROID
#include <android/log.h>
#endif

#ifdef ANDROID
#define WD_LOG_DEBUG ANDROID_LOG_DEBUG
#define WD_LOG_INFO ANDROID_LOG_INFO
#define WD_LOG_WARNING ANDROID_LOG_WARN
#define WD_LOG_ERR ANDROID_LOG_ERROR
#define WD_LOG_CRIT ANDROID_LOG_FATAL
#define SYSLOG(prio, ...) __android_log_print(prio, "kickdogd", ##__VA_ARGS__)
#define GETTID() gettid()
#define PERROR(S) SYSLOG(WD_LOG_ERR, "%s: %s", S, strerror(errno));

/* since linux/watchdog.h is not present among bionics kernel files */

#define _IOC_NRBITS	8
#define _IOC_TYPEBITS	8
#define _IOC_SIZEBITS	14
#define _IOC_DIRBITS	2

#define _IOC_WRITE	1U
#define _IOC_READ	2U

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT+_IOC_SIZEBITS)

#define _IOC(dir,type,nr,size) \
	(((dir)  << _IOC_DIRSHIFT) | \
	 ((type) << _IOC_TYPESHIFT) | \
	 ((nr)   << _IOC_NRSHIFT) | \
	 ((size) << _IOC_SIZESHIFT))

#define _IOC_TYPECHECK(t) (sizeof(t))
#define _IOWR(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))

#define	WATCHDOG_IOCTL_BASE	'W'

#define	WDIOC_SETTIMEOUT	_IOWR(WATCHDOG_IOCTL_BASE, 6, int)

#else

#include <linux/watchdog.h>

#define WD_LOG_DEBUG LOG_DEBUG
#define WD_LOG_INFO LOG_INFO
#define WD_LOG_WARNING LOG_WARNING
#define WD_LOG_ERR LOG_ERR
#define WD_LOG_CRIT LOG_CRIT
#ifdef DEBUG
#define SYSLOG(x,...) printf(__VA_ARGS__);
#else
#define SYSLOG syslog
#endif
#define GETTID() syscall(SYS_gettid)
#define PERROR(S) SYSLOG(WD_LOG_ERR, "%s: %s", S, strerror(errno));
#endif

/*
 * Variables
 */
const char *wd_dev_path = 0;
int kick_interval = 600;

int debug = 0;
int policy = 0;
int priority = 0;
volatile int stop = 0;

static void sig_handler(int n __attribute__((unused)))
{
	stop = stop ? 0 : 1;
}

int
set_process_prio(int policy, int prio)
{
	struct sched_param sp;
	int res;

	sp.sched_priority = prio;

	res = sched_setscheduler(GETTID(), policy, &sp);
	if (res) {
		SYSLOG(WD_LOG_ERR, "Failed call of set_setscheduler: %s",
		       strerror(errno));
	}
	return res;
}

void as_daemon(void)
{
	pid_t pid;
	int fd_0, fd_1, fd_2;

	/* Don't prevent the daemon from setting the file permissions the way it sees fit */
	umask(0);

	if (!debug) {
		pid = fork();
		if (pid < 0) {
			PERROR("fork failed");
			exit(EXIT_FAILURE);
		}
		else if (pid > 0) {
			exit(EXIT_SUCCESS); // Parent
		}
	}

	/* Ignore SIGHUP since this will be sent to all the other processes in the session when the
	   session leader terminates. */
	if (signal(SIGHUP, SIG_IGN)==SIG_ERR) {
		PERROR("signal ignore failed");
		exit(EXIT_FAILURE);
	}

	/* Stay in the root directory to avoid blocking umounts. */
	if (chdir("/")==-1) {
		PERROR("chdir failed");
		exit(EXIT_FAILURE);
	}


	/* Put fds 0, 1 and 2 on /dev/null in case any parts of the daemon tries to use them. They
	 * shouldn't and if they do we don't want the output showing up anywhere or any input to be
	 * grabbed. */
	if (close(0)==-1) {
		PERROR("close failed");
		exit(EXIT_FAILURE);
	}
	fd_0 = open("/dev/null", O_RDWR);
	fd_1 = dup2(fd_0, 1);
	fd_2 = dup2(fd_0, 2);

#ifndef ANDROID
	openlog("kickdogd", LOG_CONS, LOG_DAEMON);
#endif

	if (fd_0 != 0 || fd_1 != 1 || fd_2 != 2) { // This catches any errors (-1) in the open call as well
		SYSLOG(WD_LOG_ERR, "fd_0==%d, fd_1==%d, fd_2==%d", fd_0, fd_1, fd_2);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char * const *argv)
{
	int opt;
	char *conv_ret_str;
	int res;
	int fd;
	int count = 0;
	int tries;

	while ((opt = getopt(argc, argv, "d:Dht:r:f:")) != -1) {
		switch (opt) {
		case 'd':
			wd_dev_path = optarg;
			break;

		case 'D':
			debug = 1;
			break;

		case 'r':
			policy = SCHED_RR;
			priority = strtol(optarg, &conv_ret_str, 0);
			if (conv_ret_str == NULL) {
				SYSLOG(WD_LOG_ERR, "Could not convert priority argument string");
				exit(EXIT_FAILURE);
			}
			break;

		case 'f':
			policy = SCHED_FIFO;
			priority = strtol(optarg, &conv_ret_str, 0);
			if (conv_ret_str == NULL) {
				SYSLOG(WD_LOG_ERR, "Could not convert priority argument string");
				exit(EXIT_FAILURE);
			}
			break;

		case 't':
			kick_interval = strtol(optarg, &conv_ret_str, 0);
			if (kick_interval < 20) {
				SYSLOG(WD_LOG_ERR, "kick interval is too small: %d", kick_interval);
				exit(EXIT_FAILURE);
			}
			if (conv_ret_str == NULL) {
				SYSLOG(WD_LOG_ERR, "Could not convert timeout argument string");
				exit(EXIT_FAILURE);
			}
			break;

		case '?':
		case 'h':
		default: /* '?' */
			fprintf(stderr,
				"Usage: %s [options]\n\n"
				"    -d <dev>    Watchdog device to write (default no kick)\n"
				"    -D          Print debug info to stdout\n"
				"    -t <secs>   Kicker interval in seconds (default 120)\n"
				"    -r <prio>   Use RR schuduling with priority <prio>\n"
				"    -f <prio>   Use FIFO schuduling with priority <prio>\n"
				,argv[0]);
			exit(EXIT_FAILURE);
			break;
		}
	}

	as_daemon();

	policy = sched_getscheduler(0);
	if (policy==-1) {
		SYSLOG(WD_LOG_ERR, "sched_getscheduler failed");
		exit(EXIT_FAILURE);
	}

	if (priority < sched_get_priority_min(policy) || priority > sched_get_priority_max(policy)) {
		SYSLOG(WD_LOG_ERR, "Priority value out of range");
		exit(EXIT_FAILURE);
	}

	res = set_process_prio(policy, priority);
	if (res) {
		SYSLOG(WD_LOG_ERR, "Failed to set priority on thread\n");
		exit(EXIT_FAILURE);
	}

	if (debug) {
		switch (policy) {
		case SCHED_FIFO:  SYSLOG(WD_LOG_DEBUG, "SCHED_FIFO"); break;
		case SCHED_RR:  SYSLOG(WD_LOG_DEBUG, "SCHED_RR"); break;
		case SCHED_OTHER: SYSLOG(WD_LOG_DEBUG, "SCHED_OTHER"); break;
		}
	}

	if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
		SYSLOG(WD_LOG_ERR, "Could not set signal handler for SIGUSR1");
		exit(EXIT_FAILURE);
	}

	/*
	  This is where the WDs are actually started. If we want to abort, we should do it before
	  this, if at all possible.
	 */
	fd = wd_dev_path ? open(wd_dev_path, O_WRONLY) : 0;
	if (fd == -1) {
		SYSLOG(WD_LOG_ERR, "Watchdog device file %s could not be opened", wd_dev_path);
		exit(EXIT_FAILURE);
	}

	ioctl(fd, WDIOC_SETTIMEOUT, &kick_interval);

	SYSLOG(WD_LOG_INFO, "Starting watchdog kicker");

	while (1)
	{
		if (stop) {
			if (debug)
				SYSLOG(WD_LOG_DEBUG, " -> holding kick number %d",
				       count);
		}
		else {
			if (debug)
				SYSLOG(WD_LOG_DEBUG, " -> performing kick number %d",
				       count);

			/* Try several times. It's important. Then turn the watchdog off. */
			tries = 10;
			while (write(fd, "A", 1)!=1 && --tries) ;
			if (!tries) {
				SYSLOG(WD_LOG_CRIT, "failed to kick the watchdog, trying to turn it off");
				tries = 10;
				while (write(fd, "V", 1)!=1 && --tries) ;
				if (!tries) {SYSLOG(WD_LOG_CRIT, "Failed to write magic V to watchdog device");}
				close(fd);
				fd = 0;
			}
			count++;
		}
		/* Sleep until next kick is required */
		/* Make sure there is enough time for suspend + resume,
		 * since userspace is frozen but linux is still running
		 * preparing for or restoring after ApDeepSleep.
		 */
		sleep(kick_interval - 10);
	}

	return 0;
}


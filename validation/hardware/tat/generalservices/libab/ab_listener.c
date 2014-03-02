#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>

#include "ab_listener.h"

static int nfds;
static int npid2fd;
static struct pid_to_fd pid2fd[10];
static struct pollfd fds[10];
static unsigned char stop;

ab_listener *ab_value;

void abxxxx_close_fds(int sig)
{
	int i;

	if (sig == 0)
		return;

	stop = 0;

	for (i = 0; i < nfds ; i++)
		close(fds[nfds].fd);
}

void add_fd_to_poll(int pid, int sig)
{
	char tmp[50];
	int fd_irqx;

	printf("registering interrupt:%d, pid:%d, sig:%d",
		ab_value->interrupt, pid, sig);

	sprintf(tmp, "echo %d > %s", ab_value->interrupt + AB_DEBUGFS_BASE_INTERRUPT,
			AB_DEBUGFS_PATH_SUBSCRIBE);

	if ((system(tmp)) < 0)
		goto error;

	sprintf(tmp, "%s%d", AB_DEBUGFS_PATH_INTERRUPT,
			ab_value->interrupt + AB_DEBUGFS_BASE_INTERRUPT);

	fd_irqx = open(tmp, O_RDONLY);
	if (fd_irqx < 0)
		goto error;

	pid2fd[npid2fd].fd = fd_irqx;
	pid2fd[npid2fd].pid = pid;
	pid2fd[npid2fd].sig = ab_value->signal;
	pid2fd[npid2fd].it = ab_value->interrupt;
	npid2fd++;

	/* add new descriptor in fds array */
	fds[nfds].fd = fd_irqx;
	fds[nfds].events = POLLPRI | POLLERR;
	nfds++;

	return;

error:
	printf("error registering new interrupt errno:%d(%s)",
			errno, strerror(errno));

}

void abxxxx_unregistering(int interrupt)
{
	int current_size = nfds;
	int i, j, k;

	printf("unregistering interrupt:%d",
		interrupt);

	for (i = 0; i < npid2fd ; i++) {
		if (pid2fd[i].it == ab_value->interrupt) {

			for (j = 0; j < current_size; j++) {
				if (pid2fd[i].fd == fds[j].fd) {
					close(pid2fd[i].fd);
					pid2fd[i].fd = -1;
					fds[j].fd = -1;
					break;
				}
			}

			/* compress fd_array */
			for (k = 0; k < nfds; k++) {
				if (fds[k].fd == -1) {
					for (j = k; j < nfds; j++)
						fds[j].fd = fds[j+1].fd;
					nfds--;
				}
			}

			return;

		}
	}
}

void abxxxx_hand(int sig, siginfo_t *siginfo, void *context)
{
	int i;
	char buf[AB_EVENT_BUF_LEN];

	if ((sig == 0) && (context == NULL))
		return;

	if (ab_value->signal == 0) {

		abxxxx_unregistering(ab_value->interrupt);
		return;

	} else {
		/* test if fd exist */
		for (i = 0; i < npid2fd ; i++) {
			if (pid2fd[i].it == ab_value->interrupt) {
				sprintf(buf, "%s%d", AB_DEBUGFS_PATH_INTERRUPT,
						ab_value->interrupt + AB_DEBUGFS_BASE_INTERRUPT);
				pid2fd[i].fd = open(buf, O_RDONLY);

				/* add descriptor in fds array */
				fds[nfds].fd = pid2fd[i].fd;
				fds[nfds].events = POLLPRI | POLLERR;
				nfds++;

				return;
			}
		}
	}

	add_fd_to_poll(siginfo->si_pid, ab_value->signal);


}

void send_kill(int fd_to_kill)
{
	int i;

	for (i = 0; i < npid2fd; i++) {
		if (pid2fd[i].fd == fd_to_kill) {
			kill(pid2fd[i].pid, pid2fd[i].sig);
			printf("forward IT to :%d", pid2fd[i].pid);
			break;
		}
	}
}

int polling(void)
{
	int ret, i;
	char buf[AB_EVENT_BUF_LEN];
	int current_size;

	ret = poll(fds, nfds, -1);
	if (ret == -1)
		goto error;

	current_size = nfds;

	/* find readable descriptor */
	for (i = 0; i < current_size; i++) {
		if (fds[i].revents == (POLLPRI | POLLERR)) {
			read(fds[i].fd, &buf, AB_EVENT_BUF_LEN);
			send_kill(fds[i].fd);
			break;
		}
	}

	return 0;

error:
	printf("error in interrupt polling");
	return -1;
}

int main(void)
{
	struct sigaction action;
	int shmid;

	/*initialise fds*/
	memset(fds, 0 , sizeof(fds));

	/*catch SIGRTMAX signal*/
	action.sa_flags = SA_SIGINFO | SA_RESTART;
	action.sa_sigaction = &abxxxx_hand;
	sigemptyset(&action.sa_mask);
	sigaction(SIGRTMAX, &action, NULL);

	shmid = shmget(11, sizeof(ab_listener), IPC_CREAT | 0666);
	if (shmid == -1) {
		printf("error getting shmid errno:%d(%s)",
				errno, strerror(errno));
		return -1;
	}

	ab_value = shmat(shmid, 0, SHM_RDONLY);
	if (ab_value == (void *) -1) {
		printf("error getting shmat errno:%d(%s)",
				errno, strerror(errno));
		return -1;
	}

	/*catch SIGKILL signal*/
	action.sa_flags = 0;
	action.sa_handler = abxxxx_close_fds;
	sigemptyset(&action.sa_mask);
	sigaction(SIGKILL, &action, NULL);

	stop = 1;

	while (stop)
		polling();

	return 0;
}


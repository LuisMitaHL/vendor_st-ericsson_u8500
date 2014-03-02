#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

#include "libab.h"


pid_t abxxxx_listener_id(void)
{

	FILE *fp;
	pid_t pid;

	fp = popen("pidof ab_listener", "r");
	if (fp == NULL)
		goto error;

	fscanf(fp, "%d", &pid);
	pclose(fp);

	return pid;

error:
	printf("error getting pid of ab_listener");
	return -1;
}

int abxxxx_interrupt_registering(int interrupt_id, int signal_id)
{
	union sigval sval;
	int shmid;
	int ab_listener_id;
	ab_listener *ab_value;

	printf("registering interrupt:%d",
		interrupt_id);

	ab_listener_id = abxxxx_listener_id();

	if (ab_listener_id < 0)
		goto error;

	ab_value = abxxxx_shm_install(&shmid);

	ab_value->interrupt = interrupt_id;
	ab_value->signal = signal_id;

	sigqueue(ab_listener_id, SIGRTMAX, sval);

	return 0;

error:
	printf("can't find ab_listener PID");
	return -1;
}

int abxxxx_read(uint16_t addr, uint8_t *val)
{
	int fd_addr = 0, fd_bank = 0, fd_value = 0;
	int ret = 0;
	char tmp[5];

	fd_bank = open(AB_DEBUGFS_PATH_BANK, O_RDWR);
	if (fd_bank < 0) {
		printf("error opening %s",
				AB_DEBUGFS_PATH_BANK);
		ret = fd_bank;
		goto error_bank;
	}

	flock(fd_bank, LOCK_EX);
	snprintf(tmp, sizeof(tmp), "%d", AddrtoBank(addr));
	write(fd_bank, tmp, sizeof(unsigned long));

	fd_addr = open(AB_DEBUGFS_PATH_ADDRESS, O_RDWR);
	if (fd_addr < 0) {
		printf("error opening %s",
				AB_DEBUGFS_PATH_ADDRESS);
		ret = fd_addr;
		goto error_address;
	}

	flock(fd_addr, LOCK_EX);
	snprintf(tmp, sizeof(tmp), "0x%x", AddrtoAddr(addr));
	write(fd_addr, tmp, sizeof(unsigned long));

	fd_value = open(AB_DEBUGFS_PATH_VALUE, O_RDWR);
	if (fd_value < 0) {
		printf("error opening %s",
				AB_DEBUGFS_PATH_VALUE);
		ret = fd_value;
		goto error_value;
	}

	flock(fd_value, LOCK_EX);
	if(0 > read(fd_value, tmp, sizeof(tmp)))
		ret = -1;
	else
		*val = (uint8_t) strtoul(tmp, NULL, 16);

error_bank:
	flock(fd_bank, LOCK_UN);
	close(fd_bank);
error_value:
	flock(fd_value, LOCK_UN);
	close(fd_value);
error_address:
	flock(fd_addr, LOCK_UN);
	close(fd_addr);

	return ret;

}

int abxxxx_write(uint16_t addr, uint8_t val)
{
	int fd_addr = 0, fd_bank = 0, fd_value = 0;
	int ret = 0;
	char tmp[5];

	fd_bank = open(AB_DEBUGFS_PATH_BANK, O_RDWR);
	if (fd_bank < 0) {
		printf("error opening %s",
				AB_DEBUGFS_PATH_BANK);
		ret = fd_bank;
		goto error_bank;
	}

	flock(fd_bank, LOCK_EX);
	snprintf(tmp, sizeof(tmp), "%d", AddrtoBank(addr));
	write(fd_bank, tmp, sizeof(unsigned long));

	fd_addr = open(AB_DEBUGFS_PATH_ADDRESS, O_RDWR);
	if (fd_addr < 0) {
		printf("error opening %s",
				AB_DEBUGFS_PATH_ADDRESS);
		ret = fd_addr;
		goto error_address;
	}

	flock(fd_addr, LOCK_EX);
	snprintf(tmp, sizeof(tmp), "0x%x", AddrtoAddr(addr));
	write(fd_addr, tmp, sizeof(unsigned long));

	fd_value = open(AB_DEBUGFS_PATH_VALUE, O_RDWR);
	if (fd_value < 0) {
		printf("error opening %s",
				AB_DEBUGFS_PATH_VALUE);
		ret = fd_value;
		goto error_value;
	}
	flock(fd_value, LOCK_EX);
	snprintf(tmp, sizeof(tmp), "0x%hhx", val);
	if(0 > write(fd_value, tmp, sizeof(unsigned long)))
		ret = -1;

error_bank:
	flock(fd_bank, LOCK_UN);
	close(fd_bank);
error_value:
	flock(fd_value, LOCK_UN);
	close(fd_value);
error_address:
	flock(fd_addr, LOCK_UN);
	close(fd_addr);

	return ret;

}

ab_listener *abxxxx_shm_install(int *shmid)
{
	ab_listener *test_ab;

	*shmid = shmget(11, sizeof(ab_listener), IPC_CREAT | 0666);
	if (*shmid == -1) {
		printf("error getting shm errno:%d(%s)",
				errno, strerror(errno));
		return NULL;
	}

	test_ab = shmat(*shmid, 0, 0);

	return test_ab;
}


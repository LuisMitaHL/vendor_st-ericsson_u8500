/************************************************************************
 *                                                                      *
 *  Copyright 2010 ST-Ericsson                                          *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "config.h"
#include "convenience.h"
#include "fsa.h"
#include "log.h"
#include "msa.h"
#include "posix.h"
#include "process.h"
#include "wakelock.h"
#ifndef CONFIG_EXCLUDE_WAIT_FOR_MODEM
#include "shm_netlnk.h"
#endif

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

static int openTestDevice(const char *name)
{
	union {
		struct sockaddr sa;
		struct sockaddr_un sa_un;
	} addr;

	int sock, ret;
	socklen_t addrlen;

	assert(name);
	assert(1 + strlen(name) + 1 < sizeof(addr.sa_un.sun_path));

	//RPC is packet oriented.
	sock = posix_socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if ( unlikely(sock < 0) )
	{
		return sock;
	}

	memset(&addr, 0x00, sizeof(addr));
	addr.sa_un.sun_family = AF_UNIX;
	strcpy(addr.sa_un.sun_path + 1, name); //Use an anonymous socket

	ret = posix_bind(name, sock, &addr.sa, sizeof(addr.sa_un));
	if ( unlikely(ret < 0) )
	{
		return ret;
	}

	ret = posix_listen(name, sock, 0);
	if ( unlikely(ret < 0) )
	{
		return ret;
	}

	logINFO("Waiting for msa client to connect to unix socket '@%s'.",
		name);

	addrlen = sizeof(addr.sa_un);
	return posix_accept(name, sock, NULL, &addrlen);
}

static int openDevice(const char *deviceFilename)
{
	assert(deviceFilename);

	return posix_open(deviceFilename, O_RDWR, 0);
}

/* Exclude check for modem started when building LBP */
#ifndef CONFIG_EXCLUDE_WAIT_FOR_MODEM
static void wait_for_modem_online(int fd_netlnk)
{
	fd_set readfds;
	int ret = -1;
	int msg = -1;
	struct timeval tv;

	/* Check if modem is already up */
	logINFO("Wait for modem status online");
	netlnk_socket_send(fd_netlnk, MODEM_QUERY_STATUS_REQ);

	FD_ZERO(&readfds);
	FD_SET(fd_netlnk, &readfds);
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	ret = select(fd_netlnk + 1, &readfds, NULL, NULL, &tv);

	if (ret > 0) {
		if (FD_ISSET(fd_netlnk, &readfds)) {
			netlnk_socket_recv(fd_netlnk, &msg);
		}
	}

	/* Modem not up, wait for ONLINE */
	if ((msg != MODEM_STATUS_ONLINE)) {
		logINFO("Modem not up yet, wait for event");
		do {
			FD_ZERO(&readfds);
			FD_SET(fd_netlnk, &readfds);
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			ret = select(fd_netlnk + 1, &readfds, NULL, NULL, &tv);
			if (ret > 0) {
				if (FD_ISSET(fd_netlnk, &readfds)) {
					netlnk_socket_recv(fd_netlnk, &msg);
				}
			}
		} while (msg != MODEM_STATUS_ONLINE);
	}
	logINFO("Received MODEM_STATUS_ONLINE");
}
#endif

int main(int argc, char **argv)
{
	Config_t config;
	int dev;
	fsa_packet_t req;
	fsa_packet_t resp;
#ifndef CONFIG_EXCLUDE_WAIT_FOR_MODEM
	shm_netlnk_status status = SHM_NETLNK_ERROR;
	int fd_netlnk = -1;
#endif

	assert(argv);

	//First, make sure any errors during parsing of cmdline can be viewed
	log_setLogToConsole(LOGTOCONSOLE_STDERR);

	//We create our own files with exact permissions stated
	(void) umask(0);

	if ( unlikely(config_parseCmdline(argc, argv, &config) == false) )
	{
		goto End;
	}

	logINFO("MSA server started on '%s'. " MSA_VERSION_REPORT, config.rootDir);

#ifndef CONFIG_EXCLUDE_WAIT_FOR_MODEM
	logINFO("Create netlnk socket");
	status = netlnk_socket_create(&fd_netlnk);
	if (SHM_NETLNK_ERROR == status) {
		logERROR("MSA can not be initialized "
			"because of netlink socket create failed");
		goto End;
	}

	wait_for_modem_online(fd_netlnk);
#endif

	if ( unlikely(config.testOverUnixSocket) )
		dev = openTestDevice(RPC_DEVICE_FILE);
	else
		dev = openDevice(RPC_DEVICE_FILE);
	if ( unlikely(dev < 0) )
	{
		goto Error;
	}

	//Change user and group
	if ( unlikely(config_applyUserAndGroup(&config) == false) )
		goto Close;

	//Put MSA server in background, we do this at the latest point so any error
	// up until here will be written/logged to console
	if ( unlikely(config_applyBackground(&config) == false) )
		goto Close;

	//Switch into configured logging settings. We do this late, se we catch
	// and print any errors to stderr if we don't reach this point.
	config_applyLog(&config);

	//Set the MSA root
	if ( unlikely(config_applyRootDir(&config) == false) )
		goto Close;
	process_setRoot(config.rootDir);

	//Clean up from any previous instances of MSA running, not ended cleanly
	process_cleanup();

	//Alloc one receive and one send buffer. Once and for all.
	fsa_allocpacket(&req);
	fsa_allocpacket(&resp);

	//Check if system supports wake locks, and if so cleanup any lingering
	// wake lock placed by a previous instance of MSA, that did not exit cleanly.
	check_wakelock();

	//Main server loop
	logINFO("Entering main MSA serving loop.");
	while (true)
	{
		if ( unlikely(!fsa_receivepacket(dev, &req)) )
		{
			logFATAL("Can not receive FSA request.");
			break;
		}

		process_request(&req, &resp);

		if ( unlikely(!fsa_sendpacket(dev, &resp)) )
		{
			logFATAL("Can not send FSA response.");
			break;
		}
	}

	fsa_freepacket(&req);
	fsa_freepacket(&resp);

	//Release wake lock in case we forgot to release it earlier
	wakelock_release();

Close:
	logDBG1("Closing MSA RPC device.");
	(void) posix_close(RPC_DEVICE_FILE, dev);

Error:
	//Server will never stop. We will only end up here if something is wrong
	logDBG1("Cleaning up before ending MSA server.");

	/* PROBLEM: We have no free operation in the state. */
	//state_free();

	logFATAL("MSA server terminated.");

End:

	config_destroy(&config);
	log_closeAllLog();

	return EXIT_FAILURE;
}

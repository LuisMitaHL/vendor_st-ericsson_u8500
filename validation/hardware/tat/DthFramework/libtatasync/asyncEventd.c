/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   This daemon forward DTH event to PC UI
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <pwd.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>
#include <arpa/inet.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#include <signal.h>

#include <openssl/md5.h>

#include "tatasync.h"

#define IFACE_DEFAULT "usb0"
#define MAXLEN 100
#define MD5_SIZE 256

static int file_id;
static int sock_S;
static struct sockaddr_in adr_S;
static int debug;
static int stop = 1;
static unsigned long int addr_broadcast;
static unsigned long int addr_interface;
static char addr_interface_s[12];

static char *msg_type[4] = {
	"PERM_CHANGED",
	"VALUE_CHANGED",
	"NOTIFICATION_ALERT",
	"TREE_CHANGED"
};

static void usage(void)
{
	printf("asyncEvend:\n\t-p broadcast port\n\t-i interface\n\t"
	       "-d debug\n\t-c IPC key\n");
	exit(-1);
}

static void sigterm_handler(int sig, siginfo_t *siginfo, void *context)
{
	/* used to ignore compilation warning : "unused parameter" */
	if ((siginfo == NULL) && (context == NULL))
		if (debug)
			printf("captured : %d\nstop async event daemon\n", sig);
	stop = 0;
}

static int getIPofInterfaces(char *iface)
{
	int fd;
	struct ifreq ifr;
	int err = 0;
	struct sockaddr_in addr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		printf("Pb socket %s (%s)", __FUNCTION__, strerror(errno));
		err = errno;
		goto end;
	}

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to iface name */
	strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);

	if (debug)
		printf("interface %s\n", iface);

	/* get interface address */
	err = ioctl(fd, SIOCGIFADDR, &ifr);
	if (err < 0) {
		printf("error while getting interface (%s)\n", strerror(errno));
		goto error;
	}

	memcpy(&addr, &ifr.ifr_addr, sizeof(struct sockaddr_in));

	addr_interface = (addr.sin_addr).s_addr;
	strcpy(addr_interface_s, inet_ntoa(addr.sin_addr));

	if (debug)
		printf("\tip : %s\n", addr_interface_s);

	/* get broadcast address */
	err = ioctl(fd, SIOCGIFBRDADDR, &ifr);
	if (err < 0) {
		printf("error while getting interface (%s)\n", strerror(errno));
		goto error;
	}

	addr_broadcast =
		(((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr).s_addr;

error:
	close(fd);
end:
	return err;
}

static int udp_send(char *message)
{

	int nb_cars;

	/* send answer */
	nb_cars = sendto(sock_S,
			 (void *)message, strlen(message), 0,
			 (struct sockaddr *)&adr_S, sizeof(adr_S));

	return nb_cars;
}

static int msg_treatment(void)
{
	int err = 0, msg_t = 0, i = 0;
	char path[MAXLEN], *msg;
	msg_st requete;

	MD5_CTX context;
	unsigned char md[MD5_DIGEST_LENGTH];
	char mdascii[2*MD5_DIGEST_LENGTH+1];
	char alert_msg[MAXLEN] = { "" };


	/* receive message from mailbox */
	err = msgrcv(file_id, &requete, sizeof(msg_st),
			     TYPE_DTH_ASYNCH, 0);
	if (err < 0) {
		printf("message can't be received (%s)\n",
		       strerror(err));
		return -1;
	}

	msg = malloc(strlen(addr_interface_s) + MD5_SIZE +
				strlen(requete.message) * sizeof(char));

	/* extract path and message type */
	sscanf(requete.message, "%s %d %254c", path, &msg_t, alert_msg);
	printf("msg received => %s",requete.message);
	if (debug)
		printf("extract path=> %s\nextract message type=>%d\nalert notification=>%s\n", path, msg_t,alert_msg);

	/* compute MD5 */
	MD5_Init(&context);

	MD5_Update(&context, path, strlen(path));

	MD5_Final(md, &context);

	/* Convert raw valure to ascii */
	memset(mdascii, 0, 2*MD5_DIGEST_LENGTH);
	for(i=0; i < MD5_DIGEST_LENGTH; i++)
	{
		sprintf(&(mdascii[i*2]), "%02x", md[i] );
	}

	/* create message */
	sprintf(msg, "%s %s %s %s", addr_interface_s, mdascii, msg_type[msg_t],alert_msg);
	if (debug)
		printf("msg %s\n", msg);

	/*send message */
	if (debug)
		printf("send msg %s\n", msg);
	err = udp_send(msg);
	if (err < 0)
		printf("broadcast error, message can't be send (%s)\n",
		       strerror(err));

	free(msg);
	return 0;
}

int main(int argc, char *argv[])
{

	int port = 1030;
	int c = 0, err = 0;
	char *s;
	char iface[MAXLEN] = { IFACE_DEFAULT };
	int broadcast = 1;
	struct sigaction act;

	while ((c = getopt(argc, argv, "dp:i:")) != -1) {
		switch (c) {
		case 'p':
			port = strtol(optarg, &s, 10);
			if (*s != '\0')
				usage();
			break;
		case 'i':
			strcpy(iface, optarg);
			break;
		case 'd':
			debug = 1;
			break;
		default:
			usage();
		}
	}

	/* intercept SIGTERM to shutdown */
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = sigterm_handler;
	act.sa_flags = SA_RESTART;

	if (sigaction(SIGTERM, &act, NULL) < 0) {
		perror("sigaction");
		return 1;
	}

	/* BAL init */
	file_id = msgget(ASYNCH_IPC_KEY, IPC_CREAT | IPC_EXCL | 0666);
	if (file_id < 0) {
		printf("error can't create msg file(%s)\n", strerror(errno));
		goto end;
	}

	if (debug)
		printf("file id %d\n", file_id);

	/* UDP init */
	/* entre point creation */
	sock_S = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_S == -1) {
		printf("Pb socket (%s)", strerror(errno));
		goto error_sock;
	}

	/* this call is what allows broadcast packets to be sent: */
	if (setsockopt(sock_S, SOL_SOCKET, SO_BROADCAST, &broadcast,
		       sizeof broadcast) == -1) {
		printf("setsockopt (SO_BROADCAST)(%s)",strerror(errno));
		goto error;
	}

	if (debug)
		printf("listening on port %d\n", port);

	/* Init address */

	err = getIPofInterfaces(iface);
	if (err < 0)
		goto error;

	adr_S.sin_port = htons(port);
	adr_S.sin_family = AF_INET;
	adr_S.sin_addr.s_addr = addr_broadcast;
	bzero(adr_S.sin_zero, sizeof(adr_S.sin_zero));

	if (debug)
		printf("use address: %s\n", inet_ntoa(adr_S.sin_addr));

	/* treatment */
	while (stop)
		msg_treatment();

error:
	/* remove message box */
	err = msgctl(file_id, IPC_RMID, NULL);
	if (err < 0)
		printf("message box error (%s)\n", strerror(errno));
error_sock:
	if (debug)
		printf("close socket\n");
	close(sock_S);
end:
	exit(errno);
}

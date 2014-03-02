#ifndef _CONFIG_H
#define _CONFIG_H

/************************************************************************
 *                                                                      *
 *  Copyright 2010 ST-Ericsson                                          *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include "log.h"

#include <pwd.h>
#include <grp.h>

typedef struct Config_st
{
	char *rootDir;

	char  *user;
	uid_t  uid;

	char  *group;
	gid_t  gid;

	int             logDebugLvl;
	LogToConsole_t  logToConsole;
	char           *logToFile;
	bool            logToSyslog;

	bool	doChroot; //Should we chroot()?
	bool	background; //Put the server in background/daemonize it
	bool	testOverUnixSocket; //Use unix sockets for testing
} Config_t;

bool config_parseCmdline(int argc, char *argv[], Config_t *config);

void config_applyLog(Config_t *config);
bool config_applyRootDir(Config_t *config);
bool config_applyUserAndGroup(Config_t *config);
bool config_applyBackground(Config_t *config);

void config_destroy(Config_t *config);


#endif // _CONFIG_H


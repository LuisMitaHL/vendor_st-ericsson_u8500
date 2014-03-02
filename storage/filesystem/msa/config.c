/************************************************************************
 *                                                                      *
 *  Copyright 2010 ST-Ericsson                                          *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#define _BSD_SOURCE //Needed for setgroups() and daemon()

#include "config.h"

#include "convenience.h"
#include "log.h"
#include "msa.h"
#include "posix.h"

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#define config_error(fmt, args...) \
	do { \
		logERROR(fmt, ##args); \
		config_usage(); \
	} while (0)

static void config_setDefault(Config_t *config)
{
	config->rootDir = easy_strdup(MSA_ROOT_DEFAULT);

	config->user = NULL;
	config->uid = 0;
	config->group = NULL;
	config->gid = 0;

	config->logDebugLvl = 0;
	config->logToConsole = LOGTOCONSOLE_DISABLED;
	config->logToFile = NULL;
	config->logToSyslog = false;

	config->doChroot = false;
	config->background = false;
	config->testOverUnixSocket = false;
}

static void config_usage(void)
{
	logINFO(
"\n"
"Usage:\n"
"msa [-h] [-r rootdir] [-c] [-u user] [-g group] [-l logfile] [-ddddd] [-b] [-t]\n"
" -h          Prints this help.\n"
" -r rootdir  Will serve files from 'rootdir'. Default is '%s'.\n"
" -c          Will chroot into 'rootdir'. Makes for better security.\n"
" -u user     Drop privileges to this user.\n"
" -g group    Drop privileges to this group.\n"
" -l logfile  Have the MSA server log to 'logfile'.\n"
"              If 'logfile' is 'syslog' it will send the log to syslog daemon.\n"
"              If 'logfile' is 'stderr' it will log to stderr.\n"
"              If 'logfile' is 'stdout' or obmitted it will log to stdout.\n"
" -d          Enable debugging. There are several levels:\n"
"              -d     Log request, replies and DBG1.\n"
"              -dd    Log also successful system calls and DBG2.\n"
"              -ddd   Log DBG3 which is even more details.\n"
"              -dddd  Will hexdump all packets, but only first 128 bytes.\n"
"              -ddddd Will hexdump all bytes of the packets.\n"
" -b          Fork the MSA server to run in background.\n"
" -t          Don't use '%s'. Use a UNIX socket to be able\n"
"              to serve 'msa_fuse' test client for testing purpose."
"\n"
"All requests to MSA will be based in the 'rootdir' directory. Files that\n"
"exists outside (for example by following a symlink) will not be operated on.\n"
"\n"
"%s\n",
	MSA_ROOT_DEFAULT, RPC_DEVICE_FILE, MSA_VERSION_REPORT);
}

static void config_parseLog(const char *str, Config_t *config)
{
	if ( str == NULL || str[0] == '\0' || strcmp(str, "stdout") == 0 )
	{
		config->logToConsole = LOGTOCONSOLE_STDOUT;
		log_setLogToConsole(LOGTOCONSOLE_STDOUT); //apply directly
	}
	else if ( strcmp(str, "stderr") == 0 )
	{
		config->logToConsole = LOGTOCONSOLE_STDERR;
		log_setLogToConsole(LOGTOCONSOLE_STDERR);
	}
	else if ( strcmp(str, "syslog") == 0 )
	{
		config->logToSyslog = true;
		log_setLogToSyslog(true); //apply directly
	}
	//If we have only given -l without argument it will log to stdout
	else
	{
		easy_free(config->logToFile);
		config->logToFile = easy_strdup(str);
		log_setLogToFile(config->logToFile); //apply directly
	}
}

static bool config_parseUser(const char *str, Config_t *config)
{
	struct passwd *pwent;

	easy_free(config->group);
	config->group = NULL;
	config->user = easy_strdup(str);

	pwent = getpwnam(str);
	if ( unlikely(pwent == NULL) )
	{
		config_error("User '%s' does not exist.", str);
		return false;
	}
	config->uid = pwent->pw_uid;
	return true;
}

static bool config_parseGroup(const char *str, Config_t *config)
{
	struct group *grent;

	easy_free(config->group);
	config->group = easy_strdup(str);

	grent = getgrnam(str);
	if ( unlikely(grent == NULL) )
	{
		config_error("Group '%s' does not exist.", str);
		return false;
	}
	config->gid = grent->gr_gid;

	return true;
}

bool config_parseCmdline(int argc, char *argv[], Config_t *config)
{
	int c;

	config_setDefault(config);

	opterr = 0;
	while ((c = getopt (argc, argv, ":hdbtcr:u:g:l:")) != -1)
	{
		switch (c)
		{
			case 'd':
				config->logDebugLvl++;
				//Apply new log level directly, for debugging
				log_setDebugLevel(config->logDebugLvl);
				break;

			case 'c':
				config->doChroot = true;
				break;

			case 'r':
				easy_free(config->rootDir);
				config->rootDir = easy_strdup(optarg);
				break;

			case 'l':
				config_parseLog(optarg, config);
				break;

			case 't':
				config->testOverUnixSocket = true;
				break;

			case 'b':
				config->background = true;
				break;

			case 'u':
				if ( config_parseUser(optarg, config) == false )
					return false;
				break;

			case 'g':
				if ( config_parseGroup(optarg, config) == false )
					return false;
				break;

			case '?':
				config_error("Unknown option '%c'.", optopt);
				return false;

			case 'h':
				config_usage();
				return false;

			default:
				assert(false && "Hit default case in parseCmdline.");
		}
	}

	logDBG1("Options: rootDir=%s user='%s' uid=%u group='%s' gid=%u "
		"logDebugLvl=%d logToConsole=%u logToFile='%s' logToSyslog=%d "
		"doChroot=%u background=%u testOverUnixSocket=%u",
		config->rootDir, config->user, config->uid, config->group, config->gid,
		config->logDebugLvl, config->logToConsole, config->logToFile,
		config->logToSyslog,
		config->doChroot, config->background, config->testOverUnixSocket);

	return true;
}

static inline bool isRootUser(void)
{
	if ( getuid() != 0 )
	{
		config_error("Need to be root.");
		return false;
	}
	return true;
}

void config_applyLog(Config_t *config)
{
	log_setDebugLevel(config->logDebugLvl);
	log_setLogToSyslog(config->logToSyslog);
	log_setLogToConsole(config->logToConsole);
	log_setLogToFile(config->logToFile);
}

bool config_applyRootDir(Config_t *config)
{
	char *resolvedRootDir;

	//Alloc memory
	resolvedRootDir = easy_malloc(PATH_MAX);

	//Resolve the rootDir and check that it exists
	if ( unlikely(posix_realpath(config->rootDir, resolvedRootDir) == NULL) )
	{
		config_error("Unable to resolve root dir path '%s'.", config->rootDir);
		easy_free(resolvedRootDir);
		return false;
	}

	//Use new resolved root dir
	easy_free(config->rootDir);
	config->rootDir = resolvedRootDir;
	logDBG1("Resolved root directory config->rootDir='%s'",
		config->rootDir);

	//chdir to the target dir. Will check that the directory exists (again),
	// that we have permision to use it, and will also prepare for any chroot()
	if ( unlikely(posix_chdir(config->rootDir) != 0) )
	{
		config_error("Unable to change into root directory '%s'.",
			config->rootDir);
		return false;
	}

	if ( config->doChroot )
	{
		logDBG1("Applying chroot('%s').", config->rootDir);

		if ( !isRootUser() )
			return false;

		if ( chroot(config->rootDir) != 0 )
		{
			config_error("Unable to chroot into root directory '%s'.",
				config->rootDir);
			return false;
		}

		//Now the msaRoot should be just '/'
		easy_free(config->rootDir);
		config->rootDir = easy_strdup("/");
	}

	return true;
}


bool config_applyUserAndGroup(Config_t *config)
{
	if ( config->uid )
	{
		if ( !isRootUser() )
			return false;

		if ( unlikely(setuid(config->uid) != 0) )
		{
			config_error("Unable to change into user '%s' (uid %u).",
				config->user, config->uid);
			return false;
		}
	}

	//Drop root privileges (if configured to user other than root
	if ( config->gid )
	{
		if ( !isRootUser() )
			return false;

		if ( unlikely(setgid(config->gid) != 0) )
		{
			config_error("Unable to change into group '%s' (gid %u).",
				config->group, config->gid);
			return false;
		}

		if ( unlikely(setgroups(1, &config->gid) != 0) )
		{
			config_error("Unable to limit group to only group '%s' (gid %u).",
				config->group, config->gid);
			return false;
		}
	}

	return true;
}

bool config_applyBackground(Config_t *config)
{
	if ( config->background )
	{
		//We wan't to chdir() into '/' but not close stdin/out/err
		if ( unlikely(daemon(0, 1) != 0) )
		{
			config_error("Unable to put server in background.");
			return false;
		}
	}

	return true;
}

void config_destroy(Config_t *config)
{
	easy_free(config->rootDir);
	easy_free(config->user);
	easy_free(config->group);
	easy_free(config->logToFile);
}


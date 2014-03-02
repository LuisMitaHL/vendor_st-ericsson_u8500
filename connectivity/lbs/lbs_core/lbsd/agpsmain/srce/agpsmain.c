/*
* Copyright (C) ST-Ericsson 2009
*
* agpsmain.c
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#include <unistd.h>

#ifdef WIN32
#include <crtdbg.h>
#endif
#ifndef ARM_LINUX_AGPS_FTR
#include <android_log.h>
#endif
#ifndef AGPS_GENERIC_SOLUTION_FTR
#include "agpsfsm.h"
#include "agps.h"
#include "agpsosa.h"
#else
#include "gns.h"
#ifdef AGPS_FTR
#ifdef AMIL_LCS_FTR
#include "amilExtern.h"
#endif
#endif
#endif /* AGPS_GENERIC_SOLUTION_FTR */
#include "agpsmain.h"

int  InitSocketServer( void );
t_OsaThreadHandle g_SocketServerThread;
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "AgpsMain"

#ifdef AGPS_QT_FTR
#define UMASK 0117
#else

#define UMASK 0717
#endif

int main()
{
/* Our process ID and Session ID */
    pid_t pid, sid;

    /* Set restrictive umask before anything else */
    DEBUG_LOG_PRINT(("Setting restrictive umask\n"));
    umask(UMASK);

    /* Who is our parent ? */
    pid = getppid();
    DEBUG_LOG_PRINT(("Parent pid is %d\n",pid));
    if (pid == 1) {

        /* Do not fork if init (pid=1) is the parent */
        DEBUG_LOG_PRINT_LEV2(("agpsd.c : Parent is init, no fork\n"));

    } else {

        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
            DEBUG_LOG_PRINT_LEV2(("agpsd.c : forking failed\n"));
            exit(EXIT_FAILURE);
        }

        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
            DEBUG_LOG_PRINT_LEV2(("agpsd.c : Child created, Parent exits : succes\n"));
            exit(EXIT_SUCCESS);
        }

        DEBUG_LOG_PRINT_LEV2(("agpsd.c : Child execution starts here\n"));

        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
            /* Log the failure */
            DEBUG_LOG_PRINT_LEV2(("agpsd.c : Creating of new SID failed\n"));
            exit(EXIT_FAILURE);
        }
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
            /* Log the failure */
            DEBUG_LOG_PRINT_LEV2(("agpsd.c : Change directory failed\n"));
            exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
#ifndef AGPS_QT_FTR
    close(STDIN_FILENO);
#ifndef ARM_LINUX_AGPS_FTR
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
#endif
#endif /* AGPS_QT_FTR */

    /* Daemon-specific initialization goes here */

    if( !OSA_ThreadCreate( g_SocketServerThread , InitSocketServer , NULL )  )
    {
        DEBUG_LOG_PRINT_LEV2(( "SocketServer pthread_create Failed, exiting\n" ));
        exit( EXIT_FAILURE );
    }
#ifdef AMIL_FTR
    AMIL_Init();
#endif

#ifdef AGPS_GENERIC_SOLUTION_FTR
    InitAgps();
#endif

#ifdef AGPS_FTR
#ifdef AMIL_LCS_FTR
   Amil1_01Init();
#endif
#endif

 /*  OSA_Sleep( 1000 );

    if( OSA_ThreadCreate( v_TestThread , TestCGPS , NULL )  )
    {
        DEBUG_LOG_PRINT_LEV2(( "Test pthread_create %d\n", errno ));
        exit( EXIT_FAILURE );
    }*/

    /* Change to restrictive user. The g_SocketServerThread has inherited
     * the initial user. That thread needs to be updated as well. */
    DEBUG_LOG_PRINT(("Changing to restrictive uid in main thread\n"));
    setuid(1001);

    if (OSA_WaitForThreadExit( g_SocketServerThread ))
    {
        DEBUG_LOG_PRINT_LEV2(("agpsmain : OSA_WaitForThreadExit called \n"));
    }

/*    OSA_WaitForThreadExit( v_TestThread );*/

#ifdef AGPS_GENERIC_SOLUTION_FTR
    //Call GNS_Deinit
#endif

#ifdef AMIL_FTR
    AMIL_Exit();
#endif

#ifdef WIN32
    _CrtDumpMemoryLeaks();
#endif

    exit(EXIT_SUCCESS);
}

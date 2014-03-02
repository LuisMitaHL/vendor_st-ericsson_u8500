/*
* Copyright (C) ST-Ericsson 2009
*
* agpsosa.c
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#include "lbsosatrace.h"
#include "agpsosa.h"

#ifdef HEAP_MEMORY_DEBUG
static const char a_ConstSignature[SIGNATURE_SIZE] = "SIGNATURE";
static void*    a_MallocAddrDb[ARRAY_SIZE];
static int      a_MallocSizeDb[ARRAY_SIZE];
static long int v_MallocCount = 0;
#endif

#ifdef WIN32
#ifndef AGPS_DONOT_USE_OS_QUEUE
#error Platform OS Queue Usage not implemented
#endif

#elif defined( AGPS_LINUX_FTR )

#include <sys/types.h>

/* Timer implementation for linux */

#define MAX_LINUX_TIMERS    5
/*  The timer implementation here is based on sigalarm being sent by kernel.
    The resolution of this timer is set to 50ms. The actual resolution will depend on the platform */

typedef struct
{
    t_OsaTimerCallback      v_Callback;
    uint32_t                v_ReqMsTick;
    uint64_t                v_ExpiryMsTick;
    void*                   p_Param;
    t_OsaTimerHandle        v_Handle;
    BOOL                    v_TimerUsed;
}t_OsaTimerCtxt;


static t_OsaTimerCtxt g_LinuxOsaTimers[MAX_LINUX_TIMERS];
static uint16_t g_LinuxOsaTimerCount;
static struct timeval g_InitTimeStamp = {0,0};
static t_OsaCriticSection g_TimerCriticSection;

#ifdef AGPS_LINUX_BARE_FTR
typedef void (*sighandler_t)(int);
#endif

/*+LMSqc42256*/
void AgpsLinuxTimerResetEntry( uint8_t v_Index )
{

    g_LinuxOsaTimers[v_Index].v_TimerUsed    = FALSE;
    g_LinuxOsaTimers[v_Index].v_ExpiryMsTick = 0;
    g_LinuxOsaTimers[v_Index].v_Callback     = NULL;
    g_LinuxOsaTimers[v_Index].v_Handle       = 0;
    g_LinuxOsaTimers[v_Index].p_Param        = NULL;

}
/*-LMSqc42256*/

void AgpsLinuxTimerHandler( uint64_t v_CurrentMs )
{
    uint64_t nextExpiryTick = (uint64_t)-1;
    struct itimerval my_timer = { {0,0} , {0,0} };
    int i;

    for( i=0 ; ( i < MAX_LINUX_TIMERS ) ; i++ )
    {
        if( (g_LinuxOsaTimers[i].v_TimerUsed ) && ( g_LinuxOsaTimers[i].v_ExpiryMsTick < nextExpiryTick ) )
        {
            nextExpiryTick = g_LinuxOsaTimers[i].v_ExpiryMsTick;
        }
    }

    if( nextExpiryTick != (uint64_t)-1 )
    {
        int retval;

        DEBUG_LOG_PRINT_LEV2(("Current Ms : %llu s , Next Expiry Ms %llu s\n", v_CurrentMs , nextExpiryTick));

        if( nextExpiryTick <= v_CurrentMs )
        {
            DEBUG_LOG_PRINT_LEV2(("Timer expiry delayed. Immediately process!"));
            my_timer.it_value.tv_sec =  0;
            my_timer.it_value.tv_usec = 1; /* COnfigure immediate expiry to handling long awaiting timer */
        }
        else
        {
        my_timer.it_value.tv_sec =  /*g_InitTimeStamp.tv_sec + */ ( (nextExpiryTick - v_CurrentMs) / 1000);
        my_timer.it_value.tv_usec = /*g_InitTimeStamp.tv_usec + */( (nextExpiryTick - v_CurrentMs) % 1000 ) * 1000;
        }


/*        if( my_timer.it_value.tv_usec > 1000000 )
        {
            my_timer.it_value.tv_usec -= 1000000;
            my_timer.it_value.tv_sec++;
        } */
        retval = setitimer(ITIMER_REAL, &my_timer , NULL );
        DEBUG_LOG_PRINT_LEV2(("Timer set to %lu s and %lu us\n", my_timer.it_value.tv_sec , my_timer.it_value.tv_usec));
        DEBUG_LOG_PRINT_LEV2(("setitimer returned %d at time %llu\n", retval , v_CurrentMs));
    }
}

void AgpsLinuxTimerSignalHandler( int param )
{
    uint8_t  i;
    uint64_t currentMsTick;

    /* Dummy assignment to remove unused variable compiler warning */
    param = param;

    OSA_EnterCriticalSection( &g_TimerCriticSection );

    currentMsTick = GetMsCount();
    DEBUG_LOG_PRINT_LEV2(("currentMsTick = %llu\n" , currentMsTick ));

    for( i=0 ; i < MAX_LINUX_TIMERS ; i++ )
    {
        /* There is a bug in the linux timer code which can result in early timeout.
                  If timeout is within expiry - 2ms , we return immediately
            */


        /* It is possible that the callback takes a long time to execute
                  OR
                  we get transitioned out of execution and by the time we get another slice, other timers have expired.
                  In such a case, after every callback execution, check all the timers all over again! */
        if( g_LinuxOsaTimers[i].v_TimerUsed && ( ( g_LinuxOsaTimers[i].v_ExpiryMsTick - 2 )  <= currentMsTick ) )
        {
            DEBUG_LOG_PRINT_LEV2(("Expiry MS : %llu, Current MS : %llu\n" , g_LinuxOsaTimers[i].v_ExpiryMsTick , currentMsTick ));
            g_LinuxOsaTimers[i].v_TimerUsed = FALSE;
            g_LinuxOsaTimers[i].v_ExpiryMsTick = 0;
            if( NULL != g_LinuxOsaTimers[i].v_Callback )
            {
                g_LinuxOsaTimers[i].v_Callback( g_LinuxOsaTimers[i].p_Param );
                /* initiate another check with latest time */
                i=-1; // reset check count. -1 will get incremented in the loop and gets set to 0
                currentMsTick = GetMsCount(); // Load the latest time after execution of the callback
                DEBUG_LOG_PRINT_LEV2(("2. currentMsTick = %llu\n" , currentMsTick ));

            }
            else
            {
                DEBUG_LOG_PRINT(("Error : Null Timer Callback!\n"));
            }
        }
    }

    AgpsLinuxTimerHandler(currentMsTick);

    OSA_ExitCriticalSection( &g_TimerCriticSection );
}


void AgpsLinuxInit()
{
    if( g_InitTimeStamp.tv_sec == 0 )
    {
        gettimeofday( &g_InitTimeStamp , NULL );
        DEBUG_LOG_PRINT(("g_InitTimeStamp : Current Time in Seconds : %lu, sizeof of timestamp : %u bytes\n" , g_InitTimeStamp.tv_sec , sizeof( g_InitTimeStamp.tv_sec ) ));
    }

    signal( SIGALRM , AgpsLinuxTimerSignalHandler );

    /*Ingore SIGPIPE and avoid crash */
    if (signal((int)SIGPIPE, SIG_IGN) == SIG_ERR) {
        DEBUG_LOG_PRINT(("LBS : SIG_ERR when ignoring SIGPIPE"));
        abort();
    }
}


void AgpsLinuxDeinit()
{
    signal( SIGALRM , SIG_IGN );
}


int AgpsLinuxStartTimer( t_OsaTimerHandle * p_Handle , t_OsaTimerCallback v_Callback , void* p_Param , uint32_t v_DueTimeMs )
{
    uint8_t  i;
    int retVal = FALSE;
    uint64_t currentMsTick = 0;

    OSA_EnterCriticalSection( &g_TimerCriticSection );

    for( i=0 ; ( i < MAX_LINUX_TIMERS ) && ( g_LinuxOsaTimers[i].v_TimerUsed ) ; i++ );

    DEBUG_LOG_PRINT_LEV2(("AgpsLinuxStartTimer Index=%d , TimerCnt=%d,DueTimeMs=%u\n",i,g_LinuxOsaTimerCount ,v_DueTimeMs));


    if( i != MAX_LINUX_TIMERS )
    {
        /* We have found an available slot */
        g_LinuxOsaTimerCount++;
  //      DEBUG_LOG_PRINT_LEV2(("Found timer with handle %d \n" , g_LinuxOsaTimerCount));
        g_LinuxOsaTimers[i].v_Callback = v_Callback;
        g_LinuxOsaTimers[i].p_Param = p_Param;
        currentMsTick = GetMsCount();
        g_LinuxOsaTimers[i].v_ExpiryMsTick = currentMsTick + v_DueTimeMs;

        DEBUG_LOG_PRINT_LEV2(("Start : Expiry MS : %llu, Current MS : %llu\n" , g_LinuxOsaTimers[i].v_ExpiryMsTick , GetMsCount()));
        g_LinuxOsaTimers[i].v_Handle = g_LinuxOsaTimerCount;
        *p_Handle = g_LinuxOsaTimers[i].v_Handle;
        g_LinuxOsaTimers[i].v_TimerUsed = TRUE;
        DEBUG_LOG_PRINT_LEV2(("AgpsLinuxStartTimer: Linux Timer -Index %d , TimerCnt=%d Handle=%d, HandleBAddr=%p\n",i,g_LinuxOsaTimerCount,*p_Handle, p_Handle));
        retVal = TRUE;
    }

    AgpsLinuxTimerHandler(currentMsTick);

    OSA_ExitCriticalSection( &g_TimerCriticSection );

    return retVal;
}


int AgpsLinuxStopTimer( t_OsaTimerHandle v_Handle )
{
    uint8_t  i;
    int retVal = FALSE;

    OSA_EnterCriticalSection( &g_TimerCriticSection );

    DEBUG_LOG_PRINT_LEV2(("Stopping timer with handle %d\n" , v_Handle ));

    for( i=0 ; ( i < MAX_LINUX_TIMERS ) && ( g_LinuxOsaTimers[i].v_Handle != v_Handle ) ; i++ );

    if( i != MAX_LINUX_TIMERS )
    {
        /* We have found the timer slot */
        DEBUG_LOG_PRINT_LEV2(("Stopped timer with handle %d\n" , v_Handle ));
 /*+LMSqc42256*/
        AgpsLinuxTimerResetEntry(i);
 /*-LMSqc42256*/

        retVal = TRUE;
    }

    AgpsLinuxTimerHandler(GetMsCount());

    OSA_ExitCriticalSection( &g_TimerCriticSection );

    return retVal;
}


uint64_t GetMsCount()
{
    struct timespec  curr_clk;
    uint64_t    millisecondCount = 0;
    
    if( clock_gettime(CLOCK_MONOTONIC , &curr_clk) )
    {
        DEBUG_LOG_PRINT(("clock_gettime ( CLOCK_MONOTONIC ) : failed!"));
    }
    else
    {
        /* Increment by the time between subsequent requests to clock_gettime */
        millisecondCount = curr_clk.tv_sec*1000 + curr_clk.tv_nsec/1000000;
    }

    return millisecondCount;
}


#if 1
uint32_t AgpsLinuxGetRtcSecondCount()
{
    struct timespec current;
    uint32_t seconds = 0;
    if( clock_gettime( CLOCK_MONOTONIC, &current) != -1 )
    {
        seconds = current.tv_sec;
    }

    return seconds;
}

#else

#define LEAP(x) (((x) % 4) == 0 && (((x) % 100) != 0 || ((x) % 400) == 0))

#define SEC_PER_MINUTE  (60)
#define SEC_PER_HOUR    (60*SEC_PER_MINUTE)
#define SEC_PER_DAY     (24*SEC_PER_HOUR)

#define DAYS_IN_LEAP_YEAR   (366)
#define DAYS_IN_NORMAL_YEAR (365)


#define SYSTEM_RTC "/dev/rtc0"

int32_t GetUtcTimestamp(   int year,    /* Absolute years in Gregorian calender ex 2007 */
                               int month,   /* 1..12  */
                               int day,     /* 1..31  Depending on the month */
                               int hours,   /* 0..23  */
                               int minutes, /* 0..59  */
                               int seconds  /* 0..59  */
                            )
{
    int32_t utctimeSeconds = 0;

    static const DaysBeforeMonth[12] ={0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}; // only for non leap year. For leap year, compensated later

    if( year > 1970 )
    {
        int localYear = 1970;

        do
        {
            /* For each year add the seconds! */
            if( LEAP(localYear) )
            {
                utctimeSeconds += DAYS_IN_LEAP_YEAR*SEC_PER_DAY;
            }
            else
            {
                utctimeSeconds += DAYS_IN_NORMAL_YEAR*SEC_PER_DAY;
            }
            localYear++;
        } while (localYear < year );
    }

    /* Now calculate the seconds in current year */

    /* The number of days till date = all days in previous month + days till today in current month */
    utctimeSeconds +=  (DaysBeforeMonth[month-1] + (day-1) )*SEC_PER_DAY;

    /* Seconds in the current day */
    utctimeSeconds +=  hours*SEC_PER_HOUR + minutes*SEC_PER_MINUTE + seconds;

    if( LEAP(year) && month > 2 )
    {
        /* Leap Year *//*If the day is after febraury , add another day worth of seconds */
        utctimeSeconds += SEC_PER_DAY;
    }

    return (utctimeSeconds);
}

uint32_t AgpsLinuxGetRtcSecondCount()
{
    int  fd;
    struct rtc_time rtc_tm;
    int ioctlRetval;
    uint32_t utcTime;


    fd = open(SYSTEM_RTC, O_RDONLY);

    ioctlRetval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    if (ioctlRetval == -1) {
        perror("RTC_RD_TIME ioctl");
        return 0;
    }

    utcTime = GetUtcTimestamp(rtc_tm.tm_year + 1900, rtc_tm.tm_mon + 1, rtc_tm.tm_mday, rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec );
    DEBUG_LOG_PRINT_LEV2(("RTC timestamp : %u", utcTime ));

    return utcTime;

}
#endif

#ifdef AGPS_LINUX_SYSV_SEM

/* Binary Semaphore implementation for Linux */
/* We must define union semun ourselves. */
/*
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
}; */


/* Deallocate a binary semaphore. All users must have finished their
   use. Returns -1 on failure. */

int DeinitBinarySemaphore(int semid)
{
     union semun ignored_argument;
     return semctl (semid, 1, IPC_RMID, ignored_argument);
}


int InitBinarySemaphore ( BOOL bValue )
{
    union semun argument;
    key_t key = GetMsCount();
    int semid;

    semid = semget ( IPC_PRIVATE , 1, IPC_CREAT | IPC_EXCL | S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    if( semid != -1 )
    {
        DEBUG_LOG_PRINT_LEV2(("SemID : %d\n" , semid ));

        if( bValue )    argument.val = 1;
        else            argument.val = 0;

        if( -1 == semctl (semid, 0, SETVAL, argument) )
        {
            return -1;
        }
    }
    else
    {
        DEBUG_LOG_PRINT(("semget failed %d\n" , errno ));
    }

    return semid;
}

int WaitOnBinarySemaphore(int semid)
{
    struct sembuf operations[1] = { {0,  /* Operation on Sem#0 */
                                   -1,  /* Decrement count if greater than zero */
                                   SEM_UNDO}  /* If process terminates...undo the lock */
                                  };

    return semop (semid, operations, 1);
}

/* Post to a binary semaphore: increment its value by 1.
   This returns immediately. */
int SignalBinarySemaphore (int semid)
{
    struct sembuf operations[2] = { {0, 0, 0}, /* wait for sem #0 to become 0 */
                                    {0, 1, SEM_UNDO} /* then increment sem #0 by 1 */
                                   };

    return semop (semid, operations, 2);
}
#endif /* #ifdef AGPS_LINUX_SYSV_SEM */

#ifndef AGPS_DONOT_USE_OS_QUEUE

typedef struct
{
    long mtype;
    char mtext[16];
} t_MsgQueueElem;

void AgpsInitMsgQueue(t_AgpsFsmContext *p_Ctxt)
{
    p_Ctxt->v_Queue = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | S_IROTH | S_IWOTH | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
    DEBUG_LOG_PRINT_LEV2(("msgget returned : %d\n" , p_Ctxt->v_Queue ));
}

void AgpsDeinitMsgQueue(t_AgpsFsmContext *p_Ctxt)
{
    // Do Nothing
}

/*
    This is the function used to post a message to the internal queue
    */
void AgpsEnQueueFsmMsg( t_MsgHeader  *p_Msg  , t_AgpsFsmContext *p_FsmCtxt )
{
    int retVal;
    t_MsgQueueElem vl_QueueElem;

    DEBUG_LOG_PRINT_LEV2(("Message EnQueueing Event : %lx\n" , p_Msg->v_OperationType ));

    DEBUG_LOG_PRINT_LEV2(("p_Msg = %p", p_Msg ));
    /* we send the t_MsgHeader pointer via the OsMessage structure to the appropriate task */
    /* We will send the pointer in the queue */
    vl_QueueElem.mtype = 1;
    memcpy( vl_QueueElem.mtext , &p_Msg , sizeof(t_MsgHeader *) );
    retVal = msgsnd( p_FsmCtxt->v_Queue , (void *) &vl_QueueElem , sizeof(t_MsgHeader *) , 0 );

    DEBUG_LOG_PRINT_LEV2(("Message EnQueued Event : %lx\n" , p_Msg->v_OperationType ));

}

/*
    This is the function used to retrieve messages from the internal queue
    */
t_MsgHeader*   AgpsDeQueueFsmMsg( t_AgpsFsmContext *p_FsmCtxt )
{
   t_MsgHeader *p_Msg = NULL;    // temporary message pointer
   t_MsgQueueElem vl_QueueElem;

   msgrcv(p_FsmCtxt->v_Queue,   // Queue of current task
          &vl_QueueElem,        // Container to hold the pointer
          16,
          1,
          0);

   memcpy( &p_Msg , vl_QueueElem.mtext , sizeof(t_MsgHeader *) );
   DEBUG_LOG_PRINT_LEV2(("p_Msg = %p" , p_Msg ));

   if( NULL != p_Msg )
   {
       DEBUG_LOG_PRINT_LEV2(("Message DeQueued Event : %lx\n" , p_Msg->v_OperationType ));
   }

   return p_Msg;

}
#endif

#else
/* Add new platforms here */
#error Platform Not defined
#endif

void AgpsOsaInit()
{
    #ifdef AGPS_LINUX_FTR
    AgpsLinuxInit();
    #endif
}

void AgpsOsaDeinit()
{
    #ifdef AGPS_LINUX_FTR
    AgpsLinuxDeinit();
    #endif
}

#ifdef AGPS_DONOT_USE_OS_QUEUE

void AgpsInitMsgQueue(t_AgpsFsmContext *p_Ctxt)
{
#ifdef AGPS_LINUX_FTR
    if (!(OSA_InitCountingSem( p_Ctxt->v_Queue.v_Critic)) )
    {
        DEBUG_LOG_PRINT(("ERROR :AgpsInitMsgQueue OSA_InitCountingSem1"));
    }

    if ( !OSA_SemCount(p_Ctxt->v_Queue.v_Critic)) /* We initially set it to high. While posting and removing, we set it to zero */
        { DEBUG_LOG_PRINT(("ERROR :AgpsInitMsgQueue 0 != sem_post(&p_Ctxt->v_Queue.v_Critic)")); }
#else
    OSA_InitCriticalSection( &p_Ctxt->v_Queue.v_Critic );
#endif
    if (!( OSA_InitCountingSem( p_Ctxt->v_Queue.v_CountingSem )) )
    {
        DEBUG_LOG_PRINT(("ERROR :AgpsInitMsgQueue OSA_InitCountingSem2"));
    }

}

void AgpsDeinitMsgQueue(t_AgpsFsmContext *p_Ctxt)
{
#ifdef AGPS_LINUX_FTR
    if ( !OSA_DeinitCountingSem( p_Ctxt->v_Queue.v_Critic ))
        { DEBUG_LOG_PRINT(("ERROR :AgpsDeinitMsgQueue 0 != sem_destroy(&p_Ctxt->v_Queue.v_Critic)")); }
#else
    OSA_DeinitCriticalSection( &p_Ctxt->v_Queue.v_Critic );
#endif
    if ( !OSA_DeinitCountingSem( p_Ctxt->v_Queue.v_CountingSem ))
        { DEBUG_LOG_PRINT(("ERROR :AgpsDeinitMsgQueue 0 != sem_destroy(&p_Ctxt->v_Queue.v_CountingSem)")); }
}

/*
    This is the function used to post a message to the internal queue
    */
void AgpsEnQueueFsmMsg( t_MsgHeader  *p_Msg  , t_AgpsFsmContext *p_FsmCtxt )
{
    t_MsgQueueElement *p_MsgQElem=NULL;
    t_MsgQueue* p_MsgQ = &(p_FsmCtxt->v_Queue);

    if( NULL == p_Msg )
        return;

    DEBUG_LOG_PRINT_LEV2(("Message EnQueueing Event : %x\n" , p_Msg->v_OperationType ));

    p_MsgQElem = OSA_Malloc( sizeof( *p_MsgQElem ) );

    p_MsgQElem->p_MsgHdr = p_Msg;

#ifdef AGPS_LINUX_FTR
    /* mutex is not async -signal safe */
    if ( ! OSA_WaitForSemSignalled(p_FsmCtxt->v_Queue.v_Critic))
        { DEBUG_LOG_PRINT(("ERROR :AgpsEnQueueFsmMsg 0 != sem_post(&p_FsmCtxt->v_Queue.v_Critic)")); }
#else
    OSA_EnterCriticalSection(&(p_FsmCtxt->v_Queue.v_Critic));
#endif

    /// The TLS implementation uses messages to allow asynchronous handling of
    /// data between the various modules yet retain integrity.
    if( p_MsgQ->p_Tail == NULL)
    {
        // queue is empty
        p_MsgQ->p_Head = p_MsgQElem;
        p_MsgQ->p_Tail = p_MsgQElem;
        p_MsgQElem->p_NextMsg = NULL;
    }
    else
    {
        // add message to tail end.
        p_MsgQ->p_Tail->p_NextMsg = p_MsgQElem;     // point p_NextMsg of last message in queue to new message
        p_MsgQ->p_Tail = p_MsgQElem;                // point tail to new message
        p_MsgQElem->p_NextMsg = NULL;                     // keep p_NextMsg of new message NULL
    }

#ifdef AGPS_LINUX_FTR
    /* mutex is not async -signal safe */
    if ( !OSA_SemCount(p_FsmCtxt->v_Queue.v_Critic))
        { DEBUG_LOG_PRINT(("ERROR :AgpsEnQueueFsmMsg 0 != sem_post(&p_FsmCtxt->v_Queue.v_Critic)")); }
#else
    OSA_ExitCriticalSection(&(p_FsmCtxt->v_Queue.v_Critic));
#endif

    DEBUG_LOG_PRINT_LEV2(("Message EnQueued Event : %x\n" , p_Msg->v_OperationType ));

    if( FALSE == OSA_SemCount( p_FsmCtxt->v_Queue.v_CountingSem ) )
    {
        DEBUG_LOG_PRINT(("ERROR : Failed to signal semaphore! : %x\n" , p_Msg->v_OperationType ));
    }
}


/*
    This is the function used to retrieve messages from the internal queue
    */
t_MsgHeader*   AgpsDeQueueFsmMsg( t_AgpsFsmContext *p_FsmCtxt )
{
    t_MsgHeader *p_Msg = NULL;    // temporary message pointer
    t_MsgQueueElement *p_MsgQElem = NULL;
    t_MsgQueue  *p_MsgQ = &(p_FsmCtxt->v_Queue);

    if ( !OSA_WaitForSemSignalled( p_FsmCtxt->v_Queue.v_CountingSem ))
        { DEBUG_LOG_PRINT(("ERROR :AgpsDeQueueFsmMsg 0 != sem_wait(&p_FsmCtxt->v_Queue.v_CountingSem)")); }

#ifdef AGPS_LINUX_FTR
    /* mutex is not async -signal safe */
    if ( !OSA_WaitForSemSignalled(p_FsmCtxt->v_Queue.v_Critic))
        { DEBUG_LOG_PRINT(("ERROR :AgpsDeQueueFsmMsg 0 != sem_wait(&p_FsmCtxt->v_Queue.v_Critic)")); }
#else
    OSA_EnterCriticalSection(&(p_FsmCtxt->v_Queue.v_Critic));
#endif

    if(p_MsgQ->p_Head != NULL)
    {
        // remove message from head end.
        p_MsgQElem = p_MsgQ->p_Head;
        // is this the only remaingin message in the queue
        if (p_MsgQ->p_Head->p_NextMsg == NULL)
        {
            // reset the head and tail
            p_MsgQ->p_Tail = NULL;
            p_MsgQ->p_Head = NULL;
        }
        else
        {
            // join up the p_Head of the queue to the following message
            p_MsgQ->p_Head = p_MsgQElem->p_NextMsg;   // point head of queue to next message in queue
            // tidy up the message before passing it back
            p_MsgQElem->p_NextMsg = NULL;  // p_PrevMsg will be NULL already for head message
        }
    }

#ifdef AGPS_LINUX_FTR
    /* mutex is not async -signal safe */
    if ( !OSA_SemCount(p_FsmCtxt->v_Queue.v_Critic))
        { DEBUG_LOG_PRINT(("ERROR :AgpsDeQueueFsmMsg 0 != sem_post(&p_FsmCtxt->v_Queue.v_Critic)")); }
#else
    OSA_ExitCriticalSection(&(p_FsmCtxt->v_Queue.v_Critic));
#endif

    if( NULL!= p_MsgQElem )
    {
        p_Msg = p_MsgQElem->p_MsgHdr;
        OSA_Free( p_MsgQElem );
    }

    if( NULL != p_Msg )
    {
        DEBUG_LOG_PRINT_LEV2(("Message DeQueued Event : %x\n" , p_Msg->v_OperationType ));
    }

    return p_Msg;
}

#else

#ifdef AGPS_LINUX_FTR

#endif /* AGPS_LINUX_FTR */
#endif /* #ifdef AGPS_DONOT_USE_OS_QUEUE */

#ifdef HEAP_MEMORY_DEBUG
void OSA_Add_Malloc_Request_To_List(void *p_Addr, int v_size)
{

    if(v_MallocCount < ARRAY_SIZE )
    {
        a_MallocAddrDb[v_MallocCount] = p_Addr;
        a_MallocSizeDb[v_MallocCount] = v_size;
        v_MallocCount ++;
    }
}


void* OSA_Malloc_Debug(char *p_FileName, char* p_Function, int v_LineNum, int v_SizeElements)
{
    void * pl_AllocatedPointer = NULL;
   
    int v_MemRequestNeeded     = v_SizeElements + SIGNATURE_SIZE;

    if ( (v_MemRequestNeeded < SIGNATURE_SIZE) )
    {
        DEBUG_LOG_PRINT (("\n MEM_DEBUG : Unexpected malloc size %d ", v_MemRequestNeeded));
    }

    pl_AllocatedPointer = OSA_Malloc(v_MemRequestNeeded);

    if(pl_AllocatedPointer != NULL)
    {
        if( v_MallocCount < ARRAY_SIZE )
        {
            DEBUG_LOG_PRINT (("\n MEM_DEBUG : The allocated memory address is : %x and size : %d and  index is %d p_Function: %s v_LineNum %d", pl_AllocatedPointer, v_MemRequestNeeded, v_MallocCount,p_Function,v_LineNum ));

            memcpy(&pl_AllocatedPointer[v_MemRequestNeeded - SIGNATURE_SIZE], &a_ConstSignature, SIGNATURE_SIZE);
            OSA_Add_Malloc_Request_To_List( pl_AllocatedPointer,v_MemRequestNeeded );
        }
        else
        {                         
            DEBUG_LOG_PRINT (("\n MEM_DEBUG :********* Failed to store MEMORY ADDRESS %x in the Array *********", pl_AllocatedPointer));
        }
    }
    else
    {
        DEBUG_LOG_PRINT (("\n MEM_DEBUG :********* MEMORY Allocation failed - NO MEMORY ALLOCATED *********"));
    }

    return pl_AllocatedPointer;
}


void OSA_Free_Debug(char *p_FileName, char* p_Function, int v_LineNum,  void* p_FreePtr)
{
    int  index                = 0;
    int  loop                 = 0;
    int  memoryOverwrite      = 0;
    int  memoryFreeRequested  = 0;
    int  memoryRequested      = 0;
    bool memoryIndexPresent   = FALSE;

    if(p_FreePtr != NULL )
    {
        for( loop = 0; loop < ARRAY_SIZE ; loop++ )
        {
            if(a_MallocAddrDb[loop] == p_FreePtr)
            {
                index = loop;
                memoryFreeRequested = a_MallocSizeDb[index];             
                DEBUG_LOG_PRINT (("\n MEM_DEBUG :TRYING TO FREE MEMORY ADDRESS %x of size : %d and Memory index is %d p_Function: %s v_LineNum: %d ", p_FreePtr,memoryFreeRequested,index,p_Function,v_LineNum));
                memoryRequested      = memoryFreeRequested - SIGNATURE_SIZE;
                a_MallocSizeDb[loop]        = 0 ;
                a_MallocAddrDb[loop]        = NULL ;   // Avoid repetition of memory address
                memoryIndexPresent   = TRUE;
                break;
            }
        }

        if(memoryIndexPresent)
        {
            memoryOverwrite = memcmp(&p_FreePtr[memoryRequested], &a_ConstSignature, SIGNATURE_SIZE);
         
            if (memoryOverwrite != 0)
            {
                DEBUG_LOG_PRINT (("\n MEM_DEBUG : Memory Overwritten  %x and index is %d ", p_FreePtr,index));
            }
        }
        else
        {
            DEBUG_LOG_PRINT (("\n MEM_DEBUG :**********TRYING TO FREE MEMORY ADDRESS %x not present in the Array *******" , p_FreePtr ));
            memoryIndexPresent = FALSE;
        }
          
        OSA_Free(p_FreePtr);
        p_FreePtr = NULL;

    }
}
#endif


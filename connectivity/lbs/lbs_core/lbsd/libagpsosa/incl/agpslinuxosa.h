/*
* Copyright (C) ST-Ericsson 2009
*
* agpslinuxosa.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#ifndef AGPS_LINUX_OSA_H
#define AGPS_LINUX_OSA_H

/* + Linux Includes */
#ifdef LBS_LINUX_UBUNTU_FTR
#include <sys/types.h>
#endif
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <semaphore.h>
#include <pwd.h>
#include <netdb.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <linux/ipc.h>
#include <linux/sem.h>
#include <linux/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#ifndef AGPS_LINUX_BARE_FTR
#ifndef ARM_LINUX_AGPS_FTR
#include <android_log.h>
#endif
#endif
#include "agpslinuxtypes.h"
/* - Linux Includes */

/* + C-RunTime Includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef LBS_LINUX_UBUNTU_FTR
#include <time.h>
#endif
/* - C-RunTime Includes */

/* + Critical section OSA */
/*! \addtogroup Linux_CriticalSection
 *      \details
 */
/*@{*/
/**
    \brief defines the static initializer for a critic section \ref t_OsaCriticSection
*/
#define OSA_DEFAULT_CRITIC_SECTION  PTHREAD_MUTEX_INITIALIZER

/**
    \typedef t_OsaCriticSection
    \brief This structure defines a critical section. \ref OSA_DEFAULT_CRITIC_SECTION
*/
typedef pthread_mutex_t t_OsaCriticSection;

/// \brief
///     This macro will Initialize a critical section. 
/// \details
///     In case of linux, a mutex is created. 
///     After Critical section usage is done, call \ref OSA_DeinitCriticalSection
///     to delete it
///     A critic section is a light weight alternative to a semaphore. It cannot be 
///     force mutual exclusion between different processes.
///     <br>[in] pA pointer of type \ref t_OsaCriticSection
/// \returns
///      Nothing
#define OSA_InitCriticalSection( pA )       (void)pthread_mutex_init( pA  , NULL )

/// \brief
///     This macro is used to delete a critical section after use.
/// \details
///     <br>[in] pA pointer of type \ref t_OsaCriticSection
/// \returns
///     Nothing
#define OSA_DeinitCriticalSection( pA )     (void)pthread_mutex_destroy( pA )

/// \brief
///     This macro is used to signal entry to a critic section
/// \details
///     This macro will block entry to the critical section by all other
///     thread of the same process.[ in linux ].
///     If you need to a critical section with process blocking, use semaphores.
///     <br>[in] pA pointer of type \ref t_OsaCriticSection
/// \returns
///     Nothing
#define OSA_EnterCriticalSection( pA )      (void)pthread_mutex_lock( pA )

/// \brief
///     This macro is used to signal exit from a critic section
/// \details
///     This macro will free the critical section for access by
///     other requestees.
///     <br>[in] pA pointer of type \ref t_OsaCriticSection
/// \returns
///     Nothing
#define OSA_ExitCriticalSection( pA )       (void)pthread_mutex_unlock( pA )
/*@}*/
/* - Critical section OSA */

/* + Binary Semaphore OSA */
/*! \addtogroup Linux_BinarySemaphore
 *      \details
 */
/*@{*/
/**
    \brief defines the static initializer for a binary semaphore \ref t_OsaBinSemHandle
*/
#define OSA_DEFAULT_BIN_SEM     0

/**
    \brief This structure defines a handle for a binary semaphore. \ref OSA_DEFAULT_BIN_SEM
*/
typedef int t_OsaBinSemHandle;

/// \brief
///     This macro is used to initialize a binary semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hHandle of type \ref t_OsaBinSemHandle
///     <br>[in]        bValue  of BOOL . Specifies initial signalled state of the semaphore
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error
#define OSA_InitBinarySemaphore( hHandle , bValue )          ( -1 != (hHandle = InitBinarySemaphore(bValue) ) )

/// \brief
///     This macro is used to signal a binary semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaBinSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error
#define OSA_SignalBinarySemaphore( hHandle )        ( -1 != SignalBinarySemaphore( hHandle ) )

/// \brief
///     This macro is used to block infinitely till a binary semaphore moves to signalled state.
///     The semaphore is then moved to non-signalled state
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaBinSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error
#define OSA_WaitOnBinarySemaphore( hHandle )        ( -1 != WaitOnBinarySemaphore( hHandle ) )

/// \brief
///     This macro is used to deinitialize a binary semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaBinSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_DeinitBinarySemaphore( hHandle )        ( -1 != DeinitBinarySemaphore( hHandle ) )
/*@}*/
/* - Binary Semaphore OSA */

/* + Counting Semaphores OSA */
/*! \addtogroup Linux_CountingSemaphore
 *      \details
 */
/*@{*/
/**
    \brief defines the max count value for a counting semaphore
*/
#define OSA_MAX_SEM_COUNT       50
/**
    \brief defines the initialization count value for a counting semaphore
*/
#define OSA_INIT_SEM_COUNT      0

/**
    \brief defines the static initializer for a counting semaphore \ref t_OsaSemHandle
*/
#define OSA_DEFAULT_SEM_HANDLE  {0}

/**
    \brief This structure defines a handle for a counting semaphore. \ref OSA_DEFAULT_SEM_HANDLE
*/
typedef sem_t   t_OsaSemHandle;

/// \brief
///     This macro is used to initialize a counting semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hHandle of type \ref t_OsaSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_InitCountingSem( hHandle )      ( 0 == sem_init(&(hHandle), 0 , OSA_INIT_SEM_COUNT) )

/// \brief
///     This macro is used to initialize a counting semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hHandle of type \ref t_OsaSemHandle
///     <br>[in]        initSemCount
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_InitCountingSemA( hHandle , initSemCount )      ( 0 == sem_init(&(hHandle), 0 , initSemCount) )


/// \brief
///     This macro is used to signal a counting semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaSemHandle
///     If the count value has reached \ref OSA_MAX_SEM_COUNT, then a further
///     call to this function will return a FALSE
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_SemCount( hHandle )             ( 0 == sem_post(&( hHandle ) ) )

/// \brief
///     This macro is used to block till a counting semaphore is in signalled state
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_WaitForSemSignalled( hHandle )  ( 0 == sem_wait( &( hHandle ) ) )

/// \brief
///     This macro is used to deinitialize a counting semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_DeinitCountingSem( hHandle )    ( 0 == sem_destroy(&(hHandle)) )
/*@}*/
/* - Counting Semaphores OSA */

/* + Thread OSA */
/*! \addtogroup Linux_Threads
 *      \details
 */
/*@{*/
/**
    \brief defines the static initializer for a thread handle \ref t_OsaThreadHandle
*/
#define OSA_DEFAULT_THREAD_HANDLE   {0}

/* @todo Priorities to be defined on linux */
#define OSA_THREAD_PRIORITY_HIGH    0

#define OSA_THREAD_PRIORITY_ABOVE_NORMAL    0

#define OSA_THREAD_PRIORITY_NORMAL  0

#define OSA_THREAD_PRIORITY_LOW     0

#define OSA_THREAD_PRIORITY_LOWEST  0
/**
    \brief This structure defines a handle for a Thread. \ref OSA_DEFAULT_THREAD_HANDLE
*/
typedef pthread_t   t_OsaThreadHandle;

/// \brief
///     This macro is used to spawn a thread in running state
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hA : handle of type \ref t_OsaThreadHandle
///     <br>[in]        pB : thread routine
///     <br>[in]        pC : parameter to be passed to the thread routine
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_ThreadCreate(hA , pB , pC )     ( 0 == pthread_create( &hA , NULL ,(void* (*)(void*) )pB ,(void *)pC ) )

/// \brief
///     This macro is used to spawn a thread in running state with priority
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hA : handle of type \ref t_OsaThreadHandle
///     <br>[in]        pB : thread routine
///     <br>[in]        pC : parameter to be passed to the thread routine
///     <br>[in]        iD : parameter to define Thread Priority
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error.
#define OSA_ThreadCreateEx(hA , pB , pC , iD )     ( 0 == pthread_create( &hA , NULL ,(void* (*)(void*) )pB ,(void *)pC ) )

/// \brief
///     This macro is used to spawn a thread in running state with priority
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hA : handle of type \ref t_OsaThreadHandle
///     <br>[in]        pB : thread routine
///     <br>[in]        pC : parameter to be passed to the thread routine
///     <br>[in]        iD : parameter to define Thread Priority
///    <br>[in]        pStack : Stack to be used by thread
///    <br>[in]        uStackSize : size of the Stack to be used by thread
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error.
#define OSA_ThreadCreateEx2(hA , pB , pC , iD , pStack , uStackSize )       ( 0 == pthread_create( &hA , NULL ,(void* (*)(void*) )pB ,(void *)pC ) )


/// \brief
///     This macro is used to spawn a thread in running state with priority
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hA : handle of type \ref t_OsaThreadHandle
///     <br>[in]        pB : thread routine
///     <br>[in]        pC : parameter to be passed to the thread routine
///     <br>[in]        iD : parameter to define Thread Priority
///    <br>[in]        uStackSize : size of the Stack to be used by thread
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error.
#define OSA_ThreadCreateEx3(hA , pB , pC , iD , uStackSize )    ( 0 == pthread_create( &hA , NULL ,(void* (*)(void*) )pB ,(void *)pC ) )

/// \brief
///     This macro is used to block till a thread has exited.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaThreadHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_WaitForThreadExit( hA )         ( 0 == pthread_join(hA, NULL) )

/// \brief
///     This macro is used to deinitialize a thread handle
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaThreadHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_CloseThreadHandle( hA )         TRUE
/*@}*/
/* - Thread OSA */

/* + Socket OSA */
/*! \addtogroup Linux_Sockets
 *      \details
 */
/*@{*/
/**
    defines the socket family for IPV4 sockets
*/
#define OSA_IPV4                AF_INET
/**
    defines the socket family for IPV6 sockets
*/
#define OSA_IPV6                AF_INET6

/**
    defines the socket which is a STREAM socket. Used for TCP/IP connections
*/
#define OSA_SOCK_STREAM         SOCK_STREAM

/**
    defines that a socket is to use TCP/IP protocol*/
#define OSA_IPPROTO_TCP         IPPROTO_TCP

/**
    defines the type of address to be returned by getaddrinfo to be numeric host address [ IPV4 / IPV6 ]
*/
#define OSA_NUMERICHOST         AI_NUMERICHOST

/**
    error type returned by OSA_GetLastSockErr if socket call on non blocking socket fails because the call would block
*/
#define OSA_SOCK_WOULD_BLOCK    EWOULDBLOCK

/**
    error type returned by OSA_GetLastSockErr if socket connection was reset
*/
#define OSA_CONN_RESET          ECONNRESET

/**
    error type returned by OSA_GetLastSockErr if there is socket system failure
*/
#define OSA_SOCK_ERR            -1

/**
    defines invalid value for the socket
*/
#define OSA_INVALID_SOCK        -1

/**
    defines that the socket should be shutdown such that no futher reads are possible.
    does not affect the writes.
*/
#define OSA_STOP_SOCK_READ      SHUT_RD

/**
    defines that the socket should be shutdown such that no futher writes are possible.
    does not affect the reads.
*/
#define OSA_STOP_SOCK_WRITE     SHUT_WR

/**
    defines that the socket should be shutdown such that no futher writes or reads are possible.
*/
#define OSA_STOP_SOCK_BOTH      SHUT_RDWR

typedef struct hostent          OSA_HOSTENT;
/*
    structure which defines a socket. This is filled in call \ref OSA_OpenSocket
*/
typedef int                     OSA_SOCKET;
/*
    structure which specifies the address to which the socket can be bound.
*/
typedef struct sockaddr         OSA_SOCKADDR;
/*
    structure which the specifies the IPV4 address to which the socket can be bound
*/
typedef struct sockaddr_in      OSA_SOCKADDR4;
/*
    structure which the specifies the IPV6 address to which the socket can be bound
*/
typedef struct sockaddr_in6     OSA_SOCKADDR6;

/// \brief
///     This macro is used to create a socket \ref OSA_SOCKET
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    socket sA of type \ref OSA_SOCKET. socket is returned in this function
///     <br>[in]    transport provider family iB of type int \ref OSA_IPV4 and \ref OSA_IPV6
///     <br>[in]    type of socket iC of type int \ref OSA_SOCK_STREAM
///     <br>[in]    protocol used on socket iD of type int \ref OSA_IPPROTO_TCP
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_OpenSocket( sA , iB , iC , iD ) ( -1 != ( sA = socket( iB , iC , iD ) ) )
/// \brief
///     This macro is used to connect to a server
/// \details
///     The input parameters to the macro is :
///     <br>[in]    socket sA of type \ref OSA_SOCKET
///     <br>[in]    pointer pB of type \ref OSA_SOCKADDR. Will be \ref OSA_SOCKADDR4 for IPV4 and \ref OSA_SOCKADDR6 for IPV6
///     <br>[in]    sizeof the structure passed in using pB
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_ConnectSocket( sA , pB , iC )   ( 0 == connect( sA , pB , iC ) )
/// \brief
///     This macro is used to send data to a server. Does not guarantee delivery
/// \details
///     The input parameters to the macro is :
///     <br>[in]    socket sA of type \ref OSA_SOCKET
///     <br>[in]    pointer pB of type char*. points to data to be sent
///     <br>[in]    int iC : length of the data to be sent
/// \returns
///     Number of bytes sent on success.
///     on Failure returns OSA_SOCK_ERR. Use OSA_GetLastError() to determine cause of failure
/// \note 
///     On Failure, Use macro OSA_GetLastError() to get the last error.
///     In case of non-blocking sockets, OSA_SOCK_ERR may be returned if a blocking function is attempted.
///     In such a case, an actual error has happend only if OSA_GetLastError() returns value other than OSA_SOCK_WOULD_BLOCK
#define OSA_SendSocket( sA , pB , iC )      send( sA , pB , iC , 0 )
/// \brief
///     This macro is used to recv data to a server.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    socket sA of type \ref OSA_SOCKET
///     <br>[in]    pointer pB of type char*. points to data buffer where recvd data is to be stored
///     <br>[in]    int iC : length of the data buffer
/// \returns
///     Number of recvd sent on success.
///     on Failure returns OSA_SOCK_ERR. Use OSA_GetLastError() to determine cause of failure
/// \note 
///     On Failure, Use macro OSA_GetLastError() to get the last error.
///     In case of non-blocking sockets, OSA_SOCK_ERR may be returned if a blocking function is attempted.
///     In such a case, an actual error has happend only if OSA_GetLastError() returns value other than OSA_SOCK_WOULD_BLOCK
#define OSA_RecvSocket( sA , pB , iC )      recv( sA , pB , iC , 0 )
/// \brief
///     This macro is used to shutdown the socket.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    socket sA of type \ref OSA_SOCKET
///     <br>[in]    int iB. should be set to \ref OSA_STOP_SOCK_READ , \ref OSA_STOP_SOCK_WRITE or \ref OSA_STOP_SOCK_BOTH
/// \returns
///     TRUE on success and FALSE on failure.
/// \note 
///     On Failure, Use macro OSA_GetLastError() to get the last error.
///     In case of non-blocking sockets, OSA_SOCK_ERR may be returned if a blocking function is attempted.
///     In such a case, an actual error has happend only if OSA_GetLastError() returns value other than OSA_SOCK_WOULD_BLOCK
///     In case of TCP/Ip connections. First shutdown with only OSA_STOP_SOCK_WRITE. This begins the first phase of the
///     TCP/IP connection teardown. Wait till a call to OSA_RecvSocket returns 0. This indicates the TCP/IP connection
///     is shutdown at both ends. Following this call OSA_CloseSocket
#define OSA_ShutdownSocket( sA , iB )       ( 0 != shutdown( sA , iB ) )
/// \brief
///     This macro is used to close a socket.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    socket sA of type \ref OSA_SOCKET
/// \returns
///     TRUE on success and FALSE on failure.
#define OSA_CloseSocket( sA )               ( 0 != close( sA ) )

/// \brief
///     This macro is used to set a socket into non blocking mode.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    socket sA of type \ref OSA_SOCKET
///     <br>[in]    pointer pB should point to ulong element with non-zero value
/// \returns
///     TRUE on success and FALSE on failure.
#define OSA_NBSocket( sA , pB )             fcntl( sA , F_SETFL , O_NDELAY )

/// \brief
///     This macro is used get the last socket error. Not valid is no error has occured.
#define OSA_GetLastSockErr()                errno
/*@}*/
/* - Socket OSA */

/* + Timer OSA */
/*! \addtogroup Linux_Timers
 *      \details
 */
/*@{*/

typedef void (* t_OsaTimerCallback )( void* p_Param );

/// \brief
///     This structure defines the handle to an OS Timer.
typedef uint32_t         t_OsaTimerHandle;

/// \brief
///     This macro is used to launch an OS Timer.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    handle hHandle of type \ref t_OsaTimerHandle
///     <br>[in]    callback cCallback 
///     <br>[in]    pParam which is passed to the callback function
///     <br>[in]    expiry time in milliseconds
/// \returns
#define OSA_StartTimer( hHandle , cCallback , pParam , uExpiry )\
                                            AgpsLinuxStartTimer( &hHandle , cCallback , pParam , uExpiry )

/// \brief
///     This macro is used to stop an OS Timer.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    handle hHandle of type \ref t_OsaTimerHandle
/// \returns
///     TRUE on success and FALSE on failure. Call OSA_GetLastError to get further error information                                            
#define OSA_StopTimer( hHandle )            AgpsLinuxStopTimer( hHandle )
/*@}*/
/* - Timer OSA */

/* + File Handling */
/*! \addtogroup Linux_Files
 *      \details
 */
/*@{*/
/* defines invalid file handle value */
#define OSA_INVALID_FILE_HANDLE     NULL

/* Defines Handle to a file */
typedef FILE    t_OsaFile;

/// \brief
///     This macro is used create/open a file.
/// \details
///     The input parameters to the macro are :
///     <br>[in]    file name string of type const char*
///     <br>[in]    mode in which to open [r/w ,rw etc ] of type const char*
///     <br>        "r" Open a file for reading. The file must exist. 
///     <br>        "w" Create an empty file for writing. If a file with the same name 
///                 already exists its content is erased and the file is treated as a new empty file.  
///     <br>        "a" Append to a file. Writing operations append data at the end of the file. 
///                 The file is created if it does not exist. 
///     <br>        "r+" Open a file for update both reading and writing. The file must exist. 
///     <br>        "w+" Create an empty file for both reading and writing. If a file with the 
///                 same name already exists its content is erased and the file is treated as a new empty file. 
///     <br>        "a+" Open a file for reading and appending. All writing operations are performed 
///                 at the end of the file, protecting the previous content to be overwritten. 
/// \returns
///     On success t_OsaFile* pointer. On failure OSA_INVALID_FILE_HANDLE
#define OSA_FileOpen(FileName, Mode)        fopen((const char*)FileName, (const char*)Mode)
#define OSA_Open(FileName, Mode)        open((const char*)FileName, (int)Mode)

/// \brief
///     This macro is used to release file handle associated with a file.
///     Also flushes all unwritten data from the buffers to the file
/// \details
///     The input parameters to the macro is : 
///     <br>[in]    file handle of type \ref t_OsaFile*
/// \returns
///     On success returns 0. On failure call OSA_GetLastError to get error reason
#define OSA_FileClose(FileHandler)          fclose(FileHandler)
#define OSA_Close(FileHandler)          close(FileHandler)

/// \brief
///     This macro is used to make a FIFO specialfile.
/// \details
/// \returns

#define OSA_MakeFIFO(FileName, Mode)        mkfifo((const char*)FileName, (const char*)Mode)

/// \brief
///     This macro is used to remove a file.
/// \details
/// \returns

#define OSA_Unlink(FileName)        unlink((const char*)FileName)

/// \brief
///     This macro is used to write data from an array to a file
/// \details
///     The fwrite() function shall write, from the array pointed to by DataPointer, 
///     up to NoOfStructs elements whose size is specified by Size, 
///     to the stream pointed to by FileHandler
///     The input parameters to the macro are : 
///     <br>[in]    pointer to buffer which holds data to be written
///     <br>[in]    Specifies number of elements
///     <br>[in]    Specifies size of each element
///     <br>[in]    file handle of type \ref t_OsaFile*
/// \returns
///     On success returns bytes written. On failure (-1) call OSA_GetLastError to get error reason
#define OSA_FileWrite(DataPointer, NoOfStructs, Size, FileHandler) \
                                            fwrite( DataPointer, NoOfStructs, Size, FileHandler )
#define OSA_Write(FileHandler, DataPointer, Size) \
                                            write(FileHandler, DataPointer, Size)

/// \brief
///     This macro is used to read data from a file
/// \details
///     The fwrite() function shall read, into the array pointed to by DataPointer, 
///     up to NoOfStructs elements whose size is specified by Size, 
///     to the stream pointed to by FileHandler
///     The input parameters to the macro are : <br>
///     <br>[in]    pointer to buffer which will holds any read data
///     <br>[in]    Specifies number of elements
///     <br>[in]    Specifies size of each element
///     <br>[in]    file handle of type \ref t_OsaFile*
/// \returns
///     On success returns bytes read. On failure (-1) call OSA_GetLastError to get error reason
#define OSA_FileRead(DataPointer, NoOfStructs, Size, FileHandler) \
                                            fread( DataPointer, NoOfStructs, Size, FileHandler )

/// \brief
///    flushes all unwritten data from the buffers to the file
/// \details
///     The input parameters to the macro is : <br>
///     <br>[in]    file handle of type \ref t_OsaFile*
/// \returns
///     On success returns 0. On failure call OSA_GetLastError to get error reason
#define OSA_FileFlush(FileHandler)          fflush(FileHandler)

/*@}*/
/* - File Handling */

#define OSA_FileSeek(FileHandler, Offset, Whence)       fseek(FileHandler, Offset, Whence)


/// \brief
///    Return the current value of the file-position indicator for the stream
/// \details
///     The input parameters to the macro is : <br>
///     <br>[in]    file handle of type \ref t_OsaFile*
/// \returns
///     On success returns shall return the current value of the file-position indicator for the stream measured in bytes from the beginning of the file. <br>
///     On failure call OSA_GetLastError to get error reason
#define OSA_FileTell(FileHandler)                      ftell(FileHandler)


#define OSA_FileFstat(FileHandler, StatInstance)        fstat(FileHandler, StatInstance)


/* - LMSqc36093 libagpsosa - implementaion for fseek. */

#define OSA_FileDelete(FileName)          remove((const char*)FileName)

/*@}*/
/* - File Handling */

/* + Memory Management */
/*! \addtogroup Linux_MemoryManagement
 *      \details
 */
/*@{*/
/// \brief
///     Allocated Memory from Heap
/// \details
/// [in] SIZE size in bytes to be allocated
/// \returns
///     NULL pointer on failure. Valid Pointer on Success
#define OSA_Malloc( SIZE )                  malloc( SIZE )

/// \brief
///     Deallocates Memory from Heap
/// \details
/// [in] void pointer to memory previously allocated from heap
/// \returns
///     none
#define OSA_Free( POINTER )                 {\
                                 if(POINTER!= NULL)\
                                     free( POINTER );\
                                 POINTER = NULL;\
                             }

/*@}*/
/* - Memory Management */
/* + Misc OSA */
/*! \addtogroup Linux_Misc
 *      \details
 */
/*@{*/
/// \brief
///     This macro is used get the last error. Not valid is no error has occured.
#define OSA_GetLastError()                  errno

/// \brief
///     This macro is used get the current OS millisecond tick count. Resolution varies
#define OSA_GetMsCount()                    GetMsCount()
#define OSA_GetRtcSecondCount()             AgpsLinuxGetRtcSecondCount()
/// \brief
///     This macro is used put the thread to sleep for mA milliseconds
#define OSA_Sleep(mA)                       usleep(mA*1000)

#ifndef AGPS_LINUX_BARE_FTR

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define  LOG_TAG  "LBS"
#define OSA_Print( X )                      ALOGD X
#else
#define OSA_Print( X )                      printf X
#endif
/*@}*/
/* - Misc OSA */


/* + OS Specific Implementations */
uint64_t GetMsCount();
void AgpsLinuxTimerInit(void);
int WaitOnBinarySemaphore(int semid);
int InitBinarySemaphore ( BOOL bValue );
int DeinitBinarySemaphore(int semid);
int SignalBinarySemaphore (int semid);
int AgpsLinuxStartTimer( t_OsaTimerHandle * p_Handle , t_OsaTimerCallback v_Callback , void* p_Param , uint32_t v_DueTimeMs );
int AgpsLinuxStopTimer( t_OsaTimerHandle v_Handle );
/* - OS Specific Implementations */
/** \addtogroup Linux_OSA Linux_OSA
 *              Directory OSA for Linux
 *  \{
 */
#define OSA_opendir(DirPath)   opendir(DirPath)
#define OSA_readdir(DirPath)   readdir(DirPath)
#define OSA_closedir(DirPath)  closedir(DirPath)
#define OSA_filestat(FilePath,statistics)    stat(FilePath,statistics)

typedef  struct dirent DirectoryPath;
typedef struct stat     FileStatistics;
typedef DIR     directory;

  /**
 *  \}
 */
#endif /* AGPS_LINUX_OSA_H */

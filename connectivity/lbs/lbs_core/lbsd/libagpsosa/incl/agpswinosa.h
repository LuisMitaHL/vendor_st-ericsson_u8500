/*
* Copyright (C) ST-Ericsson 2009
*
* agpswinosa.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/

#ifndef AGPS_WIN_OSA_H
#define AGPS_WIN_OSA_H

#define _WIN32_WINNT 0x500

/* + Windows Includes */
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
/* - Windows Includes */

/* + C-RunTime Includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
/* - C-RunTime Includes */

#include "agpswintypes.h"

/* + Critical section OSA */
/*! \addtogroup Windows_CriticalSection
 *      \details
 */
/*@{*/
/**
    \brief defines the static initializer for a critic section \ref t_OsaCriticSection
*/
#define OSA_DEFAULT_CRITIC_SECTION  {0}

/**
    \typedef t_OsaCriticSection
    \brief This structure defines a critical section. \ref OSA_DEFAULT_CRITIC_SECTION
*/
typedef CRITICAL_SECTION    t_OsaCriticSection;

/// \brief
///     This macro will Initialize a critical section. 
/// \details
///     In case of windows, a critical section is created. 
///     After Critical section usage is done, call \ref OSA_DeinitCriticalSection
///     to delete it
///     A critic section is a light weight alternative to a semaphore. It cannot be 
///     force mutual exclusion between different processes.
///     <br>[in] pA pointer of type \ref t_OsaCriticSection
/// \returns
///      Nothing
#define OSA_InitCriticalSection( pA )       InitializeCriticalSection( pA )

/// \brief
///     This macro is used to delete a critical section after use.
/// \details
///     <br>[in] pA pointer of type \ref t_OsaCriticSection
/// \returns
///     Nothing
#define OSA_DeinitCriticalSection( pA )     DeleteCriticalSection( pA )

/// \brief
///     This macro is used to signal entry to a critic section
/// \details
///     This macro will block entry to the critical section by all other
///     thread of the same process.[ in windows ].
///     If you need to a critical section with process blocking, use semaphores.
///     <br>[in] pA pointer of type \ref t_OsaCriticSection
/// \returns
///     Nothing
#define OSA_EnterCriticalSection( pA )      EnterCriticalSection( pA )

/// \brief
///     This macro is used to signal exit from a critic section
/// \details
///     This macro will free the critical section for access by
///     other requestees.
///     <br>[in] pA pointer of type \ref t_OsaCriticSection
/// \returns
///     Nothing
#define OSA_ExitCriticalSection( pA )       LeaveCriticalSection( pA )
/*@}*/
/* - Critical section OSA */

/* + Binary Semaphore OSA */
/*! \addtogroup Windows_BinarySemaphore
 *      \details
 */
/*@{*/
/**
    \brief defines the static initializer for a binary semaphore \ref t_OsaBinSemHandle
*/
#define OSA_DEFAULT_BIN_SEM     NULL

/**
    \brief This structure defines a handle for a binary semaphore. \ref OSA_DEFAULT_BIN_SEM
*/
typedef HANDLE t_OsaBinSemHandle;

/// \brief
///     This macro is used to initialize a binary semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hHandle of type \ref t_OsaBinSemHandle
///     <br>[in]        bValue  of BOOL . Specifies initial signalled state of the semaphore
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error
#define OSA_InitBinarySemaphore( hHandle , bValue )          ( NULL != ( hHandle = CreateSemaphore( NULL,bValue,1,NULL) ) )

/// \brief
///     This macro is used to signal a binary semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaBinSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error
#define OSA_SignalBinarySemaphore( hHandle )        ( 0 != ReleaseSemaphore( hHandle,1,NULL) )

/// \brief
///     This macro is used to block infinitely till a binary semaphore moves to signalled state.
///     The semaphore is then moved to non-signalled state
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaBinSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error
#define OSA_WaitOnBinarySemaphore( hHandle )        ( WAIT_OBJECT_0 == WaitForSingleObject( hHandle , INFINITE ) )

/// \brief
///     This macro is used to deinitialize a binary semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaBinSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_DeinitBinarySemaphore( hHandle )        ( 0 != CloseHandle( hHandle ) )
/*@}*/
/* - Binary Semaphore OSA */

/* + Counting Semaphores OSA */
/*! \addtogroup Windows_CountingSemaphore
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
#define OSA_DEFAULT_SEM_HANDLE  NULL

/**
    \brief This structure defines a handle for a counting semaphore. \ref OSA_DEFAULT_SEM_HANDLE
*/
typedef HANDLE  t_OsaSemHandle;

/// \brief
///     This macro is used to initialize a counting semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hHandle of type \ref t_OsaSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_InitCountingSem( hHandle )      ( NULL != ( hHandle = CreateSemaphore( NULL,OSA_INIT_SEM_COUNT,OSA_MAX_SEM_COUNT,NULL) ) )

/// \brief
///     This macro is used to initialize a counting semaphore to a specific value
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hHandle of type \ref t_OsaSemHandle
///     <br>[in]        initSemCount
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_InitCountingSemA( hHandle , initSemCount )      ( NULL != ( hHandle = CreateSemaphore( NULL,initSemCount ,OSA_MAX_SEM_COUNT,NULL) ) )

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
#define OSA_SemCount( hHandle )             ( 0 != ReleaseSemaphore( hHandle,1,NULL) )

/// \brief
///     This macro is used to block till a counting semaphore is in signalled state
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_WaitForSemSignalled( hHandle )  ( WAIT_OBJECT_0 == WaitForSingleObject( hHandle , INFINITE ) )

/// \brief
///     This macro is used to deinitialize a counting semaphore
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaSemHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_DeinitCountingSem( hHandle )    ( 0 != CloseHandle( hHandle ) )
/*@}*/
/* - Counting Semaphores OSA */

/* + Thread OSA */
/*! \addtogroup Windows_Threads
 *      \details
 */
/*@{*/
/**
    \brief defines the static initializer for a thread handle \ref t_OsaThreadHandle
*/
#define OSA_DEFAULT_THREAD_HANDLE   NULL

/**
    \brief This structure defines a handle for a Thread. \ref OSA_DEFAULT_THREAD_HANDLE
*/
typedef HANDLE  t_OsaThreadHandle;

/* @todo Priorities to be defined on windows */
#define OSA_THREAD_PRIORITY_HIGH    0

#define OSA_THREAD_PRIORITY_ABOVE_NORMAL    0

#define OSA_THREAD_PRIORITY_NORMAL  0

#define OSA_THREAD_PRIORITY_LOW     0

#define OSA_THREAD_PRIORITY_LOWEST  0

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
#define OSA_ThreadCreate(hA , pB , pC )     (NULL != (hA = CreateThread( (LPSECURITY_ATTRIBUTES)NULL, (uint32_t)0,(LPTHREAD_START_ROUTINE)pB,(void*)pC,0,NULL )) )

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
#define OSA_ThreadCreateEx(hA , pB , pC , iD )     (NULL != (hA = CreateThread( (LPSECURITY_ATTRIBUTES)NULL, (uint32_t)0,(LPTHREAD_START_ROUTINE)pB,(void*)pC,0,NULL )) )

/// \brief
///     This macro is used to spawn a thread in running state with priority
/// \details
///     The input parameters to the macro is :
///     <br>[in/out]    hA : handle of type \ref t_OsaThreadHandle
///     <br>[in]        pB : thread routine
///     <br>[in]        pC : parameter to be passed to the thread routine
///     <br>[in]        iD : parameter to define Thread Priority
///     <br>[in]        pStack : Stack to be used by thread
///     <br>[in]        uStackSize : size of the Stack to be used by thread
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error.
#define OSA_ThreadCreateEx2(hA , pB , pC , iD , pStack , uStackSize )       (NULL != (hA = CreateThread( (LPSECURITY_ATTRIBUTES)NULL, (uint32_t)0,(LPTHREAD_START_ROUTINE)pB,(void*)pC,0,NULL )) )

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
#define OSA_ThreadCreateEx3(hA , pB , pC , iD , uStackSize )    (NULL != (hA = CreateThread( (LPSECURITY_ATTRIBUTES)NULL, (uint32_t)0,(LPTHREAD_START_ROUTINE)pB,(void*)pC,0,NULL )) )

/// \brief
///     This macro is used to block till a thread has exited.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaThreadHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_WaitForThreadExit( hA )         ( WAIT_OBJECT_0 == WaitForSingleObject( hA , INFINITE ) )

/// \brief
///     This macro is used to deinitialize a thread handle
/// \details
///     The input parameters to the macro is :
///     <br>[in]    hHandle of type \ref t_OsaThreadHandle
/// \returns
///     TRUE on success and FALSE on failure
/// \note On Failure, Use macro OSA_GetLastError() to get the last error. 
#define OSA_CloseThreadHandle( hA )         ( 0 != CloseHandle( hA ) )
/*@}*/
/* - Thread OSA */

/* + Socket OSA */
/*! \addtogroup Windows_Sockets
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
#define OSA_NUMERICHOST         AI_NUMERICHOST

/**
    error type returned by OSA_GetLastSockErr if socket call on non blocking socket fails because the call would block
*/
#define OSA_SOCK_WOULD_BLOCK    WSAEWOULDBLOCK

/**
    error type returned by OSA_GetLastSockErr if socket connection was reset
*/
#define OSA_CONN_RESET          WSAECONNRESET

/**
    error type returned by OSA_GetLastSockErr if there is socket system failure
*/
#define OSA_SOCK_ERR            SOCKET_ERROR

/**
    defines invalid value for the socket
*/
#define OSA_INVALID_SOCK        INVALID_SOCKET

/**
    defines that the socket should be shutdown such that no futher reads are possible.
    does not affect the writes.
*/
#define OSA_STOP_SOCK_READ      SD_RECV

/**
    defines that the socket should be shutdown such that no futher writes are possible.
    does not affect the reads.
*/
#define OSA_STOP_SOCK_WRITE     SD_SEND

/**
    defines that the socket should be shutdown such that no futher writes or reads are possible.
*/
#define OSA_STOP_SOCK_BOTH      SD_BOTH

typedef struct hostent          OSA_HOSTENT;

/*
    structure which defines a socket. This is filled in call \ref OSA_OpenSocket
*/
typedef SOCKET                  OSA_SOCKET;

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
#define OSA_OpenSocket( sA , iB , iC , iD ) ( OSA_INVALID_SOCK != ( sA = socket( iB , iC , iD ) ) )

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
#define OSA_CloseSocket( sA )               ( 0 != closesocket( sA ) )

/// \brief
///     This macro is used to set a socket into non blocking mode.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    socket sA of type \ref OSA_SOCKET
///     <br>[in]    pointer pB should point to ulong element with non-zero value
/// \returns
///     TRUE on success and FALSE on failure.
#define OSA_NBSocket( sA , pB )             ( 0 != ioctlsocket( sA , FIONBIO , pB ) )

/// \brief
///     This macro is used get the last socket error. Not valid is no error has occured.
#define OSA_GetLastSockErr()                WSAGetLastError()
/*@}*/
/* - Socket OSA */

/* + Timer OSA */
/*! \addtogroup Windows_Timers
 *      \details
 */
/*@{*/
/// \brief
///     This structure defines the handle to an OS Timer.
typedef HANDLE  t_OsaTimerHandle;

typedef WAITORTIMERCALLBACK t_OsaTimerCallback;

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
                                            ( 0 != CreateTimerQueueTimer( &hHandle , NULL ,\
                                            (WAITORTIMERCALLBACK)cCallback, pParam , \
                                            uExpiry, 0, 0) )
/// \brief
///     This macro is used to stop an OS Timer.
/// \details
///     The input parameters to the macro is :
///     <br>[in]    handle hHandle of type \ref t_OsaTimerHandle
/// \returns
///     TRUE on success and FALSE on failure. Call OSA_GetLastError to get further error information                                            
#define OSA_StopTimer( hHandle )            ( 0 != DeleteTimerQueueTimer(NULL,hHandle,NULL) )
/*@}*/
/* - Timer OSA */

/* + File Handling */
/*! \addtogroup Windows_Files
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
/// \returns
///     On success t_OsaFile* pointer. On failure OSA_INVALID_FILE_HANDLE
#define OSA_FileOpen(FileName, Mode)        fopen((const char*)FileName, (const char*)Mode)

/// \brief
///     This macro is used to release file handle associated with a file.
///     Also flushes all unwritten data from the buffers to the file
/// \details
///     The input parameters to the macro is : 
///     <br>[in]    file handle of type \ref t_OsaFile
/// \returns
///     On success returns 0. On failure call OSA_GetLastError to get error reason
#define OSA_FileClose(FileHandler)          fclose(FileHandler)

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
///     <br>[in]    file handle of type \ref t_OsaFile
/// \returns
///     On success returns bytes written. On failure (-1) call OSA_GetLastError to get error reason
#define OSA_FileWrite(DataPointer, NoOfStructs, Size, FileHandler) \
                                            fwrite( DataPointer, NoOfStructs, Size, FileHandler )

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
///     <br>[in]    file handle of type \ref t_OsaFile
/// \returns
///     On success returns bytes read. On failure (-1) call OSA_GetLastError to get error reason
#define OSA_FileRead(DataPointer, NoOfStructs, Size, FileHandler) \
                                            fread( DataPointer, NoOfStructs, Size, FileHandler )

/// \brief
///    flushes all unwritten data from the buffers to the file
/// \details
///     The input parameters to the macro is : <br>
///     <br>[in]    file handle of type \ref t_OsaFile
/// \returns
///     On success returns 0. On failure call OSA_GetLastError to get error reason
#define OSA_FileFlush(FileHandler)          fflush(FileHandler)

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
#define OSA_Malloc( SIZE )                  malloc( (size_t)SIZE )

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
/*! \addtogroup Windows_Misc
 *      \details
 */
/*@{*/
/// \brief
///     This macro is used get the last error. Not valid is no error has occured.
#define OSA_GetLastError()                  GetLastError()

/// \brief
///     This macro is used get the current OS millisecond tick count. Resolution varies
#define OSA_GetMsCount()                    GetTickCount()

/// \brief
///     This macro is used put the thread to sleep for mA milliseconds
#define OSA_Sleep(mA)                       Sleep(mA)

#define OSA_Print( X )                printf X
/*@}*/
/* - Misc OSA */

/* + OS Specific Implementations */
#define vsnprintf                           _vsnprintf
#define snprintf                            _snprintf
/* - OS Specific Implementations */

#endif /* AGPS_WIN_OSA_H */

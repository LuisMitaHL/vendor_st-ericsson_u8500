/* modified version from newlib documentation */
#include <sys/stat.h>
#include <sys/times.h>
#include <errno.h>
#include "t_basicdefinitions.h"

#include "r_debug.h"
#include "r_debug_macro.h"
#include "r_main.h"

#undef errno
extern int errno;

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

void _exit();
int _close(int file);
int _execve(char *name, char **argv, char **env);
int _fork(void);
int _fstat(int file, struct stat *st);
int _getpid(void);
int _isatty(int file);
int _kill(int pid, int sig);
int _link(char *old, char *new);
int _lseek(int file, int ptr, int dir);
int _open(const char *name, int flags, int mode);
int _read(int file, char *ptr, int len);
caddr_t _sbrk(int incr);
int _stat(char *file, struct stat *st);
int _times(struct tms *buf);
int _unlink(char *name);
int _wait(int *status);
int _write(int file, char *ptr, int len);

char *__env[1] = { 0 };
char **environ = __env;

/* recommendation in documentation is to
   not use the exit call in systems where
   it makes no sense, but hcl code calls
     exit */
void _exit()
{
    A_(printf("Exit reached!\n");)

    StallExecution();
}

int _close(int file)
{
    return -1;
}

int _execve(char *name, char **argv, char **env)
{
    errno = ENOMEM;
    return -1;
}

int _fork(void)
{
    errno = EAGAIN;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _getpid(void)
{
    return 1;
}

int _isatty(int file)
{
    return 1;
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}

int _link(char *old, char *new)
{
    errno = EMLINK;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _open(const char *name, int flags, int mode)
{
    return -1;
}

int _read(int file, char *ptr, int len)
{
    /*        if(STDIN_FILENO==file)
        {
            static debuguartinitialized=0;
            uint32 Chars_Received = 0;
            uint32 Receive_Status = 0;
            t_uart_it_communication_status xX_Status;

            if(0==debuguartinitialized)
            {
                UART2_Init();
                debuguartinitialized=1;
            }
            UART_ReceivexCharacters( UART_DEVICE_ID_2, len, ptr, &Chars_Received, &Receive_Status);

            do
            {
                (void)UART_GetComStatus(UART_DEVICE_ID_2, &xX_Status);
            }while( xX_Status.tx_comm_status == UART_TRANSMIT_ON_GOING);

            return Chars_Received;
        }*/

    return 0;
}

caddr_t _sbrk(int incr)
{
    extern uint32 _heap_start_addr;/* Defined by the linker */
    extern uint32 _heap_stop_addr;/* Defined by the linker */
    static uint32 *heap_end = 0;
    uint32 *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_heap_start_addr;

        if (heap_end > &_heap_stop_addr) {
            A_(printf("Illegal values for heap start and heap end!!\n");)
            StallExecution();
        }
    }

    prev_heap_end = heap_end;

    if ((heap_end + incr) > &_heap_stop_addr) {
        A_(printf("Heap memory not available\n");)
        errno = ENOMEM;
        return ((caddr_t) - 1);
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

int _stat(char *file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _times(struct tms *buf)
{
    return -1;
}

int _unlink(char *name)
{
    errno = ENOENT;
    return -1;
}

int _wait(int *status)
{
    errno = ECHILD;
    return -1;
}

int _write(int file, char *ptr, int len)
{
    B_(

        //  printf("****This is called thru regular printf and not the debug subsystem\n");
    for (int done = 0; done < len; done++) {
    char output = *(ptr + done);
        printf("%c", output);
    })
    return len;
}


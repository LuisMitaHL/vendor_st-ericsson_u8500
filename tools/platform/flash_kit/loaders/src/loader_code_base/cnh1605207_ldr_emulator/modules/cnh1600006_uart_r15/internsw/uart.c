/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/
/**
 * \addtogroup ldr_emulator
 * @{
 *      \addtogroup UART
 *      \brief Emulated UART functions for R15 emulator
 *      @{
 */
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif //WIN32


#define ASYNCTIMEOUT    -2
#define ASYNCERROR      -1
#define PTDBUFFERSIZE   4096
#define TRUE            1
#define FALSE           0
#define MIN(A,B) ((A) < (B) ? (A) : (B))

BYTE    _ptd_read_buffer[PTDBUFFERSIZE];
BYTE    _ptd_write_buffer[PTDBUFFERSIZE];
DWORD   _ptd_read_head;
DWORD   _ptd_read_tail;
DWORD   _ptd_write_head;
DWORD   _ptd_write_tail;

//BOOL bReadIsInvoked;
//BOOL bWriteIsInvoked;

OVERLAPPED ro;
OVERLAPPED wo;

DWORD dwTransfer;


uint32 bReadIsInvoked = 0;
uint32 bWriteIsInvoked = 0;




HANDLE WinOpenComm(char *strComPort)
{
    HANDLE hComm = NULL;
    COMMTIMEOUTS CommTimeouts;

    hComm = CreateFile(strComPort,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL
                      );

    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    SetupComm(hComm, 8192, 8192);

    GetCommTimeouts(hComm, &CommTimeouts);

    CommTimeouts.ReadIntervalTimeout = 10;
    CommTimeouts.ReadTotalTimeoutMultiplier = 10;
    CommTimeouts.ReadTotalTimeoutConstant = 100;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 5000;
    SetCommTimeouts(hComm, &CommTimeouts);
    SetCommMask(hComm, EV_RLSD | EV_ERR);

    return hComm;
}

void WinSetPortState(HANDLE hComm, uint32 BaudRate, uint32 ByteSize, uint32 Parity, uint32 StopBits)
{
    DCB dcb;

    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE) {
        return;
    }

    GetCommState(hComm, &dcb);
    dcb.BaudRate = BaudRate;
    dcb.ByteSize = (BYTE)ByteSize;
    dcb.Parity = (BYTE)Parity;
    dcb.StopBits = (BYTE)StopBits;

    SetCommState(hComm, &dcb);
}

uint32 WinReadComm(HANDLE hComm, char *inBuff, uint32 len)
{
    DWORD bytesRead;
    BOOL bRes;

    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE) {
        return -1;
    }

    bRes = ReadFile(hComm, (LPVOID)inBuff, len, &bytesRead, NULL);

    if (!bRes) {
        return -1;
    } else {
        return (uint32)bytesRead;
    }
}

uint32 WinWriteComm(HANDLE hComm, char *outBuff, uint32 len)
{
    DWORD bytesWritten;
    BOOL bRes;

    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE) {
        return -1;
    }

    bRes = WriteFile(hComm, (LPVOID)outBuff, len, &bytesWritten, NULL);

    if (!bRes) {
        return -1;
    } else {
        return (uint32)bytesWritten;
    }
}

void WinCloseCommPort(HANDLE hComm)
{
    if (hComm != NULL && hComm != INVALID_HANDLE_VALUE) {
        CloseHandle(hComm);
    }
}


uint32 AsyncRead(HANDLE hComm, BYTE *buff, uint32 len, DWORD dwTimeout)
{
    uint32 toCopy;
    BOOL bReadRes;

    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE) {
        return ASYNCERROR;
    }


    if (!bReadIsInvoked) {
        DWORD dwBytesRead;


        if (_ptd_read_head != _ptd_read_tail) {
            toCopy = MIN(len, _ptd_read_head - _ptd_read_tail);
            memcpy(buff, _ptd_read_buffer + _ptd_read_tail, toCopy);
            _ptd_read_tail += toCopy;

            if (_ptd_read_head == _ptd_read_tail) {
                _ptd_read_head = _ptd_read_tail = 0;
            }

            return toCopy;
        }


        bReadIsInvoked = TRUE;
        bReadRes = ReadFile(hComm, _ptd_read_buffer + _ptd_read_head, PTDBUFFERSIZE - _ptd_read_head - 1, &dwBytesRead, &ro);

        if (!bReadRes && GetLastError() != ERROR_IO_PENDING) {
            return ASYNCERROR;
        }
    }

    if (bReadIsInvoked) {
        DWORD WaitRes;

        switch (WaitRes = WaitForSingleObject(ro.hEvent, dwTimeout)) {
        case WAIT_OBJECT_0:
            bReadIsInvoked = FALSE;

            if (!GetOverlappedResult(hComm, &ro, &dwTransfer, FALSE)) {
                return ASYNCERROR;
            }

            _ptd_read_head += dwTransfer;

            toCopy = MIN(len, _ptd_read_head - _ptd_read_tail);
            memcpy(buff, _ptd_read_buffer + _ptd_read_tail, toCopy);
            _ptd_read_tail += toCopy;

            if (_ptd_read_head == _ptd_read_tail) {
                _ptd_read_head = _ptd_read_tail = 0;
            }

            return toCopy;

            break;

        case WAIT_TIMEOUT:
            return ASYNCTIMEOUT;
            break;

        default:
            return ASYNCERROR;

        }
    }

    return 0;
}

HANDLE AsyncOpen(char *PortName)
{
    HANDLE hComm = NULL;
    COMMTIMEOUTS CommTimeouts;

    hComm = CreateFile(PortName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       FILE_FLAG_OVERLAPPED,
                       NULL
                      );

    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    SetupComm(hComm, 8192, 8192);

    /*
    GetCommState(hComm, &dcb);

    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    ::SetCommState(hComm, &dcb);
    */


    GetCommTimeouts(hComm, &CommTimeouts);

    CommTimeouts.ReadIntervalTimeout = 10;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 5000;

    SetCommTimeouts(hComm, &CommTimeouts);

    SetCommMask(hComm, EV_RLSD | EV_ERR);

    bReadIsInvoked = FALSE;
    bWriteIsInvoked = FALSE;

    ro.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    ro.Internal = ro.InternalHigh = ro.Offset = ro.OffsetHigh = 0;

    wo.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    wo.Internal = wo.InternalHigh = wo.Offset = wo.OffsetHigh = 0;

    _ptd_read_head = 0;
    _ptd_read_tail = 0;
    _ptd_write_head = 0;
    _ptd_write_tail = 0;

    return hComm;
}

uint32  AsyncWrite(HANDLE hComm, BYTE *buff, uint32 len, DWORD dwTimeout)
{
    BOOL bRes;

    if (hComm == NULL || hComm == INVALID_HANDLE_VALUE) {
        return ASYNCERROR;
    }

    if (!bWriteIsInvoked) {
        DWORD dwBytesWritten;
        bWriteIsInvoked = TRUE;

        bRes = WriteFile(hComm, buff, len, &dwBytesWritten, &wo);

        if (!bRes && GetLastError() != ERROR_IO_PENDING) {
            return ASYNCERROR;
        }
    }

    if (bWriteIsInvoked) {
        DWORD dwWaitRes;


        switch (dwWaitRes = WaitForSingleObject(wo.hEvent, dwTimeout)) {
        case WAIT_OBJECT_0:
            bWriteIsInvoked = FALSE;

            if (!GetOverlappedResult(hComm, &wo, &dwTransfer, FALSE)) {
                return ASYNCERROR;
            }

            return dwTransfer;
            break;

        case WAIT_TIMEOUT:
            return ASYNCTIMEOUT;
            break;

        default:
            return ASYNCERROR;
        }
    }

    return bRes;
}


/**     @}*/
/**@}*/
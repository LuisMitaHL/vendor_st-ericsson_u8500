/***********************************************************************
 * $Copyright Ericsson AB 2008 $
 **********************************************************************/
#ifndef UART_H_
#define UART_H_

//#include "error_codes.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif //WIN32

#include "t_basicdefinitions.h"
//#include "r_inputoutput.h"
//#include "asic_uarts_map.h"




#define BAUDRATE_9600    0
#define BAUDRATE_19200   1
#define BAUDRATE_38400   2
#define BAUDRATE_57600   3
#define BAUDRATE_115200  4
#define BAUDRATE_230400  5
#define BAUDRATE_460800  6
#define BAUDRATE_921600  7
#define BAUDRATE_1843200 8

#define IRQ_MAX_ARRAY_SIZE 2048


// Functions
//extern void PutChar32(void *_word_p);
//extern void PutChar(uint8 Ch);  //should be defined in stdio.h in lower case
//extern void   PutBTChar(uint8 Ch);
//extern uint8  GetChar(void);    //should be defined in stdio.h in lower case
//extern uint8  GetBTChar(void);
//extern void       Print(uint8 *Pmessage);
//extern void       UART_Init(void);
//extern void       UART3_Init(void);
//uint8         ChangeBaudrate(uint8 Baudrate);
//uint8         ChangeBaudrate3(uint8 Baudrate);
//extern void       SetupUartInterrupt(void);
//extern void    SetupUartInterrupt_Streaming(void); //Only used by production loader
//extern void       EnableUartInterrupt(void);
//extern void       DisableUartInterrupt(void);
//extern void       UART_Init2(uint32);
//extern uint32 UART_GetCurrentBaudrate(void);

HANDLE OpenComm(char *strComPort);
void WinSetPortState(HANDLE hComm, uint32 BaudRate, uint32 ByteSize, uint32 Parity, uint32 StopBits);
uint32 WinReadComm(HANDLE hComm, char *inBuff, uint32 len);
uint32 WinWriteComm(HANDLE hComm, char *outBuff, uint32 len);
void WinCloseCommPort(HANDLE hComm);
uint32 AsyncRead(HANDLE hComm, BYTE *buff, uint32 len, DWORD dwTimeout);
HANDLE AsyncOpen(char *PortName);
uint32  AsyncWrite(HANDLE hComm, BYTE *buff, uint32 len, DWORD dwTimeout);

#endif /*UART_H_*/

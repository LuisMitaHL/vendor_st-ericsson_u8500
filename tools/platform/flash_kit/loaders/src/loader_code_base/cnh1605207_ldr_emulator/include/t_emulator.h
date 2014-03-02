#ifndef _INCLUSION_GUARD_T_EMULATOR_H_
#define _INCLUSION_GUARD_T_EMULATOR_H_
/***********************************************************************
 * $Copyright Ericsson AB 2009 $
 **********************************************************************/

/**
 * \addtogroup ldr_emulator
 * @{
 */

/* Default values for all UARTs. */
//#define HL1UART_RX_FIFO_LEVEL (UART_IFLS_RX_LEVEL_16_CHAR)   // RX FIFO level set to 16 bytes
//#define HL1UART_TX_FIFO_LEVEL (UART_IFLS_TX_LEVEL_2_CHAR)    // TX FIFO level set to 2 bytes
#define HL1UART_RX_FIFO_LEVEL 1024   // RX FIFO level set to 1024 bytes
#define HL1UART_TX_FIFO_LEVEL 1024   // TX FIFO level set to 1024 bytes
#define HL1UART_BAUD_RATE     (HL1UART_BAUDRATE_9600)        // Baud rate set to 9600 bauds
/** MAX_LINE_LENGTH - is the maximum number of characters that can be read from a file */
#define MAX_LINE_LENGTH   256
/** MAX_FILE_NAME - is the maximum length of a file name */
#define MAX_FILE_NAME     256
/** MAX_NR_ARGUMENTS - is the maximum number of arguments that one line can have */
#define MAX_NR_ARGUMENTS  20
/* This is the maximum size for the transmit buffer*/
#define MAX_SIZE_OF_TRANSMIT_BUFFER HL1UART_TX_FIFO_LEVEL

/**
 *    This is a structure that is used for passing one line from a file
 * it contains the file name, the number of the line; needed for
 * printing at which line the error occured, the data from the line,
 * the number of arguments in the line and array of pointers to the arguments.
 */
typedef struct {
    /** FileName - this is the name of a file */
    sint8   FileName[MAX_FILE_NAME];
    /** LineNr - is the number of the line */
    uint32  LineNr;
    /** LineData - is the actual data from one line from a file */
    sint8   LineData[MAX_LINE_LENGTH];
    /** ArgNr - is the number of arguments in the line */
    uint32  ArgNr;
    /**
     * Arg_p - is an array of pointers to arguments,
     * the pointers point, in the line, at the begining of the argument
     **/
    sint8  *Arg_p[MAX_NR_ARGUMENTS];
} Line_t;

/**
 * This is array of separators used for parsing arguments with strtok_s.
 */
static const sint8 *const Separators = {" \v\f\n\t\r"};

/*@}*/
#endif // _INCLUSION_GUARD_T_EMULATOR_H_

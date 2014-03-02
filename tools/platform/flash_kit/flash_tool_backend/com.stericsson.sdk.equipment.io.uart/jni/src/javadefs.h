/*
 * javadefs.h
 *
 *  Created on: Dec 9, 2009
 *      Author: emicroh
 */

#ifndef JAVADEFS_H_
#define JAVADEFS_H_

#define EX_UART_EXCEPTION "com/stericsson/sdk/equipment/io/uart/internal/UARTException"
#define EX_ARRAY_INDEX_OUT_OF_BOUND "java/lang/ArrayIndexOutOfBoundsException"

#define CLASS_SERIAL_PORT_HANDLER "com/stericsson/sdk/equipment/io/uart/internal/SerialPortHandler"
#define CLASS_SERIAL_PORT "com/stericsson/sdk/equipment/io/uart/internal/SerialPort"
#define CLASS_SP_CONFIGURATION "com/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration"

#define CLASS_SPC_BAUD_RATE "com/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$BAUD_RATE"
#define CLASS_SPC_DATA_BITS "com/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$DATA_BITS"
#define CLASS_SPC_PARITY "com/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$PARITY"
#define CLASS_SPC_STOP_BITS "com/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$STOP_BITS"
#define CLASS_SPC_ON_OFF "com/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$ON_OFF"
#define CLASS_SPC_DTR_CIRCUIT "com/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$DTR_CIRCUIT"
#define CLASS_SPC_RTS_CIRCUIT "com/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$RTS_CIRCUIT"

#define FN_SPC_BAUD_RATE "baudRate"
#define FN_SPC_BAUD_RATE_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$BAUD_RATE;"
#define FN_SPC_DATA_BITS "dataBits"
#define FN_SPC_DATA_BITS_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$DATA_BITS;"
#define FN_SPC_PARITY	 "parity"
#define FN_SPC_PARITY_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$PARITY;"
#define FN_SPC_STOP_BITS "stopBits"
#define FN_SPC_STOP_BITS_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$STOP_BITS;"
#define FN_SPC_TIMEOUT "timeout"
#define FN_SPC_TIMEOUT_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$ON_OFF;"
#define FN_SPC_XONOFF "xOnOff"
#define FN_SPC_XONOFF_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$ON_OFF;"
#define FN_SPC_DSR_HANDSHAKE "dsrHandshake"
#define FN_SPC_DSR_HANDSHAKE_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$ON_OFF;"
#define FN_SPC_CTS_HANDSHAKE "ctsHandshake"
#define FN_SPC_CTS_HANDSHAKE_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$ON_OFF;"
#define FN_SPC_DTR_CIRCUIT "dtr"
#define FN_SPC_DTR_CIRCUIT_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$DTR_CIRCUIT;"
#define FN_SPC_RTS_CIRCUIT "rts"
#define FN_SPC_RTS_CIRCUIT_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$RTS_CIRCUIT;"
#define FN_SPC_DSR "dsr"
#define FN_SPC_DSR_SIG "Lcom/stericsson/sdk/equipment/io/uart/internal/SerialPortConfiguration$ON_OFF;"
#define METHOD_SPC_BAUD_RATE "getBaudRate"
#define METHOD_SPC_BAUD_RATE_SIG "()I"
#define METHOD_SPC_DATA_BITS "getDataBits"
#define METHOD_SPC_DATA_BITS_SIG "()I"
#define METHOD_SPC_ENUM_ORDINAL "ordinal"
#define METHOD_SPC_ENUM_ORDINAL_SIG "()I"

#define METHOD_CONSTRUCTOR "<init>"
#define METHOD_SP_CONSTRUCTOR_SIG "(Ljava/lang/String;)V"

#define METHOD_SP_GET_IDENTIFIER "getIdentifier"
#define METHOD_SP_GET_IDENTIFIER_SIG "()Ljava/lang/String;"

#endif /* JAVADEFS_H_ */

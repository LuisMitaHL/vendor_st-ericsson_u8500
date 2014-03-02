/*
 * COMPort.h
 *
 *  Created on: Dec 1, 2009
 *      Author: emicroh
 */

#ifndef COMPORT_H_
#define COMPORT_H_

#include "util.h"
#include <string>

#ifdef __WIN__
	#define MAX_SCAN_PORTS 99
#else
	//OS specific includes
	#include <dirent.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>
#endif

// define how long to wait for any data before a read operation times out (in miliseconds)
// because this is set in deci seconds in linux, 100ms is minimum value
#define ONE_READ_TIMEOUT 300

enum uart_error {
	UNEXPECTED_ERROR = -1,
	SUCCESS = 0,
	INVALID_PARAMETER,
	PORT_OPEN_FAILED,
	CLOSING_PORT_FAILED,
	BUILDING_DCB_STRUCTURE_FAILED,
	CONFIGURING_DEVICE_FAILED,
	SETTING_COMM_TIMEOUTS_FAILED,
	GET_COMM_STATE_FAILED,
	READING_FAILED,
	READING_TIMEOUT,
	WRITING_FAILED,
	WRITING_TIMEOUT,
	PORT_IS_NOT_OPENED,
	IOCTL_FAILED,
	TERMINAL_SETTING_FAILED,
	UNSUPPORTED_PARAMETER,
	FLUSH_FAILED
};

enum BAUD_RATE {
	BR_110     =     110,
	BR_300     =     300,
	BR_600     =     600,
	BR_1200    =    1200,
	BR_2400    =    2400,
	BR_4800    =    4800,
	BR_9600	   =    9600,
	BR_14400   =   14400,
	BR_19200   =   19200,
	BR_38400   =   38400,
	BR_56000   =   56000,
    BR_57600   =   57600,
    BR_115200  =  115200,
    BR_128000  =  128000,
    BR_230400  =  230400,
    BR_256000  =  256000,
    BR_460800  =  460800,
    BR_921600  =  921600,
    BR_1152000 = 1152000
};

enum DATA_BITS {
	DATA_BITS_5 = 5,
	DATA_BITS_6,
	DATA_BITS_7,
	DATA_BITS_8
};

enum PARITY {
	PAR_NONE = 0,
    PAR_EVEN,
    PAR_ODD,
    PAR_MARK,
    PAR_SPACE
};

enum STOP_BITS {
    STOP_BITS_1 = 0,
    STOP_BITS_1_5,
    STOP_BITS_2,
};

enum ON_OFF {
	DEFAULT = 0,
	ON,
	OFF
};

enum DTR_CIRCUIT {
	DTR_DEFAULT = 0,
	DTR_ON,
	DTR_OFF,
	DTR_HS
};

enum RTS_CIRCUIT {
	RTS_DEFAULT = 0,
	RTS_ON,
	RTS_OFF,
	RTS_HS,
	RTS_TG
};

struct uart_config {
	BAUD_RATE baudRate;
	DATA_BITS dataBits;
	PARITY parity;
	STOP_BITS stopBits;
	ON_OFF timeout;
	ON_OFF xOnOff;
	ON_OFF dsr_handshake;
	ON_OFF cts_handshake;
	DTR_CIRCUIT dtr;
	RTS_CIRCUIT rts;
	ON_OFF dsr;

	uart_config() :
		baudRate(BR_9600),
		dataBits(DATA_BITS_8),
		parity(PAR_NONE),
		stopBits(STOP_BITS_1),
		timeout(DEFAULT),
		xOnOff(DEFAULT),
		dsr_handshake(DEFAULT),
		cts_handshake(DEFAULT),
		dtr(DTR_DEFAULT),
		rts(RTS_DEFAULT),
		dsr(DEFAULT) {}
};


//OS specific function commonly used in COMPort implementation
uart_error openPort(LPCTSTR name, HANDLE * port_h);
void closePort(HANDLE * port_h);
uart_error scanPorts(std::list<std::string> & scanPorts);
uart_error setPort(HANDLE * port_h, const uart_config * conf);
uart_error portRead(HANDLE * port_h, PTCHAR buf, int offset, int length, int * read);
uart_error portWrite(HANDLE * port_h, LPCTSTR buf, int offset, int length, int * written);
uart_error setReadTimeout(HANDLE * port_h, DWORD msTimeout);
void handle_uart_error(uart_error err);
void msleep(int ms);

//Core class definition for serial port
class COMPort {
public:

	/**
	 * Contructs COMPort object.
	 *
	 * @param name name of the port, usually /dev/ttySx or /dev/ttyUSBx on Linux, COMx on Windows.
	 */
	COMPort(const std::string& name_);

	/**
	 * Destructor for COMPort object.
	 */
	virtual ~COMPort();

	/**
	 * Opens com port.
	 */
	void open();

	/**
	 * Sets new com port configuration (speed, parity and etc.).
	 */
	void set(uart_config conf);

	/**
	 * Reads data from com port.
	 * @param buf buffer where data are stored
	 * @param offset offset in the buffer where data should be stored
	 * @param length length of data that should be read from com port (and stored to buf)
	 */
	int read(PTCHAR buf, int offset, int length);

	/**
	 * Writes data to com port.
	 * @param buf buffer where data are stored
	 * @param offset offset in the buffer from which data should be taken
	 * @param length amount of data to send to com port
	 */
	int write(LPCTSTR buf, int offset, int length);

	/**
	 * Checks if port is opened.
	 *
	 * @return true if port is opened, false otherwise.
	 */
	bool isOpened();

	/**
	 * Closes com port.
	 */
	void close();

	/**
	 * Gets com port name.
	 *
	 * @return name
	 */
	std::string getName() const {
        return name;
    }

	/**
	 * Flushes UART port.
	 * On Windows this method hangs till all the transfers are complete.
	 * On Linux this method flushes pending transfers (drops UART buffer) and returns
	 * immediately.
	 *
	 * @return 0 on success
	 */
	int flush();

private:
	const std::string name;
	HANDLE handle;
	uart_config config;
};

#endif /* COMPORT_H_ */

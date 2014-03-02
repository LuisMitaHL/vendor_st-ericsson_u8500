/*
 * COMPort.cpp
 *
 *  Created on: Dec 1, 2009
 *      Author: emicroh
 */
#include <iostream>
#include <sstream>
#include "COMPort.h"
#include "Logger.h"

using namespace std;

#ifdef __WIN__

/**
 * This function enumerates available UART ports on Windows.
 *
 * @return 0 on success, error code otherwise
 */
uart_error scanPorts(list<string> &ports) {
	string port_prefix = string("COM");
	// for some strange reason ports above 9 have to be opened with the prefix "\\.\"
	string port_system_prefix = string("\\\\.\\") + port_prefix;
	ostringstream oss_port;

	HANDLE port_h = INVALID_HANDLE_VALUE;

	ports.clear();

	for (int i = 0; i <= MAX_SCAN_PORTS; i++) {
		oss_port.str("");
		oss_port << port_system_prefix.c_str() << i;
		if (openPort(oss_port.str().c_str(), &port_h) == SUCCESS) {
			oss_port.str("");
			oss_port << port_prefix.c_str() << i;
			ports.push_back(oss_port.str());
			closePort(&port_h);
		}
	}
	return SUCCESS;
}

uart_error openPort(LPCTSTR name, HANDLE * port_h) {
	// for some strange reason ports above 9 have to be opened with the prefix "\\.\"
	string port_prefix = string("\\\\.\\");
	ostringstream oss_port;
	oss_port << port_prefix.c_str() << name;

	*port_h = CreateFile(oss_port.str().c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (*port_h == INVALID_HANDLE_VALUE) {
		return PORT_OPEN_FAILED;
	} else {
		return SUCCESS;
	}
}

void closePort(HANDLE * port_h) {
	if (*port_h != INVALID_HANDLE_VALUE) {
		CloseHandle(*port_h);
		*port_h = INVALID_HANDLE_VALUE;
	}
}

uart_error setReadTimeout(HANDLE * port_h, DWORD msTimeout) {
	COMMTIMEOUTS ct;
	GetCommTimeouts(*port_h, &ct);
	ct.ReadIntervalTimeout = 0;
	ct.ReadTotalTimeoutMultiplier = 0;
	ct.ReadTotalTimeoutConstant = msTimeout;
	if (!SetCommTimeouts(*port_h, &ct)) {
		return SETTING_COMM_TIMEOUTS_FAILED;
	}
	return SUCCESS;
}

void msleep(int ms) {
	Sleep(ms);
}

uart_error getConfigurationString(const uart_config * conf, PTCHAR confStr) {
	int ret = 0;
	//add baudrate
	ret += _stprintf(confStr + ret * sizeof(TCHAR), "baud=%d", conf->baudRate);
	//add parity
	ret += _stprintf(confStr + ret * sizeof(TCHAR), " parity=");
	switch (conf->parity) {
		case PAR_NONE:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), "n");
		break;
		case PAR_EVEN:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), "e");
		break;
		case PAR_ODD:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), "o");
		break;
		case PAR_MARK:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), "m");
		break;
		case PAR_SPACE:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), "s");
		break;
		default:
		return INVALID_PARAMETER;
	}
	//add data bits
	ret += _stprintf(confStr + ret * sizeof(TCHAR), " data=%d", conf->dataBits);

	//add stop bits
	ret += _stprintf(confStr + ret * sizeof(TCHAR), " stop=");
	switch(conf->stopBits) {
		case STOP_BITS_1:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), "1");
		break;
		case STOP_BITS_1_5:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), "1.5");
		break;
		case STOP_BITS_2:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), "2");
		break;
		default:
		return INVALID_PARAMETER;
	}

	//add timeout handling
	switch (conf->timeout) {
		case ON:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " to=on");
		break;
		case OFF:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " to=off");
		break;
		default:
		break;
	}

	switch (conf->xOnOff) {
		case ON:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " xon=on");
		break;
		case OFF:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " xon=on");
		break;
		default:
		break;
	}

	switch (conf->dsr_handshake) {
		case ON:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " odsr=on");
		break;
		case OFF:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " odsr=off");
		break;
		default:
		break;
	}

	switch (conf->cts_handshake) {
		case ON:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " octs=on");
		break;
		case OFF:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " octs=off");
		break;
		default:
		break;
	}

	switch (conf->dtr) {
		case DTR_ON:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " dtr=on");
		break;
		case DTR_OFF:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " dtr=off");
		break;
		case DTR_HS:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " dtr=hs");
		break;
		default:
		break;
	}

	switch (conf->rts) {
		case RTS_ON:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " rts=on");
		break;
		case RTS_OFF:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " rts=off");
		break;
		case RTS_HS:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " rts=hs");
		break;
		case RTS_TG:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " rts=tg");
		break;
		default:
		break;
	}

	switch (conf->dsr) {
		case ON:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " idsr=on");
		break;
		case OFF:
		ret += _stprintf(confStr + ret * sizeof(TCHAR), " idsr=off");
		break;
		default:
		break;
	}

	return SUCCESS;
}

uart_error setPort(HANDLE * port_h, const uart_config * conf) {
	PTCHAR configurationString = new TCHAR[1024];
	FillMemory(configurationString, 1024 * sizeof(TCHAR), 0);

	uart_error err = SUCCESS;

	if ((err = getConfigurationString(conf, configurationString)) != SUCCESS) {
		goto ErrorExit;
	} else {
		Logger::getInstance()->debug(LP, string("Configuration string: ")+ configurationString);
	}

	DCB dcb;

	FillMemory(&dcb, sizeof(dcb), 0);
	dcb.DCBlength = sizeof(dcb);

	if (!BuildCommDCB(configurationString, &dcb)) {
		err = BUILDING_DCB_STRUCTURE_FAILED;
		goto ErrorExit;
	}

	if (!SetCommState(*port_h, &dcb)) {
		return CONFIGURING_DEVICE_FAILED;
	}

	delete configurationString;
	return SUCCESS;

	ErrorExit:
	if (configurationString) {
		delete configurationString;
	}
	return err;
}

uart_error portRead(HANDLE * port_h, PTCHAR buf, int offset, int length, int * read) {
	if (!buf) {
		Logger::getInstance()->error(LP, "Invalid pointer to character buffer!");
		return READING_FAILED;
	}

	DWORD r = 0;
	if (!ReadFile(*port_h, buf + offset * sizeof(TCHAR), length, &r, 0)) {
		return READING_FAILED;
	}
	//dump( (unsigned char *)buf + offset * sizeof(TCHAR), r, false);

	*read = r;
	if (*read < length) {
		return READING_TIMEOUT;
	} else {
		return SUCCESS;
	}
}

uart_error portWrite(HANDLE * port_h, LPCTSTR buf, int offset, int length, int * written) {
	if (!buf) {
		Logger::getInstance()->error(LP, "Invalid pointer to character buffer!");
		return WRITING_FAILED;
	}

	DWORD w;
	if (!WriteFile(*port_h, buf + offset * sizeof(TCHAR), length, &w, 0)) {
		return WRITING_FAILED;
	}

	//dump( (unsigned char *)buf + offset * sizeof(TCHAR), w, true);

	*written = w;
	if (*written < length) {
		return WRITING_TIMEOUT;
	} else {
		return SUCCESS;
	}
}

uart_error flushPort(HANDLE * port_h) {
	int result = FlushFileBuffers(*port_h);
	if(result)
	return SUCCESS;
	else
	return FLUSH_FAILED;
}

#else //Implementation for linux
uart_error openPort(LPCTSTR name, HANDLE * port_h) {
	*port_h = open(name, O_RDWR);
	if (*port_h == INVALID_HANDLE_VALUE) {
		return PORT_OPEN_FAILED;
	} else {
		return SUCCESS;
	}
}

void closePort(HANDLE * port_h) {
	if (*port_h != INVALID_HANDLE_VALUE) {
		close(*port_h);
		*port_h = INVALID_HANDLE_VALUE;
	}
}

uart_error setPort(HANDLE * port_h, const uart_config * conf) {
	termios tio;

	// remember timeout settings, we do not want to change it
	if (tcgetattr(*port_h, &tio))
		return IOCTL_FAILED;
	cc_t vmin = tio.c_cc[VMIN];
	cc_t vtime = tio.c_cc[VTIME];

	FillMemory(&tio, sizeof(tio), 0);

	cfsetspeed(&tio, conf->baudRate);

	switch (conf->dataBits) {
	case DATA_BITS_5:
		tio.c_cflag |= CS5;
		break;
	case DATA_BITS_6:
		tio.c_cflag |= CS6;
		break;
	case DATA_BITS_7:
		tio.c_cflag |= CS7;
		break;
	case DATA_BITS_8:
		tio.c_cflag |= CS8;
		break;
	}

	if (conf->parity != PAR_NONE) {
		switch (conf->parity) {
		case PAR_EVEN:
			tio.c_cflag |= PARENB;
			break;
		case PAR_ODD:
			tio.c_cflag |= PARENB | PARODD;
			break;
		default:
			break;
		}
	}

	switch (conf->stopBits) {
	case STOP_BITS_1:
		break;
	case STOP_BITS_1_5:
		return UNSUPPORTED_PARAMETER;
		break;
	case STOP_BITS_2:
		tio.c_cflag |= CSTOPB;
		break;
	}

	tio.c_cflag |= CREAD | CLOCAL;

	// restore original timeouts
	tio.c_cc[VMIN] = vmin;
	tio.c_cc[VTIME] = vtime;

	if (tcsetattr(*port_h, TCSANOW, &tio) < 0) {
		return TERMINAL_SETTING_FAILED;
	}

	return SUCCESS;
}

uart_error portRead(HANDLE * port_h, PTCHAR buf, int offset, int length,
		int * readChars) {

	ssize_t ret = read(*port_h, buf + offset * sizeof(TCHAR), length);

	if (ret < 0) {
		return READING_FAILED;
	}
	//dump( (unsigned char *)buf + offset * sizeof(TCHAR), ret, false);

	*readChars = ret;
	if (*readChars < length) {
		return READING_TIMEOUT;
	} else {
		return SUCCESS;
	}
}

uart_error portWrite(HANDLE * port_h, LPCTSTR buf, int offset, int length,
		int * written) {
	if (!buf) {
		Logger::getInstance()->error(LP, "Invalid pointer to character buffer!");
		return WRITING_FAILED;
	}

	ssize_t ret = write(*port_h, buf + offset * sizeof(TCHAR), length);
	if (ret < 0) {
		return WRITING_FAILED;
	}
	//dump( (unsigned char *)buf + offset * sizeof(TCHAR), ret, true);

	*written = ret;
	if (*written < length) {
		return WRITING_TIMEOUT;
	} else {
		return SUCCESS;
	}
}

uart_error flushPort(HANDLE * port_h) {
	int result = tcflush(*port_h, TCIOFLUSH);
	if (!result)
		return SUCCESS;
	else
		return FLUSH_FAILED;
}

/**
 * This method is use for enumeration Linux UART ports.
 *
 * @return port error status
 */
uart_error scanPorts(list<string> &serialPorts) {
	DIR *dp;
	struct dirent *dirp;
	string filename;
	string patternSerial("ttyS");
	string patternUsb("ttyUSB");
	const string devPath("/dev/");

	serialPorts.clear();

	if ((dp = opendir("/dev")) == NULL) {
		return UNEXPECTED_ERROR;
	}

	while ((dirp = readdir(dp)) != NULL) {
		filename = string(dirp->d_name);
		if ((filename.compare(0, patternSerial.size(), patternSerial) == 0)
				|| (filename.compare(0, patternUsb.size(), patternUsb) == 0)) {
			serialPorts.push_back(filename.insert(0, devPath));
		}
	}

	closedir(dp);

	return SUCCESS;
}

void msleep(int ms) {
	usleep(ms * 1000);
}

uart_error setReadTimeout(HANDLE *port_h, DWORD msTimeout) {
	termios tio;
	if (tcgetattr(*port_h, &tio))
		return IOCTL_FAILED;
	tio.c_cc[VTIME] = msTimeout / 100; // set in deciseconds
	tio.c_cc[VMIN] = 0;
	if (tcsetattr(*port_h, TCSANOW, &tio) < 0) {
		return TERMINAL_SETTING_FAILED;
	} else {
		return SUCCESS;
	}
}

#endif

void handle_uart_error(uart_error err, LPCTSTR initPoint) {
	LPCTSTR cause;

	switch (err) {
	case SUCCESS:
		return;
	case INVALID_PARAMETER:
		cause = "INVALID_PARAMETER";
		break;
	case PORT_OPEN_FAILED:
		cause = "PORT_OPEN_FAILED";
		break;
	case CLOSING_PORT_FAILED:
		cause = "CLOSING_PORT_FAILED";
		break;
	case BUILDING_DCB_STRUCTURE_FAILED:
		cause = "BUILDING_DCB_STRUCTURE_FAILED";
		break;
	case CONFIGURING_DEVICE_FAILED:
		cause = "CONFIGURING_DEVICE_FAILED";
		break;
	case SETTING_COMM_TIMEOUTS_FAILED:
		cause = "SETTING_COMM_TIMEOUTS_FAILED";
		break;
	case GET_COMM_STATE_FAILED:
		cause = "GET_COMM_STATE_FAILED";
		break;
	case READING_FAILED:
		cause = "READING_FAILED";
		break;
	case READING_TIMEOUT:
		cause = "READING_TIMEOUT";
		break;
	case WRITING_FAILED:
		cause = "WRITING_FAILED";
		break;
	case WRITING_TIMEOUT:
		cause = "WRITING_TIMEOUT";
		break;
	case PORT_IS_NOT_OPENED:
		cause = "PORT_IS_NOT_OPENED";
		break;
	case IOCTL_FAILED:
		cause = "IOCTL_FAILED";
		break;
	case TERMINAL_SETTING_FAILED:
		cause = "TERMINAL_SETTING_FAILED";
		break;
	case UNSUPPORTED_PARAMETER:
		cause = "UNSUPPORTED_PARAMETER";
		break;
	case UNEXPECTED_ERROR:
	default:
		cause = "UNEXPECTED_ERROR";
		break;
	}

	if (initPoint) {
		throw getMessage("%s: %s", initPoint, cause);
	} else {
		throw cause;
	}
}

COMPort::COMPort(const string & name_) :
		name(name_) {
	this->handle = INVALID_HANDLE_VALUE;
}

COMPort::~COMPort() {
	if (handle != INVALID_HANDLE_VALUE) {
		close();
	}
}

void COMPort::open() {
	handle_uart_error(openPort(name.c_str(), &handle),
			getMessage("Opening port '%s' failed", name.c_str()));
	handle_uart_error(setReadTimeout(&handle, ONE_READ_TIMEOUT),
			getMessage("Setting read timeout to '%dl'ms failed",
					ONE_READ_TIMEOUT));
}

int COMPort::read(PTCHAR buf, int offset, int length) {
	if (!isOpened()) {
		throw "PortIsNotOpenedException";
	} else {
		int read;
		int totalRead = 0;
		do {
			read = 0;
			uart_error err = portRead(&handle, buf, offset, length - totalRead,
					&read);

			if ((err != SUCCESS) && (err != READING_TIMEOUT)) {
				handle_uart_error(err, "Reading failed");
				return 0; //only to avoid warning, exception should be thrown
			}

			if (!read) { // timeout without any bytes read
				//cout << "------------------------------- READ TIMEOUT" << endl;
				return totalRead;
			}

			// got something
			totalRead += read;
			offset += read;
		} while (totalRead < length);

		return totalRead;
	}
}

int COMPort::write(LPCTSTR buf, int offset, int length) {
	if (!isOpened()) {
		throw "PortIsNotOpenedException";
	} else {
		int written = 0;
		uart_error err = portWrite(&handle, buf, offset, length, &written);
		if ((err != SUCCESS) && (err != WRITING_TIMEOUT)) {
			handle_uart_error(err, "Writing failed..");
			return 0; //only to avoid warning, exception should be thrown
		} else {
			//if(err == WRITING_TIMEOUT)
			//cout << "------------------------------- WRITE TIMEOUT" << endl;
			return written;
		}
	}
}

int COMPort::flush() {
	if (!isOpened()) {
		throw "PortIsNotOpenedException";
	}

	return flushPort(&handle);
}

void COMPort::close() {
	return closePort(&handle);
}

bool COMPort::isOpened() {
	return handle != INVALID_HANDLE_VALUE;
}

void COMPort::set(uart_config conf) {
	this->config = conf;
	handle_uart_error(setPort(&handle, &this->config),
			"Setting port configuration failed");
}


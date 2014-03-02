/*
 * test.cpp
 *
 *  Created on: Dec 1, 2009
 *      Author: emicroh
 */

#include "COMPort.h"
#include "Logger.h"
#define END_CHAR '~'

using namespace std;

void testOpenCloseOperation(LPCTSTR portName) {
	COMPort * port = new COMPort(portName);
	port->open();
	Logger::getInstance()->info("Port '%s' has been opened successfuly..",
			portName);
	port->close();
	Logger::getInstance()->info("Port '%s' has been closed successfuly..",
			portName);
	delete port;
}

void startReader(COMPort * port) {
	Logger::getInstance()->info(LP,
			"Reading characters from " + port->getName()
					+ " will end up by reading '" + END_CHAR + "'..");

	TCHAR ch = 0;

	_tprintf("\nRead characters > ");
	try {
		do {
			if (port->read(&ch, 0, 1) == 1) {
				_tprintf(" %d", ch);
			}
		} while (ch != END_CHAR);
	} catch (PTCHAR errMsg) {
		Logger::getInstance()->error(LP,"Caught an Exception with message: " + string(errMsg));
		goto ErrorExit;
	} catch (...) {
		Logger::getInstance()->error(LP, "Caught unexpected exception!");
		goto ErrorExit;
	}

	return;

	ErrorExit: Logger::getInstance()->error(LP, "Error exit!");
	return;
}

void startWriter(COMPort * port) {
	Logger::getInstance()->info(LP,
			"Writing characters to " + port->getName()
					+ "' will end up by sending '" + END_CHAR + "'..");

	TCHAR ch;

	_tprintf("\nCharacters to be sent > ");
	try {
		do {
			if (_tscanf("%1c", &ch) != 1) {
				Logger::getInstance()->error(LP,
						"Reading characters from STDIN failed!");
				goto ErrorExit;
			}

			if (port->write(&ch, 0, 1) == 1) {
				Logger::getInstance()->info(LP,
						"Character '" + string(1,ch) + "' has been written");
			}

		} while (ch != END_CHAR);
	} catch (PTCHAR errMsg) {
		Logger::getInstance()->error(LP,
				"Caught an Exception with message: " + string(errMsg));
		goto ErrorExit;
	} catch (...) {
		Logger::getInstance()->error(LP, "Caught unexpected exception!");
		goto ErrorExit;
	}

	return;

	ErrorExit: Logger::getInstance()->error(LP, "Error exit!");
	return;
}

void simulateZProtocol(COMPort * port) {
	int bufLen = 64;
	int read = 0, written = 0;
	PTCHAR buf = new TCHAR[bufLen];
	FillMemory(buf, bufLen * sizeof(TCHAR), 0);

	try {
		while (((read = port->read(buf, 0, bufLen)) == 0) || (buf[0] != 'z')) {
			msleep(100);
		}
		Logger::getInstance()->info(LP, "Recieved from device: '" + string(buf) + "'");
		if (buf[0] == 'z') {
			Logger::getInstance()->info(LP,
					"Z-Protocol detected, sending CHIP ID request..");
			written = port->write("?", 0, 1);
			if (written != 1) {
				Logger::getInstance()->error(LP,
						"Sending CHIP ID request failed!");
				goto ErrorExit;
			}
			FillMemory(buf, bufLen * sizeof(TCHAR), 0);
			read = port->read(buf, 0, bufLen);
			Logger::getInstance()->info(LP,"Recieved CHIP ID response of length " + string(1,(char)read) + " byte(s)");
			_tprintf("\n\tCHIP ID:");
			for (int i = 0; i < read; i++) {
				_tprintf(" %#2x", buf[i]);
			}
		}
	} catch (PTCHAR errMsg) {
		Logger::getInstance()->error(LP,
				"Caught an Exception with message: " + string(errMsg));
		goto ErrorExit;
	} catch (...) {
		Logger::getInstance()->error(LP, "Caught unexpected exception!");
		goto ErrorExit;
	}

	ErrorExit: delete[] buf;
}

void usage() {
	_tprintf("\nUsage: uarttest <option> <port>");
	_tprintf("\n\nOptions:");
	_tprintf("\n\t/r Reading from UART");
	_tprintf("\n\t/w Writing to UART");
	_tprintf("\n\t/z Simulate z-protocol\n");

	list<string> serialPorts;
	scanPorts(serialPorts);
	list<string>::iterator it;
	_tprintf("\nDetected ports: \n");
	for (it = serialPorts.begin(); it != serialPorts.end(); it++) {
		_tprintf("\t%s\n", it->c_str());
	}
}

int _tmain(int argc, PTCHAR argv[]) {
	//testOpenCloseOperation("COM1");
	try {
		if (argc != 3) {
			usage();
			return 0;
		}

		TCHAR flow;
		LPCTSTR portName = argv[2];

		if (strcmp(argv[1], "/r") == 0) {
			flow = 'r';
		} else if (strcmp(argv[1], "/w") == 0) {
			flow = 'w';
		} else if (strcmp(argv[1], "/z") == 0) {
			flow = 'z';
		} else {
			usage();
			return 0;
		}

		uart_config config;
		COMPort port(portName);

		port.open();
		Logger::getInstance()->info(LP, "Port '"+string(portName)+"' has been opened successfuly..");

		port.set(config);
		Logger::getInstance()->info(LP, "Port configuration for '"+string(portName)+"' has been set successfuly..");

		if (flow == 'r') {
			startReader(&port);
		} else if (flow == 'w') {
			startWriter(&port);
		} else if (flow == 'z') {
			simulateZProtocol(&port);
		}

		port.close();
		Logger::getInstance()->info(LP, "Port '"+string(portName)+"' has been closed..");
	} catch (const char * ex) {
		printf("Exception has been thrown: %s\n", ex);
		Logger::getInstance()->error(LP,"Exception has been thrown: "+ string(ex));
	} catch (...) {
		printf("Unknown exception has been thrown\n");
		Logger::getInstance()->error(LP, "Unknown exception has been thrown");
	}

	return 0;
}


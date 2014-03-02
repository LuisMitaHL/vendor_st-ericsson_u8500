/*
 * Logger.cpp
 *
 *  Created on: 22.5.2012
 *      Author: vsykal01
 */
#define LOGGER_C_
#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>

#ifdef __WIN__
#include "windows.h"
#else
#include <sys/time.h>

#endif

using namespace std;
Logger::Logger() {
	// TODO Auto-generated constructor stub
	logFile = "jni.log";
	logLevel = 2;
}
Logger* Logger::m_pInstance = NULL;

Logger* Logger::getInstance() {
	if (!m_pInstance) {
		m_pInstance = new Logger;
	}
	return m_pInstance;
}

/**
 * 0 nothing to log
 * 1 only errors
 * 2 errors + info
 * 3 errors + info + debug
 */
void Logger::setLogLevel(int pLogLevel) {
	logLevel = pLogLevel;
}
void Logger::error(const char *prefix, string message) {
	if (logLevel > 0) {
		log(prefix, "ERROR", message);
	}
}
void Logger::info(const char *prefix, string message) {
	if (logLevel > 1) {
		log(prefix, "INFO", message);
	}
}
void Logger::debug(const char *prefix, string message) {
	if (logLevel > 2) {
		log(prefix, "DEBUG", message);
	}
}
void Logger::log(const char *prefix, string type, string message) {
	stringstream ss;
	ss << getFormatedTime() << " " << setiosflags(ios::left) << setw(5) << type
			<< " " << prefix << " - " << message;
	message = ss.str();
	ofstream f;
	f.open(logFile.c_str(), ios::out | ios::app);
	f << message << endl;
	f.close();
	cout << message << endl;
}

string Logger::getFormatedTime() {
	time_t t = std::time(NULL);
	tm* tm = std::localtime(&t);
	stringstream ss;
	ss << setfill('0') << (tm->tm_year + 1900) << "-" << setw(2)
			<< (tm->tm_mon + 1) << "-" << setw(2) << tm->tm_mday << " "
			<< setw(2) << tm->tm_hour << ":" << setw(2) << tm->tm_min << ":"
			<< setw(2) << tm->tm_sec << "," << setw(3);
#ifdef __WIN__
	SYSTEMTIME now;
	GetLocalTime(&now);
	ss << now.wMilliseconds;
#else
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	ss << tv.tv_usec;
#endif
	return ss.str();
}

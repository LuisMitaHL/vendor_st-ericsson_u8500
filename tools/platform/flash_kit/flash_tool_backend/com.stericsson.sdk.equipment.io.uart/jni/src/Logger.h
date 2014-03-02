/*
 * Logger.h
 *
 *  Created on: 22.5.2012
 *      Author: vsykal01
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <sstream>

#define STRINGIFY(value) #value
#define TOSTRING(value) STRINGIFY(value)
#define LP __FILE__ ":" TOSTRING(__LINE__)
class Logger {
public:
	static Logger* getInstance();
	/**
	 * 0 nothing to log
	 * 1 only errors
	 * 2 errors + info
	 * 3 errors + info + debug
	 */
	void setLogLevel(int logLevel);
	void error(const char *prefix, std::string message);
	void info(const char *prefix, std::string message);
	void debug(const char *prefix, std::string message);

private:
	Logger();
	Logger(Logger const&){};             // copy constructor is private
    Logger& operator=(Logger const&){return *this;};  // assignment operator is private
	static Logger* m_pInstance;

	/**
	 * @param prefix is usualy set by macro LP
	 */
	void log(const char *prefix, std::string type, std::string message);
	std::string getFormatedTime();
	std::string logFile;
	int logLevel;
};

#endif /* LOGGER_H_ */

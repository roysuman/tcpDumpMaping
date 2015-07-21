/*
 * =====================================================================================
 *
 *       Filename:  loghandler.hpp
 *
 *    Description:  manageing the log files
 *
 *        Version:  1.0
 *        Created:  Wednesday 29 April 2015 04:32:42  IST
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Suman Roy (), email.suman.roy@gmail.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#ifndef LOGHANDLER_HPP_
#define LOGHANDLER_HPP_
#include <stdlib.h>
#include<iostream>
#include<fstream>
#include <fstream>
#include <ctime>
#include <sstream>
#define PRINT __DATE__<<"_"<<__TIME__<<" [ "<<__FILE__<<" ] [ "<<__FUNCTION__<<" ] [ "<<__LINE__<<" ] "
extern std::ofstream     globalLogHandler;

class LogHandler{
	public:
		LogHandler(){}
		~LogHandler(){}
		bool activateLog();
		std::string getTime();
};
#endif

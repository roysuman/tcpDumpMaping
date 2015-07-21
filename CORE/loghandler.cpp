/*
 * =====================================================================================
 *
 *       Filename:  loghandler.cpp.cpp
 *
 *    Description:  for manageing the log files
 *
 *        Version:  1.0
 *        Created:  Wednesday 29 April 2015 04:20:30  IST
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Suman Roy (), email.suman.roy@gmail.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#include"loghandler.hpp"
std::ofstream globalLogHandler;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  activeLog
 *  Description:  open the log file and direct all std::cout && std::cerr to
 *  the log file
 * =====================================================================================
 */
bool LogHandler::activateLog(  ){
	std::string    logFileName;
	bool           returnValue;

	returnValue = true;
	logFileName = "LOG/log"+getTime()+".log";
	globalLogHandler.open( logFileName.c_str(), std::ios::out );
	if ( globalLogHandler.is_open() ){
		std::cout.rdbuf ( globalLogHandler.rdbuf() );
		std::cerr.rdbuf( globalLogHandler.rdbuf() );

	}else{
		returnValue = false;
	}
	return returnValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getTime
 *  Description:  get the current system time for logging
 * =====================================================================================
 */
std::string LogHandler::getTime(){


	 time_t         t;
	 struct tm      *now;
	 std::string    timeString;
	 try{
		 t  = time(0); 
		 now = localtime( & t );
		 timeString = timeString + static_cast<std::ostringstream*>(
				 &(std::ostringstream() << now->tm_year + 1900) )->str()+
			  "-"+ static_cast<std::ostringstream*>( &(std::ostringstream() 
						   << now->tm_mon + 1 ) )->str() +
			  "-"+static_cast<std::ostringstream*>( &(std::ostringstream() 
						   << now->tm_mday ) )->str()+" "+
			  static_cast<std::ostringstream*>( &(std::ostringstream() 
						  << now->tm_hour) )->str()+":"+
			  static_cast<std::ostringstream*>( &(std::ostringstream() 
						  << now->tm_min ) )->str()+":"+
			  static_cast<std::ostringstream*>( &(std::ostringstream() 
						  << now->tm_sec ) )->str() ;
	 }
	 catch (const std::exception &e ){
		 std::cerr<<PRINT<<" Exception\n"<<e.what()<<std::endl;
	 }
	 return timeString;
}


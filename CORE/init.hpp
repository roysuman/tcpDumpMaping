/*
 * =====================================================================================
 *
 *       Filename:  init.hpp
 *
 *    Description:  init programm for the tool
 *
 *        Version:  1.0
 *        Created:  Wednesday 29 April 2015 02:34:13  IST
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Suman Roy (), email.suman.roy@gmail.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#ifndef INIT_HPP_
#define INIT_HPP_
#include<iostream>
#include<stdlib.h>
#include<fstream>
#include<sstream>
#include "jsoncpp/json.h"
#include "jsoncpp/writer.h"
#include<string.h>
#include"loghandler.hpp"
#define MAX_FILE 10
typedef struct __config__ config;
struct __config__{
	bool infoLog;
	unsigned int port;
	std::string ip;
	size_t    noOfDumpFile;
	std::string    dumpFiles[ MAX_FILE ];
	long delay;
	std::string outputFileName;
};
class Init:public LogHandler {
	private:
		static bool    successReadConfig;

	public:
		static config    configuration;
		Init(){
			configuration.noOfDumpFile= -1 ;
		       	successReadConfig = false;
		}
		~Init(){}
		bool perseConfigFile( std::string filename );
		bool isInfoLogActive( );
		bool getFilterIp( std::string &);
		bool getDumpFileCount(size_t & );
		bool readDumpFileName( const size_t  , std::string &);
		bool getConfig ( config *& );
		bool getDelay(long &);
		bool getOutFile( std::string &);



};


#endif

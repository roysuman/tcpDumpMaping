/*
 * =====================================================================================
 *
 *       Filename:  init.cpp
 *
 *    Description:  init programm for the tool
 *
 *        Version:  1.0
 *        Created:  Wednesday 29 April 2015 02:33:49  IST
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Suman Roy (), email.suman.roy@gmail.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#include"init.hpp"

bool    Init::successReadConfig =false;
config  Init::configuration;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  perseConfigFile
 *  Description:  read the config file and store the file parsed data into 
 *  struct config datastructure.
 * =====================================================================================
 */
bool Init::perseConfigFile ( std::string fileName){
#ifdef DEBUG
	std::cout<<PRINT<<" File to parse--["<<fileName<<"]\n";
#endif
	std::ifstream    fileHandler;
	bool             returnValue;
	Json::Value      root;
	Json::Reader     reader;

	returnValue = true;
	try{
		fileHandler.open( fileName.c_str() , std::ifstream::in );
		if ( !fileHandler.is_open() ){
			std::cerr<<PRINT<< "  Can't open file ["<<fileName<<"] for parsing the configurations\n";
			returnValue = false;
		}
		if ( returnValue ){
			if ( !reader.parse( fileHandler , root ) ){
				std::cerr  <<PRINT<<" Failed to parse configuration\n";
				returnValue = false;
			}else{
				//read the JSON value from .json file
				Json::Value nodeValue = root [ "info" ];
				if ( not nodeValue.isNull() ){
#ifdef DEBUG
					std::cout<<PRINT<<" Log[ "<<nodeValue.asString()<<"]\n";
#endif
					configuration.infoLog  =( bool )strcmp( nodeValue.asString().c_str(),"false" );
				}else{
					std::cerr<<PRINT<<" eroor on reading info flag from config file\n";
					returnValue = false;
				}	
			}
		}
		if ( returnValue ){
			//read the JSON value from .json file
			Json::Value nodeValue = root [ "port" ];
			if ( not nodeValue.isNull() ){
#ifdef DEBUG
				std::cout<<PRINT<<" port[ "<<nodeValue.asInt()<<"]\n";
#endif
				configuration.port = nodeValue.asInt();
			}else{
				std::cerr<<PRINT<<" eroor on reading info SNIFFING_IP from config file\n";
				returnValue = false;
			}
		}
		if ( returnValue ){
			//read the JSON value from .json file
			Json::Value nodeValue = root [ "sniffingIp" ];
			if ( not nodeValue.isNull() ){
#ifdef DEBUG
				std::cout<<PRINT<<" sniffingIp[ "<<nodeValue.asString()<<"]\n";
#endif
				configuration.ip = nodeValue.asString();
			}else{
				std::cerr<<PRINT<<" eroor on reading info SNIFFING_IP from config file\n";
				returnValue = false;
			}
		}
		//read the tcpdump files name
		if ( returnValue ){

			Json::Value nodeValue = root ["noOfFiles"];
			if ( not nodeValue.isNull() ){
#ifdef DEBUG
				std::cout<<PRINT<<" No Of tcpDump File[ "<<std::stoi(nodeValue.asString() )<<"]\n";
#endif
				configuration.noOfDumpFile=std::stoi ( nodeValue.asString() );
			}
			else{
				std::cerr<<PRINT<<" eroor on reading TCP_DUMP_FILE_COUNT from config file\n";
				returnValue = false;
				
			}
			size_t tempDumpFileCount = 0;
			while ( returnValue && tempDumpFileCount< configuration.noOfDumpFile ){

				std::string jsonTagName= "file" + std::to_string( tempDumpFileCount + 1 );
#ifdef DEBUG
				std::cout<<PRINT<<"Fiel Name:{"<<jsonTagName<<"}\n";
#endif

				Json::Value nodeValue = root [ jsonTagName];
				if ( not nodeValue.isNull() ){
#ifdef DEBUG
					std::cout<<PRINT<<" Dump File[ "<<tempDumpFileCount + 1 <<"] [ Name"<<nodeValue.asString()<<"]\n";
#endif
					configuration.dumpFiles[ tempDumpFileCount ] = nodeValue.asString();
					
				}
				else{
					std::cerr<<PRINT<<" eroor on reading TCP_DUMP_FILE_COUNT from config file\n";
					returnValue = false;
				}
				tempDumpFileCount ++;
			}
		}
		if ( returnValue ){
			Json::Value nodeValue = root ["Delay"];
			if ( not nodeValue.isNull() ){
#ifdef DEBUG
				std::cout<<PRINT<<" Delay Filter [ "<<std::stoi(nodeValue.asString() )<<"]\n";
#endif
				configuration.delay =std::stoi ( nodeValue.asString() );
			}
			else{
				std::cerr<<PRINT<<" eroor on reading Delay from config file\n";
				returnValue = false;
				
			}
		}

		if ( returnValue ){

			Json::Value nodeValue = root ["OutPutFile"];
			if ( not nodeValue.isNull() ){
#ifdef DEBUG
				std::cout<<PRINT<<" Out put file name [ "<<nodeValue.asString() <<"]\n";
#endif
				configuration.outputFileName =  nodeValue.asString() ;
			}
			else{
				std::cerr<<PRINT<<" eroor on reading OutPutFile from config file\n";
				returnValue = false;
				
			}
		}

	}


	catch ( const std::exception &e ){
		std::cerr<<PRINT<<" Exception\n"<<e.what()<<std::endl;
		returnValue = false;
	}
	return returnValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  isInfoLogActive
 *  Description:  check whether teh infoLog active
 * =====================================================================================
 */

bool Init::isInfoLogActive(void  ){
	bool    returnValue;
	try{
		returnValue  = configuration.infoLog;
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<" Exception\n"<<e.what()<<std::endl;
		returnValue = false;

	}
	return returnValue;

}



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getFilterIp
 *  Description:  get the ip address have to use for filtering the tcp packets
 * =====================================================================================
 */

bool Init::getFilterIp( std::string &ipAddress){
	bool    returnValue;

	returnValue = true;
	try{
		if ( successReadConfig ){
			ipAddress =  configuration.ip;
		}
		else {
			returnValue = false;
			ipAddress = "NULL";
		}
	}catch ( const std::exception &e ){
		std::cerr<<PRINT<<" Exception\n"<<e.what()<<std::endl;
		returnValue = false;

	}
	return returnValue ;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  getDumpFileCount
 *  Description:  get the tcpDump fiel count.. read the file count at the reference var.
 *  and returnValue: true if fileCount >=1 else return false
 * =====================================================================================
 */

bool Init::getDumpFileCount( size_t &dumpFileCount ){

	bool    returnValue ;

	returnValue = false;

	dumpFileCount = configuration.noOfDumpFile;
	if ( dumpFileCount > 0 )
		returnValue = true;
	else returnValue = false;
	
	return returnValue;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  readDumpFileName
 *  Description:  raed the tcpDump file asked by position of dumpFiles[] array of string
 *  return true on success else return false.
 * =====================================================================================
 */
bool Init::readDumpFileName( const size_t index , std::string &fileName){
	bool    returnValue;

	returnValue = true;
	if ( index < configuration.noOfDumpFile ) {
		fileName = configuration.dumpFiles[ index ];
	}else returnValue = false;
	return returnValue;
}

bool Init::getDelay( long &delay ){
	bool returnValue;

	returnValue = true;
	try{
		delay = configuration.delay ;
	}
	catch (const std::exception &e ){
		std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
		returnValue = false;
	}
	return returnValue;

}
bool Init::getOutFile( std::string &fileName ){
	bool    returnValue;

	returnValue = true;
	try{
		fileName = configuration.outputFileName;
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exceoption:"<<e.what()<<std::endl;
		returnValue = false;
	}
	return returnValue;
}

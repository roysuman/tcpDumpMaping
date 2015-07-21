/*
 * =====================================================================================
 *
 *       Filename:  report.cpp
 *
 *    Description:  see the header file
 *
 *        Version:  1.0
 *        Created:  Tuesday 05 May 2015 08:25:39  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#include "report.hpp"
long           rep::Report::delay;
std::string    rep::Report::reportFile;
Init           *rep::Report::initObj;
bool            rep::Report::isConfig = false;
std::fstream    rep::Report::fileHandler;
bool           rep::Report::fileOpend= false;
bool           rep::Report::initCreated= false;
DbManager      *rep::Report::dbInstance = NULL;
bool           rep::Report::isDbActive = false;
rep::Report::Report(){
	try {
		initObj = new Init();
		initCreated   = initObj->getDelay(delay );
		initCreated  == true ? isConfig =initObj->getOutFile(reportFile ):isConfig = false;
		if ( isConfig ){
			dbInstance =  new DbManager();
		       	isDbActive = true ;
		}else{ dbInstance = NULL; isDbActive = false;}
	}
	catch ( const std::bad_alloc &ba ){
		isConfig = false;
		initCreated = false;
		isDbActive = false;
		std::cerr<<PRINT<<"Exception"<<ba.what()<<std::endl;
	}
	
}
rep::Report::~Report(){
	try{
		if ( initCreated )delete initObj;
		if ( fileOpend ) fileHandler.close();
	}catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
	}
}

bool rep::Report::setFilter(){return true;}
/* 
bool rep::Report::setFilter( ){
	bool    returnValue;
	if ( isConfig ){
		returnValue   = initObj->getDelay(delay );
		 returnValue  == true ? returnValue   =initObj->getOutFile(reportFile ):
			  returnValue  = false;
		 isConfig = returnValue;
	}
	return returnValue;
}
*/
bool rep::Report::openFile( ){
	bool    returnValue;

	returnValue = true;
	fileHandler.open(reportFile.c_str() , std::ios::out | std::ios::app );
	fileOpend = true;
	if ( !fileHandler.is_open() ){
		returnValue = false;
		fileOpend = true;
		isConfig = false;
	}
	return returnValue;

}
bool rep::Report::genReport(){
	bool     returnValue;
	returnValue = setFilter () && openFile () && isDbActive  ;
	if ( returnValue ){

		//read from data base and write
		returnValue = readData();

	}
	return returnValue ;
}


bool rep::Report::readData( ){

	bool    returnValue;

	returnValue = true;
	__groupReportStruct__   *groupReportContainer = NULL;
	__packetInfo__          *packetInfo;
	int    noOfPackets;

	  noOfPackets = dbInstance->fetchGroupReport ( groupReportContainer , 0.0 );
	 std::cout<<"No PACKETS:[ "<<noOfPackets<<std::endl;
	 for ( int index = 0 ; index <= noOfPackets ; index++ ){
		 fileHandler<<"@@@@---------------PACKET[ "<<index + 1 <<"]-------------\n";
		 fileHandler<<"Source Ip Address[ "<<groupReportContainer[ index ].sourceIpAddr<<"] "<<std::endl;
		 fileHandler<<"Destination Ip Address: [ "<<groupReportContainer[ index].destinationIpAddr <<" ] "<<std::endl;
		 fileHandler<<"Start Time stamp:["<<groupReportContainer[ index ].startTime<<"] "<<std::endl;
		 fileHandler<<"End time stamp [ "<<groupReportContainer[ index].endTime<<" ] "<<std::endl;
		 //all -ackets between start and end time
		 int  size = dbInstance->getBulkPacket( packetInfo ,groupReportContainer [ index].startTime , groupReportContainer[ index ].endTime , 2 );

		 fileHandler<<"OTHER SERVERS ON SAME TIME STAMP"<<std::endl;
		 for ( int loop = 0 ; loop <= size ; ++loop ){
			 fileHandler<<"SOURCE IP:"<< packetInfo [ loop ].sourceIpAddr<<"DESTINATION IP:"<<packetInfo[ loop ].destinationIpAddr<<"Time:"<<packetInfo[ loop ].time<<"Length:"<<packetInfo [ loop ].length<<std::endl;
		 
		 }
		 fileHandler<<"PACKET [ "<<index + 1<<" ]######################################__ENDS\n";
		 try {
			 if ( packetInfo != NULL )delete []packetInfo;
		 }catch ( const std::exception &e ){
			 std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
			 exit(0);
		 }

	 }
	 try{
		 if ( groupReportContainer != NULL ) delete []groupReportContainer;
	 }catch ( const std::exception &e ){
		 std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
		 exit(0);
	 }
	 return returnValue;
}

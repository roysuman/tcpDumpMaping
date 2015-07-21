/*
 * =====================================================================================
 *
 *       Filename:  databaseManagement.cpp
 *
 *    Description:  all database management
 *
 *        Version:  1.0
 *        Created:  Wednesday 29 April 2015 04:45:39  IST
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Suman Roy (), email.suman.roy@gmail.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#include "databaseManagement.hpp"
using namespace std;
 char *zErrMsg ;
bool DbManager::dataBaseOpened = false;
bool DbManager::tableCreated = false;
sqlite3 *DbManager::db = NULL;
std::string quotesql( const std::string& s ) {
        return std::string("\"") + s + std::string("\"");
}

bool executeStatus (const int status , char * zErr ){
	bool    returnValue;

	returnValue = true;


         if ( status != SQLITE_OK ){
		 std::cerr<<__DATE__<<"_"<<__TIME__<<"["<<__FUNCTION__<<"] ["<<
			 __FILE__<<" ] ["<<__LINE__
			 <<"sqlite3  executeStatus -> error"<<std::endl;
		 returnValue = false;
	 }
         else {
#ifdef DEBUG
		 std::cerr<<__DATE__<<"_"<<__TIME__<<"["<<__FUNCTION__<<"] ["<<
			 __FILE__<<" ] ["<<__LINE__
			 <<"sqlite3  executeStatus -> ok"<<std::endl;
#endif
	 }
	 sqlite3_free ( zErr );
	 return returnValue;
}

int callback(void *temp, const  int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		std::cout<<(char*)temp<<std::endl;
		std::cout<<azColName[i]<<argv[i]<<std::endl;
	//	printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	return 0;
}


DbManager::DbManager(){
	std::string    databaseName;

	databaseName = "filterDb.db";
	rc = sqlite3_open ( databaseName.c_str() , &db );
	if ( rc ){
		std::cerr<<PRINT<<"] Database notAble to open\n";
		dataBaseOpened = false;
	}else dataBaseOpened = true;
}
DbManager::~DbManager(){
	if ( dataBaseOpened == true ){
		if ( sqlite3_close( db ) == SQLITE_OK  ){
			dataBaseOpened = false;
		}
	}
}

bool DbManager::createTable ( void ){
	bool           returnValue;
	std::string    sqlStatementToCreateTable;
	std::string    sqlStatementToCreateTableIndex;


	returnValue = true;
	if ( dataBaseOpened ){
		sqlStatementToCreateTable ="CREATE TABLE PACKETS( PACKET_NUMBER PRIMARY KEY NOT NULL,CONVERSATIONID NOT NULL ,SOURCE_IP TEXT ,\
					    DESTINATION_IP TEXT,\
					    TIME TEXT,\
					    SOURCE_PORT INTEGER NOT NULL,DESTINATION_PORT INTEGER NOT NULL, URL TEXT , FILE_NUMBER INTEGER , LENGTH INTEGER);";
		returnValue = executeStatus ( sqlite3_exec(db,sqlStatementToCreateTable.c_str(), callback, 0, &zErrMsg ) , zErrMsg);
		sqlStatementToCreateTableIndex ="CREATE TABLE PACKETSINDEX(CONVERSATIONID INTEGER PRIMARY KEY NOT NULL , SOURCE_IP TEXT ,\
					    DESTINATION_IP TEXT ,\
					    START_TIME TEXT , END_TIME TEXT , FILE_NUMBER INTEGER, DELAY INTEGER);";
		returnValue = executeStatus ( sqlite3_exec(db,sqlStatementToCreateTableIndex.c_str(), callback, 0, &zErrMsg ) , zErrMsg);
	}else returnValue = false;
	tableCreated = returnValue;


	return returnValue;
           
}
bool DbManager::insertPacketInfo(  int packetNumber, int conversationNumber  ,std::string sourceIp, 
		std::string destinationIp, std::string time , int sourcePort, 
		int destinationPort , std::string url , int fileNumber , int length ){
#ifdef DEBUG
	std::cout<<PRINT<<" Packet Number: [ "<<packetNumber<<" ] ConversationId [ "
		<<conversationNumber <<" ] SourceIp:[ "<<sourceIp 
		<<" ] Destination Ip: [ "<<destinationIp <<" ] Time : [ "
		<<time <<" ] SourcePort: [ "<<sourcePort<<" ] Destination POrt: [ "
		<<destinationPort<<" ] url : ["<<url<<" ] FileNumber :["
		<<fileNumber<<" ] Length [ "<<length <<" ] "<< std::endl;
#endif
	bool           returnValue;
	std::string    sqlStatement;

	returnValue = true;
	if ( dataBaseOpened ){
		
		sqlStatement = "Insert INTO PACKETS ( PACKET_NUMBER, CONVERSATIONID, SOURCE_IP , DESTINATION_IP , TIME , SOURCE_PORT ,DESTINATION_PORT ,URL, FILE_NUMBER, LENGTH ) VALUES (" +std::to_string( packetNumber  )+","+ std::to_string( conversationNumber )+","+ quotesql( sourceIp ) + "," + quotesql( destinationIp) +","+ quotesql( time )+","+ std::to_string(sourcePort) + ","+std::to_string ( destinationPort)+"," + quotesql( url )+","+ std::to_string ( fileNumber )+","+std::to_string( length )+ ");";
#ifdef DEBUG
		std::cout<<PRINT<<"Sql Statement ; ["<<sqlStatement<<" ]"<<std::endl;
#endif
		returnValue = executeStatus ( sqlite3_exec( db , sqlStatement.c_str(), callback , 0 , &zErrMsg), zErrMsg);
	}else returnValue = false;

       return returnValue;	

}

bool DbManager::insertPacketIndex( int conversationId, std::string sourceIp , std::string destinationIp ,
	       	std::string startTime ,  int fileNumber ){
	std::string    queryStatement;
	bool returnValue;
	 returnValue = true;
	if ( dataBaseOpened ){
		queryStatement  = "INSERT INTO PACKETSINDEX ( CONVERSATIONID , SOURCE_IP , DESTINATION_IP, START_TIME , FILE_NUMBER) VALUES ("+std::to_string ( conversationId ) +","+quotesql( sourceIp )+ ","+quotesql( destinationIp ) +","+ quotesql ( startTime ) +","+std::to_string ( fileNumber ) +");";
		returnValue = executeStatus ( sqlite3_exec( db , queryStatement.c_str(), callback , 0 , &zErrMsg), zErrMsg);
	}
	else returnValue = false;
	return returnValue;
}


bool DbManager::updatePacketIndex( int conversationId , std::string endTime ){
#ifdef DEBUG
	std::cout<<PRINT<<" Conversation id [ "<< conversationId<<" ] End Time [ "<<endTime<<" ] \n";
#endif

	bool returnValue;
	std::string sqlStatement;
	std::string  startTime;
	sqlite3_stmt *statement;
	std::string  tempEndTime = endTime ;

	sqlStatement = "SELECT START_TIME FROM PACKETSINDEX WHERE CONVERSATIONID = " +std::to_string ( conversationId )+";";
	if( sqlite3_prepare ( db , sqlStatement.c_str(), -1 , &statement, 0 ) == SQLITE_OK ){
		if ( sqlite3_step( statement ) != SQLITE_ROW) returnValue = false;
		else {
			startTime = 
				std::string( reinterpret_cast< const char*> ( sqlite3_column_text ( statement , 0 ) ) );
		}
	}
	//find the diff
	std::string delimiter = " ";

	size_t pos = 0;
	std::string token;
	while ( ( pos = startTime.find(delimiter)) != std::string::npos) {
		 //   token = startTime.substr(0 , pos );
		  //  std::cout<<token<<std::endl;
		     startTime.erase(0, pos + delimiter.length());
	}
	std::cout << startTime << std::endl;
	while ( (pos = endTime.find(delimiter ) != std::string::npos )){
		endTime.erase( 0 , pos + delimiter.length() );
	}
	int hour1 , hour2 , min1 , min2 , sec1, sec2;
	long usec1 , usec2;
	delimiter = ":";
	pos = startTime.find( delimiter);
	if ( pos!= std::string::npos ){
		hour1 = std::stoi( startTime.substr( 0 , pos  ) );
		startTime.erase(0 , pos + delimiter.length());
		
	}
	pos = startTime.find( delimiter);
	if ( pos!= std::string::npos ){
		min1 = std::stoi( startTime.substr( 0 , pos  ) );
		startTime.erase(0 , pos + delimiter.length());
	}
	
	pos = endTime.find( delimiter);
	if ( pos != std::string::npos ){
		hour2 = std::stoi( endTime.substr( 0 , pos  ) );
		endTime.erase(0 , pos + delimiter.length());
		
		
	}
	pos = endTime.find( delimiter);
	if ( pos!= std::string::npos ){
		min2 = std::stoi( endTime.substr( 0 , pos  ) );
	        endTime.erase(0 , pos + delimiter.length());
	}
	delimiter = ".";
	pos = startTime.find ( delimiter );
	if ( pos!= std::string::npos ){
		sec1 = std::stoi( startTime.substr( 0 , pos  ) );
		startTime.erase(0 , pos + delimiter.length());
	}
	pos = endTime.find ( delimiter );
	if ( pos!= std::string::npos ){
		sec2 = std::stoi( endTime.substr( 0 , pos  ) );
		endTime.erase(0 , pos + delimiter.length());
	}
	usec1  = std::stoi( startTime.substr( 0 , startTime.length()  ) );
	usec2 = std::stoi ( endTime.substr ( 0 , endTime.length() ) );
	long diff = (( sec2 - sec1 ) * 1000000 ) + 
		( ( min2 - min1 ) * 60 * 1000000 ) + usec2 - usec1 ;//+		( ( hour2 - hour1 ) * 3600 * 1000000 );
#ifdef DEBUG
	std::cout<<hour1<<"--"<<min1 <<"--"<<sec1<<"--"<<usec1<<std::endl;
	std::cout<<hour2<<"--"<<min2<<"--"<<sec2<<"--"<<usec2<<std::endl;
	std::cout<<"Diff:["<<diff<<"] \n";
#endif

	//now update in db
	sqlStatement.clear();
	//find time diff
	sqlStatement ="UPDATE PACKETSINDEX SET END_TIME = "+ quotesql ( tempEndTime )+ 
		", DELAY = " + std::to_string ( diff )+" WHERE CONVERSATIONID ="+std::to_string ( conversationId )+";";
#ifdef DEBUG
	std::cout<<PRINT<<"State ment [ "<<sqlStatement<<"] \n";
#endif

	returnValue = executeStatus( sqlite3_exec ( db , sqlStatement.c_str() , callback , 0 , &zErrMsg ),zErrMsg );
	


	return returnValue;


}




size_t  DbManager::fetchGroupReport( __groupReportStruct__ *& groupReportContainer  , double delaySec ){
	bool    returnValue;
	
	std::string    sqlStatement;
	size_t    rowCount;
	int res;
	size_t    index;
	sqlite3_stmt    *statement;

	index = -1;
	size_t  noPacketFetched = -1;

	returnValue = isOpen() && isCreated();
	

	if ( returnValue ){
		sqlStatement = "SELECT COUNT( *) from PACKETSINDEX WHERE DELAY >="+ std::to_string( delaySec* 1000000 )+";";
	
		if (sqlite3_prepare( db , sqlStatement.c_str() , -1 , &statement , 0 ) == SQLITE_OK ){
			if (  sqlite3_step( statement ) != SQLITE_ROW )returnValue = false;
			else{
				rowCount  = sqlite3_column_int( statement , 0 );
				std::cout<<PRINT<<"RowCount [ "<<rowCount<<" ] \n";
				
			}
			
		}
		if (returnValue  && rowCount != 0){
			try{
				groupReportContainer = new __groupReportStruct__ [ rowCount ];
			}
			catch ( const std::bad_alloc &ba ){
				std::cerr<<PRINT<<"Exception:"<<ba.what()<<std::endl;
				returnValue = false;
			}
			if( returnValue ){
				sqlStatement.clear();
				sqlStatement = "SELECT CONVERSATIONID ,SOURCE_IP , DESTINATION_IP ,"\
					       "START_TIME , END_TIME , DELAY FROM PACKETSINDEX WHERE DELAY >="+std::to_string( delaySec* 1000000 )+";";
				std::cout<<PRINT<<"Execute statement [ "<<sqlStatement<<"] \n";
			       if ( sqlite3_prepare ( db , sqlStatement.c_str() , -1 , &statement , 0 ) == SQLITE_OK ){
				       while (returnValue  ){
					       res =  sqlite3_step ( statement ) ;
					       if ( res == SQLITE_ROW && returnValue ){
						       ++index;
						       std::cout<<PRINT<<"Reading packet No["<<index<<"] \n";
						       groupReportContainer [ index ].conversationNumber = 
							       sqlite3_column_int64( statement , 0 );
						       groupReportContainer [ index ].sourceIpAddr = 
							       std::string (reinterpret_cast<const char*> ( sqlite3_column_text( statement , 1 ) ) );
						       groupReportContainer[ index].destinationIpAddr =
							       std::string (reinterpret_cast<const char*>(  sqlite3_column_text( statement , 2 )) );
						       groupReportContainer[ index].startTime =
							       std::string ( reinterpret_cast<const char*>( sqlite3_column_text( statement ,  3 ) ));
						       groupReportContainer[ index].endTime =
							       std::string (reinterpret_cast<const char*>( sqlite3_column_text ( statement , 4 ) ));
						       groupReportContainer[ index ].delay = 
							       sqlite3_column_double( statement , 5 );

					       }  if ( res == SQLITE_DONE ||res == SQLITE_ERROR) returnValue = false; //if ( res == SQLITE_DONE || res == SQLITE_ERROR ) returnValue = false;
				       }
				       noPacketFetched = index;
#ifdef DEBUG
				       for (size_t index = 0 ; index <= noPacketFetched ; index++ ){
					       std::cout<<"---------------PACKET[ "<<index + 1 <<"]-------------\n";
					       std::cout<<"Source Ip Address[ "
						       <<groupReportContainer[ index ].sourceIpAddr<<"] "<<std::endl;
					       std::cout<<"Destination Ip Address: [ "
						       <<groupReportContainer[ index].destinationIpAddr <<" ] "<<std::endl;
					       std::cout<<"Start Time stamp:["
						       <<groupReportContainer[ index ].startTime<<"] "<<std::endl;
					       std::cout<<"End time stamp [ "
						       <<groupReportContainer[ index].endTime<<" ] "<<std::endl;
				       }
#endif
			       }else {
				       std::cerr<<PRINT<<"sqlite3 prepare statemen terror"<<std::endl;
				       returnValue = false;
			       }
			}
		}
	}
	return noPacketFetched;
}

bool DbManager::fetchConversationReport( __packetInfo__ *&conversationInfo , 
		int conversationNumber , size_t &noRowFetched){
	bool    returnValue;
	
	std::string    sqlStatement;
	sqlite3_stmt    *statement;
	size_t    rowCount;
	int res;
	noRowFetched = 0;
	returnValue = isOpen() && isCreated();

	if ( returnValue ){
		sqlStatement = "SELECT COUNT( *) from PACKETSINDEX WHERE CONVERSATIONID = "+ std::to_string ( conversationNumber ) +";";
		if (sqlite3_prepare( db , sqlStatement.c_str() , -1 , &statement , 0 ) == SQLITE_OK ){
			if (  sqlite3_step( statement ) != SQLITE_ROW ) returnValue = false;
			else{
				rowCount = sqlite3_column_int( statement , 0 );
				
			}
			
		}
		if (returnValue ){
			try{
				conversationInfo = new __packetInfo__ [ rowCount ];
			}
			catch ( const std::bad_alloc &ba ){
				std::cerr<<PRINT<<"Exception:"<<ba.what()<<std::endl;
				returnValue = false;
			}
			if( returnValue ){
				sqlStatement.clear();
				sqlStatement = "SELECT SOURCE_IP , DESTITION_IP"\
					       "TIME , LENGTH  FROM PACKETINDEX WHERE CONVERSATIONID="+ std::to_string ( conversationNumber ) + ";";
			       if ( sqlite3_prepare ( db , sqlStatement.c_str() , -1 , &statement , 0 ) == SQLITE_OK ){
				       while (1 ){
					       res =  sqlite3_step ( statement ) ;
					       ++noRowFetched;
					       if ( res == SQLITE_ROW ){
						       conversationInfo [ noRowFetched ].sourceIpAddr = 
							       std::string (reinterpret_cast<const char*> ( sqlite3_column_text( statement , 1 )));
						       conversationInfo[ noRowFetched].destinationIpAddr = 
							       std::string (reinterpret_cast<const char*> ( sqlite3_column_text( statement , 2 )) );
						       conversationInfo[ noRowFetched].time =
							       std::string (reinterpret_cast<const char*> ( sqlite3_column_text ( statement , 3 )) );
						       conversationInfo[ noRowFetched ].length  = 
							       sqlite3_column_int64 ( statement ,  4 );
					       }if ( res == SQLITE_DONE || res == SQLITE_ERROR ) returnValue = false;
				       }
			       }else {
				       std::cerr<<PRINT<<"sqlite3 prepare statemen terror"<<std::endl;
				       returnValue = false;
			       }
			}
		}
	}
	return returnValue;

}

size_t DbManager::getBulkPacket ( __packetInfo__ *&packetInfo ,std::string startTime , 
		std::string endTime , size_t fileNo){
	bool    returnValue;
	returnValue = true;
	std::string    sqlStatement;
	size_t    rowCount;
	int    index;
	size_t    noPacketFetched;
	sqlite3_stmt    *statement;
	int res;

	index = -1;
	noPacketFetched = -1;

	sqlStatement ="SELECT COUNT(*) FROM PACKETS WHERE TIME BETWEEN "+quotesql ( startTime.c_str() )+" AND " + quotesql (endTime.c_str() )+"AND FILE_NUMBER = "+ std::to_string( fileNo )+";";
#ifdef DEBUG
	std::cout<<PRINT<<"String to execute [ "<<sqlStatement<<" ] "<<std::endl;
#endif
	if (sqlite3_prepare( db , sqlStatement.c_str() , -1 , &statement , 0 ) == SQLITE_OK ){
		if (  sqlite3_step( statement ) != SQLITE_ROW ){returnValue = false; exit (0);}
		else{
			rowCount  = sqlite3_column_int( statement , 0 );
			std::cout<<PRINT<<"RowCount [ "<<rowCount<<" ] \n";
				
			}
	}
	if (returnValue && rowCount != 0 ){
		try{
			std::cout<<"ROE COUNT:"<<rowCount<<std::endl;
				packetInfo = new __packetInfo__  [ rowCount ];
			}
			catch ( const std::bad_alloc &ba ){
				std::cerr<<PRINT<<"Exception:"<<ba.what()<<std::endl;
				returnValue = false;
			}
			if( returnValue ){
				sqlStatement.clear();
				sqlStatement = "SELECT SOURCE_IP , DESTINATION_IP , TIME , LENGTH  FROM PACKETS WHERE TIME BETWEEN "+ quotesql( startTime.c_str())+" AND " + quotesql (endTime.c_str() )+" AND FILE_NUMBER = "+std::to_string( fileNo) +";";
#ifdef DEBUG
	std::cout<<PRINT<<"String to execute [ "<<sqlStatement<<" ] "<<std::endl;
#endif
				if ( sqlite3_prepare ( db , sqlStatement.c_str() , -1 , &statement , 0 ) == SQLITE_OK ){
					while (returnValue  ){
					       res =  sqlite3_step ( statement ) ;
					       if ( res == SQLITE_ROW && returnValue ){
						       ++index;
						       packetInfo [ index ].sourceIpAddr = 
							       std::string (reinterpret_cast<const char*> ( sqlite3_column_text( statement , 0 ) ) );
						       packetInfo[ index].destinationIpAddr =
							       std::string (reinterpret_cast<const char*>(  sqlite3_column_text( statement , 1 )) );
						       packetInfo[ index].time =
							       std::string ( reinterpret_cast<const char*>( sqlite3_column_text( statement ,  2 ) ));
						       packetInfo [ index ].length = sqlite3_column_int64( statement , 3 );
					       }if ( res == SQLITE_DONE || res == SQLITE_ERROR ) returnValue = false;
					}
					noPacketFetched = index;
				 }else {
					 std::cerr<<PRINT<<"Sqlite3 prepare statement error"<<std::endl;
					 returnValue = false;
					 exit(0);
				 }
#ifdef DEBUG
				for ( size_t loop = 0 ; loop <= noPacketFetched  ; ++loop ){
					std::cout<<"Source Ip"<<packetInfo [ loop ].sourceIpAddr
						<<"Destination Ip"<<packetInfo[ loop].destinationIpAddr
						<<"Time:"<<packetInfo[ loop].time
						<<"Length:"<<packetInfo [ loop ].length<<std::endl;
				}
#endif
			}
	}
	return noPacketFetched;
}



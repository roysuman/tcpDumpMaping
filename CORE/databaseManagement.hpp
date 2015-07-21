/*
 * =====================================================================================
 *
 *       Filename:  databaseManagement.hpp
 *
 *    Description:  all the logic for accessing sqlite3 database
 *
 *        Version:  1.0
 *        Created:  Wednesday 29 April 2015 04:41:09  IST
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Suman Roy (), email.suman.roy@gmail.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#ifndef DATABASEMANAGEMENT_HPP_
#define DATABASEMANAGEMENT_HPP_
#include <iostream>
#include <sqlite3.h>
#include <cstdlib>
#include <sstream>

#include<string>
#include "packetDataStructure.hpp"
#include"reportStruct.hpp"
#define PRINT __DATE__<<"_"<<__TIME__<<" [ "<<__FILE__<<" ] [ "<<__FUNCTION__<<" ] [ "<<__LINE__<<" ] "
 extern char *zErrMsg ;
class DbManager{
	private:
		int rc;
		static bool dataBaseOpened;
		static bool tableCreated;
		static sqlite3 *db;
	public:
char *sql;
		DbManager();
		~DbManager();
		bool createDb();
		bool createTable();
		bool insertPacketInfo( int , int ,std::string , std::string , std::string ,  int ,int , std::string , int , int  );
		bool insertPacketIndex ( int , std::string , std::string , std::string ,  int );
		bool isOpen(){return dataBaseOpened;}
		bool isCreated(){ return tableCreated; }
		size_t  fetchGroupReport( __groupReportStruct__  *& reportContainer  , double );
		bool fetchConversationReport( __packetInfo__ *&conversationInfo , int conversationNumber, size_t & ); 
		bool updatePacketIndex ( int , std::string );
		size_t getBulkPacket( __packetInfo__ *& , std::string , std::string , size_t);
//		int callback( void * , const int , char ** , char ** );
//		bool executeStatus( const int , char * );
//		std::string quotesql( const std::string & );

};

#endif

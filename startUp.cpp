/*
 * =====================================================================================
 *
 *       Filename:  startUp.cpp
 *
 *    Description:  the main code...
 *
 *        Version:  1.0
 *        Created:  Thursday 30 April 2015 02:43:01  IST
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Suman Roy (), suman.roy@cognizant.com
 *   Organization:  Cognizant Technological Solutions
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include<iostream>
#include<thread>
#include "CORE/sniffing.hpp"
#include "CORE/init.hpp"
#include "CORE/grupPackets.hpp"
#include "CORE/buffer.hpp"
#include "CORE/report.hpp"

int main( ){

	bool           flag;
	Init           initObj;
	std::string    dumpFileName;


	//config
	LogHandler logHandlerObj;
	flag =  initObj.perseConfigFile( "environment.config");
       if ( !flag ){
	       std::cerr<<"Could Not able to parse config file--exit"<<std::endl;
	       exit ( 1 );
       }
       flag= initObj.isInfoLogActive ( );
       if ( flag )initObj.activateLog();else std::cout<<"\n Log not avtive\n";
       //get the input file name and send it to pcap lib
       size_t    tcpDumpFileCount;
       flag = initObj.getDumpFileCount( tcpDumpFileCount );
       //thread for processing data
       GrupPacket grpObj;
       std::thread grupingThread( &GrupPacket::startGruping,&grpObj );

       //create a thread that will call the sniffing method
       for ( size_t i= 0 ; i < tcpDumpFileCount && flag ; ++i ){
	       flag = initObj.readDumpFileName( i , dumpFileName );
	       std::cout<<"File Read:"<<dumpFileName<<std::endl;
	       if ( !flag ) break;
	       //else call libc to process tcpDump
	       snif::analysisFromFile ( dumpFileName );
       }
       Buffer *buf= new Buffer();
       buf->giveSignal();
       delete buf;

       grupingThread.join();

       /*-----------------------------------------------------------------------------
	*  interface for result
	*------------------------------
	-----------------------------------------------*/
	       rep::Report    *report=  new rep::Report();
	       bool    returnValue;
	       returnValue = report->genReport();
       if ( !returnValue ){
	       std::cerr<<PRINT<<" Error on user query\n";
       }else std::cout<<PRINT<<"check output file\n";




       return 0;

}

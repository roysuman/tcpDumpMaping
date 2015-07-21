/*
 * =====================================================================================
 *
 *       Filename:  grupPackets.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Friday 01 May 2015 02:44:11  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#include "grupPackets.hpp"

long k=0;
//static datatype
bool                GrupPacket::isInit = false;
int                 GrupPacket::packetNumber =-1;
bool                GrupPacket::isSynFin = false;
__conversation__    GrupPacket::conversationDiary[ 1000];
int                 GrupPacket::activeConversationId = -1;
bool                GrupPacket::lastDb = false;
int                 GrupPacket::fileCount = 1;
int                 GrupPacket::relativeActiveConversation = 0;
int                 GrupPacket::relativePacketNumber = 0;
GrupPacket::GrupPacket(){
	try{
		dbInstance = new DbManager();
		if ( dbInstance->isOpen() ){
			dbInstance->createTable();
		}
		for ( int i = 0 ; i < 1000 ; ++i ){
			conversationDiary [ i ].currentIndexSourceArray = -1;
			conversationDiary [ i].currentIndexDestinationArray = -1;
			conversationDiary [ i ] .sourcePreviousPackets =new __prevNetPacket__ [ 10 ];
			conversationDiary [ i ] .destinationPreviousPackets = new __prevNetPacket__ [ 10 ];
			packetNumber = -1;



		}
		isInit = true;
	}
	catch ( const std::bad_alloc &ba){
		std::cerr<<PRINT<<"Exception:"<<ba.what()<<std::endl;
		isInit = false;
	}
}
bool GrupPacket::gInit(){
	bool    returnValue;
	returnValue = true;
	try{
		for ( int i = 0 ; i < 1000 ; ++i ){
			delete conversationDiary[ i ].destinationPreviousPackets;
			delete conversationDiary [ i ].sourcePreviousPackets;
			conversationDiary [ i ].currentIndexSourceArray = -1;
			conversationDiary [ i].currentIndexDestinationArray = -1;
			conversationDiary [ i ] .sourcePreviousPackets =new __prevNetPacket__ [ 10 ];
			conversationDiary [ i ] .destinationPreviousPackets = new __prevNetPacket__ [ 10 ];
		}
		relativeActiveConversation ++;
		relativePacketNumber ++;
		relativeActiveConversation += activeConversationId;
		relativePacketNumber += packetNumber;
		fileCount++;
		packetNumber= -1;
		activeConversationId = -1;
	}
	catch ( const std::exception &e ){
		std::cerr<<"Not able to set the environmane to parse read buffer packet\n";
		returnValue = false;
		std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
	}
	return returnValue;

}
GrupPacket::~GrupPacket(){
	try{
		delete dbInstance;
		for ( int i = 0 ; i < 1000 ; ++i ){
			delete conversationDiary[ i ].destinationPreviousPackets;
			delete conversationDiary [ i ].sourcePreviousPackets;
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
	}
}

bool GrupPacket::copyPacket( bool cliSerFlag , __conversation__ *activeConversation , __rawNetData__ *packetData ){
	bool    returnValue;
#ifdef DEBUG
	std::cout<<PRINT<<"Data going to insert\n"
		<<"Sequence Number :[ "<<packetData->sequenceNumber<<" ]\n"
		<<"Acknowledge Number:["<<packetData->acknowledgeNumber<<" ] \n"
		<<"Data Length:["<<packetData->dataLength<<"] \n"
		<<"Acknowledge Flag:[ "<<packetData->networkTcpFlags.__ACK__<<"]"<<std::endl;
#endif

	returnValue = true;
	try {
		++packetNumber;

		if ( cliSerFlag ){ // this a client packet
			activeConversation->sourcePreviousPackets[ (++activeConversation->currentIndexSourceArray) ].
				sequenceNumber = packetData->sequenceNumber ;
			activeConversation->sourcePreviousPackets[ ( activeConversation->currentIndexSourceArray)].
				acknowledgeNumber = packetData->acknowledgeNumber;

			activeConversation->sourcePreviousPackets[ ( activeConversation->currentIndexSourceArray)].
				dataLength = packetData->dataLength ;

			activeConversation->sourcePreviousPackets[ ( activeConversation->currentIndexSourceArray)].
				networkTcpFlags.__ACK__  = packetData->networkTcpFlags.__ACK__;
#ifdef DEBUG
			std::cout<<PRINT<<"It's copying at client array Index:["
				<<activeConversation->currentIndexSourceArray <<" \n"<<std::endl;
#endif
		}
		else{
			activeConversation->destinationPreviousPackets[ (++activeConversation->currentIndexDestinationArray) ].
				sequenceNumber = packetData->sequenceNumber;
			activeConversation->destinationPreviousPackets[ (activeConversation->currentIndexDestinationArray) ].
				acknowledgeNumber = packetData->acknowledgeNumber;

			activeConversation->destinationPreviousPackets[ (activeConversation->currentIndexDestinationArray) ].
				dataLength = packetData->dataLength;
			activeConversation->destinationPreviousPackets[ (activeConversation->currentIndexDestinationArray) ].
				networkTcpFlags.__ACK__ = packetData->networkTcpFlags.__ACK__;
#ifdef DEBUG
			std::cout<<PRINT<<"It's copying at server array Index:["
				<<activeConversation->currentIndexDestinationArray <<" ]"<<std::endl;
#endif
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
		returnValue = false;
	}
	return returnValue;
}


bool GrupPacket::searchMother( bool cliSerFlag , __conversation__ *activeConversation , __rawNetData__ *packetData ){
	int    index;
	bool      returnValue;
	returnValue = true;
	if ( activeConversationId < zero ){ // conversationId must be >0
		std::cout<<PRINT<<"RETURNONG FALSE\n";
		returnValue = false;
	}
	try {
		if( cliSerFlag && returnValue ){
			for ( index = activeConversation->currentIndexDestinationArray ; index>= zero ; index -- ){
				if ( ( packetData->acknowledgeNumber == activeConversation->destinationPreviousPackets[ index ].
							acknowledgeNumber ) && 
						( packetData->sequenceNumber == activeConversation->destinationPreviousPackets[ index ]
						  .sequenceNumber + activeConversation->destinationPreviousPackets[ index ].dataLength ) );// returnValue = true;
				else returnValue = false;
			}
		}
		else if ( returnValue ){
			for ( index = activeConversation->currentIndexSourceArray ; index>= zero ; index -- ){
				if ( ( packetData->acknowledgeNumber == activeConversation->sourcePreviousPackets[ index ].
							acknowledgeNumber ) && ( packetData->sequenceNumber == 
								activeConversation->sourcePreviousPackets[ index ].sequenceNumber +
							       	activeConversation->sourcePreviousPackets[ index ].dataLength ) );// returnValue = true;
				else returnValue = false;
			}
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
		returnValue = false;
	}
	return returnValue;
}


bool GrupPacket::validPack(bool cliSerFlag ,  __conversation__ *activeConversation , __rawNetData__ *packetData ){
	int    index;
	bool      returnValue;

	returnValue = true;

	if ( activeConversationId < zero ) returnValue = false;
		std::cout<<PRINT<<"RETURNONG FALSE\n";
	try {
		if ( cliSerFlag  && returnValue ){
			for ( index = activeConversation->currentIndexSourceArray; index >=zero ; --index ){
				if ( ( packetData->acknowledgeNumber == activeConversation->sourcePreviousPackets[ index ].
							sequenceNumber + activeConversation->sourcePreviousPackets[ index ].
							dataLength ) && ( packetData->sequenceNumber == 
								activeConversation->sourcePreviousPackets[ index ].
								acknowledgeNumber ) ) ;//returnValue = true;
				else returnValue = false;
			}
		}
		else if ( returnValue ){
			for ( index = activeConversation->currentIndexDestinationArray; index >=zero ; --index ){
				if ( ( packetData->acknowledgeNumber == activeConversation->destinationPreviousPackets[ index ].
							sequenceNumber + activeConversation->destinationPreviousPackets[ index ].
							dataLength ) && ( packetData->sequenceNumber == 
								activeConversation->destinationPreviousPackets[ index ].
								acknowledgeNumber ) );// returnValue = true;
				else returnValue = false;
			}
		}
	}
	catch ( const std::exception &e ){
		std::cerr<<PRINT<<"Exception"<<e.what()<<std::endl;
		returnValue = false;
	}
	return returnValue;
}

bool GrupPacket::insertDb( __rawNetData__ *packetData , bool flag , bool updateFlag= false ){
	bool    returnValue;

	returnValue = false;
#ifdef DEBUG
	std::cout<<__FUNCTION__<<" Source Ip Address:[ " <<packetData->sourceIp 
		<<"] Destination Ip Address:[ "<< packetData->destinationIp
		<<" ] COnversation: [ "<<activeConversationId<<"] Packet: [ "
		<<packetNumber<<" ] SourcePOrt : [ "
		<<packetData->sourcePort<<" ]  Destination POrt:["
		<<packetData->destinationPort 
		<<" ] Time:["<<packetData->dateTime<<"]"<<std::endl;
#endif
	if ( updateFlag ){
		returnValue = 
			dbInstance->updatePacketIndex( activeConversationId + relativeActiveConversation , packetData->dateTime );
	}
	
	
  
	if ( flag ){
		 returnValue =  
			 dbInstance->insertPacketIndex( activeConversationId + relativeActiveConversation  , packetData->sourceIp  , packetData->destinationIp , packetData->dateTime , fileCount );
		if ( returnValue ){
			returnValue = 
				insertPacketInfo(  packetNumber + relativePacketNumber, activeConversationId + relativeActiveConversation  , packetData->sourceIp , packetData->destinationIp , packetData->dateTime  , packetData->sourcePort , packetData->destinationPort ,"test.com" , fileCount, packetData->dataLength );
			
		}

	}else{
			returnValue = 
				insertPacketInfo(  packetNumber + relativePacketNumber, activeConversationId + relativeActiveConversation  , packetData->sourceIp , packetData->destinationIp , packetData->dateTime  , packetData->sourcePort , packetData->destinationPort ,"test.com" , fileCount , packetData->dataLength);

	}if ( !returnValue )
			std::cerr<<PRINT<<"not able to insert data \n";

	return returnValue ;


}

bool GrupPacket::packetHandler ( __rawNetData__ * packetData ){
#ifdef DEBUG
	        std::cout<<PRINT<<" Source Ip ["<<packetData->sourceIp<<" ] Destination Ip [ "<<packetData->destinationIp
			<<"  ]\nSourcePOrt [ "<<packetData->sourcePort<<" ] \nDestinationPOrt ["
		<<packetData->destinationPort<<" ] \nSequence Number [ "<<packetData->sequenceNumber
		<<"] \n AcknowledgeNumber: [ "<<packetData->acknowledgeNumber<<"]\n Flags\n ACK [ "
		<<packetData->networkTcpFlags.__ACK__<<" ] \n SYN[ "<<packetData->networkTcpFlags.__SYN__
		<<" ] \n FIN"<<packetData->networkTcpFlags.__FIN__<<" ] Data Length:[ "
		<<packetData->dataLength<<std::endl;
#endif

	bool    returnValue;

	returnValue = true;
	if ( packetData->networkTcpFlags.__SYN__ || packetData->networkTcpFlags.__FIN__ ){
		isSynFin = true;
		return true;

	}
	if ( packetData->destinationPort == Init::configuration.port ){// 3306  ){ 
		//check whether the packet is acknowledgement packet or not
		//for acknwledgement packet length of the packet is [0] and ACK flag [ set]
		std::cout<<PRINT<<"Outgoing Packet"<<std::endl;
		if ( packetData->dataLength ==0 && packetData->networkTcpFlags.__ACK__  ){
			if ( isSynFin  ){   return true; }
			std::cout<<PRINT<<" ACK FLAG ON-- OUTGOING \n";
			if ( validPack ( false , &conversationDiary [ activeConversationId ] , packetData ) ){
				std::cout<<PRINT<<"VALID ACK -- OUTGOING PACKET \n";
				//insertData into database ( true , activeConversationId , ++packetNumber , packetData );
				returnValue = copyPacket ( true , &conversationDiary [ activeConversationId ] , packetData );
				
				returnValue == true ?returnValue = insertDb( packetData , false , true ): returnValue = false;
				
				
			}else{
				std::cout<<PRINT<<" ACK PACKET NOT VALID--- OUTGOING\n";
				returnValue = false;

			}
			
		}else if ( searchMother ( false , &conversationDiary [ activeConversationId ] , packetData ) && !lastDb ){
			std::cout<<PRINT<<" CONSICUTIEVE PACKETS-- OUTGOING \n";
			returnValue = copyPacket( true , &conversationDiary[ activeConversationId ], packetData );
			if ( packetData->networkTcpFlags.__ACK__ )
			returnValue == true ?returnValue = insertDb( packetData , false , true ):returnValue = false ;
			returnValue == true ?returnValue = insertDb( packetData , false ):returnValue = false ;
			//insert the packet
		}
	       //it's a new packet--start conversation
	       else  { 
		       std::cout<<"It's a fresh Outgoing Packet"<<std::endl;
		       lastDb = false;
		  //     packetNumber = 0;
		        
		       returnValue = copyPacket( true , &conversationDiary [ ++activeConversationId ] , packetData );
		       returnValue == true ?returnValue = insertDb( packetData , true ):returnValue = false ;
		       	//insertpacketSetting( true , ++packet);
	       }
	}else if ( packetData->sourcePort ==  Init::configuration.port  ){// 3306 ){
		std::cout<<PRINT<<" IN COMING PACKET \n";
		if (   packetData ->dataLength == 0 && packetData->networkTcpFlags.__ACK__  ) {
			if ( isSynFin ){  return true;}
			std::cout<<PRINT<<"IT;S ACK PACKET -- IN COMING \n";
			if (  validPack ( true , &conversationDiary [ activeConversationId] , packetData ) ){
			std::cout<<"VALID ACK RESPONSE PACKET -- IN COMING"<<std::endl;
			returnValue = copyPacket( false , &conversationDiary [ activeConversationId ] , packetData );
			returnValue == true ?returnValue = insertDb( packetData , false , true ):returnValue = false ;
			}else{
				std::cerr<<PRINT<<" ACK PACKET NOT VALID-- IN COMING\n";
				returnValue = false;
			}

		}else if ( searchMother ( true , &conversationDiary [activeConversationId ] , packetData ) ){
			std::cout<<PRINT<<"CONTIMNIOUS PACKET--- IN COMING"<<std::endl;
			returnValue = copyPacket( false , &conversationDiary [ activeConversationId ], packetData );
			if ( packetData->networkTcpFlags.__ACK__ )
			returnValue == true ?returnValue = insertDb( packetData , false , true ):returnValue = false ;
			else

			returnValue == true ?returnValue = insertDb( packetData , false ):returnValue = false ;
		} // after sending a request client may get a request response without ACK
	       	else {// freash packet
			std::cout<<"FRESH PACKET -- INCOMING"<<std::endl;
		//	packetNumber = 0;
		        if( isSynFin ) lastDb = true;

			returnValue = copyPacket ( false , &conversationDiary[ ++activeConversationId ] , packetData );
			returnValue == true ?returnValue = insertDb( packetData , true):returnValue = false ;
		}
	}
	isSynFin = false;
	return returnValue;
}
void GrupPacket::startGruping( void ){
	
#ifdef DEBUG
	std::cout<<PRINT<<"Reading and processing packet from shared Buffer\n";
#endif
	int    readCount;

	readCount = 0;

	Buffer            *readBufferObj;
	__rawNetData__    *packet;
	bool              flag;

	flag = false;
	try{
		readBufferObj = new Buffer();
		packet = new __rawNetData__();
	}
	catch ( const std::bad_alloc &ba ){
		std::cerr<<PRINT<<"Exception:"<<ba.what()<<std::endl;
	}
	int readReturn;
	while ( !flag ){
		readReturn = readBufferObj->readBuffer( 1 , packet ) ;
		std::cout<<PRINT<<"read Return:[ "<<readReturn<<" ] \n";
		switch ( readReturn ){
			case 1:
#ifdef ERROR
				std::cout<<PRINT<<"Read PacketSource POOrt:[ "<<packet->sourcePort
					<<" ]:: Destination Port:[ "<<packet->destinationPort<<" ]"<<std::endl;
#endif
				readCount++;
				switch ( packet->sourcePort ){
					case 0: 

						flag = !gInit();

						break;
					default:
						if (! packetHandler ( packet ) ){
							std::cerr<<PRINT<<"Error on packet grouping\n";
							flag = true;
						}
				}
				//	readBufferObj->giveSignal();

				//	flag = true;
				
				break;
			case -1:
				std::cerr<<PRINT<<"Error on read packet from buffer";
				exit(1);
				break;
			case 0:
				flag = readBufferObj->checkSignal();
				std::this_thread::sleep_for ( std::chrono::milliseconds( 100 ) );
				break;
			default:
				break;
				//std::cout<<PRINT<<"Unhandled return Value from read Buffer \n";
		}
	}
	std::cout<<PRINT<<"Total packet read from buffer: [ "<<readCount<< " ] \n";
}

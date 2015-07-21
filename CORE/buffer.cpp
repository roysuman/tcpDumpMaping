/*
 * =====================================================================================
 *
 *       Filename:  buffer.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  Friday 01 May 2015 10:44:10  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#include"buffer.hpp"
size_t            Buffer::buffObj = 0;
std::mutex        Buffer::bufferMutex;
int               Buffer::readHead = -1;
int               Buffer::writeHead = -1;
bool              Buffer::signal = false;
__rawNetData__    *Buffer::buffer = NULL;
bool              Buffer::bufferInit = false;
size_t            Buffer::bufferReadCount = 0;
size_t            Buffer::bufferWriteCount= 0;
 pthread_mutex_t lock;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Buffer
 *  Description:  constructor for the buffer class. It's innitiate the buffer shared 
 *                memory. Mutex lock protected.
 * =====================================================================================
 */
Buffer::Buffer(){

#ifdef DEBUG
	std::cout<<PRINT<<"Creating BUffer obj\n";
#endif
	pthread_mutex_lock(&lock);
//	std::lock_guard<std::mutex> guard(bufferMutex);
	if ( !bufferInit ){
		try{
			buffer = new __rawNetData__[ BUFFER_SIZE];
			bufferInit = true;

//		buffObj = 0;
		}
		catch ( const std::bad_alloc &ba ){
			std::cerr<<PRINT<<"Exception:"<<ba.what()<<std::endl;
			bufferInit = false;
		}
	}
	buffObj ++;
	 pthread_mutex_unlock(&lock);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  initBuffer
 *  Description:  method that getting called by the constructor. Which actually allocates
 *                the buffer memory.
 * =====================================================================================
 */
bool Buffer::initBuffer(){
	bool    returnValue;

	pthread_mutex_lock(&lock);
//	std::lock_guard<std::mutex> guard(bufferMutex);
	returnValue = true;
	try{
		buffer = new __rawNetData__[ BUFFER_SIZE];
//		buffObj = 0;
	}
	catch ( const std::bad_alloc &ba ){
		std::cerr<<PRINT<<"Exception:"<<ba.what()<<std::endl;
		returnValue = false;
	}
	bufferInit = returnValue;
	pthread_mutex_unlock(&lock);
	return returnValue;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ~Buffer
 *  Description:  destructor for the buffer class. That frees the buffer memory( condition
 *                object reference must be 0 ).
 * =====================================================================================
 */
Buffer::~Buffer(){
	pthread_mutex_lock(&lock);
//	std::lock_guard<std::mutex> guard(bufferMutex);
	buffObj--;
	if ( buffObj == 0 ){
		try{
			std::cout<<PRINT<<"Deleting Buffer obj\n";
//			delete []buffer;
		}
		catch ( const std::exception &e ){
			std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
		}
	} else std::cout<<PRINT<<"Buffer Obj Count: [ "<<buffObj<<" ] \n";
	 pthread_mutex_unlock(&lock);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  writeBuffer
 *  Description:  write __rawData__ structure's object into the buffer.
 *                lock Protected. It's a circular buffer
 * =====================================================================================
 */
int Buffer::writeBuffer( __rawNetData__ *&packet ){
	std::cout<<PRINT<<"write data in Buffer\n";
	int    returnValue;
	pthread_mutex_lock(&lock);

//	std::lock_guard<std::mutex> guard(bufferMutex);
	returnValue = 1;
#ifdef DEBUG
	std::cout<<PRINT<<"Buffer Read head Position :[ "<<readHead<<std::endl;
	std::cout<<PRINT<<"Buffer Write Head Position:[ "<<writeHead<<std::endl;
#endif
	if ( bufferInit ){
		if ( writeHead == -1 ){
			++writeHead;
			++readHead;
		}else{ 
			if ( writeHead == BUFFER_SIZE -1 ){
				if ( readHead == 0 ) returnValue = 0;
				else writeHead = 0;
				
			}else ++writeHead;
		}
		if ( returnValue == 1 ){
#ifdef DEBUG
			buffer [ writeHead ].sourcePort = packet->sourcePort;
			buffer [ writeHead ].destinationPort = packet->destinationPort;
			buffer [ writeHead ].sequenceNumber = packet->sequenceNumber;
			buffer [ writeHead].acknowledgeNumber = packet->acknowledgeNumber;
			buffer [ writeHead ].dataLength = packet->dataLength;
			buffer [ writeHead ].sourceIp= packet->sourceIp.c_str();
			buffer [ writeHead ].destinationIp = packet->destinationIp.c_str();

			buffer [ writeHead ].dateTime = packet->dateTime.c_str();
			buffer[ writeHead ].networkTcpFlags.__ACK__ = packet->networkTcpFlags.__ACK__ ;
			buffer[ writeHead ].networkTcpFlags.__SYN__ = packet->networkTcpFlags.__SYN__ ;
			buffer[ writeHead ].networkTcpFlags.__FIN__ = packet->networkTcpFlags.__FIN__ ;

			buffer[ writeHead ].networkTcpFlags.__PSH__ = packet->networkTcpFlags.__PSH__ ;

			buffer[ writeHead ].networkTcpFlags.__RST__ = packet->networkTcpFlags.__RST__ ;

			buffer[ writeHead ].networkTcpFlags.__URG__ = packet->networkTcpFlags.__URG__ ;
#endif

//			memcpy ( (void*)(buffer + writeHead  ) ,(void*)packet , sizeof ( __rawNetData__ ) );
#ifdef ERROR
			__rawNetData__ *pp =  new __rawNetData__;
			memcpy( pp , buffer + writeHead , sizeof ( __rawNetData__ ) );
			std::cout<<"\nip;"<<pp->sourceIp<<std::endl;
			delete pp;

#endif
			bufferWriteCount++;
		}
	}else returnValue = -1;
	pthread_mutex_unlock(&lock);
	return returnValue;
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  readBuffer
 *  Description:  read a __rawNetData__ element from the buffer into the reference variable
 *                 Lock protected, and circular.
 * =====================================================================================
 */
int Buffer::readBuffer( size_t noOfPackets , __rawNetData__ *& packet ){
	int    returnValue;
	pthread_mutex_lock(&lock);
//	std::lock_guard<std::mutex> guard(bufferMutex);
	noOfPackets = 1;

#ifdef DEBUG
	std::cout<<PRINT<<"Buffer write Count :[ "<<readHead<<std::endl;
	std::cout<<PRINT<<"Buffer Read Count:[ "<<writeHead<<std::endl;
#endif

	returnValue = 1;
	if ( bufferInit ){
		if ( readHead == -1 ) returnValue = 0;
		else {
			packet->sourcePort = buffer [ writeHead ].sourcePort ;
		        packet->destinationPort=buffer [ writeHead ].destinationPort ;
			packet->sequenceNumber =buffer [ writeHead ].sequenceNumber ;
			packet->acknowledgeNumber= buffer [ writeHead].acknowledgeNumber; 
			packet->dataLength = buffer [ writeHead ].dataLength ;
			packet->sourceIp = buffer [ writeHead ].sourceIp.c_str();
			packet->destinationIp = buffer [ writeHead ].destinationIp.c_str();

			packet->dateTime = buffer [ writeHead ].dateTime .c_str();
		        packet->networkTcpFlags.__ACK__ =buffer[ writeHead ].networkTcpFlags.__ACK__ ;
		        packet->networkTcpFlags.__SYN__ = buffer[ writeHead ].networkTcpFlags.__SYN__ ;
			packet->networkTcpFlags.__FIN__  = buffer[ writeHead ].networkTcpFlags.__FIN__ ;

			packet->networkTcpFlags.__PSH__  = buffer[ writeHead ].networkTcpFlags.__PSH__ ;

			 packet->networkTcpFlags.__RST__= buffer[ writeHead ].networkTcpFlags.__RST__ ;

			packet->networkTcpFlags.__URG__= buffer[ writeHead ].networkTcpFlags.__URG__ ;

		//	memcpy ((void*) packet ,(void*)( buffer+ readHead )  , sizeof ( __rawNetData__  )   );
#ifdef ERROR
			__rawNetData__ *pp =  new __rawNetData__;
			memcpy( pp , buffer + readHead , sizeof ( __rawNetData__ ) );
			std::cout<<"\nip;"<<pp->sourceIp<<std::endl;
			exit(0);
#endif			
			bufferReadCount++;
			if ( readHead == writeHead ){
				readHead = -1;
				writeHead = -1;
			}else if ( readHead == BUFFER_SIZE -1 )
				readHead = 0;
			else ++readHead;
		}
	}else returnValue = -1;
	 pthread_mutex_unlock(&lock);
	return returnValue;
}
bool Buffer::giveSignal(){
	bool    returnValue;
	pthread_mutex_lock(&lock);

	returnValue = true;

//	std::lock_guard<std::mutex> guard(bufferMutex);
	signal = true;
	 pthread_mutex_unlock(&lock);
	return returnValue;
}

bool Buffer::checkSignal( ){
	bool    returnValue;
	pthread_mutex_lock(&lock);
	std::lock_guard<std::mutex> guard(bufferMutex);
	returnValue = signal;
	 pthread_mutex_unlock(&lock);
	return ( returnValue );
}


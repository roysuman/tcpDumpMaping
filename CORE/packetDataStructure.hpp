#ifndef PACKETDATASTRUCTURE_HPP
#define PACKETDATASTRUCTURE_HPP 
#include<iostream>
#include<stdlib.h>
#include<string>
#include <unistd.h>
#include<stdint.h>
#define PRINT __DATE__<<"_"<<__TIME__<<" [ "<<__FILE__<<" ] [ "<<__FUNCTION__<<" ] [ "<<__LINE__<<" ] "
#define DEBUG 1
//each tcp packet( network packet ) has some flags that helps to understand the type of packet. 
//        URG (1 bit) – indicates that the Urgent pointer field is significant
//        ACK (1 bit) – indicates that the Acknowledgment field is significant. All packets after the initial SYN packet sent by the client should have this flag set.
//        PSH (1 bit) – Push function. Asks to push the buffered data to the receiving application.
//        RST (1 bit) – Reset the connection
//        SYN (1 bit) – Synchronize sequence numbers. Only the first packet sent from each end should have this flag set. Some other flags change meaning based on this flag, and some are only valid for when it is set, and others when it is clear.
//        FIN (1 bit) – No more data from sender


typedef struct _netFlags_ __netFlags__;
struct _netFlags_{
	bool    __URG__;
	bool    __ACK__;
	bool    __PSH__;
	bool    __RST__;
	bool    __SYN__;
	bool    __FIN__;
};
typedef struct _rawNetData_ __rawNetData__;
struct _rawNetData_{
	std::string     sourceIp;
	std::string     destinationIp;
	uint16_t        sourcePort;
	uint16_t        destinationPort;
	uint32_t        sequenceNumber;
	uint32_t        acknowledgeNumber;
	__netFlags__    networkTcpFlags;
//	const u_char    *data;
	uint32_t        dataLength;
	std::string     dateTime;

};
//all packets ar communicating between a client and a server, it's mainly a client server model.
//a client is sending a query to the server and server sending back the result set to the client.
//the query being sebd by the client or the response from server end may contain 1/2.. tcp packets. We are grooping this communication between 
//client and server by a unique conversation id.The following structure helps to perform the grooping of packets under a unique conversation id. 
typedef struct _prevNetPacket_ __prevNetPacket__;
struct _prevNetPacket_{
	uint32_t        sequenceNumber;
	uint32_t        acknowledgeNumber;
	uint32_t        dataLength;
	__netFlags__    networkTcpFlags;
};
//group all packets under a unique conversation id
typedef struct _conversation __conversation__;
struct _conversation{
	size_t    conversationId;
	__prevNetPacket__    *sourcePreviousPackets;//source meane... my machine
	__prevNetPacket__    *destinationPreviousPackets;
	size_t               currentIndexSourceArray;
	size_t               currentIndexDestinationArray;
};

//__conversation__ converationDiary[ 1000];



#endif /* PACKETDATASTRUCTURE_HPP */

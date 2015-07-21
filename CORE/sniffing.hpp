
/*
 * =====================================================================================
 *
 *       Filename:  sniffing.hpp
 *
 *    Description:  alalyse tcpdump file
 *        Version:  1.0
 *        Created:  ThursDay 30 April 2015 08:02:02  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Suman Roy  (email.suman.roy@gmail.com),
 *   Organization:  Cognizant Technological SOlutions
 *
 * =====================================================================================
 */



#ifndef SNIFFING_HPP_
#define SNIFFING_HPP_
#include<pcap.h>
#include<iostream>
#include<stdlib.h>
#include<stdint.h>
#include <stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<net/ethernet.h>
#include<netinet/ip_icmp.h>
#include<netinet/udp.h>   //Provides declarations for udp header
#include<netinet/tcp.h>   //Provides declarations for tcp header
#include<netinet/ip.h>    //Provides declarations for ip header
#include<fstream>
#include<signal.h>
#include<iomanip>
#include <pthread.h>
#include<iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include<thread>
#include "packetDataStructure.hpp"
#include "buffer.hpp"
#include "init.hpp"
/*
#define ntohl32(p)   ((unsigned int)*((const unsigned char *)(p)+0)<<24|  \
		                     (unsigned int)*((const unsigned char *)(p)+1)<<16|  \
		                     (unsigned int)*((const unsigned char *)(p)+2)<<8|   \
		                     (unsigned int)*((const unsigned char *)(p)+3)<<0)
*/
//global instance for databaseHandler.
extern Buffer       *__bufHndler__; // violating data encryption
#define tflag -3
extern bool __flagCapture__, __flagStop__;
namespace snif{
	extern bool ackInit;
	extern bool seqInit;
	extern uint32_t    delta;
	extern int32_t thiszone;		/* seconds offset from gmt to local time */
	extern int successCount;
	extern  struct sockaddr_in stSource;
	extern struct sockaddr_in stDest;
	extern  pcap_t * pHandle;
	extern size_t __iTcp__,__iUdp__,__iIcmp__,__iOthers__,__iIgmp__,__iTotal__;
	extern bool processPacket( const u_char * , struct pcap_pkthdr );

	extern bool processTcpPacket ( const u_char * , struct pcap_pkthdr );
	extern void printData ( const u_char * , size_t );
	extern void signalCallbackHandler( int  );
        extern void  stopSniffing( );
	extern bool analysisFromFile( std::string  );
	extern std::string getTime( register const struct timeval *tvp );
	extern int32_t gmt2local(time_t t);

}
#endif /* SNIFFING_HPP */

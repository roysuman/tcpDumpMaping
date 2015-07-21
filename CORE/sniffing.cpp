/*
 * =====================================================================================
 *
 *       Filename:  sniffing.cpp
 *
 *    Description:  defination of header file sniffing.hpp
 *
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
#include "sniffing.hpp"

struct sockaddr_in    snif::stDest;
int32_t               snif::thiszone;		/* seconds offset from gmt to local time */
pcap_t                *snif::pHandle;
Buffer                *__bufHndler__;
struct sockaddr_in    snif::stSource;
uint32_t              snif::delta = 0;
size_t                snif::__iTcp__ = 0;
size_t                snif::__iUdp__ = 0;
size_t                snif::__iIcmp__ = 0;
size_t                snif::__iIgmp__ = 0;
size_t                snif::__iTotal__ = 0;
bool                  __flagStop__ = false;
size_t                snif::__iOthers__ = 0;
bool                  snif::ackInit = false;
bool                  snif::seqInit = false;
int                   snif::successCount = 0;
bool                  __flagCapture__ = false;
/* 
 * ===  FUNCTION  ======================================================================
 *          Name:  signalCallbackHandler
 *          *  Description:  signal handling for stoping the process
 * =====================================================================================
 */

void snif::signalCallbackHandler( int iSignum){

	std::cout<<PRINT<<"] Signal received-"<<iSignum<<std::endl;
	exit(iSignum);
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  processTcpPacket
 *  Description:  print tcp packets dield info
 * =====================================================================================
 */


bool snif::processTcpPacket( const u_char *ucBuffer , struct pcap_pkthdr pktHeader   ){
	unsigned short               usIphdrlen;
	size_t                       headerSize;
	struct iphdr                 *stIpHeader;
	bool                         returnValue = true;
	__rawNetData__               *rawData;
	size_t                       iSize;

	iSize = pktHeader.len;
	stIpHeader = ( struct iphdr * ) 
		( ucBuffer + sizeof ( struct ethhdr ) );
	usIphdrlen = stIpHeader->ihl * 4 ;
	struct tcphdr *stTcpHeader = ( struct tcphdr * ) 
		( ucBuffer  + (stIpHeader->ihl * 4 ) +  sizeof ( struct ethhdr ) );
	headerSize= sizeof ( struct ethhdr ) + usIphdrlen + 
		( stTcpHeader->doff * 4 );
#ifdef DEBUG
	std::cout<<"SOurce Ip ADDRES:"<<
		inet_ntoa( *(struct in_addr *) &stIpHeader->saddr)<<std::endl;
	std::cout<<"Destination Ip Address"<<
		inet_ntoa( *(struct in_addr *) &stIpHeader->daddr);
	std::cout<<"Length of IP data:"<<
		ntohs( stIpHeader->tot_len ) - sizeof ( struct iphdr )<<std::endl;
#endif

	try {
		rawData = new __rawNetData__();
	}
	catch ( const std::bad_alloc &e){
		std::cerr<<PRINT<<"Exception:"<<e.what() <<std::endl;
		returnValue = false;
	}
	if ( returnValue ){
		//get the dateTime from the packet header
		rawData->dateTime = getTime(&pktHeader.ts);
		rawData->sourceIp = 
			std::string( inet_ntoa( *(struct in_addr *) &stIpHeader->saddr) );
		rawData->destinationIp = 
			std::string ( inet_ntoa( *(struct in_addr *) &stIpHeader->daddr) );
		rawData->sourcePort = ntohs( stTcpHeader->source );
		rawData->destinationPort = ntohs ( stTcpHeader->dest) ;
		if ( !seqInit ){
			delta =  ntohl ( stTcpHeader->seq );
			seqInit = true;
		}
		rawData->sequenceNumber =
			(uint32_t) ntohl( stTcpHeader->seq ) -delta;// /- 33278;// ( stTcpHeader->seq ) ;
		rawData->acknowledgeNumber=
			ntohl( stTcpHeader->ack_seq ) - delta;
		rawData->dataLength =  (iSize - headerSize );
		( ( unsigned int )stTcpHeader->psh ) == 1 ? 
			rawData->networkTcpFlags.__PSH__ = true 
			:  rawData->networkTcpFlags.__PSH__ = false ;
		( ( unsigned int ) stTcpHeader->ack ) == 1 ?
			rawData->networkTcpFlags.__ACK__ = true
			: rawData->networkTcpFlags.__ACK__ = false ;
	/* 	if( ( iSize - headerSize) > 0 ){
			rawData->data = new  u_char [ rawData->dataLength];
			memcpy ( ( u_char* ) ( rawData->data + 0 ) , 
					( u_char * )( ucBuffer + headerSize ) , 
					rawData->dataLength );
		}*/
		( unsigned int )stTcpHeader->syn  == 1 ? 
			rawData->networkTcpFlags.__SYN__ = true :
			rawData->networkTcpFlags.__SYN__ = 0;
		( unsigned int ) stTcpHeader->fin  ==  1 ? 
			rawData->networkTcpFlags.__FIN__ = true : 
			rawData->networkTcpFlags.__FIN__ = false;
		
#ifdef DEBUG
		std::cout<<PRINT<<"__PSH__ flag [ "<<rawData->networkTcpFlags.__PSH__
			<<" ] source port [ "<<rawData->sourcePort<< " ]destination port [ "
			<<rawData->destinationPort<<"] sequence number ["
			<<(int)rawData->sequenceNumber<<" ]Acknowledge number["
			<<(int)rawData->acknowledgeNumber<<"]Acknowledge flag [ "
			<<rawData->networkTcpFlags.__ACK__
			<<"] SYN flag:["<<rawData->networkTcpFlags.__SYN__
			<<"] FIN flag:["<<rawData->networkTcpFlags.__FIN__
			<<"] Size is :["<<iSize - headerSize <<std::endl;
		
#endif
		try{
			while (  __bufHndler__->writeBuffer ( rawData )  == 0 && returnValue  ){
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//	returnValue =  !__bufHndler__->checkSignal() ;//check whether grouping thread raised and signal to terminate pcap?
				//close pcap
			}
			//delete []rawData->data;
			delete rawData;
		}
		catch ( std::exception &ea ){
			std::cerr<<PRINT<<" Exception:"<<ea.what()<<std::endl;
			returnValue = false;
		}
	}
	return returnValue;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  printData
 *  Description:  print char by char
 * =====================================================================================
 */
void snif::printData ( const u_char *ucData , size_t iSize ) {

	for ( size_t i=0 ; i < iSize ; ++ i ){
		if ( i!= 0 && i % 16 == 0 ){ //if one line of hex print os complete.....

			std::cout<<"        ";
			for ( size_t j= i - 16 ; j < i ; ++ j ){
				if ( ucData [ j ] >=32 && ucData [ j ] <= 128 )
					std::cout<<( unsigned char ) ucData [ j ] ;//if it's a number or alphabet
				else
					std::cout<<".";// else print a dot
			}
		}
		if ( i % 16 == 0 ) 
			std::cout<<std::hex<<( unsigned int ) ucData [ i ] ;
		if ( i == iSize - 1 ){ //print the last space
			for ( size_t j= 0 ; j < 15 - i % 16 ; ++ j )
				std::cout<<"]"<<"   ";//extra space
			std::cout<<"       ";
			for ( size_t j= i - i % 16 ; j <=i ; ++ j){
				if ( ucData [ j ] >= 32 && ucData [ j ] <=128 )
					std::cout<<( unsigned char ) ucData [ j ] ;
				else
					std::cout<<".";
			}
		}
	}
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  stopSniffing
 *  Description:  stop the capturing packet operation.
 * =====================================================================================
 */

void snif::stopSniffing ( ){
	__flagCapture__ = false;
	while ( __flagStop__ );
	try {
		pcap_close( pHandle );
		__flagStop__ = true;
	}
	catch ( std::exception &e ){
		std::cerr<<PRINT<<"Exception:"<<e.what()<<std::endl;
	}
}


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  analysisFromFile
 *  Description:  dissect and learn from the already captured tcpdump file
 * =====================================================================================
 */

bool snif::analysisFromFile ( std::string fileName ){
#ifdef DEBUG
	std::cout<<PRINT<<"Reading file:[ "<<fileName<<" ] "<<std::endl;
#endif

	pcap_t                *pcap;
	struct pcap_pkthdr    header;
	const u_char          *packet;
	struct iphdr          *stIpHeader;
	bool                  returnValue;
	char                  errbuf[PCAP_ERRBUF_SIZE];

	returnValue = true;
	
	pcap = pcap_open_offline( fileName.c_str() , errbuf);
	if (pcap == NULL){
		std::cerr<<PRINT<<"error reading pcap file: \n"<< errbuf<<std::endl;
		returnValue = false;
	}
	if ( returnValue ){
		try{
			__bufHndler__ = new Buffer();
			while ( ( packet = pcap_next(pcap, &header ) ) != NULL && returnValue  ){
				stIpHeader = ( struct iphdr * ) ( packet + sizeof ( struct ethhdr ) );
				if ( std::string(inet_ntoa ( *(struct in_addr * )&stIpHeader->saddr) )== Init::configuration.ip ||  std::string(inet_ntoa (*(struct in_addr * ) &stIpHeader->daddr) )== Init::configuration.ip ) {
					returnValue = snif::processPacket( packet,  header );//caplen
				}
			}

			__rawNetData__    *newFileNextIndicator = new __rawNetData__();
			newFileNextIndicator->sourcePort = 0;

			while (  __bufHndler__->writeBuffer ( newFileNextIndicator )  == 0 ){
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			delete newFileNextIndicator;
			pcap_close( pcap );
			delete __bufHndler__;
		}
		catch ( std::exception &ea ){
			std::cerr<<PRINT<<" Exception:"<<ea.what()<<std::endl;
			returnValue = false;
		}
	}
	return returnValue;
}
//the following code coppied form
//http://old.honeynet.org/scans/scan22/sol/submission/dump.c.txt

/*
 * Returns the difference between gmt and local time in seconds.
 * Use gmtime() and localtime() to keep things simple. (Stolen
 * verbatim from tcpdump.)
 */
int32_t snif::gmt2local(time_t t)
{
	register int          dt;
	register int          dir;
	register struct tm    *gmt;
	register struct tm    *loc;
	struct tm             sgmt;

	if (t == 0)
		t = time(NULL);
	gmt = &sgmt;
	*gmt = *gmtime(&t);
	loc = localtime(&t);
	dt = (loc->tm_hour - gmt->tm_hour) * 60 * 60 +
	    (loc->tm_min - gmt->tm_min) * 60;

	/*
	 * If the year or julian day is different, we span 00:00 GMT
	 * and must add or subtract a day. Check the year first to
	 * avoid problems when the julian day wraps.
	 */
	dir = loc->tm_year - gmt->tm_year;
	if (dir == 0)
		dir = loc->tm_yday - gmt->tm_yday;
	dt += dir * 24 * 60 * 60;

	return (dt);
}
std::string  snif::getTime (register const struct timeval *tvp ){
	register int       s;
	struct tm          *tm;
	time_t             Time;
	static unsigned    b_sec;
	static unsigned    b_usec;
	std::string date;
	switch(tflag) {
		case 1: /* Default */
			s = (tvp->tv_sec + thiszone) % 86400;
			(void)printf("%02d:%02d:%02d.%06u ",s / 3600, (s % 3600) / 60, s % 60,(unsigned)tvp->tv_usec);
			break;
		case -1: /* Unix timeval style */
			(void)printf("%u.%06u ",(unsigned)tvp->tv_sec,(unsigned)tvp->tv_usec);
			break;
		case -2:
			if (b_sec == 0) {
				printf("000000 ");
			} else {
				int d_usec = tvp->tv_usec - b_usec;
				int d_sec = tvp->tv_sec - b_sec;
				
				while (d_usec < 0) {
					d_usec += 1000000;
					d_sec--;
				}
				if (d_sec)
					printf("%d. ", d_sec);
				printf("%06d ", d_usec);
			}
			b_sec = tvp->tv_sec;
			b_usec = tvp->tv_usec;
			break;
			
		case -3: /* Default + Date*/
			s = (tvp->tv_sec + thiszone) % 86400;
			Time = (tvp->tv_sec + thiszone) - s;
			tm  = gmtime (&Time);
			std::stringstream    dateStream;
			
			dateStream.width(2);
			dateStream<<std::setfill('0')<<tm->tm_mon +1;
			date = dateStream.str();
			dateStream.str("");

			dateStream.width(2);
			dateStream<<std::setfill('0')<<tm->tm_mday ;
			date =date+ "-" +  dateStream.str();
			dateStream.str("");

			dateStream.width(4);
			dateStream<<std::setfill('0')<<tm->tm_year + 1900 ;
			date = date + "-" +  dateStream.str();
			dateStream.str("");

			dateStream.width(2);
			dateStream<<std::setfill('0')<<( s / 3600 ) ;
			date =date + " "+dateStream.str();
			dateStream.str("");

			dateStream.width(2);
			dateStream<<std::setfill('0')<<( ( s% 3600 ) / 60 ) ;
			date = date + ":" +dateStream.str();
			dateStream.str("");

			dateStream.width(2);
			dateStream<<std::setfill('0')<<( s % 60 ) ;
			date = date +":" +dateStream.str();
			dateStream.str("");

			dateStream.width(10);
			dateStream<<std::setfill('0')<<((unsigned)tvp->tv_usec)  ;
			date =date +"."+ dateStream.str();
			return date;

		//	date =  std::to_string (tm->tm_year+1900 )+"-"+std::to_string (tm->tm_mon+1)+"-"+std::to_string ( tm->tm_mday)+"-"+" " +std::to_string (s / 3600)+":"+std::to_string( (s % 3600) / 60) +":"+ std::to_string (s % 60)+"."+ std::to_string ( (unsigned)tvp->tv_usec) ;

			//date = std::to_string (tm->tm_mon+1) +std::to_string ( tm->tm_mday) + std::to_string (tm->tm_year+1900 ) +std::to_string (s / 3600)+std::to_string( (s % 3600) / 60) + std::to_string (s % 60)+ std::to_string ( (unsigned)tvp->tv_usec) ;
			break;
	}
	return date;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  processPacket
 *  Description:  process the packet.
 * =====================================================================================
 */

bool snif::processPacket ( const u_char* ucBuffer , struct pcap_pkthdr pktHeader ){
	std::cout<<PRINT<<"\nProcessPacket\n";

/*	size_t         length;

	length = pktHeader.len;
*/

	struct iphdr    *stIph;
	bool            returnValue;

	returnValue = true;

	stIph = ( struct iphdr * ) ( ucBuffer +	sizeof ( struct ethhdr ) );
	++ __iTotal__ ;
	switch ( stIph->protocol ){ // check the protocol and take action depending on protocol
		case 1: //ICMP protocol
                ++__iIcmp__;
                break;
				
            case 2: // IGMP protocol
                ++__iIgmp__;
                break;
				
            case 6: //TCP protocol , this protocol we need to sniff for database port
                ++__iTcp__;
                 returnValue = snif::processTcpPacket ( ucBuffer , pktHeader ) ;
                break;
				
            case 17: // UDP protocol
                ++ __iUdp__;
                break;
				
            default: // some other protocol like ARP etc..
                ++__iOthers__;
                break;
				
        }//end of switch
#ifdef DEBUG
	std::cout<<PRINT<<"\nTCP:"<<__iTcp__<<"\t UDP: "
		<<__iUdp__<<"\tICMP:"<<__iIcmp__
		<<"\tGMP:"<<__iIgmp__
		<<"\tOthers:"<<__iOthers__
		<<"\tTotal:"<<__iTotal__<<std::endl;
#endif
	return returnValue; 
}
	

/*
 * =====================================================================================
 *
 *       Filename:  grupPackets.hpp
 *
 *    Description:  grup the packet
 *
 *        Version:  1.0
 *        Created:  Friday 01 May 2015 02:37:42  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#ifndef GRUPPACKET_HPP_
#define GRUPPACKET_HPP_
#include<iostream>
#include<stdlib.h>
#include "init.hpp"
#include "buffer.hpp"
#include "packetDataStructure.hpp"
#include "databaseManagement.hpp"
#define zero 0
extern long k;

class GrupPacket: public DbManager {
	private:
		static bool                isInit;
		static bool                lastDb;
		static int                 fileCount;
		static bool                isSynFin;
		static __conversation__    conversationDiary[1000];
		static int                 packetNumber;
		static int                 activeConversationId;
		DbManager                  *dbInstance;
		static int                 relativeActiveConversation;
		static int                 relativePacketNumber;


	public:
		GrupPacket();
		~GrupPacket();
		bool gInit();
		void startGruping( void);
		bool packetHandler( __rawNetData__ * );
		bool copyPacket( bool , __conversation__ * , __rawNetData__ * );
		bool searchMother( bool , __conversation__ * , __rawNetData__ * );
		bool validPack( bool , __conversation__ * , __rawNetData__  *);
		bool insertDb( __rawNetData__ * , bool, bool );
		//bool checkSimultaniousNewPacket( bool , __conversation__ * , __rawNetData__ *);
		//bool checkGreeting( 	__conversation__ *, __rawNetData__ * );


};
/*
class ParsePacket{
	public:
		bool sendData
};
*/	       	


#endif

/*
 * =====================================================================================
 *
 *       Filename:  buffer.hpp
 *
 *    Description:  buffer management for shared data between two threades
 *
 *        Version:  1.0
 *        Created:  Friday 01 May 2015 10:37:40  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#ifndef BUFFER_HPP_
#define BUFFER_HPP_
#include<iostream>
#include <thread>
#include <list>
#include <algorithm>
#include <mutex>
#include<string.h>
#include "packetDataStructure.hpp"
extern pthread_mutex_t lock;
#define BUFFER_SIZE 1
class Buffer{
	private:
		static bool              signal;
		static __rawNetData__    *buffer;
		static size_t            buffObj;
		static int               readHead;
		static int               writeHead;
		static bool              bufferInit;
		static std::mutex        bufferMutex;
		static size_t            bufferReadCount;
		static size_t            bufferWriteCount;
	public:
		Buffer();
		~Buffer();
		bool initBuffer();
		bool giveSignal();
		bool checkSignal();
		int writeBuffer( __rawNetData__ *& );
		int readBuffer( size_t , __rawNetData__*& );

};

#endif

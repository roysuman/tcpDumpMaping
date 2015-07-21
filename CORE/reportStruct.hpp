/*
 * =====================================================================================
 *
 *       Filename:  reportStruct.hpp
 *
 *    Description:  structure defination to fetch the report from db
 *
 *        Version:  1.0
 *        Created:  Tuesday 05 May 2015 08:51:47  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#ifndef REPORTSTRUCT_HPP_
#define REPORTSTRUCT_HPP_ 
#include <iostream>
#include<stdlib.h>

typedef struct __groupReportStruct __groupReportStruct__;
struct __groupReportStruct{
	int conversationNumber ;
	std::string  sourceIpAddr;
	std::string  destinationIpAddr;
	long delay;
//	long length;
	std::string startTime;	
	std::string endTime;
};
typedef struct __packetInfo __packetInfo__;
struct __packetInfo{
	std::string sourceIpAddr;
	std::string destinationIpAddr;
	long length;
	std::string time;
};

#endif

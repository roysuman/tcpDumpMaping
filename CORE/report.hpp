/*
 * =====================================================================================
 *
 *       Filename:  report.hpp
 *
 *    Description:  generate the report
 *
 *        Version:  1.0
 *        Created:  Tuesday 05 May 2015 08:14:47  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ROY (), email.suman.roy@gmail.com
 *   Organization:  NA
 *
 * =====================================================================================
 */
#include "init.hpp"
#include "reportStruct.hpp"
#include "databaseManagement.hpp"
#include <fstream>
#include<iostream>
namespace rep{
	class Report{
		private:
			static long           delay;
			static std::string    reportFile;
			static Init           *initObj;
			static bool            isConfig;
			static std::fstream    fileHandler;
			static bool           fileOpend;
			static bool           initCreated;
			static DbManager      *dbInstance; 
			static bool           isDbActive;
			bool setFilter();
			bool openFile();
			bool readData( );

		public:
			Report();
			~Report();
			bool genReport();

	};

}

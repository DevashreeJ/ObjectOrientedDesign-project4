#pragma once
//////////////////////////////////////////////////////////////////////////
// Display.h- To demonstrate code for displaying type table ,		//
// dependency table and strong components				//
//                                                                      //
// Author: Devashree Jadhav		SUID: 244997526		        //
/////////////////////////////////////////////////////////////////////////
/*
* Operations:		
* -------------------
* This file is created to display the results of analysis.
* showTypeTable()= function to display the type table
*
* Required Files:
* ---------------
* NoSqlDb.h
*
* Maintenance History:
* --------------------
* Ver 1.0 : March 12, 2017
* - first release
*/

#include "../NoSqlDb/NoSqlDb.h"
using namespace NoSQLDB;

class Display
{
public:


	inline void showTypeTable(std::unordered_map<std::string, std::pair<std::string, std::string>> typeTable)
	{
		std::cout << "\n\n\n -------------------------------------------------------------------------------------------------------------------------------";
		std::cout << "\n DISPLAYING TYPE TABLE";
		std::cout << "\n -------------------------------------------------------------------------------------------------------------------------------";
		std::cout << "\n " << std::setw(50) << "Name" << std::setw(50) << "Type" << std::setw(50) << "FileName" << "\n";
		std::cout << "\n -------------------------------------------------------------------------------------------------------------------------------";
		for (auto it = typeTable.begin(); it != typeTable.end(); it++)
		{
			std::cout << "\n " << std::setw(50) << it->first << std::setw(50) << it->second.first << std::setw(50) << it->second.second << "\n";
		}
	}
};

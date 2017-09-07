#pragma once
/////////////////////////////////////////////////////////////////////
// dependencytable.h - dependency analysis and database creation	//
//  Mandar Mathure, CSE687 - Object Oriented Design, Spring 2017  //
//																   //
//  Language:      Visual C++ 2015		                           //
//  Platform:      Lenovo y700, Windows 10						   //
//  Application:   TypeBased-Dependency Analyzer CSE687 Pr2, Mar-7 //
//  Author:        Mandar Mathure, CST 4-187, Syracuse University  //
//                 (315) 949-8682, mmathure@syr.edu                //
//																	//
/////////////////////////////////////////////////////////////////////
/*
* - Manual Information
This package performs dependency analysis on the typetable created in typetable package.
The dependency analysis is stored in the database created in project 1 and a separate package is used to display the 
contents in xml format.

Public Interface:
=================
int makedatabase(),int makexml()

* - Build Process:
Required files
- Tokenizer.h,typetable.h,NoSqlDb.h,ActionsAndRules.h,Persist.h
Build commands
This is a static library builds with code analyzer
- devenv CodeAnalyzerEx.sln

* - Maintenance information
ver 1.0 : 4 Mar 2017
-	first release
ver 2.0: 12 Mar 2017
-added makexml()
-modified existing code to suit modular needs
*/
#include<unordered_map>
#include<vector>
#include <fstream>
#include <iomanip>
#include <exception>
#include <locale>
#include <iostream>
#include "../Tokenizer/Tokenizer.h"
#include "../typetable/typetable.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../Parser/ActionsAndRules.h"
#include<algorithm>
#include "../XmlDocument/Persist/Persist.h"
#include <set>
using namespace Scanner;
using namespace CodeAnalysis;
class dependencytable {

public:
	using mytablecolumns = std::pair<std::string, std::string>;
	using mytableentry = std::unordered_map<std::string, mytablecolumns>;
	using deptableentry = std::unordered_map<std::string, std::set<std::string>>;
	dependencytable();
	int makedatabase(std::string  filespec);
	int makexml(std::string xml);

	NoSqlDb<StrData> makedeptable(typetable& x, std::string filespec, std::string xmlname);


	deptableentry& dependencytable::getter_() {

		return deptableentry_;
	}



	void dependencytable::setter(deptableentry& x) {

		deptableentry_ = x;

	}






private:
	deptableentry deptableentry_;
	std::unordered_map<std::string, int> strongmap;
	NoSqlDb<StrData> DB;
	Element<StrData> elementadd;
	AbstrSynTree& ASTref_;







};









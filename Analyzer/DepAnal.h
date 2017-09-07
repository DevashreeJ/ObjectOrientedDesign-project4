/////////////////////////////////////////////////////////////////////////
// DepAnal.h - Demonstrate how to start developing dependency analyzer //
//                                                                     //
// Author: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017   //
// Used by: Devashree Jadhav 		SUID: 244997526		       //
/////////////////////////////////////////////////////////////////////////
/*
* Operations:
* -------------------
* This file is created to design the functions that generate the TypeTable and the Dependency Table
* DFS()= runs depth first search on the AST and generates the TypeTable
* doTypeAnal()= base function to call DFS()
* AnalyseDependency()= Takes arguments for xml file name and performs the following functions : 
					1) Creates dependency table.
					2) Checks the argument for xml file 
					3) Displays dependency table
					4) Creates Persistance file and store it to XML
* Required Files:
* ---------------
* ActionsAndRules.h, DependencyAnalysis.h,
* TypeTable.h, Display.h
*
* Maintenance History:
* --------------------
* Ver 1.0 : March 12, 2017
* - first release
*/




#pragma once

#include "../Parser/ActionsAndRules.h"
#include"../TypeAnalysis/TypeTable.h"
#include"../DependencyAnalysis/DependencyAnalysis.h"

#include <iostream>
#include <functional>
#include<set>

#pragma warning (disable : 4101)  // disable warning re unused variable x, below

using namespace NoSQLDB;

namespace CodeAnalysis
{
	class TypeAnal
	{

	private:
		void DFS(ASTNode* pNode);
		//void DeapthFS(ASTNode* pNode);
		TypeTable table;
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
		std::unordered_map<std::string, std::pair<std::string, std::string>>tempTable = table.getTable();
		NoSqlDb<std::string> db2;

	public:
		
		TypeAnal();
		TypeTable doTypeAnal();
		std::set<std::string> pathHolder;
		void AnalyseDependency(std::string s);
		Dependence_class dc;

	};

	inline TypeAnal::TypeAnal() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack()),
		toker_(*(Repository::getInstance()->Toker()))
	{
		std::function<void()> test = [] { int x; };  // This is here to test detection of lambdas.
	}                                              // It doesn't do anything useful for dep anal.

	inline bool doDisplay(ASTNode* pNode)
	{
		static std::string toDisplay[] = {
			"function", "lambda", "class", "struct", "enum", "alias", "typedef"
		};
		for (std::string type : toDisplay)
		{
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}
	inline void TypeAnal::DFS(ASTNode* pNode)
	{
		static std::string path = "";
		if (pNode->path_ != path)
		{
				path = pNode->path_;
		}
		
		table.insertIntoTable(pNode);
		pathHolder.insert(pNode->path_);
		for (auto pChild : pNode->children_)
			DFS(pChild);
	}



	inline TypeTable TypeAnal::doTypeAnal()
	{
		
		ASTNode* pRoot = ASTref_.root();
		DFS(pRoot);
		std::cout << "\n----------------------------------------------------------------------------------------------------------------------\n";
		std::cout << "\n                                                 TYPE TABLE                                                           \n";
		std::cout << "\n----------------------------------------------------------------------------------------------------------------------\n";
		std::cout << "\nName" << std::setw(55) << "Type" << std::setw(55) << "File Name\n";
		std::cout << "------------------------------------------------------------------------------------------------------------------------\n";

		for (auto it = table.getTable().begin(); it != table.getTable().end(); ++it)
		{

			std::cout << it->first << std::setw(55) << it->second.first << std::setw(55) << it->second.second << "\n";
			std::cout << "\n";
		}

		return table;
	}


	inline void TypeAnal::AnalyseDependency(std::string xmlName)
	{
		

		std::cout << "\n ---------------------------------------------------------------------------------------------------------------------------------\n";
		std::cout << "\n                             DISPLAYING DEPENDENCY TABLE                                    \n";
		std::cout << "\n ---------------------------------------------------------------------------------------------------------------------------------\n";
		
		auto i = pathHolder.begin();
		while (i != pathHolder.end())
		{
			dc.createDependencyTable(table, *i);
			++i;
		}
		

		/*std::cout << "\n ---------------------------------------------------------------------------------------------------------------------------------\n";
		std::cout << "\n                                         XML Persistance                                   \n";
		std::cout << "\n ---------------------------------------------------------------------------------------------------------------------------------\n";
*/
		if ((xmlName == "") || (xmlName == "*.cpp") || (xmlName == ".*h") || (xmlName == "/m") || (xmlName == "/s") || (xmlName == "/a") || (xmlName == "/r") || (xmlName == "/d") || (xmlName == "/b") || (xmlName == "/f")) {
			xmlName = "DefaultName.txt";
		}
		else
		{
			xmlName = xmlName;
		}
		dc.dependencyPersistance(xmlName);

		dc.callCodePublisher();
   	}
}

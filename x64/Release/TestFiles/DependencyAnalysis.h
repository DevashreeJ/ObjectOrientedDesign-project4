#pragma once

///////////////////////////////////////////////////////////////////////////
// DependencyAnalysis.h- To demonstrate code for generating 
// dependency table																		
// Author: Devashree Jadhav		SUID: 244997526		       
///////////////////////////////////////////////////////////////////////////
/*
* Operations:
* -------------------
* This file is created to create the dependency analysis table
* getDepTable()= funtion to get dependency table as an unordered map
* 
*1) createDependencyTable () - Creates a dependency table by using the type table. Saves elements in db.
*2) dependencyPersistance () - Saves Db to XML file and prints the same. 
*
* Required Files:
* ---------------
* AbstrSynTree.h, NoSqlDb.h, TypeTable.h
*
* Maintenance History:
* --------------------
* Ver 1.0 : March 12, 2017
* - first release
*/


#include <unordered_map>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <utility>
#include <clocale>
#include <locale>
#include <set>

#include"../Parser/ActionsAndRules.h"
#include"../TypeAnalysis/TypeTable.h"
#include"../Tokenizer/Tokenizer.h"
#include"../Persistance/Persistance.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../CodePublisher/CodePublisher.h"	

#pragma warning (disable : 4101)  // disable warning re unused variable x, below

using namespace NoSQLDB;
using namespace CodeAnalysis;
using namespace Scanner;

using StrData = std::string;


using SPtr = std::shared_ptr<ASTNode*>;
	class Dependence_class
	{
	
	private:
		std::unordered_map<std::string,std::set<std::string>> depTable;
		std::unordered_map<std::string,std::pair<std::string,std::string>> tempTypeTable;
		std::set<std::string> tempfiles;
		NoSqlDb<StrData> db2;
		Element<StrData> element;
		AbstrSynTree& ASTref_;
		TypeTable table;
		Publish codepub;

	public:

		Dependence_class();
		std::unordered_map<std::string, std::set<std::string>>& getDepTable()
		{
			return depTable;
		}
		void createDependencyTable(TypeTable& t, std::string fileName);
		
		void  dependencyPersistance(std::string);
		void  callCodePublisher();

	};

	
	inline  Dependence_class::Dependence_class():ASTref_(Repository::getInstance()->AST())
	{
	}


	inline void Dependence_class::dependencyPersistance(std::string xmlFilename)
	{


		std::ofstream file;

		file.open(xmlFilename);
		XmlNamespace::Persistance p;		 
		file << p.toXml(db2);
		
		file.close();

	}
	
	inline void Dependence_class::createDependencyTable(TypeTable& t, std::string fileName)
	{

		tempTypeTable = t.getTable();	
		if (fileName != "") {
			static std::string path = "";
			try {
				std::ifstream in(fileName);
				if (!in.good())
				{
					std::cout << "can't open";
				}
				else
				{
					Toker toker;
					toker.returnComments();	toker.attach(&in);

					do
					{
						std::string tok = toker.getTok();

						for (auto tt : tempTypeTable) {
							if (tok == tt.first) {

								tempfiles.insert(tt.second.second);
								//		std::cout <<"\n"<< std::setw(55) << fileName << std::setw(55) << tt.second.second << "\n";
							}
						}

					} while (in.good());
					element.name = fileName;
					while (!element.Children.empty())
					{
						element.Children.pop_back();
					}
					for (auto iter = tempfiles.begin(); iter != tempfiles.end(); ++iter) {
						element.Children.push_back(*iter);
						//std::cout << *iter;
					}
					db2.save(fileName, element);
					depTable.insert(std::make_pair(fileName, tempfiles));
				}

				std::cout << std::endl;
				std::cout << std::setw(60) << std::left << fileName << "\t---Dependent on----\t ";
				for (auto it = depTable[fileName].begin(); it != depTable[fileName].end(); ++it) {
					std::cout << std::endl;
					std::cout << std::setw(150) << " " << *it << "\n";
				}
			}
				catch (std::logic_error& ex)
				{
					std::cout << "\n  " << ex.what();
				}
				std::cout << "\n\n";
			}
						
	}

	inline void	Dependence_class::callCodePublisher()
	{
		codepub.makeHtmFiles(depTable);
		codepub.dependencyMapping();
		//codepub.showTable();
	}





	























	


	
	


#pragma once
#pragma once
#include<unordered_map>
#include"../AbstractSyntaxTree/AbstrSynTree.h"

///////////////////////////////////////////////////////////////////////////
// TypeTable.h- To demonstrate code for generating type table																		
// Author: Devashree Jadhav		SUID:244997526		       						 
///////////////////////////////////////////////////////////////////////////
/*
* Operations:
* -------------------
* This file is created to create the type table
* insertTypeTable()= funtion to insert data into the type table
* Required Files:
* ---------------
* AbstrSynTree.h
*
* Maintenance History:
* --------------------
* Ver 1.0 : March 12, 2017
* - first release
*/


#pragma warning (disable : 4101)  // disable warning re unused variable x, below

class TypeTable
{
private:
	std::unordered_map<std::string, std::pair<std::string, std::string>> typetable;

public:

	std::unordered_map<std::string, std::pair<std::string, std::string>>& getTable()
	{
		return typetable;
	}

	void setvalue(std::unordered_map<std::string, std::pair<std::string, std::string>> &table)
	{
		this->typetable = table;
	}
	void insertIntoTable(CodeAnalysis::ASTNode* pNode);
};

inline void  TypeTable::insertIntoTable(CodeAnalysis::ASTNode* pNode)
{
	if (pNode->type_ == "class" || pNode->type_ == "enum" || pNode->type_ == "struct" || pNode->type_ == "typedef" || pNode->type_ == "using")
	{
		std::pair<std::string, std::string> temp;
		temp.first = pNode->type_;
		temp.second = pNode->package_;
		typetable.insert(std::make_pair(pNode->name_, temp));
	}
}


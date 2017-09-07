////////////////////////////////////////////////////////////////////////////
// PersistXml.h- used for the functionality as specified by requirement 5 //
// Ver 1.0                                                       	      //
// Application: CSE687 Project 1- Spring 2017               	 	      //
// Platform:    Dell Inspiron 13, Windows 10, Visual Studio 2015 	      //
// Author:      Anita Ajwani, SUID: 725553665                 	 	      //
////////////////////////////////////////////////////////////////////////////
/*
* Operations:
* -------------------
* This file is created to design the functions that persist the data to an Xml and from an Xml.
* This is used to satisfy requirement 5 of Project 1.
* The function descriptions are as follows-
* toXml()= Takes an Element<Data> as input and genetares a string containg the exact XML format
* for that Element;
* fromXml()= Takes an XML string as input and generates an Element for the specified Xml structure.
* Required Files:
* ---------------
* CppProperties.h, XmlDocument.h, Convert.h,
* XmlElement.h, StrHelper.h, NoSqlDb.h
* Maintenance History:
* --------------------
* Ver 1.0 : Feb 7,2017
* - first release
*/
#pragma once
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "../XmlDocument/XmlDocument.h"
#include "../XmlDocument/XmlElement/XmlElement.h"
#include"../NoSqlDb/NoSqlDb.h"
#include "../StrHelper.h"

using namespace NoSQLDB;

namespace XmlNamespace
{
	using StrData = std::string;
	using namespace XmlProcessing;//used because compiler does not know where the XmlDocument is
	using SPtr = std::shared_ptr<AbstractXmlElement>;

	class Persistance
	{
	public:
		std::string toXml(NoSqlDb<StrData> db);
	};

	
	inline std::string  Persistance::toXml(NoSqlDb<StrData> db) {


		using sPtr = std::shared_ptr < AbstractXmlElement >;

		sPtr root = makeTaggedElement("Dependency Table");

		for (unsigned int i = 0; i < db.keys().size(); ++i)
		{

			sPtr pKeyElem = makeTaggedElement("File Name");
			root->addChild(pKeyElem);
			sPtr pkElem = makeTextElement(db.keys().at(i));
			pKeyElem->addChild(pkElem);


			sPtr pChildren = makeTaggedElement("Dependencies");
			root->addChild(pChildren);
			//at(i)).getChild().size()

			for (unsigned int m = 0; m < db.value(db.keys().at(i)).getChild().size(); ++m)
			{
				sPtr pchild = makeTextElement(db.value(db.keys().at(i)).getChild().at(m));
				pChildren->addChild(pchild);
			}


		}


		sPtr docEl = makeDocElement(root);
	//	std::cout << "  " << docEl->toString();
	//	std::cout << "\n\n";
		return docEl->toString();
	}
}




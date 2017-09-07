///////////////////////////////////////////////////////////////
//    NoSqlDb.cpp - defines test stub for functionalities    //
//                    implemented in NoSqlDb.h               //
//                                                           //
//                                                           //
///////////////////////////////////////////////////////////////

#include "NoSqlDb.h"
#include <string>
#include <iostream>
#include <time.h>
#include <vector>
//#include "../query/query.h"
//#include "../CppProperties/CppProperties.h"
#include <fstream>
#include<time.h>
//#include "../XmlDocument/XmlDocument/XmlDocument.h"
using StrData = std::string;
using intData = int;
using Key = NoSqlDb<StrData>::Key;
using Keys = NoSqlDb<StrData>::Keys;



#ifdef TEST_NOSQLDB

int main()
{
	std::cout << "-------------------Test Stub-------------------------";
	NoSqlDb<StrData> db;
	Element<StrData> el1;
	Element<StrData> el2;

	el1.name = "Mandar";
	el1.category = "Grad";
	el1.data = "CE";
	time_t now = time(0);
	struct tm * curtime = localtime(&now);
	el1.timeDate = asctime(curtime);
	el1.child.push_back("Jane");
	el2.name = "Mark";
	el2.category = "Grad";
	el2.data = "CS";
	el2.timeDate = asctime(curtime);
	el2.child.push_back("Mandar");

	std::cout << "-------Testing save function-----";
	db.save(el1.name, el1);
	db.save(el2.name, el2);
	std::cout << "\n --------------Testing show function----------\n";
	std::cout << el1.show() << std::endl;
	std::cout << el2.show() << std::endl;
	std::cout << "-------------------Testing keys function--------\n";
	for (unsigned int i = 0; i < db.keys().size(); ++i) {

		std::cout << db.keys().at(i);
	}
	std::cout << "---------------Testing Delete Function------";
	db.del("Mark");
	std::cout << "\n ----------------------Testing Update Function---------";
	db.update("Mandar", el1);
	std::cout << "-------------------Testing value function----------------";
	db.value("Mandar").show();
	std::cout << "----------------Testing updatemeta function--------------";
	db.updatemeta("Mandar", "undergrad", "EE", { "john,jane" });
	std::cout << "-------Testing database to XMl function--------";
	//This function shows Xml peristence. the fromxml and to xml functions are both linked in this function
	db.databasetoXml(db);
}

#endif

////////////////////////////////////////////////////////////////////////
//  PersistXml.cpp - Executes the toXml() and fromXml() functions	  //
//  which are needed for database persistance	     				  //	
//  ver 1.0                                                           //                                                        //
//  Language:      Visual C++		                                  //
//  Platform:       Dell Inspiron 13, Windows 10, Visual Studio 2015  //
//  Author:        Anita Ajwani, SUID: 725553665		              //
////////////////////////////////////////////////////////////////////////
#include <fstream>
#include "Persistance.h"
using namespace XmlNamespace;
using namespace NoSQLDB;
#ifdef persisting
int main()
{
	using StrData = std::string;
	using Key = NoSqlDb<StrData>::Key;
	using Keys = NoSqlDb<StrData>::Keys;
	std::cout << "\n  ---------Demonstrating Persistance Code---------";
	std::cout << "\n  ================================================\n";
	Persistance<StrData> obj;
	NoSqlDb<StrData> db;
	std::cout << "\n Sending following data to XML file:";
	std::ofstream myXmlDoc;
	myXmlDoc.open("../test.xml");
	Keys keys = db.keys();
	for (Key key : keys)
	{
		Element<StrData> e = db.value(key);
		std::string ans = obj.toXml(e);
		//toSaveXml.push_back(ans);
		std::cout << ans << "\n";
		myXmlDoc << ans;
	}
	std::cout << "\nData Successfully sent to XML file : Check test.xml in project folder\n";
	myXmlDoc.close();
}
#endif // persisting
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

#include "dependencytable.h"
dependencytable::dependencytable() :
	ASTref_(Repository::getInstance()->AST())

{
}


int dependencytable::makedatabase(std::string filespec) {
	if (filespec != "") {
		std::cout << "\n" << DB.value(filespec).show();
	}

	return 0;
}

 int dependencytable::makexml(std::string xml) {
	std::ofstream myfile;

	myfile.open(xml);

	std::cout << "==========================================================Database Contents are saved to a File in XML format(default : Project2.txt)===============================";
	X x;
	x.makexml(DB);
	myfile.close();

	return 0;
}


 NoSqlDb<StrData> dependencytable::makedeptable(typetable& x, std::string filespec, std::string xmlname) {
	if (filespec != "") {
		static std::string path = "";
		try {
			std::ifstream in(filespec);
			if (!in.good())
			{
				std::cout << "cant open";
				return DB;
			}
			else
			{	Toker toker;
				toker.returnComments();
				toker.attach(&in);
				mytableentry my = x.getterx();
				std::set<std::string> tempvector;
				do
				{
					std::string tok = toker.getTok();
					for (auto tt : my) {
						if (tok == tt.first) {
							tempvector.insert(tt.second.second);
						}
					}
				} while (in.good());
				elementadd.name = filespec;
				while (!elementadd.child.empty())
				{
					elementadd.child.pop_back();
				}
				for (auto iter = tempvector.begin(); iter != tempvector.end(); ++iter) {
					elementadd.child.push_back(*iter);
				}
				DB.save(elementadd.name, elementadd);
				deptableentry_.insert(std::make_pair(filespec, tempvector));
			}
			std::cout << std::setw(140) << std::left << filespec << "=>";
			for (auto set_it = deptableentry_[filespec].begin(); set_it != deptableentry_[filespec].end(); ++set_it) {
				std::cout << *set_it << ", ";
			}
		} catch (std::logic_error& ex)
		{
			std::cout << "\n  " << ex.what();
		}
		std::cout << "\n\n";
	}
	return DB;
}


#ifdef DEPENDENCYTABLE_TEST


 int main() {


	 d.makexml("filename.txt");
	 //other interface are not included in this test stub because they are dependent on multiple files to suppy the arguments 
	 //just like DepAnal.cpp
	 
	 
	 //testing tependency table;


 }







#endif 

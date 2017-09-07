#pragma once
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  NoSqlDb.h defines two classes that are used in populating and calling important function in our database//
//                                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Package operation:
------------------
1. Defines two classes of template type namely : Element and NoSqlDb.
2. Element class is used to create individual elements of the database.
3. NoSqlDb class is used to add these individual elements to an unordered map using a save function.
4. These two classes together foem our data base.

*/
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "../CppProperties/CppProperties.h"
#include<ctime>


//#include "../StrHelper.h"
using StrData = std::string;
/////////////////////////////////////////////////////////////////////
// Element class represents a data record in our NoSql database
// - in our NoSql database that is just the value in a key/value pair

template<typename Data>
class Element
{
public:
	using Name = std::string;
	using Category = std::string;
	using Description = std::string;
	using TimeDate = std::string;
	using Children = std::string;
	Property<Name> name;            // metadata
	Property<Category> category;    // metadata
	Property<TimeDate> timeDate;    // metadata

	Property<Data> data;            // data

	std::vector<Children> child;		//child data


	std::string show();
};


template<typename Data>
std::string Element<Data>::show()
{


	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "name" << " : " << name;
	out << "\n    " << std::setw(8) << "category" << " : " << category;

	out << "\n    " << std::setw(8) << "data" << " : " << data;
	for (unsigned int i = 0; i < child.size(); ++i)
	{
		out << "\n    " << std::setw(8) << "child" << i + 1 << " : " << child.at(i);//showing children

	}
	out << "\n    " << std::setw(8) << "timeDate" << " : " << timeDate;



	out << "\n";
	return out.str();
}

/////////////////////////////////////////////////////////////////////
// NoSqlDb class is a key/value pair in-memory database
// - stores and retrieves elements

template<typename Data>
class NoSqlDb
{
public:
	using Key = std::string;
	using Keys = std::vector<Key>;

	Keys keys();
	bool save(Key key, Element<Data> elem);
	void del(Key key);
	void update(Key key, Element<Data> &c);
	void NoSqlDb <Data> ::updatemeta(Key key, std::string b, std::string c, std::vector<std::string> children);
	void addChild(Key key, std::string a);//addition of child to any key value
	//std::string NoSqlDb<Data>::toxml(Keys keys, NoSqlDb <Data> x);
	Element<Data> value(Key key);
	size_t count();
private:
	using Item = std::pair<Key, Element<Data>>;

	std::unordered_map<Key, Element<Data>> store;
};

template<typename Data>
typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
{
	Keys keys;


	for (Item item : store)
	{
		keys.push_back(item.first);
	}
	return keys; //returns a vector of keys
}

template<typename Data>
bool NoSqlDb<Data>::save(Key key, Element<Data> elem)//save functionality to store key/value pairs
{
	if (store.find(key) != store.end())
		return false;
	store[key] = elem;
	return true;
}

template<typename Data>
void NoSqlDb<Data> ::del(Key key)
{
	store.erase(key);
	std::cout << "\"" << key << "\"" << " deleted\n ";//deleting any value by taking its key as input

}



template<typename Data>
void NoSqlDb <Data> ::update(Key key, Element <Data> &c)
{
	if (store.find(key) != store.end())
		store[key] = c;

	else
		std::cout << "error";

}

template<typename Data>
Element<Data> NoSqlDb<Data>::value(Key key)
{
	if (store.find(key) != store.end())
		return store[key];
	return Element<Data>();
}
template<typename Data>
size_t NoSqlDb<Data>::count()
{
	return store.size();
}

template<typename Data>
void NoSqlDb <Data> ::updatemeta(Key key, std::string b, std::string c, std::vector<std::string> children)
{
	store[key].category = b;
	store[key].data = c;
	store[key].child = children;
	std::cout << key << " metadata updated\n\n";
}



template <typename Data>
void NoSqlDb<Data>::addChild(Key key, std::string a) {


	store[key].child.push_back(a);
	std::cout << "Child added to " << key;
}












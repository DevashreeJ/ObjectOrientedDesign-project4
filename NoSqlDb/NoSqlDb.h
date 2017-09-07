///////////////////////////////////////////////////////////////////////
// NoSqlDb.h - key/value pair in-memory database					 //
// Application: Creating Nosql db for storing and retrieving data    //
// Platform:    Lenovo, Win 10, Visual Studio 2015		             //
// Author:	    Jim Fawcett	                                         //
// CSE687 - Object Oriented Design, Spring 2017		                 //
///////////////////////////////////////////////////////////////////////
 /*
 * Package Operations:
 * -------------------
 * This Package helps in the creation of map of data for storing key-value pair.
 * The Nosql class declares methods to perform operations on the created map and 
 * the Element class helps in the creation of elements  to be inserted in the map. 
 * The key part consists of a string and the value part consists of element data.
 *
 * Element class:
 * Consists of data, name, description, category, timedata, children.
 * data, name, description, category, timedate are of the type string.
 * The data variable might be any kind of data ranging from string to struct.
 * Member functions: 
 * setchild() - adds child to children vector
 * getchild() - returns child from children vector
 * removechild() - removes child from specified element
 *
 * NoSqlDb Class:
 * Created for saving data in a map. Uses object of element class to perform 
 * following operations on it.
 * save() - adds a key value pair in a map 
 * updateData() - updates the values in an element
 * deleteElement () - deletes element from the map
 * showChild () - shows children of the specified key
 *
 * Files Used:
 * -------------
 * Cppproperties.h
*/


#pragma once
#include <unordered_map>
#include<algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <iterator>
#include<time.h>
#include"CppProperties.h"



/////////////////////////////////////////////////////////////////////
// Element class represents a data record in our NoSql database
// - in our NoSql database that is just the value in a key/value pair
// - it needs to store child data, something for you to implement
//
namespace NoSQLDB
{
	using Key = std::string;
	using Keys = std::vector<Key>;
	using Name = std::string;
	using Category = std::string;
	using TimeDate = std::string;
	using Description = std::string;
	
	
	template<typename Data>
	class Element
	{
	public:

		Property<Name> name;            // metadata
		Property<Category> category;    // metadata
		Property<TimeDate> timeDate;    // metadata
		Property<Description>description;
		Property<Data> data;            // data

	  //  NoSqlDb<Data> nsdb;	
		

	public:
		std::vector<std::string> Children;
		void setChild(std::string key)  //sets the element as the child of the element from which this method is called.
		{

			this->Children.push_back(key);
		}
		std::vector<std::string> getChild()
		{
			return this->Children;
		}

		void removeChild(std::string key) //This method deletes a child key from the given element if it exists.
		{
			std::vector<std::string>::iterator result = find(Children.begin(), Children.end(), key);

			if (result != Children.end())
				Children.erase(result);

			else
				std::cout << "\n" << "child not found";
		}
		//std::string currentDateTime();
		std::string show();
	};



	template<typename Data>
	std::string Element<Data>::show()
	{
		// show children when you've implemented them

		std::ostringstream out;
		std::vector<std::string>::iterator it;
		out.setf(std::ios::adjustfield, std::ios::left);

		out << "\n    " << "name" << " : " << name;
		out << "\n    " << "category" << " : " << category;
		out << "\n    " << "description" << " : " << description;
		out << "\n    " << "timeDate" << " : " << timeDate;
		out << "    data" << " : " << data;
		//out << "\n    " << std::setw(8) << "children" << " : ";
		//out << "\n";
		return out.str();
	}


	/////////////////////////////////////////////////////////////////////
	// NoSqlDb class is a key/value pair in-memory database
	// - stores and retrieves elements
	// - you will need to add query capability
	//   That should probably be another class that uses NoSqlDb
	//
	template<typename Data>
	class NoSqlDb
	{

	public:

		template<typename Data>
		friend class Query;

		template<typename Data>
		friend class Test;

		using Key = std::string;
		using Keys = std::vector<Key>;

		Keys keys();
		bool save(Key key, Element<Data> elem);
		bool updateData(Key key, Element<Data> &elem);
		void  deleteElement(Element<Data> &newElem);
		Element<Data> value(Key key);

		size_t count();
		void showChild(Element<Data> element);
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
		return keys;
	}

	template<typename Data>
	bool NoSqlDb<Data>::save(Key key, Element<Data> elem)
	{
		if (store.find(key) != store.end())
			return false;
		store[key] = elem;
		return true;
	}

	/////////////////////////////////////////////////////////////////////
	//updateData will be called when existing data has to be modified
	// - finds key to be updated
	// - replaces the updated data with old data.
	//
	template<typename Data>
	bool NoSqlDb<Data>::updateData(Key key, Element<Data> &newElem)
	{
		//NoSqlDb<Data> ndb;
		//ndb.value(key).name = key;
		if (store.find(key) != store.end())
		{
			store[key] = newElem;
			return true;
		}
		return false;

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


	/////////////////////////////////////////////////////////////////////
	// showchild is a method which will be called to display children
	// of a given element. 
	// - uses getchild for retreiving keys in child vector.
	// - iterates through vector and displays each child.

	template<typename Data>
	inline void NoSqlDb<Data>::showChild(Element<Data> element)
	{
		std::vector<std::string>  first = element.getChild();
		std::vector<std::string>::iterator it;
		for (it = first.begin(); it != first.end(); ++it)
		{
			if (element.getChild().empty())
			{
				std::cout << "No child";
			}
			else
			{
				std::cout << *it << "\t";
			}
		}
	}

	/////////////////////////////////////////////////////////////////////
	// deleteElement is a method which will be called to delete 
	// a specific element.
	// - deletes the key of specified element from children vector of 
	//   other elements if it exists.
	// - deletes the element from the map 

	template<typename Data>
	inline void NoSqlDb<Data>::deleteElement(Element<Data> &newElem)
	{
		NoSqlDb<Data> ndb;

		std::string keycomparator = newElem.name;

		for (auto &mapIterator = store.begin(); mapIterator != store.end(); ++mapIterator)
		{

			Element <Data> temp = mapIterator->second;
			std::vector<std::string> comparator = temp.getChild();

			for (std::vector<std::string>::iterator iter = comparator.begin(); iter != comparator.end(); ++iter)
			{
				if (*iter == keycomparator)
				{
					temp.removeChild(keycomparator); //removes the key from child vector
					store[temp.name] = temp; //saves changes.
				}
			}
		}
		store.erase(keycomparator);
	}
}

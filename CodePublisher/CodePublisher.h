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
* makeHtmFiles() = Creates Htm files from source code files 
* createCopy() = function doing actual copying of content
* dependencyMapping() = adds dependencies in the htm head and links for hiding 
						and unhiding class and functions.
*ExtractFilename() = extracts filename from the path of file
*LaunchFiles () = Launches files in the browser
* ---------------
* ActionsAndRules.h, DependencyAnalysis.h,
* TypeTable.h, Display.h
*
* Maintenance History:
* --------------------
* Ver 1.0 : April 2, 2017
* - first release
*/


#pragma once
#include<iostream>
#include<unordered_map>
#include<set>
#include<string>
#include<fstream>
#include<direct.h>

class Publish
{
private:
	std::unordered_map<std::string, std::set<std::string>> deptab;
	std::unordered_map<std::string, std::set<std::string>> tempTab;
	std::set<std::string> filesDependedOn;
	std::set<std::string> filesDependedOnfortempTab;

public:
	void makeHtmFiles(std::unordered_map<std::string, std::set<std::string>> &table);
	void createCopy(std::string src, std::string dest);
	void dependencyMapping();
	std::string ExtractFileName(const std::string &path);
	void LaunchFiles();
};

inline void Publish::makeHtmFiles(std::unordered_map<std::string, std::set<std::string>> &table)
{
	_mkdir("newFolder");
	auto iter = table.begin();
	while (iter != table.end())
	{
		std::string common = "newFolder/";
		std::string temp = iter->first;
		std::string file2 = ExtractFileName(temp);
		std::string file = ExtractFileName(temp);
		std::string filewithExtension = file.append(".htm");
		std::string finaldestFile = common.append(filewithExtension);
		createCopy(temp, finaldestFile);
		auto it = iter->second.begin();
		while (it != iter->second.end())
		{
			std::string tmp = *it;
			filesDependedOnfortempTab.insert(tmp);
			tmp = tmp.append(".htm");
			filesDependedOn.insert(tmp);
			it++;

		}
		deptab.insert(std::make_pair(filewithExtension, filesDependedOn));
		tempTab.insert(std::make_pair(file2, filesDependedOnfortempTab));
		iter++;
	}
}

inline void Publish::createCopy(std::string src, std::string dest)
{
	std::ifstream f_src;
	std::ofstream f_dest;
	f_src.open(src);
	f_dest.open(dest);
	char tmp;
	while (f_src.get(tmp))
	{
		if (tmp == '<')
			f_dest << "&lt";
		else if (tmp == '>')
			f_dest << "&gt";
		else
			f_dest << tmp;
	}
}

inline std::string Publish::ExtractFileName(const std::string& fullPath)
{
	const size_t lastSlashIndex = fullPath.find_last_of("/\\");
	return fullPath.substr(lastSlashIndex + 1);
}

void Publish::dependencyMapping()
{
	
	_mkdir("ResultFolder");
	auto iter = deptab.begin();
	auto tempiter = tempTab.begin();
	while (iter != deptab.end())
	{
		std::string opstring = "ResultFolder/";
		std::string ipstring = "newFolder/";
		std::string inputFilestring = ipstring.append(iter->first);
		std::string outputFilestring = opstring.append(iter->first);
		std::ofstream Indexfile;
		std::ofstream outputFile;
		std::ifstream inputFile;
		int bracketNumber = 0;
		int i = 0;
		char temp;
		outputFile.open(outputFilestring);
		inputFile.open(inputFilestring);
		outputFile << "<html>";
		outputFile << "<head>";
		outputFile << "<link rel=" << "stylesheet" << " type=" << "text/css" << " href=" << "../styleFile.css" << ">";
		outputFile << "<script src=" << "../scriptFile.js" << ">" << "</script>";
		outputFile << "<body>";
		outputFile << "<h3>" << iter->first << "</h3>";
		outputFile << "<hr/>";
		outputFile << "<div class =" << "\"indent\"" << ">";
		outputFile << "<h4>Dependencies:</h4>";
		auto it = tempiter->second.begin();
		auto it2 = iter->second.begin();
		while (it != tempiter->second.end() && it2 != iter->second.end())
		{
			outputFile << "<br>";
			outputFile << "<a href =../ResultFolder/" << *it2 << ">" << *it << "</a>";
			outputFile << "<br>";
			it++;
			it2++;
		}
		outputFile << "</div>";
		outputFile << "<hr/>";
		outputFile << "<pre>";
		while (inputFile.get(temp))
		{
			if (temp == '<')
				outputFile << "&lt";
			else if (temp == '>')
				outputFile << "&gt";
			else if (temp == '{')
			{
				bracketNumber++;
				outputFile << "<a href = " << "javascript:toggle_visibility('" << i << "');" << ">{</a>";
				outputFile << "<div id=" << i << ">";
				bracketNumber--;
				i++;
			}
			else if (temp == '}')
			{
				outputFile << temp;
				if (bracketNumber == 0)
				{
					outputFile << "</div>";
				}
			}
			else
				outputFile << temp;
		}
		outputFile << "</pre>";
		outputFile << "</body>";
		outputFile << "</html>";
		iter++;
		tempiter++;
	}

	LaunchFiles();
}
inline void Publish::LaunchFiles()
{
	auto iter = deptab.begin();
	auto tempiter = tempTab.begin();
	std::ofstream IndexFile;
	IndexFile.open("ResultFolder/IndexFile.htm");
	IndexFile << "<html>";
	IndexFile << "<head>";
	IndexFile << "<link rel =" << "\"stylesheet\"" << "href = " << "\"../styleFile.css\"" << ">";
	IndexFile << "</head>";
	IndexFile << "<body>";
	IndexFile << "<h3>Index Of Files</h3>";
	while (iter != deptab.end() && tempiter != tempTab.end())
	{
		IndexFile << "<a href =../ResultFolder/" << iter->first << ">" << tempiter->first << "</a>";
		//std::cout << tempiter->first;
		IndexFile << "<br>";
		iter++;
		tempiter++;
	}
	IndexFile << "</pre>";
	IndexFile << "</body>";
	IndexFile << "</html>";
	IndexFile.close();
	std::string destFile;
	destFile = "ResultFolder/IndexFile.htm";
	std::string command("start \"\" \"" + destFile + "\"");
	std::system(command.c_str());
}

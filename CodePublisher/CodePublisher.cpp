
#ifdef DEBUG
#include"CodePublisher.h"
int main()
{
	std::string destFile;
	destFile = "../ResultFolder/A.h.htm";
	std::string command("start \"\" \"" + destFile + "\"");
	// the command is: [ start "" "..\test files\page.html" ]
	// using [ start ..\text files\page.html ] will fail to start
	// using [ start "..\test files\page.html" ] will only start new command window with the current directory
	std::system(command.c_str());
	return 0;
}

#endif // DEBUG
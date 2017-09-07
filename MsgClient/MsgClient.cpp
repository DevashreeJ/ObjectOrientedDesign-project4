/////////////////////////////////////////////////////////////////////////
// MsgServer.cpp -Demonstrates 2 Way HTTP Message transfer and file    //
//                 transfer                                            //
// Jim Fawcett, CSE687 - Object Oriented Design,  Spring 2016          //
// Application:    OOD Project #4                                      //
// Platform:       Visual Studio 2015, Dell XPS 8900, Windows 10 pro   //
// Author :        Jim Fawcett  									   // 
// MOdified by  :        Devashree Jadhav                              // 
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a client that sends HTTP style messages and
* files to a server that simply displays messages and stores files.
*
* It's purpose is to provide a very simple illustration of how to use
* the Socket Package provided for Project #4.
*/
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/
/*
* ToDo:
* - pull the sending parts into a new Sender class
* - You should create a Sender like this:
*     Sender sndr(endPoint);  // sender's EndPoint
*     sndr.PostMessage(msg);
*   HttpMessage msg has the sending adddress, e.g., localhost:8080.
*/
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>

using namespace Logging;
using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;

/////////////////////////////////////////////////////////////////////
// ServerManager class
//- This class uses the following methods to perform the reception 
// operation when the server sends files to the client.
// readMessage () - saves the message received in the socket in a string
//					and extracts the the msgbody from the HTTP message.
// readFile ()    - accepts the file contents received from  in a socket 
//                  and puts them into a file. This method uses transfer 
//                  using a buffer 
//

class ServerManager
{
public:
	ServerManager(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
};
//----< this defines processing to frame messages >------------------

HttpMessage ServerManager::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg;

	// read message attributes

	while (true)
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
		{
			break;
		}
	}

	if (msg.attributes().size() == 0)
	{
		connectionClosed_ = true;
		return msg;
	}

	if (msg.attributes()[0].first == "POST")
	{
		std::string filename = msg.findValue("file");
		if (filename != "")
		{
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;

			readFile(filename, contentSize, socket);
		}

		if (filename != "")
		{
			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
		}
		else
		{
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size())
			{
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
		}
	}
	return msg;
}
//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/
bool ServerManager::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname = "../ClientDownloads/" + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		/*
		* This error handling is incomplete.  The client will continue
		* to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's
		* an exercise left for students.
		*/
		std::cout << "\n\n  can't open file " + fqname;
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
//----< receiver functionality is defined by this function >---------

void ServerManager::operator()(Socket socket)
{
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			std::cout << "\n\n server thread is terminating";
			break;
		}
		msgQ_.enQ(msg);
	}
}
/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class  ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t  ClientCounter::clientCount = 0;

/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
// Modifications to this class
// makeMessage () - Added a switch case in makeMessage method to 
//					create different types of messages 
// execute()      - Added switch case to demonstrate different types
//				    of functionalities by clients on different threads.   
//
//
class MsgClient
{
public:
	using EndPoint = std::string;
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages, int messagetype);
	int targc; 
	char* targv[5];
	void usingCommandLine(int argc, char * argv[] );

private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket);
};
//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage MsgClient::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	case 2:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("GET", "DownloadCommand"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	case 3:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "PublishCommand"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	case 4:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "File"));
		msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
		msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

		msg.addBody(body);
		if (body.size() > 0)
		{
			attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
			msg.addAttribute(attrib);
		}
		break;
	default:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
	}
	return msg;
}
//----< send message using socket >----------------------------------

void MsgClient::usingCommandLine(int argc, char * argv[])
{
	for (int i = 0; i<argc; i++)
	{
		targc = argc;
		targv[i] = argv[i];
	}
}
/**/

void MsgClient::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
* - Sends a message to tell receiver a file is coming.
* - Then sends a stream of bytes until the entire file
*   has been sent.
* - Sends in binary mode which works for either text or binary.
*/
bool MsgClient::sendFile(const std::string& filename, Socket& socket)
{

	std::string fqname = "../TestFiles/" + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8080");
	msg.addAttribute(HttpMessage::Attribute("file", filename));
	msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
	sendMessage(msg, socket);
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	while (true)
	{
		FileSystem::Block blk = file.getBlock(BlockSize);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); ++i)
			buffer[i] = blk[i];
		socket.send(blk.size(), buffer);
		if (!file.isGood())
			break;
	}
	file.close();
	return true;
}
//----< this defines the behavior of the client >--------------------

void MsgClient::execute(const size_t TimeBetweenMessages, const size_t NumMessages, int messageType)
{

	ClientCounter counter;
	size_t myCount = counter.count();
	std::string myCountString = Utilities::Converter<size_t>::toString(myCount);
	std::cout <<
		"Starting HttpMessage client" + myCountString +
		" on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id())
		;
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			std::cout << "\n client waiting to connect";
			::Sleep(100);
		}

		HttpMessage msg;
		std::string msgBody;
		BlockingQueue<HttpMessage> msgQ;
		switch (messageType)
		{
		case 1:
			std::cout << "\n----------------------------------Requirement 7	(HTTP Message format communication)--------------------------------------------\n";
			msgBody =
				"<msg>Message from client #" + myCountString + "</msg>";
			msg = makeMessage(1, msgBody, "localhost:8080");
			
			sendMessage(msg, si);
			std::cout << "\n\n  client" + myCountString + " sent\n" + msg.toIndentedString();
			::Sleep(TimeBetweenMessages);
			break;

		case 2:
			std::cout << "\n----------------------------------Requirement 8()--------------------------------------------\n";
			msgBody =
				"<msg>Message from client #" + myCountString + "Send me files from server</msg>";
			msg = makeMessage(2, msgBody, "localhost:8080");
			/*
			* Sender class will need to accept messages from an input queue
			* and examine the toAddr attribute to see if a new connection
			* is needed.  If so, it would either close the existing connection
			* or save it in a map[url] = socket, then open a new connection.
			*/
			sendMessage(msg, si);
			std::cout << "\n\n  client" + myCountString + " sent\n" + msg.toIndentedString();
			::Sleep(TimeBetweenMessages);
			try
			{
				std::cout << "client reception";

				SocketSystem ss;
				SocketListener sl(8081, Socket::IP6); 
				ServerManager cp(msgQ);
				sl.start(cp);
				while (true)
				{
					HttpMessage msg = msgQ.deQ();
					std::cout << "\n\n  client recvd message with body contents:\n" + msg.bodyString();
				}

			}
			catch (std::exception& exc)
			{
				std::cout << "\n  Exception caught: ";
				std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
				std::cout << exMsg;
			}
			break;

		case 4:
			std::cout << "\n----------------------------------Requirement 5 and Requirement 8--------------------------------------------\n";
			msgBody =
				"<msg>Message from client #" + myCountString + "</msg>";
			msg = makeMessage(4, msgBody, "localhost:8080");

			std::vector<std::string> files = FileSystem::Directory::getFiles("../TestFiles","*.cpp");
			for (size_t i = 0; i < files.size(); ++i)
			{
				std::cout << "\n\n  sending file " + files[i];
				sendFile(files[i], si);
			}
			 files = FileSystem::Directory::getFiles("../TestFiles", "*.h");
			for (size_t i = 0; i < files.size(); ++i)
			{
				std::cout << "\n\n  sending file " + files[i];
				sendFile(files[i], si);
			}
			sendMessage(msg, si);
			std::cout << "\n\n  client" + myCountString + " sent\n" + msg.toIndentedString();
			::Sleep(TimeBetweenMessages);
			break;
		}

		msg = makeMessage(1, "quit", "toAddr:localhost:8080");
		sendMessage(msg, si);
		std::cout << "\n\n  client" + myCountString + " sent\n" + msg.toIndentedString();

		std::cout << "\n";
		std::cout << "\n  All done folks";
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
}
//----< entry point - runs three clients each on its own thread >------

int main(int argc, char *argv)
{
	
	::SetConsoleTitle(L"Clientside");
	std::mutex mt;

	std::cout << "Client side";
	MsgClient c1;
	

	std::thread t1(
		[&]() { mt.lock(); c1.execute(100, 1, 1); mt.unlock(); }
	);

	MsgClient c2;
	std::thread t2(
		[&]() { mt.lock(); c2.execute(120, 1, 4); mt.unlock(); }
	);
	MsgClient c3;
	std::thread t3(
		[&]() {  mt.lock(); c3.execute(120, 1, 2); mt.unlock(); }
	);
	t1.join();
	t2.join();
	t3.join();
}
/////////////////////////////////////////////////////////////////////////
// MsgServer.cpp - Demonstrates 2 Way HTTP Message transfer and file   //
//                 transfer                                            //
// Jim Fawcett, CSE687 - Object Oriented Design,  Spring 2016          //
// Application:    OOD Project #4                                      //
// Platform:       Visual Studio 2015, Dell XPS 8900, Windows 10 pro   //
// Author :        Jim Fawcett  									   // 
// MOdified :      Devashree Jadhav                                  // 
/////////////////////////////////////////////////////////////////////////
/*
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files.
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
*   Executive.h, Executive.cpp
*   Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp
*   ConfigureParser.h, ConfigureParser.cpp
*   ScopeStack.h, ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp
*   ITokenCollection.h, SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp
*   IFileMgr.h, FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
*/

/*  Modifications
*   Added condition in readMessage method to detect 'GET' type of a message.
*/
/*
* ToDo:
* - pull the receiving code into a Receiver class
* - Receiver should own a BlockingQueue, exposed through a
*   public method:
*     HttpMessage msg = Receiver.GetMessage()
* - You will start the Receiver instance like this:
*     Receiver rcvr("localhost:8080");
*     ClientHandler ch;
*     rcvr.start(ch);
*/
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Analyzer/Executive.h"

#include <string>
#include <iostream>
//using namespace Logging;

//using Show = StaticLogger<1>;
using namespace Utilities;

/////////////////////////////////////////////////////////////////////
// ServerTransmission class
///////////////////////////////////////////////////////////////////////////////
// - This class defines the transmission methods required by the server
//   to send the files required by the user and send communication messages 
//   during transmission.
// - sendFile()   : used to send the requested file to the user, when the user 
//                  wishes to download a file.The server uses the <file> attribute
//				    to recognise a file request and parses the message to acquire 
//                  the filename.
//   sendMessage(): This method enables the server to communicate with the user.
//   makeMessage(): This method creates an HTTP messsage template while sending
//					messages to the user.
//   execute()    : This method defines the specific action to be taken when a file 
//	                request is sent by the client.
//

class Serversidetransmission
{
public:
	using EndPoint = std::string;
	void execute(const size_t TimeBetweenMessages, const size_t NumMessages);
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& fqname, Socket& socket);
};
/////////////////////////////////////////////////////////////////////
// ClientHandler class
/////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.
// - You no longer need to be careful using data members of this class
//   because each client handler thread gets its own copy of this 
//   instance so you won't get unwanted sharing.
// - I changed the SocketListener semantics to pass
//   instances of this class by value for version 5.2.
// - that means that all ClientHandlers need copy semantics.
//
//   Additional Operations performed - 
//   usingCodeAnalyser - This method takes the commandline arguments from int 
//                       main and assigns these values to the variables 'targc'
//                       and 'targv'.
//   readMessage () -    Added a condition to detect 'GET' type of a message.
//   


class ClientHandler
{

public:
	ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	void usingCodeAnalyser(int argc, char* argv[]);
	int targc;
	char* targv[5];
private:

	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
};
//----< this defines processing to frame messages >------------------
void ClientHandler::usingCodeAnalyser(int argc, char*argv[])
{

	for (int i = 0; i<argc; i++)
	{
		targc = argc;
		targv[i] = argv[i];
	}
}
HttpMessage ClientHandler::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg;

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

	if (msg.attributes()[0].first == "GET")
	{
		
		size_t numBytes = 0;
		size_t pos = msg.findAttribute("content-length");
		if (pos < msg.attributes().size())
		{
			CodeAnalysis::CodeAnalysisExecutive ce;
			ce.completeCodeAnalysis(targc, targv);
			Sleep(100);

			Serversidetransmission server;
			server.execute(100, 1);
			numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
			Socket::byte* buffer = new Socket::byte[numBytes + 1];
			socket.recv(numBytes, buffer);
			buffer[numBytes] = '\0';
			std::string msgBody(buffer);
			msg.addBody(msgBody);
			delete[] buffer;
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
bool ClientHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname = "../ServerFiles/" + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
	
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

void ClientHandler::operator()(Socket socket)
{
	while (true)
	{

		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			std::cout << "\n\n  clienthandler thread is terminating";
			break;
		}
		msgQ_.enQ(msg);
	}
}


//----< factory for creating messages >------------------------------
/*
* This function only creates one type of message for this demo.
* - To do that the first argument is 1, e.g., index for the type of message to create.
* - The body may be an empty string.
* - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
*   expects the receiver EndPoint for the toAddr attribute.
*/
HttpMessage Serversidetransmission::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
											 // given to its constructor.
	switch (n)
	{
	case 1:
		msg.clear();
		msg.addAttribute(HttpMessage::attribute("POST", "Message"));
		msg.addAttribute(HttpMessage::Attribute("mode", "twoway"));
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

void Serversidetransmission::sendMessage(HttpMessage& msg, Socket& socket)
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
bool Serversidetransmission::sendFile(const std::string& filename, Socket& socket)
{
	// assumes that socket is connected

	std::string fqname = "ResultFolder/" + filename;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8081");
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

void Serversidetransmission::execute(const size_t TimeBetweenMessages, const size_t NumMessages)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{
			std::cout << "\n server waiting to connect";
			::Sleep(100);
		}
		HttpMessage msg;
		for (size_t i = 0; i < NumMessages; ++i)
		{
			std::cout << "\n----------------------------------Requirement 3 and Requirement 4 (Publish Wepages)--------------------------------------------\n";
			std::string msgBody =
				"<msg>Message #" + Converter<size_t>::toString(i + 1) +
				" from server. Here are the requested files! </msg>";
			msg = makeMessage(1, msgBody, "localhost:8081");		
			sendMessage(msg, si);
			std::cout << "\n\n  server sent\n" + msg.toIndentedString();
				::Sleep(TimeBetweenMessages);
		}
		std::vector<std::string> files = FileSystem::Directory::getFiles("ResultFolder", "*.htm");
		for (size_t i = 0; i < files.size(); ++i)
		{
			std::cout << "\n\n  sending file " + files[i];
			sendFile(files[i], si);
		}		
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exeception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
}


//----< test stub >--------------------------------------------------

int main(int argc, char* argv[])
{
	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");
	BlockingQueue<HttpMessage> msgQ;
	try
	{
		SocketSystem ss;
		SocketListener sl(8080, Socket::IP6);
		//Serversidetransmission c1;
		ClientHandler cp(msgQ);
		sl.start(cp);
		cp.usingCodeAnalyser(argc, argv);		
		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			std::cout << "\n\n  server recvd message with body contents:\n" + msg.bodyString();
		}
	}
	catch (std::exception& exc)
	{
		std::cout << "\n  Exeception caught: ";
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		std::cout << exMsg;
	}
}
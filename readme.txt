OOD- Project 4 Remote Code Publisher

The project's main functionality lies in MsgServer and MsgClient.
The operations performed by the client :
1. Communicate with server using HTTP Message communication. Implemented a switch case accepting
   different types of messages from different clients running on 3 different threads.
2. The sending and receiving of bytes is done during the transmission of files from client 
   folder to server folder.
3. When the client requests for download, the files are sent by detecting the type GET.

Server side communication. 

1. On getting a message, displays on the console. 
2. On receiving a file, prints out reception acknowledgement.
3. On detecting GET type of message, begins socket connector, Publishes the sent files
   and sends htm files to client.

   The folder in which all the downloaded files are sent is 'ClientDownloaded'. The folder where the server receives the files is 
   'ServerFiles'. The folder where all the files to be uploaded from the client are in the 'TestFiles'. 

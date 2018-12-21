// UDP client that uses blocking sockets

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "conio.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SERVER_IP_ADDRESS "0:0:0:0:0:0:0:1"	// IPv6 address of server in localhost
#define SERVER_PORT 27014					// Port number of server that will be used for communication with clients
#define BUFFER_SIZE 512						// Size of buffer that will be used for sending and receiving messages to client
#define START_BUFF 20                       // Size of buffer that will be used for start request message

char dataBuffer[BUFFER_SIZE + 1];
FILE* outFile;
int recvResult;
SOCKET clientSocket;
sockaddr_in6 serverAddress;

void creatingSocket(SOCKET* socketName)
{

		// Create a socket
    *socketName = socket(AF_INET6,      // IPv6 address famly
								 SOCK_STREAM,   // stream socket
								 IPPROTO_TCP); // TCP protocol

	// Check if socket creation succeeded
    if (*socketName == INVALID_SOCKET)
    {
        printf("Creating socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return ;
	}else{
		printf("Napravio je novi socket!\n");
	}




}

void receiving()
{
	int i;

	for (i = 0; i < 50/*POF*/; i++)
	{
		memset(dataBuffer, 0, BUFFER_SIZE);
		
		recvResult = recv(	clientSocket,						// Own socket
			            dataBuffer,							// Buffer that will be used for receiving message
						BUFFER_SIZE,							// Maximal size of buffer
						0);									// No flags

		printf("\n%s\n",dataBuffer);
		fprintf(outFile,"%s", dataBuffer);
	}

}

int main()
{
    // Server address structure
    
	
    // Size of server address structure
	int sockAddrLen = sizeof(serverAddress);

	// Buffer that will be used for sending and receiving messages to client
	outFile = fopen("izlaz.txt", "wb");
    //char dataBuffer[BUFFER_SIZE + 1];
	char sendReq[START_BUFF];

	// WSADATA data structure that is used to receive details of the Windows Sockets implementation
    WSADATA wsaData;
    
	// Initialize windows sockets for this process
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    
	// Check if library is succesfully initialized
	if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

   // Initialize memory for address structure
    memset((char*)&serverAddress, 0, sizeof(serverAddress));		
    
	
	 // Initialize address structure of server
	serverAddress.sin6_family = AF_INET6;								// IPv6 address famly
    inet_pton(AF_INET6, SERVER_IP_ADDRESS, &serverAddress.sin6_addr);	// Set server IP address using string
    serverAddress.sin6_port = htons(SERVER_PORT);						// Set server port
	serverAddress.sin6_flowinfo = 0;									// flow info
	 
	creatingSocket(&clientSocket);

	if(connect(clientSocket,(struct sockaddr *)&serverAddress , sizeof(serverAddress))<0)
	{
	    perror("connect failed. Error");
        return 1;
	}



	printf("Send ""START"" if you want to start receiving file !:\n");

	// Read string from user into outgoing buffer
	gets_s(sendReq, BUFFER_SIZE);

	iResult = send(		clientSocket,						// Own socket
						 sendReq,						// Text of message
						 strlen(sendReq),				// Message size
						 0);								// 

	


	//memset(dataBuffer, 0, BUFFER_SIZE);

	// PRIMANJEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    /*iResult = recv(	   clientSocket,						// Own socket
			            dataBuffer,							// Buffer that will be used for receiving message
						BUFFER_SIZE - 1,							// Maximal size of buffer
						0);									// No flags
						//(struct sockaddr *)&clientAddress,	// Client information from received message (ip address and port)
						// &sockAddrLen);						// Size of sockadd_in structure
	*/
	receiving();
	//printf("\n%s\n",dataBuffer);
	//fprintf(outFile,"%s", dataBuffer);
	fclose(outFile);
	// Check if message is succesfully received
	if (iResult == SOCKET_ERROR)
	{
		printf("recvfrom failed with error: %d\n", WSAGetLastError());
	}
	printf("\n%d\n",iResult);



	while(1)
	{
		printf("Enter message to send:\n");

		// Read string from user into outgoing buffer
		gets_s(dataBuffer, BUFFER_SIZE);
	
		// Send message to server
		iResult = send(clientSocket,						// Own socket
						 dataBuffer,						// Text of message
						 strlen(dataBuffer),				// Message size
						 0);								// No flags
						 //(SOCKADDR *)&serverAddress,		// Address structure of server (type, IP address and port)
						// sizeof(serverAddress));			// Size of sockadr_in structure

		// Check if message is succesfully sent. If not, close client application
		if (iResult == SOCKET_ERROR)
		{
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}
	}
	// Only for demonstration purpose
	printf("Press any key to exit: ");
	_getch();

	// Close client application
    iResult = closesocket(clientSocket);
    if (iResult == SOCKET_ERROR)
    {
        printf("closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
        return 1;
    }

	// Close Winsock library
    WSACleanup();

	// Client has succesfully sent a message
    return 0;
}

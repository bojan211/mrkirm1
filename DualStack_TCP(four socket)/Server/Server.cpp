// UDP server that use blocking sockets

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

#define SERVER_PORT 27015	// Port number of server that will be used for communication with clients
#define BUFFER_SIZE 512		// Size of buffer that will be used for sending and receiving messages to clients
#define FILE_NAME "test8mb.txt"
#define START_BUFF 20                       // Size of buffer that will be used for start request message

SOCKET serverSocket1;
SOCKET serverSocket2;
SOCKET serverSocket3;
SOCKET serverSocket4;


int POF;  //parts of file
int sendResult;
int conffd;
FILE* fileForSend;
char dataBuffer[BUFFER_SIZE + 1];

// Checks if ip address belongs to IPv4 address family
bool is_ipV4_address(sockaddr_in6 address);

void creatingSocket(){}



void sending()
{
	int i;
	int offset = 0;
	for (i = 0; i < 50/*POF*/; i++)
	{
		memset(dataBuffer, 0, BUFFER_SIZE);

		fseek(fileForSend, offset, SEEK_SET);
		fread(dataBuffer, sizeof(char), BUFFER_SIZE + 1, fileForSend);
		offset += BUFFER_SIZE;
		puts(dataBuffer);

		sendResult = send(	conffd,						// Own socket
						 dataBuffer,						// Text of message
						 BUFFER_SIZE,				// Message size
						 0);
	}

}

int main()
{
    // Server address 
     sockaddr_in6  serverAddress , clientAddr; 
	 
	 int len;
	 int lenOfFile;
	 int i;
	 

	 // Buffer we will use to send and receive clients' messages
     
	 char startBuffer[START_BUFF];

	
	 
	//OPENING FILE FOR SENDING
	fileForSend = fopen(FILE_NAME, "rb");
	if(fileForSend == NULL) {
		printf("Can't open the file %s", FILE_NAME);
	}
	else {
		printf("File is open!");
		fseek(fileForSend, 0, SEEK_END);
		lenOfFile = ftell(fileForSend);
		printf("%d\n", lenOfFile);
		//fgets(globalBuffer, sizeof globalBuffer, fileForSend);
	}

	POF = lenOfFile / BUFFER_SIZE + 1;
	printf("\n%d\n", POF);

	// WSADATA data structure that is to receive details of the Windows Sockets implementation
    WSADATA wsaData;

	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Initialize serverAddress structure used by bind function
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin6_family = AF_INET6; 			// set server address protocol family
    serverAddress.sin6_addr = in6addr_any;			// use all available addresses of server
    serverAddress.sin6_port = htons(SERVER_PORT);	// Set server port
	serverAddress.sin6_flowinfo = 0;				// flow info

    // Create a socket 
    SOCKET serverSocket = socket(AF_INET6,      // IPv6 address famly
								 SOCK_STREAM,   // stream socket
								 IPPROTO_TCP); // TCP

	// Check if socket creation succeeded
    if (serverSocket == INVALID_SOCKET)
    {
        printf("Creating socket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
	
	// Disable receiving only IPv6 packets. We want to receive both IPv4 and IPv6 packets.
	int no = 0;     
	int iResult = setsockopt(serverSocket, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&no, sizeof(no)); 
	
	if (iResult == SOCKET_ERROR) 
			printf("failed with error: %u\n", WSAGetLastError());


    // Bind server address structure (type, port number and local address) to socket
    iResult = bind(serverSocket,(SOCKADDR *)&serverAddress, sizeof(serverAddress));

	// Check if socket is succesfully binded to server datas
    if (iResult == SOCKET_ERROR)
    {
        printf("Socket bind failed with error: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

 if ((listen(serverSocket, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(clientAddr); 


	conffd = accept(serverSocket, (sockaddr*)&clientAddr, &len); 
    if (conffd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
	
	//Receiving start message! If that message is "START" , start sending
	iResult = recv(	   conffd,						// Own socket
			            startBuffer,							// Buffer that will be used for receiving message
						START_BUFF,							// Maximal size of buffer
						0);	

	if (!strncmp(startBuffer, "START", iResult)) 
	{
		printf("Sending start!");
		sending();
	}
	
	// Set whole buffer to zero
    //    memset(dataBuffer, 0, BUFFER_SIZE);
	//for (i = 0; i < POF; i++)
	//{
	//	fseek(fileForSend, 0*BUFFER_SIZE/**i*/, SEEK_SET);
	//	fread(dataBuffer, sizeof(char), BUFFER_SIZE - 1, fileForSend);
		
	//	puts( dataBuffer);
	//}

	//APFAPFPSAFPASPFPAS
	/*iResult = send(	conffd,						// Own socket
						 dataBuffer,						// Text of message
						 BUFFER_SIZE - 1,				// Message size
						 0);								// 
	*/

    // Main server loop
    while(1)
    {
        // Declare and initialize client address that will be set from recvfrom
        sockaddr_in6 clientAddress;
		memset(&clientAddress, 0, sizeof(clientAddress));

		// Set whole buffer to zero
        memset(dataBuffer, 0, BUFFER_SIZE);

		// size of client address
		int sockAddrLen = sizeof(clientAddress);

		// Receive client message
        iResult = recv(	   conffd,						// Own socket
			               dataBuffer,							// Buffer that will be used for receiving message
						   BUFFER_SIZE,							// Maximal size of buffer
						   0);									// No flags
						   //(struct sockaddr *)&clientAddress,	// Client information from received message (ip address and port)
						  // &sockAddrLen);						// Size of sockadd_in structure
		
		// Check if message is succesfully received
		if (iResult == SOCKET_ERROR)
		{
			printf("recvfrom failed with error: %d\n", WSAGetLastError());
			continue;
		}

        char ipAddress[INET6_ADDRSTRLEN]; // INET6_ADDRSTRLEN 65 spaces for hexadecimal notation of IPv6
		
		// Copy client ip to local char[]
		inet_ntop(clientAddress.sin6_family, &clientAddress.sin6_addr, ipAddress, sizeof(ipAddress));
        
		// Convert port number from network byte order to host byte order
        unsigned short clientPort = ntohs(clientAddress.sin6_port);

		bool isIPv4 = is_ipV4_address(clientAddress); //true for IPv4 and false for IPv6

		if(isIPv4){
			char ipAddress1[15]; // 15 spaces for decimal notation (for example: "192.168.100.200") + '\0'
			struct in_addr *ipv4 = (struct in_addr*)&((char*)&clientAddress.sin6_addr.u)[12]; 
			
			// Copy client ip to local char[]
			strcpy_s(ipAddress1, sizeof(ipAddress1), inet_ntoa( *ipv4 ));
			printf("IPv4 Client connected from ip: %s, port: %d, sent: %s.\n", ipAddress1, clientPort, dataBuffer);
		}else
			printf("IPv6 Client connected from sent: %s.\n",dataBuffer);
		
		// Possible server-shutdown logic could be put here
    }

    // Close server application
    iResult = closesocket(serverSocket);
    if (iResult == SOCKET_ERROR)
    {
        printf("closesocket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
        return 1;
    }
	
	printf("Server successfully shut down.\n");
	
	// Close Winsock library
	WSACleanup();
	_getch();
	return 0;
}

bool is_ipV4_address(sockaddr_in6 address)
{
	char *check = (char*)&address.sin6_addr.u;

	for (int i = 0; i < 10; i++)
		if(check[i] != 0)
			return false;
		
	if(check[10] != -1 || check[11] != -1)
		return false;

	return true;
}
/*  // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 
   */
#include "AddSocket.h"
#include <iostream>
#include <winsock2.h>
#include <thread>
SOCKET s;
AddSocket::AddSocket()
{ 
	s = NULL;
}

int AddSocket::InitSock() {
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("There was an error creating a socket: %d", WSAGetLastError());
		std::terminate();
	}
	return 1;
}

AddSocket::~AddSocket()
{
}

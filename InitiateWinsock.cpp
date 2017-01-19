# include "InitiateWinsock.h"
# include <stdio.h>
#include <iostream>
# include<WinSock2.h>

# pragma comment(lib,"ws2_32.lib")//WinSockLibrary

InitiateWinsock::InitiateWinsock()
{
	WSADATA wsa;

	std::cout << "Initializing winsock ...";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(0);
	}
	std::cout << "\nInitialization successful" << std::endl;
}



InitiateWinsock::~InitiateWinsock()
{
}

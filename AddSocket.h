#ifndef ADDSOCKET_H
#define ADDSOCKET_H
#pragma once
#include <winsock2.h>

class AddSocket
{
public:
	SOCKET s;
	AddSocket();
	int InitSock();
	~AddSocket();
};
#endif


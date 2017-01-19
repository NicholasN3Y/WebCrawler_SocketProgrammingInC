#ifndef DRIVER_H
#define DRIVER_H

#pragma once
#include <unordered_map>
#include <queue>
#include <string>
#include <winsock2.h>
#include "InitiateWinsock.h"
#include "AddSocket.h"
#include <iostream>
#include <fstream>
#include "stdio.h"
#include "stdlib.h"
#include "Parser.h"
#include "WebpageRep.h"
#include <mutex>

class Driver
{

public:
	static std::mutex m;
	static std::mutex q;
	static std::mutex f;
	static std::mutex c;
	static int totalCrawled;
	Driver();
	static std::unordered_map<std::string, int> url_lists;
	static std::unordered_map<std::string, std::vector<int>> serverList;
	static std::queue <std::string> queue;
	static void mythread(int threadid);
	static void CrawlInstance(int threadid);
	static WebpageRep bindAUrl(SOCKET sock);
	~Driver();
};
#endif

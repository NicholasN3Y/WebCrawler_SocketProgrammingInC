#include "Driver.h"
#include "WebpageRep.h"
#include <chrono>
#include <regex>
#include <thread>
#include <sstream>
#include <future>
#include <exception>
#include <numeric>
#define SOCKET_READ_TIMEOUT_SEC 5

typedef std::chrono::high_resolution_clock Clock;
std::unordered_map<std::string, int> Driver::url_lists;
std::unordered_map<std::string, std::vector<int>> Driver::serverList;
std::queue<std::string> Driver::queue;
std::ofstream info;
std::ofstream consoleLog;
std::ofstream avgInfo;
std::ofstream links;
std::mutex Driver::m;
std::mutex Driver::q;
std::mutex Driver::f;
std::mutex Driver::c;
int Driver::totalCrawled;

//This is the function that a thread runs.
void Driver::CrawlInstance(int threadid) {

		//initiate an instance of a socket
		AddSocket *A = new AddSocket();
		A->InitSock();
		SOCKET soc = A->s;

		WebpageRep urlBound = Driver::bindAUrl(soc);
		char buffer[1024];
		std::string source = "";
		std::string inform;
		if (urlBound.hostname.compare("null") == 0)
		{
			return;
		}
		else
		{
			inform = "***CONNECTION ESTABLISHED ***\t Thread: " + std::to_string(threadid) + "\n";
			std::string reqstr = "GET " + urlBound.page + " HTTP/1.1\r\n";
			reqstr += "Host: " + urlBound.hostname + "\r\n";
			reqstr += "Connection: close\r\n\r\n";
			inform += reqstr;
			size_t reqstr_size = reqstr.size();
			char request[4096];
			strncpy(request, reqstr.c_str(), reqstr_size);

			send(soc, request, strlen(request), 0);

			int i = 0;
			DWORD timeout = SOCKET_READ_TIMEOUT_SEC * 1000;
			setsockopt(soc, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

			while ((i = recv(soc, buffer, sizeof(buffer) - 1, 0)) > 0) 
			{
				buffer[i] = '\0';
				source += buffer;
			}
			if (i == SOCKET_ERROR) 
			{
				if (WSAGetLastError() != WSAETIMEDOUT) 
				{
					std::cout << "Socket timeout";
					std::this_thread::yield();
					closesocket(soc);
					return;
				}
			}

			std::cout << std::endl << inform;
			Driver::c.lock();
			consoleLog << inform << "\n";
			Driver::c.unlock();
			Parser::parseFile(&urlBound, source);
		}
		closesocket(soc);
}

//converts hostname to ip, binds it to a socket and establish a connection
WebpageRep Driver::bindAUrl(SOCKET sock) {
	try {
		//obtain topmost url
		q.lock();
		while (Driver::queue.empty()) 
		{
			q.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(2));
			q.lock();
		}
		std::string url = Driver::queue.front();

		//represent it as a webpage object
		WebpageRep* a = new WebpageRep();
		std::string hostname = a->getHostName(&url);
		a->parse(hostname, &url);

		Driver::queue.pop();
		q.unlock();

		std::string domain = a->hostname;
		if (gethostbyname(domain.c_str()) == nullptr)
		{
			throw new std::exception();
		}

		//construct the socket.
		HOSTENT webDomain = *gethostbyname(domain.c_str());
		in_addr addr = *reinterpret_cast<in_addr*>(webDomain.h_addr_list[0]);
		sockaddr_in sockAddr;
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_port = htons(80);
		sockAddr.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(addr));
		auto start = Clock::now();
		//establish connection
		int status = connect(sock, (SOCKADDR *)&sockAddr, sizeof(sockAddr));
		if (status == 0)
		{
			auto end = Clock::now();
			auto period = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
			std::string info_str = "connection to webserver " + a->hostname + " took " + period + " milliseconds (RTT)\n";
			std::cout << info_str;
			if (serverList.find(a->hostname) != serverList.end()) 
			{
				serverList.at(a->hostname).push_back(std::stoi(period));
			}
			else 
			{
				serverList.insert({ (a->hostname), std::vector<int>() });
				serverList.at(a->hostname).push_back(std::stoi(period));
			}

			Driver::f.lock();
			info << (info_str.c_str());
			Driver::totalCrawled++;
			Driver::f.unlock();
			return *a;
		}
		else
		{
			//add back the url to the list
			q.lock();
			Driver::queue.push(url);
			q.unlock();
			a->clear();
			return *a;
		}
	}
	catch (...) {
		//for all exceptions, jsut return a empty webpage object
		WebpageRep* a = new WebpageRep();
		return *a;
	}
}

Driver::Driver()
{
	url_lists = std::unordered_map<std::string, int>();
	queue = std::queue<std::string>();
	serverList = std::unordered_map<std::string, std::vector<int>>();
}

Driver::~Driver()
{
}

void Driver::mythread(int threadid)
{
	try {
		Driver::CrawlInstance(threadid);
	}
	catch (std::exception e) 
	{
		;
	}
}

int main(int argc, char*argv[]) {

	std::cout << "main thread is " << std::this_thread::get_id() << std::endl;
	info.open("communication_info.txt", std::ios::app);
	
	char pause = 0;
	InitiateWinsock::InitiateWinsock();

	std::ifstream a("url.dat");
	std::string url;

	//reading initial set of websites 
	if (a.is_open())
	{
		while (getline(a, url))
		{
			if (Driver::url_lists.find(url) == Driver::url_lists.end())
			{
				std::regex http("http://");
				Driver::url_lists.insert({ std::regex_replace(url,http,""), 0 });
				Driver::queue.push(url);
			}
		}
		a.close();
	}

	//multithread
	int numThreads = std::stoi(argv[2]);
	consoleLog.open("consoleLog.txt", std::ios::app);
	do
	{
		std::thread* ThreadList = new std::thread[numThreads];
		for (int i = 0; i < numThreads; i++)
		{
			ThreadList[i] = std::thread(Driver::mythread, i+1);
		}

		for (int i = 0; i < numThreads; i++)
		{
			ThreadList[i].join();
		}

		for (int i = 0; i < numThreads; i++)
		{
			ThreadList[i].~thread();
		}

		std::this_thread::sleep_for(std::chrono::seconds(2));
		
	} while (Driver::totalCrawled < ((std::stoi(argv[1]) * 100) + 1));

	links.open("crawlerResult.txt");
	for (auto linkitr = Driver::url_lists.begin(); linkitr != Driver::url_lists.end(); ++linkitr)
	{
		links << linkitr->first << std::endl;
	}
	
	avgInfo.open("average_comm_info.txt", std::ios::app);
	for (auto infoitr = Driver::serverList.begin(); infoitr != Driver::serverList.end(); ++infoitr)
	{
		std::vector <int> v = infoitr->second;
		double average = std::accumulate(v.begin(), v.end(), 0LL) / v.size();
		avgInfo << infoitr->first + " has average communication RRT of " + std::to_string(average) + " milliseconds\n" ;
	}

	links.close();
	avgInfo.close();
	consoleLog.close();
	info.close();
	
	return 0;

}




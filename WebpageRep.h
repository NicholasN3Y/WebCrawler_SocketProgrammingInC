#ifndef WEBPAGEREP_H
#define WEBPAGEREP_H
#pragma once
#include "boost/algorithm/string.hpp"
#include "boost/regex.hpp"
#include <string>
class WebpageRep
{
public:
	std::string nature;
	std::string hostname;
	std::string page;
	WebpageRep();
	std::string getHostName(std::string* string);
	void parseNewLink(const std::string hostname, const std::string link);
	void parse(const std::string source_host, std::string* link);
	void clear();
	~WebpageRep();
};
#endif


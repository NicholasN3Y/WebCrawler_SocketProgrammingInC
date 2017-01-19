#ifndef PARSER_H
#define PARSER_H
#pragma once
#include <string>

#include "boost/algorithm/string.hpp"
#include "boost/regex.hpp"
#include "WebpageRep.h"

#include <algorithm>

class Parser
{

public:
	Parser();
	static void parseFile(WebpageRep* current_website, std::string source);

	~Parser();
};
#endif


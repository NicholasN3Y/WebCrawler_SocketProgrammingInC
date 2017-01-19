#include "Parser.h"
#include "WebpageRep.h"
#include <iostream>
#include "Driver.h"
#include <regex>


Parser::Parser()
{
}

//given a source file, parse the websites.
void Parser::parseFile(WebpageRep* current_website, std::string source) {
	const boost::regex rmv_all("[\\r|\\n]");
	const std::string trimmedsource = boost::regex_replace(source, rmv_all, "");
	const std::string s = trimmedsource;
	const boost::regex re("<a\\s+(?:[^>]*?\\s+)?href=\"([^\"]*)\"");
	std::regex e("//+");
	boost::cmatch matches;

	const int subs[] = { 2, 4 };
	boost::sregex_token_iterator link_itr(s.begin(), s.end(), re, 1);
	boost::sregex_token_iterator j;
	Driver::q.lock();
	Driver::m.lock();
	for (; link_itr != j; link_itr++) {
		//iterate through listed LINKS.
		std::string link = *link_itr;
		if (link.length() != 0) {
			WebpageRep *page = new WebpageRep();
			page->parse(current_website->hostname, &link);
			if (page->nature == "https") {
				//std::cout << "SKIP HTTPS";
				continue;
			}
			std::string buildlink = page->hostname + "/" + page->page;
			buildlink = std::regex_replace(buildlink, e, "/");
			std::regex z("#/");
			buildlink = std::regex_replace(buildlink, z, "");
			
			if (Driver::url_lists.find(buildlink) == Driver::url_lists.end())
			{
					Driver::url_lists.insert({ buildlink, NULL });
					Driver::queue.push("http://" + buildlink);
			}
		}
	}
	std::string curr =  current_website->hostname + "/" + current_website->page;
	curr = std::regex_replace(curr, e, "/");
	Driver::url_lists.at(curr) = 1;
	Driver::q.unlock();
	Driver::m.unlock();
}

Parser::~Parser()
{
}

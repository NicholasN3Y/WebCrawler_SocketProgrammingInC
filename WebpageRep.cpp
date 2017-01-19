#include "WebpageRep.h"

WebpageRep::WebpageRep()
{
	hostname = "";
	page = "";
	nature = "";
}

//extract hostname from URL
std::string WebpageRep::getHostName(std::string* string)
{
	const boost::regex re("(?i)(http|https):\/\/(.*?)\/(.*)");
	boost::smatch what;
	if (boost::regex_match(*string, what, re)) {
		std::string hst = what[2];
		boost::algorithm::to_lower(hst);
		return hst;
	}else {
		*string = *string + "/";
		if (boost::regex_match(*string, what, re)) {
			std::string hst = what[2];
			boost::algorithm::to_lower(hst);
			return hst;
		}
	}
	return "";
}

//parse new link new domain
void WebpageRep::parseNewLink(const std::string hostname, const std::string link)
{
	const boost::regex re("(?i)(http|https):\/\/(.*?)\/(.*)");
	boost::smatch match;
	if (boost::regex_match(link, match, re)) {
		this->nature = match[1];
		this->hostname = match[2];
		boost::algorithm::to_lower(this->hostname);
		page = match[3];
	}
	else {
		//is direct hostname main page
		this->hostname = hostname;
		page = "";
	}
}

//parse to see if new domain or not
void WebpageRep::parse(const std::string source_host, std::string* link)
{
	std::string newhost = getHostName(link);
	if (newhost.empty()) 
	{
		//the link is relative to source host
		hostname = source_host;
		page = *link;
	}
	else 
	{
		//link given is under another domain
		parseNewLink(newhost, *link);
		if (page == "") 
		{
			page = "/";
		}
	}
}

void WebpageRep::clear() {
	hostname = "null";
	page = "null";
	nature = "null";
}

WebpageRep::~WebpageRep()
{
}

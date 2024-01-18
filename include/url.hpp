#include <netdb.h>
#include <regex.hpp>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include "struct.hpp"

#ifndef URL_H
#define URL_H

class URL : public info {
public:
    URL(std::string url) {
        this->url = url;
        setRegex(url, hostname, REGEX_DOMAIN, 0);
        ipv4 = getIP(hostname.c_str());
        port = getPort(url);
    }
    
    URL(const char* url) : URL(std::string(url)) {}
private:
    static std::string getIP(const char *host);
    static unsigned short getPort(const std::string& url);
};

#endif // URL_H
#include <url.hpp>
#include <logger.hpp>

std::string URL::getIP(const char *host) {
    if (host == NULL) {
        return std::string();
    }

    struct hostent *pstHostent = NULL;
    if (inet_addr(host) == INADDR_NONE) {
        if ((pstHostent = gethostbyname(host)) == NULL) {
            return std::string();
        }
        char *ipaddr = inet_ntoa(*(struct in_addr *)pstHostent->h_addr);
        if (ipaddr) {
            return std::string(ipaddr);
        }
    } else {
        return std::string(host);
    }
    return std::string();
}

unsigned short URL::getPort(const std::string& url) {
    std::smatch match;
    std::regex pattern("http[s]?://[^:]+:([0-9]+)/.*");
    std::regex pattern2("^(http|https)://");
    if (std::regex_match(url, match, pattern)) {
        return static_cast<unsigned short>(std::stoi(match[1]));
    } else if (std::regex_search(url, match, pattern2)) {
        std::string scheme = match[1];
        if (scheme == "https") return 443;
        else if (scheme == "http") return 80;
        else Logger::error("Invalid scheme: ", scheme);
    } else {
        Logger::error("Invalid URL: ", url);
    }
    return 80;
}
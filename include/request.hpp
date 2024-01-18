#include <string>
#include "url.hpp"
#include "struct.hpp"
#include "response.hpp"
#include "logger.hpp"

#ifndef REQUEST_H
#define REQUEST_H

class Request : public info, public request {
public:
    Request(URL url) : info(url) {}
    Request(std::string url) : info(URL(url)) {}
    Request(const char* url) : info(URL(url)) {}
    Response perform();
private:
    std::string http();
    std::string https();
};
#endif // REQUEST_H
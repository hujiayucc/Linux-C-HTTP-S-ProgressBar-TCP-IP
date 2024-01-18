#include <string>

#ifndef RESPONSE_H
#define RESPONSE_H

class Response {
public:
    Response(std::string request) : body(request) {
        if (body == "") {
            throw std::string("\033[31mResponseException:\033[0m Request Body is empty!");
        }
    }
    
    Response() {
        throw std::string("\033[31mResponseException:\033[0m Request Body is empty!");
    }
    
    std::string body;
};
#endif // RESPONSE_H
#include <string>
#include "enum.h"

#ifndef STRUCT_H
#define STRUCT_H
struct info {
    std::string ipv4;
    std::string url;
    std::string hostname;
    unsigned short port = 443;
};

struct request {
    std::string ua;
    methods method;
};

#endif // STRUCT_H
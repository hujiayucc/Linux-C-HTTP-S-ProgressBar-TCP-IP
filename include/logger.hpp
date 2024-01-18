#ifndef LOGGER_H
#define LOGGER_H
#include "enum.h"
#include <cstdio>
#include <string>
class Logger {
    public:
        static void info(const char* tag, const char* log);
        static void error(const char* tag, const char* log);
        static void warn(const char* tag, const char* log);
        static void info(std::string tag, std::string log) { info(tag.c_str(), log.c_str()); }
        static void error(std::string tag, std::string log) { error(tag.c_str(), log.c_str()); }
        static void warn(std::string tag, std::string log) { warn(tag.c_str(), log.c_str()); }
        static void info(const char* tag, std::string log) { info(tag, log.c_str()); }
        static void error(const char* tag, std::string log) { error(tag, log.c_str()); }
        static void warn(const char* tag, std::string log) { warn(tag, log.c_str()); }
};
#endif // LOGGER_H
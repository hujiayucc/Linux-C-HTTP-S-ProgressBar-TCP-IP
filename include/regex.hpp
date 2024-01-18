#include <string>
#include <regex>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef REGEX_H
#define REGEX_H

static const char* REGEX_DOMAIN = "[^//]*?\\.([^/ | ^:{1,5}/]*)";

static void setRegex(std::string oldtext, std::string &text, const char* regex_, int index) {
    // 定义正则表达式模式
    std::regex regex(regex_);
    // 查找匹配的子串
    std::smatch match;
    if (std::regex_search(oldtext, match, regex)) {
        // 提取主机名
        text = match[index].str();
    }
}
#endif // REGEX_H
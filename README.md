### Linux C++ HTTP(S) && ProgressBar && TCP/IP

```cpp
static const char* REGEX_DOMAIN = "[^//]*?\\.([^/|^:{1,5}/]*)";
```

先写个URL方便存储URL的信息

```cpp
struct info {
    std::string ipv4;
    std::string url;
    std::string hostname;
    unsigned short port = 443;
};
```

**url.cpp**

```cpp
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
```

**url.hpp**

```cpp
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
```

**http.hpp**

```cpp
#include <logger.hpp>
#include "request.hpp"

bool http(Request request) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        Logger::error("XYL", "Error creating socket.");
        return false;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(request.port);
    server_addr.sin_addr.s_addr = inet_addr(request.ipv4.c_str());
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        Logger::error("XYL", "Error connecting.");
        return false;
    }
    std::string GET_REQUEST = ("GET / HTTP/1.1\r\nHost: " + request.hostname + "\r\nConnection: close\r\n\r\n");
    if (write(sockfd, GET_REQUEST.c_str(), GET_REQUEST.length()) < 0) {
        Logger::error("XYL", "Error writing to socket.");
        return false;
    }
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    std::string body;
    while (read(sockfd, buffer, sizeof(buffer)) > 0) {
        body.append(buffer);
    }
    std::cout << buffer << std::endl;
    close(sockfd);
    return true;
}
```

既然有http了，那就再加个https吧。整合一下思路，这里不使用网络编程库和libcurl，使用C++标准库。通过OpenSSL进行TLS握手，不过我这里没做证书验证，方便一些开发环境下的自签证书。

**request.cpp**

```cpp
#include <request.hpp>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

Response Request::perform() {
    if (port == 80) {
        return Response(http());
    } else if (port == 443) {
        return Response(https());
    } else {
        std::smatch match;
        std::regex pattern2("^(http|https)://");
        if (std::regex_search(url, match, pattern2)) {
            std::string scheme = match[1];
            if (scheme == "https") return Response(https());
            else if (scheme == "http") return Response(http());
            else Logger::error("Invalid scheme: ", scheme);
        } else {
            Logger::error("Invalid URL: ", url);
        }
    }
    
    return Response();
}

std::string Request::http() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        Logger::error("XYL", "Error creating socket.");
        return std::string();
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ipv4.c_str());
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        Logger::error("XYL", "Error connecting.");
        return std::string();
    }
    std::string REQUEST = ("GET / HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n");
    Logger::error("Invalid URL: ", REQUEST);
    if (write(sockfd, REQUEST.c_str(), REQUEST.length()) < 0) {
        Logger::error("XYL", "Error writing to socket.");
        return std::string();
    }
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    std::string body;
    while (read(sockfd, buffer, sizeof(buffer)) > 0) {
        body.append(buffer);
    }
    close(sockfd);
    return buffer;
}

std::string Request::https() {
    // 创建socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return std::string();
    }
    // 服务器地址结构体
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ipv4.c_str());
    serv_addr.sin_port = htons(port);
    // 连接服务器
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return std::string();
    }
    // SSL上下文
    SSL_CTX* ctx = SSL_CTX_new(TLS_method());
    if (!ctx) {
        perror("SSL_CTX_new");
        return std::string();
    }
    // SSL连接
    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        perror("SSL_new");
        SSL_CTX_free(ctx);
        return std::string();
    }
    if (SSL_set_fd(ssl, sockfd) != 1) {
        perror("SSL_set_fd");
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return std::string();
    }
    if (SSL_connect(ssl) != 1) {
        perror("SSL_connect");
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return std::string();
    }
    // HTTPS请求
    std::string REQUEST = ("GET / HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n");
    if (SSL_write(ssl, REQUEST.c_str(), REQUEST.length()) != REQUEST.length()) {
        perror("SSL_write");
    }
    // 接收和打印响应
    char *buffer;
    buffer = (char*) malloc(1024);
    std::string body;
    while (SSL_read(ssl, buffer, sizeof(buffer)) > 0) {
        body.append(buffer);
        free(buffer);
        buffer = (char*) malloc(1024);
    }
    free(buffer);
    // 关闭连接
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);
    SSL_CTX_free(ctx);
    return body;
}
```

**request.hpp**

```cpp
#include <string>
#include "url.hpp"
#include "struct.hpp"
#include "response.hpp"
#include "logger.hpp"

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
```

以上代码写了两个结构体，一个是URL的信息，一个是Request结构体。`enum.h`这里可以忽略，因为考虑到GET以及其他请求模式做的请求类型枚举，不过具体功能还没写。

**struct.hpp**

```cpp
#include <string>
#include "enum.h"

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
```

**enum.h**

```cpp
#ifndef ENUM_H
#define ENUM_H

typedef enum {
    DEFAULT = 0,
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    WHITE = 37
} textcolor;

typedef enum {
    GET,
    POST,
    HEAD,
    DELETE,
    PUT
} methods;

#endif // ENUM_H
```

**logger.cpp**

```cpp
#include <logger.hpp>

void Logger::info(const char* tag, const char* log) {
    /*
    // ANSI转义序列，设置文本颜色为青色
    std::cout << colorToString(CYAN) << tag << ":";
    // 重置颜色为默认
    std::cout << colorToString(DEFAULT) << " " << log << std::endl;
    */
    printf("\033[%dm%s:\033[0m %s\n", CYAN, tag, log);
}

void Logger::error(const char* tag, const char* log) {
    /*
    // ANSI转义序列，设置文本颜色为红色
    std::cout << colorToString(RED) << tag << ":";
    // 重置颜色为默认
    std::cout << colorToString(DEFAULT) << " " << log << std::endl;
    */
    printf("\033[%dm%s:\033[0m %s\n", RED, tag, log);
}

void Logger::warn(const char* tag, const char* log) {
    /*
    // ANSI转义序列，设置文本颜色为黄色
    std::cout << colorToString(YELLOW) << tag << ":";
    // 重置颜色为默认
    std::cout << colorToString(DEFAULT) << " " << log << std::endl;
    */
    printf("\033[%dm%s:\033[0m %s\n", YELLOW, tag, log);
}
```

**logger.hpp**

```cpp
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
```

废话讲了这么多，回到原题

现在需要通过http(s)请求获取一个返回体的判断，如果返回体为空就抛出错误

**response.hpp**

```cpp
#include <string>

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
```

写个调用示例运行一下看看

**XYL.cpp**

```cpp
#include <logger.hpp>
#include <request.hpp>
#include <iostream>

int main() {
    URL url("https://www.hujiayucc.cn");
    Logger::info("url", url.url);
    Logger::info("hostname", url.hostname);
    Logger::info("ipv4", url.ipv4);
    Logger::info("port", std::to_string(url.port));
    Request request```cpp
(url);
    try {
        Response response = request.perform();
        std::cout << response.body << std::endl;
    } catch (const std::string& e) {
        std::cerr << e << std::endl;
    }

    return 0;
}
```
![](https://oss.hujiayucc.cn/blog/OSSdTsI20231102101033.jpg)

效果似乎还行

下面是写的一个简洁型进度条(ProgressBar)

**progress.cpp**
```cpp
#include <iostream>
#include <chrono>
#include <thread>

// 定义一个进度条类
class ProgressBar {
    // 定义私有的成员变量
    private:
        int width; // 进度条的宽度，即显示多少个字符
        int progress; // 进度条的进度，即完成了多少百分比
        char fill; // 进度条的填充字符，即用什么字符来表示进度
        char empty; // 进度条的空白字符，即用什么字符来表示剩余空间

    // 定义公有的成员函数
    public:
        // 构造函数，初始化进度条的参数
        ProgressBar(int w, char f, char e) {
            width = w; // 设置宽度
            progress = 0; // 设置初始进度为0
            fill = f; // 设置填充字符
            empty = e; // 设置空白字符
        }

        // 更新函数，根据给定的百分比更新进度条的显示
        void update(int p) {
            progress = p; // 更新进度
            int n = width * progress / 100; // 计算需要显示多少个填充字符
            std::cout << "\r["; // 输出一个回车符和一个左括号，将光标移动到行首
            for (int i = 0; i < n; i++) { // 循环输出填充字符
                std::cout << fill;
            }
            for (int i = 0; i < width - n; i++) { // 循环输出空白字符
                std::cout << empty;
            }
            std::cout << "] " << progress << "%"; // 输出一个右括号和一个百分比
            std::cout.flush(); // 刷新输出缓冲区，使进度条立即显示
        }
};

// 主函数，测试进度条的效果
int main() {
    ProgressBar bar(50, '#', '-'); // 创建一个宽度为50，填充字符为#，空白字符为-的进度条对象
    for (int i = 0; i <= 100; i++) { // 循环更新进度条的进度，从0到100
        bar.update(i); // 调用更新函数，传入当前的百分比
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 暂停0.1秒，模拟一个耗时的任务
    }
    std::cout << std::endl; // 输出一个换行符，结束进度条的显示
    return 0;
}
```
运行看一下效果
![](https://oss.hujiayucc.cn/blog/OSSNpGE20231102101243.jpg)
稍作调整改成彩色试试看
```cpp
// 导入需要的头文件
#include <iostream>
#include <chrono>
#include <thread>

// 定义一些ANSI转义序列，用于设置文本颜色
#define NONE "\033[0m" // 关闭所有属性
#define RED "\033[0;31m" // 红色
#define GREEN "\033[0;32m" // 绿色
#define YELLOW "\033[0;33m" // 黄色
#define BLUE "\033[0;34m" // 蓝色
#define PURPLE "\033[0;35m" // 紫色

// 定义一个进度条类
class ProgressBar {
    // 定义私有的成员变量
    private:
        int width; // 进度条的宽度，即显示多少个字符
        int progress; // 进度条的进度，即完成了多少百分比
        char fill; // 进度条的填充字符，即用什么字符来表示进度
        char empty; // 进度条的空白字符，即用什么字符来表示剩余空间

    // 定义公有的成员函数
    public:
        // 构造函数，初始化进度条的参数
        ProgressBar(int w, char f, char e) {
            width = w; // 设置宽度
            progress = 0; // 设置初始进度为0
            fill = f; // 设置填充字符
            empty = e; // 设置空白字符
        }

        // 更新函数，根据给定的百分比更新进度条的显示
        void update(int p) {
            progress = p; // 更新进度
            int n = width * progress / 100; // 计算需要显示多少个填充字符
            std::cout << "\r["; // 输出一个回车符和一个左括号，将光标移动到行首
            for (int i = 0; i < n; i++) { // 循环输出填充字符
                switch (i % 5) { // 根据不同的位置，选择不同的颜色
                    case 0: std::cout << RED << fill << NONE; break;
                    case 1: std::cout << GREEN << fill << NONE; break;
                    case 2: std::cout << YELLOW << fill << NONE; break;
                    case 3: std::cout << BLUE << fill << NONE; break;
                    case 4: std::cout << PURPLE << fill << NONE; break;
                }
            }
            for (int i = 0; i < width - n; i++) { // 循环输出空白字符
                std::cout << empty;
            }
            std::cout << "] " << progress << "%"; // 输出一个右括号和一个百分比
            std::cout.flush(); // 刷新输出缓冲区，使进度条立即显示
        }
};

// 主函数，测试进度条的效果
int main() {
    ProgressBar bar(50, '#', '-'); // 创建一个宽度为50，填充字符为#，空白字符为-的进度条对象
    for (int i = 0; i <= 100; i++) { // 循环更新进度条的进度，从0到100
        bar.update(i); // 调用更新函数，传入当前的百分比
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 暂停0.1秒，模拟一个耗时的任务
    }
    std::cout << std::endl; // 输出一个换行符，结束进度条的显示
    return 0;
}
```
![](https://oss.hujiayucc.cn/blog/OSSQp5K20231102101409.jpg)
写了大半天，就先到这里吧，下次再讲，晚安

如果对你有帮助，记得关注＋点赞哦～～～

蓝奏云：[点击下载](https://hujiayucc.lanzouq.com/iW7DW1daid3e "点击下载")
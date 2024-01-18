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
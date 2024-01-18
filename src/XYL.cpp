#include <logger.hpp>
#include <request.hpp>
#include <iostream>

int main() {
    URL url("https://www.hujiayucc.cn");
    Logger::info("url", url.url);
    Logger::info("hostname", url.hostname);
    Logger::info("ipv4", url.ipv4);
    Logger::info("port", std::to_string(url.port));
    Request request(url);
    try {
        Response response = request.perform();
        std::cout << response.body << std::endl;
    } catch (const std::string& e) {
        std::cerr << e << std::endl;
    }

    return 0;
}

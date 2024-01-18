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
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

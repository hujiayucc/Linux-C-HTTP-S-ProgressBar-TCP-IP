cmake_minimum_required(VERSION 3.26.4)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
project(XYL)

file(REMOVE_RECURSE "../cache")
file(GLOB_RECURSE SRC ../src/**.cpp ../src/**)
# 生成可执行目标
add_executable(${PROJECT_NAME} ${SRC})
include_directories(../include)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Release")
set(EXECUTABLE_OUTPUT_PATH ../build)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

target_link_libraries(${PROJECT_NAME} PRIVATE curl ssl)
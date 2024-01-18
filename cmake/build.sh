#!/bin/bash

function clean() {
    echo "Cleaned cache folder"
    rm -rf ../cache
}

function build() {
    cmake -DCMAKE_CXX_FLAGS="-std=c++17 -g -O3 $1" -DCMAKE_CXX_COMPILER="g++" $(dirname $0) -B $(dirname $0)/../cache
    cd $(dirname $0)/../cache && make

    echo "Clean cache folder? y(es) or n(o) (default：no)"
    read clean
    if [ "${clean,,}" == "y" ] || [ "${clean,,}" == "yes" ]; then
        clean
    fi
}

if [ "$1" == "clean" ] || [ "$1" == "-clean" ]; then
    clean && exit
fi

if [ "$1" == "static" ] || [ "$1" == "-static" ]; then
    build -static && exit
fi

build
# build a stand-alone program and run
clang++ ./src/main.cpp -std=c++11 -stdlib=libc++ -o ./build/mini-browser
./build/mini-browser

# build a stand-alone program and run

set -e

clang++ ./src/main.cpp -std=c++11 -stdlib=libc++ -o ./build/mini-browser

# json
#./build/mini-browser test/html/basic/index.html test/html/basic/index.json
./build/mini-browser test/css/basic/index.css test/css/basic/index.json
# jq . test/css/basic/index.json | sponge test/css/basic/index.json

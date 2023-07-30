# build a stand-alone program and run

set -e



# Compile the parser.c file
clang -c parser.c -o parser.o

# Compile the lexer.c file
clang -c lexer.c -o lexer.o

# Compile the main.c file
clang -c main.c -o main.o

# Link all object files together to create the final executable
clang parser.o lexer.o main.o -o ./build/css-parser


# clang++ main.c  -std=c++11 -stdlib=libc++ -o ./build/css-parser

./build/css-parser ./test.css

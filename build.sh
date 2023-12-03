mkdir ./output
g++ -std=c++17 -O2 ./src/main.cpp ./src/Lexer.cpp ./src/LR1Parser.cpp -o ./output/LR1Parser

./output/LR1Parser ./test/input/input.txt test/grammer/grammer.txt
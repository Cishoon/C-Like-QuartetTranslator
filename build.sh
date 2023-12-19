mkdir ./output
g++ -std=c++17 -O2 ./src/main.cpp ./src/Lexer.cpp ./src/LR1Parser.cpp ./src/SemanticAnalyzer.cpp -o ./output/Translator

./output/Translator ./test/input/input.txt test/grammer/grammer.txt
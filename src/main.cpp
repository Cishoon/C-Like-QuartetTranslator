#include "Lexer.hpp"
#include "LR1Parser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

void createLogFileIfNotExists(const std::string& filename)
{
	std::ofstream outfile(filename);
	if (outfile) {
		std::cout << "日志文件创建成功！" << std::endl;
	} else {
		std::cerr << "日志文件创建失败！" << std::endl;
	}
	outfile.close();
}

int main(int argc, char* argv[])
{
	if (argc < 3) {
		std::cerr << "用法: " << argv[0] << " <输入文件> <文法文件>" << std::endl;
		return 1;
	}

	createLogFileIfNotExists("./output/test.log");
	freopen("./output/test.log", "w", stdout);

	std::string inputFile = argv[1];
	std::string grammarFile = argv[2];


	std::ifstream file(inputFile);
	if (!file.is_open()) {
		std::cerr << "无法打开文件: " << inputFile << std::endl;
		return 1;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::string content = buffer.str();

	Lexer lexer(content);
	std::vector<Symbol> sentence;
	Token token;
	std::cout << "--------------------词法分析--------------------" << std::endl;
	do {
		token = lexer.getNextToken();
		std::cout << token.toString() << std::endl;
		sentence.push_back(Symbol(SymbolType::Terminal, token.toString()));
	} while (token.type != T_EOF);
	std::cout << "--------------------词法分析--------------------" << std::endl;

	LR1Parser parser(grammarFile);
	std::cout << "--------------------FIRST集--------------------" << std::endl;
	parser.print_firstSet();
	std::cout << "--------------------FIRST集--------------------" << std::endl;
	
	std::cout << "---------------ACTION表和GOTO表-----------------" << std::endl;
	parser.print_tables();
	std::cout << "---------------ACTION表和GOTO表-----------------" << std::endl;
	
	std::cout << "--------------------语法分析--------------------" << std::endl;
	parser.parse(sentence);
	std::cout << "--------------------语法分析--------------------" << std::endl;


	return 0;
}

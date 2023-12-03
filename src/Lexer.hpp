#pragma once

#include "Token.hpp"
#include <string>
#include <map>

class Lexer {
private:
	std::string input;
	size_t index = 0;

	char peek();
	char getChar();
	void skipWhitespace();
	Token getIdentifierOrKeyword();
	Token getCharLiteral();
	Token getStringLiteral();
	Token getNumber();
	Token getOperator();
	Token getDelimiter();
	void skipComment();

	std::map<std::string, TokenType> operatorMap = {
	    {"+", T_PLUS},
	    {"-", T_MINUS},
	    {"*", T_MULTIPLY},
	    {"/", T_DIVIDE},
	    {"=", T_ASSIGN},
	    {"==", T_EQUAL},
	    {"!=", T_NOTEQUAL},
	    {"<", T_LESS},
	    {"<=", T_LESSEQUAL},
	    {">", T_GREATER},
	    {">=", T_GREATEREQUAL},
	    {"&&", T_AND},
	    {"||", T_OR},
	    {"!", T_NOT},
	    {"%", T_MOD},
	    {"++", T_INCREMENT},
	    {"--", T_DECREMENT},
	    {"&", T_BITAND},
	    {"|", T_BITOR},
	    {"^", T_BITXOR},
	    {"~", T_BITNOT},
	    {"<<", T_LEFTSHIFT},
	    {">>", T_RIGHTSHIFT},
	    // ... 你可以根据需要添加其他操作符
	};

	std::map<std::string, TokenType> keywordMap = {
	    {"if", T_IF},
	    {"else", T_ELSE},
	    {"while", T_WHILE},
	    {"for", T_FOR},
	    {"return", T_RETURN},
	    {"int", T_INT},
	    {"float", T_FLOAT},
	    {"char", T_CHAR},
	    {"void", T_VOID},
	    {"struct", T_STRUCT},
	    // ... 你可以根据需要添加其他关键字，例如 switch, case 等
	};

	std::map<std::string, TokenType> delimiterMap = {
	    {";", T_SEMICOLON},
	    {"{", T_LEFT_BRACE},
	    {"}", T_RIGHT_BRACE},
	    {"(", T_LEFT_PAREN},
	    {")", T_RIGHT_PAREN},
	    {"[", T_LEFT_SQUARE},
	    {"]", T_RIGHT_SQUARE},
	    {",", T_COMMA},
	    {".", T_DOT},
	    {"->", T_ARROW},
	    {":", T_COLON},
	    {"?", T_QUESTION},
	    // ... 其他界定符
	};

public:
	Lexer(const std::string& input);
	Token getNextToken();
};

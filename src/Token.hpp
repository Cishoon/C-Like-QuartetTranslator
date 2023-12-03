#pragma once

#include <string>


enum TokenType {
	// 字面量 (Literals)
	T_IDENTIFIER,       // 例如: variableName, 任意其他变量名、函数名
	T_INTEGER_LITERAL,  // 例如: 42
	T_FLOAT_LITERAL,    // 例如: 3.14
	T_STRING_LITERAL,   // 例如: "hello"
	T_CHAR_LITERAL,     // 例如: 'a'

	// 关键字 (Keywords)
	T_IF,      // if
	T_ELSE,    // else
	T_WHILE,   // while
	T_FOR,     // for
	T_RETURN,  // return
	T_INT,     // int
	T_FLOAT,   // float
	T_CHAR,    // char
	T_VOID,    // void
	T_STRUCT,  // struct
	// ... 可以继续添加其他关键字，如: switch, case, etc.

	// 操作符 (Operators)
	T_PLUS,          // +
	T_MINUS,         // -
	T_MULTIPLY,      // *
	T_DIVIDE,        // /
	T_ASSIGN,        // =
	T_EQUAL,         // ==
	T_NOTEQUAL,      // !=
	T_LESS,          // <
	T_LESSEQUAL,     // <=
	T_GREATER,       // >
	T_GREATEREQUAL,  // >=
	T_AND,           // &&
	T_OR,            // ||
	T_NOT,           // !
	T_MOD,           // %
	T_INCREMENT,     // ++
	T_DECREMENT,     // --
	T_BITAND,        // &
	T_BITOR,         // |
	T_BITXOR,        // ^
	T_BITNOT,        // ~
	T_LEFTSHIFT,     // <<
	T_RIGHTSHIFT,    // >>
	// ... 其他操作符

	// 界符 (Delimiters/Punctuators)
	T_SEMICOLON,     // ;
	T_LEFT_BRACE,    // {
	T_RIGHT_BRACE,   // }
	T_LEFT_PAREN,    // (
	T_RIGHT_PAREN,   // )
	T_LEFT_SQUARE,   // [
	T_RIGHT_SQUARE,  // ]
	T_COMMA,         // ,
	T_DOT,           // .
	T_ARROW,         // ->
	T_COLON,         // :
	T_QUESTION,      // ?
	// ... 其他界符

	T_UNKNOWN,  // 未知Token
	T_EOF,      // 文件结束
};

struct Token
{
	Token(const TokenType& type, const std::string& value) : type(type), value(value) {}
	Token() {}

	TokenType type;
	std::string value;

	std::string toString()
	{
		// return "Type: \t" + tokenTypeToString(type) + ", \tValue: \t" + value;
		return tokenTypeToString(type);
	};

private:
	std::string tokenTypeToString(TokenType type)
	{
		switch (type) {
			case T_IDENTIFIER:
				return "T_IDENTIFIER";
			case T_INTEGER_LITERAL:
				return "T_INTEGER_LITERAL";
			case T_FLOAT_LITERAL:
				return "T_FLOAT_LITERAL";
			case T_STRING_LITERAL:
				return "T_STRING_LITERAL";
			case T_CHAR_LITERAL:
				return "T_CHAR_LITERAL";
			case T_IF:
				return "T_IF";
			case T_ELSE:
				return "T_ELSE";
			case T_WHILE:
				return "T_WHILE";
			case T_FOR:
				return "T_FOR";
			case T_RETURN:
				return "T_RETURN";
			case T_INT:
				return "T_INT";
			case T_FLOAT:
				return "T_FLOAT";
			case T_CHAR:
				return "T_CHAR";
			case T_VOID:
				return "T_VOID";
			case T_STRUCT:
				return "T_STRUCT";
			case T_PLUS:
				return "T_PLUS";
			case T_MINUS:
				return "T_MINUS";
			case T_MULTIPLY:
				return "T_MULTIPLY";
			case T_DIVIDE:
				return "T_DIVIDE";
			case T_ASSIGN:
				return "T_ASSIGN";
			case T_EQUAL:
				return "T_EQUAL";
			case T_NOTEQUAL:
				return "T_NOTEQUAL";
			case T_LESS:
				return "T_LESS";
			case T_LESSEQUAL:
				return "T_LESSEQUAL";
			case T_GREATER:
				return "T_GREATER";
			case T_GREATEREQUAL:
				return "T_GREATEREQUAL";
			case T_AND:
				return "T_AND";
			case T_OR:
				return "T_OR";
			case T_NOT:
				return "T_NOT";
			case T_MOD:
				return "T_MOD";
			case T_INCREMENT:
				return "T_INCREMENT";
			case T_DECREMENT:
				return "T_DECREMENT";
			case T_BITAND:
				return "T_BITAND";
			case T_BITOR:
				return "T_BITOR";
			case T_BITXOR:
				return "T_BITXOR";
			case T_BITNOT:
				return "T_BITNOT";
			case T_LEFTSHIFT:
				return "T_LEFTSHIFT";
			case T_RIGHTSHIFT:
				return "T_RIGHTSHIFT";
			case T_SEMICOLON:
				return "T_SEMICOLON";
			case T_LEFT_BRACE:
				return "T_LEFT_BRACE";
			case T_RIGHT_BRACE:
				return "T_RIGHT_BRACE";
			case T_LEFT_PAREN:
				return "T_LEFT_PAREN";
			case T_RIGHT_PAREN:
				return "T_RIGHT_PAREN";
			case T_LEFT_SQUARE:
				return "T_LEFT_SQUARE";
			case T_RIGHT_SQUARE:
				return "T_RIGHT_SQUARE";
			case T_COMMA:
				return "T_COMMA";
			case T_DOT:
				return "T_DOT";
			case T_ARROW:
				return "T_ARROW";
			case T_COLON:
				return "T_COLON";
			case T_QUESTION:
				return "T_QUESTION";
			case T_UNKNOWN:
				return "T_UNKNOWN";
			case T_EOF:
				return "T_EOF";
			default:
				return "INVALID_TOKEN_TYPE";
		}
	}
};

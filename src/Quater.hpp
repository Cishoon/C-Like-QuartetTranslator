#pragma once

#include <string>
#include <cstring>

// 四元式
class Quater {
public:
	std::string op;
	std::string arg1;
	std::string arg2;
	std::string result;

	Quater(const std::string& op, const std::string& arg1, const std::string& arg2, const std::string& result)
	    : op(op), arg1(arg1), arg2(arg2), result(result) {}

	std::string to_string() const
	{
		std::string formatted_arg1 = arg1.empty() ? "_" : arg1;
		std::string formatted_arg2 = arg2.empty() ? "_" : arg2;
		return "(" + op + ", " + formatted_arg1 + ", " + formatted_arg2 + ", " + result + ")";
	}

	bool is_jump() const { return op[0] == 'j'; }
};

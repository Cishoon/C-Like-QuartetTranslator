#pragma once
#include "LR1Parser.hpp"

struct VaribleMeta
{
	std::string type;   // 变量类型，int，float，char等
	std::string value;  // 变量的值，为了简化操作，全部存为string，在需要使用时进行转换
};


class SemanticAnalyzer {
public:
	SemanticAnalyzer(SemanticTreeNode*& root) : root(root), next_temp_varible_id(0) {}

	void semantic_analyze();
	void print_intermediate_code();

private:
	void handle_defalt(SemanticTreeNode*& node);
	void handle_var_declaration(SemanticTreeNode*& node);
	void handle_opt_init(SemanticTreeNode*& node);
	void handle_expression(SemanticTreeNode*& node);
	void handle_simple_expression(SemanticTreeNode*& node);
	void handle_additive_expression(SemanticTreeNode*& node);
	void handle_term(SemanticTreeNode*& node);
	void handle_postfix_expression(SemanticTreeNode*& node);
	void handle_var(SemanticTreeNode*& node);
	void handle_factor(SemanticTreeNode*& node);
	void handle_prefix_expression(SemanticTreeNode*& node);

private:
	std::string new_temp_varible()
	{
		return "t" + std::to_string(next_temp_varible_id++);
	}

private:
	bool exists_var_declaration(std::string var)
	{
		return varible_table.find(var) != varible_table.end();
	}

private:
	SemanticTreeNode* root;

	std::map<std::string, VaribleMeta> varible_table;  // 变量表，存储已经声明过的变量

	std::vector<std::pair<size_t, Quater>> intermediate_code() { return root->quater_list; }  // 中间代码，一个数字编号，一个四元式

	size_t next_temp_varible_id;
};
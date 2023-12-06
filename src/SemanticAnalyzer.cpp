#include "SemanticAnalyzer.hpp"

void SemanticAnalyzer::print_intermediate_code()
{
	for (const auto& [id, item] : intermediate_code()) {
		std::cout << id << "\t" << item.to_string() << "\n";
	}
}

void SemanticAnalyzer::semantic_analyze()
{
	if (root == nullptr) return;

	std::stack<SemanticTreeNode*> stack1, stack2;
	stack1.push(root);

	while (!stack1.empty()) {
		SemanticTreeNode* node = stack1.top();
		stack1.pop();
		stack2.push(node);

		for (SemanticTreeNode* child : node->children) {
			stack1.push(child);
		}
	}

	while (!stack2.empty()) {
		SemanticTreeNode* node = stack2.top();
		stack2.pop();

		// Process node - this can be customized as needed
		// For example, print node information
		if (node->leaf()) {
			continue;
		}

		handle_defalt(node);

		if (node->literal == "var_declaration") {
			handle_var_declaration(node);
		} else if (node->literal == "opt_init") {
			handle_opt_init(node);
		} else if (node->literal == "expression") {
			handle_expression(node);
		} else if (node->literal == "simple_expression") {
			handle_simple_expression(node);
		} else if (node->literal == "additive_expression") {
			handle_additive_expression(node);
		} else if (node->literal == "term") {
			handle_term(node);
		} else if (node->literal == "postfix_expression") {
			handle_postfix_expression(node);
		} else if (node->literal == "factor") {
			handle_factor(node);
		} else if (node->literal == "prefix_expression") {
			handle_prefix_expression(node);
		} else if (node->literal == "selection_stmt") {
			handle_selection_stmt(node);
		} else if (node->literal == "iteration_stmt") {
			handle_iteration_stmt(node);
		} else if (node->literal == "opt_expression_stmt") {
			handle_opt_expression_stmt(node);
		}

		if (node->children.size() == 1 && node->children[0]->literal == "T_IDENTIFIER") {
			node->literal = node->children[0]->literal;
		}
	}
}

void SemanticAnalyzer::handle_defalt(SemanticTreeNode*& node)
{
	for (const auto& child : node->children) {
		node->real_value += child->real_value;
		if (node->literal != "selection_stmt" && node->literal != "iteration_stmt") {
			node->append_quaters(child->quater_list);
		}
	}
}

void SemanticAnalyzer::handle_var_declaration(SemanticTreeNode*& node)
{
	// type_specifier T_IDENTIFIER opt_init T_SEMICOLON
	// type_specifier T_IDENTIFIER T_SEMICOLON
	const auto& list = node->children;

	const std::string& type = list[0]->real_value;
	const std::string& varible_name = list[1]->real_value;
	const std::string& init_val = list[2]->literal == "opt_init" ? list[2]->real_value : "NULL";

	if (varible_table.find(varible_name) != varible_table.end()) {
		// 如果变量表中已经有了这个变量，报错
		std::cout << "Error：重定义变量：" << varible_name << std::endl;
		exit(-1);
	}

	varible_table[varible_name] = {type, init_val};
	node->real_value = type + " " + varible_name + " " + init_val + ";";
}

void SemanticAnalyzer::handle_opt_init(SemanticTreeNode*& node)
{
	// T_ASSIGN expression
	const auto& list = node->children;

	node->real_value = list[1]->real_value;
}

void SemanticAnalyzer::handle_expression(SemanticTreeNode*& node)
{
	/*
	simple_expression
	postfix_expression

	var T_ASSIGN expression
	a = exp
	(=, t, _, a)
	*/
	const auto& list = node->children;

	if (list.size() == 1) {
		return;
	}

	const std::string& var = list[0]->real_value;
	const std::string& op = list[1]->real_value;
	const std::string& exp = list[2]->real_value;
	if (list[0]->literal == "T_IDENTIFIER" && !exists_var_declaration(var)) {
		std::cout << "Error: 未定义变量：" << var << std::endl;
		exit(-1);
	}
	if (list[2]->literal == "T_IDENTIFIER" && !exists_var_declaration(exp)) {
		std::cout << "Error: 未定义变量：" << exp << std::endl;
		exit(-1);
	}
	node->add_quater(op, exp, "", var);
	node->real_value = var;
}

void SemanticAnalyzer::handle_simple_expression(SemanticTreeNode*& node)
{
	/*
	additive_expression relop additive_expression
	simple_expression relop simple_expression
	additive_expression
	*/
	const auto& list = node->children;

	if (list.size() == 1) {
		return;
	}

	const std::string& arg1 = list[0]->real_value;
	const std::string& op = list[1]->real_value;
	const std::string& arg2 = list[2]->real_value;

	std::string new_temp = new_temp_varible();
	node->add_quater(op, arg1, arg2, new_temp);
	node->real_value = new_temp;
}

void SemanticAnalyzer::handle_additive_expression(SemanticTreeNode*& node)
{
	/*
	term
	additive_expression addop term
	*/

	const auto& list = node->children;

	if (list.size() == 1) {
		return;
	}

	const std::string& arg1 = list[0]->real_value;
	const std::string& op = list[1]->real_value;
	const std::string& arg2 = list[2]->real_value;

	std::string new_temp = new_temp_varible();
	node->add_quater(op, arg1, arg2, new_temp);
	node->real_value = new_temp;
}

void SemanticAnalyzer::handle_term(SemanticTreeNode*& node)
{
	/*
	factor
	postfix_expression
	term mulop factor
	*/
	const auto& list = node->children;

	if (list.size() == 1) {
		return;
	}

	const std::string& arg1 = list[0]->real_value;
	const std::string& op = list[1]->real_value;
	const std::string& arg2 = list[2]->real_value;

	std::string new_temp = new_temp_varible();
	node->add_quater(op, arg1, arg2, new_temp);
	node->real_value = new_temp;
}

void SemanticAnalyzer::handle_factor(SemanticTreeNode*& node)
{
	/*
	T_LEFT_PAREN expression T_RIGHT_PAREN
	var
	T_INTEGER_LITERAL
	T_STRING_LITERAL
	T_CHAR_LITERAL
	T_FLOAT_LITERAL
	prefix_expression

	call 		*本次作业不处理函数调用
	*/
	const auto& list = node->children;

	if (list.size() == 1) {
		return;
	}

	node->real_value = list[1]->real_value;
}

void SemanticAnalyzer::handle_prefix_expression(SemanticTreeNode*& node)
{
	/*
	inc_dec_operator var   ++a
	(++, a, _, a)

	T_NOT factor
	T_BITNOT factor
	(~/!, a, _, t1)
	*/
	const auto& list = node->children;

	const std::string& op = list[0]->real_value;
	const std::string& varible_name = list[1]->real_value;

	if (list[0]->literal == "inc_dec_operator") {
		std::string op_ = op == "++" ? "+" : "-";
		node->add_quater(op_, varible_name, "1", varible_name);
		node->real_value = varible_name;
	} else {
		std::string new_temp = new_temp_varible();
		node->add_quater(op, varible_name, "", new_temp);
		node->real_value = new_temp;
	}
}

void SemanticAnalyzer::handle_postfix_expression(SemanticTreeNode*& node)
{
	/*
	var inc_dec_operator   			a++, b--

	(=, a, _, t1)
	(++, a, _, a)
	*/
	const auto& list = node->children;

	const std::string& varible_name = list[0]->real_value;
	const std::string& op = list[1]->real_value;

	std::string new_temp = new_temp_varible();
	node->add_quater("=", varible_name, "", new_temp);
	std::string op_ = op == "++" ? "+" : "-";
	node->add_quater(op_, varible_name, "1", varible_name);

	node->real_value = new_temp;
}

void SemanticAnalyzer::handle_var(SemanticTreeNode*& node)
{
	/*
	T_IDENTIFIER
	T_IDENTIFIER T_LEFT_SQUARE expression T_RIGHT_SQUARE a[]
	*/

	// 暂时不考虑数组
	return;
}

void SemanticAnalyzer::handle_selection_stmt(SemanticTreeNode*& node)
{
	/*
	T_IF T_LEFT_PAREN expression T_RIGHT_PAREN statement
	T_IF T_LEFT_PAREN expression T_RIGHT_PAREN statement T_ELSE statement
	*/

	const auto& list = node->children;

	const auto& stmt_THEN = list[4];
	const auto& cond = list[2]->real_value;
	node->append_quaters(list[2]->quater_list);

	size_t THEN = -1;
	size_t ELSE = -1;
	size_t ENDIF = -1;


	if (list.size() == 5) {
		THEN = 2 + list[2]->quater_list.size();
		ENDIF = THEN + stmt_THEN->quater_list.size();

		node->add_quater("jnz", cond, "", THEN);
		node->add_quater("j", "", "", ENDIF);
		node->append_quaters(stmt_THEN->quater_list);
	} else if (list.size() == 7) {
		const auto& stmt_ELSE = list[6];
		ELSE = 1 + list[2]->quater_list.size();
		THEN = ELSE + stmt_ELSE->children.size() + 1;
		ENDIF = THEN + stmt_THEN->children.size();

		node->add_quater("jnz", cond, "", THEN);
		node->append_quaters(stmt_ELSE->quater_list);
		node->add_quater("j", "", "", ENDIF);
		node->append_quaters(stmt_THEN->quater_list);
	}
}

void SemanticAnalyzer::handle_iteration_stmt(SemanticTreeNode*& node)
{
	const auto& list = node->children;


	if (list[0]->literal == "T_WHILE") {
		/*
		T_WHILE T_LEFT_PAREN expression T_RIGHT_PAREN statement
		*/
		const auto& cond = list[2]->real_value;
		const auto& stmt = list[4];
		size_t LOOP = 0;
		size_t BODY = LOOP + list[2]->quater_list.size() + 2;
		size_t END_LOOP = BODY + stmt->quater_list.size() + 1;

		node->append_quaters(list[2]->quater_list);
		node->add_quater("jnz", cond, "", BODY);
		node->add_quater("j", "", "", END_LOOP);
		node->append_quaters(stmt->quater_list);
		node->add_quater("j", "", "", LOOP);
	} else if (list[0]->literal == "T_FOR") {
		/*
		T_FOR T_LEFT_PAREN opt_expression_stmt opt_expression_stmt expression T_RIGHT_PAREN statement
		*/
		const auto& exp1 = list[2];
		const auto& exp2 = list[3];
		const auto& exp3 = list[4];
		const auto& stmt = list[6];

		size_t START = exp1->quater_list.size();
		size_t BODY = START + exp2->quater_list.size() + 2;
		size_t END_LOOP = BODY + stmt->quater_list.size() + exp3->quater_list.size() + 1;

		node->append_quaters(exp1->quater_list);
		node->append_quaters(exp2->quater_list);
		node->add_quater("jnz", exp2->real_value, "", BODY);
		node->add_quater("j", "", "", END_LOOP);
		node->append_quaters(stmt->quater_list);
		node->append_quaters(exp3->quater_list);
		node->add_quater("j", "", "", START);
	}
}

void SemanticAnalyzer::handle_opt_expression_stmt(SemanticTreeNode*& node)
{
	/*
	expression_stmt 
	T_SEMICOLON
	*/
	node->real_value = node->real_value.substr(0, node->real_value.size() - 1);  // 去掉分号;

	return;
}
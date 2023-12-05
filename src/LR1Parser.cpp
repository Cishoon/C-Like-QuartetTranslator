#include <iostream>
#include "LR1Parser.hpp"

LR1Parser::LR1Parser(const std::vector<Production>& productions, Symbol start, Symbol end)
    : productions(productions), start_symbol(start), end_symbol(end)
{
	for (const auto& production : productions) {
		productionMap[production.lhs].push_back(production);
	}
	calculate_firstSets();
	construct_tables();
}

LR1Parser::LR1Parser(const std::string file_path)
{
	std::ifstream file(file_path);
	if (!file.is_open()) {
		std::cerr << "无法打开文件: " + file_path << '\n';
	}

	std::string first_line;
	if (!std::getline(file, first_line)) {
		std::cerr << "文件格式错误: 第首行必须定义起始符合终止符\n";
	}
	std::istringstream is(first_line);
	std::string temp;
	is >> temp;
	start_symbol = Symbol(SymbolType::NonTerminal, temp);
	is >> temp;
	end_symbol = Symbol(SymbolType::Terminal, temp);

	std::string terminalsLine;
	if (!std::getline(file, terminalsLine)) {
		std::cerr << "文件格式错误: 第二行必须定义终结符\n";
	}
	std::istringstream iss(terminalsLine);
	std::string terminal;
	while (iss >> terminal) {
		terminals.insert(terminal);
	}

	std::string line;
	while (std::getline(file, line)) {
		parse_EBNF_line(line);
	}

	calculate_firstSets();
	construct_tables();
}

void LR1Parser::parse_EBNF_line(const std::string& line)
{
	std::istringstream iss(line);
	std::string lhs, rhsPart, token;

	// 读取非终结符，自动忽略前导空格
	iss >> lhs;
	// 忽略 "::="
	std::string ignoreStr;
	std::getline(iss, ignoreStr, '=');

	// 读取并解析产生式右侧
	std::getline(iss, rhsPart);
	std::istringstream rhsStream(rhsPart);

	// 用 '|' 分割产生式右边的语句
	while (std::getline(rhsStream, token, '|')) {
		std::istringstream tokenStream(token);
		std::vector<Symbol> rhsSymbols;

		// 获取产生式右边的符号
		std::string sym;
		while (tokenStream >> sym) {
			SymbolType type;
			if (sym == "Epsilon") {
				type = SymbolType::Epsilon;
			} else {
				type = terminals.find(sym) == terminals.end() ? SymbolType::NonTerminal : SymbolType::Terminal;
			}
			rhsSymbols.push_back(Symbol(type, sym));
		}

		// 创建产生式并添加到某个容器中
		productionMap[Symbol(SymbolType::NonTerminal, lhs)].push_back(Production{Symbol(SymbolType::NonTerminal, lhs), rhsSymbols});
		productions.push_back(Production(Symbol(SymbolType::NonTerminal, lhs), rhsSymbols));
	}
}


void LR1Parser::calculate_firstSets()
{
	for (const auto& [lhs, rhs] : productions) {
		firstSet[lhs] = {};
		for (const auto& symbol : rhs) {
			if (symbol.type == SymbolType::NonTerminal) {
				firstSet[symbol] = {};
			} else if (symbol.type == SymbolType::Terminal) {
				firstSet[symbol] = {symbol};
			} else if (symbol.type == SymbolType::Epsilon) {
				firstSet[symbol] = {Symbol(SymbolType::Epsilon, "")};
			}
		}
	}

	bool changed;
	do {
		changed = false;
		for (const auto& [nonTerminal, productions] : productionMap) {
			for (const auto& prod : productions) {
				size_t i;
				for (i = 0; i < prod.rhs.size(); ++i) {
					const Symbol& symbol = prod.rhs[i];
					std::unordered_set<Symbol, SymbolHash, SymbolEqual> symbolFirstSet = firstSet[symbol];
					size_t oldSize = firstSet[nonTerminal].size();
					bool has_epsilon = true;

					// If the symbol does not contain epsilon, break
					if (symbolFirstSet.find(Symbol(SymbolType::Epsilon, "")) == symbolFirstSet.end()) {
						has_epsilon = false;
					}

					// Remove epsilon if not the last symbol
					if (i < prod.rhs.size() - 1) {
						symbolFirstSet.erase(Symbol(SymbolType::Epsilon, ""));
					}
					firstSet[nonTerminal].insert(symbolFirstSet.begin(), symbolFirstSet.end());

					// Check if FIRST set has changed
					if (firstSet[nonTerminal].size() != oldSize) {
						changed = true;
					}

					if (!has_epsilon) break;
				}

				// If all symbols in production can lead to epsilon, add epsilon to FIRST(nonTerminal)
				if (i == prod.rhs.size()) {
					firstSet[nonTerminal].insert(Symbol(SymbolType::Epsilon, ""));
				}
			}
		}
	} while (changed);  // Repeat until no changes are made
}

std::unordered_set<Symbol, SymbolHash, SymbolEqual> LR1Parser::get_first_string(std::vector<Symbol> content) const
{
	std::unordered_set<Symbol, SymbolHash, SymbolEqual> ret;
	for (const auto& symbol : content) {
		if (symbol.type == SymbolType::Epsilon) continue;
		if (symbol.type == SymbolType::Terminal) {
			ret.insert(symbol);
			break;
		}
		std::unordered_set<Symbol, SymbolHash, SymbolEqual> symbolFirstSet = firstSet.at(symbol);
		symbolFirstSet.erase(Symbol(SymbolType::Epsilon, ""));
		ret.insert(symbolFirstSet.begin(), symbolFirstSet.end());
		if (ret.size() > 0) break;
	}
	return ret;
}


LR1Item::State LR1Parser::get_lr1item_state(const LR1Item& item) const
{
	const auto& production = item.production;
	const auto& dot_position = item.dot_position;
	const auto& lookahead = item.lookahead;

	// 检查是否是接受状态
	if (production.lhs == start_symbol && dot_position == production.rhs.size() && lookahead == end_symbol) {
		return LR1Item::State::ACCEPT;
	}

	// 检查是否是归约状态
	if (dot_position == production.rhs.size()) {
		return LR1Item::State::REDUCE;
	}

	// 检查是否是移进状态
	if (dot_position < production.rhs.size() && production.rhs[dot_position].type == SymbolType::Terminal) {
		return LR1Item::State::SHIFT;
	}

	// 检查是否是转换状态
	if (dot_position < production.rhs.size() && production.rhs[dot_position].type == SymbolType::NonTerminal) {
		return LR1Item::State::GOTO;
	}

	// 如果没有匹配的状态，可能是一个错误或未定义的状态
	throw std::runtime_error("Undefined state for LR1 item.");
}

void LR1Parser::closure(std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual>& old_set) const
{
	bool changed = true;
	std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual> vis_item;
	while (changed) {
		changed = false;
		std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual> items_to_add;

		for (const auto& item : old_set) {
			// 只有GOTO状态(.后面是非终结符)才需要处理
			if (get_lr1item_state(item) != LR1Item::State::GOTO) continue;
			if (vis_item.find(item) != vis_item.end()) continue;
			vis_item.insert(item);

			Symbol next_symbol = item.next_symbol();
			Symbol nnext_symbol = item.nnext_symbol();
			// 找到文法中所有以next_symbol为lhs的产生式
			std::vector<Production> next_prods = get_productions_start_by_symbol(next_symbol);
			for (const auto& prod : next_prods) {
				std::unordered_set<Symbol, SymbolHash, SymbolEqual> lookaheads = get_first_string({nnext_symbol, item.lookahead});
				for (const auto& p_lh : lookaheads) {
					LR1Item new_item(prod, 0, p_lh);
					if (old_set.find(new_item) == old_set.end()) {
						items_to_add.insert(new_item);
						changed = true;
					}
				}
			}
		}

		old_set.insert(items_to_add.begin(), items_to_add.end());
	}
}


std::vector<Production> LR1Parser::get_productions_start_by_symbol(const Symbol& symbol) const
{
	auto it = productionMap.find(symbol);
	if (it == productionMap.end()) return std::vector<Production>();
	return it->second;
}

void LR1Parser::print_firstSet() const
{
	for (const auto& [symbol, firstSetSymbols] : firstSet) {
		std::cout << "FIRST(" << symbol.literal << ") = { ";
		for (const auto& sym : firstSetSymbols) {
			std::cout << sym.literal << " ";
		}
		std::cout << "}\n";
	}
}

void LR1Parser::print_tables() const
{
	// 打印ACTION表
	std::cout << "ACTION Table:\n";
	for (const auto& entry : actionTable) {
		const auto& state_symbol_pair = entry.first;
		const auto& action = entry.second;

		std::cout << "State " << state_symbol_pair.first << ", Symbol " << state_symbol_pair.second.to_string()
		          << ": " << action.to_string() << "\n";
	}

	// 打印GOTO表
	std::cout << "\nGOTO Table:\n";
	for (const auto& entry : gotoTable) {
		const auto& state_symbol_pair = entry.first;
		const auto& nextState = entry.second;

		std::cout << "State " << state_symbol_pair.first << ", Symbol " << state_symbol_pair.second.to_string()
		          << ": " << nextState << "\n";
	}
}

void LR1Parser::construct_tables()
{
	Production begin_production = get_productions_start_by_symbol(start_symbol).at(0);
	lr1ItemSets.push_back(std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual>({LR1Item(begin_production, 0, end_symbol)}));
	closure(lr1ItemSets[0]);

	for (size_t index = 0; index < lr1ItemSets.size(); ++index) {
		std::vector<LR1Item> shift_items, reduce_items, accept_items, goto_items;
		std::vector<Symbol> VT_shift, VN_goto;
		// std::cout << index << '\n';
		for (auto& item : lr1ItemSets[index]) {
			// std::cout << item.to_string() << ' ' << (int)get_lr1item_state(item) << '\n';
			switch (get_lr1item_state(item)) {
				case LR1Item::State::ACCEPT:
					accept_items.push_back(item);
					break;
				case LR1Item::State::GOTO:
					goto_items.push_back(item);
					VN_goto.push_back(item.next_symbol());
					break;
				case LR1Item::State::REDUCE:
					reduce_items.push_back(item);
					break;
				case LR1Item::State::SHIFT:
					shift_items.push_back(item);
					VT_shift.push_back(item.next_symbol());
					break;
				default:
					std::cerr << "construct_tables() 中LR1Item类型错误" << std::endl;
			}
		}

		for (auto& vt : VT_shift) {
			std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual> new_set;
			for (auto& item : lr1ItemSets[index]) {
				if (get_lr1item_state(item) == LR1Item::State::SHIFT && item.next_symbol() == vt) {
					new_set.insert(LR1Item(item.production, item.dot_position + 1, item.lookahead));
				}
			}
			closure(new_set);
			size_t id = find(lr1ItemSets.begin(), lr1ItemSets.end(), new_set) - lr1ItemSets.begin();
			if (id == lr1ItemSets.size()) {
				lr1ItemSets.push_back(new_set);
			}
			actionTable[{index, vt}] = Action(Action::Type::SHIFT, id, Production());
		}

		for (auto& vn : VN_goto) {
			std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual> new_set;
			for (auto& item : lr1ItemSets[index]) {
				if (get_lr1item_state(item) == LR1Item::State::GOTO && item.next_symbol() == vn) {
					new_set.insert(LR1Item(item.production, item.dot_position + 1, item.lookahead));
				}
			}
			closure(new_set);
			size_t id = find(lr1ItemSets.begin(), lr1ItemSets.end(), new_set) - lr1ItemSets.begin();
			if (id == lr1ItemSets.size()) {
				lr1ItemSets.push_back(new_set);
			}
			gotoTable[{index, vn}] = id;
		}

		for (auto& item : accept_items) {
			actionTable[{index, item.lookahead}] = Action(Action::Type::ACCEPT, -1, Production());
		}

		for (auto& item : reduce_items) {
			actionTable[{index, item.lookahead}] = Action(Action::Type::REDUCE, -1, item.production);
		}
	}
}

bool LR1Parser::parse(const std::vector<Symbol>& sentence, ParserTreeNode*& root) const
{
	std::stack<int> stateStack;                                          // 状态栈
	std::stack<Symbol> symbolStack;                                      // 符号栈
	std::vector<Symbol> inputStack(sentence.rbegin(), sentence.rend());  // 输入栈

	std::vector<ParserTreeNode*> nodeStack;  // 解析树结点栈

	// 初始状态
	stateStack.push(0);


	while (!inputStack.empty()) {
		int currentState = stateStack.top();
		Symbol currentSymbol = inputStack.back();

		// 打印当前栈的状态
		print_stacks(stateStack, symbolStack, inputStack);

		auto actionIt = actionTable.find({currentState, currentSymbol});
		if (actionIt != actionTable.end()) {
			const Action& action = actionIt->second;

			switch (action.type) {
				case Action::Type::SHIFT: {
					stateStack.push(action.number);
					symbolStack.push(currentSymbol);
					inputStack.pop_back();

					// 创建一个新的叶子节点并压入节点栈
					ParserTreeNode* newNode = new ParserTreeNode(currentSymbol);
					nodeStack.push_back(newNode);
					break;
				}
				case Action::Type::REDUCE: {
					// 创建一个新的非叶子节点
					ParserTreeNode* newNode = new ParserTreeNode(action.production.lhs);

					// 根据产生式右侧的长度，从栈中弹出相应数量的符号和状态
					for (size_t i = 0; i < action.production.rhs.size(); ++i) {
						symbolStack.pop();
						stateStack.pop();

						newNode->children.insert(newNode->children.begin(), nodeStack.back());
						nodeStack.pop_back();
					}
					// 将新节点压入节点栈
					nodeStack.push_back(newNode);

					// 将产生式左侧的非终结符压入符号栈
					symbolStack.push(action.production.lhs);

					// 更新状态栈
					int nextState = gotoTable.at({stateStack.top(), action.production.lhs});
					stateStack.push(nextState);
					break;
				}
				case Action::Type::ACCEPT:
					std::cout << "Accept\n";
					root = nodeStack.back();  // 设置解析树的根
					return true;
				default:
					std::cerr << "Parse error\n";
					return false;
			}
		} else {
			std::cerr << "Parse error: no action\n";
			return false;
		}
	}

	std::cerr << "Parse error: input not consumed\n";
	return false;
}

void LR1Parser::print_stacks(const std::stack<int>& stateStack,
                             const std::stack<Symbol>& symbolStack,
                             const std::vector<Symbol>& inputStack) const
{
	// 使用向量来反转栈的内容
	std::vector<int> reversedStateStack;
	std::vector<Symbol> reversedSymbolStack;

	// 从栈中复制到向量
	auto tempStateStack = stateStack;
	while (!tempStateStack.empty()) {
		reversedStateStack.push_back(tempStateStack.top());
		tempStateStack.pop();
	}

	auto tempSymbolStack = symbolStack;
	while (!tempSymbolStack.empty()) {
		reversedSymbolStack.push_back(tempSymbolStack.top());
		tempSymbolStack.pop();
	}

	// 逆序打印向量，从而按栈底到栈顶的顺序输出
	std::cout << "State Stack: ";
	for (auto it = reversedStateStack.rbegin(); it != reversedStateStack.rend(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << "\n";

	std::cout << "Symbol Stack: ";
	for (auto it = reversedSymbolStack.rbegin(); it != reversedSymbolStack.rend(); ++it) {
		std::cout << it->to_string() << " ";
	}
	std::cout << "\n";

	std::cout << "Input Stack: ";
	for (auto it = inputStack.rbegin(); it != inputStack.rend(); ++it) {
		std::cout << it->to_string() << " ";
	}
	std::cout << "\n\n";
}

void LR1Parser::save_tables(const std::string& file_path)
{
	std::ofstream fout(file_path);
	if (!fout.is_open()) {
		std::cerr << "文件打开失败！" << std::endl;
		exit(-1);
	}

	// 序列化 actionTable
	for (auto& item : actionTable) {
		int t1 = item.first.first;
		Symbol t2 = item.first.second;
		Action t3 = item.second;
		fout << t1 << " " << t2 << " " << t3 << "\n";
	}

	// 可能需要一个分隔符来区分两个表
	fout << "---\n";

	// 序列化 gotoTable
	for (auto& item : gotoTable) {
		int t1 = item.first.first;
		Symbol t2 = item.first.second;
		int t3 = item.second;
		fout << t1 << " " << t2 << " " << t3 << "\n";
	}

	fout.close();
}

void LR1Parser::load_tables(const std::string& file_path)
{
	std::ifstream fin(file_path);
	if (!fin.is_open()) {
		std::cerr << "文件打开失败！" << std::endl;
		exit(-1);
	}

	std::string line;
	bool readingActionTable = true;  // 初始假设从actionTable开始读取

	while (std::getline(fin, line)) {
		if (line == "---") {
			// 遇到分隔符，切换到读取 gotoTable
			readingActionTable = false;
			continue;
		}
		std::istringstream iss(line);
		int state;
		Symbol symbol;
		iss >> state >> symbol;

		if (readingActionTable) {
			Action action;
			iss >> action;
			actionTable[{state, symbol}] = action;
		} else {
			int gotoState;
			iss >> gotoState;
			gotoTable[{state, symbol}] = gotoState;
		}
	}

	fin.close();
}


// int main()
// {
// 	// 定义非终结符
// 	Symbol S(SymbolType::NonTerminal, "S");
// 	Symbol B(SymbolType::NonTerminal, "B");
// 	Symbol S_prime(SymbolType::NonTerminal, "S'");

// 	// 定义终结符
// 	Symbol a(SymbolType::Terminal, "a");
// 	Symbol b(SymbolType::Terminal, "b");

// 	// 定义产生式
// 	Production p1(S_prime, {S});
// 	Production p2(S, {B, B});
// 	Production p3(B, {a, B});
// 	Production p4(B, {b});

// 	// std::vector<Production> productions = {p1, p2, p3, p4};

// 	// // 创建LR1解析器实例
// 	// LR1Parser parser(productions, S_prime, Symbol(SymbolType::Terminal, "$"));
// 	LR1Parser parser("grammer.txt");

// 	parser.print_firstSet();
// 	parser.print_tables();

// 	// std::vector<Symbol> sentence = {a, b, a, b, Symbol(SymbolType::Terminal, "$")};
// 	// parser.parse(sentence);

// 	return 0;
// }
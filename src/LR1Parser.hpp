#pragma once

#include <string>
#include <set>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "Quater.hpp"


enum class SymbolType {
	Terminal,     // 终结符
	NonTerminal,  // 非终结符
	Epsilon       // 空串
};

class Symbol {
public:
	SymbolType type;         // 终结符/非终结符/空串
	std::string literal;     // 这个符号的字面量，比如一个非终结符名为 S,A,B e.g.一个终结符名为T_INT,T_xxxx
	std::string real_value;  // 这个符号的真实值，比如一个变量名a，一个int数值3

	Symbol(const SymbolType& type = SymbolType::Epsilon,
	       const std::string& literal = "",
	       const std::string& real_value = "")
	    : type(type), literal(literal), real_value(real_value) {}


	std::string to_string() const
	{
		return literal;  // 或者任何合适的表示方式
	}

	friend bool operator==(const Symbol& lhs, const Symbol& rhs)
	{
		return lhs.type == rhs.type && lhs.literal == rhs.literal;
		// && lhs.real_value == rhs.real_value;
	}

	friend bool operator<(const Symbol& lhs, const Symbol& rhs)
	{
		if (lhs.type != rhs.type) {
			return lhs.type < rhs.type;
		}
		// if (lhs.literal != rhs.literal) {
		return lhs.literal < rhs.literal;
		// }
		// return lhs.real_value < rhs.real_value;
	}

	friend std::ostream& operator<<(std::ostream& os, Symbol& symbol)
	{
		if (symbol.type == SymbolType::Epsilon) {
			return os << static_cast<int>(symbol.type);
		}
		return os << static_cast<int>(symbol.type) << " " << symbol.literal << " "
		          << (symbol.real_value.empty() ? "NULL" : symbol.real_value);
	}
	friend std::istream& operator>>(std::istream& is, Symbol& symbol)
	{
		int temp;
		is >> temp;
		symbol.type = static_cast<SymbolType>(temp);
		if (symbol.type == SymbolType::Epsilon) {
			return is;
		}

		is >> symbol.literal >> symbol.real_value;
		if (symbol.real_value == "NULL") symbol.real_value = "";
		return is;
	}
};

struct Production
{
	Symbol lhs;               // 左手边，非终结符
	std::vector<Symbol> rhs;  // 右手边，非终结符和终结符的 组合

	Production() {}
	Production(Symbol lhs, std::vector<Symbol> rhs) : lhs(lhs), rhs(rhs) {}
	std::string to_string() const
	{
		std::string res;
		res += lhs.literal + " -> ";
		for (auto& item : rhs) {
			res += item.literal;
		}
		return "[" + res + "]";
	}

	friend bool operator==(const Production& lhs, const Production& rhs)
	{
		return lhs.lhs == rhs.lhs && lhs.rhs == rhs.rhs;
	}
	friend bool operator<(const Production& lhs, const Production& rhs)
	{
		if (lhs.lhs < rhs.lhs) return true;
		if (rhs.lhs < lhs.lhs) return false;
		return lhs.rhs < rhs.rhs;
	}

	friend std::ostream& operator<<(std::ostream& os, Production& production)
	{
		os << production.lhs << " " << production.rhs.size();
		for (auto& symbol : production.rhs) {
			os << " " << symbol;
		}
		return os;
	}

	friend std::istream& operator>>(std::istream& is, Production& production)
	{
		size_t size;
		is >> production.lhs >> size;
		production.rhs.clear();
		Symbol symbol;
		for (size_t i = 0; i < size; ++i) {
			is >> symbol;
			production.rhs.push_back(symbol);
		}
		return is;
	}
};

class LR1Item {
public:
	enum class State {
		SHIFT,   // 移进
		REDUCE,  // 归约
		GOTO,    // 待约
		ACCEPT   // 接受
	};

	LR1Item(Production production, size_t dot_position, Symbol lookahead)
	    : production(production), dot_position(dot_position), lookahead(lookahead) {}

	std::string to_string() const
	{
		std::string res;
		res += production.lhs.literal + " -> ";
		size_t index = 0;
		for (auto& item : production.rhs) {
			if (index++ == dot_position) res += ".";
			res += item.literal;
		}
		if (index++ == dot_position) res += ".";
		res += ", " + lookahead.literal;
		return "[" + res + "]";
	}

	Symbol next_symbol() const
	{
		if (dot_position == production.rhs.size()) return Symbol(SymbolType::Epsilon, "");
		return production.rhs[dot_position];
	}

	Symbol nnext_symbol() const
	{
		if (dot_position == production.rhs.size()) return Symbol(SymbolType::Epsilon, "");

		LR1Item tmp = *this;
		tmp.dot_position++;
		return tmp.next_symbol();
	}

	Production production;
	size_t dot_position;
	Symbol lookahead;

	friend bool operator==(const LR1Item& lhs, const LR1Item& rhs)
	{
		return lhs.production == rhs.production &&
		       lhs.dot_position == rhs.dot_position &&
		       lhs.lookahead == rhs.lookahead;
	}

	friend bool operator<(const LR1Item& lhs, const LR1Item& rhs)
	{
		if (lhs.production < rhs.production) return true;
		if (rhs.production < lhs.production) return false;
		if (lhs.dot_position < rhs.dot_position) return true;
		if (rhs.dot_position < lhs.dot_position) return false;
		return lhs.lookahead < rhs.lookahead;
	}
};

struct Action
{
	enum class Type {
		SHIFT,
		REDUCE,
		ACCEPT,
		ERROR
	};
	Type type;
	size_t number;          // shift的新状态编号
	Production production;  // reduce使用的产生式

	Action() {}
	Action(const Type& type, const size_t& number, const Production& production) : type(type), number(number), production(production) {}

	std::string to_string() const
	{
		switch (type) {
			case Type::SHIFT:
				return "S " + std::to_string(number);
			case Type::REDUCE:
				return "R " + production.to_string();
			case Type::ACCEPT:
				return "Accept";
			case Type::ERROR:
				return "x";
		}
		return "";
	}


	friend std::ostream& operator<<(std::ostream& os, Action::Type& type)
	{
		return os << static_cast<int>(type);
	}

	friend std::istream& operator>>(std::istream& is, Action::Type& type)
	{
		int intType;
		is >> intType;
		type = static_cast<Type>(intType);
		return is;
	}


	friend std::ostream& operator<<(std::ostream& os, Action& action)
	{
		return os << action.type << " " << action.number << " " << action.production;
	}

	friend std::istream& operator>>(std::istream& is, Action& action)
	{
		int temp;
		is >> temp >> action.number >> action.production;
		action.type = static_cast<Type>(temp);
		return is;
	}
};


struct SymbolHash
{
	size_t operator()(const Symbol& sym) const
	{
		size_t h1 = std::hash<std::string>()(sym.literal);
		// size_t h2 = std::hash<std::string>()(sym.real_value);
		size_t h3 = std::hash<int>()(static_cast<int>(sym.type));

		return h1 ^ h3;  // Combine and mix the hashes
	}
};

struct SymbolEqual
{
	bool operator()(const Symbol& lhs, const Symbol& rhs) const
	{
		return lhs.type == rhs.type &&
		       lhs.literal == rhs.literal;
		//    && lhs.real_value == rhs.real_value;
	}
};

struct ProductionHash
{
	size_t operator()(const Production& prod) const
	{
		size_t hash = SymbolHash()(prod.lhs);
		for (const auto& sym : prod.rhs) {
			hash ^= SymbolHash()(sym) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		return hash;
	}
};

struct ProductionEqual
{
	bool operator()(const Production& lhs, const Production& rhs) const
	{
		if (!SymbolEqual()(lhs.lhs, rhs.lhs) || lhs.rhs.size() != rhs.rhs.size()) {
			return false;
		}
		for (size_t i = 0; i < lhs.rhs.size(); ++i) {
			if (!SymbolEqual()(lhs.rhs[i], rhs.rhs[i])) {
				return false;
			}
		}
		return true;
	}
};

// 自定义哈希函数
struct LR1ItemHash
{
	size_t operator()(const LR1Item& item) const
	{
		return ProductionHash()(item.production) ^ std::hash<size_t>()(item.dot_position) ^ SymbolHash()(item.lookahead);
	}
};

// 自定义相等比较函数
struct LR1ItemEqual
{
	bool operator()(const LR1Item& lhs, const LR1Item& rhs) const
	{
		return ProductionEqual()(lhs.production, rhs.production) && lhs.dot_position == rhs.dot_position && SymbolEqual()(lhs.lookahead, rhs.lookahead);
	}
};

class SemanticTreeNode : public Symbol {
public:
	SemanticTreeNode(const Symbol& sym) : Symbol(sym), next_quater_id(0) {}
	bool leaf() { return children.empty(); }

	// 返回添加四元式的ID
	size_t add_quater(const Quater& quater);
	size_t add_quater(const size_t& base_id, const Quater& quater);
	size_t add_quater(const std::string& op, const std::string& arg1, const std::string& arg2, const std::string& result);
	size_t add_quater(const std::string& op, const std::string& arg1, const std::string& arg2, const size_t& result);

	// 追加一个quaters进来
	void append_quaters(const std::vector<std::pair<size_t, Quater>>& quaters);

public:
	std::vector<SemanticTreeNode*> children;
	std::vector<std::pair<size_t, Quater>> quater_list;

private:
	size_t next_quater_id;
};

class LR1Parser {
public:
	LR1Parser(const std::vector<Production>& productions, Symbol start, Symbol end);
	LR1Parser(const std::string file_path);
	LR1Parser() {}

	void print_firstSet() const;
	void print_tables() const;
	bool parse(const std::vector<Symbol>& sentence, SemanticTreeNode*& root) const;
	void save_tables(const std::string& file_path);
	void load_tables(const std::string& file_path);

private:
	void
	parse_EBNF_line(const std::string& line);
	void print_stacks(const std::stack<int>& stateStack,
	                  const std::stack<Symbol>& symbolStack,
	                  const std::vector<Symbol>& inputStack) const;

	void construct_tables();
	/**
	 * @brief 求项目集族的闭包
	 *
	 * @param lr1ItemSet 待求闭包的项目集族
	 * @return std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual> 该项目集族的闭包
	 */
	void closure(std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual>& lr1ItemSet) const;

private:
	/**
	 * @brief 获取一个LR1项目当前的状态(移进、归约、接受、待约)
	 *
	 * @param item LR1项目
	 * @return LR1Item::State 状态
	 */
	LR1Item::State get_lr1item_state(const LR1Item& item) const;

	/**
	 * @brief 获取由某非终结符为产生式左边的所有产生式
	 *
	 * @param symbol 非终结符
	 * @return std::vector<Production> 所有产生式
	 */
	std::vector<Production> get_productions_start_by_symbol(const Symbol& symbol) const;

private:  // 求FIRST集
	void calculate_firstSets();
	std::unordered_set<Symbol, SymbolHash, SymbolEqual> get_first_string(std::vector<Symbol> content) const;

private:
	std::vector<Production> productions;
	std::unordered_map<Symbol, std::vector<Production>, SymbolHash, SymbolEqual> productionMap;
	std::unordered_map<Symbol, std::unordered_set<Symbol, SymbolHash, SymbolEqual>, SymbolHash, SymbolEqual> firstSet, followSet;

	Symbol start_symbol;  // 起始符
	Symbol end_symbol;    // 终止符


	std::map<std::pair<int, Symbol>, Action> actionTable;                             // ACTION表
	std::map<std::pair<int, Symbol>, int> gotoTable;                                  // GOTO表
	std::vector<std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual>> lr1ItemSets;  // 项目集族

	std::unordered_set<std::string> terminals;  // 终结符集
};

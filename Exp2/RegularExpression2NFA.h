#pragma once
#include <cstring>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <stack>
#include <queue>
#include <set>
#include <string>
#include <iostream>
using namespace std;

// 表格
struct Table
{
	int row, col;
	char* content;
};

// 图结点
struct NfaNode {
	int id;
	vector<NfaNode*> nextNode;		// 下一结点
	map<NfaNode*, char> transition; // 下一结点 与 转移条件
};

// NFA块
struct NfaChunk {
	char op;		// 若为#，说明这仅是结点块；若不为#，说明这是运算符，后面的结点元素无实际作用
	NfaNode* start;
	NfaNode* end;

	NfaChunk() {
		start = new NfaNode();
		end = new NfaNode();
		op = '#';
	}
};

class RegularExpression2NFA
{
	NfaChunk Nfa_Graph;
	int maxId;
	vector<char> totalCondition;	// 存储所有转移条件
	vector<vector<int>> Nfa_Table;	// NFA表
	bool** mark;					// 二维数组，用于在遍历的时候标记已路过的结点
	vector<vector<int>> Dfa_Table;	// DFA表
	vector<int> simpleDfa_Table;	// 化简后的DFA表
	vector<vector<int>> minDfa_Table;		// 最小化的DFA表

public:

	string GetNFA(string regularExpression);

	NfaChunk And(char c);
	NfaChunk Or(NfaChunk a, NfaChunk b);
	NfaChunk Expand_zero(NfaChunk a);
	NfaChunk Expand_one(NfaChunk a);
	NfaChunk Choose(NfaChunk a);

	NfaChunk Connect(NfaChunk a, NfaChunk b);

	void PrintNfaGraph();
	void DFSPrint(NfaNode* root);
	string PrintNfaTable();
	void DFSPrint_table(NfaNode* root);

	string GetDFA();

	string GetMinDFA();
	void Combine(int srcRow, int destRow);


};


#pragma once
#include <cstring>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <stack>
#include <queue>
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
	NfaChunk Nfa;
	int currentId;
	vector<int> totalCondition;// 存储所有转移条件
	bool** mark;//二维数组，用于在遍历的时候标记已路过的结点

public:

	void GetNFA(string regularExpression);

	NfaChunk And(char c);
	NfaChunk Or(NfaChunk a, NfaChunk b);
	NfaChunk Expand_zero(NfaChunk a);
	NfaChunk Expand_one(NfaChunk a);

	NfaChunk Connect(NfaChunk a, NfaChunk b);

	void PrintNfaGraph();
	void DFSPrint(NfaNode* root);
	void PrintNfaTable();
	void DFSPrint_table(NfaNode* root);

};


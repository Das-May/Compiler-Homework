#pragma once

#include <vector>
#include <map>
#include <string>

using namespace std;

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

class Regex2NFA
{
public:
    vector<vector<int>> GetNFA(string Regex);

private:
    NfaChunk Connect(NfaChunk a, NfaChunk b);
    NfaChunk Expand_zero(NfaChunk a);
    NfaChunk Expand_one(NfaChunk a);
    NfaChunk Choose(NfaChunk a);
    NfaChunk And(char c);
    NfaChunk Or(NfaChunk a, NfaChunk b);

    void GenerateNFA(string Regex);
    void ChangeNFAGraphToTable();
    void DFSNFA(NfaNode* root);

private:
    NfaChunk NFAGraph;
	int maxId;
	vector<char> totalCondition;	// 存储所有转移条件
    bool** mark;					// 二维数组，用于在遍历的时候标记已路过的结点

    vector<vector<int>> NFATable;           // NFA表
};


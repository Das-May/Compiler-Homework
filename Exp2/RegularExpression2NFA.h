#pragma once
#include <cstring>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <stack>
#include <queue>
#include <set>
using namespace std;

// ���
struct Table
{
	int row, col;
	char* content;
};

// ͼ���
struct NfaNode {
	int id;
	vector<NfaNode*> nextNode;		// ��һ���
	map<NfaNode*, char> transition; // ��һ��� �� ת������
};

// NFA��
struct NfaChunk {
	char op;		// ��Ϊ#��˵������ǽ��飻����Ϊ#��˵�����������������Ľ��Ԫ����ʵ������
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
	vector<char> totalCondition;	// �洢����ת������
	vector<vector<int>> Nfa_Table;	// NFA��
	bool** mark;					// ��ά���飬�����ڱ�����ʱ������·���Ľ��
	vector<vector<int>> Dfa_Table;	// DFA��
	vector<int> MinDfa_Table;	// ��С��DFA��

public:

	void GetNFA(string regularExpression);

	NfaChunk And(char c);
	NfaChunk Or(NfaChunk a, NfaChunk b);
	NfaChunk Expand_zero(NfaChunk a);
	NfaChunk Expand_one(NfaChunk a);
	NfaChunk Choose(NfaChunk a);

	NfaChunk Connect(NfaChunk a, NfaChunk b);

	void PrintNfaGraph();
	void DFSPrint(NfaNode* root);
	void PrintNfaTable();
	void DFSPrint_table(NfaNode* root);

	void GetDFA();

	void GetMinDFA();


};


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
    /**
     * @brief Generate a new NfaChunk
     */
    NfaChunk GenerateNFAChunk(char c);

    /**
     * @brief Connect two words
     */
    NfaChunk Connect(NfaChunk a, NfaChunk b);
    /**
     * @brief Regex operator *
     */
    NfaChunk Expand_zero(NfaChunk a);
    /**
     * @brief Regex operator ?
     */
    NfaChunk Choose(NfaChunk a);
    /**
     * @brief Regex operator |
     */
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
    int NFARow;
    int NFACol;
};


#ifndef REGULAREXPRESSIONPROCESSOR_H
#define REGULAREXPRESSIONPROCESSOR_H

#include <cstring>
#include <vector>
#include <map>
#include <utility>
#include <stack>
#include <queue>
#include <set>
#include <list>
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
    char id;
    vector<char> nextNode;		// 下一结点
    vector<string> transition; // 下一结点 与 转移条件
};

static class RegularExpressionProcessor
{
    static vector<NfaNode> Nfa_Graph;

    static string PrintNfaGraph();

public:
    static string GetNFA(string regularExpression);
    static string GetDFA();
    static string GetMinDFA();

};

#endif // REGULAREXPRESSIONPROCESSOR_H

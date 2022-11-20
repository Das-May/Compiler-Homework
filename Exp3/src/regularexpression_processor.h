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

class RegularExpressionProcessor
{
    vector<vector<char>> NFA;
    vector<vector<char>> DFA;
    vector<vector<char>> minDFA;

    static string PrintFATable(int row, int col, vector<vector<int>>& FA);

public:
    void SetNFA(vector<vector<char>> nfa);
    string GetNFA();
    string GetDFA();
    string GetMinDFA();

};

#endif // REGULAREXPRESSIONPROCESSOR_H

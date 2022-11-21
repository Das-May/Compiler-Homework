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

    string PrintFATable(vector<vector<char>>::iterator it);

    bool find(vector<char>::iterator begin, vector<char>::iterator end, char value);

public:
    void SetNFA(vector<vector<char>> nfa);
    string GetNFA();
    string GetDFA();
    string GetMinDFA();

};

#endif // REGULAREXPRESSIONPROCESSOR_H

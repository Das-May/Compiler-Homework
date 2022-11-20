#ifndef GRAMMARPROCESSOR_H
#define GRAMMARPROCESSOR_H

#include <vector>
#include <map>
#include <list>
#include <cstring>
#include <iostream>
using namespace std;

struct Rule{
    int left;
    vector<int> right;
    bool operator == (const Rule& a) const{
        if(left == a.left && right == a.right)
            return true;
        return false;
    }

};

class GrammarProcessor
{
private:
    map<int, char> v;// 字典映射表
    list<Rule> grammar;// 文法规则
    list<int> vn;//非终结符集合
    int n_num = 0, t_num = 100;
    char n_char = 0;
    int temp_set[200];
    list<int> first;
    vector<vector<char>> NFA;

    void AddVn(char c);
    void AddVt(char c);
    string PrintRule(Rule r);
    int value2key(char c);
    bool isVn(int num);
    bool isVt(int num);
    bool find(vector<int>::iterator a, vector<int>::iterator b, int value);
    void OrganizeDict();

    void RemoveHarmfulRules();
    void RemoveUnreachableRules();
    bool RemoveUnterminableRules_sub(int vn, int depth);
    void RemoveUnterminableRules();

    void GetFirst_sub(int x);
    list<int>& GetFirst(int x);

    void GetFollow_sub(int x);
    list<int> GetFollow(int x);

    bool LL2NFA();
    bool RL2NFA();

public:
    GrammarProcessor(char *c);

    string SimplifyGrammar();

    string GetFirst();

    string GetFollow();

    string RemoveLeftCommonFactor();
    string RemoveLeftRecursion();

    string GetRegularExpression();

    vector<vector<char>> GetNFA();

    string PrintGrammar();
};

#endif // GRAMMARPROCESSOR_H

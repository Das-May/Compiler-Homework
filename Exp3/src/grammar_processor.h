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

};

class GrammarProcessor
{
private:
    map<int, char> v;
    list<Rule> grammar;
    list<int> vn;
    int n_num = 0, t_num = 101;

    void AddVn(char c);
    void AddVt(char c);
    string PrintRule(Rule r);
    int value2key(char c);

public:
    GrammarProcessor(char *c);

    void RemoveHarmfulRules();
    void RemoveUnreachableRules();
    void RemoveUnterminableRules();

    void GetFirst(int x);
    void GetFollow(int x);

    void RemoveLeftCommonFactor();
    void RemoveLeftRecursion();

    string PrintGrammar();
};

#endif // GRAMMARPROCESSOR_H

#ifndef GRAMMARPROCESSOR_H
#define GRAMMARPROCESSOR_H

#include <regularExpression_processor.h>
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
    map<int, char> v;
    list<Rule> grammar;
    list<int> vn;
    int n_num = 0, t_num = 101;
    char n_char = 0;
    int temp_set[200];
    list<int> first;

    void AddVn(char c);
    void AddVt(char c);
    string PrintRule(Rule r);
    int value2key(char c);
    bool MatchFormat(Rule r, bool alpha, bool B, bool beta, bool BsameA);
    bool find(vector<int>::iterator a, vector<int>::iterator b, int value);

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

    string PrintGrammar();
};

#endif // GRAMMARPROCESSOR_H

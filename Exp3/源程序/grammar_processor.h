#ifndef GRAMMARPROCESSOR_H
#define GRAMMARPROCESSOR_H

#include <vector>
#include <map>
#include <list>
#include <queue>
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
    bool operator < (const Rule& a) const{
        if(left < a.left)
            return true;
        return false;
    }
};

class GrammarProcessor
{
private:
    map<int, char> v;           // 字典映射表
    list<Rule> grammar;         // 文法规则
    //list<int> vn;               // 非终结符集合
    int n_num = 0, t_num = 100; // 非终结符起始符号，终结符起始符号
    char n_char = 0;            // 记录最新的非终结符，以便分配新的非终结符字符（n_char + 1）
    int temp_set[200];          // 算法中多次用到并查集的思想，所以预先分配一个数组
    list<int> first;            // first集采用深度优先算法生成，故在函数外定义存储结构
    vector<vector<char>> NFA;   // NFA表格

    /*工具函数*/
    void AddVn(char c);         // 添加非终结符
    void AddVt(char c);         // 添加终结符
    string PrintRule(Rule r);   // 打印文法
    int value2key(char c);      // 根据符号查找映射表中的序号
    bool isVn(int num);         // 判断是否为非终结符
    bool isVt(int num);         // 判断是否为终结符
    bool find(vector<int>::iterator a, vector<int>::iterator b, int value);// 查询容器中是否含有某值（不知为何Qt中用不了std::find(…)）
    void OrganizeDict();        // 整理映射表，去除已经被删去的字符索引
    void OrganizeGrammar();     // 排序文法规则

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

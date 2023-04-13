#ifndef GRAMMARPROCESSOR_H
#define GRAMMARPROCESSOR_H

#include <vector>
#include <map>
#include <list>
#include <queue>
#include <cstring>

using namespace std;

struct Rule{
    int Left;
    vector<int> Right;
    bool operator == (const Rule& a) const{
        if(Left == a.Left && Right == a.Right)
            return true;
        return false;
    }
    bool operator < (const Rule& a) const{
        if(Left < a.Left)
            return true;
        return false;
    }
};

class GrammarProcessor
{
private:
    map<int, string> ID2Word;           // 字典映射表
    map<int, char> v;           // 字典映射表
    list<Rule> Grammar;         // 文法规则
    //list<int> vn;               // 非终结符集合
    int NonterminalLatestID = 0, TerminalLatestID = 100; // 非终结符起始符号，终结符起始符号
    char n_char = 0;            // 记录最新的非终结符，以便分配新的非终结符字符（n_char + 1）
    int TempSet[200];          // 算法中多次用到并查集的思想，所以预先分配一个数组
    list<int> first;            // first集采用深度优先算法生成，故在函数外定义存储结构

    /*工具函数*/
    void AddVn(char c);         // 添加非终结符
    void AddVt(char c);         // 添加终结符
    string PrintRule(Rule r);   // 打印文法
    /**
     * @brief Find the corresponding ID by Word in ID2Word
     * @param the word we known
     * @return the ID of the Word; if the key is not existed, return -1.
     */
    int Word2ID(string TargetWord);      // 根据符号查找映射表中的序号
    bool isVn(int num);         // 判断是否为非终结符
    bool isVt(int num);         // 判断是否为终结符
    bool find(vector<int>::iterator a, vector<int>::iterator b, int value);// 查询容器中是否含有某值（不知为何Qt中用不了std::find(…)）
    void OrganizeDict();        // 整理映射表，去除已经被删去的字符索引

    /**
     * @brief Organize map ID2Word, divide terminal(0~99) and non-terminal(100~199)
     */
    void OrganizeID2Word();

    /**
     * @brief Organize the grammar rules by the left ID
     */
    void OrganizeGrammar();

    void RemoveHarmfulRules();
    void RemoveUnreachableRules();
    bool RemoveUnterminableRules_sub(int vn, int depth);
    void RemoveUnterminableRules();

    void GetFirst_sub(int x);
    list<int>& GetFirst(int x);

    void GetFollow_sub(int x);
    list<int> GetFollow(int x);

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

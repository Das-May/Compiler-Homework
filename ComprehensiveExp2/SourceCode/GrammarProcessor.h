#ifndef GRAMMARPROCESSOR_H
#define GRAMMARPROCESSOR_H

#include <vector>
#include <map>
#include <list>
#include <string>

using namespace std;

struct Rule{
    int Left;
    vector<int> Right;

    Rule(){}
    Rule(int left, vector<int> right)
    {
        Left = left;
        Right = right;
    }

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
    /**
     * @brief ID2Word 字典映射表
     */
    map<int, string> ID2Word;
    /**
     * @brief 存储文法规则
     */
    list<Rule> Grammar;
    int NonterminalLatestID = 0, TerminalLatestID = 100; // 非终结符起始符号，终结符起始符号

    /**
     * @brief 算法中多次用到并查集的思想，所以分配一个公用的数组。
     * 请尤其注意数组污染的问题，使用前务必初始化。
     */
    int TempSet[200];
    /**
     * @brief first集采用深度优先算法生成，为了减少传值带来的开销，故在函数外定义存储结构
     */
    list<int> first;

private:
    /**
     * @brief Print a single rule, whose simbols are transformed to specific expression
     * @param The rule which you want to print
     * @return Specific grammar rule string
     */
    string PrintRule(const Rule& Rule);

    /**
     * @brief Find the corresponding ID by Word in ID2Word
     * @param the word we known
     * @return the ID of the Word; if the key is not existed, return -1.
     */
    int Word2ID(string TargetWord);

    /**
     * @brief Add non-terminal to ID2Word
     * @param BaseSymbol : the new non-terminal was transformed from the base symbol, generally, append "'" after it.
     */
    void AddNonterminal(string BaseSymbol);
    /**
     * @brief Add terminal to ID2Word
     * @param BaseSymbol : the new terminal was transformed from the base symbol, generally, append "'" after it.
     */
    void AddTerminal(string BaseSymbol);

    bool IsNonterminal(int num);
    bool IsTerminal(int num);

    /**
     * @brief Organize map ID2Word, divide terminal(0~99) and non-terminal(100~199)
     */
    void OrganizeID2Word();
    void OrganizeGrammar();

    /**
     * @brief Sort the grammar rules by the left ID
     */
    void SortGrammar();

    void RemoveHarmfulRules();
    void RemoveUnreachableRules();
    bool RemoveUnterminableRules_sub(int NonterminalID, int depth);
    void RemoveUnterminableRules();

    vector<list<Rule>::iterator> FindSameLeftRules(int Left);
    list<int> FindPossibleLCF(int ID, list<int> First);

    /**
     * @brief Find left Recursion(LR) by using DFS, the LR may be direct or indirect
     * @param ID of target left of a grammar
     * @param ID of current symbol, may be nonterminal or terminal
     * @return if exist LR, for example, A -> Aabcde, return abcde.
     */
    vector<int> FindLeftRecur(int TargetLeft, int CurrentLeft);

    void GetFirst_sub(int x);
    list<int>& GetFirst(int x);

    void GetFollow_sub(int x);
    list<int> GetFollow(int x);

public:
    /**
     * @brief Create symbol dictionary and grammar rules
     * @param grammar rules text, see readme.md for the text format
     */
    GrammarProcessor(char *c);

    string SimplifyGrammar();

    string GetFirst();

    string GetFollow();

    string RemoveLeftCommonFactor();

    string RemoveLeftRecursion();

    vector<vector<int>> GetLL1Table();

public:
    string PrintGrammar();
    map<int, string> GetID2Word();
};

#endif // GRAMMARPROCESSOR_H

#ifndef GRAMMARANALYST_H
#define GRAMMARANALYST_H

#include <vector>
#include <map>
#include <string>
using namespace std;

struct TreeNode
{
    string Symbol;
    vector<TreeNode*> Child;

    TreeNode(string symbol){
        Symbol = symbol;
    }
    TreeNode(){Symbol = "";}
};

class GrammarAnalyst
{
public:
    void Init(const vector<vector<int>>& LL1Table, const map<int, string> ID2Word);
    string Execute(char *code);
    TreeNode* GetRoot();

private:
    vector<vector<int>> LL1Table;
    map<int, string> ID2Word;
    int NumberID;
    int IdentiferID;
    map<int, int> Terminal2Col;

    char* Buffer = 0;
    int Pos = 0;
    struct {
      int ID;
      string Word;
    } Token;
    string Output = "";

    TreeNode *Root;

private:
    TreeNode* BuildGrammarTree(int ID);
    void GetToken();
    void Match(int ExpectTokenId);
    void Error();
    void GetGrammarTree(TreeNode* node, int deepth);
};

#endif // GRAMMARANALYST_H

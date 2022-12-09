#ifndef TINY_H
#define TINY_H

#include <string>
#include <map>

using namespace std;

// 单词种类的枚举
enum TokenID {
    IDENTIFIER, NUMBER,
    ADD,SUB,MUL,DIV,LBRACKET,RBRACKET,
    IF, THEN, ELSE, END, REPEAT, READ, WRITE,ENDINPUT,
    BEGIN_PROGRAM, END_PROGRAM, ERROR
};
// 单词结构
struct TokenStru
{
  TokenID  ID;
  int num;
  string word;
};
// 二叉树结点结构体
struct BTreeNode
{
    TokenStru data;
    BTreeNode* lc, * rc;
};

class Tiny
{
public:
    Tiny();
    void execute(char *c);
    string GetOutput();
    string GetsyntaxTree();
private:
    map<string, TokenID> keyword;// 关键字字典
    TokenStru token;
    char *buffer = 0;
    int pos=0;
    string output = "";
    string syntaxTree = "";

    void GetToken();
    void match(TokenID expecttokenid);
    void preorder(BTreeNode* t, int deepth);
    void error();


    // 测试用例1 测这部分
    BTreeNode* program();
    BTreeNode* stmt_sequence();
    BTreeNode* statement();
    BTreeNode* read_stmt();
    BTreeNode* write_stmt();

    // 测试用例2 测这部分
    BTreeNode* exp();


};

#endif // TINY_H

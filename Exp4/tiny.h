#ifndef TINY_H
#define TINY_H

#include <string>
#include <map>

using namespace std;

// 单词种类的枚举
enum TokenID {
    IDENTIFIER, NUMBER,
    ASSIGN, ADD,SUB,MUL,DIV,SUB_ASSIGN, REMAIN, POWER, LBRACKET,RBRACKET,//算数运算
    EQUAL, MORE, MORE_OR_EQUAL, LESS, LESS_OR_EQUAL, NOT_EQUAL,//比较
    AND, OR, NOT,//逻辑运算
    REGULAR_ASSIGN, REGULAR_OR, REGULAR_CONNECT, REGULAR_CLOSURE,//正则运算
    SEMICOLON, IF, THEN, ELSE, END, REPEAT, READ, WRITE,//关键字
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
    BTreeNode(TokenStru d)// 构造函数
    {
        data = d;
        lc = rc = 0;
    }
    BTreeNode(){}
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

    //
    BTreeNode* read_stmt();
    BTreeNode* write_stmt();
    BTreeNode* assign_stmt();

    // 测试用例2 测这部分
    BTreeNode* exp();
    BTreeNode* arithmetic_exp();
    BTreeNode* term();
    BTreeNode* factor();
    BTreeNode* comop();
    BTreeNode* addop();
    BTreeNode* mulop();

    // 测试用例3 测这部分
    BTreeNode* bool_exp();
    BTreeNode* bterm();
    BTreeNode* bfactor();

    //
    BTreeNode* if_stmt();
    BTreeNode* repeat_stmt();

};

#endif // TINY_H

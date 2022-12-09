#include "tiny.h"

#include <iostream>

Tiny::Tiny()
{

    keyword["if"] = IF;
    keyword["then"] = THEN;
    keyword["else"] = ELSE;
    keyword["end"] = END;
    keyword["repeat"] = REPEAT;
    keyword["read"] = READ;
    keyword["write"] = WRITE;
    keyword["end"] = ENDINPUT;

}

#pragma region "对外接口"{
void Tiny::execute(char *c)
{
    buffer = 0;
    pos = 0;
    string output = "";
    string syntaxTree = "";

    GetToken();

    BTreeNode* root = program();

    preorder(root, 0);
}

string Tiny::GetOutput()
{
    return output;
}

string Tiny::GetsyntaxTree()
{
    return syntaxTree;
}
#pragma endregion }

#pragma region "框架函数"{
void Tiny::GetToken()
{
    while (buffer[pos]==32||buffer[pos]==9)  // 遇到 空格或换行符
           pos++; // 持续向前扫描
    if (buffer[pos] == '{')// 遇到 注释
    {
        while(buffer[pos]!='}')
            pos++;// 持续向前扫描
        pos++;//直到扫过注释结束符
    }

    if((buffer[pos]>='0' )&& (buffer[pos]<='9'))  // 遇到 数字
    {
        int temp_num = 0;
        while (buffer[pos]>='0' && buffer[pos]<='9')
        {
            temp_num = temp_num * 10 + buffer[pos] - '0';
            pos++ ;
        }
        token.ID=NUMBER;
        token.num = temp_num;
    }

    else if((buffer[pos]>='a' )&& (buffer[pos]<='z'))// 遇到 字母
    {
        string temp_word = "";
        while((buffer[pos]>='a' )&& (buffer[pos]<='z'))
        {
            temp_word += buffer[pos];
            pos++;
        }
        token.word = temp_word;

        token.ID = IDENTIFIER;
        for(auto& k : keyword)
        {
            if(k.first == temp_word)
            {
                token.ID = k.second;
                break;
            }
        }
    }

    //else if(buffer[pos]!='\0')//遇到其它字符
    else
    {
        switch(buffer[pos])
        {
            case '+':
                token.ID = ADD;
                token.word = buffer[pos];
                break;
            case '-':
                token.ID=SUB;
                token.word=buffer[pos];
                break;
            case '*':
                token.ID=MUL;
                token.word=buffer[pos];
                break;
            case '/':
                token.ID=DIV;
                token.word=buffer[pos];
                break;
            case '(':
                token.ID=LBRACKET;
                token.word=buffer[pos];
                break;
            case ')':
                token.ID=RBRACKET;
                token.word=buffer[pos];
                break;
            default:
                cout<<" Error Input at: "<<pos+1;
                exit(1);
        }
        pos++;
    }
}

void Tiny::match(TokenID expecttokenid)
{
    if (token.ID == expecttokenid)
        GetToken();
    else
        error();
}

void Tiny::preorder(BTreeNode* t, int deepth)
{
    if (t == 0) return;

    for(int i = 0; i < deepth; i++)
        syntaxTree += '\t';
    if (t->data.ID == NUMBER)
    {
        syntaxTree = syntaxTree + to_string(t->data.num) + '\n' ;
    }
    else
        syntaxTree = syntaxTree + t->data.word  + '\n' ;

    preorder(t->lc, deepth+1);
    preorder(t->rc, deepth+1);
}

void Tiny::error()
{
    output = "error input at position: " + to_string(pos + 1);
    token.ID = ERROR;
}

#pragma endregion }

#pragma region "文法规则"{
BTreeNode* Tiny::program()
{
    BTreeNode* root = new BTreeNode;
    root->data.word = "BEGIN_PROGRAM";
    root->rc = 0;
    root->lc = stmt_sequence();
    return root;
}
BTreeNode* Tiny::stmt_sequence()
{
    BTreeNode* nodex, * nodey;

    nodex = statement();

    while (token.ID == IDENTIFIER && token.word == ";")
    {
        nodey = new BTreeNode;   // 生成结点
        nodey->data = token;
        nodey->lc = nodex;
        match(token.ID);
        nodey->rc = statement();
        nodex = nodey;
    }
    return nodex;
}

BTreeNode* Tiny::statement()
{
    BTreeNode* node;

    switch (token.ID)
    {
    case READ:
        node = read_stmt();
        break;
    case WRITE:
        node = write_stmt();
        break;
    default:
        error();
    }

    return node;
}

BTreeNode* Tiny::read_stmt()
{
    BTreeNode* nodex = new BTreeNode;
    nodex->data = token;
    nodex->lc = nodex->rc = 0;
    match(READ);
    match(IDENTIFIER);
    return nodex;
}

BTreeNode* Tiny::write_stmt()
{
    BTreeNode* nodex = new BTreeNode;
    nodex->data = token;
    nodex->rc = 0;
    match(WRITE);
    nodex->lc = exp();
    return nodex;
}

BTreeNode* Tiny::exp()
{
    return 0;
}
#pragma endregion }

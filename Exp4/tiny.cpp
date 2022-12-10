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

}

#pragma region "对外接口"{
void Tiny::execute(char *c)
{
    buffer = c;
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
    while (buffer[pos]==' ' || buffer[pos]=='\t' || buffer[pos]=='\n' || buffer[pos]=='{')  // 遇到 空格或制表符或换行符
    {
        if (buffer[pos] == '{')// 遇到 注释
        {
            while(buffer[pos]!='}')
               pos++;// 持续向前扫描
            pos++;//直到扫过注释结束符
        }
        else
            pos++; // 持续向前扫描
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
    else if(buffer[pos]!='\0')
    {
        token.word = buffer[pos];
        switch(buffer[pos])
        {

        #pragma region "算数运算"{
        case '+':
            token.ID = ADD;
            break;
        case '-':
            if(buffer[pos + 1] == '='){
                token.ID = SUB_ASSIGN;
                token.word = "-=";
                pos++;
            }
            else
                token.ID=SUB;
            break;
        case '*':
            token.ID=MUL;
            break;
        case '/':
            token.ID=DIV;
            break;
        case '%':
            token.ID=REMAIN;
            break;
        case '^':
            token.ID=POWER;
            break;
        case '=':
            if(buffer[pos + 1] == '='){
                token.ID = EQUAL;
                token.word = "==";
                pos++;
            }
            else
                token.ID = ASSIGN;
            break;
        #pragma endregion }

        #pragma region "比较"{
        case '>':
            if(buffer[pos + 1] == '='){
                token.ID = MORE_OR_EQUAL;
                token.word = ">=";
                pos++;
            }
            else
                token.ID = MORE;
            break;
        case '<':
            if(buffer[pos + 1] == '='){
                token.ID = LESS_OR_EQUAL;
                token.word = "<=";
                pos++;
            }
            else if(buffer[pos + 1] == '>'){
                token.ID = NOT_EQUAL;
                token.word = "<>";
                pos++;
            }
            else
                token.ID = LESS;
            break;
        #pragma endregion }

        case '(':
            token.ID=LBRACKET;
            token.word=buffer[pos];
            break;
        case ')':
            token.ID=RBRACKET;
            token.word=buffer[pos];
            break;

        case ';':
            token.ID=SEMICOLON;
            token.word=buffer[pos];
            break;

        default:
            cout<<" Error Input at: "<<pos+1;
            exit(1);
        }
        pos++;
    }

    else
        token.ID = END_PROGRAM;
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
        syntaxTree += ' ';

    if (t->data.ID == NUMBER)
    {
        syntaxTree = syntaxTree + to_string(t->data.num) + '\n' ;
    }
    else
        syntaxTree = syntaxTree + t->data.word  + '\n' ;
    //syntaxTree = syntaxTree + string(t->data.ID) + '\n' ;

    preorder(t->lc, deepth+1);
    preorder(t->rc, deepth+1);
}

void Tiny::error()
{
    output = "error input at position: " + to_string(pos + 1);
    token.ID = ERROR;
}

#pragma endregion }

#pragma region "基础架构"{
BTreeNode* Tiny::program()
{
    BTreeNode* root = stmt_sequence();
    return root;
}

BTreeNode* Tiny::stmt_sequence()
{
    BTreeNode* nodex, * nodey;

    nodex = statement();

    while (token.ID == SEMICOLON)
    {
        nodey = new BTreeNode(token);   // 生成结点
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
    case IDENTIFIER:
        node = assign_stmt();
        break;
    case IF:
        node = if_stmt();
        break;
    case REPEAT:
        node = repeat_stmt();
        break;
    case END:
        node = new BTreeNode(token);
        break;
    default:
        error();
    }

    return node;
}

#pragma endregion }

BTreeNode* Tiny::read_stmt()
{
    BTreeNode* nodex = new BTreeNode(token);
    match(READ);
    nodex->lc = new BTreeNode(token);
    match(IDENTIFIER);
    return nodex;
}

BTreeNode* Tiny::write_stmt()
{
    BTreeNode* nodex = new BTreeNode(token);
    match(WRITE);
    nodex->lc = exp();
    return nodex;
}

BTreeNode* Tiny::exp()
{
    return arithmetic_exp();
}

BTreeNode* Tiny::assign_stmt()
{
    BTreeNode* nodex = 0, *nodey;
    nodey = new BTreeNode(token);
    match(IDENTIFIER);

    if(token.ID == ASSIGN || token.ID == SUB_ASSIGN ||  token.ID == REGULAR_ASSIGN)
    {
        nodex = new BTreeNode(token);
        nodex->lc = nodey;
        match(token.ID);

        nodex->rc = exp();
    }
    else
    {
        error();
    }
    return nodex;
}

#pragma region "算数表达式"{

BTreeNode* Tiny::arithmetic_exp()
{
    BTreeNode* nodex = 0, *nodey = 0;
    nodex = term();

    while (token.ID == ADD || token.ID == SUB)
    {
        nodey = new BTreeNode;// 新的父节点
        nodey->data = token;
        nodey->lc = nodex;
        match(token.ID);

        nodey->rc = term();
        nodex = nodey;
    }
    return nodex;
}

BTreeNode* Tiny::term()
{
    BTreeNode* nodex, * nodey;
    // factor
    nodex = factor();
    // mulop
    while (token.ID == MUL || token.ID == DIV ||
           token.ID == REMAIN || token.ID == POWER)
    {
        nodey = new BTreeNode;   // 新的父节点
        nodey->data = token;
        nodey->lc = nodex;
        /*switch (token.ID)
        {
        case MUL:
            break;
        }*/
        match(token.ID);

        // factor
        nodey->rc = factor();
        nodex = nodey;
    }

    return nodex;
}

BTreeNode* Tiny::factor()
{
    BTreeNode* nodex = 0;

    switch (token.ID)
    {
    case LBRACKET:
        match(LBRACKET);
        nodex = exp();
        match(RBRACKET);
        break;
    case NUMBER:
        nodex = new BTreeNode(token);    // 生成结点
        match(NUMBER);
        break;
    case IDENTIFIER:
        nodex = new BTreeNode(token);
        match(IDENTIFIER);
        break;
    default:
        error();
    }
    return nodex;
}

#pragma endregion }

BTreeNode* Tiny::if_stmt()
{

}

BTreeNode* Tiny::repeat_stmt()
{

}




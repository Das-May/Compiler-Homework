#include "Loader.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

char* Loader::FilePath = "";

char* Loader::ReadText(const char* FilePath)
{
    ifstream Reader;
    Reader.open(FilePath, ios::in);
    if (!Reader)
    {
        //cerr << "文件读取失败！" << endl;
        cout <<"文件读取失败！" << FilePath;
        return 0;
    }

    Loader::FilePath = new char[strlen(FilePath) + 1];
    strcpy(Loader::FilePath, FilePath);
    cout << "FilePath: " << Loader::FilePath << endl;

    stringstream in;
    in << Reader.rdbuf();
    string s = in.str();
    cout << "Context: \n" << in.str() << endl;

    int len = in.str().size();
    cout << "Text Length: " << len << endl;

    char* c = new char[len+1];
    memcpy(c, s.c_str(), len+1);
    cout << "Record: \n" << c << endl;
    return c;
}

void Loader::WriteText(const char* FilePath, const char*Text)
{
    cout << "文件保存路径： " << FilePath << endl;

    ofstream Writer;
    Writer.open(FilePath, ios::out);
    if (!Writer)              // 如果打开失败
    {
        cerr << "文件保存失败！" << endl;
        abort();
    }

    Writer.write(Text, strlen(Text));
    cout << "保存成功！" << endl;

    Writer.close();
}

char* Loader::GetFilePath()
{
    return FilePath;
}

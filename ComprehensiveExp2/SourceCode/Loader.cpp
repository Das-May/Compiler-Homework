#include "Loader.h"
#include <cstring>
#include <iostream>
#include <fstream>

char* Loader::FilePath = "";

char* Loader::ReadText(const char* FilePath)
{
    ifstream Reader;
    Reader.open(FilePath, ios::in);

    if (!Reader.is_open())
    {
        //cerr << "文件读取失败！" << endl;
        cout <<"文件读取失败！" << FilePath;
        return 0;
    }

    // 记录文件读取路径
    Loader::FilePath = new char[strlen(FilePath) + 1];
    strcpy(Loader::FilePath, FilePath);
    cout << "文件读取路径： " << Loader::FilePath << endl;

    // 获取文本长度
    Reader.seekg(0, ios::end);
    int len = Reader.tellg();
    cout << "文件内容长度： " << len << endl;

    // 获取文本内容
    Reader.seekg(0, ios::beg);
    char* c = new char[ len + 1 ];
    memset(c, 0, len + 1);
    Reader.read(c,len);
    cout << "文件内容： " << c << endl;

    Reader.close();

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

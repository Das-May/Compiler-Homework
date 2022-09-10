#include "Loader.h"

char* Loader::InputTxt(const char* filePath)
{
    // 打开.cpp文件
    ifstream cppFile(filePath, ios::in | ios::_Noreplace);
    if (!cppFile)
    {
        cerr << "文件读取失败" << endl;
        abort();
    }

    // 记录文件路径
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);

    // 获取文本长度
    int len;
    cppFile.seekg(0, ios::end);
    len = cppFile.tellg();
    cout << len << endl;

    // 获取文本内容
    cppFile.seekg(0, ios::beg);
    char* cppChar = new char[ len + 1 ];
    memset(cppChar, 0, len + 1);
    cppFile.read(cppChar,len);
    cout << cppChar << endl;

    //返回char指针
    return cppChar;
}

void Loader::OutputBin(const char* compressed)
{
    
}

char* InputBin(const char* filePath);
void OutputTxt(const char* decompressed);

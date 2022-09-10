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

    // 读取文件
    cout << cppFile.tellg();

    //返回char指针

}

void OutputBin(const char* compressed);
char* InputBin(const char* filePath);
void OutputTxt(const char* decompressed);

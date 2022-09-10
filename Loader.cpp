#include "Loader.h"

char* Loader::InputCpp(const char* filePath)
{
    // 打开.cpp文件
    ifstream cppFile(filePath, ios::in | ios::_Noreplace);
    if (!cppFile)
    {
        cerr << "cpp文件读取失败" << endl;
        abort();
    }

    // 记录文件路径
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);

    // 获取文本长度
    int len;
    cppFile.seekg(0, ios::end);
    len = cppFile.tellg();
    cout << "cpp文本内容长度： " << len << endl;

    // 获取文本内容
    cppFile.seekg(0, ios::beg);
    char* cppChar = new char[ len + 1 ];
    memset(cppChar, 0, len + 1);
    cppFile.read(cppChar,len);
    cout << "cpp文本内容： " << cppChar << endl;

    //返回char指针
    return cppChar;
}

void Loader::OutputBin(const char* compressed)
{
    cout << filePath << endl;
    int len = strlen(filePath) - 3;          
    filePath[len] = '\0';                       // 截掉cpp后缀
    strcat(filePath, "bin");   // 追加bin后缀
    cout << "二进制文件保存路径： " << filePath << endl;
    ofstream binFile(filePath, ios::out | ios::binary);
    if (!binFile)    // 如果打开失败
    {
        cerr << "压缩文件写入失败！" << endl;
        abort();
    }

    binFile.write(compressed, sizeof(compressed));
    cout << "压缩成功！" << endl;
}

char* Loader::InputBin(const char* filePath)
{
    // 打开二进制文件
    ifstream binFile(filePath, ios::in | ios::binary);
    if (!binFile)
    {
        cerr << "cpp文件读取失败" << endl;
        abort();
    }

    // 记录文件路径
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);

    // 获取文本长度
    int len;
    binFile.seekg(0, ios::end);
    len = binFile.tellg();
    cout << len << endl;

    // 获取文本内容
    binFile.seekg(0, ios::beg);
    char* cppChar = new char[len + 1];
    memset(cppChar, 0, len + 1);
    binFile.read(cppChar, len);
    cout << cppChar << endl;

    //返回char指针
    return cppChar;
}

void Loader::OutputCpp(const char* decompressed)
{
    cout << filePath << endl;
    ofstream binFile(strcat(filePath, ".txt"), ios::out | ios::binary);
    if (!binFile)    // 如果打开失败
    {
        cerr << "压缩文件写入失败！" << endl;
        abort();
    }

    binFile.write(decompressed, sizeof(decompressed));
    cout << "压缩成功！" << endl;
}

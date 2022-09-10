#include "Loader.h"

char* Loader::Input(const char* filePath, int type)    // type=1,读入cpp文件；type=0,读入bin文件
{
    // 打开文件
    ifstream file;
    if (type)
        file.open(filePath, ios::in | ios::binary);
    else
        file.open(filePath, ios::in | ios::binary);
    if (!file)
    {
        cerr << "文件读取失败！" << endl;
        abort();
    }

    // 记录文件路径
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);
    cout << "文件路径： " << filePath << endl;

    // 获取文本长度
    int len;
    file.seekg(0, ios::end);
    len = file.tellg();
    cout << "文件内容长度： " << len << endl;

    // 获取文本内容
    file.seekg(0, ios::beg);
    char* c = new char[ len + 1 ];
    memset(c, 0, len + 1);
    file.read(c,len);
    cout << "文件内容： " << c << endl;

    //返回char指针
    return c;
}

void Loader::Output(const char* processed, int type)    // type=1,导出bin文件；type=0，导出cpp文件
{
    // 处理文件保存路径
    int len = strlen(filePath) - 3;          
    filePath[len] = '\0';                       // 截掉
    if(type)                                    // 追加
        strcat(filePath, "bin");             
    else
        strcat(filePath, "cpp");
    cout << "文件保存路径： " << filePath << endl;

    // 保存文件
    ofstream file;
    if (type)
        file.open(filePath, ios::out | ios::binary);
    else
        file.open(filePath, ios::out);
    if (!file)    // 如果打开失败
    {
        cerr << "文件写入失败！" << endl;
        abort();
    }

    file.write(processed, sizeof(processed));
    if (type)
        cout << "压缩成功！" << endl;
    else
        cout << "解压成功！" << endl;
}

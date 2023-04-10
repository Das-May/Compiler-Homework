#include "Loader.h"

char* Loader::filePath = "";

char* Loader::Input(const char* filePath)
{
    // 判断文件类型
    int len = strlen(filePath);

    // 打开文件
    ifstream file;
    file.open(filePath, ios::in);
    if (!file)
    {
        //cerr << "文件读取失败！" << endl;
        qDebug()<<"文件读取失败！" << filePath;
        return 0;
    }

    // 记录文件读取路径
    Loader::filePath = new char[strlen(filePath) + 1];
    strcpy(Loader::filePath, filePath);
    cout << "文件读取路径： " << Loader::filePath << endl;

    // 获取文本长度
    file.seekg(0, ios::end);
    len = file.tellg();
    cout << "文件内容长度： " << len << endl;

    // 获取文本内容
    file.seekg(0, ios::beg);
    char* c = new char[ len + 1 ];
    memset(c, 0, len + 1);
    file.read(c,len);
    cout << "文件内容： " << c << endl;

    // 关闭文件
    file.close();

    //返回char指针
    return c;
}

void Loader::Output(const char* processed)
{
    // 处理文件保存路径
    cout << "文件保存路径： " << filePath << endl;

    // 保存文件
    ofstream file;
    file.open(filePath, ios::out);
    if (!file)              // 如果打开失败
    {
        cerr << "文件保存失败！" << endl;
        abort();
    }

    file.write(processed, strlen(processed));
    cout << "保存成功！" << endl;

    // 关闭文件
    file.close();
}

char* Loader::GetFilepath()
{
    return filePath;
}

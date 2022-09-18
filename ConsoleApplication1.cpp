// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "Loader.h"
#include "Handler.h"
#include <iostream>
using namespace std;

int main()
{
    Loader l;
    char* c = l.Input("01Chinese.cpp");
    Handler h;
    c = h.compress(c);
    cout << "(main函数)压缩后的内容为:" << c << endl;
    l.Output(c);

    c = l.Input("01Chinese.bin");
    c = h.decompress(c);
    cout << "解压后的内容为：" << c << endl;
}

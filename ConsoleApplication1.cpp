// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "Loader.h"
#include <iostream>
using namespace std;

int main()
{
    Loader l;
    char* c =  l.Input("1-3main.bin", 0);
    l.Output("ccc", 0);
}

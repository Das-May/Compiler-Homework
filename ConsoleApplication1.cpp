// ConsoleApplication1.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
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
    cout << "(main����)ѹ���������Ϊ:" << c << endl;
    l.Output(c);

    c = l.Input("01Chinese.bin");
    c = h.decompress(c);
    cout << "��ѹ�������Ϊ��" << c << endl;
}

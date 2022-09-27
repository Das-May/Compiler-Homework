#include "mainwindow.h"
#include <Loader.h>
#include <Handler.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    // ---以下为VS 控制台模式下的运行代码---
    /*
    Loader l;
    char* c = l.Input("05Comprehensive-short.cpp");
    Handler h;
    c = h.compress(c);
    cout << "(main函数)压缩后的内容为:" << c << endl;
    l.Output(c);

    c = l.Input("05Comprehensive-short-compress.bin");
    c = h.decompress(c);
    cout << "解压后的内容为：" << c << endl;
    l.Output(c);
    */

    // ---以下为Qt Windows窗口模式下运行的代码---
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

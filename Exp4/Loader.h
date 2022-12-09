#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <QDebug>

using namespace std;
class Loader
{
private:
    static char* filePath;

public:
    static char* Input(const char* filePath);
    static void Output(const char* compressed);
    static char* GetFilepath();
};


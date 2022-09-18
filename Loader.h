#pragma once
#include <iostream>
#include <fstream>
#include <windows.h>//SetConsoleOutputCP函数所在库函数，让cmd程序的编码更为utf-8
using namespace std;
class Loader
{
private:
	char* filePath;

public:
	Loader();
	char* Input(const char* filePath);
	void Output(const char* compressed);
};


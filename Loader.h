#pragma once
#include <iostream>
#include <fstream>
#include <windows.h>//SetConsoleOutputCP�������ڿ⺯������cmd����ı����Ϊutf-8
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


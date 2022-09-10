#pragma once
#include <iostream>
#include <fstream>
using namespace std;
class Loader
{
private:
	char* filePath;

public:
	char* Input(const char* filePath, int type);
	void Output(const char* compressed, int type);
};


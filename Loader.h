#pragma once
#include <iostream>
#include <fstream>
using namespace std;
class Loader
{
private:
	char* filePath;

public:
	char* Input(const char* filePath);
	void Output(const char* compressed);
};


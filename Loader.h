#pragma once
#include <iostream>
#include <fstream>
using namespace std;
class Loader
{
private:
	char* filePath;

public:
	char* InputCpp(const char* filePath);
	void OutputBin(const char* compressed);
	char* InputBin(const char* filePath);
	void OutputCpp(const char* decompressed);
};


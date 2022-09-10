#pragma once
#include <iostream>
#include <fstream>
using namespace std;
class Loader
{
private:
	char* filePath;

public:
	char* InputTxt(const char* filePath);
	void OutputBin(const char* compressed);
	char* InputBin(const char* filePath);
	void OutputTxt(const char* decompressed);
};


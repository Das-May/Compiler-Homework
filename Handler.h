#pragma once
#include <map>
#include <iostream>
#include <windows.h>//SetConsoleOutputCP�������ڿ⺯������cmd����ı����Ϊutf-8
using namespace std;
class Handler
{
	map<const char, const char*> word;

public:
	Handler();
	char* compress(char* c);
	char* decompress(char* c);
};


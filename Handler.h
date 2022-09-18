#pragma once
#include <map>
#include <iostream>
#include <string>
#include <windows.h>//SetConsoleOutputCP函数所在库函数，让cmd程序的编码更为utf-8
using namespace std;
class Handler
{
	map<const char, const char*> word;

public:
	Handler();
	
	char* compress(char* c);
	string myCharToString(char c);
	void processAlphabet(char *begin, char *end);
	char* myStringToChar(string s);

	char* decompress(char* c);
};


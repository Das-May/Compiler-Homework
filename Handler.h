#pragma once
#include <map>
#include <iostream>
#include <string>
#include <windows.h>//SetConsoleOutputCP�������ڿ⺯������cmd����ı����Ϊutf-8
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


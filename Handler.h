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
	
	/*压缩原则：
	* 一般ASCII码		以0开头，后接7位编码，共8位（也就是ASCII码）
	* 关键字			以1开头，后接7位编码，共8位（自主设计编码）
	*/
	char* compress(char* c);						// 压缩函数
	string myCharToString(char c);					// char二进制码转string01字符串
	string processField(char *begin, char *end);	//
	char* myStringToChar(string s);					// string01字符串转char二进制码

	/*解压原则：
	* 
	*/
	char* decompress(char* c);						// 解压函数
};


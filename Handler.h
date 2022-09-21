#pragma once
#include <map>
#include <iostream>
#include <string>
using namespace std;
class Handler
{
	map<const char, string> word;

public:
	Handler();
	
	/*压缩原则：
	* 一般ASCII码		以0开头，后接7位编码，共8位（也就是ASCII码）
	* 关键字			以1开头，后接7位编码，共8位（自主设计编码）
	*/
	char* compress(char* c);						// 压缩函数
	string myCharToString(char c);					// char二进制码转string01字符串
	string processField(char *begin, char *end);	// 处理字段
	char* myStringToChar(string s);					// string01字符串转char二进制码

	/*解压原则：对于每个8位二进制码
	* 以0开头		只接输出该ASCII码对应的字符
	* 以1开头		查找映射表，输出对应的关键字
	*/
	char* decompress(char* c);						// 解压函数
};


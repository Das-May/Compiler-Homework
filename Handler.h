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
	
	/*ѹ��ԭ��
	* һ��ASCII��		��0��ͷ�����7λ���룬��8λ��Ҳ����ASCII�룩
	* �ؼ���			��1��ͷ�����7λ���룬��8λ��������Ʊ��룩
	*/
	char* compress(char* c);						// ѹ������
	string myCharToString(char c);					// char��������תstring01�ַ���
	string processField(char *begin, char *end);	//
	char* myStringToChar(string s);					// string01�ַ���תchar��������

	/*��ѹԭ��
	* 
	*/
	char* decompress(char* c);						// ��ѹ����
};


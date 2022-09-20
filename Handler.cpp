#include "Handler.h"

Handler::Handler()
{
	SetConsoleOutputCP(936);

	// �ؼ���
#pragma region KEYWORD
	word[0x80] = "asm";
	word[0x81] = "auto";
	word[0x82] = "bool";
	word[0x83] = "break";
	word[0x84] = "case";
	word[0x85] = "catch";
	word[0x86] = "char";
	word[0x87] = "class";

	word[0x88] = "const";
	word[0x89] = "const_cast";
	word[0x8a] = "continue";
	word[0x8b] = "catch";
	word[0x8c] = "catch";
	word[0x8d] = "catch";
	word[0x8e] = "catch";
	word[0x8f] = "catch";

	word[0x90] = "asm";
	word[0x91] = "int";
	word[0x92] = "bool";
	word[0x93] = "break";
	word[0x94] = "case";
	word[0x95] = "char";
	word[0x96] = "class";
	word[0x97] = "const";

	word[0x98] = "catch";
	word[0x99] = "catch";
	word[0x9a] = "catch";
	word[0x9b] = "catch";
	word[0x9c] = "catch";
	word[0x9d] = "catch";
	word[0x9e] = "catch";
	word[0x9f] = "catch";

	word[0xa0] = "asm";
	word[0xa1] = "auto";
	word[0xa2] = "bool";
	word[0xa3] = "break";
	word[0xa4] = "case";
	word[0xa5] = "char";
	word[0xa6] = "class";
	word[0xa7] = "const";

	word[0xa8] = "catch";
	word[0xa9] = "catch";
	word[0xaa] = "catch";
	word[0xab] = "catch";
	word[0xac] = "catch";
	word[0xad] = "catch";
	word[0xae] = "catch";
	word[0xaf] = "catch";

	word[0xb0] = "asm";
	word[0xb1] = "auto";
	word[0xb2] = "bool";
	word[0xb3] = "break";
	word[0xb4] = "case";
	word[0xb5] = "char";
	word[0xb6] = "class";
	word[0xb7] = "const";

	word[0xb8] = "catch";
	word[0xb9] = "catch";
	word[0xba] = "catch";
	word[0xbb] = "catch";
	word[0xbc] = "catch";
	word[0xbd] = "catch";
	word[0xbe] = "catch";
	word[0xbf] = "catch";
#pragma endregion

	// �����
	// һ���ַ�
}

char* Handler::compress(char* c)
{
	char* headOfC = c;
	string temp;		// ��¼Ҫд���ļ��������ֽ���

	for (; c[0] != 0; c++)				// ����ÿ���ַ�
	{
		// ����ע�ͣ����ڽ�����֮ǰ���ķ�Χ�ڣ�ֱ��������ɾ����
		if (c[0] == '/')
		{
			if (c[1] == '/')		// ����ע��"//"
			{
				while (c[0] != '\n' && c[0] != 0)//ע�⣺�ж�ָ���Ƿ�Խ��
					c++;
			}
			else if (c[1] == '*')	// ����ע��"/**/"
			{
				while (!(c[0] == '*' && c[1] == '/') && c[0] != 0)
					c++;
				c++;// ע�⣺����*/�������ַ���ָ��Ҫ�ƶ���λ
			}
			else					// ����ע��
				temp += myCharToString(c[0]);	// ֱ��д��ASCII��
		}

		// �������η�����������
		else if (c[0] == ' ' || c[0] == '\n' || c[0] == '\t')
		{}

		// ����Сд��ĸ�����ƹؼ��֣���ɨ����д��
		else if (c[0] >= 'a' && c[0] <= 'z')
		{
			char* begin = c;
			while ((c[0] >= 'a' && c[0] <= 'z') || c[0] == '_')
				c++;
			char* end = c;
			temp += processField(begin, end);
		}
		
		//��ע�����������������ģ���������ǿ����ֹ
		else if ((*c & 0x80) != 0)
		{
			cerr << "�����ļ��У����ɳ��ֳ���ע����������ģ� " << endl;
			abort();
		}

		//���������ַ���ֱ��д��
		else
		{
			temp += myCharToString(c[0]);
		}
	}
	

	//�ڴ����
	delete headOfC;

	//����ѹ���õ����ݣ��������룩
	cout << "(�ຯ����)ѹ���������Ϊ�� " << temp << endl;

	c = myStringToChar(temp);
	return c;
}

string Handler::myCharToString(char c)
{
	string s = "";
	for (int i = 0; i < 8; i++)
	{
		s += char('0' + ((c & 0x80) >> 7));
		c = c << 1;
	}
	return s;
}

string Handler::processField(char* begin, char* end)
{
	// ��ȡ��Ӧ�Ĵ����
	string tempCode = "";
	for (;begin != end; begin++)
	{
		tempCode += *begin;
	}

	// �����ֵ䣬����ǹؼ��֣��򷵻عؼ��ֶ�Ӧ��8λ��������
	map<const char, const char*>::iterator it;
	for (it = word.begin(); it != word.end(); it++)
	{
		if ((*it).second == tempCode)
		{
			return myCharToString((*it).first);
		}
	}

	// ������ǹؼ��֣������ַ�ת��Ϊ8λ01�ַ���
	string temp01 = "";
	for (int i = 0; i < tempCode.length(); i++)
	{
		temp01 += myCharToString(tempCode[i]);
	}
	return temp01;
}

char* Handler::myStringToChar(string s)
{
	// ��ȡ�ַ�������
	int len1 = s.length();	// string���ַ�������
	int len2 = len1 / 8;// char����ĳ���
	if (len1 % 8 != 0)		// ����8λ��Ҳ��1
	{
		len2++;
	}
		
	// �����ַ����� ���һλ��������'\0'
	char* c = new char[len2 + 1];
	char* headOfChar = &c[0];
	std::memset(c, 0, len2 + 1);

	// ����string
	string::iterator it = s.begin();
	while (it != s.end())
	{
		for (int j = 0; j < 8; j++)			// ÿ8��01�ַ���д��1��char
		{
			if (it != s.end())
			{
				c[0] = (c[0] << 1) | (*it++ - 48);
			}		
			else
				c[0] = (c[0] << 1) | ('0' - 48);
		}
		c++;
	}

	//cout << "myStringToChar" << headOfChar << endl;
	return headOfChar;
}

char* Handler::decompress(char* c)
{
	char* headOfC = c;
	int i = 0, j = 0, tabCount = 0;
	int len = strlen(headOfC);

	// �Ѷ����Ķ�����ת��Ϊ01�ַ���
	string temp01 = "";					
	for(i=0;i< len;i++)
	{
		temp01 += myCharToString(c[0]);
		c++;
	}
	len = temp01.length();

	// ��01�ַ�����ѹ��Ϊ����
	string tempCode = "";
	string s = "";
	i = 0;
	while(i < len)
	{
		if (temp01[i] == '0') // ASCII��
		{
			for (s = "", j = 0; j < 8; j++, i++)
				s += temp01[i];
			s = *myStringToChar(s);
			tempCode += s;
			if (s == ";" || s == "{" || s == "}")
			{
				tempCode += '\n';
			}
			if (s == "{")
			{
				tabCount++;
				for (j = 0; j < tabCount; j++)
					tempCode += '\t';
			}
			else if (s == "}")
			{
				tabCount--;
				for (j = 0; j < tabCount; j++)
					tempCode += '\t';
			}
				
				
		}
		else				  // �ؼ���
		{
			for (s = "", j = 0; j < 8; j++, i++)
				s += temp01[i];
			tempCode += word.find(*myStringToChar(s))->second;
			tempCode += " ";
		}
	}

	len = strlen(tempCode.c_str());
	c = new char[len + 1];
	std::strcpy(c, tempCode.c_str());

	delete headOfC;
	
	return c;
}
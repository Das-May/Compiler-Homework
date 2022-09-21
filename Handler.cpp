#include "Handler.h"

Handler::Handler()
{
	// �ؼ��� 63�� �����80��be
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
	word[0x8b] = "default";
	word[0x8c] = "delete";
	word[0x8d] = "do";
	word[0x8e] = "double";
	word[0x8f] = "dynamic_cast";

	word[0x90] = "else";
	word[0x91] = "enum";
	word[0x92] = "explicit";
	word[0x93] = "export";
	word[0x94] = "extern";
	word[0x95] = "false";
	word[0x96] = "float";
	word[0x97] = "for";

	word[0x98] = "friend";
	word[0x99] = "goto";
	word[0x9a] = "if";
	word[0x9b] = "inline";
	word[0x9c] = "int";
	word[0x9d] = "long";
	word[0x9e] = "mutable";
	word[0x9f] = "namespace";

	word[0xa0] = "new";
	word[0xa1] = "operator";
	word[0xa2] = "private";
	word[0xa3] = "protected";
	word[0xa4] = "public";
	word[0xa5] = "register";
	word[0xa6] = "reinterpret_cast";
	word[0xa7] = "return";

	word[0xa8] = "short";
	word[0xa9] = "signed";
	word[0xaa] = "sizeof";
	word[0xab] = "static";
	word[0xac] = "static_cast";
	word[0xad] = "struct";
	word[0xae] = "switch";
	word[0xaf] = "template";

	word[0xb0] = "this";
	word[0xb1] = "throw";
	word[0xb2] = "true";
	word[0xb3] = "try";
	word[0xb4] = "typedef";
	word[0xb5] = "typeid";
	word[0xb6] = "typename";
	word[0xb7] = "unoin";

	word[0xb8] = "unsigned";
	word[0xb9] = "using";
	word[0xba] = "virtual";
	word[0xbb] = "void";
	word[0xbc] = "volatile";
	word[0xbd] = "wchar_t";
	word[0xbe] = "while";
#pragma endregion

	// �����
	/*�������룺��Ϊ����������Ǻܳ���ֱ�����Ҳ���ԣ�Ϊ����������ƺ��Լ۱Ȳ���*/

}

char* Handler::compress(char* c)
{
	char* headOfC = c;
	string temp;		// ��¼Ҫд���ļ��������ֽ���

	for (; *c != 0; c++)			// ����ÿ���ַ�
	{
		// ����Сд��ĸ�����ƹؼ��֣���ɨ����д��
		if (*c >= 'a' && *c <= 'z')
		{
		char* begin = c;
		while ((*c >= 'a' && *c <= 'z') || *c == '_')
			c++;
		char* end = c;
		c--;//����!!!!!!
		temp += processField(begin, end);
		}
		
		// �������η�����������
		else if (*c == ' ' || *c == '\n' || *c == '\t')
		{}

		// ����ע�ͣ����ڽ�����֮ǰ���ķ�Χ�ڣ�ֱ��������ɾ����
		else if (*c == '/')
		{
			if (c[1] == '/')		// ����ע��"//"
			{
				while (*c != '\n' && *c != 0)//ע�⣺�ж�ָ���Ƿ�Խ��
					c++;
			}
			else if (c[1] == '*')	// ����ע��"/**/"
			{
				while (!(*c == '*' && c[1] == '/') && *c != 0)
					c++;
				c++;// ע�⣺����*/�������ַ���ָ��Ҫ�ƶ���λ
			}
			else					// ����ע��
				temp += myCharToString(*c);	// ֱ��д��ASCII��
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
			temp += myCharToString(*c);
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
	string strCode = "";
	for (;begin != end; begin++)
	{
		strCode += *begin;
	}

	// �����ֵ䣬����ǹؼ��֣��򷵻عؼ��ֶ�Ӧ��8λ��������
	map<const char, string>::iterator it;
	for (it = word.begin(); it != word.end(); it++)
	{
		if ((*it).second == strCode)
		{
			return myCharToString((*it).first);
		}
	}

	// ������ǹؼ��֣������ַ�ת��Ϊ8λ01�ַ���
	string str01 = "";
	for (int i = 0; i < strCode.length(); i++)
	{
		str01 += myCharToString(strCode[i]);
	}
	return str01;
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
	char* headOfChar = &*c;
	std::memset(c, 0, len2 + 1);

	// ����string
	string::iterator it = s.begin();
	while (it != s.end())
	{
		for (int j = 0; j < 8; j++)			// ÿ8��01�ַ���д��1��char
		{
			if (it != s.end())
			{
				*c = (*c << 1) | (*it++ - 48);
			}		
			else
				*c = (*c << 1) | ('0' - 48);
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
	string str01 = "";					
	for(i=0;i< len;i++)
	{
		str01 += myCharToString(*c);
		c++;
	}
	len = str01.length();

	// ��01�ַ�����ѹ��Ϊ����
	string strCode = "";
	string temp01 = "";
	char tempChar = 0;
	i = 0;
	while(i < len)
	{
		if (str01[i] == '0') // ASCII��
		{
			for (temp01 = "", j = 0; j < 8; j++, i++)
				temp01 += str01[i];
			tempChar = *myStringToChar(temp01);
			

			// ��������
			switch (tempChar)
			{
			case ';':
				strCode += ";\n";
				for (j = 0; j < tabCount; j++)
					strCode += '\t';
				break;
			case '{':
				strCode += "{\n";
				tabCount++;
				for (j = 0; j < tabCount; j++)
					strCode += '\t';
				break;
			case '}':
				strCode += "}\n";
				tabCount--;
				for (j = 0; j < tabCount; j++)
					strCode += '\t';
				break;
			case '#':
				strCode += "\n#";
				break;
			default:
				strCode += tempChar;
				break;
			}				
		}
		else				  // �ؼ���
		{
			for (temp01 = "", j = 0; j < 8; j++, i++)
				temp01 += str01[i];
			strCode += word.find(*myStringToChar(temp01))->second;
			strCode += " ";
		}
	}

	len = strlen(strCode.c_str());
	c = new char[len + 1];
	std::strcpy(c, strCode.c_str());

	delete headOfC;
	
	return c;
}
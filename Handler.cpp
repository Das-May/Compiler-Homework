#include "Handler.h"

Handler::Handler()
{
	SetConsoleOutputCP(936);

	// �ؼ���
#pragma region KEYWORD
	word[0x00] = "asm";
	word[0x01] = "auto";
	word[0x02] = "bool";
	word[0x03] = "break";
	word[0x04] = "case";
	word[0x05] = "catch";
#pragma endregion

	// �����
	// һ���ַ�
}

char* Handler::compress(char* c)
{
	char* headOfC = c;

	string temp;		// ��¼Ҫд���ļ��������ֽ���
	int count = 0;

	for (; c[0] != 0; c++)				// ����ÿ���ַ�
	{
		if ((c[0] & 0x80) == 0)			// ����ASCII��
		{
			// ����ע�ͣ�ֱ������
			if (c[0] == '/')
			{
				c++;
				if (c[0] == '/')		// ����ע��"//"
				{
					c++;
					while (c[0] != '\n')
						c++;
				}
				else if (c[0] == '*')	// ����ע��"/**/"
				{
					c++;
					while (!(c[0] == '*' && c[1] == '/'))
						c++;
				}
			}

			// ����Сд��ĸ�����ƹؼ��֣���ɨ����д��
			else if (c[0] >= 'a' && c[0] <= 'z')
			{
				char* begin = c;
				c++;
				while ((c[0] >= 'a' && c[0] <= 'z') || c[0] == '_')
					c++;
				char* end = c;
				// TODO:
				processAlphabet(begin, end);
			}

			//���������ַ���ֱ��д��
			else
			{
				temp += '0' + myCharToString(c[0]);	// 0+ASCII��
			}
			//cout << c[0] << "��Ӣ��" << endl;
		}

		// ��������
		else                            
		{
			char chinese[3] = {c[0], c[1], '\0'};
			//cout << chinese << "������" << endl;

			temp += myCharToString(c[0]) + myCharToString(c[1]);
			c++;
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

void Handler::processAlphabet(char* begin, char* end)
{

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
				//it++;
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
	int i = 0, j = 0;
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
		if (temp01[i] == '0') // ASCII�� �� �ؼ���
		{
			i++;
			if (temp01[i] == '1' && (i + 6) < len)	   // �ؼ���
			{
				i += 6;
			}
			else if (temp01[i] == '0' && (i + 7) < len)// ASCII��
			{
				i += 7;
			}
			else
				break;
		}
		else				  // ����
		{
			s = "";
			for (j = 0; j < 16; j++, i++)
				s += temp01[i];
			tempCode += myStringToChar(s);
		}
	}

	len = strlen(tempCode.c_str());
	c = new char[len + 1];
	//std::memset(c, 0, len);
	std::strcpy(c, tempCode.c_str());

	delete headOfC;
	
	return c;
}
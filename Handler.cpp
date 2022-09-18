#include "Handler.h"

Handler::Handler()
{
	SetConsoleOutputCP(936);

	// 关键字
#pragma region KEYWORD
	word[0x00] = "asm";
	word[0x01] = "auto";
	word[0x02] = "bool";
	word[0x03] = "break";
	word[0x04] = "case";
	word[0x05] = "catch";
#pragma endregion

	// 运算符
	// 一般字符
}

char* Handler::compress(char* c)
{
	char* headOfC = c;

	string temp;		// 记录要写入文件的所有字节码
	//int saveBin = 0x0000;				// 暂存单个字符的字节码
	int count = 0;

	for (; c[0] != 0; c++)				// 遍历每个字符
	{
		if ((c[0] & 0x80) == 0)			// 遇到ASCII码
		{
			// 遇到注释，直接跳过
			if (c[0] == '/')
			{
				c++;
				if (c[0] == '/')		// 单行注释"//"
				{
					c++;
					while (c[0] != '\n')
						c++;
				}
				else if (c[0] == '*')	// 多行注释"/**/"
				{
					c++;
					while (!(c[0] == '*' && c[1] == '/'))
						c++;
				}
			}

			// 遇到小写字母，疑似关键字，先扫描再写入
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

			//遇到其它字符，直接写入
			else
			{
				temp += '0' + myCharToString(c[0]);	// 0+ASCII码
			}
			//cout << c[0] << "是英文" << endl;
		}

		// 遇到中文
		else                            
		{
			char chinese[3] = {c[0], c[1], '\0'};
			//cout << chinese << "是中文" << endl;

			temp += myCharToString(c[0]) + myCharToString(c[1]);
			c++;
		}
	}

	//内存回收
	delete headOfC;

	//返回压缩好的内容（二进制码）
	cout << "(类函数内)压缩后的内容为： " << temp << endl;

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
	int len1 = s.length();
	int len2 = len1 / 8 + 1;
	if (len1 % 8 != 0)
	{
		len2++;
	}
		

	char* c = new char[len2 + 1];
	char* headOfChar = c;
	memset(c, 0, sizeof(c));
	char t = 0;

	string::iterator it = s.begin();
	while (it != s.end())
	{
		for (int j = 0; j < 8; j++)
		{
			if(it != s.end())
				t = (t << 1) | (*it++ - 48);
			else
				t = (t << 1) | ('0' - 48);
		}
		c[0] = t;
		c++;
	}

	//cout << "myStringToChar" << headOfChar << endl;
	return headOfChar;
}

char* Handler::decompress(char* c)
{
	char* headOfC = c;

	string temp01 = "";
	string tempCode = "";
	while (c[0] != 0)
	{
		temp01 += myCharToString(c[0]);
		c++;
	}

	int len = temp01.length();
	int i = 0, j = 0;

	while(i < len)
	{
		if (temp01[i] == '0') // ASCII码 或 关键字
		{

		}
		else				// 汉字
		{
			string chineseChar1 = "";
			/*string chineseChar2 = "";
			for (j = 0; j < 8; j++)
				chineseChar1 += temp01[i + j];
			for (j = 8; j < 16; j++)
				chineseChar2 += temp01[i + j];*/
			for (j = 0; j < 16; j++, i++)
				chineseChar1 += temp01[i];
			tempCode += myStringToChar(chineseChar1);
		}
	}

	c = new char[strlen(tempCode.c_str()) + 1];
	strcpy(c, tempCode.c_str());

	delete headOfC;
	
	return c;
}
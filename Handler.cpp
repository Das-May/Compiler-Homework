#include "Handler.h"

Handler::Handler()
{
	SetConsoleOutputCP(936);

	// 关键字
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

	// 运算符
	// 一般字符
}

char* Handler::compress(char* c)
{
	char* headOfC = c;
	string temp;		// 记录要写入文件的所有字节码

	for (; c[0] != 0; c++)				// 遍历每个字符
	{
		// 遇到注释，“在结束符之前”的范围内，直接跳过（删除）
		if (c[0] == '/')
		{
			if (c[1] == '/')		// 单行注释"//"
			{
				while (c[0] != '\n' && c[0] != 0)//注意：判断指针是否越界
					c++;
			}
			else if (c[1] == '*')	// 多行注释"/**/"
			{
				while (!(c[0] == '*' && c[1] == '/') && c[0] != 0)
					c++;
				c++;// 注意：由于*/是两个字符，指针要移动两位
			}
			else					// 不是注释
				temp += myCharToString(c[0]);	// 直接写入ASCII码
		}

		// 遇到修饰符，不做处理
		else if (c[0] == ' ' || c[0] == '\n' || c[0] == '\t')
		{}

		// 遇到小写字母，疑似关键字，先扫描再写入
		else if (c[0] >= 'a' && c[0] <= 'z')
		{
			char* begin = c;
			while ((c[0] >= 'a' && c[0] <= 'z') || c[0] == '_')
				c++;
			char* end = c;
			temp += processField(begin, end);
		}
		
		//在注释区域以外遇到中文，立即报错，强制中止
		else if ((*c & 0x80) != 0)
		{
			cerr << "代码文件中，不可出现除了注释以外的中文！ " << endl;
			abort();
		}

		//遇到其它字符，直接写入
		else
		{
			temp += myCharToString(c[0]);
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

string Handler::processField(char* begin, char* end)
{
	// 获取对应的代码段
	string tempCode = "";
	for (;begin != end; begin++)
	{
		tempCode += *begin;
	}

	// 遍历字典，如果是关键字，则返回关键字对应的8位二进制码
	map<const char, const char*>::iterator it;
	for (it = word.begin(); it != word.end(); it++)
	{
		if ((*it).second == tempCode)
		{
			return myCharToString((*it).first);
		}
	}

	// 如果不是关键字，则逐字符转化为8位01字符串
	string temp01 = "";
	for (int i = 0; i < tempCode.length(); i++)
	{
		temp01 += myCharToString(tempCode[i]);
	}
	return temp01;
}

char* Handler::myStringToChar(string s)
{
	// 获取字符串长度
	int len1 = s.length();	// string的字符串长度
	int len2 = len1 / 8;// char数组的长度
	if (len1 % 8 != 0)		// 不足8位，也进1
	{
		len2++;
	}
		
	// 创建字符数组 最后一位留结束符'\0'
	char* c = new char[len2 + 1];
	char* headOfChar = &c[0];
	std::memset(c, 0, len2 + 1);

	// 遍历string
	string::iterator it = s.begin();
	while (it != s.end())
	{
		for (int j = 0; j < 8; j++)			// 每8个01字符，写入1个char
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

	// 把读到的二进制转化为01字符串
	string temp01 = "";					
	for(i=0;i< len;i++)
	{
		temp01 += myCharToString(c[0]);
		c++;
	}
	len = temp01.length();

	// 把01字符串解压化为代码
	string tempCode = "";
	string s = "";
	i = 0;
	while(i < len)
	{
		if (temp01[i] == '0') // ASCII码
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
		else				  // 关键字
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
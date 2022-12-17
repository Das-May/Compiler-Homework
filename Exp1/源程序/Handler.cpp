#include "Handler.h"

Handler::Handler()
{
	// 关键字 63个 编码从80到be
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

	// 运算符
	/*放弃编码：因为运算符都不是很长，直接输出也可以，为此特意编码似乎性价比不高*/

}

char* Handler::compress(char* c)
{
	char* headOfC = c;
    string temp;                    // 记录要写入文件的所有字节码

	for (; *c != 0; c++)			// 遍历每个字符
	{
		// 遇到小写字母，疑似关键字，先扫描再写入
		if (*c >= 'a' && *c <= 'z')
		{
		char* begin = c;
		while ((*c >= 'a' && *c <= 'z') || *c == '_')
			c++;
		char* end = c;
        c--;// !
		temp += processField(begin, end);
		}
		
		// 遇到修饰符，不做处理
		else if (*c == ' ' || *c == '\n' || *c == '\t')
		{}

		// 遇到注释，“在结束符之前”的范围内，直接跳过（删除）
		else if (*c == '/')
		{
			if (c[1] == '/')		// 单行注释"//"
			{
				while (*c != '\n' && *c != 0)//注意：判断指针是否越界
					c++;
			}
			else if (c[1] == '*')	// 多行注释"/**/"
			{
				while (!(*c == '*' && c[1] == '/') && *c != 0)
					c++;
				c++;// 注意：由于*/是两个字符，指针要移动两位
			}
			else					// 不是注释
				temp += myCharToString(*c);	// 直接写入ASCII码
		}
	
		//在注释区域以外遇到中文，立即报错，强制中止
		else if ((*c & 0x80) != 0)
		{
			cerr << "代码文件中，不可出现除了注释以外的中文！ " << endl;
            return 0;
            //abort();
		}

		//遇到其它字符，直接写入
		else
		{
			temp += myCharToString(*c);
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
	string strCode = "";
	for (;begin != end; begin++)
	{
		strCode += *begin;
	}

	// 遍历字典，如果是关键字，则返回关键字对应的8位二进制码
	map<const char, string>::iterator it;
	for (it = word.begin(); it != word.end(); it++)
	{
		if ((*it).second == strCode)
		{
			return myCharToString((*it).first);
		}
	}

	// 如果不是关键字，则逐字符转化为8位01字符串
	string str01 = "";
	for (int i = 0; i < strCode.length(); i++)
	{
		str01 += myCharToString(strCode[i]);
	}
	return str01;
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
	char* headOfChar = &*c;
	std::memset(c, 0, len2 + 1);

	// 遍历string
	string::iterator it = s.begin();
	while (it != s.end())
	{
		for (int j = 0; j < 8; j++)			// 每8个01字符，写入1个char
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

	// 把读到的二进制转化为01字符串
	string str01 = "";					
	for(i=0;i< len;i++)
	{
		str01 += myCharToString(*c);
		c++;
	}
	len = str01.length();

	// 把01字符串解压化为代码
	string strCode = "";
	string temp01 = "";
	char tempChar = 0;
	i = 0;
	while(i < len)
	{
		if (str01[i] == '0') // ASCII码
		{
			for (temp01 = "", j = 0; j < 8; j++, i++)
				temp01 += str01[i];
			tempChar = *myStringToChar(temp01);
			

			// 后期修饰
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
		else				  // 关键字
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

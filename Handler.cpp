#include "Handler.h"

Handler::Handler()
{
	SetConsoleOutputCP(936);

	// 关键字
	word[char(0)] = "";
	// 运算符
	// 一般字符
}

char* Handler::compress(char* c)
{
	char* temp = new char[1];
	temp = '\0';

	for (; c[0] != 0; c++)
	{
		if ((c[0] & 0x80) == 0)
		{
			cout << c[0] << "是英文" << endl;
		}
		else
		{
			char chinese[3] = {c[0], c[1], '\0'};
			cout << chinese << "是中文" << endl;
			c++;
		}
			
		
	}
	return 0;
}

char* Handler::decompress(char* c)
{
	char* temp = new char[1];
	temp = '\0';

	for (; c[0] != 0; c++)
	{
		if ((c[0] & 0x80) == 0)
		{
			cout << c[0] << "是英文" << endl;
		}
		else
		{
			char chinese[3] = { c[0], c[1], '\0' };
			cout << chinese << "是中文" << endl;
			c++;
		}


	}
	return 0;
}
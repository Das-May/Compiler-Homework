#include "Handler.h"

Handler::Handler()
{
	SetConsoleOutputCP(936);

	// �ؼ���
	word[char(0)] = "";
	// �����
	// һ���ַ�
}

char* Handler::compress(char* c)
{
	char* temp = new char[1];
	temp = '\0';

	for (; c[0] != 0; c++)
	{
		if ((c[0] & 0x80) == 0)
		{
			cout << c[0] << "��Ӣ��" << endl;
		}
		else
		{
			char chinese[3] = {c[0], c[1], '\0'};
			cout << chinese << "������" << endl;
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
			cout << c[0] << "��Ӣ��" << endl;
		}
		else
		{
			char chinese[3] = { c[0], c[1], '\0' };
			cout << chinese << "������" << endl;
			c++;
		}


	}
	return 0;
}
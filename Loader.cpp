#include "Loader.h"

char* Loader::InputTxt(const char* filePath)
{
    // ��.cpp�ļ�
    ifstream cppFile(filePath, ios::in | ios::_Noreplace);
    if (!cppFile)
    {
        cerr << "�ļ���ȡʧ��" << endl;
        abort();
    }

    // ��¼�ļ�·��
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);

    // ��ȡ�ı�����
    int len;
    cppFile.seekg(0, ios::end);
    len = cppFile.tellg();
    cout << len << endl;

    // ��ȡ�ı�����
    cppFile.seekg(0, ios::beg);
    char* cppChar = new char[ len + 1 ];
    memset(cppChar, 0, len + 1);
    cppFile.read(cppChar,len);
    cout << cppChar << endl;

    //����charָ��
    return cppChar;
}

void Loader::OutputBin(const char* compressed)
{
    
}

char* InputBin(const char* filePath);
void OutputTxt(const char* decompressed);

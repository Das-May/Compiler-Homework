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

    // ��ȡ�ļ�
    cout << cppFile.tellg();

    //����charָ��

}

void OutputBin(const char* compressed);
char* InputBin(const char* filePath);
void OutputTxt(const char* decompressed);

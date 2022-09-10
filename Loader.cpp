#include "Loader.h"

char* Loader::InputCpp(const char* filePath)
{
    // ��.cpp�ļ�
    ifstream cppFile(filePath, ios::in | ios::_Noreplace);
    if (!cppFile)
    {
        cerr << "cpp�ļ���ȡʧ��" << endl;
        abort();
    }

    // ��¼�ļ�·��
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);

    // ��ȡ�ı�����
    int len;
    cppFile.seekg(0, ios::end);
    len = cppFile.tellg();
    cout << "cpp�ı����ݳ��ȣ� " << len << endl;

    // ��ȡ�ı�����
    cppFile.seekg(0, ios::beg);
    char* cppChar = new char[ len + 1 ];
    memset(cppChar, 0, len + 1);
    cppFile.read(cppChar,len);
    cout << "cpp�ı����ݣ� " << cppChar << endl;

    //����charָ��
    return cppChar;
}

void Loader::OutputBin(const char* compressed)
{
    cout << filePath << endl;
    int len = strlen(filePath) - 3;          
    filePath[len] = '\0';                       // �ص�cpp��׺
    strcat(filePath, "bin");   // ׷��bin��׺
    cout << "�������ļ�����·���� " << filePath << endl;
    ofstream binFile(filePath, ios::out | ios::binary);
    if (!binFile)    // �����ʧ��
    {
        cerr << "ѹ���ļ�д��ʧ�ܣ�" << endl;
        abort();
    }

    binFile.write(compressed, sizeof(compressed));
    cout << "ѹ���ɹ���" << endl;
}

char* Loader::InputBin(const char* filePath)
{
    // �򿪶������ļ�
    ifstream binFile(filePath, ios::in | ios::binary);
    if (!binFile)
    {
        cerr << "cpp�ļ���ȡʧ��" << endl;
        abort();
    }

    // ��¼�ļ�·��
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);

    // ��ȡ�ı�����
    int len;
    binFile.seekg(0, ios::end);
    len = binFile.tellg();
    cout << len << endl;

    // ��ȡ�ı�����
    binFile.seekg(0, ios::beg);
    char* cppChar = new char[len + 1];
    memset(cppChar, 0, len + 1);
    binFile.read(cppChar, len);
    cout << cppChar << endl;

    //����charָ��
    return cppChar;
}

void Loader::OutputCpp(const char* decompressed)
{
    cout << filePath << endl;
    ofstream binFile(strcat(filePath, ".txt"), ios::out | ios::binary);
    if (!binFile)    // �����ʧ��
    {
        cerr << "ѹ���ļ�д��ʧ�ܣ�" << endl;
        abort();
    }

    binFile.write(decompressed, sizeof(decompressed));
    cout << "ѹ���ɹ���" << endl;
}

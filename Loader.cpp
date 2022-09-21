#include "Loader.h"

char* Loader::Input(const char* filePath)    
{
    // �ж��ļ�����
    int type = 0;           // type=0,����bin�ļ�
    int len = strlen(filePath);
    if (filePath[len - 4] == '.' && filePath[len - 3] == 'c' &&
        filePath[len - 2] == 'p' && filePath[len - 1] == 'p')
        type = 1;           // type=1,����cpp�ļ�
    cout << "����type: " << type << endl;

    // ���ļ�
    ifstream file;
    if (type)
        file.open(filePath, ios::in);
    else
        file.open(filePath, ios::in | ios::binary);
    if (!file)
    {
        cerr << "�ļ���ȡʧ�ܣ�" << endl;
        abort();
    }

    // ��¼�ļ���ȡ·��
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);
    cout << "�ļ���ȡ·���� " << filePath << endl;

    // ��ȡ�ı�����
    file.seekg(0, ios::end);
    len = file.tellg();
    cout << "�ļ����ݳ��ȣ� " << len << endl;

    // ��ȡ�ı�����
    file.seekg(0, ios::beg);
    char* c = new char[ len + 1 ];
    memset(c, 0, len + 1);
    file.read(c,len);
    cout << "�ļ����ݣ� " << c << endl;

    // �ر��ļ�
    file.close();

    //����charָ��
    return c;
}

void Loader::Output(const char* processed)    
{
    //�ж��ļ�����
    int type = 0;           // type=0������cpp�ļ�
    int len = strlen(filePath);
    if (filePath[len - 4] == '.' && filePath[len - 3] == 'c' &&
        filePath[len - 2] == 'p' && filePath[len - 1] == 'p')
        type = 1;           // type=1,����bin�ļ�

    // �����ļ�����·��
    len = strlen(filePath) - 4;          
    filePath[len] = '\0';   // �ص�
    if(type)                // ׷��
        strcat(filePath, "-compress.bin");             
    else
        strcat(filePath, "-decompress.cpp");
    cout << "�ļ�����·���� " << filePath << endl;

    // �����ļ�
    ofstream file;
    if (type)
        file.open(filePath, ios::out | ios::binary);
    else
        file.open(filePath, ios::out);
    if (!file)              // �����ʧ��
    {
        cerr << "�ļ�д��ʧ�ܣ�" << endl;
        abort();
    }

    file.write(processed, strlen(processed));
    if (type)
        cout << "ѹ���ɹ���" << endl;
    else
        cout << "��ѹ�ɹ���" << endl;

    // �ر��ļ�
    file.close();
}
